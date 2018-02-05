//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionReadStructFromCFile_
#define _unionReadStructFromCFile_

// 判断一个名字是否是*
int UnionIsThisNameDefinedAsStarTag(char *varName);

/* 从一个字符串中读取变量的定义
输入参数
	str		域定义串
	lenOfStr	域定义串的长度
输出参数
	prefix		读出的前缀
	nameOfType	读出的定义的类型
	varName		读出的变量的名字
	isPointer	读出的变量是否是指针
返回值：
	>=0 		读出的串在字符串中占用的大小
	<0		出错代码	
	
*/
int UnionReadVarDefFromCProgramStr(char *str,int lenOfStr,char *prefix,char *nameOfType,char *varName,int *isPointer);

/* 从一个字符串中读取结构域的定义
输入参数
	str		域定义串
	lenOfStr	域定义串的长度
输出参数
	pfldDef		读出的域定义
返回值：
	>=0 		成功
	<0		出错代码	
	
*/
int UnionReadStructFldDefFromStr(char *str,int lenOfStr,PUnionVarDef pfldDef);

/* 从指定文件读指定名称的结构的定义
输入参数
	fileName	文件名称
	structName	结构名称
	writeToDefFile	控制写到定义文件中，1写，0，不写
	outFp		将读出的记录输出到该文件中
输出参数
	pdef	读出的结构定义
返回值：
	>=0 	读出的结构的大小
	<0	出错代码	
	
*/
int UnionReadSpecStructDefFromCHeaderFile(char *fileName,char *structName,PUnionStructDef pdef,int writeToDefFile,FILE *outFp);

/* 将指定结构的域定义打印到文件中
输入参数
	fileName	文件名称
	structName	结构名称
	fp		文件句柄
	writeToDefFile	控制写到定义文件中，1写，0，不写
输出参数
	无
返回值：
	>=0 	成功
	<0	出错代码	
	
*/
int UnionPrintSpecStructDefInCHeaderFileToFp(char *fileName,char *nameOfStructDef,FILE *fp,int writeToDefFile);

/* 将指定结构的域定义打印到文件中
输入参数
	fileName	文件名称
	fp		文件句柄
输出参数
	无
返回值：
	>=0 	成功
	<0	出错代码	
	
*/
int UnionPrintUserSpecStructDefInCHeaderFileToFp(char *fileName,FILE *fp);

/* 从指定文件读指定名称的结构的定义
输入参数
	fileName	文件名称
	readName	结构名称
	writeToDefFile	控制写到定义文件中，1写，0，不写
	outFp		将读出的记录输出到该文件中
输出参数
	pdef	读出的结构定义
返回值：
	>=0 	读出的结构的大小
	<0	出错代码
*/
int UnionReadSpecTypeDefFromCHeaderFile(char *fileName,char *readName,int writeToDefFile,FILE *outFp);

/* 将指定的类型定义打印到文件中
输入参数
	fileName	文件名称
	nameOfDef	结构名称
	fp		文件句柄
	writeToDefFile	控制写到定义文件中，1写，0，不写
输出参数
	无
返回值：
	>=0 	成功
	<0	出错代码	
	
*/
int UnionPrintSpecTypeDefInCHeaderFileToFp(char *fileName,char *nameOfDef,FILE *fp,int writeToDefFile);

#endif
