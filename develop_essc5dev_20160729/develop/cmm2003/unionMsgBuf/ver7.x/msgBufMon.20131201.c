//	Author:		Wolfgang Wang
//	Date:		2003/09/22
//	Version:	1.0

#define _UnionTask_3_x_	
#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef _AIX_
#include <cur00.h>
#else
#include <curses.h>
#endif
#include <signal.h>
#include <time.h>

#include "UnionTask.h"
#include "UnionLog.h"
#include "unionMsgBuf7.x.h"
#include "unionMonitor.h"
#include "unionVersion.h"
#include "unionCommand.h"

extern unsigned char		*pgpdfzMsgBuf;
extern PUnionMsgBufHDL		pgpdfzMsgBufHDL;

extern WINDOW		*pgContentWin;

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionKillMonitorTwinProcess();
	CloseTaskMonitor();
	UnionDisconnectMsgBufMDL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int RefreshExistMsgWin(int interval)
{
	int	i,j;
	char	Buf[128];
	PUnionMessageHeader	pmsgHeader;
	int	posNum = 0;
	//int	len;
	time_t	nowTime;
	//long	sizeOfPerMsg;
	long	offset;
	
	if (pgContentWin == NULL)
		return(-1);
	
	if ((pgpdfzMsgBuf == NULL) || (pgpdfzMsgBufHDL == NULL))
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
		sprintf(Buf,"  第%04d页 %12s %12s   %6s %6s  %12s   %6s",
			i / (gLineOfWin - 2) + 1,
			" 消 息 类 型"," 消 息 标 识","提供者","处理者","   等待时间  "," 长度 ");
		wmove(pgContentWin,(i % (gLineOfWin - 2)) + 1,2);
		waddstr(pgContentWin,Buf);
		++i;
		for (;posNum < UnionGetAllMaxNumOfMsg(); posNum++)
		{
			offset = UnionGetOffsetOfMsgBufByIndex(posNum);
			
			//sizeOfPerMsg = sizeof(*pmsgHeader) + pgpdfzMsgBufHDL->maxSizeOfMsg;
	
			//if ((pmsgHeader = (PUnionMessageHeader)(pgpdfzMsgBuf+sizeOfPerMsg*posNum)) == NULL)
			if ((pmsgHeader = (PUnionMessageHeader)(pgpdfzMsgBuf+offset)) == NULL)
				return(-1);
			if ((pmsgHeader->type <= 0) || (pmsgHeader->locked))
				continue;
			memset(Buf,0,sizeof(Buf));
			time(&nowTime);
			sprintf(Buf,"%6d     %12ld %12ld    %5d  %5d    %8ld     %5d",
				posNum,pmsgHeader->type,
				pmsgHeader->msgIndex,
				pmsgHeader->provider,
				pmsgHeader->dealer,
				nowTime-pmsgHeader->time,
				pmsgHeader->len);
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
		if (posNum < UnionGetAllMaxNumOfMsg())
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

int RefreshFreePosWin(int interval)
{
	int	i,j;
	char	Buf[128];
	PUnionMessageHeader	pmsgHeader;
	int	posNum = 0;
	int	len;
	int	offset;
	//long	sizeOfPerMsg;
	

	if (pgContentWin == NULL)
		return(-1);
	
	if ((pgpdfzMsgBuf == NULL) || (pgpdfzMsgBufHDL == NULL))
		return(-1);
	
	//sizeOfPerMsg = sizeof(*pmsgHeader) + pgpdfzMsgBufHDL->maxSizeOfMsg;
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
		sprintf(Buf,"  第%04d页",
			i / (gLineOfWin - 2) + 1);
		wmove(pgContentWin,(i % (gLineOfWin - 2)) + 1,2);
		waddstr(pgContentWin,Buf);
		++i;
		memset(Buf,0,sizeof(Buf));
		for (len = 0;posNum < UnionGetAllMaxNumOfMsg(); posNum++)
		{
			offset = UnionGetOffsetOfMsgBufByIndex(posNum);
			//if ((pmsgHeader = (PUnionMessageHeader)(pgpdfzMsgBuf+sizeOfPerMsg*posNum)) == NULL)
			if ((pmsgHeader = (PUnionMessageHeader)(pgpdfzMsgBuf+offset)) == NULL)
				return(-1);
			if ((pmsgHeader->type > 0) || (pmsgHeader->locked))
				continue;
			sprintf(Buf+len,"%8d ",posNum);
			len += 9;
			if (len < 9 * 7)
				continue;
			wmove(pgContentWin,(i % (gLineOfWin - 2))+1,2);
			waddstr(pgContentWin,Buf);
			len = 0;
			memset(Buf,0,sizeof(Buf));
			++i;
			if ((i % (gLineOfWin - 2) == 0) && (i != 0))
			{
				posNum++;
				break;
			}
		}
		wrefresh(pgContentWin);
		if (posNum < UnionGetAllMaxNumOfMsg())
		{
			//if (interval < 10)
			//	sleep(10);
			//else
				sleep(interval);
		}
		else
		{
			if ((len < 9 * 7) && (len > 0))
			{
				wmove(pgContentWin,(i % (gLineOfWin - 2))+1,2);
				waddstr(pgContentWin,Buf);
				wrefresh(pgContentWin);
			}
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
	int	interval;
	int	monitorMsg;
	
	UnionSetApplicationName(argv[0]);

	/*
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"msgBufMon")) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	*/

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	if (argc < 2)
	{
		interval = 1;
		monitorMsg = 1;
	}
	if (argc == 2)
	{
		if (strcasecmp(argv[1],"MESSAGE") == 0)
		{
			monitorMsg = 1;
			interval = 1;
		}
		else if (strcasecmp(argv[1],"POS") == 0)
		{
			monitorMsg = 0;
			interval = 1;
		}
		else
		{
			if ((interval = abs(atoi(argv[1]))) <= 0)
				interval = 1;
			monitorMsg = 1;
		}
	}
	if (argc >= 3)
	{
		if (strcasecmp(argv[1],"MESSAGE") == 0)
		{
			monitorMsg = 1;
			if ((interval = abs(atoi(argv[2]))) <= 0)
				interval = 1;
		}
		else if (strcasecmp(argv[1],"POS") == 0)
		{
			monitorMsg = 0;
			if ((interval = abs(atoi(argv[2]))) <= 0)
				interval = 1;
		}
		else
		{
			if ((interval = abs(atoi(argv[1]))) <= 0)
				interval = 1;
			if (strcasecmp(argv[2],"MESSAGE") == 0)
				monitorMsg = 1;
			else if (strcasecmp(argv[2],"POS") == 0)
				monitorMsg = 0;
			else
				monitorMsg = 1;
				
		}
	}
	if (monitorMsg)	
		return(UnionMonitorMain(interval,"msgBufMon message","消息交换区消息监控程序",UnionConnectMsgBufMDL,RefreshExistMsgWin,UnionTaskActionBeforeExit));
	else
		return(UnionMonitorMain(interval,"msgBufMon pos","消息交换区空闲位置监控程序",UnionConnectMsgBufMDL,RefreshFreePosWin,UnionTaskActionBeforeExit));
}

int UnionHelp()
{
	printf("Usage:: %s [message/pos] [seconds]\n",UnionGetApplicationName());
	return(0);
}
