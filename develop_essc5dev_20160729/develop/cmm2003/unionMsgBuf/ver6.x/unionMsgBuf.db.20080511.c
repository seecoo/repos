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

#include "unionUnionMsgBuf.h"

extern PUnionSharedMemoryModule		pgpdfzMsgBufMDL;
extern PUnionMsgBufHDL			pgpdfzMsgBufHDL;
extern unsigned char			*pgpdfzMsgBuf;
extern int				gpdfzMsgBufMDLConnected;

int UnionGetFileNameOfMsgBufHDL(char *fileName)
{
	sprintf(fileName,"unionMsgBuf");
	return(0);
}

int UnionInitMsgBufHDL(PUnionMsgBufHDL pdef)
{
	int	ret = 0;
	char szValue[128+1];

	if (pdef == NULL)
		return(errCodeParameter);

	if( (ret = UnionReadUnionMsgBufRecFld("maxNumOfMsg", "propertyValue", szValue, 129)) < 0 )
	{
		UnionUserErrLog("in UnionInitMsgBufHDL:: UnionReadUnionMsgBufRecFld maxNumOfMsg\n");
		return (ret);
	}

	pdef->maxNumOfMsg = atoi(szValue);

	if( (ret = UnionReadUnionMsgBufRecFld("maxSizeOfMsg", "propertyValue", szValue, 129)) < 0 )
	{
		UnionUserErrLog("in UnionInitMsgBufHDL:: UnionReadUnionMsgBufRecFld maxSizeOfMsg\n");
		return (ret);
	}
	pdef->maxSizeOfMsg = atol(szValue);
	
	if( (ret = UnionReadUnionMsgBufRecFld("maxStayTime", "propertyValue", szValue, 129)) < 0 )
	{
		UnionUserErrLog("in UnionInitMsgBufHDL:: UnionReadUnionMsgBufRecFld maxStayTime\n");
		return (ret);
	}
	pdef->maxStayTime = atoi(szValue);

	pdef->userID = -1;
	pdef->queueIDOfFreePos = pdef->queueIDOfOccupiedPos = -1;

	return(0);
}
