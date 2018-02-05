#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif  


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionHsmCmd.h"
#include "unionXMLPackage.h"
#include "symmetricKeyDB.h"
#include "UnionLog.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "essc5Interface.h"
#include "UnionDes.h"
#include "unionRealBaseDB.h"
#include "unionHsmCmdVersion.h"
#include "asymmetricKeyDB.h"
#include "commWithHsmSvr.h"
#include "asymmetricKeyDB.h"



/***************************************
  服务代码:	E201
  服务名:  	E201 数字PIN转换(PK->ZPK)	
  功能描述:	 E201 数字PIN转换(PK->ZPK)
  判断公钥算法标识进行相对应的算法转换  
 ***************************************/
int UnionDealServicePINBYSM2PKTOZPK(PUnionHsmGroupRec phsmGroupRec);

int UnionDealServicePINBYRSAPKTOZPK(PUnionHsmGroupRec phsmGroupRec);


int UnionDealServiceCodeE201(PUnionHsmGroupRec phsmGroupRec)
{										
	int 	ret;
	char	keyNameofpk[136];


	TUnionAsymmetricKeyDB		asymmetricKeyDB;


	//读取密钥名称
	if((ret = UnionReadRequestXMLPackageValue("body/pkKeyName", keyNameofpk, sizeof(keyNameofpk))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE201:: UnionReadRequestXMLPackageValue[%s]\n", "body/keyName");
		return(ret);
	}
	keyNameofpk[ret] = 0;
	UnionFilterHeadAndTailBlank(keyNameofpk);

	//读取非对称密钥信息
	memset(&asymmetricKeyDB, 0, sizeof(asymmetricKeyDB));
	if ((ret =  UnionReadAsymmetricKeyDBRec(keyNameofpk,1,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE201:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",keyNameofpk);
		return(ret);
	}

	//进行公钥算法标签类型的判断，并进行不同的转加密操作
	switch (asymmetricKeyDB.algorithmID)
	{
		case conAsymmetricAlgorithmIDOfSM2:	

			if((ret = UnionDealServicePINBYSM2PKTOZPK(phsmGroupRec))<0)
			{
				UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: error!\n");	
				return(ret);
			}
			break;

		case conAsymmetricAlgorithmIDOfRSA:

			if((ret = UnionDealServicePINBYRSAPKTOZPK(phsmGroupRec))<0)
			{
				UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: error!\n");
				return(ret);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE201:: keyName [%s] algorithmID = [%d] illegal!\n", asymmetricKeyDB.keyName, asymmetricKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	return 0;
}


int UnionDealServicePINBYRSAPKTOZPK(PUnionHsmGroupRec phsmGroupRec)
{

	int		ret;
	int		lenOfPinByPK;
	char	zpkName[136];
	char	pkName[136];

	TUnionSymmetricKeyDB	tZpk;
	TUnionAsymmetricKeyDB 	tPk;

	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;

	char	accNo[32+1];			//	账号
	char	pinByPK[512+1];			//	公钥加密的PIN			
	char 	pinByPKBcd[512+1];
	char 	formatTmp[2+1];			
	char	pinByZPK[32+1];			//	ZPK加密的PIN

	char			vkIndex[32];
	char			vkValue[4096+1];	
	int 			lenOfVK=0;

	memset(&tZpk,0,sizeof(tZpk));
	memset(&tPk,0,sizeof(tPk));

	if ((ret = UnionReadRequestXMLPackageValue("body/pkKeyName", pkName, sizeof(pkName))) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkKeyName");
		return(ret);
	}
	pkName[ret] = 0;

	if ((ret = UnionReadRequestXMLPackageValue("body/zpkKeyName", zpkName, sizeof(zpkName))) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: UnionReadRequestXMLPackageValue[%s]!\n","body/zpkKeyName");
		return(ret);
	}
	zpkName[ret] = 0;

	if ((ret = UnionReadRequestXMLPackageValue("body/accNo", accNo, sizeof(accNo)))< 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
		return(ret);
	}

	// 检测账号
	accNo[ret] = 0;
	UnionFilterHeadAndTailBlank(accNo);
	if (strlen(accNo) == 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: accNo can not be null!\n");
		UnionSetResponseRemark("账号不能为空!");
		return(errCodeParameter);
	}		
	if (!UnionIsDigitString(accNo))
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: accNo[%s] is error!\n",accNo);
		UnionSetResponseRemark("账号非法[%s],必须为数字",accNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}

	if ((ret = UnionReadRequestXMLPackageValue("body/format", formatTmp, sizeof(formatTmp)))< 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: UnionReadRequestXMLPackageValue[%s]!\n","body/format");
		return(ret);
	}
	else
	{
		formatTmp[ret] = 0;
		if (formatTmp[0] != '0' && formatTmp[0] != '1')
		{
			UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: format[%s] error!\n",formatTmp);
			return(errCodeParameter);
		}
	}

	if ((lenOfPinByPK = UnionReadRequestXMLPackageValue("body/pinByPK", pinByPK, sizeof(pinByPK)))< 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinByPK");
		return(lenOfPinByPK);
	}
	else
	{
		//检测公钥加密的PIN
		pinByPK[lenOfPinByPK] = 0;
		if (lenOfPinByPK == 0)
		{
			UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: pinByPK can not be null!\n");
			UnionSetResponseRemark("公钥加密的PIN不能为空!");
			return(errCodeParameter);
		}
		if (!UnionIsBCDStr(pinByPK))
		{
			UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: pinByPK[%s] is error!\n",pinByPK);
			UnionSetResponseRemark("公钥加密的PIN非法,必须为十六进制数");
			return(errCodeParameter);
		}
	}

	aschex_to_bcdhex(pinByPK, lenOfPinByPK, pinByPKBcd);
	pinByPKBcd[lenOfPinByPK/2] = 0;

	// 读取对称密钥
	if ((ret = UnionReadSymmetricKeyDBRec(zpkName, 1, &tZpk)) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", zpkName);
		return(ret);
	}

	// 读取对称密钥值
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&tZpk,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("读取密钥值失败");
		return(errCodeParameter);
	}

	//读取非对称秘钥
	if ((ret = UnionReadAsymmetricKeyDBRec(pkName, 1, &tPk)) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", pkName);
		return(ret);
	}

	//add  begin by lusj 20160125 添加外带私钥功能
	// 检测是否存在索引
	if (tPk.vkStoreLocation != 0)
	{
		sprintf(vkIndex,"%02d",atoi(tPk.vkIndex));
		vkIndex[2] = 0;
	}
	else
	{
		ret=sprintf(vkIndex,"%s","99");
		vkIndex[ret]=0;

		lenOfVK = strlen(tPk.vkValue);
		memcpy(vkValue,tPk.vkValue,lenOfVK);
		vkValue[lenOfVK] = 0;
	}
	//add end by lusj 20160125

	switch(tZpk.algorithmID)
	{
		case	conSymmetricAlgorithmIDOfDES:   // DES算法法
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					//modify by lusj 20160125 修改内容：添加外带私钥功能，兼容以前功能
					if ((ret= UnionHsmCmd43(vkIndex,lenOfVK,vkValue, '2', psymmetricKeyValue->keyValue, '1' , accNo, (unsigned char*)pinByPKBcd, lenOfPinByPK/2, pinByZPK)) < 0)
					{
						UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK: UnionHsmCmd43 return[%d]!\n", ret);
						return(ret);
					}
					//modify end by lusj 20160125
					if (ret == 0)
					{
						UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK: UnionHsmCmd43 return[%d]!\n", ret);
						return(errCodeHsmCmdMDL_ReturnLen);
					}
					pinByZPK[ret] = 0;
					break;
				default:
					UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}	
			break;
		case	conSymmetricAlgorithmIDOfSM4:   // SM4算法
		default:
			UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: key [%s] algorithmID = [%d] illegal!\n", tZpk.keyName, tZpk.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	//设置 pinByZPK
	if ((ret = UnionSetResponseXMLPackageValue("body/pinByZPK", pinByZPK)) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK: UnionSetResponseXMLPackageValue [%s]!\n", pinByZPK);
		return(ret);
	}
	return 0;
}

int UnionDealServicePINBYSM2PKTOZPK(PUnionHsmGroupRec phsmGroupRec)
{

	int 	ret;
	int 	keyType=0;
	char	keyName[136];
	char	pinByPK[512];
	char	zpkName[136];
	char	format[8];
	char	accNo[40];	
	char	pinByZPK[136];
	int	lenOfPinByPK = 0;
	int 	algorithmID = 0;
	int	lenOfAccNo = 0;
	char	vkIndex[16];
	char				specialAlg[8];

	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue 	psymmetricKeyValue;
	TUnionAsymmetricKeyDB		asymmetricKeyDB;

	//读取SM2密钥名称
	if((ret = UnionReadRequestXMLPackageValue("body/pkKeyName", keyName, sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionReadRequestXMLPackageValue[%s]\n", "body/pkKeyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);

	//读取非对称密钥信息
	memset(&asymmetricKeyDB, 0, sizeof(asymmetricKeyDB));
	if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,1,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",keyName);
		return(ret);
	}

	// 检测私钥类型
	if (asymmetricKeyDB.keyType != conAsymmetricKeyTypeOfEncryption && asymmetricKeyDB.keyType != conAsymmetricKeyTypeOfSignatureAndEncryption)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: keyType[%d] is not 1 or 2!\n",asymmetricKeyDB.keyType);
		UnionSetResponseRemark("私钥类型[%d]不支持加密,必须为[1或2]");
		return(errCodeHsmCmdMDL_InvalidKeyType);
	}

	if (asymmetricKeyDB.vkStoreLocation != 1 && asymmetricKeyDB.vkStoreLocation != 2)
		snprintf(vkIndex,sizeof(vkIndex),"99");
	else
		snprintf(vkIndex,sizeof(vkIndex),"%s",asymmetricKeyDB.vkIndex);

	//读取加密的密文
	if((lenOfPinByPK = UnionReadRequestXMLPackageValue("body/pinByPK", pinByPK, sizeof(pinByPK))) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionReadRequestXMLPackageValue[%s]\n","body/pinByPK");
		return(lenOfPinByPK);
	}
	pinByPK[lenOfPinByPK] = 0;

	//读取zpk密钥名称
	if((ret = UnionReadRequestXMLPackageValue("body/zpkKeyName", zpkName, sizeof(zpkName))) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionReadRequestXMLPackageValue[%s]\n", "body/zpkKeyName");
		return(ret);
	}
	zpkName[ret] = 0;
	UnionFilterHeadAndTailBlank(zpkName);

	//读取对称密钥信息
	memset(&symmetricKeyDB, 0, sizeof(symmetricKeyDB));
	if((ret = UnionReadSymmetricKeyDBRec(zpkName, 1, &symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionReadAsymmetricKeyDBRec zpkName[%s]\n",zpkName);
		return(ret);
	}

	//modify by lusj 2016118 原版本只支持ZPK类型，先添加TPK类型
	if(conZPK == symmetricKeyDB.keyType)
	{	
		keyType=1;
	}
	else if (conTPK == symmetricKeyDB.keyType)
	{
		keyType=2;
	}
	else
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: keyType err\n");
		UnionSetResponseRemark("密钥类型错,不为zpk/tpk密钥");
		return(errCodeHsmCmdMDL_InvalidKeyType);
	}
	//modify end by lusj 

	if(conSymmetricAlgorithmIDOfSM4 == symmetricKeyDB.algorithmID) // 国密
		algorithmID = 1;
	else if(conSymmetricAlgorithmIDOfDES == symmetricKeyDB.algorithmID) // 国际
		algorithmID = 2;
	else
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: algorithmID[%d]\n",symmetricKeyDB.algorithmID);
		UnionSetResponseRemark("zpk密钥算法标识错[必须是DES或SM4密钥]");
		return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	//读取对称密钥值
	if((psymmetricKeyValue= UnionGetSymmetricKeyValue(&symmetricKeyDB, phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionGetSymmetricKeyValue err\n");
		UnionSetResponseRemark("读取对称密钥值失败");
		return(errCodeParameter);
	}

	//读取PIN格式
	if((ret = UnionReadRequestXMLPackageValue("body/format",format,sizeof(format))) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionReadRequestXMLPackageValue[%s]\n", "body/format");
		return(ret);
	}
	format[ret] = 0;

	//读取账号
	if((lenOfAccNo = UnionReadRequestXMLPackageValue("body/accNo", accNo, sizeof(accNo))) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionReadRequestXMLPackageValue[%s]\n", "body/accNo");	
		return(lenOfAccNo);
	}
	accNo[lenOfAccNo] = 0;

	//add by lusj 20160118   柳州银行专用算法，兼容通用版本
	//专用算法标示specialAlg
	memset(specialAlg,0,sizeof(specialAlg));
	if ((ret = UnionReadRequestXMLPackageValue("body/specialAlg",specialAlg,sizeof(specialAlg))) >0)
	{
		specialAlg[ret]=0;
	}
	//add end lusj

	switch(asymmetricKeyDB.algorithmID)
	{
		case	conAsymmetricAlgorithmIDOfSM2:
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					if((ret = unionHsmCmdKK(vkIndex, strlen(asymmetricKeyDB.vkValue), asymmetricKeyDB.vkValue, lenOfPinByPK, pinByPK, algorithmID,keyType, format,lenOfAccNo, accNo, psymmetricKeyValue->keyValue,specialAlg, pinByZPK,sizeof(pinByZPK))) < 0)
					{
						UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionHsmCmdKK \n");
						return(ret);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}	
			break;
		case	conAsymmetricAlgorithmIDOfRSA:
		default:
			UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: keyName [%s] algorithmID = [%d] illegal!\n", asymmetricKeyDB.keyName, asymmetricKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	if((ret = UnionSetResponseXMLPackageValue("body/pinByZPK", pinByZPK)) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionSetResponseXMLPackageValue[%s]\n", "body/pinByZPK");
		return(ret);
	}	
	return 0;
}
