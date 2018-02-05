// wolfang wang
// 2008/10/3

#ifndef _unionConstDefFile_
#define _unionConstDefFile_

#include "unionConstDef.h"

/*
功能	
	从缺省定义文件读取常量的值
输入参数
	constName	常量名称
	previousName	constName对应的值
输出参数
	constValue	常量值
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadConstValueFromDefaultDefFile(char *constName,char *previousName,char *constValue);

/* 从指定文件读指定名称的类型标识的定义
输入参数
	fileName	文件名称
输出参数
	pdef	读出的类型标识定义
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionReadConstDefFromSpecFile(char *fileName,PUnionConstDef pdef);

#endif
