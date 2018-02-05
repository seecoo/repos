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

extern PUnionSharedMemoryModule		pgpdfzMsgBufMDL;
extern PUnionMsgBufHDL			pgpdfzMsgBufHDL;
extern unsigned char			*pgpdfzMsgBuf;
extern int				gpdfzMsgBufMDLConnected;

int UnionGetFileNameOfMsgBufHDL(char *fileName)
{
	sprintf(fileName,"%s/unionMsgBuf.Def",getenv("UNIONETC"));
	return(0);
}

int UnionInitMsgBufHDL(PUnionMsgBufHDL pdef)
{
	int		ret;
	char		fileName[512];
	char		*p;

	if (pdef == NULL)
		return(errCodeParameter);
			
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfMsgBufHDL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionInitMsgBufHDL:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("maxNumOfMsg")) == NULL)
	{
		UnionUserErrLog("in UnionInitMsgBufHDL:: UnionGetEnviVarByName for [%s]\n!","maxNumOfMsg");
		goto abnormalExit;
	}
	pdef->maxNumOfMsg = atoi(p);
	
	if ((p = UnionGetEnviVarByName("maxSizeOfMsg")) == NULL)
	{
		UnionUserErrLog("in UnionInitMsgBufHDL:: UnionGetEnviVarByName for [%s]\n!","maxSizeOfMsg");
		goto abnormalExit;
	}
	pdef->maxSizeOfMsg = atol(p);
	
	if ((p = UnionGetEnviVarByName("maxStayTime")) == NULL)
	{
		UnionUserErrLog("in UnionInitMsgBufHDL:: UnionGetEnviVarByName for [%s]\n!","maxStayTime");
		goto abnormalExit;
	}
	pdef->maxStayTime = atoi(p);
	
	UnionClearEnvi();
	
	pdef->userID = -1;
	pdef->queueIDOfFreePos = pdef->queueIDOfOccupiedPos = -1;
		
	return(0);

abnormalExit:	
	UnionClearEnvi();
	return(errCodeMsgBufMDL_InitDef);
}
