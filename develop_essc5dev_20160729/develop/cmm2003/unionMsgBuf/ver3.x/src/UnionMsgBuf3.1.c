//	Author:	Wolfgang Wang
//	Date:	2001/10/22

//	2002/3/7 Wolfgang Wang, rename all the functions of the name XXXXMdl to
//	new name XXXMDL.
//	2002/3/7 Wolfgang Wang, change the parameter of the function 
//		UnionConnectMsgBufMDL, which is formerly named UnionConnectMsgBufMdl.

// 2002/8/17，Wolfgang Wang
//	由2.x版升级为3.x版
//	3.x版与2.x版的差异在于，对于消息不再区分请求与响应，而只有一组消息函数。

// 2003/02/27，Wolfgang Wang
/*
	在3.0基础上升级为3.1
	升级的原因为：
		由于不明原因,msgrcv读取的消息长度会为8，而sizeof(int)长度为4
		接收消息的缓冲的大小为4，从而导致程序异常中止。
	修改：
		经陈家梅查，msgrcv的第3个参数的长度为实际存放消息的最大长度，
		3.0以前版本，该长度为sizeof(PosOfMsgBuf)，现改为sizeof(int)
	
*/
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "UnionMsgBuf3.x.h"
#include "UnionLog.h"

unsigned char *UnionGetFirstMsgAddrOfMsgBufMDL(PUnionMsgBufHDL pMsgBufHDL)
{
	return((unsigned char *)(pMsgBufHDL + sizeof(TUnionMsgBufHDL)));
}

// UserID是用户分配的消息缓冲标识号
// SizeOfBuf是消息缓冲的总长度，以KByte计长度
// LargestSizeOfBuf，是一条消息的最大长度
PUnionMsgBufHDL UnionConnectMsgBufMDL(int UserID,int SizeOfBuf,int LargestSizeOfMsg)
{
	int		ID;
	int		Index;
	unsigned char 	*pFirstMsgAddr;
	PUnionMsgBufHDL pMsgBufHDL;
	int		NewCreated = 0;
	
	//	检查参数
	if (UserID <= 0)
	{
		UnionUserErrLog("in UnionConnectMsgBufMDL:: UserID [%d] Error!\n",UserID);
		return(NULL);
	}
	if ((SizeOfBuf <= 0) || ((SizeOfBuf > gMaxSizeOfMsgBuf) && (SizeOfBuf / 1000) > gMaxSizeOfMsgBuf))
	{
		UnionUserErrLog("in UnionConnectMsgBufMDL:: SizeOfBuf [%d] Error!\n",SizeOfBuf);
		return(NULL);
	}
	if ((LargestSizeOfMsg <= 0) || (LargestSizeOfMsg > SizeOfBuf * 1000) || (LargestSizeOfMsg > gMaxLargestSizeOfMsg))
	{
		UnionUserErrLog("in UnionConnectMsgBufMDL:: LargestSizeOfMsg [%d] Error!\n",SizeOfBuf);
		return(NULL);
	}

	//	初始化MsgBuf
	if ((ID = shmget(UserID,sizeof(TUnionMsgBufHDL) + SizeOfBuf * 1000,0666)) == -1)
	{
		if ((ID = shmget(UserID,sizeof(TUnionMsgBufHDL) + SizeOfBuf * 1000,0666|IPC_CREAT)) == -1)
		{
			UnionSystemErrLog("in UnionConnectMsgBufMDL:: shmget! [%d] [%d] [%d]\n",
					UserID,
					SizeOfBuf * 1000,
					LargestSizeOfMsg);
			return(NULL);
		}
		NewCreated = 1;
	}
			
	if ((pMsgBufHDL = (PUnionMsgBufHDL)shmat(ID,0,SHM_RND)) == NULL)
	{
		UnionSystemErrLog("in UnionConnectMsgBufMDL:: shmat!\n");
		return(NULL);
	}
	if (!NewCreated)
		return(pMsgBufHDL);
		
	pFirstMsgAddr = UnionGetFirstMsgAddrOfMsgBufMDL(pMsgBufHDL);
	pMsgBufHDL->ShareMemoryID = ID;
	
	if ((pMsgBufHDL->QueueIDOfFreePos = msgget(UserID,0666 | IPC_CREAT)) == -1)
	{
		UnionSystemErrLog("in UnionConnectMsgBufMDL:: msgget [%d]!\n",UserID);
	}
			
	if ((pMsgBufHDL->QueueIDOfOccupiedPos = msgget(UserID + 1,0666 | IPC_CREAT)) == -1)
	{
		UnionSystemErrLog("in UnionConnectMsgBufMDL:: msgget [%d]!\n",UserID+1);
		goto UnionConnectMsgBufMDLExit;
	}
			
	pMsgBufHDL->NumOfMsg = SizeOfBuf * 1000 / (LargestSizeOfMsg + sizeof(int));
	pMsgBufHDL->UserID = UserID;
	pMsgBufHDL->SizeOfBuf = SizeOfBuf;
	pMsgBufHDL->LargestSizeOfMsg = LargestSizeOfMsg;
	
	for (Index = 0; Index < pMsgBufHDL->NumOfMsg; Index++)
	{
		if (UnionFreeMsgBufPos(pMsgBufHDL,Index) < 0)
		{
			UnionUserErrLog("in UnionConnectMsgBufMDL:: UnionFreeMsgBufPos!\n");
			goto UnionConnectMsgBufMDLExit;
		}
	}
	
	return(pMsgBufHDL);
	
UnionConnectMsgBufMDLExit:
	shmdt(pMsgBufHDL);
	pMsgBufHDL = NULL;
	return(NULL);
}

int UnionDisconnectMsgBufMDL(PUnionMsgBufHDL pMsgBufHDL)
{
	int	Ret;
	
	if (pMsgBufHDL != NULL)
	{
		if ((Ret = shmdt(pMsgBufHDL)) < 0)
		{
			UnionSystemErrLog("in UnionDisconnectMsgBufMDL:: shmdt!\n");
			return(-1);
		}
	}
	pMsgBufHDL = NULL;
	
	return(0);
}

// 调用该函数之前，一定要先调用UnionConnectMsgBufMDL
int UnionRemoveMsgBufMDL(int UserID,int SizeOfMsgBuf)
{
	struct shmid_ds		buf;
	int 			ID;
	char			Command[80];
	int			Ret = 0;

	if ((ID = msgget(UserID,0666)) >= 0)
	{
		memset(Command,0,sizeof(Command));
		sprintf(Command,"ipcrm -q %d",ID);
		if (system(Command) < 0)
		{
			UnionUserErrLog("in UnionReleaseMsgBufMDL:: [%s]\n",Command);
			Ret--;
		}
	}

	if ((ID = msgget(UserID+1,0666)) >= 0)
	{
		memset(Command,0,sizeof(Command));
		sprintf(Command,"ipcrm -q %d",ID);
		if (system(Command) < 0)
		{
			UnionUserErrLog("in UnionReleaseMsgBufMDL:: [%s]\n",Command);
			Ret--;
		}
	}

	if ((ID = shmget(UserID,sizeof(TUnionMsgBufHDL) + SizeOfMsgBuf * 1000,0666)) == -1)
	{
		UnionSystemErrLog("in UnionRemoveMsgBufMDL:: shmget [%d]\n",UserID);
		Ret--;
	}
	
	if (shmctl(ID,IPC_RMID,&buf) != 0)
	{
		UnionSystemErrLog("in UnionRemoveMsgBufMDL:: shmctl IPC_RMID ID = [%d]\n",UserID);
		Ret--;
	}

	return(Ret);
}	

int UnionPrintStatusOfMsgBufMDL(PUnionMsgBufHDL pMsgBufHDL)
{
	if (pMsgBufHDL == NULL)
		return(0);
	
	printf("Status of Msg Buf:\n");
	printf("[UserID]			[%d]\n",pMsgBufHDL->UserID);
	printf("[SizeOfBuf]			[%d]\n",pMsgBufHDL->SizeOfBuf);
	printf("[LargestSizeOfMsg]		[%d]\n",pMsgBufHDL->LargestSizeOfMsg);
	printf("[NumOfMsgPos]			[%d]\n",pMsgBufHDL->NumOfMsg);
	printf("[QueueIDOfFreePos]		[%d]\n",pMsgBufHDL->QueueIDOfFreePos);
	printf("[QueueIDOfOccupiedPos]	[%d]\n",pMsgBufHDL->QueueIDOfOccupiedPos);
	printf("[ShareMemoryID]			[%d]\n",pMsgBufHDL->ShareMemoryID);
	printf("[Num Of Free Pos]		[%d]\n",UnionGetNumOfFreeMsgBufPos(pMsgBufHDL));
	printf("[Num Of Occupied Pos]		[%d]\n",UnionGetNumOfOccupiedMsgBufPos(pMsgBufHDL));
	return(0);
}

// 将Index对应的消息位置置为可用
int UnionFreeMsgBufPos(PUnionMsgBufHDL pMsgBufHDL,int Index)
{
	TUnionFreePosOfMsgBuf	PosOfMsgBuf;
	
	if (pMsgBufHDL == NULL)
	{
		UnionUserErrLog("in UnionFreeMsgBufPos:: pMsgBufHDL is NULL!\n");
		return(-1);
	}
	
	PosOfMsgBuf.StatusOfPos = gAvailableMsgBufPos;
	memcpy(PosOfMsgBuf.IndexOfPos,&Index,sizeof(int));
	
	if (msgsnd(pMsgBufHDL->QueueIDOfFreePos,&PosOfMsgBuf,sizeof(int),~IPC_NOWAIT) == -1)
	{
		UnionSystemErrLog("in UnionFreeMsgBufPos:: msgsnd [%d]!\n",Index);
		return(-1);
	}
	else
		return(0);
}

// 获得可用的消息位置，返回值为所获位置
int UnionGetAvailableMsgBufPos(PUnionMsgBufHDL pMsgBufHDL)
{
	TUnionFreePosOfMsgBuf	PosOfMsgBuf;
	int			Ret;
	int			Index;
	
	if (pMsgBufHDL == NULL)
	{
		UnionUserErrLog("in UnionGetAvailableMsgBufPos:: pMsgBufHDL is NULL!\n");
		return(-1);
	}
	
	memset(PosOfMsgBuf.IndexOfPos,0,sizeof(PosOfMsgBuf.IndexOfPos));
	
	if ((Ret = msgrcv(pMsgBufHDL->QueueIDOfFreePos,&PosOfMsgBuf,sizeof(int),0,~IPC_NOWAIT)) != sizeof(int))
	{
		UnionSystemErrLog("in UnionGetAvailableMsgBufPos:: msgrcv! Ret = [%d]\n",Ret);
		return(-1);
	}
	memcpy(&Index,PosOfMsgBuf.IndexOfPos,sizeof(int));
	
	return(Index);
}

// 获得可用位置的数量
int UnionGetNumOfFreeMsgBufPos(PUnionMsgBufHDL pMsgBufHDL)
{
	struct	msqid_ds pmsgq;
	
	if (pMsgBufHDL == NULL)
		return(-1);
		
	if (msgctl(pMsgBufHDL->QueueIDOfFreePos,IPC_STAT,&pmsgq) != 0)
	{
		UnionSystemErrLog("in UnionGetNumOfFreeMsgBufPos:: msgctl! [%d]\n",pMsgBufHDL->QueueIDOfFreePos);
		return(-1);
	}
	else
		return(pmsgq.msg_qnum);
}

// 获得已占位置的数量
int UnionGetNumOfOccupiedMsgBufPos(PUnionMsgBufHDL pMsgBufHDL)
{
	struct	msqid_ds pmsgq;
	
	if (pMsgBufHDL == NULL)
		return(-1);
		
	if (msgctl(pMsgBufHDL->QueueIDOfOccupiedPos,IPC_STAT,&pmsgq) != 0)
	{
		UnionSystemErrLog("in UnionGetNumOfOccupiedMsgBufPos:: msgctl! [%d]\n",pMsgBufHDL->QueueIDOfOccupiedPos);
		return(-1);
	}
	else
		return(pmsgq.msg_qnum);
}

// 将Index对应的消息位置置为有消息
// TypeOfMsg为消息类型
int UnionOccupyMsgBufPos(PUnionMsgBufHDL pMsgBufHDL,int Index,long TypeOfMsg)
{
	TUnionOccupiedPosOfMsgBuf	PosOfMsgBuf;
	
	if (pMsgBufHDL == NULL)
	{
		UnionUserErrLog("in UnionOccupyMsgBufPos:: pMsgBufHDL is NULL!\n");
		return(-1);
	}
	
	PosOfMsgBuf.TypeOfMsg = TypeOfMsg;
	memcpy(PosOfMsgBuf.IndexOfPos,&Index,sizeof(int));
	
	if (msgsnd(pMsgBufHDL->QueueIDOfOccupiedPos,&PosOfMsgBuf,sizeof(int),~IPC_NOWAIT) == -1)
	{
		UnionSystemErrLog("in UnionOccupyMsgBufPos:: msgsnd!\n");
		return(-1);
	}
	else
		return(0);
}

// 获得第一个有消息的缓冲位置，返回值为该位置
// *TypeOfMsg，返回占据该位置的消息的类型
int UnionGetOccupiedMsgBufPos(PUnionMsgBufHDL pMsgBufHDL,long *TypeOfMsg)
{
	TUnionOccupiedPosOfMsgBuf	PosOfMsgBuf;
	int				Ret;
	int				Index;
	
	if (pMsgBufHDL == NULL)
	{
		UnionUserErrLog("in UnionGetOccupiedMsgBufPos:: pMsgBufHDL is NULL!\n");
		return(-1);
	}
	
	memset(PosOfMsgBuf.IndexOfPos,0,sizeof(PosOfMsgBuf.IndexOfPos));
	
	if ((Ret = msgrcv(pMsgBufHDL->QueueIDOfOccupiedPos,&PosOfMsgBuf,sizeof(int),0,~IPC_NOWAIT)) != sizeof(int))
	{
		UnionSystemErrLog("in UnionGetOccupiedMsgBufPos:: msgrcv! Ret = [%d]\n",Ret);
		return(-1);
	}
	memcpy(&Index,PosOfMsgBuf.IndexOfPos,sizeof(int));
	*TypeOfMsg = PosOfMsgBuf.TypeOfMsg;
		
	return(Index);
}

// 获得第一个有指定消息的缓冲位置，返回值为该位置
// TypeOfMsg，指定的消息类型
int UnionGetOccupiedMsgBufPosOfMsgType(PUnionMsgBufHDL pMsgBufHDL,long TypeOfMsg)
{
	TUnionOccupiedPosOfMsgBuf	PosOfMsgBuf;
	int				Ret;
	int				Index;
	
	if (pMsgBufHDL == NULL)
	{
		UnionUserErrLog("in UnionGetOccupiedMsgBufPosOfMsgType:: pMsgBufHDL is NULL!\n");
		return(-1);
	}
	
	memset(PosOfMsgBuf.IndexOfPos,0,sizeof(PosOfMsgBuf.IndexOfPos));
	
	if ((Ret = msgrcv(pMsgBufHDL->QueueIDOfOccupiedPos,&PosOfMsgBuf,sizeof(int),TypeOfMsg,~IPC_NOWAIT)) != sizeof(int))
	{
		UnionSystemErrLog("in UnionGetOccupiedMsgBufPosOfMsgType:: msgrcv! Ret = [%d]\n",Ret);
		return(-1);
	}
	memcpy(&Index,PosOfMsgBuf.IndexOfPos,sizeof(int));
		
	return(Index);
}

// Msg为要缓冲的消息
// LenOfMsg，是要缓冲的消息的长度
// TypeOfMsg，是消息类型
int UnionBufferMsg(PUnionMsgBufHDL pMsgBufHDL,unsigned char * Msg,int LenOfMsg,long TypeOfMsg)
{
	int		IndexOfMsg;
	unsigned char *	pAddr;
	int		Ret;
	unsigned char * pFirstMsgAddr;
	
	
	if (Msg == NULL)
	{
		UnionUserErrLog("in UnionBufferMsg:: Msg is NULL!\n");
		return(-1);
	}
	
	if ((LenOfMsg <= 0) || (LenOfMsg > pMsgBufHDL->LargestSizeOfMsg))
	{
		UnionUserErrLog("in UnionBufferMsg:: LenOfMsg [%d] Error!\n",LenOfMsg);
		return(-1);
	}
	
	if ((IndexOfMsg = UnionGetAvailableMsgBufPos(pMsgBufHDL)) < 0)
	{
		UnionUserErrLog("in UnionBufferMsg:: UnionGetAvailableMsgBufPos!\n");
		return(IndexOfMsg);
	}
	
	pFirstMsgAddr = UnionGetFirstMsgAddrOfMsgBufMDL(pMsgBufHDL);
	pAddr = pFirstMsgAddr + IndexOfMsg * (pMsgBufHDL->LargestSizeOfMsg + sizeof(int));
	if (pAddr + LenOfMsg + sizeof(int) > pFirstMsgAddr + pMsgBufHDL->SizeOfBuf * 1000)
	{
		UnionUserErrLog("in UnionBufferMsg:: Out of memory range\n!");
		UnionNullLog("Index = [%d]\n",IndexOfMsg);
		UnionNullLog("LenOfMsg = [%d]\n",LenOfMsg);
		UnionNullLog("SizeOfBuf = [%d]\n",pMsgBufHDL->SizeOfBuf);
		UnionNullLog("LargestSizeOfMsg = [%d]\n",pMsgBufHDL->LargestSizeOfMsg);
		UnionNullLog("pAddr = [%0x]\n",pAddr);
		UnionNullLog("pFirstMsgAddr = [%0x]\n",pFirstMsgAddr);
		UnionFreeMsgBufPos(pMsgBufHDL,IndexOfMsg);
		return(-1);
	}
	
	memcpy(pAddr,&LenOfMsg,sizeof(int));
	memcpy(pAddr + sizeof(int),Msg,LenOfMsg);

	if ((Ret = UnionOccupyMsgBufPos(pMsgBufHDL,IndexOfMsg,TypeOfMsg)) < 0)
	{
		UnionUserErrLog("in UnionBufferMsg:: UnionOccupyMsgBufPos IndexOfMs = [%d]\n",IndexOfMsg);
	}
	
	return(0);
}

// 读取第一条消息
// LenOfMsgBuf，是Msg缓冲的大小
// TypeOfMsg，是第一条消息的类型
// 返回值为消息的长度
int UnionReadFirstMsg(PUnionMsgBufHDL pMsgBufHDL,unsigned char * Msg,int LenOfMsgBuf,long *TypeOfMsg)
{
	int		IndexOfMsg;
	unsigned char *	pAddr;
	int		LenOfMsg;
	unsigned char * pFirstMsgAddr;
	
	if (Msg == NULL)
	{
		UnionUserErrLog("in UnionReadFirstMsg:: Msg is NULL!\n");
		return(-1);
	}
	
	if ((IndexOfMsg = UnionGetOccupiedMsgBufPos(pMsgBufHDL,TypeOfMsg)) < 0)
	{
		UnionUserErrLog("in UnionReadFirstMsg:: UnionGetOccupiedMsgBufPos!\n");
		return(IndexOfMsg);
	}
	
	pFirstMsgAddr = UnionGetFirstMsgAddrOfMsgBufMDL(pMsgBufHDL);
	pAddr = pFirstMsgAddr + IndexOfMsg * (pMsgBufHDL->LargestSizeOfMsg + sizeof(int));
	memcpy(&LenOfMsg,pAddr,sizeof(int));
	
	if (pAddr + LenOfMsg + sizeof(int) > pFirstMsgAddr + pMsgBufHDL->SizeOfBuf * 1000)
	{
		UnionUserErrLog("in UnionReadFirstMsg:: Out of memory range Index = [%d] LenOfMsg = [%d]\n",IndexOfMsg,LenOfMsg);
		UnionFreeMsgBufPos(pMsgBufHDL,IndexOfMsg);
		return(-1);
	}
	
	if (LenOfMsgBuf < LenOfMsg)
	{
		UnionUserErrLog("in UnionReadFirstMsg:: Buffer [%d] too short to Receive Msg of Length [%d]!\n",
				LenOfMsgBuf,LenOfMsg);
		UnionFreeMsgBufPos(pMsgBufHDL,IndexOfMsg);
		return(-1);
	}
	
	memcpy(Msg,pAddr + sizeof(int),LenOfMsg);

	if (UnionFreeMsgBufPos(pMsgBufHDL,IndexOfMsg) < 0)
	{
		UnionUserErrLog("in UnionReadFirstMsg:: UnionFreeMsgBufPos!\n");
	}


	return(LenOfMsg);
}

// 读取第一条消息
// LenOfMsgBuf，是Msg缓冲的大小
// TypeOfMsg，是指定消息的类型
// 返回值为消息的长度
int UnionReadSpecifiedMsg(PUnionMsgBufHDL pMsgBufHDL,unsigned char * Msg,int LenOfMsgBuf,long TypeOfMsg)
{
	int		IndexOfMsg;
	unsigned char *	pAddr;
	int		LenOfMsg;
	unsigned char	*pFirstMsgAddr;
	
	if (Msg == NULL)
	{
		UnionUserErrLog("in UnionReadSpecifiedMsg:: Msg is NULL!\n");
		return(-1);
	}
	
	if ((IndexOfMsg = UnionGetOccupiedMsgBufPosOfMsgType(pMsgBufHDL,TypeOfMsg)) < 0)
	{
		UnionUserErrLog("in UnionReadSpecifiedMsg:: UnionGetOccupiedMsgBufPos!\n");
		return(IndexOfMsg);
	}
	
	pFirstMsgAddr = UnionGetFirstMsgAddrOfMsgBufMDL(pMsgBufHDL);
	pAddr = pFirstMsgAddr + IndexOfMsg * (pMsgBufHDL->LargestSizeOfMsg + sizeof(int));
	memcpy(&LenOfMsg,pAddr,sizeof(int));
	
	if (pAddr + LenOfMsg + sizeof(int) > pFirstMsgAddr + pMsgBufHDL->SizeOfBuf * 1000)
	{
		UnionUserErrLog("in UnionReadSpecifiedMsg:: Out of memory range Index = [%d] LenOfMsg = [%d]\n",IndexOfMsg,LenOfMsg);
		UnionFreeMsgBufPos(pMsgBufHDL,IndexOfMsg);
		return(-1);
	}
	
	if (LenOfMsgBuf < LenOfMsg)
	{
		UnionUserErrLog("in UnionReadSpecifiedMsg:: Buffer [%d] too short to Receive Msg of Length [%d]!\n",
				LenOfMsgBuf,LenOfMsg);
		UnionFreeMsgBufPos(pMsgBufHDL,IndexOfMsg);
		return(-1);
	}
	
	memcpy(Msg,pAddr + sizeof(int),LenOfMsg);

	if (UnionFreeMsgBufPos(pMsgBufHDL,IndexOfMsg) < 0)
	{
		UnionUserErrLog("in UnionReadSpecifiedMsg:: UnionFreeMsgBufPos!\n");
	}

	return(LenOfMsg);
}


// 获得第一个有指定消息的缓冲位置，返回值为该位置
// TypeOfMsg，指定的消息类型
int UnionGetOccupiedMsgBufPosOfMsgTypeNoWait(PUnionMsgBufHDL pMsgBufHDL,long TypeOfMsg)
{
	TUnionOccupiedPosOfMsgBuf	PosOfMsgBuf;
	int				Ret;
	int				Index;
	
	if (pMsgBufHDL == NULL)
	{
		UnionUserErrLog("in UnionGetOccupiedMsgBufPosOfMsgTypeNoWait:: pMsgBufHDL is NULL!\n");
		return(-1);
	}
	
	memset(PosOfMsgBuf.IndexOfPos,0,sizeof(PosOfMsgBuf.IndexOfPos));
	
	if ((Ret = msgrcv(pMsgBufHDL->QueueIDOfOccupiedPos,&PosOfMsgBuf,sizeof(int),TypeOfMsg,IPC_NOWAIT)) != sizeof(int))
	{
		//UnionSystemErrLog("in UnionGetOccupiedMsgBufPosOfMsgTypeNoWait:: msgrcv! Ret = [%d]\n",Ret);
		return(-1);
	}
	memcpy(&Index,PosOfMsgBuf.IndexOfPos,sizeof(int));
		
	return(Index);
}

// 删除指定的响应消息
// LenOfMsgBuf，是Msg缓冲的大小
// TypeOfMsg，是指定消息的类型
// 返回值为消息的长度
int UnionDeleteSpecifiedMsg(PUnionMsgBufHDL pMsgBufHDL,long TypeOfMsg)
{
	int		IndexOfMsg;
	int		DeletedNum = 0;
		
	for (;;)
	{
		if ((IndexOfMsg = UnionGetOccupiedMsgBufPosOfMsgTypeNoWait(pMsgBufHDL,TypeOfMsg)) < 0)
		{
			//UnionUserErrLog("in UnionDeleteSpecifiedMsg:: UnionGetOccupiedMsgBufPos!\n");
			return(DeletedNum);
		}
		DeletedNum++;
		if (UnionFreeMsgBufPos(pMsgBufHDL,IndexOfMsg) < 0)
		{
			UnionUserErrLog("in UnionDeleteSpecifiedMsg:: UnionFreeMsgBufPos!\n");
		}
	}
}

