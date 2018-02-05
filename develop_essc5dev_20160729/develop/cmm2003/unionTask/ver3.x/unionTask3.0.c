//	Author:		Wolfgang Wang
//	Date:		2003/09/19
//	Version:	3.0

//	2003/09/19王纯军由 UnionTask2.1.c升级为 unionTask3.0.c
//	3.0与之前的版本比，代码完全不同。

#define _UnionTask_3_x_
#define _UnionEnv_3_x_		// 使用3.x版本的UnionEnv模块
#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdarg.h>
#include <unistd.h>

#include "UnionTask.h"
#include "UnionEnv.h"
#include "unionModule.h"
#include "unionVersion.h"
#include "unionCommand.h"
#include "UnionLog.h"

PUnionSharedMemoryModule	pgunionTaskMDL = NULL;
PUnionTaskTBL			pgunionTaskTBL = NULL;
PUnionTaskClass			pgunionTaskClass = NULL;
PUnionTaskInstance		pgunionTaskInstance = NULL;
PUnionTaskInstance		pgthisInstance = NULL;

PUnionLogFile UnionGetLogFile()
{
	if (pgthisInstance)
		return(pgthisInstance->plogFile);
	return(NULL);
}

int UnionGetNameOfTaskTBL(char *fileName)
{
	sprintf(fileName,"%s/unionTaskTBL.CFG",getenv("UNIONETC"));
	return(0);
}

int UnionGetMaxNumOfTaskClass()
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		maxClassNum;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfTaskTBL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionGetMaxNumOfTaskClass:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("maxClassNum")) == NULL)
	{
		UnionUserErrLog("in UnionGetMaxNumOfTaskClass:: UnionGetEnviVarByName for [%s]\n!","maxClassNum");
		UnionClearEnvi();
		return(-1);
	}
	maxClassNum = atoi(p);
	UnionClearEnvi();
	return(maxClassNum);
}

int UnionGetMaxNumOfTaskInstance()
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		maxInstanceNum;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfTaskTBL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionGetMaxNumOfTaskClass:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("maxInstanceNum")) == NULL)
	{
		UnionUserErrLog("in UnionGetMaxNumOfTaskClass:: UnionGetEnviVarByName for [%s]\n!","maxInstanceNum");
		UnionClearEnvi();
		return(-1);
	}
	maxInstanceNum = atoi(p);
	UnionClearEnvi();
	return(maxInstanceNum);
}

int UnionConnectTaskTBL()
{
	int				maxClassNum,maxInstanceNum;
	
	// 已经连接
	if (pgunionTaskMDL != NULL)
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
		return(-1);
	}
	if ((pgunionTaskTBL = (PUnionTaskTBL)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionTaskMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectTaskTBL:: PUnionTaskTBL!\n");
		return(-1);
	}
	
	if ((pgunionTaskTBL->pclassTBL = (PUnionTaskClass)((unsigned char *)pgunionTaskTBL + sizeof(*pgunionTaskTBL))) == NULL)
	{
		UnionUserErrLog("in UnionConnectTaskTBL:: PUnionTaskClass!\n");
		return(-1);
	}
	pgunionTaskClass = pgunionTaskTBL->pclassTBL;
	if ((pgunionTaskTBL->pinstanceTBL = (PUnionTaskInstance)((unsigned char *)pgunionTaskTBL 
								+ sizeof(*pgunionTaskTBL)
								+ sizeof(TUnionTaskClass) * maxClassNum)) == NULL)
	{
		UnionUserErrLog("in UnionConnectTaskTBL:: PUnionTaskInstance!\n");
		return(-1);
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
	pgunionTaskTBL = NULL;
	pgunionTaskClass = NULL;
	pgunionTaskInstance = NULL;
	pgunionTaskMDL = NULL;
	return(UnionRemoveSharedMemoryModule(conMDLNameOfUnionTaskTBL));
}

int UnionReloadTaskTBL()
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		i;
	PUnionTaskClass	ptaskClass;
	int		firstNullPos;
	
	if ((pgunionTaskTBL == NULL) || (pgunionTaskClass == NULL) || (pgunionTaskInstance == NULL))
	{
		UnionUserErrLog("in UnionReloadTaskTBL:: pgunionTaskTBL is NULL!\n");
		return(-1);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfTaskTBL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionReloadTaskTBL:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("maxClassNum")) == NULL)
	{
		UnionUserErrLog("in UnionReloadTaskTBL:: UnionGetEnviVarByName for [%s]\n!","maxClassNum");
		UnionClearEnvi();
		return(-1);
	}
	pgunionTaskTBL->maxClassNum = atoi(p);
	if ((p = UnionGetEnviVarByName("maxInstanceNum")) == NULL)
	{
		UnionUserErrLog("in UnionReloadTaskTBL:: UnionGetEnviVarByName for [%s]\n!","maxInstanceNum");
		UnionClearEnvi();
		return(-1);
	}
	pgunionTaskTBL->maxInstanceNum = atoi(p);
	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		if (strlen((pgunionTaskClass+i)->name) == 0)
			continue;
		if ((strcmp((pgunionTaskClass+i)->name,"maxClassNum") == 0) || 
			(strcmp((pgunionTaskClass+i)->name,"maxInstanceNum") == 0))
		{
			memset(pgunionTaskClass+i,0,sizeof(*ptaskClass));
			continue;
		}
		if ((p = UnionGetEnviVarByName((pgunionTaskClass+i)->name)) == NULL)
		{
			if ((pgunionTaskClass+i)->currentNum > 0)
				continue;
			memset(pgunionTaskClass+i,0,sizeof(*ptaskClass));
		}
	}
	
	for (i = 0,firstNullPos = 0; (i < UnionGetEnviVarNum()) && (firstNullPos < pgunionTaskTBL->maxClassNum); i++)
	{
		// 读取名称
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
		{
			UnionUserErrLog("in UnionReloadTaskTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,0);
			continue;
		}
		if (strlen(p) > sizeof(ptaskClass->name) - 1)
			continue;
		if ((strcmp(p,"maxClassNum") == 0) || (strcmp(p,"maxInstanceNum") == 0))
			continue;
		if ((ptaskClass = UnionFindTaskClass(p)) == NULL)
		{
			for (;firstNullPos < pgunionTaskTBL->maxClassNum;firstNullPos++)
			{
				ptaskClass = pgunionTaskClass + firstNullPos;
				if (strlen(ptaskClass->name) == 0)
					break;
				else
					continue;
			}
			if (firstNullPos >=  pgunionTaskTBL->maxClassNum)
			{
				UnionUserErrLog("in UnionReloadTaskTBL:: taskTBL is full! maxClassNum = [%d]\n",pgunionTaskTBL->maxClassNum);
				break;
			}
			ptaskClass->currentNum = 0;
		}
		strcpy(ptaskClass->name,p);
		// 读取startCmd
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
		{
			UnionUserErrLog("in UnionReloadTaskTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
			continue;
		}
		if (strlen(p) > sizeof(ptaskClass->startCmd) - 1)
			continue;
		strcpy(ptaskClass->startCmd,p);
			
		// 读取minNum
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,2)) == NULL)
		{
			UnionUserErrLog("in UnionReloadTaskTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,2);
			continue;
		}
		if ((ptaskClass->minNum = atoi(p)) < 0)
			ptaskClass->minNum = 1;
		
		// 读取logFileName
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,3)) == NULL)
		{
			UnionUserErrLog("in UnionReloadTaskTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,3);
			continue;
		}
		if (strlen(p) > sizeof(ptaskClass->logFileName) - 1)
			continue;
		strcpy(ptaskClass->logFileName,p);
		/*UnionNullLog("[%s][%s][%d][%d][%s]\n",
			ptaskClass->name,
			ptaskClass->startCmd,
			ptaskClass->minNum,
			ptaskClass->currentNum,
			ptaskClass->logFileName);
		*/
		firstNullPos++;
	}
	
	UnionClearEnvi();
		
	return(0);
}

int UnionPrintTaskClassInTBL()
{
	int	i;
	int	num = 0;
	
	if (pgunionTaskTBL == NULL)
		return(-1);
	printf("(maxNum,currentNum,name,logFile,startCommand)\n");
	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		//UnionNullLog("%d %0x\n",i,pgunionTaskClass+i);
		if (UnionPrintTaskClassToFile(pgunionTaskClass+i,stdout) < 0)
			continue;
		else
		{
			if ((num != 0) && (num % 23 == 0))
				if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
					break;
			num++;
		}
	}
	printf("Total Class Num = [%d]\n",num);
	return(num);
}

int UnionPrintTaskInstanceInTBL()
{
	int	i;
	int	num = 0;
	
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		//UnionNullLog("%d %0x\n",i,pgunionTaskInstance+i);
		if (UnionPrintTaskInstanceToFile(pgunionTaskInstance+i,stdout) < 0)
			continue;
		else
		{
			if ((num != 0) && (num % 23 == 0))
				if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
					break;
			num++;
		}
	}
	printf("Total Instance Num = [%d]\n",num);
	return(num);
}

int UnionPrintStatusOfTaskTBL()
{
	if (pgunionTaskTBL == NULL)
		return(-1);
	printf("\n");
	printf("maxClassNum	[%d]\n",pgunionTaskTBL->maxClassNum);
	printf("maxInstanceNum	[%d]\n",pgunionTaskTBL->maxInstanceNum);
	printf("\n");
	return(UnionPrintSharedMemoryModuleToFile(pgunionTaskMDL,stdout));
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
		return(-1);
	if (strlen(ptaskClass->name) == 0)
		return(-1);
	fprintf(fp,"(%03d,",ptaskClass->minNum);
	fprintf(fp,"%03d,",ptaskClass->currentNum);
	fprintf(fp,"%s,   ",ptaskClass->name);
	fprintf(fp,"%s,   ",ptaskClass->logFileName);
	fprintf(fp,"%s)\n",ptaskClass->startCmd);
	return(0);
}

int UnionPrintTaskInstanceToFile(PUnionTaskInstance ptaskInstance,FILE *fp)
{
	if ((ptaskInstance == NULL) || (fp == NULL))
		return(-1);
	if (ptaskInstance->procID <= 0)
		return(-1);
	fprintf(fp,"(%05d,",ptaskInstance->procID);
	fprintf(fp,"%15s,",ptaskInstance->startTime);
	fprintf(fp,"%12p,",ptaskInstance->plogFile);
	if (ptaskInstance->pclass)
		fprintf(fp,"%40s)\n",ptaskInstance->pclass->name);
	else
		fprintf(fp,"%40s)\n","null");
	return(0);
}

PUnionTaskInstance UnionFindTaskInstance(int procID)
{
	int	i;
	
	if ((pgunionTaskTBL == NULL) || (pgunionTaskInstance == NULL))
		return(NULL);
	
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		if ((pgunionTaskInstance+i)->procID == procID)
			return(pgunionTaskInstance+i);
	}
	
	return(NULL);
}
	
PUnionTaskClass UnionFindTaskClass(char *taskName)
{
	int	i;
	
	if ((pgunionTaskTBL == NULL) || (pgunionTaskClass == NULL) || (taskName == NULL))
		return(NULL);
	
	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		if (strcmp((pgunionTaskClass+i)->name,taskName) == 0)
			return(pgunionTaskClass+i);
	}
	
	return(NULL);
}
	
PUnionTaskInstance UnionCreateTaskInstance(int (*UnionTaskActionBeforeExit)(),char *fmt,...)
{
	va_list 		args;
	char			tmpBuf[128];
	PUnionTaskInstance	pinstance;
	PUnionTaskClass		pclass;
	int			i;
	int			ret;
	
	if (pgunionTaskMDL == NULL)
	{
		if ((ret = UnionConnectTaskTBL()) < 0)
		{
			UnionUserErrLog("in UnionCreateTaskInstance:: UnionConnectTaskTBL!\n");
			return(NULL);
		}
	}
	
	memset(tmpBuf,0,sizeof(tmpBuf));
        va_start(args,fmt);
        vsprintf(tmpBuf,fmt,args);
        va_end(args);
	
	if ((pclass = UnionFindTaskClass(tmpBuf)) == NULL)
	{
		UnionUserErrLog("in UnionCreateTaskInstance:: taskClass [%s] not defined!\n",tmpBuf);
		return(NULL);
	}
	if ((pinstance = UnionFindTaskInstance(getpid())) != NULL)	// 已经创建了一个进程号相同的任务
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
		return(pgthisInstance=pinstance);
	}
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		if ((pgunionTaskInstance+i)->procID > 0)
			continue;
		(pgunionTaskInstance+i)->procID = getpid();
		(pgunionTaskInstance+i)->pclass = pclass;
		++pclass->currentNum;
		memset((pgunionTaskInstance+i)->startTime,0,sizeof((pgunionTaskInstance+i)->startTime));
		UnionGetFullSystemDateTime((pgunionTaskInstance+i)->startTime);
		(pgunionTaskInstance+i)->plogFile = UnionConnectLogFile(pclass->logFileName);
		
		UnionSetProductVersion((pgunionTaskInstance+i)->pclass->name,UnionReadPredfinedProductVersionNumber());

		// 设置成后台任务
		UnionIgnoreSignals();
		signal(SIGUSR1,(void *)UnionTaskActionBeforeExit);

		return(pgthisInstance = pgunionTaskInstance+i);
	}
	UnionUserErrLog("in UnionCreateTaskInstance:: taskInstanceTBL is full!\n");
	return(NULL);
}	
	
int UnionUnregisterTaskInstance(PUnionTaskInstance pinstance)
{
	if (pinstance == NULL)
		return(-1);
	if (pinstance->pclass)
	{
		if (pinstance->pclass->currentNum > 0)
			--pinstance->pclass->currentNum;
		UnionDisconnectLogFile(pinstance->plogFile);
		pinstance->plogFile = NULL;
	}
	pinstance->pclass = NULL;
	pinstance->procID = -1;
	memset(pinstance->startTime,0,sizeof(pinstance->startTime));
	//memset(pinstance,0,sizeof(*pinstance));
	return(0);
}	

int UnionUnregisterThisTaskInstance()
{
	return(UnionUnregisterTaskInstance(UnionFindTaskInstance(getpid())));
}	

int UnionKillAllTaskInstance()
{
	int	i;
	int	num = 0;
	
	if ((pgunionTaskTBL == NULL) || (pgunionTaskInstance == NULL))
	{
		UnionUserErrLog("in UnionKillAllTaskInstance:: null pointer!\n");
		return(-1);
	}
	
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		if ((pgunionTaskInstance+i)->procID > 0)
		{
			if ((pgunionTaskInstance+i)->procID == getpid())	// 自身不杀
				continue;
			if (kill((pgunionTaskInstance+i)->procID,SIGUSR1) != 0)
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
	int	i;
	int	num = 0;
	int	myPid;
	
	if ((pgunionTaskTBL == NULL) || (pgunionTaskInstance == NULL))
		return(-1);
	
	myPid = getpid();
	
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		if ((pgunionTaskInstance+i)->procID <= 0)
			continue;
		if ((pgunionTaskInstance+i)->procID == myPid)
			continue;
		if ((pgunionTaskInstance+i)->pclass == NULL)
			continue;
		if (strcmp((pgunionTaskInstance+i)->pclass->name,taskName) != 0)
			continue;
		if (kill((pgunionTaskInstance+i)->procID,SIGUSR1) != 0)
		{
			UnionSystemErrLog("in UnionKillTaskInstanceByName:: send SIGUSR1 to Process [%d] Failure!\n",(pgunionTaskInstance+i)->procID);
			continue;
		}
		num++;
	}
	return(num);
}

int UnionKillTaskInstanceByProcID(int taskProcID)
{
	int	i;
	
	if ((pgunionTaskTBL == NULL) || (pgunionTaskInstance == NULL))
		return(-1);
	
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		if ((pgunionTaskInstance+i)->procID != taskProcID)
			continue;
		if (kill((pgunionTaskInstance+i)->procID,SIGUSR1) != 0)
		{
			UnionSystemErrLog("in UnionKillTaskInstanceByProcID:: send SIGUSR1 to Process [%d] Failure!\n",(pgunionTaskInstance+i)->procID);
			continue;
		}
		else
			return(0);
	}
	return(-1);
}

int UnionClearRubbishTaskInstance(int seconds)
{
	int	i;
	int	num;
	
	if (pgunionTaskTBL == NULL)
	{
		UnionUserErrLog("in UnionClearRubbishTaskInstance:: pgunionTaskTBL is NULL!\n");
		return(-1);
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
					UnionUnregisterTaskInstance(pgunionTaskInstance+i);
					num++;
				}
			}
		}
		sleep(seconds);
	}
	
	return(num);
}

int UnionStartTaskGuard(int seconds)
{
	int	i;
	int	j;
	
	if (pgunionTaskTBL == NULL)
	{
		UnionUserErrLog("in UnionStartTaskGuard:: pgunionTaskTBL is NULL!\n");
		return(-1);
	}
	
	for (;;)
	{
		for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
		{
			if (strlen((pgunionTaskClass+i)->name) == 0)
				continue;
			for (j = (pgunionTaskClass+i)->currentNum; j < (pgunionTaskClass+i)->minNum; j++)
			{
				system((pgunionTaskClass+i)->startCmd);
			}
		}
		sleep(seconds);
	}
}
	
