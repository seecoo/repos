
#define _UnionTask_3_x_
#define _UnionEnv_3_x_		// 使用3.x版本的UnionEnv模块
#define _UnionLogMDL_3_x_

#define _UnionTask_3_2_

//#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <signal.h>
#endif
#include <stdarg.h>
#include <stdlib.h>

#ifndef _WIN32
#include <sys/ipc.h>
#include <sys/shm.h>
#else
#include "unionSimuUnixSharedMemory.h"
#include "simuUnixSystemCall.h"
#endif


#include "UnionProc.h"

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


#include "unionREC.h"
#include "UnionLog.h"
#include "unionErrCode.h"
#include "UnionStr.h"

#include "unionUnionTaskTBL.h"

extern PUnionSharedMemoryModule	pgunionTaskMDL;
extern PUnionTaskTBL			pgunionTaskTBL;
extern PUnionTaskClass			pgunionTaskClass;
extern PUnionTaskInstance		pgunionTaskInstance;
extern PUnionTaskInstance		pgthisInstance;

int UnionGetNameOfTaskTBL(char *fileName)
{
	sprintf(fileName,"unionTaskTBL");
	return(0);
}

int UnionGetMaxNumOfTaskClass()
{
	int	num = 0;
	if ((num=UnionReadIntTypeRECVar("maxClassNum")) < 0)
	{
		UnionUserErrLog("in UnionGetMaxNumOfTaskClass:: UnionReadIntTypeRECVar for [%s]\n!","maxClassNum");
		return(errCodeTaskMDL_ConfFile);
	}

	return num;
}

int UnionGetMaxNumOfTaskInstance()
{
	int	num = 0;
	if ((num=UnionReadIntTypeRECVar("maxInstanceNum")) < 0)
	{
		UnionUserErrLog("in UnionGetMaxNumOfTaskInstance:: UnionReadIntTypeRECVar for [%s]\n!","maxInstanceNum");
		return(errCodeTaskMDL_ConfFile);
	}

	return num;
}

int UnionReloadTaskTBL()
{
	int		ret = 0;
	int		i = 0;
	PUnionTaskClass	ptaskClass;
	int		firstNullPos = 0;
	char		tmpBuf[128+1];
	TUnionUnionTaskTBL tTaskTBLObj;
	TUnionUnionTaskTBL tTaskTBL[1024];
	int iCnt = 0;

	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionReloadTaskTBL:: UnionConnectTaskTBL!\n");
		return(ret);
	}

	if( (pgunionTaskTBL->maxClassNum = UnionGetMaxNumOfTaskClass()) < 0 )
	{
		UnionUserErrLog("in UnionReloadTaskTBL:; UnionGetMaxNumOfTaskClass\n");
		return (errCodeTaskMDL_ConfFile);
	}

	if( (pgunionTaskTBL->maxInstanceNum = UnionGetMaxNumOfTaskInstance()) < 0 )
	{
		UnionUserErrLog("in UnionReloadTaskTBL:; UnionGetMaxNumOfTaskInstance\n");
		return (errCodeTaskMDL_ConfFile);
	}

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
		memset(&tTaskTBLObj, 0, sizeof(TUnionUnionTaskTBL));
		if ((ret = UnionReadUnionTaskTBLRec((pgunionTaskClass+i)->name, &tTaskTBLObj)) < 0)
		{
			if ((pgunionTaskClass+i)->currentNum > 0)
				continue;
			memset(pgunionTaskClass+i,0,sizeof(*ptaskClass));
		}
	}

	if( pgunionTaskTBL->maxClassNum > 1024 )
	{
		UnionUserErrLog("in UnionReloadTaskTBL:: MaxNumOfTaskClass is too large\n");
		return (errCodeTaskMDL_ConfFile);
	}

	memset(tTaskTBL, 0, 1024 * sizeof(TUnionUnionTaskTBL));
	if( (iCnt = UnionBatchReadUnionTaskTBLRec("", tTaskTBL, 1024)) < 0 )
	{
		UnionUserErrLog("in UnionReloadTaskTBL:: UnionBatchReadUnionTaskTBLRec");
		return (iCnt);
	}

	for (i = 0,firstNullPos = 0; (i < iCnt) && (firstNullPos < pgunionTaskTBL->maxClassNum); i++)
	{
		// 读取名称
		memset(tmpBuf, 0, sizeof(tmpBuf));
		if( (ret = UnionFilterRubbisBlank(tTaskTBL[i].name, strlen(tTaskTBL[i].name), tmpBuf, sizeof(tmpBuf))) <= 0 )
		{
			UnionUserErrLog("in UnionReloadTaskTBL:: UnionFilterRubbisBlank [%s]\n", tTaskTBL[i].name);
			continue;
		}
		if( (ptaskClass = UnionFindTaskClass(tmpBuf)) == NULL )
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
		if( strlen(tmpBuf) >= sizeof(ptaskClass->name) )
		{
			UnionUserErrLog("in UnionReloadTaskTBL:: taskName [%s] too long!\n",tmpBuf);
			continue;
		}
		strcpy(ptaskClass->name, tmpBuf);
		// 读取startCmd
		if (strlen(tTaskTBL[i].startCmd) > sizeof(ptaskClass->startCmd) - 1)
			continue;
		strcpy(ptaskClass->startCmd, tTaskTBL[i].startCmd);
			
		// 读取minNum
		if ((ptaskClass->minNum = tTaskTBL[i].num) < 0)
			ptaskClass->minNum = 1;
		
		// 读取logFileName

		if (strlen(tTaskTBL[i].logFileName) > sizeof(ptaskClass->logFileName) - 1)
			continue;
		strcpy(ptaskClass->logFileName, tTaskTBL[i].logFileName);
		/*UnionNullLog("[%s][%s][%d][%d][%s]\n",
			ptaskClass->name,
			ptaskClass->startCmd,
			ptaskClass->minNum,
			ptaskClass->currentNum,
			ptaskClass->logFileName);
		*/
		firstNullPos++;
	}

	return(0);
}

int UnionReloadTaskClass(char *taskName)
{
	int	ret = 0;
	TUnionTaskClass		taskClass;
	int	index = 0;
	PUnionTaskClass		ptaskClass;
	TUnionUnionTaskTBL tTaskTBL;

	if (taskName == NULL)
		return(UnionReloadTaskTBL());
	if (strlen(taskName) >= sizeof(taskClass.name))
	{
		UnionUserErrLog("in UnionReloadTaskClass:: taskName [%s] too long!\n",taskName);
		return(errCodeParameter);
	}

	// 从数据库中读取任务定义
	memset(&tTaskTBL, 0, sizeof(tTaskTBL));
	if( (ret = UnionReadUnionTaskTBLRec(taskName, &tTaskTBL)) < 0 )
	{
		UnionUserErrLog("in UnionReloadTaskClass:: UnionReadUnionTaskTBLRec taskName[%s]\n", taskName);
		return (ret);
	}

	// 拼装任务
	memset(&taskClass, 0, sizeof(taskClass));
	strcpy(taskClass.name, taskName);
	if( strlen(tTaskTBL.startCmd) < sizeof(taskClass.startCmd) )
		strcpy(taskClass.startCmd, tTaskTBL.startCmd);
	if( strlen(taskClass.startCmd) != 0 )
		taskClass.minNum = tTaskTBL.num;
	if( strlen(tTaskTBL.logFileName) < sizeof(taskClass.logFileName) )
		strcpy(taskClass.logFileName, tTaskTBL.logFileName);
	else
		memcpy(taskClass.logFileName, tTaskTBL.logFileName, sizeof(taskClass.logFileName)-1);

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
