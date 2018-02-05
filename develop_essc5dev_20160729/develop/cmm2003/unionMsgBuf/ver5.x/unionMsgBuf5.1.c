//	Author:		Wolfgang Wang
//	Date:		2003/11/04
//	Version:	5.0

//	5.1 2005/05/30，在5.0基础上升级
//	修改了	UnionApplyNewMsgIndexOfMsgBuf

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
#include "unionIndexTBL.h"
#include "unionMsgBuf.h"
#include "unionErrCode.h"

PUnionMsgBufHDL			pgunionMsgBufHDL = NULL;
unsigned char			*pgunionMsgBuf = NULL;
PUnionIndexStatusTBL		pgunionIndexStatusTBL = NULL;
PUnionSharedMemoryModule	pgunionMsgBufMDL = NULL;
long				gunionMsgBufIndex = 1;

int UnionIsMsgBufMDLConnected()
{
	if ((pgunionMsgBufHDL == NULL) || (pgunionMsgBuf == NULL) || (pgunionIndexStatusTBL == NULL) || (!UnionIsSharedMemoryInited(conMDLNameOfMsgBuf)))
		return(0);
	else
		return(1);
}

int UnionGetFileNameOfMsgBufDef(char *fileName)
{
	sprintf(fileName,"%s/unionMsgBuf.Def",getenv("UNIONETC"));
	return(0);
}

int UnionGetTotalNumOfMsgBufMDL()
{
	if (pgunionMsgBufHDL == NULL)
		return(errCodeMsgBufMDL_NotConnected);
	return(pgunionMsgBufHDL->msgBufDef.maxNumOfMsg);
}

int UnionGetFreePosNumOfMsgBufMDL()
{
	int			index;
	PUnionMessageHeader	pmsgHeader;
	int			num=0;
	
	if (pgunionMsgBuf == NULL)
		return(errCodeMsgBufMDL_NotConnected);

	for (index = 0,num = 0; index < pgunionMsgBufHDL->msgBufDef.maxNumOfMsg; index++)
	{
		if ((pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index)) == NULL)
			return(errCodeMsgBufMDL_OutofRange);
		if (pmsgHeader->type <= 0)
			num++;
	}
	return(num);
}

int UnionInitMsgBufDef(PUnionMsgBufDef pdef)
{
	int		ret;
	char		fileName[512];
	char		*p;

	if (pdef == NULL)
		return(errCodeParameter);
			
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
	return(errCodeMsgBufMDL_InitDef);
}


int UnionConnectMsgBufMDL()
{
	PUnionSharedMemoryModule	pmdl;
	int				ret;
	TUnionMsgBufDef			msgBufDef;
	
	if (UnionIsMsgBufMDLConnected())
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
		return(errCodeSharedMemoryModule);
	}

	if ((pgunionMsgBufHDL = (PUnionMsgBufHDL)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionMsgBufMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectMsgBufMDL:: PUnionMsgBufHDL!\n");
		return(errCodeSharedMemoryModule);
	}
	
	if ((pgunionMsgBufHDL->pmsgBuf = (unsigned char *)((unsigned char *)pgunionMsgBufHDL + sizeof(*pgunionMsgBufHDL))) == NULL)
	{
		UnionUserErrLog("in UnionConnectMsgBufMDL:: unsigned char *!\n");
		return(errCodeSharedMemoryModule);
	}
	pgunionMsgBuf = pgunionMsgBufHDL->pmsgBuf;
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionMsgBufMDL))
	{
		pgunionMsgBufHDL->msgBufDef = msgBufDef;
		pgunionMsgBufHDL->currentMsgIndex = pgunionMsgBufHDL->msgBufDef.minMsgIndex;
	}
	
	if ((pgunionIndexStatusTBL = UnionConnectIndexStatusTBL(UnionGetUserIDOfSharedMemoryModule(pgunionMsgBufMDL),
		pgunionMsgBufHDL->msgBufDef.maxNumOfMsg)) == NULL)
	{
		UnionUserErrLog("in UnionConnectMsgBufMDL:: UnionConnectIndexStatusTBL!\n");
		return(errCodeMsgBufMDL_ConnectIndexTBL);
	}
	
	return(0);
}

	
int UnionDisconnectMsgBufMDL()
{
	pgunionMsgBuf = NULL;
	pgunionMsgBufHDL = NULL;
	UnionDisconnectIndexStatusTBL(pgunionIndexStatusTBL);
	return(UnionDisconnectShareModule(pgunionMsgBufMDL));
}

int UnionRemoveMsgBufMDL()
{
	pgunionMsgBuf = NULL;
	pgunionMsgBufHDL = NULL;
	if (pgunionMsgBufMDL != NULL)
	{
		UnionRemoveIndexStatusTBL(UnionGetUserIDOfSharedMemoryModule(pgunionMsgBufMDL));
	}
	pgunionMsgBufMDL = NULL;
	return(UnionRemoveSharedMemoryModule(conMDLNameOfUnionLogTBL));
}

int UnionReloadMsgBufDef()
{
	int		ret;
	TUnionMsgBufDef	msgBufDef;
	
	if ((pgunionMsgBufHDL == NULL) || (pgunionIndexStatusTBL == NULL))
		return(errCodeMsgBufMDL_NotConnected);
	memset(&msgBufDef,0,sizeof(&msgBufDef));
	if ((ret = UnionInitMsgBufDef(&msgBufDef)) < 0)
	{
		UnionUserErrLog("in UnionReloadMsgBufDef:: UnionInitMsgBufDef!\n");
		return(ret);
	}
	if ((sizeof(TUnionMessageHeader) + sizeof(unsigned char) * msgBufDef.maxSizeOfMsg) * msgBufDef.maxNumOfMsg
		> 
	    (sizeof(TUnionMessageHeader) + sizeof(unsigned char) * pgunionMsgBufHDL->msgBufDef.maxSizeOfMsg) * 
	    pgunionMsgBufHDL->msgBufDef.maxNumOfMsg)
	{
		UnionUserErrLog("in UnionReloadMsgBufDef:: parameter error!\n");
		return(ret);
	}
	if ((ret = UnionResetMaxIndexNumOfIndexStatusTBL(pgunionIndexStatusTBL,msgBufDef.maxNumOfMsg)) < 0)
	{
		UnionUserErrLog("in UnionReloadMsgBufDef:: UnionResetMaxIndexNumOfIndexStatusTBL!\n");
		return(ret);
	}
	pgunionMsgBufHDL->msgBufDef = msgBufDef;
	return(0);
}

long UnionApplyNewMsgIndexOfMsgBuf()
{
#ifndef _selfMsgIndex_	
	if ((pgunionMsgBufHDL->currentMsgIndex >= pgunionMsgBufHDL->msgBufDef.maxMsgIndex) || 
			(pgunionMsgBufHDL->currentMsgIndex <= 0) || 
			(pgunionMsgBufHDL->currentMsgIndex < pgunionMsgBufHDL->msgBufDef.minMsgIndex))
		pgunionMsgBufHDL->currentMsgIndex = pgunionMsgBufHDL->msgBufDef.minMsgIndex;
	return((++(pgunionMsgBufHDL->currentMsgIndex)) * 100000);
#else
	if ((gunionMsgBufIndex >= 9999) || (gunionMsgBufIndex <= 0))
		gunionMsgBufIndex = 1;
	return((++gunionMsgBufIndex)*100000);
#endif
}
	
long UnionWriteNewMessage(unsigned char * msg,int lenOfMsg,long typeOfMsg,PUnionMessageHeader precvMsgHeader)
{
	int			index;
	PUnionMessageHeader	pmsgHeader;
	long			msgIndex;
		
	if ((pgunionMsgBufHDL == NULL) || (msg == NULL) || (lenOfMsg > pgunionMsgBufHDL->msgBufDef.maxSizeOfMsg) || (typeOfMsg <= 0) || (lenOfMsg <= 0))
	{
		UnionUserErrLog("in UnionWriteNewMessage:: wrong parameter!\n");
		return(errCodeParameter);
	}
	if (((index = UnionGetAvailableIndex(pgunionIndexStatusTBL)) < 0) || (index >= pgunionMsgBufHDL->msgBufDef.maxNumOfMsg))
	{
		UnionUserErrLog("in UnionWriteNewMessage:: UnionGetAvailableIndex!\n");
		return(index);
	}
	pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index);
	pmsgHeader->locked = 1;
	pmsgHeader->type = typeOfMsg;
	pmsgHeader->len = lenOfMsg;
	time(&(pmsgHeader->time));
	pmsgHeader->dealer = 0;
	pmsgHeader->provider = getpid();
	pmsgHeader->msgIndex = UnionApplyNewMsgIndexOfMsgBuf();
	memcpy(pgunionMsgBuf + conMsgSizeInMsgBuf * index + sizeof(TUnionMessageHeader),msg,lenOfMsg);
	memcpy(precvMsgHeader,pmsgHeader,sizeof(*pmsgHeader));
	pmsgHeader->locked = 0;
	return(UnionSetIndexWithUserStatus(pgunionIndexStatusTBL,index,pmsgHeader->type+pgunionMsgBufHDL->msgBufDef.maxMsgIndex));
}
	
int UnionWriteOriginMessage(unsigned char *msg,int lenOfMsg,PUnionMessageHeader pOriMsgHeader)
{
	int			index;
	PUnionMessageHeader	pmsgHeader;
	
	if ((pgunionMsgBufHDL == NULL) || (msg == NULL) || (pOriMsgHeader == NULL) 
	    || (lenOfMsg > pgunionMsgBufHDL->msgBufDef.maxSizeOfMsg) || (pOriMsgHeader->type <= 0) || (lenOfMsg <= 0))
	{
		UnionUserErrLog("in UnionWriteOriginMessage:: wrong parameter! lenOfMsg = [%d]\n",lenOfMsg);
		return(errCodeParameter);
	}
#ifndef _IgnoreProvider_
	if (kill(pOriMsgHeader->provider,0) != 0)
	{
		UnionUserErrLog("in UnionWriteOriginMessage:: the provider [%ld] not exists any more!\n",pOriMsgHeader->provider);
		return(errCodeMsgBufMDL_ProviderExit);
	}
#endif
	if (((index = UnionGetAvailableIndex(pgunionIndexStatusTBL)) < 0) || (index >= pgunionMsgBufHDL->msgBufDef.maxNumOfMsg))
	{
		UnionUserErrLog("in UnionWriteOriginMessage:: UnionGetAvailableIndex!\n");
		return(index);
	}
	
	pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index);
	pmsgHeader->locked = 1;
	pmsgHeader->type = pOriMsgHeader->type;
	pmsgHeader->len = lenOfMsg;
	time(&(pmsgHeader->time));
	pmsgHeader->dealer = getpid();
	pmsgHeader->msgIndex = pOriMsgHeader->msgIndex;
	pmsgHeader->provider = pOriMsgHeader->provider;
	memcpy(pgunionMsgBuf + conMsgSizeInMsgBuf * index + sizeof(TUnionMessageHeader),msg,lenOfMsg);
	pmsgHeader->locked = 0;

	return(UnionSetIndexWithUserStatus(pgunionIndexStatusTBL,index,pmsgHeader->provider+pmsgHeader->msgIndex));
}

int UnionReadMsgOfSpecifiedType(unsigned char *msg,int sizeOfMsg,long typeOfMsg,PUnionMessageHeader precvMsgHeader)
{
	int			index;
	int			len;
	PUnionMessageHeader 	pmsgHeader;
	
	if ((pgunionMsgBuf == NULL) || (msg == NULL) || (precvMsgHeader == NULL) || (typeOfMsg <= 0))
	{
		UnionUserErrLog("in UnionReadMsgOfSpecifiedType:: wrong parameter!\n");
		return(errCodeParameter);
	}
loopRead:
	if (((index = UnionGetFirstIndexOfUserStatus(pgunionIndexStatusTBL,typeOfMsg+pgunionMsgBufHDL->msgBufDef.maxMsgIndex)) < 0) || (index >= pgunionMsgBufHDL->msgBufDef.maxNumOfMsg))
	{
		UnionUserErrLog("in UnionReadMsgOfSpecifiedType:: UnionGetFirstIndexOfUserStatus!\n");
		return(index);
	}
	pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index);
	if (sizeOfMsg < (len = pmsgHeader->len))
	{
		UnionUserErrLog("in UnionReadMsgOfSpecifiedType:: sizeOfMsg [%d] < expected [%d]\n",sizeOfMsg,pmsgHeader->len);
		UnionSetIndexAvailable(pgunionIndexStatusTBL,index);
		return(errCodeParameter);
	}
	if (len <= 0)
	{
		UnionUserErrLog("in UnionReadMsgOfSpecifiedType:: len = [%d]\n",len);
		UnionSetIndexAvailable(pgunionIndexStatusTBL,index);
		goto loopRead;
	}
	memcpy(msg,pgunionMsgBuf + conMsgSizeInMsgBuf * index + sizeof(TUnionMessageHeader),pmsgHeader->len);
	memcpy(precvMsgHeader,pmsgHeader,sizeof(*pmsgHeader));
	memset(pgunionMsgBuf + conMsgSizeInMsgBuf * index,0,conMsgSizeInMsgBuf);
	UnionSetIndexAvailable(pgunionIndexStatusTBL,index);

	return(len);
}

int UnionReadMsgOfSpecifiedTypeUntilSuccess(unsigned char *msg,int sizeOfMsg,long typeOfMsg,PUnionMessageHeader precvMsgHeader)
{
	int			index;
	int			len;
	PUnionMessageHeader 	pmsgHeader;
	
	if ((pgunionMsgBuf == NULL) || (msg == NULL) || (precvMsgHeader == NULL) || (typeOfMsg <= 0))
	{
		UnionUserErrLog("in UnionReadMsgOfSpecifiedTypeUntilSuccess:: wrong parameter!\n");
		return(errCodeParameter);
	}
loopRead:
	if (((index = UnionGetFirstIndexOfUserStatusUntilSuccess(pgunionIndexStatusTBL,typeOfMsg+pgunionMsgBufHDL->msgBufDef.maxMsgIndex)) < 0) || (index >= pgunionMsgBufHDL->msgBufDef.maxNumOfMsg))
	{
		UnionUserErrLog("in UnionReadMsgOfSpecifiedTypeUntilSuccess:: UnionGetFirstIndexOfUserStatusUntilSuccess!\n");
		return(index);
	}
	pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index);
	if (sizeOfMsg < (len = pmsgHeader->len))
	{
		UnionUserErrLog("in UnionReadMsgOfSpecifiedTypeUntilSuccess:: sizeOfMsg [%d] < expected [%d]\n",sizeOfMsg,pmsgHeader->len);
		UnionSetIndexAvailable(pgunionIndexStatusTBL,index);
		return(errCodeParameter);
	}
	if (len <= 0)
	{
		UnionUserErrLog("in UnionReadMsgOfSpecifiedTypeUntilSuccess:: len = [%d]\n",len);
		UnionSetIndexAvailable(pgunionIndexStatusTBL,index);
		goto loopRead;
	}
	memcpy(msg,pgunionMsgBuf + conMsgSizeInMsgBuf * index + sizeof(TUnionMessageHeader),pmsgHeader->len);
	memcpy(precvMsgHeader,pmsgHeader,sizeof(*pmsgHeader));
	memset(pgunionMsgBuf + conMsgSizeInMsgBuf * index,0,conMsgSizeInMsgBuf);
	UnionSetIndexAvailable(pgunionIndexStatusTBL,index);

	return(len);
}

int UnionReadOriginMessage(unsigned char *msg,int sizeOfMsg,PUnionMessageHeader pOriMsgHeader)
{
	int			index;
	int			len;
	PUnionMessageHeader 	pmsgHeader;
	
	if ((pgunionMsgBuf == NULL) || (msg == NULL) || (pOriMsgHeader == NULL))
	{
		UnionUserErrLog("in UnionReadOriginMessage:: wrong parameters!\n");
		return(errCodeParameter);
	}
loopRead:
	if (((index = UnionGetFirstIndexOfUserStatus(pgunionIndexStatusTBL,pOriMsgHeader->provider+pOriMsgHeader->msgIndex)) < 0) || (index >= pgunionMsgBufHDL->msgBufDef.maxNumOfMsg))
	{
		UnionUserErrLog("in UnionReadOriginMessage:: UnionGetFirstIndexOfUserStatus! [%ld] [%ld]\n",pOriMsgHeader->provider,pOriMsgHeader->msgIndex);
		return(index);
	}
	pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index);
	if ((pmsgHeader->msgIndex != pOriMsgHeader->msgIndex) || (pmsgHeader->type != pOriMsgHeader->type))
	{
		UnionUserErrLog("in UnionReadOriginMessage:: read msgIndex [%ld] type [%ld] not origin message [%ld] [%ld]!\n",
				pmsgHeader->msgIndex,pmsgHeader->type,pOriMsgHeader->msgIndex,pOriMsgHeader->type);
		UnionSetIndexAvailable(pgunionIndexStatusTBL,index);
		return(errCodeMsgBufMDL_NotOriginMsg);
	}
		
	if (sizeOfMsg < (len = pmsgHeader->len))
	{
		UnionUserErrLog("in UnionReadOriginMessage:: sizeOfMsg [%d] < expected [%d]\n",sizeOfMsg,pmsgHeader->len);
		UnionSetIndexAvailable(pgunionIndexStatusTBL,index);
		return(errCodeParameter);
	}
	if (len <= 0)
	{
		UnionUserErrLog("in UnionReadOriginMessage:: len = [%d]\n",len);
		UnionSetIndexAvailable(pgunionIndexStatusTBL,index);
		goto loopRead;
	}
	memcpy(msg,pgunionMsgBuf + conMsgSizeInMsgBuf * index + sizeof(TUnionMessageHeader),pmsgHeader->len);
	memcpy(pOriMsgHeader,pmsgHeader,sizeof(*pmsgHeader));
	memset(pgunionMsgBuf + conMsgSizeInMsgBuf * index,0,conMsgSizeInMsgBuf);

	UnionSetIndexAvailable(pgunionIndexStatusTBL,index);

	return(len);
}

int UnionPrintAvailablMsgBufPosToFile(FILE *fp)
{
	int			index;
	PUnionMessageHeader	pmsgHeader;
	int			num;
	
	if ((pgunionMsgBuf == NULL) || (fp == NULL))
		return(errCodeParameter);

	for (index = 0,num = 0; index < pgunionMsgBufHDL->msgBufDef.maxNumOfMsg; index++)
	{
		if ((pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index)) == NULL)
			return(errCodeMsgBufMDL_OutofRange);
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
	
	if ((pgunionMsgBuf == NULL) || (fp == NULL))
		return(errCodeParameter);

	fprintf(fp,"%06s  %12s %12s %06s %06s %12s %06s\n",
		" 位置 "," 消 息 类 型"," 消 息 标 识","提供者","处理者","  提供时间  "," 长度 ");
	time(&nowTime);
	for (index = 0,num = 0; index < pgunionMsgBufHDL->msgBufDef.maxNumOfMsg; index++)
	{
		if ((pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index)) == NULL)
			return(errCodeMsgBufMDL_OutofRange);
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
	if ((pgunionMsgBuf == NULL) || (fp == NULL))
		return(errCodeParameter);

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
	long			type;
	
	if (pgunionMsgBuf == NULL)
		return(errCodeParameter);

	time(&nowTime);
	for (index = 0,num = 0; index < pgunionMsgBufHDL->msgBufDef.maxNumOfMsg; index++)
	{
		if ((pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index)) == NULL)
			return(errCodeMsgBufMDL_OutofRange);
		if (pmsgHeader->type <= 0)
			continue;
		if (pmsgHeader->time <= 0)
			continue;
		if (nowTime - pmsgHeader->time < pgunionMsgBufHDL->msgBufDef.maxStayTime)
			continue;
		UnionAuditLog("in UnionFreeRubbishMsg:: Free Rubbish [%d] [%ld] [%d] [%d] [%ld]\n",
				index,pmsgHeader->type,pmsgHeader->len,pmsgHeader->provider,nowTime - pmsgHeader->time);
		type = pmsgHeader->type;
		memset(pgunionMsgBuf + conMsgSizeInMsgBuf * index,0,conMsgSizeInMsgBuf);
		UnionSetIndexOfUserStatusAndIndexAvailable(pgunionIndexStatusTBL,type,index);
		num++;
	}
	if (num > 0)
		UnionAuditLog("in UnionFreeRubbishMsg:: [%d] Rubbish are cleared\n",num);
	return(0);
}

int UnionPrintMessageHeaderToFile(PUnionMessageHeader pmsgHeader,FILE *fp)
{
	time_t	nowTime;
	
	if ((pmsgHeader == NULL) || (fp == NULL))
		return(errCodeParameter);
	
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
	if ((fp == NULL) || (pgunionMsgBuf == NULL))
		return(errCodeParameter);
	if (index >= pgunionMsgBufHDL->msgBufDef.maxNumOfMsg)
		return(errCodeParameter);
	return(UnionPrintMessageHeaderToFile((PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index),fp));
}

int UnionRebufferOriginMessage(unsigned char *msg,int lenOfMsg,PUnionMessageHeader pOriMsgHeader)
{
	int			index;
	PUnionMessageHeader	pmsgHeader;
	
	if ((pgunionMsgBufHDL == NULL) || (msg == NULL) || (pOriMsgHeader == NULL) 
	    || (lenOfMsg > pgunionMsgBufHDL->msgBufDef.maxSizeOfMsg) || (pOriMsgHeader->type <= 0))
	{
		UnionUserErrLog("in UnionRebufferOriginMessage:: wrong parameter!\n");
		return(errCodeParameter);
	}
#ifndef _IgnoreProvider_
	if (kill(pOriMsgHeader->provider,0) != 0)
	{
		UnionUserErrLog("in UnionRebufferOriginMessage:: the provider [%ld] not exists any more!\n",pOriMsgHeader->provider);
		return(errCodeMsgBufMDL_ProviderExit);
	}
#endif
	if (((index = UnionGetAvailableIndex(pgunionIndexStatusTBL)) < 0) || (index >= pgunionMsgBufHDL->msgBufDef.maxNumOfMsg))
	{
		UnionUserErrLog("in UnionRebufferOriginMessage:: UnionGetAvailableIndex!\n");
		return(index);
	}
	
	pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index);
	pmsgHeader->locked = 1;
	memcpy(pmsgHeader,pOriMsgHeader,sizeof(*pOriMsgHeader));
	memcpy(pgunionMsgBuf + conMsgSizeInMsgBuf * index + sizeof(TUnionMessageHeader),msg,lenOfMsg);
	pmsgHeader->locked = 0;

	return(UnionSetIndexWithUserStatus(pgunionIndexStatusTBL,index,pmsgHeader->provider+pmsgHeader->msgIndex));
}


int UnionReadFirstMsgOfTypeUntilSuccess(unsigned char *msg,int sizeOfMsg,long typeOfMsg)
{
	int			index;
	int			len;
	PUnionMessageHeader 	pmsgHeader;
	
	if ((pgunionMsgBuf == NULL) || (msg == NULL) || (typeOfMsg <= 0))
	{
		UnionUserErrLog("in UnionReadFirstMsgOfTypeUntilSuccess:: wrong parameter!\n");
		return(errCodeParameter);
	}
loopRead:
	if (((index = UnionGetFirstIndexOfUserStatusUntilSuccess(pgunionIndexStatusTBL,typeOfMsg)) < 0) || (index >= pgunionMsgBufHDL->msgBufDef.maxNumOfMsg))
	{
		UnionUserErrLog("in UnionReadFirstMsgOfTypeUntilSuccess:: UnionGetFirstIndexOfUserStatusUntilSuccess!\n");
		return(index);
	}
	pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index);
	if (sizeOfMsg < (len = pmsgHeader->len))
	{
		UnionUserErrLog("in UnionReadFirstMsgOfTypeUntilSuccess:: sizeOfMsg [%d] < expected [%d]\n",sizeOfMsg,pmsgHeader->len);
		UnionSetIndexAvailable(pgunionIndexStatusTBL,index);
		return(errCodeParameter);
	}
	if (len <= 0)
	{
		UnionUserErrLog("in UnionReadFirstMsgOfTypeUntilSuccess:: len = [%d]\n",len);
		UnionSetIndexAvailable(pgunionIndexStatusTBL,index);
		goto loopRead;
	}
	memcpy(msg,pgunionMsgBuf + conMsgSizeInMsgBuf * index + sizeof(TUnionMessageHeader),pmsgHeader->len);
	memset(pgunionMsgBuf + conMsgSizeInMsgBuf * index,0,conMsgSizeInMsgBuf);
	UnionSetIndexAvailable(pgunionIndexStatusTBL,index);

	return(len);
}

int UnionBufferMessage(unsigned char * msg,int lenOfMsg,long typeOfMsg)
{
	int			index;
	PUnionMessageHeader	pmsgHeader;
	long			msgIndex;
		
	if ((pgunionMsgBufHDL == NULL) || (msg == NULL) || (lenOfMsg > pgunionMsgBufHDL->msgBufDef.maxSizeOfMsg) || (typeOfMsg <= 0) || (lenOfMsg <= 0))
	{
		UnionUserErrLog("in UnionBufferMessage:: wrong parameter!\n");
		return(errCodeParameter);
	}
	if (((index = UnionGetAvailableIndex(pgunionIndexStatusTBL)) < 0) || (index >= pgunionMsgBufHDL->msgBufDef.maxNumOfMsg))
	{
		UnionUserErrLog("in UnionBufferMessage:: UnionGetAvailableIndex!\n");
		return(errCodeMsgBufMDL_InvalidIndex);
	}
	pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index);
	pmsgHeader->locked = 1;
	pmsgHeader->type = typeOfMsg;
	pmsgHeader->len = lenOfMsg;
	time(&(pmsgHeader->time));
	pmsgHeader->dealer = 0;
	pmsgHeader->provider = getpid();
	pmsgHeader->msgIndex = UnionApplyNewMsgIndexOfMsgBuf();
	memcpy(pgunionMsgBuf + conMsgSizeInMsgBuf * index + sizeof(TUnionMessageHeader),msg,lenOfMsg);
	pmsgHeader->locked = 0;
	return(UnionSetIndexWithUserStatus(pgunionIndexStatusTBL,index,typeOfMsg));
}
	
int UnionBufferRequestMessage(unsigned char * msg,int lenOfMsg,long typeOfMsg)
{
	return(UnionBufferMessage(msg,lenOfMsg,typeOfMsg));
}
	

int UnionBufferResponseMessage(unsigned char * msg,int lenOfMsg,long typeOfMsg,PUnionMessageHeader pOriMsgHeader)
{
	int			index;
	PUnionMessageHeader	pmsgHeader;
	
	if ((pgunionMsgBufHDL == NULL) || (msg == NULL) || (pOriMsgHeader == NULL) 
	    || (lenOfMsg > pgunionMsgBufHDL->msgBufDef.maxSizeOfMsg) || (pOriMsgHeader->type <= 0) || (lenOfMsg <= 0))
	{
		UnionUserErrLog("in UnionBufferResponseMessage:: wrong parameter! lenOfMsg = [%d]\n",lenOfMsg);
		return(errCodeParameter);
	}
#ifndef _IgnoreProvider_
	if (kill(pOriMsgHeader->provider,0) != 0)
	{
		UnionUserErrLog("in UnionBufferResponseMessage:: the provider [%ld] not exists any more!\n",pOriMsgHeader->provider);
		return(errCodeMsgBufMDL_ProviderExit);
	}
#endif
	if (((index = UnionGetAvailableIndex(pgunionIndexStatusTBL)) < 0) || (index >= pgunionMsgBufHDL->msgBufDef.maxNumOfMsg))
	{
		UnionUserErrLog("in UnionBufferResponseMessage:: UnionGetAvailableIndex!\n");
		return(index);
	}
	
	pmsgHeader = (PUnionMessageHeader)(pgunionMsgBuf + conMsgSizeInMsgBuf * index);
	pmsgHeader->locked = 1;
	pmsgHeader->type = typeOfMsg;
	pmsgHeader->len = lenOfMsg;
	time(&(pmsgHeader->time));
	pmsgHeader->dealer = getpid();
	pmsgHeader->msgIndex = pOriMsgHeader->msgIndex;
	pmsgHeader->provider = pOriMsgHeader->provider;
	memcpy(pgunionMsgBuf + conMsgSizeInMsgBuf * index + sizeof(TUnionMessageHeader),msg,lenOfMsg);
	pmsgHeader->locked = 0;

	return(UnionSetIndexWithUserStatus(pgunionIndexStatusTBL,index,typeOfMsg));
}
