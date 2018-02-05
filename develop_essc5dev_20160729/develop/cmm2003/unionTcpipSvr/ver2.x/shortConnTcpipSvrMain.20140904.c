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
#include "commWithHsmSvr.h"

extern PUnionCommConf	pgunionCommConf;
// modify by leipp 20151022  ,��1000��Ϊ2500
#define	conMinProcNum	2500
// modify by leipp end

PUnionTaskInstance	ptaskInstance = NULL;
int			gunionBoundSckHDL = -1;
int			gunionFatherProcID;
int			gunionChildPidGrp[conMinProcNum];

int			gunionHsmCmd = 0;

int gunionGetFreePos(int minTaskNum,int maxTaskNum);

int UnionTaskActionBeforeExit()
{
	int	sckHDL;
	int	index;
	
	if (gunionFatherProcID == getpid())
	{
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
//        UnionCloseDatabase();	// modify by leipp 20150923
	UnionCloseHighCachedService();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int UnionTcpipSvrTask(int port)
{
	int			ret;
	unsigned int		clilen;
	struct sockaddr_in	cli_addr;
	int			sckinstance;
	struct linger		Linger;	// Added by Wolfgang Wang, 2004/7/13

	//add begin by lusj 20160406  ��ݸũ�̽ӿ��ж�  ����ˣ�������
        char    appTaskName[128];
        int             IsESBTask=0;//0:�Ƕ�ݸũ��ESBϵͳ�ӿ�           1:��ݸũ��ESBϵͳ�ӿ�
        //add end

	
        //add begin  by lusj  20160406 ��ݸũ��ESB�ӿ��ж�
        memset(appTaskName,0,sizeof(appTaskName));
        snprintf(appTaskName,sizeof(appTaskName),"%s",UnionGetApplicationName());
        if (memcmp(appTaskName,"appTask-esb",11) == 0)
        {
                        IsESBTask=1;
        }
        //add end
	
	/*
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s client %d",UnionGetApplicationName(),port)) == NULL)
	{
		UnionUserErrLog("in UnionTcpipSvrTask:: UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if ((ptaskInstance = UnionResetTaskInstanceLogFile(UnionTaskActionBeforeExit,"%s %d",UnionGetApplicationName(),port)) == NULL)
	{
		UnionUserErrLog("in UnionTcpipSvrTask:: UnionResetTaskInstanceLogFile Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	*/
	
	if ((ptaskInstance = UnionCreateTaskInstanceForClient(UnionTaskActionBeforeExit,"%s client %d",UnionGetApplicationName(),port)) == NULL)
	{
		UnionUserErrLog("in UnionTcpipSvrTask:: UnionCreateTaskInstanceForClient Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	// add by leipp  20160718 ,���ʹ���˸��ٻ��棬��Ӧȥ�������ݿ�
	// ����Ƿ�ʹ�ø��ٻ���
        if (UnionReadIntTypeRECVar("isUseHighCached") <= 0)
	{
		if ((ret = UnionConnectDatabase()) < 0)
		{
			UnionUserErrLog("in UnionTcpipSvrTask:: UnionConnectDatabase ret = [%d]\n",ret);
			return(ret);
		}
	}
	//add end leipp 20160718

	clilen = sizeof(cli_addr);
	sckinstance = accept(gunionBoundSckHDL, (struct sockaddr *)&cli_addr,&clilen);
	if ( sckinstance < 0 )
	{
		UnionSystemErrLog("in UnionTcpipSvrTask:: accept()!\n");
		//if (errno == 22)	// 2006/10/27 ����
		//	break;
		return(errCodeUseOSErrCode);
	}
	
	//modify by lusj 20160406 ��ݸũ�̶����Ӵ���
	if(IsESBTask==1)
        {
		Linger.l_onoff = 1;	//��ݸũ�̶��������� 
		Linger.l_linger = 1;
        }
        else
        {
                Linger.l_onoff = 1;	//ԭ�е�����
                Linger.l_linger = 0;
        }
	//end
	

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

int UnionHelp()
{
	if (gunionHsmCmd)
	{
		printf("�÷�:: %s �˿ں� ��С������ �������ID [�����򳤶�] [<DT> ˯�߿�ʼʱ��-˯��ʱ��-˯�߽���ʱ��-˯��ʱ�䲨����Χ]\n",UnionGetApplicationName());
		printf("       �����򳤶ȣ�Ĭ��ֵ��2\n");
		printf("       �ӳٲ��ԣ�Ĭ��ֵ��0,��λΪ����,���������ó����򳤶ȣ���������˯��ʱ���ʶ<DT>,˯�߿�ʼʱ��Ϊ��,δ��Ԥ�ƶ����뿪ʼ, ʵ��:DT 20-1000-50-300\n");
		printf("               ˯�߿�ʼʱ��: �Գ���ʼ��������Ϊ��㣬Ԥ��n���ʼ˯�߲���,                                             ʵ��:�ڴ������ݿ�ʼ��20s��ʼ����˯�߲���\n");
		printf("               ˯��ʱ��: �ڵ���˯�߿�ʼʱ���ÿ�δ�������˯�ߵ�ʱ��,                                                  ʵ��:ÿ��˯��1000ms\n");
		printf("               ˯�߽���ʱ��: ��˯�߿�ʼ��n���ָ�����,ֹͣ˯�߲���,                                                    ʵ��:��˯�߲��Կ�ʼΪ׼��50s�����˯�߲���\n");
		printf("               ˯��ʱ�䲨����Χ: ÿ��˯��ʱ���������˵��ȡ˯��ʱ��,                                                      ʵ��:ÿ��˯��ʱ���в���,��700-1000֮��\n");
	}
	else
	{
		printf("�÷�:: %s �˿ں� [��С������] [���������] [�����򳤶�]\n",UnionGetApplicationName());
		printf("       ��С��������Ĭ��ֵ��5\n");
		printf("       �����������Ĭ��ֵ����С������+50\n");
		printf("       �����򳤶ȣ�Ĭ��ֵ��2\n");
	}
	return(0);
}

int main(int argc,char *argv[])
{
	int			ret;
	char			taskName[256];
	int			childPid;
	int			index;
	int			minTaskNum = 5;
	int			maxTaskNum = minTaskNum;
	int			lenOfLenField = 2;
	int			currTaskNum = 0;
	int			realCliNum = 0;
	int			first = 1;
	char			cliName[256];
	char			busyCliName[256];	//add by linxj 20151229
	int			i = 0;

	char			instanceNameGrp[2][64];
	int			numGrp[2] = {0, 0};
	char			parentLogName[256];

	int			daemonFlag = 0;	//add by linxj 20151230

	UnionSetApplicationName(argv[0]);
		
	if (strstr(argv[0],"-hsmcmd") != NULL)
		gunionHsmCmd = 1;
	
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);
	
	if (!UnionIsDigitString(argv[1]))
		return(UnionHelp());
	
	if (strstr(argv[0],"-hsmcmd") != NULL)
	{
		if (argc < 4)
			return(UnionHelp());
		
		minTaskNum = atoi(argv[2]);
		maxTaskNum = minTaskNum + 50;
		UnionSetHsmGroupIDForHsmSvr(argv[3]);

		if (argc > 4)
		{
			lenOfLenField = atoi(argv[4]);
			if ((lenOfLenField != 0) && (lenOfLenField != 2) && (lenOfLenField != 8)) 
			{
				printf("����4���󣬱�����0,2,8!\n");
				return(-1);
			}
			UnionSetLenOfLenFieldForApp(lenOfLenField);
		}
		if (argc > 6)
		{
			if (strcmp(argv[5],"DT") == 0)	// �ӳٲ��Թ���ѡ��
			{
				UnionSetFunSvrStartVar(argv[6]);
			}
		}
	}
	else
	{
		if (argc < 2)
			return(UnionHelp());

		if (argc > 2)
			minTaskNum = atoi(argv[2]);
			
		if (argc > 3)
		{
			if ((maxTaskNum = atoi(argv[3])) < minTaskNum)
				maxTaskNum = minTaskNum;
		}
		else
			maxTaskNum = minTaskNum + 50;
			
		if (argc > 4)
		{
			lenOfLenField = atoi(argv[4]);
			//if ((lenOfLenField != 0) && (lenOfLenField != 2))  modified 2016-03-15 ֧��8�ֽڵĵı���ͷ����
			if ((lenOfLenField != 0) && (lenOfLenField != 2) && (lenOfLenField != 8)) 
			{
				printf("����4���󣬱�����0,2,8!\n");
				return(-1);
			}
			UnionSetLenOfLenFieldForApp(lenOfLenField);
		}
	}


	memset(taskName,0,sizeof(taskName));
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
	
	// ��ʼ��һ�������˿�
	if ((gunionBoundSckHDL = UnionInitializeTCPIPServer(atoi(argv[1]))) < 0)
	{
		UnionPrintf("in %s:: UnionInitializeTCPIPServer[%d] Error!\n",UnionGetApplicationName(),atoi(argv[1]));
		return(UnionTaskActionBeforeExit());
	}
	UnionSuccessLog("a servier bound to [%d] started OK!\n",atoi(argv[1]));

	
	if (minTaskNum > conMinProcNum)
		minTaskNum = conMinProcNum;
	if (maxTaskNum > conMinProcNum)
		maxTaskNum = conMinProcNum;

	currTaskNum = minTaskNum;

	// ���´��븸����ִ��
	snprintf(cliName,sizeof(cliName),"%s client %d",UnionGetApplicationName(),atoi(argv[1]));
	snprintf(busyCliName,sizeof(busyCliName),"%s busy %d",UnionGetApplicationName(),atoi(argv[1]));

	// ��ʼ���ӽ��̵����¼ 2015-06-08
	snprintf(instanceNameGrp[0], sizeof(instanceNameGrp[0]), "%s client %d", UnionGetApplicationName(), atoi(argv[1]));
	if (gunionHsmCmd)
	{
		snprintf(instanceNameGrp[1], sizeof(instanceNameGrp[1]), "%s busy %d %s", UnionGetApplicationName(), atoi(argv[1]),UnionGetHsmGroupIDForHsmSvr());
	}
	else
	{
		snprintf(instanceNameGrp[1], sizeof(instanceNameGrp[1]), "%s busy %d", UnionGetApplicationName(), atoi(argv[1]));
	}

	if(ptaskInstance->pclass->logFileName)
	{
		strcpy(parentLogName, ptaskInstance->pclass->logFileName);
	}
	else
	{
		strcpy(parentLogName, "unknown");
	}
	ret = UnionUpdateClientClassNum(instanceNameGrp[0], parentLogName, 0);
	if(ret < 0)
	{
		UnionPrintf("in %s:: UnionUpdateClientClassNum taskName [%s] ret = %d!\n",UnionGetApplicationName(), instanceNameGrp[0], ret);
		return(UnionTaskActionBeforeExit());
	}
	ret = UnionUpdateClientClassNum(instanceNameGrp[1], parentLogName, 0);
	if(ret < 0)
	{
		UnionPrintf("in %s:: UnionUpdateClientClassNum taskName [%s] ret = %d!\n",UnionGetApplicationName(), instanceNameGrp[1], ret);
		return(UnionTaskActionBeforeExit());
	}
loop:	
	// ά�����ٵĽ�����
	//for (index = 0; index < currTaskNum; index++)
	for (index = 0; index < minTaskNum; index++)
	{
		daemonFlag = 1;		//add by linxj 20151230		����Ϊ��פ�ӽ���
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


	// ά��������Ŀ��added 2015-06-08
	UnionCountTaskInstanceNumByNameGrp(2, instanceNameGrp, numGrp);
	UnionUpdateClientClassNum(instanceNameGrp[0], parentLogName, numGrp[0]);
	UnionUpdateClientClassNum(instanceNameGrp[1], parentLogName, numGrp[1]);

	// ��ǰ�������Ѿ������
	// add by leipp 20160302
	currTaskNum = UnionExistsTaskOfAlikeName(cliName) + UnionExistsTaskOfAlikeName(busyCliName);
	// modify end 20160302

	if (currTaskNum >= maxTaskNum)
	{
		sleep(1);
		goto loop;
	}
	
	if (first)
	{
		first = 0;
		sleep(1);
	}

	realCliNum = UnionExistsTaskOfAlikeName(cliName);

	if (realCliNum == 0 || realCliNum == 1)
	{
		//add by linxj 20151230
		//������ʱ�ӽ���
		for(i = 0; i < 10; i++)
		{
			// add by leipp 20160307
			if ((index = gunionGetFreePos(minTaskNum,maxTaskNum)) < 0)
			{
				UnionProgramerLog("in %s:: current childNum  is already reach maxTaskNum[%d]!\n",UnionGetApplicationName(),maxTaskNum);
				break;
			}
			// add end by leipp 20160307

			daemonFlag = 0;		//����Ϊ���ǳ�פ�ӽ���
			if ((childPid = UnionCreateProcess()) == 0)	// ���ӽ���
				goto callChildProcess;
			UnionProgramerLog("in %s:: create child %d port = %d!\n",UnionGetApplicationName(),childPid,atoi(argv[1]));
			gunionChildPidGrp[index] = childPid;
			usleep(50000);	// ÿ�����̼��50ms����
		}
		//add end 20151230
	}
	else
		sleep(1);
	
	goto loop;
	
	// �ӽ���ִ�����´���
callChildProcess:
	ret = UnionTcpipSvrTask(atoi(argv[1]));
	if(ret < 0) 
		return(UnionTaskActionBeforeExit());
	//add by linxj 20151230
	else if((UnionExistsTaskOfAlikeName(cliName) > 10) && (daemonFlag != 1))	//�����ӽ���������5���Ҳ�Ϊ��פ�ӽ������˳�
	{
		return(UnionTaskActionBeforeExit());
	}
	//add end 20151230
	else	
		goto callChildProcess;
}


// add by leipp 20160307
/*
	����:	Ϊ�ӽ��̻�ȡ����λ��

	�������:
		minTaskNum	��С�����������ڳ�פ����,��ʱ�ӽ��̲�����ռ��
		maxTaskNum	���������

	���ز���:
		���ؿ���λ��
*/
int gunionGetFreePos(int minTaskNum,int maxTaskNum)
{
	int	index = 0;


	for (index = minTaskNum; index < maxTaskNum; index++)
	{
		if (gunionChildPidGrp[index] > 0)
		{
			if (kill(gunionChildPidGrp[index],0) == 0)	// ���̴���
				continue;
			else
				gunionChildPidGrp[index] = -1;
		}

		return index;
	}

	return(errCodeParameter);
}

// add end by leipp 20150307
