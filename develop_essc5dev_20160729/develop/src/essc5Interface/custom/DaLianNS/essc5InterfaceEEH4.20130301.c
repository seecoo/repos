
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

服务代码:	EEH4
服务名:  	EEH4 将私钥解密后的登录口令转换为网银的EDK加密
功能描述:	EEH4 将私钥解密后的登录口令转换为网银的EDK加密

***************************************/
int UnionDealServiceCodeEEH4(PUnionHsmGroupRec phsmGroupRec)
{
	int             ret = 0;
	int		lenOfVK = 0;
	int		lenOfPinByPK = 0;
	char		mode[2];
        char            pinByPK[1032];
	char		vkValue[2568];
        char  		oriPinOffset[64];
        char  		newPinOffset[64];
	char		keyName[136];
	char		edkName[136];	     //edk名称
        int 		vkIndex = 0;        //私钥索引
	char		fillMode[2];

	TUnionSymmetricKeyDB		edk;
	PUnionSymmetricKeyValue         pedkValue = NULL;
	TUnionAsymmetricKeyDB           asymmetricKeyDB;

	// 读取填充模式
	if ((ret = UnionReadRequestXMLPackageValue("body/fillMode", fillMode, sizeof(fillMode))) <= 0)
		strcpy(fillMode,"1");
	else
		fillMode[ret] = 0;

	if (fillMode[0] != '0' && fillMode[0] != '1')
	{
                UnionUserErrLog("in UnionDealServiceCodeEEH4:: fillMode[%s] not in [0,1]!\n",fillMode);
                return(errCodeParameter);
	}

	//读取mode
	if ((ret = UnionReadRequestXMLPackageValue("body/mode", mode, sizeof(mode))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH4:: UnionReadRequestXMLPackageValue[%s]!\n","body/mode");
                return(ret);
        }
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH4:: mode is null!\n");
		return(errCodeParameter);
	}
	mode[ret] = 0;
	if(mode[0] == '0')
	{
		//读取私钥名称
  		memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));
		if ((ret = UnionReadRequestXMLPackageValue("body/rsaName", keyName, sizeof(keyName))) < 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeEEH4:: UnionReadRequestXMLPackageValue[%s]!\n","body/rsaName");
                	return(ret);
        	}
        	else if (ret == 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeEEH4:: rsaName is null!\n");
                	return(errCodeParameter);
        	}
		keyName[ret] = 0;

        	// 读取密钥密钥信息     
        	if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,1,&asymmetricKeyDB)) < 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeEEH4:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",keyName);
                	return(ret);
        	}

		// 检测是否存在索引
        	if (asymmetricKeyDB.vkStoreLocation != 0)
			vkIndex = atoi(asymmetricKeyDB.vkIndex);
		else
		{
			vkIndex = 99;	
			lenOfVK = snprintf(vkValue,sizeof(vkValue),"%s",asymmetricKeyDB.vkValue);
		}
	}
	else if (mode[0] == '1')
	{
                //vkValue
                if ((lenOfVK = UnionReadRequestXMLPackageValue("body/vkValue",vkValue, sizeof(vkValue))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeEEH4:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkValue");
                        return(lenOfVK);
                }
		vkValue[lenOfVK] = 0;
		vkIndex = 99;
	}
	else	
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH4:: mode[%s] not in [0,1]!\n",mode);
		return(errCodeParameter);
	}
	
        // 公钥加密的数据
	if ((lenOfPinByPK = UnionReadRequestXMLPackageValue("body/pinByPK",pinByPK, sizeof(pinByPK))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH4:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinByPK");
                return(lenOfPinByPK);
        }
        else if (lenOfPinByPK == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH4:: pinByPK is null!\n");
                return(errCodeParameter);
        }
	pinByPK[lenOfPinByPK] = 0;

	if ((ret = UnionReadRequestXMLPackageValue("body/edkName",edkName, sizeof(edkName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH4:: UnionReadRequestXMLPackageValue[%s]!\n","body/edkName");
                return(ret);
        }
	else if (ret == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH4:: edkName is null!\n");
                return(errCodeParameter);
        }
	edkName[ret] = 0;

	// 读取pinOffset
	if ((ret = UnionReadRequestXMLPackageValue("body/pinOffset",oriPinOffset, sizeof(oriPinOffset))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH4:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinOffset");
                return(ret);
        }
	else if (ret == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH4:: pinOffset is null!\n");
                return(errCodeParameter);
        }
	oriPinOffset[ret] = 0;

	// 读取密钥
        memset(&edk,0,sizeof(edk));
        if ((ret = UnionReadSymmetricKeyDBRec(edkName,1,&edk)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH4:: UnionReadSymmetricKeyDBRec[%s]!\n",edkName);
                return(ret);
        }
	// 读取对称密钥值
        if ((pedkValue = UnionGetSymmetricKeyValue(&edk,phsmGroupRec->lmkProtectMode)) == NULL)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH4:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
                UnionSetResponseRemark("读取密钥值失败");
                return(errCodeParameter);
        }

	// 调密码机指令解密PIN
	switch(edk.algorithmID)
        {
                case    conSymmetricAlgorithmIDOfDES:   // DES算法
                        if ((ret = UnionHsmCmd4E(atoi(fillMode), vkIndex,lenOfPinByPK, pinByPK, lenOfVK,vkValue,pedkValue->keyValue, newPinOffset)) < 0)
                        {
                                UnionUserErrLog("in UnionDealServiceCodeEEH4: UnionHsmCmd4E ret[%d]!\n", ret);
                                return(ret);
                        }
			newPinOffset[ret] = 0;
                        break;
                case    conSymmetricAlgorithmIDOfSM4:   // SM4算法
                default:
                        UnionUserErrLog("in UnionDealServiceCodeEEH4:: key [%s] algorithmID = [%d] illegal!\n", edk.keyName, edk.algorithmID);
                        return(errCodeEsscMDL_InvalidAlgorithmID);
        }
	
	//读取pinOffset
	if (strcmp(oriPinOffset,newPinOffset) != 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH4:: inputPinOffset[%s] != localPinOffset[%s]!\n",oriPinOffset,newPinOffset);
		UnionSetResponseRemark("两个pinOffset不相同");
		return(errCodeParameter);
	}

	return 0;
}
