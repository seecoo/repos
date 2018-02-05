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

// ���SQL������н��
int UnionCheckSQLError(char *pMsg, struct sqlca *pSQLCA);

// ���Ի�SQLDA
int UnionInitSQLDA(SQLDA **pSQLDA,int pSQLDALen);

// Ϊ��������ݵ�sd_column�ṹ����ռ�
int UnionAllocateHostVars(SQLDA *pSQLDA);

// �ͷ�SQLDA������Ŀռ�
void UnionFreeSQLDA(SQLDA *pSQLDA);

// ��ȡ������Ϣ
char * UnionReadColumnName(SQLDA *pSQLDA,short sqlVarIdx,char *pNameBuf);

// ��ȡ������
char * UnionReadColumnValue(SQLDA *pSQLDA,short sqlVarIdx,char *pValueBuf);

#endif

/*
�������ܣ�
	�������ݿ��û�������
�����������
�����������
�������أ�0
*/
int UnionInitDatabaseConfig();

/*
�������ܣ�
	�������ݿ�
���������
	timeout����ʱ(��)��>=0��ʱʱ�䣬<0�����ó�ʱ
�����������
�������أ�
0�ɹ���<0ʧ��
*/
int UnionConnectDatabaseWithTimeOut(int timeout);
int UnionConnectDatabase();
int UnionConnectDB();


/*
�������ܣ�
	�Ͽ�����
�����������
�����������
�������أ�sqlcode
*/
int UnionCloseDatabase();
int UnionCloseDB();

/*
�������ܣ�
	ɾ���ո�
���������
	str:	�ַ���
���������
	str:	�ַ���
�������أ�
*/
void UnionDeleteSpace(char *str);

char *UnionGetDBUserName();

int UnionGetDBUSER(char *dbuser);

int UnionGetDBPASSWD(char *dbpasswd);

int UnionGetDBNAME(char *dbname);

int UnionGetDBReconnectTime();

#endif

