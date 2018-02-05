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

#include "unionREC.h"
#include "unionCommand.h"


extern PUnionREC			pgunionREC;
extern PUnionRECVar			pgunionRECVar;

extern WINDOW		*pgContentWin;

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionKillMonitorTwinProcess();
	CloseTaskMonitor();
	UnionDisconnectREC();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int RefreshContentWin(int interval)
{
	int	i,j;
	char	Buf[128];
	int	num;
	
	if (pgContentWin == NULL)
		return(-1);
	
	if ((pgunionREC == NULL) || (pgunionRECVar == NULL))
		return(-1);
	
	for (num = 0,i = 0;num < pgunionREC->realNum;)
	{
		memset(Buf,0,sizeof(Buf));
		memset(Buf,' ',gColOfWin - 4);
		for (j = 1; j <= gLineOfWin - 2; j++)
		{
			wmove(pgContentWin,j,2);
			waddstr(pgContentWin,Buf);
		}
		memset(Buf,0,sizeof(Buf));
		sprintf(Buf,"第%04d页%22s %8s %35s",
			i / (gLineOfWin - 2) + 1,
			"     环 境 变 量 名 称",
			"变量类型",
			"        环    境    变    量     值");
		wmove(pgContentWin,(i % (gLineOfWin - 2)) + 1,2);
		waddstr(pgContentWin,Buf);
		++i;
		for (;num < pgunionREC->realNum; num++)
		{
			memset(Buf,0,sizeof(Buf));
			sprintf(Buf,"%30s ",(pgunionRECVar+num)->name);
			switch ((pgunionRECVar+num)->type)
			{
				case	conInt:
					sprintf(Buf+31,"%8s %35d","interger",(pgunionRECVar+num)->value.intValue);
					break;
				case	conLong:
					sprintf(Buf+31,"%8s %35ld","  long  ",(pgunionRECVar+num)->value.longValue);
					break;
				case	conShort:
					sprintf(Buf+31,"%8s %35d","  short ",(pgunionRECVar+num)->value.shortValue);
					break;
				case	conString:
					sprintf(Buf+31,"%8s %35s"," string ",(pgunionRECVar+num)->value.strValue);
					break;
				case	conChar:
					sprintf(Buf+31,"%8s %35c","  char  ",(pgunionRECVar+num)->value.charValue);
					break;
				case	conDouble:
					sprintf(Buf+31,"%8s %35.2f"," double ",(pgunionRECVar+num)->value.doubleValue);
					break;
				default:
					sprintf(Buf+31,"%8s ","不明类型");
					break;
			}
			wmove(pgContentWin,(i % (gLineOfWin - 2))+1,2);
			waddstr(pgContentWin,Buf);
			++i;
			if ((i % (gLineOfWin - 2) == 0) && (i != 0))
			{
				num++;
				break;
			}
		}
		wrefresh(pgContentWin);
		if (num < pgunionREC->realNum)
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
			
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"recMon",interval)) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());

	return(UnionMonitorMain(interval,"recMon","运行环境变量监控程序",UnionConnectREC,RefreshContentWin,UnionTaskActionBeforeExit));

}

int UnionHelp()
{
	printf("Usage:: %s [seconds]\n",UnionGetApplicationName());
	return(0);
}
