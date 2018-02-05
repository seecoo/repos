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
#include <unistd.h>

#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"
#include "UnionLog.h"

#include "unionMonitor.h"

extern PUnionTaskClass		pgunionTaskClass;
extern PUnionTaskTBL		pgunionTaskTBL;

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

int RefreshTaskWin(int interval)
{
	int	i,j;
	char	Buf[128];
	int	taskNum = 0;
	int	len;
	
	if (pgContentWin == NULL)
		return(-1);
	
	if (pgunionTaskClass == NULL)
		return(-1);
	
	for (taskNum = 0,i = 0;;)
	{
		memset(Buf,0,sizeof(Buf));
		memset(Buf,' ',gColOfWin - 4);
		for (j = 1; j <= gLineOfWin - 2; j++)
		{
			wmove(pgContentWin,j,2);
			waddstr(pgContentWin,Buf);
		}
		memset(Buf,0,sizeof(Buf));
		sprintf(Buf,"第%04d页%35s %8s %8s",
			i / (gLineOfWin - 2) + 1,
			"      任    务    名    称      ",
			"当前任务",
			"必须任务");
		wmove(pgContentWin,(i % (gLineOfWin - 2)) + 1,2);
		waddstr(pgContentWin,Buf);
		++i;
		for (;taskNum < pgunionTaskTBL->maxClassNum; taskNum++)
		{
			if (strlen((pgunionTaskClass+taskNum)->name) == 0)
				continue;
			if ((pgunionTaskClass+taskNum)->currentNum == 0)
				continue;
			memset(Buf,0,sizeof(Buf));
			sprintf(Buf,"%s",(pgunionTaskClass+taskNum)->name);
			len = strlen(Buf);
			memset(Buf+len,' ',40 - len);
			len = 40;
			memcpy(Buf+len," ",1);
			++len;
			sprintf(Buf+len,"  %4d     %4d",
				(pgunionTaskClass+taskNum)->currentNum,
				(pgunionTaskClass+taskNum)->minNum);
			wmove(pgContentWin,(i % (gLineOfWin - 2))+1,2);
			waddstr(pgContentWin,Buf);
			++i;
			if ((i % (gLineOfWin - 2) == 0) && (i != 0))
			{
				taskNum++;
				break;
			}
		}
		wrefresh(pgContentWin);
		if (taskNum < pgunionTaskTBL->maxClassNum)
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

int main(int argc,char *argv[])
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

	/*
	if (UnionExistsAnotherTaskOfName("taskMon"))
	{
		printf("Another task started!\n");
		return(UnionTaskActionBeforeExit());
	}
				
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"taskMon")) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	*/
	if((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in %s:: UnionCreateTaskInstance !\n", argv[0]);
		return(ret);
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);


	return(UnionMonitorMain(interval,"taskMon","在线任务监控程序",NULL,RefreshTaskWin,UnionTaskActionBeforeExit));
}

int UnionHelp()
{
	printf("Usage:: %s [seconds]\n",UnionGetApplicationName());
	return(0);
}
