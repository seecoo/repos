//	chenwd	
//	2015/11/02
// 	Version	1.0

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif
#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif
#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include <stdio.h>
#ifdef _AIX_
#include <cur00.h>
#else
#include <curses.h>
#endif
#include <signal.h>
#include <unistd.h>

#include "unionCommand.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "unionRealDBCommon.h"
#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"
#include "unionVersion.h"
#include "sjl06Grp.h"
#include "unionHsm.h"
#include "UnionTask.h"
#include "UnionLog.h"

#include "unionMonitor.h"


extern WINDOW		*pgContentWin;

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionKillMonitorTwinProcess();
	CloseTaskMonitor();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int RefreshHsmTransWin(int interval)
{
	int	i,j,num,ret;
	char	Buf[128];
	char	sql[128];
	char	hsmGroupID[128];
	char	hsmID[128];
	char	ipAddr[32];
	char	port[32];
	char	status[32];
	char	enabled[32];
	int	hsmNum;
	int	len;

	len = sprintf(sql,"select * from hsm");
	sql[len] = 0;
	if ((num = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in RefreshHsmTransWin:: UnionExecRealDBSql[%s]!\n",sql);
		return(num);
	}
	else if (num == 0)
	{
		UnionLog("in RefreshHsmTransWin:: hsm not find!\n");
	}

	if (pgContentWin == NULL)
		return(-1);

	for (hsmNum = 0,i = 0;;)
	{
		memset(Buf,0,sizeof(Buf));
		memset(Buf,' ',gColOfWin - 4);
		for (j = 1; j <= gLineOfWin - 2; j++)
		{
			wmove(pgContentWin,j,2);
			waddstr(pgContentWin,Buf);
		}
		memset(Buf,0,sizeof(Buf));
		sprintf(Buf,"%-16s%-16s%-16s%-6s%s  %s 第%04d页",
			"密码机组",
			"密码机ID",
			"IP地址",
			"端口",
			"状态",
			"激活",
			i / (gLineOfWin - 2) + 1);
		wmove(pgContentWin,(i % (gLineOfWin - 2)) + 1,2);
		waddstr(pgContentWin,Buf);
		++i;
		for (;hsmNum < num;hsmNum++)
		{
			UnionLocateXMLPackage("detail", i);
			// 密码机组		hsmGroupID
			if ((ret = UnionReadXMLPackageValue("hsmGroupID", hsmGroupID, sizeof(hsmGroupID))) < 0)
			{
				UnionUserErrLog("in UnionReadHsmInfo:: UnionReadXMLPackageValue[%s]!\n","hsmGroupID");
				return(ret);
			}
			hsmGroupID[ret] = 0;
			// 密码机ID		hsmID
			if ((ret = UnionReadXMLPackageValue("hsmID", hsmID, sizeof(hsmID))) < 0)
			{
				UnionUserErrLog("in UnionReadHsmInfo:: UnionReadXMLPackageValue[%s]!\n","hsmID");
				return(ret);
			}
			hsmID[ret] = 0;
			// IP地址		ipAddr
			if ((ret = UnionReadXMLPackageValue("ipAddr", ipAddr, sizeof(ipAddr))) < 0)
			{
				UnionUserErrLog("in UnionReadHsmInfo:: UnionReadXMLPackageValue[%s]!\n","ipAddr");
				return(ret);
			}
			ipAddr[ret] = 0;
			// 端口			port
			if ((ret = UnionReadXMLPackageValue("port", port, sizeof(port))) < 0)
			{
				UnionUserErrLog("in UnionReadHsmInfo:: UnionReadXMLPackageValue[%s]!\n","port");
				return(ret);
			}
			port[ret] = 0;
			// 状态			status
			if ((ret = UnionReadXMLPackageValue("status", status, sizeof(status))) < 0)
			{
				UnionUserErrLog("in UnionReadHsmInfo:: UnionReadXMLPackageValue[%s]!\n","status");
				return(ret);
			}
			status[ret] = 0;

			if(strcmp(status,"1") == 0)
			{
				ret = sprintf(status,"%s","正常");
				status[ret] = 0;
			}
			else if(strcmp(status,"0") == 0)
			{
				ret = sprintf(status,"%s","异常");
				status[ret] = 0;
			}

			// 激活			enabled
			if ((ret = UnionReadXMLPackageValue("enabled", enabled, sizeof(enabled))) < 0)
			{
				UnionUserErrLog("in UnionReadHsmInfo:: UnionReadXMLPackageValue[%s]!\n","enabled");
				return(ret);
			}
			enabled[ret] = 0;

			if(strcmp(enabled,"1") == 0)
			{
				ret = sprintf(enabled,"%s","激活");
				enabled[ret] = 0;
			}
			else if(strcmp(enabled,"0") == 0)
			{
				ret = sprintf(enabled,"%s","锁定");
				enabled[ret] = 0;
			}
			len = sprintf(Buf,"%-16s%-16s%-16s%-6s%s  %s",
				hsmGroupID,
				hsmID,
				ipAddr,
				port,
				status,
				enabled);
			Buf[len] = 0;

			wmove(pgContentWin,(i % (gLineOfWin - 2))+1,2);
			waddstr(pgContentWin,Buf);
			++i;
			if ((i % (gLineOfWin - 2) == 0) && (i != 0))
			{
				hsmNum++;
				break;	
			}
		}

		wrefresh(pgContentWin);
		if (hsmNum < num)
		{
			if (interval < 10)
				sleep(10);
			else
				sleep(interval);
		}
		else
		{
			if (i <= gLineOfWin - 2)
				break;
			if (interval < 10)
				sleep(10);
			else
				sleep(interval);
			break;
		}
	}	
			
	return(0);
}

int UnionHelp()
{
	printf("Usage:: %s [seconds]\n",UnionGetApplicationName());
	return(0);
}

int main(int argc,char **argv)
{
	int	interval;
	int	ret;
	
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		interval = 1;
	else
	{
		if ((interval = abs(atoi(argv[1]))) <= 0)
			interval = 1;
	}

	if((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in %s:: UnionCreateTaskInstance !\n", argv[0]);
		return(ret);
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	return(UnionMonitorMain(interval,argv[0],"密码机监控程序",NULL,RefreshHsmTransWin,UnionTaskActionBeforeExit));
}
	

