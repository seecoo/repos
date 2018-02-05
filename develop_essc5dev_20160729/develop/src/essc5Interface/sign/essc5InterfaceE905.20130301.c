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
服务代码:	E905
服务名:		PKCS7 Detached验签(883)
功能描述:	PKCS7 Detached验签(883)
***************************************/
int UnionDealServiceCodeE905(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	orgData[8192];
	//char	bcdOrgData[8192];
	char	*bcdOrgData;
	char	tmpBuf[128];
	char	signData[8192];
	unsigned char baseSignData[8192*2];
	char	sCert[8192];
	int	orgDataLen;
	int	signDataLen;
	int	certExportFlag;
	int	iCertLen;
	int	signFlag;


	// 读取原始数据
	if ((orgDataLen = UnionReadRequestXMLPackageValue("body/orgData",orgData,sizeof(orgData) - 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE905:: UnionReadRequestXMLPackageValue[%s]!\n","head/orgData");
		return(orgDataLen);
	}
	orgData[orgDataLen] = 0;
	/*
	if (orgDataLen % 2)
	{
		UnionUserErrLog("in UnionDealServiceCodeE905:: orgDataLen[%d]!\n",orgDataLen);
		return(errCodeParameter);
	}
	aschex_to_bcdhex(orgData, orgDataLen, bcdOrgData);
	bcdOrgData[orgDataLen / 2] = 0;
	orgDataLen /= 2;
	*/
	bcdOrgData = orgData;

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
		UnionUserErrLog("in UnionDealServiceCodeE905:: signFlag[%d] error!\n",signFlag);
		return(errCodeParameter);
	}

	// 签名
	if ((ret = UnionReadRequestXMLPackageValue("body/signData",(char *)baseSignData,sizeof(baseSignData) - 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE905:: UnionReadRequestXMLPackageValue[%s]!\n","head/signData");
		return(ret);
	}
	signData[ret] = 0;
	if (signFlag == 0)
	{
		aschex_to_bcdhex((char *)baseSignData, ret, signData);
		signData[ret / 2] = 0;
		signDataLen = ret/ 2;
	}
	else
	{
		signDataLen = from64tobits(signData, (char *)baseSignData);
		signData[signDataLen] = 0;
	}

	// 输出证书标识
	if ((ret = UnionReadRequestXMLPackageValue("body/certExportFlag",tmpBuf,sizeof(tmpBuf) - 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE905:: UnionReadRequestXMLPackageValue[%s]!\n","head/certExportFlag");
		return(ret);
	}
	tmpBuf[ret] = 0;
	certExportFlag = atoi(tmpBuf);

	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case conHsmCmdVerSRJ1401StandardHsmCmd:
			if ((ret = UnionSRJ1401CmdCM883((unsigned char *)bcdOrgData, orgDataLen, signData,signDataLen, certExportFlag, sCert, &iCertLen)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE905:: UnionSRJ1401CmdCM883 ret = [%d]!\n",ret);
				return(ret);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE905:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("非法的加密机指令类型");
			return(errCodeParameter);
	}

	// 设置证书数据
	if (certExportFlag)
	{
		if((ret = UnionSetResponseXMLPackageValue("body/certData",sCert)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE905:: UnionSetResponseXMLPackageValue[%s] certData[%s]!\n","body/certData", sCert);
			return(ret);
		}
	}

	return(0);
}

