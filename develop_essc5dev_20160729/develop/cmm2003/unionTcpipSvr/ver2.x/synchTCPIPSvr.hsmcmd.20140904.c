// Copyright: Union Tech.
// Author:	Wolfgang Wang
// Date:	2008/5/11

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#define _realBaseDB_2_x_

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
#include "UnionStr.h"
#include "errCodeTranslater.h"
#include "unionFunSvrName.h"
#include "unionMsgBufGroup.h"
#include "commWithHsmSvr.h"

#include "unionVersion.h"
#include "UnionSocket.h"
#include "synchTCPIPSvr.h"
#include "unionCommBetweenMDL.h"
#include "commWithTransSpier.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "packageConvert.h"
#include "registerRunningTime.h"
#include "unionHighCachedAPI.h"
#include "unionTableData.h"

#ifndef _WIN32
int				gsynchTCPIPSvrSckHDL = -1;
extern PUnionTaskInstance 	ptaskInstance;

#ifndef _noRegisterClient_
PUnionCommConf			pgunionCommConf = NULL;
#endif

jmp_buf gunionTCPSvrJmpEnvForEsscClient;
jmp_buf gunionTCPSvrJmpEnvForMaxIdleTime;

void UnionDealTCPSvrJmpEnvForEsscClient();
void UnionDealTCPSvrJmpEnvForMaxIdleTime();
int getMaxConnNumByHsmGropID();

int UnionFunSvrInterprotor(char *cliIPAddr,int port,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr);

int UnionGetActiveTCIPSvrSckHDL()
{
	return(gsynchTCPIPSvrSckHDL);
}
#endif
static int gunionIsInitFunStr = 0;
static int gunionMaxSleepTime  = 0;
static int gunionSleepRandRange  = 0;
static char gunionSleepEndTime[16]="";
static char gunionSleepStartTime[16]="";

// 返回延时时间
static void union_sleep_time()
{
	char	fldGrp[4][32];
	int	fldNum = 0;
	int	lenOfFldStr = 0;
	char	fldStr[128];
	char	dateTime[16];
	int 	sleepTime  = 0;
	int	randSleepTime = 0;

	lenOfFldStr = snprintf(fldStr,sizeof(fldStr),"%s",UnionGetFunSvrStartVar());

	if (lenOfFldStr <= 0)
		return;

	UnionGetFullSystemDateTime(dateTime);
	dateTime[14] = 0;
	if (!gunionIsInitFunStr)
	{
		// 拼分域定义串
		if ((fldNum = UnionSeprateVarStrIntoArray(fldStr,lenOfFldStr,'-',(char *)&fldGrp,4,32)) < 0)
		{
			UnionUserErrLog("in union_sleep_time:: UnionSeprateVarStrIntoArray [%04d][%s]!\n",lenOfFldStr,fldStr);
			return;
		}

		UnionGetTimeAfterSpecTime(dateTime,atoi(fldGrp[0]),gunionSleepStartTime);

		gunionMaxSleepTime = atoi(fldGrp[1]);

		UnionGetTimeAfterSpecTime(gunionSleepStartTime,atoi(fldGrp[2]),gunionSleepEndTime);

		gunionSleepRandRange = atoi(fldGrp[3]);
		gunionIsInitFunStr = 1;
	}

	srand((unsigned)time(NULL));
	randSleepTime = (int)(1.0 * gunionSleepRandRange * rand() / (RAND_MAX));
	sleepTime = gunionMaxSleepTime - randSleepTime;
	
	if ((strcmp(dateTime,gunionSleepStartTime) > 0) && (strcmp(dateTime,gunionSleepEndTime) < 0))
	{
		UnionDebugLog("in union_sleep_time:: sleep [%d]ms!\n",sleepTime);
		usleep(sleepTime*1000);
	}
}

int UnionSynchTCPIPTaskServer(int handle,struct sockaddr_in *cli_addr,int port,int (*UnionTaskActionBeforeExit)())
{
	unsigned char	reqStr[conMaxSizeOfClientMsg];
	unsigned char	resStr[conMaxSizeOfClientMsg];
	int		lenOfReq = 0;
	int		lenOfRes = 0;
	int		ret;
	//unsigned long	len = 0;
	int		timeoutOfClient = 0;
	int		maxIdleTimeOfSckConn = 0;
	char		cliName[256];
	char		taskName[256];
	char		cliIPAddr[64];
	//int		finishThisTask = 0;
	int		recvFromClientOK = 0;
	int		lenOfLengthField = 2;
	int		commType;
	char		dateTime[16];	// add by leipp 20160617 获取当前时间

	memset(cliIPAddr,0,sizeof(cliIPAddr));
	inet_ntop(AF_INET, (void *)&cli_addr->sin_addr, cliIPAddr, sizeof(cliIPAddr));

	snprintf(cliName,sizeof(cliName),"%s %d",cliIPAddr,port);
	snprintf(taskName,sizeof(taskName),"%s client %d",UnionGetApplicationName(),port);


#ifndef _WIN32		
	gsynchTCPIPSvrSckHDL = handle;
	// 创建任务实例
	/*
	if (UnionExistsTaskClassOfName(taskName))
	{
		if ((ptaskInstance = UnionCreateTaskInstanceOfName(UnionTaskActionBeforeExit,cliName)) == NULL)
		{
			ret = errCodeCreateTaskInstance;
			UnionUserErrLog("in %s:: UnionCreateTaskInstanceOfName! ret = [%d]\n",cliName,ret);
			goto errorExit;
		}
		commType = conCommShortConn;
	}
	else
	{
		if ((ptaskInstance = UnionCreateTaskInstanceOfName(UnionTaskActionBeforeExit,"%s %d",UnionGetApplicationName(),port)) == NULL)
		{
			ret = errCodeCreateTaskInstance;
			UnionUserErrLog("in %s:: UnionCreateTaskInstanceOfName! ret = [%d]\n",cliName,ret);
			goto errorExit;
		}
	*/
		// 2015-10-15 增加IP地址注册,zhangyd 
		if ((ptaskInstance = UnionCreateTaskInstanceForClient(UnionTaskActionBeforeExit,"%s busy %d %s",UnionGetApplicationName(),port,cliIPAddr)) == NULL)
		{
			ret = errCodeCreateTaskInstance;
			UnionUserErrLog("in %s:: UnionCreateTaskInstanceForClient! ret = [%d]\n",cliName,ret);
			goto errorExit;
		}
		commType = conCommShortConn;
	//}
#ifndef _noRegisterClient_
	if ((pgunionCommConf = UnionAddServerCommConf(cliIPAddr,port,commType,"通讯服务")) == NULL)
	{
#ifdef _limitClient_
		ret = errCodeYLCommConfMDL_AddCommConf;
		UnionUserErrLog("in %s:: UnionAddServerCommConf [%s] [%d]! ret = [%d]\n",cliName,cliIPAddr,port,ret);
		goto errorExit;
#endif
	}
	UnionSetCommConfOK(pgunionCommConf);
#endif
#endif

loop:	// 循环处理客户端请求
	recvFromClientOK = 0;

	// 读取超时值
	if ((timeoutOfClient = UnionReadIntTypeRECVar("timeoutOfRecvClientData")) < 0)
		timeoutOfClient = 3;

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
	lenOfLengthField = UnionGetLenOfLenFieldForApp();
	if (lenOfLengthField == 0)
	{
#ifndef _WIN32
		alarm(0);
#endif

#ifdef _WIN32
		// addcodes here
#else
#if ( defined __linux__ ) || ( defined __hpux )
		if (sigsetjmp(gunionTCPSvrJmpEnvForEsscClient,1) != 0)	// 超时退出
#elif ( defined _AIX )
		if (setjmp(gunionTCPSvrJmpEnvForEsscClient) != 0) // 超时退出
		{
			ret = errCodeSocketMDL_RecvDataTimeout;
			UnionUserErrLog("in %s:: recv from client time out! ret = [%d]\n",cliName,ret);
			goto errorExit;
		}
#endif
		alarm(timeoutOfClient);
		signal(SIGALRM,UnionDealTCPSvrJmpEnvForEsscClient);
#endif
		if ((ret = UnionReceiveFromSocket(handle,reqStr,8192)) < 0)
		{
			UnionUserErrLog("in %s:: UnionReceiveFromSocket! ret = [%d]\n",cliName,ret);
			goto errorExit;
		}
		// 重置当前时间
		UnionReInitUserSetTime();
		if (ret == 0)
		{
			UnionProgramerLog("in %s:: connection closed by peer!\n",cliName);
			UnionSetCommConfAbnormal(pgunionCommConf);
			return(0);
		}
		reqStr[ret] = 0;
		lenOfReq = ret;
#ifndef _WIN32
		alarm(0);
#ifndef _noRegisterClient_
		UnionIncreaseCommConfWorkingTimes(pgunionCommConf);
#endif
#endif
	}
	else
	{
		// 接收数据长度
		if ((ret = UnionReceiveFromSocketUntilLen(handle,reqStr,2)) < 0)
		{
			UnionUserErrLog("in %s:: UnionReceiveFromSocketUntilLen %d! ret = [%d]\n",cliName,2,ret);
			goto errorExit;
		}
		if (memcmp(reqStr,"00",2) == 0)		// 长度域是8个字节
		{
			if ((ret = UnionReceiveFromSocketUntilLen(handle,reqStr,6)) < 0)
			{
				UnionUserErrLog("in %s:: UnionReceiveFromSocketUntilLen %d! ret = [%d]\n",cliName,6,ret);
				goto errorExit;
			}
			lenOfLengthField = 8;
		}
		else
			lenOfLengthField = 2;

		// 重置当前时间
		UnionReInitUserSetTime();


#ifndef _WIN32
		alarm(0);
#endif
		if (ret == 0)
		{
			UnionProgramerLog("in %s:: connection closed by peer!\n",cliName);
			UnionSetMultiLogEnd();
			return(0);
		}
		if (lenOfLengthField == 2)
			lenOfReq = reqStr[0] * 256 + reqStr[1];
		else
			lenOfReq = atol((char *)reqStr);
	
		if ((lenOfReq  < 0) || (lenOfReq >= (int)sizeof(reqStr)))
		{
			ret = errCodeSocketMDL_DataLen;
			UnionUserErrLog("in %s:: lenOfReq = [%d] error! ret = [%d]\n",cliName,lenOfReq,ret);
			goto errorExit;
		}
		if (lenOfReq == 0)	// 测试报文
		{
			UnionProgramerLog("in %s:: testing pack received!\n",cliName);
			lenOfRes = 0;
			goto sendToClient;
		}
		// 设置接收客户端数据的超时机制
#ifdef _WIN32
		// addcodes here
#else
#if ( defined __linux__ ) || ( defined __hpux )
		if (sigsetjmp(gunionTCPSvrJmpEnvForEsscClient,1) != 0)	// 超时退出
#elif ( defined _AIX )
		if (setjmp(gunionTCPSvrJmpEnvForEsscClient) != 0) // 超时退出
		{
			ret = errCodeSocketMDL_RecvDataTimeout;
			UnionUserErrLog("in %s:: recv from client time out! ret = [%d]\n",cliName,ret);
			goto errorExit;
		}
#endif
		alarm(timeoutOfClient);
		signal(SIGALRM,UnionDealTCPSvrJmpEnvForEsscClient);
#endif
		// 接收客户端数据
		if ((ret = UnionReceiveFromSocketUntilLen(handle,reqStr,lenOfReq)) != lenOfReq)
		{
			UnionUserErrLog("in %s:: UnionReceiveFromSocketUntilLen expected [%d] real = [%d]!\n",cliName,lenOfReq,ret);
			ret = errCodeSocketMDL_DataLen;
			goto errorExit;
		}
		reqStr[ret] = 0;
		recvFromClientOK = 1;
#ifndef _WIN32
		alarm(0);
#ifndef _noRegisterClient_
		UnionIncreaseCommConfWorkingTimes(pgunionCommConf);
#endif
#endif
	}
	
	UnionSetMultiLogBegin();
	SetErrorLogAloneFile();	// add by leipp 20151112
	UnionGetFullSystemDateTime(dateTime);
	dateTime[14] = 0;
	if ((lenOfRes = UnionFunSvrInterprotor(cliIPAddr,port,reqStr,lenOfReq,resStr + lenOfLengthField,sizeof(resStr) - lenOfLengthField)) < 0)
	{
		UnionUserErrLog("in %s:: UnionFunSvrInterprotor,ret = [%d]!\n",cliName,lenOfRes);
	  	//add begin by lusj 21051117 当中断时返回信息为:1234578XX99SocketStop
		resStr[0]=0;
		resStr[1]=22; 
		resStr[24]=0;
		// 设置睡眠时间	// add by leipp 20160617
		union_sleep_time();
		if ((ret = UnionSendToSocket(handle,resStr,24)) < 0)
        	{
                	UnionUserErrLog("in %s:: UnionSendToSocket ret = [%d]!\n",cliName,ret);
        	}
		//add end by lusj 20151117	
		ret = lenOfRes;
		goto errorExit;
	}

sendToClient: // 向客户端发送数据

	if (lenOfLengthField == 2)
	{
		resStr[0] = lenOfRes / 256;
		resStr[1] = lenOfRes % 256;
	}
	else if (lenOfLengthField == 0)
	{
		// 没有长度域
	}
	else
	{
		sprintf((char *)resStr,"%0*d",lenOfLengthField,lenOfRes);
	}
	resStr[lenOfRes+lenOfLengthField] = 0;
	
	// 设置睡眠时间	// add by leipp 20160617
	union_sleep_time();

	if ((ret = UnionSendToSocket(handle,resStr,lenOfRes+lenOfLengthField)) < 0)
	{
		UnionUserErrLog("in %s:: UnionSendToSocket ret = [%d]!\n",cliName,ret);
		UnionSetMultiLogEnd();
		return(ret);
	}
	UnionSetMultiLogEnd();


	goto loop;
errorExit:
	if (ret >= 0)
		ret = errCodeSocketMDL_ErrCodeNotSet;

	if (!recvFromClientOK)
	{
		UnionSetMultiLogEnd();
		return(ret);
	}

	//finishThisTask = 1;
	goto sendToClient;
}

#ifndef _WIN32
void UnionDealTCPSvrJmpEnvForEsscClient()
{
	UnionUserErrLog("gunionTCPSvrJmpEnvForEsscClient:: timeout!\n");
#if ( defined __linux__ ) || ( defined __hpux )
	siglongjmp(gunionTCPSvrJmpEnvForEsscClient,10);
#elif ( defined _AIX )
	longjmp(gunionTCPSvrJmpEnvForEsscClient,10);
#endif
}

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

