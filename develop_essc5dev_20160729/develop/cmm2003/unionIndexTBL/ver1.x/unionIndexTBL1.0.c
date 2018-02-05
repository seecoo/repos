//	Author:	Wolfgang Wang
//	Date:	2003/11/04

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>

#include "UnionLog.h"
#include "unionIndexTBL.h"
#include "unionErrCode.h"

int gunionIndexTBLTimeout = 0;
#define conTimeoutValueOfIndexTBL 30

void UnionDealIndexTBLTimeout();

int UnionMsgRcv(int id,PUnionIndexStatus buf,int sizeOfBuf,long type)
{
	int	ret;
	
	if (gunionIndexTBLTimeout)
		gunionIndexTBLTimeout = 0;
	alarm(conTimeoutValueOfIndexTBL);
	signal(SIGALRM,UnionDealIndexTBLTimeout);
	if (gunionIndexTBLTimeout)
	{
		UnionUserErrLog("in UnionMsgRcv:: timeout type [%ld]\n",type);
		gunionIndexTBLTimeout = 0;
		alarm(0);
		return(errIndexTBLTimeout);
	}
	
	if ((ret = msgrcv(id,buf,sizeOfBuf,type,~IPC_NOWAIT)) < 0)
		UnionSystemErrLog("in UnionMsgRcv:: msgrcv!\n");
	alarm(0);
	return(ret);
}

int UnionMsgSnd(int id,PUnionIndexStatus buf,int len)
{
	int	ret;
	
	if (gunionIndexTBLTimeout)
		gunionIndexTBLTimeout = 0;
	alarm(conTimeoutValueOfIndexTBL);
	signal(SIGALRM,UnionDealIndexTBLTimeout);
	if (gunionIndexTBLTimeout)
	{
		UnionUserErrLog("in UnionMsgSnd:: timeout!\n");
		gunionIndexTBLTimeout = 0;
		alarm(0);
		return(errIndexTBLTimeout);
	}
	if ((ret = msgsnd(id,buf,len,~IPC_NOWAIT)) < 0)
		UnionSystemErrLog("in UnionMsgSnd:: msgsnd!\n");
	alarm(0);
	return(ret);
}

void UnionDealIndexTBLTimeout()
{
	gunionIndexTBLTimeout = 1;
	return;
}
PUnionIndexStatusTBL UnionConnectIndexStatusTBL(int userID,int maxNumOfIndex)
{
	int		newCreated = 0;
	PUnionIndexStatusTBL	ptbl;
	
	//	检查参数
	if ((userID <= 0) || (maxNumOfIndex <= 0))
	{
		UnionUserErrLog("in UnionConnectIndexStatusTBL:: userID [%d] or maxNumOfIndex [%d] Error!\n",
				userID,maxNumOfIndex);
		return(NULL);
	}

	if ((ptbl = (PUnionIndexStatusTBL)malloc(sizeof(TUnionIndexStatusTBL))) == NULL)
	{
		UnionSystemErrLog("in UnionConnectIndexStatusTBL:: malloc!\n");
		return(NULL);
	}

	if ((ptbl->id = msgget(userID,0666)) == -1)
	{
		if ((ptbl->id = msgget(userID,0666 | IPC_CREAT)) == -1)
		{
			UnionSystemErrLog("in UnionConnectIndexStatusTBL:: msgget [%d]!\n",userID);
			UnionDisconnectIndexStatusTBL(ptbl);
			return(NULL);
		}
		newCreated = 1;
	}

	ptbl->tblDef.userID = userID;
	ptbl->tblDef.maxNumOfIndex = maxNumOfIndex;

	if (newCreated)
	{
		UnionResetAllIndexAvailable(ptbl);
	}
	
	return(ptbl);
}

int UnionDisconnectIndexStatusTBL(PUnionIndexStatusTBL ptbl)
{
	if (ptbl != NULL)
	{
		free(ptbl);
		ptbl = NULL;
	}
	
	return(0);
}

int UnionRemoveIndexStatusTBL(int userID)
{
	int 			resID;
	char			tmpBuf[80];

	if ((resID = msgget(userID,0666)) >= 0)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"ipcrm -q %d",resID);
		return(system(tmpBuf));
	}
	else
	{
		UnionUserErrLog("in UnionRemoveIndexStatusTBL:: msgget! [%d]\n",userID);
		return(errCodeUseOSErrCode);
	}
}	

// 将index置为可用
int UnionSetIndexAvailable(PUnionIndexStatusTBL ptbl,int index)
{
	TUnionIndexStatus	indexStatus;
	
	if ((ptbl == NULL) || (index < 0) || (index >= ptbl->tblDef.maxNumOfIndex))
	{
		UnionUserErrLog("in UnionSetIndexAvailable:: parameter error!\n");
		return(errCodeParameter);
	}
	
	indexStatus.status = conAvailabeIndex;
	memcpy(indexStatus.index,&index,sizeof(int));
	
	return(UnionMsgSnd(ptbl->id,&indexStatus,sizeof(indexStatus.index)));
	//return(msgsnd(ptbl->id,&indexStatus,sizeof(indexStatus.index),~IPC_NOWAIT));
}

// 获得可用的位置
// 返回值为获得的位置
int UnionGetAvailableIndex(PUnionIndexStatusTBL ptbl)
{
	TUnionIndexStatus	indexStatus;
	int			ret;
	int			index;
	
	if (ptbl == NULL)
	{
		UnionUserErrLog("in UnionGetAvailableIndex:: parameter error!\n");
		return(errCodeParameter);
	}
	
	memset(indexStatus.index,0,sizeof(indexStatus.index));
	
	//if ((ret = msgrcv(ptbl->id,&indexStatus,sizeof(indexStatus.index),conAvailabeIndex,~IPC_NOWAIT)) < 0)
	if ((ret = UnionMsgRcv(ptbl->id,&indexStatus,sizeof(indexStatus.index),conAvailabeIndex)) < 0)
		return(ret);
	else
	{
		memcpy(&index,indexStatus.index,sizeof(int));
		return(index);
	}
}

// 将指定索引的状态置为用户指定的状态
int UnionSetIndexWithUserStatus(PUnionIndexStatusTBL ptbl,int index,long status)
{
	TUnionIndexStatus	indexStatus;
	int			ret;
		
	if ((ptbl == NULL) || (index < 0) || (index >= ptbl->tblDef.maxNumOfIndex) || (status < 0))
	{
		UnionUserErrLog("in UnionSetIndexWithUserStatus:: parameter error!\n");
		return(errCodeParameter);
	}
	
	indexStatus.status = conOccupiedIndex + status;
	memcpy(indexStatus.index,&index,sizeof(int));
	
	//if ((ret = msgsnd(ptbl->id,&indexStatus,sizeof(indexStatus.index),~IPC_NOWAIT)) < 0)
	if ((ret = UnionMsgSnd(ptbl->id,&indexStatus,sizeof(indexStatus.index))) < 0)
		return(ret);
	else
		return(0);
}

// 获得第一个状态为指定状态位置，返回获得的位置
int UnionGetFirstIndexOfUserStatus(PUnionIndexStatusTBL ptbl,long status)
{
	TUnionIndexStatus	indexStatus;
	int			ret;
	int			index;
	
	if ((ptbl == NULL) || (status < 0))
	{
		UnionUserErrLog("in UnionGetFirstIndexOfUserStatus:: parameter error!\n");
		return(errCodeParameter);
	}
	
	memset(indexStatus.index,0,sizeof(indexStatus.index));
	
	//if ((ret = msgrcv(ptbl->id,&indexStatus,sizeof(indexStatus.index),conOccupiedIndex + status,~IPC_NOWAIT)) < 0)
	if ((ret = UnionMsgRcv(ptbl->id,&indexStatus,sizeof(indexStatus.index),conOccupiedIndex + status)) < 0)
		return(ret);
	else
	{
		memcpy(&index,indexStatus.index,sizeof(int));
		return(index);
	}
}

// 获得第一个状态为指定状态位置，返回获得的位置
int UnionGetFirstIndexOfUserStatusUntilSuccess(PUnionIndexStatusTBL ptbl,long status)
{
	TUnionIndexStatus	indexStatus;
	int			ret;
	int			index;
	
	if ((ptbl == NULL) || (status < 0))
	{
		UnionUserErrLog("in UnionGetFirstIndexOfUserStatusUntilSuccess:: parameter error!\n");
		return(errCodeParameter);
	}
	
	memset(indexStatus.index,0,sizeof(indexStatus.index));
	
	if ((ret = msgrcv(ptbl->id,&indexStatus,sizeof(indexStatus.index),conOccupiedIndex + status,~IPC_NOWAIT)) < 0)
	{
		UnionSystemErrLog("in UnionGetFirstIndexOfUserStatusUntilSuccess:: msgrcv! ret = [%d]\n",ret);
		return(ret);
	}
	else
	{
		memcpy(&index,indexStatus.index,sizeof(int));
		return(index);
	}
}

// 将状态为指定状态的位置置为可用
int UnionSetIndexOfUserStatusAvailable(PUnionIndexStatusTBL ptbl,long status)
{
	TUnionIndexStatus	indexStatus;
	int			ret;
	int			index;
	
	if ((ptbl == NULL) || (status < 0))
	{
		UnionUserErrLog("in UnionSetIndexOfUserStatusAvailable:: parameter error!\n");
		return(errCodeParameter);
	}
	
	for (;;)
	{
		memset(indexStatus.index,0,sizeof(indexStatus.index));
		if ((ret = msgrcv(ptbl->id,&indexStatus,sizeof(indexStatus.index),conOccupiedIndex + status,IPC_NOWAIT)) < 0)
			return(0);
		memcpy(&index,indexStatus.index,sizeof(int));
		UnionSetIndexAvailable(ptbl,index);
	}
}

// 将状态为指定状态的位置置为可用
int UnionSetIndexOfUserStatusAndIndexAvailable(PUnionIndexStatusTBL ptbl,long status,int index)
{
	TUnionIndexStatus	indexStatus;
	int			ret;
	
	if ((ptbl == NULL) || (status < 0))
	{
		UnionUserErrLog("in UnionSetIndexOfUserStatusAvailable:: parameter error!\n");
		return(errCodeParameter);
	}
	
	for (;;)
	{
		memset(indexStatus.index,0,sizeof(indexStatus.index));
		if ((ret = msgrcv(ptbl->id,&indexStatus,sizeof(indexStatus.index),conOccupiedIndex + status,IPC_NOWAIT)) < 0)
			return(0);
		memcpy(&index,indexStatus.index,sizeof(int));
		if (index == index)
			UnionSetIndexAvailable(ptbl,index);
		else
			UnionSetIndexWithUserStatus(ptbl,index,status);
	}
}

// 将所有位置置为可用
int UnionResetAllIndexAvailable(PUnionIndexStatusTBL ptbl)
{
	TUnionIndexStatus	indexStatus;
	int			ret;
	int			index;
	
	if (ptbl == NULL)
	{
		UnionUserErrLog("in UnionResetAllIndexAvailable:: parameter error!\n");
		return(errCodeParameter);
	}
	
	for (;;)
	{
		//memset(indexStatus.index,0,sizeof(indexStatus.index));
		if ((ret = msgrcv(ptbl->id,&indexStatus,sizeof(indexStatus.index),0,IPC_NOWAIT)) < 0)
			break;
	}
	
	
	for (index = 0; index < ptbl->tblDef.maxNumOfIndex; index++)
		UnionSetIndexAvailable(ptbl,index);
	
	return(0);
}

// 重置最大索引数目
int UnionResetMaxIndexNumOfIndexStatusTBL(PUnionIndexStatusTBL ptbl,int maxNumOfIndex)
{
	if ((ptbl == NULL) || (maxNumOfIndex <= 0))
	{
		UnionUserErrLog("in UnionResetMaxIndexNumOfIndexStatusTBL:: parameter error!\n");
		return(errCodeParameter);
	}
	
	ptbl->tblDef.maxNumOfIndex = maxNumOfIndex;
	
	return(UnionResetAllIndexAvailable(ptbl));
}

