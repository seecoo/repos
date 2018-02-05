//	Wolfgang Wang
//	2001/10/15
// 	Version	1.0

//	20040623 Wolfgang Wang, 对异常加密机采用反显

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#ifdef _AIX_
#include <cur00.h>
#else
#include <curses.h>
#endif
#include <signal.h>

#include "unionLockTBL.h"
#include "UnionTask.h"
#include "unionMonitor.h"

PUnionLockTBL	pgunionLockTBL;

extern WINDOW		*pgContentWin;

PUnionTaskInstance	ptaskInstance = NULL;
int			gunionDisplayAbnormalLockTBLMerely = 0;
char			gunionLockTBLRemark[100];

int UnionTaskActionBeforeExit()
{
	UnionKillMonitorTwinProcess();
	CloseTaskMonitor();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

// this is rewritten by Wolfgang Wang, 2004/06/23
int RefreshLockTBLWin(int interval)
{
	int		i,j;
	char		Buf[128];
	time_t		now;
	int		len = 0;
	int		ret;
	int		num;
	int		pages = 0;
			
	if (pgContentWin == NULL)
		return(-1);
	
	memset(Buf,0,sizeof(Buf));
	sprintf(Buf,"%10s %30s %6s %2s %4s%14s",
		"资源名","关键字","锁级别","写锁否","读锁数","写锁时间");
	wmove(pgContentWin,1,2);
	waddstr(pgContentWin,Buf);

	memset(Buf,0,sizeof(Buf));
	memset(Buf,' ',gColOfWin - 4);
	for (j = 2; j < gLineOfWin - 1; j++)
	{
		wmove(pgContentWin,j, 2);
		waddstr(pgContentWin,Buf);
	}
	wrefresh(pgContentWin);

	for (i = 0,num = 0; i < pgunionLockTBL->maxNum; i++)
	{
		if (strlen(pgunionLockTBL->lockGrp[i].resName) == 0)
			continue;
		len = 0;
		if (gunionDisplayAbnormalLockTBLMerely)	// 只显示异常的
		{		
			if (UnionIsCommConfNormal(&(pgunionLockTBL->lockGrp[i])))
				continue;
		}
		++num;
		sprintf(Buf+len,"%10s %30s",pgunionLockTBL->lockGrp[i].resName,pgunionLockTBL->lockGrp[i].keyWord);
		len = strlen(Buf);
		switch (strlen(pgunionLockTBL->lockGrp[i].keyWord))
		{
			case	0:
				sprintf(Buf+len," 资源锁");
				break;
			default:
				sprintf(Buf+len," 记录锁");
				break;
		}
		len = strlen(Buf);
		/*switch (pgunionLockTBL->lockGrp[i].isWritingLocked)
		{
			case	1:
				sprintf(Buf+len,"     写锁     ");
				break;
			case	0:
				sprintf(Buf+len,"    未写锁    ");
				break;
			default:
				sprintf(Buf+len,"      ");
				break;
		}
		*/
		time(&now);
		len = strlen(Buf);
		sprintf(Buf+len,"    %d    %04d%15s",
				pgunionLockTBL->lockGrp[i].isWritingLocked,
				pgunionLockTBL->lockGrp[i].readingLocksNum,
				pgunionLockTBL->lockGrp[i].writingTime);
		wmove(pgContentWin,num + 1,2);
		if (!UnionIsLockTBLConnected())
		{
#ifndef _AIX_
			wstandout(pgContentWin);
#endif
			waddstr(pgContentWin,Buf);
#ifndef _AIX_
			wstandend(pgContentWin);
#endif
		}
		else
			waddstr(pgContentWin,Buf);
		if (num + 3 == gLineOfWin)
		{
			pages++;
			wrefresh(pgContentWin);
			sleep(5);
			num = 0;
			memset(Buf,0,sizeof(Buf));
			memset(Buf,' ',gColOfWin - 4);
			for (j = 2; j < gLineOfWin - 1; j++)
			{
				wmove(pgContentWin,j, 2);
				waddstr(pgContentWin,Buf);
			}
			wrefresh(pgContentWin);
		}
	}
	wrefresh(pgContentWin);
	if (pages >= 1)
		sleep(5);		
	return(0);
}

int main(int argc,char **argv)
{
	int	interval;
	int	i;
	
	UnionSetApplicationName(argv[0]);

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	memset(gunionLockTBLRemark,0,sizeof(gunionLockTBLRemark));
	if (argc < 2)
		interval = 1;
	else
	{
		for (i = 1; i < argc; i++)
		{
			UnionToUpperCase(argv[i]);
			if (strcasecmp(argv[i],"ABNORMAL") == 0)
				gunionDisplayAbnormalLockTBLMerely = 1;
			else if (UnionIsDigitStr(argv[i]))
			{
				if ((interval = abs(atoi(argv[i]))) <= 0)
					interval = 1;
			}
			else
				strcpy(gunionLockTBLRemark,argv[i]);
		}
	}
			
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	return(UnionMonitorMain(interval,argv[0],"锁操作程序",UnionConnectLockTBL,RefreshLockTBLWin,UnionTaskActionBeforeExit));
}
	
int UnionHelp()
{
	printf("Usage:: %s [seconds] [abnormal] [bankID]\n",UnionGetApplicationName());
	return(0);
}

