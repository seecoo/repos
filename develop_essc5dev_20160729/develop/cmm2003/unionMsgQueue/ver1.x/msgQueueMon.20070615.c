//	Author:		Wolfgang Wang
//	Date:		2003/09/22
//	Version:	1.0

#define _UnionTask_3_x_	
#define _UnionLogMDL_3_x_

#include <stdio.h>
#ifdef _AIX_
#include <cur00.h>
#else
#include <curses.h>
#endif
#include <signal.h>
#include <time.h>

#include "UnionTask.h"
#include "UnionLog.h"
#include "unionMsgQueue.h"
#include "unionMonitor.h"

extern PUnionMsgQueueGrp pgunionMsgQueueGrp;


extern WINDOW		*pgContentWin;

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionKillMonitorTwinProcess();
	CloseTaskMonitor();
	//UnionDisconnectMsgQueueMDL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int RefreshMsgQueueWin(int interval)
{
	int			i,j;
	char			Buf[128];
	PUnionMsgQueueRec	prec;
	int			queueNum = 0;
	
	if (pgContentWin == NULL)
		return(-1);
	
	if (pgunionMsgQueueGrp == NULL)
		return(-1);
	
	for (queueNum = 0,i = 0;;)
	{
		memset(Buf,0,sizeof(Buf));
		memset(Buf,' ',gColOfWin - 4);
		for (j = 1; j <= gLineOfWin - 2; j++)
		{
			wmove(pgContentWin,j,2);
			waddstr(pgContentWin,Buf);
		}
		memset(Buf,0,sizeof(Buf));
		sprintf(Buf,"%10s   %10s %10s %10s","用户标识号","系统标识号"," 消息数目 "," 字符数目 ");
		wmove(pgContentWin,(i % (gLineOfWin - 2)) + 1,2);
		waddstr(pgContentWin,Buf);
		++i;
		UnionQueryAllMsgQueueStatus();		
		for (;queueNum < pgunionMsgQueueGrp->num; queueNum++)
		{
			prec = &(pgunionMsgQueueGrp->queue[queueNum]);
			memset(Buf,0,sizeof(Buf));
			sprintf(Buf,"%010x %10d %10d %10ld",prec->userID,prec->systemID,prec->msgNum,prec->charNum);
			wmove(pgContentWin,(i % (gLineOfWin - 2))+1,2);
			waddstr(pgContentWin,Buf);
			++i;
			if ((i % (gLineOfWin - 2) == 0) && (i != 0))
			{
				queueNum++;
				break;
			}
		}
		wrefresh(pgContentWin);
		if (queueNum < pgunionMsgQueueGrp->num)
		{
			sleep(interval);
		}
		else
		{
			if (i <= gLineOfWin - 2)
				break;
			sleep(interval);
			break;
		}
	}
	return(0);
}


int main(int argc,char *argv[])
{
	int	interval = 1;
	
	UnionSetApplicationName(argv[0]);

	if (argc > 1)
	{
		if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
			return(UnionTaskActionBeforeExit());
	}
	if (argc >= 2)
		interval = atoi(argv[1]);
	if (interval <= 0)
		interval = 1;
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	return(UnionMonitorMain(interval,argv[0],"消息队列监控程序",UnionConnectMsgQueueMDL,RefreshMsgQueueWin,UnionTaskActionBeforeExit));
}

int UnionHelp()
{
	printf("Usage:: %s [seconds]\n",UnionGetApplicationName());
	return(0);
}
