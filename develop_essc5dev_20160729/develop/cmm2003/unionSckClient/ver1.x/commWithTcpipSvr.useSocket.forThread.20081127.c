//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2006/3/6

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <stdlib.h>

#include "UnionSocket.h"
#include "unionErrCode.h"

#include "commWithTcpipSvr.h"
#include "centerREC.h"
#include "UnionLog.h"


jmp_buf			gcommSvrJmpEnv;
int			gcommSvrJmpSet = 0;
int			gunionIgnoreSignals = 0;
int 			gunionSckHDL = -1;

void UnionClientCommTimeout();

void UnionReleaseCommWithHsmSvr(unsigned char *connTag)
{
	if (gunionSckHDL >= 0)
		UnionCloseSocket(gunionSckHDL);
	gunionSckHDL = -1;
}

int UnionCommWithHsmSvrForThread(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,unsigned char *connTag)
{
	unsigned char	lenBuf[10+1];
	int		ret;
	int		len;
	int		tryTimes = 0;

	if ((reqStr == NULL) || (lenOfReqStr < 0) || (resStr == NULL))
	{
		UnionUserErrLog("in UnionCommWithHsmSvrForThread:: parameter error! lenOfReqStr = [%d]!\n",lenOfReqStr);
		return(errCodeParameter);
	}

#ifndef _WIN32
	if (!gunionIgnoreSignals)
	{
		gunionIgnoreSignals = 1;
		signal(SIGPIPE,SIG_IGN);
	}
#endif

#ifndef _WIN32
	// 设置超时机制
#if ( defined __linux__ ) || ( defined __hpux )
	if (sigsetjmp(gcommSvrJmpEnv,1) != 0)
if ( defined _AIX )
	if (setjmp(gcommSvrJmpEnv) != 0)
#endif
	{
		UnionSystemErrLog("in UnionCommWithHsmSvrForThread:: timeout!\n");
		if (lenOfReqStr > 0)
		{
			reqStr[lenOfReqStr] = 0;
			UnionUserErrLog("in UnionCommWithHsmSvrForThread:: lenOfReqStr=[%04d]reqStr=[%s]\n",lenOfReqStr,reqStr);
		}
		goto errorExit;
	}
	alarm(UnionGetTimeoutOfCenterSecuSvr());
	signal(SIGALRM,UnionClientCommTimeout);
#endif

	reqStr[lenOfReqStr] = 0;
	UnionNullLog("req::[%04d][%s]\n",lenOfReqStr,reqStr);
retry:
	tryTimes++;
	if (tryTimes > conMaxNumOfEssc + 1)	// 已试过指定次数
	{
		if (gunionSckHDL >= 0)
		{
			UnionCloseSocket(gunionSckHDL);
			gunionSckHDL = -1;
		}
		return(errCodeAPICommWithEssc);
	}
	if (gunionSckHDL < 0)
	{	// 创建与ESSC的连接
		if ((gunionSckHDL = UnionCreateSocketClient(UnionGetIPAddrOfCenterSecuSvr(tryTimes),UnionGetPortOfCenterSecuSvr(tryTimes))) < 0)
		{
			UnionUserErrLog("in UnionCommWithHsmSvrForThread:: UnionCreateSocketClient [%s][%d]!\n",
					UnionGetIPAddrOfCenterSecuSvr(tryTimes),UnionGetPortOfCenterSecuSvr(tryTimes));
			goto retry;
		}
	}

	// 发送报文长度	
	lenBuf[0] = lenOfReqStr / 256;
	lenBuf[1] = lenOfReqStr % 256;
	if ((ret = UnionSendToSocket(gunionSckHDL,lenBuf,2)) < 0)
	{
		UnionUserErrLog("in UnionCommWithHsmSvrForThread:: UnionSendToSocket!\n");
		goto sckErr;
	}
	// 发送报文正文
	if ((ret = UnionSendToSocket(gunionSckHDL,(unsigned char *)reqStr,lenOfReqStr)) < 0)
	{
		UnionUserErrLog("in UnionCommWithHsmSvrForThread:: UnionSendToSocket!\n");
		goto sckErr;
	}
	if ((ret = UnionReceiveFromSocketUntilLen(gunionSckHDL,lenBuf,2)) != 2)
	{
		UnionUserErrLog("in UnionCommWithHsmSvrForThread:: UnionReceiveFromSocketUntilLen!\n");
		goto sckErr;
	}
	if ((len = lenBuf[0] * 256 + lenBuf[1]) >= sizeOfResStr)
	{
		UnionUserErrLog("in UnionCommWithHsmSvrForThread:: expected len [%d] > receiveBuffer [%d]!\n",len,sizeOfResStr);
		ret = errCodeAPIBufferSmallForRecvData;
		goto errorExit;
	}
	if (len == 0)
		goto normalExit;
	if (len < 0)
		goto errorExit;
	if ((ret = UnionReceiveFromSocketUntilLen(gunionSckHDL,resStr,len)) != len)
	{
		UnionUserErrLog("in UnionCommWithHsmSvrForThread:: UnionReceiveFromSocketUntilLen [%d] != expected [%d]!\n",ret,len);
		ret = errCodeAPIRecvDataLenNotEqualDefinedLen;
		goto errorExit;
	}
	resStr[len] = 0;
	UnionNullLog("res::[%04d][%s]\n",len,resStr);
normalExit:
#ifndef _WIN32
	alarm(0);
#endif
	if (UnionIsShortConnectionUsed())
	{
		UnionCloseSocket(gunionSckHDL);
		gunionSckHDL = -1;
	}
	return(len);

sckErr:
	UnionCloseSocket(gunionSckHDL);
	gunionSckHDL = -1;
	goto retry;
	
errorExit:
#ifndef _WIN32
	alarm(0);
#endif
	UnionCloseSocket(gunionSckHDL);
	gunionSckHDL = -1;
	if (ret >= 0)
		return(errCodeAPIShouldReturnMinusButRetIsNotMinus);
	else
		return(ret);
}

#ifndef _WIN32
void UnionClientCommTimeout()
{
	//signal(SIGALRM,SIG_IGN);
	UnionUserErrLog("in UnionClientCommTimeout:: HSM time out!\n");
	gcommSvrJmpSet = 1;
#if ( defined __linux__ ) || ( defined __hpux )
	siglongjmp(gcommSvrJmpEnv,10);
if ( defined _AIX )
	longjmp(gcommSvrJmpEnv,10);
#endif
}
#endif
