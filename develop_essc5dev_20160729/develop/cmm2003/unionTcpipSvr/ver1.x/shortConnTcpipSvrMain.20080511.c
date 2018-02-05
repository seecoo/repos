// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2005/12/30
// Version:	1.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#ifdef _SCO_Unix_	// 2007/11/29�޸�
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
		//if (errno == 22)	// 2006/10/27 ����
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
	// ��ʼ���ӽ��̱�
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
	
	// ��ʼ��һ���ź���
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

	// ��ʼ��һ�������˿�
	if ((gunionBoundSckHDL = UnionInitializeTCPIPServer(atoi(argv[1]))) < 0)
	{
		UnionPrintf("in %s:: UnionInitializeTCPIPServer Error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	UnionSuccessLog("a servier bound to [%d] started OK!\n",atoi(argv[1]));

	// ���´��븸����ִ��
loop:
	// �����ӽ���ʵ����Ŀ
	if ((minTaskNum = UnionReadIntTypeRECVar("minProcNumOfTcpipSvrTask")) <= 0)
		minTaskNum = 10;
	//UnionProgramerLog("in %s:: minTaskNum = [%d]\n",minTaskNum); 
	if (minTaskNum > conMinProcNum)
		minTaskNum = conMinProcNum;
	
	// ά�����ٵĽ�����
	for (index = 0; index < minTaskNum; index++)
	{
		if (gunionChildPidGrp[index] > 0)
		{
			if (kill(gunionChildPidGrp[index],0) == 0)	// ���̴���
				continue;
		}
		// �����ӽ���
		isChildLoop = 1;
		if ((childPid = UnionCreateProcess()) == 0)	// ���ӽ���
			goto callChildProcess;
		UnionProgramerLog("in %s:: create child %d port = %d!\n",UnionGetApplicationName(),childPid,atoi(argv[1]));
		gunionChildPidGrp[index] = childPid;
	}

	// 2013-05-14 ����������
	memset(cliName,0,sizeof(cliName));
	sprintf(cliName,"%s client %d",UnionGetApplicationName(),atoi(argv[1]));
	
	if ((cliNum = UnionExistsTaskOfName(cliName)) < 0)
		goto loop;

	// �����ӽ���С��10��ʱ��������ʱ����
	for (index = 0; index < 10 - cliNum; index++)
	{
		// �����ӽ���
		isChildLoop = 0;
		if ((childPid = UnionCreateProcess()) == 0)	// ���ӽ���
			goto callChildProcess;
		UnionProgramerLog("in %s:: create tmp child %d port = %d!\n",UnionGetApplicationName(),childPid,atoi(argv[1]));
	}
	// 2013-05-14 ���������ӽ���
	sleep(1);
	goto loop;
	return(UnionTaskActionBeforeExit());
	
	// �ӽ���ִ�����´���
callChildProcess:
	// ��ȡ�ź���
	//sem_wait(&gunionSemID);
	if (argc > 2)
		UnionTcpipSvrTask(atoi(argv[1]),argv[2]);
	else
		UnionTcpipSvrTask(atoi(argv[1]),NULL);
	
	// ������н����� < 10 ����ʹ��
	if ((cliNum = UnionExistsTaskOfName(cliName)) < 10)
		goto callChildProcess;
	
	// �ͷ��ź���
	//sem_post(&gunionSemID);
	if (isChildLoop)
		goto callChildProcess;
	else
		return(UnionTaskActionBeforeExit());
}

