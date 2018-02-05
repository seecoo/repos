// author	Wolfgang Wang
// date		2010-5-5

#ifndef _unionVarListDef_
#define _unionVarListDef_

#define conMaxNumOfVarPerList	128

typedef struct
{
	char	*varName;
	int	len;
	char	*varValue;
} TUnionVarValue;
typedef TUnionVarValue	*PUnionVarValue;

typedef struct
{
	char		varListName[40+1];
	int		varNum;
	PUnionVarValue	varList[conMaxNumOfVarPerList];
} TUnionVarList;
typedef TUnionVarList	*PUnionVarList;

/*
功能
	将一个变量列表写到日志中
输入参数：
	pvarList	变量列表
	title		标题
输出参数
	无
返回值
	无
*/
void UnionLogVarList(PUnionVarList pvarList,char *title);

/*
功能
	初始化一个变量
输入参数：
	varName		变量名称
	len		变量值
	varValue	变量值
输出参数
	无
返回值
	成功	变量指针
	失败	NULL
*/
PUnionVarValue UnionInitOneVar(char *varName,int len,char *varValue);

/*
功能
	释放一个变量
输入参数：
	ptr		变量指针
输出参数
	无
返回值
	无
*/
void UnionFreeOneVar(PUnionVarValue ptr);

/*
功能
	登记一个变量
输入参数：
	pvarList	变量列表
	varName		变量名称
	len		变量值
	varValue	变量值
输出参数
	无
返回值
	>=0		变量数据
	<0		出错代码
*/
int UnionAddOneVarToVarList(PUnionVarList pvarList,char *varName,int len,char *varValue);

/*
功能
	破坏一个变量列表
输入参数：
	pvarList	变量列表
输出参数
	无
返回值
	无
*/
void UnionFreeVarList(PUnionVarList pvarList);

/*
功能
	创建一个变量列表
输入参数：
	varListName	列表名称
输出参数
	无
返回值
	成功	变量列表指针
	失败	NULL
*/
PUnionVarList UnionCreateVarList(char *varListName);

/*
功能
	读取一个变量
输入参数：
	pvarList	变量列表
	varName		变量名称
	sizeOfBuf	变量缓冲大小
输出参数
	varValue	变量值
返回值
	>=0		变量数据
	<0		出错代码
*/
int UnionReadOneVarByVarName(PUnionVarList pvarList,char *varName,char *varValue,int sizeOfBuf);

#endif
