// wolfang wang
// 2008/10/3

#ifndef _unionFunDefFile_
#define _unionFunDefFile_

#include "unionFunDef.h"

/*
功能	
	从缺省定义文件读取一个函数定义
输入参数
	funName		函数名称
输出参数
	pdef		读出的函数定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadFunDefFromDefaultDefFile(char *funName,PUnionFunDef pdef);

/*
功能	
	从文件读取一个函数定义
输入参数
	fileName	文件名称
输出参数
	pdef		读出的函数定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadFunDefFromSpecFile(char *fileName,PUnionFunDef pdef);

#endif
