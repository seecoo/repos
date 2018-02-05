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

// 2011/05/10 黄宝鑫修改 kill((pgunionTaskInstance+i)->procID,SIGUSR2)) 改为 kill(otherPid,SIGUSR2) 代替，避免出现kill(-1,SIGUSR2) 的情况
*/

#define _UnionTask_3_2_

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
#include "unionErrCode.h"
#include "UnionStr.h"
#include "UnionLog.h"

PUnionSharedMemoryModule	pgunionTaskMDL = NULL;
PUnionTaskTBL			pgunionTaskTBL = NULL;
PUnionTaskClass			pgunionTaskClass = NULL;
PUnionTaskInstance		pgunionTaskInstance = NULL;
PUnionTaskInstance		pgthisInstance = NULL;

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
		return(errCodeTaskMDL_ConfFile);
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
		return(errCodeTaskMDL_ConfFile);
	}
	maxInstanceNum = atoi(p);
	UnionClearEnvi();
	return(maxInstanceNum);
}

int UnionConnectTaskTBL()
{
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

int UnionReloadTaskTBL()
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		i;
	PUnionTaskClass	ptaskClass;
	int		firstNullPos;
	char		tmpBuf[128+1];
		
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionReloadTaskTBL:: UnionConnectTaskTBL!\n");
		return(ret);
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
		return(errCodeTaskMDL_ConfFile);
	}
	pgunionTaskTBL->maxClassNum = atoi(p);
	if ((p = UnionGetEnviVarByName("maxInstanceNum")) == NULL)
	{
		UnionUserErrLog("in UnionReloadTaskTBL:: UnionGetEnviVarByName for [%s]\n!","maxInstanceNum");
		UnionClearEnvi();
		return(errCodeTaskMDL_ConfFile);
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
		if ((strcmp(p,"maxClassNum") == 0) || (strcmp(p,"maxInstanceNum") == 0))
			continue;
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionFilterRubbisBlank(p,strlen(p),tmpBuf,sizeof(tmpBuf))) <= 0)
		{
			UnionUserErrLog("in UnionReloadTaskTBL:: UnionFilterRubbisBlank [%s]\n",p);
			continue;
		}
		if ((ptaskClass = UnionFindTaskClass(tmpBuf)) == NULL)
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
		if (strlen(tmpBuf) >= sizeof(ptaskClass->name))
		{
			UnionUserErrLog("in UnionReloadTaskTBL:: taskName [%s] too long!\n",tmpBuf);
			continue;
		}
		strcpy(ptaskClass->name,tmpBuf);
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
	fprintf(fp,"(maxNum,currentNum,name,logFile,startCommand)\n");
	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		//UnionNullLog("%d %0x\n",i,pgunionTaskClass+i);
		if (UnionPrintTaskClassToFile(pgunionTaskClass+i,fp) < 0)
			continue;
		else
		{
			if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
				if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
					break;
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
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		//UnionNullLog("%d %0x\n",i,pgunionTaskInstance+i);
		if (UnionPrintTaskInstanceToFile(pgunionTaskInstance+i,fp) < 0)
			continue;
		else
		{
			if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
				if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
					break;
			num++;
		}
	}
	fprintf(fp,"Total Instance Num = [%d]\n",num);
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
	fprintf(fp,"%03d,",ptaskClass->minNum);
	fprintf(fp,"%03d,",ptaskClass->currentNum);
	fprintf(fp,"%25s,",ptaskClass->name);
	fprintf(fp,"%15s,",ptaskClass->logFileName);
	fprintf(fp,"%25s\n",ptaskClass->startCmd);
	return(0);
}

int UnionPrintTaskInstanceToFile(PUnionTaskInstance ptaskInstance,FILE *fp)
{
	if ((ptaskInstance == NULL) || (fp == NULL))
		return(errCodeTaskMDL_NotConnected);
	if (ptaskInstance->procID <= 0)
		return(-1);
	fprintf(fp,"(%05d,",ptaskInstance->procID);
	fprintf(fp,"%15s,",ptaskInstance->startTime);
	fprintf(fp,"%12p,",ptaskInstance->plogFile);
	// Deleted by Wolfgang Wang, 2004/6/18
	/*
	if (ptaskInstance->pclass)
		fprintf(fp,"%40s)\n",ptaskInstance->pclass->name);
	else
		fprintf(fp,"%40s)\n","null");
	*/
	// Added by Wolfgang Wang, 2004/6/18
	fprintf(fp,"%40s)\n",ptaskInstance->name);
	// End of addition of 2004/6/18
	
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

PUnionTaskInstance UnionFindTaskInstance(int procID)
{
	int	i;
	int	ret;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionFindTaskInstance:: UnionConnectTaskTBL!\n");
		return(NULL);
	}
	
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
		if ((pgunionTaskInstance+i)->procID > 0)
		{
			continue;
		}
		(pgunionTaskInstance+i)->procID = getpid();
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
	pinstance->procID = -1;

	memset(pinstance->startTime,0,sizeof(pinstance->startTime));
	//memset(pinstance,0,sizeof(*pinstance));
	return(0);
}	

// This function is rewritten by Wolfgang Wang, 2004/06/30	
int UnionUnregisterTaskInstance(PUnionTaskInstance pinstance)
{
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
			// mod by huangbx, 2011-05-10, 防止procID变为-1的情况
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
		// mod by huangbx, 2011-05-10, 防止procID变为-1
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
	int	otherPid;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionKillTaskInstanceByProcID:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionTaskTBL->maxInstanceNum; i++)
	{
		otherPid = (pgunionTaskInstance+i)->procID;
		if (otherPid != taskProcID)
			continue;
					
		/*
		if ((pgunionTaskInstance+i)->procID != taskProcID)
			continue;
		*/
#ifdef _useSigUser2_
		//if (kill((pgunionTaskInstance+i)->procID,SIGUSR2) != 0)
		if (kill(otherPid,SIGUSR2) != 0)
#else
		//if (kill((pgunionTaskInstance+i)->procID,SIGUSR1) != 0)
		if (kill(otherPid,SIGUSR1) != 0)
#endif
		{
			UnionSystemErrLog("in UnionKillTaskInstanceByProcID:: send SIGUSR1 to Process [%d] Failure!\n",otherPid);
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
#ifndef _RepairAllInst_    //add by hzh in 2012.10.30,当定义了_RepairAllInst_则维护所有任务
		if ((pgunionTaskClass+i)->minNum <= 0)	// 只维护必须在线的任务
			continue;
#endif
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
	
	myPid = getpid();
	
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
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionExistsAnotherTaskOfName:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	myPid = getpid();
	
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
	int	otherPid;
	
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
	int	otherPid;
	
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionCloseTaskInstanceByName:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	myPid = getpid();

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
			UnionSystemErrLog("in UnionCloseTaskInstanceByName:: send SIGUSR1 to Process [%d] Failure!\n",otherPid);
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
	int	pid;
	char	tmpBuf[128];
	va_list 		args;
		
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionRenameTaskNameOfMyself:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	pid = getpid();
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

// 增加一个新的任务类型定义
int UnionAddNewTaskClass(char *taskName,char *command,int num,char *logFileName)
{
	int		i;
	int		ret;
	PUnionTaskClass	ptaskClass;
	char		fileName[512];
	//char		dateTime[100];
		
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionAddNewTaskClass:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	if (UnionFindTaskClass(taskName) != NULL)
	{
		UnionUserErrLog("in UnionAddNewTaskClass:: task class [%s] already exists!\n",taskName);
		return(errCodeTaskMDL_TaskClassAlreadyExists);
	}
	
	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		if (strlen((pgunionTaskClass+i)->name) == 0)	// 在类型表中查找到了一个空位置
		{
			// 插入任务类型
			ptaskClass = pgunionTaskClass+i;
			if (taskName == NULL)
			{
				UnionUserErrLog("in UnionAddNewTaskClass:: null task class name!\n");
				return(errCodeTaskMDL_TaskClassNameIsNull);
			}
			if (strlen(taskName) >= sizeof(ptaskClass->name))
			{
				UnionUserErrLog("in UnionAddNewTaskClass:: task class name [%s] too long!\n",taskName);
				return(errCodeTaskMDL_TaskClassNameTooLong);
			}
			strcpy(ptaskClass->name,taskName);
			if ((command == NULL) || (strlen(command) == 0))
				strcpy(ptaskClass->startCmd,"null");
			else
			{
				if (strlen(command) >= sizeof(ptaskClass->startCmd))
				{
					UnionUserErrLog("in UnionAddNewTaskClass:: task class start command [%s] too long!\n",command);
					return(errCodeTaskMDL_TaskClassStartCmdTooLong);
				}
				strcpy(ptaskClass->startCmd,command);
			}
			ptaskClass->minNum = num;
			ptaskClass->currentNum = 0;
			if ((logFileName == NULL) || (strlen(logFileName) == 0))
			{
				UnionUserErrLog("in UnionAddNewTaskClass:: null task class logfile name!\n");
				return(errCodeTaskMDL_TaskClassLogFileNameIsNull);
			}
			if (strlen(logFileName) >= sizeof(ptaskClass->logFileName))
			{
				UnionUserErrLog("in UnionAddNewTaskClass:: task log file name [%s] too long!\n",logFileName);
				return(errCodeTaskMDL_TaskClassLogFileNameTooLong);
			}
			strcpy(ptaskClass->logFileName,logFileName);
			
			// 在任务定义文件中增加记录
			memset(fileName,0,sizeof(fileName));
			UnionGetNameOfTaskTBL(fileName);
			if ((command == NULL) || (strlen(command) == 0))
				ret = UnionInsertEnviVar(fileName,taskName,"[null]	[%d]	[%s]",num,logFileName);
			else
				ret = UnionInsertEnviVar(fileName,taskName,"[%s]	[%d]	[%s]",command,num,logFileName);
			if (ret < 0)
			{
				UnionUserErrLog("in UnionAddNewTaskClass:: UnionInsertEnviVar [%s]\n",taskName);
				return(ret);
			}
			return(0);
			/*
			if ((fp = fopen(fileName,"a")) == NULL)
			{
				UnionSystemErrLog("in UnionAddNewTaskClass:: fopen [%s]\n",fileName);
				return(errCodeUseOSErrCode);
			}
			fprintf(fp,"\n");
			memset(dateTime,0,sizeof(dateTime));
			UnionGetFullSystemDateTime(dateTime);
			//fprintf(fp,"# %s auto added by ESSC\n",dateTime);
			fprintf(fp,"[%s]		[%s]		[%d]	[%s]\n",
				ptaskClass->name,ptaskClass->startCmd,ptaskClass->minNum,ptaskClass->logFileName);
			fflush(fp);
			fclose(fp);
			return(0);
			*/
		}
	}
	UnionUserErrLog("in UnionAddNewTaskClass:: task class table is full!\n");
	return(errCodeTaskMDL_TaskClassTableIsFull);
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
	UnionDebugNullLog("pgunionTaskTBL->maxInstanceNum = [%d]\n",pgunionTaskTBL->maxInstanceNum);
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
// 删除一个任务类型定义
int UnionDeleteTaskClass(char *taskName)
{
	int		ret;
	PUnionTaskClass	ptaskClass;
	char		fileName[512];
		
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionDeleteTaskClass:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	if ((ptaskClass = UnionFindTaskClass(taskName)) != NULL)
	{
		if (ptaskClass->currentNum > 0)	// 现在有任务使用此类
		{
			UnionUserErrLog("in UnionDeleteTaskClass:: taskClass [%s] is used now!\n",taskName);
			return(errCodeTaskMDL_TaskClassIsUsed);
		}
		memset(ptaskClass->name,0,sizeof(ptaskClass->name));	// 从内存中删除任务
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfTaskTBL(fileName);
	if ((ret = UnionDeleteEnviVar(fileName,taskName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteTaskClass:: UnionDeleteEnviVar!\n");
		return(ret);
	}
	return(ret);
}
// 更新一个任务类型定义
// 如果command,logFileName为空，不更新，如果num为负值，不更新
int UnionUpdateTaskClass(char *taskName,char *command,int num,char *logFileName)
{
	int		ret;
	PUnionTaskClass	ptaskClass;
	char		fileName[512];
	int		offset = 0;
	char		tmpBuf[512];
		
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionUpdateTaskClass:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	if ((ptaskClass = UnionFindTaskClass(taskName)) == NULL)
	{
		UnionUserErrLog("in UnionUpdateTaskClass:: UnionFindTaskClass [%s]!\n",taskName);
		return(ret);
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf+offset,"[",1);
	offset = 1;
	if ((command != NULL) || (strlen(command) != 0))	// 修改启动命令
	{
		if (strlen(command) >= sizeof(ptaskClass->startCmd))
		{
			memcpy(ptaskClass->startCmd,command,sizeof(ptaskClass->startCmd)-1);
			ptaskClass->startCmd[sizeof(ptaskClass->startCmd)-1] = 0;
			memcpy(tmpBuf+offset,command,sizeof(ptaskClass->startCmd)-1);
			tmpBuf[offset+sizeof(ptaskClass->startCmd)-1] = ']';
		}
		else
		{
			strcpy(ptaskClass->startCmd,command);
			sprintf(tmpBuf+offset,"%s]	",command);
		}
	}
	else	// 不修改启动命令
	{
		sprintf(tmpBuf+offset,"%s]	",ptaskClass->startCmd);
	}
	offset = strlen(tmpBuf);
	if (num < 0)	// 不改最小数量
		sprintf(tmpBuf+offset,"[%d]	",ptaskClass->minNum);
	else		// 改最小数量
	{
		sprintf(tmpBuf+offset,"[%d]	",num);
		ptaskClass->minNum = num;
	}
	offset = strlen(tmpBuf);
	memcpy(tmpBuf+offset,"[",1);
	offset += 1;
	if ((logFileName != NULL) || (strlen(logFileName) != 0))	// 修改日志文件
	{
		if (strlen(logFileName) >= sizeof(ptaskClass->logFileName))
		{
			memcpy(ptaskClass->logFileName,logFileName,sizeof(ptaskClass->logFileName)-1);
			ptaskClass->logFileName[sizeof(ptaskClass->logFileName)-1] = 0;
			memcpy(tmpBuf+offset,logFileName,sizeof(ptaskClass->logFileName)-1);
			tmpBuf[offset+sizeof(ptaskClass->logFileName)-1] = ']';
		}
		else
		{
			strcpy(ptaskClass->logFileName,logFileName);
			sprintf(tmpBuf+offset,"%s]",logFileName);
		}
	}
	else	// 不修改日志文件
		sprintf(tmpBuf+offset,"%s]",ptaskClass->logFileName);
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfTaskTBL(fileName);
	if ((ret = UnionUpdateEnviVar(fileName,taskName,tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionUpdateTaskClass:: UnionUpdateEnviVar [%s]!\n",taskName);
		return(ret);
	}
	return(ret);
}

// 2006/9/8增加
int UnionReloadTaskClass(char *taskName)
{
	char			taskDefLine[512+1];
	char			fileName[512+1];
	int			ret;
	PUnionEnviVariable	penviVar;
	TUnionTaskClass		taskClass;
	int			index;
	PUnionTaskClass		ptaskClass;
	
	if (taskName == NULL)
		return(UnionReloadTaskTBL());
	if (strlen(taskName) >= sizeof(taskClass.name))
	{
		UnionUserErrLog("in UnionReloadTaskClass:: taskName [%s] too long!\n",taskName);
		return(errCodeParameter);
	}
	// 从文件中读取任务定义
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfTaskTBL(fileName);
	memset(taskDefLine,0,sizeof(taskDefLine));
	if ((ret = UnionReadEnviVarDefLineInFile(fileName,taskName,taskDefLine)) < 0)
	{
		UnionUserErrLog("in UnionReloadTaskClass:: UnionReadEnviVarDefLineInFile [%s]!\n",taskName);
		return(ret);
	}
	if ((penviVar = ReadUnionEnviVarFromStr(taskDefLine)) == NULL)
	{
		UnionUserErrLog("in UnionReloadTaskClass:: ReadUnionEnviVarFromStr [%s]!\n",taskDefLine);
		return(errCodeTaskMDL_TaskClassDefLineError);
	}
	for (index = 0; index < 4; index++)
	{
		if (penviVar->pVariableValue[index] == NULL)
		{
			UnionUserErrLog("in UnionReloadTaskClass:: ReadUnionEnviVarFromStr [%s]!\n",taskDefLine);
			UnionFreeEnviVar(penviVar);
			return(errCodeTaskMDL_TaskClassDefLineError);
		}
	}
	// 拼装任务
	memset(&taskClass,0,sizeof(taskClass));
	strcpy(taskClass.name,taskName);
	if (strlen(penviVar->pVariableValue[1]) < sizeof(taskClass.startCmd))
		strcpy(taskClass.startCmd,penviVar->pVariableValue[1]);
	if (strlen(taskClass.startCmd) != 0)
		taskClass.minNum = atoi(penviVar->pVariableValue[2]);
	if (strlen(penviVar->pVariableValue[3]) < sizeof(taskClass.logFileName))
		strcpy(taskClass.logFileName,penviVar->pVariableValue[3]);
	else
		memcpy(taskClass.logFileName,penviVar->pVariableValue[3],sizeof(taskClass.logFileName)-1);
	UnionFreeEnviVar(penviVar);
	// 在内存中查找任务定义是否存在
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionReloadTaskClass:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	if ((ptaskClass	= UnionFindTaskClass(taskName)) == NULL)	// 不存在
	{
		for (index = 0; index < pgunionTaskTBL->maxClassNum; index++)
		{
			if (strlen((pgunionTaskClass+index)->name) == 0)	// 在类型表中查找到了一个空位置
			{
				memcpy(pgunionTaskClass+index,&taskClass,sizeof(taskClass));
				return(0);
			}
		}
		UnionUserErrLog("in UnionReloadTaskClass:: table is full!\n");
		return(errCodeTaskMDL_TaskClassTableIsFull);
	}
	else
	{
		strcpy(ptaskClass->startCmd,taskClass.startCmd);
		ptaskClass->minNum = taskClass.minNum;
		strcpy(ptaskClass->logFileName,taskClass.logFileName);
		return(0);
	}
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
		if ((pgunionTaskInstance+i)->procID > 0)
			continue;
		(pgunionTaskInstance+i)->procID = getpid();
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
