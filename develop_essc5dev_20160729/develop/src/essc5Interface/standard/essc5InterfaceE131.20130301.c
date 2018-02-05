//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5Interface.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "defaultIBMPinOffsetDef.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmdVersion.h"

/***************************************
服务代码:	E131
服务名:		生成PVV
功能描述:	生成PVV,由一个LMK对加密的PIN导出一个VISA PIN校验值
		产生一个4位的VISA PVV,对应的加密机指令为：DG
***************************************/
int UnionEncryptPinBlockFromZPKToLMK(PUnionHsmGroupRec phsmGroupRec,char *pinBlock,char *accNo,char *newPinBlock);
int UnionGeneratePVVByPinBlockOfTPKEncrypt(PUnionHsmGroupRec phsmGroupRec,char *pvk,char *pinBlock,char *accNo,char *pvv);
int UnionDealServiceCodeE131(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				accNoLen = 0;
	int				pinLen = 0;
	char				PVV[32];
	char				accNo[64];
	char				pinBlock[64];
	char				keyName[160];
	char				tmpBuf[32];
	int				protectFlag = 0;

	TUnionSymmetricKeyDB		pvkKeyDB;
	PUnionSymmetricKeyValue		ppvkKeyValue = NULL;
	
	// PVK密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE131:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);
	if (strlen(keyName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE131:: keyName can not be null!\n");
		UnionSetResponseRemark("密钥名称不能为空");
		return(errCodeParameter);
	}

	// 读取PVK密钥
	if ((ret = UnionReadSymmetricKeyDBRec(keyName,1,&pvkKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE131:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}

	if (pvkKeyDB.keyType != conPVK)
	{
		UnionUserErrLog("in UnionDealServiceCodeE131:: is not pvk [%s]!\n",keyName);
		return(errCodeEsscMDL_WrongUsageOfKey);
	}

	// 读取密钥值
	if ((ppvkKeyValue = UnionGetSymmetricKeyValue(&pvkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE131:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("读取密钥值失败");
		return(errCodeParameter);
	}

	// PIN密文
	if ((pinLen = UnionReadRequestXMLPackageValue("body/pinBlock",pinBlock,sizeof(pinBlock))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE131:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinBlock");
		return(pinLen);
	}
	else
        {
		UnionFilterHeadAndTailBlank(pinBlock);
		if(strlen(pinBlock) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE131:: pinBlock can not be null!\n");
                	UnionSetResponseRemark("PIN密文不能为空!");
                	return(errCodeParameter);
		}
                if (!UnionIsBCDStr(pinBlock))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE131:: pinBlock[%s] is error!\n",pinBlock);
                        UnionSetResponseRemark("PIN密文非法,必须为十六进制数");
                        return(errCodeParameter);
                }
        }

	// pin保护方式
	if ((ret = UnionReadRequestXMLPackageValue("body/protectFlag",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		protectFlag = 1;
	}
	tmpBuf[ret] = 0;
	protectFlag = atoi(tmpBuf);


	// 账号
	if ((accNoLen = UnionReadRequestXMLPackageValue("body/accNo",accNo,sizeof(accNo))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE131:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
		return(accNoLen);
	}
	accNo[accNoLen] = 0;
	
	// 检测账号
	if(accNoLen == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE131:: accNo can not be null!\n");
                UnionSetResponseRemark("账号不能为空!");
                return(errCodeParameter);
        }
	if (!UnionIsDigitString(accNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE131:: accNo[%s] is error!\n",accNo);
		UnionSetResponseRemark("账号非法[%s],必须为数字",accNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}

	switch(pvkKeyDB.algorithmID)
	{
		case	conSymmetricAlgorithmIDOfDES:	// DES算法

			if (protectFlag == 2)
			{
				if ((ret = UnionEncryptPinBlockFromZPKToLMK(phsmGroupRec,pinBlock,accNo,pinBlock)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE131:: UnionEncryptPinBlockFromZPKToLMK!\n");
					return(ret);
				}
				pinLen = strlen(pinBlock);
			}
			switch(phsmGroupRec->hsmCmdVersionID)
        		{
                		case conHsmCmdVerRacalStandardHsmCmd:
                		case conHsmCmdVerSJL06StandardHsmCmd:
					if (strcasecmp(UnionGetIDOfCustomization(),"UnionPay") == 0)
						UnionSetUseSpecPVKI(1);
					else
						UnionSetUseSpecPVKI(0);
					if ((ret = UnionHsmCmdDG(pinLen,pinBlock,UnionConvertSymmetricKeyKeyLen(pvkKeyDB.keyLen),ppvkKeyValue->keyValue,accNoLen,accNo,PVV)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE131:: UnionHsmCmdDG!\n");
						return(ret);
					}
					break;
                		default:
                        		UnionUserErrLog("in UnionDealServiceCodeE131:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        		UnionSetResponseRemark("非法的加密机指令类型");
                 		       return(errCodeParameter);
        		}
			break;
		case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
			if (accNoLen < 16)
			{
				memset(tmpBuf,'0',sizeof(tmpBuf));
				memcpy(tmpBuf+16-accNoLen,accNo,accNoLen);
				memcpy(accNo,tmpBuf,16);
				accNoLen = 16;
				accNo[accNoLen] = 0;
			}
			else if (accNoLen > 16)
			{
				memcpy(tmpBuf,accNo+accNoLen-16,16);
				memcpy(accNo,tmpBuf,16);
				accNoLen = 16;
				accNo[accNoLen] = 0;
			}

			if ((ret = UnionGeneratePVVByPinBlockOfTPKEncrypt(phsmGroupRec,ppvkKeyValue->keyValue,pinBlock,accNo,PVV)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE131:: UnionGeneratePVVByPinBlockOfTPKEncrypt!\n");
				return(ret);	
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE131:: pvkKeyDB.algorithmID[%d] error!\n",pvkKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	
	// 设置PIN密文
	if ((ret = UnionSetResponseXMLPackageValue("body/PVV",PVV)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE131:: UnionSetResponseXMLPackageValue PVV[%s]!\n",PVV);
		return(ret);
	}
	
	return(0);
}

int UnionGeneratePVVByPinBlockOfTPKEncrypt(PUnionHsmGroupRec phsmGroupRec,char *pvk,char *pinBlock,char *accNo,char *pvv)
{
	int	ret = 0;	
	char	keyName[160];

	TUnionSymmetricKeyDB		tpkKeyDB;
	PUnionSymmetricKeyValue		ptpkKeyValue = NULL;

	// TPK密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyNameOfZPK",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionGeneratePVVByPinBlockOfTPKEncrypt:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyNameOfZPK");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(keyName);
	if (strlen(keyName) == 0)
	{
		UnionUserErrLog("in UnionGeneratePVVByPinBlockOfTPKEncrypt:: keyNameOfZPK can not be null!\n");
		UnionSetResponseRemark("TPK密钥名称不能为空");
		return(errCodeParameter);
	}

	// 读取TPK密钥
	if ((ret = UnionReadSymmetricKeyDBRec(keyName,1,&tpkKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionGeneratePVVByPinBlockOfTPKEncrypt:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}

	if (tpkKeyDB.keyType != conTPK)
	{
		UnionUserErrLog("in UnionGeneratePVVByPinBlockOfTPKEncrypt:: is not tpk [%s]!\n",keyName);
		return(errCodeEsscMDL_WrongUsageOfKey);
	}

	// 读取密钥值
	if ((ptpkKeyValue = UnionGetSymmetricKeyValue(&tpkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionGeneratePVVByPinBlockOfTPKEncrypt:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("读取密钥值失败");
		return(errCodeParameter);
	}

	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case	conHsmCmdVerRacalStandardHsmCmd:
		case	conHsmCmdVerSJL06StandardHsmCmd:
			if ((ret = UnionHsmCmdPV(pvk,'3',ptpkKeyValue->keyValue,pinBlock,accNo,pvv)) < 0)
			{
				// modify by leipp 20150323 begin
				if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
					UnionOldVersionSymmetricKeyIsUsed(&tpkKeyDB))
				{
					if ((ret = UnionHsmCmdPV(pvk,'3',ptpkKeyValue->oldKeyValue,pinBlock,accNo,pvv)) < 0)
					{
						UnionUserErrLog("in UnionGeneratePVVByPinBlockOfTPKEncrypt:: UnionHsmCmdPV old version key!\n");
						return(ret);
					}
				}
				else
				{
					UnionUserErrLog("in UnionGeneratePVVByPinBlockOfTPKEncrypt:: UnionHsmCmdPV!\n");
					return(ret);
				}
				// modify end
			}
			break;
		default:
			UnionUserErrLog("in UnionGeneratePVVByPinBlockOfTPKEncrypt:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("非法的加密机指令类型");
		       return(errCodeParameter);
	}
	return 0;
}

int UnionEncryptPinBlockFromZPKToLMK(PUnionHsmGroupRec phsmGroupRec,char *pinBlock,char *accNo,char *newPinBlock)
{
	int	ret = 0;	
	char	keyName[128];

	TUnionSymmetricKeyDB		keyDB;
	PUnionSymmetricKeyValue		pkeyValue = NULL;

	// ZPK密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyNameOfZPK",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionEncryptPinBlockFromZPKToLMK:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyNameOfZPK");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(keyName);
	if (strlen(keyName) == 0)
	{
		UnionUserErrLog("in UnionEncryptPinBlockFromZPKToLMK:: keyNameOfZPK can not be null!\n");
		UnionSetResponseRemark("ZPK密钥名称不能为空");
		return(errCodeParameter);
	}

	// 读取ZPK密钥
	if ((ret = UnionReadSymmetricKeyDBRec(keyName,1,&keyDB)) < 0)
	{
		UnionUserErrLog("in UnionEncryptPinBlockFromZPKToLMK:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}

	if (keyDB.keyType != conZPK)
	{
		UnionUserErrLog("in UnionEncryptPinBlockFromZPKToLMK:: is not zpk [%s]!\n",keyName);
		return(errCodeEsscMDL_WrongUsageOfKey);
	}

	// 读取密钥值
	if ((pkeyValue = UnionGetSymmetricKeyValue(&keyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionEncryptPinBlockFromZPKToLMK:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("ZPK读取密钥值失败");
		return(errCodeParameter);
	}

	switch(keyDB.algorithmID)
	{
		case	conSymmetricAlgorithmIDOfDES:	// DES算法
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmdJE(UnionConvertSymmetricKeyKeyLen(keyDB.keyLen),pkeyValue->keyValue,"01",
						pinBlock,accNo,strlen(accNo),newPinBlock)) < 0)
					{
						// 使用旧版本密钥
						if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
							UnionOldVersionSymmetricKeyIsUsed(&keyDB))
						{
							if ((ret = UnionHsmCmdJE(UnionConvertSymmetricKeyKeyLen(keyDB.keyLen),pkeyValue->oldKeyValue,"01",
								pinBlock,accNo,strlen(accNo),newPinBlock)) < 0)
							{
								UnionUserErrLog("in UnionEncryptPinBlockFromZPKToLMK:: UnionHsmCmdJE old key ret = [%d]!\n",ret);
								return(ret);
							}
						}
						else
						{
							UnionUserErrLog("in UnionEncryptPinBlockFromZPKToLMK:: UnionHsmCmdJE current key ret = [%d]!\n",ret);
							return(ret);
						}
					}
					newPinBlock[ret] = 0;
					break;
				default:
					UnionUserErrLog("in UnionEncryptPinBlockFromZPKToLMK:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}
			break;
		case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
		default:
			UnionUserErrLog("in UnionEncryptPinBlockFromZPKToLMK:: keyDB.algorithmID[%d] error!\n",keyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	
	return 0;
}
