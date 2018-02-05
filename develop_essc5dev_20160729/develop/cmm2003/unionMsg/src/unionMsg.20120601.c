//	Author:	Fenglk
//	Date:	2012/06/07
//	v1.0

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>
#include <setjmp.h>

#include "unionMsg.h"
#include "unionErrCode.h"
#include "UnionLog.h"

jmp_buf gunionWaitMsgEnv;
 
void UnionJmpToWaitMsgEnv();


int UnionInitQueue(int resID)
{
	int				ret;
	struct msqid_ds			tmsqds;

	memset(&tmsqds, 0, sizeof(tmsqds));
	if((ret = msgctl(resID, IPC_STAT, &tmsqds)) < 0)
	{
		ret = 0 - abs(errno);
		UnionSystemErrLog("in UnionInitQueue:: msgctl IPC_STAT errno = %d!\n", ret);
		return(ret);
	}
	//tmsqds.msg_qbytes = MAX_MSG_DATA;
	//UnionProgramerLog("in UnionInitQueue:: max size of queue = %ld!\n", tmsqds.msg_qbytes);
	if((ret = msgctl(resID, IPC_SET, &tmsqds)) < 0)
	{
		ret = 0-abs(errno);
		UnionSystemErrLog("in UnionInitQueue:: msgctl IPC_SET max size of queue = [%d]!\n", ret);
		return(ret);
	}
	return(0);
}
int UnionConnectQueue(int userID)
{
	int				resID;

	if((resID = msgget(userID, UNION_QUEUE_PERM)) == -1)
	{
		if ((resID = msgget(userID, UNION_QUEUE_PERM | IPC_CREAT)) == -1)
		{
			UnionSystemErrLog("in UnionConnectQueue:: msgget [%d]!\n", userID);
			return(0-abs(errno));
		}
		// new queue
		UnionInitQueue(resID);
	}
	return(resID);
}

int UnionRemoveQueue(int userID)
{
	int			ret;
	int 			resID;
	char			cmd[32];

	//UnionLog("in: UnionRemoveQueue:: begin userID  = 0x%08x \n", userID);
	if ((resID = msgget(userID, UNION_QUEUE_PERM)) > 0)
	{
		/*
		if((ret = msgctl(resID, IPC_RMID, NULL)) < 0)
		{
			ret = 0 - abs(errno);
			UnionSystemErrLog("in: UnionRemoveQueue:: msgctl IPC_RMID errno = %d!\n", ret);
			return(ret);
		}
		*/
                memset(cmd, 0, sizeof(cmd));
		sprintf(cmd, "ipcrm -q %d", resID);
		ret = system(cmd);
		UnionLog("in: UnionRemoveQueue:: system cmd = [%s] ret = [%d]!\n", cmd, ret);
		return(ret);
	}
	else
	{
		UnionSystemErrLog("in UnionRemoveQueue:: resID = [%d] queue nolonger exist!\n", resID);
		return(errCodeUseOSErrCode);
	}
}

int UnionClearQueueWithTimeout(int resID, int timeout)
{
	int				ret, i;
	struct msqid_ds			tmsqds;
	TUnionQueueData			tqueue;
	time_t				curTime;

	//UnionLog("in UnionClearQueueWithTimeout:: resID = %d timeout = %d!\n", resID, timeout);

	memset(&tmsqds, 0, sizeof(tmsqds));
	if((ret = msgctl(resID, IPC_STAT, &tmsqds)) < 0)
	{
		ret = 0 - abs(errno);
		UnionSystemErrLog("in UnionClearQueueWithTimeout:: msgctl IPC_STAT errno = %d!\n", ret);
		return(ret);
	}

	if(tmsqds.msg_qnum == 0)
	{
		return(0);
	}

	time(&curTime);
	if(curTime - tmsqds.msg_stime > timeout)
	{
		for(i = 0; i < tmsqds.msg_qnum; i++)
		{
			if((ret = UnionRcvQueueData(resID, &tqueue, sizeof(tqueue.data), 0, IPC_NOWAIT)) < 0)
			{	ret = 0 - abs(errno);
				UnionSystemErrLog("in UnionClearQueueWithTimeout:: UnionRcvQueueData ret = %d!\n", ret);
				continue;
			}
		}
		return(i);
	}

	return(0);
}
int UnionClearQueue(int resID)
{
	int				ret, i;
	struct msqid_ds			tmsqds;
	TUnionQueueData			tqueue;

	memset(&tmsqds, 0, sizeof(tmsqds));
	if((ret = msgctl(resID, IPC_STAT, &tmsqds)) < 0)
	{
		ret = 0 - abs(errno);
		UnionSystemErrLog("in UnionClearQueue:: msgctl IPC_STAT errno = %d!\n", ret);
		return(ret);
	}
	if(tmsqds.msg_qnum == 0)
	{
		return(0);
	}
	for(i = 0; i < tmsqds.msg_qnum; i++)
	{
		if((ret = UnionRcvQueueData(resID, &tqueue, sizeof(tqueue.data), 0, IPC_NOWAIT)) < 0)
		{	ret = 0 - abs(errno);
			UnionSystemErrLog("in UnionClearQueue:: UnionRcvQueueData ret = %d!\n", ret);
			continue;
		}
	}
	return(i);
}
int UnionRcvQueueData(int resID, PUnionQueueData pqueue, int queueSize, long type, int flag)
{
	int				ret;

	//UnionProgramerLog("in UnionRcvQueueData:: rcv from resID = [%d] type = %d flag = %d, bufsize = %d!\n", resID, type, flag, queueSize);
	if((ret = msgrcv(resID, pqueue, queueSize, type, flag)) < 0)
	{
		ret = 0 - abs(errno);
		UnionSystemErrLog("in UnionRcvQueueData:: errno = %d!\n", ret);
		return(ret);
	}
	return(ret);
	
}
int UnionSndQueueData(int resID, PUnionQueueData pqueue, int dataLen, int flag)
{
	int				ret;

	//UnionProgramerLog("in UnionSndQueueData:: snd to resID = [%d] type = %d flag = %d, data = [%s] len = %d!\n", resID, pqueue->type, flag, pqueue->data, dataLen);
	if ((ret = msgsnd(resID, pqueue, dataLen, flag)) < 0)
	{
		UnionSystemErrLog("in UnionSndQueueData:: msgsnd!\n");
		return(errCodeUseOSErrCode);
	}
	else
	{
		return(ret);
	}
}

int UnionPrintQueueInfo(int resID)
{
	int				ret;
	struct msqid_ds			tmsqds;
	time_t				curTime;

	memset(&tmsqds, 0, sizeof(tmsqds));
	if((ret = msgctl(resID, IPC_STAT, &tmsqds)) < 0)
	{
		UnionSystemErrLog("in UnionPrintQueueInfo:: msgctl IPC_STAT errno = %d!\n", ret);
		return(0);
	}

	time(&curTime);
	printf("---------------------------------------------------\n");
	printf("    max bytes on queue:               %ld\n", tmsqds.msg_qbytes);
	printf("    last access time(seconds ago):    %ld\n", curTime - tmsqds.msg_stime);
	printf("    current msg num:                  %ld\n", tmsqds.msg_qnum);
	printf("    current bytes on queue:           %ld\n", tmsqds.msg_cbytes);
	printf("---------------------------------------------------\n");
	return(0);
}

int UnionMaintainQueueData(int resID, int timeout)
{
	return(0);
}
int UnionGetQueueCurrentNum(int resID)
{
	int				ret;
	struct msqid_ds			tmsqds;

	memset(&tmsqds, 0, sizeof(tmsqds));
	if((ret = msgctl(resID, IPC_STAT, &tmsqds)) < 0)
	{
		UnionSystemErrLog("in UnionGetQueueCurrentNum:: msgctl IPC_STAT errno = %d!\n", ret);
		return(0);
	}
	return(tmsqds.msg_qnum);
}

int UnionRcvQueueDataWithTimeout(int resID, PUnionQueueData pqueue, int queueSize, long type, int timeout)
{
	int				ret;

#if ( defined __linux__ ) || ( defined __hpux )
	if (sigsetjmp(gunionWaitMsgEnv, 1) != 0)   // 超时退出
#elif ( defined _AIX )
	if (setjmp(gunionWaitMsgEnv) != 0)        // 超时退出
#endif
	{
		ret = errCodeEsscMDL_TimeoutForReceivingData;
		return(ret);
	}

	alarm(timeout); 
	signal(SIGALRM, UnionJmpToWaitMsgEnv); 
	if((ret = UnionRcvQueueData(resID, pqueue, queueSize, type, 0)) < 0) 
	{        
		UnionUserErrLog("in UnionRcvQueueDataWithTimeout:: UnionRcvQueueData timeout = %d, ret = [%d]!\n", timeout, ret); 
		alarm(0); 
		return(ret); 
	}
	alarm(0); 
	return(ret);
}
 
void UnionJmpToWaitMsgEnv()
{
	UnionUserErrLog("in UnionJmpToWaitMsgEnv:: timeout!\n");
#if ( defined __linux__ ) || ( defined __hpux )
	siglongjmp(gunionWaitMsgEnv, 10);
#elif ( defined _AIX )
	longjmp(gunionWaitMsgEnv, 10);
#endif
}

