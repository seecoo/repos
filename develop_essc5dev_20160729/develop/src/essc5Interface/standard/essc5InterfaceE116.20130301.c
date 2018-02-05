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
#include "symmetricKeyDB.h"
#include "remoteKeyPlatform.h"

/***************************************
服务代码:	E116
服务名:		销毁对称密钥
功能描述:	销毁对称密钥
***************************************/
int UnionDealServiceCodeE116(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	int			isRemoteApplyKey = 0;
	int			isRemoteDistributeKey = 0;
	int			isRemoteKeyOperate = 0;
	char			sysID[32];
	char			appID[32];
	char			keyName[160];
	char			creator[64];

	TUnionSymmetricKeyDB	symmetricKeyDB;
	TUnionRemoteKeyPlatform	tkeyApplyPlatform;
	TUnionRemoteKeyPlatform	tkeyDistributePlatform;
	
	isRemoteKeyOperate = UnionIsRemoteKeyOperate();
	memset(&symmetricKeyDB, 0, sizeof(symmetricKeyDB));

	// 读取密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE116:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);
	if (strlen(keyName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE116:: keyName can not be null!\n");
		UnionSetResponseRemark("密钥名称不能为空!");
		return(errCodeParameter);
	}

	UnionInitSymmetricKeyDB(&symmetricKeyDB);
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE116:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}
	
	// modify by leipp 20151209
	// 读取并检查申请和分发平台
	if ((ret = UnionCheckRemoteKeyPlatform(symmetricKeyDB.keyName, symmetricKeyDB.keyApplyPlatform, symmetricKeyDB.keyDistributePlatform, &tkeyApplyPlatform, &tkeyDistributePlatform, &isRemoteApplyKey, &isRemoteDistributeKey)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE116:: UnionCheckRemoteKeyPlatform!\n");
		return(ret);
	}
	// modify by leipp end
	
	if (isRemoteApplyKey)		// 远程申请密钥
	{
		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE116:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}
		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE116:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	if (isRemoteDistributeKey)	// 远程分发密钥
	{
		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE116:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE116:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	if (isRemoteKeyOperate)		// 远程密钥操作
	{
		if ((ret = UnionReadRemoteSysIDAndAppID(sysID,sizeof(sysID),appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE116:: UnionReadRemoteSysIDAndAppID!\n");
			return(ret);	
		}
	}
	else				// 本地密钥操作
	{
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE116:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);	
		}
		if ((ret = UnionReadRequestXMLPackageValue("head/appID",appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE116:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");
			return(ret);	
		}
	}

	// 设置创建者
	UnionPieceSymmetricKeyCreator(sysID,appID,creator);

	if (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfApp)
	{
		if (strcasecmp(UnionGetIDOfCustomization(),"aliyun") == 0)	// 阿里云
		{
			if ((strcmp(symmetricKeyDB.creator,creator) == 0))
			{
				// 销毁当前密钥
				if ((ret = UnionDropSymmetricKeyDB(&symmetricKeyDB)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE116:: UnionDropSymmetricKeyDB[%s]!\n",keyName);
					return(ret);
				}
				return(0);
			}
			UnionSetResponseRemark("密钥[%s]非此单位[%s]创建,不允许删除!",keyName,creator);
			UnionUserErrLog("in UnionDealServiceCodeE116:: 密钥[%s]非此单位[%s]创建,不允许删除!!\n",keyName,creator);
			return(errCodeEsscMDLKeyOperationNotPermitted);
		}
		else
		{
			// APP为3.x或4.x密钥移植到5.x的创建者
			if ((strcmp(symmetricKeyDB.creator,creator) == 0) || 
				strcmp(symmetricKeyDB.creator,"APP") == 0) 
			{
				// 销毁当前密钥
				if ((ret = UnionDropSymmetricKeyDB(&symmetricKeyDB)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE116:: UnionDropSymmetricKeyDB[%s]!\n",keyName);
					return(ret);
				}
				return(0);
			}
			UnionSetResponseRemark("密钥[%s]非此创建者[%s]创建,不允许删除!",keyName,creator);
			UnionUserErrLog("in UnionDealServiceCodeE116:: 密钥[%s]非此创建者[%s]创建,不允许删除!",keyName,creator);
			return(errCodeEsscMDLKeyOperationNotPermitted);
		}
	}
	else if ((symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfMove) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfKMS))
	{
		if ((ret = UnionDropSymmetricKeyDB(&symmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE116:: UnionDropSymmetricKeyDB[%s]!\n",keyName);
			return(ret);
		}
		return 0;
	}

	UnionSetResponseRemark("密钥[%s]非应用系统创建,不允许删除!",keyName);
	UnionUserErrLog("in UnionDealServiceCodeE116:: 密钥[%s]非应用系统创建,不允许删除!\n",keyName);
	return(errCodeEsscMDLKeyOperationNotPermitted);
}
