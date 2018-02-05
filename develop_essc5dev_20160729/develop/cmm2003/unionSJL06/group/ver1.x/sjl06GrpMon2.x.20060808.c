//	Wolfgang Wang
//	2001/10/15
// 	Version	1.0

//	20040623 Wolfgang Wang, 对异常加密机采用反显

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

#include "sjl06.h"
#include "sjl06Grp.h"
#include "UnionTask.h"
#include "unionMonitor.h"
#include "UnionLog.h"

// Defined in HsmGrp.a
extern PUnionSJL06	pgunionSJL06Grp;

extern WINDOW		*pgContentWin;

PUnionTaskInstance	ptaskInstance = NULL;

long gunionLastTotalTimes = 0;

int UnionTaskActionBeforeExit()
{
	UnionKillMonitorTwinProcess();
	CloseTaskMonitor();
	UnionDisconnectWorkingSJL06MDL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

// this is rewritten by Wolfgang Wang, 2004/06/23
int RefreshHsmTransWin(int interval)
{
	int		i;
	char		Buf[128];
	PUnionSJL06	pSJL06;
	int		hsmNum;
	long		totalNum = 0;
			
	if (pgContentWin == NULL)
		return(-1);
	
	if (pgunionSJL06Grp == NULL)
	{
		UnionUserErrLog("in RefreshHsmTransWin:: pgunionSJL06Grp is NULL!\n");
		return(-1);
	}
	
	//UnionLog("in RefreshHsmTransWin:: HsmNum = [%d]\n",pgunionSJL06Grp->HsmNum);
	memset(Buf,0,sizeof(Buf));
	//UnionLog("HsmNum = [%d]\n",pgunionSJL06Grp->HsmNum);
	sprintf(Buf,"%6s %15s %6s %5s %4s %13s %13s",
		"工作组","  密码机IP地址 "," 端口"," 状态 ","连接","   失败次数  ","   成功次数  ");
	wmove(pgContentWin,1,2);
	waddstr(pgContentWin,Buf);
	for (hsmNum = 0,i = 0; i < conMaxNumOfSJL06 && i < gLineOfWin; i++)
	{
		pSJL06 = pgunionSJL06Grp + i;
		if (strlen(pSJL06->staticAttr.ipAddr) == 0)
			continue;
		memset(Buf,0,sizeof(Buf));
		if ((pSJL06->dynamicAttr.activeLongConn > 0) && (pSJL06->dynamicAttr.status != conOnlineSJL06))
			UnionKillTaskInstanceByName(pSJL06->staticAttr.ipAddr);
		sprintf(Buf,"%5s %15s %6d ",
			pSJL06->staticAttr.hsmGrpID,
			pSJL06->staticAttr.ipAddr,
			pSJL06->staticAttr.port);
		switch (pSJL06->dynamicAttr.status)
		{
			case	'0':
				sprintf(Buf+strlen(Buf),"  异常 ");
				break;
			case	'1':
				sprintf(Buf+strlen(Buf),"  在线 ");
				break;
			case	'2':
				sprintf(Buf+strlen(Buf),"  备份 ");
				break;
			default:
				sprintf(Buf+strlen(Buf),"  不明 ");
				break;
		}
		sprintf(Buf+strlen(Buf),"%4d %12ld %12ld",
			pSJL06->dynamicAttr.activeLongConn,
			pSJL06->dynamicAttr.timeoutTimes + pSJL06->dynamicAttr.connFailTimes,
			pSJL06->dynamicAttr.normalCmdTimes + pSJL06->dynamicAttr.abnormalCmdTimes);
		wmove(pgContentWin,hsmNum + 2,2);
		totalNum += pSJL06->dynamicAttr.normalCmdTimes + pSJL06->dynamicAttr.abnormalCmdTimes;
		//UnionNullLog("Buf = [%s]\n",Buf);
		switch (pSJL06->dynamicAttr.status)
		{
			case	'0':
#ifndef _AIX_
				wstandout(pgContentWin);
#endif
				waddstr(pgContentWin,Buf);
#ifndef _AIX_
				wstandend(pgContentWin);
#endif
				break;
			case	'1':
			case	'2':
				waddstr(pgContentWin,Buf);
				break;
			default:
#ifndef _AIX_
				wstandout(pgContentWin);
#endif
				waddstr(pgContentWin,Buf);
#ifndef _AIX_
				wstandend(pgContentWin);
#endif
				break;
		}
		hsmNum++;
	}
	memset(Buf,0,sizeof(Buf));
	memset(Buf,' ',gColOfWin - 4);
	for (i = hsmNum + 2; i < gLineOfWin - 2; i++)
	{
		wmove(pgContentWin,i, 2);
		waddstr(pgContentWin,Buf);
	}
	wrefresh(pgContentWin);
	if (gunionLastTotalTimes != 0)
		UnionAuditNullLogWithTime("excutedHsmCmdNum = [%012ld]\n",totalNum - gunionLastTotalTimes);
	if ((gunionLastTotalTimes = totalNum) < 0)
		gunionLastTotalTimes = 0;
			
	return(0);
}

int main(int argc,char **argv)
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
			
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());

#ifndef _nullTaskName_
	return(UnionMonitorMain(interval,argv[0],"密码机工作组监控程序",UnionConnectWorkingSJL06MDL,RefreshHsmTransWin,UnionTaskActionBeforeExit));
#else
	return(UnionMonitorMain(interval,argv[0],"",UnionConnectWorkingSJL06MDL,RefreshHsmTransWin,UnionTaskActionBeforeExit));
#endif
}
	
int UnionHelp()
{
	printf("Usage:: %s [seconds]\n",UnionGetApplicationName());
	return(0);
}

