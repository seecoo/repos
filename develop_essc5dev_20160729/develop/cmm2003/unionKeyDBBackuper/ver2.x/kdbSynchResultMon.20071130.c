//	Wolfgang Wang
//	2001/11/15
//	modify   jevons
//	2004/08/23  �����˹���ͨ�ļ����Ϣ��ʾ


#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <setjmp.h>
#include <signal.h>
#ifdef _AIX_
#include <cur00.h>
#else
#include <curses.h>
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif
#include "UnionTask.h"
#ifndef _UnionSocket_3_x_
#define _UnionSocket_3_x_
#endif

#include "keyDBBackuper.h"
#include "kdbSvrService.h"
#include "unionVersion.h"
#include "UnionTask.h"
#include "unionMonitor.h"
#include "UnionLog.h"

PUnionTaskInstance			ptaskInstance = NULL;
extern WINDOW				*gTimeWin;

#define	conMaxNumOfInfoPerWin		((gLineOfWin-2)/2-1)
#define conNormalInfoStartLine		1
#define conAbnormalInfoStartLine	(1+conMaxNumOfInfoPerWin+1)

TUnionKeySynchronizeResult		gunionNormalInfo[conMaxNumOfInfoPerWin];	// �������
int					gunionNormalInfoNum = 0;
TUnionKeySynchronizeResult		gunionAbnormalInfo[conMaxNumOfInfoPerWin];	// �쳣���
int					gunionAbnormalInfoNum = 0;
int					isAbnormalInfo = 0;
char					gunionOperRemark[20+1] = "";
int					interval = 60;

extern WINDOW				*pgContentWin;

int UnionHelp()
{
	printf("Usage:: %s [check|checkbrothers]\n",UnionGetApplicationName());
	return(0);
}

char *UnionFindKeyDBOperRemark(TUnionKeyDBOperation oper)
{
	switch (oper)
	{
		case conInsertRecord:
			strcpy(gunionOperRemark,"������Կ");
			break;
		case conDeleteRecord:
			strcpy(gunionOperRemark,"ɾ����Կ");
			break;
		case conUpdateRecord:
			strcpy(gunionOperRemark,"������Կ");
			break;
		case conUpdateRecordAllFlds:
			strcpy(gunionOperRemark,"����������");
			break;
		case conUpdateRecordValueFlds:
			strcpy(gunionOperRemark,"��������");
			break;
		case conUpdateRecordAttrFlds:
			strcpy(gunionOperRemark,"����ֵ");
			break;
		case conCompareKeyValue:
			strcpy(gunionOperRemark,"�������");
			break;
		default:
			strcpy(gunionOperRemark,"��������");
			break;
	}
	return(gunionOperRemark);
}

int UnionTaskActionBeforeExit()
{
	int	ret;
	char	data[100+1];
	char	*fileName;
	
	CloseTaskMonitor();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int UnionRecvOneInfo()
{
	int				ret;
	int				i;
	TUnionKeySynchronizeResult	result;

	memset(&result,0,sizeof(result));
	if ((ret = UnionReadKeySynchronizeResultSpierRequest(&result)) < 0)
	{
		UnionUserErrLog("in UnionRecvOneInfo:: UnionReadKeySynchronizeResultSpierRequest! ret = [%d]\n",ret);
		return(ret);
	}
		
	if (result.resCode < 0)	// �ж������������쳣��Ϣ
		isAbnormalInfo = 1;
	else
		isAbnormalInfo = 0;

	if (isAbnormalInfo)
	{
		if ((gunionAbnormalInfoNum < 0) || (gunionAbnormalInfoNum > conMaxNumOfInfoPerWin))
			gunionAbnormalInfoNum = 0;
		if (gunionAbnormalInfoNum < conMaxNumOfInfoPerWin)
		{
			memcpy(&(gunionAbnormalInfo[gunionAbnormalInfoNum]),&result,sizeof(result));
			gunionAbnormalInfoNum++;
			return(0);
		}
		for (i = 0; i < conMaxNumOfInfoPerWin-1; i++)
		{
			memcpy(&(gunionAbnormalInfo[i]),&(gunionAbnormalInfo[i+1]),sizeof(result));
		}
		memcpy(&(gunionAbnormalInfo[conMaxNumOfInfoPerWin-1]),&result,sizeof(result));
	}
	else
	{
		if ((gunionNormalInfoNum < 0) || (gunionNormalInfoNum > conMaxNumOfInfoPerWin))
			gunionNormalInfoNum = 0;
		if (gunionNormalInfoNum < conMaxNumOfInfoPerWin)
		{
			memcpy(&(gunionNormalInfo[gunionNormalInfoNum]),&result,sizeof(result));
			gunionNormalInfoNum++;
			return(0);
		}
		for (i = 0; i < conMaxNumOfInfoPerWin-1; i++)
		{
			memcpy(&(gunionNormalInfo[i]),&(gunionNormalInfo[i+1]),sizeof(result));
		}
		memcpy(&(gunionNormalInfo[conMaxNumOfInfoPerWin-1]),&result,sizeof(result));
	}
	return(0);
}

int UnionRefreshNormalInfoWin()
{
	int	i;
	char	Buf[512+100];
	int	len;

	if (pgContentWin == NULL)
	{
		UnionUserErrLog("in UnionRefreshNormalInfoWin:: pgContentWin is NULL\n");
		return(errCodeParameter);
	}

	for (i = 0; (i < gunionNormalInfoNum) && (i < conMaxNumOfInfoPerWin); i++)
	{
		memset(Buf,0,sizeof(Buf));
		sprintf(Buf,"%10s %10s %15s %6d �ɹ� %s",
			gunionNormalInfo[i].time,
			UnionFindKeyDBOperRemark(gunionNormalInfo[i].operation),
			gunionNormalInfo[i].ipAddr,
			gunionNormalInfo[i].port,
			gunionNormalInfo[i].fullKeyName);
		wmove(pgContentWin,conNormalInfoStartLine + i + 1,2);
		//UnionNullLog("Buf = [%s]\n",Buf);
		if ((len = strlen(Buf)) >= 80 - 4)
		{
			memcpy(Buf + 80 - 7,"...",3);
			Buf[80-4] = 0;
		}
		else
		{
			memset(Buf+len,' ',80-4-len);
		}
		waddstr(pgContentWin,Buf);
	}
	wrefresh(pgContentWin);
	return(0);
}

int UnionRefreshAbnormalInfoWin()
{
	int	i;
	char	Buf[4096+100];
	int	len;
	char	*operationName;
	char	defaultName[100];
	
	if (pgContentWin == NULL)
	{
		UnionUserErrLog("in UnionRefreshAbnormalInfoWin:: pgContentWin is NULL\n");
		return(errCodeParameter);
	}

	for (i = 0; (i < gunionAbnormalInfoNum) && (i < conMaxNumOfInfoPerWin); i++)
	{
		memset(Buf,0,sizeof(Buf));
		sprintf(Buf,"%10s %10s %15s %6d ʧ�� %s",
			gunionAbnormalInfo[i].time,
			UnionFindKeyDBOperRemark(gunionAbnormalInfo[i].operation),
			gunionAbnormalInfo[i].ipAddr,
			gunionAbnormalInfo[i].port,
			gunionAbnormalInfo[i].fullKeyName);
		wmove(pgContentWin,conAbnormalInfoStartLine + i + 1,2);
		//UnionNullLog("Buf = [%s]\n",Buf);
		if ((len = strlen(Buf)) >= 80 - 4)
		{
			memcpy(Buf + 80 - 7,"...",3);
			Buf[80-4] = 0;
		}
		else
		{
			memset(Buf+len,' ',80-4-len);
		}
		waddstr(pgContentWin,Buf);
	}
	wrefresh(pgContentWin);
	return(0);
}

int UnionRefreshTransMonWin()
{
	char	Buf[128+1];
	int	i;
	
	memset(Buf,0,sizeof(Buf));
	sprintf(Buf,"%10s %10s %15s %6s %4s %s",
		"ʱ��","ͬ������","IP��ַ","�˿�","���","��Կ����");
	wmove(pgContentWin,conNormalInfoStartLine,2);
	waddstr(pgContentWin,Buf);
	memset(Buf,0,sizeof(Buf));
	wmove(pgContentWin,conAbnormalInfoStartLine,0);
	waddstr(pgContentWin,"��");
	for (i = 2;i < 80-2; i = i+2)
	{
		wmove(pgContentWin,conAbnormalInfoStartLine,i);
		waddstr(pgContentWin,"��");
	}
	wmove(pgContentWin,conAbnormalInfoStartLine,78);
	waddstr(pgContentWin,"��");
	wrefresh(pgContentWin);
	
	for (;;)
	{
		if (UnionRecvOneInfo() < 0)
			continue;
		if (isAbnormalInfo)
			UnionRefreshAbnormalInfoWin();
		else
			UnionRefreshNormalInfoWin();
		DisplayTime();
	}
}

int main(int argc,char *argv[])
{
	int	ret;
	char	taskName[100+1];
	
	UnionSetApplicationName(argv[0]);

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	if (UnionExistsAnotherTaskOfName(conTaskNameOfKDBSynchronizerSpier))
	{
		printf("another task of name [%s] already exists!\n",conTaskNameOfKDBSynchronizerSpier);
		return(UnionTaskActionBeforeExit());	
	}
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,conTaskNameOfKDBSynchronizerSpier)) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	return(UnionMonitorMain(1,argv[0],"�������ƽ̨��Կͬ�����",NULL,UnionRefreshTransMonWin,UnionTaskActionBeforeExit));
}
