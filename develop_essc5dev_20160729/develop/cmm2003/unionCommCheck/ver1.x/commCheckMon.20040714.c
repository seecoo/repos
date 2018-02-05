//	Author:		Wolfgang Wang
//	Date:		2003/09/22


#define _UnionTask_3_x_	
#define _UnionLogMDL_3_x_

#include <stdio.h>
#ifdef _AIX_
#include <cur00.h>
#else
#include <curses.h>
#endif
#include <signal.h>

#include "unionErrCode.h"
#include "UnionTask.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "commCheck.h"

#include "unionMonitor.h"
#include "unionVersion.h"
#include "unionCommand.h"

extern PUnionHostGroup	pghostGroup;
extern WINDOW		*pgContentWin;

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionKillMonitorTwinProcess();
	CloseTaskMonitor();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int RefreshHostWin(int interval)
{
	int	i;
	char	buf[128];
	int	hostNum = 0;
	int	ret;
		
	if (pgContentWin == NULL)
		return(errCodeParameter);
	
	if ((ret = UnionConnectHostGroup()) < 0)
	{
		UnionUserErrLog("in RefreshHostWin:: UnionConnectHostGroup!\n");
		return(ret);
	}

	memset(buf,0,sizeof(buf));
	memset(buf,' ',gColOfWin - 4);
	for (i = 1; i <= gLineOfWin - 2; i++)
	{
		wmove(pgContentWin,i,2);
		waddstr(pgContentWin,buf);
	}
	wmove(pgContentWin,1,2);
	if (pghostGroup->terminateCheckIfAllHostsDown)
		waddstr(pgContentWin,"当所有主机中止时----中止检查");
	else
		waddstr(pgContentWin,"当所有主机中止时----继续检查");
	sprintf(buf,"检测进程数目:%ld; 检查间隔(秒):%ld",pghostGroup->numOfChecker,pghostGroup->intervalPerChecking);
	wmove(pgContentWin,2,2);
	waddstr(pgContentWin,buf);
	for (i = 0,hostNum=0; i < conMaxHosts; i++)
	{
		if (!UnionIsValidIPAddrStr(pghostGroup->hosts[i].ipAddr))
			continue;
		sprintf(buf,"%15s %05d",pghostGroup->hosts[i].ipAddr,pghostGroup->hosts[i].port);
		if (!(pghostGroup->hosts[i].checked))
			sprintf(buf+strlen(buf)," 不检测 ");
		else
		{
			sprintf(buf+strlen(buf),"  检测  ");
			if (pghostGroup->hosts[i].active)
			{
				sprintf(buf+strlen(buf),"正常");
			}
			else
				sprintf(buf+strlen(buf),"异常");
		}
		wmove(pgContentWin,3+hostNum,2);
		if ((!pghostGroup->hosts[i].active) && (pghostGroup->hosts[i].checked))
		{
			wstandout(pgContentWin);
			waddstr(pgContentWin,buf);
			wstandend(pgContentWin);
		}
		else
			waddstr(pgContentWin,buf);
		hostNum++;
	}
	wrefresh(pgContentWin);
	return(0);
}

int main(int argc,char *argv[])
{
	int	interval;
	
	
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		interval = 1;
	else
	{
		if ((interval = abs(atoi(argv[1]))) <= 0)
			interval = 1;
	}
			
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"commCheckMon")) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());


	return(UnionMonitorMain(interval,"commCheckMon","在线任务监控程序",NULL,RefreshHostWin,UnionTaskActionBeforeExit));
}

int UnionHelp()
{
	printf("Usage:: %s [seconds]\n",UnionGetApplicationName());
	return(0);
}
