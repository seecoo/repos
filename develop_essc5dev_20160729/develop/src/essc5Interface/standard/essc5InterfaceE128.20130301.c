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
服务代码:	E128
服务名:		启用/挂起对称密钥
功能描述:	启用/挂起对称密钥
***************************************/
int UnionDealServiceCodeE128(PUnionHsmGroupRec phsmGroupRec)
{
/*	int			ret;
	int			isRemoteApplyKey = 0;
	int			isRemoteDistributeKey = 0;
	int			isRemoteKeyOperate = 0;
	char			sysID[16+1];
	char			appID[16+1];
	char			usingUnit[16+1];
	char			keyName[128+1];
	char			creator[40+1];
	int			mode = 0;
	char			tmpNum[1+1];
	TUnionSymmetricKeyDB	symmetricKeyDB;
	TUnionRemoteKeyPlatform	tkeyApplyPlatform;
	TUnionRemoteKeyPlatform	tkeyDistributePlatform;
	
	isRemoteKeyOperate = UnionIsRemoteKeyOperate();

	// 读取密钥名称
	memset(keyName,0,sizeof(keyName));
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE128:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(keyName);
	if (strlen(keyName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE128:: keyName can not be null!\n");
		UnionSetResponseRemark("密钥名称不能为空!");
		return(errCodeParameter);
	}

	// 读取模式
	// 1：启用密钥
	// 2：挂起密钥
	memset(tmpNum,0,sizeof(tmpNum));
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpNum,sizeof(tmpNum))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE128:: UnionReadRequestXMLPackageValue[%s]!\n","body/mode");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(tmpNum);
	mode = atoi(tmpNum);

	if (mode != 1 && mode != 2)
	{
		UnionUserErrLog("in UnionDealServiceCodeE128:: mode[%d] != 1 or 2!\n",mode);
		UnionSetResponseRemark("模式[%d]必须为1或2",mode);
		return(errCodeParameter);
	}

	// 获取密钥信息
	memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE128:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}

	// 只有挂起状态才能被启用
	if (conSymmetricKeyStatusOfSuspend == symmetricKeyDB.status && mode == 1)
		symmetricKeyDB.status = conSymmetricKeyStatusOfEnabled;
	if (mode == 2)
		symmetricKeyDB.status = conSymmetricKeyStatusOfSuspend;
	
	// 检查是否存在远程申请平台
	if (strlen(symmetricKeyDB.keyApplyPlatform) > 0)
	{
		memset(&tkeyApplyPlatform,0,sizeof(tkeyApplyPlatform));
		if ((ret =  UnionReadRemoteKeyPlatformRec(symmetricKeyDB.keyApplyPlatform,conRemoteKeyPlatformKeyDirectionOfApply,&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE128:: UnionReadRemoteKeyPlatformRec keyApplyPlatform[%s]!\n",symmetricKeyDB.keyApplyPlatform);
			isRemoteApplyKey = 0;
		}
		else
			isRemoteApplyKey = 1;
	}

	// 检查是否存在远程分发平台
	if (strlen(symmetricKeyDB.keyDistributePlatform) > 0)
	{
		memset(&tkeyDistributePlatform,0,sizeof(tkeyDistributePlatform));
		if ((ret =  UnionReadRemoteKeyPlatformRec(symmetricKeyDB.keyDistributePlatform,conRemoteKeyPlatformKeyDirectionOfDistribute,&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE128:: UnionReadRemoteKeyPlatformRec keyDistributePlatform[%s]!\n",symmetricKeyDB.keyDistributePlatform);
			isRemoteDistributeKey = 0;
		}
		else
			isRemoteDistributeKey = 1;
	}
	
	if (isRemoteApplyKey)		// 远程申请密钥
	{
		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE128:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}
		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE128:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	if (isRemoteDistributeKey)	// 远程分发密钥
	{
		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE128:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE128:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
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
			UnionUserErrLog("in UnionDealServiceCodeE128:: UnionReadRemoteSysIDAndAppID!\n");
			return(ret);	
		}
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",usingUnit,sizeof(usingUnit))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE128:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);	
		}
	}
	else				// 本地密钥操作
	{
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE128:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);	
		}
		if ((ret = UnionReadRequestXMLPackageValue("head/appID",appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE128:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");
			return(ret);	
		}
		if (strcasecmp(UnionGetIDOfCustomization(),"aliyun") == 0)	// 阿里云
		{
			if ((ret = UnionReadRequestXMLPackageValue("head/unitID",usingUnit,sizeof(usingUnit))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE128:: UnionReadRequestXMLPackageValue[%s]!\n","head/unitID");
				return(ret);	
			}
		}
	}
	// 设置创建者
	memset(creator,0,sizeof(creator));
	UnionPieceSymmetricKeyCreator(sysID,appID,creator);

	if (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfApp)
	{
//		if ((strcmp(symmetricKeyDB.creator,creator) == 0) && 
//			(strcmp(symmetricKeyDB.usingUnit,usingUnit) == 0))
		{
			// 更新密钥状态
			if ((ret = UnionUpdateSymmetricKeyDBStatus(&symmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE128:: UnionUpdateSymmetricKeyDBStatus[%s]!\n",keyName);
				return(ret);
			}

			return(0);
		}
	}

	UnionSetResponseRemark("此密钥[%s]非本单位[%s:%s]创建,不允许更新状态!",keyName,usingUnit,creator);
	return(errCodeEsscMDLKeyOperationNotPermitted);
	*/
	//UnionSetResponseRemark("此密钥非本单位[]创建,不允许更新状态!");
	return 0;
}
