//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "UnionMD5.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "baseUIService.h"

/***************************************
服务代码:	0901
服务名:		重启服务
功能描述:	重启服务
***************************************/
int UnionDealServiceCode0901(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	reqMac[40+1];
	char	resMac[40+1];
	char	userID[128+1];
	time_t	nowTime = 0;
	time_t	identifyCodeTime = 0;
	char	transTime[14+1];
	char	tmpBuf[1024+1];
	char	identifyCode[20+1];
	char	sql[512+1];	
	

	// 读取mac
	memset(reqMac,0,sizeof(reqMac));
	if ((ret = UnionReadRequestXMLPackageValue("body/mac",reqMac,sizeof(reqMac))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0901:: UnionReadRequestXMLPackageValue[%s]!\n","body/mac");
		return(ret);
	}

	// 读取时间
	memset(transTime,0,sizeof(transTime));
	if ((ret = UnionReadRequestXMLPackageValue("head/transTime",transTime,sizeof(transTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0901:: UnionReadRequestXMLPackageValue[%s]!\n","head/transTime");
		return(ret);
	}

	// 读取用户ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0901:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);
	}
	//
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select identifyCode from sysUser where userID = '%s'",userID);
	
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0901:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0901:: userID not found!\n");
		return(errCodeBinaryFileMDL_RecordNotFound);
	}
	
	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0901:: UnionLocateXMLPackage[detail]!\n");
		return(ret);
	}
	
	// 获取认证码
	memset(identifyCode,0,sizeof(identifyCode));	
	if ((ret = UnionReadXMLPackageValue("identifyCode",identifyCode,sizeof(identifyCode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0901:: UnionReadXMLPackageValue[%s]\n","identifyCode");
		return(ret);
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%s%s%s",userID,transTime,identifyCode);
	memset(resMac,0,sizeof(resMac));
	UnionMD5((unsigned char *)tmpBuf, strlen(tmpBuf), (unsigned char *)resMac);

	if (strcmp(resMac,reqMac) != 0)	
	{
		UnionUserErrLog("in UnionDealServiceCode0901:: reqMac[%s] != resMac[%s]!\n",reqMac,resMac);	
		UnionSetResponseRemark("mac错误");
		return(errCodeParameter);
	}
	
	time(&nowTime);	
	sscanf(identifyCode + (strlen(identifyCode) - 10),"%ld",&identifyCodeTime);
	
	if ((nowTime - identifyCodeTime) > 5) 
	{
		memset(sql,0,sizeof(sql));
		sprintf(sql,"update sysUser set identifyCode = '' where userID = '%s'",userID);
		
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0901:: UnionExecRealDBSql[%s]!\n",sql);
			return(ret);
		}

		UnionUserErrLog("in UnionDealServiceCode0901:: timeout (nowTime[%ld] - identifyCodeTime[%ld] > 5)!\n",nowTime,identifyCodeTime);
		UnionSetResponseRemark("重启服务超时");
		return(errCodeParameter);
	}
	
	// 重启命令
	system("service.x  silence restart 5&");

	return(0);
}

