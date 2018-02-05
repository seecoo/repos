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
服务代码:	E133
服务名:		生成CVV
功能描述:	生成CVV,为VISA卡编码产生VISA卡的CVV，用于产生磁条1或磁条2的CVV
		对应的加密机指令为：CW
***************************************/
int UnionDealServiceCodeE133(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	int	accNoLen = 0;
	char	CVV[32];
	char	accNo[64];
	char	expirationDate[32];
	char	serviceCode[32];
	char	keyName[160];
	int	lenOfExpirationDate;
	
	TUnionSymmetricKeyDB		cvkKeyDB;
	PUnionSymmetricKeyValue		pcvkKeyValue = NULL;
	
	// CVK密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE133:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);
	if (strlen(keyName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE133:: keyName can not be null!\n");
		UnionSetResponseRemark("密钥名称不能为空");
		return(errCodeParameter);
	}

	// 读取CVK密钥
	if ((ret = UnionReadSymmetricKeyDBRec(keyName,1,&cvkKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE133:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}
	if (cvkKeyDB.keyType != conCVK)
	{
		UnionUserErrLog("in UnionDealServiceCodeE133:: is not cvk [%s]!\n",keyName);
		return(errCodeEsscMDL_WrongUsageOfKey);
	}

	// 读取密钥值
	if ((pcvkKeyValue = UnionGetSymmetricKeyValue(&cvkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE133:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("读取密钥值失败");
		return(errCodeParameter);
	}

	// 有效期
	// modify by chenqy 201601015
	if ((ret = UnionReadRequestXMLPackageValue("body/expirationDate",expirationDate,sizeof(expirationDate))) <= 0)
	{
		//UnionUserErrLog("in UnionDealServiceCodeE133:: UnionReadRequestXMLPackageValue[%s]!\n","body/expirationDate");
		//return(ret);
		strcpy(expirationDate, "0000");
		UnionDebugLog("in UnionDealServiceCodeE133:: UnionReadRequestXMLPackageValue[%s][%s]!\n","body/expirationDate", expirationDate);
	}
	else
	{
		expirationDate[ret] = 0;
	}
	// modify end

	UnionFilterHeadAndTailBlank(expirationDate);
	
	if ((lenOfExpirationDate = strlen(expirationDate)) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE133:: expirationDate can not be null!\n");
		return(errCodeParameter);
	}
	
	if (!UnionIsDigitString(expirationDate))
	{
		UnionUserErrLog("in UnionDealServiceCodeE133:: expirationDate[%s] is error!\n",expirationDate);
		UnionSetResponseRemark("有效期非法[%s],必须为数字",expirationDate);
		return(errCodeParameter);
	}
	//add by zhouxw 20150910
	if(lenOfExpirationDate != 4)
	{
		UnionUserErrLog("in UnionDealServiceCodeE133:: len[%d] of expirationDate[%s] is error!\n", lenOfExpirationDate, expirationDate);
		UnionSetResponseRemark("有效期长度非法[%s::%d],必须为4位", expirationDate, lenOfExpirationDate);
		return(errCodeParameter);
	}
	//add end
	// 服务码
	if ((ret = UnionReadRequestXMLPackageValue("body/serviceCode",serviceCode,sizeof(serviceCode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE133:: UnionReadRequestXMLPackageValue[%s]!\n","body/serviceCode");
		return(ret);
	}
	serviceCode[ret] = 0;
	if (strlen(serviceCode) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE133:: serviceCode can not be null!\n");
		return(errCodeParameter);
	}

	// 账号
	if ((accNoLen = UnionReadRequestXMLPackageValue("body/accNo",accNo,sizeof(accNo))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE133:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
		return(accNoLen);
	}
	accNo[accNoLen] = 0;

	// 检测账号
	UnionFilterHeadAndTailBlank(accNo);
	if(strlen(accNo) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE133:: accNo can not be null!\n");
                UnionSetResponseRemark("账号不能为空!");
                return(errCodeParameter);
	}
	if (!UnionIsDigitString(accNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE133:: accNo[%s] is error!\n",accNo);
		UnionSetResponseRemark("账号非法[%s],必须为数字",accNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}

	switch(cvkKeyDB.algorithmID)
	{
		case	conSymmetricAlgorithmIDOfDES:	// DES算法
			switch(phsmGroupRec->hsmCmdVersionID)
        		{
                		case conHsmCmdVerRacalStandardHsmCmd:
                		case conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmdCW(expirationDate,UnionConvertSymmetricKeyKeyLen(cvkKeyDB.keyLen),
						pcvkKeyValue->keyValue,accNo,accNoLen,serviceCode,CVV)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE133:: UnionHsmCmdCW!\n");
						return(ret);
					}
					break;
                		default:
                        		UnionUserErrLog("in UnionDealServiceCodeE133:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        		UnionSetResponseRemark("非法的加密机指令类型");
                        		return(errCodeParameter);
        		}
			break;
		case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
			switch(phsmGroupRec->hsmCmdVersionID)
        		{
                		case	conHsmCmdVerRacalStandardHsmCmd:
                		case	conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmdCL(expirationDate,pcvkKeyValue->keyValue,accNo,accNoLen,serviceCode,CVV,sizeof(CVV))) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE133:: UnionHsmCmdCL!\n");
						return(ret);
					}
					break;
                		default:
                        		UnionUserErrLog("in UnionDealServiceCodeE133:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        		UnionSetResponseRemark("非法的加密机指令类型");
                        		return(errCodeParameter);
        		}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE133:: cvkKeyDB.algorithmID[%d] error!\n",cvkKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	
	// 设置CVV
	if ((ret = UnionSetResponseXMLPackageValue("body/CVV",CVV)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE133:: UnionSetResponseXMLPackageValue CVV[%s]!\n",CVV);
		return(ret);
	}

	return(0);
}
