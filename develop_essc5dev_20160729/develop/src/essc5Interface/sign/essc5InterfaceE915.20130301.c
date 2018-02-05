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
#include "commWithHsmSvr.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "unionHsmCmdVersion.h"
#include "unionSRJ1401Cmd.h"
#include "base64.h"

/***************************************
服务代码:	E915
服务名:		证书CRL检测(878)
功能描述:	证书CRL检测(878)
***************************************/
int UnionDealServiceCodeE915(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	appName[128];
	char	tmpBuf[128];
	char	certData[8192];
	int	isUseAppName = 0;
	int	certDataLen = 0;
	int	CRLFlag = 0;


	// 证书应用名
	if ((ret = UnionReadRequestXMLPackageValue("body/appName",appName,sizeof(appName) - 1)) < 0)
	{
		if ((certDataLen = UnionReadRequestXMLPackageValue("body/certData",certData,sizeof(certData) - 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE915:: UnionReadRequestXMLPackageValue[%s] or UnionReadRequestXMLPackageValue[%s]!\n","head/appName", "body/certData");
			return(certDataLen);
		}
		else
		{
			certData[certDataLen] = 0;
			isUseAppName = 0;
		}
	}
	else
	{
		appName[ret] = 0;
		isUseAppName = 1;
	}

	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case conHsmCmdVerSRJ1401StandardHsmCmd:
			if (isUseAppName)
			{
				if ((CRLFlag = UnionSRJ1401CmdCM878(appName, NULL,0)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE915:: UnionSRJ1401CmdCM878 use appName,ret = [%d]!\n",CRLFlag);
					return(CRLFlag);
				}
			}
			else
			{
				if ((CRLFlag = UnionSRJ1401CmdCM878(NULL, certData,certDataLen)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE915:: UnionSRJ1401CmdCM878 use certData,ret = [%d]!\n",CRLFlag);
					return(CRLFlag);
				}
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE915:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("非法的加密机指令类型");
			return(errCodeParameter);
	}

	snprintf(tmpBuf, sizeof(tmpBuf), "%d", CRLFlag);
	if((ret = UnionSetResponseXMLPackageValue("body/CRLFlag",tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE915:: UnionSetResponseXMLPackageValue[%s]!\n","body/CRLFlag");
                return(ret);
        }

	return(0);
}

