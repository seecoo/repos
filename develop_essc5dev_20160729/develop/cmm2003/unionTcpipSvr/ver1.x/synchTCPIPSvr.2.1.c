// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2011-1-23

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionREC.h"
#include "UnionTask.h"
#include "unionCommConf.h"
#include "unionREC.h"

#include "UnionLog.h"
#include "errCodeTranslater.h"
#include "unionFunSvrName.h"

#include "UnionSocket.h"
#include "synchTCPIPSvr.h"
#include "unionCommBetweenMDL.h"
#include "commWithTransSpier.h"
#include "unionServiceChanelRouter.h"
#include "externalPack-InternalPack-Trans.h"
#include "unionCommWithClient.h"
#include "commManager.h"

int UnionSynchTCPIPTaskServer(int handle,struct sockaddr_in *cli_addr,int port,int (*UnionTaskActionBeforeExit)())
{
	unsigned char	exteranlReqPack[conMaxSizeOfClientMsg+1];
	unsigned char	exteranlResPack[conMaxSizeOfClientMsg+1];
	unsigned char	interanlReqPack[conMaxSizeOfClientMsg+1];
	unsigned char	interanlResPack[conMaxSizeOfClientMsg+1];
	int		lenOfExternalReqPack,lenOfExternalResPack;
	int		lenOfInternalReqPack,lenOfInternalResPack;
	int		ret;
	int		maxIdleTimeOfSckConn,timeoutOfFunSvr;
	char		varName[100];
	long		currentServiceChanelID,defaultServiceChanelID;
		
	UnionSetActiveTCPIPSvrSckHDL(handle);
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
	defaultServiceChanelID = UnionGetFixedMDLIDOfFunSvr();
	
loop: // 循环处理客户端请求
	// 从客户端接收数据长度
	if ((ret = UnionRecvExternalPackFromClient(handle,exteranlReqPack,sizeof(exteranlReqPack),maxIdleTimeOfSckConn)) < 0)
	{
		UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionRecvExternalPackFromClient! ret = [%d]\n",ret);
		return(ret);
	}
	if (ret == 0)
	{
		UnionLog("in UnionSynchTCPIPTaskServer:: connection closed by peer!\n");
		//return(errCodeSJL08_SckClosedByClient);
		return(0);
	}
	lenOfExternalReqPack = ret;
	exteranlReqPack[lenOfExternalReqPack] = 0;
	
	// 将外部请求转换为内部请求
	if ((lenOfInternalReqPack = UnionTransExternalReqPackIntoInternalReqPack(exteranlReqPack,lenOfExternalReqPack,
		interanlReqPack,sizeof(interanlReqPack))) < 0)
	{
		UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionTransExternalReqPackIntoInternalReqPack! ret = [%d]\n",ret);
		return(lenOfInternalReqPack);
	}
		
	// 获取当前服务通道的标识
	if ((currentServiceChanelID = UnionGetServiceChanelID((char *)interanlReqPack,lenOfInternalReqPack)) <= 0)
		currentServiceChanelID = defaultServiceChanelID;

	// 执行服务
	if ((lenOfInternalResPack = UnionExchangeWithSpecModuleVerifyReqAndRes(currentServiceChanelID,interanlReqPack,lenOfInternalReqPack,
			interanlResPack,sizeof(interanlResPack),timeoutOfFunSvr)) < 0)
	{
		UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionExchangeWithSpecModuleVerifyReqAndRes! ret = [%d]\n",ret);
		return(lenOfInternalResPack);
	}
	
	// 将内部响应转换为外部响应
	if ((lenOfExternalResPack = UnionTransInternalResPackIntoExternalResPack(interanlResPack,lenOfInternalResPack,
		exteranlResPack,sizeof(exteranlResPack))) < 0)
	{
		UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionTransInternalResPackIntoExternalResPack! ret = [%d]\n",ret);
		return(lenOfExternalResPack);
	}
	ret = lenOfExternalResPack;
	exteranlResPack[lenOfExternalResPack] = 0;
	
sendToClient: 
	// 向客户端发送数据
	if ((ret = UnionSendExternalPackToClient(handle,exteranlResPack,lenOfExternalResPack)) < 0)
	{
		UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionSendExternalPackToClient ret = [%d]!\n",ret);
		return(ret);
	}
	goto loop;
}
