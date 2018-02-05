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

int UnionDealServiceCodeE702(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	char				termType[64];
	char				termID[64];
	char				keyPlatformID[64];
	char				protectKey[1024];
	char				keyName[160];
	char				keyValue[64];
	char				checkValue[32];
	char				sysID[32];
	char				appID[32];

	TUnionRemoteKeyPlatform         tkeyDistributePlatform;

	
	//读取终端类型
	if ((ret = UnionReadRequestXMLPackageValue("body/termType",termType,sizeof(termType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE702:: UnionReadRequestXMLPackageValue[%s]!\n","body/termType");
		return(ret);
	}
	termType[ret] = 0;

	//读取终端号
	if ((ret = UnionReadRequestXMLPackageValue("body/termID",termID,sizeof(termID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE702:: UnionReadRequestXMLPackageValue[%s]!\n","body/termID");
		return(ret);
	}
	termID[ret] = 0;

        // 读取密钥类型
	if ((ret = UnionReadRequestXMLPackageValue("body/protectKey",protectKey,sizeof(protectKey))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE702:: UnionReadRequestXMLPackageValue[%s]!\n","body/protectKey");
		return(ret);
	}
	protectKey[ret] = 0;

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

	//读取系统ID
        if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE702:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
                return(ret);
        }

	//读取应用ID
        if ((ret = UnionReadRequestXMLPackageValue("head/appID",appID,sizeof(appID))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE702:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");
                return(ret);
        }

	// 获取分发平台信息
        if ((ret =  UnionReadRemoteKeyPlatformRec(keyPlatformID,conRemoteKeyPlatformKeyDirectionOfApply,&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE702:: UnionReadRemoteKeyPlatformRec keyDistributePlatform[%s]!\n",keyPlatformID);
                return(ret);
        }
	
	// 初始化远程密钥操作请求报文
        if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE702:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
                return(ret);
        }
	
        // 修改对应服务码 
        if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","T002")) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE702:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","T001");
                return(ret);
        }
	if ((ret = UnionSetRequestRemoteXMLPackageValue("body/termType",termType)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE702:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/termType",termType);
                return(ret);
        }
	if ((ret = UnionSetRequestRemoteXMLPackageValue("body/termID",termID)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE702:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/termID",termID);
                return(ret);
        }
	if ((ret = UnionSetRequestRemoteXMLPackageValue("body/protectKey",protectKey)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE702:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/protectKey",protectKey);
                return(ret);
        }
	// 转发密钥操作到远程平台
        if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE702:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
                return(ret);
        }
	
	// 读取远程响应
      	if((ret = UnionReadResponseRemoteXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
       	{
               	UnionUserErrLog("in UnionDealServiceCodeE702:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/keyName");
               	return(ret);
       	}

      	if((ret = UnionReadResponseRemoteXMLPackageValue("body/keyValue",keyValue,sizeof(keyValue))) < 0)
       	{
               	UnionUserErrLog("in UnionDealServiceCodeE702:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/keyValue");
               	return(ret);
       	}

      	if((ret = UnionReadResponseRemoteXMLPackageValue("body/checkValue",checkValue,sizeof(checkValue))) < 0)
       	{
               	UnionUserErrLog("in UnionDealServiceCodeE702:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/checkValue");
               	return(ret);
       	}

	//返回远程响应	
		
	if ((ret = UnionSetResponseXMLPackageValue("body/keyName",keyName)) < 0)
       	{
        	UnionUserErrLog("in UnionDealServiceCodeE702:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyName");
              	return(ret);
       	}
	if ((ret = UnionSetResponseXMLPackageValue("body/keyValue",keyValue)) < 0)
       	{
            	UnionUserErrLog("in UnionDealServiceCodeE702:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyValue");
               	return(ret);
	}
	if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",checkValue)) < 0)
        {
            	UnionUserErrLog("in UnionDealServiceCodeE702:: UnionSetResponseXMLPackageValue[%s]!\n","body/checkValue");
               	return(ret);
	}
	return(ret);
}
