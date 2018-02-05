//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2006/3/6

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "UnionSocket.h"
#include "unionErrCode.h"

#include "UnionCenterREC.h"

jmp_buf			gcommSvrJmpEnvToEssc;
int			gcommSvrJmpSetToEssc = 0;
int			gunionIgnoreSignalsToEssc = 0;
int 			gunionSckHDLToEssc = -1;

void UnionClientCommTimeoutToEssc();

void UnionReleaseCommWithHsmSvrToEssc()
{
	if (gunionSckHDLToEssc >= 0)
		UnionCloseSocket(gunionSckHDLToEssc);
	gunionSckHDLToEssc = -1;
}

int UnionCommWithHsmSvrToEssc(char *serviceCode,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	unsigned char	lenBuf[10+1];
	int		ret;
	int		len;
	int		tryTimes = 0;
	char		tmpBuf[2048+1];
	char		answerAppID[10+1];
	char		answerServiceID[10+1];
	int		offset;
	char		resCode[6+1];

	if ((serviceCode == NULL) || (reqStr == NULL) || (lenOfReqStr < 0) || (resStr == NULL))
	{
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: parameter error! lenOfReqStr = [%d]!\n",lenOfReqStr);
		return(errCodeParameter);
	}

	// 初始化运行环境
	if ((ret = UnionConnectCenterREC()) < 0)
	{
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: UnionConnectCenterREC!\n");
		return(ret);
	}

	if (!gunionIgnoreSignalsToEssc)
	{
		gunionIgnoreSignalsToEssc = 1;
		signal(SIGPIPE,SIG_IGN);
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	len = 0;
	memcpy(tmpBuf+len,UnionGetIDOfEsscAPI(),2);
	len += 2;
	memcpy(tmpBuf+len,serviceCode,3);
	len += 3;
	tmpBuf[len] = '1';
	len++;
	if (lenOfReqStr + len >= sizeof(tmpBuf))
	{
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: lenOfReqStr [%d] too long!\n",lenOfReqStr);
		if (lenOfReqStr > 0)
		{
			reqStr[lenOfReqStr] = 0;
			UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: lenOfReqStr=[%04d]reqStr=[%s]\n",lenOfReqStr,reqStr);
		}
		return(errCodeParameter);
	}
	memcpy(tmpBuf+len,reqStr,lenOfReqStr);
	len += lenOfReqStr;
	tmpBuf[len] = 0;	
	UnionNullLogWithTime("lenOfReqStr=[%04d]reqStr=[%s]\n",len,tmpBuf);

	alarm(UnionGetTimeoutOfCenterSecuSvr());
	signal(SIGALRM,UnionClientCommTimeoutToEssc);

retry:
	tryTimes++;
	if (tryTimes > conMaxNumOfEssc + 1)	// 已试过指定次数
	{
		if (gunionSckHDLToEssc >= 0)
		{
			UnionCloseSocket(gunionSckHDLToEssc);
			gunionSckHDLToEssc = -1;
		}
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: lenOfReqStr=[%04d]reqStr=[%s%s%s%s]\n",lenOfReqStr+2+3+1,
			UnionGetIDOfEsscAPI(),serviceCode,"1",reqStr);
		alarm(0);
		return(errCodeAPICommWithEssc);
	}
	if (gunionSckHDLToEssc < 0)
	{	
		UnionDebugLog("in UnionCommWithHsmSvrToEssc:: ip=[%s],port=[%d]\n",UnionGetIPAddrOfCenterSecuSvr(tryTimes),UnionGetPortOfCenterSecuSvr(tryTimes));
		// 创建与ESSC的连接
		if ((gunionSckHDLToEssc = UnionCreateSocketClient(UnionGetIPAddrOfCenterSecuSvr(tryTimes),UnionGetPortOfCenterSecuSvr(tryTimes))) < 0)
		{
			UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: UnionCreateSocketClient [%s][%d]!\n",
				UnionGetIPAddrOfCenterSecuSvr(tryTimes),UnionGetPortOfCenterSecuSvr(tryTimes));
			goto retry;
		}
	}

	// 发送报文长度	
	lenBuf[0] = len / 256;
	lenBuf[1] = len % 256;
	if ((ret = UnionSendToSocket(gunionSckHDLToEssc,lenBuf,2)) < 0)
	{
		if (tryTimes > 1)		// Mary add, 20081114
			UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: UnionSendToSocket!\n");
		goto sckErr;
	}
	// 发送报文正文
	if ((ret = UnionSendToSocket(gunionSckHDLToEssc,(unsigned char *)tmpBuf,len)) < 0)
	{
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: UnionSendToSocket!\n");
		goto sckErr;
	}
	if ((ret = UnionReceiveFromSocketUntilLen(gunionSckHDLToEssc,lenBuf,2)) != 2)
	{
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: UnionReceiveFromSocketUntilLen!\n");
		goto sckErr;
	}
	if ((len = lenBuf[0] * 256 + lenBuf[1]) >= sizeof(tmpBuf))
	{
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: expected len [%d] > receiveBuffer [%d]!\n",len,sizeof(tmpBuf));
		ret = errCodeAPIBufferSmallForRecvData;
		goto errorExit;
	}
	if (len == 0)
		goto normalExit;
	if (len < 0)
		goto errorExit;
	if ((ret = UnionReceiveFromSocketUntilLen(gunionSckHDLToEssc,(unsigned char *)tmpBuf,len)) != len)
	{
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: UnionReceiveFromSocketUntilLen [%d] != expected [%d]!\n",ret,len);
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: lenOfReqStr=[%04d]reqStr=[%s%s%s%s]\n",lenOfReqStr+2+3+1,
			UnionGetIDOfEsscAPI(),serviceCode,"1",reqStr);
		ret = errCodeAPIRecvDataLenNotEqualDefinedLen;
		goto errorExit;
	}
	tmpBuf[len] = 0;
	UnionNullLogWithTime("lenOfResStr=[%04d]resStr=[%s]\n",len,tmpBuf);
normalExit:
	alarm(0);
	if (UnionIsShortConnectionUsed())
	{
		UnionCloseSocket(gunionSckHDLToEssc);
		gunionSckHDLToEssc = -1;
	}
	if ((len < 2 + 3 + 1 + 6) || (tmpBuf[2+3] != '0'))
	{
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: UnionUnpackEsscResponsePackage!\n");
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: lenOfReqStr=[%04d]reqStr=[%s%s%s%s]\n",lenOfReqStr+2+3+1,
			UnionGetIDOfEsscAPI(),serviceCode,"1",reqStr);
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: lenOfResStr=[%04d]resStr=[%s]\n",len,tmpBuf);
		return(errCodeAPIRecvDataLenNotEqualDefinedLen);
	}
	memset(answerServiceID,0,sizeof(answerServiceID));
	memset(answerAppID,0,sizeof(answerAppID));
	offset = 0;
	memcpy(answerAppID,tmpBuf+offset,2);
	offset += 2;
	memcpy(answerServiceID,tmpBuf+offset,3);
	offset += 3;
	if ((strcmp(answerAppID,UnionGetIDOfEsscAPI()) != 0) || 
		(strcmp(answerServiceID,serviceCode) != 0))
	{
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: not response to this request!\n");
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: lenOfReqStr=[%04d]reqStr=[%s%s%s%s]\n",lenOfReqStr+2+3+1,
			UnionGetIDOfEsscAPI(),serviceCode,"1",reqStr);
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: lenOfResStr=[%04d]resStr=[%s]\n",len,tmpBuf);
		return(errCodeEsscMDL_ReqAndResNotIsIndentified);
	}
	offset++;
	memcpy(resCode,tmpBuf+offset,6);
	resCode[6] = 0;
	offset += 6;
	if ((ret = atoi(resCode)) < 0)
	{
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: ret from server = [%d]\n",ret);
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: lenOfReqStr=[%04d]reqStr=[%s%s%s%s]\n",lenOfReqStr+2+3+1,
			UnionGetIDOfEsscAPI(),serviceCode,"1",reqStr);
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: lenOfResStr=[%04d]resStr=[%s]\n",len,tmpBuf);
		return(ret);
	}
	if ((len - offset) >= sizeOfResStr)
	{
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: sizeOfResStr too small!\n");
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: lenOfReqStr=[%04d]reqStr=[%s%s%s%s]\n",lenOfReqStr+2+3+1,
			UnionGetIDOfEsscAPI(),serviceCode,"1",reqStr);
		UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: lenOfResStr=[%04d]resStr=[%s]\n",len,tmpBuf);
		return(errCodeSmallBuffer);
	}
	memcpy(resStr,tmpBuf+offset,len-offset);	
	return(len-offset);

sckErr:
	UnionCloseSocket(gunionSckHDLToEssc);
	gunionSckHDLToEssc = -1;
	goto retry;

errorExit:
	alarm(0);
	UnionCloseSocket(gunionSckHDLToEssc);
	gunionSckHDLToEssc = -1;
	UnionUserErrLog("in UnionCommWithHsmSvrToEssc:: lenOfReqStr=[%04d]reqStr=[%s%s%s%s]\n",lenOfReqStr+2+3+1,
		UnionGetIDOfEsscAPI(),serviceCode,"1",reqStr);
	if (ret >= 0)
		return(errCodeAPIShouldReturnMinusButRetIsNotMinus);
	else
		return(ret);
}

void UnionClientCommTimeoutToEssc()
{
	UnionUserErrLog("in UnionClientCommTimeoutToEssc:: ESSC time out!\n");
	gcommSvrJmpSetToEssc = 1;
#ifdef _LINUX_
	siglongjmp(gcommSvrJmpEnvToEssc,10);
#else
	longjmp(gcommSvrJmpEnvToEssc,10);
#endif
}



