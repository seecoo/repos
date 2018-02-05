// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2011-1-23

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#include "unionServiceChanelRouter.h"

#ifndef _WIN32
int gsynchTCPIPSvrSckHDL = -1;
extern PUnionTaskInstance 	ptaskInstance;

#ifndef _noRegisterClient_
PUnionCommConf			pgunionCommConf = NULL;
#endif

int UnionGetActiveTCIPSvrSckHDL()
{
	return(gsynchTCPIPSvrSckHDL);
}
#endif

/*
功能
	登记任务
输入参数
	cli_addr	客户端信息
	port		客户端端口
输出参数
	无
返回值
	>=0		成功
	<0		失败，错误码
*/
int UnionRegisterTcpipConnTask(struct sockaddr_in *cli_addr,int port,int (*UnionTaskActionBeforeExit)())
{
	int		ret;
	char		cliName[100];
	char		cliIPAddr[40+1];

	memset(cliIPAddr,0,sizeof(cliIPAddr));
	inet_ntop(AF_INET, (void *)&cli_addr->sin_addr, cliIPAddr, sizeof(cliIPAddr));
	ret = sprintf(cliName,"%s %d",cliIPAddr,port);
	cliName[ret] = 0;
#ifndef _noSpierInfo_
	UnionSetClientAttrToldTransSpier(cliIPAddr,port);
#endif

	// 创建任务实例
	if (UnionExistsTaskClassOfName(cliName))
	{
		if ((ptaskInstance = UnionCreateTaskInstanceOfName(UnionTaskActionBeforeExit,cliName)) == NULL)
		{
			UnionUserErrLog("in %s:: UnionCreateTaskInstanceOfName! ret = [%d]\n",cliName,ret);
			return(UnionSetUserDefinedErrorCode(errCodeCreateTaskInstance));
		}
	}
	else
	{
		if ((ptaskInstance = UnionCreateTaskInstanceOfName(UnionTaskActionBeforeExit,"client %d",port)) == NULL)
		{
			ret = errCodeCreateTaskInstance;
			UnionUserErrLog("in %s:: UnionCreateTaskInstanceOfName! ret = [%d]\n",cliName,ret);
			return(UnionSetUserDefinedErrorCode(errCodeCreateTaskInstance));
		}
	}
#ifndef _noRegisterClient_
	if ((pgunionCommConf = UnionAddServerCommConf(cliIPAddr,port,conCommShortConn,"安全服务")) == NULL)
	{
#ifdef _limitClient_
		UnionUserErrLog("in %s:: UnionAddServerCommConf [%s] [%d]! ret = [%d]\n",cliName,cliIPAddr,port,ret);
		return(UnionSetUserDefinedErrorCode(errCodeYLCommConfMDL_AddCommConf));
#endif
	}
	UnionSetCommConfOK(pgunionCommConf);
#endif

	return(0);
}

int UnionSynchTCPIPTaskServer(int handle,struct sockaddr_in *cli_addr,int port,int (*UnionTaskActionBeforeExit)())
{
	unsigned char	reqStr[conMaxSizeOfClientMsg+1];
	unsigned char	resStr[conMaxSizeOfClientMsg+1];
	int		lenOfReq,lenOfRes;
	int		ret;
	int		maxIdleTimeOfSckConn,timeoutOfFunSvr;
	char		varName[100];
	long		currentServiceID,defaultServiceID;
		
	gsynchTCPIPSvrSckHDL = handle;
	//UnionSetMyTransInfoResID(conEssResIDSecurityService);
	// 登记任务 
	if ((ret = UnionRegisterTcpipConnTask(cli_addr,port,UnionTaskActionBeforeExit)) < 0)
	{
		UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionRegisterTcpipConnTask!\n");
		return(ret);
	}
	// 读取连接空闲时间控制
	if ((maxIdleTimeOfSckConn = UnionReadIntTypeRECVar("maxIdleTimeOfSckConn")) < 0)
		maxIdleTimeOfSckConn = 1800;
	// 读取服务超时时间
	sprintf(varName,"timeoutOf%s",UnionGetFunSvrName());
	if ((timeoutOfFunSvr = UnionReadIntTypeRECVar(varName)) < 0)
		timeoutOfFunSvr = 3;
	// 获取缺省服务标识
	defaultServiceID = UnionGetFixedMDLIDOfFunSvr();
	
loop: // 循环处理客户端请求
	// 从客户端接收数据长度
	if ((ret = UnionReceiveFromSocketWith2BytesLen(handle,reqStr,sizeof(reqStr),maxIdleTimeOfSckConn)) < 0)
	{
		UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionReceiveFromSocketWith2BytesLen! ret = [%d]\n",ret);
		return(ret);
	}
	if (ret == 0)
	{
		UnionLog("in UnionSynchTCPIPTaskServer:: connection closed by peer!\n");
		//return(errCodeSJL08_SckClosedByClient);
		return(0);
	}
	lenOfReq = ret;
	reqStr[lenOfReq] = 0;
	
#ifndef _noRegisterClient_	// 将客户端包数目加1
	UnionIncreaseCommConfWorkingTimes(pgunionCommConf);
#endif

#ifdef _client_use_ebcdic_
	// 客户端使用ebcdic码通讯
	UnionEbcdicToAscii(reqStr,reqStr,lenOfReq);
#endif		

#ifndef _noSpierInfo_
	UnionSendRequestInfoToTransSpier(lenOfReq,reqStr);	// 监控这个消息
#else
	UnionNullLog("[%04d][%s]\n",lenOfReq,reqStr);
#endif		

	// 获取当前服务的标识
	if ((currentServiceID = UnionGetServiceChanelID((char *)reqStr,lenOfReq)) <= 0)
		currentServiceID = defaultServiceID;
	// 执行服务
	if ((lenOfRes = UnionExchangeWithSpecModuleVerifyReqAndRes(currentServiceID,reqStr,lenOfReq,
			resStr,sizeof(resStr),timeoutOfFunSvr)) < 0)
	{
		UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionExchangeWithSpecModuleVerifyReqAndRes! ret = [%d]\n",ret);
		return(lenOfRes);
	}
	ret = lenOfRes;
	resStr[lenOfRes] = 0;
	
sendToClient: 
#ifndef _noSpierInfo_
	UnionSendResponseInfoToTransSpier(lenOfRes,resStr); // 监控这个消息
#else
	UnionNullLog("[%04d][%s]\n",lenOfRes,resStr);
#endif	

#ifdef _client_use_ebcdic_	// 进行码制转换
	UnionAsciiToEbcdic(resStr,resStr,lenOfRes);
#endif	


	if ((ret = UnionSendToSocketWithTowBytesLen(handle,resStr,lenOfRes)) < 0)
	{
		UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionSendToSocketWithTowBytesLen ret = [%d]!\n",ret);
		return(ret);
	}
	goto loop;
}

