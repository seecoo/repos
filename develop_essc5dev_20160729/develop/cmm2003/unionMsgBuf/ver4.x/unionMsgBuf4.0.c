//	Author:		Wolfgang Wang
//	Date:		2003/09/26
//	Version:	4.0

#define _UnionLogMDL_3_x_
#define _UnionEnv_3_x_

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "unionCommand.h"

#include "unionMsgBuf.h"

PUnionMsgBufHDL			pgunionMsgBufHDL = NULL;
unsigned char			*pgunionMsgBuf = NULL;
PUnionSharedMemoryModule	pgunionMsgBufMDL = NULL;

int UnionGetFileNameOfMsgBufDef(char *fileName)
{
	sprintf(fileName,"%s/unionMsgBuf.Def",getenv("UNIONETC"));
	return(0);
}

int UnionInitMsgBufDef(PUnionMsgBufDef pdef)
{
	int		ret;
	char		fileName[512];
	char		*p;

	if (pdef == NULL)
		return(-1);
			
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfMsgBufDef(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionInitMsgBufDef:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("maxNumOfMsg")) == NULL)
	{
		UnionUserErrLog("in UnionInitMsgBufDef:: UnionGetEnviVarByName for [%s]\n!","maxNumOfMsg");
		goto abnormalExit;
	}
	pdef->maxNumOfMsg = atoi(p);
	
	if ((p = UnionGetEnviVarByName("maxSizeOfMsg")) == NULL)
	{
		UnionUserErrLog("in UnionInitMsgBufDef:: UnionGetEnviVarByName for [%s]\n!","maxSizeOfMsg");
		goto abnormalExit;
	}
	pdef->maxSizeOfMsg = atol(p);
	
	if ((p = UnionGetEnviVarByName("msgNumExchangedPerSeconds")) == NULL)
	{
		UnionUserErrLog("in UnionInitMsgBufDef:: UnionGetEnviVarByName for [%s]\n!","msgNumExchangedPerSeconds");
		goto abnormalExit;
	}
	if ((pdef->msgNumExchangedPerSeconds = atol(p)) <= 0)
		pdef->msgNumExchangedPerSeconds = 1000;
	

	if ((p = UnionGetEnviVarByName("maxReadWaitTime")) == NULL)
	{
		UnionUserErrLog("in UnionInitMsgBufDef:: UnionGetEnviVarByName for [%s]\n!","maxReadWaitTime");
		goto abnormalExit;
	}
	pdef->maxReadWaitTime = atoi(p);
	
	if ((p = UnionGetEnviVarByName("maxWriteWaitTime")) == NULL)
	{
		UnionUserErrLog("in UnionInitMsgBufDef:: UnionGetEnviVarByName for [%s]\n!","maxWriteWaitTime");
		goto abnormalExit;
	}
	pdef->maxWriteWaitTime = atoi(p);
	
	if ((p = UnionGetEnviVarByName("maxStayTime")) == NULL)
	{
		UnionUserErrLog("in UnionInitMsgBufDef:: UnionGetEnviVarByName for [%s]\n!","maxStayTime");
		goto abnormalExit;
	}
	pdef->maxStayTime = atoi(p);
	
	if ((p = UnionGetEnviVarByName("maxMsgIndex")) == NULL)
	{
		UnionUserErrLog("in UnionInitMsgBufDef:: UnionGetEnviVarByName for [%s]\n!","maxMsgIndex");
		goto abnormalExit;
	}
	pdef->maxMsgIndex = atol(p);
	
	if ((p = UnionGetEnviVarByName("minMsgIndex")) == NULL)
	{
		UnionUserErrLog("in UnionInitMsgBufDef:: UnionGetEnviVarByName for [%s]\n!","minMsgIndex");
		goto abnormalExit;
	}
	pdef->minMsgIndex = atol(p);
	
	if ((p = UnionGetEnviVarByName("intervalWhenReadingFailure")) == NULL)
	{
		UnionUserErrLog("in UnionInitMsgBufDef:: UnionGetEnviVarByName for [%s]\n!","intervalWhenReadingFailure");
		goto abnormalExit;
	}
	pdef->intervalWhenReadingFailure = atol(p);
	
	if ((p = UnionGetEnviVarByName("intervalWhenWritingFailure")) == NULL)
	{
		UnionUserErrLog("in UnionInitMsgBufDef:: UnionGetEnviVarByName for [%s]\n!","intervalWhenWritingFailure");
		goto abnormalExit;
	}
	pdef->intervalWhenWritingFailure = atol(p);
	

	UnionClearEnvi();
	return(0);

abnormalExit:	
	UnionClearEnvi();
	return(-1);
}


int UnionConnectMsgBufMDL()
{
	PUnionSharedMemoryModule	pmdl;
	int				ret;
	TUnionMsgBufDef			msgBufDef;
	
	if (pgunionMsgBufHDL != NULL)	// 已经连接
		return(0);
	
	memset(&msgBufDef,0,sizeof(msgBufDef));
	if ((ret = UnionInitMsgBufDef(&msgBufDef)) < 0)
	{
		UnionUserErrLog("in UnionConnectMsgBufMDL:: UnionInitMsgBufDef!\n");
		return(ret);
	}
	
	if ((pgunionMsgBufMDL = UnionConnectSharedMemoryModule(conMDLNameOfMsgBuf,
			sizeof(*pgunionMsgBufHDL) + ((sizeof(TUnionMessageHeader) + sizeof(unsigned char) * msgBufDef.maxSizeOfMsg) * msgBufDef.maxNumOfMsg))) == NULL)
	{
		UnionUserErrLog("in UnionConnectMsgBufMDL:: UnionConnectSharedMemoryModule!\n");
		return(-1);
	}

	if ((pgunionMsgBufHDL = (PUnionMsgBufHDL)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionMsgBufMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectMsgBufMDL:: PUnionMsgBufHDL!\n");
		return(-1);
	}
	
	if ((pgunionMsgBufHDL->pmsgBuf = (unsigned char *)((unsigned char *)pgunionMsgBufHDL + sizeof(*pgunionMsgBufHDL))) == NULL)
	{
		UnionUserErrLog("in UnionConnectMsgBufMDL:: unsigned char *!\n");
		return(-1);
	}
	pgunionMsgBuf = pgunionMsgBufHDL->pmsgBuf;
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionMsgBufMDL))
	{
		pgunionMsgBufHDL->msgBufDef = msgBufDef;
		pgunionMsgBufHDL->currentMsgIndex = pgunionMsgBufHDL->msgBufDef.maxMsgIndex;
	}
	return(0);
}

	
int UnionDisconnectMsgBufMDL()
{
	pgunionMsgBuf = NULL;
	pgunionMsgBufHDL = NULL;
	return(UnionDisconnectShareModule(pgunionMsgBufMDL));
}

int UnionRemoveMsgBufMDL()
{
	pgunionMsgBuf = NULL;
	pgunionMsgBufHDL = NULL;
	pgunionMsgBufMDL = NULL;
	return(UnionRemoveSharedMemoryModule(conMDLNameOfUnionLogTBL));
}

int UnionReloadMsgBufDef()
{
	if (pgunionMsgBufHDL == NULL)
		return(-1);
	return(UnionInitMsgBufDef(&(pgunionMsgBufHDL->msgBufDef)));
	
}

long UnionWriteNewMessage(unsigned char * msg,int lenOfMsg,long typeOfMsg,PUnionMessageHeader precvMsgHeader)
{
	int			index;
	PUnionMessageHeader	pmsgHeader;
	time_t			startTime,nowTime;
	long			msgIndex;
		
	if ((pgunionMsgBufHDL == NULL) || (msg == NULL))
		return(-1);
	if (lenOfMsg > pgunionMsgBufHDL->msgBufDef.maxSizeOfMsg)
	{
		UnionUserErrLog("in UnionWriteNewMessage:: lenOfMsg [%d] > pgunionMsgBufHDL->msgBufDef.maxSizeOfMsg [%ld]\n",
				lenOfMsg,pgunionMsgBufHDL->msgBufDef.maxSizeOfMsg);
		return(-1);
	}
	if (typeOfMsg <= 0)
	{
		UnionUserErrLog("in UnionWriteNewMessage:: typeOfMsg [%ld] Error!\n",typeOfMsg);
		return(-1);
	}
	
	time(&startTime);
loopWrite:	
	for (index = 0; index < pgunionMsgBufHDL->msgBufDef.maxNumOfMsg; index++)
	{
		if ((pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index)) == NULL)
			return(-1);
		if (pmsgHeader->type > 0)
			continue;
		if (!UnionLockMessageHeader(pmsgHeader))
			continue;
		pmsgHeader->type = typeOfMsg;
		pmsgHeader->len = lenOfMsg;
		time(&(pmsgHeader->time));
		pmsgHeader->dealer = 0;
		pmsgHeader->provider = getpid();
		if ((pgunionMsgBufHDL->currentMsgIndex >= pgunionMsgBufHDL->msgBufDef.maxMsgIndex) || 
			(pgunionMsgBufHDL->currentMsgIndex <= 0))
			pgunionMsgBufHDL->currentMsgIndex = pgunionMsgBufHDL->msgBufDef.minMsgIndex;
		msgIndex = pmsgHeader->msgIndex = pgunionMsgBufHDL->currentMsgIndex++;
		memcpy(pgunionMsgBuf + conMsgSizeInMsgBuf * index + sizeof(TUnionMessageHeader),msg,lenOfMsg);
		memcpy(precvMsgHeader,pmsgHeader,sizeof(*pmsgHeader));
		UnionUnlockMessageHeader(pmsgHeader);
		return(msgIndex);
	}
	
	usleep(pgunionMsgBufHDL->msgBufDef.intervalWhenWritingFailure);
	time(&nowTime);
	if (nowTime - startTime >= pgunionMsgBufHDL->msgBufDef.maxWriteWaitTime)
	{
		//UnionUserErrLog("in UnionWriteNewMessage:: timeout!\n");
		return(-2);
	}
	goto loopWrite;
}
	
long UnionWriteNewMessageUntilSuccess(unsigned char * msg,int lenOfMsg,long typeOfMsg,PUnionMessageHeader pmsgHeader)
{
	long	msgIndex;
	
	for (;;)
	{
		if ((msgIndex = UnionWriteNewMessage(msg,lenOfMsg,typeOfMsg,pmsgHeader)) > 0)
			return(msgIndex);
		if (msgIndex == -2)
			continue;
		return(msgIndex);
	}
}
	
int UnionWriteOriginMessage(unsigned char *msg,int lenOfMsg,PUnionMessageHeader pOriMsgHeader)
{
	int			index;
	PUnionMessageHeader	pmsgHeader;
	time_t			startTime,nowTime;
	
	if ((pgunionMsgBufHDL == NULL) || (msg == NULL) || (pOriMsgHeader == NULL))
		return(-1);
	if (lenOfMsg > pgunionMsgBufHDL->msgBufDef.maxSizeOfMsg)
	{
		UnionUserErrLog("in UnionWriteOriginMessage:: lenOfMsg [%d] > pgunionMsgBufHDL->msgBufDef.maxSizeOfMsg [%ld]\n",
				lenOfMsg,pgunionMsgBufHDL->msgBufDef.maxSizeOfMsg);
		return(-1);
	}
	if (pOriMsgHeader->type <= 0)
	{
		UnionUserErrLog("in UnionWriteOriginMessage:: typeOfMsg [%ld] indexOfMsg = [%ld] lenOfData = [%d] provider = [%d] dealer = [%d] Error!\n",
			pOriMsgHeader->type,pOriMsgHeader->msgIndex,pOriMsgHeader->len,pOriMsgHeader->provider,pOriMsgHeader->dealer);
		return(-1);
	}
	if (kill(pOriMsgHeader->provider,0) != 0)
	{
		UnionUserErrLog("in UnionWriteOriginMessage:: the provider [%ld] not exists any more!\n",pOriMsgHeader->provider);
		return(-1);
	}
	
	time(&startTime);
loopWrite:	
	for (index = 0; index < pgunionMsgBufHDL->msgBufDef.maxNumOfMsg; index++)
	{
		if ((pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index)) == NULL)
			return(-1);
		if (pmsgHeader->type > 0)
			continue;
		if (!UnionLockMessageHeader(pmsgHeader))
			continue;
		pmsgHeader->type = pOriMsgHeader->type;
		pmsgHeader->len = lenOfMsg;
		time(&(pmsgHeader->time));
		pmsgHeader->dealer = getpid();
		pmsgHeader->msgIndex = pOriMsgHeader->msgIndex;
		pmsgHeader->provider = pOriMsgHeader->provider;
		memcpy(pgunionMsgBuf + conMsgSizeInMsgBuf * index + sizeof(TUnionMessageHeader),msg,lenOfMsg);
		UnionUnlockMessageHeader(pmsgHeader);
		return(0);
	}
	
	usleep(pgunionMsgBufHDL->msgBufDef.intervalWhenWritingFailure);
	time(&nowTime);
	if (nowTime - startTime >= pgunionMsgBufHDL->msgBufDef.maxWriteWaitTime)
	{
		return(-2);
	}
	goto loopWrite;
}

int UnionWriteOriginMessageUntilSuccess(unsigned char * msg,int lenOfMsg,PUnionMessageHeader pOriMsgHeader)
{
	int	ret;
	
	for (;;)
	{
		if ((ret = UnionWriteOriginMessage(msg,lenOfMsg,pOriMsgHeader)) >= 0)
			return(ret);
		if (ret == -2)
			continue;
		return(ret);
	}
}
	
int UnionReadMsgOfSpecifiedType(unsigned char *msg,int sizeOfMsg,long typeOfMsg,PUnionMessageHeader precvMsgHeader)
{
	int			index;
	int			len;
	time_t			startTime,nowTime;
	PUnionMessageHeader 	pmsgHeader;
	
	if ((pgunionMsgBufHDL == NULL) || (msg == NULL) || (precvMsgHeader == NULL))
		return(-1);

	if (typeOfMsg <= 0)
	{
		UnionUserErrLog("in UnionReadMsgOfSpecifiedType:: typeOfMsg [%ld] Error!\n",typeOfMsg);
		return(-1);
	}

	time(&startTime);
loopRead:	
	for (index = 0; index < pgunionMsgBufHDL->msgBufDef.maxNumOfMsg; index++)
	{
		if ((pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index)) == NULL)
			return(-1);
		if ((pmsgHeader->type != typeOfMsg)  || (pmsgHeader->dealer > 0) || (pmsgHeader->locked))
			continue;
		if (sizeOfMsg < (len = pmsgHeader->len))
		{
			UnionUserErrLog("in UnionReadMsgOfSpecifiedType:: sizeOfMsg [%d] < expected [%d]\n",sizeOfMsg,pmsgHeader->len);
			return(-1);
		}
		if (!UnionLockMessageHeader(pmsgHeader))
			continue;
		memcpy(msg,pgunionMsgBuf + conMsgSizeInMsgBuf * index + sizeof(TUnionMessageHeader),pmsgHeader->len);
		memcpy(precvMsgHeader,pmsgHeader,sizeof(*pmsgHeader));
		memset(pgunionMsgBuf + conMsgSizeInMsgBuf * index,0,conMsgSizeInMsgBuf);
		//UnionUnlockMessageHeader(pmsgHeader); // memset时已解锁
		return(len);
	}
	
	usleep(pgunionMsgBufHDL->msgBufDef.intervalWhenReadingFailure);
	time(&nowTime);
	if (nowTime - startTime >= pgunionMsgBufHDL->msgBufDef.maxReadWaitTime)
	{
		//UnionUserErrLog("in UnionReadMsgOfSpecifiedType:: timeout!\n");
		return(-2);
	}
	//UnionLog("in UnionReadMsgOfSpecifiedType:: waitTime = [%ld]\n",nowTime-startTime);
	
	goto loopRead;
}

int UnionReadMsgOfSpecifiedTypeUntilSuccess(unsigned char * msg,int sizeOfMsg,long typeOfMsg,PUnionMessageHeader pmsgHeader)
{
	int	len;
	
	for (;;)
	{
		if ((len = UnionReadMsgOfSpecifiedType(msg,sizeOfMsg,typeOfMsg,pmsgHeader)) >= 0)
			return(len);
		if (len == -2)
			continue;
		return(len);
	}
}

int UnionReadMsgOfSpecifiedMsgIndex(unsigned char *msg,int sizeOfMsg,long msgIndex,PUnionMessageHeader precvMsgHeader)
{
	int			index;
	int			len;
	time_t			startTime,nowTime;
	PUnionMessageHeader 	pmsgHeader;
	
	if ((pgunionMsgBufHDL == NULL) || (msg == NULL) || (precvMsgHeader == NULL))
		return(-1);

	if (msgIndex <= 0)
	{
		UnionUserErrLog("in UnionReadMsgOfSpecifiedMsgIndex:: msgIndex [%ld] Error!\n",msgIndex);
		return(-1);
	}

	time(&startTime);
loopRead:	
	for (index = 0; index < pgunionMsgBufHDL->msgBufDef.maxNumOfMsg; index++)
	{
		if ((pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index)) == NULL)
			return(-1);
		if ((pmsgHeader->type <= 0) || (pmsgHeader->msgIndex != msgIndex) || 
			(pmsgHeader->dealer <= 0) || (pmsgHeader->locked))
			continue;
		if (sizeOfMsg < (len = pmsgHeader->len))
		{
			UnionUserErrLog("in UnionReadMsgOfSpecifiedMsgIndex:: sizeOfMsg [%d] < expected [%d]\n",sizeOfMsg,pmsgHeader->len);
			return(-1);
		}
		if (!UnionLockMessageHeader(pmsgHeader))
			continue;
		memcpy(msg,pgunionMsgBuf + conMsgSizeInMsgBuf * index + sizeof(TUnionMessageHeader),pmsgHeader->len);
		memcpy(precvMsgHeader,pmsgHeader,sizeof(*pmsgHeader));
		memset(pgunionMsgBuf + conMsgSizeInMsgBuf * index,0,conMsgSizeInMsgBuf);	
		// memset已解锁，故不调用解锁函数
		return(len);
	}
	
	usleep( pgunionMsgBufHDL->msgBufDef.intervalWhenReadingFailure);
	time(&nowTime);
	if (nowTime - startTime >= pgunionMsgBufHDL->msgBufDef.maxReadWaitTime)
	{
		return(-2);
	}
	
	goto loopRead;
}

int UnionReadMsgOfSpecifiedMsgIndexUntilSuccess(unsigned char * msg,int sizeOfMsg,long msgIndex,PUnionMessageHeader pmsgHeader)
{
	int	len;
	
	for (;;)
	{
		if ((len = UnionReadMsgOfSpecifiedMsgIndex(msg,sizeOfMsg,msgIndex,pmsgHeader)) >= 0)
			return(len);
		if (len == -2)
			continue;
		return(len);
	}
}

int UnionReadFirstMsg(unsigned char * msg,int sizeOfMsg,long *typeOfMsg,PUnionMessageHeader precvMsgHeader)
{
	int			index;
	time_t			startTime,nowTime;
	int			len;
	PUnionMessageHeader 	pmsgHeader;
	
	if ((pgunionMsgBufHDL == NULL) || (msg == NULL) || (precvMsgHeader == NULL))
		return(-1);

	time(&startTime);
loopRead:	
	for (index = 0; index < pgunionMsgBufHDL->msgBufDef.maxNumOfMsg; index++)
	{
		if ((pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index)) == NULL)
			return(-1);
		if ((pmsgHeader->type <= 0) || (pmsgHeader->dealer > 0) || (pmsgHeader->locked))
			continue;
		if (sizeOfMsg < (len = pmsgHeader->len))
		{
			UnionUserErrLog("in UnionReadFirstMsg:: sizeOfMsg [%d] < expected [%d]\n",sizeOfMsg,pmsgHeader->len);
			return(-1);
		}
		if (!UnionLockMessageHeader(pmsgHeader))
			continue;
		*typeOfMsg = pmsgHeader->type;
		memcpy(msg,pgunionMsgBuf + conMsgSizeInMsgBuf * index + sizeof(TUnionMessageHeader),pmsgHeader->len);
		memcpy(precvMsgHeader,pmsgHeader,sizeof(*pmsgHeader));
		memset(pgunionMsgBuf + conMsgSizeInMsgBuf * index,0,conMsgSizeInMsgBuf);
		// memset已解锁，故不调用解锁函数
		return(len);
	}
	
	usleep( pgunionMsgBufHDL->msgBufDef.intervalWhenReadingFailure);
	time(&nowTime);
	if (nowTime - startTime >= pgunionMsgBufHDL->msgBufDef.maxReadWaitTime)
	{
		//UnionUserErrLog("in UnionReadFirstMsg:: timeout!\n");
		return(-2);
	}
	goto loopRead;
}

int UnionReadFirstMsgUntilSuccess(unsigned char * msg,int sizeOfMsg,long *typeOfMsg,PUnionMessageHeader pmsgHeader)
{
	int	len;
	
	for (;;)
	{
		if ((len = UnionReadFirstMsg(msg,sizeOfMsg,typeOfMsg,pmsgHeader)) >= 0)
			return(len);
		if (len == -2)
			continue;
		return(len);
	}
}

int UnionReadOriginMessage(unsigned char *msg,int sizeOfMsg,PUnionMessageHeader pOriMsgHeader)
{
	int			index;
	int			len;
	time_t			startTime,nowTime;
	PUnionMessageHeader 	pmsgHeader;
	
	if ((pgunionMsgBufHDL == NULL) || (msg == NULL) || (pOriMsgHeader == NULL))
	{
		UnionUserErrLog("in UnionReadOriginMessage:: wrong parameters!\n");
		return(-1);
	}

	if (pOriMsgHeader->msgIndex <= 0)
	{
		UnionUserErrLog("in UnionReadOriginMessage:: msgIndex [%ld] Error!\n",pOriMsgHeader->msgIndex);
		return(-1);
	}

	time(&startTime);
loopRead:	
	for (index = 0; index < pgunionMsgBufHDL->msgBufDef.maxNumOfMsg; index++)
	{
		if ((pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index)) == NULL)
			return(-1);
		if ((pmsgHeader->type <= 0) || (pmsgHeader->dealer <= 0) || (pmsgHeader->locked))
			continue;
		if ((pmsgHeader->msgIndex != pOriMsgHeader->msgIndex) || (pmsgHeader->provider != pOriMsgHeader->provider))
			continue;
		if (sizeOfMsg < (len = pmsgHeader->len))
		{
			UnionUserErrLog("in UnionReadOriginMessage:: sizeOfMsg [%d] < expected [%d]\n",sizeOfMsg,pmsgHeader->len);
			return(-1);
		}
		if (!UnionLockMessageHeader(pmsgHeader))
			continue;
		memcpy(msg,pgunionMsgBuf + conMsgSizeInMsgBuf * index + sizeof(TUnionMessageHeader),pmsgHeader->len);
		memcpy(pOriMsgHeader,pmsgHeader,sizeof(*pmsgHeader));
		memset(pgunionMsgBuf + conMsgSizeInMsgBuf * index,0,conMsgSizeInMsgBuf);
		// memset已解锁，故不调用解锁函数
		return(len);
	}
	
	usleep(pgunionMsgBufHDL->msgBufDef.intervalWhenReadingFailure);
	time(&nowTime);
	if (nowTime - startTime >= pgunionMsgBufHDL->msgBufDef.maxReadWaitTime)
		return(-2);
	goto loopRead;
}

int UnionReadOriginMessageUntilSuccess(unsigned char * msg,int sizeOfMsg,PUnionMessageHeader pmsgHeader)
{
	int	len;
	
	for (;;)
	{
		if ((len = UnionReadOriginMessage(msg,sizeOfMsg,pmsgHeader)) >= 0)
			return(len);
		if (len == -2)
			continue;
		return(len);
	}
}

int UnionPrintAvailablMsgBufPosToFile(FILE *fp)
{
	int			index;
	PUnionMessageHeader	pmsgHeader;
	int			num;
	
	if ((pgunionMsgBufHDL == NULL) || (fp == NULL))
		return(-1);

	for (index = 0,num = 0; index < pgunionMsgBufHDL->msgBufDef.maxNumOfMsg; index++)
	{
		if ((pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index)) == NULL)
			return(-1);
		if ((pmsgHeader->type > 0) || (pmsgHeader->locked))
			continue;
		fprintf(fp,"%06d\n",index);
		num++;
		if ((num % 23 == 0) && (num != 0) && ((fp == stdout) || (fp == stderr)))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit ...")))
				break;
		}
	}
	fprintf(fp,"Total Available Num = [%d]\n",num);
	return(0);
}

int UnionPrintInavailabeMsgBufPosToFile(FILE *fp)
{
	int			index;
	PUnionMessageHeader	pmsgHeader;
	int			num;
	time_t			nowTime;
	
	if ((pgunionMsgBufHDL == NULL) || (fp == NULL))
		return(-1);

	fprintf(fp,"%06s  %12s %12s %06s %06s %12s %06s\n",
		" 位置 "," 消 息 类 型"," 消 息 标 识","提供者","处理者","  提供时间  "," 长度 ");
	time(&nowTime);
	for (index = 0,num = 0; index < pgunionMsgBufHDL->msgBufDef.maxNumOfMsg; index++)
	{
		if ((pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index)) == NULL)
			return(-1);
		if ((pmsgHeader->type <= 0) || (pmsgHeader->locked))
			continue;
		fprintf(fp,"%6d %12ld %12ld %6d %6d %12ld %6d\n",
			index,pmsgHeader->type,pmsgHeader->msgIndex,pmsgHeader->provider,pmsgHeader->dealer,nowTime-pmsgHeader->time,pmsgHeader->len);
		num++;
		if ((num % 22 == 0) && (num != 0) && ((fp == stdout) || (fp == stderr))) 
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit ...")))
				break;
		}
	}
	fprintf(fp,"Total Inavailable Num = [%d]\n",num);
	return(0);
}

int UnionPrintMsgBufStatusToFile(FILE *fp)
{
	if ((pgunionMsgBufHDL == NULL) || (fp == NULL))
		return(-1);

	fprintf(fp,"\n");
	fprintf(fp,"maxSizeOfMsg			[%ld]\n",pgunionMsgBufHDL->msgBufDef.maxSizeOfMsg);
	fprintf(fp,"maxNumOfMsg			[%d]\n",pgunionMsgBufHDL->msgBufDef.maxNumOfMsg);
	fprintf(fp,"msgNumExchangedPerSeconds	[%ld]\n",pgunionMsgBufHDL->msgBufDef.msgNumExchangedPerSeconds);
	fprintf(fp,"maxReadWaitTime			[%d]\n",pgunionMsgBufHDL->msgBufDef.maxReadWaitTime);
	fprintf(fp,"maxWriteWaitTime		[%d]\n",pgunionMsgBufHDL->msgBufDef.maxWriteWaitTime);
	fprintf(fp,"maxStayTime			[%d]\n",pgunionMsgBufHDL->msgBufDef.maxStayTime);
	fprintf(fp,"maxMsgIndex			[%d]\n",pgunionMsgBufHDL->msgBufDef.maxMsgIndex);
	fprintf(fp,"minMsgIndex			[%d]\n",pgunionMsgBufHDL->msgBufDef.minMsgIndex);
	fprintf(fp,"intervalWhenWritingFailure	[%ld]\n",pgunionMsgBufHDL->msgBufDef.intervalWhenWritingFailure);
	fprintf(fp,"intervalWhenReadingFailure	[%ld]\n",pgunionMsgBufHDL->msgBufDef.intervalWhenReadingFailure);
	fprintf(fp,"currentMsgIndex		[%ld]\n",pgunionMsgBufHDL->currentMsgIndex);
	fprintf(fp,"\n");
	if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or quit/exit to exit...")))
		return(0);
	UnionPrintSharedMemoryModuleToFile(pgunionMsgBufMDL,fp);
	return(0);
}

int UnionPrintMsgBufToFile(FILE *fp)
{
	UnionPrintAvailablMsgBufPosToFile(fp);
	UnionPrintInavailabeMsgBufPosToFile(fp);
	UnionPrintMsgBufStatusToFile(fp);
	return(0);
}

int UnionFreeRubbishMsg()
{
	int			index;
	PUnionMessageHeader	pmsgHeader;
	int			num;
	time_t			nowTime;
	
	if (pgunionMsgBufHDL == NULL)
		return(-1);

	time(&nowTime);
	for (index = 0,num = 0; index < pgunionMsgBufHDL->msgBufDef.maxNumOfMsg; index++)
	{
		if ((pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index)) == NULL)
			return(-1);
		if (pmsgHeader->type <= 0)
			continue;
		if (nowTime - pmsgHeader->time < pgunionMsgBufHDL->msgBufDef.maxStayTime)
			continue;
		memset(pgunionMsgBuf + conMsgSizeInMsgBuf * index,0,conMsgSizeInMsgBuf);
		num++;
		UnionAuditLog("in UnionFreeRubbishMsg:: Free Rubbish [%d] [%ld] [%d] [%d] [%ld]\n",
				index,pmsgHeader->type,pmsgHeader->len,pmsgHeader->provider,pmsgHeader->time);
	}
	if (num > 0)
		UnionAuditLog("in UnionFreeRubbishMsg:: [%d] Rubbish are cleared\n",num);
	return(0);
}

int UnionPrintMessageHeaderToFile(PUnionMessageHeader pmsgHeader,FILE *fp)
{
	time_t	nowTime;
	
	if ((pmsgHeader == NULL) || (fp == NULL))
		return(-1);
	
	time(&nowTime);
	fprintf(fp,"\ntype		[%ld]\n",pmsgHeader->type);
	fprintf(fp,"provider	[%ld]\n",pmsgHeader->provider);
	fprintf(fp,"len		[%d]\n",pmsgHeader->len);
	fprintf(fp,"time		[%d]\n",nowTime-pmsgHeader->time);
	fprintf(fp,"msgIndex	[%ld]\n",pmsgHeader->msgIndex);
	fprintf(fp,"dealer		[%d]\n",pmsgHeader->dealer);
	fprintf(fp,"locked		[%d]\n\n",pmsgHeader->locked);	
	return(0);
}

int UnionPrintStatusOfMsgPosToFile(long index,FILE *fp)
{
	if ((fp == NULL) || (pgunionMsgBufHDL == NULL))
		return(-1);
	if (index >= pgunionMsgBufHDL->msgBufDef.maxNumOfMsg)
		return(-1);
	return(UnionPrintMessageHeaderToFile((PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index),fp));
}
	
int UnionLockMessageHeader(PUnionMessageHeader pmsgHeader)
{
	if (pmsgHeader == NULL)
		return(0);
	else
		if (pmsgHeader->locked == 0)
			return(pmsgHeader->locked = 1);
		else
			return(0);
}

int UnionUnlockMessageHeader(PUnionMessageHeader pmsgHeader)
{
	if (pmsgHeader == NULL)
		return(0);
	else
		return(pmsgHeader->locked = 0);
}
