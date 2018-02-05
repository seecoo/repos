#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"

#define MAX_SQL_KEY_LEN 128

// ��memcached��д�����ݿ��¼����
int UnionWriteUniqueDBRecIntoMemcached(char *sqlKey, int expiration, int lenOfRec, char *recBuf)
{
	UnionProgramerLog("in UnionWriteUniqueDBRecIntoMemcached:: ret = -1\n");
	return(-1);
}

// ��memcached�ж�Ψһ���ݿ��¼����
int UnionReadUniqueDDBRecFromMemcached(char *sqlKey, int sizeOfRecBuf, char *recBuf)
{
	UnionProgramerLog("in UnionReadUniqueDDBRecFromMemcached:: ret = -1\n");
	return(-1);
}

int UnionFormUniqueSQLStr(char *tableName, char *selectFld, char *condition, int sizeOfSqlStrBuf, char *sqlStr)
{
	UnionProgramerLog("in UnionFormUniqueSQLStr:: ret = -1\n");
	return(-1);
}

int UnionGenDBMemcachedKey(char *tableName, char *sql, char *sqlKey)
{
	UnionProgramerLog("in UnionGenDBMemcachedKey:: ret = -1\n");
	return(-1);
}

// ���memchached�洢���ݿ��¼�Ĺؼ���sqlKey
int UnionGetDBSelectStatementKey(char *tableName, char *selectFld, char *condition, char *sqlKey)
{
	UnionProgramerLog("in UnionGetDBSelectStatementKey:: ret = -1\n");
	return(-1);
}

// ���ݿ��ȡ����ʹ��memcached����, ��������Ϊ1ʱ������
int UnionIsUseDBMemcached()
{
	UnionProgramerLog("in UnionIsUseDBMemcached:: ret = -1\n");
	return(-1);
}

// ���ָ�����memcached��� �洢ʱ��
//  ����-1: �ñ��洢��cache
//  ����N:  ���ݴ洢N��
//  0ֵ:    ���ô洢
int UnionGetDBMemcachedTableExpireTime(char *tableName)
{
	//UnionProgramerLog("in UnionGetDBMemcachedTableExpireTime:: ret = -1\n");
	return(-1);
}

