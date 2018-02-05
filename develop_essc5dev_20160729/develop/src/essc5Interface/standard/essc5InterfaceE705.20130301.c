//	Author: fus	
//	Date:		2014/08/04
//	Version:	2.0

// Modification History

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
//#include "commWithHsmSvr.h"
#include "unionHsmCmdVersion.h"

int UnionDealServiceCodeE705(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	char				termType[40+1];
	char				termID[50+1];
	char				authCode[40+1];
	char				authCodeLen[40+1];
	char				authCodeType[40+1];
	char				keyPlatformID[40+1];

//	TUnionSymmetricKeyDB            symmetricKeyDB;
//      PUnionSymmetricKeyValue         psymmetricKeyValue = NULL;
	TUnionRemoteKeyPlatform         tkeyDistributePlatform;

	
	//��ȡ�ն�����
	//memset(termType,0,sizeof(termType));
	if ((ret = UnionReadRequestXMLPackageValue("body/termType",termType,sizeof(termType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE705:: UnionReadRequestXMLPackageValue[%s]!\n","body/termType");
		return(ret);
	}
	termType[ret] = 0;

	//��ȡ�ն˺�
	//memset(termID,0,sizeof(termID));
	if ((ret = UnionReadRequestXMLPackageValue("body/termID",termID,sizeof(termID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE705:: UnionReadRequestXMLPackageValue[%s]!\n","body/termID");
		return(ret);
	}
	termID[ret] = 0;

	//��ȡ��֤�볤��
        //memset(authCodeLen,0,sizeof(authCodeLen));
	if ((ret = UnionReadRequestXMLPackageValue("body/authCodeLen",authCodeLen,sizeof(authCodeLen))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE705:: UnionReadRequestXMLPackageValue[%s]!\n","body/authCodeLen");
		return(ret);
	}
	authCodeLen[ret] = 0;

	//��ȡ��֤�븴�Ӷ�
	//memset(authCodeType,0,sizeof(authCodeType));
        if ((ret = UnionReadRequestXMLPackageValue("body/authCodeType",authCodeType,sizeof(authCodeType))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE705:: UnionReadRequestXMLPackageValue[%s]!\n","body/authCodeType");
                return(ret);
        }
	authCodeType[ret] = 0;

	//��ȡƽ̨ID
	//memset(keyPlatformID,0,sizeof(keyPlatformID));
        if ((ret = UnionReadRequestXMLPackageValue("body/keyPlatformID",keyPlatformID,sizeof(keyPlatformID))) < 0)
        {
		if ((ret = UnionReadRequestXMLPackageValue("head/appID",keyPlatformID,sizeof(keyPlatformID))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE705:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");
                        return(ret);
                }
        }
	keyPlatformID[ret] = 0;

	// ��ȡ�ַ�ƽ̨��Ϣ
        memset(&tkeyDistributePlatform,0,sizeof(tkeyDistributePlatform));
        if ((ret =  UnionReadRemoteKeyPlatformRec(keyPlatformID,conRemoteKeyPlatformKeyDirectionOfApply,&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE705:: UnionReadRemoteKeyPlatformRec keyDistributePlatform[%s]!\n",keyPlatformID);
                return(ret);
        }
	
	// ��ʼ��Զ����Կ����������
        if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE705:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
                return(ret);
        }
	
        // �޸Ķ�Ӧ������ 
        if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","T005")) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE705:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","T001");
                return(ret);
        }
	if ((ret = UnionSetRequestRemoteXMLPackageValue("body/termType",termType)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE705:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/termType",termType);
                return(ret);
        }
	if ((ret = UnionSetRequestRemoteXMLPackageValue("body/termID",termID)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE705:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/termID",termID);
                return(ret);
        }
	if ((ret = UnionSetRequestRemoteXMLPackageValue("body/authCodeLen",authCodeLen)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE705:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/authCodeLen",authCodeLen);
                return(ret);
        }
	if ((ret = UnionSetRequestRemoteXMLPackageValue("body/authCodeType",authCodeType)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE705:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/authCodeType",authCodeType);
                return(ret);
        }
	// ת����Կ������Զ��ƽ̨
        if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE705:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
                return(ret);
        }
	// ��ȡԶ����Կֵ
        //memset(authCode,0,sizeof(authCode));
      	if((ret = UnionReadResponseRemoteXMLPackageValue("body/authCode",authCode,sizeof(authCode))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE705:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/authCode");
                return(ret);
        }
	authCode[ret] = 0;

	//����Զ����Կֵ	
	if ((ret = UnionSetResponseXMLPackageValue("body/authCode",authCode)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE705:: UnionSetResponseXMLPackageValue[%s]!\n","body/authCode");
                return(ret);
        }
	return(ret);
}


