// wolfang wang
// 2008/10/3

#ifndef _unionProgramDefFile_
#define _unionProgramDefFile_

#include "unionProgramDef.h"

/* 从指定文件读指定名称的类型标识的定义
输入参数
	fileName	文件名称
nameOfPrograme	指定的类型
输出参数
	pdef	读出的类型标识定义
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionReadProgramDefFromSpecFile(char *fileName,char *nameOfProgram,PUnionProgramDef pdef);

/* 从缺省文件读指定名称的类型标识的定义
输入参数
nameOfPrograme	指定的类型
输出参数
	pdef	读出的类型标识定义
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionReadProgramDefFromDefaultFile(char *nameOfProgram,PUnionProgramDef pdef);

#endif
