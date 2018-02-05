//	Wolfgang Wang
//	2004/11/23
// 	Version	1.0

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif
#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_	
#endif

#include <stdio.h>
#ifdef _AIX_
#include <cur00.h>
#else
#include <curses.h>
#endif
#include <signal.h>

#include "unionSckCommCli.h"
#include "UnionTask.h"
#include "unionMonitor.h"
#include "UnionLog.h"
#include "unionVersion.h"
#include "unionCommand.h"

// Defined in HsmGrp.a
extern PUnionCommSvrDef	pgunionCommSvrDef;

extern WINDOW		*pgContentWin;

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionKillMonitorTwinProcess();
	CloseTaskMonitor();
	UnionDisconnectCommSvrDefMDL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int RefreshHsmTransWin(int interval)
{
	int		i;
	char		Buf[128];
	
	if (pgContentWin == NULL)
		return(-1);
	
	if (pgunionCommSvrDef == NULL)
	{
		UnionUserErrLog("in RefreshHsmTransWin:: pgunionCommSvrDef is NULL!\n");
		return(-1);
	}

	wmove(pgContentWin,1,2);
	memset(Buf,0,sizeof(Buf));
	switch (pgunionCommSvrDef->workingMode)
	{
		case	conCommSvrHotBackupWorkingMode:
			sprintf(Buf,"%s -- ��ʱ����[%d]��","�ȱ��ݹ���ģʽ",pgunionCommSvrDef->timeout);
			break;
		case	conCommSvrBalanceWorkingMode:
			sprintf(Buf,"%s -- ��ʱ����[%d]��","���ؾ��⹤��ģʽ",pgunionCommSvrDef->timeout);
			break;
		case	conCommSvrMaintainWorkingMode:
			sprintf(Buf,"%s -- ��ʱ����[%d]��","ά������ģʽ",pgunionCommSvrDef->timeout);
			break;
		default:
			sprintf(Buf,"%s -- ��ʱ����[%d]��","��������ģʽ",pgunionCommSvrDef->timeout);
			break;
	}
	waddstr(pgContentWin,Buf);
	memset(Buf,0,sizeof(Buf));
	sprintf(Buf,"%15s %6s %4s %5s %6s %13s %13s",
		"   ������IP��ַ"," �˿�","����","  ״̬ ","  ���� ","   �ɹ�����  ","   ʧ�ܴ���  ");
	wmove(pgContentWin,2,2);
	waddstr(pgContentWin,Buf);
	for (i = 0; i < 2; i++)
	{
		sprintf(Buf,"%15s %6d ",
			pgunionCommSvrDef->sckSvrDef[i].ipAddr,
			pgunionCommSvrDef->sckSvrDef[i].port);
		if (pgunionCommSvrDef->sckSvrDef[i].isPrimaryServer)
			sprintf(Buf+strlen(Buf)," ��  ");
		else
			sprintf(Buf+strlen(Buf)," ��  ");
		switch (pgunionCommSvrDef->sckSvrDef[i].status)
		{
			case	conSckSvrAbnormal:
				sprintf(Buf+strlen(Buf),"  �쳣 ");
				break;
			case	conSckSvrNormal:
				sprintf(Buf+strlen(Buf),"  ���� ");
				break;
			case	conSckSvrMaintaining:
				sprintf(Buf+strlen(Buf),"  ά�� ");
				break;
			case	conSckSvrColdBackup:
				sprintf(Buf+strlen(Buf),"�䱸�� ");
				break;
			default:
				sprintf(Buf+strlen(Buf),"  ���� ");
				break;
		}
		sprintf(Buf+strlen(Buf),"%6ld %12ld %12ld",
			pgunionCommSvrDef->sckSvrDef[i].activeConnNum,
			pgunionCommSvrDef->sckSvrDef[i].successTimes,
			pgunionCommSvrDef->sckSvrDef[i].failTimes);
		wmove(pgContentWin,3+i,2);
		switch (pgunionCommSvrDef->sckSvrDef[i].status)
		{
			case	conSckSvrColdBackup:
			case	conSckSvrNormal:
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
	}
	memset(Buf,0,sizeof(Buf));
	memset(Buf,' ',gColOfWin - 4);
	for (i = 1 + 1 + 2 + 1; i < gLineOfWin - 2; i++)
	{
		wmove(pgContentWin,i, 2);
		waddstr(pgContentWin,Buf);
	}
	wrefresh(pgContentWin);
			
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

	return(UnionMonitorMain(interval,argv[0],"ͨѶ��������س���",UnionConnectCommSvrDefMDL,RefreshHsmTransWin,UnionTaskActionBeforeExit));
}
	
int UnionHelp()
{
	printf("Usage:: %s [seconds]\n",UnionGetApplicationName());
	return(0);
}

