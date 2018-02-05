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
服务代码:	E141
服务名:		解密PIN
功能描述:	解密PIN
***************************************/
int UnionDealServiceCodeE141(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				pinLen = 0;
	int				mode = 1;
	char				plainPin[32];
	char				tmpBuf[32];
	char				accNo[64];
	char				pinBlock[64];
	char				keyName[160];
	char				format[32];
	char				referenceNumber[32];

	TUnionSymmetricKeyDB		zpkKeyDB;
	PUnionSymmetricKeyValue		pzpkKeyValue = NULL;
	
	// PIN密文
	if ((pinLen = UnionReadRequestXMLPackageValue("body/pinBlock",pinBlock,sizeof(pinBlock))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE141:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinBlock");
		return(pinLen);
	}
	else
        {
		UnionFilterHeadAndTailBlank(pinBlock);
        	if (strlen(pinBlock) == 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeE141:: pinBlock can not be null!\n");
                	UnionSetResponseRemark("PIN密文不能为空!");
                	return(errCodeParameter);
        	}
                if (!UnionIsBCDStr(pinBlock))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE141:: pinBlock[%s] is error!\n",pinBlock);
                        UnionSetResponseRemark("PIN密文[%s]非法,必须为十六进制数",pinBlock);
                        return(errCodeParameter);
                }
        }
	
	// 账号
	if ((ret = UnionReadRequestXMLPackageValue("body/accNo",accNo,sizeof(accNo))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE141:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
		return(ret);
	}

	// 检测账号
	UnionFilterHeadAndTailBlank(accNo);
        if (strlen(accNo) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE141:: accNo can not be null!\n");
                UnionSetResponseRemark("账号不能为空!");
                return(errCodeParameter);
        }
	if (!UnionIsDigitString(accNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE141:: accNo[%s] is error!\n",accNo);
		UnionSetResponseRemark("账号非法[%s],必须为数字",accNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}

	// 模式
	// 1：LMK02-03解密明文PIN
	// 2：指定zpk解密pin
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) < 0)
		mode = 1;
	else
		mode = atoi(tmpBuf);

	if (mode == 2)
	{
		// ZPK密钥名称
		if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE141:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
			return(ret);
		}
		keyName[ret] = 0;

		UnionFilterHeadAndTailBlank(keyName);
		if (strlen(keyName) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE141:: keyName can not be null!\n");
			UnionSetResponseRemark("密钥名称不能为空");
			return(errCodeParameter);
		}
		
		// 以前报文format不填数据会报错 , 修改添加=0这种情况 bychengj 20160721 
		if ((ret = UnionReadRequestXMLPackageValue("body/format",format,sizeof(format))) <= 0)
			strcpy(format,"01");
		//end

		if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1,&zpkKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE141:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
			return(ret);
		}
		if (zpkKeyDB.keyType != conZPK)
		{
			UnionUserErrLog("in UnionDealServiceCodeE141:: is not zpk [%s]!\n",keyName);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}
		// 读取密钥值
		if ((pzpkKeyValue = UnionGetSymmetricKeyValue(&zpkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE141:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("读取密钥值失败");
			return(errCodeParameter);
		}

		switch(zpkKeyDB.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:	// DES算法
				switch(phsmGroupRec->hsmCmdVersionID)
        			{
                			case conHsmCmdVerRacalStandardHsmCmd:
                			case conHsmCmdVerSJL06StandardHsmCmd:
						if ((pinLen = UnionHsmCmdJE(zpkKeyDB.keyLen/4,pzpkKeyValue->keyValue,format,pinBlock,accNo,strlen(accNo),pinBlock)) < 0)
						{
							// 使用旧版本密钥
							if ((pinLen <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&zpkKeyDB))
							{
								if ((pinLen = UnionHsmCmdJE(zpkKeyDB.keyLen/4,pzpkKeyValue->oldKeyValue,format,pinBlock,accNo,strlen(accNo),pinBlock)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE141:: UnionHsmCmdJG old key ret = [%d]!\n",ret);
									return(pinLen);
								}
							}
							else
							{
								UnionUserErrLog("in UnionDealServiceCodeE141:: UnionHsmCmdJG current key ret = [%d]!\n",ret);
								return(pinLen);
							}
						}
						break;
                			default:
                        			UnionUserErrLog("in UnionDealServiceCodeE141:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        			UnionSetResponseRemark("非法的加密机指令类型");
                        			return(errCodeParameter);
        			}
				break;
			case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
				switch(phsmGroupRec->hsmCmdVersionID)
        			{
                			case conHsmCmdVerRacalStandardHsmCmd:
                			case conHsmCmdVerSJL06StandardHsmCmd:
						if ((pinLen = UnionHsmCmdWX(1,pzpkKeyValue->keyValue,pinBlock,format,accNo,strlen(accNo),pinBlock,sizeof(pinBlock))) < 0)
						{
							// 使用旧版本密钥
							if ((pinLen <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&zpkKeyDB))
							{
								if ((pinLen = UnionHsmCmdWX(1,pzpkKeyValue->oldKeyValue,pinBlock,format,accNo,strlen(accNo),pinBlock,sizeof(pinBlock))) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE141:: UnionHsmCmdWX old key ret = [%d]!\n",ret);
									return(pinLen);
								}
							}
							else
							{
								UnionUserErrLog("in UnionDealServiceCodeE141:: UnionHsmCmdJG current key ret = [%d]!\n",ret);
								return(pinLen);
							}
						}
						break;
                			default:
                        			UnionUserErrLog("in UnionDealServiceCodeE141:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        			UnionSetResponseRemark("非法的加密机指令类型");
                        			return(errCodeParameter);
        			}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE141:: zpkKeyDB.algorithmID[%d] error!\n",zpkKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
	}
	
	if (mode == 1 || (zpkKeyDB.algorithmID == conSymmetricAlgorithmIDOfDES && mode == 2))
	{
		switch(phsmGroupRec->hsmCmdVersionID)
		{
			case conHsmCmdVerRacalStandardHsmCmd:
			case conHsmCmdVerSJL06StandardHsmCmd:
				if ((ret = UnionHsmCmdNG(pinLen,pinBlock,accNo,strlen(accNo),referenceNumber,plainPin)) < 0)
				{  
					UnionUserErrLog("in UnionDealServiceCodeE141:: UnionHsmCmdNG!\n");
					return(ret);
				}
				 break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE141:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
				UnionSetResponseRemark("非法的加密机指令类型");
				return(errCodeParameter);
		}
	}
	else if (mode == 0 || (zpkKeyDB.algorithmID == conSymmetricAlgorithmIDOfSM4 && mode == 2))
	{
		switch(phsmGroupRec->hsmCmdVersionID)
		{
			case	conHsmCmdVerRacalStandardHsmCmd:
			case	conHsmCmdVerSJL06StandardHsmCmd:
				if ((ret = UnionHsmCmdWW(1,pinBlock,accNo,strlen(accNo),plainPin,sizeof(plainPin),referenceNumber,sizeof(referenceNumber))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE141:: UnionHsmCmdWW!\n");
					return(ret);
				}
				break;
			default:	
				UnionUserErrLog("in UnionDealServiceCodeE141:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
				UnionSetResponseRemark("非法的加密机指令类型");
				return(errCodeParameter);
		}
	
	}
	
	// 设置PIN密文
	if ((ret = UnionSetResponseXMLPackageValue("body/plainPin",plainPin)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE141:: UnionSetResponseXMLPackageValue plainPin[%s]!\n",plainPin);
		return(ret);
	}

	// 设置参考数
	if ((ret = UnionSetResponseXMLPackageValue("body/referenceNumber",referenceNumber)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE141:: UnionSetResponseXMLPackageValue referenceNumber[%s]!\n",referenceNumber);
		return(ret);
	}
	
	return(0);
}
