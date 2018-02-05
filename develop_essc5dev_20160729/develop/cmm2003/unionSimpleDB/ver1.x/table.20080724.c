// Author:	ChenJiaMei
// Date:	2008-7-16

#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#include <vcl.h>
#include <Filectrl.hpp>
#endif

#include "unionRecFile.h"
#include "table.h"
#include "database.h"
#include "recInfo.h"
#include "allTableInfo.h"

#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"

// 说明：本文件中的程序基于文件数据库的操作

int UnionGetDirOfTable(char *tableName,char *dir)
{
        sprintf(dir,"%s/%s",UnionGetCurrentDatabaseName(),tableName);
        return(0);
}

/*
功能：创建一张表
输入参数：
	tableName：表名
	primaryKeyStr ：表的关键字域名，如果关键字由多个域组成，则各个域名之间用'|'分隔
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionCreateTable(char *tableName,char *primaryKeyStr)
{
	char	tmpBuf[512];
        char    dir[512];
	int	ret;
	
	if (tableName == NULL || primaryKeyStr == NULL || strlen(primaryKeyStr) == 0)
	{
		UnionUserErrLog("in UnionCreateTable:: parameters null pointer!\n");
		return(errCodeParameter);
	}
	
	if (UnionExistTable(tableName))
	{
		UnionUserErrLog("in UnionCreateTable:: table [%s] already exist!\n",tableName);
		return(errCodeDatabaseMDL_TableAlreadyExist);
	}

	// 创建表目录
        if ((ret = UnionGetDirOfTable(tableName,dir)) < 0)
	{
		UnionUserErrLog("in UnionCreateTable:: UnionGetDirOfTable of [%s]!\n",tableName);
		return(ret);
	}
	
	// 修改表信息文件
	ret=UnionAddToAllTableInfo(tableName);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionCreateTable:: UnionAddToAllTableInfo for %s fail! return=[%d]\n",tableName,ret);
		return(ret);
	}

	if ((ret = UnionCreateDir(dir)) < 0)
	{
		UnionUserErrLog("in UnionCreateTable:: UnionCreateDir of [%s]!\n",dir);
		UnionDeleteFromAllTableInfo(tableName);	// 删除表信息
		return(ret);
	}

	// 创建记录信息文件
	ret=UnionCreateRecInfo(tableName,primaryKeyStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionCreateTable:: UnionCreateRecInfo for %s fail! return=[%d]\n",tableName,ret);
		UnionDeleteFromAllTableInfo(tableName);	// 删除表信息
		UnionRemoveDir(dir);			// 删除表目录		
		return(ret);
	}
	return(0);
}

/*
功能：删除一张表
输入参数：
	tableName：表名
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionDropTable(char *tableName)
{
	char	tmpBuf[512];
	char	dir[512];
	int	ret;

	if (tableName == NULL)
	{
		UnionUserErrLog("in UnionDropTable:: parameters null pointer!\n");
		return(errCodeParameter);
	}

	if (!UnionExistTable(tableName))
	{
		UnionUserErrLog("in UnionDropTable:: table [%s] does not exist!\n",tableName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// 表目录
        if ((ret = UnionGetDirOfTable(tableName,dir)) < 0)
	{
		UnionUserErrLog("in UnionDropTable:: UnionGetDirOfTable of [%s]!\n",tableName);
		return(ret);
	}
	
	// 删除所有记录和记录信息文件
	sprintf(tmpBuf,"%s/*.rec",dir);
	UnionDeleteRecFile(tmpBuf);
	sprintf(tmpBuf,"%s/rec.info",dir);
	UnionDeleteRecFile(tmpBuf);
	
	// 修改表信息文件
	ret=UnionDeleteFromAllTableInfo(tableName);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionDropTable:: UnionDeleteFromAllTableInfo for %s fail! return=[%d]\n",tableName,ret);
		return(ret);
	}

	// 删除目录
	ret=UnionRemoveDir(dir);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionDropTable:: UnionRemoveDir of %s fail! return=[%d]\n",dir,ret);
		return(ret);
	}
	return(ret);
}

/*
功能：判断一张表是否存在
输入参数：
	tableName：表名
输出参数：
	无
返回值：
	>0：表存在
	0：表不存在
	<0：失败
*/
int UnionExistTable(char *tableName)
{
	char	dir[512];
        int     ret;

	if (tableName == NULL)
	{
		UnionUserErrLog("in UnionExistTable:: parameters null pointer!\n");
		return(errCodeParameter);
	}

	// 表目录
        if ((ret = UnionGetDirOfTable(tableName,dir)) < 0)
	{
		UnionUserErrLog("in UnionExistTable:: UnionGetDirOfTable of [%s]!\n",tableName);
		return(ret);
	}
	return(UnionExistsDir(dir));
}
