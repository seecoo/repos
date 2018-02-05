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
服务代码:	E913
服务名:		证书解析(898)
功能描述:	证书解析(898)
***************************************/
int UnionDealServiceCodeE913(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	appName[128];
	char	tmpBuf[128];
	char	certData[8192];
	char	outCertData[8192];
	unsigned char	messData[8192];
	int	isUseAppName = 0;
	int	certDataLen = 0;
	int	outCertDataLen;
	int	tag;


	// 证书应用名
	if ((ret = UnionReadRequestXMLPackageValue("body/appName",appName,sizeof(appName) - 1)) < 0)
	{
		if ((certDataLen = UnionReadRequestXMLPackageValue("body/certData",certData,sizeof(certData) - 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE913:: UnionReadRequestXMLPackageValue[%s] or UnionReadRequestXMLPackageValue[%s]!\n","head/appName", "body/certData");
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

	// tag
	if ((ret = UnionReadRequestXMLPackageValue("body/tag",tmpBuf,sizeof(tmpBuf) - 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE913:: UnionReadRequestXMLPackageValue[%s]!\n","head/tag");
		return(ret);
	}
	tmpBuf[ret] = 0;
	tag = atoi(tmpBuf);

	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case conHsmCmdVerSRJ1401StandardHsmCmd:
			memset(messData, 0, sizeof(messData));
			if (isUseAppName)
			{
				if ((ret = UnionSRJ1401CmdCM898(appName, NULL,0,tag, outCertData, &outCertDataLen)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE913:: UnionSRJ1401CmdCM898 use appName,ret = [%d]!\n",ret);
					return(ret);
				}
			}
			else
			{
				if ((ret = UnionSRJ1401CmdCM898(NULL, certData,certDataLen,tag, outCertData, &outCertDataLen)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE913:: UnionSRJ1401CmdCM896 use certData,ret = [%d]!\n",ret);
					return(ret);
				}
			}
			outCertData[outCertDataLen] = 0;
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE913:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("非法的加密机指令类型");
			return(errCodeParameter);
	}

	if((ret = UnionSetResponseXMLPackageValue("body/certData",outCertData)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE913:: UnionSetResponseXMLPackageValue[%s]!\n","body/certData");
                return(ret);
        }

	return(0);
}

