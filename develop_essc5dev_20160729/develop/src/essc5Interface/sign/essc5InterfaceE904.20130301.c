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
服务代码:	E904
服务名:		PKCS7 Detached签名(882)
功能描述:	PKCS7 Detached签名(882)
***************************************/
int UnionDealServiceCodeE904(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	orgData[8192];
	//char	bcdOrgData[8192];
	char	*bcdOrgData = NULL;
	char	appName[128];
	char	tmpBuf[128];
	char	signData[8192];
	unsigned char	baseSignData[8192*2];
	int	algFlag;
	int	orgDataLen;
	int	signDataLen;
	int	signFlag;

	// 读取原始数据
	if ((orgDataLen = UnionReadRequestXMLPackageValue("body/orgData",orgData,sizeof(orgData) - 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE904:: UnionReadRequestXMLPackageValue[%s]!\n","head/orgData");
		return(orgDataLen);
	}
	orgData[orgDataLen] = 0;
	/*
	if (orgDataLen % 2)
	{
		UnionUserErrLog("in UnionDealServiceCodeE904:: orgDataLen[%d]!\n",orgDataLen);
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
		UnionUserErrLog("in UnionDealServiceCodeE904:: UnionReadRequestXMLPackageValue[%s]!\n","head/appName");
		return(ret);
	}
	appName[ret] = 0;

	// 算法标识
	if ((ret = UnionReadRequestXMLPackageValue("body/algFlag",tmpBuf,sizeof(tmpBuf) - 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE904:: UnionReadRequestXMLPackageValue[%s]!\n","head/algFlag");
		return(ret);
	}
	tmpBuf[ret] = 0;
	algFlag = atoi(tmpBuf);

	// 签名数据类型
	if ((ret = UnionReadRequestXMLPackageValue("body/signFlag",tmpBuf,sizeof(tmpBuf) - 1)) < 0)
	{
		signFlag = 0;
	}
	else
	{
		tmpBuf[ret] = 0;
		signFlag = atoi(tmpBuf);
	}
	if (signFlag != 0 && signFlag != 1)
	{
		UnionUserErrLog("in UnionDealServiceCodeE904:: signFlag[%d] error!\n",signFlag);
		return(errCodeParameter);
	}

	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case conHsmCmdVerSRJ1401StandardHsmCmd:
			if ((ret = UnionSRJ1401CmdCM882((unsigned char *)bcdOrgData, orgDataLen, appName, algFlag, signData, &signDataLen)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE904:: UnionSRJ1401CmdCM882,ret = [%d]!\n",ret);
				return(ret);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE904:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("非法的加密机指令类型");
			return(errCodeParameter);
	}

	if (signFlag == 0)
	{
		// 十六进制数
		bcdhex_to_aschex(signData, signDataLen, (char *)baseSignData);
		baseSignData[signDataLen * 2] = 0;
	}
	else
	{
		// 转换为 base64
		to64frombits(baseSignData, (unsigned char *)signData, signDataLen);
	}
	
	// 设置签名结果
	if((ret = UnionSetResponseXMLPackageValue("body/signData",(char *)baseSignData)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE904:: UnionSetResponseXMLPackageValue[%s]sigData[%s]!\n","body/signData", baseSignData);
                return(ret);
        }

	return(0);
}

