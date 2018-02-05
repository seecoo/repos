// 2008/7/26
// Wolfang Wang

#ifndef _unionComplexDBOperationDef_
#define _unionComplexDBOperationDef_

/*
功能	
	从一个定义口串中读取一个数据库操作定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
	sizeOfTblName	表名缓冲的大小
	sizeOfCondition	操作条件缓冲的大小
输入出数
	tblName		表名
	operationID	操作标识
	condition	操作条件
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadComplexDBOperationDefFromStr(char *str,int lenOfStr,char *tblName,int sizeOfTblName,int *operationID,char *operationCondition,int sizeOfCondition);

/*
功能	
	从一个定义口串中读取一个数据库选择操作定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
	sizeOfTblName	表名缓冲的大小
	sizeOfOperationDef	操作条件缓冲的大小
	sizeOfFldList	域清单缓冲的大小
输入出数
	tblName		表名
	operationID	操作标识
	condition	操作条件
	fldList		要选择的域清单
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadComplexDBSelectOperationDefFromStr(char *str,int lenOfStr,char *tblName,int sizeOfTblName,int *operationID,char *condition,int sizeOfOperationDef,char *fldList,int sizeOfFldList);

#endif

