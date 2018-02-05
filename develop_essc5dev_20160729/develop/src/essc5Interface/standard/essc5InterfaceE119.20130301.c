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
�������:	E119
������:		����/����Գ���Կ
��������:	����/����Գ���Կ
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

	// ��ȡ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE119:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(keyName);
	if (strlen(keyName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE119:: keyName can not be null!\n");
		UnionSetResponseRemark("��Կ���Ʋ���Ϊ��!");
		return(errCodeParameter);
	}

	// ��ȡģʽ
	// 1��������Կ
	// 2��������Կ
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
		UnionSetResponseRemark("ģʽ[%d]����Ϊ1��2",mode);
		return(errCodeParameter);
	}

	// ��ȡ��Կ��Ϣ
	UnionInitSymmetricKeyDB(&symmetricKeyDB);
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE119:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}

	// ֻ�й���״̬���ܱ�����
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
	// ��ȡ���������ͷַ�ƽ̨
	if ((ret = UnionCheckRemoteKeyPlatform(symmetricKeyDB.keyName, symmetricKeyDB.keyApplyPlatform, symmetricKeyDB.keyDistributePlatform, &tkeyApplyPlatform, &tkeyDistributePlatform, &isRemoteApplyKey, &isRemoteDistributeKey)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE119:: UnionCheckRemoteKeyPlatform!\n");
		return(ret);
	}
	// modify by leipp end

	if (isRemoteApplyKey)		// Զ��������Կ
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE119:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}
		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE119:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	if (isRemoteDistributeKey)	// Զ�̷ַ���Կ
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE119:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE119:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	if (isRemoteKeyOperate)		// Զ����Կ����
	{
		if ((ret = UnionReadRemoteSysIDAndAppID(sysID,sizeof(sysID),appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE119:: UnionReadRemoteSysIDAndAppID!\n");
			return(ret);	
		}
	}
	else				// ������Կ����
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
	// ���ô�����
	UnionPieceSymmetricKeyCreator(sysID,appID,creator);

	if (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfApp)
	{
//		if ((strcmp(symmetricKeyDB.creator,creator) == 0) && 
//			(strcmp(symmetricKeyDB.usingUnit,usingUnit) == 0))
		if (strcmp(symmetricKeyDB.creator,creator) == 0) 
		{
			// ������Կ״̬
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

	UnionSetResponseRemark("����Կ[%s]�Ǳ���λ[%s]����,���������״̬!",keyName,creator);
	return(errCodeEsscMDLKeyOperationNotPermitted);
}


