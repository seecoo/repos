//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionSimpleTypeDefFile_
#define _unionSimpleTypeDefFile_

#include "unionSimpleTypeDef.h"

/* 从指定文件读指定名称的简单类型的定义
输入参数
	fileName	文件名称
输出参数
	pdef	读出的简单类型定义
返回值：
	>=0 	读出的简单类型的大小
	<0	出错代码	
	
*/
int UnionReadSimpleTypeDefFromSpecFile(char *fileName,PUnionSimpleTypeDef pdef);

/* 从缺省定义文件读指定名称的简单类型的定义
输入参数
	nameOfType	结构类型
输出参数
	pdef	读出的结构定义
返回值：
	>=0 	读出的结构的大小
	<0	出错代码	
	
*/
int UnionReadSimpleTypeDefFromDefaultDefFile(char *nameOfType,PUnionSimpleTypeDef pdef);

/* 将简单类型的定义写入到指定的文件中
输入参数
	pdef		简单类型定义
	fileName	文件名称
输出参数
	无
返回值：
	>=0 	读出的简单类型的大小
	<0	出错代码	
	
*/
int UnionWriteSimpleTypeDefIntoSpecFile(PUnionSimpleTypeDef pdef,char *fileName);

/* 将简单类型的定义写入到缺省的文件中
输入参数
	pdef		简单类型定义
	programFileName	简单类型所在的程序名称
输出参数
	无
返回值：
	>=0 	读出的简单类型的大小
	<0	出错代码	
	
*/
int UnionWriteSimpleTypeDefIntoDefaultFile(PUnionSimpleTypeDef pdef,char *programFileName);

/* 将简单类型的定义写入到缺省的文件中
输入参数
	nameOfType	简单类型定义
	name		类型名称
	remark		说明
	programFileName	简单类型所在的程序名称
输出参数
	无
返回值：
	>=0 	读出的简单类型的大小
	<0	出错代码	
	
*/
int UnionWriteSpecSimpleTypeDefIntoDefaultFile(char *nameOfType,char *name,char *remark,char *programFileName);


#endif
