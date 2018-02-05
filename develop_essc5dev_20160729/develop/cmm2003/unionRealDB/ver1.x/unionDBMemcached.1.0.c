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
	int				ret;

	if((ret = UnionSetMemcachedValue(sqlKey, recBuf, lenOfRec, expiration)) < 0)
	{
		UnionUserErrLog("in UnionWriteUniqueDBRecIntoMemcached:: UnionSetMemcachedValue sqlKey = [%s] ret = [%d]\n", sqlKey, ret);
		return(ret);
	}

	UnionAuditLog("in UnionWriteUniqueDBRecIntoMemcached:: UnionSetMemcachedValue sqlKey = [%s] expiration = [%d] OK!\n", sqlKey, expiration);
	return(ret);
}

// 从memcached中读唯一数据库记录缓存
int UnionReadUniqueDDBRecFromMemcached(char *sqlKey, int sizeOfRecBuf, char *recBuf)
{
	int				ret;

	if((ret = UnionGetMemcachedValue(sqlKey, recBuf, sizeOfRecBuf)) < 0)
	{
		UnionAuditLog("in UnionReadUniqueDDBRecFromMemcached:: UnionGetMemcachedValue sqlKey = [%s] ret = [%d]\n",sqlKey, ret);
		return(ret);
	}

	UnionProgramerLog("in UnionReadUniqueDDBRecFromMemcached:: UnionGetMemcachedValue sqlKey = [%s] recStr = [%s]\n", sqlKey, recBuf);
	return(ret);
}

int UnionFormUniqueSQLStr(char *tableName, char *selectFld, char *condition, int sizeOfSqlStrBuf, char *sqlStr)
{
	return(snprintf(sqlStr, sizeOfSqlStrBuf, "SELECT %s FROM %s WHERE %s", selectFld, tableName, condition));
}

int UnionGenDBMemcachedKey(char *tableName, char *sql, char *sqlKey)
{
	char            keyBuf[MAX_SQL_KEY_LEN+1];
	int             keyLen = 0;

	memset(keyBuf, 0, sizeof(keyBuf));

	UnionMD5(sql, strlen(sql), keyBuf);
	keyLen = snprintf(sqlKey, MAX_SQL_KEY_LEN, "DB_%s_%s", tableName, keyBuf);
	UnionProgramerLog("in UnionGenDBMemcachedKey:: tableName = [%s] sql = [%s] sqlKey = [%s]\n", tableName, sql, sqlKey);

	return(keyLen);
}

// 获得memchached存储数据库记录的关键字sqlKey
int UnionGetDBSelectStatementKey(char *tableName, char *selectFld, char *condition, char *sqlKey)
{
	char			*ptableName = NULL;
	char			*pselectFld = NULL;
	char			*pcondition = NULL;
	char			*BLANK = "";
	char			tmpBuf[4096];

	if((ptableName = tableName) == NULL)
	{
		ptableName = BLANK;
	}

	if((pselectFld = selectFld) == NULL)
	{
		pselectFld = BLANK;
	}

	if((pcondition = condition) == NULL)
	{
		pcondition = BLANK;
	}

	memset(tmpBuf, 0, sizeof(tmpBuf));
	UnionFormUniqueSQLStr(ptableName, pselectFld, pcondition, sizeof(tmpBuf) - 1, tmpBuf);

	return(UnionGenDBMemcachedKey(ptableName, tmpBuf, sqlKey));
}

// 数据库读取数据使用memcached开关, 变量定义为1时返回真
int UnionIsUseDBMemcached()
{

	if(UnionReadIntTypeRECVar("isUseDBMemchached") == 1)
	{
		return(1);
	}
	else
	{
		return(0);
	}

}

// 获得指定表的memcached数� 存储时间
//  负数-1: 该表不存储在cache
//  正数N:  数据存储N秒
//  0值:    永久存储
int UnionGetDBMemcachedTableExpireTime(char *tableName)
{
	int		ret, expTime;
	char            strBuf[64];

	if(!UnionIsUseDBMemcached())
	{
		return(-1);
	}

	memset(strBuf, 0, sizeof(strBuf));
	sprintf(strBuf, "maxHoldTimeOfHighCached");
	if((ret = UnionReadIntTypeRECVar(strBuf)) < 0)
	{
		expTime = 60; // 默认存储60秒
	}
	else
	{
		expTime = ret;
	}

	memset(strBuf, 0, sizeof(strBuf));
	sprintf(strBuf, "specCacheHoldTimeOf%s", tableName);
	// 为指定表定义特定的存储时间, -1为不对该表进行cache存储
	if((ret = UnionReadIntTypeRECVar(strBuf)) == -1 || ret >= 0)
	{
		expTime = ret;
	}
	UnionProgramerLog("in UnionGetDBMemcachedTableExpireTime:: tableName= [%s] expireTime = [%d]\n", tableName, expTime);

	return(expTime);

}

