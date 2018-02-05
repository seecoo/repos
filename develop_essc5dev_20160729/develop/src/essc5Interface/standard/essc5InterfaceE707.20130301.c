//	Author: chenqy	
//	Date:		2015/08/18
//	Version:	1.0

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
#include "unionHsmCmdVersion.h"

int UnionDealServiceCodeE707(PUnionHsmGroupRec phsmGroupRec)
{
	int		ret;
	char	termType[64];
	char	factoryId[64];
	char	deviceId[64];
	char	organization[64];
	char	keyPlatformID[64];
	
	TUnionRemoteKeyPlatform		tkeyDistributePlatform;
	
	// 报文中读取终端类型
	if ((ret = UnionReadRequestXMLPackageValue("body/termType", termType, sizeof(termType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE707:: UnionReadRequestXMLPackageValue[%s]!\n","body/termType");
		return(ret);
	}
	termType[ret] = 0;
	
	// 报文中读取厂商编号
	if ((ret = UnionReadRequestXMLPackageValue("body/factoryId", factoryId, sizeof(factoryId))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE707:: UnionReadRequestXMLPackageValue[%s]!\n","body/factoryId");
		return(ret);
	}
	factoryId[ret] = 0;
	
	// 报文中读取设备编号
	if ((ret = UnionReadRequestXMLPackageValue("body/deviceId", deviceId, sizeof(deviceId))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE707:: UnionReadRequestXMLPackageValue[%s]!\n","body/deviceId");
		return(ret);
	}
	deviceId[ret] = 0;
	
	// 报文中读取机构号
	if ((ret = UnionReadRequestXMLPackageValue("body/organization", organization, sizeof(organization))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE707:: UnionReadRequestXMLPackageValue[%s]!\n","body/organization");
		return(ret);
	}
	organization[ret] = 0;
	
	// 报文中读取TKMS系统ID
	if ((ret = UnionReadRequestXMLPackageValue("body/keyPlatformID", keyPlatformID, sizeof(keyPlatformID))) < 0)
	{
		if ((ret = UnionReadRequestXMLPackageValue("head/appID", keyPlatformID, sizeof(keyPlatformID))) < 0)
		{
		    UnionUserErrLog("in UnionDealServiceCodeE707:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");
		    return(ret);
		}
	}
	keyPlatformID[ret] = 0;
	
	// 获取远程密钥平台信息
	if ((ret =  UnionReadRemoteKeyPlatformRec(keyPlatformID, conRemoteKeyPlatformKeyDirectionOfApply, &tkeyDistributePlatform)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE707:: UnionReadRemoteKeyPlatformRec keyDistributePlatform[%s]!\n",keyPlatformID);
		return(ret);
	}
	
	// 初始化远程请求报文
	if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE707:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
		return(ret);
	}
	
	// 修改远程请求报文
	if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","T007")) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE707:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","T007");
		return(ret);
	}
	if ((ret = UnionSetRequestRemoteXMLPackageValue("body/termType",termType)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE707:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/termType",termType);
		return(ret);
	}
	if ((ret = UnionSetRequestRemoteXMLPackageValue("body/factoryId",factoryId)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE707:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/factoryId",factoryId);
		return(ret);
	}
	if ((ret = UnionSetRequestRemoteXMLPackageValue("body/deviceId",deviceId)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE707:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/deviceId",deviceId);
		return(ret);
	}
	if ((ret = UnionSetRequestRemoteXMLPackageValue("body/organization",organization)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE707:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/organization",organization);
		return(ret);
	}
	
	// 转发报文到远程密钥平台
	if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE707:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
		return(ret);
	}
	
	return(ret);
	
}


