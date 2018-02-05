// wolfang wang
// 2008/10/3

#ifndef _unionVarTypeDefFile_
#define _unionVarTypeDefFile_

#include "unionVarTypeDef.h"

/*
功能	
	获得一个关键字所在的程序名称
输入参数
	keyWord		函数名称
输出参数
	programName	程序名称
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGetProgramFileNameOfKeyWord(char *keyWord,char *nameOfProgram);

/* 判断是否存在一个类型的定义
输入参数
	fileName	文件名称
	nameOfType	指定的类型
输出参数
	无
返回值：
	>=1 	存在
	0	不存在	
	<0	出错
	
*/
int UnionExistsVarTypeDefInSpecFile(char *fileName,char *nameOfType);

/* 判断缺省定义文件中是否存在一个类型的定义
输入参数
	nameOfType	指定的类型
输出参数
	无
返回值：
	>=1 	存在
	0	不存在	
	<0	出错
	
*/
int UnionExistsVarTypeDefInDefaultDefFile(char *nameOfType);

/* 从指定文件读指定名称的类型标识的定义
输入参数
	fileName	文件名称
	nameOfType	指定的类型
输出参数
	pdef	读出的类型标识定义
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionReadVarTypeDefFromSpecFile(char *fileName,char *nameOfType,PUnionVarTypeDef pdef);

/* 从缺省文件读指定名称的类型标识的定义
输入参数
	nameOfType	指定的类型
输出参数
	pdef	读出的类型标识定义
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionReadVarTypeDefFromDefaultFile(char *nameOfType,PUnionVarTypeDef pdef);

/* 将类型标识的定义写入到指定的文件中
输入参数
	pdef		类型标识定义
	fileName	文件名称
输出参数
	无
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionWriteVarTypeDefIntoSpecFile(PUnionVarTypeDef pdef,char *fileName);

/* 将类型标识的定义写入到缺省的文件中
输入参数
	pdef		类型标识定义
	fileName	文件名称
输出参数
	无
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionWriteVarTypeDefIntoDefaultFile(PUnionVarTypeDef pdef);

#endif
