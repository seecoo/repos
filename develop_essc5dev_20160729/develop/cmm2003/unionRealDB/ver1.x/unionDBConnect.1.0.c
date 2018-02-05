#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionRealBaseDB.h"
#include "unionRealDBCommon.h"
#include "unionREC.h"

static int		gunionDatabaseIndex = 0;
static int		gunionDatabaseConnected[DB_NUM] = {0};
static int		gunionPidConnectToDB = -1;
static int		gunionDatabaseLastConnTime = 0;

// 设置数据库索引，为了支持连接多个数据库
void UnionSetDatabaseIndex(int index)
{
	if ((index < 0) || (index >= DB_NUM))
		gunionDatabaseIndex = 0;
	else
		gunionDatabaseIndex = index;
}

int UnionGetDatabaseIndex()
{
	return(gunionDatabaseIndex);
}

void UnionSetConnectFlag(int flag)
{
	gunionDatabaseConnected[gunionDatabaseIndex] = flag;
}

int UnionSetDatabaseLastConnTime()
{
	gunionDatabaseLastConnTime = UnionGetCurrentSecondTime();
	return(gunionDatabaseLastConnTime);
}

int UnionGetDatabaseLastConnTime()
{
	return(gunionDatabaseLastConnTime);
}

int UnionIsDatabaseLinkTimeout()
{
	int			timeout;

	timeout = UnionGetDBReconnectTime();

	if(timeout == 0)
	{
		return(0);
	}
	
	if(UnionGetCurrentSecondTime() - UnionGetDatabaseLastConnTime() > timeout)
	{
		return(1);
	}

	return(0);
}

int UnionIsConnectDatabaseOK()
{                       
	if (gunionDatabaseConnected[gunionDatabaseIndex] && (gunionPidConnectToDB == getpid()))
	{
		return(1);
	}
	else
	{
		return(0);
	}
}

static void timeout_close_database(int signo)
{
	UnionLog("in timeout_close_database:: close database!\n");
	UnionCloseDatabase();
	return;
}
 
static void init_database_sigaction(void)
{
	struct sigaction	act;
 
	act.sa_handler = timeout_close_database;
	act.sa_flags   = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGPROF, &act, NULL);
	return;
}
 
static void init_database_time(int timeout)
{
	struct itimerval val;
 
	val.it_value.tv_sec = timeout;
	val.it_value.tv_usec = 0;
	val.it_interval = val.it_value;
	setitimer(ITIMER_PROF, &val, NULL);
	return;
}

int UnionConnectDatabase()
{
	int	ret;
	int	maxIdleTimeOfDBConn;

	if(UnionIsConnectDatabaseOK())
	{
		if(UnionIsDatabaseLinkTimeout())
		{
        		UnionCloseDB();
        		if ((ret = UnionConnectDB()) < 0)
			{
				UnionUserErrLog("in UnionConnectDatabase:: UnionConnectDB ret = [%d]!\n", ret);
				return(ret);
			}
			UnionAuditLog("in UnionConnectDatabase:: reconnectDB OK!\n");
			UnionSetDatabaseLastConnTime();
		}
		UnionSetDatabaseLastConnTime(); // added 2016-05-31 刷新时间 
		return(0);
	}

	if ((ret = UnionInitDatabaseConfig()) < 0)
	{
		return(ret);
	}

        if ((ret = UnionConnectDB()) < 0)
        {
		UnionUserErrLog("in UnionConnectDatabase:: UnionConnectDB ret = [%d]!\n", ret);
		return(ret);
        }

	UnionSetDatabaseLastConnTime();
	gunionDatabaseConnected[gunionDatabaseIndex] = 1;
	//if (gunionPidConnectToDB <= 0)
		gunionPidConnectToDB = getpid();

	/*
	if ((maxIdleTimeOfDBConn = UnionReadIntTypeRECVar("maxIdleTimeOfDBConn")) < 0)
		maxIdleTimeOfDBConn = 60;
		
	init_database_sigaction();
	init_database_time(maxIdleTimeOfDBConn);
	*/
	
	UnionLog("in UnionConnectDatabase:: connect DB OK!\n");

	return(0);
}

int UnionCloseDatabase()
{
	int		ret = 0;

	if (!UnionIsConnectDatabaseOK())
	{
		return(0);
	}

	if ((ret = UnionCloseDB()) < 0)
	{
		UnionUserErrLog("in UnionCloseDatabase:: UnionCloseDB ret = [%d]\n", ret);
	}

	gunionDatabaseConnected[gunionDatabaseIndex] = 0;
	//gunionPidConnectToDB = -1;

	UnionLog("in UnionCloseDatabase:: close DB OK!\n");

	return(ret);
}

int UnionCloseDatabaseBySpecErrCode(int dbErrCode)
{
	if(UnionIsFatalDBErrCode(dbErrCode))
	{
		UnionAuditLog("in UnionCloseDatabaseBySpecErrCode:: meet sqlcode = [%d] UnionCloseDB\n", dbErrCode);
		UnionCloseDatabase();
	}

	return 0;
}

