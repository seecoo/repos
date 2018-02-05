/*	
Author:	zhangyongding
Date:	20081223
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include <sqlca.h>
#include <sqlda.h>

#include "unionErrCode.h"

#include "unionTblDef.h"
#include "unionTableDef.h"
#include "UnionLog.h"

EXEC SQL include sqlca;
EXEC SQL include sqltypes;
EXEC SQL include blob;

struct sqlda *da_ptr;

jmp_buf jmp_continue;

void free_sqlda(struct sqlda *sqlda);


int UnionGetDBErrCode()
{
	int                     ret;

	ret = 0 - abs(sqlca.sqlcode);

	// 设计数据库错误码偏移
	// ret = ret + InformixErrCodeOffset .......

	return(UnionSetUserDefinedErrorCode(ret));
}

int UnionIsFatalDBErrCode(int dbErrCode)
{
	int     ret;
	char    varName[100];

	memset(varName, 0, sizeof varName);

	sprintf(varName, "fatalORACLEDBErr%d", abs(dbErrCode));
	if (UnionReadIntTypeRECVar(varName) > 0)
	{
		return(1);
	}

	return 0;
}

/* 把rtrim和ltrim替换为trim */
int UnionConverTrimStringofCondition(char *condition)
{
	/*
	int	ret;
	int	lenOfStr;
	int	offset = 0;	
	char	tmpBuf[8192+1];
	char	*pos = NULL;

	if (condition == NULL)
		return(0);

	lenOfStr = strlen(condition);
	// 替换rtrim和ltrim
	while (1)
	{
		if ((pos = strstr(condition,"rtrim")) == NULL)
			if ((pos = strstr(condition,"ltrim")) == NULL)
				break;

		memset(tmpBuf,0,sizeof(tmpBuf));	
		offset = pos - condition;
		memcpy(tmpBuf,condition,offset);
		memcpy(tmpBuf + offset,pos + 1,strlen(condition) - offset - 1);
		
		memcpy(condition,tmpBuf,strlen(tmpBuf) + 1);
	}

	return(strlen(tmpBuf));	
	*/
	return(0);
}


/*
函数功能：
连接数据库
输入参数：
timeout：超时(秒)，>=0超时时间，<0不设置超时
输出参数：无
函数返回：
0成功，<0失败
*/
int UnionConnectDatabaseWithTimeOut(int timeout)
{
	int				     ret;
	int				     seconds;

	ret		     =	       0;
	seconds		 =	       0;

	EXEC SQL BEGIN DECLARE SECTION;
	char	DBname[64];
	EXEC SQL END DECLARE SECTION;
	
	memset(DBname, 0, sizeof(DBname));
	UnionGetDBNAME(DBname);

	while (1)
	{
		EXEC SQL DATABASE :DBname;
		if ((ret = (0 - abs(sqlca.sqlcode))) < 0)
		{
			UnionUserErrLog("in UnionConnectDatabaseWithTimeOut:: DBname = [%s]\n",DBname);
			UnionUserErrLog("in UnionConnectDatabaseWithTimeOut:: database sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
			if ((timeout >= 0) && (seconds < timeout))
			{
				seconds++;
				sleep(1);
			}
			else if ((timeout >= 0) && (seconds >= timeout))
				break;
			else
				sleep(1);
		}
		else {
			return(0);
		}
	}

	return(UnionSetUserDefinedErrorCode(ret));
}

int UnionConnectDB()
{
	int	ret;
	
	EXEC SQL BEGIN DECLARE SECTION;
	char	DBname[64];
	char    DBuser[64];
	char    DBpassword[64];
	EXEC SQL END DECLARE SECTION;
	
	memset(DBname, 0, sizeof(DBname));
	memset(DBuser, 0, sizeof(DBuser));
	memset(DBpassword, 0, sizeof(DBpassword));
	UnionGetDBNAME(DBname);
	UnionGetDBUSER(DBuser);
	UnionGetDBPASSWD(DBpassword);

	if ((strlen(DBuser) == 0) || (strlen(DBpassword) == 0))
	{
		EXEC SQL DATABASE :DBname;
	}
	else
	{
    		/* 激活指定连接 */ 
		//EXEC SQL SET CONNECTION :DBcnname;
		EXEC SQL CONNECT TO :DBname user :DBuser using :DBpassword; 
	}
	//EXEC SQL CONNECT TO :DBname AS 'UNION_CONN' WITH CONCURRENT;
	if ((ret = (0 - abs(sqlca.sqlcode))) < 0)
	{
		UnionUserErrLog("in UnionConnectDB:: DBname = [%s]\n",DBname);
		UnionUserErrLog("in UnionConnectDB:: database sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		return(UnionSetUserDefinedErrorCode(ret));
	}

	UnionLog("in UnionConnectDB:: connect DB OK!\n");
	return(0);
}

int UnionConnectBackupDB()
{
	int	ret;
	
	/*声明宿主变量*/
	EXEC SQL BEGIN DECLARE SECTION;

	char	db_usr[128+1];
	char	db_pass[128+1];
	char	db_name[128+1];

	EXEC SQL END DECLARE SECTION;
		
	/* 设置连接备份数据库的用户名、密码 */
	memset(db_name,0,sizeof(db_name));
	strcpy(db_name,getenv("BACKUPDBNAME"));

	EXEC SQL DATABASE :db_name;
	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionConnectBackupDB:: db_name[%s]\n",db_name);
		UnionUserErrLog("in UnionConnectBackupDB:: database sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	
	return(0);
}

int UnionCloseDB()
{
	int	ret;

	EXEC SQL CLOSE DATABASE;
	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionCloseDB:: EXEC SQL ROLLBACK WORK RELEASE sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		//return(ret);
	}

	UnionLog("in UnionCloseDB:: close DB OK!\n");
	
	return(0);
}

int UnionCloseBackupDB()
{
	int	ret;

	EXEC SQL CLOSE DATABASE;
	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionCloseDatabase:: EXEC SQL ROLLBACK WORK RELEASE sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		//return(0);
	}

	return(0);
}

// 回滚事务
int UnionRollBackWork()
{
	int	ret;

	EXEC SQL ROLLBACK WORK;
	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionRollBackWork:: EXEC SQL ROLLBACK WORK sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
	}

	UnionLog("in UnionRollBackWork:: rollback work OK!\n");
	
	return(0);
}

int UnionSelectRealDBRecord(char *tableName,char *field,char *condition,char *record,int sizeOfRecord,char *fileName)
{
	int     numOfPerPage=0,currPage=0,isUseNewMech=1;
	int     beginNum=0,endNum=0;
	char    tmpStr[20+1];
	int	i;
	int	ret;
	int	sqlLen;
	FILE	*fp;
	
	EXEC SQL BEGIN DECLARE SECTION;
	int	count;
	char	sql[8192+1];
	char	v_tableName[128];
	EXEC SQL END DECLARE SECTION;
	
	memset(tmpStr, 0, sizeof tmpStr);

	//UnionAuditLog("in UnionSelectRealDBRecord::begin[%s][%s][%s]\n", tableName, field, condition);
	
	if ((ret = UnionConnectDatabase()) < 0)
	{
		UnionUserErrLog("in UnionSelectRealDBRecord:: UnionConnectDatabase!\n");
		return(ret);
	}

	// 替换rtrim和ltrim为trim
	if ((ret = UnionConverTrimStringofCondition(condition)) < 0)
	{
		UnionUserErrLog("in UnionSelectRealDBRecord:: UnionConverTrimStringofCondition condition[%s]!\n",condition);
		return(ret);
	}

	memset(tmpStr, 0, sizeof tmpStr);
	if (UnionReadRecFldFromRecStr(condition,strlen(condition),"currentPage",tmpStr,sizeof(tmpStr)) < 0)
	{
		isUseNewMech=0;
	}
	else
	{
		currPage = atoi(tmpStr);
	}
	memset(tmpStr, 0, sizeof tmpStr);
	if (UnionReadRecFldFromRecStr(condition,strlen(condition),"numOfPerPage",tmpStr,sizeof(tmpStr)) < 0)
	{
		isUseNewMech=0;
	}
	else
	{
		numOfPerPage = atoi(tmpStr);
	}
	UnionDeleteRecFldFromRecStr(condition,"currentPage");
	UnionDeleteRecFldFromRecStr(condition,"numOfPerPage");
	if (isUseNewMech)
	{
		//beginNum = (currPage-1)*numOfPerPage+1;
		beginNum = (currPage-1)*numOfPerPage;
		//endNum = beginNum + numOfPerPage;

		memset(sql, 0, sizeof sql);
		if ((condition == NULL) || (strlen(condition) == 0))
			sprintf(sql,"select count(*) from %s",tableName);
		else
			sprintf(sql,"select count(*) from %s where %s",tableName, condition);

		EXEC SQL PREPARE S FROM :sql;
		EXEC SQL DECLARE CUR_S CURSOR FOR S;
		EXEC SQL OPEN CUR_S;
		EXEC SQL FETCH CUR_S INTO :count;
		EXEC SQL CLOSE CUR_S;
		if (sqlca.sqlcode != 0)
		{
			UnionUserErrLog("in UnionSelectRealDBRecord:: sql=[%s],errcode=[%d]!\n",sql,sqlca.sqlcode);
			UnionCloseDatabase();
			return(UnionSetUserDefinedErrorCode(0-abs(sqlca.sqlcode)));
		}
		if ( count == 0)
		{
			UnionLog("in UnionSelectRealDBRecord::count is 0\n");
			if (fileName != NULL)
			{
				if ((fp = fopen(fileName,"w")) == NULL)
				{
					return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
				}
			}
			fclose(fp);
			return(0);
		}
	}

	memset(sql,0,sizeof(sql));
	if (isUseNewMech)
	{
		if ((condition == NULL) || (strlen(condition) == 0))
		{
			sprintf(sql,"select SKIP %d FIRST %d %s from %s",beginNum,numOfPerPage,field,tableName);
			/*
			if (strcmp(field, "*") != 0)
				sprintf(sql,"select SKIP %d FIRST %d %s from %s",beginNum,endNum,field,tableName);
			else
				sprintf(sql,"select %s from (select a.%s,rownum r from (select * from %s",field,field,tableName);
			*/
		}
		else
		{
			sprintf(sql,"select SKIP %d FIRST %d %s from %s where %s",beginNum,numOfPerPage,field,tableName,condition);
			/*
			if (strcmp(field, "*") != 0)
				sprintf(sql,"select %s from (select %s,rownum r from (select %s from %s where %s",
					field,field,field,tableName,condition);
			else
				sprintf(sql,"select %s from (select a.%s,rownum r from (select * from %s where %s",
					field,field,tableName,condition);
			*/
		}
		if ((fileName != NULL) && (strlen(fileName) != 0))
		{
			// 20120929 张永定修改,检查是否已经存在order by
			if (strstr(sql,"order by") == NULL)
			{
				sqlLen = strlen(sql);
				strcpy(sql+sqlLen," order by ");
				sqlLen += 10;
				if ((ret = UnionFormTablePrimaryKeyList(tableName,sql+sqlLen)) < 0)
				{
					UnionUserErrLog("in UnionSelectRealDBRecord:: UnionFormTablePrimaryKeyList!\n");
					return(ret);
				}
			}
		}
		//sprintf(sql, "%s) a  where rownum<=%d) where r>=%d ", sql, endNum, beginNum);
	}
	else
	{
		if ((condition == NULL) || (strlen(condition) == 0))
			sprintf(sql,"select %s from %s",field,tableName);
		else
			sprintf(sql,"select %s from %s where %s",field,tableName,condition);
	}
	
	
	if (!isUseNewMech)
	{
		if ((fileName != NULL) && (strlen(fileName) != 0))
		{
			// 20120929 张永定修改,检查是否已经存在order by
			if (strstr(sql,"order by") == NULL)
			{
				sqlLen = strlen(sql);
				strcpy(sql+sqlLen," order by ");
				sqlLen += 10;
				if ((ret = UnionFormTablePrimaryKeyList(tableName,sql+sqlLen)) < 0)
				{
					UnionUserErrLog("in UnionSelectRealDBRecord:: UnionFormTablePrimaryKeyList!\n");
					return(ret);
				}
			}
		}
	}

	UnionProgramerLog("in UnionSelectRealDBRecord:: sql=[%s]!\n",sql);

	/* 对该SQL语句做PREPARE操作 */
	EXEC SQL prepare selct_id from :sql;
	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionSelectRealDBRecord:: select sql = [%s]\n",sql);
		UnionUserErrLog("in UnionSelectRealDBRecord:: prepare sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		UnionCloseDatabase();
		return(UnionSetUserDefinedErrorCode(ret));
	}
	
	EXEC SQL describe selct_id into da_ptr;
	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionSelectRealDBRecord:: select sql = [%s]\n",sql);
		UnionUserErrLog("in UnionSelectRealDBRecord:: describe sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		return(UnionSetUserDefinedErrorCode(ret));
	}

	init_sqlda();

	/* 声明游标 */
	EXEC SQL DECLARE Curs CURSOR FOR selct_id;
	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionSelectRealDBRecord:: select sql = [%s]\n",sql);
		UnionUserErrLog("in UnionSelectRealDBRecord:: DECLARE CURSOR sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		UnionCloseDatabase();
		return(UnionSetUserDefinedErrorCode(ret));
	}
	/* 打开游标 */
	EXEC SQL OPEN Curs;
	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionSelectRealDBRecord:: select sql = [%s]\n",sql);
		UnionUserErrLog("in UnionSelectRealDBRecord:: OPEN CURSOR sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		UnionCloseDatabase();
		return(UnionSetUserDefinedErrorCode(ret));
	}

	/* 处理语句，并输出结果 */
	if ((ret = read_sqlda(tableName,record,sizeOfRecord,fileName,isUseNewMech,beginNum, endNum, count)) < 0)
	{
		free_sqlda(da_ptr);
		if ((ret == errCodeDatabaseMDL_RecordNotFound) && (fileName != NULL))
		{
			UnionProgramerLog("in UnionSelectRealDBRecord::no data found from [%s]!\n",tableName);
			return(0);
		}
		if (ret == errCodeDatabaseMDL_RecordNotFound)
			return(UnionSetUserDefinedErrorCode(ret));
		UnionUserErrLog("in UnionSelectRealDBRecord:: select sql = [%s]\n",sql);
		UnionCloseDatabase();
		return(UnionSetUserDefinedErrorCode(ret));
	}

	//UnionDebugLog("in UnionSelectRealDBRecord:: select success record = [%s]\n",record);
	/* Step 6: 释放申请的内存空间，如游标、SQLDA、创建的临时表等*/
	free_sqlda(da_ptr);
	
	return(ret);
}

int UnionSelectRealDBRecordCounts(char *tableName,char *condition)
{
	char    tmpStr[20+1];
	int     i;
	int     ret;
	int     sqlLen;
	FILE    *fp;

	EXEC SQL BEGIN DECLARE SECTION;
	int     count;
	char    sql[8192+1];
	char    v_tableName[128];
	EXEC SQL END DECLARE SECTION;

	memset(tmpStr, 0, sizeof tmpStr);

	if ((ret = UnionConnectDatabase()) < 0)
	{
		UnionUserErrLog("in UnionSelectRealDBRecordCounts:: UnionConnectDatabase!\n");
		return(ret);
	}

	// 替换rtrim和ltrim为trim
	if ((ret = UnionConverTrimStringofCondition(condition)) < 0)
	{
		UnionUserErrLog("in UnionSelectRealDBRecordCounts:: UnionConverTrimStringofCondition condition[%s]!\n",condition);
		return(ret);
	}

	memset(sql, 0, sizeof sql);
	if ((condition == NULL) || (strlen(condition) == 0))
		sprintf(sql,"select count(*) from %s",tableName);
	else
		sprintf(sql,"select count(*) from %s where %s",tableName, condition);

	EXEC SQL PREPARE S_1 FROM :sql;
	EXEC SQL DECLARE CUR_S_1 CURSOR FOR S_1;
	EXEC SQL OPEN CUR_S_1;
	EXEC SQL FETCH CUR_S_1 INTO :count;
	EXEC SQL CLOSE CUR_S_1;
	if (sqlca.sqlcode != 0)
	{
		UnionUserErrLog("in UnionSelectRealDBRecordCounts:: sql=[%s],errcode=[%d]!\n",sql,sqlca.sqlcode);
		UnionCloseDatabase();
		return(UnionSetUserDefinedErrorCode(0-abs(sqlca.sqlcode)));
	}
	UnionCloseDatabase();
	return(count);
}

int UnionUpdateRealDBRecord(char *tableName,char *field,char *condition)
{
	int	ret;

	EXEC SQL BEGIN DECLARE SECTION;
	char	sql[8192*2+1];
	EXEC SQL END DECLARE SECTION;
	
	if ((ret = UnionConnectDatabase()) < 0)
	{
		UnionLog("in UnionUpdateRealDBRecord:: UnionConnectDatabase!\n");
		return(ret);
	}

	// 替换rtrim和ltrim为trim
	if ((ret = UnionConverTrimStringofCondition(condition)) < 0)
	{
		UnionUserErrLog("in UnionUpdateRealDBRecord:: UnionConverTrimStringofCondition condition[%s]!\n",condition);
		return(ret);
	}

	memset(sql,0,sizeof(sql));
	if ((condition == NULL) || (strlen(condition) == 0))
		sprintf(sql,"update %s set %s",tableName,field);
	else
		sprintf(sql,"update %s set %s where %s",tableName,field,condition);
	
	UnionProgramerLog("in UnionUpdateRealDBRecord::sql=[%s]\n", sql);
	
	EXEC SQL prepare update_id from :sql;
	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionUpdateRealDBRecord:: update sql = [%s]\n",sql);
		UnionUserErrLog("in UnionUpdateRealDBRecord:: prepare sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		UnionCloseDatabase();
		return(UnionSetUserDefinedErrorCode(ret));
	}
	EXEC SQL execute update_id;
	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionUpdateRealDBRecord:: update sql = [%s]\n",sql);
		UnionUserErrLog("in UnionUpdateRealDBRecord:: execute sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		UnionCloseDatabase();
		return(UnionSetUserDefinedErrorCode(ret));
	}
	EXEC SQL free update_id;
	return(0);
}

int UnionInsertRealDBRecord(char *tableName,char *record,int lenOfRecord)
{
	int	ret;

	EXEC SQL BEGIN DECLARE SECTION;
	char	sql[8192*2+1];
	EXEC SQL END DECLARE SECTION;
	
	if ((ret = UnionConnectDatabase()) < 0)
	{
		UnionLog("in UnionInsertRealDBRecord:: UnionConnectDatabase!\n");
		return(ret);
	}

	memset(sql,0,sizeof(sql));
	sprintf(sql,"insert into %s %s",tableName,record);
	
	UnionProgramerLog("in UnionInsertRealDBRecord::sql=[%s]\n", sql);
	
	EXEC SQL prepare insert_id from :sql;
	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionInsertRealDBRecord:: insert sql = [%s]\n",sql);
		UnionUserErrLog("in UnionInsertRealDBRecord:: prepare sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		UnionCloseDatabase();
		return(UnionSetUserDefinedErrorCode(ret));
	}
	EXEC SQL execute insert_id;
	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionInsertRealDBRecord:: insert sql = [%s]\n",sql);
		UnionUserErrLog("in UnionInsertRealDBRecord:: execute sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		UnionCloseDatabase();
		if ((ret == -239) || (ret == -268))
			return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_PrimaryKeyIsRepeat));
		return(UnionSetUserDefinedErrorCode(ret));
	}
	EXEC SQL free insert_id;

	return(0);
}

int UnionDeleteRealDBRecord(char *tableName,char *condition)
{
	int	ret;

	EXEC SQL BEGIN DECLARE SECTION;
	char	sql[1024+1];
	EXEC SQL END DECLARE SECTION;
	
	if ((ret = UnionConnectDatabase()) < 0)
	{
		UnionLog("in UnionDeleteRealDBRecord:: UnionConnectDatabase!\n");
		return(ret);
	}

	// 替换rtrim和ltrim为trim
	if ((ret = UnionConverTrimStringofCondition(condition)) < 0)
	{
		UnionUserErrLog("in UnionDeleteRealDBRecord:: UnionConverTrimStringofCondition condition[%s]!\n",condition);
		return(ret);
	}

	memset(sql,0,sizeof(sql));
	if ( (condition == NULL) || (strlen(condition) == 0))
		sprintf(sql,"delete from %s",tableName);
	else
		sprintf(sql,"delete from %s where %s",tableName,condition);

	UnionProgramerLog("in UnionDeleteRealDBRecord::sql=[%s]\n", sql);

	EXEC SQL prepare delete_id from :sql;
	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionDeleteRealDBRecord:: delete sql = [%s]\n",sql);
		UnionUserErrLog("in UnionDeleteRealDBRecord:: prepare sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		UnionCloseDatabase();
		return(UnionSetUserDefinedErrorCode(ret));
	}
	EXEC SQL execute delete_id;
	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionDeleteRealDBRecord:: delete sql = [%s]\n",sql);
		UnionUserErrLog("in UnionDeleteRealDBRecord:: execute sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		UnionCloseDatabase();
		if (ret == -100)
			return (UnionSetUserDefinedErrorCode(errCodeDatabaseMDL_RecordNotFound));
		return(UnionSetUserDefinedErrorCode(ret));
	}
	EXEC SQL free delete_id;

	return(0);
}

int UnionExecRealDBSql(char *sql)
{
	int	ret;
	int	cnt;

	EXEC SQL BEGIN DECLARE SECTION;
	char	sqlStr[1024*2+1];
	EXEC SQL END DECLARE SECTION;

	if ((ret = UnionConnectDatabase()) < 0)
	{
		UnionLog("in UnionExecRealDBSql:: UnionConnectDatabase!\n");
		return(ret);
	}

	// 替换rtrim和ltrim为trim
	if ((ret = UnionConverTrimStringofCondition(sql)) < 0)
	{
		UnionUserErrLog("in UnionExecRealDBSql:: UnionConverTrimStringofCondition sql[%s]!\n",sql);
		return(ret);
	}

	memset(sqlStr, 0, sizeof(sqlStr));
	strcpy(sqlStr, sql);

	EXEC SQL EXECUTE IMMEDIATE :sqlStr;
	//UNIONCHECKERR("in UnionExecRealDBSql:: EXEC SQL EXECUTE IMMEDIATE :sqlStr");

	if ((sqlca.sqlcode == 100) || (sqlca.sqlcode == 1403))
	{
		EXEC SQL COMMIT;
		return 0;
	}

	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionExecRealDBSql:: EXECUTE IMMEDIATE sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		UnionUserErrLog("in UnionExecRealDBSql:: sqlStr = [%s]\n",sqlStr);
		EXEC SQL ROLLBACK;
		//UNIONCHECKERR ("in UnionExecRealDBSql:: EXEC SQL ROLLBACK");
		UnionCloseDatabase();
		return(UnionSetUserDefinedErrorCode(ret));
	}

	cnt = sqlca.sqlerrd[2];
	UnionProgramerLog("in UnionExecRealDBSql::cnt=[%d]\n", cnt);

	EXEC SQL COMMIT;
	//UNIONCHECKERR ("in UnionExecRealDBSql:: EXEC SQL COMMIT");
	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionExecRealDBSql:: sqlStr = [%s]\n", sqlStr);
		UnionUserErrLog("in UnionExecRealDBSql:: EXEC SQL COMMIT sqlca.sqlcode = [%d]\n", sqlca.sqlcode);
		EXEC SQL ROLLBACK;
		//UNIONCHECKERR("in UnionExecRealDBSql:: EXEC SQL ROLLBACK");
		UnionCloseDatabase();
		return(UnionSetUserDefinedErrorCode(ret));
	}

	return(cnt);
}

// Add By Huangbaoxin, 2009/10/15
int UnionOpenRealDBSql(char *sql, char *record, int sizeOfRecord)
{
	int	ret, i;
	int	null_ok, scale, precision;
	char	value[1024+1], strFieldName[128+1];
	struct	sqlvar_struct	*col_ptr;
	struct 	decimal	d1;
	double		d2;

	EXEC SQL BEGIN DECLARE SECTION;
	char	sqlStr[1024+1];
	EXEC SQL END DECLARE SECTION;
	
	if ((ret = UnionConnectDatabase()) < 0)
	{
		UnionUserErrLog("in UnionOpenRealDBSql:: UnionConnectDatabase!\n");
		return(ret);
	}

	// 替换rtrim和ltrim为trim
	if ((ret = UnionConverTrimStringofCondition(sql)) < 0)
	{
		UnionUserErrLog("in UnionOpenRealDBSql:: UnionConverTrimStringofCondition sql[%s]!\n",sql);
		return(ret);
	}

	memset(sqlStr, 0, sizeof(sqlStr));
	strcpy(sqlStr, sql);
	
	UnionProgramerLog("in UnionOpenRealDBSql::sqlStr=[%s]\n", sqlStr);
	
	/* 对该SQL语句做PREPARE操作 */
	EXEC SQL PREPARE openSelectCmd FROM :sqlStr;
	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionOpenRealDBSql:: sqlStr = [%s]\n",sqlStr);
		UnionUserErrLog("in UnionOpenRealDBSql:: prepare sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		UnionCloseDatabase();
		goto exitFlag;
	}
	EXEC SQL describe openSelectCmd into da_ptr;
	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionOpenRealDBSql:: sqlStr = [%s]\n",sql);
		UnionUserErrLog("in UnionOpenRealDBSql:: describe sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		UnionCloseDatabase();
		goto exitFlag;
	}
	init_sqlda();
	/*声明游标*/
	EXEC SQL DECLARE openSelectCur CURSOR FOR openSelectCmd;
 	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionOpenRealDBSql:: sqlStr = [%s]\n",sqlStr);
		UnionUserErrLog("in UnionOpenRealDBSql:: DECLARE CURSOR sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		UnionCloseDatabase();
		goto exitFlag;
	}
	
	/* 打开游标 */
	EXEC SQL OPEN openSelectCur;
 	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionOpenRealDBSql:: sqlStr = [%s]\n",sql);
		UnionUserErrLog("in UnionOpenRealDBSql:: OPEN CURSOR sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		UnionCloseDatabase();
		goto exitFlag;
	}

	// 读出数据
	EXEC SQL FETCH openSelectCur USING DESCRIPTOR da_ptr;
	if (ret = 0 - abs(sqlca.sqlcode))
	{
		if (SQLNOTFOUND == sqlca.sqlcode)
			goto exitFlag;
		UnionUserErrLog("in UnionOpenRealDBSql:: sqlca.sqlcode[%d]\n",sqlca.sqlcode);
		goto exitFlag;
	}
		
	for (i = 0,col_ptr = da_ptr->sqlvar; i < da_ptr->sqld;i++, col_ptr++)
	{				
		memset(value,0,sizeof(value));
		switch (col_ptr->sqltype)
		{
			// 数字型
			case CINTTYPE:
			case CLONGTYPE:
				if (!risnull(col_ptr->sqltype,col_ptr->sqldata))
					sprintf(value,"%d",*(int *)col_ptr->sqldata);
				break;
			// 金额型
			case CMONEYTYPE:
				if (!risnull(col_ptr->sqltype,col_ptr->sqldata))
				{
					d1 = *(struct decimal *)col_ptr->sqldata;
					dectodbl(&d1,&d2);
					sprintf(value,"%.2f",d2);
				}
				break;		
			// double型		
			case CDECIMALTYPE:
				if (!risnull(col_ptr->sqltype,col_ptr->sqldata))
				{
					d2 = 0.00;
					d1 = *(struct decimal *)col_ptr->sqldata;
					dectodbl(&d1,&d2);
					sprintf(value,"%f",d2);
				}
				break;	
			default:
				memcpy(value,col_ptr->sqldata,col_ptr->sqllen);
				break;
		}
		UnionFilterHeadAndTailBlank(value);
		
		sprintf(strFieldName, "FIELD%d", i);

		if ( (ret = UnionPutRecFldIntoRecStr(strFieldName, value, strlen(value),
				record + strlen(record), sizeOfRecord - strlen(record))) < 0 )
		{
			UnionUserErrLog("in UnionOpenRealDBSql:: UnionPutRecFldIntoRecStr [%s] [%04d] [%s]\n",
				strFieldName, strlen(value), value);
			goto exitFlag;
		}
	}
	
	UnionProgramerLog("in UnionOpenRealDBSql:: record=[%s]\n", record);
	ret = 0;
exitFlag:

	EXEC SQL free openSelectCmd;
	EXEC SQL close openSelectCur;
	EXEC SQL free openSelectCur;
	free_sqlda(da_ptr);
		
	return(ret);
}

/*
char *UnionGetDBUserName()
{
	return  DBname;
}
*/

int UnionSelectDBRecordByRealSQL(char *tableName, char *strSql, char *record, int sizeOfRecord, char *fileName)
{
	char    tmpStr[20+1];
	int	i;
	int	ret;
	int	sqlLen;
	FILE	*fp;
	
	EXEC SQL BEGIN DECLARE SECTION;
	char	sql[8192+1];
	char	v_tableName[128];
	EXEC SQL END DECLARE SECTION;
	
	memset(tmpStr, 0, sizeof tmpStr);

	//UnionAuditLog("in UnionSelectDBRecordByRealSQL::begin[%s][%s][%s]\n", tableName, field, condition);
	
	if ((ret = UnionConnectDatabase()) < 0)
	{
		UnionUserErrLog("in UnionSelectDBRecordByRealSQL:: UnionConnectDatabase!\n");
		return(ret);
	}

	/*
	if ((ret = alloc_descriptors(MAX_ITEMS, MAX_VNAME_LEN, MAX_INAME_LEN)) != 0)
	{
		UnionUserErrLog("in UnionSelectDBRecordByRealSQL:: alloc_descriptors!\n");
		UnionCloseDatabase();
		return(UnionSetUserDefinedErrorCode(ret));
	}

	*/
	memset(sql,0,sizeof(sql));
	strcpy(sql, strSql);

	// 替换rtrim和ltrim为trim
	if ((ret = UnionConverTrimStringofCondition(sql)) < 0)
	{
		UnionUserErrLog("in UnionSelectDBRecordByRealSQL:: UnionConverTrimStringofCondition sql[%s]!\n",sql);
		return(ret);
	}

	UnionProgramerLog("in UnionSelectDBRecordByRealSQL:: sql=[%s]!\n",sql);

	/* 对该SQL语句做PREPARE操作 */
	EXEC SQL prepare selct_realSql from :sql;
	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionSelectDBRecordByRealSQL:: select sql = [%s]\n",sql);
		UnionUserErrLog("in UnionSelectDBRecordByRealSQL:: prepare sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		UnionCloseDatabase();
		return(UnionSetUserDefinedErrorCode(ret));
	}
	EXEC SQL describe selct_realSql into da_ptr;
	{
		UnionUserErrLog("in UnionSelectDBRecordByRealSQL:: select sql = [%s]\n",sql);
		UnionUserErrLog("in UnionSelectDBRecordByRealSQL:: describe sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		UnionCloseDatabase();
		return(UnionSetUserDefinedErrorCode(ret));
	}
	init_sqlda();
	/* 声明游标 */
	EXEC SQL DECLARE selct_Curs CURSOR FOR da_ptr;
 	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionSelectDBRecordByRealSQL:: select sql = [%s]\n",sql);
		UnionUserErrLog("in UnionSelectDBRecordByRealSQL:: DECLARE CURSOR sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		UnionCloseDatabase();
		return(UnionSetUserDefinedErrorCode(ret));
	}
	/* 打开游标 */
	EXEC SQL OPEN selct_Curs;
 	if (ret = 0 - abs(sqlca.sqlcode))
	{
		UnionUserErrLog("in UnionSelectDBRecordByRealSQL:: select sql = [%s]\n",sql);
		UnionUserErrLog("in UnionSelectDBRecordByRealSQL:: OPEN CURSOR sqlca.sqlcode = [%d]\n",sqlca.sqlcode);
		UnionCloseDatabase();
		return(UnionSetUserDefinedErrorCode(ret));
	}

	/* 处理语句，并输出结果*/
	if ((ret = read_sqlda(tableName,record,sizeOfRecord,fileName,0,0, 0, 0)) < 0)
	{
		EXEC SQL CLOSE selct_Curs;
		free_sqlda(da_ptr);
		if (ret == errCodeDatabaseMDL_RecordNotFound)
		{
			UnionLog("in UnionSelectDBRecordByRealSQL::no data found from [%s]!\n",tableName);
			return(0);
		}
		UnionUserErrLog("in UnionSelectDBRecordByRealSQL:: select sql = [%s]\n",sql);
		UnionCloseDatabase();
		return(UnionSetUserDefinedErrorCode(ret));
	}
	
	UnionDebugLog("in UnionSelectDBRecordByRealSQL:: select success record = [%s]\n",record);
	/* Step 6: 释放申请的内存空间，如游标、SQLDA、创建的临时表等*/

	EXEC SQL CLOSE selct_Curs;
	free_sqlda(da_ptr);

	return(ret);

}

int init_sqlda()
{
	int i, j, row_size=0, msglen=0, num_to_alloc;
	struct sqlvar_struct *col_ptr;

	/* Step 1: 获得一行数据的长度 */
	for (i = 0, col_ptr = da_ptr->sqlvar; i < da_ptr->sqld; i++, col_ptr++)
	{
		/* msglen变量存放查询数据的所有列的长度和。*/
		msglen += col_ptr->sqllen; /* get database sizes */
		/* 为col_ptr->sqllen 重新赋值，该值是在C下的大小。如：在数据库中的字符串，在C中应该多一个字节空间来存放NULL的结束符。*/
		col_ptr->sqllen = rtypmsize(col_ptr->sqltype, col_ptr->sqllen);
		/*row_size变量存放了在C程序中的所有列的长度和。这个值是应用程序为存放一行数据所需要申请的内存空间*/
		row_size += col_ptr->sqllen;
	}
	/* Step 2: 设置FetArrSize值*/
	if (FetArrSize == -1) /* if FetArrSize not yet initialized */
	{
		if (FetBufSize == 0) /* if FetBufSize not set */
			FetBufSize = 4096; /* default FetBufSize */
		FetArrSize = FetBufSize/msglen;
	}
	num_to_alloc = (FetArrSize == 0)? 1: FetArrSize;
	/* 设置sqlvar_struct结构中的数据类型为相应的C的数据类型*/
	for (i = 0, col_ptr = da_ptr->sqlvar; i < da_ptr->sqld; i++, col_ptr++)
	{	
		switch(col_ptr->sqltype)
		{
			case SQLCHAR:		// 0
			case SQLDTIME:		// 10
				col_ptr->sqltype = CCHARTYPE;
				break;
			case SQLSMINT:		// 1
				col_ptr->sqltype = CSHORTTYPE;
				break;				
			case SQLINT:		// 2
			case SQLDATE:		// 7
				col_ptr->sqltype = CINTTYPE;
				break;
			case SQLFLOAT:		// 3
			case SQLSMFLOAT:	// 4
			case SQLDECIMAL:	// 5
				col_ptr->sqltype = CDECIMALTYPE;
				break;
			case SQLSERIAL:		// 6
				col_ptr->sqltype = CINTTYPE;
				break;
			case SQLMONEY:		// 8
				col_ptr->sqltype = CMONEYTYPE;
				break;
			case SQLVCHAR:		// 13
				col_ptr->sqltype = CVCHARTYPE;
				break;				
			default: /* 其他数据类型*/
				col_ptr->sqltype = CCHARTYPE;
				break;
		} /* switch */
		/* Step 5: 为指示符变量申请空间*/
		col_ptr->sqlind = (short *) malloc(sizeof(short) * num_to_alloc);
		if (!col_ptr->sqlind)
		{
			UnionUserErrLog("in init_sqlda:: malloc error\n");
			return(-1);
		}
		/* Step 6 ：为存放非TEXT 和BLOB的数据类型的sqldata申请空间.注意的是，申请的地址是(char *)，在输出数据时，要按照相应的数据类型做转换。*/
		if (col_ptr->sqltype != CLOCATORTYPE)
		{
			col_ptr->sqldata = (char *) malloc(col_ptr->sqllen * num_to_alloc);
			if (!col_ptr->sqldata)
			{
				UnionUserErrLog("in init_sqlda:: sqldata malloc error\n");
				return(-1);
			}
		}
	} /* end for */
	return msglen;
}

int read_sqlda(char *tableName,char *record,int sizeOfRecord,char *fileName,int isUseNewMech, int beginNum, int endNum, int totalNum)
{
	int		i,j;
	int		ret;
	int		count = 0;
	int		offset = 0;
	int		len = 0;
	char		tmpBuf[128+1];
	char		buf[8192*2+1];
	char		value[8192*2+1];
	double		d2;
	FILE		*fp = NULL;
	struct 	decimal	d1;
	dtime_t		t1;
	struct	sqlvar_struct	*col_ptr;
	PUnionTableDef	object = NULL;
	
	if ((record == NULL) && (fileName == NULL))
	{
		UnionUserErrLog("in read_sqlda:: record && fileName is null\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}

	if((object = UnionFindTableDef(tableName)) == NULL)
	{
		ret = UnionSetUserDefinedErrorCode(errCodeParameter);	
		UnionUserErrLog("in read_sqlda :: tableName = [%s] not found in image!\n", tableName);
		return(ret);
	}

	if (fileName != NULL)
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionUserErrLog("in read_sqlda:: fopen [%s] error\n",fileName);
			return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
		}
	}

	/* 取出每一行数据*/
	for (;;)
	{
		EXEC SQL FETCH Curs USING DESCRIPTOR da_ptr;
		if (ret = 0 - abs(sqlca.sqlcode))
		{
			if (SQLNOTFOUND == sqlca.sqlcode)
				break;
			UnionUserErrLog("in read_sqlda:: sqlca.sqlcode[%d]\n",sqlca.sqlcode);
			if (fp != NULL)
			{
				fflush(fp);
				fclose(fp);
			}
			return(UnionSetUserDefinedErrorCode(ret));
		}

		count ++;
		offset = 0;
		memset(buf,0,sizeof(buf));
		for (i = 0,col_ptr = da_ptr->sqlvar; i < da_ptr->sqld;i++, col_ptr++)
		{				
			memset(value,0,sizeof(value));
			switch (col_ptr->sqltype)
			{
				// 数字型
				case CINTTYPE:
				case CLONGTYPE:
					if (!risnull(col_ptr->sqltype,col_ptr->sqldata))
						sprintf(value,"%d",*(int *)col_ptr->sqldata);
					break;
				// 金额型
				case CMONEYTYPE:
					if (!risnull(col_ptr->sqltype,col_ptr->sqldata))
					{
						d1 = *(struct decimal *)col_ptr->sqldata;
						dectodbl(&d1,&d2);
						sprintf(value,"%.2f",d2);
					}
					break;		
				// double型		
				case CDECIMALTYPE:
					if (!risnull(col_ptr->sqltype,col_ptr->sqldata))
					{
						d2 = 0.00;
						d1 = *(struct decimal *)col_ptr->sqldata;
						dectodbl(&d1,&d2);
						sprintf(value,"%f",d2);
					}
					break;	
				default:
					memcpy(value,col_ptr->sqldata,col_ptr->sqllen);
					break;
			}
			//UnionLog("in read_sqlda:: col_ptr->sqlname[%s] col_ptr->sqltype [%d] value[%s]\n",col_ptr->sqlname,col_ptr->sqltype,value);
			UnionFilterHeadAndTailBlank(value);
			memset(tmpBuf,0,sizeof(tmpBuf));
			strcpy(tmpBuf,col_ptr->sqlname);
			UnionFilterHeadAndTailBlank(tmpBuf);
			UnionConvertObjectFldName(object,tmpBuf,tmpBuf);

			//UnionLog("in read_sqlda:: UnionPutRecFldIntoRecStr [%s] [%04d] [%s]\n",tmpBuf,strlen(value),value);
			if ((ret = UnionPutRecFldIntoRecStr(tmpBuf,value,strlen(value),buf+offset,sizeof(buf)-offset)) < 0)
			{
				UnionUserErrLog("in read_sqlda:: UnionPutRecFldIntoRecStr [%s] [%04d] [%s]\n",tmpBuf,strlen(value),value);
				if (fp != NULL)
					fclose(fp);
				return(ret);
			}
			offset += ret;
		}
		
		if (fp != NULL)
		{
			strcat(buf,"\n");
			fputs(buf, fp);	
		}
		else
		{
			if (count > 1)
				return(UnionSetUserDefinedErrorCode(errCodeDatabaseMDL_MoreRecordFound));
			
			if (strlen(buf) > sizeOfRecord - 1)
			{
				UnionUserErrLog("in read_sqlda:: lenOfBuf[%d] >  sizeOfRecord - 1[%d]\n",strlen(buf),sizeOfRecord - 1);
				return(UnionSetUserDefinedErrorCode(errCodeParameter));
			}
			strcpy(record,buf);
		}
	}
	
	if (count == 0) 
	{
		if (fp != NULL)
			fclose(fp);
		return(UnionSetUserDefinedErrorCode(errCodeDatabaseMDL_RecordNotFound));
	}

	if (fp != NULL)
	{
		if (isUseNewMech)
		{
			memset(buf, 0, sizeof buf);
			sprintf(buf, "%s=%d", "totalRecNum", totalNum);
			fputs(buf,fp);
		}
		fflush(fp);
		fclose(fp);
		return(count);
	}
	else
		return(strlen(record));
}


void free_sqlda(struct sqlda *sqlda)
{
	int i,j, num_to_dealloc;
	struct sqlvar_struct *col_ptr;
        if(sqlda == NULL)
        {
                return;
        }

	for (i = 0, col_ptr = sqlda->sqlvar; i < sqlda->sqld; i++, col_ptr++)
	{
		if(col_ptr->sqldata != NULL)
                { 
			free(col_ptr->sqldata);
		}
	}
	/* Free memory for sqlda structure */

	/* 关闭游标*/
	EXEC SQL CLOSE Curs;

	free(sqlda);
}

int UnionGetDBSequenceValueOfICCardID(char *certID)
{
	int			ret;

	ret = errCodeObjectMDL_ObjectDefinitionNotExists;
	UnionProgramerLog("in UnionGetDBSequenceValueOfICCardID:: not support!!!! ret = [%d]\n", ret);
        return(UnionSetUserDefinedErrorCode(ret));
}

int UnionSelectOneFieldBySQL(char *fldDescription, char *tableName, char *condition, int lenOfResultBuf, char *resultBuf)
{      
	int			ret;


	char			sql[2048];
	char			result[4096];

	memset(sql, 0, sizeof(sql));
	memset(result, 0, sizeof(result));

	if(condition == NULL || strlen(condition) == 0)
	{
		sprintf(sql, "select %s from %s", fldDescription, tableName);
	}
        else
	{
		sprintf(sql, "select %s from %s where %s", fldDescription, tableName, condition);
	}
	UnionProgramerLog("in UnionSelectOneFieldBySQL:: excute sql = [%s]!\n", sql);

	strncpy(resultBuf, result, lenOfResultBuf);

	if ((ret = UnionOpenRealDBSql(sql, result, sizeof(result))) < 0)
	{
		UnionUserErrLog("in UnionSelectOneFieldBySQL:: UnionOpenRealDBSql sql=[%s] return=%d Error\n", sql, ret);
		return(ret);
	}
	UnionRTrim(result);

	if ((ret = UnionReadRecFldFromRecStr(result, strlen(result), "FIELD0", resultBuf, lenOfResultBuf) < 0))
        {
		UnionUserErrLog("in UnionSelectOneFieldBySQL:: UnionReadRecFldFromRecStr fld = %s ret = %d error !\n","FIELD0", ret);
		return(ret);
	}
	return(ret);
}
