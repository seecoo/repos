#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"

#define MAX_SQL_KEY_LEN 128

// 向memcached中写入数据库记录缓存
int UnionWriteUniqueDBRecIntoMemcached(char *sqlKey, int expiration, int lenOfRec, char *recBuf)
{
	UnionProgramerLog("in UnionWriteUniqueDBRecIntoMemcached:: ret = -1\n");
	return(-1);
}

// 从memcached中读唯一数据库记录缓存
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

// 获得memchached存储数据库记录的关键字sqlKey
int UnionGetDBSelectStatementKey(char *tableName, char *selectFld, char *condition, char *sqlKey)
{
	UnionProgramerLog("in UnionGetDBSelectStatementKey:: ret = -1\n");
	return(-1);
}

// 数据库读取数据使用memcached开关, 变量定义为1时返回真
int UnionIsUseDBMemcached()
{
	UnionProgramerLog("in UnionIsUseDBMemcached:: ret = -1\n");
	return(-1);
}

// 获得指定表的memcached数� 存储时间
//  负数-1: 该表不存储在cache
//  正数N:  数据存储N秒
//  0值:    永久存储
int UnionGetDBMemcachedTableExpireTime(char *tableName)
{
	//UnionProgramerLog("in UnionGetDBMemcachedTableExpireTime:: ret = -1\n");
	return(-1);
}

