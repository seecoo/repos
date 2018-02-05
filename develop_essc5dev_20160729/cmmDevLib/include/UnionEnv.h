// Author:	Wolfgang Wang
// Date:	2001/8/30

// Version:	3.0
// 2003/09/19,Wolfgang Wang, 升级为3.0
/*
	增加了编译开关	_UnionEnv_3_x_
	3.0以前的版本，最多可以为一个变量定义1个值
	3.0以后的版本，最多可以为一个变量定义10个值
*/

#ifndef _UnionVar
#define _UnionVar

int UnionGetEnviVarNum();
char *UnionGetEnviVarByIndex(int index);
char *UnionGetEnviVarByName(char *varname);
int UnionInitEnvi(char *filename);
int UnionClearEnvi();
char *UnionGetEnviVarNameByIndex(int index);

#ifndef _UnionEnv_3_x_

#define MAXUNIONENVIVARIABLES	128

typedef struct
{
	char	*pVariableName;
	char	*pVariableValue;
} TUnionEnviVariable;
typedef TUnionEnviVariable	*PUnionEnviVariable;

#endif	//#ifndef _UnionEnv_3_x_

// Added by Wolfgang Wang, 2003/09/19
#ifdef _UnionEnv_3_x_

#ifdef _MaxVarNum_1024_
#define MAXUNIONENVIVARIABLES	1024
#else
#ifdef _MaxVarNum_2048_
#define MAXUNIONENVIVARIABLES	2048
#else
#ifdef _MaxVarNum_4096_
#define MAXUNIONENVIVARIABLES	4096
#else
#ifdef _LargeEnvTBL_
#define MAXUNIONENVIVARIABLES	1000
#else
#define MAXUNIONENVIVARIABLES	256
#endif	// _LargeEnvTBL_
#endif  // _MaxVarNum_4096_
#endif  // _MaxVarNum_2048_
#endif  // _MaxVarNum_1024_

//#endif // ifdef _UnionEnv_3_x_

#ifndef _LargeEnvVar_
#define conMaxNumOfValuesForAnEnvVar	10
typedef struct
{
	//char	*pVariableName;
	char	*pVariableValue[conMaxNumOfValuesForAnEnvVar];
} TUnionEnviVariable;
#else
#define conMaxNumOfValuesForAnEnvVar	100
typedef struct
{
	//char	*pVariableName;
	char	*pVariableValue[conMaxNumOfValuesForAnEnvVar];
} TUnionEnviVariable;
#endif
typedef TUnionEnviVariable	*PUnionEnviVariable;

// 读取指定索引变量的，第N个值，N=varValueIndex
char *UnionGetEnviVarOfTheIndexByIndex(int varIndex,int varValueIndex);
// 读取指定名称变量对应的索引号
int UnionGetVarIndexOfTheVarName(char *varName);

char *UnionReadValueOfTheIndexOfEnviVar(PUnionEnviVariable pVar,int index);
#endif	//#ifdef _UnionEnv_3_x_

PUnionEnviVariable ReadUnionEnviVarFromStr(char *str);
int UnionReadOneFileLine(FILE *fp,char *tmpBuf);
int UnionFreeEnviVar(PUnionEnviVariable pvar);
int UnionIsThisVarDefinitionLine(char *line,char *varName);
int UnionReadVarNameFromLine(char *line,char *varName);

// 2006/08/24 增加以下定义
#define conMaxFldNumOfEnviRec	20
typedef int	TUnionEnviRecFldLength[conMaxFldNumOfEnviRec];
// 2006/8/21 增加函数
int UnionUpdateEnviVar(char *fileName,char *varName,char *fmt,...);
int UnionInsertEnviVar(char *fileName,char *varName,char *fmt,...);
int UnionReadEnviVarValueOfName(char *fileName,char *varName,int index,char *value);
int UnionInsertEnviVarWithRemark(char *fileName,char *remark,char *varName,char *fmt,...);
int UnionDeleteEnviVar(char *fileName,char *varName);
int UnionIsRemarkFileLine(char *str);
int UnionExistEnviVarInFile(char *fileName,char *varName);
int UnionIsRecFormatDefLine(char *str);
int UnionReadEnviRecFldLengthFromStr(char *str,TUnionEnviRecFldLength fldLenDef);
int UnionConvertEnviRecStrIntoFormat(TUnionEnviRecFldLength fldLenDef,char *recStr,int sizeOfRecStr);
int UnionInitRecFldLengthDef(TUnionEnviRecFldLength fldLenDef);
int UnionReadEnviRecFldLengthDefFromFile(char *fileName,TUnionEnviRecFldLength recFldLen);
int UnionReadEnviVarDefLineInFile(char *fileName,char *varName,char *varDefLine);
// 2008/5/22,在文件最后增加一行	
int UnionAddStrToFile(char *fileName,char *str);

// 2008/6/20增加
/*
功能
	读取指定名称的变量
输入参数
	varName		变量名称
	sizeOfBuf	接收变量的缓冲大小
输出参数
	buf		读取的变量
返回值
	>=0		变量值长度
	<0		错误码
*/
int UnionReadEnviStrVarByName(char *varName,char *buf,int sizeOfBuf);

// 2008/6/20增加
/*
功能
	读取指定名称的变量
输入参数
	varName		变量名称
输出参数
	value		读取的变量
返回值
	>=0		变量值长度
	<0		错误码
*/
int UnionReadEnviIntVarByName(char *varName,int *value);

// 2008/6/20增加
/*
功能
	读取指定名称的变量
输入参数
	varName		变量名称
输出参数
	value		读取的变量
返回值
	>=0		变量值长度
	<0		错误码
*/
int UnionReadEnviLongVarByName(char *varName,long *value);

#endif
