
#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "baseUIService.h"

/***************************************
服务代码:	0206
服务名:		修改用户状态
功能描述:	修改用户状态
***************************************/
int UnionDealServiceCode0206(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	userID[40+1];
	int	userStatus = 0;
	char	tmpBuf[128+1];
	char	sql[512+1];
	
	// 读取用户ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("body/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0206:: UnionReadRequestXMLPackageValue[%s]!\n","body/userID");
		return(ret);
	}

	// 读取要修改的用户ID
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0206:: UnionReadRequestXMLPackageValue[%s]!\n","body/userID");
		return(ret);
	}
	if (strcmp(tmpBuf,userID) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0206:: tmpBuf[%s] == userID[%s]!\n",tmpBuf,userID);
		return(errCodeEsscMDL_InvalidOperation);
	}
	

	// 读取用户状态
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/userStatus",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0206:: UnionReadRequestXMLPackageValue[%s]!\n","body/userStatus");
		return(ret);
	}
	else
		userStatus = atoi(tmpBuf);

	
	// 修改用户状态 
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysUser set userStatus = %d,wrongPasswordTimes = 0,loginFlag = 0 where userID = '%s'",userStatus,userID);
	
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0206:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	
	return(0);
}

