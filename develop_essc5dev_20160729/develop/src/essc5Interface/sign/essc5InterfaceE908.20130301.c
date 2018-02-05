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
服务代码:	E908
服务名:		数字信封加密(896)
功能描述:	数字信封加密(896)
***************************************/
int UnionDealServiceCodeE908(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	orgData[8192];
	//char	bcdOrgData[8192];
	char	*bcdOrgData = NULL;
	char	appName[128];
	char	certData[8192];
	unsigned char	messData[8192];
	int	messDataLen;
	int	orgDataLen;
	int	isUseAppName = 0;

	// 读取原始数据
	if ((orgDataLen = UnionReadRequestXMLPackageValue("body/orgData",orgData,sizeof(orgData) - 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE908:: UnionReadRequestXMLPackageValue[%s]!\n","head/orgData");
		return(orgDataLen);
	}
	orgData[orgDataLen] = 0;
	/*
	if (orgDataLen % 2)
	{
		UnionUserErrLog("in UnionDealServiceCodeE908:: orgDataLen[%d]!\n",orgDataLen);
		return(errCodeParameter);
	}
	aschex_to_bcdhex(orgData, orgDataLen, bcdOrgData);
	bcdOrgData[orgDataLen / 2] = 0;
	orgDataLen /= 2;
	*/
	bcdOrgData = orgData;

	// 证书应用名
	if ((ret = UnionReadRequestXMLPackageValue("body/appName",appName,sizeof(appName) - 1)) < 0)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/certData",certData,sizeof(certData) - 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE908:: UnionReadRequestXMLPackageValue[%s] or UnionReadRequestXMLPackageValue[%s]!\n","head/appName", "body/certData");
			return(ret);
		}
		else
		{
			certData[ret] = 0;
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
			memset(messData, 0, sizeof(messData));
			if (isUseAppName)
			{
				if ((ret = UnionSRJ1401CmdCM896((unsigned char *)bcdOrgData, orgDataLen,appName, NULL, messData, &messDataLen)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE908:: UnionSRJ1401CmdCM896 use appName,ret = [%d]!\n",ret);
					return(ret);
				}
			}
			else
			{
				if ((ret = UnionSRJ1401CmdCM896((unsigned char *)bcdOrgData, orgDataLen,NULL, certData, messData, &messDataLen)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE908:: UnionSRJ1401CmdCM896 use certData,ret = [%d]!\n",ret);
					return(ret);
				}
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE908:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("非法的加密机指令类型");
			return(errCodeParameter);
	}

	// 设置数字信封
	if((ret = UnionSetResponseXMLPackageValue("body/pkcs7Env",(char *)messData)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE908:: UnionSetResponseXMLPackageValue[%s]sigData[%s]!\n","body/pkcs7Env", messData);
                return(ret);
        }

	return(0);
}

