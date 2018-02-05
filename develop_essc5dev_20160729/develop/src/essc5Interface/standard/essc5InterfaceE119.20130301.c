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
服务代码:	E119
服务名:		启用/挂起对称密钥
功能描述:	启用/挂起对称密钥
***************************************/
int UnionDealServiceCodeE119(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	int			isRemoteApplyKey = 0;
	int			isRemoteDistributeKey = 0;
	int			isRemoteKeyOperate = 0;
	char			sysID[32];
	char			appID[32];
	char			keyName[128];
	char			creator[64];
	int			mode = 0;
	char			tmpNum[32];

	TUnionSymmetricKeyDB	symmetricKeyDB;
	TUnionRemoteKeyPlatform	tkeyApplyPlatform;
	TUnionRemoteKeyPlatform	tkeyDistributePlatform;
	
	isRemoteKeyOperate = UnionIsRemoteKeyOperate();

	// 读取密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE119:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(keyName);
	if (strlen(keyName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE119:: keyName can not be null!\n");
		UnionSetResponseRemark("密钥名称不能为空!");
		return(errCodeParameter);
	}

	// 读取模式
	// 1：启用密钥
	// 2：挂起密钥
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpNum,sizeof(tmpNum))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE119:: UnionReadRequestXMLPackageValue[%s]!\n","body/mode");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(tmpNum);
	mode = atoi(tmpNum);

	if (mode != 1 && mode != 2)
	{
		UnionUserErrLog("in UnionDealServiceCodeE119:: mode[%d] != 1 or 2!\n",mode);
		UnionSetResponseRemark("模式[%d]必须为1或2",mode);
		return(errCodeParameter);
	}

	// 获取密钥信息
	UnionInitSymmetricKeyDB(&symmetricKeyDB);
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE119:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}

	// 只有挂起状态才能被启用
	if (conSymmetricKeyStatusOfSuspend == symmetricKeyDB.status && mode == 1)
	{
		if (strlen(symmetricKeyDB.checkValue) > 0)
			symmetricKeyDB.status = conSymmetricKeyStatusOfEnabled;
		else
			symmetricKeyDB.status = conSymmetricKeyStatusOfInitial;
	}
	if (mode == 2)
		symmetricKeyDB.status = conSymmetricKeyStatusOfSuspend;

	// modify by leipp 20151209
	// 读取并检查申请和分发平台
	if ((ret = UnionCheckRemoteKeyPlatform(symmetricKeyDB.keyName, symmetricKeyDB.keyApplyPlatform, symmetricKeyDB.keyDistributePlatform, &tkeyApplyPlatform, &tkeyDistributePlatform, &isRemoteApplyKey, &isRemoteDistributeKey)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE119:: UnionCheckRemoteKeyPlatform!\n");
		return(ret);
	}
	// modify by leipp end

	if (isRemoteApplyKey)		// 远程申请密钥
	{
		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE119:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}
		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE119:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	if (isRemoteDistributeKey)	// 远程分发密钥
	{
		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE119:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE119:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	if (isRemoteKeyOperate)		// 远程密钥操作
	{
		if ((ret = UnionReadRemoteSysIDAndAppID(sysID,sizeof(sysID),appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE119:: UnionReadRemoteSysIDAndAppID!\n");
			return(ret);	
		}
	}
	else				// 本地密钥操作
	{
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE119:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);	
		}
		if ((ret = UnionReadRequestXMLPackageValue("head/appID",appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE119:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");
			return(ret);	
		}
	}
	// 设置创建者
	UnionPieceSymmetricKeyCreator(sysID,appID,creator);

	if (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfApp)
	{
//		if ((strcmp(symmetricKeyDB.creator,creator) == 0) && 
//			(strcmp(symmetricKeyDB.usingUnit,usingUnit) == 0))
		if (strcmp(symmetricKeyDB.creator,creator) == 0) 
		{
			// 更新密钥状态
			if ((ret = UnionUpdateSymmetricKeyDBStatus(&symmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE119:: UnionUpdateSymmetricKeyDBStatus[%s]!\n",keyName);
				return(ret);
			}

			return(0);
		}
	}

	if ((symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfUser) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfMove) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfKMS))
	{
		if ((ret = UnionUpdateSymmetricKeyDBStatus(&symmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE119:: UnionUpdateSymmetricKeyDBStatus[%s]!\n",keyName);
			return(ret);
		}

                        return(0);
	}

	UnionSetResponseRemark("此密钥[%s]非本单位[%s]创建,不允许更新状态!",keyName,creator);
	return(errCodeEsscMDLKeyOperationNotPermitted);
}


