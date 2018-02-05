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
#include "database.h"
#include "allTableInfo.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionWorkingDir.h"
#include "unionErrCode.h"

// 说明：本文件中的程序基于文件数据库的操作

char	gunionDatabaseName[512] = "";	// 存放当前数据库目录
int	gunionIsDatabaseConnected=0;	// 该标志限制只能连接一次数据库，且只能在关闭原来的数据库后，才能与新的数据库建立连接

// 获得当前数据库的名称
char *UnionGetCurrentDatabaseName()
{
        return(gunionDatabaseName);
}

// 获得数据库的目录
int UnionGetDatabaseDir(char *databaseName,char *dir)
{
	char	*ptr;

#ifdef _WIN32
	char	mainDir[512+1];
	
	memset(mainDir,0,sizeof(mainDir));
	UnionGetMainWorkingDir(mainDir);
	sprintf(dir,"%s/%s",mainDir,databaseName);
	return(0);
#else
	if ((ptr = (char *)getenv("UNIONDB")) == NULL)
	{
		UnionUserErrLog("in UnionCreateDatabase:: UNIONDB not defined in .profile!\n");
		return(errCodeParameter);
	}
	sprintf(dir,"%s/%s",ptr,databaseName);	
	return(0);
#endif
}

/*
功能：创建一个数据库
输入参数：
	databaseName：数据库名
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionCreateDatabase(char *databaseName)
{
	char	dir[512];
	int	ret;

	if (UnionExistDatabase(databaseName))
	{
		UnionUserErrLog("in UnionCreateDatabase:: database [%s] already exist!\n",databaseName);
		return(errCodeDatabaseMDL_DatabaseAlreadyExist);
	}

	// 创建数据库目录
	memset(dir,0,sizeof(dir));
	if ((ret = UnionGetDatabaseDir(databaseName,dir)) < 0)
	{
		UnionUserErrLog("in UnionCreateDatabase:: UnionGetDatabaseDir [%s]!\n",databaseName);
		return(ret);
	}
	if ((ret = UnionCreateDir(dir)) < 0)
	{
		UnionUserErrLog("in UnionCreateDatabase:: UnionCreateDir [%s]!\n",dir);
		return(ret);
	}
        if ((ret = UnionConnectDatabase(databaseName)) < 0)
	{
		UnionUserErrLog("in UnionCreateDatabase:: UnionConnectDatabase [%s]!\n",databaseName);
		return(ret);
	}
	// 创建表信息文件
	ret=UnionCreateAllTableInfo();
	if (ret < 0)
	{
		UnionUserErrLog("in UnionCreateDatabase:: UnionCreateAllTableInfo for [%s] fail! return=[%d]\n",databaseName,ret);
		UnionRemoveDir(dir);
		return(ret);
	}
	return(0);
}

/*
功能：删除一个数据库
输入参数：
	databaseName：数据库名
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionDropDatabase(char *databaseName)
{
	char	dir[512];
	int	ret;

	if (!UnionExistDatabase(databaseName))
	{
		UnionUserErrLog("in UnionDropDatabase:: database [%s] already exist!\n",databaseName);
		return(errCodeDatabaseMDL_DatabaseNotFound);
	}

	// 创建数据库目录
	memset(dir,0,sizeof(dir));
	if ((ret = UnionGetDatabaseDir(databaseName,dir)) < 0)
	{
		UnionUserErrLog("in UnionDropDatabase:: UnionGetDatabaseDir [%s]!\n",databaseName);
		return(ret);
	}
	if ((ret = UnionRemoveDir(dir)) < 0)
	{
		UnionUserErrLog("in UnionDropDatabase:: UnionRemoveDir [%s]!\n",dir);
		return(ret);
	}
	return(ret);
}

/*
功能：判断一个数据库是否存在
输入参数：
	databaseName：数据库名
输出参数：
	无
返回值：
	>0：数据库存在
	0：数据库不存在
	<0：失败
*/
int UnionExistDatabase(char *databaseName)
{
	char	dir[512+1];
	int	ret;
	
	// 创建数据库目录
	memset(dir,0,sizeof(dir));
	if ((ret = UnionGetDatabaseDir(databaseName,dir)) < 0)
	{
		UnionUserErrLog("in UnionExistDatabase:: UnionGetDatabaseDir [%s]!\n",databaseName);
		return(0);
	}
	return(UnionExistsDir(dir));
}

/*
功能：建立与数据库的连接
输入参数：
	databaseName：数据库名
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionConnectDatabase(char *databaseName)
{
	char	dir[512];
        int     ret;
	
	if (gunionIsDatabaseConnected)
		return(0);
	if (!UnionExistDatabase(databaseName))
	{
		UnionUserErrLog("in UnionConnectDatabase:: database [%s] does not exist!\n",databaseName);
		return(errCodeDatabaseMDL_DatabaseNotFound);
	}
	if ((ret = UnionGetDatabaseDir(databaseName,dir)) < 0)
	{
		UnionUserErrLog("in UnionConnectDatabase:: UnionGetDatabaseDir [%s]!\n",databaseName);
		return(ret);
	}
	strcpy(gunionDatabaseName,dir);
	gunionIsDatabaseConnected = 1;
	return(0);
}

/*
功能：断开与数据库的连接
输入参数：
	无
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionDisconnectDatabase()
{
	memset(gunionDatabaseName,0,sizeof(gunionDatabaseName));
	gunionIsDatabaseConnected = 0;
	return(0);
}
