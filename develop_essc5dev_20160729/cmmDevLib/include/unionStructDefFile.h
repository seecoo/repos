//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionStructDefFile_
#define _unionStructDefFile_

#include "unionVarDef.h"
#include "unionStructDef.h"

/* 从缺省定义文件读指定名称的结构的定义
输入参数
	nameOfType	结构类型
输出参数
	pstructDef	读出的结构定义
返回值：
	>=0 	读出的结构的大小
	<0	出错代码	
	
*/
int UnionReadStructDefFromDefaultDefFile(char *nameOfType,PUnionStructDef pstructDef);

/* 从指定文件读指定名称的结构的定义
输入参数
	fileName	文件名称
输出参数
	pstructDef	读出的结构定义
返回值：
	>=0 	读出的结构的大小
	<0	出错代码	
	
*/
int UnionReadStructDefFromSpecFile(char *fileName,PUnionStructDef pstructDef);

/* 将结构的定义写入到指定的文件中
输入参数
	pdef		结构定义
	fileName	文件名称
输出参数
	无
返回值：
	>=0 	读出的结构的大小
	<0	出错代码	
	
*/
int UnionWriteStructDefIntoSpecFile(PUnionStructDef pdef,char *fileName);

/* 将结构的定义写入到缺省的文件中
输入参数
	pdef		结构定义
	programFileName	结构所在的程序名称
输出参数
	无
返回值：
	>=0 	读出的结构的大小
	<0	出错代码	
	
*/
int UnionWriteStructDefIntoDefaultFile(PUnionStructDef pdef,char *programFileName);

#endif
