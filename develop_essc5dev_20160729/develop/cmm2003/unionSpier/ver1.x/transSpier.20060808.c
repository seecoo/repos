// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2005/12/30
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "UnionSocket.h"
#include "mngSvrServicePackage.h"
#include "unionResID.h"
#include "unionMngSvrBrothers.h"
#include "UnionLog.h"
#include "transSpierBuf.h"

// modify by liwj 将这个定义移动到 
/*
typedef struct
{
	int	used;
	char	ipAddr[15+1];
	int	port;
	int	sckHDL;
	time_t	lastConnFailTime;	// 上次通讯失败时间
} TUnionConn;
typedef TUnionConn	*PUnionConn;
*/

#define conMaxNumOfTransSpierConn	10
TUnionConn gunionTransSpierConn[conMaxNumOfTransSpierConn];
int gunionMonitorNum = 0;
int gunionLongConnWithMonitor = 0;

int	gunionIntervalTimeOfLastConnFail = 1800;

char *UnionGetMyMngSvrIDAtMonitor()
{
	return(getenv("myMngSvrIDAtMonitor"));
}

void UnionSetLongConnwithMonitor()
{
	gunionLongConnWithMonitor = 1;
	return;
}

void UnionSetIntervalTimeOfLastConnFail(int seconds)
{
	if (seconds > 0)
		gunionIntervalTimeOfLastConnFail = seconds;
}

int UnionReconnectMonitorConn(PUnionConn pconn)
{
	time_t	now;
	
	if (pconn == NULL)
		return(errCodeParameter);
	if (pconn->sckHDL >= 0)
		return(0);
	time(&now);
	if (now - pconn->lastConnFailTime < gunionIntervalTimeOfLastConnFail)	// 上次通讯失败不足半小时，不重建连
		return(errCodeParameter);
	if ((pconn->sckHDL = UnionCreateSocketClient(pconn->ipAddr,pconn->port)) < 0)
	{
		UnionUserErrLog("in UnionReconnectMonitorConn:: UnionCreateSocketClient [%s] [%d]\n",pconn->ipAddr,pconn->port);
		time(&(pconn->lastConnFailTime));
		return(pconn->sckHDL);
	}
	pconn->lastConnFailTime = 0;
	return(0);
}

int UnionCloseCommWithMonitor()
{
	int	index;

	for (index = 0; index < gunionMonitorNum; index++)
	{
		if (gunionTransSpierConn[index].sckHDL < 0)
			continue;
		UnionCloseSocket(gunionTransSpierConn[index].sckHDL);
		gunionTransSpierConn[index].sckHDL = -1;
		gunionTransSpierConn[index].lastConnFailTime = 0;
	}
	return(0);
}

int UnionInitCommWithMonitor()
{
	char	ipAddrGrp[conMaxNumOfTransSpierConn][15+1];
	int	portGrp[conMaxNumOfTransSpierConn];
	int	index;
	
	if (gunionMonitorNum > 0)
		return gunionMonitorNum;
		
	if ((gunionMonitorNum = UnionReadAllMonitorConf(ipAddrGrp,portGrp,conMaxNumOfTransSpierConn)) < 0)
	{
		UnionUserErrLog("in UnionInitCommWithMonitor:: UnionReadAllMonitorConf!\n");
		return(gunionMonitorNum);
	}
	for (index = 0; index < gunionMonitorNum; index++)
	{
		strcpy(gunionTransSpierConn[index].ipAddr,ipAddrGrp[index]);
		gunionTransSpierConn[index].port = portGrp[index];
		gunionTransSpierConn[index].sckHDL = -1;
		gunionTransSpierConn[index].lastConnFailTime = 0;
		UnionLog("ipaddr=[%s]port=[%d]\n",gunionTransSpierConn[index].ipAddr,gunionTransSpierConn[index].port);
	}
	UnionLog("in UnionInitCommWithMonitor:: [%d] monitorSvr read!\n",gunionMonitorNum);
	return(gunionMonitorNum);
}

int UnionSendTransInfoToMonitor(int resID,char *data,int len)
{
	int			ret;
	int			retryTimes = 0;
	char			tmpBuf[2048+1];
	int			lenOfData;
	int			index;
	char			*tellerNo;
	
	// 打请求包
	memset(tmpBuf, 0, sizeof(tmpBuf));
	if ((tellerNo = UnionGetMyMngSvrIDAtMonitor()) == NULL)
	{
		UnionUserErrLog("in UnionSendTransInfoToMonitor:: UnionGetMyMngSvrIDAtMonitor!!\n");
		return(errCodeParameter);
	}
	if ((lenOfData = UnionPackMngSvrRequestPackage(data,len,tellerNo,resID,1,tmpBuf+2,sizeof(tmpBuf)-2)) < 0)
	{
		UnionUserErrLog("in UnionSendTransInfoToMonitor:: UnionPackMngSvrRequestPackage! len = [%d]\n",len);
		return(lenOfData);
	}
	tmpBuf[0] = lenOfData / 256;
	tmpBuf[1] = lenOfData % 256;
	lenOfData += 2;
	
	// 建立通讯连接
	if ((ret = UnionInitCommWithMonitor()) < 0)
	{
		UnionUserErrLog("in UnionSendTransInfoToMonitor:: UnionInitCommWithMonitor!\n");
		return(ret);
	}
	
	// 向服务器发包
	for (index = 0; index < gunionMonitorNum; index++)
	{
		UnionLog("in UnionSendTransInfoToMonitor::ipAddr[%s]port[%d]\n",
			gunionTransSpierConn[index].ipAddr, gunionTransSpierConn[index].port);
		UnionLog("in UnionSendTransInfoToMonitor::[%04d][%s]\n", lenOfData-2, tmpBuf+2);
		
		retryTimes = 0;
loop:
		if (retryTimes >= 2)
			continue;	// 最多向一个服务器重发一次
		// 重连监控服务器
		if ((ret = UnionReconnectMonitorConn(&gunionTransSpierConn[index])) < 0)
			continue;
		if ((ret = UnionSendToSocket(gunionTransSpierConn[index].sckHDL,(unsigned char *)tmpBuf,lenOfData)) < 0)
		{
			UnionUserErrLog("in UnionSendTransInfoToMonitor:: UnionSendToSocket!\n");
			UnionCloseSocket(gunionTransSpierConn[index].sckHDL);
			gunionTransSpierConn[index].sckHDL = -1;
			retryTimes++;
			goto loop;	// 发送失败，重发
		}
	}
	if (!gunionLongConnWithMonitor)
		UnionCloseCommWithMonitor();	// 使用短连接与监控服务器通讯
	return(0);
}

int UnionSendTransInfoToMonitor2(char *mngSvrID, int resID, char *data, int len)
{
	int			ret;
	int			retryTimes = 0;
	char			tmpBuf[2048+1];
	int			lenOfData;
	int			index;
	//char			*tellerNo;
	
	// 打请求包
	memset(tmpBuf, 0, sizeof(tmpBuf));
	/*
	if ((tellerNo = UnionGetMyMngSvrIDAtMonitor()) == NULL)
	{
		UnionUserErrLog("in UnionSendTransInfoToMonitor:: UnionGetMyMngSvrIDAtMonitor!!\n");
		return(errCodeParameter);
	}
	*/
	if ((lenOfData = UnionPackMngSvrRequestPackage(data,len,mngSvrID,resID,1,tmpBuf+2,sizeof(tmpBuf)-2)) < 0)
	{
		UnionUserErrLog("in UnionSendTransInfoToMonitor2:: UnionPackMngSvrRequestPackage! len = [%d]\n",len);
		return(lenOfData);
	}
	tmpBuf[0] = lenOfData / 256;
	tmpBuf[1] = lenOfData % 256;
	lenOfData += 2;
	
	// 建立通讯连接
	if ((ret = UnionInitCommWithMonitor()) < 0)
	{
		UnionUserErrLog("in UnionSendTransInfoToMonitor2:: UnionInitCommWithMonitor!\n");
		return(ret);
	}
	
	// 向服务器发包
	for (index = 0; index < gunionMonitorNum; index++)
	{
		UnionLog("in UnionSendTransInfoToMonitor2::ipAddr[%s]port[%d]\n",
			gunionTransSpierConn[index].ipAddr, gunionTransSpierConn[index].port);
		UnionLog("in UnionSendTransInfoToMonitor2::[%04d][%s]\n", lenOfData-2, tmpBuf+2);
		
		retryTimes = 0;
loop:
		if (retryTimes >= 2)
			continue;	// 最多向一个服务器重发一次
		// 重连监控服务器
		if ((ret = UnionReconnectMonitorConn(&gunionTransSpierConn[index])) < 0)
			continue;
		if ((ret = UnionSendToSocket(gunionTransSpierConn[index].sckHDL,(unsigned char *)tmpBuf,lenOfData)) < 0)
		{
			UnionUserErrLog("in UnionSendTransInfoToMonitor2:: UnionSendToSocket!\n");
			UnionCloseSocket(gunionTransSpierConn[index].sckHDL);
			gunionTransSpierConn[index].sckHDL = -1;
			retryTimes++;
			goto loop;	// 发送失败，重发
		}
	}
	if (!gunionLongConnWithMonitor)
		UnionCloseCommWithMonitor();	// 使用短连接与监控服务器通讯
	return(0);
}
