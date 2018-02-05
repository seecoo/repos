//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5Interface.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "asymmetricKeyDB.h"
#include "remoteKeyPlatform.h"

/***************************************
服务代码:	E126
服务名:		销毁非对称密钥
功能描述:	销毁非对称密钥
***************************************/
int UnionDealServiceCodeE126(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	char			sysID[16+1];
	char			appID[16+1];
	char			usingUnit[16+1];
	char			keyName[128+1];
	char			creator[40+1];
	int			isRemoteApplyKey = 0;
	int			isRemoteDistributeKey = 0;
	int			isRemoteKeyOperate = 0;

	TUnionAsymmetricKeyDB	asymmetricKeyDB;
	TUnionRemoteKeyPlatform	tkeyApplyPlatform;
	TUnionRemoteKeyPlatform	tkeyDistributePlatform;

	// 检测远程标识
	isRemoteKeyOperate = UnionIsRemoteKeyOperate();
	
	// 读取密钥名称
	memset(keyName,0,sizeof(keyName));
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE126:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(keyName);
	if (strlen(keyName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE126:: keyName can not be null!\n");
		UnionSetResponseRemark("密钥名称不能为空!");
		return(errCodeParameter);
	}

	memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));
	if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,0,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE126:: UnionReadAsymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}

	// modify by leipp 20151209
	// 读取并检查申请和分发平台
	if ((ret = UnionCheckRemoteKeyPlatform(asymmetricKeyDB.keyName, asymmetricKeyDB.keyApplyPlatform, asymmetricKeyDB.keyDistributePlatform, &tkeyApplyPlatform, &tkeyDistributePlatform, &isRemoteApplyKey, &isRemoteDistributeKey)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE126:: UnionCheckRemoteKeyPlatform!\n");
		return(ret);
	}
	// modify by leipp end

	// 远程申请
	if (isRemoteApplyKey)
	{
		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE126:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE126:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	// 远程分发
	if (isRemoteDistributeKey)
	{
		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE126:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}
		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE126:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}
	
	memset(sysID,0,sizeof(sysID));
	memset(appID,0,sizeof(appID));
	memset(usingUnit,0,sizeof(usingUnit));
	if (isRemoteKeyOperate)		// 远程密钥操作
	{
		if ((ret = UnionReadRemoteSysIDAndAppID(sysID,sizeof(sysID),appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE126:: UnionReadRemoteSysIDAndAppID!\n");
			return(ret);	
		}
	}
	else				// 本地密钥操作
	{
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE126:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);	
		}
		if ((ret = UnionReadRequestXMLPackageValue("head/appID",appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE126:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");
			return(ret);	
		}
	}
	// 设置创建者
	memset(creator,0,sizeof(creator));
	UnionPieceAsymmetricKeyCreator(sysID,appID,creator);

	if (asymmetricKeyDB.creatorType == conAsymmetricCreatorTypeOfApp)
	{
		if (strcasecmp(UnionGetIDOfCustomization(),"aliyun") == 0)	// 阿里云
		{
			if ((strcmp(asymmetricKeyDB.creator,creator) == 0))
				//(strcmp(asymmetricKeyDB.usingUnit,usingUnit) == 0))
			{
				if ((ret = UnionDropAsymmetricKeyDB(&asymmetricKeyDB)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE126:: UnionDropAsymmetricKeyDB[%s]!\n",keyName);
					return(ret);
				}
				return(0);
			}
			UnionSetResponseRemark("密钥[%s]非此单位[%s:%s]创建,不允许删除!",keyName,usingUnit,creator);
			UnionUserErrLog("in UnionDealServiceCodeE126:: 密钥[%s]非此单位[%s:%s]创建,不允许删除!",keyName,usingUnit,creator);
			return(errCodeEsscMDLKeyOperationNotPermitted);
		}
		else
		{
			if (strcmp(asymmetricKeyDB.creator,creator) == 0)
			{
				if ((ret = UnionDropAsymmetricKeyDB(&asymmetricKeyDB)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE126:: UnionDropAsymmetricKeyDB[%s]!\n",keyName);
					return(ret);
				}
				return(0);
			}
			UnionSetResponseRemark("密钥[%s]非此创建者[%s]创建,不允许删除!",keyName,creator);
			UnionUserErrLog("in UnionDealServiceCodeE126:: 密钥[%s]非此创建者[%s]创建,不允许删除!",keyName,creator);
			return(errCodeEsscMDLKeyOperationNotPermitted);
		}
	}
	UnionSetResponseRemark("密钥[%s]非应用系统创建,不允许删除!",keyName);
	UnionUserErrLog("in UnionDealServiceCodeE126:: 密钥[%s]非应用系统创建,不允许删除!",keyName);
	return(errCodeEsscMDLKeyOperationNotPermitted);
}


