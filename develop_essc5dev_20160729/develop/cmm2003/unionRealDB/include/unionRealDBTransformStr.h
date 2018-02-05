#ifndef _realDBTransformStr_
#define _realDBTransformStr_

#include "unionComplexDBCommon.h"

typedef enum
{
	conTransformTypeInsert = 0,		// 插入值串
	conTransformTypeUpdate = 1,		// 更新值串
	conTransformTypeCondition = 2,		// 条件串
} TUnionTransformType;
typedef TUnionTransformType	*PUnionTransformType;

typedef struct
{
	char str[1024];	// SQL语句拆分出来的字符串
	int iType;		// 字符串类型
					// 1：关键字"SELECT UPDATE DELETE AS SUM COUNT DISTINCT FROM WHERE ORDER GROUP BY HAVING AND OR"等 
					// 2：标识符，域名、表名、自定义变量等
					// 3：数值
					// 4：带''或""的字符串
					// 5：特殊分割符，',','(',')','.'
					// 6: 运算符，"=,+,-,*,/,<,>,<=,>=,<>,!="
} TUnionSQLAnalyseRes;
typedef TUnionSQLAnalyseRes   *PUnionSQLAnalyseRes;

/*
功能：将一个记录串转换为SQL的字符串
输入参数：
	recStr：	请求字符串
	lenOfRecStr：	recStr的长度
	type：		SQL的类型
	sizeOfBuf：	sqlStr的存储大小
输出参数：
	sqlStr：	插入SQL的值串
返回值：
	>=0：		成功
	<0：		失败，错误码
*/
int UnionIsSQLString(char *recStr,int lenOfRecStr,char *sqlStr,int sizeOfBuf);

/*
功能：
	将一个条件串中的域名替换为库中的域名
输入参数：
	resName		表名
	oriSQLStr	原串
输出参数：
	desSQLStr	目标串
返回值：
	>=0：		目标串的长度
	<0：		失败，错误码
*/
int UnionConvertFldDefNameInSQLStrIntoDBFldNameOfSpecObject(char *resName,char *oriSQLStr,char *desSQLStr);

/*
功能：将一个记录串转换为插入SQL的值串
输入参数：
	recStr：	请求字符串
	lenOfRecStr：	recStr的长度
	sizeOfBuf：	sqlStr的存储大小
输出参数：
	sqlStr：	插入SQL的值串
返回值：
	>=0：		成功
	<0：		失败，错误码
*/
int UnionTransformRecStrToSQLStr(char *tblName,char *recStr,int lenOfRecStr,TUnionTransformType type,char *sqlStr,int sizeOfBuf);

/*
功能：从一个记录串获取操作符
输入参数：
	recStr：	请求字符串
	lenOfRecStr：	recStr的长度
输出参数：
	oper：		操作符
返回值：
	>=0：		成功
	<0：		失败，错误码
*/
int UnionGetOperatorFromRecStr(char *recStr,int lenOfRecStr,char *oper);

/*
功能：
格式化域值
输入参数：
pobject：	指向对象定义的指针
fldName		域名称
输出参数：
formatFldVal	格式化后的域值
返回值：
>=0：	类型
<0：	失败，返回错误码
*/
int UnionGetFormatFldValueOfSpecFldFromObjectDef(PUnionObject pobject,char *fldName,char *fldValue,char *formatFldVal);

/*
功能：
对SQL语句做词法分析
输入参数：
strSQL：	SQL语句
iMaxSQLAnalyseResNum：	词法分析出的单词最大个数
输出参数：
tSQLAnalyseRes：	词法分析结果
返回值：
>=0：	成功，词法分析出的单词个数
<0：	失败，返回错误码
*/
int UnionAnalyzeSQL(char *strSQL, TUnionSQLAnalyseRes tSQLAnalyseRes[], int iMaxSQLAnalyseResNum);

/*
功能：
将前台传入的SQL语句的域名转化为真正数据库域名
输入参数：
strSQL：		SQL语句
strTableName：	表名
iSizeOfDestSQL：存放转化后的SQL语句缓冲区的大小
输出参数：
strDestSQL：	转化后的SQL语句
返回值：
>=0：	成功，转换后SQL语句的长度
<0：	失败，返回错误码
*/
int UnionTransSQL(char *strSQL, char *strTableName, char *strDestSQL, int iSizeOfDestSQL);

int UnionTransRecStrFileToSpecFormatFile(char *oriFile, char *desFile, char speratorTag);

#endif
