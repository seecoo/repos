//	Author:		Wolfgang Wang
//	Date:		2003/09/22
//	Version:	1.0

#ifndef _UnionMonitorMDL_
#define _UnionMonitorMDL_

#ifdef _AIX_
#include <cur00.h>
#else
#include <curses.h>
#endif

#define gLineOfWin		22
#define gColOfWin		80
#define gLineOffsetOfHsmWin	1
#define gColOffsetOfHsmWin	0

int DrawTitleBox(WINDOW	*pWin,char *Title);
int StartTaskMonitor(char *userTitle);
int CloseTaskMonitor();
int DisplayTime();
int UnionMonitorMain(int interval,char *taskName,char *winTitle,int (*UnionTaskActionBeforeStart)(),int (*UnionRefreshContentWin)(),int (*UnionTaskActionBeforeExit)());
int UnionKillMonitorTwinProcess();

int UnionPopWindow(char *winTitle,char *content);
int UnionDrawWinTitleBox(WINDOW	*pWin,char *Title,int mylines,int mycolumns);
int UnionClearWin(WINDOW *pwin,int mylines,int mycolumns);

#endif
