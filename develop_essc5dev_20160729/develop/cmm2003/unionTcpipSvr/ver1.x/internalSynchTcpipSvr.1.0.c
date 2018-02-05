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
#include "unionCommWithClient.h"
#include "commManager.h"

int UnionSynchTCPIPTaskServer(int handle,struct sockaddr_in *cli_addr,int port,int (*UnionTaskActionBeforeExit)())
{
	unsigned char	internalReqPack[conMaxSizeOfClientMsg+1];
	unsigned char	internalResPack[conMaxSizeOfClientMsg+1];
	int		lenOfInternalReqPack,lenOfInternalResPack;
	int		maxIdleTimeOfSckConn;
	int		ret;
	char		varName[100];
		
	UnionSetActiveTCPIPSvrSckHDL(handle);
	// 登记任务 
	if ((ret = UnionRegisterTcpipConnTask(cli_addr,port,UnionTaskActionBeforeExit)) < 0)
	{
		UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionRegisterTcpipConnTask!\n");
		return(ret);
	}
	// 读取连接空闲时间控制
	if ((maxIdleTimeOfSckConn = UnionReadIntTypeRECVar("maxIdleTimeOfSckConn")) < 0)
		maxIdleTimeOfSckConn = 1800;
	
loop: // 循环处理客户端请求
	// 从客户端接收数据长度
	if ((ret = UnionRecvInternalPackFromClient(handle,internalReqPack,sizeof(internalReqPack),maxIdleTimeOfSckConn)) < 0)
	{
		UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionRecvInternalPackFromClient! ret = [%d]\n",ret);
		return(ret);
	}
	if (ret == 0)
	{
		UnionLog("in UnionSynchTCPIPTaskServer:: connection closed by peer!\n");
		//return(errCodeSJL08_SckClosedByClient);
		return(0);
	}
	lenOfInternalReqPack = ret;
	internalReqPack[lenOfInternalReqPack] = 0;
	
	// 执行服务
	if ((lenOfInternalResPack = UnionDealServiceRequest(handle,internalReqPack,lenOfInternalReqPack,internalResPack,sizeof(internalResPack))) < 0)
	{
		UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionDealServiceRequest! ret = [%d]\n",ret);
		return(lenOfInternalResPack);
	}
	
	// 向客户端发送数据
	if ((ret = UnionSendInternalPackToClient(handle,internalResPack,lenOfInternalResPack)) < 0)
	{
		UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionSendInternalPackToClient ret = [%d]!\n",ret);
		return(ret);
	}
	goto loop;
}
