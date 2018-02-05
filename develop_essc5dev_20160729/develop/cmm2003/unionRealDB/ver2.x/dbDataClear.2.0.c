// Copyright:	Union Tech.
// Author:	lics
// Date:	2014-04-03
// Version:	2.0

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "unionREC.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"

int UnionDealDaemonTask(char *parameter)
{
	int	ret = 0;
	int	len;
	int	counts = 0;
	int	i = 0;
	char	tmpBuf[128+1];
	char	tableName[32+1];
	char	dateFiledName[32+1];
	int	days;
	char	delDate[8+1];
	
	char	sql[512+1];
	
	len = sprintf(sql,"select * from dbDataClear where enabled = 1 and days > 0");
	sql[len] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealDaemonTask:: UnionSelectRealDBRecord[%s], ret = %d!\n",sql, ret);
		return(ret);
	}
	else if (ret == 0)
		return 0;
 
	UnionLog("in UnionDealDaemonTask:: UnionSelectRealDBRecord[%s], recNum[%d]!\n",sql, ret);
	
	counts = ret;
	
	for (i = 1; i <= counts; i++)
	{
		UnionLocateXMLPackage("detail", i);
			
		memset(tableName,0,sizeof(tableName));
		// 读取表名
		if ((ret = UnionReadXMLPackageValue("tableName", tableName, sizeof(tableName))) < 0)
		{
			UnionUserErrLog("in UnionDealDaemonTask:: UnionReadXMLPackageValue[%s], ret[%d]!\n", "tableName", ret);
			continue;
		}
			
		// 读取时间字段名
		memset(dateFiledName,0,sizeof(dateFiledName));
		if ((ret = UnionReadXMLPackageValue("dateFiledName", dateFiledName, sizeof(dateFiledName))) < 0)
		{
			UnionUserErrLog("in UnionDealDaemonTask:: UnionReadXMLPackageValue[%s], ret[%d]!\n","dateFiledName", ret);
			continue;
		}
			
		// 读取数据保留期限
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("days", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealDaemonTask:: UnionReadXMLPackageValue[%s], ret[%d]!\n","days", ret);
			continue;
		}
		days = atoi(tmpBuf);
		
		memset(delDate,0,sizeof(delDate));
		UnionGetDateBeforSpecDays(days,delDate);
		
		len = sprintf(sql,"delete from %s where %s < '%s'",tableName,dateFiledName,delDate);
		sql[len] = 0;

		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealDaemonTask:: UnionExecRealDBSql[%s], ret[%d]!\n",sql, ret);
			continue;
		}
			
		UnionLog("in UnionDealDaemonTask:: delete recNum[%d]!\n",ret);
	}
	
	return(counts);
		
}
