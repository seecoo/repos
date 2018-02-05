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
	char    varName[100];
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
	char			taskName[40];
	int			childPid;
	int			index;
	int			minTaskNum = 5;
	int			cliNum = 0;
	int			isChildLoop = 1;
	char			cliName[40+1];
		
	UnionSetApplicationName(argv[0]);
		
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);
	
	if (argc < 2)
		return(UnionHelp());

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

	// 以下代码父进程执行
loop:
	// 计算子进程实际数目
	if ((minTaskNum = UnionReadIntTypeRECVar("minProcNumOfTcpipSvrTask")) <= 0)
		minTaskNum = 10;
	//UnionProgramerLog("in %s:: minTaskNum = [%d]\n",minTaskNum); 
	if (minTaskNum > conMinProcNum)
		minTaskNum = conMinProcNum;
	
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
	memset(cliName,0,sizeof(cliName));
	sprintf(cliName,"%s client %d",UnionGetApplicationName(),atoi(argv[1]));
	
	if ((cliNum = UnionExistsTaskOfName(cliName)) < 0)
		goto loop;

	// 空闲子进程小于10个时，创建临时进程
	for (index = 0; index < 10 - cliNum; index++)
	{
		// 创建子进程
		isChildLoop = 0;
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
		UnionTcpipSvrTask(atoi(argv[1]),argv[2]);
	else
		UnionTcpipSvrTask(atoi(argv[1]),NULL);
	
	// 如果空闲进程数 < 10 继续使用
	if ((cliNum = UnionExistsTaskOfName(cliName)) < 10)
		goto callChildProcess;
	
	// 释放信号量
	//sem_post(&gunionSemID);
	if (isChildLoop)
		goto callChildProcess;
	else
		return(UnionTaskActionBeforeExit());
}

