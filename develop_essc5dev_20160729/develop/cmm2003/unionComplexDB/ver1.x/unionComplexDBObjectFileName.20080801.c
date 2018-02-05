//	Author: Wolfgang Wang
//	Date: 2008/3/6

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#include "unionREC.h"
#else
#include <windows.h>
#include <vcl.h>
#include <Filectrl.hpp>

#include "unionWorkingDir.h"
#endif

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionRecFile.h"
#include "unionComplexDBObjectFileName.h"

const char OBJECTDIR[64] = "object";

// 功能：获得对象的主定义目录
/*
输入参数：
	无
输出参数：
	mainDir		对象的主定义目录
返回值：
	>=0，成功
	<0，错误码
*/
int UnionGetMainObjectDir(char *mainDir)
{
#ifndef _WIN32
	char *ptr;
	if ((ptr = getenv("UNIONOBJECTDIR")) == NULL)
	{
		if ((ptr = UnionReadStringTypeRECVar("unionObjectDir")) == NULL)
		{
			sprintf(mainDir,"%s/object",getenv("UNIONREC"));
			return(0);
		}
	}
	sprintf(mainDir,"%s",ptr);
	//UnionLog("in UnionGetMainObjectDir:: mainDir = [%s]\n",mainDir);
#else
	char	tmpBuf[512];
	memset(tmpBuf,0,sizeof(tmpBuf));
 	UnionGetMainWorkingDir(tmpBuf);
        sprintf(mainDir,"%s/%s",tmpBuf, OBJECTDIR);
#endif
	return(0);
}

// 功能：获得一个插入操作执行前的触发器操作定义，并判断文件是否存在
/*
输入参数：
	idOfObject	对象的名称
	operationTag	操作标识
	beforeOrAfter	是操作之前执行还是之后执行
输出参数：
	fileName	文件名称
返回值：
	>0，文件存在
	=0，文件不存在
	<0，错误码
*/
int UnionGetObjectTriggerFileName(TUnionIDOfObject idOfObject,char *operationTag,char *beforeOrAfter,char *fileName)
{
        char	mainDir[512+1];

	if ((idOfObject == NULL) || (operationTag == NULL) || (beforeOrAfter == NULL) || (fileName == NULL))
		return(0);
        memset(mainDir,0,sizeof(mainDir));
	UnionGetMainObjectDir(mainDir);
        sprintf(fileName,"%s/trigger/%s.%s.%s.sql",mainDir,idOfObject,operationTag,beforeOrAfter);
        
        UnionProgramerLog("in UnionGetObjectTriggerFileName, fileName is: [%s].\n", fileName);
        return(UnionExistsFile(fileName));
}

/* **** 第一部分	对象的创建	目录及文件函数 **** */

// 功能：获得一个创建一个对象的文件名称
/*
输入参数：
	idOfObject	对象的名称
输出参数：
	fileName	对象定义的创建文件名称
返回值：
	>=0，成功
	<0，错误码
*/
int UnionGetObjectDefCreateFileName(TUnionIDOfObject idOfObject,char *fileName)
{
        char	mainDir[512+1];

	if ((idOfObject == NULL) || (fileName == NULL))
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
        memset(mainDir,0,sizeof(mainDir));
#ifndef _WIN32
	UnionGetMainObjectDir(mainDir);
	sprintf(fileName,"%s/sql/%s.sql",mainDir,idOfObject);
#else
	UnionGetMainWorkingDir(mainDir);

        sprintf(fileName,"%s/%s/sql/%s.sql",mainDir, OBJECTDIR, idOfObject);
#endif
	return(0);
}

/* **** 第二部分	对象的定义	目录及文件函数 **** */

// 功能：获得一个对象定义的存储目录
/*
输入参数：
	idOfObject	对象的名称
输出参数：
	objectDir	对象定义的存储目录
返回值：
	>=0，成功
	<0，错误码
*/
int UnionGetObjectDefDirName(TUnionIDOfObject idOfObject,char *objectDir)
{
        char	mainDir[512+1];

	if ((idOfObject == NULL) || (objectDir == NULL))
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
        memset(mainDir,0,sizeof(mainDir));
#ifndef _WIN32
	UnionGetMainObjectDir(mainDir);
	sprintf(objectDir,"%s/objDef",mainDir);
#else
	UnionGetMainWorkingDir(mainDir);

        sprintf(objectDir,"%s/%s/objDef", mainDir, OBJECTDIR);
#endif
	return(0);
}

// 功能：获得一个对象的定义文件名称
/*
输入参数：
	idOfObject	对象的名称
输出参数：
	objectDir	对象的定义文件名称
返回值：
	>=0，成功
	<0，错误码
*/
int UnionGetObjectDefFileName(TUnionIDOfObject idOfObject,char *fileName)
{
	char	dirName[512];
	int	ret;
	
	if (fileName == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	memset(dirName,0,sizeof(dirName));
	if ((ret = UnionGetObjectDefDirName(idOfObject,dirName)) < 0)
		return(ret);
	sprintf(fileName,"%s/%s.def",dirName,idOfObject);
	return(0);
}

// 功能：获得一个对象的外部关键字定义清单文件名称
/*
输入参数：
	idOfObject	对象的名称
输出参数：
	fileName	对象的外部关键字定义清单文件名称
返回值：
	>=0，成功
	<0，错误码
*/
int UnionGetObjectChildrenDefFileName(TUnionIDOfObject idOfObject,char *fileName)
{
	char	dirName[512];
	int	ret;
	
	if (fileName == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	memset(dirName,0,sizeof(dirName));
	if ((ret = UnionGetObjectDefDirName(idOfObject,dirName)) < 0)
		return(ret);
	sprintf(fileName,"%s/%s.child.def",dirName,idOfObject);
	return(0);
}

/* **** 第三部分	对象的记录	目录及文件函数 **** */

// 功能：获得所有对象记录的存储目录
/*
输入参数：
	无
输出参数：
	objectDir	对象记录的存储目录
返回值：
	>=0，成功
	<0，错误码
*/
int UnionGetAllObjectRecDirName(char *objectDir)
{
        char	mainDir[512+1];

	if (objectDir == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
        memset(mainDir,0,sizeof(mainDir));
#ifndef _WIN32
	UnionGetMainObjectDir(mainDir);
	sprintf(objectDir,"%s/objRec",mainDir);
	//UnionLog("in UnionGetAllObjectRecDirName:: objectDir = [%s]\n",objectDir);
#else
	UnionGetMainWorkingDir(mainDir);

        sprintf(objectDir,"%s/%s/objRec", mainDir, OBJECTDIR);
#endif
	return(0);
}

// 功能：获得一个对象记录的存储目录
/*
输入参数：
	idOfObject	对象的名称
输出参数：
	objectDir	对象的存储目录
返回值：
	>=0，成功
	<0，错误码
*/
int UnionGetObjectRecMainDirName(TUnionIDOfObject idOfObject,char *objectDir)
{
        char	mainDir[512+1];

	if ((idOfObject == NULL) || (objectDir == NULL))
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
        memset(mainDir,0,sizeof(mainDir));
#ifndef _WIN32
	UnionGetMainObjectDir(mainDir);
	sprintf(objectDir,"%s/objRec/%s",mainDir,idOfObject);
	//UnionLog("in UnionGetObjectRecMainDirName:: objectDir = [%s]\n",objectDir);
#else
	UnionGetMainWorkingDir(mainDir);

        sprintf(objectDir,"%s/%s/objRec/%s", mainDir, OBJECTDIR, idOfObject);
#endif
	return(0);
}

// 功能：获得一个对象的键值索引文件名称
/* 输入参数：
	idOfObject	对象的名称
输出参数：
	fileName	键值索引文件名称
返回值：
	>=0，成功
	<0，错误码
*/
int UnionGetObjecRecPrimaryKeyIndexFileName(TUnionIDOfObject idOfObject,char *fileName)
{
	char	dirName[512];
	int	ret;
	
	if (fileName == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	memset(dirName,0,sizeof(dirName));
	if ((ret = UnionGetObjectRecMainDirName(idOfObject,dirName)) < 0)
		return(ret);
	sprintf(fileName,"%s/primaryKey",dirName);
	return(0);
}

// 功能：获得一个对象的唯一值文件名称
/*
输入参数：
	idOfObject	对象的名称
	fldGrpName	唯一值域名称
输出参数：
	objectDir	键值索引文件名称
返回值：
	>=0，成功
	<0，错误码
*/
int UnionGetObjectRecUniqueKeyIndexFileName(TUnionIDOfObject idOfObject,char *fldGrpName,char *fileName)
{
	char	dirName[512];
	int	ret;
	
	if (fileName == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	memset(dirName,0,sizeof(dirName));
	if ((ret = UnionGetObjectRecMainDirName(idOfObject,dirName)) < 0)
		return(ret);
	sprintf(fileName,"%s/unique-%s",dirName,fldGrpName);
	return(0);
}

// 功能：获得一个对象实例的存储目录
/*
输入参数：
	idOfObject	对象的名称
	primaryKey	对象实例的关键字
输出参数：
	dir		对象实例的存储目录
返回值：
	>=0，成功
	<0，错误码
*/
int UnionGetObjectRecDirName(TUnionIDOfObject idOfObject,char *primaryKey,char *dir)
{
	char	dirName[512];
	int	ret;
	
	if (dir == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	memset(dirName,0,sizeof(dirName));
	if ((ret = UnionGetObjectRecMainDirName(idOfObject,dirName)) < 0)
		return(ret);
	sprintf(dir,"%s/%s",dirName,primaryKey);
	return(0);
}

// 功能：获得一个对象实例的值文件名称
/*
输入参数：
	idOfObject	对象的名称
	primaryKey	对象实例的关键字
输出参数：
	fileName	对象实例的值文件名称
返回值：
	>=0，成功
	<0，错误码
*/
int UnionGetObjectRecValueFileName(TUnionIDOfObject idOfObject,char *primaryKey,char *fileName)
{
	char	dirName[512];
	int	ret;
	
	if (fileName == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	memset(dirName,0,sizeof(dirName));
	if ((ret = UnionGetObjectRecDirName(idOfObject,primaryKey,dirName)) < 0)
		return(ret);
	sprintf(fileName,"%s/value",dirName);
	return(0);
}

// 功能：获得一个对象实例引用的对象实例清单文件名称
/*
输入参数：
	idOfObject	对象的名称
	primaryKey	对象实例的关键字
输出参数：
	fileName	引用的对象实例清单文件名称
返回值：
	>=0，成功
	<0，错误码
*/
int UnionGetObjectRecParentsListFileName(TUnionIDOfObject idOfObject,char *primaryKey,char *fileName)
{
	char	dirName[512];
	int	ret;
	
	if (fileName == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	memset(dirName,0,sizeof(dirName));
	if ((ret = UnionGetObjectRecDirName(idOfObject,primaryKey,dirName)) < 0)
		return(ret);
	sprintf(fileName,"%s/parents",dirName);
	return(0);
}

// 功能：获得一个使用该对象实例的对象实例清单文件名称
/*
输入参数：
	objectName：本对象的名称
	primaryKey：本对象实例的关键字
	foreignObjectName：外部对象的名称
输出参数：
	fileName	使用该对象实例的对象实例清单文件名称
返回值：
	>=0，成功
	<0，错误码
*/
int UnionGetObjectRecChildListFileName(TUnionIDOfObject idOfObject,char *primaryKey,char *foreignObjectName,char *fileName)
{
	char	dirName[512];
	int	ret;
	
	if (fileName == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	memset(dirName,0,sizeof(dirName));
	if ((ret = UnionGetObjectRecDirName(idOfObject,primaryKey,dirName)) < 0)
		return(ret);
	sprintf(fileName,"%s/child-%s",dirName,foreignObjectName);
	return(0);
}

// 功能：过滤掉 主工作目录 字符串
/*
输入参数：
	absolutePath：绝对路径
输出参数：
	relativelyPath:	相对路径
返回值：
	>=0，成功
	<0，错误码
*/
int UnionFilterMainWorkingDirStr(char *absolutePath, char *relativelyPath)
{
        char	mainDir[512+1];
        char    *pos;

	if ((absolutePath == NULL) || (relativelyPath == NULL))
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
        memset(mainDir,0,sizeof(mainDir));
#ifndef _WIN32
	UnionGetMainObjectDir(mainDir);
        sprintf(mainDir,"%s",mainDir);

        // 判断绝对路径中是否含有  主工作目录 字符串
        pos = strstr(absolutePath, mainDir);
        if((pos - absolutePath) < 0)
                return (UnionSetUserDefinedErrorCode(errCodeParameter));

        sprintf(relativelyPath, "%s/%s", OBJECTDIR, absolutePath + strlen(mainDir) + 1);
#else
	UnionGetMainWorkingDir(mainDir);

        // 判断绝对路径中是否含有  主工作目录 字符串
        pos = strstr(absolutePath, mainDir);
        if((pos - absolutePath) < 0)
                return (UnionSetUserDefinedErrorCode(errCodeParameter));

        sprintf(relativelyPath, "%s", absolutePath + strlen(mainDir) + 1);
#endif
	return(0);
}

// 功能：将值文件的相对路径改为绝对路径
/*
输入参数：
	valueFileName		值文件名称
输出参数：
	fullValueFileName:	值文件绝对路径
返回值：
	>=0，成功
	<0，错误码
*/
int UnionPatchMainDirToObjectValueFileName(char *valueFileName, char *fullValueFileName)
{
        char	mainDir[512+1];
        char	value[512+1];
        char    *pos;

	if ((valueFileName == NULL) || (fullValueFileName == NULL))
		return(UnionSetUserDefinedErrorCode(errCodeParameter));

        memset(mainDir,0,sizeof(mainDir));
#ifndef _WIN32
	UnionGetMainObjectDir(mainDir);
        memset(value,0,sizeof(value));
        UnionFilterDirFromDirStr(valueFileName,strlen(valueFileName),1,value);
        sprintf(fullValueFileName,"%s/%s",mainDir,value);
        //UnionLog("in UnionPatchMainDirToObjectValueFileName:: mainDir = [%s]fullValueFileName = [%s]\n",mainDir,fullValueFileName);
#else
	UnionGetMainWorkingDir(mainDir);
	sprintf(fullValueFileName,"%s/%s",mainDir,valueFileName);
#endif
	return(0);
}	

