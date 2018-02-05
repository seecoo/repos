//	Author:	Wolfgang Wang
//	Date:	2006/8/1

#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <time.h>

#include "unionMsgBuf6.x.h"
#include "unionModule.h"
#include "unionErrCode.h"
#include "UnionEnv.h"
#include "UnionLog.h"

#define MSG_QUEUE_PERM 0600

PUnionSharedMemoryModule	pgpdfzMsgBufMDL = NULL;
PUnionMsgBufHDL			pgpdfzMsgBufHDL = NULL;
unsigned char			*pgpdfzMsgBuf = NULL;
int				gpdfzMsgBufMDLConnected = 0;
#define maxSize			8192

// 2007/11/15 增加
unsigned long			gunionMsgIndex = 1;
long UnionApplyUniqueMsgIndex()
{
	return(++gunionMsgIndex);
}
// 2007/11/15 增加结束

// 2013/10/11 增加
PUnionMsgBufHDL UnionGetCurrentMsgBufHDLGrp()
{
	if (UnionConnectMsgBufMDL() < 0)
		return(NULL);
	else
		return(pgpdfzMsgBufHDL);
}
unsigned char *UnionGetCurrentMsgBuf()
{
	if (UnionConnectMsgBufMDL() < 0)
		return(NULL);
	else
		return(pgpdfzMsgBuf);
}
// 2013/10/11 增加结束

long UnionGetMaxStayTimeOfMsg()
{
	int	ret;
	
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionGetMaxNumOfMsg:: UnionConnectMsgBufMDL!\n");
		return(ret);
	}
	return(pgpdfzMsgBufHDL->maxStayTime);
}

int UnionGetMaxNumOfMsg()
{
	int	ret;
	
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionGetMaxNumOfMsg:: UnionConnectMsgBufMDL!\n");
		return(ret);
	}
	return(pgpdfzMsgBufHDL->maxNumOfMsg);
}

int UnionIsMsgBufMDLConnected()
{
	if ((!UnionIsSharedMemoryInited(conMDLNameOfMsgBuf)) || (pgpdfzMsgBufHDL == NULL) || (pgpdfzMsgBuf == NULL))
		return(0);
	else
		return(1);
	//return(gpdfzMsgBufMDLConnected);
}

int UnionReconnectMsgBufMDLAnyway()
{
	PUnionSharedMemoryModule	pmdl;
	int				ret;
	TUnionMsgBufHDL			msgBufHDL;
	int				newCreatedQueue = 0;
	int				index;
	
	memset(&msgBufHDL,0,sizeof(msgBufHDL));
	if ((ret = UnionInitMsgBufHDL(&msgBufHDL)) < 0)
	{
		UnionUserErrLog("in UnionReconnectMsgBufMDLAnyway:: UnionInitMsgBufHDL!\n");
		return(ret);
	}
	
	if ((pgpdfzMsgBufMDL = UnionConnectSharedMemoryModule(conMDLNameOfMsgBuf,
			sizeof(*pgpdfzMsgBufHDL) + 
			((sizeof(TUnionMessageHeader) + sizeof(unsigned char) * msgBufHDL.maxSizeOfMsg) * (msgBufHDL.maxNumOfMsg + 1)))) == NULL)
	{
		UnionUserErrLog("in UnionReconnectMsgBufMDLAnyway:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgpdfzMsgBufHDL = (PUnionMsgBufHDL)UnionGetAddrOfSharedMemoryModuleUserSpace(pgpdfzMsgBufMDL)) == NULL)
	{
		UnionUserErrLog("in UnionReconnectMsgBufMDLAnyway:: PUnionMsgBufHDL!\n");
		return(errCodeSharedMemoryModule);
	}
	pgpdfzMsgBuf = (unsigned char *)pgpdfzMsgBufHDL + sizeof(*pgpdfzMsgBufHDL);
	
	if (UnionIsNewCreatedSharedMemoryModule(pgpdfzMsgBufMDL))
	{
		memcpy(pgpdfzMsgBufHDL,&msgBufHDL,sizeof(*pgpdfzMsgBufHDL));
	}
	
	pgpdfzMsgBufHDL->userID = UnionGetUserIDOfSharedMemoryModule(pgpdfzMsgBufMDL);
	//UnionDebugLog("userID = [%d]\n",pgpdfzMsgBufHDL->userID);

	if ((pgpdfzMsgBufHDL->queueIDOfFreePos = msgget(pgpdfzMsgBufHDL->userID,MSG_QUEUE_PERM)) == -1)
	{
		if ((pgpdfzMsgBufHDL->queueIDOfFreePos = msgget(pgpdfzMsgBufHDL->userID,MSG_QUEUE_PERM | IPC_CREAT)) == -1)
		{
			UnionSystemErrLog("in UnionReconnectMsgBufMDLAnyway:: msgget [%d]!\n",pgpdfzMsgBufHDL->userID);
			return(errCodeUseOSErrCode);
		}
		else
			newCreatedQueue = 1;
	}
			
	if ((pgpdfzMsgBufHDL->queueIDOfOccupiedPos = msgget(pgpdfzMsgBufHDL->userID + 1,MSG_QUEUE_PERM)) == -1)
	{
		if ((pgpdfzMsgBufHDL->queueIDOfOccupiedPos = msgget(pgpdfzMsgBufHDL->userID + 1,MSG_QUEUE_PERM | IPC_CREAT)) == -1)
		{
			UnionSystemErrLog("in UnionReconnectMsgBufMDLAnyway:: msgget [%d]!\n",pgpdfzMsgBufHDL->userID+1);
			return(errCodeUseOSErrCode);
		}
		else
			newCreatedQueue = 1;
	}
	gpdfzMsgBufMDLConnected = 1;
	
	if (newCreatedQueue)
	{
		if ((ret = UnionInitAllMsgIndex()) < 0)
		{
			UnionUserErrLog("in UnionReconnectMsgBufMDLAnyway:: UnionInitAllMsgIndex!\n");
		}
	}
	
	//UnionLogMsgBufStatus(pgpdfzMsgBufHDL);
	
	return(0);
}

int UnionConnectMsgBufMDL()
{
	if (gpdfzMsgBufMDLConnected)
		return(0);

	return(UnionReconnectMsgBufMDLAnyway());
}

int UnionReloadMsgBufDef()
{
	return(UnionConnectMsgBufMDL());
}

int UnionPrintAvailablMsgBufPosToFile(FILE *fp)
{
	TUnionFreePosOfMsgBuf		freePos;
	int				ret;
	int				tmpIndex;
	int				num = 0;
	int				pos[maxSize+1];
	int				existPos[maxSize+1];
	int				i;
	
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAvailablMsgBufPosToFile:: UnionConnectMsgBufMDL!\n");
		return(ret);
	}
	
	for (i = 0; i < maxSize; i++)
		existPos[i] = 0;
	for (;;)
	{
		memset(freePos.indexOfPos,0,sizeof(freePos.indexOfPos));
		if ((ret = msgrcv(pgpdfzMsgBufHDL->queueIDOfFreePos,&freePos,sizeof(freePos.indexOfPos),0,IPC_NOWAIT)) < 0)
			break;
		memcpy(&tmpIndex,freePos.indexOfPos,sizeof(int));
		if (num >= sizeof(pos))
			continue;
		if ((tmpIndex >= maxSize) || (tmpIndex < 0))
			continue;
		existPos[tmpIndex] = 1;
		pos[num] = tmpIndex;
		num++;
		//UnionFreeMsgBufPos(tmpIndex);
		
	}
	for (tmpIndex = 0; tmpIndex < num; tmpIndex++)
		UnionFreeMsgBufPos(pos[tmpIndex]);
	for (i = 0; i < maxSize; i++)
	{
		if (!existPos[i])
			continue;
		fprintf(fp,"[%04d]\n",i);
	}			
	fprintf(fp,"freeNum = [%04d]\n",num);
	return(num);
}

int UnionPrintInavailabeMsgBufPosToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintInavailabeMsgBufPosToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	ret = UnionPrintInavailabeMsgBufPosToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

int UnionPrintInavailabeMsgBufPosToFile(FILE *fp)
{
	TUnionOccupiedPosOfMsgBuf	occupiedPos;
	int				ret;
	int				tmpIndex;
	int				num = 0;
	int				pos[maxSize+1];
	long				type[maxSize+1];
	int				existPos[maxSize+1];
	long				printType[maxSize+1];
	int				i;
	
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionPrintInavailabeMsgBufPosToFile:: UnionConnectMsgBufMDL!\n");
		return(ret);
	}
	
	for (i = 0; i < maxSize; i++)
		existPos[i] = 0;
	for (;;)
	{
		memset(occupiedPos.indexOfPos,0,sizeof(occupiedPos.indexOfPos));
		if ((ret = msgrcv(pgpdfzMsgBufHDL->queueIDOfOccupiedPos,&occupiedPos,sizeof(occupiedPos.indexOfPos),0,IPC_NOWAIT)) < 0)
			break;
		memcpy(&tmpIndex,occupiedPos.indexOfPos,sizeof(int));
		//fprintf(fp,"%04d %012ld\n",tmpIndex,occupiedPos.typeOfMsg);
		if (num >= sizeof(pos))
			break;
		if ((tmpIndex >= maxSize) || (tmpIndex < 0))
			continue;
		existPos[tmpIndex] = 1;
		printType[tmpIndex] = occupiedPos.typeOfMsg;
		pos[num] = tmpIndex;
		type[num] = occupiedPos.typeOfMsg;
		num++;
		//UnionOccupyMsgBufPos(tmpIndex,occupiedPos.typeOfMsg);
	}
	for (tmpIndex = 0; tmpIndex < num; tmpIndex++)
		UnionOccupyMsgBufPos(pos[tmpIndex],type[tmpIndex]);
	for (i = 0; i < maxSize; i++)
	{
		if (!existPos[i])
			continue;
		fprintf(fp,"[%04d] [%012ld]\n",i,printType[i]);
	}			
	fprintf(fp,"occupiedNum = [%04d]\n",num);
	return(num);
}

int UnionPrintMsgBufToFile(FILE *fp)
{
	return(0);
}

int UnionDisconnectMsgBufMDL()
{
	pgpdfzMsgBuf = NULL;
	pgpdfzMsgBufHDL = NULL;
	return(UnionDisconnectShareModule(pgpdfzMsgBufMDL));
}

int UnionRemoveMsgBufMDL()
{
	struct shmid_ds		buf;
	int 			ID;
	char			tmpBuf[80];
	int			ret = 0;

	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionRemoveMsgBufMDL:: UnionConnectMsgBufMDL!\n");
		return(ret);
	}
	
	if ((ID = msgget(pgpdfzMsgBufHDL->userID,MSG_QUEUE_PERM)) >= 0)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"ipcrm -q %d",ID);
		system(tmpBuf);
	}

	if ((ID = msgget(pgpdfzMsgBufHDL->userID+1,MSG_QUEUE_PERM)) >= 0)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"ipcrm -q %d",ID);
		system(tmpBuf);
	}

	UnionDisconnectMsgBufMDL();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfMsgBuf));
}	

int UnionPrintMsgBufStatusToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintMsgBufStatusToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	ret = UnionPrintMsgBufStatusToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

int UnionPrintMsgBufStatusToFile(FILE *fp)
{
	int	ret;
	
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusToFile:: UnionConnectMsgBufMDL!\n");
		return(ret);
	}
	
	if (fp == NULL)
		fp = stderr;
		
	fprintf(fp,"Status of MsgBuf:\n");
	fprintf(fp,"[userID]                [%d]\n",pgpdfzMsgBufHDL->userID);
	fprintf(fp,"[maxSizeOfMsg]          [%d]\n",pgpdfzMsgBufHDL->maxSizeOfMsg);
	fprintf(fp,"[maxNumOfMsgPos]        [%d]\n",pgpdfzMsgBufHDL->maxNumOfMsg);
	fprintf(fp,"[maxStayTime]           [%d]\n",pgpdfzMsgBufHDL->maxStayTime);
	fprintf(fp,"[queueIDOfFreePos]      [%d]\n",pgpdfzMsgBufHDL->queueIDOfFreePos);
	fprintf(fp,"[queueIDOfOccupiedPos]  [%d]\n",pgpdfzMsgBufHDL->queueIDOfOccupiedPos);
	fprintf(fp,"[Num Of Free Pos]       [%d]\n",UnionGetNumOfFreeMsgBufPos());
	fprintf(fp,"[Num Of Occupied Pos]   [%d]\n",UnionGetNumOfOccupiedMsgBufPos());
	fprintf(fp,"\n");
	if ((fp == stderr) || (fp == stdout))
	{
		if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or quit/exit to exit...")))
			return(0);
	}
	UnionPrintSharedMemoryModuleToFile(pgpdfzMsgBufMDL,fp);
	return(0);
}

// 将index对应的消息位置置为可用
int UnionFreeMsgBufPos(int index)
{
	TUnionFreePosOfMsgBuf	PosOfMsgBuf;
	int			ret;
	PUnionMessageHeader	pheader;
		
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionFreeMsgBufPos:: UnionConnectMsgBufMDL!\n");
		return(ret);
	}
	
	if ((index < 0) || (index >= pgpdfzMsgBufHDL->maxNumOfMsg))
	{
		UnionUserErrLog("in UnionFreeMsgBufPos:: index = [%04d] out of range!\n",index);
		return(errCodeParameter);
	}
	
	pheader = (PUnionMessageHeader)(pgpdfzMsgBuf + index * (sizeof(TUnionMessageHeader) + pgpdfzMsgBufHDL->maxSizeOfMsg));
	pheader->type = 0;
	
	PosOfMsgBuf.statusOfPos = gAvailableMsgBufPos;
	memcpy(PosOfMsgBuf.indexOfPos,&index,sizeof(int));
	
	//if (msgsnd(pgpdfzMsgBufHDL->queueIDOfFreePos,&PosOfMsgBuf,sizeof(int),~IPC_NOWAIT) < 0)
	if (msgsnd(pgpdfzMsgBufHDL->queueIDOfFreePos,&PosOfMsgBuf,sizeof(int),0) < 0)
	{
		UnionSystemErrLog("in UnionFreeMsgBufPos:: msgsnd [%d]!\n",index);
		return(errCodeUseOSErrCode);
	}
	return(0);
}

// 获得可用的消息位置，返回值为所获位置
int UnionGetAvailableMsgBufPos()
{
	TUnionFreePosOfMsgBuf	PosOfMsgBuf;
	int			ret;
	int			index;
	
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionGetAvailableMsgBufPosUnionConnectMsgBufMDL!\n");
		return(ret);
	}
	
	memset(PosOfMsgBuf.indexOfPos,0,sizeof(PosOfMsgBuf.indexOfPos));
	//if ((ret = msgrcv(pgpdfzMsgBufHDL->queueIDOfFreePos,&PosOfMsgBuf,sizeof(int),0,~IPC_NOWAIT)) < 0)
	if ((ret = msgrcv(pgpdfzMsgBufHDL->queueIDOfFreePos,&PosOfMsgBuf,sizeof(int),0,0)) < 0)
	{
		UnionSystemErrLog("in UnionGetAvailableMsgBufPos:: msgrcv! Ret = [%d]\n",ret);
		return(errCodeUseOSErrCode);
	}
	memcpy(&index,PosOfMsgBuf.indexOfPos,sizeof(int));
	
	return(index);
}

// 获得可用位置的数量
int UnionGetNumOfFreeMsgBufPos()
{
	struct	msqid_ds pmsgq;
	int	ret;
	
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionGetNumOfFreeMsgBufPosUnionConnectMsgBufMDL!\n");
		return(ret);
	}
		
	if (msgctl(pgpdfzMsgBufHDL->queueIDOfFreePos,IPC_STAT,&pmsgq) != 0)
	{
		UnionSystemErrLog("in UnionGetNumOfFreeMsgBufPos:: msgctl! [%d]\n",pgpdfzMsgBufHDL->queueIDOfFreePos);
		return(errCodeUseOSErrCode);
	}
	else
		return(pmsgq.msg_qnum);
}

// 获得已占位置的数量
int UnionGetNumOfOccupiedMsgBufPos()
{
	struct	msqid_ds pmsgq;
	int	ret;
	
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionGetNumOfOccupiedMsgBufPosUnionConnectMsgBufMDL!\n");
		return(ret);
	}
		
	if (msgctl(pgpdfzMsgBufHDL->queueIDOfOccupiedPos,IPC_STAT,&pmsgq) != 0)
	{
		UnionSystemErrLog("in UnionGetNumOfOccupiedMsgBufPos:: msgctl! [%d]\n",pgpdfzMsgBufHDL->queueIDOfOccupiedPos);
		return(errCodeUseOSErrCode);
	}
	else
		return(pmsgq.msg_qnum);
}

// 将index对应的消息位置置为有消息
// typeOfMsg为消息类型
int UnionOccupyMsgBufPos(int index,long typeOfMsg)
{
	int				ret;
	TUnionOccupiedPosOfMsgBuf	PosOfMsgBuf;
	
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionOccupyMsgBufPosUnionConnectMsgBufMDL!\n");
		return(ret);
	}
	
	PosOfMsgBuf.typeOfMsg = typeOfMsg;
	memcpy(PosOfMsgBuf.indexOfPos,&index,sizeof(int));
	//if (msgsnd(pgpdfzMsgBufHDL->queueIDOfOccupiedPos,&PosOfMsgBuf,sizeof(PosOfMsgBuf.indexOfPos),~IPC_NOWAIT) < 0)
	if (msgsnd(pgpdfzMsgBufHDL->queueIDOfOccupiedPos,&PosOfMsgBuf,sizeof(PosOfMsgBuf.indexOfPos),0) < 0)
	{
		UnionSystemErrLog("in UnionOccupyMsgBufPos:: msgsnd!\n");
		return(errCodeUseOSErrCode);
	}
	else
		return(0);
}

// 获得第一个有消息的缓冲位置，返回值为该位置
// *typeOfMsg，返回占据该位置的消息的类型
int UnionGetOccupiedMsgBufPos(long *typeOfMsg)
{
	TUnionOccupiedPosOfMsgBuf	PosOfMsgBuf;
	int				ret;
	int				index;
	
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionGetOccupiedMsgBufPosUnionConnectMsgBufMDL!\n");
		return(ret);
	}
	
	memset(PosOfMsgBuf.indexOfPos,0,sizeof(PosOfMsgBuf.indexOfPos));
	//if ((ret = msgrcv(pgpdfzMsgBufHDL->queueIDOfOccupiedPos,&PosOfMsgBuf,sizeof(int),0,~IPC_NOWAIT)) < 0)
	if ((ret = msgrcv(pgpdfzMsgBufHDL->queueIDOfOccupiedPos,&PosOfMsgBuf,sizeof(int),0,0)) < 0)
	{
		UnionSystemErrLog("in UnionGetOccupiedMsgBufPos:: msgrcv! Ret = [%d]\n",ret);
		return(errCodeUseOSErrCode);
	}
	memcpy(&index,PosOfMsgBuf.indexOfPos,sizeof(int));
	*typeOfMsg = PosOfMsgBuf.typeOfMsg;
		
	return(index);
}

// 获得第一个有指定消息的缓冲位置，返回值为该位置
// typeOfMsg，指定的消息类型
int UnionGetOccupiedMsgBufPosOfMsgType(long typeOfMsg)
{
	TUnionOccupiedPosOfMsgBuf	PosOfMsgBuf;
	int				ret;
	int				index;

	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionGetOccupiedMsgBufPosOfMsgTypeUnionConnectMsgBufMDL!\n");
		return(ret);
	}
	memset(PosOfMsgBuf.indexOfPos,0,sizeof(PosOfMsgBuf.indexOfPos));
	PosOfMsgBuf.typeOfMsg = typeOfMsg;
	//if ((ret = msgrcv(pgpdfzMsgBufHDL->queueIDOfOccupiedPos,&PosOfMsgBuf,sizeof(PosOfMsgBuf.indexOfPos),typeOfMsg,~IPC_NOWAIT)) < 0)
	if ((ret = msgrcv(pgpdfzMsgBufHDL->queueIDOfOccupiedPos,&PosOfMsgBuf,sizeof(int),typeOfMsg,0)) < 0)
	{
		UnionSystemErrLog("in UnionGetOccupiedMsgBufPosOfMsgType:: msgrcv! Ret = [%d]\n",ret);
		return(errCodeUseOSErrCode);
	}
	else
	{
		memcpy(&index,PosOfMsgBuf.indexOfPos,sizeof(int));
		return(index);
	}
}

// Msg为要缓冲的消息
// LenOfMsg，是要缓冲的消息的长度
// typeOfMsg，是消息类型
// poriHeader，原始的消息头
int UnionBufferMessageWithOriginHeader(unsigned char * Msg,int LenOfMsg,long typeOfMsg,PUnionMessageHeader poriHeader)
{
	int			indexOfMsg;
	unsigned char 		*pAddr;
	int			Ret;
	PUnionMessageHeader	pheader;
	
	if ((indexOfMsg = UnionGetAvailableMsgBufPos()) < 0)
	{
		UnionUserErrLog("in UnionBufferMessageWithOriginHeader:: UnionGetAvailableMsgBufPos!\n");
		if (indexOfMsg < 0)
			return(indexOfMsg);
		else
			return(errCodeMsgBufMDL_OutofRange);
	}
	if ((Msg == NULL) || (LenOfMsg <= 0) || (LenOfMsg > pgpdfzMsgBufHDL->maxSizeOfMsg) || (typeOfMsg <= 0)  || (indexOfMsg >= pgpdfzMsgBufHDL->maxNumOfMsg))
	{
		UnionFreeMsgBufPos(indexOfMsg); //add by hzh in 2012.4.10
		UnionUserErrLog("in UnionBufferMessageWithOriginHeader:: parameter error! lenOfMsg = [%d] pgpdfzMsgBufHDL->maxSizeOfMsg = [%d] typeOfMsg = [%ld]\n",LenOfMsg,pgpdfzMsgBufHDL->maxSizeOfMsg,typeOfMsg);
		return(errCodeParameter);
	}
	
	pAddr = pgpdfzMsgBuf + indexOfMsg * (sizeof(TUnionMessageHeader) + pgpdfzMsgBufHDL->maxSizeOfMsg);
	pheader = (PUnionMessageHeader)pAddr;
	pheader->len = LenOfMsg;
	if (poriHeader == NULL)
	{
		time(&(pheader->time));
		pheader->provider = getpid();
		pheader->dealer = 0;
		pheader->msgIndex = UnionApplyUniqueMsgIndex();	// 2007/11/15增加
		//pheader->msgIndex = typeOfMsg;	// 2007/11/15 删除
	}
	else
	{
		pheader->time = poriHeader->time;
		pheader->provider = poriHeader->provider;
		pheader->dealer = getpid();
		pheader->msgIndex = poriHeader->msgIndex;
	}
	pheader->type = typeOfMsg;
	memcpy(pAddr + sizeof(TUnionMessageHeader),Msg,LenOfMsg);
	if ((Ret = UnionOccupyMsgBufPos(indexOfMsg,typeOfMsg)) < 0)
	{
		UnionUserErrLog("in UnionBufferMessageWithOriginHeader:: UnionOccupyMsgBufPos indexOfMs = [%d]\n",indexOfMsg);
		//pheader->type = 0;
		UnionFreeMsgBufPos(indexOfMsg);
		return(Ret);
	}
	return(0);
}

// Msg为要缓冲的消息
// LenOfMsg，是要缓冲的消息的长度
// typeOfMsg，是消息类型
// pnewHeader，新的消息头，输出参数
int UnionBufferMessageWithNewHeader(unsigned char * Msg,int LenOfMsg,long typeOfMsg,PUnionMessageHeader pnewHeader)
{
	int			indexOfMsg;
	unsigned char 		*pAddr;
	int			Ret;
	PUnionMessageHeader	pheader;
	
	if ((indexOfMsg = UnionGetAvailableMsgBufPos()) < 0)
	{
		UnionUserErrLog("in UnionBufferMessageWithNewHeader:: UnionGetAvailableMsgBufPos!\n");
		if (indexOfMsg < 0)
			return(indexOfMsg);
		else
			return(errCodeMsgBufMDL_OutofRange);
	}
	if ((Msg == NULL) || (LenOfMsg <= 0) || (LenOfMsg > pgpdfzMsgBufHDL->maxSizeOfMsg) || (typeOfMsg <= 0)  || (indexOfMsg >= pgpdfzMsgBufHDL->maxNumOfMsg))
	{
		UnionFreeMsgBufPos(indexOfMsg);  //add by hzh in 2012.04.10
		UnionUserErrLog("in UnionBufferMessageWithNewHeader:: parameter error! lenOfMsg = [%d] pgpdfzMsgBufHDL->maxSizeOfMsg = [%d] typeOfMsg = [%ld]\n",LenOfMsg,pgpdfzMsgBufHDL->maxSizeOfMsg,typeOfMsg);
		return(errCodeParameter);
	}
	
	pAddr = pgpdfzMsgBuf + indexOfMsg * (sizeof(TUnionMessageHeader) + pgpdfzMsgBufHDL->maxSizeOfMsg);
	pheader = (PUnionMessageHeader)pAddr;
	pheader->len = LenOfMsg;
	time(&(pheader->time));
	pheader->provider = getpid();
	pheader->dealer = 0;
	//pheader->msgIndex = typeOfMsg;	// 2007/11/15 删除
	pheader->msgIndex = UnionApplyUniqueMsgIndex(); // 2007/11/15 增加
	pheader->type = typeOfMsg;
	if (pnewHeader != NULL)
		memcpy(pnewHeader,pheader,sizeof(*pheader));
	memcpy(pAddr + sizeof(TUnionMessageHeader),Msg,LenOfMsg);
	if ((Ret = UnionOccupyMsgBufPos(indexOfMsg,typeOfMsg)) < 0)
	{
		UnionUserErrLog("in UnionBufferMessageWithNewHeader:: UnionOccupyMsgBufPos indexOfMs = [%d]\n",indexOfMsg);
		//pheader->type = 0;
		UnionFreeMsgBufPos(indexOfMsg);
		return(Ret);
	}
	return(0);
}
// Msg为要缓冲的消息
// LenOfMsg，是要缓冲的消息的长度
// typeOfMsg，是消息类型
// pnewHeader，新的消息头，输出参数
int UnionBufferMessageWithNewHeaderForHsm(int applierMDLID, int seqReqTime, unsigned char * Msg,int LenOfMsg,long typeOfMsg,PUnionMessageHeader pnewHeader)
{
	int			indexOfMsg;
	unsigned char 		*pAddr;
	int			Ret;
	PUnionMessageHeader	pheader;
	
	if ((indexOfMsg = UnionGetAvailableMsgBufPos()) < 0)
	{
		UnionUserErrLog("in UnionBufferMessageWithNewHeader:: UnionGetAvailableMsgBufPos!\n");
		if (indexOfMsg < 0)
			return(indexOfMsg);
		else
			return(errCodeMsgBufMDL_OutofRange);
	}
	if ((Msg == NULL) || (LenOfMsg <= 0) || (LenOfMsg > pgpdfzMsgBufHDL->maxSizeOfMsg) || (typeOfMsg <= 0)  || (indexOfMsg >= pgpdfzMsgBufHDL->maxNumOfMsg))
	{
		UnionFreeMsgBufPos(indexOfMsg);  //add by hzh in 2012.04.10
		UnionUserErrLog("in UnionBufferMessageWithNewHeader:: parameter error! lenOfMsg = [%d] pgpdfzMsgBufHDL->maxSizeOfMsg = [%d] typeOfMsg = [%ld]\n",LenOfMsg,pgpdfzMsgBufHDL->maxSizeOfMsg,typeOfMsg);
		return(errCodeParameter);
	}
	
	pAddr = pgpdfzMsgBuf + indexOfMsg * (sizeof(TUnionMessageHeader) + pgpdfzMsgBufHDL->maxSizeOfMsg);
	pheader = (PUnionMessageHeader)pAddr;
	pheader->len = LenOfMsg;
	pheader->time = seqReqTime;
	pheader->provider = applierMDLID;
	pheader->dealer = 0;
	//pheader->msgIndex = typeOfMsg;	// 2007/11/15 删除
	pheader->msgIndex = UnionApplyUniqueMsgIndex(); // 2007/11/15 增加
	pheader->type = typeOfMsg;
	if (pnewHeader != NULL)
		memcpy(pnewHeader,pheader,sizeof(*pheader));
	memcpy(pAddr + sizeof(TUnionMessageHeader),Msg,LenOfMsg);
	if ((Ret = UnionOccupyMsgBufPos(indexOfMsg,typeOfMsg)) < 0)
	{
		UnionUserErrLog("in UnionBufferMessageWithNewHeader:: UnionOccupyMsgBufPos indexOfMs = [%d]\n",indexOfMsg);
		//pheader->type = 0;
		UnionFreeMsgBufPos(indexOfMsg);
		return(Ret);
	}
	return(0);
}

// Msg为要缓冲的消息
// LenOfMsg，是要缓冲的消息的长度
// typeOfMsg，是消息类型
int UnionBufferMessage(unsigned char * Msg,int LenOfMsg,long typeOfMsg)
{
	return(UnionBufferMessageWithNewHeader(Msg,LenOfMsg,typeOfMsg,NULL));
}

// 读取第一条消息
// LenOfMsgBuf，是Msg缓冲的大小
// typeOfMsg，是指定消息的类型
// poutputHeader是消息的提供者信息
// 返回值为消息的长度
int UnionReadFirstMsgOfTypeUntilSuccessWithHeader(unsigned char * Msg,int LenOfMsgBuf,long typeOfMsg,PUnionMessageHeader poutputHeader)
{
	int			indexOfMsg;
	unsigned char 		*pAddr;
	int			LenOfMsg;
	PUnionMessageHeader	pheader;
	
	if (Msg == NULL)
	{
		UnionUserErrLog("in UnionReadFirstMsgOfTypeUntilSuccessWithHeader:: Msg is NULL!\n");
		return(errCodeParameter);
	}

	if (((indexOfMsg = UnionGetOccupiedMsgBufPosOfMsgType(typeOfMsg)) < 0)  || (indexOfMsg >= pgpdfzMsgBufHDL->maxNumOfMsg))
	{
		UnionUserErrLog("in UnionReadFirstMsgOfTypeUntilSuccessWithHeader:: UnionGetOccupiedMsgBufPosOfMsgType! indexOfMsg = [%d]\n",indexOfMsg);
		if (indexOfMsg < 0)
			return(indexOfMsg);
		else
			return(errCodeMsgBufMDL_OutofRange);
	}
	pAddr = pgpdfzMsgBuf + indexOfMsg * (sizeof(TUnionMessageHeader) + pgpdfzMsgBufHDL->maxSizeOfMsg);
	pheader = (PUnionMessageHeader)pAddr;
	if (((LenOfMsg = pheader->len) < 0) || (LenOfMsg >= LenOfMsgBuf) || (LenOfMsg > pgpdfzMsgBufHDL->maxSizeOfMsg))
	{
		UnionUserErrLog("in UnionReadFirstMsgOfTypeUntilSuccessWithHeader:: LenOfMsg [%d] error!\n",LenOfMsg);
		//pheader->type = 0;
		UnionFreeMsgBufPos(indexOfMsg);
		return(errCodeMsgBufMDL_MsgLen);
	}
	memcpy(Msg,pAddr + sizeof(TUnionMessageHeader),LenOfMsg);
	if (poutputHeader != NULL)
		memcpy(poutputHeader,pheader,sizeof(*pheader));	// 2007/1/29增加
	//pheader->type = 0;
	
	UnionFreeMsgBufPos(indexOfMsg);

	return(LenOfMsg);
}

// 读取第一条消息
// LenOfMsgBuf，是Msg缓冲的大小
// typeOfMsg，是第一条消息的类型
// 返回值为消息的长度
int UnionReadFirstMsg(unsigned char * Msg,int LenOfMsgBuf,long *typeOfMsg)
{
	int			indexOfMsg;
	unsigned char 		*pAddr;
	int			LenOfMsg;
	PUnionMessageHeader	pheader;
	
	if ((Msg == NULL) || (typeOfMsg == NULL))
	{
		UnionUserErrLog("in UnionReadFirstMsg:: Msg is NULL!\n");
		return(errCodeParameter);
	}
	
	if (((indexOfMsg = UnionGetOccupiedMsgBufPos(typeOfMsg)) < 0) || (indexOfMsg >= pgpdfzMsgBufHDL->maxNumOfMsg))
	{
		UnionUserErrLog("in UnionReadFirstMsg:: UnionGetOccupiedMsgBufPos! indexOfMsg = [%ld]\n",indexOfMsg);
		if (indexOfMsg < 0)
			return(indexOfMsg);
		else
			return(errCodeMsgBufMDL_OutofRange);
	}
	
	pAddr = pgpdfzMsgBuf + indexOfMsg * (sizeof(TUnionMessageHeader) + pgpdfzMsgBufHDL->maxSizeOfMsg);
	pheader = (PUnionMessageHeader)pAddr;
	if (((LenOfMsg = pheader->len) < 0) || (LenOfMsg >= LenOfMsgBuf) || (LenOfMsg > pgpdfzMsgBufHDL->maxSizeOfMsg))
	{
		UnionUserErrLog("in UnionReadFirstMsg:: LenOfMsg [%d] error!\n",LenOfMsg);
		//pheader->type = 0;
		UnionFreeMsgBufPos(indexOfMsg);
		return(errCodeMsgBufMDL_MsgLen);
	}
	memcpy(Msg,pAddr + sizeof(TUnionMessageHeader),LenOfMsg);
	//pheader->type = 0;
	UnionFreeMsgBufPos(indexOfMsg);

	return(LenOfMsg);
}

// 读取第一条消息
// LenOfMsgBuf，是Msg缓冲的大小
// typeOfMsg，是指定消息的类型
// 返回值为消息的长度
int UnionReadFirstMsgOfTypeUntilSuccess(unsigned char * Msg,int LenOfMsgBuf,long typeOfMsg)
{
	return(UnionReadFirstMsgOfTypeUntilSuccessWithHeader(Msg,LenOfMsgBuf,typeOfMsg,NULL));
}


// 读取第一条消息
// LenOfMsgBuf，是Msg缓冲的大小
// typeOfMsg，是指定消息的类型
// waitTime，是消息可以等待处理的最大时间值，超过该值的消息将直接丢弃
// 返回值为消息的长度
int UnionReadFirstMsgOfTypeUntilSuccess_FilterOutdateMsg(unsigned char * Msg,int LenOfMsgBuf,long typeOfMsg,long waitTime)
{
	int			LenOfMsg;
	TUnionMessageHeader	header;
	time_t			now_time;
	int			offset;
loop:
	if ((LenOfMsg = UnionReadFirstMsgOfTypeUntilSuccessWithHeader(Msg,LenOfMsgBuf,typeOfMsg,&header)) < 0)
	{
		UnionUserErrLog("in UnionReadFirstMsgOfTypeUntilSuccess_FilterOutdateMsg:: UnionReadFirstMsgOfTypeUntilSuccessWithHeader!\n");
		return(LenOfMsg);
	}
	if (waitTime > 0)
	{	
		time(&now_time);
		if (now_time - header.time > waitTime)
		{
			Msg[LenOfMsg] = 0;
			UnionAuditLog("outdate msg:: [%012ld] [%07d] [%04ld] [%04d]\n",
					header.type,header.provider,now_time - header.time,LenOfMsg);
			Msg[LenOfMsg] = 0;
			if ((offset = UnionReadIntTypeRECVar("offsetOfMsgInMsgBuf")) < 0)
				offset = 0;
			if (LenOfMsg - offset >= 0)
				UnionAuditNullLog("[%s]\n",Msg+offset);
			else			
				UnionMemNullLogWithTime(Msg,LenOfMsg);
			goto loop;
		}
	}
	return(LenOfMsg);
}

// 获得第一个有指定消息的缓冲位置，返回值为该位置
// typeOfMsg，指定的消息类型
int UnionGetOccupiedMsgBufPosOfMsgTypeNoWait(long typeOfMsg)
{
	TUnionOccupiedPosOfMsgBuf	PosOfMsgBuf;
	int				ret;
	int				index;

	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionGetOccupiedMsgBufPosOfMsgTypeNoWaitUnionConnectMsgBufMDL!\n");
		return(ret);
	}
	memset(PosOfMsgBuf.indexOfPos,0,sizeof(PosOfMsgBuf.indexOfPos));
	
	if ((ret = msgrcv(pgpdfzMsgBufHDL->queueIDOfOccupiedPos,&PosOfMsgBuf,sizeof(int),typeOfMsg,IPC_NOWAIT)) < 0)
	{
		//UnionSystemErrLog("in UnionGetOccupiedMsgBufPosOfMsgTypeNoWait:: msgrcv! Ret = [%d]\n",ret);
		return(errCodeUseOSErrCode);
	}
	else
	{
		memcpy(&index,PosOfMsgBuf.indexOfPos,sizeof(int));
		return(index);
	}
}

// 删除指定的响应消息
// LenOfMsgBuf，是Msg缓冲的大小
// typeOfMsg，是指定消息的类型
// 返回值为消息的长度
int UnionDeleteSpecifiedMsg(long typeOfMsg)
{
	int			indexOfMsg;
	int			DeletedNum = 0;
	int			ret;
	PUnionMessageHeader	pheader;
		
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionDeleteSpecifiedMsgUnionConnectMsgBufMDL!\n");
		return(ret);
	}
	for (;;)
	{
		if ((indexOfMsg = UnionGetOccupiedMsgBufPosOfMsgTypeNoWait(typeOfMsg)) < 0)
		{
			//UnionUserErrLog("in UnionDeleteSpecifiedMsg:: UnionGetOccupiedMsgBufPos!\n");
			return(DeletedNum);
		}
		DeletedNum++;
		if (UnionFreeMsgBufPos(indexOfMsg) < 0)
		{
			UnionUserErrLog("in UnionDeleteSpecifiedMsg:: UnionFreeMsgBufPos!\n");
		}
	}
}

// 将所有位置置为可用
int UnionInitAllMsgIndex()
{
	TUnionFreePosOfMsgBuf		freePos;
	TUnionOccupiedPosOfMsgBuf	occupiedPos;
	int				ret;
	int				index;
	//PUnionMessageHeader		pheader;
	
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionInitAllMsgIndex:: UnionConnectMsgBufMDL!\n");
		return(ret);
	}
	
	
	for (;;)
	{
		if ((ret = msgrcv(pgpdfzMsgBufHDL->queueIDOfFreePos,&freePos,sizeof(freePos.indexOfPos),0,IPC_NOWAIT)) < 0)
			break;
	}
	for (;;)
	{
		if ((ret = msgrcv(pgpdfzMsgBufHDL->queueIDOfOccupiedPos,&occupiedPos,sizeof(occupiedPos.indexOfPos),0,IPC_NOWAIT)) < 0)
			break;
	}
	
	
	for (index = 0; index < pgpdfzMsgBufHDL->maxNumOfMsg; index++)
	{
		UnionFreeMsgBufPos(index);
	}
	
	return(0);
}


int UnionFreeRubbishMsg()
{
	int			index;
	PUnionMessageHeader	pheader;
	TUnionMessageHeader	header;
	unsigned char		*pmsg;
	int			num;
	time_t			nowTime;
	int			ret;
	unsigned char		buf[8192+1];
	int			logLen;
	int			offset;
		
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionFreeRubbishMsg:: UnionConnectMsgBufMDL!\n");
		return(ret);
	}	
	
	// 清空垃圾消息
	time(&nowTime);
	for (index = 0,num = 0; index < pgpdfzMsgBufHDL->maxNumOfMsg; index++)
	{
		if ((pheader = (PUnionMessageHeader)(pgpdfzMsgBuf + (sizeof(TUnionMessageHeader) + pgpdfzMsgBufHDL->maxSizeOfMsg) * index)) == NULL)
			return(errCodeMsgBufMDL_OutofRange);
		if (pheader->type <= 0)
			continue;
		if (pheader->time <= 0)
			continue;
		if (nowTime - pheader->time < pgpdfzMsgBufHDL->maxStayTime)
			continue;
		//if (UnionGetPIDOutOfMDLID(pheader->type) <= 0)	// 2007/04/12 屏蔽该句
		//	continue;
		memcpy(&header,pheader,sizeof(header));
		pmsg = (unsigned char *)(pgpdfzMsgBuf + index * (sizeof(TUnionMessageHeader) + pgpdfzMsgBufHDL->maxSizeOfMsg) + sizeof(*pheader));
		if (pheader->len > 0)
		{
			if (pheader->len >= sizeof(buf))
				logLen = sizeof(buf) - 1;
			else
				logLen = pheader->len;
			memcpy(buf,pmsg,logLen);
			buf[logLen] = 0;
		}
		else
			logLen = 0;
		//memset(pheader,0,sizeof(*pheader));
		if ((ret = UnionResetMsgIndexOfSpecTypeAvailable(header.type,index)) >= 0)
		{
			if ((offset = UnionReadIntTypeRECVar("offsetOfMsgInMsgBuf")) < 0)
				offset = 0;
			if (logLen - offset > 0)
				UnionAuditLog("free rubbish:: [%04d] [%012ld] [%07d] [%04ld] [%04d]\n[%s]\n",
						index,header.type,header.provider,nowTime - header.time,header.len,
						buf+offset);
			else
				UnionAuditLog("free rubbish:: [%04d] [%012ld] [%07d] [%04ld] [%04d]\n",
						index,header.type,header.provider,nowTime - header.time,header.len);
			num++;
			continue;	// 2007/04/12，增加该句
		}
		/* 2007/04/12，增加以下内容 */
		if (ret != errCodeMsgBufMDL_MsgOfTypeAndOfIndexNotExists)
			continue;
		// 消息是丢失了的消息，执行以下代码
		if ((ret = UnionRepairSpecMsgBufIndex(index)) >= 0)
		{
			if ((offset = UnionReadIntTypeRECVar("offsetOfMsgInMsgBuf")) < 0)
				offset = 0;
			if (logLen - offset > 0)
				UnionAuditLog("repair  lost:: [%04d] [%012ld] [%07d] [%04ld] [%04d]\n[%s]\n",
						index,header.type,header.provider,nowTime - header.time,header.len,
						buf+offset);
			else
				UnionAuditLog("repair  lost:: [%04d] [%012ld] [%07d] [%04ld] [%04d]\n",
						index,header.type,header.provider,nowTime - header.time,header.len);
			num++;
			continue;
		}
		UnionUserErrLog("in UnionFreeRubbishMsg:: clear type = [%012ld] index = [%04d] message failure!\n",header.type,index);
		/* 2007/04/12，增加结束 */
	}
	if (num > 0)
		UnionAuditLog("in UnionFreeRubbishMsg:: [%d] Rubbish are cleared\n",num);
	return(0);
}

int UnionLogMsgBufStatus(PUnionMsgBufHDL pmsgBufHDL)
{
	if (pmsgBufHDL == NULL)
	{
		UnionUserErrLog("in UnionLogMsgBufStatus:: pmsgBufHDL is null!\n");
		return(errCodeParameter);
	}
	
	UnionNullLog("Status of MsgBuf:\n");
	UnionNullLog("[userID]                [%d]\n",pgpdfzMsgBufHDL->userID);
	UnionNullLog("[maxSizeOfMsg]          [%d]\n",pgpdfzMsgBufHDL->maxSizeOfMsg);
	UnionNullLog("[maxNumOfMsgPos]        [%d]\n",pgpdfzMsgBufHDL->maxNumOfMsg);
	UnionNullLog("[maxStayTime]           [%d]\n",pgpdfzMsgBufHDL->maxStayTime);
	UnionNullLog("[queueIDOfFreePos]      [%d]\n",pgpdfzMsgBufHDL->queueIDOfFreePos);
	UnionNullLog("[queueIDOfOccupiedPos]  [%d]\n",pgpdfzMsgBufHDL->queueIDOfOccupiedPos);
	UnionNullLog("[Num Of Free Pos]       [%d]\n",UnionGetNumOfFreeMsgBufPos());
	UnionNullLog("[Num Of Occupied Pos]   [%d]\n",UnionGetNumOfOccupiedMsgBufPos());
	return(0);
}

// 将所有位置置为可用
int UnionResetAllMsgIndexAvailable()
{
	TUnionOccupiedPosOfMsgBuf	occupiedPos;
	int				ret;
	int				index;
	PUnionMessageHeader		pheader;
	int				num = 0;
	time_t				now_time;
	unsigned char			*pmsg;
	int				offset;
		
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionResetAllMsgIndexAvailable:: UnionConnectMsgBufMDL!\n");
		return(ret);
	}
	
	for (;;)
	{
		//memset(indexStatus.index,0,sizeof(indexStatus.index));
		if ((ret = msgrcv(pgpdfzMsgBufHDL->queueIDOfOccupiedPos,&occupiedPos,sizeof(occupiedPos.indexOfPos),0,IPC_NOWAIT)) < 0)
			break;
		memcpy(&index,occupiedPos.indexOfPos,sizeof(int));
		if ((index < 0) || (index >= pgpdfzMsgBufHDL->maxNumOfMsg))
			continue;
		pheader = (PUnionMessageHeader)(pgpdfzMsgBuf + index * (sizeof(TUnionMessageHeader) + pgpdfzMsgBufHDL->maxSizeOfMsg));
		time(&now_time);
		UnionAuditLog("reset msg:: [%04d] [%012ld] [%07d] [%04ld] [%04d]\n",
				index,pheader->type,pheader->provider,now_time - pheader->time,pheader->len);
		pmsg = (unsigned char *)(pgpdfzMsgBuf + index * (sizeof(TUnionMessageHeader) + pgpdfzMsgBufHDL->maxSizeOfMsg) + sizeof(*pheader));
		if (pheader->len > 0)
		{
			if ((offset = UnionReadIntTypeRECVar("offsetOfMsgInMsgBuf")) < 0)
				offset = 0;
			if ((pheader->len - offset >= 0) && (pheader->len < pgpdfzMsgBufHDL->maxSizeOfMsg))
			{
				pmsg[pheader->len] = 0;
				UnionAuditNullLog("[%s]\n",pmsg+offset);
			}
			else
				UnionMemNullLogWithTime(pmsg,pheader->len);
		}
		//pheader->type = 0;
		UnionFreeMsgBufPos(index);
		num++;
	}
	
	return(num);
}

long UnionGetTotalNumOfMsgBufMDL()
{
	int	ret;
	
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionGetTotalNumOfMsgBufMDL:: UnionConnectMsgBufMDL!\n");
		return(ret);
	}
	return(pgpdfzMsgBufHDL->maxNumOfMsg);
}

int UnionModifyMsgMaxStayTimeInBuf(long time)
{
	int	ret;
	char	fileName[512+1];
	
	if (time <= 0)
		return(errCodeParameter);
		
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionModifyMsgMaxStayTimeInBuf:: UnionConnectMsgBufMDL!\n");
		return(ret);
	}
	pgpdfzMsgBufHDL->maxStayTime = time;
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfMsgBufHDL(fileName);
	if ((ret = UnionUpdateEnviVar(fileName,"maxStayTime","[%ld]",time)) < 0)
	{
		UnionUserErrLog("in UnionModifyMsgMaxStayTimeInBuf:: UnionUpdateEnviVar!\n");
		return(ret);
	}
	return(ret);
}

PUnionMessageHeader UnionGetMessageHeaderOfIndexInMsgGrp(int index)
{
	if (UnionConnectMsgBufMDL() < 0)
		return(NULL);
	if ((index < 0) || (index >= pgpdfzMsgBufHDL->maxNumOfMsg))
		return(NULL);
	return((PUnionMessageHeader)(pgpdfzMsgBuf + index * (sizeof(TUnionMessageHeader) + pgpdfzMsgBufHDL->maxSizeOfMsg)));
}

int UnionGetMaxMsgNumOfMsgBufMDL()
{
	int	ret;
	
	if ((ret = UnionConnectMsgBufMDL()) < 0)
		return(ret);
	else
		return(pgpdfzMsgBufHDL->maxNumOfMsg);
}

// 2007/04/12 增加
// 设置指令的消息索引丢失
int UnionSetSpecMsgBufIndexLosted(int index)
{
	TUnionOccupiedPosOfMsgBuf	occupiedPos;
	TUnionFreePosOfMsgBuf		freePos;
	int				ret;
	int				tmpIndex;
	int				pos[maxSize+1];
	long				type[maxSize+1];
	int				num = 0;
	int				lostIndexFound = 0;
		
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionSetSpecMsgBufIndexLosted:: UnionConnectMsgBufMDL!\n");
		return(ret);
	}
	
	for (num = 0;;)
	{
		memset(occupiedPos.indexOfPos,0,sizeof(occupiedPos.indexOfPos));
		if ((ret = msgrcv(pgpdfzMsgBufHDL->queueIDOfOccupiedPos,&occupiedPos,sizeof(occupiedPos.indexOfPos),0,IPC_NOWAIT)) < 0)
			break;
		memcpy(&tmpIndex,occupiedPos.indexOfPos,sizeof(int));
		if (tmpIndex == index)
		{
			UnionAuditLog("in UnionSetSpecMsgBufIndexLosted:: set occupied index %04d %012ld losted OK!\n",tmpIndex,occupiedPos.typeOfMsg);
			lostIndexFound = 1;
			continue;
		}
		if (num >= sizeof(pos))
			continue;
		pos[num] = tmpIndex;
		type[num] = occupiedPos.typeOfMsg;
		num++;
	}
	for (tmpIndex = 0; tmpIndex < num; tmpIndex++)
		UnionOccupyMsgBufPos(pos[tmpIndex],type[tmpIndex]);
	for (num = 0;;)
	{
		memset(freePos.indexOfPos,0,sizeof(freePos.indexOfPos));
		if ((ret = msgrcv(pgpdfzMsgBufHDL->queueIDOfFreePos,&freePos,sizeof(freePos.indexOfPos),0,IPC_NOWAIT)) < 0)
			break;
		memcpy(&tmpIndex,freePos.indexOfPos,sizeof(int));
		if (tmpIndex == index)
		{
			UnionAuditLog("in UnionSetSpecMsgBufIndexLosted:: set free index %04d %012ld losted OK!\n",tmpIndex,occupiedPos.typeOfMsg);
			lostIndexFound = 1;
			continue;
		}
		if (num >= sizeof(pos))
			continue;
		pos[num] = tmpIndex;
		num++;
		
	}
	for (tmpIndex = 0; tmpIndex < num; tmpIndex++)
		UnionFreeMsgBufPos(pos[tmpIndex]);
	if (lostIndexFound)
		return(0);
	return(errCodeMsgBufMDL_MsgIndexAlreadyLosted);
}

// 2007/04/12 增加
// 修复索引
int UnionRepairSpecMsgBufIndex(int index)
{
	int	ret;
	
	if ((ret = UnionSetSpecMsgBufIndexLosted(index)) < 0)
	{
		if (ret != errCodeMsgBufMDL_MsgIndexAlreadyLosted)
		{
			UnionUserErrLog("in UnionRepairSpecMsgBufIndex:: UnionSetSpecMsgBufIndexLosted!\n");
			return(ret);
		}
	}
	return(UnionFreeMsgBufPos(index));
}

// 2007/4/12,增加
int UnionRepairAllMsgBufIndex()
{
	int			index;
	PUnionMessageHeader	pheader;
	TUnionMessageHeader	header;
	unsigned char		*pmsg;
	int			num;
	time_t			nowTime;
	int			ret;
	unsigned char		buf[8192+1];
	int			logLen;
	int			offset;
		
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionRepairAllMsgBufIndex:: UnionConnectMsgBufMDL!\n");
		return(ret);
	}	
	// 清空垃圾消息
	time(&nowTime);
	for (index = 0,num = 0; index < pgpdfzMsgBufHDL->maxNumOfMsg; index++)
	{
		if ((pheader = (PUnionMessageHeader)(pgpdfzMsgBuf + (sizeof(TUnionMessageHeader) + pgpdfzMsgBufHDL->maxSizeOfMsg) * index)) == NULL)
			return(errCodeMsgBufMDL_OutofRange);
		memcpy(&header,pheader,sizeof(header));
		pmsg = (unsigned char *)(pgpdfzMsgBuf + index * (sizeof(TUnionMessageHeader) + pgpdfzMsgBufHDL->maxSizeOfMsg) + sizeof(*pheader));
		if (pheader->len > 0)
		{
			if (pheader->len >= sizeof(buf))
				logLen = sizeof(buf) - 1;
			else
				logLen = pheader->len;
			memcpy(buf,pmsg,logLen);
			buf[logLen] = 0;
		}
		else
			logLen = 0;
		if ((ret = UnionRepairSpecMsgBufIndex(index)) >= 0)
		{
			if ((offset = UnionReadIntTypeRECVar("offsetOfMsgInMsgBuf")) < 0)
				offset = 0;
			if (logLen - offset > 0)
				UnionAuditLog("repair:: [%04d] [%012ld] [%07d] [%04ld] [%04d]\n[%s]\n",
						index,header.type,header.provider,nowTime - header.time,header.len,
						buf+offset);
			else
				UnionAuditLog("repair:: [%04d] [%012ld] [%07d] [%04ld] [%04d]\n",
						index,header.type,header.provider,nowTime - header.time,header.len);
			num++;
			continue;
		}
		UnionUserErrLog("in UnionRepairAllMsgBufIndex:: repair type = [%012ld] index = [%04d] message failure!\n",header.type,index);
	}
	if (num > 0)
		UnionAuditLog("in UnionRepairAllMsgBufIndex:: [%04d] are repaired\n",num);
	return(0);
}

// 2007/04/12 修改
int UnionResetMsgIndexOfSpecTypeAvailable(long type,int index)
{
	TUnionOccupiedPosOfMsgBuf	occupiedPos;
	int				ret;
	int				tmpIndex;
	int				pos[maxSize+1];
	long				typeGrp[maxSize+1];
	int				num = 0;
	int				isReset = 0;
	
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionResetMsgIndexOfSpecTypeAvailable:: UnionConnectMsgBufMDL!\n");
		return(ret);
	}
	
	for (;;)
	{
		memset(occupiedPos.indexOfPos,0,sizeof(occupiedPos.indexOfPos));
		if ((ret = msgrcv(pgpdfzMsgBufHDL->queueIDOfOccupiedPos,&occupiedPos,sizeof(occupiedPos.indexOfPos),type,IPC_NOWAIT)) < 0)
			break;
		memcpy(&tmpIndex,occupiedPos.indexOfPos,sizeof(int));
		if (tmpIndex == index)
		{
			if (!isReset)
			{
				UnionFreeMsgBufPos(index);
				isReset = 1;
			}
			continue;
		}
		if (num >= sizeof(pos))
			continue;
		pos[num] = tmpIndex;
		typeGrp[num] = occupiedPos.typeOfMsg;
		num++;
	}
	for (tmpIndex = 0; tmpIndex < num; tmpIndex++)
		UnionOccupyMsgBufPos(pos[tmpIndex],typeGrp[tmpIndex]);
	if (isReset)
		return(0);
	return(errCodeMsgBufMDL_MsgOfTypeAndOfIndexNotExists);
}

// wangk add 2009-9-24
int UnionPrintMsgBufStatusInRecStrFormatToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusInRecStrFormatToSpecFile:: NULL poionter!\n");
		return(errCodeParameter);
	}

	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintMsgBufStatusInRecStrFormatToSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	ret = UnionPrintMsgBufStatusInRecStrFormatToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

int UnionPrintMsgBufStatusInRecStrFormatToFile(FILE *fp)
{
	int	ret = 0;
	char recStr[1024];
	int sizeOfBuf = 0;
	int offSet = 0;

	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusInRecStrFormatToFile:: NULL poionter!\n");
		return(errCodeParameter);
	}

	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusInRecStrFormatToFile:: UnionConnectMsgBufMDL!\n");
		return(ret);
	}

	sizeOfBuf = sizeof(recStr);

	memset(recStr, 0, sizeof(recStr));
	offSet = 0;
	ret = UnionPutRecFldIntoRecStr("propertyName", "userID", strlen("userID"), recStr+offSet, sizeOfBuf-offSet);
	if( ret < 0 )
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusInRecStrFormatToFile:: UnionPutRecFldIntoRecStr [userID] error!\n");
		return (ret);
	}
	offSet += ret;

	ret = UnionPutIntTypeRecFldIntoRecStr("propertyValue", pgpdfzMsgBufHDL->userID, recStr+offSet, sizeOfBuf-offSet);
	if( ret < 0 )
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusInRecStrFormatToFile:: UnionPutIntTypeRecFldIntoRecStr [userID] error!\n");
		return (ret);
	}
	offSet += ret;
	fprintf(fp, "%s\n", recStr);

	memset(recStr, 0, sizeof(recStr));
	offSet = 0;
	ret = UnionPutRecFldIntoRecStr("propertyName", "maxSizeOfMsg", strlen("maxSizeOfMsg"), recStr+offSet, sizeOfBuf-offSet);
	if( ret < 0 )
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusInRecStrFormatToFile:: UnionPutRecFldIntoRecStr [maxSizeOfMsg] error!\n");
		return (ret);
	}
	offSet += ret;

	ret = UnionPutIntTypeRecFldIntoRecStr("propertyValue", pgpdfzMsgBufHDL->maxSizeOfMsg, recStr+offSet, sizeOfBuf-offSet);
	if( ret < 0 )
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusInRecStrFormatToFile:: UnionPutIntTypeRecFldIntoRecStr [maxSizeOfMsg] error!\n");
		return (ret);
	}
	offSet += ret;
	fprintf(fp, "%s\n", recStr);

	memset(recStr, 0, sizeof(recStr));
	offSet = 0;
	ret = UnionPutRecFldIntoRecStr("propertyName", "maxNumOfMsgPos", strlen("maxNumOfMsgPos"), recStr+offSet, sizeOfBuf-offSet);
	if( ret < 0 )
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusInRecStrFormatToFile:: UnionPutRecFldIntoRecStr [maxNumOfMsgPos] error!\n");
		return (ret);
	}
	offSet += ret;

	ret = UnionPutIntTypeRecFldIntoRecStr("propertyValue", pgpdfzMsgBufHDL->maxNumOfMsg, recStr+offSet, sizeOfBuf-offSet);
	if( ret < 0 )
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusInRecStrFormatToFile:: UnionPutIntTypeRecFldIntoRecStr [maxNumOfMsgPos] error!\n");
		return (ret);
	}
	offSet += ret;
	fprintf(fp, "%s\n", recStr);

	memset(recStr, 0, sizeof(recStr));
	offSet = 0;
	ret = UnionPutRecFldIntoRecStr("propertyName", "maxStayTime", strlen("maxStayTime"), recStr+offSet, sizeOfBuf-offSet);
	if( ret < 0 )
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusInRecStrFormatToFile:: UnionPutRecFldIntoRecStr [maxStayTime] error!\n");
		return (ret);
	}
	offSet += ret;

	ret = UnionPutIntTypeRecFldIntoRecStr("propertyValue", pgpdfzMsgBufHDL->maxStayTime, recStr+offSet, sizeOfBuf-offSet);
	if( ret < 0 )
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusInRecStrFormatToFile:: UnionPutIntTypeRecFldIntoRecStr [maxStayTime] error!\n");
		return (ret);
	}
	offSet += ret;
	fprintf(fp, "%s\n", recStr);

	memset(recStr, 0, sizeof(recStr));
	offSet = 0;
	ret = UnionPutRecFldIntoRecStr("propertyName", "queueIDOfFreePos", strlen("queueIDOfFreePos"), recStr+offSet, sizeOfBuf-offSet);
	if( ret < 0 )
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusInRecStrFormatToFile:: UnionPutRecFldIntoRecStr [queueIDOfFreePos] error!\n");
		return (ret);
	}
	offSet += ret;

	ret = UnionPutIntTypeRecFldIntoRecStr("propertyValue", pgpdfzMsgBufHDL->queueIDOfFreePos, recStr+offSet, sizeOfBuf-offSet);
	if( ret < 0 )
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusInRecStrFormatToFile:: UnionPutIntTypeRecFldIntoRecStr [queueIDOfFreePos] error!\n");
		return (ret);
	}
	offSet += ret;
	fprintf(fp, "%s\n", recStr);

	memset(recStr, 0, sizeof(recStr));
	offSet = 0;
	ret = UnionPutRecFldIntoRecStr("propertyName", "queueIDOfOccupiedPos", strlen("queueIDOfOccupiedPos"), recStr+offSet, sizeOfBuf-offSet);
	if( ret < 0 )
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusInRecStrFormatToFile:: UnionPutRecFldIntoRecStr [queueIDOfOccupiedPos] error!\n");
		return (ret);
	}
	offSet += ret;

	ret = UnionPutIntTypeRecFldIntoRecStr("propertyValue", pgpdfzMsgBufHDL->queueIDOfOccupiedPos, recStr+offSet, sizeOfBuf-offSet);
	if( ret < 0 )
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusInRecStrFormatToFile:: UnionPutIntTypeRecFldIntoRecStr [queueIDOfOccupiedPos] error!\n");
		return (ret);
	}
	offSet += ret;
	fprintf(fp, "%s\n", recStr);

	memset(recStr, 0, sizeof(recStr));
	offSet = 0;
	ret = UnionPutRecFldIntoRecStr("propertyName", "Num Of Free Pos", strlen("Num Of Free Pos"), recStr+offSet, sizeOfBuf-offSet);
	if( ret < 0 )
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusInRecStrFormatToFile:: UnionPutRecFldIntoRecStr [Num Of Free Pos] error!\n");
		return (ret);
	}
	offSet += ret;

	ret = UnionPutIntTypeRecFldIntoRecStr("propertyValue", UnionGetNumOfFreeMsgBufPos(), recStr+offSet, sizeOfBuf-offSet);
	if( ret < 0 )
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusInRecStrFormatToFile:: UnionPutIntTypeRecFldIntoRecStr [Num Of Free Pos] error!\n");
		return (ret);
	}
	offSet += ret;
	fprintf(fp, "%s\n", recStr);

	memset(recStr, 0, sizeof(recStr));
	offSet = 0;
	ret = UnionPutRecFldIntoRecStr("propertyName", "Num Of Occupied Pos", strlen("Num Of Occupied Pos"), recStr+offSet, sizeOfBuf-offSet);
	if( ret < 0 )
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusInRecStrFormatToFile:: UnionPutRecFldIntoRecStr [Num Of Free Pos] error!\n");
		return (ret);
	}
	offSet += ret;

	ret = UnionPutIntTypeRecFldIntoRecStr("propertyValue", UnionGetNumOfOccupiedMsgBufPos(), recStr+offSet, sizeOfBuf-offSet);
	if( ret < 0 )
	{
		UnionUserErrLog("in UnionPrintMsgBufStatusInRecStrFormatToFile:: UnionPutIntTypeRecFldIntoRecStr [Num Of Occupied Pos] error!\n");
		return (ret);
	}
	offSet += ret;
	fprintf(fp, "%s\n", recStr);

	return(0);
}
// wangk add end 2009-9-24

// add by wangk 2009-12-09
int UnionPrintAvailabeMsgBufPosInRecStrToFile(FILE *fp)
{
	TUnionFreePosOfMsgBuf		freePos;
	int				ret;
	int				tmpIndex;
	int				num = 0;
	int				pos[maxSize+1];
	int				existPos[maxSize+1];
	int				i;
	
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAvailabeMsgBufPosInRecStrToFile:: UnionConnectMsgBufMDL!\n");
		return(ret);
	}
	
	for (i = 0; i < maxSize; i++)
		existPos[i] = 0;
	for (;;)
	{
		memset(freePos.indexOfPos,0,sizeof(freePos.indexOfPos));
		if ((ret = msgrcv(pgpdfzMsgBufHDL->queueIDOfFreePos,&freePos,sizeof(freePos.indexOfPos),0,IPC_NOWAIT)) < 0)
			break;
		memcpy(&tmpIndex,freePos.indexOfPos,sizeof(int));
		if (num >= sizeof(pos))
			continue;
		if ((tmpIndex >= maxSize) || (tmpIndex < 0))
			continue;
		existPos[tmpIndex] = 1;
		pos[num] = tmpIndex;
		num++;
	}
	for (tmpIndex = 0; tmpIndex < num; tmpIndex++)
		UnionFreeMsgBufPos(pos[tmpIndex]);
	for (i = 0; i < maxSize; i++)
	{
		if (!existPos[i])
			continue;
		fprintf(fp,"index=%d|\n",i);
	}

	return(num);
}

int UnionPrintAvailabeMsgBufPosInRecStrFormatToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
	{
		UnionUserErrLog("in UnionPrintAvailabeMsgBufPosInRecStrFormatToSpecFile:: NULL poionter!\n");
		return(errCodeParameter);
	}

	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintAvailabeMsgBufPosInRecStrFormatToSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	ret = UnionPrintAvailabeMsgBufPosInRecStrToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

int UnionPrintInavailabeMsgBufPosInRecStrToFile(FILE *fp)
{
	TUnionOccupiedPosOfMsgBuf	occupiedPos;
	int				ret;
	int				tmpIndex;
	int				num = 0;
	int				pos[maxSize+1];
	long				type[maxSize+1];
	int				existPos[maxSize+1];
	long				printType[maxSize+1];
	int				i;

	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionPrintInavailabeMsgBufPosInRecStrToFile:: UnionConnectMsgBufMDL!\n");
		return(ret);
	}

	for (i = 0; i < maxSize; i++)
		existPos[i] = 0;
	for (;;)
	{
		memset(occupiedPos.indexOfPos,0,sizeof(occupiedPos.indexOfPos));
		if ((ret = msgrcv(pgpdfzMsgBufHDL->queueIDOfOccupiedPos,&occupiedPos,sizeof(occupiedPos.indexOfPos),0,IPC_NOWAIT)) < 0)
			break;
		memcpy(&tmpIndex,occupiedPos.indexOfPos,sizeof(int));
		if (num >= sizeof(pos))
			break;
		if ((tmpIndex >= maxSize) || (tmpIndex < 0))
			continue;
		existPos[tmpIndex] = 1;
		printType[tmpIndex] = occupiedPos.typeOfMsg;
		pos[num] = tmpIndex;
		type[num] = occupiedPos.typeOfMsg;
		num++;
	}
	for (tmpIndex = 0; tmpIndex < num; tmpIndex++)
		UnionOccupyMsgBufPos(pos[tmpIndex],type[tmpIndex]);
	for (i = 0; i < maxSize; i++)
	{
		if (!existPos[i])
			continue;
		fprintf(fp,"index=%d|printType=%ld|\n",i,printType[i]);
	}

	return(num);
}

int UnionPrintInavailablMsgBufPosInRecStrFormatToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
	{
		UnionUserErrLog("in UnionPrintInavailablMsgBufPosInRecStrFormatToSpecFile:: NULL poionter!\n");
		return(errCodeParameter);
	}

	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintInavailablMsgBufPosInRecStrFormatToSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	ret = UnionPrintInavailabeMsgBufPosInRecStrToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}
// add end wangk 2009-12-09
