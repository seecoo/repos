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
服务代码:	E911
服务名:		证书是否存在(877)
功能描述:	证书是否存在(877)
***************************************/
int UnionDealServiceCodeE911(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	appName[128];
	char	tmpBuf[128];
	int	certExist = 0;


	// 证书应用名
	if ((ret = UnionReadRequestXMLPackageValue("body/appName",appName,sizeof(appName) - 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE911:: UnionReadRequestXMLPackageValue[%s]!\n","head/appName");
		return(ret);
	}
	appName[ret] = 0;

	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case conHsmCmdVerSRJ1401StandardHsmCmd:
			if ((certExist = UnionSRJ1401CmdCM877(appName)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE911:: UnionSRJ1401CmdCM877,ret = [%d]!\n",certExist);
				return(certExist);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE911:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("非法的加密机指令类型");
			return(errCodeParameter);
	}

	// 设置证书存在标识
	snprintf(tmpBuf, sizeof(tmpBuf), "%d", certExist);
	if((ret = UnionSetResponseXMLPackageValue("body/certExist",tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE911:: UnionSetResponseXMLPackageValue[%s]certExist[%d]!\n","body/certExist", certExist);
                return(ret);
        }

	return(0);
}

