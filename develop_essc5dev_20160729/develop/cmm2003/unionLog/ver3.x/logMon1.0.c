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
#include <unistd.h>

#include "UnionTask.h"
#include "UnionLog.h"
#include "unionMonitor.h"
#include "unionVersion.h"
#include "unionCommand.h"

extern PUnionLogFileTBL		pgunionLogFileTBL;
extern PUnionLogFile		pgunionLogFile;

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

int RefreshContentWin(int interval)
{
	int	i,j;
	char	Buf[128];
	int	logFileNum = 0;
	int	len;
	
	if (pgContentWin == NULL)
		return(-1);
	
	if ((pgunionLogFileTBL == NULL) || (pgunionLogFile == NULL))
		return(-1);
	
	for (logFileNum = 0,i = 0;;)
	{
		memset(Buf,0,sizeof(Buf));
		memset(Buf,' ',gColOfWin - 4);
		for (j = 1; j <= gLineOfWin - 2; j++)
		{
			wmove(pgContentWin,j,2);
			waddstr(pgContentWin,Buf);
		}
		memset(Buf,0,sizeof(Buf));
		sprintf(Buf,"第%04d页%22s %4s %4s(M) %4s %8s(k) %10s",
			i / (gLineOfWin - 2) + 1,
			"    日  志  名  称    ",
			"级别","尺寸","备份","当前大小","当前用户数");
		wmove(pgContentWin,(i % (gLineOfWin - 2)) + 1,2);
		waddstr(pgContentWin,Buf);
		++i;
		for (;logFileNum < pgunionLogFileTBL->maxLogFileNum; logFileNum++)
		{
			if (strlen((pgunionLogFile+logFileNum)->name) == 0)
				continue;
			//if ((pgunionLogFile+logFileNum)->users == 0)
			//	continue;
			memset(Buf,0,sizeof(Buf));
			sprintf(Buf,"%s",(pgunionLogFile+logFileNum)->name);
			len = strlen(Buf);
			memset(Buf+len,' ',30 - len);
			len = 30;
			memcpy(Buf+len," ",1);
			++len;
			switch ((pgunionLogFile+logFileNum)->level)
			{
				case	conLogNoneLogLevel:
					memcpy(Buf+len,"不写 ",5);
					break;
				case	conLogSystemErrLevel:
					memcpy(Buf+len,"系统 ",5);
					break;
				case	conLogUserErrLevel:
					memcpy(Buf+len,"程序 ",5);
					break;
				case	conLogAuditLevel:
					memcpy(Buf+len,"审计 ",5);
					break;
				case	conLogDebugLevel:
					memcpy(Buf+len,"调试 ",5);
					break;
				case	conLogAlwaysLevel:
					memcpy(Buf+len,"全写 ",5);
					break;
				default:
					memcpy(Buf+len,"不明 ",5);
					break;
			}
			len += 5;
			sprintf(Buf+len,"%4ld    ",(pgunionLogFile+logFileNum)->maxSize / 1000000);
			len += 8;
			if ((pgunionLogFile+logFileNum)->rewriteAttr == conLogFullRewriteAfterBackup)
				memcpy(Buf+len," 是  ",5);
			else
				memcpy(Buf+len," 否  ",5);
			len += 5;
			sprintf(Buf+len,"%8ld    ",(pgunionLogFile+logFileNum)->currentSize / 1000);
			len += 11;
			sprintf(Buf+len,"%8d",(pgunionLogFile+logFileNum)->users);
			wmove(pgContentWin,(i % (gLineOfWin - 2))+1,2);
			waddstr(pgContentWin,Buf);
			++i;
			if ((i % (gLineOfWin - 2) == 0) && (i != 0))
			{
				logFileNum++;
				break;
			}
		}
		wrefresh(pgContentWin);
		if (logFileNum < pgunionLogFileTBL->maxLogFileNum)
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
	
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		interval = 1;
	else
	{
		if ((interval = abs(atoi(argv[1]))) <= 0)
			interval = 1;
	}
			
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"logMon",interval)) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());

	return(UnionMonitorMain(interval,"logMon","日志监控程序",NULL,RefreshContentWin,UnionTaskActionBeforeExit));

}

int UnionHelp()
{
	printf("Usage:: %s [seconds]\n",UnionGetApplicationName());
	return(0);
}
