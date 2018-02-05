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
#ifdef _forThread_
#include "unionLockTBL.h"
#endif

jmp_buf			gcommSvrJmpEnv;
int			gcommSvrJmpSet = 0;
int			gunionIgnoreSignals = 0;
int 			gunionSckHDL[conMaxNumOfEssc];
int			gunionSckHDLInited = 0;
unsigned int		gunionTotalCacuNum = 0;

int UnionLockSckHDL(int sckHDLIndex)
{
	char	key[100];
	
#ifdef _forThread_
	sprintf(key,"%d",sckHDLIndex);
	return(UnionApplyRecWritingLockOfSpecRecWithTimeout("unionCommWithTcpipSvr",key,1));
#else
	return(0);
#endif
}

void UnionUnlockSckHDL(int sckHDLIndex)
{
	char	key[100];
	
#ifdef _forThread_
	sprintf(key,"%d",sckHDLIndex);
	UnionFreeRecWritingLockOfSpecRec("unionCommWithTcpipSvr",key);
	return;
#else
	return;
#endif
}

void UnionInitAllSckHDL()
{
	int	index;

	if (gunionSckHDLInited)
		return;	
	for (index = 0; index < conMaxNumOfEssc; index++)
	{
		gunionSckHDL[index] = -1;
	}
	gunionSckHDLInited = 1;
	return;
}

void UnionClientCommTimeout();

void UnionReleaseCommWithHsmSvr(unsigned char *connTag)
{
	int	index;

	if (!gunionSckHDLInited)
		return;
			
	for (index = 0; index < conMaxNumOfEssc; index++)
	{
		if (gunionSckHDL[index] >= 0)
			UnionCloseSocket(gunionSckHDL[index]);
		gunionSckHDL[index] = -1;
	}
}

int UnionSelectIdleSckHDL(int *sckHDLIndex)
{
	int	realSvrNum;
	int	selectIndex;
	int	ret;
		
	if ((realSvrNum = UnionGetRealNumOfEssc()) <= 0)
	{
		UnionUserErrLog("in UnionSelectIdleSckHDL:: UnionGetRealNumOfEssc! realSvrNum = [%d]\n",realSvrNum);
		if (realSvrNum < 0)
			return(realSvrNum);
		else
			return(errCodeTcpipSvrNotDefined);
	}
	if (gunionTotalCacuNum < 0)
		gunionTotalCacuNum = 0;
	selectIndex = gunionTotalCacuNum % realSvrNum;
	++gunionTotalCacuNum;
	if (gunionSckHDL[selectIndex] >= 0)
	{
		*sckHDLIndex = selectIndex;
		return(gunionSckHDL[selectIndex]);
	}
	if ((gunionSckHDL[selectIndex] = UnionCreateSocketClient(UnionGetIPAddrOfCenterSecuSvr(selectIndex),UnionGetPortOfCenterSecuSvr(selectIndex))) < 0)
	{
		UnionUserErrLog("in UnionSelectIdleSckHDL:: UnionCreateSocketClient [%s][%d]!\n",
				UnionGetIPAddrOfCenterSecuSvr(selectIndex),UnionGetPortOfCenterSecuSvr(selectIndex));
		return(gunionSckHDL[selectIndex]);
	}
	if ((ret = UnionLockSckHDL(selectIndex)) < 0)
	{
		UnionUserErrLog("in UnionSelectIdleSckHDL:: UnionLockSckHDL!\n");
		return(ret);
	}
	return(gunionSckHDL[selectIndex]);
}

void UnionCloseSckHDLOfIndex(int sckHDLIndex)
{
	int	realSvrNum;
	
	if ((realSvrNum = UnionGetRealNumOfEssc()) <= 0)
	{
		UnionUserErrLog("in UnionCloseSckHDLOfIndex:: UnionGetRealNumOfEssc! realSvrNum = [%d]\n",realSvrNum);
		return;
	}
	if ((sckHDLIndex < 0) || (sckHDLIndex >= realSvrNum))
		return;
	if (gunionSckHDL[sckHDLIndex] >= 0)
		UnionCloseSocket(gunionSckHDL[sckHDLIndex]);
	gunionSckHDL[sckHDLIndex] = -1;
}
	
int UnionCommWithHsmSvrForThread(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,unsigned char *connTag)
{
	unsigned char	lenBuf[10+1];
	int		ret;
	int		len;
	int		tryTimes = 0;
	int		sckHDL,sckHDLIndex = -1;

	if ((reqStr == NULL) || (lenOfReqStr < 0) || (resStr == NULL))
	{
		UnionUserErrLog("in UnionCommWithHsmSvrForThread:: parameter error! lenOfReqStr = [%d]!\n",lenOfReqStr);
		return(errCodeParameter);
	}

	UnionInitAllSckHDL();
	
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
	// 选择一个空闲的连接
	if ((sckHDL = UnionSelectIdleSckHDL(&sckHDLIndex)) < 0)
	{
		UnionUserErrLog("in UnionCommWithHsmSvrForThread:: UnionSelectIdleSckHDL!\n");
		if (tryTimes > conMaxNumOfEssc + 1)	// 已试过指定次数
		{
			//UnionCloseSckHDLOfIndex(sckHDLIndex);
			return(errCodeAPICommWithEssc);
		}
	}
	// 发送报文长度	
	lenBuf[0] = lenOfReqStr / 256;
	lenBuf[1] = lenOfReqStr % 256;
	if ((ret = UnionSendToSocket(sckHDL,lenBuf,2)) < 0)
	{
		UnionUserErrLog("in UnionCommWithHsmSvrForThread:: UnionSendToSocket!\n");
		goto sckErr;
	}
	// 发送报文正文
	if ((ret = UnionSendToSocket(sckHDL,(unsigned char *)reqStr,lenOfReqStr)) < 0)
	{
		UnionUserErrLog("in UnionCommWithHsmSvrForThread:: UnionSendToSocket!\n");
		goto sckErr;
	}
	if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,lenBuf,2)) != 2)
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
	if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,resStr,len)) != len)
	{
		UnionUserErrLog("in UnionCommWithHsmSvrForThread:: UnionReceiveFromSocketUntilLen [%d] != expected [%d]!\n",ret,len);
		ret = errCodeAPIRecvDataLenNotEqualDefinedLen;
		goto errorExit;
	}
	UnionUnlockSckHDL(sckHDLIndex);
	resStr[len] = 0;
	UnionNullLog("res::[%04d][%s]\n",len,resStr);
normalExit:
#ifndef _WIN32
	alarm(0);
#endif
	if (UnionIsShortConnectionUsed())
	{
		UnionCloseSckHDLOfIndex(sckHDLIndex);
	}
	return(len);

sckErr:
	UnionCloseSckHDLOfIndex(sckHDLIndex);
	goto retry;
	
errorExit:
#ifndef _WIN32
	alarm(0);
#endif
	UnionUnlockSckHDL(sckHDLIndex);
	UnionCloseSckHDLOfIndex(sckHDLIndex);
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
