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
	int				ret;

	if((ret = UnionSetMemcachedValue(sqlKey, recBuf, lenOfRec, expiration)) < 0)
	{
		UnionUserErrLog("in UnionWriteUniqueDBRecIntoMemcached:: UnionSetMemcachedValue sqlKey = [%s] ret = [%d]\n", sqlKey, ret);
		return(ret);
	}

	UnionAuditLog("in UnionWriteUniqueDBRecIntoMemcached:: UnionSetMemcachedValue sqlKey = [%s] expiration = [%d] OK!\n", sqlKey, expiration);
	return(ret);
}

// ´ÓmemcachedÖÐ¶ÁÎ¨Ò»Êý¾Ý¿â¼ÇÂ¼»º´æ
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

// »ñµÃmemchached´æ´¢Êý¾Ý¿â¼ÇÂ¼µÄ¹Ø¼ü×ÖsqlKey
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

// Êý¾Ý¿â¶ÁÈ¡Êý¾ÝÊ¹ÓÃmemcached¿ª¹Ø, ±äÁ¿¶¨ÒåÎª1Ê±·µ»ØÕæ
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

// »ñµÃÖ¸¶¨±íµÄmemcachedÊý¾ ´æ´¢Ê±¼ä
//  ¸ºÊý-1: ¸Ã±í²»´æ´¢ÔÚcache
//  ÕýÊýN:  Êý¾Ý´æ´¢NÃë
//  0Öµ:    ÓÀ¾Ã´æ´¢
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
		expTime = 60; // Ä¬ÈÏ´æ´¢60Ãë
	}
	else
	{
		expTime = ret;
	}

	memset(strBuf, 0, sizeof(strBuf));
	sprintf(strBuf, "specCacheHoldTimeOf%s", tableName);
	// ÎªÖ¸¶¨±í¶¨ÒåÌØ¶¨µÄ´æ´¢Ê±¼ä, -1Îª²»¶Ô¸Ã±í½øÐÐcache´æ´¢
	if((ret = UnionReadIntTypeRECVar(strBuf)) == -1 || ret >= 0)
	{
		expTime = ret;
	}
	UnionProgramerLog("in UnionGetDBMemcachedTableExpireTime:: tableName= [%s] expireTime = [%d]\n", tableName, expTime);

	return(expTime);

}

