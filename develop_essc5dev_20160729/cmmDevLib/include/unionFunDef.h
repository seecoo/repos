// wolfang wang
// 2008/10/3

#ifndef _unionFunDef_
#define _unionFunDef_

#include "unionVarDef.h"

#define conFunDefTagFunName	"funName"
#define conFunDefTagVarDef	"varDef="
#define conFunDefTagIsOutputVar	"isOutput"
#define conFunDefTagReturnType	"returnType="
#define conFunDefTagFunRemark	"remark"

// 函数参数定义
typedef struct
{
	TUnionVarDef		varDef;		// 参数定义
	int			isOutput;	// 1表示输出参数，0表示输入参数
} TUnionFunParDef;
typedef TUnionFunParDef	*PUnionFunParDef;

// 定义一个报文域组中域的最大数目
#define conMaxNumOfVarPerFun	16
// 函数定义
typedef struct
{
	char			funName[64+1];	// 函数名称
	int			varNum;		// 参数数目
	TUnionFunParDef		varGrp[conMaxNumOfVarPerFun];	// 参数数目
	TUnionVarDef		returnType;	// 返回值类型
	char			remark[128+1];	// 函数说明
} TUnionFunDef;	
typedef TUnionFunDef		*PUnionFunDef;

/*
功能	
	为一个函数产生一段调用代码
输入参数
	prefixBlankNum	调用代码前缀的空格数
	retVarName	返回值名称
	pdef		函数定义
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateCallCodesOfFun(int prefixBlankNum,char *retVarName,PUnionFunDef pdef,FILE *fp);

/*
功能	
	打印一个函数定义的说明
输入参数
	pdef		要打印的定义
输出参数
	fp		输出的文件指针
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintFunDefRemarkToFp(PUnionFunDef pdef,FILE *fp);

/*
功能	
	打印一个函数定义
输入参数
	pdef			要打印的函数定义
	inCProgramFormat	1，使用c语言定义格式输出，0，不使用c语言定义输出
	isDeclaration		1，函数声明，0，函数体
输出参数
	fp		输出的文件指针
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintFunDefToFp(PUnionFunDef pdef,int inCProgramFormat,int isDeclaration,FILE *fp);

/*
功能	
	为一个函数的参数定义一组变量声明
输入参数
	pdef				要打印的函数定义
	defaultName			缺省的名字，如果未定义名字，则采用这个名字
	defaultArraySizeWhenSizeNotSet	缺省的数组大小，如果数组大小未定义	
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintFunVarDefToFpOfCPragram(PUnionFunDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp);

/*
功能	
	为一个函数的参数定义一组变量声明
输入参数
	prefixBlankNum			变量名前缀的空格数
	pdef				要打印的函数定义
	defaultName			缺省的名字，如果未定义名字，则采用这个名字
	defaultArraySizeWhenSizeNotSet	缺省的数组大小，如果数组大小未定义	
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintFunVarDefToFpOfCPragramWithPrefixBlank(int prefixBlankNum,PUnionFunDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp);

/*
功能	
	为一个函数的参数定义一组变量声明,在变量名称前加上前缀
输入参数
	prefix				变量前缀
	pdef				要打印的函数定义
	defaultName			缺省的名字，如果未定义名字，则采用这个名字
	defaultArraySizeWhenSizeNotSet	缺省的数组大小，如果数组大小未定义	
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintFunVarDefWithPrefixToFpOfCPragram(char *prefix,PUnionFunDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp);

/*
功能	
	打印一个函数定义
输入参数
	fileName		文件名称
	inCProgramFormat	1，使用c语言定义格式输出，0，不使用c语言定义输出
	isDeclaration		1，函数声明，0，函数体
输出参数
	fp		输出的文件指针
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintFunDefInFileDefToFp(char *fileName,int inCProgramFormat,int isDeclaration,FILE *fp);

/*
功能	
	打印一个函数定义
输入参数
	fileName		文件名称
	inCProgramFormat	1，使用c语言定义格式输出，0，不使用c语言定义输出
	isDeclaration		1，函数声明，0，函数体
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionOutputFunDefInFileDef(char *fileName,int inCProgramFormat,int isDeclaration);

/*
功能	
	打印缺省定义文件中的一个函数定义
输入参数
	funName			函数名称
	isDeclaration		1，函数声明，0，函数体
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionOutputFunDefInDefaultDefFile(char *funName,int isDeclaration);

/* 将指定名称的函数定义输出到文件中
输入参数
	funName	函数名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSpecFunDefToFp(char *funName,FILE *fp);
	
/* 将指定文件中定义的函数打印到屏幕上
输入参数
	funName	函数名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputSpecFunDef(char *funName);

/*
功能	
	读一个函数的变量域的类型
输入参数
	pdef				要打印的函数定义
	specFldOfVar			缺省的名字，如果未定义名字，则采用这个名字
输出参数
	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadTypeTagOfSpecVarFldOfFun(PUnionFunDef pdef,char *specFldOfVar);

/*
功能	
	读一个函数的变量域的类型
输入参数
	funName				函数名称
	specFldOfVar			缺省的名字，如果未定义名字，则采用这个名字
输出参数
	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadTypeTagOfSpecVarFldOfSpecFun(char *funName,char *specFldOfVar);

#endif
