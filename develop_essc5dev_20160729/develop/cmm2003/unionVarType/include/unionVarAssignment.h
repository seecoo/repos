//	Wolfgang Wang
//	2008/11/15

// 定义变量的赋值方法

#ifndef _unionVarAssignment_
#define _unionVarAssignment_

#define conVarAssignmentMethodValue		"value"
#define conVarAssignmentMethodFunVar		"fun"
#define conVarAssignmentMethodGolobalVar	"var"
#define conVarAssignmentMethodInput		"input"

#define conVarAssignmentMethodFldVarValue	"value"
#define conVarAssignmentMethodFldVarName	"var"
#define conVarAssignmentMethodFldVarLen		"len"
#define conVarAssignmentMethodFldVarIsBinary	"isBinary"
#define conVarAssignmentMethodFldVarFunIndex	"index"

typedef struct
{
	char		varName[128+1];		// 变量名称
	char		method[128+1];		// 变量赋值方法
	char		value[4096+1];		// 变量的值
	int		isVarName;		// 变量的值是不是另一个变量的名称
	char		lenTag[128+1];		// 变量值的长度
	int		isBinary;		// 变量的值是不是二进制,1是,0否
} TUnionVarAssignmentDef;
typedef TUnionVarAssignmentDef	*PUnionVarAssignmentDef;

/*
功能	判断是否是合法的变量赋值方法
输入参数
	method		方法
输出参数
        无
返回值
	1		合法
	0		不合法
*/
int UnionIsValidVarAssignmentMethod(char *method);

/*
功能	读指定变量的赋值方法
输入参数
	str         	串
        lenOfStr  	串长度
        sizeOfBuf	接收赋值方法的值的缓冲的大小
输出参数
        varName		变量名称
	method		读出的赋值方法
	value		赋值方法使用的值
返回值
	>=0		域赋值方法的长度
	<0		出错代码
*/
int UnionReadVarValueAssignMethod(char *str,int lenOfStr,char *varName,char *method,char *value,int sizeOfBuf);

/*
功能	
	从一个串中读取变量的赋值方法
输入参数
	str		串
	lenOfStr	串长度
输出参数
	pdef		赋值方法
返回值
	>=0		成功,值的长度
	<0		出错代码
*/
int UnionReadVarAssignmentDefFromStr(char *str,int lenOfStr,PUnionVarAssignmentDef pdef);

/*
功能	
	将一个变量定义写入到指定文件中
输入参数
	pdef		赋值方法
	fp		指定文件句柄
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintVarAssignmentDefToFile(PUnionVarAssignmentDef pdef,FILE *fp);

#endif
