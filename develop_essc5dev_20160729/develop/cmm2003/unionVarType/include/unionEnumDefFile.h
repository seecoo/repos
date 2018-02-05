//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionEnumDefFile_
#define _unionEnumDefFile_

#include "unionEnumDef.h"

/* 从缺省定义文件读指定名称的枚举的定义
输入参数
	nameOfType	枚举类型
输出参数
	pdef	读出的枚举定义
返回值：
	>=0 	读出的枚举的大小
	<0	出错代码	
	
*/
int UnionReadEnumDefFromDefaultDefFile(char *nameOfType,PUnionEnumDef pdef);

/* 从指定文件读指定名称的枚举的定义
输入参数
	fileName	文件名称
输出参数
	pdef	读出的枚举定义
返回值：
	>=0 	读出的枚举的大小
	<0	出错代码	
	
*/
int UnionReadEnumDefFromSpecFile(char *fileName,PUnionEnumDef pdef);

#endif
