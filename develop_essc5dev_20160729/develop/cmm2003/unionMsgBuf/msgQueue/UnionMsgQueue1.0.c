//	Author:		Wolfgang Wang
//	Created Date:	2002/2/1

#include <stdio.h>
#include <sys/msg.h>

#include "UnionMsgQueue0.h"
#include "UnionEnv.h"
#include "UnionLog.h"

int				gQueueNumBeMonitored 	= -1;
TUnionMsgQueueRec		QueueBeMonitored[gMsgQueueMaxQueueNum];


int UnionConnectMsgQueueMDL(char *FileName)
{
	int	i;
	char	*pVar;
	char	VarName[20];
	int	Ret;
	char	tmpBuf[512];
		
	// Ever Connected, then no reconnected needed.
	if (gQueueNumBeMonitored >= 0)
		return(0);
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%s/etc/%s",getenv("HOME"),FileName);	
	if ((Ret = UnionInitEnvi(tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionConnectMsgQueueMDL:: UnionInitEnvi!\n");
		return(Ret);
	}
		
	gQueueNumBeMonitored = 0;
	for (i = 0; i < gMsgQueueMaxQueueNum; i++)
	{
		memset(VarName,0,sizeof(VarName));
		sprintf(VarName,"Queue%02d",i);
		if ((pVar = UnionGetEnviVarByName(VarName)) == NULL)
			continue;
		QueueBeMonitored[gQueueNumBeMonitored].QueueID = atoi(pVar);
		if ((QueueBeMonitored[gQueueNumBeMonitored].MsgQueueID 
		    = msgget(QueueBeMonitored[gQueueNumBeMonitored].QueueID,0666 | IPC_CREAT)) < 0)
		{
			UnionSystemErrLog("in UnionConnectMsgQueueMDL:: msgget [%d]\n",QueueBeMonitored[gQueueNumBeMonitored].QueueID);
			continue;
		}
		gQueueNumBeMonitored++;
	}
	
	if (UnionClearEnvi() < 0)
	{
		UnionUserErrLog("in UnionConnectMsgQueueMDL:: UnionClearEnvi!\n");
	}

	UnionMsgQueueRefreshAllRec();
		
	return(0);
}

int UnionDisconnectMsgQueueMDL()
{
	gQueueNumBeMonitored = -1;
	return(0);
}

int UnionMsgQueueRefreshAllRec()
{
	int i;
	struct msqid_ds pmsgq;
	
	
	for (i = 0; i < gQueueNumBeMonitored; i++)
	{
		if (msgctl(QueueBeMonitored[i].MsgQueueID,IPC_STAT,&pmsgq) != 0)
		{
			UnionSystemErrLog("in UnionMsgQueueSelectAllRec::Cannot Control Message Queue %d",QueueBeMonitored[i].MsgQueueID);
			continue;
		}
		QueueBeMonitored[i].CurrentMsgNum = pmsgq.msg_qnum;
		QueueBeMonitored[i].CurrentCharNum = pmsgq.msg_cbytes;
	}
	return(0);
}

long UnionMsgQueueOutputAllRec(unsigned char *Buf,int SizeOfBuf)
{
	int i;
	struct msqid_ds pmsgq;
	
	if (SizeOfBuf < sizeof(TUnionMsgQueueRec) * gQueueNumBeMonitored)
	{
		UnionUserErrLog("in UnionMsgQueueSelectAllRec:: Input Bufsize [%d] less than Expected [%d]\n",SizeOfBuf,sizeof(TUnionMsgQueueRec) * gQueueNumBeMonitored);
		return(gErrMsgQueueSmallBuf);
	}
	
	if (Buf == NULL)
	{
		UnionUserErrLog("in UnionMsgQueueSelectAllRec:: Null Pointer!\n");
		return(gErrMsgQueueNullPointer);
	}
	
	UnionMsgQueueRefreshAllRec();
	
	memcpy(Buf,QueueBeMonitored,sizeof(TUnionMsgQueueRec) * gQueueNumBeMonitored);
	
	return(gQueueNumBeMonitored * sizeof(TUnionMsgQueueRec));
}
