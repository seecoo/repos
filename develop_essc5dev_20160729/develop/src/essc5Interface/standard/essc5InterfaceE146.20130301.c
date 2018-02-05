//	Author:		chenqy
//	Copyright:	Union Tech. Guangzhou
//	Date:		2015-08-03

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symmetricKeyDB.h"
#include "asymmetricKeyDB.h"
#include "essc5Interface.h"
#include "unionHsmCmd.h"
#include "unionXMLPackage.h"
#include "unionErrCode.h"
#include "unionRealBaseDB.h"
#include "unionHsmCmdVersion.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "UnionDes.h"

/***************************************
服务代码:	E146
服务名:		将ZPK（DES和SM4）加密的pin转成SM2公钥加密
功能描述:	将ZPK（DES和SM4）加密的pin转成SM2公钥加密
***************************************/

int UnionDealServiceCodeE146(PUnionHsmGroupRec phsmGroupRec)
{
	int 	ret;
	char 	keyName[136];
	int 	algorithmID = 0;
	int 	keyType = 0;
	char 	format[8];
	char 	accNo[40];
	int		lenOfAccNo = 0;
	char 	pinBlock[136];
	char 	SM2KeyName[136];
	char 	SM2Index[16];
	char 	pinByPK[512];
	
	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue;
	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	
	// 报文中读取ZPK或TPK密钥名称
	if (0 > (ret = UnionReadRequestXMLPackageValue("body/keyName", keyName, sizeof(keyName)))) 
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: UnionReadRequestXMLPackageValue[%s]\n", "body/keyName");
		return ret;
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);
	
	// 对称密钥信息
	memset(&symmetricKeyDB, 0, sizeof(symmetricKeyDB));
	if (0 > (ret = UnionReadSymmetricKeyDBRec(keyName, 1, &symmetricKeyDB)))
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: UnionReadSymmetricKeyDBRec keyName[%s]\n", keyName);
		return ret;
	}
	
	// 对称密钥算法标识
	if(conSymmetricAlgorithmIDOfSM4 == symmetricKeyDB.algorithmID) // 国密
		algorithmID = 1;
	else if(conSymmetricAlgorithmIDOfDES == symmetricKeyDB.algorithmID) // 国际
		algorithmID = 2;
	else
	{
		UnionUserErrLog("in UnionDealServiceCode146:: algorithmID[%d]\n",symmetricKeyDB.algorithmID);
		UnionSetResponseRemark("zpk密钥算法标识错[必须是DES或SM4密钥]");
		return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	
	// 对称密钥类型
	if (conZPK == symmetricKeyDB.keyType)
		keyType = 1;
	else if (conTPK == symmetricKeyDB.keyType)
		keyType = 2;
	else
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: keyType err\n");
		UnionSetResponseRemark("密钥类型错,不为ZPK或TPK密钥");
		return(errCodeHsmCmdMDL_InvalidKeyType);
	}
	
	// 对称密钥值
	if (NULL == (psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB, phsmGroupRec->lmkProtectMode)))
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: UnionGetSymmetricKeyValue err\n");
		UnionSetResponseRemark("读取对称密钥值失败");
		return(errCodeParameter);
	}
	
	// 报文中读取PIN格式
	if (0 > (ret = UnionReadRequestXMLPackageValue("body/format", format, sizeof(format))))
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: UnionReadRequestXMLPackageValue[%s]\n", "body/format");
		return ret;
	}
	format[ret] = 0;
	
	// 报文中读取账号
	if (0 > (lenOfAccNo = UnionReadRequestXMLPackageValue("body/accNo", accNo, sizeof(accNo))))
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: UnionReadRequestXMLPackageValue[%s]\n", "body/accNo");		
		return lenOfAccNo;
	}
	accNo[lenOfAccNo] = 0;
	
	// 报文中读取对称密钥加密的pin块
	if (0 > (ret = UnionReadRequestXMLPackageValue("body/pinBlock", pinBlock, sizeof(pinBlock))))
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: UnionReadRequestXMLPackageValue[%s]\n","body/pinBlock");
		return ret;
	}
	pinBlock[ret] = 0;
			
	// 报文中读取SM2密钥名称
	if(0 > (ret = UnionReadRequestXMLPackageValue("body/SM2KeyName", SM2KeyName, sizeof(SM2KeyName))))
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: UnionReadRequestXMLPackageValue[%s]\n", "body/SM2KeyName");
		return(ret);
	}
	SM2KeyName[ret] = 0;
	UnionFilterHeadAndTailBlank(SM2KeyName);
	
	// SM2密钥密钥信息
	memset(&asymmetricKeyDB, 0, sizeof(asymmetricKeyDB));
	if ((ret =  UnionReadAsymmetricKeyDBRec(SM2KeyName,1,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: UnionReadAsymmetricKeyDBRec SM2KeyName[%s]!\n",SM2KeyName);
		return(ret);
	}
	
	// SM2密钥索引
	if (asymmetricKeyDB.vkStoreLocation != 1)
		snprintf(SM2Index, sizeof(SM2Index), "99");
	else
	{
		//snprintf(SM2Index, sizeof(SM2Index), "%s", asymmetricKeyDB.vkIndex);
		UnionUserErrLog("in UnionDealServiceCodeE146:: asymmetricKeyDB.vkIndex[%d]\n", atoi(asymmetricKeyDB.vkIndex));
		UnionSetResponseRemark("只能使用存放在密钥库里的SM2密钥");
		return(errCodeParameter);
	}
	
	// 向密码机发送指令
	memset(pinByPK, 0, sizeof(pinByPK));
	switch (phsmGroupRec->hsmCmdVersionID)
	{
		  case	conHsmCmdVerRacalStandardHsmCmd:
		  case	conHsmCmdVerSJL06StandardHsmCmd:
		  		if (0 > (ret = UnionHsmCmdKN(algorithmID, atoi(SM2Index), strlen(asymmetricKeyDB.vkValue), asymmetricKeyDB.vkValue, 
					keyType, NULL, psymmetricKeyValue->keyValue, format, lenOfAccNo, accNo, pinBlock, pinByPK, sizeof(pinByPK))))
				{
					UnionUserErrLog("in UnionDealServiceCodeE146:: UnionHsmCmdKN, ret = [%d]\n", ret);
					return ret;
				}
				break;
		  default:
				UnionUserErrLog("in UnionDealServiceCodeE146:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
				UnionSetResponseRemark("非法的加密机指令类型");
				return errCodeParameter;
	}
	
	// 设置报文返回
	if (0 > (ret = UnionSetResponseXMLPackageValue("body/pinByPK", pinByPK)))
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: UnionSetResponseXMLPackageValue[%s]\n", "body/pinByPK");
		return ret;
	}
	
	return 0;
}

