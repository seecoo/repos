// wolfang wang
// 2008/10/3

#ifndef _unionVarValue_
#define _unionVarValue_

#include "unionVarTypeDef.h"
#include "unionVarDef.h"

// 定义缺省字符串变量的最大长度
#ifndef conMaxDefaultSizeOfVarString	
#define conMaxDefaultSizeOfVarString	(8192+1)
#endif

#define conVarValueTagBaseTypeTag	"baseTypeTag"
#define conVarValueTagValueTag		"valueTag"

// 设置数组的缺省大小
void UnionSetDefaultArraySizeWhenNoSizeSet(int size);

// 定义打印变量赋值标识列表的类型
typedef enum
{
	conTagOfPrintValueTagListToFilePrintToFile = 10,			// 将变量的取值清单打印到文件中
	conTagOfPrintValueTagListToFileWriteMallocOfPointerToFile = 20,		// 将分配指针变量空间的语句打印到文件中
	conTagOfPrintValueTagListToFileWriteFreeOfPointerToFile = 21,		// 将释放指针变量空间的语句打印到文件中
} TUnionTagOfPrintValueTagListToFile;

// 定义变量的值
typedef struct
{
	char				varValueTag[256+1];	// 变量名
	int				baseTypeTag;		// 基础变量标识
} TUnionVarValueTagDef;
typedef TUnionVarValueTagDef		*PUnionVarValueTagDef;

// 设置将变量的取值清单打印到文件中时，打印的内容
void UnionSetTagOfPrintValueTagListToFile(int tag);

/*
功能	
	从文件读取一个指定取值标识的变量的值
输入参数
	fileName	文件名称
输出参数
	pdef		读出的函数定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadValueOfSpecValueTagDefFromSpecFile(char *fileName,PUnionVarValueTagDef pdef,char *value,int sizeOfValue);

/*
功能	
	从一个定义口串中读取一个变量取值标识定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输出参数
	pdef		变量定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadVarValueTagDefFromStr(char *str,int lenOfStr,PUnionVarValueTagDef pdef);

/*
功能	
	将一个简单变量取值写入到指定文件中
输入参数
	varNamePrefix	变量名称的前缀
	pdef		变量的定义
	fp		输出文件指针
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintValueTagListToFile(char *varNamePrefix,PUnionVarDef pdef,FILE *fp);

/*
功能	
	将一个数组变量取值写入到指定文件中
输入参数
	varNamePrefix	变量名称的前缀
	pdef		变量的定义
	fp		输出文件指针
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintArrayVarValueTagListToFile(char *varNamePrefix,PUnionVarDef pdef,int dimisionIndex,FILE *fp);

/*
功能	
	将一个数组变量取值写入到指定文件中
输入参数
	varNamePrefix	变量名称的前缀
	isPointer	指针标识
	nameOfType	变量的类型
	fp		输出文件指针
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintArrayTypeValueTagListToFile(char *varNamePrefix,int isPointer,char *nameOfType,int dimisionIndex,FILE *fp);

/*
功能	
	将一个简单c语言类型取值方法写入到指定文件中
输入参数
	varName		变量名称
	typeTag		类型
	isPointer	指标标识，1表示是指针，0表示不是指针
	fp		输出文件指针
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintCProgramSimpleTypeValueTagListToFile(char *varName,int isPointer,int typeTag,FILE *fp);

/*
功能	
	将一个结构取值方法写入到指定文件中
输入参数
	varNamePrefix	变量名称的前缀
	nameOfType	类型名称
	isPointer	指明变量是否是指针
	fp		输出文件指针
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintStructTypeValueTagListToFile(char *varNamePrefix,int isPointer,char *nameOfType,FILE *fp);

/*
功能	
	将一个简单类型取值方法写入到指定文件中
输入参数
	varNamePrefix	变量名称的前缀
	nameOfType	类型名称
	fp		输出文件指针
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintSimpleTypeValueTagListToFile(char *varNamePrefix,int isPointer,char *nameOfType,FILE *fp);

/*
功能	
	将一个指针类型取值方法写入到指定文件中
输入参数
	varNamePrefix	变量名称的前缀
	nameOfType	类型名称
	fp		输出文件指针
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintPointerTypeValueTagListToFile(char *varNamePrefix,int isPointer,char *nameOfType,FILE *fp);

/*
功能	
	打印一个变量定义的赋值列表
输入参数
	str		定义串
	lenOfStr	定义串的长度
输入出数
	fp		输出的文件指针
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintValueTagListDefinedInStrToFp(char *str,int lenOfStr,FILE *fp);

/*
功能	
	打印一个变量定义的赋值列表
输入参数
	str		定义串
	lenOfStr	定义串的长度
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionOutputValueTagListDefinedInStr(char *str,int lenOfStr);

/*
功能	
	读一个变量的指定域的类型
输入参数
	specFldOfVar	指定的变量域
	varNamePrefix	变量名称的前缀
	pdef		变量的定义
输出参数	
	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varNamePrefix,PUnionVarDef pdef);

/*
功能	
	读一个数组变量取值的类型
输入参数
	specFldOfVar	指定的变量域
	varNamePrefix	变量名称的前缀
	pdef		变量的定义
输出参数	
	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadArrayVarTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varNamePrefix,PUnionVarDef pdef,int dimisionIndex);

/*
功能	
	读一个数组变量取值的类型
输入参数
	specFldOfVar	指定的变量域
	varNamePrefix	变量名称的前缀
	isPointer	指针标识
	nameOfType	变量的类型
输出参数	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadArrayTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varNamePrefix,int isPointer,char *nameOfType,int dimisionIndex);

/*
功能	
	读一个简单c语言类型取值方法的类型
输入参数
	specFldOfVar	指定的变量域
	varName		变量名称
	typeTag		类型
	isPointer	指标标识，1表示是指针，0表示不是指针
输出参数	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadCProgramSimpleTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varName,int isPointer,int typeTag);

/*
功能	
	读一个结构取值方法的类型
输入参数
	specFldOfVar	指定的变量域
	varNamePrefix	变量名称的前缀
	nameOfType	类型名称
	isPointer	指明变量是否是指针
输出参数	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadStructTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varNamePrefix,int isPointer,char *nameOfType);

/*
功能	
	读一个简单类型取值方法的类型
输入参数
	specFldOfVar	指定的变量域
	varNamePrefix	变量名称的前缀
	nameOfType	类型名称
输出参数	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadSimpleTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varNamePrefix,int isPointer,char *nameOfType);

/*
功能	
	读一个指针类型取值方法的类型
输入参数
	specFldOfVar	指定的变量域
	varNamePrefix	变量名称的前缀
	nameOfType	类型名称
输出参数	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadPointerTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varNamePrefix,int isPointer,char *nameOfType);

/*
功能	
	读一个变量域的类型
输入参数
	specFldOfVar	指定的变量域
	str		定义串
	lenOfStr	定义串的长度
输入出数
	无
返回值
	>=0		类型
	<0		出错代码
*/
int UnionReadValueTagFromVarDefStrOfSpecFldOfVar(char *specFldOfVar,char *str,int lenOfStr);

#endif
