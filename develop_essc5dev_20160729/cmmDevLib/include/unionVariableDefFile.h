// wolfang wang
// 2008/10/3

#ifndef _unionVariableDefFile_
#define _unionVariableDefFile_

#include "unionVarDef.h"
#include "unionVariableDef.h"

/* 从指定文件读指定名称的类型标识的定义
输入参数
	fileName	文件名称
输出参数
	pdef	读出的类型标识定义
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionReadVariableDefFromSpecFile(char *fileName,PUnionVariableDef pdef);

/* 从缺省定义文件读指定名称的全局变量的定义
输入参数
	nameOfVariable	全局变量类型
输出参数
	pdef	读出的全局变量定义
返回值：
	>=0 	读出的全局变量的大小
	<0	出错代码	
	
*/
int UnionReadVariableDefFromDefaultDefFile(char *nameOfVariable,PUnionVariableDef pdef);

#endif
