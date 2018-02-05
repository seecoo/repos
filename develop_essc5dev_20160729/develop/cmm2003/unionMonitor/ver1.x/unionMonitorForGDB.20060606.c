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
#include <stdarg.h>
#include <unistd.h>

#include "UnionTask.h"
#include "UnionLog.h"
#include "unionVersion.h"

#include "unionREC.h"
#include "unionMonitor.h"
#include "unionErrCode.h"

int		gmonitorStarted = 0;
int		gmonitorFather = 0;
int		gmonitorChildProcID = -1;

WINDOW		*pgContentWin = NULL;
WINDOW		*gTimeWin = NULL;
WINDOW		*gTitleWin = NULL;

extern PUnionTaskInstance	ptaskInstance;

int UnionClearWin(WINDOW *pwin,int mylines,int mycolumns)
{
	int	i;
	char	buf[128];
	
	if (pwin == NULL)
		return(errCodeParameter);

	memset(buf,0,sizeof(buf));
	memset(buf,' ',mycolumns);
	for (i = 0; i < mylines; i++)
	{
		wmove(pwin,i,0);
		waddstr(pwin,buf);
	}
	wrefresh(pwin);

	return(0);
}

int UnionDrawWinTitleBox(WINDOW	*pWin,char *Title,int mylines,int mycolumns)
{
	int	i,j;
	int	Pos;
	
	if (pWin == NULL)
		return(errCodeParameter);
		
	for (i = 0; i < (mycolumns - 2)/ 2; i++)
	{
		wmove(pWin,0,i*2);
		waddstr(pWin,"━");
		wmove(pWin,mylines-1,i*2);
		waddstr(pWin,"━");
	}
	for (j = 0; j < mylines - 1; j++)
	{
		wmove(pWin,j,0);
		waddstr(pWin,"┃");
		wmove(pWin,j,mycolumns-1-1);
		waddstr(pWin,"┃");
	}
	wmove(pWin,0,0);
	waddstr(pWin,"┏");
	wmove(pWin,0,mycolumns-1-1);
	waddstr(pWin,"┓");
	wmove(pWin,mylines-1,0);
	waddstr(pWin,"┗");
	wmove(pWin,mylines-1,mycolumns-1-1);	
	waddstr(pWin,"┛");

	if (Title != NULL)
	{
		Pos = (mycolumns - strlen(Title))/2;
		if (Pos % 2 != 0)
			Pos = Pos - 1;
		wmove(pWin,0,Pos);
		waddstr(pWin,Title);
	}
		
	wrefresh(pWin);
	return(0);
}

int DrawTitleBox(WINDOW	*pWin,char *Title)
{
	int	i,j;
	int	Pos;
	
	if (pWin == NULL)
		return(errCodeParameter);
		
	for (i = 0; i < (gColOfWin - 2)/ 2; i++)
	{
		wmove(pWin,0,i*2);
		waddstr(pWin,"━");
		wmove(pWin,gLineOfWin-1,i*2);
		waddstr(pWin,"━");
	}
	for (j = 0; j < gLineOfWin - 1; j++)
	{
		wmove(pWin,j,0);
		waddstr(pWin,"┃");
		wmove(pWin,j,gColOfWin-1-1);
		waddstr(pWin,"┃");
	}
	wmove(pWin,0,0);
	waddstr(pWin,"┏");
	wmove(pWin,0,gColOfWin-1-1);
	waddstr(pWin,"┓");
	wmove(pWin,gLineOfWin-1,0);
	waddstr(pWin,"┗");
	wmove(pWin,gLineOfWin-1,gColOfWin-1-1);	
	waddstr(pWin,"┛");

	if (Title != NULL)
	{
		Pos = (gColOfWin - strlen(Title))/2;
		if (Pos % 2 != 0)
			Pos = Pos - 1;
		wmove(pWin,0,Pos);
		waddstr(pWin,Title);
	}
		
	wrefresh(pWin);
	return(0);
}

int StartTaskMonitor(char *userTitle)
{
	char			title[256];
	
	memset(title,0,sizeof(title));
	sprintf(title,"%s   %s   %s",UnionGetApplicationSystemName(),userTitle,	UnionReadPredfinedProductVersionNumber());
		
	initscr();
	gmonitorStarted = 1;
	
	if ((gTitleWin = newwin(1,80,0,0)) == NULL)
	{
		UnionSystemErrLog("in StartTaskMonitor:: newwin gTitleWin!\n");
		return(errCodeUseOSErrCode);
	}
	
	wmove(gTitleWin,0,1);
	waddstr(gTitleWin,title);
	wrefresh(gTitleWin);
	
	if ((pgContentWin = newwin(gLineOfWin,gColOfWin,gLineOffsetOfHsmWin,gColOffsetOfHsmWin)) == NULL)
	{
		UnionSystemErrLog("in StartTaskMonitor:: newwin pgContentWin!\n");
		return(errCodeUseOSErrCode);
	}
	
	if ((gTimeWin = newwin(1,80,gLineOffsetOfHsmWin + gLineOfWin,0)) == NULL)
	{
		UnionSystemErrLog("in StartTaskMonitor:: newwin gTimeWin!\n");
		return(errCodeUseOSErrCode);
	}
	
	
	DrawTitleBox(pgContentWin,"");

	DisplayTime();
	
	//wrefresh(pgContentWin);
	//wrefresh(gResTransWin);
	
	return(0);
}

int CloseTaskMonitor()
{
	//UnionLog("in CloseTaskMonitor:: entering...\n");
	if (!gmonitorStarted)
	{
		//UnionLog("in CloseTaskMonitor:: return without finishing!\n");
		return(0);
	}
	//UnionLog("in CloseTaskMonitor:: clear...\n");
	clear();
	//UnionLog("in CloseTaskMonitor:: refresh...\n");
	refresh();
	//UnionLog("in CloseTaskMonitor:: endwin...\n");
	endwin();
	//UnionLog("in CloseTaskMonitor:: finish...\n");
	//system("clear");
	return(0);
}

int DisplayTime()
{
	char	Buf[128];
	char	SystemDateTime[40];
	
	memset(SystemDateTime,0,sizeof(SystemDateTime));
	UnionGetFullSystemDateTime(SystemDateTime);
	
	memset(Buf,0,sizeof(Buf));
	
	sprintf(Buf,"当前时间:%s                                    版权:广东发展银行",SystemDateTime);
	
	wmove(gTimeWin,0,1);
	waddstr(gTimeWin,Buf);
	wrefresh(gTimeWin);
	
	return(0);
}

int UnionMonitorMain(int interval,char *taskName,char *winTitle,int (*UnionTaskActionBeforeStart)(),int (*UnionRefreshContentWin)(),int (*UnionTaskActionBeforeExit)())
{
	int	ret;
	char	ch;
	
	if ((gmonitorChildProcID = UnionCreateProcess()) > 0)	// 父进程
	{
		gmonitorFather = 1;
		for (;;)
		{
			ch = getchar();
			//if ((ch == 0xff) || (ch == 0xffffffff))
			if (ch == 0xffffffff)
				goto killChildProc;
			if (ch != 0x1b)
				continue;
			ch = getchar();
			//if ((ch == 0xff) || (ch == 0xffffffff))
			if (ch == 0xffffffff)
				goto killChildProc;
			if (ch != 0x1b)
				continue;
killChildProc:
			UnionKillTaskInstanceByProcID(gmonitorChildProcID);
			if (kill(gmonitorChildProcID,0) == 0)
			{
				UnionLog("in UnionMonitorMain:: child Process [%d] still exists!\n",gmonitorChildProcID);
				sleep(1);
				goto killChildProc;
			}
			return(UnionTaskActionBeforeExit());
		}
	}
	
	gmonitorFather = 0;
	// 子进程
	if (UnionTaskActionBeforeStart != NULL)
	{
		UnionLog("in UnionMonitorMain:: call UnionTaskActionBeforeStart!\n");
		if ((ret = UnionTaskActionBeforeStart()) < 0)
		{
			UnionPrintf("in UnionMonitorMain:: UnionTaskActionBeforeStart Error!\n");
			return(UnionTaskActionBeforeExit());
		}
	}
	else
		UnionLog("in UnionMonitorMain:: No UnionTaskActionBeforeStart called!\n");
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s %d",taskName,interval)) == NULL)
	{
		UnionPrintf("in UnionMonitorMain:: UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	
	if ((ret = StartTaskMonitor(winTitle)) < 0)
		return(UnionTaskActionBeforeExit());

	for (;;)
	{
		UnionRefreshContentWin(interval);
		DisplayTime();
		sleep(interval);
	}
}

int UnionKillMonitorTwinProcess()
{
	if (gmonitorFather)
	{
		UnionKillTaskInstanceByProcID(gmonitorChildProcID);
		for (;;)
		{
			if (kill(gmonitorChildProcID,0) == 0)
			{
				UnionLog("in UnionMonitorMain:: child Process [%d] still exists!\n",gmonitorChildProcID);
				sleep(1);
				continue;
			}
			else
				break;
		}
		return(0);
	}
	else
		return(0);
}

int UnionPopWindow(char *winTitle,char *content)
{
	int	maxColLen = 50,displayCols;
	int	startx,starty,mylines,mycolumns;
	int	i;
	char	buf[128];
	WINDOW	*pPopWin;
	int	loopTimes;
	
	mylines = strlen(content) / (maxColLen-4) + 1 + 1 + 1;
	mycolumns = maxColLen;
	displayCols = mycolumns - 4;

	startx = (gColOfWin - mycolumns) / 2;
	starty = (gLineOfWin - mylines) / 2;
	
	loopTimes = 0;
//loop:
	if ((pPopWin = newwin(mylines,mycolumns,starty,startx)) == NULL)
	{
		UnionUserErrLog("in UnionPopWindow:: newwin!\n");
		return(errCodeUseOSErrCode);
	}
	
	UnionClearWin(pPopWin,mylines,mycolumns);
	//wstandout(pPopWin);
	UnionDrawWinTitleBox(pPopWin,winTitle,mylines,mycolumns);
	//wstandend(pPopWin);
	//UnionLog("in UnionPopWindow:: content = [%s]\n",content);
	for (i = 1; i < mylines - 1; i++)
	{
		memset(buf,0,sizeof(buf));
		if (strlen(content) < i * displayCols)
			strcpy(buf,content+(i-1)*displayCols);
		else
			memcpy(buf,content+(i-1)*displayCols,displayCols);
		wmove(pPopWin,i,2);
		waddstr(pPopWin,buf);
		//UnionLog("in UnionPopWindow:: buf = [%s]\n",buf);
		wrefresh(pPopWin);
	}
	sleep(2);
	UnionClearWin(pPopWin,mylines,mycolumns);
	delwin(pPopWin);
	
	loopTimes++;
	
	//if (loopTimes < 2)
	//	goto loop;

	return(0);
}
