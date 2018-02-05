// wolfang wang
// 2008/10/3

#ifndef _unionVarTypeDef_
#define _unionVarTypeDef_

#define conVarTypeDefTagTypeTag			"typeTag"
#define conVarTypeDefTagNameOfType		"nameOfType"
#define conVarTypeDefTagProgramName		"nameOfProgram"

#include "unionDesignKeyWord.h"

#ifndef conDesginKeyWordTagVarTypeOffset
#define conDesginKeyWordTagVarTypeOffset	100
#endif

#define conDesginKeyWordTagStrInt			"int"
#define conDesginKeyWordTagStrLong			"long"
#define conDesginKeyWordTagStrLong1			"long int"
#define conDesginKeyWordTagStrDouble			"double"
#define conDesginKeyWordTagStrFloat			"float"
#define conDesginKeyWordTagStrChar			"char"
#define conDesginKeyWordTagStrVoid			"void"
#define conDesginKeyWordTagStrFile			"file"
#define conDesginKeyWordTagStrFile1			"FILE"
#define conDesginKeyWordTagStrString			"string"
#define conDesginKeyWordTagStrEnum			"enum"
#define conDesginKeyWordTagStrSimpleType		"simple"
#define conDesginKeyWordTagStrStruct			"struct"
#define conDesginKeyWordTagStrUnion			"union"
#define conDesginKeyWordTagStrArray			"array"
#define conDesginKeyWordTagStrPointer			"pointer"
#define conDesginKeyWordTagStrUnsignedInt		"unsigned int"
#define conDesginKeyWordTagStrUnsignedLong		"unsigned long"
#define conDesginKeyWordTagStrUnsignedLong1		"unsigned long int"
#define conDesginKeyWordTagStrUnsignedChar		"unsigned char"
#define conDesginKeyWordTagStrFun			"fun"
#define conDesginKeyWordTagStrConst			"const"
#define conDesginKeyWordTagStrGlobalVar			"var"
#define conDesginKeyWordTagStrUnsignedString		"unsigned string"

// 变量分类
typedef enum
{
	conVarTypeTagInt = conDesginKeyWordTagVarTypeOffset + 1,
	conVarTypeTagLong = conDesginKeyWordTagVarTypeOffset + 2,
	conVarTypeTagDouble = conDesginKeyWordTagVarTypeOffset + 3,
	conVarTypeTagFloat = conDesginKeyWordTagVarTypeOffset + 4,
	conVarTypeTagChar = conDesginKeyWordTagVarTypeOffset + 5,
	conVarTypeTagVoid = conDesginKeyWordTagVarTypeOffset + 6,
	conVarTypeTagFile = conDesginKeyWordTagVarTypeOffset + 7,
	conVarTypeTagString = conDesginKeyWordTagVarTypeOffset + 8,
	conVarTypeTagEnum = conDesginKeyWordTagVarTypeOffset + 9,
	conVarTypeTagSimpleType = conDesginKeyWordTagVarTypeOffset + 10,
	conVarTypeTagStruct = conDesginKeyWordTagVarTypeOffset + 11,
	conVarTypeTagUnion = conDesginKeyWordTagVarTypeOffset + 12,
	conVarTypeTagArray = conDesginKeyWordTagVarTypeOffset + 13,
	conVarTypeTagPointer = conDesginKeyWordTagVarTypeOffset + 14,
	conVarTypeTagUnknown = conDesginKeyWordTagVarTypeOffset + 15,
	conVarTypeTagUnsignedInt = conDesginKeyWordTagVarTypeOffset + 16,
	conVarTypeTagUnsignedLong = conDesginKeyWordTagVarTypeOffset + 17,
	conVarTypeTagUnsignedChar = conDesginKeyWordTagVarTypeOffset + 18,
	conVarTypeTagUnsignedString = conDesginKeyWordTagVarTypeOffset + 19,
} TUnionVarTypeTag;
typedef TUnionVarTypeTag	TUnionBaseVarTypeTag;	// 定义基础变量标识

typedef struct
{
	char			nameOfType[64+1];	// 变量类型名称
	int			typeTag;		// 变量类型标识
	char			nameOfProgram[64+1];	// 模块名称
} TUnionVarTypeDef;
typedef TUnionVarTypeDef	*PUnionVarTypeDef;

/*
功能	
	从一个定义口串中读取一个变量类型定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输入出数
	pdef		变量定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadVarTypeDefFromStr(char *str,int lenOfStr,PUnionVarTypeDef pdef);

/*
功能	
	从一个定义口串中读取一个变量类型定义
输入参数
	pdef		变量定义
	sizeOfBuf	定义串的大小
输入出数
	str		定义串
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPutVarTypeDefIntoStr(PUnionVarTypeDef pdef,char *str,int sizeOfBuf);

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

/*
功能	
	将一个科友内部关键字标识转换为标识值
输入参数
	keyWord		c言类型定义的关键字
输入出数
	无
返回值
	>=0		转换出的内部标识
	<0		出错代码
*/
int UnionConvertUnionKeyWordIntoTag(char *keyWord);

/*
功能	
	将一个c言类型定义的关键字转化为科友内部标识
输入参数
	keyWord		c言类型定义的关键字
输入出数
	无
返回值
	>=0		转换出的内部标识
	<0		出错代码
*/
int UnionConvertCVarDefTypeIntoTag(char *keyWord);

/*
功能	
	判断一个类型是否是一个简单类型
输入参数
	nameOfType	变量类型
输入出数
	无
返回值
	1		是
	0		不是
*/
int UnionIsBaseType(char *nameOfType);

/*
功能	
	获得指定类型的变量的内部标识
输入参数
	nameOfType	变量类型
输入出数
	无
返回值
	奕量类型的内部标识
*/
int UnionGetTypeTagOfSpecNameOfType(char *nameOfType);

/*
功能	
	获得指定类型的变量的最原始类型
输入参数
	oriNameOfType	变量类型
输入出数
	finalNameOfType	变量最终类型
返回值
	>= 0		成功
	<0		错误代码
*/
int UnionGetFinalTypeNameOfSpecNameOfType(char *oriNameOfType,char *finalNameOfType);

/*
功能	
	判断是否是一个简单类型
输入参数
	keyTag		类型标识
输入出数
	无
返回值
	1		是
	0		不是
*/
int UnionIsBaseTypeTag(TUnionVarTypeTag keyTag);

/*
功能	
	将一个科友内部标识转换为c言类型定义的关键字
输入参数
	keyTag		类型标识
输入出数
	keyWord		c言类型定义的关键字
返回值
	>=0		转换出的内部标识
	<0		出错代码
*/
int UnionConvertTypeTagIntoCVarDefKeyWord(TUnionVarTypeTag keyTag,char *keyWord);

/*
功能	
	将一个c言类型定义的关键字转化为科友内部标识
输入参数
	keyWord		c言类型定义的关键字
输入出数
	无
返回值
	>=0		转换出的内部标识
	<0		出错代码
*/
int UnionConvertCVarDefKeyWordIntoTag(char *keyWord);

/*
功能	
	从一个定义口串中读取一个变量类型定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输入出数
	pdef		变量定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadVarTypeDefFromStr(char *str,int lenOfStr,PUnionVarTypeDef pdef);

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

/* 将指定类型标识的定义打印到文件中
输入参数
	pdef	类型标识定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintVarTypeDefToFp(PUnionVarTypeDef pdef,FILE *fp);

/* 将指定类型标识的定义以定义格式打印到文件中
输入参数
	pdef	类型标识定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintVarTypeDefToFpInDefFormat(PUnionVarTypeDef pdef,FILE *fp);

/* 将指定文件中定义的类型标识打印到文件中
输入参数
	fileName	文件名称
	nameOfType	类型名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintVarTypeDefInFileToFp(char *fileName,char *nameOfType,FILE *fp);
	
/* 将指定文件中定义的类型标识打印到屏幕上
输入参数
	pdef	类型标识定义
	nameOfType	类型名称
输出参数
	无
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputVarTypeDefInFile(char *nameOfType,char *fileName);
#endif
