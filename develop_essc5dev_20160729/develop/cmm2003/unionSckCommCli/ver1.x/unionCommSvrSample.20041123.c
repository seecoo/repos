//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2004/11/23

#include <stdio.h>
#include <string.h>

#include "unionSckCommCli.h"
#include "UnionSocket.h"
#include "UnionLog.h"

short gunionCommSvrTimeout = 0;

void UnionDealCommSvrTimeout();

int UnionCommWithSckSvr(PUnionSckSvrConn psckSvrConn,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr)
{
	unsigned char	tmpBuf[100];
	int		ret;
	int		len;
	
	if ((lenOfReqStr <= 0) || (psckSvrConn == NULL))
	{
		UnionUserErrLog("in UnionCommWithSckSvr:: psckSvrConn is null or lenOfReqStr = [%d]\n",lenOfReqStr);
		return(-1);
	}
	tmpBuf[0] = lenOfReqStr / 256;
	tmpBuf[1] = lenOfReqStr % 256;
	
	if ((ret = UnionSendToSocket(psckSvrConn->sckHDL,tmpBuf,2)) < 0)
	{
		UnionUserErrLog("in UnionCommWithSckSvr:: UnionSendToSocket!\n");
		goto errorExit;
	}
	if ((ret = UnionSendToSocket(psckSvrConn->sckHDL,reqStr,lenOfReqStr)) < 0)
	{
		UnionUserErrLog("in UnionCommWithSckSvr:: UnionSendToSocket!\n");
		goto errorExit;
	}

	if (gunionCommSvrTimeout)
		gunionCommSvrTimeout = 0;
	alarm(UnionGetTimeoutOfCommSvr());
	signal(SIGALRM,UnionDealCommSvrTimeout);
	if (gunionCommSvrTimeout)
	{
		UnionUserErrLog("in UnionCommWithSckSvr:: timeout [%d]\n",UnionGetTimeoutOfCommSvr());
		gunionCommSvrTimeout = 0;
		alarm(0);
		return(-1);
	}

	if ((ret = UnionReceiveFromSocketUntilLen(psckSvrConn->sckHDL,tmpBuf,2)) < 0)
	{
		UnionUserErrLog("in UnionCommWithSckSvr:: UnionReceiveFromSocketUntilLen!\n");
		goto errorExit;
	}
	if ((len = tmpBuf[0] * 256 + tmpBuf[1]) > sizeOfResStr)
	{
		UnionUserErrLog("in UnionCommWithSckSvr:: expected len [%d] > receiveBuffer [%d]!\n",len,sizeOfResStr);
		goto errorExit;
	}
	if ((ret = UnionReceiveFromSocketUntilLen(psckSvrConn->sckHDL,resStr,len)) < 0)
	{
		UnionUserErrLog("in UnionCommWithSckSvr:: UnionReceiveFromSocketUntilLen!\n");
		goto errorExit;
	}
	++psckSvrConn->psckSvrDef->successTimes;
	alarm(0);
	return(ret);

errorExit:
	++psckSvrConn->psckSvrDef->failTimes;
	alarm(0);
	return(errSocketError);
}

void UnionDealCommSvrTimeout()
{
	gunionCommSvrTimeout = 1;
}

