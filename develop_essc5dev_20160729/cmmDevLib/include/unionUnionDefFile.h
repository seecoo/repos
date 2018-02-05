//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionUnionDefFile_
#define _unionUnionDefFile_

#include "unionVarDef.h"
#include "unionUnionDef.h"

/* 从缺省定义文件读指定名称的结构的定义
输入参数
	nameOfType	结构类型
输出参数
	punionDef	读出的结构定义
返回值：
	>=0 	读出的结构的大小
	<0	出错代码	
	
*/
int UnionReadUnionDefFromDefaultDefFile(char *nameOfType,PUnionUnionDef punionDef);

/* 从指定文件读指定名称的结构的定义
输入参数
	fileName	文件名称
输出参数
	punionDef	读出的结构定义
返回值：
	>=0 	读出的结构的大小
	<0	出错代码	
	
*/
int UnionReadUnionDefFromSpecFile(char *fileName,PUnionUnionDef punionDef);


#endif
