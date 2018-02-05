// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2005/12/30
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
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

#include "UnionSocket.h"
#include "UnionTask.h"
#include "unionCommConf.h"
#include "unionResID.h"
#include "unionMDLID.h"
#include "UnionLog.h"

PUnionCommConf		pgunionCommConf = NULL;

#define	conMinProcNum	1000

PUnionTaskInstance	ptaskInstance = NULL;
int 			gsynchTCPIPSvrSckHDL = -1;
int			gunionBoundSckHDL = -1;
int			gunionSemIDInitialized = 0;
//sem_t			gunionSemID;
int			gunionFatherProcID;
int			gunionChildPidGrp[conMinProcNum];

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
	if (gsynchTCPIPSvrSckHDL >= 0)
		UnionCloseSocket(gsynchTCPIPSvrSckHDL);
	if (gunionBoundSckHDL >= 0)
		UnionCloseSocket(gunionBoundSckHDL);
	UnionSetCommConfAbnormal(pgunionCommConf);	
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int main(int argc,char *argv[])
{
	int			ret;
	char			taskName[40];
	int			childPid;
	int			index;
	int			minTaskNum = 5;
		
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);
	
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
		
	UnionSetMyResIDToTransSpier(conResIDMngService);
	UnionSetMyModuleType(conMDLTypeUnionMngSvr);
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,taskName)) == NULL)
	{
		UnionPrintf("in %s:: UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
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
	if ((minTaskNum = UnionReadIntTypeRECVar("minProcNumOfMngSvrTask")) <= 0)
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
		UnionProgramerLog("in %s:: create child %d port = %d!\n",UnionGetApplicationName(),childPid,atoi(argv[1]));
		gunionChildPidGrp[index] = childPid;
	}
	sleep(1);
	goto loop;
	return(UnionTaskActionBeforeExit());
	
	// �ӽ���ִ�����´���
callChildProcess:	
	// ��ȡ�ź���
	//sem_wait(&gunionSemID);
	UnionTcpipSvrTask(atoi(argv[1]));
	// �ͷ��ź���
	//sem_post(&gunionSemID);	
	goto callChildProcess;
}

int UnionTcpipSvrTask(int port)
{
	int			ret;
	unsigned int		clilen;
	struct sockaddr_in	serv_addr;
	struct sockaddr_in	cli_addr;
	int			sckinstance;
	struct linger		Linger;	// Added by Wolfgang Wang, 2004/7/13
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s client %d",UnionGetApplicationName(),port)) == NULL)
	{
		UnionUserErrLog("in UnionTcpipSvrTask:: UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	clilen = sizeof(cli_addr);
	sckinstance = accept(gunionBoundSckHDL, (struct sockaddr *)&cli_addr,(unsigned int *)&clilen);
	if ( sckinstance < 0 )
	{
		UnionSystemErrLog("in UnionTCPIPServer:: accept()!\n");
		//if (errno == 22)	// 2006/10/27 ����
		//	break;
		return(errCodeUseOSErrCode);
	}
	Linger.l_onoff = 1;
	Linger.l_linger = 0;
	if (setsockopt(sckinstance,SOL_SOCKET,SO_LINGER,(char *)&Linger,sizeof(Linger)) != 0)
	{
		UnionSystemErrLog("in UnionTCPIPServerf:: setsockopt linger!");
		close(sckinstance);
		return(errCodeUseOSErrCode);
	}
	ret = UnionSynchTCPIPTaskServer(sckinstance,&cli_addr,port,UnionTaskActionBeforeExit);
	UnionCloseSocket(sckinstance);
	return(ret);
}

int UnionHelp()
{
	printf("Usage:: %s port\n",UnionGetApplicationName());
	return(0);
}
