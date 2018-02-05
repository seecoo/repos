// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2005/12/30
// Version:	1.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<signal.h>

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
#include "commWithHsmSvr.h"
#include "unionCommand.h"

int UnionConnectRunningEnv();

extern PUnionCommConf	pgunionCommConf;
#define	conMinProcNum	1000

PUnionTaskInstance	ptaskInstance = NULL;
int			gunionBoundSckHDL = -1;
int			gunionSemIDInitialized = 0;
//sem_t			gunionSemID;
int			gunionFatherProcID;
int			gunionChildPidGrp[conMinProcNum];
int			gunionSckInstance = -1;
char			gunionFirstVarOfTcpipSvr[128+1] = "";
int			gunionIsInitFixedMDLID = 0;

char *UnionGetFirstVarOfTcpipSvr()
{
	return(gunionFirstVarOfTcpipSvr);
}

// 2010-1-22, Wolfgang Wang added
int			gunionShortConnTcipSvrPort;
int UnionGetShortConnTcipSvrPort()
{
	return(gunionShortConnTcipSvrPort);
}
// end of addition of 2010-1-22

int UnionTaskActionBeforeExit()
{
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
	if (gunionSckInstance >= 0)
		UnionCloseSocket(gunionSckInstance);
	UnionFreeUserSpecResource();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s port [hsmGroupID]\n",UnionGetApplicationName());
	return(0);
}

int UnionTcpipSvrTask(int port,char *funSvr)
{
	int			ret;
	unsigned int		clilen;
	struct sockaddr_in	cli_addr;
	struct linger		Linger;	
	
	if (gunionIsInitFixedMDLID <= 0)
	{
		UnionSetFunSvrName(funSvr);
		UnionSetHsmGroupIDForHsmSvr(funSvr);
		UnionSetMyModuleType(conMDLTypeUnionLongConnTcpipSvr);
		gunionIsInitFixedMDLID = 1;
	}

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s client %d",UnionGetApplicationName(),port)) == NULL)
	{
		UnionUserErrLog("in UnionTcpipSvrTask:: UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	clilen = sizeof(cli_addr);
	gunionSckInstance = accept(gunionBoundSckHDL, (struct sockaddr *)&cli_addr,&clilen);
	if ( gunionSckInstance < 0 )
	{
		UnionSystemErrLog("in UnionTcpipSvrTask:: accept()!\n");
		//if (errno == 22)	// 2006/10/27 ����
		//	break;
		return(errCodeUseOSErrCode);
	}
	Linger.l_onoff = 0;
	Linger.l_linger = 0;
	if (setsockopt(gunionSckInstance,SOL_SOCKET,SO_LINGER,(char *)&Linger,sizeof(Linger)) != 0)
	{
		UnionSystemErrLog("in UnionTcpipSvrTask:: setsockopt linger!");
		close(gunionSckInstance);
		return(errCodeUseOSErrCode);
	}
	ret = UnionSynchTCPIPTaskServer(gunionSckInstance,&cli_addr,port,UnionTaskActionBeforeExit);
	UnionCloseSocket(gunionSckInstance);
	gunionSckInstance = -1;
	return(ret);
}

int main(int argc,char *argv[])
{
	int			ret;
	char			taskName[40];
	int			childPid;
	int			index;
	int			minTaskNum = 5;
	char			varName[100];
	int			port;
	char			funSvr[40];
	
	UnionSetApplicationName(argv[0]);
		
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);
	
	if (argc < 2)
		return(UnionHelp());

	if (UnionIsDigitStr(argv[1]))
		port = atoi(argv[1]);
	else
	{
		sprintf(varName,"portOf%s",argv[1]);
		if ((port = UnionReadIntTypeRECVar(varName)) < 0)
		{
			printf("not defined [%s] in unionREC!\n",varName);
			return(errCodeParameter);
		}
		strcpy(gunionFirstVarOfTcpipSvr,argv[1]);
	}
	gunionShortConnTcipSvrPort = port;
	memset(taskName,0,sizeof(taskName));
	UnionFormTaskNameOfMngSvr(port,taskName);
	
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
	if ((gunionBoundSckHDL = UnionInitializeTCPIPServer(port)) < 0)
	{
		UnionPrintf("in %s:: UnionInitializeTCPIPServer Error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	UnionSuccessLog("a servier bound to [%d] started OK!\n",port);

	// ���´��븸����ִ��
loop:
	// �����ӽ���ʵ����Ŀ
	sprintf(varName,"minProcNumOf%s",UnionGetApplicationName());
	if ((minTaskNum = UnionReadIntTypeRECVar(varName)) <= 0)
		minTaskNum = 10;
	//UnionProgramerLog("in %s:: minTaskNum = [%d]\n",minTaskNum); 
	if (minTaskNum > conMinProcNum)
		minTaskNum = conMinProcNum;
	for (index = 0; index < minTaskNum; index++)
	{
		if (gunionChildPidGrp[index] > 0)
		{
			if (kill(gunionChildPidGrp[index],0) == 0)	// ���̴���
				continue;
		}
		// �����ӽ���
		if ((childPid = UnionCreateProcess()) == 0)	// ���ӽ���
			goto callChildProcess;
		UnionProgramerLog("in %s:: create child %d port = %d!\n",UnionGetApplicationName(),childPid,port);
		gunionChildPidGrp[index] = childPid;
	}
	sleep(1);
	goto loop;
	return(UnionTaskActionBeforeExit());
	
	// �ӽ���ִ�����´���
callChildProcess:	
	// ��ȡ�ź���
	//sem_wait(&gunionSemID);
	memset(funSvr, 0, sizeof(funSvr));
	if (argc < 3)
	{
		ret = UnionGetFunSvr(funSvr);
		if (ret < 0)
		{
			UnionProgramerLog("in %s:: UnionGetFunSvr error! ret=[%d]\n", UnionGetApplicationName(), ret);
			return(ret);
		}
	}
	else
	{
		strcpy(funSvr, argv[2]);
		UnionSetDefaultHsm(argv[2]);
	}
	UnionTcpipSvrTask(port,funSvr);
	// �ͷ��ź���
	//sem_post(&gunionSemID);	
	goto callChildProcess;
}

// add by wuhy at 20140903 (û�͵ڶ������������REC���ȡȱʡ�������ID)
int UnionGetFunSvr(char *funSvr)
{
	int     fldNum;
	char    hsmGroupID[128];
	char    varName[64];
	char    hsmGrp[10][128];
	char    *ptr = NULL;

	memset(hsmGrp, 0, sizeof(hsmGrp));
	memset(varName, 0, sizeof(varName));

	// ��ȡ�������
	strcpy(varName, "GJHsmGrp");

	memset(hsmGroupID, 0, sizeof(hsmGroupID));
	if ((ptr = UnionReadStringTypeRECVar(varName)) == NULL)
	{
		UnionProgramerLog("in UnionGetFunSvr:: varName is null...\n");
		return -1;
	}

	strcpy(hsmGroupID, UnionReadStringTypeRECVar(varName));
	fldNum = UnionSeprateVarStrIntoVarGrp(hsmGroupID, strlen(hsmGroupID), ',', hsmGrp, 10);
	if (fldNum < 0)
	{
		UnionUserErrLog("in UnionGetFunSvr :: UnionSeprateVarStrIntoVarGrp error! hsmGroupID=[%s]\n",hsmGroupID);
		return (fldNum);
	}
	strcpy(funSvr, hsmGrp[0]);

	return 0;
}
