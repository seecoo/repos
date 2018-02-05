// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2005/12/30
// Version:	1.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#ifdef _SCO_Unix_	// 2007/11/29修改
#include <sys/semaphore.h>
#else
#include <semaphore.h>
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#ifndef _UnionSocket_3_x_
#define _UnionSocket_3_x_
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_   
#endif

#include "UnionSocket.h"
#include "synchTCPIPSvr.h"
#include "unionMDLID.h"
#include "UnionTask.h"
#include "unionCommConf.h"
#include "unionFunSvrName.h"
#include "unionREC.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionVersion.h"
#include "unionCommand.h"
#include "unionMsgBuf.h"
#include "unionHighCachedAPI.h"
#include "unionRealBaseDB.h"
#include "unionRealDBCommon.h"

extern PUnionCommConf	pgunionCommConf;
#define	conMinProcNum	1000

PUnionTaskInstance	ptaskInstance = NULL;
int			gunionBoundSckHDL = -1;
int			gunionSemIDInitialized = 0;
//sem_t			gunionSemID;
int			gunionFatherProcID;
int			gunionChildPidGrp[conMinProcNum];
int			gunionIsInitFixedMDLID = 0;

int UnionTaskActionBeforeExit()
{
	int	sckHDL;
	int	index;
	
	if ((gunionSemIDInitialized) && (gunionFatherProcID == getpid()))
	{
		//sem_destroy(&gunionSemID);
		for (index = 0; index < conMinProcNum; index++)
		{
			if (gunionChildPidGrp[index] > 0)
				kill(gunionChildPidGrp[index],9);
		}
	}
	if (gunionBoundSckHDL >= 0)
		UnionCloseSocket(gunionBoundSckHDL);
	if ((sckHDL = UnionGetActiveTCIPSvrSckHDL()) >= 0)
		UnionCloseSocket(sckHDL);

	UnionSetCommConfAbnormal(pgunionCommConf);	
	UnionDisconnectMsgBufMDL();
        UnionCloseDatabase();
	UnionCloseHighCachedService();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s port [funSvrName]\n",UnionGetApplicationName());
	return(0);
}

int UnionTcpipSvrTask(int port,char *funSvr)
{
	int			ret;
	unsigned int		clilen;
	struct sockaddr_in	cli_addr;
	int			sckinstance;
	struct linger		Linger;	// Added by Wolfgang Wang, 2004/7/13
	char			varName[100];
	
	memset(varName, 0, sizeof varName);
	
	if ((gunionIsInitFixedMDLID <= 0) && (funSvr != NULL))
	{
		UnionSetFunSvrName(funSvr);
		sprintf(varName,"fixedMDLIDOf%s",funSvr);
		UnionSetFixedMDLIDOfFunSvr(UnionReadLongTypeRECVar(varName));
		UnionSetMyModuleType(conMDLTypeUnionLongConnTcpipSvr);
	}

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s client %d",UnionGetApplicationName(),port)) == NULL)
	{
		UnionUserErrLog("in UnionTcpipSvrTask:: UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	clilen = sizeof(cli_addr);
	sckinstance = accept(gunionBoundSckHDL, (struct sockaddr *)&cli_addr,&clilen);
	if ( sckinstance < 0 )
	{
		UnionSystemErrLog("in UnionTcpipSvrTask:: accept()!\n");
		//if (errno == 22)	// 2006/10/27 增加
		//	break;
		return(errCodeUseOSErrCode);
	}
	Linger.l_onoff = 0;
	Linger.l_linger = 0;
	if (setsockopt(sckinstance,SOL_SOCKET,SO_LINGER,(char *)&Linger,sizeof(Linger)) != 0)
	{
		UnionSystemErrLog("in UnionTcpipSvrTask:: setsockopt linger!");
		close(sckinstance);
		return(errCodeUseOSErrCode);
	}
	ret = UnionSynchTCPIPTaskServer(sckinstance,&cli_addr,port,UnionTaskActionBeforeExit);
	UnionCloseSocket(sckinstance);
	return(ret);
}

int main(int argc,char *argv[])
{
	int			ret;
	char			taskName[256+1];
	int			childPid;
	int			index;
	int			minTaskNum = 5;
	int			realCliNum = 0;
	int			minFreeCliNum = 0;
	int			isChildLoop = 1;
	char			cliName[256+1];
		
	UnionSetApplicationName(argv[0]);
		
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);
	
	if (argc < 2)
		return(UnionHelp());

	memset(taskName,0,sizeof(taskName));
	sprintf(taskName,"%s %d",UnionGetApplicationName(),atoi(argv[1]));
	
	if (UnionExistsAnotherTaskOfName(taskName) > 0)
	{
		printf("Another task of name [%s] already exists!\n",taskName);
		return(-1);
	}

	gunionFatherProcID = getpid();
	// 初始化子进程表
	for (index = 0; index < conMinProcNum; index++)
		gunionChildPidGrp[index] = -1;
	
	if (UnionCreateProcess() > 0)
		return(0);
		
	UnionSetMyModuleType(conMDLTypeUnionLongConnTcpipSvr);
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,taskName)) == NULL)
	{
		UnionPrintf("in %s:: UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}

	if ((ret = UnionConnectRunningEnv()) < 0)
	{
		UnionPrintf("in %s:: UnionConnectRunningEnv!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	// 初始化一个信号量
	/*
	UnionProgramerLog("begin to init sem...\n");
        if (sem_init(&gunionSemID,1,100))
        {
		UnionPrintf("in %s:: sem_init Error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	*/
	gunionSemIDInitialized = 1;
	//UnionProgramerLog("init sem ok!\n");

	// 初始化一个侦听端口
	if ((gunionBoundSckHDL = UnionInitializeTCPIPServer(atoi(argv[1]))) < 0)
	{
		UnionPrintf("in %s:: UnionInitializeTCPIPServer Error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	UnionSuccessLog("a servier bound to [%d] started OK!\n",atoi(argv[1]));

	ret = sprintf(cliName,"%s client %d",UnionGetApplicationName(),atoi(argv[1]));
	cliName[ret] = 0;

	// 以下代码父进程执行
loop:
	// 计算子进程实际数目
	if ((minTaskNum = UnionReadIntTypeRECVar("minProcNumOfTcpipSvrTask")) <= 0)
		minTaskNum = 10;

	if (minTaskNum > conMinProcNum)
		minTaskNum = conMinProcNum;
	
	if (minTaskNum > 5)
		minFreeCliNum = 5;
	else
		minFreeCliNum = minTaskNum;
	
	// 维护最少的进程数
	for (index = 0; index < minTaskNum; index++)
	{
		if (gunionChildPidGrp[index] > 0)
		{
			if (kill(gunionChildPidGrp[index],0) == 0)	// 进程存在
				continue;
		}
		// 创建子进程
		isChildLoop = 1;
		if ((childPid = UnionCreateProcess()) == 0)	// 是子进程
			goto callChildProcess;
		UnionProgramerLog("in %s:: create child %d port = %d!\n",UnionGetApplicationName(),childPid,atoi(argv[1]));
		gunionChildPidGrp[index] = childPid;
	}

	// 2013-05-14 张永定增加
	
	if ((realCliNum = UnionExistsTaskOfName(cliName)) < 0)
		goto loop;

	// 空闲子进程不足，创建临时进程
	for (index = 0; index < minFreeCliNum - realCliNum; index++)
	{
		// 创建子进程
		isChildLoop = 0;
		
		// 关闭
		UnionCloseHighCachedService();

		if ((childPid = UnionCreateProcess()) == 0)	// 是子进程
			goto callChildProcess;
		UnionProgramerLog("in %s:: create tmp child %d port = %d!\n",UnionGetApplicationName(),childPid,atoi(argv[1]));
	}
	// 2013-05-14 张永定增加结束
	sleep(1);
	goto loop;
	return(UnionTaskActionBeforeExit());
	
	// 子进程执行以下代码
callChildProcess:
	// 获取信号量
	//sem_wait(&gunionSemID);
	if (argc > 2)
		ret = UnionTcpipSvrTask(atoi(argv[1]),argv[2]);
	else
		ret = UnionTcpipSvrTask(atoi(argv[1]),NULL);

	// added 2013-12-05
	if(ret < 0)
	{
		return(UnionTaskActionBeforeExit());
	}
	
	// 如果实际空闲进程数 < 最小空闲进程数 继续使用
	if (UnionExistsTaskOfName(cliName) < minFreeCliNum)
		goto callChildProcess;

	// 释放信号量
	//sem_post(&gunionSemID);
	if (isChildLoop)
		goto callChildProcess;
	else
		return(UnionTaskActionBeforeExit());
}

