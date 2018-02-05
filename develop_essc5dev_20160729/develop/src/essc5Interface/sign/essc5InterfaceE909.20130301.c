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
#include "commWithHsmSvr.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "unionHsmCmdVersion.h"
#include "unionSRJ1401Cmd.h"
#include "base64.h"

/***************************************
�������:	E909
������:		�����ŷ����(897)
��������:	�����ŷ����(897)
***************************************/
int UnionDealServiceCodeE909(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	orgData[8192];
	char	ascOrgData[8192*2];
	char	appName[128];
	unsigned char	messData[8192];
	int	messDataLen;
	int	orgDataLen;

	// ��ȡ�����ŷ�
	if ((messDataLen = UnionReadRequestXMLPackageValue("body/pkcs7Env",(char *)messData,sizeof(messData) - 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE909:: UnionReadRequestXMLPackageValue[%s]!\n","head/pkcs7Env");
		return(messDataLen);
	}
	messData[messDataLen] = 0;

	// ֤��Ӧ����
	if ((ret = UnionReadRequestXMLPackageValue("body/appName",appName,sizeof(appName) - 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE909:: UnionReadRequestXMLPackageValue[%s]!\n","head/appName");
		return(ret);
	}
	appName[ret] = 0;

	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case conHsmCmdVerSRJ1401StandardHsmCmd:
			if ((ret = UnionSRJ1401CmdCM897((char *)messData,appName,(unsigned char *)orgData,&orgDataLen)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE909:: UnionSRJ1401CmdCM897,ret = [%d]!\n",ret);
				return(ret);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE909:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
			return(errCodeParameter);
	}

	bcdhex_to_aschex(orgData, orgDataLen, ascOrgData);
	ascOrgData[orgDataLen*2] = 0;
	// ����ԭʼ����
	if((ret = UnionSetResponseXMLPackageValue("body/orgData",ascOrgData)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE909:: UnionSetResponseXMLPackageValue[%s]sigData[%s]!\n","body/orgData", ascOrgData);
                return(ret);
        }

	return(0);
}

