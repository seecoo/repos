// wolfang wang
// 2008/10/3

#ifndef _unionVariableDef_
#define _unionVariableDef_

#define conVariableDefTagDefaultValue	"defaultValue"

#include "unionVarDef.h"

// 定义全局变量
typedef struct
{
	TUnionVarDef		varDef;
	char			defaultValue[128+1];	// 缺省值
} TUnionVariableDef;
typedef TUnionVariableDef	*PUnionVariableDef;

/*
功能	
	从一个定义口串中读取一个全局变量类型定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输出参数
	pdef		全局变量定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadVariableDefFromStr(char *str,int lenOfStr,PUnionVariableDef pdef);

/* 将指定类型标识的定义打印到文件中
输入参数
	pdef	类型标识定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintVariableDefToFp(PUnionVariableDef pdef,FILE *fp);

/* 将指定文件中定义的类型标识打印到文件中
输入参数
	fileName	文件名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintVariableDefInFileToFp(char *fileName,FILE *fp);
	
/* 将指定文件中定义的类型标识打印到屏幕上
输入参数
	pdef	类型标识定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputVariableDefInFile(char *fileName);

/* 将指定名称的全局变量定义输出到文件中
输入参数
	nameOfVariable	全局变量名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSpecVariableDefToFp(char *nameOfVariable,FILE *fp);
	
/* 将指定文件中定义的全局变量打印到屏幕上
输入参数
	nameOfVariable	全局变量名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputSpecVariableDef(char *nameOfVariable);

#endif
