// wolfang wang
// 2008/10/3

#ifndef _unionConstDef_
#define _unionConstDef_

#define conConstDefTagValue		"value"
#define conConstDefTagName		"name"
#define conConstDefTagType		"type"
#define conConstDefTagRemark		"remark"

typedef struct
{
	char			name[64+1];		// 常量名称
	char			value[128+1];		// 值
	int			type;			// 类型
	char			remark[128+1];		// 说明
} TUnionConstDef;
typedef TUnionConstDef	*PUnionConstDef;

/*
功能	
	从缺省定义文件读取整数常量的值
输入参数
	constName	常量名称
输出参数
	constValue	常量值
返回值
	>=0		常量值
	<0		出错代码
*/
int UnionCalculateIntConstValue(char *constName);

/*
功能	
	从一个定义口串中读取一个常量类型定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输出参数
	pdef		常量定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadConstDefFromStr(char *str,int lenOfStr,PUnionConstDef pdef);

/* 将指定类型标识的定义打印到文件中
输入参数
	pdef	类型标识定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintConstDefToFp(PUnionConstDef pdef,FILE *fp);

/* 将指定文件中定义的类型标识打印到文件中
输入参数
	fileName	文件名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintConstDefInFileToFp(char *fileName,FILE *fp);
	
/* 将指定文件中定义的类型标识打印到屏幕上
输入参数
	pdef	类型标识定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputConstDefInFile(char *fileName);

/* 从缺省定义文件读指定名称的常量的定义
输入参数
	nameOfConst	常量类型
输出参数
	pdef	读出的常量定义
返回值：
	>=0 	读出的常量的大小
	<0	出错代码	
	
*/
int UnionReadConstDefFromDefaultDefFile(char *nameOfConst,PUnionConstDef pdef);

/* 将指定名称的常量定义输出到文件中
输入参数
	nameOfConst	常量名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintSpecConstDefToFp(char *nameOfConst,FILE *fp);
	
/* 将指定文件中定义的常量打印到屏幕上
输入参数
	nameOfConst	常量名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputSpecConstDef(char *nameOfConst);

#endif
