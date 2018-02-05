// Copyright: Union Tech.
// Author:	Wolfgang Wang
// Date:	2008/5/11

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef _WIN32
#include <signal.h>
#include <setjmp.h>
#include <time.h>
#include "unionREC.h"
#include "UnionTask.h"
#include "unionCommConf.h"
#include "unionREC.h"
#else
#include "unionRECVar.h"
#include <winsock.h>
#endif
#include "UnionLog.h"
#include "errCodeTranslater.h"
#include "unionFunSvrName.h"

#ifdef _client_use_ebcdic_
#include "ebcdicAscii.h"
#endif

#include "UnionSocket.h"
#include "synchTCPIPSvr.h"
#include "unionCommBetweenMDL.h"
#include "commWithTransSpier.h"
#include "unionRealBaseDB.h"
#include "unionRealDBCommon.h"

#ifndef _WIN32
int gsynchTCPIPSvrSckHDL = -1;
extern PUnionTaskInstance 	ptaskInstance;
PUnionCommConf			pgunionCommConf = NULL;

jmp_buf gunionTCPSvrJmpEnvForEsscClient;
jmp_buf gunionTCPSvrJmpEnvForMaxIdleTime;

void UnionDealTCPSvrJmpEnvForMaxIdleTime();

int UnionGetActiveTCIPSvrSckHDL()
{
	return(gsynchTCPIPSvrSckHDL);
}
#endif

int UnionSynchTCPIPTaskServer(int handle,struct sockaddr_in *cli_addr,int port,int (*UnionTaskActionBeforeExit)())
{
	unsigned char	reqStr[conMaxSizeOfClientMsg+1];
	unsigned char	resStr[conMaxSizeOfClientMsg+1];
	int		lenOfReq,lenOfRes;
	int		ret;
	int		maxIdleTimeOfSckConn,timeoutOfFunSvr;
	char		cliName[100];
	char		varName[100];
	char		cliIPAddr[40+1];
	int		finishThisTask = 0;
	int		recvFromClientOK = 0;
	// add by xusj begin 20091116
	char            reqHeader[6+1];
	char            specHsmTask[100];
	char            fixVarName[100];
	TUnionModuleID		oriHsmTaskID;  //add by hzh in 2011.10.10
	
	memset(specHsmTask, 0, sizeof specHsmTask);
	memset(fixVarName, 0, sizeof fixVarName);
	memset(reqHeader, 0, sizeof reqHeader);
	// add by xusj end 20091116

	memset(cliIPAddr,0,sizeof(cliIPAddr));
	inet_ntop(AF_INET, (void *)&cli_addr->sin_addr, cliIPAddr, sizeof(cliIPAddr));
	ret = sprintf(cliName,"%s %d",cliIPAddr,port);
	cliName[ret] = 0;
	UnionSetClientAttrToldTransSpier(cliIPAddr,port);
	
#ifndef _WIN32		
	gsynchTCPIPSvrSckHDL = handle;
	// 创建任务实例
	if (UnionExistsTaskClassOfName(cliName))
	{
		if ((ptaskInstance = UnionCreateTaskInstanceOfName(UnionTaskActionBeforeExit,cliName)) == NULL)
		{
			ret = errCodeCreateTaskInstance;
			UnionUserErrLog("in %s:: UnionCreateTaskInstanceOfName! ret = [%d]\n",cliName,ret);
			goto errorExit;
		}
	}
	else
	{
		if ((ptaskInstance = UnionCreateTaskInstanceOfName(UnionTaskActionBeforeExit,"client %d",port)) == NULL)
		{
			ret = errCodeCreateTaskInstance;
			UnionUserErrLog("in %s:: UnionCreateTaskInstanceOfName! ret = [%d]\n",cliName,ret);
			goto errorExit;
		}
	}
	if ((pgunionCommConf = UnionAddServerCommConf(cliIPAddr,port,conCommShortConn,"安全服务")) == NULL)
	{
#ifdef _limitClient_
		ret = errCodeYLCommConfMDL_AddCommConf;
		UnionUserErrLog("in %s:: UnionAddServerCommConf [%s] [%d]! ret = [%d]\n",cliName,cliIPAddr,port,ret);
		goto errorExit;
#endif
	}
	UnionSetCommConfOK(pgunionCommConf);
#endif
	
	//UnionSetMyTransInfoResID(conEssResIDSecurityService);
	
loop: // 循环处理客户端请求
	recvFromClientOK = 0;
	// 设置连接空闲时间控制
	if ((maxIdleTimeOfSckConn = UnionReadIntTypeRECVar("maxIdleTimeOfSckConn")) < 0)
		maxIdleTimeOfSckConn = 1800;
#ifdef _WIN32
	// add codes here!
#else
#if ( defined __linux__ ) || ( defined __hpux )
	if (sigsetjmp(gunionTCPSvrJmpEnvForMaxIdleTime,1) != 0) // 超时退出
#elif ( defined _AIX )
	if (setjmp(gunionTCPSvrJmpEnvForMaxIdleTime) != 0)	// 超时退出
#endif
	{
		ret = errCodeEsscMDL_TCPIPTaskTooIdle;
		UnionUserErrLog("in %s:: this connection too idle! close it now! ret = [%d]\n",cliName,ret);
		goto errorExit;
	}
	alarm(maxIdleTimeOfSckConn);
	signal(SIGALRM,UnionDealTCPSvrJmpEnvForMaxIdleTime);
#endif
		
	// 开始从客户端接收数据
	memset(reqStr,0,sizeof(reqStr));
	// 接收数据长度
	if ((ret = UnionReceiveFromSocket(handle,reqStr,sizeof(reqStr))) < 0)
	{
		UnionUserErrLog("in %s:: UnionReceiveFromSocket 2! ret = [%d]\n",cliName,ret);
		goto errorExit;
	}
	if (ret == 0)
	{
		UnionLog("in %s:: connection closed by peer!\n",cliName);
		//return(errCodeSJL08_SckClosedByClient);
		return(0);
	}
	lenOfReq = ret;
	if (lenOfReq == 0)	// 测试报文
	{
		UnionNullLog("in %s:: testing pack received!\n",cliName);
		goto loop;
	}
	recvFromClientOK = 1;
#ifndef _WIN32
	UnionIncreaseCommConfWorkingTimes(pgunionCommConf);
#endif
	// 客户端使用ebcdic码通讯
#ifdef _client_use_ebcdic_
	UnionEbcdicToAscii(reqStr,reqStr,lenOfReq);
#endif		
	UnionSendRequestInfoToTransSpier(lenOfReq,(char *)reqStr);	// 监控这个消息
		
	// 执行指令
#ifndef _WIN32
	sprintf(varName,"timeoutOf%s",UnionGetFunSvrName());
#else
	sprintf(varName,"timeoutOfTcpipSvr");
#endif

	// add by xusj begin 20091116 增加路由功能
	memcpy(reqHeader, reqStr, 6);
	oriHsmTaskID = UnionGetFixedMDLIDOfFunSvr();   //add by hzh in 2011.10.11保存原始路由
	if (UnionReadStringTypeRECVar(reqHeader) != NULL)
	{
		strcpy(specHsmTask, UnionReadStringTypeRECVar(reqHeader));
		sprintf(fixVarName,"fixedMDLIDOf%s",specHsmTask);
		UnionSetFixedMDLIDOfFunSvr(UnionReadLongTypeRECVar(fixVarName));
	}
	// add by xusj end 20091116

	if ((timeoutOfFunSvr = UnionReadIntTypeRECVar(varName)) <= 0)
		timeoutOfFunSvr = 5;
	//if ((lenOfRes = UnionHsmCmdInterprotor(reqStr,lenOfReq,resStr+2,sizeof(resStr)-1-2)) < 0)
	if ((lenOfRes = UnionExchangeWithSpecModuleVerifyReqAndRes(UnionGetFixedMDLIDOfFunSvr(),reqStr,lenOfReq,
			resStr,sizeof(resStr),timeoutOfFunSvr)) < 0)
	{
		UnionSetFixedMDLIDOfFunSvr(oriHsmTaskID);   //add by hzh in 2011.10.10恢复原始路由
		ret = lenOfRes;
		UnionUserErrLog("in %s:: UnionExchangeWithSpecModuleVerifyReqAndRes! ret = [%d]\n",cliName,ret);
		goto errorExit;
	}
	else
	{
		UnionSetFixedMDLIDOfFunSvr(oriHsmTaskID);   //add by hzh in 2011.10.10恢复原始路由
		ret = lenOfRes;
		finishThisTask = 0;
	}
		
sendToClient: // 向客户端发送数据
#ifndef _WIN32
	alarm(0);
#endif
	UnionSendResponseInfoToTransSpier(lenOfRes,(char *)resStr); // 监控这个消息
	
#ifdef _client_use_ebcdic_
	UnionAsciiToEbcdic(resStr,resStr,lenOfRes);
#endif	
	if ((ret = UnionSendToSocket(handle,resStr,lenOfRes)) < 0)
	{
		UnionUserErrLog("in %s:: UnionSendToSocket ret = [%d]!\n",cliName,ret);
		return(ret);
	}
	// 循环处理
	if (finishThisTask)
		return(ret);
	else
	{
		UnionCloseDatabase(); // add by xusj 20091215
		goto loop;
	}
errorExit:
	if (ret >= 0)
		ret = errCodeAPIErrCodeNotSet;
	if (!recvFromClientOK)
		return(ret);
	lenOfRes = UnionTranslateErrCodeIntoRemark(ret,(char *)resStr,sizeof(resStr));
	finishThisTask = 1;
	goto sendToClient;
}

#ifndef _WIN32
void UnionDealTCPSvrJmpEnvForMaxIdleTime()
{
	UnionUserErrLog("gunionTCPSvrJmpEnvForEsscClient:: timeout!\n");
#if ( defined __linux__ ) || ( defined __hpux )
	siglongjmp(gunionTCPSvrJmpEnvForMaxIdleTime,10);
#elif ( defined _AIX )
	longjmp(gunionTCPSvrJmpEnvForMaxIdleTime,10);
#endif
}
#endif

