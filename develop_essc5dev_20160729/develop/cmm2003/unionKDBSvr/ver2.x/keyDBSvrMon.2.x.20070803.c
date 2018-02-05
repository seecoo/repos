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
#include "kdbSvrService.h"
#include "unionMonitor.h"
#include "unionVersion.h"

extern PUnionKDBSvrBufHDL		pgunionKDBSvrBufHDL;
extern unsigned char			*pgunionKDBSvrBuf;

extern WINDOW		*pgContentWin;

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionKillMonitorTwinProcess();
	CloseTaskMonitor();
	UnionDisconnectKDBSvrBufMDL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int RefreshExistMsgWin(int interval)
{
	int	i,j;
	char	Buf[128];
	PUnionKDBSvrServiceHeader	pheader;
	int	posNum = 0;
	int	len;
	time_t	nowTime;
	
	if (pgContentWin == NULL)
		return(-1);
	
	if ((pgunionKDBSvrBuf == NULL) || (pgunionKDBSvrBufHDL == NULL))
		return(-1);
	
	for (posNum = 0,i = 0;;)
	{
		memset(Buf,0,sizeof(Buf));
		memset(Buf,' ',gColOfWin - 4);
		for (j = 1; j <= gLineOfWin - 2; j++)
		{
			wmove(pgContentWin,j,2);
			waddstr(pgContentWin,Buf);
		}
		memset(Buf,0,sizeof(Buf));
		sprintf(Buf,"  第%04d页 %12s %12s %8s %8s  %8s %6s",
			i / (gLineOfWin - 2) + 1,
			" 消 息 类 型"," 消 息 标 识"," 提供者 "," 处理者 ","等待时间","长度/响应 ");
		wmove(pgContentWin,(i % (gLineOfWin - 2)) + 1,2);
		waddstr(pgContentWin,Buf);
		++i;
		for (;posNum < pgunionKDBSvrBufHDL->maxNumOfRec; posNum++)
		{
			if ((pheader = (PUnionKDBSvrServiceHeader)(pgunionKDBSvrBuf+conSizeOfEachKDBSvrRec*posNum)) == NULL)
				return(-1);
			//if ((pheader->type <= 0) || (pheader->locked))
			if ((pheader->provider == 0) || (pheader->time <= 0) || (pheader->type <= 0))
				continue;
			memset(Buf,0,sizeof(Buf));
			time(&nowTime);
			sprintf(Buf,"%6d     %12d %12d %8d %8d %8d  %6d",
				posNum,pheader->type,
				pheader->index,
				pheader->provider,
				pheader->dealer,
				nowTime-pheader->time,
				pheader->lenOfRec);
			wmove(pgContentWin,(i % (gLineOfWin - 2))+1,2);
			waddstr(pgContentWin,Buf);
			++i;
			if ((i % (gLineOfWin - 2) == 0) && (i != 0))
			{
				posNum++;
				break;
			}
		}
		wrefresh(pgContentWin);
		if (posNum < pgunionKDBSvrBufHDL->maxNumOfRec)
		{
			//if (interval < 10)
			//	sleep(10);
			//else
				sleep(interval);
		}
		else
		{
			if (i <= gLineOfWin - 2)
				break;
			//if (interval < 10)
			//	sleep(10);
			//else
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
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());

	if (argc >= 2)
		interval = atoi(argv[1]);
	if (interval <= 0)
		interval = 1;
	
	return(UnionMonitorMain(interval,UnionGetApplicationName(),"密钥库操作服务监控程序",UnionConnectKDBSvrBufMDL,RefreshExistMsgWin,UnionTaskActionBeforeExit));
}

int UnionHelp()
{
	printf("Usage:: %s [seconds]\n",UnionGetApplicationName());
	return(0);
}
