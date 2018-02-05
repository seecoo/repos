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
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "unionVersion.h"
#include "baseUIService.h"

/*********************************
服务代码:	0405
服务名:		生成建表SQL
功能描述:	生成建表SQL
**********************************/

int UnionDealServiceCode0405(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	int	totalNum = 0;
	char	tableName[2048+1];
	char	tmpBuf[128+1];
	char	sql[40960+1];
	char	flag[16+1];
	char	*database = NULL;

	// 表名
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionLog("in UnionDealServiceCode0405:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		//return(ret);
	}

	// 数据库
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/database",tmpBuf,sizeof(tmpBuf))) <= 0)
	{
		UnionLog("in UnionDealServiceCode0405:: UnionReadRequestXMLPackageValue[%s]!\n","body/database");
		database = UnionGetDataBaseType();
	}
	else
	{
		UnionToUpperCase(tmpBuf);
		database = tmpBuf;
	}

	// 标识
	memset(flag,0,sizeof(flag));
	if ((ret = UnionReadRequestXMLPackageValue("body/flag",flag,sizeof(flag))) <= 0)
	{
		UnionLog("in UnionDealServiceCode0405:: UnionReadRequestXMLPackageValue[%s]!\n","body/flag");
		strcpy(flag,"create");
	}
	UnionToUpperCase(flag);

	totalNum = 0;
	
	memset(sql,0,sizeof(sql));
	if ((totalNum =  UnionGetSQLScript(tableName, database, flag, sql, sizeof(sql) -1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0405:: UnionGetSQLScript tableName=[%s], database=[%s], flag=[%s]!\n", tableName, database, flag);
		return(totalNum);
	}

	// 添加指定函数
	/*
	if ((strcasecmp(database,"ORACLE") != 0) && 
		(strcasecmp(database,"DB2") != 0) &&
		(strstr(tableName,"sysMenu") != NULL))
	*/
	if (strcasecmp(database,"MYSQL") == 0 && (strstr(tableName,"sysMenu") != NULL))
	{
		if (strcasecmp(flag,"drop") == 0)
		{
			sprintf(sql + strlen(sql),"drop function if exists getChildList;");
			sprintf(sql + strlen(sql),"drop function if exists getLowerOrganization;");
		}
		else
		{
			sprintf(sql + strlen(sql),"drop function if exists getChildList;\n"
						"delimiter //\n"
						"create function getChildList(rootId int)\n"
						"returns varchar(4000)\n"
						"begin\n"
						"	declare menuID varchar(4000);\n"
						"	declare menuChild varchar(4000);\n"
						"	set menuID = '';\n"
						"	set menuChild =cast(rootId as char);\n"
						"	while menuChild is not null do\n"
						"	set menuID = concat(menuID,',',menuChild);\n"
						"	select group_concat(menuName) into menuChild from sysMenu where FIND_IN_SET(menuParentName,menuChild)>0;\n"
						"	end while;\n"
						"	return menuID;\n"
						"end//\n"
						"delimiter ;\n");
			sprintf(sql + strlen(sql),"drop function if exists getLowerOrganization;\n"
						"delimiter //\n"
						"create function getLowerOrganization(rootOrganization varchar(256))\n"
						"returns varchar(4000)\n"
						"begin\n"
						"	declare organizationList varchar(4000);\n"
						"	declare organizationChild varchar(4000);\n"
						"	set organizationList = '';\n"
						"	set organizationChild = rootOrganization;\n"
						"	while organizationChild is not null do\n"
						"	set organizationList = concat(organizationList,',',organizationChild);\n"
						"	select group_concat(organizationName) into organizationChild from organization where FIND_IN_SET(higherOrganization,organizationChild)>0;\n"
						"	end while;\n"
						"	return organizationList;\n"
						"end//\n"
						"delimiter ;\n");
		}
	}

	snprintf(tmpBuf,sizeof(tmpBuf),"%d",totalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0405:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
		return(ret);
	}

	if ((ret = UnionSetResponseXMLPackageValue("body/sql",sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0405:: UnionSetResponseXMLPackageValue[%s]!\n","body/sql");
		return(ret);
	}
	return(0);		
}
