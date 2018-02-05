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
�������:	E910
������:		�ϴ��ⲿ֤��(870)
��������:	�ϴ��ⲿ֤��(870)
***************************************/
int UnionDealServiceCodeE910(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	certData[8192];
	char	appName[128];
	char	tmpBuf[128];
	int	nCertLen;
	int	certExist = 0;


	// ֤��Ӧ����
	if ((ret = UnionReadRequestXMLPackageValue("body/appName",appName,sizeof(appName) - 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE910:: UnionReadRequestXMLPackageValue[%s]!\n","head/appName");
		return(ret);
	}
	appName[ret] = 0;

	// ֤������
	if ((nCertLen = UnionReadRequestXMLPackageValue("body/certData",certData,sizeof(certData) - 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE910:: UnionReadRequestXMLPackageValue[%s]!\n","head/certData");
		return(nCertLen);
	}
	certData[nCertLen] = 0;

	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case conHsmCmdVerSRJ1401StandardHsmCmd:
			if ((certExist = UnionSRJ1401CmdCM870(appName,(unsigned char *)certData,nCertLen)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE910:: UnionSRJ1401CmdCM870,ret = [%d]!\n",certExist);
				return(certExist);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE910:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
			return(errCodeParameter);
	}

	// ����֤����ڱ�ʶ
	snprintf(tmpBuf, sizeof(tmpBuf), "%d", certExist);
	if((ret = UnionSetResponseXMLPackageValue("body/certExist",tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE910:: UnionSetResponseXMLPackageValue[%s]certExist[%d]!\n","body/certExist", certExist);
                return(ret);
        }

	return(0);
}

