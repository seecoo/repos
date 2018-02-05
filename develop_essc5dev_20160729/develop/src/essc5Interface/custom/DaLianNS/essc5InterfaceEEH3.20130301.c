
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

/***************************************

服务代码:	EEH3
服务名:  	EEH3 将由公钥加密的PIN转换成ANSI9.8标准 
功能描述:	EEH3 将由公钥加密的PIN转换成ANSI9.8标准 

***************************************/
int UnionDealServiceCodeEEH3(PUnionHsmGroupRec phsmGroupRec)
{

	int             ret;
	int		lenOfPinByPK = 0;
	char		mode[1+1];
	char		keyType[1+1];
	char		pinType[1+1];
	char		fillMode[1+1];
	char      	pinByPK[1024+1];
	char      	pinByKey[16+1];
	char      	accNo[32+1];
	char		keyName[128+1];	     //edk名称
        int 		vkIndex = 0;        //私钥索引

	TUnionSymmetricKeyDB		symmetricKeyDB;
	TUnionAsymmetricKeyDB           asymmetricKeyDB;
	PUnionSymmetricKeyValue         pKeyValue = NULL;

	memset(mode,0,sizeof(mode));
	
	// 读取填充模式
	if ((ret = UnionReadRequestXMLPackageValue("body/fillMode", fillMode, sizeof(fillMode))) <= 0)
	{
		//UnionUserErrLog("in UnionDealServiceCodeEEH3:: UnionReadRequestXMLPackageValue[%s]!\n","body/fillMode");
		//return(ret);
		strcpy(fillMode,"1");
	}
	fillMode[ret] = 0;
	if (fillMode[0] != '0' && fillMode[0] != '1')
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH3:: fillMode[%s] not in[0,1]!\n",fillMode);
		return(errCodeParameter);
	}

	//读取type
	memset(keyType,0,sizeof(keyType));
	if ((ret = UnionReadRequestXMLPackageValue("body/keyType", keyType, sizeof(keyType))) <= 0)
        {
                //UnionUserErrLog("in UnionDealServiceCodeEEH3:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyType");
                //return(ret);
		strcpy(keyType,"2");
        }
	if (keyType[0] != '1' && keyType[0] != '2')
	{
                UnionUserErrLog("in UnionDealServiceCodeEEH3:: keyType[%s] not in[1,2]!\n",keyType);
                return(errCodeParameter);
	}

	//读取私钥名称
  	memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));
	if ((ret = UnionReadRequestXMLPackageValue("body/rsaName", asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH3:: UnionReadRequestXMLPackageValue[%s]!\n","body/rsaName");
                return(ret);
        }
        else if (ret == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH3:: rsaName is null!\n");
                return(errCodeParameter);
        }

        // 读取密钥密钥信息     
        if ((ret =  UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName,1,&asymmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH3:: UnionReadAsymmetricKeyDBRec rsaName[%s]!\n", asymmetricKeyDB.keyName);
                return(ret);
        }
	// 检测是否存在索引
        if (asymmetricKeyDB.vkStoreLocation != 0)
		vkIndex = atoi(asymmetricKeyDB.vkIndex);
	else
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH3:: vkStoreLocation[%d] is not vkIndex!\n",asymmetricKeyDB.vkStoreLocation);
		UnionSetResponseRemark("密钥索引不存在");
		return(errCodeParameter);
	}

	//读取密钥名称
	memset(keyName,0,sizeof(keyName));
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName, sizeof(keyName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH3:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
                return(ret);
        }
	else if (ret == 0)
	{
                UnionUserErrLog("in UnionDealServiceCodeEEH3:: keyName is null!\n");
                return(errCodeParameter);
	}

        // 读keyvalue
	memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));
        if ((ret = UnionReadSymmetricKeyDBRec(keyName,1,&symmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH3:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
                return(ret);
        }
	
	 // 读取对称密钥值
        if ((pKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH3:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
                UnionSetResponseRemark("读取密钥值失败");
                return(errCodeParameter);
        }
	// pintype
        if ((ret = UnionReadRequestXMLPackageValue("body/pinType",pinType, sizeof(pinType))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH3:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinType");
                return(ret);
        }
	pinType[ret] = 0;	
	if (pinType[0] != '0' && pinType[0] != '1')
	{
                UnionUserErrLog("in UnionDealServiceCodeEEH3:: pinType[%s] not in[0,1]!\n",pinType);
                return(errCodeParameter);
	}

        //accNo
	if ((ret = UnionReadRequestXMLPackageValue("body/accNo",accNo, sizeof(accNo))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH3:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
                return(ret);
        }
	accNo[ret] = 0;

        // 公钥加密的pin
	if ((lenOfPinByPK = UnionReadRequestXMLPackageValue("body/pinByPK",pinByPK, sizeof(pinByPK))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH3:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinByPK");
                return(lenOfPinByPK);
        }
	pinByPK[lenOfPinByPK] = 0;
	if (lenOfPinByPK == 0)
	{
                UnionUserErrLog("in UnionDealServiceCodeEEH3:: pinByPK is null!\n");
                return(errCodeParameter);
	}

	// 调密码机指令解密PIN
	memset(pinByKey,0,sizeof(pinByKey));
	switch(symmetricKeyDB.algorithmID)
        {
                case    conSymmetricAlgorithmIDOfDES:   // DES算法
                        if ((ret = UnionHsmCmd4C(vkIndex,atoi(keyType),pKeyValue->keyValue,atoi(pinType),atoi(fillMode),accNo, lenOfPinByPK,pinByPK,pinByKey)) < 0)
                        {
                                UnionUserErrLog("in UnionDealServiceCodeEEH3: UnionHsmCmd4C ret[%d]!\n", ret);
                                return(ret);
                        }
                        break;
                case    conSymmetricAlgorithmIDOfSM4:   // SM4算法
                default:
                        UnionUserErrLog("in UnionDealServiceCodeEEH3:: key [%s] algorithmID = [%d] illegal!\n", symmetricKeyDB.keyName, symmetricKeyDB.algorithmID);
                        return(errCodeEsscMDL_InvalidAlgorithmID);
        }

	
	//返回pinPK
	if ((ret = UnionSetResponseXMLPackageValue("body/pinBlock", pinByKey)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH3: UnionSetResponseXMLPackageValue [body/pinBlock]!\n");
		return(ret);
	}

	return 0;
}
