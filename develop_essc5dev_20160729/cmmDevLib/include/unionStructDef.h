//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionStructDef_
#define _unionStructDef_

#include "unionVarDef.h"

#define conStructDefTagStructName		"structName"
#define conStructDefTagTypeDefName		"typeDefName"
#define conStructDefTagRemark		"remark"
#define conStructDeclareLineTag		"typeDefName="

// 结构声明定义
typedef struct
{
	char			structName[48+1];	// 结构的名称,struct后跟的名称
	char			typeDefName[48+1];	// 结构的名称,typedef的名称
	char			remark[128+1];		// 说明
} TUnionStructDeclareDef;
typedef TUnionStructDeclareDef	*PUnionStructDeclareDef;

#ifdef _maxNumOfVarPerStructIs512_
#define conMaxNumOfVarPerStruct	512
#else
#define conMaxNumOfVarPerStruct	256
#endif
// 定义结构的定义
typedef struct
{
	TUnionStructDeclareDef	declareDef;				// 结构声明定义
	int			fldNum;					// 结构的域数量
	TUnionVarDef		fldGrp[conMaxNumOfVarPerStruct];	// 域的定义
} TUnionStructDef;
typedef TUnionStructDef		*PUnionStructDef;

/* 从一个字符串中读取结构声明的定义
输入参数
	str		声明定义串
	lenOfStr	声明定义串的长度
输出参数
	pdeclareDef	读出的声明定义
返回值：
	>=0 	读出的声明数目
	<0	出错代码	
	
*/
int UnionReadStructDeclareDefFromStr(char *str,int lenOfStr,PUnionStructDeclareDef pdeclareDef);

/* 将指定结构的声明头打印到文件中
输入参数
	pdef		声明定义
输出参数
	fp		文件句柄
返回值：
	>=0 	正确
	<0	出错代码	
	
*/
int UnionPrintStructHeaderDeclareDefToFp(PUnionStructDeclareDef pdef,FILE *fp);

/* 将指定结构的声明尾打印到文件中
输入参数
	pdef		声明定义
输出参数
	fp		文件句柄
返回值：
	>=0 	正确
	<0	出错代码	
	
*/
int UnionPrintStructTailDeclareDefToFp(PUnionStructDeclareDef pdef,FILE *fp);

/* 将指定结构的定义打印到文件中
输入参数
	pstructDef	结构定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintStructDefToFp(PUnionStructDef pstructDef,FILE *fp);

/* 将指定文件中定义的结构打印到文件中
输入参数
	fileName	文件名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintStructDefInFileToFp(char *fileName,FILE *fp);
	
/* 将指定文件中定义的结构打印到屏幕上
输入参数
	fileName	文件名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputStructDefInFile(char *fileName);

/* 将指定名称的结构定义输出到文件中
输入参数
	nameOfType	结构名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSpecStructDefToFp(char *nameOfType,FILE *fp);
	
/* 将指定文件中定义的结构打印到屏幕上
输入参数
	nameOfType	结构名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputSpecStructDef(char *nameOfType);

/* 将指定结构的定义以定义格式打印到文件中
输入参数
	pdef	结构定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintStructDefToFpInDefFormat(PUnionStructDef pdef,FILE *fp);

#endif
