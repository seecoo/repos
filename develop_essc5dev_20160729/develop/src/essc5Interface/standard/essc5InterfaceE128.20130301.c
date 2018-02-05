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
�������:	E128
������:		����/����Գ���Կ
��������:	����/����Գ���Կ
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

	// ��ȡ��Կ����
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
		UnionSetResponseRemark("��Կ���Ʋ���Ϊ��!");
		return(errCodeParameter);
	}

	// ��ȡģʽ
	// 1��������Կ
	// 2��������Կ
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
		UnionSetResponseRemark("ģʽ[%d]����Ϊ1��2",mode);
		return(errCodeParameter);
	}

	// ��ȡ��Կ��Ϣ
	memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE128:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}

	// ֻ�й���״̬���ܱ�����
	if (conSymmetricKeyStatusOfSuspend == symmetricKeyDB.status && mode == 1)
		symmetricKeyDB.status = conSymmetricKeyStatusOfEnabled;
	if (mode == 2)
		symmetricKeyDB.status = conSymmetricKeyStatusOfSuspend;
	
	// ����Ƿ����Զ������ƽ̨
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

	// ����Ƿ����Զ�̷ַ�ƽ̨
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
	
	if (isRemoteApplyKey)		// Զ��������Կ
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE128:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}
		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE128:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	if (isRemoteDistributeKey)	// Զ�̷ַ���Կ
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE128:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE128:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
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
			UnionUserErrLog("in UnionDealServiceCodeE128:: UnionReadRemoteSysIDAndAppID!\n");
			return(ret);	
		}
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",usingUnit,sizeof(usingUnit))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE128:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);	
		}
	}
	else				// ������Կ����
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
		if (strcasecmp(UnionGetIDOfCustomization(),"aliyun") == 0)	// ������
		{
			if ((ret = UnionReadRequestXMLPackageValue("head/unitID",usingUnit,sizeof(usingUnit))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE128:: UnionReadRequestXMLPackageValue[%s]!\n","head/unitID");
				return(ret);	
			}
		}
	}
	// ���ô�����
	memset(creator,0,sizeof(creator));
	UnionPieceSymmetricKeyCreator(sysID,appID,creator);

	if (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfApp)
	{
//		if ((strcmp(symmetricKeyDB.creator,creator) == 0) && 
//			(strcmp(symmetricKeyDB.usingUnit,usingUnit) == 0))
		{
			// ������Կ״̬
			if ((ret = UnionUpdateSymmetricKeyDBStatus(&symmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE128:: UnionUpdateSymmetricKeyDBStatus[%s]!\n",keyName);
				return(ret);
			}

			return(0);
		}
	}

	UnionSetResponseRemark("����Կ[%s]�Ǳ���λ[%s:%s]����,���������״̬!",keyName,usingUnit,creator);
	return(errCodeEsscMDLKeyOperationNotPermitted);
	*/
	//UnionSetResponseRemark("����Կ�Ǳ���λ[]����,���������״̬!");
	return 0;
}
