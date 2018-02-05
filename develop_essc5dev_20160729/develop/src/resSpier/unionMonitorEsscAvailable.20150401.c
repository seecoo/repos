#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <setjmp.h>

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "unionMDLID.h"
#include "UnionTask.h"

#include "unionCommand.h"
#include "transSpierBuf.h"
#include "unionMonitorTask.h"
#include "unionREC.h"
#include "UnionLog.h"
#include "UnionSocket.h"
#include "unionErrCode.h"
#include "unionTransInfoToMonitor.h"
#include "unionVersion.h"
#include "UnionStr.h"
#include "unionMonitorResID.h"
#include "unionXMLPackage.h"
#include "unionMXML.h"

#define conMaxSvrProcess	10
extern PUnionTaskInstance	ptaskInstance;

jmp_buf gunionEsscAvailableJmpEnv;

void UnionDealEsscAvailableJmpEnv();

int UnionCheckEsscAvailable(char *ipAddr, int port, char *reqBuf, int sizeOfReqBuf, \
				char *resBuf, int sizeOfResBuf, char *errCode)
{
	int		esscSckHDL = 0;
	int		ret;
	int		resLen;
	unsigned char	tmpBuf[2048];
	unsigned char	lenBuf[4];
	char		responseCode[8];
	char		responseRemark[256];


	if ((ipAddr == NULL) || (reqBuf == NULL) || (resBuf == NULL) || (responseCode == NULL))
	{
		UnionUserErrLog("in UnionCheckEsscAvailable:: parameter error!\n");
		ret = errCodeParameter;
		goto dealError;
	}

	if ((esscSckHDL = UnionCreateSocketClient(ipAddr, port)) < 0)
	{
		UnionUserErrLog("in UnionCheckEsscAvailable:: UnionCreateSocketClient [%s] [%d]\n",ipAddr,port);
		ret = esscSckHDL;
		goto dealError;
	}

	if (sizeOfReqBuf >= sizeof(tmpBuf))
	{
		UnionUserErrLog("in UnionCheckEsscAvailable:: tmpBuf size is small!\n");
		ret = errCodeSmallBuffer;
		goto dealError;
	}
	tmpBuf[0] = sizeOfReqBuf / 256;
	tmpBuf[1] = sizeOfReqBuf % 256;

	memcpy(tmpBuf + 2, reqBuf, sizeOfReqBuf);
	tmpBuf[sizeOfReqBuf + 2] = 0;

	if ((ret = UnionSendToSocket(esscSckHDL,(unsigned char *)tmpBuf, sizeOfReqBuf + 2)) < 0)
	{
		UnionUserErrLog("in UnionCheckEsscAvailable:: UnionSendToSocket!\n");
		goto dealError;
	}

	if ((ret = UnionReceiveFromSocketUntilLen(esscSckHDL, (unsigned char *)lenBuf, 2)) < 0)
	{
		UnionUserErrLog("in UnionCheckEsscAvailable:: UnionReceiveFromSocketUntilLen!\n");
		goto dealError;
	}

	resLen = lenBuf[0] * 256 + lenBuf[1];
	if (resLen >= sizeof(tmpBuf))
	{
		UnionUserErrLog("in UnionCheckEsscAvailable:: tmpBuf len[%zd] is small!\n", sizeof(tmpBuf));
		ret = errCodeSmallBuffer;
		goto dealError;
	}

	if ((ret = UnionReceiveFromSocketUntilLen(esscSckHDL, (unsigned char *)tmpBuf, resLen)) < 0)
	{
		UnionUserErrLog("in UnionCheckEsscAvailable:: UnionReceiveFromSocketUntilLen!\n");
		goto dealError;
	}
	tmpBuf[resLen] = 0;
	UnionLog("in UnionCheckEsscAvailable:: essc responseBuf [%d] [%s]\n", resLen, tmpBuf);

	// 解析响应报文
	if (memcmp((char *)tmpBuf,PACKAGE_VERSION_001,4) == 0)
        {
                UnionSetPackageType(PACKAGE_TYPE_V001);
        }
        else if (memcmp((char *)tmpBuf,"<?xml",5) == 0)
        {
                UnionSetPackageType(PACKAGE_TYPE_XML);
        }
	else
	{
		UnionUserErrLog("UnionCheckEsscAvailable:: package type error!\n");
		//return(errCodePackageDefMDL_InvalidPackageType);
		ret = errCodePackageDefMDL_InvalidPackageType;
		goto dealError;
	}

	if ((ret = UnionInitResponseXMLPackage(NULL,(char *)tmpBuf,resLen)) < 0)
	{
		UnionUserErrLog("UnionCheckEsscAvailable:: UnionInitResponseXMLPackage !\n");
		goto dealError;
	}
	memset(responseCode, 0, sizeof(responseCode));
	if ((ret = UnionReadResponseXMLPackageValue("head/responseCode", responseCode, sizeof(responseCode))) < 0)
	{
		UnionUserErrLog("in UnionCheckEsscAvailable:: UnionReadResponseXMLPackageValue responseCode!\n");
		goto dealError;
	}
	memset(responseRemark, 0, sizeof(responseRemark));
	if ((ret = UnionReadResponseXMLPackageValue("head/responseRemark", responseRemark, sizeof(responseRemark))) < 0)
	{
		UnionUserErrLog("in UnionCheckEsscAvailable:: UnionReadResponseXMLPackageValue responseRemark!\n");
		goto dealError;
	}

	ret = snprintf(resBuf, sizeOfResBuf, "responseCode=%s|responseRemark=%s", responseCode, responseRemark);
	sprintf(errCode, "%06d", 0);
	close(esscSckHDL);
	return(ret);

dealError:
	if (esscSckHDL > 0)
		close(esscSckHDL);
	sprintf(errCode, "%06d", ret);
	return(ret);
}

int UnionDealEsscAvailableRequest(int handle)
{
	int		ret;
	char		varName[128];
	int		timeoutOfFunSvr;
	int		recvLen;
	int		port;
	int		tmeoutOfFunSvr;
	unsigned char	lenBuf[4];
	char		reqBuf[2048];
	//char		resBuf[2048];
	char		ipAddr[16];
	char		msgCheck[1024];
	char		tmpBuf[1024];
	char		esscRes[1024];
	unsigned char	sendBuf[2048];
	char		errCode[8];
	char		resStr[2048];
	char		*esscID;
	char		systemDateTime[20];

	if (handle <= 0)
	{
                UnionUserErrLog("in UnionDealEsscAvailableRequest:: parameter error!\n");
                return(errCodeParameter);
	}

	// 获取平台ID
	if ((esscID = UnionGetMyMngSvrIDAtMonitor()) == NULL)
        {       
                UnionUserErrLog("in UnionDealEsscAvailableRequest:: UnionGetMyMngSvrIDAtMonitor return NULL!!\n");
                return(errCodeParameter);
        }

	// 读取服务超时时间
	snprintf(varName, sizeof(varName), "timeoutOf%s", UnionGetApplicationName());
	if ((timeoutOfFunSvr = UnionReadIntTypeRECVar(varName)) < 0)
		tmeoutOfFunSvr = 10;

#if ( defined __linux__ ) || ( defined __hpux )
        if (sigsetjmp(gunionEsscAvailableJmpEnv,1) != 0) // 超时退出
#elif ( defined _AIX )
        if (setjmp(gunionEsscAvailableJmpEnv) != 0)      // 超时退出
#endif
	{
		ret = errCodeTimeout;
		UnionUserErrLog("in UnionDealEsscAvailableRequest:: receve timeout!\n");
		goto errorExit;
        }
	alarm(tmeoutOfFunSvr);
        signal(SIGALRM,UnionDealEsscAvailableJmpEnv);

	// 接收监控服务器发送的请求报文
	if ((ret = UnionReceiveFromSocketUntilLen(handle,(unsigned char *)lenBuf,2)) < 0)
	{
		UnionUserErrLog("in UnionDealEsscAvailableRequest:: UnionReceiveFromSocketUntilLen Error ret = [%d]!\n",ret);
		goto errorExit;
	}
	recvLen = lenBuf[0] * 256 + lenBuf[1];
	if (recvLen >= sizeof(reqBuf))
	{
		UnionUserErrLog("in UnionDealEsscAvailableRequest:: resBuf is small!\n");
		ret = errCodeSmallBuffer;
		goto errorExit;
	}
	if ((ret = UnionReceiveFromSocketUntilLen(handle,(unsigned char *)reqBuf,recvLen)) < 0)
	{
		UnionUserErrLog("in UnionDealEsscAvailableRequest:: UnionReceiveFromSocketUntilLen Error ret = [%d]!\n",ret);
		goto errorExit;
	}
	alarm(0);
	reqBuf[recvLen] = 0;

	UnionLog("in UnionDealEsscAvailableRequest:: recv [%04d] [%s]\n", recvLen, reqBuf);

	// 解析监控服务器请求的报文
	// 监控服务器探测的平台IP地址
	if ((ret = UnionReadRecFldFromRecStr(reqBuf,recvLen,"ipAddr",ipAddr,sizeof(ipAddr))) < 0)
	{
		UnionUserErrLog("in UnionDealEsscAvailableRequest:: UnionReadRecFldFromRecStr ipAddr error!\n");
		return(ret);
	}

	// 平台端口
	if ((ret = UnionReadIntTypeRecFldFromRecStr(reqBuf, recvLen, "port", &port)) < 0)
	{
		UnionUserErrLog("in UnionDealEsscAvailableRequest:: UnionReadIntTypeRecFldFromRecStr port error!\n");
		return(ret);
	}

	// 平台请求报文
	if ((ret = UnionReadRecFldFromRecStr(reqBuf,recvLen,"msgCheck",msgCheck,sizeof(msgCheck))) < 0)
	{
		UnionUserErrLog("in UnionDealEsscAvailableRequest:: UnionReadRecFldFromRecStr msgCheck error!\n");
		return(ret);
	}

	memset(systemDateTime, 0, sizeof(systemDateTime));
	UnionGetFullSystemDateTime(systemDateTime);

	// 检测平台是否可用
	snprintf(tmpBuf, sizeof(tmpBuf), "%s", msgCheck);
	memset(esscRes, 0, sizeof(esscRes));
	memset(resStr, 0, sizeof(resStr));
	ret = UnionCheckEsscAvailable(ipAddr, port, tmpBuf, strlen(tmpBuf), esscRes, sizeof(esscRes), errCode);

	snprintf(resStr, sizeof(resStr), "%s|ipAddr=%s|port=%d|systemTime=%s|", esscRes,ipAddr, port, systemDateTime);
	UnionLog("in UnionDealEsscAvailableRequest:: send to monitor buf [%s]\n", resStr);
	ret = UnionPackTransInfoResponsePackage(resStr, strlen(resStr), errCode, esscID, conResIDAvailable, 0, (char *)sendBuf+2, sizeof(sendBuf)-2);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionDealEsscAvailableRequest:: UnionPackTransInfoResponsePackage!\n");
		return(ret);
	}

	sendBuf[0] = ret / 256;
	sendBuf[1] = ret % 256;
	ret += 2;

#if ( defined __linux__ ) || ( defined __hpux )
        if (sigsetjmp(gunionEsscAvailableJmpEnv,1) != 0) // 超时退出
#elif ( defined _AIX )
        if (setjmp(gunionEsscAvailableJmpEnv) != 0)      // 超时退出
#endif
	{
		ret = errCodeTimeout;
		UnionUserErrLog("in UnionDealEsscAvailableRequest:: send timeout!\n");
		goto errorExit;
        }
	alarm(tmeoutOfFunSvr);
        signal(SIGALRM,UnionDealEsscAvailableJmpEnv);
	if ((ret = UnionSendToSocket(handle, (unsigned char *)sendBuf, ret)) < 0)
	{
		UnionUserErrLog("in UnionDealEsscAvailableRequest:: UnionSendToSocket!\n");
		goto errorExit;
	}
	alarm(0);
	return(ret);
errorExit:
	alarm(0);
	return(ret);
}

int UnionStartEsscAvailableSvr(int port, int (*UnionTaskActionBeforeExit)())
{
	int			ret;
	int			boundSckHDL;
	int			sckinstance;
	unsigned int		clilen;
	struct sockaddr_in	cli_addr;
	struct linger		Linger;

	// 初始化一个侦听端口
	if ((boundSckHDL = UnionInitializeTCPIPServer(port)) < 0)
	{
		UnionUserErrLog("in UnionStartEsscAvailableSvr:: UnionInitializeTCPIPServer Error!\n");
		return(boundSckHDL);
	}
	UnionSuccessLog("a servier bound to [%d] started OK!\n", port);

	signal(SIGCHLD, SIG_IGN);

	while (1)
	{
		clilen = sizeof(cli_addr);
		sckinstance = accept(boundSckHDL, (struct sockaddr *)&cli_addr,&clilen);
		if ( sckinstance < 0 )
		{
			UnionSystemErrLog("in UnionStartEsscAvailableSvr:: accept()!\n");
			continue;
		}

		if (UnionCreateProcess() > 0)
		{
			close(sckinstance);
			continue;
		}

		Linger.l_onoff = 1;
		Linger.l_linger = 0;
		if (setsockopt(sckinstance,SOL_SOCKET,SO_LINGER,(char *)&Linger,sizeof(Linger)) != 0)
		{
			UnionSystemErrLog("in UnionStartEsscAvailableSvr:: setsockopt linger!");
			close(sckinstance);
			return(errCodeUseOSErrCode);
		}
		/*
		if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s child %d",UnionGetApplicationName(), port)) == NULL)
		{
			UnionUserErrLog("in UnionStartEsscAvailableSvr:: UnionCreateTaskInstance '%s child %d' Error!\n",\
					UnionGetApplicationName(),port);
			close(sckinstance);
			return(UnionTaskActionBeforeExit());
		}
		*/

		if ((ret = UnionDealEsscAvailableRequest(sckinstance)) < 0)
		{
			UnionUserErrLog("in UnionStartEsscAvailableSvr:: UnionDealEsscAvailableRequest\n");
		}
		close(sckinstance);
		return(UnionTaskActionBeforeExit());
	}

	return 0;
}

void UnionDealEsscAvailableJmpEnv()
{
	UnionUserErrLog("in UnionDealEsscAvailableJmpEnv:: timeout!\n");
#if ( defined __linux__ ) || ( defined __hpux )
        siglongjmp(gunionEsscAvailableJmpEnv,10);
#elif ( defined _AIX )
        longjmp(gunionEsscAvailableJmpEnv,10);
#endif
}
