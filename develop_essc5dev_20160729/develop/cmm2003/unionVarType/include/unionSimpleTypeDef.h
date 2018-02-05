//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionSimpleTypeDef_
#define _unionSimpleTypeDef_

#include "unionSimpleTypeDef.h"

#define conSimpleTypeDefTagVarName	"name"
#define conSimpleTypeDefTagNameOfType	"nameOfType"
#define conSimpleTypeDefTagRemark	"remark"

// 定义简单类型的定义
typedef struct
{
	char		name[64+1];		// 简单类型类型的名称
	char		nameOfType[64+1];	// 简单类型的类型
	char		remark[80+1];
} TUnionSimpleTypeDef;
typedef TUnionSimpleTypeDef	*PUnionSimpleTypeDef;

/*
功能	
	获得指定简单类型对应的c语言的类型
输入参数
	nameOfType	变量类型
输入出数
	无
返回值
	奕量类型的内部标识
*/
int UnionGetFinalTypeTagOfSpecNameOfSimpleType(char *nameOfType);

/*
功能	
	获得指定简单类型对应的最终类型
输入参数
	oriNameOfType	变量类型
输入出数
	finalNameOfType	变量最终类型
返回值
	>= 0		成功
	<0		错误代码
*/
int UnionGetFinalTypeNameOfSpecNameOfSimpleType(char *oriNameOfType,char *finalNameOfType);

/* 将指定简单类型的定义打印到文件中
输入参数
	pdef	简单类型定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSimpleTypeDefToFp(PUnionSimpleTypeDef pdef,FILE *fp);

/* 将指定文件中定义的简单类型打印到文件中
输入参数
	fileName	文件名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSimpleTypeDefInFileToFp(char *fileName,FILE *fp);
	
/* 将指定文件中定义的简单类型打印到屏幕上
输入参数
	pdef	简单类型定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputSimpleTypeDefInFile(char *fileName);

/* 将指定名称的简单类型定义输出到文件中
输入参数
	nameOfType	简单类型名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSpecSimpleTypeDefToFp(char *nameOfType,FILE *fp);

/* 将指定简单类型的定义以定义格式打印到文件中
输入参数
	pdef	简单类型定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSimpleTypeDefToFpInDefFormat(PUnionSimpleTypeDef pdef,FILE *fp);
	
/* 将指定文件中定义的简单类型打印到屏幕上
输入参数
	nameOfType	简单类型名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputSpecSimpleTypeDef(char *nameOfType);

#endif
