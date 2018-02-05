//	Author:	Wolfgang Wang
//	Date:	2001/10/22

//	2002/3/7 Wolfgang Wang, rename all the functions of the name XXXXMdl to
//	new name XXXMDL.
//	2002/3/7 Wolfgang Wang, change the parameter of the function 
//		UnionConnectMsgBufMDL, which is formerly named UnionConnectMsgBufMdl.

#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "UnionMsgBuf0.h"
#include "UnionLog.h"

PUnionMsgBufHDL	pgMsgBufHDL = NULL;
unsigned char *	pgFirstMsgAddr;

// UserID是用户分配的消息缓冲标识号
// SizeOfBuf是消息缓冲的总长度，以KByte计长度
// LargestSizeOfBuf，是一条消息的最大长度
int UnionConnectMsgBufMDL(int UserID,int SizeOfBuf,int LargestSizeOfMsg)
{
	int	ResID;
	int	Index;
	int	Created = 0;
		
	//	检查参数
	if (UserID <= 0)
	{
		UnionUserErrLog("in UnionConnectMsgBufMDL:: UserID [%d] Error!\n",UserID);
		return(-1);
	}
	if ((SizeOfBuf <= 0) || ((SizeOfBuf > gMaxSizeOfMsgBuf) && (SizeOfBuf / 1000) > gMaxSizeOfMsgBuf))
	{
		UnionUserErrLog("in UnionConnectMsgBufMDL:: SizeOfBuf [%d] Error!\n",SizeOfBuf);
		return(-1);
	}
	if ((LargestSizeOfMsg <= 0) || (LargestSizeOfMsg > SizeOfBuf * 1000) || (LargestSizeOfMsg > gMaxLargestSizeOfMsg))
	{
		UnionUserErrLog("in UnionConnectMsgBufMDL:: LargestSizeOfMsg [%d] Error!\n",SizeOfBuf);
		return(-1);
	}

	if (SizeOfBuf > gMaxSizeOfMsgBuf)
		SizeOfBuf = SizeOfBuf / 1000;
	
	//	MsgBuf已初始化
	if (pgMsgBufHDL != NULL)
	{
		if ((pgMsgBufHDL->UserID != UserID) || (pgMsgBufHDL->SizeOfBuf != SizeOfBuf) || (pgMsgBufHDL->LargestSizeOfMsg != LargestSizeOfMsg))
		{
			UnionUserErrLog("in UnionConnectMsgBufMDL:: Another MsgBuf Already Exists!\n");
			UnionNullLog(" [UserID]			[%d]\n",pgMsgBufHDL->UserID);
			UnionNullLog(" [SizeOfBuf]		[%d]\n",pgMsgBufHDL->SizeOfBuf);
			UnionNullLog(" [LargestSizeOfMsg]	[%d]\n",pgMsgBufHDL->LargestSizeOfMsg);
			return(-1);
		}
		else
			return(0);
	}
	
	//	初始化MsgBuf
	if ((ResID = shmget(UserID,sizeof(TUnionMsgBufHDL) + SizeOfBuf * 1000,0666)) == -1)
	{
		if ((ResID = shmget(UserID,sizeof(TUnionMsgBufHDL) + SizeOfBuf * 1000,0666|IPC_CREAT)) == -1)
		{
			UnionSystemErrLog("in UnionConnectMsgBufMDL:: shmget!\n");
			return(-1);
		}
		Created = 1;
	}
			
	if ((pgMsgBufHDL = (PUnionMsgBufHDL)shmat(ResID,0,SHM_RND)) == NULL)
	{
		UnionSystemErrLog("in UnionConnectMsgBufMDL:: shmat!\n");
		return(-1);
	}
	
	pgFirstMsgAddr = (unsigned char *)(pgMsgBufHDL + sizeof(TUnionMsgBufHDL));
	pgMsgBufHDL->ShareMemoryID = ResID;
	
	if ((pgMsgBufHDL->QueueIDOfFreePos = msgget(UserID,0666 | IPC_CREAT)) == -1)
	{
		UnionSystemErrLog("in UnionConnectMsgBufMDL:: msgget [%d]!\n",UserID);
		shmdt(pgMsgBufHDL);
		pgMsgBufHDL = NULL;
		return(-1);
	}
			
	if ((pgMsgBufHDL->QueueIDOfOccupiedReqPos = msgget(UserID + 1,0666 | IPC_CREAT)) == -1)
	{
		UnionSystemErrLog("in UnionConnectMsgBufMDL:: msgget [%d]!\n",UserID+1);
		shmdt(pgMsgBufHDL);
		pgMsgBufHDL = NULL;
		return(-1);
	}
			
	if ((pgMsgBufHDL->QueueIDOfOccupiedResPos = msgget(UserID + 2,0666 | IPC_CREAT)) == -1)
	{
		UnionSystemErrLog("in UnionConnectMsgBufMDL:: msgget [%d]!\n",UserID+2);
		shmdt(pgMsgBufHDL);
		pgMsgBufHDL = NULL;
		return(-1);
	}
			
	pgMsgBufHDL->NumOfMsg = SizeOfBuf * 1000 / (LargestSizeOfMsg + sizeof(int));
	pgMsgBufHDL->UserID = UserID;
	pgMsgBufHDL->SizeOfBuf = SizeOfBuf;
	pgMsgBufHDL->LargestSizeOfMsg = LargestSizeOfMsg;
	
	if (!Created && (UnionGetNumOfFreeMsgBufPos() + UnionGetNumOfOccupiedMsgBufPos() != 0))
		return(0);
		
	for (Index = 0; Index < pgMsgBufHDL->NumOfMsg; Index++)
	{
		if (UnionFreeMsgBufPos(Index) < 0)
		{
			UnionUserErrLog("in UnionConnectMsgBufMDL:: UnionFreeMsgBufPos!\n");
			shmdt(pgMsgBufHDL);
			pgMsgBufHDL = NULL;
			return(-1);
		}
	}
	return(0);
}

int UnionDisconnectMsgBufMDL()
{
	int	Ret;
	
	
	if (pgMsgBufHDL != NULL)
	{
		if ((Ret = shmdt(pgMsgBufHDL)) < 0)
		{
			UnionSystemErrLog("in UnionDisconnectMsgBufMDL:: shmdt!\n");
			return(-1);
		}
	}
	pgFirstMsgAddr = NULL;
	pgMsgBufHDL = NULL;
	
	return(0);
}

// 调用该函数之前，一定要先调用UnionConnectMsgBufMDL
int UnionRemoveMsgBufMDL(int UserID,int SizeOfMsgBuf)
{
	struct shmid_ds		buf;
	int 			ResID;
	char			Command[80];
	int			Ret = 0;

	pgMsgBufHDL = NULL;
	
	if ((ResID = msgget(UserID,0666)) >= 0)
	{
		memset(Command,0,sizeof(Command));
		sprintf(Command,"ipcrm -q %d",ResID);
		if (system(Command) < 0)
		{
			UnionUserErrLog("in UnionReleaseMsgBufMDL:: [%s]\n",Command);
			Ret--;
		}
	}

	if ((ResID = msgget(UserID+1,0666)) >= 0)
	{
		memset(Command,0,sizeof(Command));
		sprintf(Command,"ipcrm -q %d",ResID);
		if (system(Command) < 0)
		{
			UnionUserErrLog("in UnionReleaseMsgBufMDL:: [%s]\n",Command);
			Ret--;
		}
	}

	if ((ResID = msgget(UserID+2,0666)) >= 0)
	{
		memset(Command,0,sizeof(Command));
		sprintf(Command,"ipcrm -q %d",ResID);
		if (system(Command) < 0)
		{
			UnionUserErrLog("in UnionReleaseMsgBufMDL:: [%s]\n",Command);
			Ret--;
		}
	}

	if ((ResID = shmget(UserID,SizeOfMsgBuf * 1000,0666)) == -1)
	{
		UnionSystemErrLog("in UnionRemoveMsgBufMDL:: shmget [%d]\n",UserID);
		Ret--;
	}
	
	if (shmctl(ResID,IPC_RMID,&buf) != 0)
	{
		UnionSystemErrLog("in UnionRemoveMsgBufMDL:: shmctl IPC_RMID ResID = [%d]\n",UserID);
		Ret--;
	}

	return(Ret);
}	

// 将Index对应的消息位置置为可用
int UnionFreeMsgBufPos(int Index)
{
	TUnionFreePosOfMsgBuf	PosOfMsgBuf;
	
	if (pgMsgBufHDL == NULL)
	{
		UnionUserErrLog("in UnionFreeMsgBufPos:: pgMsgBufHDL is NULL!\n");
		return(-1);
	}
	
	PosOfMsgBuf.StatusOfPos = gAvailableMsgBufPos;
	memcpy(PosOfMsgBuf.IndexOfPos,&Index,sizeof(int));
	
	if (msgsnd(pgMsgBufHDL->QueueIDOfFreePos,&PosOfMsgBuf,sizeof(int),~IPC_NOWAIT) == -1)
	{
		UnionSystemErrLog("in UnionFreeMsgBufPos:: msgsnd!\n");
		return(-1);
	}
	else
		return(0);
}

// 获得可用的消息位置，返回值为所获位置
int UnionGetAvailableMsgBufPos()
{
	TUnionFreePosOfMsgBuf	PosOfMsgBuf;
	int			Ret;
	int			Index;
	
	if (pgMsgBufHDL == NULL)
	{
		UnionUserErrLog("in UnionGetAvailableMsgBufPos:: pgMsgBufHDL is NULL!\n");
		return(-1);
	}
	
	memset(PosOfMsgBuf.IndexOfPos,0,sizeof(PosOfMsgBuf.IndexOfPos));
	
	if ((Ret = msgrcv(pgMsgBufHDL->QueueIDOfFreePos,&PosOfMsgBuf,sizeof(PosOfMsgBuf),0,~IPC_NOWAIT)) != sizeof(int))
	{
		UnionSystemErrLog("in UnionGetAvailableMsgBufPos:: msgrcv! Ret = [%d]\n",Ret);
		return(-1);
	}
	memcpy(&Index,PosOfMsgBuf.IndexOfPos,sizeof(int));
	
	return(Index);
}

int UnionPrintStatusOfMsgBufMDL()
{
	if (pgMsgBufHDL == NULL)
		return(0);
	
	printf("Status of Msg Buf:\n");
	printf("[UserID]			[%d]\n",pgMsgBufHDL->UserID);
	printf("[SizeOfBuf]			[%d]\n",pgMsgBufHDL->SizeOfBuf);
	printf("[LargestSizeOfMsg]		[%d]\n",pgMsgBufHDL->LargestSizeOfMsg);
	printf("[NumOfMsgPos]			[%d]\n",pgMsgBufHDL->NumOfMsg);
	printf("[QueueIDOfFreePos]		[%d]\n",pgMsgBufHDL->QueueIDOfFreePos);
	printf("[QueueIDOfOccupiedReqPos]	[%d]\n",pgMsgBufHDL->QueueIDOfOccupiedReqPos);
	printf("[QueueIDOfOccupiedResPos]	[%d]\n",pgMsgBufHDL->QueueIDOfOccupiedResPos);
	printf("[ShareMemoryID]			[%d]\n",pgMsgBufHDL->ShareMemoryID);
	
	return(0);
}

int UnionGetNumOfFreeMsgBufPos()
{
	struct	msqid_ds pmsgq;
	
	if (pgMsgBufHDL == NULL)
		return(-1);
		
	if (msgctl(pgMsgBufHDL->QueueIDOfFreePos,IPC_STAT,&pmsgq) != 0)
	{
		UnionSystemErrLog("in UnionGetNumOfFreeMsgBufPos:: msgctl! [%d]\n",pgMsgBufHDL->QueueIDOfFreePos);
		return(-1);
	}
	else
		return(pmsgq.msg_qnum);
}

int UnionGetNumOfOccupiedMsgBufPos()
{
	struct	msqid_ds pmsgq;
	
	if (pgMsgBufHDL == NULL)
		return(-1);
		
	if (msgctl(pgMsgBufHDL->QueueIDOfOccupiedReqPos,IPC_STAT,&pmsgq) != 0)
	{
		UnionSystemErrLog("in UnionGetNumOfOccupiedMsgBufPos:: msgctl! [%d]\n",pgMsgBufHDL->QueueIDOfOccupiedReqPos);
		return(-1);
	}
	else
		return(pmsgq.msg_qnum);
}

// 将Index对应的消息位置置为有消息
// TypeOfMsg为消息类型
int UnionOccupyMsgBufPosForReqForReq(int Index,long TypeOfMsg)
{
	TUnionOccupiedPosOfMsgBuf	PosOfMsgBuf;
	
	if (pgMsgBufHDL == NULL)
	{
		UnionUserErrLog("in UnionOccupyMsgBufPosForReq:: pgMsgBufHDL is NULL!\n");
		return(-1);
	}
	
	PosOfMsgBuf.TypeOfMsg = TypeOfMsg;
	memcpy(PosOfMsgBuf.IndexOfPos,&Index,sizeof(int));
	
	if (msgsnd(pgMsgBufHDL->QueueIDOfOccupiedReqPos,&PosOfMsgBuf,sizeof(int),~IPC_NOWAIT) == -1)
	{
		UnionSystemErrLog("in UnionOccupyMsgBufPosForReq:: msgsnd!\n");
		return(-1);
	}
	else
		return(0);
}

// 获得第一个有消息的缓冲位置，返回值为该位置
// *TypeOfMsg，返回占据该位置的消息的类型
int UnionGetOccupiedMsgBufPosForReq(long *TypeOfMsg)
{
	TUnionOccupiedPosOfMsgBuf	PosOfMsgBuf;
	int				Ret;
	int				Index;
	
	if (pgMsgBufHDL == NULL)
	{
		UnionUserErrLog("in UnionGetOccupiedMsgBufPos:: pgMsgBufHDL is NULL!\n");
		return(-1);
	}
	
	memset(PosOfMsgBuf.IndexOfPos,0,sizeof(PosOfMsgBuf.IndexOfPos));
	
	if ((Ret = msgrcv(pgMsgBufHDL->QueueIDOfOccupiedReqPos,&PosOfMsgBuf,sizeof(PosOfMsgBuf),0,~IPC_NOWAIT)) != sizeof(int))
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
int UnionGetOccupiedMsgBufPosOfMsgTypeForReq(long TypeOfMsg)
{
	TUnionOccupiedPosOfMsgBuf	PosOfMsgBuf;
	int				Ret;
	int				Index;
	
	if (pgMsgBufHDL == NULL)
	{
		UnionUserErrLog("in UnionGetOccupiedMsgBufPosOfMsgTypeForReq:: pgMsgBufHDL is NULL!\n");
		return(-1);
	}
	
	memset(PosOfMsgBuf.IndexOfPos,0,sizeof(PosOfMsgBuf.IndexOfPos));
	
	if ((Ret = msgrcv(pgMsgBufHDL->QueueIDOfOccupiedReqPos,&PosOfMsgBuf,sizeof(PosOfMsgBuf),TypeOfMsg,~IPC_NOWAIT)) != sizeof(int))
	{
		UnionSystemErrLog("in UnionGetOccupiedMsgBufPosOfMsgTypeForReq:: msgrcv! Ret = [%d]\n",Ret);
		return(-1);
	}
	memcpy(&Index,PosOfMsgBuf.IndexOfPos,sizeof(int));
		
	return(Index);
}

// Msg为要缓冲的消息
// LenOfMsg，是要缓冲的消息的长度
// TypeOfMsg，是消息类型
int UnionBufferReqMsg(unsigned char * Msg,int LenOfMsg,long TypeOfMsg)
{
	int		IndexOfMsg;
	unsigned char *	pAddr;
	int		Ret;
	
	if (Msg == NULL)
	{
		UnionUserErrLog("in UnionBufferReqMsg:: Msg is NULL!\n");
		return(-1);
	}
	
	if ((LenOfMsg <= 0) || (LenOfMsg > pgMsgBufHDL->LargestSizeOfMsg))
	{
		UnionUserErrLog("in UnionBufferReqMsg:: LenOfMsg [%d] Error!\n",LenOfMsg);
		return(-1);
	}
	
	if ((IndexOfMsg = UnionGetAvailableMsgBufPos()) < 0)
	{
		UnionUserErrLog("in UnionBufferReqMsg:: UnionGetAvailableMsgBufPos!\n");
		return(IndexOfMsg);
	}
	
	pAddr = pgFirstMsgAddr + IndexOfMsg * (pgMsgBufHDL->LargestSizeOfMsg + sizeof(int));
	if (pAddr + LenOfMsg + sizeof(int) > pgFirstMsgAddr + pgMsgBufHDL->SizeOfBuf * 1000)
	{
		UnionUserErrLog("in UnionBufferReqMsg:: Out of memory range\n!");
		UnionNullLog("Index = [%d]\n",IndexOfMsg);
		UnionNullLog("LenOfMsg = [%d]\n",LenOfMsg);
		UnionNullLog("SizeOfBuf = [%d]\n",pgMsgBufHDL->SizeOfBuf);
		UnionNullLog("LargestSizeOfMsg = [%d]\n",pgMsgBufHDL->LargestSizeOfMsg);
		UnionNullLog("pAddr = [%0x]\n",pAddr);
		UnionNullLog("pgFirstMsgAddr = [%0x]\n",pgFirstMsgAddr);
		UnionFreeMsgBufPos(IndexOfMsg);
		return(-1);
	}
	
	memcpy(pAddr,&LenOfMsg,sizeof(int));
	memcpy(pAddr + sizeof(int),Msg,LenOfMsg);

	if ((Ret = UnionOccupyMsgBufPosForReqForReq(IndexOfMsg,TypeOfMsg)) < 0)
	{
		UnionUserErrLog("in UnionBufferReqMsg:: UnionOccupyMsgBufPosForReq IndexOfMs = [%d]\n",IndexOfMsg);
	}
	
	return(0);
}

// 读取第一条消息
// LenOfMsgBuf，是Msg缓冲的大小
// TypeOfMsg，是第一条消息的类型
// 返回值为消息的长度
int UnionReadFirstReqMsg(unsigned char * Msg,int LenOfMsgBuf,long *TypeOfMsg)
{
	int		IndexOfMsg;
	unsigned char *	pAddr;
	int		LenOfMsg;
	
	if (Msg == NULL)
	{
		UnionUserErrLog("in UnionReadFirstReqMsg:: Msg is NULL!\n");
		return(-1);
	}
	
	if ((IndexOfMsg = UnionGetOccupiedMsgBufPosForReq(TypeOfMsg)) < 0)
	{
		UnionUserErrLog("in UnionReadFirstReqMsg:: UnionGetOccupiedMsgBufPos!\n");
		return(IndexOfMsg);
	}
	
	pAddr = pgFirstMsgAddr + IndexOfMsg * (pgMsgBufHDL->LargestSizeOfMsg + sizeof(int));
	memcpy(&LenOfMsg,pAddr,sizeof(int));
	
	if (pAddr + LenOfMsg + sizeof(int) > pgFirstMsgAddr + pgMsgBufHDL->SizeOfBuf * 1000)
	{
		UnionUserErrLog("in UnionReadFirstReqMsg:: Out of memory range Index = [%d] LenOfMsg = [%d]\n",IndexOfMsg,LenOfMsg);
		UnionFreeMsgBufPos(IndexOfMsg);
		return(-1);
	}
	
	if (LenOfMsgBuf < LenOfMsg)
	{
		UnionUserErrLog("in UnionReadFirstReqMsg:: Buffer [%d] too short to Receive Msg of Length [%d]!\n",
				LenOfMsgBuf,LenOfMsg);
		UnionFreeMsgBufPos(IndexOfMsg);
		return(-1);
	}
	
	memcpy(Msg,pAddr + sizeof(int),LenOfMsg);

	if (UnionFreeMsgBufPos(IndexOfMsg) < 0)
	{
		UnionUserErrLog("in UnionReadFirstReqMsg:: UnionFreeMsgBufPos!\n");
	}


	return(LenOfMsg);
}

// 读取第一条消息
// LenOfMsgBuf，是Msg缓冲的大小
// TypeOfMsg，是指定消息的类型
// 返回值为消息的长度
int UnionReadSpecifiedReqMsg(unsigned char * Msg,int LenOfMsgBuf,long TypeOfMsg)
{
	int		IndexOfMsg;
	unsigned char *	pAddr;
	int		LenOfMsg;
	
	if (Msg == NULL)
	{
		UnionUserErrLog("in UnionReadSpecifiedReqMsg:: Msg is NULL!\n");
		return(-1);
	}
	
	if ((IndexOfMsg = UnionGetOccupiedMsgBufPosOfMsgTypeForReq(TypeOfMsg)) < 0)
	{
		UnionUserErrLog("in UnionReadSpecifiedReqMsg:: UnionGetOccupiedMsgBufPos!\n");
		return(IndexOfMsg);
	}
	
	pAddr = pgFirstMsgAddr + IndexOfMsg * (pgMsgBufHDL->LargestSizeOfMsg + sizeof(int));
	memcpy(&LenOfMsg,pAddr,sizeof(int));
	
	if (pAddr + LenOfMsg + sizeof(int) > pgFirstMsgAddr + pgMsgBufHDL->SizeOfBuf * 1000)
	{
		UnionUserErrLog("in UnionReadSpecifiedReqMsg:: Out of memory range Index = [%d] LenOfMsg = [%d]\n",IndexOfMsg,LenOfMsg);
		UnionFreeMsgBufPos(IndexOfMsg);
		return(-1);
	}
	
	if (LenOfMsgBuf < LenOfMsg)
	{
		UnionUserErrLog("in UnionReadSpecifiedReqMsg:: Buffer [%d] too short to Receive Msg of Length [%d]!\n",
				LenOfMsgBuf,LenOfMsg);
		UnionFreeMsgBufPos(IndexOfMsg);
		return(-1);
	}
	
	memcpy(Msg,pAddr + sizeof(int),LenOfMsg);

	if (UnionFreeMsgBufPos(IndexOfMsg) < 0)
	{
		UnionUserErrLog("in UnionReadSpecifiedReqMsg:: UnionFreeMsgBufPos!\n");
	}

	return(LenOfMsg);
}

// 将Index对应的消息位置置为有消息
// TypeOfMsg为消息类型
int UnionOccupyMsgBufPosForRes(int Index,long TypeOfMsg)
{
	TUnionOccupiedPosOfMsgBuf	PosOfMsgBuf;
	
	if (pgMsgBufHDL == NULL)
	{
		UnionUserErrLog("in UnionOccupyMsgBufPosForRes:: pgMsgBufHDL is NULL!\n");
		return(-1);
	}
	
	PosOfMsgBuf.TypeOfMsg = TypeOfMsg;
	memcpy(PosOfMsgBuf.IndexOfPos,&Index,sizeof(int));
	
	if (msgsnd(pgMsgBufHDL->QueueIDOfOccupiedResPos,&PosOfMsgBuf,sizeof(int),~IPC_NOWAIT) == -1)
	{
		UnionSystemErrLog("in UnionOccupyMsgBufPosForRes:: msgsnd!\n");
		return(-1);
	}
	else
		return(0);
}

// 获得第一个有消息的缓冲位置，返回值为该位置
// *TypeOfMsg，返回占据该位置的消息的类型
int UnionGetOccupiedMsgBufPosForRes(long *TypeOfMsg)
{
	TUnionOccupiedPosOfMsgBuf	PosOfMsgBuf;
	int				Ret;
	int				Index;
	
	if (pgMsgBufHDL == NULL)
	{
		UnionUserErrLog("in UnionGetOccupiedMsgBufPos:: pgMsgBufHDL is NULL!\n");
		return(-1);
	}
	
	memset(PosOfMsgBuf.IndexOfPos,0,sizeof(PosOfMsgBuf.IndexOfPos));
	
	if ((Ret = msgrcv(pgMsgBufHDL->QueueIDOfOccupiedResPos,&PosOfMsgBuf,sizeof(PosOfMsgBuf),0,~IPC_NOWAIT)) != sizeof(int))
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
int UnionGetOccupiedMsgBufPosOfMsgTypeForRes(long TypeOfMsg)
{
	TUnionOccupiedPosOfMsgBuf	PosOfMsgBuf;
	int				Ret;
	int				Index;
	
	if (pgMsgBufHDL == NULL)
	{
		UnionUserErrLog("in UnionGetOccupiedMsgBufPosOfMsgTypeForRes:: pgMsgBufHDL is NULL!\n");
		return(-1);
	}
	
	memset(PosOfMsgBuf.IndexOfPos,0,sizeof(PosOfMsgBuf.IndexOfPos));
	
	if ((Ret = msgrcv(pgMsgBufHDL->QueueIDOfOccupiedResPos,&PosOfMsgBuf,sizeof(PosOfMsgBuf),TypeOfMsg,~IPC_NOWAIT)) != sizeof(int))
	{
		UnionSystemErrLog("in UnionGetOccupiedMsgBufPosOfMsgTypeForRes:: msgrcv! Ret = [%d]\n",Ret);
		return(-1);
	}
	memcpy(&Index,PosOfMsgBuf.IndexOfPos,sizeof(int));
		
	return(Index);
}

// Msg为要缓冲的消息
// LenOfMsg，是要缓冲的消息的长度
// TypeOfMsg，是消息类型
int UnionBufferResMsg(unsigned char * Msg,int LenOfMsg,long TypeOfMsg)
{
	int		IndexOfMsg;
	unsigned char *	pAddr;
	int		Ret;
	
	if (Msg == NULL)
	{
		UnionUserErrLog("in UnionBufferResMsg:: Msg is NULL!\n");
		return(-1);
	}
	
	if ((LenOfMsg <= 0) || (LenOfMsg > pgMsgBufHDL->LargestSizeOfMsg))
	{
		UnionUserErrLog("in UnionBufferResMsg:: LenOfMsg [%d] Error!\n",LenOfMsg);
		return(-1);
	}
	
	if ((IndexOfMsg = UnionGetAvailableMsgBufPos()) < 0)
	{
		UnionUserErrLog("in UnionBufferResMsg:: UnionGetAvailableMsgBufPos!\n");
		return(IndexOfMsg);
	}
	
	pAddr = pgFirstMsgAddr + IndexOfMsg * (pgMsgBufHDL->LargestSizeOfMsg + sizeof(int));
	if (pAddr + LenOfMsg + sizeof(int) > pgFirstMsgAddr + pgMsgBufHDL->SizeOfBuf * 1000)
	{
		UnionUserErrLog("in UnionBufferResMsg:: Out of memory range\n!");
		UnionNullLog("Index = [%d]\n",IndexOfMsg);
		UnionNullLog("LenOfMsg = [%d]\n",LenOfMsg);
		UnionNullLog("SizeOfBuf = [%d]\n",pgMsgBufHDL->SizeOfBuf);
		UnionNullLog("LargestSizeOfMsg = [%d]\n",pgMsgBufHDL->LargestSizeOfMsg);
		UnionNullLog("pAddr = [%0x]\n",pAddr);
		UnionNullLog("pgFirstMsgAddr = [%0x]\n",pgFirstMsgAddr);
		UnionFreeMsgBufPos(IndexOfMsg);
		return(-1);
	}
	
	memcpy(pAddr,&LenOfMsg,sizeof(int));
	memcpy(pAddr + sizeof(int),Msg,LenOfMsg);

	if ((Ret = UnionOccupyMsgBufPosForRes(IndexOfMsg,TypeOfMsg)) < 0)
	{
		UnionUserErrLog("in UnionBufferResMsg:: UnionOccupyMsgBufPosForRes IndexOfMs = [%d]\n",IndexOfMsg);
	}
	
	return(0);
}

// 读取第一条消息
// LenOfMsgBuf，是Msg缓冲的大小
// TypeOfMsg，是第一条消息的类型
// 返回值为消息的长度
int UnionReadFirstResMsg(unsigned char * Msg,int LenOfMsgBuf,long *TypeOfMsg)
{
	int		IndexOfMsg;
	unsigned char *	pAddr;
	int		LenOfMsg;
	
	if (Msg == NULL)
	{
		UnionUserErrLog("in UnionReadFirstResMsg:: Msg is NULL!\n");
		return(-1);
	}
	
	if ((IndexOfMsg = UnionGetOccupiedMsgBufPosForRes(TypeOfMsg)) < 0)
	{
		UnionUserErrLog("in UnionReadFirstResMsg:: UnionGetOccupiedMsgBufPos!\n");
		return(IndexOfMsg);
	}
	
	pAddr = pgFirstMsgAddr + IndexOfMsg * (pgMsgBufHDL->LargestSizeOfMsg + sizeof(int));
	memcpy(&LenOfMsg,pAddr,sizeof(int));
	
	if (pAddr + LenOfMsg + sizeof(int) > pgFirstMsgAddr + pgMsgBufHDL->SizeOfBuf * 1000)
	{
		UnionUserErrLog("in UnionReadFirstResMsg:: Out of memory range Index = [%d] LenOfMsg = [%d]\n",IndexOfMsg,LenOfMsg);
		UnionFreeMsgBufPos(IndexOfMsg);
		return(-1);
	}
	
	if (LenOfMsgBuf < LenOfMsg)
	{
		UnionUserErrLog("in UnionReadFirstResMsg:: Buffer [%d] too short to Receive Msg of Length [%d]!\n",
				LenOfMsgBuf,LenOfMsg);
		UnionFreeMsgBufPos(IndexOfMsg);
		return(-1);
	}
	
	memcpy(Msg,pAddr + sizeof(int),LenOfMsg);

	if (UnionFreeMsgBufPos(IndexOfMsg) < 0)
	{
		UnionUserErrLog("in UnionReadFirstResMsg:: UnionFreeMsgBufPos!\n");
	}


	return(LenOfMsg);
}

// 读取第一条消息
// LenOfMsgBuf，是Msg缓冲的大小
// TypeOfMsg，是指定消息的类型
// 返回值为消息的长度
int UnionReadSpecifiedResMsg(unsigned char * Msg,int LenOfMsgBuf,long TypeOfMsg)
{
	int		IndexOfMsg;
	unsigned char *	pAddr;
	int		LenOfMsg;
	
	if (Msg == NULL)
	{
		UnionUserErrLog("in UnionReadSpecifiedResMsg:: Msg is NULL!\n");
		return(-1);
	}
	
	if ((IndexOfMsg = UnionGetOccupiedMsgBufPosOfMsgTypeForRes(TypeOfMsg)) < 0)
	{
		UnionUserErrLog("in UnionReadSpecifiedResMsg:: UnionGetOccupiedMsgBufPos!\n");
		return(IndexOfMsg);
	}
	
	pAddr = pgFirstMsgAddr + IndexOfMsg * (pgMsgBufHDL->LargestSizeOfMsg + sizeof(int));
	memcpy(&LenOfMsg,pAddr,sizeof(int));
	
	if (pAddr + LenOfMsg + sizeof(int) > pgFirstMsgAddr + pgMsgBufHDL->SizeOfBuf * 1000)
	{
		UnionUserErrLog("in UnionReadSpecifiedResMsg:: Out of memory range Index = [%d] LenOfMsg = [%d]\n",IndexOfMsg,LenOfMsg);
		UnionFreeMsgBufPos(IndexOfMsg);
		return(-1);
	}
	
	if (LenOfMsgBuf < LenOfMsg)
	{
		UnionUserErrLog("in UnionReadSpecifiedResMsg:: Buffer [%d] too short to Receive Msg of Length [%d]!\n",
				LenOfMsgBuf,LenOfMsg);
		UnionFreeMsgBufPos(IndexOfMsg);
		return(-1);
	}
	
	memcpy(Msg,pAddr + sizeof(int),LenOfMsg);

	if (UnionFreeMsgBufPos(IndexOfMsg) < 0)
	{
		UnionUserErrLog("in UnionReadSpecifiedResMsg:: UnionFreeMsgBufPos!\n");
	}

	return(LenOfMsg);
}


// 获得第一个有指定消息的缓冲位置，返回值为该位置
// TypeOfMsg，指定的消息类型
int UnionGetOccupiedMsgBufPosOfMsgTypeForResNoWait(long TypeOfMsg)
{
	TUnionOccupiedPosOfMsgBuf	PosOfMsgBuf;
	int				Ret;
	int				Index;
	
	if (pgMsgBufHDL == NULL)
	{
		UnionUserErrLog("in UnionGetOccupiedMsgBufPosOfMsgTypeForResNoWait:: pgMsgBufHDL is NULL!\n");
		return(-1);
	}
	
	memset(PosOfMsgBuf.IndexOfPos,0,sizeof(PosOfMsgBuf.IndexOfPos));
	
	if ((Ret = msgrcv(pgMsgBufHDL->QueueIDOfOccupiedResPos,&PosOfMsgBuf,sizeof(PosOfMsgBuf),TypeOfMsg,IPC_NOWAIT)) != sizeof(int))
	{
		//UnionSystemErrLog("in UnionGetOccupiedMsgBufPosOfMsgTypeForResNoWait:: msgrcv! Ret = [%d]\n",Ret);
		return(-1);
	}
	memcpy(&Index,PosOfMsgBuf.IndexOfPos,sizeof(int));
		
	return(Index);
}

// 删除指定的响应消息
// LenOfMsgBuf，是Msg缓冲的大小
// TypeOfMsg，是指定消息的类型
// 返回值为消息的长度
int UnionDeleteSpecifiedResMsg(long TypeOfMsg)
{
	int		IndexOfMsg;
	
	for (;;)
	{
		if ((IndexOfMsg = UnionGetOccupiedMsgBufPosOfMsgTypeForResNoWait(TypeOfMsg)) < 0)
		{
			//UnionUserErrLog("in UnionDeleteSpecifiedResMsg:: UnionGetOccupiedMsgBufPos!\n");
			return(0);
		}
	
		if (UnionFreeMsgBufPos(IndexOfMsg) < 0)
		{
			UnionUserErrLog("in UnionDeleteSpecifiedResMsg:: UnionFreeMsgBufPos!\n");
		}
	}

}

// 获得第一个有指定消息的缓冲位置，返回值为该位置
// TypeOfMsg，指定的消息类型
int UnionGetOccupiedMsgBufPosOfMsgTypeForReqNoWait(long TypeOfMsg)
{
	TUnionOccupiedPosOfMsgBuf	PosOfMsgBuf;
	int				Ret;
	int				Index;
	
	if (pgMsgBufHDL == NULL)
	{
		UnionUserErrLog("in UnionGetOccupiedMsgBufPosOfMsgTypeForReqNoWait:: pgMsgBufHDL is NULL!\n");
		return(-1);
	}
	
	memset(PosOfMsgBuf.IndexOfPos,0,sizeof(PosOfMsgBuf.IndexOfPos));
	
	if ((Ret = msgrcv(pgMsgBufHDL->QueueIDOfOccupiedReqPos,&PosOfMsgBuf,sizeof(PosOfMsgBuf),TypeOfMsg,IPC_NOWAIT)) != sizeof(int))
	{
		//UnionSystemErrLog("in UnionGetOccupiedMsgBufPosOfMsgTypeForReqNoWait:: msgrcv! Ret = [%d]\n",Ret);
		return(-1);
	}
	memcpy(&Index,PosOfMsgBuf.IndexOfPos,sizeof(int));
		
	return(Index);
}

// 删除指定的请求消息
// LenOfMsgBuf，是Msg缓冲的大小
// TypeOfMsg，是指定消息的类型
// 返回值为消息的长度
int UnionDeleteSpecifiedReqMsg(long TypeOfMsg)
{
	int		IndexOfMsg;
	
	for (;;)
	{
		if ((IndexOfMsg = UnionGetOccupiedMsgBufPosOfMsgTypeForReqNoWait(TypeOfMsg)) < 0)
		{
			//UnionUserErrLog("in UnionDeleteSpecifiedReqMsg:: UnionGetOccupiedMsgBufPos!\n");
			return(0);
		}
	
		if (UnionFreeMsgBufPos(IndexOfMsg) < 0)
		{
			UnionUserErrLog("in UnionDeleteSpecifiedReqMsg:: UnionFreeMsgBufPos!\n");
		}
	}
}

