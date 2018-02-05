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
服务代码:	E130
服务名:		生成PIN
功能描述:	生成PIN
***************************************/
int UnionDealServiceCodeE130(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	int	mode = 1;
	int	len = 0;
	int	pinLen = 0;
	char	tmpBuf[32];
	char	accNo[64];
	char	pinByLmk[64];
	char	pinOffset[32];
	char	keyName[160];

	TUnionSymmetricKeyDB		pvkKeyDB;
	PUnionSymmetricKeyValue		ppvkKeyValue = NULL;
	
	// PIN长度
	if ((ret = UnionReadRequestXMLPackageValue("body/pinLen",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE130:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinLen");
		return(ret);
	}
	pinLen = atoi(tmpBuf);
	if (pinLen < 4 || pinLen > 12)
	{
		UnionUserErrLog("in UnionDealServiceCodeE130:: pinLen[%d]!\n",pinLen);
		UnionSetResponseRemark("PIN长度[%d]非法,必须在[4,12]之间",pinLen);
		return(errCodeYLQZMDL_InvalidPinLength);
	}

	// 账号
	if ((len = UnionReadRequestXMLPackageValue("body/accNo",accNo,sizeof(accNo))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE130:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
		return(len);
	}
	accNo[len] = 0;	

	// 检测账号
	UnionFilterHeadAndTailBlank(accNo);
	if((len = strlen(accNo)) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE130:: accNo can not be null!\n");
                UnionSetResponseRemark("账号不能为空!");
                return(errCodeParameter);
	}
	if (!UnionIsDigitString(accNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE130:: accNo[%s] is error!\n",accNo);
		UnionSetResponseRemark("账号非法[%s],必须为数字",accNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}

	// 模式
	// 0: 随机生成(国密)
	// 1：随机生成
	// 2：IBM PINOffset导出
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) < 0)
		mode = 1;
	else if (atoi(tmpBuf) != 1 && atoi(tmpBuf) != 2 && atoi(tmpBuf) != 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE130:: mode[%s] is error!\n",tmpBuf);
                UnionSetResponseRemark("模式[%s]非法,必须为0、1或者2",tmpBuf);
                return(errCodeParameter);
	}
	else
		mode = atoi(tmpBuf);

	switch(mode)
	{
		case 0:
		// 调用WV指令
		switch(phsmGroupRec->hsmCmdVersionID)
		{
			case	conHsmCmdVerRacalStandardHsmCmd:
			case	conHsmCmdVerSJL06StandardHsmCmd:	
				if ((ret =  UnionHsmCmdWV(pinLen,accNo,len,pinByLmk,sizeof(pinByLmk))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE130:: UnionHsmCmdWV ret = [%d]!\n",ret);
					return(ret);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE130:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
				UnionSetResponseRemark("非法的加密机指令类型");
				return(errCodeParameter);
		}
		break;
		case 1:
			// 调用JA指令
		switch(phsmGroupRec->hsmCmdVersionID)
	        {
        	        case conHsmCmdVerRacalStandardHsmCmd:
        	        case conHsmCmdVerSJL06StandardHsmCmd:	
				if ((ret =  UnionHsmCmdJA(pinLen,accNo,len,pinByLmk)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE130:: UnionHsmCmdJA ret = [%d]!\n",ret);
					return(ret);
				}
				break;
                	default:
                        	UnionUserErrLog("in UnionDealServiceCodeE130:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        	UnionSetResponseRemark("非法的加密机指令类型");
                        	return(errCodeParameter);
        	}
			break;
		case 2:
			// pinOffset
			if ((ret = UnionReadRequestXMLPackageValue("body/pinOffset",pinOffset,sizeof(pinOffset))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE130:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinOffset");
				return(ret);
			}
			else
        		{
				UnionFilterHeadAndTailBlank(pinOffset);
				if(strlen(pinOffset) == 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE130:: pinOffset can not be null!\n");
                			UnionSetResponseRemark("PINOffset不能为空!");
                			return(errCodeParameter);
				}
                		if (!UnionIsBCDStr(pinOffset))
                		{
                        		UnionUserErrLog("in UnionDealServiceCodeE1301:: pinOffset[%s] is error!\n",pinOffset);
                       			UnionSetResponseRemark("pinOffset[%s]非法,必须为十六进制数",pinOffset);
                        		return(errCodeParameter);
                		}
       			}
		
			// PVK密钥名称
			if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE130:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
				return(ret);
			}
			keyName[ret] = 0;
	
			if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1,&pvkKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE130:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
				return(ret);
			}
			if (pvkKeyDB.keyType != conPVK)
			{
				UnionUserErrLog("in UnionDealServiceCodeE130:: is not pvk [%s]!\n",keyName);
				return(errCodeEsscMDL_WrongUsageOfKey);
			}

			// 读取密钥值
			if ((ppvkKeyValue = UnionGetSymmetricKeyValue(&pvkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
			{
				UnionUserErrLog("in UnionDealServiceCodeE130:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
				UnionSetResponseRemark("读取密钥值失败");
				return(errCodeParameter);
			}

			switch(pvkKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DES算法
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdEE(pinLen,UnionGetIBMUserDefinedData(),UnionGetIBMDecimalizationTable(),
							pinOffset,UnionConvertSymmetricKeyKeyLen(pvkKeyDB.keyLen),ppvkKeyValue->keyValue,accNo,strlen(accNo),pinByLmk)) < 0)
							{
								// 使用旧版本密钥
								if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
									UnionOldVersionSymmetricKeyIsUsed(&pvkKeyDB))
								{
									if ((ret = UnionHsmCmdEE(pinLen,UnionGetIBMUserDefinedData(),UnionGetIBMDecimalizationTable(),
									pinOffset,UnionConvertSymmetricKeyKeyLen(pvkKeyDB.keyLen),ppvkKeyValue->oldKeyValue,accNo,strlen(accNo),pinByLmk)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE130:: UnionHsmCmdEE old key ret = [%d]!\n",ret);
										return(ret);
									}
								}
								else
								{
									UnionUserErrLog("in UnionDealServiceCodeE130:: UnionHsmCmdEE curerent key ret = [%d]!\n",ret);
									return(ret);
								}
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE130:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
				default:
					UnionUserErrLog("in UnionDealServiceCodeE130:: pvkKeyDB.algorithmID[%d] error!\n",pvkKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE130:: mode[%d] error!\n",mode);
			return(errCodeParameter);
	}
	// 设置PIN密文
	if ((ret = UnionSetResponseXMLPackageValue("body/pinBlock",pinByLmk)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE130:: UnionSetResponseXMLPackageValue pinBlock[%s]!\n",pinByLmk);
		return(ret);
	}
	
	return(0);
}


