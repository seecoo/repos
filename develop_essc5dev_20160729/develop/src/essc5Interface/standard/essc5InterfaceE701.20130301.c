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

int UnionDealServiceCodeE701(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	char				termType[64];
	char				termID[64];
	char				authCode[64];
	char				remotePKValue[1024];
	char				keyPlatformID[64];

	TUnionRemoteKeyPlatform         tkeyDistributePlatform;

	
	//读取终端类型
	if ((ret = UnionReadRequestXMLPackageValue("body/termType",termType,sizeof(termType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE701:: UnionReadRequestXMLPackageValue[%s]!\n","body/termType");
		return(ret);
	}
	termType[ret] = 0;

	//读取终端号
	if ((ret = UnionReadRequestXMLPackageValue("body/termID",termID,sizeof(termID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE701:: UnionReadRequestXMLPackageValue[%s]!\n","body/termID");
		return(ret);
	}
	termID[ret] = 0;

        // 读取认证码
	if ((ret = UnionReadRequestXMLPackageValue("body/authCode",authCode,sizeof(authCode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE701:: UnionReadRequestXMLPackageValue[%s]!\n","body/authCode");
		return(ret);
	}
	authCode[ret] = 0;

	//读取平台ID
        if ((ret = UnionReadRequestXMLPackageValue("body/keyPlatformID",keyPlatformID,sizeof(keyPlatformID))) < 0)
        {
		if ((ret = UnionReadRequestXMLPackageValue("head/appID",keyPlatformID,sizeof(keyPlatformID))) < 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeE701:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");
                	return(ret);
        	}
        }
	keyPlatformID[ret] = 0;

	// 获取分发平台信息
        if ((ret =  UnionReadRemoteKeyPlatformRec(keyPlatformID,conRemoteKeyPlatformKeyDirectionOfApply,&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE701:: UnionReadRemoteKeyPlatformRec keyDistributePlatform[%s]!\n",keyPlatformID);
                return(ret);
        }
	
	// 初始化远程密钥操作请求报文
        if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE701:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
                return(ret);
        }
	
        // 修改对应服务码 
        if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","T001")) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE701:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","T001");
                return(ret);
        }
	if ((ret = UnionSetRequestRemoteXMLPackageValue("body/termType",termType)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE701:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/termType",termType);
                return(ret);
        }
	if ((ret = UnionSetRequestRemoteXMLPackageValue("body/termID",termID)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE701:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/termID",termID);
                return(ret);
        }
	if ((ret = UnionSetRequestRemoteXMLPackageValue("body/authCode",authCode)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE701:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/authCode",authCode);
                return(ret);
        }
	// 转发密钥操作到远程平台
        if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE701:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
                return(ret);
        }
	// 读取远程密钥值
      	if((ret = UnionReadResponseRemoteXMLPackageValue("body/pk",remotePKValue,sizeof(remotePKValue))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE701:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/remotePKValue");
                return(ret);
        }

	//返回远程密钥值	
	if ((ret = UnionSetResponseXMLPackageValue("body/pk",remotePKValue)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE701:: UnionSetResponseXMLPackageValue[%s]!\n","body/remotePKValue");
                return(ret);
        }
	return(ret);
}
