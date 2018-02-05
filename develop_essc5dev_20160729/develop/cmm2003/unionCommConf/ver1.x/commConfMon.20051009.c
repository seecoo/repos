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

#include "unionCommConf.h"
#include "UnionTask.h"
#include "unionMonitor.h"
#include "unionVersion.h"
#include "UnionStr.h"
#include "unionCommand.h"

PUnionCommConfTBL	pgunionCommConfTBL;

extern WINDOW		*pgContentWin;

PUnionTaskInstance	ptaskInstance = NULL;
int			gunionDisplayAbnormalCommConfMerely = 0;
char			gunionCommConfRemark[100];

int UnionTaskActionBeforeExit()
{
	UnionKillMonitorTwinProcess();
	CloseTaskMonitor();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

// this is rewritten by Wolfgang Wang, 2004/06/23
int RefreshCommConfWin(int interval)
{
	int		i,j;
	char		Buf[128];
	time_t		now;
	int		len = 0;
	int		num;
	int		pages = 0;
			
	if (pgContentWin == NULL)
		return(-1);
	
	memset(Buf,0,sizeof(Buf));
	sprintf(Buf,"%10s %6s %4s %15s %6s %6s %8s %8s",
		" 备    注 "," 类型 ","连接","  对端IP地址 "," 端口 "," 连接数 "," 包数目 "," 空闲 ");
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

	for (i = 0,num = 0; i < conMaxNumOfConn; i++)
	{
		if (strlen(pgunionCommConfTBL->rec[i].ipAddr) == 0)
			continue;
		len = 0;
		if (gunionDisplayAbnormalCommConfMerely)	// 只显示异常的
		{		
			if (UnionIsCommConfNormal(&(pgunionCommConfTBL->rec[i])))
				continue;
		}
		if (strlen(gunionCommConfRemark) != 0)		// 只显示指定remark
		{
			if (strcasecmp(gunionCommConfRemark,pgunionCommConfTBL->rec[i].remark) != 0)
				continue;
		}
		++num;
		sprintf(Buf+len,"%10s ",pgunionCommConfTBL->rec[i].remark);
		len = 11;
		switch (pgunionCommConfTBL->rec[i].procType)
		{
			case	conCommServer:
				sprintf(Buf+len,"server");
				break;
			case	conCommClient:
				sprintf(Buf+len,"client");
				break;
			default:
				sprintf(Buf+len,"xxxxxx");
				break;
		}
		len = strlen(Buf);;
		switch (pgunionCommConfTBL->rec[i].connType)
		{
			case	conCommShortConn:
				sprintf(Buf+len,"  短 ");
				break;
			case	conCommLongConn:
				sprintf(Buf+len,"  长 ");
				break;
			default:
				sprintf(Buf+len,"     ");
				break;
		}
		time(&now);
		len = strlen(Buf);
		sprintf(Buf+len," %15s %6d %6ld %10ld %8ld",
				pgunionCommConfTBL->rec[i].ipAddr,
				pgunionCommConfTBL->rec[i].port,
				pgunionCommConfTBL->rec[i].connNum,
				pgunionCommConfTBL->rec[i].totalNum,
				now - pgunionCommConfTBL->rec[i].lastWorkingTime);
		wmove(pgContentWin,num + 1,2);
		if (!UnionIsCommConfNormal(&(pgunionCommConfTBL->rec[i])))
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
	int	interval = 0;
	int	i;
	
	UnionSetApplicationName(argv[0]);

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	memset(gunionCommConfRemark,0,sizeof(gunionCommConfRemark));
	if (argc < 2)
		interval = 1;
	else
	{
		for (i = 1; i < argc; i++)
		{
			UnionToUpperCase(argv[i]);
			if (strcasecmp(argv[i],"ABNORMAL") == 0)
				gunionDisplayAbnormalCommConfMerely = 1;
			else if (UnionIsDigitStr(argv[i]))
			{
				if ((interval = abs(atoi(argv[i]))) <= 0)
					interval = 1;
			}
			else
				strcpy(gunionCommConfRemark,argv[i]);
		}
	}
			
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	return(UnionMonitorMain(interval,argv[0],"通讯监控程序",UnionConnectCommConfTBL,RefreshCommConfWin,UnionTaskActionBeforeExit));
}
	
int UnionHelp()
{
	printf("Usage:: %s [seconds] [abnormal] [bankID]\n",UnionGetApplicationName());
	return(0);
}

