//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionArrayDefFile_
#define _unionArrayDefFile_

#include "unionArrayDef.h"

/* 从缺省文件读指定名称的数组的类型定义
输入参数
	nameOfType	数组类型的名称
输出参数
	typeNameOfArray	数组类型的类型
返回值：
	>=0 	成功
	<0	出错代码	
	
*/
int UnionReadTypeOfArrayDefFromDefaultDefFile(char *nameOfType,char *typeNameOfArray);

/* 从缺省定义文件读指定名称的数组的定义
输入参数
	nameOfType	变量类型
输出参数
	pdef	读出的数组定义
返回值：
	>=0 	读出的数组的大小
	<0	出错代码	
	
*/
int UnionReadArrayDefFromDefaultDefFile(char *nameOfType,PUnionArrayDef pdef);

/* 从指定文件读指定名称的数组的定义
输入参数
	fileName	文件名称
输出参数
	pdef	读出的数组定义
返回值：
	>=0 	读出的数组的大小
	<0	出错代码	
	
*/
int UnionReadArrayDefFromSpecFile(char *fileName,PUnionArrayDef pdef);

#endif
