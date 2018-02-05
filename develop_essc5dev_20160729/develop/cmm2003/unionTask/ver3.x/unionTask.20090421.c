//	Author:		Wolfgang Wang
//	Date:		2003/09/19
//	Version:	3.x

#define _UnionTask_3_x_
#define _UnionEnv_3_x_		// 使用3.x版本的UnionEnv模块
#define _UnionLogMDL_3_x_

// 2005/9/19，王纯军：在20050909版本的基础上升级
// 升级后的程序支持任务定义中的任务名称，包括多个空格，在加载任务表时
// 会自动将多个空格合并为一个空格。

// 2005/10/09，王纯军：在20050910版本基础上升级
// 增加了一个重新命名任务注册名的函数UnionRenameTaskNameOfMyself

// 2004/6/18，Wolfgang Wang, Added

// 2005/11/03 增加了一个编译参数，
//	_useSigUser2_,控制使用信号量1还是2进行进程控制。

// 2006/7/26 增加以下函数
/*
// 检查是否有名字中含有taskName的任务类型定义,返回数量
int UnionExistsTaskClassOfAlikeName(char *taskName);
// 检查是否有名字为taskName的任务类型定义,返回数量
int UnionExistsTaskClassOfName(char *taskName)
// 增加一个新的任务类型定义
int UnionAddNewTaskClass(char *taskName,char *command,int num,char *logFileName);

// 2006/7/26 王纯军修改 如果任务定义的类型不存在，那么使用缺省类型
PUnionTaskInstance UnionCreateTaskInstance(int (*UnionTaskActionBeforeExit)(),char *fmt,...)
*/

// 2006/11/01在UnionClearRubbishTaskInstance函数中，增加了对任务类currentNum的维护

#define _UnionTask_3_2_

#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <signal.h>
#endif
#include <stdarg.h>
#include <stdlib.h>

#include <setjmp.h> //add in 2013.6.13

#ifndef _WIN32
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#else
#include "unionSimuUnixSharedMemory.h"
#include "simuUnixSystemCall.h"
#endif


#include "UnionLog.h"
#include "UnionProc.h"
#include "unionOSLockMachinism.h"
#include "unionREC.h"

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "UnionTask.h"
#include "UnionEnv.h"
#include "unionModule.h"

#include "unionVersion.h"
#ifndef _WIN32
#include "unionCommand.h"
#endif

#include "unionErrCode.h"
#include "UnionStr.h"
#include "unionUnionTaskTBL.h"

PUnionSharedMemoryModule	pgunionTaskMDL = NULL;
PUnionTaskTBL			pgunionTaskTBL = NULL;
PUnionTaskClass			pgunionTaskClass = NULL;
PUnionTaskInstance		pgunionTaskInstance = NULL;
PUnionTaskInstance		pgthisInstance = NULL;
int				gunionLastFindTaskInstanceIndex = 0;

//add in 2013.6.13 hzh 增加给任务实例表项加、解锁的功能 
static int gLock = -1;
jmp_buf	gunionLockTaskInstanceIndexTimeoutJmpEnv;

void UnionLockTaskInstanceIndexTimeOut()
{
	UnionUserErrLog("in UnionLockTaskInstanceIndexTimeOut:: time out!\n");
#if ( defined __linux__ ) || ( defined __hpux )
	siglongjmp(gunionLockTaskInstanceIndexTimeoutJmpEnv,10);
#elif ( defined _AIX )
	longjmp(gunionLockTaskInstanceIndexTimeoutJmpEnv,10);
#endif
}

//给任务实例表项加锁
int UnionLockTaskInstanceIndex(int timeout)
{
#if ( defined __linux__ ) || ( defined __hpux )
	if (sigsetjmp(gunionLockTaskInstanceIndexTimeoutJmpEnv,1) != 0)
#elif ( defined _AIX )
	if (setjmp(gunionLockTaskInstanceIndexTimeoutJmpEnv) != 0)
#endif
	{
		UnionUserErrLog("in UnionLockTaskInstanceIndex:: timeout! \n");
		alarm(0);
		gLock = -1;
		return(errCodeCreateTaskInstance);
	}
	signal(SIGALRM,UnionLockTaskInstanceIndexTimeOut);
	alarm(timeout);
	if ((gLock = UnionApplyOsLock(conMDLNameOfUnionTaskTBL)) < 0)
	{
		UnionUserErrLog("in UnionLockTaskInstanceIndex:: UnionApplyOsLock failed[%d]\n",gLock);
	}
	alarm(0);
	return gLock;
}

//给任务实例表项解锁
int UnionUnLockTaskInstanceIndex()
{
	if(gLock>=0) 
	{
		gLock = -1;
		UnionFreeOsLock(conMDLNameOfUnionTaskTBL);
	}
	return 0;
}	
//add end 2013.6.13

// 20060808 增加
int UnionGetCurrentTaskClassNum()
{
	int	ret;

	if ((ret = UnionConnectTaskTBL()) < 0)
		return(ret);
	else
		return(pgunionTaskTBL->maxClassNum);
}

// 20060808 增加
int UnionGetCurrentTaskInstanceNum()
{
	int	ret;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
		return(ret);
	else
		return(pgunionTaskTBL->maxInstanceNum);
}

// 20060808 增加
PUnionTaskClass UnionGetCurrentTaskClassGrp()
{
	if (UnionConnectTaskTBL() < 0)
		return(NULL);
	else
		return(pgunionTaskClass);
}

// 20060808 增加
PUnionTaskInstance UnionGetCurrentTaskInstanceGrp()
{
	if (UnionConnectTaskTBL() < 0)
		return(NULL);
	else
		return(pgunionTaskInstance);
}

int UnionIsTaskTBLConnected()
{
	if ((!UnionIsSharedMemoryInited(conMDLNameOfUnionTaskTBL)) || (pgunionTaskTBL == NULL) || (pgunionTaskClass == NULL) || (pgunionTaskInstance == NULL))
		return(0);
	else
		return(1);
}

PUnionLogFile UnionGetLogFile()
{
	if (pgthisInstance)
		return(pgthisInstance->plogFile);
	return(NULL);
}

int UnionConnectTaskTBL()
{
	//PUnionSharedMemoryModule	pmdl;
	int				maxClassNum,maxInstanceNum;
	
	// 已经连接
	if (UnionIsTaskTBLConnected())
		return(0);
		
	if ((maxClassNum = UnionGetMaxNumOfTaskClass()) <= 0) 
	{
		UnionUserErrLog("in UnionConnectTaskTBL:: UnionGetMaxNumOfTaskClass [%d]\n",maxClassNum);
		return(maxClassNum);
	}
	if ((maxInstanceNum = UnionGetMaxNumOfTaskInstance()) <= 0)
	{
		UnionUserErrLog("in UnionConnectTaskTBL:: UnionGetMaxNumOfTaskInstance [%d]\n",maxInstanceNum);
		return(maxInstanceNum);
	}

	if ((pgunionTaskMDL = UnionConnectSharedMemoryModule(conMDLNameOfUnionTaskTBL,
			sizeof(TUnionTaskTBL) + sizeof(TUnionTaskClass) * maxClassNum + sizeof(TUnionTaskInstance) * maxInstanceNum)
			) == NULL)
	{
		UnionUserErrLog("in UnionConnectTaskTBL:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}
	if ((pgunionTaskTBL = (PUnionTaskTBL)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionTaskMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectTaskTBL:: PUnionTaskTBL!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgunionTaskTBL->pclassTBL = (PUnionTaskClass)((unsigned char *)pgunionTaskTBL + sizeof(*pgunionTaskTBL))) == NULL)
	{
		UnionUserErrLog("in UnionConnectTaskTBL:: PUnionTaskClass!\n");
		return(errCodeSharedMemoryModule);
	}
	pgunionTaskClass = pgunionTaskTBL->pclassTBL;
	if ((pgunionTaskTBL->pinstanceTBL = (PUnionTaskInstance)((unsigned char *)pgunionTaskTBL 
								+ sizeof(*pgunionTaskTBL)
								+ sizeof(TUnionTaskClass) * maxClassNum)) == NULL)
	{
		UnionUserErrLog("in UnionConnectTaskTBL:: PUnionTaskInstance!\n");
		return(errCodeSharedMemoryModule);
	}
	pgunionTaskInstance = pgunionTaskTBL->pinstanceTBL;
			
	if (UnionIsNewCreatedSharedMemoryModule(pgunionTaskMDL))
	{
		return(UnionReloadTaskTBL());
	}
	else
		return(0);
}


int UnionDisconnectTaskTBL()
{
	UnionUnregisterThisTaskInstance();
	pgunionTaskTBL = NULL;
	pgunionTaskClass = NULL;
	pgunionTaskInstance = NULL;
	return(UnionDisconnectShareModule(pgunionTaskMDL));
}

int UnionRemoveTaskTBL()
{
	UnionKillAllTaskInstance();
	UnionDisconnectTaskTBL();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfUnionTaskTBL));
}

int UnionPrintTaskClassInTBLToFile(FILE *fp)
{
	int	i;
	int	num = 0;
	int	ret;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintTaskClassInTBLToFile:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintTaskClassInTBLToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	fprintf(fp,"%-6s %-8s %-40s %-40s %s\n","任务数","当前数量","任务名","启动命令","日志名");
	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		//UnionNullLog("%d %0x\n",i,pgunionTaskClass+i);
		if (UnionPrintTaskClassToFile(pgunionTaskClass+i,fp) < 0)
			continue;
		else
		{
#ifndef _WIN32
			if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
				if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
					break;
#endif
			num++;
		}
	}
	fprintf(fp,"Total Class Num = [%d]\n",num);
	return(num);
}

int UnionPrintTaskClassInTBLInFormatToFile(FILE *fp)
{
	int	i;
	int	num = 0;
	int	ret;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintTaskClassInTBLInFormatToFile:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintTaskClassInTBLInFormatToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	fprintf(fp,"(maxNum,currentNum,name,logFile,startCommand)\n");
	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		//UnionNullLog("%d %0x\n",i,pgunionTaskClass+i);
		if (UnionPrintTaskClassInFormatToFile(pgunionTaskClass+i,fp) < 0)
			continue;
		else
		{
#ifndef _WIN32
			if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
				if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
					break;
#endif
			num++;
		}
	}
	fprintf(fp,"Total Class Num = [%d]\n",num);
	return(num);
}

int UnionPrintTaskClassInTBL()
{
	return(UnionPrintTaskClassInTBLToFile(stdout));
        //return(0);
}

int UnionPrintTaskInstanceInTBLToFile(FILE *fp)
{
	int	i;
	int	num = 0;
	int	ret;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintTaskInstanceInTBLToFile:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintTaskInstanceInTBLToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	fprintf(fp,"%10s %19s %12s %40s\n","进程号","启动时间","日志地址","任务名");
	fprintf(fp,"--------------------------------------------------------------------------------\n");
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		//UnionNullLog("%d %0x\n",i,pgunionTaskInstance+i);
		if (UnionPrintTaskInstanceToFile(pgunionTaskInstance+i,fp) < 0)
			continue;
		else
		{
#ifndef _WIN32
                        if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
				if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
					break;
#endif
			num++;
		}
	}
	fprintf(fp,"总任务数 = [%d]\n",num);
	return(num);
}

int UnionPrintTaskInstanceInTBL()
{
	return(UnionPrintTaskInstanceInTBLToFile(stdout));
}

int UnionPrintStatusOfTaskTBLToFile(FILE *fp)
{
	int	ret;

	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintStatusOfTaskTBLToFile:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintStatusOfTaskTBLToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
	if (pgunionTaskTBL == NULL)
		return(errCodeTaskMDL_NotConnected);
	fprintf(fp,"\n");
	fprintf(fp,"maxClassNum	[%d]\n",pgunionTaskTBL->maxClassNum);
	fprintf(fp,"maxInstanceNum	[%d]\n",pgunionTaskTBL->maxInstanceNum);
	fprintf(fp,"\n");
	return(UnionPrintSharedMemoryModuleToFile(pgunionTaskMDL,stdout));
}

int UnionPrintStatusOfTaskTBL()
{
	return(UnionPrintStatusOfTaskTBLToFile(stdout));
}

int UnionPrintTaskTBL()
{
	UnionPrintTaskClassInTBL();
	UnionPrintTaskInstanceInTBL();
	UnionPrintStatusOfTaskTBL();
	return(0);
}

int UnionPrintTaskClassToFile(PUnionTaskClass ptaskClass,FILE *fp)
{
	if ((ptaskClass == NULL) || (fp == NULL))
		return(errCodeTaskMDL_NotConnected);
	if (strlen(ptaskClass->name) == 0)
		return(-1);
	fprintf(fp,"%-6d ",ptaskClass->minNum);
	fprintf(fp,"%-8d ",ptaskClass->currentNum);
	fprintf(fp,"%-40s ",ptaskClass->name);
	fprintf(fp,"%-40s ",ptaskClass->startCmd);
	fprintf(fp,"%s\n",ptaskClass->logFileName);
	return(0);
}

int UnionPrintTaskClassInFormatToFile(PUnionTaskClass ptaskClass,FILE *fp)
{
	//char		tmpBuf[3072 + 1];
	
	if ((ptaskClass == NULL) || (fp == NULL))
		return(errCodeTaskMDL_NotConnected);
	if (strlen(ptaskClass->name) == 0)
		return(-1);
	/*
	fprintf(fp,"%03d,",ptaskClass->minNum);
	fprintf(fp,"%03d,",ptaskClass->currentNum);
	fprintf(fp,"%25s,",ptaskClass->name);
	fprintf(fp,"%15s,",ptaskClass->logFileName);
	fprintf(fp,"%25s\n",ptaskClass->startCmd);
	*/
	fprintf(fp,"fld1=%s|fld2=%s|fld3=%d|fld4=%s|fld5=%d|\n",ptaskClass->name,
		ptaskClass->startCmd,
		ptaskClass->minNum,
		ptaskClass->logFileName,
		ptaskClass->currentNum);
	return(0);
}


int UnionPrintTaskInstanceToFile(PUnionTaskInstance ptaskInstance,FILE *fp)
{
	if ((ptaskInstance == NULL) || (fp == NULL))
		return(errCodeTaskMDL_NotConnected);
	if (ptaskInstance->procID <= 0)
		return(-1);
	fprintf(fp,"%10d ",ptaskInstance->procID);
	fprintf(fp,"%.4s-%.2s-%.2s %.2s:%.2s:%.2s ",
		ptaskInstance->startTime,
		ptaskInstance->startTime + 4,
		ptaskInstance->startTime + 6,
		ptaskInstance->startTime + 8,
		ptaskInstance->startTime + 10,
		ptaskInstance->startTime + 12);
	fprintf(fp,"%12p ",ptaskInstance->plogFile);

	//fprintf(fp,"%20s ",ptaskInstance->plogFile->name);
	fprintf(fp,"%40s\n",ptaskInstance->name);
	
	return(0);
}

PUnionTaskInstance UnionFindTaskInstance(int procID)
{
	int	i, times;
	int	ret;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionFindTaskInstance:: UnionConnectTaskTBL!\n");
		return(NULL);
	}
	
	for (i = gunionLastFindTaskInstanceIndex, times = 0; times < pgunionTaskTBL->maxInstanceNum; times++, i++)
	{
		if(i == pgunionTaskTBL->maxInstanceNum) // 从头开始
		{
			i = 0;
		}

		if ((pgunionTaskInstance+i)->procID == procID)
		{
			gunionLastFindTaskInstanceIndex = i;
			return(pgunionTaskInstance+i);
		}
	}
	
	return(NULL);
}

PUnionTaskClass UnionFindTaskClass(char *taskName)
{
	int	i;
	int	ret;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionFindTaskClass:: UnionConnectTaskTBL!\n");
		return(NULL);
	}
	
	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		if (strcmp((pgunionTaskClass+i)->name,taskName) == 0)
			return(pgunionTaskClass+i);
	}
	
	return(NULL);
}
	
// 2006/7/26 王纯军修改 如果任务定义的类型不存在，那么使用缺省类型
PUnionTaskInstance UnionCreateTaskInstance(int (*UnionTaskActionBeforeExit)(),char *fmt,...)
{
	va_list 		args;
	char			taskName[256];
	PUnionTaskInstance	pinstance;
	PUnionTaskClass		pclass;
	int			i;
	int			ret;

	volatile PUnionTaskInstance tmp_p_TaskInstance;

	// Added by Wolfgang Wang, 2004/11/25
	if (!UnionIsProductStillEffective())
	{
		UnionUserErrLog("in UnionCreateTaskInstance:: product outof date of Application [%s]!\n",UnionGetApplicationName());
		return(NULL);
	}
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionCreateTaskInstance:: UnionConnectTaskTBL!\n");
		return(NULL);
	}
	
	memset(taskName,0,sizeof(taskName));
	va_start(args,fmt);
	vsprintf(taskName,fmt,args);
	va_end(args);

	if ((pclass = UnionFindTaskClass(taskName)) == NULL)
	{
		//UnionLog("in UnionCreateTaskInstance:: taskClass [%s] not defined! default task Class [defaultTask] used!\n",taskName);
		if ((pclass = UnionFindTaskClass("defaultTask")) == NULL)
		{
			UnionUserErrLog("in UnionCreateTaskInstance:: taskClass [%s] not defined! and default task Class [defaultTask] not defined,either!\n",taskName);
			return(NULL);
		}
	}
	if ((pinstance = UnionFindTaskInstance(UnionGetTaskID())) != NULL) 	// 已经创建了一个进程号相同的任务
	{
		if (pinstance->pclass != NULL)
		{
			if (strcmp(pinstance->pclass->name,pclass->name) == 0)
				return(pgthisInstance=pinstance);			// 该任务已经存在
		}
		// 将已存在的同进程号的任务指向的类改为重置新名的类
		if (pinstance->pclass->currentNum > 0)
			--pinstance->pclass->currentNum;
		pinstance->pclass = pclass;
		++pclass->currentNum;
		// Added by Wolfgang Wang, 20040406
		// 删除对原日志的指向
		UnionDisconnectLogFile(pinstance->plogFile);
		pinstance->plogFile =  UnionConnectLogFile(pinstance->pclass->logFileName);
		// End of 20040406 addition
		
		// Added by Wolfgang Wang, 2004/06/18
		strcpy(pinstance->name,taskName);
		// End of Addition of 2004/06/18
		
		return(pgthisInstance=pinstance);
	}
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		/*
		if ((pgunionTaskInstance+i)->procID > 0)
			continue;
		(pgunionTaskInstance+i)->procID = UnionGetTaskID();
		*/
		//modify by hzh in 2013.6.13
		tmp_p_TaskInstance = (volatile PUnionTaskInstance)(pgunionTaskInstance+i);
		//借用一个volatile指针，防止下面的两次条件语句
		// "if ((tmp_p_TaskInstance)->procID > 0)" 被编译器优化成一次。
		if ((tmp_p_TaskInstance)->procID > 0)
			continue;
		//进行加锁保护。 防止并发创建任务时取到相同的位置，引起日志文件乱码及产生core的问题
		UnionLockTaskInstanceIndex(2);   //为保证在加锁异常情况下系统仍能继续运转，这里没判断是否加锁成功
		
		if ((tmp_p_TaskInstance)->procID > 0)  
		{   //在_LOCK()之前，此资源以及被别的任务拿走，放弃，找下一个
			UnionUnLockTaskInstanceIndex();
			continue;
		} //else: 资源成功拿到，而且此此时是有锁的，无需担心被别人抢走。
		(tmp_p_TaskInstance)->procID = UnionGetTaskID();    //把资源抓到手
		UnionUnLockTaskInstanceIndex();  //可以释放锁了，因为资源已经被稳拿.
		//modify end 2013.6.13

		(pgunionTaskInstance+i)->pclass = pclass;
		// Added by Wolfgang Wang, 2004/06/18
		strcpy((pgunionTaskInstance+i)->name,taskName);
		// End of Addition of 2004/06/18
		++pclass->currentNum;
		memset((pgunionTaskInstance+i)->startTime,0,sizeof((pgunionTaskInstance+i)->startTime));
		UnionGetFullSystemDateTime((pgunionTaskInstance+i)->startTime);
		(pgunionTaskInstance+i)->plogFile = UnionConnectLogFile(pclass->logFileName);
		//tanhj
		UnionSetProductVersion((pgunionTaskInstance+i)->pclass->name,UnionReadPredfinedProductVersionNumber());
		// 设置成后台任务
		UnionIgnoreSignals();
#ifdef _useSigUser2_
		signal(SIGUSR2,(void *)UnionTaskActionBeforeExit);
#else
		signal(SIGUSR1,(void *)UnionTaskActionBeforeExit);
#endif
		return(pgthisInstance = pgunionTaskInstance+i);
	}
	UnionUserErrLog("in UnionCreateTaskInstance:: taskInstanceTBL is full!\n");
	return(NULL);
}	

// this function is added by wolfgang wang, 2004/07/01
int UnionRepairDeadTaskInstanceResouce(PUnionTaskInstance pinstance)
{
	PUnionTaskClass		pclass;
	PUnionLogFile		plogfile;

	if (pinstance == NULL)
		return(errCodeParameter);

	UnionLog("in UnionRepairDeadTaskInstanceResouce:: task name = [%s]\n",pinstance->name);
	if ((pclass = UnionFindTaskClass(pinstance->name)) != NULL)
	{
		if (pclass->currentNum > 0)
			--pclass->currentNum;
		if ((plogfile = UnionConnectLogFile(pclass->logFileName)) != NULL)
		{
			if (plogfile->users > 1)
				plogfile->users -= 1;
			UnionDisconnectLogFile(plogfile);
		}
	}
	pinstance->plogFile = NULL;
	pinstance->pclass = NULL;
	/*
	pinstance->procID = -1;

	memset(pinstance->startTime,0,sizeof(pinstance->startTime));
	*/
	//modify by hzh in 2012.6.27, pinstance->procID = -1应该放在最后，标致这资源被释放，此后不应该有任何对资源的修改
	memset(pinstance->startTime,0,sizeof(pinstance->startTime));
	pinstance->procID = -1;
	

	//memset(pinstance,0,sizeof(*pinstance));
	return(0);
}

// This function is rewritten by Wolfgang Wang, 2004/06/30
int UnionUnregisterTaskInstance(PUnionTaskInstance pinstance)
{
	//PUnionTaskClass		pclass;

	if (pinstance == NULL)
		return(errCodeParameter);

	if (pinstance->pclass != NULL)
	{
		if (pinstance->pclass->currentNum > 0)
			--(pinstance->pclass->currentNum);
	}
	
	UnionDisconnectLogFile(pinstance->plogFile);
	pinstance->plogFile = NULL;

	pinstance->pclass = NULL;

	memset(pinstance->startTime,0,sizeof(pinstance->startTime));

	pinstance->procID = -1;
	//memset(pinstance,0,sizeof(*pinstance));
	return(0);
}	

int UnionUnregisterThisTaskInstance()
{
	return(UnionUnregisterTaskInstance(UnionFindTaskInstance(UnionGetTaskID())));
}	

int UnionKillAllTaskInstance()
{
	int	ret = 0;
	int	i;
	int	num = 0;
	int	otherPid;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionKillAllTaskInstance:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		if ((pgunionTaskInstance+i)->procID > 0)
		{
			// mod by hzh, 2012-11-15, 防止procID变为-1的情况
			otherPid = (pgunionTaskInstance+i)->procID;
			if (otherPid <= 0)
				continue;
			if (otherPid == getpid())
				continue;
							
			//if ((pgunionTaskInstance+i)->procID == getpid())	// 自身不杀
			//	continue;
#ifdef _useSigUser2_
			//if (kill((pgunionTaskInstance+i)->procID,SIGUSR2) != 0)
			if (kill(otherPid,SIGUSR2) != 0)
#else
			//if (kill((pgunionTaskInstance+i)->procID,SIGUSR1) != 0)
			if (kill(otherPid,SIGUSR1) != 0)
#endif
			{
				UnionSystemErrLog("in UnionKillAllTaskInstance:: send SIGUSR1 to Process [%d] Failure!\n",(pgunionTaskInstance+i)->procID);
				continue;
			}
			//UnionNullLog("in UnionKillAllTaskInstance:: send exit notice to [%d]\n",(pgunionTaskInstance+i)->procID);
			num++;
		}
	}
	UnionLog("in UnionKillAllTaskInstance:: [%d] tasks killed OK!\n",num);
	return(num);
}

int UnionKillTaskInstanceByName(char *taskName)
{
	int	ret = 0;
	int	i;
	int	num = 0;
	int	myPid;
	int	otherPid;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionKillTaskInstanceByName:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	myPid = getpid();
	
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		// mod by hzh, 2012-11-15, 防止procID变为-1
		otherPid = (pgunionTaskInstance+i)->procID;
		
		if (otherPid <= 0)
			continue;
		if (otherPid == myPid)
			continue;
					
		/*
		if ((pgunionTaskInstance+i)->procID <= 0)
			continue;
		if ((pgunionTaskInstance+i)->procID == myPid)
			continue;
		*/
		
		if ((pgunionTaskInstance+i)->pclass == NULL)
			continue;
		// Deleted by Wolfgang Wang, 2004/06/18 
		// if (strcmp((pgunionTaskInstance+i)->pclass->name,taskName) != 0)
		// added by Wolfgang Wang, 2004/06/18
		if (strcmp((pgunionTaskInstance+i)->name,taskName) != 0)
			continue;
#ifdef _useSigUser2_
		//if (kill((pgunionTaskInstance+i)->procID,SIGUSR2) != 0)
		if (kill(otherPid,SIGUSR2) != 0)
#else
		//if (kill((pgunionTaskInstance+i)->procID,SIGUSR1) != 0)
		if (kill(otherPid,SIGUSR1) != 0)
#endif
		{
			UnionSystemErrLog("in UnionKillTaskInstanceByName:: send SIGUSR1 to Process [%d] Failure!\n",otherPid);
			continue;
		}
		num++;
	}
	return(num);
}

int UnionKillTaskInstanceByProcID(int taskProcID)
{
	int	ret = 0;
	int	i;
	//int	num = 0;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionKillTaskInstanceByProcID:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		if ((pgunionTaskInstance+i)->procID != taskProcID)
			continue;
#ifdef _useSigUser2_
		if (kill((pgunionTaskInstance+i)->procID,SIGUSR2) != 0)
#else
		if (kill((pgunionTaskInstance+i)->procID,SIGUSR1) != 0)
#endif
		{
			UnionSystemErrLog("in UnionKillTaskInstanceByProcID:: send SIGUSR1 to Process [%d] Failure!\n",(pgunionTaskInstance+i)->procID);
			continue;
		}
		else
			return(0);
	}
	return(errCodeTaskMDL_TaskNotExists);
}

int UnionClearRubbishTaskInstance(int seconds)
{
	int	i;
	int	num;
	int	ret;
	//int	realTaskNum;
	//int	currentNum;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionClearRubbishTaskInstance:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	for (;;)
	{
		for (i = 0,num=0; i < pgunionTaskTBL->maxInstanceNum; i++)
		{
			if ((pgunionTaskInstance+i)->procID > 0)
			{
				if (kill((pgunionTaskInstance+i)->procID,0) != 0)
				{
					UnionAuditLog("in UnionClearRubbishTaskInstance:: [%d] no longer exists!\n",(pgunionTaskInstance+i)->procID);
					// UnionUnregisterTaskInstance(pgunionTaskInstance+i); // deleted by Wolfgang Wang, 2004/07/01
					UnionRepairDeadTaskInstanceResouce(pgunionTaskInstance+i); // added by Wolfgang Wang, 2004/07/01
					num++;
				}
			}
		}
		/*	2009/5/26，移到UnionStartTaskGuard中
		// 2006/11/01增加以下函数
		UnionRepairTaskInstanceNum();
		*/
		sleep(seconds);
	}
	
	//return(num);
}

// 修复任务实例计数器,2009/5/26增加
int UnionRepairTaskInstanceNum()
{
	int	i;
	int	num = 0;
	int	ret;
	int	realTaskNum;
	int	currentNum;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionRepairTaskInstanceNum:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		if (strlen((pgunionTaskClass+i)->name) == 0)
			continue;
		if ((pgunionTaskClass+i)->minNum <= 0)	// 只维护必须在线的任务
			continue;
		if ((realTaskNum = UnionExistsTaskOfName((pgunionTaskClass+i)->name)) != 
				(currentNum = (pgunionTaskClass+i)->currentNum))
		{
			(pgunionTaskClass+i)->currentNum = realTaskNum;
			UnionAuditLog("in UnionRepairTaskInstanceNum:: repair [%s] num from [%04d] to [%04d]\n",
				(pgunionTaskClass+i)->name,currentNum,realTaskNum);
			num++;
		}
	}
	return(num);
}

	
int UnionStartTaskGuard(int seconds)
{
	int	i;
	int	j;
	int	ret;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionStartTaskGuard:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	for (;;)
	{
		UnionRepairTaskInstanceNum();
		UnionProgramerLog("in UnionStartTaskGuard:: now working [pgunionTaskTBL->maxClassNum] = [%d]...\n",pgunionTaskTBL->maxClassNum);
		for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
		{
			if (strlen((pgunionTaskClass+i)->name) == 0)
				continue;
			for (j = (pgunionTaskClass+i)->currentNum; j < (pgunionTaskClass+i)->minNum; j++)
			{
				UnionLog("in UnionStartTaskGuard:: now calling [%s]\n",(pgunionTaskClass+i)->startCmd);
				system((pgunionTaskClass+i)->startCmd);
				UnionLog("in UnionStartTaskGuard:: calling [%s] sucess!\n",(pgunionTaskClass+i)->startCmd);
			}
		}
		sleep(seconds);
	}
}
	
int UnionKillTaskInstanceByAlikeName(char *taskName)
{
	int	ret = 0;
	int	i;
	int	num = 0;
	long	myPid;
	char	*ptr;
	long 	otherPid = 0;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionKillTaskInstanceByAlikeName:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	myPid = UnionGetTaskID();
	
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		/*
		if ((pgunionTaskInstance+i)->procID <= 0)
			continue;
		if ((pgunionTaskInstance+i)->procID == myPid)
			continue;
		*/
		
		otherPid = (pgunionTaskInstance+i)->procID;
		if (otherPid <= 0 || otherPid == myPid)
			continue;
			
		if ((pgunionTaskInstance+i)->pclass == NULL)
			continue;
		//if (strcmp((pgunionTaskInstance+i)->name,taskName) != 0)
		//	continue;

		// add by xusj 20100326 begin
                if (strlen(taskName) == 0)
                        continue;
                // add by xusj 20100326 end

		if ((ptr = strstr((pgunionTaskInstance+i)->name,taskName)) == NULL)
			continue;
/*
#ifdef _useSigUser2_
		if (kill((pgunionTaskInstance+i)->procID,SIGUSR2) != 0)
#else
		if (kill((pgunionTaskInstance+i)->procID,SIGUSR1) != 0)
#endif
		{
			UnionSystemErrLog("in UnionKillTaskInstanceByAlikeName:: send SIGUSR1 to Process [%d] Failure!\n",(pgunionTaskInstance+i)->procID);
			continue;
		}
		*/
		// modify by hzh in 2009.11.10, 避免共享内存值(pgunionTaskInstance+i)->procID,刚好在此时被其他进程改为-1的情况。
#ifdef _useSigUser2_
		if (kill(otherPid,SIGUSR2) != 0)
#else
		if (kill(otherPid,SIGUSR1) != 0)
#endif
		{
			UnionSystemErrLog("in UnionKillTaskInstanceByAlikeName:: send SIGUSR1 to Process [%ld] Failure!\n",otherPid);
			continue;
		}
		
		num++;
	}
	return(num);
}

// 2013-01-25 added UnionKillTaskInstanceByAlikeName_use_SIGKILL
int UnionKillTaskInstanceByAlikeName_use_SIGKILL(char *taskName)
{
	int	ret = 0;
	int	i;
	int	num = 0;
	long	myPid;
	char	*ptr;
	long 	otherPid = 0;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionKillTaskInstanceByAlikeName_use_SIGKILL:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	myPid = UnionGetTaskID();
	
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		otherPid = (pgunionTaskInstance+i)->procID;
		if (otherPid <= 0 || otherPid == myPid)
			continue;
			
		if ((pgunionTaskInstance+i)->pclass == NULL)
			continue;

                if (strlen(taskName) == 0)
                        continue;

		if (strstr(taskName, "mngTask") != 0)
			continue;

		if ((ptr = strstr((pgunionTaskInstance+i)->name,taskName)) == NULL)
			continue;

		if (kill(otherPid,SIGKILL) != 0)
		{
			UnionSystemErrLog("in UnionKillTaskInstanceByAlikeName_use_SIGKILL:: send SIGKILL to Process [%ld] Failure!\n",otherPid);
			continue;
		}
		//UnionRepairDeadTaskInstanceResouce(pgunionTaskInstance+i);
		
		num++;
	}
	return(num);
}

// 2008/10/24，王纯军增加
int UnionExistsTaskOfAlikeName(char *taskName)
{
	int	ret = 0;
	int	i;
	int	num = 0;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionExistsTaskOfAlikeName:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		if ((pgunionTaskInstance+i)->procID <= 0)
			continue;
		if (strstr((pgunionTaskInstance+i)->name,taskName) == NULL)
			continue;
		num++;
	}
	return(num);
}

int UnionExistsTaskOfName(char *taskName)
{
	int	ret = 0;
	int	i;
	int	num = 0;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionExistsTaskOfName:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		if ((pgunionTaskInstance+i)->procID <= 0)
			continue;
		if (strcmp((pgunionTaskInstance+i)->name,taskName) != 0)
			continue;
		num++;
	}
	return(num);
}

int UnionExistsAnotherTaskOfName(char *taskName)
{
	int	ret = 0;
	int	i;
	int	num = 0;
	int	myPid;
	//char	*ptr;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionExistsAnotherTaskOfName:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	myPid = UnionGetTaskID();
	
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		if ((pgunionTaskInstance+i)->procID <= 0)
			continue;
		if ((pgunionTaskInstance+i)->procID == myPid)
			continue;
		if (strcmp((pgunionTaskInstance+i)->name,taskName) != 0)
			continue;
		num++;
	}
	return(num);
}

// added by Wolfgang Wang, 2005/09/08
int UnionPrintTaskInstanceByAlikeNameToFile(char *taskName,FILE *fp)
{
	int	ret = 0;
	int	i;
	int	num = 0;
	char	*ptr;
	
	if ((fp == NULL) || (taskName == NULL))
	{
		UnionUserErrLog("in UnionPrintTaskInstanceByAlikeNameToFile:: fp/taskName is null!\n");
		return(errCodeParameter);
	}
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintTaskInstanceByAlikeNameToFile:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		if ((pgunionTaskInstance+i)->procID <= 0)
			continue;
		if ((pgunionTaskInstance+i)->pclass == NULL)
			continue;
		if ((ptr = strstr((pgunionTaskInstance+i)->name,taskName)) == NULL)
			continue;
		fprintf(fp,"[%05d] [%40s]\n",(pgunionTaskInstance+i)->procID,(pgunionTaskInstance+i)->name);
		num++;
	}
	return(num);
}

// 这个函数和Kill函数的区别在于，他还置这类函数的启动数量为0。
// added by Wolfgang Wang, 2005/09/08
int UnionCloseTaskInstanceByAlikeName(char *taskName)
{
	int	ret = 0;
	int	i;
	int	num = 0;
	int	myPid;
	char	*ptr;
	int	otherPid;  //add by hzh in 2012.11.15
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionCloseTaskInstanceByAlikeName:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	myPid = getpid();

	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		if (strlen((pgunionTaskClass+i)->name) == 0)
			continue;
		if ((ptr = strstr((pgunionTaskClass+i)->name,taskName)) == NULL)
			continue;
		(pgunionTaskClass+i)->minNum = 0;
	}
	
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		otherPid = (pgunionTaskInstance+i)->procID;
		
		if (otherPid <= 0)
			continue;
		if (otherPid == myPid)
			continue;
				
		/*
		if ((pgunionTaskInstance+i)->procID <= 0)
			continue;
		if ((pgunionTaskInstance+i)->procID == myPid)
			continue;
		*/
		if ((pgunionTaskInstance+i)->pclass == NULL)
			continue;
		if ((ptr = strstr((pgunionTaskInstance+i)->name,taskName)) == NULL)
			continue;
#ifdef _useSigUser2_
		//if (kill((pgunionTaskInstance+i)->procID,SIGUSR2) != 0)
		if (kill(otherPid,SIGUSR2) != 0)
#else
		//if (kill((pgunionTaskInstance+i)->procID,SIGUSR1) != 0)
		if (kill(otherPid,SIGUSR1) != 0)
#endif
		{
			UnionSystemErrLog("in UnionCloseTaskInstanceByAlikeName:: send SIGUSR1 to Process [%d] Failure!\n",otherPid);
			//continue;
		}
		else
			num++;
	}
	return(num);
}
// 这个函数和Kill函数的区别在于，他还置这类函数的启动数量为0。
// added by Wolfgang Wang, 2005/09/08
int UnionCloseTaskInstanceByName(char *taskName)
{
	int	ret = 0;
	int	i;
	int	num = 0;
	int	myPid;
	//char	*ptr;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionCloseTaskInstanceByName:: UnionConnectTaskTBL!\n");
		return(ret);
	}

	myPid = UnionGetTaskID();

	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		if (strlen((pgunionTaskClass+i)->name) == 0)
			continue;
		if (strcmp((pgunionTaskClass+i)->name,taskName) != 0)
			continue;
		(pgunionTaskClass+i)->minNum = 0;
	}
	
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		if ((pgunionTaskInstance+i)->procID <= 0)
			continue;
		if ((pgunionTaskInstance+i)->procID == myPid)
			continue;
		if ((pgunionTaskInstance+i)->pclass == NULL)
			continue;
		if (strcmp((pgunionTaskInstance+i)->name,taskName) != 0)
			continue;
#ifdef _useSigUser2_
		if (kill((pgunionTaskInstance+i)->procID,SIGUSR2) != 0)
#else
		if (kill((pgunionTaskInstance+i)->procID,SIGUSR1) != 0)
#endif
		{
			UnionSystemErrLog("in UnionCloseTaskInstanceByName:: send SIGUSR1 to Process [%d] Failure!\n",(pgunionTaskInstance+i)->procID);
			//continue;
		}
		else
			num++;
	}
	return(num);
}
// added by Wolfgang Wang,2005/10/09
int UnionRenameTaskNameOfMyself(char *fmt,...)
{
	int	ret = 0;
	int	i;
	//int	num = 0;
	int	pid;
	char	tmpBuf[128];
	va_list 		args;
		
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionRenameTaskNameOfMyself:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	pid = UnionGetTaskID();
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		if ((pgunionTaskInstance+i)->procID != pid)
			continue;
		memset(tmpBuf,0,sizeof(tmpBuf));
        	va_start(args,fmt);
        	vsprintf(tmpBuf,fmt,args);
        	va_end(args);
        	if (strlen(tmpBuf) >= sizeof((pgunionTaskInstance+i)->name))
        	{
        		memset((pgunionTaskInstance+i)->name,0,sizeof((pgunionTaskInstance+i)->name));
        		memcpy((pgunionTaskInstance+i)->name,tmpBuf,sizeof((pgunionTaskInstance+i)->name)-1);
        	}
        	else
        		strcpy((pgunionTaskInstance+i)->name,tmpBuf);
        	return(0);
	}
	return(errCodeTaskMDL_TaskNotExists);
}

// 检查是否有名字中含有taskName的任务类型定义,返回数量
int UnionExistsTaskClassOfAlikeName(char *taskName)
{
	int	i;
	int	ret;
	int	num = 0;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionExistsTaskClassOfAlikeName:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		if (strstr((pgunionTaskClass+i)->name,taskName) == NULL)
			continue;
		num++;
	}
	
	return(num);
}

// 检查是否有名字为taskName的任务类型定义,返回数量
int UnionExistsTaskClassOfName(char *taskName)
{
	int	i;
	int	ret;
	int	num = 0;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionExistsTaskClassOfName:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		if (strcmp((pgunionTaskClass+i)->name,taskName) == 0)
			num++;
	}
	
	return(num);
}

// 将指定的域输出到文件中
// 合法的域名
// minnum/currentnum/name/logfile/startcmd
// activeNum指明输出的任务的当前启动数量必须达到的数目
int UnionPrintTaskClassInTBLToSpecFile(char *fileName,char *fldList,int activeNum)
{
	int	i;
	int	num = 0;
	int	ret;
	int	outputMinNum=0,outputCurrentNum=0,outputLogFile=0,outputStartCommand=0;
	FILE	*fp;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintTaskClassInTBLToSpecFile:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	if ((fileName == NULL) || (strlen(fileName) == 0) || (strcmp(fileName,"stdout") == 0) || (strcmp(fileName,"stderr") == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintTaskClassInTBLToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	UnionToUpperCase(fldList);
	if (strstr(fldList,"MINNUM") != NULL)
		outputMinNum = 1;
	if (strstr(fldList,"CURRENTNUM") != NULL)
		outputCurrentNum = 1;
	if (strstr(fldList,"LOGFILE") != NULL)
		outputLogFile = 1;
	if (strstr(fldList,"STARTCMD") != NULL)
		outputStartCommand = 1;
	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		if (strlen((pgunionTaskClass+i)->name) == 0)
			continue;
		if ((pgunionTaskClass+i)->currentNum < activeNum)
			continue;
		if (outputMinNum)
			fprintf(fp,"minNum=%04d|",(pgunionTaskClass+i)->minNum);
		if (outputCurrentNum)
			fprintf(fp,"currentNum=%04d|",(pgunionTaskClass+i)->currentNum);
		fprintf(fp,"name=%s|",(pgunionTaskClass+i)->name);
		if (outputLogFile)
			fprintf(fp,"logFile=%s|",(pgunionTaskClass+i)->logFileName);
		if (outputStartCommand)
			fprintf(fp,"startCmd=%s|",(pgunionTaskClass+i)->startCmd);
		fprintf(fp,"\n");
		num++;
	}
	if (fp != stdout)
		fclose(fp);
	return(num);
}
// 将必须在线的任务的指定的域输出到文件中
// 合法的域名
// minnum/currentnum/name/logfile/startcmd
int UnionPrintMustOnlineTaskClassToSpecFile(char *fileName,char *fldList)
{
	int	i;
	int	num = 0;
	int	ret;
	int	outputMinNum=0,outputCurrentNum=0,outputLogFile=0,outputStartCommand=0;
	FILE	*fp;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintMustOnlineTaskClassToSpecFile:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	if ((fileName == NULL) || (strlen(fileName) == 0) || (strcmp(fileName,"stdout") == 0) || (strcmp(fileName,"stderr") == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintMustOnlineTaskClassToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	UnionToUpperCase(fldList);
	if (strstr(fldList,"MINNUM") != NULL)
		outputMinNum = 1;
	if (strstr(fldList,"CURRENTNUM") != NULL)
		outputCurrentNum = 1;
	if (strstr(fldList,"LOGFILE") != NULL)
		outputLogFile = 1;
	if (strstr(fldList,"STARTCMD") != NULL)
		outputStartCommand = 1;
	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		if (strlen((pgunionTaskClass+i)->name) == 0)
			continue;
		if ((pgunionTaskClass+i)->minNum <= 0)
			continue;
		if (outputMinNum)
			fprintf(fp,"minNum=%04d|",(pgunionTaskClass+i)->minNum);
		if (outputCurrentNum)
			fprintf(fp,"currentNum=%04d|",(pgunionTaskClass+i)->currentNum);
		fprintf(fp,"name=%s|",(pgunionTaskClass+i)->name);
		if (outputLogFile)
			fprintf(fp,"logFile=%s|",(pgunionTaskClass+i)->logFileName);
		if (outputStartCommand)
			fprintf(fp,"startCmd=%s|",(pgunionTaskClass+i)->startCmd);
		fprintf(fp,"\n");
		num++;
	}
	if (fp != stdout)
		fclose(fp);
	return(num);
}

// 将异常任务输出到文件中
// 合法的域名
// minnum/currentnum/name/logfile/startcmd
int UnionPrintAbnormalTaskClassToSpecFile(char *fileName,char *fldList)
{
	int	i;
	int	num = 0;
	int	ret;
	int	outputMinNum=0,outputCurrentNum=0,outputLogFile=0,outputStartCommand=0;
	FILE	*fp;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAbnormalTaskClassToSpecFile:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	if ((fileName == NULL) || (strlen(fileName) == 0) || (strcmp(fileName,"stdout") == 0) || (strcmp(fileName,"stderr") == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintAbnormalTaskClassToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	UnionToUpperCase(fldList);
	if (strstr(fldList,"MINNUM") != NULL)
		outputMinNum = 1;
	if (strstr(fldList,"CURRENTNUM") != NULL)
		outputCurrentNum = 1;
	if (strstr(fldList,"LOGFILE") != NULL)
		outputLogFile = 1;
	if (strstr(fldList,"STARTCMD") != NULL)
		outputStartCommand = 1;
	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		if (strlen((pgunionTaskClass+i)->name) == 0)
			continue;
		if ((pgunionTaskClass+i)->minNum <= 0)
			continue;
		if ((pgunionTaskClass+i)->minNum <= (pgunionTaskClass+i)->currentNum)
			continue;
		if (outputMinNum)
			fprintf(fp,"minNum=%04d|",(pgunionTaskClass+i)->minNum);
		if (outputCurrentNum)
			fprintf(fp,"currentNum=%04d|",(pgunionTaskClass+i)->currentNum);
		fprintf(fp,"name=%s|",(pgunionTaskClass+i)->name);
		if (outputLogFile)
			fprintf(fp,"logFile=%s|",(pgunionTaskClass+i)->logFileName);
		if (outputStartCommand)
			fprintf(fp,"startCmd=%s|",(pgunionTaskClass+i)->startCmd);
		fprintf(fp,"\n");
		num++;
	}
	if (fp != stdout)
		fclose(fp);
	return(num);
}
// 将任务实例输出到文件中
// 合法的域名
// pid/starttime/logfile
int UnionPrintTaskInstanceToSpecFile(char *fileName,char *fldList)
{
	int	i;
	int	num = 0;
	int	ret;
	int	outputPID=0,outputLogFile=0,outputStartTime=0;
	FILE	*fp;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintTaskInstanceToSpecFile:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	if ((fileName == NULL) || (strlen(fileName) == 0) || (strcmp(fileName,"stdout") == 0) || (strcmp(fileName,"stderr") == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintTaskInstanceToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	//UnionAuditNullLog("in UnionPrintTaskInstanceToSpecFile:: before toupper case...\n");
	UnionToUpperCase(fldList);
	UnionDebugLog("pgunionTaskTBL->maxInstanceNum = [%d]\n",pgunionTaskTBL->maxInstanceNum);
	if (strstr(fldList,"PID") != NULL)
		outputPID = 1;
	if (strstr(fldList,"LOGFILE") != NULL)
		outputLogFile = 1;
	if (strstr(fldList,"STARTTIME") != NULL)
		outputStartTime = 1;
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
	
		if ((pgunionTaskInstance+i)->procID <= 0)
			continue;
		if (outputPID)
			fprintf(fp,"pid=%05d|",(pgunionTaskInstance+i)->procID);
		if (outputStartTime)
			fprintf(fp,"startTime=%s|",(pgunionTaskInstance+i)->startTime);
		if (outputLogFile)
			fprintf(fp,"logFile=%s|",(pgunionTaskInstance+i)->plogFile->name);
		fprintf(fp,"name=%s|",(pgunionTaskInstance+i)->name);
		fprintf(fp,"\n");
		num++;
	}
	if (fp != stdout)
		fclose(fp);
	return(num);
}

// 重置任务的当前数量值为真正值
int UnionResetTaskNumOfName(char *taskName)
{
	int	i;
	int	ret;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionResetTaskNumOfName:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		if (strlen((pgunionTaskClass+i)->name) == 0)
			continue;
		if (strcmp((pgunionTaskClass+i)->name,taskName) == 0)
		{
			(pgunionTaskClass+i)->currentNum = UnionExistsTaskOfName(taskName);
			return((pgunionTaskClass+i)->currentNum);
		}
	}
	return(errCodeTaskMDL_TaskClassNotExists);
}

// 设置任务的当前数量为指定值
int UnionSetTaskNumOfName(char *taskName,int num)
{
	int	i;
	int	ret;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionSetTaskNumOfName:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		if (strlen((pgunionTaskClass+i)->name) == 0)
			continue;
		if (strcmp((pgunionTaskClass+i)->name,taskName) == 0)
		{
			(pgunionTaskClass+i)->currentNum = num;
			return((pgunionTaskClass+i)->currentNum);
		}
	}
	return(errCodeTaskMDL_TaskClassNotExists);
}
PUnionTaskInstance UnionCreateTaskInstanceOfName(int (*UnionTaskActionBeforeExit)(),char *fmt,...)
{
	va_list 		args;
	char			taskName[256];
	PUnionTaskInstance	pinstance;
	PUnionTaskClass		pclass;
	int			i;
	int			ret;
	
	volatile PUnionTaskInstance tmp_p_TaskInstance;
	
	// Added by Wolfgang Wang, 2004/11/25
	if (!UnionIsProductStillEffective())
	{
		UnionUserErrLog("in UnionCreateTaskInstance:: product outof date of Application [%s]!\n",UnionGetApplicationName());
		return(NULL);
	}
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionCreateTaskInstanceOfName:: UnionConnectTaskTBL!\n");
		return(NULL);
	}
	
	memset(taskName,0,sizeof(taskName));
        va_start(args,fmt);
        vsprintf(taskName,fmt,args);
        va_end(args);
	
	if ((pclass = UnionFindTaskClass(taskName)) == NULL)
	{
		UnionUserErrLog("in UnionCreateTaskInstanceOfName:: taskClass [%s] not defined! default task Class [defaultTask] used!\n",taskName);
		return(NULL);
	}
	if ((pinstance = UnionFindTaskInstance(UnionGetTaskID())) != NULL)	// 已经创建了一个进程号相同的任务
	{
		if (pinstance->pclass != NULL)
		{
			if (strcmp(pinstance->pclass->name,pclass->name) == 0)
				return(pgthisInstance=pinstance);			// 该任务已经存在
		}
		// 将已存在的同进程号的任务指向的类改为重置新名的类
		if (pinstance->pclass->currentNum > 0)
			--pinstance->pclass->currentNum;
		pinstance->pclass = pclass;
		++pclass->currentNum;
		// Added by Wolfgang Wang, 20040406
		// 删除对原日志的指向
		UnionDisconnectLogFile(pinstance->plogFile);
		pinstance->plogFile =  UnionConnectLogFile(pinstance->pclass->logFileName);
		// End of 20040406 addition
		
		// Added by Wolfgang Wang, 2004/06/18
		UnionProgramerLog("in UnionCreateTaskInstanceOfName:: pinstance->name[%s] taskName[%s]\n",pinstance->name,taskName);
		strcpy(pinstance->name,taskName);
		// End of Addition of 2004/06/18
		
		return(pgthisInstance=pinstance);
	}
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		/*
		if ((pgunionTaskInstance+i)->procID > 0)
			continue;
		(pgunionTaskInstance+i)->procID = UnionGetTaskID();
		*/
		//modify by hzh in 2013.6.13
		tmp_p_TaskInstance = (volatile PUnionTaskInstance)  pgunionTaskInstance+i;
		//借用一个volatile指针，防止下面的两次条件语句
		// "if ((tmp_p_TaskInstance)->procID > 0)" 被编译器优化成一次。
		if ((tmp_p_TaskInstance)->procID > 0)
			continue;
		//进行加锁保护。 防止并发创建任务时取到相同的位置，引起日志文件乱码及产生core的问题
		UnionLockTaskInstanceIndex(2); //为保证在加锁异常情况下系统仍能继续运转，这里没判断是否加锁成功
		
		if ((tmp_p_TaskInstance)->procID > 0)  
		{   //在_LOCK()之前，此资源以及被别的任务拿走，放弃，找下一个
			UnionUnLockTaskInstanceIndex();
			continue;
		} //else: 资源成功拿到，而且此此时是有锁的，无需担心被别人抢走。
		(tmp_p_TaskInstance)->procID = UnionGetTaskID();    //把资源抓到手
		UnionUnLockTaskInstanceIndex();  //可以释放锁了，因为资源已经被稳拿.
		//modify end 2013.6.13


		(pgunionTaskInstance+i)->pclass = pclass;
		// Added by Wolfgang Wang, 2004/06/18
		strcpy((pgunionTaskInstance+i)->name,taskName);
		// End of Addition of 2004/06/18
		++pclass->currentNum;
		memset((pgunionTaskInstance+i)->startTime,0,sizeof((pgunionTaskInstance+i)->startTime));
		UnionGetFullSystemDateTime((pgunionTaskInstance+i)->startTime);
		(pgunionTaskInstance+i)->plogFile = UnionConnectLogFile(pclass->logFileName);
		
		UnionSetProductVersion((pgunionTaskInstance+i)->pclass->name,UnionReadPredfinedProductVersionNumber());

		// 设置成后台任务
		UnionIgnoreSignals();
#ifdef _useSigUser2_
		signal(SIGUSR2,(void *)UnionTaskActionBeforeExit);
#else
		signal(SIGUSR1,(void *)UnionTaskActionBeforeExit);
#endif

		return(pgthisInstance = pgunionTaskInstance+i);
	}
	UnionUserErrLog("in UnionCreateTaskInstanceOfName:: taskInstanceTBL is full!\n");
	return(NULL);
}


// 20150605 zhangyd增加，用于子进程创建任务实例
PUnionTaskInstance UnionCreateTaskInstanceForClient(int (*UnionTaskActionBeforeExit)(), char *fmt,...)
{
	va_list 		args;
	char			taskName[256];
	PUnionTaskInstance	pinstance;
	PUnionTaskClass		pclass;
	int			i;
	int			ret;

	volatile PUnionTaskInstance tmp_p_TaskInstance;

	/*
	// Added by Wolfgang Wang, 2004/11/25
	if (!UnionIsProductStillEffective())
	{
		UnionUserErrLog("in UnionCreateTaskInstanceForClient:: product outof date of Application [%s]!\n",UnionGetApplicationName());
		return(NULL);
	}
	*/
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionCreateTaskInstanceForClient:: UnionConnectTaskTBL!\n");
		return(NULL);
	}
	
	memset(taskName,0,sizeof(taskName));
	va_start(args,fmt);
	vsprintf(taskName,fmt,args);
	va_end(args);

	if ((pinstance = UnionFindTaskInstance(UnionGetTaskID())) != NULL) 	// 已经创建了一个进程号相同的任务
	{
		//UnionLog("in UnionCreateTaskInstanceForClient:: rename from [%s] to [%s]!\n", pinstance->name, taskName);
		strcpy(pinstance->name, taskName); // 换名
		
		return(pinstance);
	}

	if ((pclass = UnionFindTaskClass(taskName)) == NULL)
	{
		UnionUserErrLog("in UnionCreateTaskInstanceForClient:: UnionFindTaskClass taskName [%s] return null!\n", taskName);
		return(NULL);
	}

	UnionLog("in UnionCreateTaskInstanceForClient:: create instance taskName [%s]!\n", taskName);

	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		/*
		if ((pgunionTaskInstance+i)->procID > 0)
			continue;
		(pgunionTaskInstance+i)->procID = UnionGetTaskID();
		*/
		//modify by hzh in 2013.6.13
		tmp_p_TaskInstance = (volatile PUnionTaskInstance)(pgunionTaskInstance+i);
		//借用一个volatile指针，防止下面的两次条件语句
		// "if ((tmp_p_TaskInstance)->procID > 0)" 被编译器优化成一次。
		if ((tmp_p_TaskInstance)->procID > 0)
			continue;
		//进行加锁保护。 防止并发创建任务时取到相同的位置，引起日志文件乱码及产生core的问题
		UnionLockTaskInstanceIndex(2);   //为保证在加锁异常情况下系统仍能继续运转，这里没判断是否加锁成功
		
		if ((tmp_p_TaskInstance)->procID > 0)  
		{   //在_LOCK()之前，此资源以及被别的任务拿走，放弃，找下一个
			UnionUnLockTaskInstanceIndex();
			continue;
		} //else: 资源成功拿到，而且此此时是有锁的，无需担心被别人抢走。
		(tmp_p_TaskInstance)->procID = UnionGetTaskID();    //把资源抓到手
		UnionUnLockTaskInstanceIndex();  //可以释放锁了，因为资源已经被稳拿.
		//modify end 2013.6.13

		(pgunionTaskInstance+i)->pclass = pclass;
		// Added by Wolfgang Wang, 2004/06/18
		strcpy((pgunionTaskInstance+i)->name,taskName);
		// End of Addition of 2004/06/18
		++pclass->currentNum;
		memset((pgunionTaskInstance+i)->startTime,0,sizeof((pgunionTaskInstance+i)->startTime));
		UnionGetFullSystemDateTime((pgunionTaskInstance+i)->startTime);
		(pgunionTaskInstance+i)->plogFile = UnionConnectLogFile(pclass->logFileName);
		//tanhj
		UnionSetProductVersion((pgunionTaskInstance+i)->pclass->name,UnionReadPredfinedProductVersionNumber());
		// 设置成后台任务
		UnionIgnoreSignals();
#ifdef _useSigUser2_
		signal(SIGUSR2,(void *)UnionTaskActionBeforeExit);
#else
		signal(SIGUSR1,(void *)UnionTaskActionBeforeExit);
#endif
		return(pgthisInstance = pgunionTaskInstance+i);
	}
	UnionUserErrLog("in UnionCreateTaskInstanceForClient:: taskInstanceTBL is full!\n");
	return(NULL);
}

PUnionTaskInstance UnionCreateTaskInstanceForSub(char *parentLog, int (*UnionTaskActionBeforeExit)(), char *fmt,...)
{
	int			ret = 0;
	char			instanceName[128];
	va_list 		args;
	PUnionTaskInstance	pinstance = NULL;

	va_start(args, fmt);
	vsprintf(instanceName, fmt, args);
	va_end(args);

	if(UnionFindTaskClass(instanceName) == NULL)
	{
		ret = UnionUpdateClientClassNum(instanceName, parentLog, 0);
		if(ret < 0)
		{
			UnionUserErrLog("in UnionCreateTaskInstanceForSub:: UnionUpdateClientClassNum taskName [%s] ret = %d!\n", instanceName, ret);
			return(NULL);
		}
		if ((pinstance = UnionCreateTaskInstanceForClient(UnionTaskActionBeforeExit, "%s", instanceName)) == NULL)
		{
			UnionUserErrLog("in UnionCreateTaskInstanceForSub:: UnionCreateTaskInstanceForClient [%s]Error!\n", instanceName);
			return(NULL);
		}
	}
	else
	{
		if ((pinstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit, "%s", instanceName)) == NULL)
		{
			UnionUserErrLog("in UnionCreateTaskInstanceForSub:: UnionCreateTaskInstance [%s]Error!\n", instanceName);
			return(NULL);
		}
	}
	return(pinstance);
}

// 更新类表的实例的子进程类数目
int UnionUpdateClientClassNum(char *className, char *logName, int num)
{
	int			index = 0, ret = 0;
	PUnionTaskClass		pclass = NULL;
	TUnionTaskClass		newClass;

	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionUpdateClientClassNum:: UnionConnectTaskTBL!\n");
		return(ret);
	}

	if ((pclass = UnionFindTaskClass(className)) == NULL)
	{
		memset(&newClass, 0, sizeof(newClass));
		strcpy(newClass.name, className);
		strcpy(newClass.startCmd, "null");
		newClass.minNum = 0;
		if(logName != NULL)
		{
			strcpy(newClass.logFileName, logName);
		}
		else
		{
			strcpy(newClass.logFileName, "unknown");
		}

		for (index = 0; index < pgunionTaskTBL->maxClassNum; index++)
		{
			if (strlen((pgunionTaskClass+index)->name) == 0)	// 在类型表中查找到了一个空位置
			{
				pclass = pgunionTaskClass+index;
				memcpy(pclass, &newClass, sizeof(newClass));
				break;
			}
		}
		if (index == pgunionTaskTBL->maxClassNum)
		{
			UnionUserErrLog("in UnionUpdateClientClassNum:: table is full!\n");
			return(errCodeParameter);
		}
	}

	pclass->currentNum = num;

	return(num);
}
// 统计各个实例名的数目
int UnionCountTaskInstanceNumByNameGrp(int grpNum, char instanceNameGrp[][64], int *numGrp)
{
	int		ret = 0, i = 0, j = 0;
	char		curTaskname[64];

	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionCountTaskInstanceNumByNameGrp:: UnionConnectTaskTBL!\n");
		return(ret);
	}

	// 初始化
	for(j = 0; j < grpNum; j++)
	{
		*(numGrp + j) = 0;
	}

	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		if ((pgunionTaskInstance+i)->procID > 0)
		{
			if((pgunionTaskInstance+i)->name)
			{
				strcpy(curTaskname, (pgunionTaskInstance+i)->name);
			}
			else
			{
				continue;
			}

			for(j = 0; j < grpNum; j++)
			{
				// modify by leipp 20151017, 因为子进程追加了IP地址,所以采用模糊查询
				//if(strcmp(curTaskname, instanceNameGrp[j]) == 0)
				if(strstr(curTaskname, instanceNameGrp[j]) != NULL)
				{
					*(numGrp + j) += 1;
				}
			}
		}
	}

	return(grpNum);
}

// wangk add 2009-9-24
int UnionPrintTaskClassInTBLInRecStrToSpecFile(char *fileName)
{
	int	i;
	int	num = 0;
	int	ret;
	//int	outputMinNum=0,outputCurrentNum=0,outputLogFile=0,outputStartCommand=0;
	FILE	*fp;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintTaskClassInTBLInRecStrToSpecFile:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	if ((fileName == NULL) || (strlen(fileName) == 0))
	{
		UnionUserErrLog("in UnionPrintTaskClassInTBLInRecStrToSpecFile:: NULL poionter!\n");
		return(errCodeParameter);
	}

	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintTaskClassInTBLInRecStrToSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		if (strlen((pgunionTaskClass+i)->name) == 0)
			continue;

		fprintf(fp,"name=%s|startCmd=%s|num=%d|logFileName=%s|\n", (pgunionTaskClass+i)->name,
			(pgunionTaskClass+i)->startCmd, (pgunionTaskClass+i)->currentNum, (pgunionTaskClass+i)->logFileName);
		num++;
	}
	if (fp != stdout)
		fclose(fp);
	return(num);
}
// wangk add end 2009-9-24
// wangk add 2009-9-29
int UnionPrintTaskInstanceInRecStrFormatToSpecFile(char *fileName)
{
	int	i;
	int	num = 0;
	int	ret;
	//int	outputPID=0,outputLogFile=0,outputStartTime=0;
	FILE	*fp;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintTaskInstanceInRecStrFormatToSpecFile:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	if( (fileName == NULL) || (strlen(fileName) == 0) )
	{
		UnionUserErrLog("in UnionPrintTaskInstanceInRecStrFormatToSpecFile:: NULL poionter!\n");
		return(errCodeParameter);
	}

	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintTaskInstanceInRecStrFormatToSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	UnionDebugLog("pgunionTaskTBL->maxInstanceNum = [%d]\n",pgunionTaskTBL->maxInstanceNum);
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		if ((pgunionTaskInstance+i)->procID <= 0)
			continue;

		fprintf(fp,"name=%s|procID=%d|startTime=%s|logFile=%12p|\n", (pgunionTaskInstance+i)->name,
			(pgunionTaskInstance+i)->procID, (pgunionTaskInstance+i)->startTime, 
			(pgunionTaskInstance+i)->plogFile);
		num++;
	}
	if (fp != stdout)
		fclose(fp);
	return(num);
}
// wangk add end 2009-9-29

PUnionTaskInstance UnionResetTaskInstanceLogFile(int (*UnionTaskActionBeforeExit)(),char *fmt,...)
{
	va_list 		args;
	char			taskName[256];
	PUnionTaskInstance	pinstance;
	PUnionTaskClass		pclass;
	//int			i;
	int			ret;
	
	// Added by Wolfgang Wang, 2004/11/25
	if (!UnionIsProductStillEffective())
	{
		UnionUserErrLog("in UnionResetTaskInstanceLogFile:: product outof date of Application [%s]!\n",UnionGetApplicationName());
		return(NULL);
	}
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionResetTaskInstanceLogFile:: UnionConnectTaskTBL!\n");
		return(NULL);
	}
	
	memset(taskName,0,sizeof(taskName));
        va_start(args,fmt);
        vsprintf(taskName,fmt,args);
        va_end(args);
	
	if ((pclass = UnionFindTaskClass(taskName)) == NULL)
	{
		UnionUserErrLog("in UnionResetTaskInstanceLogFile:: taskClass [%s] not defined! default task Class [defaultTask] used!\n",taskName);
		return(NULL);
	}
	if ((pinstance = UnionFindTaskInstance(UnionGetTaskID())) != NULL)	// 已经创建了一个进程号相同的任务
	{
		if (pinstance->pclass != NULL)
		{
			if (strcmp(pinstance->pclass->name,pclass->name) == 0)
				return(pgthisInstance=pinstance);			// 该任务已经存在
		}
		// 将已存在的同进程号的任务指向的类改为重置新名的类
		if (pinstance->pclass->currentNum > 0)
			--pinstance->pclass->currentNum;
		pinstance->pclass = pclass;
		++pclass->currentNum;
		// Added by Wolfgang Wang, 20040406
		// 删除对原日志的指向
		UnionDisconnectLogFile(pinstance->plogFile);
		pinstance->plogFile =  UnionConnectLogFile(pinstance->pclass->logFileName);
		// End of 20040406 addition
		
		return(pgthisInstance=pinstance);
	}
	UnionUserErrLog("in UnionResetTaskInstanceLogFile:: taskInstanceTBL is not find!\n");
	return(NULL);
}

// added 2012-11-29
// 设置任务执行时间
int UnionUpdateServiceTime()
{
	int		curTime;
	unsigned char	tmpBuf[128+1];
	
	UnionTime(&curTime);

	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf((char *)tmpBuf,"%d",curTime);

	return(UnionUpdateRECImageValue("lastOnlineServiceTime", tmpBuf));
}

// 判断近seconds秒内是否有任务在执行
int UnionIsServiceTimeInSpecSecends(int seconds)
{
	int		lastTime;
	int		curTime;
	
	UnionTime(&curTime);
	lastTime = UnionReadIntTypeRECVar("lastOnlineServiceTime");
	if(lastTime <= 0)
	{
		return(0);
	}
	
	if(curTime - lastTime < seconds)
	{
		UnionProgramerLog("in UnionIsServiceTimeInSpecSecends:: curTime = [%d] lastTime = [%d] spanTime = [%d] !\n", curTime, lastTime, seconds);
		return(1);
	}
	return(0);
}

// 是否有存在线任务在运行
int UnionIsOnlineServiceRuning()
{
	return(UnionReadIntTypeRECVar("lastOnlineServiceTime"));
}

// 设置任务标识1为执行，0为空闲
int UnionUpdateOnlineServiceTag(int runingTag)
{
	int		tag = 1;
	unsigned char	tmpBuf[128+1];

	if((tag = UnionIsOnlineServiceRuning()) == runingTag) // 原值与设置值相等则不需更新
	{
		return(0);
	}
	if(tag < 0)
	{
		UnionLog("in UnionUpdateOnlineServiceTag:: UnionIsOnlineServiceRuning warning ret = %d\n", tag);
		return(0);
	}
	tag = runingTag;
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf((char *)tmpBuf,"%d",tag);
	return(UnionUpdateRECImageValue("lastOnlineServiceTime", (unsigned char *)&tag));
}
// 设置任务为正在进行中
int UnionSetOnlineServiceRuningTag()
{
	return(UnionUpdateOnlineServiceTag(1));
}
// 设置任务标识为空闲
int UnionSetOnlineServiceFreeTag()
{
	return(UnionUpdateOnlineServiceTag(0));
}

