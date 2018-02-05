#include <stdio.h>
#include <sys/msg.h>
#include <curses.h>
#include <signal.h>

#include "UnionEnv.h"
#include "UnionTask.h"
#include "UnionLog.h"

#define gLineOfWin		22
#define gColOfWin		80
#define gLineOffsetOfHsmWin	1
#define gColOffsetOfHsmWin	0

#define	MaxTaskNum		(gLineOfWin - 2) * 3

char		gTitle[80];
int 		gIsDebug = 1;
int 		gIDOfTaskMDL = 30012;
int 		gSizeOfLogFile = 10000000;
// Defined in HsmGrp.a

WINDOW		*gQueueMonWin = NULL;
WINDOW		*gTimeWin = NULL;
WINDOW		*gTitleWin = NULL;

#define		MAXQUEUENUM	(gLineOfWin - 2)
//
typedef struct
{
	int QueueId;		// 
	int MsgQueueId;		// Identification of a message queue;
	int CurrentMsgNum;	// Current Message Num in Queue;
	int CurrentCharNum;	// Current Chars in Queue;
} TMyMsgQ;
typedef TMyMsgQ		*PMyMsgQ;


int		QueueBeMonitoredNum 	= 0;
TMyMsgQ		QueueBeMonitored	[MAXQUEUENUM];

void DealSigUsr1();

int DrawTitleBox(WINDOW	*pWin,char *Title)
{
	int	i,j;
	int	Pos;
	
	if (pWin == NULL)
		return(-1);
		
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

int StartQueueMonitor()
{
	initscr();

	if ((gTitleWin = newwin(1,80,0,0)) == NULL)
		return(-1);
	wmove(gTitleWin,0,1);
	waddstr(gTitleWin,gTitle);
	wrefresh(gTitleWin);
	
	if ((gQueueMonWin = newwin(gLineOfWin,gColOfWin,gLineOffsetOfHsmWin,gColOffsetOfHsmWin)) == NULL)
		return(-1);
	if ((gTimeWin = newwin(1,80,gLineOffsetOfHsmWin + gLineOfWin,0)) == NULL)
		return(-1);
	
	DrawTitleBox(gQueueMonWin,"");

	DisplayTime();
	
	//wrefresh(gQueueMonWin);
	//wrefresh(gResTransWin);
	
	return(0);
}

int CloseQueueMonitor()
{
	clear();
	refresh();
	endwin();
	return(0);
}

int DisplayTime()
{
	char	Buf[128];
	char	SystemDateTime[40];
	
	memset(SystemDateTime,0,sizeof(SystemDateTime));
	UnionGetFullSystemDateTime(SystemDateTime);
	
	memset(Buf,0,sizeof(Buf));
	
	sprintf(Buf,"当前时间：%s		广州科友科技股份有限公司   2001年10月",SystemDateTime);
	
	wmove(gTimeWin,0,1);
	waddstr(gTimeWin,Buf);
	wrefresh(gTimeWin);
	
	return(0);
}

int InitQueueBeMonitored(char *FileName)
{
	int	i;
	char	*pVar;
	char	VarName[20];
	char	tmpBuf[512];
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%s/etc/%s",getenv("HOME"),FileName);
	
	if (UnionInitEnvi(FileName) < 0)
	{
		UnionUserErrLog("in InitQueueBeMonitored:: UnionInitEnvi!\n");
		return(-1);
	}
		
	QueueBeMonitoredNum = 0;
	for (i = 0; i < MAXQUEUENUM; i++)
	{
		memset(VarName,0,sizeof(VarName));
		sprintf(VarName,"Queue%02d",i);
		if ((pVar = UnionGetEnviVarByName(VarName)) == NULL)
			continue;
		QueueBeMonitored[QueueBeMonitoredNum].QueueId = atoi(pVar);
		if ((QueueBeMonitored[QueueBeMonitoredNum].MsgQueueId 
		    = msgget(QueueBeMonitored[QueueBeMonitoredNum].QueueId,0666 | IPC_CREAT)) < 0)
		{
			UnionSystemErrLog("in InitQueueBeMonitored:: msgget [%d]\n",QueueBeMonitored[QueueBeMonitoredNum].QueueId);
			continue;
		}
		QueueBeMonitoredNum++;
	}
	
	memset(gTitle,0,sizeof(gTitle));
	if ((pVar = UnionGetEnviVarByName("Title")) == NULL)
	{
		strcpy(gTitle,"消息队列监控程序");
	}
	else
	{
		if (strlen(pVar) >= sizeof(gTitle))
			memcpy(gTitle,pVar,sizeof(gTitle)-1);
		else
			strcpy(gTitle,pVar);
	}
	if ((pVar = UnionGetEnviVarByName("Debug")) != NULL)
		gIsDebug = atoi(pVar);
		
	if ((pVar = UnionGetEnviVarByName("IDOfTaskMDL")) != NULL)
		gIDOfTaskMDL = atoi(pVar);
	UnionLog("in InitializeEnvi:: gIDOfTaskMDL = [%d]\n",gIDOfTaskMDL);
	
	if ((pVar = UnionGetEnviVarByName("SizeOfLogFile")) != NULL)
		gSizeOfLogFile = atol(pVar);

	if (UnionClearEnvi() < 0)
	{
		UnionUserErrLog("in InitQueueBeMonitored:: UnionClearEnvi!\n");
	}
	
	return(QueueBeMonitoredNum);
}

int GetQueueInfo()
{
	int i;
	struct msqid_ds pmsgq;
	
	
	for (i = 0; i < QueueBeMonitoredNum; i++)
	{
		if (msgctl(QueueBeMonitored[i].MsgQueueId,IPC_STAT,&pmsgq) != 0)
		{
			UnionSystemErrLog("in GetQueueInfo::Cannot Control Message Queue %d",QueueBeMonitored[i].MsgQueueId);
			continue;
		}
		QueueBeMonitored[i].CurrentMsgNum = pmsgq.msg_qnum;
		QueueBeMonitored[i].CurrentCharNum = pmsgq.msg_cbytes;
	}
	return;
}


int DisplayQueueInfo()
{
	int i;
	char	buf[128];

	memset(buf,0,sizeof(buf));
	sprintf(buf,"%12s         %12s    %8s       %8s"," 用户队列号 "," 系统队列号 "," 消息数 "," 字节数 ");
	wmove(gQueueMonWin,1,2);
	waddstr(gQueueMonWin,buf);
	for (i = 0; i < QueueBeMonitoredNum; i++)
	{
		memset(buf,0,sizeof(buf));
		sprintf(buf,"%012x    %12d      %8d        %8d",
			QueueBeMonitored[i].QueueId,
			QueueBeMonitored[i].MsgQueueId,
			QueueBeMonitored[i].CurrentMsgNum,
			QueueBeMonitored[i].CurrentCharNum);
		wmove(gQueueMonWin,i+2,2);
		waddstr(gQueueMonWin,buf);
	}
	wrefresh(gQueueMonWin);
	return(0);
}
		

int InitializeResource()
{
	int	Ret;
	
	if ((Ret = UnionConnectTaskMDL("QueueMon",gIDOfTaskMDL)) < 0)
	{
		UnionPrintf("in InitializeResource:: UnionConnectMDL Failure!! Ret = [%d]\n",Ret);
		return(Ret);
	}
	
	
	return(0);
}

int ReleaseResource()
{
	int	Ret;
	
	if ((Ret = UnionDisconnectTaskMDL()) < 0)
	{
		UnionPrintf("in ReleaseResource:: UnionDisconnectMDL!\n");
		return(Ret);
	}
	
	return(0);
}

int main(int argc,char **argv)
{
	int	Len;
	int	Ret;

	if (argc < 2)
	{
		printf("Usage:: queueMon1.x queueMonFileName\n");
		return(-1);
	}
	
	if (InitQueueBeMonitored(argv[1]) < 0)
	{
		printf("in main:: No Queue Be Monitored!\n");
		return(-1);
	}
	
	UnionIgnoreSignals();
		
	if ((Ret = InitializeResource()) < 0)
		return(Ret);
		
	if ((Ret = StartQueueMonitor()) < 0)
		return(Ret);

	signal(SIGUSR1,DealSigUsr1);
	
	UnionRegisterTask("QueueMon2.0");
	
	for (;;)
	{
		GetQueueInfo();
		DisplayQueueInfo();
		DisplayTime();
		sleep(1);
	}
}

void DealSigUsr1()
{
	UnionLog("in DealSigUsr1:: SigUsr1 Received!\n");
	CloseQueueMonitor();
	UnionUnregisterTask();
	ReleaseResource();
	UnionLog("in DealSigUsr1:: Exit!\n");
	exit(-1);
}

// This Following Functions will be called by UnionLog Module
int UnionGetNameOfLogFile(char *NameOfLogFile)
{
	sprintf(NameOfLogFile,"%s/log/QueueMon.log",getenv("HOME"));
	return(0);
}

int UnionIsDebug()
{
	return(gIsDebug);
}

long UnionGetSizeOfLogFile()
{
	return(gSizeOfLogFile);
}

// The Following Functions will be called by UnionTask Module
int UnionGetIDOfTaskMDL()
{
	return(gIDOfTaskMDL);
}

