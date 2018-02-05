//	Author:		Wolfgang Wang
//	Created Date:	2002/2/1

#include <stdio.h>
#include <sys/msg.h>

#include "unionMsgQueue.h"
#include "UnionEnv.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionMsgBuf7.x.h"

TUnionMsgQueueGrp		gunionMsgQueueGrp;
PUnionMsgQueueGrp		pgunionMsgQueueGrp = NULL;
int				gunionQueueMDLConnected = 0;

int UnionConnectMsgQueueMDL()
{
	int			i = 0;
	PUnionMsgBufHDL         pMsgBufHDL = NULL;
	
	if (gunionQueueMDLConnected)
		return((pgunionMsgQueueGrp->num));

	if ((pMsgBufHDL = UnionGetCurrentMsgBufHDLGrp()) == NULL)
	{
		UnionUserErrLog("in msgBufConfRunInfo:: UnionGetCurrentMsgBufHDLGrp");
		return(errCodeParameter);
	}
	
	pgunionMsgQueueGrp = &gunionMsgQueueGrp;
	pgunionMsgQueueGrp->num = 0;

	for (i = 0; i < 2; i++)
	{
		pgunionMsgQueueGrp->queue[pgunionMsgQueueGrp->num].userID = pMsgBufHDL->userID + i;

		if ((pgunionMsgQueueGrp->queue[pgunionMsgQueueGrp->num].systemID 
			= msgget(pgunionMsgQueueGrp->queue[pgunionMsgQueueGrp->num].userID,0666)) < 0)
		{
			UnionSystemErrLog("in UnionConnectMsgQueueMDL:: msgget [%d]\n",pgunionMsgQueueGrp->queue[pgunionMsgQueueGrp->num].userID);
			return(errCodeParameter);
		}

		pgunionMsgQueueGrp->num++;
	}

	if ((pgunionMsgQueueGrp->num) > 0)
		gunionQueueMDLConnected = 1;
	return(pgunionMsgQueueGrp->num);
}

int UnionDisconnectMsgQueueMDL()
{
	if (pgunionMsgQueueGrp != NULL);
		pgunionMsgQueueGrp->num = 0;
	gunionQueueMDLConnected = 0;
	return(0);
}

int UnionQueryAllMsgQueueStatus()
{
	struct msqid_ds 	pmsgq;
	int			index;
	int			ret;
	PUnionMsgQueueRec	prec;
	
	if ((ret = UnionConnectMsgQueueMDL()) < 0)
	{
		UnionUserErrLog("in UnionQueryAllMsgQueueStatus:: UnionConnectMsgQueueMDL!\n");
		return(ret);
	}
	
	for (index = 0; index < pgunionMsgQueueGrp->num; index++)
	{
		prec = &(pgunionMsgQueueGrp->queue[index]);
		if (msgctl(prec->systemID,IPC_STAT,&pmsgq) != 0)
		{
			UnionSystemErrLog("in UnionQueryAllMsgQueueStatus::Cannot Control Message Queue %d",prec->systemID);
			continue;
		}
		prec->msgNum = pmsgq.msg_qnum;
		prec->charNum = pmsgq.msg_cbytes;
	}
	return(0);
}

// 2008/11/25,王纯军增加
PUnionMsgQueueGrp UnionGetAllMsgQueueStatus()
{
	int	ret;
	
	if ((ret = UnionQueryAllMsgQueueStatus()) < 0)
	{
		return(NULL);
	}
	return(pgunionMsgQueueGrp);
}
