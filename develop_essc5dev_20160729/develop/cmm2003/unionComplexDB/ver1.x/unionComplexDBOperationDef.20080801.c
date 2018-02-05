// 2008/7/26
// Wolfang Wang

#ifdef _WIN32
#include <vcl.h>
#include <windows.h>
#include "dir.h"
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "unionComplexDBOperationDef.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "UnionLog.h"			// 使用3.x版本

/*
功能	
	从一个定义口串中读取一个数据库操作定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
	sizeOfTblName	表名缓冲的大小
	sizeOfCondition	操作条件缓冲的大小
输入出数
	tblName		表名
	operationID	操作标识
	condition	操作条件
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadComplexDBOperationDefFromStr(char *str,int lenOfStr,char *tblName,int sizeOfTblName,int *operationID,char *condition,int sizeOfCondition)
{
	return(UnionReadComplexDBSelectOperationDefFromStr(str,lenOfStr,tblName,sizeOfTblName,operationID,condition,sizeOfCondition,NULL,0));
}

/*
功能	
	从一个定义口串中读取一个数据库选择操作定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
	sizeOfTblName	表名缓冲的大小
	sizeOfOperationDef	操作条件缓冲的大小
	sizeOfFldList	域清单缓冲的大小
输入出数
	tblName		表名
	operationID	操作标识
	condition	操作条件
	fldList		要选择的域清单
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadComplexDBSelectOperationDefFromStr(char *str,int lenOfStr,char *tblName,int sizeOfTblName,int *operationID,char *condition,int sizeOfOperationDef,char *fldList,int sizeOfFldList)
{
	int			ret;
	int			index;
	char			var[128];
	
	if (str == NULL)
	{
		UnionUserErrLog("in UnionReadComplexDBSelectOperationDefFromStr:: null parameter!\n");
		return(errCodeParameter);
	}
	
	// 表名
	if (tblName != NULL)
	{
		if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,"tableName",tblName,sizeOfTblName)) < 0)
		{
			UnionUserErrLog("in UnionReadComplexDBSelectOperationDefFromStr:: UnionReadRecFldFromRecStr [%s]!\n","tableName");
			return(ret);
		}
	}
	// 操作标识
	if (operationID != NULL)
	{
		if ((ret = UnionReadIntTypeRecFldFromRecStr(str,lenOfStr,"operationID",operationID)) < 0)
		{
			UnionUserErrLog("in UnionReadComplexDBSelectOperationDefFromStr:: UnionReadIntTypeRecFldFromRecStr [%s]!\n","operationID");
			return(ret);
		}
	}
	// 操作条件
	if (condition != NULL)
	{
		if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,"condition",condition,sizeOfOperationDef)) < 0)
		{
			UnionUserErrLog("in UnionReadComplexDBSelectOperationDefFromStr:: UnionReadRecFldFromRecStr [%s]!\n","condition");
			return(ret);
		}
	}
	// 域清单
	if (fldList != NULL)
	{
		if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,"fldList",fldList,sizeOfFldList)) < 0)
		{
			UnionUserErrLog("in UnionReadComplexDBSelectOperationDefFromStr:: UnionReadRecFldFromRecStr [%s]!\n","fldList");
			return(ret);
		}
	}
	return(0);
}

