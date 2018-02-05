// Wolfgang Wang,2009/8/1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
功能：
	请求备份服务器同步一个插入动作
输入参数:
	tableName	表名
	record		动作操作的参数串
	lenOfRecord	参数串的长度
输出参数:
	无
返回值
	>=0		成功
	<0		失败
*/
int UnionSynchronizeComplexDBInsertToBrothers(char *tableName,char *record,int lenOfRecord)
{
	return 0;
}

/*
功能：
	请求备份服务器同步一个删除动作
输入参数:
	tableName	表名
	primaryKey	动作操作的关键字
输出参数:
	无
返回值
	>=0		成功
	<0		失败
*/
int UnionSynchronizeComplexDBDeleteToBrothers(char *tableName,char *primaryKey)
{
	return 0;
}

/*
功能：
	请求备份服务器同步一个更新动作
输入参数:
	tableName	表名
	primaryKey	动作操作的关键字
	record		动作操作的参数串
	lenOfRecord	参数串的长度
输出参数:
	无
返回值
	>=0		成功
	<0		失败
*/
int UnionSynchronizeComplexDBUpdateToBrothers(char *tableName,char *primaryKey,char *record,int lenOfRecord)
{
	return 0;
}

/*
功能：
	获取本mngSvr的标识
输入参数:
	无
输出参数:
	无
返回值
	成功：	本mngSvr的标识
	失败	空指针
*/
char *UnionGetMyMngSvrID()
{
	return(getenv("myMngSvrID"));
}

/*
功能：
	读取本mngSvr的所有兄弟
输入参数:
	maxNum		可读取的最大数目
输出参数:
	brotherID	兄弟mngSvr的标识
返回值
	>=0		读取的数目
	<0		错误码
*/
int UnionReadAllMngSvrBrothersOfMine(char brotherID[][128+1],int maxNum)
{
	return(0);
}


/*
功能：
	请求指定备份服务器同步一个动作
输入参数:
	mngSvrID	备份服务器的标识
	tableName	表名
	operation	动作
	record		动作操作的参数串
	lenOfRecord	参数串的长度
输出参数:
	无
返回值
	>=0		成功
	<0		失败
*/
int UnionSynchronizeComplexDBOperationToSpecBrother(char *mngSvrID,char *tableName,int operation,char *record,int lenOfRecord)
{
	return 0;
}

/*
功能：
	请求备份服务器同步一个动作
输入参数:
	tableName	表名
	operation	动作
	record		动作操作的参数串
	lenOfRecord	参数串的长度
输出参数:
	无
返回值
	>=0		成功
	<0		失败
*/
int UnionSynchronizeComplexDBOperationToBrothers(char *tableName,int operation,char *record,int lenOfRecord)
{
	return(0);
}	

// 登记数据同步流水表
int UnionRegComplexDBOperationToDBDataSynJnl(char *mngSvrID, char *tableName, int operation, char *record, int lenOfRecord)
{
        return 0;
}

// 登记文件同步流水表
int UnionRegFileOperationToFileDataSynJnl(char *tableName, char *dirName,char *fileName, int fileType)
{
        return 0;
}
