
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
服务代码:	0204
服务名:		删除用户
功能描述:	删除用户
***************************************/
int UnionDealServiceCode0204(PUnionHsmGroupRec phsmGroupRec)
{
	int	i;
	int	ret;
	char	authUserID[60+1];
	char	userID[60+1];
	char	sql[2][512+1];

	// 管理员ID 
	memset(authUserID,0,sizeof(authUserID));
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",authUserID,sizeof(authUserID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0204:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);
	}

	// 读取修改用户ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("body/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0204:: UnionReadRequestXMLPackageValue[%s]!\n","body/userID");
		return(ret);
	}

	// userID不能为自己和admin
	if (strcmp(authUserID,userID) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0204:: 用户[%s]不允许删除!\n",userID);
		return(errCodeOperatorMDL_CannotDeleteSelf);
	}
	if (strcmp("admin",userID) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0204:: 用户[%s]不允许删除!\n",userID);
		return(0);
	}
	// 删除数据库中的指定用户
	memset(sql[0],0,sizeof(sql[0]));
	sprintf(sql[0],"delete from  sysUser where userID = '%s'",userID);

	// 删除授权表中用户信息
	memset(sql[1],0,sizeof(sql[1]));
	sprintf(sql[1],"delete from  privilege where privilegeMaster = 'USER' and  privilegeMasterValue = '%s'",userID);

	for (i = 0; i < 2; i++)
	{
		if ((ret = UnionExecRealDBSql(sql[i])) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0204:: UnionExecRealDBSql[%s]!\n",sql[i]);
			return(ret);
		}
	}

	return(0);
}

