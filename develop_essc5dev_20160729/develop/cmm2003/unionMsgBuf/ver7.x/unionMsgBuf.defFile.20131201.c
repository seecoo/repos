//	Author:	Wolfgang Wang
//	Date:	2006/8/1

#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <time.h>

#include "unionMsgBuf7.x.h"
#include "unionModule.h"
#include "unionErrCode.h"
#include "UnionEnv.h"
#include "UnionLog.h"
#include "UnionStr.h"

//extern int	gisMultiMSMQ;

int UnionGetFileNameOfMsgBufHDL(char *fileName)
{
	return(sprintf(fileName,"%s/unionMsgBuf.Def",getenv("UNIONETC")));
}

int UnionInitMsgBufHDL(PUnionMsgBufHDL pdef)
{
	int		i;
	int		ret;
	char		fileName[512];
	char		tmpBuf[128];
	char		*p;

	if (pdef == NULL)
		return(errCodeParameter);
			
	ret = UnionGetFileNameOfMsgBufHDL(fileName);
	fileName[ret] = 0;
	
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionInitMsgBufHDL:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("groupNumOfMsg")) == NULL)
		pdef->groupNumOfMsg = 1;
	else
		pdef->groupNumOfMsg = atoi(p);

	if (pdef->groupNumOfMsg > defMaxMsgBufGroupNum)
	{
		UnionUserErrLog("in UnionInitMsgBufHDL:: groupNumOfMsg[%d] > defMaxMsgBufGroupNum[%d]\n!",pdef->groupNumOfMsg,defMaxMsgBufGroupNum);
		goto abnormalExit;
	}
	
	// 多个消息队列	
	/*
	if ((p = UnionGetEnviVarByName("isMultiMSMQ")) == NULL)
		gisMultiMSMQ = 0;
	else
		gisMultiMSMQ = atoi(p);
	*/

	if ((p = UnionGetEnviVarByName("maxNumOfMsg")) == NULL)
	{
		UnionUserErrLog("in UnionInitMsgBufHDL:: UnionGetEnviVarByName for [%s]\n!","maxNumOfMsg");
		goto abnormalExit;
	}
	for (i = 0; i < pdef->groupNumOfMsg; i++)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret =  UnionReadFldFromBuf(p,':',i + 1,tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionInitMsgBufHDL:: UnionReadFldFromBuf  maxNumOfMsg[%s] index[%d]\n!",p,i+1);
			goto abnormalExit;
		}
		pdef->msgGrp[i].maxNumOfMsg = atoi(tmpBuf);
	}

	if ((p = UnionGetEnviVarByName("maxSizeOfMsg")) == NULL)
	{
		UnionUserErrLog("in UnionInitMsgBufHDL:: UnionGetEnviVarByName for [%s]\n!","maxSizeOfMsg");
		goto abnormalExit;
	
	}
	for (i = 0; i < pdef->groupNumOfMsg; i++)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret =  UnionReadFldFromBuf(p,':',i + 1,tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionInitMsgBufHDL:: UnionReadFldFromBuf  maxSizeOfMsg[%s] index[%d]\n!",p,i+1);
			goto abnormalExit;
		}
		pdef->msgGrp[i].maxSizeOfMsg = atoi(tmpBuf);
	}
	
	if ((p = UnionGetEnviVarByName("maxStayTime")) == NULL)
	{
		UnionUserErrLog("in UnionInitMsgBufHDL:: UnionGetEnviVarByName for [%s]\n!","maxStayTime");
		goto abnormalExit;
	}
	pdef->maxStayTime = atoi(p);
	
	if ((p = UnionGetEnviVarByName("isNoWaitWhenReading")) == NULL)
		pdef->isNoWaitWhenReading = 0;
	else
		pdef->isNoWaitWhenReading = atoi(p);

	if (pdef->isNoWaitWhenReading)
	{
		if ((p = UnionGetEnviVarByName("intervalWhenReading")) == NULL)
			pdef->intervalWhenReading = 5;
		else
			pdef->intervalWhenReading = atoi(p);
	}
	else
		pdef->intervalWhenReading = 0;
	
	UnionClearEnvi();
	
	pdef->userID = -1;
	pdef->queueIDOfFreePos = pdef->queueIDOfOccupiedPos = -1;
		
	return(0);

abnormalExit:	
	UnionClearEnvi();
	return(errCodeMsgBufMDL_InitDef);
}
