/*
Author:	zhangyongding
Date:	20081223
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"

#define MAX_DB_PARAM_LEN	64

static int	gunionIsDBConfigInit = 0;

static char	gunionDBUSER[MAX_DB_PARAM_LEN] = "";
static char	gunionDBPASSWD[MAX_DB_PARAM_LEN] = "";
static char	gunionDBNAME[MAX_DB_PARAM_LEN] = "";
static int	gunionDBConfFlag = 0;
static int	gunionReconnectTime = 0;

int UnionGetDBUSER(char *dbuser)
{
	strcpy(dbuser, gunionDBUSER);
	return(0);
}

int UnionGetDBPASSWD(char *dbpasswd)
{
	int		len;
	strcpy(dbpasswd, gunionDBPASSWD);
	return(0);
}

int UnionGetDBNAME(char *dbname)
{
	strcpy(dbname, gunionDBNAME);
	return(0);
}

int UnionGetDBConfFlag()
{
	return(gunionDBConfFlag);
}

int UnionSetDBReconnectTime()
{
	char		*ptr = NULL;
	int		maxIdleTimeOfDBConn = 0;

	gunionReconnectTime = 60 * 10; // 10 minutes default.


	if ((maxIdleTimeOfDBConn = UnionReadIntTypeRECVar("maxIdleTimeOfDBConn")) > 0)
	{
		return(gunionReconnectTime = maxIdleTimeOfDBConn);
	}

	if((ptr = getenv("DBRECONNTIME")) != NULL)
	{
		gunionReconnectTime = atoi(ptr);
	}
	return(gunionReconnectTime);
}

int UnionGetDBReconnectTime()
{
	return(gunionReconnectTime);
}

/*
函数功能：
设置数据库用户和密码
输入参数：无
输出参数：无
函数返回：0
*/

int UnionInitDatabaseConfig()
{
	int			ret = 0;
	int			len;
	//char			passwdCiper[48+1];
	char			passwdCiper[128];
	//char			*ptr = NULL;

	if(gunionIsDBConfigInit)
	{
		return(0);
	}

	if((ret = UnionGetDatabaseConfig(gunionDBUSER, passwdCiper, gunionDBNAME)) < 0)
	{
		UnionUserErrLog("in UnionInitDatabaseConfig:: UnionGetDatabaseConfig ret = [%d] !!\n", ret);
		return(ret);
	}

	if((len = strlen(passwdCiper)) > 0)
	{
		if(len % 16 == 0)
		{
			UnionDecryptDatabasePassword(passwdCiper, gunionDBPASSWD);
		}
		else
		{
			UnionAuditLog("in UnionGetDBPASSWD:: DBPASSWD = [%s] invalid format!!\n", passwdCiper);
		}
	}
	else
		strcpy(gunionDBPASSWD,"");
	
	UnionAuditLog("in UnionInitDatabaseConfig:: UnionGetDatabaseConfig dbConf: ConfFlag = [%d] user/passwd@db = '%s/%c%.*s@%s'!!\n", gunionDBConfFlag, gunionDBUSER, gunionDBPASSWD[0], (strlen(gunionDBPASSWD) > 0) ? (strlen(gunionDBPASSWD) - 1) : 0, "******************************", gunionDBNAME);

	/*
	gunionReconnectTime = 60 * 10; // 10 minutes default.
	if((ptr = getenv("DBRECONNTIME")) != NULL)
	{
		gunionReconnectTime = atoi(ptr);
	}
	*/
	UnionSetDBReconnectTime();

	gunionIsDBConfigInit = 1;

	return(0);
}


/*
函数功能：
	获取数据库用户和密码
输入参数：
	无
输出参数：
	dbUser		用户名
	dbPasswd	密码
	dbName		数据库名
函数返回：0
*/
int UnionGetDatabaseConfig(char *dbUser,char *dbPasswd,char *dbName)
{
	int			flag = 0;
	int			ret = 0;
	char			*ptr = NULL;

	if((ptr = getenv("DBUSER")) == NULL)
	{
		UnionAuditLog("in UnionGetDatabaseConfig:: DBUSER is not define in .profile or .bash_profile\n");
		strcpy(dbUser,"");
	}
	else
	{
		strncpy(dbUser, ptr, MAX_DB_PARAM_LEN);
		flag = 1;
	}

	if((ptr = getenv("DBPASSWD")) == NULL)
	{
		UnionAuditLog("in UnionGetDatabaseConfig:: DBPASSWD is not define in .profile or .bash_profile\n");
		strcpy(dbPasswd,"");
	}
	else
	{
		strncpy(dbPasswd, ptr, 64+1);
		flag += 10;
	}
	
	if((ptr = getenv("DBNAME")) == NULL)
	{
		UnionAuditLog("in UnionGetDatabaseConfig:: DBNAME is not define in .profile or .bash_profile\n");
		strcpy(dbName,"");
	}
	else
	{
		strncpy(dbName, ptr, MAX_DB_PARAM_LEN);
		flag += 100;
	}

	switch(flag)
	{
		case   0:
		case   1:
		case  10:
			ret = errCodeEsscMDL_DBSvrNotDefined;
			UnionUserErrLog("in UnionGetDatabaseConfig:: DB var DBNAME,DBUSER,DBPASSWD defined Err!!\n");
			break;	
		case  11:
		case 100:
		case 101:
		case 111:
			ret = 0;
			UnionProgramerLog("in UnionGetDatabaseConfig:: DB var DBUSER=[%s] DBPASSWD=[%.2s...] DBNAME=[%s] defined flag = %d !!\n", dbUser, dbPasswd, dbName, flag);
			break;	
	}

	gunionDBConfFlag = flag;

	return(ret);
}

int UnionConvertStrIntoDBFldStr(char *oriStr,int lenOfOriStr,char *desStr,int sizeOfDesStr)
{
	int		len;
	char		tmpBuf[8192+2+1];

	len = snprintf(tmpBuf, sizeOfDesStr - 2, "'%s'", oriStr);
	strcpy(desStr, tmpBuf);
	return(len);
}

