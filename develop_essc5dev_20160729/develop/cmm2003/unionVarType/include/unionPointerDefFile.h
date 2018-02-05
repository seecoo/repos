//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionPointerDefFile_
#define _unionPointerDefFile_

#include "unionPointerDef.h"

/* 从缺省文件读指定名称的指针的定义
输入参数
	nameOfType	指针类型的名称
输出参数
	pdef	读出的指针定义
返回值：
	>=0 	读出的指针的大小
	<0	出错代码	
	
*/
int UnionReadPointerDefFromDefaultDefFile(char *nameOfType,PUnionPointerDef pdef);

/* 从指定文件读指定名称的指针的定义
输入参数
	fileName	文件名称
输出参数
	pdef	读出的指针定义
返回值：
	>=0 	读出的指针的大小
	<0	出错代码	
	
*/
int UnionReadPointerDefFromSpecFile(char *fileName,PUnionPointerDef pdef);

/* 将指针类型的定义写入到指定的文件中
输入参数
	pdef		指针类型定义
	fileName	文件名称
输出参数
	无
返回值：
	>=0 	读出的指针类型的大小
	<0	出错代码	
	
*/
int UnionWritePointerDefIntoSpecFile(PUnionPointerDef pdef,char *fileName);

/* 将指针类型的定义写入到缺省的文件中
输入参数
	pdef		指针类型定义
	programFileName	指针类型所在的程序名称
输出参数
	无
返回值：
	>=0 	读出的指针类型的大小
	<0	出错代码	
	
*/
int UnionWritePointerDefIntoDefaultFile(PUnionPointerDef pdef,char *programFileName);

/* 将指针类型的定义写入到缺省的文件中
输入参数
	nameOfType	指针类型定义
	name		类型名称
	remark		说明
	programFileName	指针类型所在的程序名称
输出参数
	无
返回值：
	>=0 	读出的指针类型的大小
	<0	出错代码	
	
*/
int UnionWriteSpecPointerDefIntoDefaultFile(char *nameOfType,char *name,char *remark,char *programFileName);

#endif
