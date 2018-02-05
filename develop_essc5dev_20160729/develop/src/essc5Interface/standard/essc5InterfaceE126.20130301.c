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
#include "asymmetricKeyDB.h"
#include "remoteKeyPlatform.h"

/***************************************
�������:	E126
������:		���ٷǶԳ���Կ
��������:	���ٷǶԳ���Կ
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

	// ���Զ�̱�ʶ
	isRemoteKeyOperate = UnionIsRemoteKeyOperate();
	
	// ��ȡ��Կ����
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
		UnionSetResponseRemark("��Կ���Ʋ���Ϊ��!");
		return(errCodeParameter);
	}

	memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));
	if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,0,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE126:: UnionReadAsymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}

	// modify by leipp 20151209
	// ��ȡ���������ͷַ�ƽ̨
	if ((ret = UnionCheckRemoteKeyPlatform(asymmetricKeyDB.keyName, asymmetricKeyDB.keyApplyPlatform, asymmetricKeyDB.keyDistributePlatform, &tkeyApplyPlatform, &tkeyDistributePlatform, &isRemoteApplyKey, &isRemoteDistributeKey)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE126:: UnionCheckRemoteKeyPlatform!\n");
		return(ret);
	}
	// modify by leipp end

	// Զ������
	if (isRemoteApplyKey)
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE126:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE126:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	// Զ�̷ַ�
	if (isRemoteDistributeKey)
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE126:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}
		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE126:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}
	
	memset(sysID,0,sizeof(sysID));
	memset(appID,0,sizeof(appID));
	memset(usingUnit,0,sizeof(usingUnit));
	if (isRemoteKeyOperate)		// Զ����Կ����
	{
		if ((ret = UnionReadRemoteSysIDAndAppID(sysID,sizeof(sysID),appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE126:: UnionReadRemoteSysIDAndAppID!\n");
			return(ret);	
		}
	}
	else				// ������Կ����
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
	// ���ô�����
	memset(creator,0,sizeof(creator));
	UnionPieceAsymmetricKeyCreator(sysID,appID,creator);

	if (asymmetricKeyDB.creatorType == conAsymmetricCreatorTypeOfApp)
	{
		if (strcasecmp(UnionGetIDOfCustomization(),"aliyun") == 0)	// ������
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
			UnionSetResponseRemark("��Կ[%s]�Ǵ˵�λ[%s:%s]����,������ɾ��!",keyName,usingUnit,creator);
			UnionUserErrLog("in UnionDealServiceCodeE126:: ��Կ[%s]�Ǵ˵�λ[%s:%s]����,������ɾ��!",keyName,usingUnit,creator);
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
			UnionSetResponseRemark("��Կ[%s]�Ǵ˴�����[%s]����,������ɾ��!",keyName,creator);
			UnionUserErrLog("in UnionDealServiceCodeE126:: ��Կ[%s]�Ǵ˴�����[%s]����,������ɾ��!",keyName,creator);
			return(errCodeEsscMDLKeyOperationNotPermitted);
		}
	}
	UnionSetResponseRemark("��Կ[%s]��Ӧ��ϵͳ����,������ɾ��!",keyName);
	UnionUserErrLog("in UnionDealServiceCodeE126:: ��Կ[%s]��Ӧ��ϵͳ����,������ɾ��!",keyName);
	return(errCodeEsscMDLKeyOperationNotPermitted);
}


