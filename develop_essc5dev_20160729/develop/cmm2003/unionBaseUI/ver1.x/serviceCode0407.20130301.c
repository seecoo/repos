//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-02-22

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/dir.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "unionVersion.h"
#include "inputAndOutputDataForDB.h"
#include "baseUIService.h"

/*********************************
服务代码:	0407
服务名:		在数据库中新建表
功能描述:	在数据库中新建表
**********************************/
int UnionDealServiceCode0407(PUnionHsmGroupRec phsmGroupRec)
{
	int	i;
	int	ret;
	int	len;
	int	successNum = 0;
	int	failNum = 0;
	char	tableName[40+1];
	char	sql[8192+1];
	char	*database = NULL;
	char	*psql = NULL;
	
	// 表名
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0407:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);
	}

	// 数据库
	database = UnionGetDataBaseType();

	memset(sql,0,sizeof(sql));
	if ((len =  UnionGetCreateSQLByTableName(tableName,database,sql,sizeof(sql))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0407:: UnionGetCreateSQLByTableName[%s]!\n",tableName);
		return(len);
	}

	psql = sql;
	for(i = 0; i < len; i++)
	{
		if (sql[i] != ';')
			continue;
		else
			sql[i] = 0;

		if ((ret = UnionExecRealDBSql(psql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0407:: UnionExecRealDBSql!\n");
			return(ret);
		}
		psql = sql + i + 2;
	}
	
	if ((ret = UnionInputOneTableDataToDB(tableName,&successNum, &failNum)) < 0)
		UnionLog("in UnionDealServiceCode0407:: UnionInputOneTableDataToDB error! ret = [%d]!\n",ret);
	else
		UnionLog("in UnionDealServiceCode0407:: UnionInputOneTableDataToDB success! 总记录数[%d] 成功记录数[%d] 失败记录数[%d]!\n",ret,successNum,failNum);
			
	return(0);		
}
