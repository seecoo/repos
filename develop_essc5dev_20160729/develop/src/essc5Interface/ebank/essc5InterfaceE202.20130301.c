//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "commWithHsmSvr.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "symmetricKeyDB.h"
#include "asymmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "unionHsmCmdVersion.h"


/***************************************

服务代码:	E202
服务名:  	E202 字符PIN转换(PK->ZPK)
功能描述:	E202 字符PIN转换(PK->ZPK)

***************************************/
int UnionDealServicePINBYSM2PKTOZPK(PUnionHsmGroupRec phsmGroupRec);
 
int UnionDealServicePINBYRSAPKTOZPK(PUnionHsmGroupRec phsmGroupRec);

int UnionDealServiceCodeE202(PUnionHsmGroupRec phsmGroupRec)
{
	int 	ret;
	char	keyNameofpk[136];


	TUnionAsymmetricKeyDB		asymmetricKeyDB;


		//读取密钥名称
	if((ret = UnionReadRequestXMLPackageValue("body/pkKeyName", keyNameofpk, sizeof(keyNameofpk))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE202:: UnionReadRequestXMLPackageValue[%s]\n", "body/keyName");
		return(ret);
	}
	keyNameofpk[ret] = 0;
	UnionFilterHeadAndTailBlank(keyNameofpk);

	//读取非对称密钥信息
	memset(&asymmetricKeyDB, 0, sizeof(asymmetricKeyDB));
	if ((ret =  UnionReadAsymmetricKeyDBRec(keyNameofpk,1,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE202:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",keyNameofpk);
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
			UnionUserErrLog("in UnionDealServiceCodeE202:: keyName [%s] algorithmID = [%d] illegal!\n", asymmetricKeyDB.keyName, asymmetricKeyDB.algorithmID);
            return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	return 0;
}

int UnionDealServicePINBYSM2PKTOZPK(PUnionHsmGroupRec phsmGroupRec)
{
	int 	ret;
	char	keyName[136];
	char	pinByPK[512];
	char	zpkName[136];
	char	pinByZPK[136];
	int	lenOfPinByPK = 0;
	int 	algorithmID = 0;
	char	vkIndex[16];
	
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
	
	if(conZPK != symmetricKeyDB.keyType)
	{	
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: keyType err\n");
		UnionSetResponseRemark("密钥类型错,不为zpk密钥");
		return(errCodeHsmCmdMDL_InvalidKeyType);
	}
	
	if(conSymmetricAlgorithmIDOfSM4 == symmetricKeyDB.algorithmID) // 国密
		algorithmID = 1;
	else if(conSymmetricAlgorithmIDOfDES == symmetricKeyDB.algorithmID) // 国际
		algorithmID = 2;
	else
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPKL:: algorithmID[%d]\n",symmetricKeyDB.algorithmID);
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
	
	switch(asymmetricKeyDB.algorithmID)
	{
		case	conAsymmetricAlgorithmIDOfSM2:
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					if((ret = UnionHsmCmdKL(vkIndex, strlen(asymmetricKeyDB.vkValue), asymmetricKeyDB.vkValue, lenOfPinByPK, pinByPK, algorithmID, 1, psymmetricKeyValue->keyValue, 0, 0, NULL, pinByZPK,sizeof(pinByZPK), NULL, 0)) < 0)
					{
						UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionHsmCmdKL \n");
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
	
	return(0);
}

int UnionDealServicePINBYRSAPKTOZPK(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	pkName[136];
	char	zpkName[136];

	TUnionAsymmetricKeyDB		tPk;
	TUnionSymmetricKeyDB		tZpk;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;

	char	accNo[32];			//	账号
	char	pinByPK[1024];			//	公钥加密的PIN			
	char 	format[32];	
	char	Specialalgorithm[16];		
	char	tmpBuf[2048];
	char	pinBlock[64];
	char	newPinBlock[64];
	int	lenOfVK = 0;
	int	lenOfAccNo = 0;
	int	pinLen = 0;

	if ((ret = UnionReadRequestXMLPackageValue("body/pkKeyName", pkName, sizeof(pkName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE202:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkKeyName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE202:: pkKeyName is null!\n");
		return(errCodeParameter);
	}
	pkName[ret] = 0;

	if ((ret = UnionReadRequestXMLPackageValue("body/zpkKeyName", zpkName, sizeof(zpkName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE202:: UnionReadRequestXMLPackageValue[%s]!\n","body/zpkKeyName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE202:: zpkKeyName is null!\n");
		return(errCodeParameter);
	}
	zpkName[ret] = 0;

	if ((ret = UnionReadRequestXMLPackageValue("body/accNo", accNo, sizeof(accNo))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE202:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
		return(ret);
	}else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE202:: accNo can not be null!\n");
		return(errCodeParameter);
	}
	accNo[ret] = 0;
	
	//add by zhouxw 20150923
	UnionFilterHeadAndTailBlank(accNo);
	if((lenOfAccNo = strlen(accNo)) > 12)
		accNo[lenOfAccNo - 1] = '0';
	//add end

	// 检测账号
	if (!UnionIsDigitString(accNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE202:: accNo[%s] is error!\n",accNo);
		UnionSetResponseRemark("账号非法[%s],必须为数字",accNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}
	
	if ((ret = UnionReadRequestXMLPackageValue("body/pinByPK", pinByPK, sizeof(pinByPK))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE202:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinByPK");
		return(ret);
	}
	else
	{
		//检测公钥加密的PIN
		pinByPK[ret] = 0;
		if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE202:: pinByPK can not be null!\n");
			UnionSetResponseRemark("公钥加密的PIN不能为空!");
			return(errCodeParameter);
		}
		if (!UnionIsBCDStr(pinByPK))
		{
			UnionUserErrLog("in UnionDealServiceCodeE202:: pinByPK[%s] is error!\n",pinByPK);
			UnionSetResponseRemark("公钥加密的PIN非法,必须为十六进制数");
	       	 	return(errCodeParameter);
		}
    	}

	if ((ret = UnionReadRequestXMLPackageValue("body/format", format, sizeof(format)))< 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE202:: UnionReadRequestXMLPackageValue[%s]!\n","body/format");
		return(ret);
	}
	format[ret] = 0;

	//读取专用算法标识
	if ((ret = UnionReadRequestXMLPackageValue("body/Specialalgorithm", Specialalgorithm, sizeof(Specialalgorithm))) < 0) 
	{
		Specialalgorithm[0] = 0;
	}
	Specialalgorithm[ret] = 0;

	// 读取对称密钥
	if ((ret =  UnionReadSymmetricKeyDBRec(zpkName, 1, &tZpk)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE202:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", zpkName);
		return(ret);
	}
	// 读取对称密钥值
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&tZpk,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE202:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("读取密钥值失败");
		return(errCodeParameter);
	}

	//读取非对称秘钥
	if ((ret = UnionReadAsymmetricKeyDBRec(pkName, 1, &tPk)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE202:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n", pkName);
		return(ret);
	}
	
	switch(tZpk.algorithmID)
	{
		case    conSymmetricAlgorithmIDOfDES:   // DES算法法
			memset(newPinBlock,0,sizeof(newPinBlock));
			memset(pinBlock,0,sizeof(pinBlock));
			lenOfVK = strlen(tPk.vkValue);
			if (lenOfVK > 0)
			{
				aschex_to_bcdhex(tPk.vkValue,lenOfVK,tmpBuf);
				tmpBuf[lenOfVK/2] = 0;
			}
			if ((ret= UnionHsmCmd70(lenOfVK/2,tmpBuf,atoi(tPk.vkIndex), 1,strlen(pinByPK), pinByPK, 1, psymmetricKeyValue->keyValue,Specialalgorithm, pinBlock))<0 )
			{
				UnionUserErrLog("in UnionDealServiceCodeE202: UnionHsmCmd70 return[%d]!\n", ret);
				//modify by lisq 20150313 当前版本密钥失败后尝试旧版本密钥
				if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && UnionOldVersionAsymmetricKeyIsUsed(&tPk))
				{

					lenOfVK = strlen(tPk.oldVKValue);
					if (lenOfVK <= 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE202:: oldVKValue length [%d]!\n", lenOfVK);
						return(ret);
					}

					aschex_to_bcdhex(tPk.oldVKValue,lenOfVK,tmpBuf);
					tmpBuf[lenOfVK/2] = 0;

					if ((ret= UnionHsmCmd70(lenOfVK/2,tmpBuf,atoi(tPk.vkIndex), 1,strlen(pinByPK), pinByPK, 1, psymmetricKeyValue->keyValue, Specialalgorithm,pinBlock))<0 )
					{
						UnionUserErrLog("in UnionDealServiceCodeE202: UnionHsmCmd70 old version vk return[%d]!\n", ret);
						return(ret);
					}
				}
				else
				{
					UnionUserErrLog("in UnionDealServiceCodeE202: old version vk not used!\n");
					return(ret);
				}
				//modify by lisq 20150313 end
			}

			if ((memcmp(format,"01",2) == 0) || (memcmp(format,"04",2) == 0))
			{
				if ((ret = UnionHsmCmdCC(UnionConvertSymmetricKeyKeyLen(tZpk.keyLen),psymmetricKeyValue->keyValue,
						UnionConvertSymmetricKeyKeyLen(tZpk.keyLen),psymmetricKeyValue->keyValue,
						12,"03",pinBlock,accNo,strlen(accNo),format,newPinBlock,&pinLen)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE202: UnionHsmCmdCC return[%d]!\n", ret);
					return(ret);
				}
			}
			else
				strcpy(newPinBlock,pinBlock);

			break;
		case    conSymmetricAlgorithmIDOfSM4:   // SM4算法
		default:
			UnionUserErrLog("in UnionDealServiceCodeE202:: key [%s] algorithmID = [%d] illegal!\n", tZpk.keyName, tZpk.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	//设置 pinBlock
	if ((ret = UnionSetResponseXMLPackageValue("body/pinByZPK", newPinBlock)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE202: UnionSetResponseXMLPackageValue [body/pinByZPK]!\n");
		return(ret);
	}

	return 0;
}
