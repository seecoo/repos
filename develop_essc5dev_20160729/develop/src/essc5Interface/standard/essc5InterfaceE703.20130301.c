//	Author: fus	
//	Date:		2014/08/04
//	Version:	2.0

// Modification History

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "essc5UIService.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
//#include "commWithHsmSvr.h"
#include "unionHsmCmdVersion.h"

int UnionDealServiceCodeE703(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	char				termType[64];
	char				termID[64];
	char				oldKeyName[160];
	char				keyPlatformID[64];
	char				keyType[32];
	char				keyName[160];
	char				keyValue[64];
	char				checkValue[32];

	TUnionSymmetricKeyDB            symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;
	TUnionRemoteKeyPlatform         tkeyDistributePlatform;
	
	//读取终端类型
	if ((ret = UnionReadRequestXMLPackageValue("body/termType",termType,sizeof(termType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE703:: UnionReadRequestXMLPackageValue[%s]!\n","body/termType");
		return(ret);
	}
	termType[ret] = 0;

	//读取终端号
	if ((ret = UnionReadRequestXMLPackageValue("body/termID",termID,sizeof(termID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE703:: UnionReadRequestXMLPackageValue[%s]!\n","body/termID");
		return(ret);
	}
	termID[ret] = 0;

        // 读取密钥类型
	if ((ret = UnionReadRequestXMLPackageValue("body/keyType",keyType,sizeof(keyType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE703:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyType");
		return(ret);
	}
	//读取平台ID
        if ((ret = UnionReadRequestXMLPackageValue("body/keyPlatformID",keyPlatformID,sizeof(keyPlatformID))) < 0)
        {
		if ((ret = UnionReadRequestXMLPackageValue("head/appID",keyPlatformID,sizeof(keyPlatformID))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE702:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");
                        return(ret);
                }
        }
	keyPlatformID[ret] = 0;

	// 获取分发平台信息
        if ((ret =  UnionReadRemoteKeyPlatformRec(keyPlatformID,conRemoteKeyPlatformKeyDirectionOfApply,&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE703:: UnionReadRemoteKeyPlatformRec keyDistributePlatform[%s]!\n",keyPlatformID);
                return(ret);
        }
	
	// 初始化远程密钥操作请求报文
        if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE703:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
                return(ret);
        }
	
        // 修改对应服务码 
        if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","T003")) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE703:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","T003");
                return(ret);
        }
	if ((ret = UnionSetRequestRemoteXMLPackageValue("body/termType",termType)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE703:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/termType",termType);
                return(ret);
        }
	if ((ret = UnionSetRequestRemoteXMLPackageValue("body/termID",termID)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE703:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/termID",termID);
                return(ret);
        }
	if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyType",keyType)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE703:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyType",keyType);
                return(ret);
        }
	// 转发密钥操作到远程平台
        if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE703:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
                return(ret);
        }
	// 读取远程响应
      	if((ret = UnionReadResponseRemoteXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE703:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/keyName");
                return(ret);
        }

      	if((ret = UnionReadResponseRemoteXMLPackageValue("body/keyValue",keyValue,sizeof(keyValue))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE703:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/keyValue");
                return(ret);
        }
/*
      	if((ret = UnionReadResponseRemoteXMLPackageValue("body/keyValue2",keyValue2,sizeof(keyValue2))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE703:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/keyValue2");
                return(ret);
        }
*/
      	if((ret = UnionReadResponseRemoteXMLPackageValue("body/checkValue",checkValue,sizeof(checkValue))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE703:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/checkValue");
                return(ret);
        }

      	if((ret = UnionReadResponseRemoteXMLPackageValue("body/ZMKName",oldKeyName,sizeof(oldKeyName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE703:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/oldKeyName");
                return(ret);
        }
	oldKeyName[ret] = 0;

	//返回远程响应	
	if ((ret = UnionSetResponseXMLPackageValue("body/keyName",keyName)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE703:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyName");
                return(ret);
        }
	if ((ret = UnionSetResponseXMLPackageValue("body/keyValue",keyValue)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE703:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyValue");
                return(ret);
	}
/*
	if ((ret = UnionSetResponseXMLPackageValue("body/keyValue2",keyValue2)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE703:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyValue2");
                return(ret);
	}
*/
	if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",checkValue)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE703:: UnionSetResponseXMLPackageValue[%s]!\n","body/checkValue");
                return(ret);
	}
	if ((ret = UnionReadSymmetricKeyDBRec(oldKeyName,0,&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE703::UnionReadSymmetricKeyDBRec[%s]!\n",oldKeyName);
		return(ret);
	}

	// modify by leipp 20151113
	// 读取密钥值
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) != NULL)
	{
		// 当前密钥置为旧密钥
		strcpy(psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue);
		strcpy(symmetricKeyDB.oldCheckValue,symmetricKeyDB.checkValue);
	}
	else
		psymmetricKeyValue = &symmetricKeyDB.keyValue[0];

	//strcpy(symmetricKeyDB.keyValue[0].oldKeyValue,symmetricKeyDB.keyValue[0].keyValue);
	switch(symmetricKeyDB.algorithmID)
	{
		case	conSymmetricAlgorithmIDOfDES:
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,keyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue))< 0)	
					{
						UnionUserErrLog("in UnionDealServiceCodeE703::UnionHsmCmdA6[%d]!\n",ret);
						return(ret);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE703:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}
			break;
		case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,keyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE703:: UnionHsmCmdSV!\n");
						return(ret);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE703:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE703:: Distribute Key algorithmID[%d] is error!\n",symmetricKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	if(memcmp(checkValue,symmetricKeyDB.checkValue,strlen(checkValue)) != 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE703::UnionHsmCmdA6::[%s] checkVallue not match!\n",keyName);
		return(errCodeEssc_CheckValue);
	}
	// modify end by leipp 20151113

	//更新插入平台密钥
	if ((ret = UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB))<0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE703:: UnionUpdateSymmetricKeyDBKeyValue[%s]!\n",keyName);
		return(ret);
	}
	return 0;
}
