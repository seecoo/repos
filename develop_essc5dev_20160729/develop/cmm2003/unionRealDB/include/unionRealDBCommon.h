/*	
Author:	DengJF	
Date:	20090531
*/
#ifndef _UNIONREALDBCOMMON_H_
#define _UNIONREALDBCOMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"

#define CONNECT_DB_TIMEOUT	3

#ifdef _IS_USE_DB2_

#include <sqlca.h>
#include <sqlda.h>
#include <sqlcodes.h>
#include <sql.h>

typedef SQL_STRUCTURE  sqlda SQLDA;

#define	UNIONCHECKERR(CE_STR) UnionCheckSQLError(CE_STR,&sqlca)
#define	SQLSTATE sqlca.sqlstate

// 检查SQL语句运行结果
int UnionCheckSQLError(char *pMsg, struct sqlca *pSQLCA);

// 初试化SQLDA
int UnionInitSQLDA(SQLDA **pSQLDA,int pSQLDALen);

// 为存放列数据的sd_column结构申请空间
int UnionAllocateHostVars(SQLDA *pSQLDA);

// 释放SQLDA所申请的空间
void UnionFreeSQLDA(SQLDA *pSQLDA);

// 获取列名信息
char * UnionReadColumnName(SQLDA *pSQLDA,short sqlVarIdx,char *pNameBuf);

// 获取列数据
char * UnionReadColumnValue(SQLDA *pSQLDA,short sqlVarIdx,char *pValueBuf);

#endif

/*
函数功能：
	设置数据库用户和密码
输入参数：无
输出参数：无
函数返回：0
*/
int UnionInitDatabaseConfig();

/*
函数功能：
	连接数据库
输入参数：
	timeout：超时(秒)，>=0超时时间，<0不设置超时
输出参数：无
函数返回：
0成功，<0失败
*/
int UnionConnectDatabaseWithTimeOut(int timeout);
int UnionConnectDatabase();
int UnionConnectDB();


/*
函数功能：
	断开连接
输入参数：无
输出参数：无
函数返回：sqlcode
*/
int UnionCloseDatabase();
int UnionCloseDB();

/*
函数功能：
	删除空格
输入参数：
	str:	字符串
输出参数：
	str:	字符串
函数返回：
*/
void UnionDeleteSpace(char *str);

char *UnionGetDBUserName();

int UnionGetDBUSER(char *dbuser);

int UnionGetDBPASSWD(char *dbpasswd);

int UnionGetDBNAME(char *dbname);

int UnionGetDBReconnectTime();

#endif

