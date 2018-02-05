//	Author: Wolfgang Wang
//	Date: 2008/3/6

#ifndef _unionComplexDBObjectFileName_
#define _unionComplexDBObjectFileName_

#include "unionComplexDBCommon.h"

/* **** 第一部分	对象的创建	目录及文件函数 **** */

// 功能：获得一个插入操作执行前的触发器操作定义
/*
输入参数：
	idOfObject	对象的名称
输出参数：
	fileName	文件名称
返回值：
	>=0，成功
	<0，错误码
*/
int UnionGetObjectInsertBeforeTriggerFileName(TUnionIDOfObject idOfObject,char *fileName);

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
int UnionGetObjectDefCreateFileName(TUnionIDOfObject idOfObject,char *fileName);

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
int UnionGetObjectDefDirName(TUnionIDOfObject idOfObject,char *objectDir);

// 功能：获得一个对象的定义文件名称
/*
输入参数：
	idOfObject	对象的名称
输出参数：
	defFileName	对象的定义文件名称
返回值：
	>=0，成功
	<0，错误码
*/
int UnionGetObjectDefFileName(TUnionIDOfObject idOfObject,char *defFileName);

// 功能：获得一个对象的外部关键字定义清单文件名称
/*
输入参数：
	idOfObject	对象的名称
输出参数：
	defFileName	对象的外部关键字定义清单文件名称
返回值：
	>=0，成功
	<0，错误码
*/
int UnionGetObjectChildrenDefFileName(TUnionIDOfObject idOfObject,char *defFileName);

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
int UnionGetAllObjectRecDirName(char *objectDir);

// 功能：获得所有对象记录的存储目录
/*
输入参数：
	idOfObject	对象的名称
输出参数：
	objectDir	对象记录的存储目录
返回值：
	>=0，成功
	<0，错误码
*/
int UnionGetAllObjectRecDirName(char *objectDir);

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
int UnionGetObjectRecMainDirName(TUnionIDOfObject idOfObject,char *objectDir);

// 功能：获得一个对象的键值索引文件名称
/* 输入参数：
	idOfObject	对象的名称
输出参数：
	objectDir	键值索引文件名称
返回值：
	>=0，成功
	<0，错误码
*/
int UnionGetObjecRecPrimaryKeyIndexFileName(TUnionIDOfObject idOfObject,char *fileName);

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
int UnionGetObjectRecUniqueKeyIndexFileName(TUnionIDOfObject idOfObject,char *fldGrpName,char *fileName);

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
int UnionGetObjectRecDirName(TUnionIDOfObject idOfObject,char *primaryKey,char *dir);

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
int UnionGetObjectRecValueFileName(TUnionIDOfObject idOfObject,char *primaryKey,char *fileName);

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
int UnionGetObjectRecParentsListFileName(TUnionIDOfObject idOfObject,char *primaryKey,char *fileName);

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
int UnionGetObjectRecChildListFileName(TUnionIDOfObject idOfObject,char *primaryKey,char *foreignObjectName,char *fileName);

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
int UnionFilterMainWorkingDirStr(char *absolutePath, char *relativelyPath);

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
int UnionPatchMainDirToObjectValueFileName(char *valueFileName, char *fullValueFileName);

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
int UnionGetMainObjectDir(char *mainDir);

// 功能：获得一个触发器操作定义文件，并判断文件是否存在
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
int UnionGetObjectTriggerFileName(TUnionIDOfObject idOfObject,char *operationTag,char *beforeOrAfter,char *fileName);

#endif
