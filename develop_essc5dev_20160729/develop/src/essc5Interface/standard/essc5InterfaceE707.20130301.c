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
	
	// �����ж�ȡ�ն�����
	if ((ret = UnionReadRequestXMLPackageValue("body/termType", termType, sizeof(termType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE707:: UnionReadRequestXMLPackageValue[%s]!\n","body/termType");
		return(ret);
	}
	termType[ret] = 0;
	
	// �����ж�ȡ���̱��
	if ((ret = UnionReadRequestXMLPackageValue("body/factoryId", factoryId, sizeof(factoryId))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE707:: UnionReadRequestXMLPackageValue[%s]!\n","body/factoryId");
		return(ret);
	}
	factoryId[ret] = 0;
	
	// �����ж�ȡ�豸���
	if ((ret = UnionReadRequestXMLPackageValue("body/deviceId", deviceId, sizeof(deviceId))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE707:: UnionReadRequestXMLPackageValue[%s]!\n","body/deviceId");
		return(ret);
	}
	deviceId[ret] = 0;
	
	// �����ж�ȡ������
	if ((ret = UnionReadRequestXMLPackageValue("body/organization", organization, sizeof(organization))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE707:: UnionReadRequestXMLPackageValue[%s]!\n","body/organization");
		return(ret);
	}
	organization[ret] = 0;
	
	// �����ж�ȡTKMSϵͳID
	if ((ret = UnionReadRequestXMLPackageValue("body/keyPlatformID", keyPlatformID, sizeof(keyPlatformID))) < 0)
	{
		if ((ret = UnionReadRequestXMLPackageValue("head/appID", keyPlatformID, sizeof(keyPlatformID))) < 0)
		{
		    UnionUserErrLog("in UnionDealServiceCodeE707:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");
		    return(ret);
		}
	}
	keyPlatformID[ret] = 0;
	
	// ��ȡԶ����Կƽ̨��Ϣ
	if ((ret =  UnionReadRemoteKeyPlatformRec(keyPlatformID, conRemoteKeyPlatformKeyDirectionOfApply, &tkeyDistributePlatform)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE707:: UnionReadRemoteKeyPlatformRec keyDistributePlatform[%s]!\n",keyPlatformID);
		return(ret);
	}
	
	// ��ʼ��Զ��������
	if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE707:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
		return(ret);
	}
	
	// �޸�Զ��������
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
	
	// ת�����ĵ�Զ����Կƽ̨
	if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE707:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
		return(ret);
	}
	
	return(ret);
	
}


