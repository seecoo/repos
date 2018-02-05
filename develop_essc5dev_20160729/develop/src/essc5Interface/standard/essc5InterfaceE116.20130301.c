//	Author:		������
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
�������:	E116
������:		���ٶԳ���Կ
��������:	���ٶԳ���Կ
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

	// ��ȡ��Կ����
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
		UnionSetResponseRemark("��Կ���Ʋ���Ϊ��!");
		return(errCodeParameter);
	}

	UnionInitSymmetricKeyDB(&symmetricKeyDB);
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE116:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}
	
	// modify by leipp 20151209
	// ��ȡ���������ͷַ�ƽ̨
	if ((ret = UnionCheckRemoteKeyPlatform(symmetricKeyDB.keyName, symmetricKeyDB.keyApplyPlatform, symmetricKeyDB.keyDistributePlatform, &tkeyApplyPlatform, &tkeyDistributePlatform, &isRemoteApplyKey, &isRemoteDistributeKey)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE116:: UnionCheckRemoteKeyPlatform!\n");
		return(ret);
	}
	// modify by leipp end
	
	if (isRemoteApplyKey)		// Զ��������Կ
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE116:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}
		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE116:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	if (isRemoteDistributeKey)	// Զ�̷ַ���Կ
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE116:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE116:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	if (isRemoteKeyOperate)		// Զ����Կ����
	{
		if ((ret = UnionReadRemoteSysIDAndAppID(sysID,sizeof(sysID),appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE116:: UnionReadRemoteSysIDAndAppID!\n");
			return(ret);	
		}
	}
	else				// ������Կ����
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

	// ���ô�����
	UnionPieceSymmetricKeyCreator(sysID,appID,creator);

	if (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfApp)
	{
		if (strcasecmp(UnionGetIDOfCustomization(),"aliyun") == 0)	// ������
		{
			if ((strcmp(symmetricKeyDB.creator,creator) == 0))
			{
				// ���ٵ�ǰ��Կ
				if ((ret = UnionDropSymmetricKeyDB(&symmetricKeyDB)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE116:: UnionDropSymmetricKeyDB[%s]!\n",keyName);
					return(ret);
				}
				return(0);
			}
			UnionSetResponseRemark("��Կ[%s]�Ǵ˵�λ[%s]����,������ɾ��!",keyName,creator);
			UnionUserErrLog("in UnionDealServiceCodeE116:: ��Կ[%s]�Ǵ˵�λ[%s]����,������ɾ��!!\n",keyName,creator);
			return(errCodeEsscMDLKeyOperationNotPermitted);
		}
		else
		{
			// APPΪ3.x��4.x��Կ��ֲ��5.x�Ĵ�����
			if ((strcmp(symmetricKeyDB.creator,creator) == 0) || 
				strcmp(symmetricKeyDB.creator,"APP") == 0) 
			{
				// ���ٵ�ǰ��Կ
				if ((ret = UnionDropSymmetricKeyDB(&symmetricKeyDB)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE116:: UnionDropSymmetricKeyDB[%s]!\n",keyName);
					return(ret);
				}
				return(0);
			}
			UnionSetResponseRemark("��Կ[%s]�Ǵ˴�����[%s]����,������ɾ��!",keyName,creator);
			UnionUserErrLog("in UnionDealServiceCodeE116:: ��Կ[%s]�Ǵ˴�����[%s]����,������ɾ��!",keyName,creator);
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

	UnionSetResponseRemark("��Կ[%s]��Ӧ��ϵͳ����,������ɾ��!",keyName);
	UnionUserErrLog("in UnionDealServiceCodeE116:: ��Կ[%s]��Ӧ��ϵͳ����,������ɾ��!\n",keyName);
	return(errCodeEsscMDLKeyOperationNotPermitted);
}
