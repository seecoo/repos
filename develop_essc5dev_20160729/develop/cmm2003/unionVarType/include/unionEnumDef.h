//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionEnumDef_
#define _unionEnumDef_

#include "unionVarDef.h"

#define conEnumDefTagTypeDefName	"enumName"
#define conEnumDefTagRemark		"remark"
#define conEnumDeclareLineTag		"enumName="

#include "unionConstDef.h"

// 枚举声明定义
typedef struct
{
	char			name[48+1];		// 枚举的名称,typedef的名称
	char			remark[128+1];		// 说明
} TUnionEnumDeclareDef;
typedef TUnionEnumDeclareDef	*PUnionEnumDeclareDef;

#ifdef _maxNumOfVarPerEnumIs512_
#define conMaxNumOfVarPerEnum	512
#else
#define conMaxNumOfVarPerEnum	256
#endif
// 定义枚举的定义
typedef struct
{
	TUnionEnumDeclareDef	declareDef;			// 枚举声明定义
	int			constNum;				// 枚举的域数量
	TUnionConstDef		constGrp[conMaxNumOfVarPerEnum];	// 域的定义
} TUnionEnumDef;
typedef TUnionEnumDef		*PUnionEnumDef;

/* 从一个字符串中读取枚举声明的定义
输入参数
	str		声明定义串
	lenOfStr	声明定义串的长度
输出参数
	pdeclareDef	读出的声明定义
返回值：
	>=0 	读出的声明数目
	<0	出错代码	
	
*/
int UnionReadEnumDeclareDefFromStr(char *str,int lenOfStr,PUnionEnumDeclareDef pdeclareDef);

/* 将指定枚举的声明头打印到文件中
输入参数
	pdef		声明定义
输出参数
	fp		文件句柄
返回值：
	>=0 	正确
	<0	出错代码	
	
*/
int UnionPrintEnumHeaderDeclareDefToFp(PUnionEnumDeclareDef pdef,FILE *fp);

/* 将指定枚举的声明尾打印到文件中
输入参数
	pdef		声明定义
输出参数
	fp		文件句柄
返回值：
	>=0 	正确
	<0	出错代码	
	
*/
int UnionPrintEnumTailDeclareDefToFp(PUnionEnumDeclareDef pdef,FILE *fp);

/* 将指定枚举的定义打印到文件中
输入参数
	pdef	枚举定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintEnumDefToFp(PUnionEnumDef pdef,FILE *fp);

/* 将指定文件中定义的枚举打印到文件中
输入参数
	fileName	文件名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintEnumDefInFileToFp(char *fileName,FILE *fp);
	
/* 将指定文件中定义的枚举打印到屏幕上
输入参数
	fileName	文件名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputEnumDefInFile(char *fileName);

/* 将指定名称的枚举定义输出到文件中
输入参数
	nameOfType	枚举名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSpecEnumDefToFp(char *nameOfType,FILE *fp);
	
/* 将指定文件中定义的枚举打印到屏幕上
输入参数
	nameOfType	枚举名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputSpecEnumDef(char *nameOfType);

#endif
