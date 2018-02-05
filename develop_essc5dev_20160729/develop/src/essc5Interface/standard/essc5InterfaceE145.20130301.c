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
#include "symmetricKeyDB.h"
#include "unionHsmCmdVersion.h"

/***************************************
服务代码:	E145
服务名:		验证CVV
功能描述:	验证CVV
***************************************/
int UnionDealServiceCodeE145(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				accNoLen = 0;
	char				CVV[32];
	char				localCVV[32];
	char				accNo[64];
	char				expirationDate[32];
	char				serviceCode[32];
	char				keyName[160];
	int				isVerifyOldKey = 0;
	char				keyValue[64];
	TUnionSymmetricKeyDB		cvkKeyDB;
	PUnionSymmetricKeyValue		pcvkKeyValue = NULL;
	
	// CVV
	if ((ret = UnionReadRequestXMLPackageValue("body/CVV",CVV,sizeof(CVV))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE145:: UnionReadRequestXMLPackageValue[%s]!\n","body/CVV");
		return(ret);
	}

	// CVK密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE145:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;

	// 读取CVK密钥
	if ((ret = UnionReadSymmetricKeyDBRec(keyName,1,&cvkKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE145:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}
	if (cvkKeyDB.keyType != conCVK)
	{
		UnionUserErrLog("in UnionDealServiceCodeE145:: is not cvk [%s]!\n",keyName);
		return(errCodeEsscMDL_WrongUsageOfKey);
	}
	// 读取密钥值
	if ((pcvkKeyValue = UnionGetSymmetricKeyValue(&cvkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE145:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("读取密钥值失败");
		return(errCodeParameter);
	}

	// 有效期
	if ((ret = UnionReadRequestXMLPackageValue("body/expirationDate",expirationDate,sizeof(expirationDate))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE145:: UnionReadRequestXMLPackageValue[%s]!\n","body/expirationDate");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(expirationDate);

	// add modify by leipp 20151027,只判断长度和是否是数字
	if ((strlen(expirationDate) != 4) || (!UnionIsDigitString(expirationDate)))
	{
		UnionUserErrLog("in UnionDealServiceCodeE145:: expirationDate[%s] is error!\n", expirationDate);
		UnionSetResponseRemark("有效期[%s],长度必须为4位,且必须为数字", expirationDate);
		return(errCodeParameter);
	}
	// add end

	// 服务码
	if ((ret = UnionReadRequestXMLPackageValue("body/serviceCode",serviceCode,sizeof(serviceCode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE145:: UnionReadRequestXMLPackageValue[%s]!\n","body/serviceCode");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(serviceCode);
        if (strlen(serviceCode) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE145:: serviceCode can not be null!\n");
                UnionSetResponseRemark("服务码不能为空!");
                return(errCodeParameter);
        }

	// 账号
	if ((accNoLen = UnionReadRequestXMLPackageValue("body/accNo",accNo,sizeof(accNo))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE145:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
		return(accNoLen);
	}
	accNo[accNoLen] = 0;

	// 检测账号
	UnionFilterHeadAndTailBlank(accNo);
        if (strlen(accNo) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE145:: accNo can not be null!\n");
                UnionSetResponseRemark("账号不能为空!");
                return(errCodeParameter);
        }
	if (!UnionIsDigitString(accNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE145:: accNo[%s] is error!\n",accNo);
		UnionSetResponseRemark("账号非法[%s],必须为数字",accNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}
	
	switch(cvkKeyDB.algorithmID)
	{
		case	conSymmetricAlgorithmIDOfDES:	// DES算法
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmdCY(CVV,expirationDate,UnionConvertSymmetricKeyKeyLen(cvkKeyDB.keyLen),
						pcvkKeyValue->keyValue,accNo,accNoLen,serviceCode)) < 0)
					{
						// 使用旧版本密钥
						if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
							UnionOldVersionSymmetricKeyIsUsed(&cvkKeyDB))
						{
							if ((ret = UnionHsmCmdCY(CVV,expirationDate,UnionConvertSymmetricKeyKeyLen(cvkKeyDB.keyLen),
								pcvkKeyValue->oldKeyValue,accNo,accNoLen,serviceCode)) < 0)
							{
								if (ret == errCodeOffsetOfHsmReturnCodeMDL - 1)
									ret = errCodeEsscMDL_CVVVerifyFailure;
								UnionUserErrLog("in UnionDealServiceCodeE145:: UnionHsmCmdCY current key ret = [%d]!\n",ret);
								return(ret);
							}
						}
						else
						{
							if (ret == errCodeOffsetOfHsmReturnCodeMDL - 1)
								ret = errCodeEsscMDL_CVVVerifyFailure;
							UnionUserErrLog("in UnionDealServiceCodeE145:: UnionHsmCmdCY current key ret = [%d]!\n",ret);
							return(ret);
						}
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE145:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}
			break;
		case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
			switch(phsmGroupRec->hsmCmdVersionID)
        		{
                		case	conHsmCmdVerRacalStandardHsmCmd:
                		case	conHsmCmdVerSJL06StandardHsmCmd:
					snprintf(keyValue,sizeof(keyValue),"%s",pcvkKeyValue->keyValue);
againVerify:
					if ((ret = UnionHsmCmdCL(expirationDate,keyValue,accNo,accNoLen,serviceCode,localCVV,sizeof(localCVV))) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE145:: UnionHsmCmdCL current key ret = [%d]!\n",ret);
						return(ret);
					}
					if (memcmp(CVV,localCVV,3) != 0)
					{
						if (isVerifyOldKey == 0 && 
							UnionOldVersionSymmetricKeyIsUsed(&cvkKeyDB))
						{
							isVerifyOldKey = 1;
							snprintf(keyValue,sizeof(keyValue),"%s",pcvkKeyValue->oldKeyValue);
							goto againVerify;
						}	
						UnionUserErrLog("in UnionDealServiceCodeE145:: CVV[%s] is error!\n",CVV);
						return(errCodeEsscMDL_CVVVerifyFailure);
					}
					break;
                		default:
                        		UnionUserErrLog("in UnionDealServiceCodeE133:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        		UnionSetResponseRemark("非法的加密机指令类型");
                        		return(errCodeParameter);
        		}
			break;

		default:
			UnionUserErrLog("in UnionDealServiceCodeE145:: cvkKeyDB.algorithmID[%d] error!\n",cvkKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	
	return(0);
}
