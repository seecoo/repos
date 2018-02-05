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
#include "transSpierBuf.h"
#include "unionMonitor.h"
#include "unionVersion.h"
#include "unionCommand.h"

extern PUnionTransSpierBufHDL		pgunionTransSpierBufHDL;
extern unsigned char			*pgunionTransSpierBuf;

extern WINDOW		*pgContentWin;

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionKillMonitorTwinProcess();
	CloseTaskMonitor();
	UnionDisconnectTransSpierBufMDL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int RefreshExistMsgWin(int interval)
{
	int	i,j;
	char	Buf[128];
	PUnionTransSpierMsgHeader	pheader;
	TUnionTransSpierMsgHeader	header;
	int	posNum = 0;
	time_t	nowTime;
	
	if (pgContentWin == NULL)
		return(-1);
	
	if ((pgunionTransSpierBuf == NULL) || (pgunionTransSpierBufHDL == NULL))
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
#ifdef _transSpier_2_x_
		sprintf(Buf,"    第%04d页  %30s %4s %4s %4s",
			i / (gLineOfWin - 2) + 1,
			"程序名","资源","位置","长度");
#else
		sprintf(Buf,"    第%04d页  %4s %4s %4s",
			i / (gLineOfWin - 2) + 1,
			"资源","位置","长度");
#endif
		wmove(pgContentWin,(i % (gLineOfWin - 2)) + 1,2);
		waddstr(pgContentWin,Buf);
		++i;
		for (;posNum < pgunionTransSpierBufHDL->maxNumOfMsg; posNum++)
		{
			if ((pheader = (PUnionTransSpierMsgHeader)(pgunionTransSpierBuf+conSizeOfEachTransSpierMsg*posNum)) == NULL)
				return(-1);
			memcpy(&header,pheader,sizeof(header));
			pheader = &header;
			//if ((pheader->type <= 0) || (pheader->locked))
			if (pheader->len <= 0)
				continue;
			memset(Buf,0,sizeof(Buf));
			time(&nowTime);
#ifdef _transSpier_2_x_
			sprintf(Buf,"%14s %30s %4d %4d %4d",
				pheader->dateTime,
				pheader->applicationName,
				pheader->resID,
				posNum,
				pheader->len);
#else
			sprintf(Buf,"%4d %4d %4d",
				pheader->resID,
				posNum,
				pheader->len);
#endif
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
		if (posNum < pgunionTransSpierBufHDL->maxNumOfMsg)
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
	
	return(UnionMonitorMain(interval,UnionGetApplicationName(),"密钥库操作服务监控程序",UnionConnectTransSpierBufMDL,RefreshExistMsgWin,UnionTaskActionBeforeExit));
}

int UnionHelp()
{
	printf("Usage:: %s [seconds]\n",UnionGetApplicationName());
	return(0);
}
