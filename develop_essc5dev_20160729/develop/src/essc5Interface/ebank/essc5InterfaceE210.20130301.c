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

int UnionDealServiceCodeE210(PUnionHsmGroupRec phsmGroupRec)
{
	int 	ret;
	char	keyName[136];
	char	pinByPK[512];
	char	zpkName[136];
	char	mode[16];
	char	format[8];
	char	accNo[40];	
	char	pinByZPK[136];
	int	lenOfPinByPK = 0;
	int 	algorithmID = 0;
	int	lenOfAccNo = 0;
	char	vkIndex[16];
	
	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue 	psymmetricKeyValue;
	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	
	//读取SM2密钥名称
	if((ret = UnionReadRequestXMLPackageValue("body/keyName", keyName, sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE210:: UnionReadRequestXMLPackageValue[%s]\n", "body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);
	
	//读取非对称密钥信息
	memset(&asymmetricKeyDB, 0, sizeof(asymmetricKeyDB));
	if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,1,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE210:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",keyName);
		return(ret);
	}
	
	// 检测私钥类型
        if (asymmetricKeyDB.keyType != conAsymmetricKeyTypeOfEncryption && asymmetricKeyDB.keyType != conAsymmetricKeyTypeOfSignatureAndEncryption)
        {
                UnionUserErrLog("in UnionDealServiceCodeE210:: keyType[%d] is not 1 or 2!\n",asymmetricKeyDB.keyType);
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
		UnionUserErrLog("in UnionDealServiceCodeE210:: UnionReadRequestXMLPackageValue[%s]\n","body/pinByPK");
		return(lenOfPinByPK);
	}
	pinByPK[lenOfPinByPK] = 0;
	
	//读取zpk密钥名称
	if((ret = UnionReadRequestXMLPackageValue("body/zpkName", zpkName, sizeof(zpkName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE210:: UnionReadRequestXMLPackageValue[%s]\n", "body/zpkName");
		return(ret);
	}
	zpkName[ret] = 0;
	UnionFilterHeadAndTailBlank(zpkName);

	//读取对称密钥信息
	memset(&symmetricKeyDB, 0, sizeof(symmetricKeyDB));
	if((ret = UnionReadSymmetricKeyDBRec(zpkName, 1, &symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE210:: UnionReadAsymmetricKeyDBRec zpkName[%s]\n",zpkName);
		return(ret);
	}
	
	if(conZPK != symmetricKeyDB.keyType)
	{	
		UnionUserErrLog("in UnionDealServiceCodeE210:: keyType err\n");
		UnionSetResponseRemark("密钥类型错,不为zpk密钥");
		return(errCodeHsmCmdMDL_InvalidKeyType);
	}
	
	if(conSymmetricAlgorithmIDOfSM4 == symmetricKeyDB.algorithmID) // 国密
		algorithmID = 1;
	else if(conSymmetricAlgorithmIDOfDES == symmetricKeyDB.algorithmID) // 国际
		algorithmID = 2;
	else
	{
		UnionUserErrLog("in UnionDealServiceCodeE210L:: algorithmID[%d]\n",symmetricKeyDB.algorithmID);
		UnionSetResponseRemark("zpk密钥算法标识错[必须是DES或SM4密钥]");
		return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	
	//读取对称密钥值
	if((psymmetricKeyValue= UnionGetSymmetricKeyValue(&symmetricKeyDB, phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE210:: UnionGetSymmetricKeyValue err\n");
		UnionSetResponseRemark("读取对称密钥值失败");
		return(errCodeParameter);
	}
	
	//读取模式
	if((ret = UnionReadRequestXMLPackageValue("body/mode", mode, sizeof(mode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE210:: UnionReadRequestXMLPackageValue[%s]\n", "body/mode");
		return(ret);
	}
	mode[ret] = 0;
	if (mode[0] != '1' && mode[0] != '2')
	{
		UnionUserErrLog("in UnionDealServiceCodeE210:: mode[%s] not in [1,2]\n",mode);
		return(errCodeParameter);
	}
	
	if('1' == mode[0])
	{
		//读取PIN格式
		if((ret = UnionReadRequestXMLPackageValue("body/format",format,sizeof(format))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE210:: UnionReadRequestXMLPackageValue[%s]\n", "body/format");
			return(ret);
		}
		format[ret] = 0;
	
		//读取账号
		if((lenOfAccNo = UnionReadRequestXMLPackageValue("body/accNo", accNo, sizeof(accNo))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE210:: UnionReadRequestXMLPackageValue[%s]\n", "body/accNo");		
			return(lenOfAccNo);
		}
		accNo[lenOfAccNo] = 0;
	}
	
	switch(asymmetricKeyDB.algorithmID)
	{
		case	conAsymmetricAlgorithmIDOfSM2:
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					if('1' == mode[0]) // pin转加密
					{
						if((ret = unionHsmCmdKK(vkIndex, strlen(asymmetricKeyDB.vkValue), asymmetricKeyDB.vkValue, lenOfPinByPK, pinByPK, algorithmID, 1, format,lenOfAccNo, accNo, psymmetricKeyValue->keyValue, NULL, pinByZPK,sizeof(pinByZPK))) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE210:: UnionHsmCmdKK \n");
							return(ret);
						}
						
					}
					else if('2' == mode[0])	// 渠道密码转加密
					{
						if((ret = UnionHsmCmdKL(vkIndex, strlen(asymmetricKeyDB.vkValue), asymmetricKeyDB.vkValue, lenOfPinByPK, pinByPK, algorithmID, 1, psymmetricKeyValue->keyValue, 0, 0, NULL, pinByZPK,sizeof(pinByZPK), NULL, 0)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE210:: UnionHsmCmdKL \n");
							return(ret);
						}
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE210:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}	
			break;
		case	conAsymmetricAlgorithmIDOfRSA:
		default:
			UnionUserErrLog("in UnionDealServiceCodeE210:: keyName [%s] algorithmID = [%d] illegal!\n", asymmetricKeyDB.keyName, asymmetricKeyDB.algorithmID);
                        return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	
	if((ret = UnionSetResponseXMLPackageValue("body/pinByZPK", pinByZPK)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE210:: UnionSetResponseXMLPackageValue[%s]\n", "body/pinByZPK");
		return(ret);
	}
	
	return(0);
}
