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
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionWorkingDir.h"
#include "unionErrCode.h"

#include "database.h"
#include "recInfo.h"

// 说明：本文件中的程序基于文件数据库的操作

// 获得记录信息文件
int UnionGetFileNameOfRecInfo(char *tableName,char *fileName)
{
        sprintf(fileName,"%s/%s/rec.info",UnionGetCurrentDatabaseName(),tableName);
        return(0);
}

/*
功能：创建一个表的记录信息文件，文件中只有两行，第一行为“0”，第2行为表的关键字
输入参数：
	tableName：对应的表名
	keyStr：表的关键字域名，如果关键字由多个域组成，则各个域之间用'|'分隔
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionCreateRecInfo(char *tableName,char *keyStr)
{
	char	fileName[512];
	int	lenOfKey;
	char	tmpBuf[512+1];
	char	tmpKey[512+1];
        int     ret;
		
	if (tableName == NULL || keyStr == NULL)
	{
		UnionUserErrLog("in UnionCreateRecInfo:: parameters null pointer!\n");
		return(errCodeParameter);
	}

	// 获得文件名
	memset(fileName,0,sizeof(fileName));
        if ((ret = UnionGetFileNameOfRecInfo(tableName,fileName)) < 0)
	{
		UnionUserErrLog("in UnionCreateRecInfo:: UnionGetFileNameOfRecInfo [%s]\n",tableName);
		return(ret);
	}
	// 写记录数目与关键字
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionPutRecFldIntoRecStr("recNum","0",1,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionCreateRecInfo:: UnionPutRecFldIntoRecStr recNum of [%s]\n",tableName);
		return(ret);
	}
        if ((ret = UnionAppendRecStrToFile(fileName,tmpBuf,strlen(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionCreateRecInfo:: UnionAppendRecStrToFile recNum to [%s]\n",tableName);
		return(ret);
	}
	// 写关键字
	memset(tmpKey,0,sizeof(tmpKey));
	if ((ret = UnionConvertOneFldSeperatorInRecStrIntoAnother(keyStr,strlen(keyStr),'|',',',tmpKey,sizeof(tmpKey))) < 0)
	{
		UnionUserErrLog("in UnionCreateRecInfo:: UnionConvertOneFldSeperatorInRecStrIntoAnother! key = [%s] for [%s]\n",keyStr,tableName);
		return(ret);
	}
	if ((ret = UnionPutRecFldIntoRecStr("primaryKey",tmpKey,strlen(tmpKey),tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionCreateRecInfo:: UnionPutRecFldIntoRecStr key [%s] of [%s]\n",tmpKey,tableName);
		return(ret);
	}
        if ((ret = UnionAppendRecStrToFile(fileName,tmpBuf,strlen(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionCreateRecInfo:: UnionAppendRecStrToFile primaryKey to [%s]\n",tableName);
		return(ret);
	}
	return(ret);
}

/*
功能：在记录信息文件中增加一条记录的记录文件名称，同时修改记录总数
输入参数：
	tableName：对应的表名
	recFileName：要增加的记录文件名称
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionAddToRecInfo(char *tableName,char *recFileName)
{
	char	fileName[512];
        int     ret;
	
	if (tableName == NULL || recFileName == NULL)
	{
		UnionUserErrLog("in UnionAddToRecInfo:: parameters null pointer!\n");
		return(errCodeParameter);
	}
	
	memset(fileName,0,sizeof(fileName));
        if ((ret = UnionGetFileNameOfRecInfo(tableName,fileName)) < 0)
	{
		UnionUserErrLog("in UnionAddToRecInfo:: UnionGetFileNameOfRecInfo [%s]\n",tableName);
		return(ret);
	}
	return(UnionAppendRecStrToFile(fileName,recFileName,strlen(recFileName)));
}

/*
功能：从记录信息文件中删除一条记录的记录文件名称，同时修改记录总数
输入参数：
	tableName：对应的表名
	recFileName：要删除的记录文件名称
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionDeleteFromRecInfo(char *tableName,char *recFileName)
{
	char	fileName[512];
        int     ret;
	
	if (tableName == NULL || recFileName == NULL)
	{
		UnionUserErrLog("in UnionDeleteFromRecInfo:: parameters null pointer!\n");
		return(errCodeParameter);
	}
	
	memset(fileName,0,sizeof(fileName));
        if ((ret = UnionGetFileNameOfRecInfo(tableName,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteFromRecInfo:: UnionGetFileNameOfRecInfo [%s]\n",tableName);
		return(ret);
	}
	return(UnionDeleteRecStrFromFile(fileName,recFileName,strlen(recFileName)));
}

/*
功能：从记录信息文件获取一张表的记录总数
输入参数：
	tableName：对应的表名
输出参数：
	无
返回值：
	>=0：成功，返回表的记录总数
	<0：失败
*/
int UnionGetTableRecordNumber(char *tableName)
{
	char	fileName[512+1];
        int     ret;

	if (tableName == NULL)
	{
		UnionUserErrLog("in UnionGetTableRecordNumber:: parameters null pointer!\n");
		return(errCodeParameter);
	}

	memset(fileName,0,sizeof(fileName));
        if ((ret = UnionGetFileNameOfRecInfo(tableName,fileName)) < 0)
	{
		UnionUserErrLog("in UnionGetTableRecordNumber:: UnionGetFileNameOfRecInfo [%s]\n",tableName);
		return(ret);
	}
	return(UnionGetTotalRecNumInFile(fileName)-2);
}

/*
功能：从记录信息文件获取一张表的记录总数和关键字
输入参数：
	tableName：对应的表名
输出参数：
	primaryKey      关键字
返回值：
	>=0：成功，返回表的记录总数
	<0：失败
*/
int UnionGetTableRecInfo(char *tableName,char *primaryKey,int sizeOfPrimaryKey)
{
	int	ret;
	int	recNum;

	if ((recNum = UnionGetTableRecordNumber(tableName)) < 0)
	{
		UnionUserErrLog("in UnionGetTableRecInfo:: UnionGetTableRecordNumber [%s]!\n",tableName);
		return(recNum);
	}
	if ((ret = UnionGetPrimaryKeyFromTableRecInfo(tableName,primaryKey,sizeOfPrimaryKey)) < 0)
	{
		UnionUserErrLog("in UnionGetTableRecInfo:: UnionGetPrimaryKeyFromTableRecInfo [%s]!\n",tableName);
		return(ret);
	}
	return(recNum);
}

/*
功能：从记录信息文件获取一张表的记录总数和关键字
输入参数：
	tableName：对应的表名
输出参数：
	primaryKey      关键字
返回值：
	>=0：		关键字的长度
	<0：失败
*/
int UnionGetPrimaryKeyFromTableRecInfo(char *tableName,char *primaryKey,int sizeOfPrimaryKey)
{
	char	fileName[512+1];
	char	recStr[512+1];
	int	lenOfRecStr;
        int     ret;

	if ((tableName == NULL) || (primaryKey == NULL))
	{
		UnionUserErrLog("in UnionGetPrimaryKeyFromTableRecInfo:: parameters null pointer!\n");
		return(errCodeParameter);
	}

	memset(fileName,0,sizeof(fileName));
        if ((ret = UnionGetFileNameOfRecInfo(tableName,fileName)) < 0)
	{
		UnionUserErrLog("in UnionGetPrimaryKeyFromTableRecInfo:: UnionGetFileNameOfRecInfo [%s]\n",tableName);
		return(ret);
	}
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionReadRecOfIndexDirectlyFromFile(fileName,1,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionGetTableRecInfo:: UnionReadRecOfIndexDirectlyFromFile [%s] index = 1\n",tableName);
		return(lenOfRecStr);
	}
	memset(primaryKey,0,sizeOfPrimaryKey);
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"primaryKey",primaryKey,sizeOfPrimaryKey)) < 0)
	{
		UnionUserErrLog("in UnionGetTableRecInfo:: UnionReadRecFldFromRecStr [primaryKey] from [%s] of [%s]\n",recStr,tableName);
		return(ret);
	}
	UnionConvertOneFldSeperatorInRecStrIntoAnother(primaryKey,ret,',','|',primaryKey,sizeOfPrimaryKey);
	return(ret);
}
