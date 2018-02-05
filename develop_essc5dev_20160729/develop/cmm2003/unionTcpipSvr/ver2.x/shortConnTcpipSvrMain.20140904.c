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
#include "commWithHsmSvr.h"

extern PUnionCommConf	pgunionCommConf;
// modify by leipp 20151022  ,由1000改为2500
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

	//add begin by lusj 20160406  东莞农商接口判断  审核人：林晓杰
        char    appTaskName[128];
        int             IsESBTask=0;//0:非东莞农商ESB系统接口           1:东莞农商ESB系统接口
        //add end

	
        //add begin  by lusj  20160406 东莞农商ESB接口判断
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

	// add by leipp  20160718 ,如果使用了高速缓存，则不应去连接数据库
	// 检查是否使用高速缓存
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
		//if (errno == 22)	// 2006/10/27 增加
		//	break;
		return(errCodeUseOSErrCode);
	}
	
	//modify by lusj 20160406 东莞农商短链接处理
	if(IsESBTask==1)
        {
		Linger.l_onoff = 1;	//东莞农商短连接配置 
		Linger.l_linger = 1;
        }
        else
        {
                Linger.l_onoff = 1;	//原有的配置
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
		printf("用法:: %s 端口号 最小任务数 密码机组ID [长度域长度] [<DT> 睡眠开始时间-睡眠时间-睡眠结束时间-睡眠时间波动范围]\n",UnionGetApplicationName());
		printf("       长度域长度：默认值是2\n");
		printf("       延迟测试：默认值是0,单位为毫秒,必须先设置长度域长度，才能设置睡眠时间标识<DT>,睡眠开始时间为秒,未来预计多少秒开始, 实例:DT 20-1000-50-300\n");
		printf("               睡眠开始时间: 以程序开始处理数据为起点，预计n秒后开始睡眠测试,                                             实例:在处理数据开始后，20s开始进行睡眠测试\n");
		printf("               睡眠时间: 在到达睡眠开始时间后，每次处理交易所睡眠的时长,                                                  实例:每次睡眠1000ms\n");
		printf("               睡眠结束时间: 在睡眠开始后，n秒后恢复正常,停止睡眠测试,                                                    实例:以睡眠测试开始为准，50s后结束睡眠测试\n");
		printf("               睡眠时间波动范围: 每次睡眠时，根据随机说来取睡眠时间,                                                      实例:每次睡眠时间有波动,在700-1000之间\n");
	}
	else
	{
		printf("用法:: %s 端口号 [最小任务数] [最大任务数] [长度域长度]\n",UnionGetApplicationName());
		printf("       最小任务数：默认值是5\n");
		printf("       最大任务数：默认值是最小任务数+50\n");
		printf("       长度域长度：默认值是2\n");
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
				printf("参数4错误，必须是0,2,8!\n");
				return(-1);
			}
			UnionSetLenOfLenFieldForApp(lenOfLenField);
		}
		if (argc > 6)
		{
			if (strcmp(argv[5],"DT") == 0)	// 延迟测试功能选项
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
			//if ((lenOfLenField != 0) && (lenOfLenField != 2))  modified 2016-03-15 支持8字节的的报文头长度
			if ((lenOfLenField != 0) && (lenOfLenField != 2) && (lenOfLenField != 8)) 
			{
				printf("参数4错误，必须是0,2,8!\n");
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
	
	// 初始化一个侦听端口
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

	// 以下代码父进程执行
	snprintf(cliName,sizeof(cliName),"%s client %d",UnionGetApplicationName(),atoi(argv[1]));
	snprintf(busyCliName,sizeof(busyCliName),"%s busy %d",UnionGetApplicationName(),atoi(argv[1]));

	// 初始化子进程的类记录 2015-06-08
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
	// 维护最少的进程数
	//for (index = 0; index < currTaskNum; index++)
	for (index = 0; index < minTaskNum; index++)
	{
		daemonFlag = 1;		//add by linxj 20151230		设置为常驻子进程
		if (gunionChildPidGrp[index] > 0)
		{
			if (kill(gunionChildPidGrp[index],0) == 0)	// 进程存在
				continue;
		}
		// 创建子进程
		if ((childPid = UnionCreateProcess()) == 0)	// 是子进程
			goto callChildProcess;
		UnionProgramerLog("in %s:: create child %d port = %d!\n",UnionGetApplicationName(),childPid,atoi(argv[1]));
		gunionChildPidGrp[index] = childPid;
	}


	// 维护类表的数目　added 2015-06-08
	UnionCountTaskInstanceNumByNameGrp(2, instanceNameGrp, numGrp);
	UnionUpdateClientClassNum(instanceNameGrp[0], parentLogName, numGrp[0]);
	UnionUpdateClientClassNum(instanceNameGrp[1], parentLogName, numGrp[1]);

	// 当前进程数已经最大了
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
		//创建临时子进程
		for(i = 0; i < 10; i++)
		{
			// add by leipp 20160307
			if ((index = gunionGetFreePos(minTaskNum,maxTaskNum)) < 0)
			{
				UnionProgramerLog("in %s:: current childNum  is already reach maxTaskNum[%d]!\n",UnionGetApplicationName(),maxTaskNum);
				break;
			}
			// add end by leipp 20160307

			daemonFlag = 0;		//设置为不是常驻子进程
			if ((childPid = UnionCreateProcess()) == 0)	// 是子进程
				goto callChildProcess;
			UnionProgramerLog("in %s:: create child %d port = %d!\n",UnionGetApplicationName(),childPid,atoi(argv[1]));
			gunionChildPidGrp[index] = childPid;
			usleep(50000);	// 每个进程间隔50ms创建
		}
		//add end 20151230
	}
	else
		sleep(1);
	
	goto loop;
	
	// 子进程执行以下代码
callChildProcess:
	ret = UnionTcpipSvrTask(atoi(argv[1]));
	if(ret < 0) 
		return(UnionTaskActionBeforeExit());
	//add by linxj 20151230
	else if((UnionExistsTaskOfAlikeName(cliName) > 10) && (daemonFlag != 1))	//空闲子进程数超过5并且不为常驻子进程则退出
	{
		return(UnionTaskActionBeforeExit());
	}
	//add end 20151230
	else	
		goto callChildProcess;
}


// add by leipp 20160307
/*
	功能:	为子进程获取空闲位置

	输入参数:
		minTaskNum	最小任务数，属于常驻进程,临时子进程不允许占用
		maxTaskNum	最大任务数

	返回参数:
		返回空闲位置
*/
int gunionGetFreePos(int minTaskNum,int maxTaskNum)
{
	int	index = 0;


	for (index = minTaskNum; index < maxTaskNum; index++)
	{
		if (gunionChildPidGrp[index] > 0)
		{
			if (kill(gunionChildPidGrp[index],0) == 0)	// 进程存在
				continue;
			else
				gunionChildPidGrp[index] = -1;
		}

		return index;
	}

	return(errCodeParameter);
}

// add end by leipp 20150307
