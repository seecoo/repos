#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"

#define MAX_SQL_KEY_LEN 128

// ÏòmemcachedÖÐÐ´ÈëÊý¾Ý¿â¼ÇÂ¼»º´æ
int UnionWriteUniqueDBRecIntoMemcached(char *sqlKey, int expiration, int lenOfRec, char *recBuf)
{
	UnionProgramerLog("in UnionWriteUniqueDBRecIntoMemcached:: ret = -1\n");
	return(-1);
}

// ´ÓmemcachedÖÐ¶ÁÎ¨Ò»Êý¾Ý¿â¼ÇÂ¼»º´æ
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

// »ñµÃmemchached´æ´¢Êý¾Ý¿â¼ÇÂ¼µÄ¹Ø¼ü×ÖsqlKey
int UnionGetDBSelectStatementKey(char *tableName, char *selectFld, char *condition, char *sqlKey)
{
	UnionProgramerLog("in UnionGetDBSelectStatementKey:: ret = -1\n");
	return(-1);
}

// Êý¾Ý¿â¶ÁÈ¡Êý¾ÝÊ¹ÓÃmemcached¿ª¹Ø, ±äÁ¿¶¨ÒåÎª1Ê±·µ»ØÕæ
int UnionIsUseDBMemcached()
{
	UnionProgramerLog("in UnionIsUseDBMemcached:: ret = -1\n");
	return(-1);
}

// »ñµÃÖ¸¶¨±íµÄmemcachedÊý¾ ´æ´¢Ê±¼ä
//  ¸ºÊý-1: ¸Ã±í²»´æ´¢ÔÚcache
//  ÕýÊýN:  Êý¾Ý´æ´¢NÃë
//  0Öµ:    ÓÀ¾Ã´æ´¢
int UnionGetDBMemcachedTableExpireTime(char *tableName)
{
	//UnionProgramerLog("in UnionGetDBMemcachedTableExpireTime:: ret = -1\n");
	return(-1);
}

