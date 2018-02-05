
#define _UnionTask_3_x_
#define _UnionEnv_3_x_		// 使用3.x版本的UnionEnv模块
#define _UnionLogMDL_3_x_

#define _UnionTask_3_2_
#define _realBaseDB_2_x_

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

#include "UnionTask.h"
#include "UnionEnv.h"
#include "unionModule.h"

#include "unionVersion.h"
#ifndef _WIN32
#include "unionCommand.h"
#endif

#include "unionErrCode.h"
#include "UnionStr.h"

#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"
#include "UnionLog.h"
#include "unionREC.h"

extern PUnionSharedMemoryModule	pgunionTaskMDL;
extern PUnionTaskTBL			pgunionTaskTBL;
extern PUnionTaskClass			pgunionTaskClass;
extern PUnionTaskInstance		pgunionTaskInstance;
extern PUnionTaskInstance		pgthisInstance;

int UnionGetNameOfTaskTBL(char *fileName)
{
	sprintf(fileName,"unionTask");
	return(0);
}

int UnionGetMaxNumOfTaskClass()
{
	int	num = 0;
	if ((num = UnionReadIntTypeRECVar("maxClassNum")) < 0)
	{
		UnionUserErrLog("in UnionGetMaxNumOfTaskClass:: UnionReadIntTypeRECVar for [%s]!\n","maxClassNum");
		return(errCodeTaskMDL_ConfFile);
	}

	return num;
}

int UnionGetMaxNumOfTaskInstance()
{
	int	num = 0;
	if ((num = UnionReadIntTypeRECVar("maxInstanceNum")) < 0)
	{
		UnionUserErrLog("in UnionGetMaxNumOfTaskInstance:: UnionReadIntTypeRECVar for [%s]\n!","maxInstanceNum");
		return(errCodeTaskMDL_ConfFile);
	}

	return num;
}

int UnionReloadTaskTBL()
{
	int		j;
	int		foundFlag = 0;
	int		ret = 0;
	int		i = 0;
	int		firstNullPos = 0;
	char		tmpBuf[128+1];
	char		sql[128+1];
	int		iCnt = 0;
	char		taskName[50+1];
	char		startCmd[100+1];
	char		taskNum[16+1];
	char		logFileName[100+1];
	PUnionTaskClass	ptaskClass;

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

	memset(sql,0,sizeof(sql));
	sprintf(sql,"select taskName,startCmd,taskNum,logFileName from unionTask order by taskName");

	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionReloadTaskTBL:: UnionSelectRealDBRecord\n");
		return (ret);
	}
	
	// 读取总数量
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReloadTaskTBL:: UnionReadXMLPackageValue[totalNum]\n");
		return (ret);
	}
	iCnt = atoi(tmpBuf);

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
		
		for (j = 0; j < iCnt; j++)
		{
			if ((ret = UnionLocateXMLPackage("detail", j+1)) < 0)
			{
				UnionUserErrLog("in UnionReloadTaskTBL:: UnionLocate\n");
				continue;
			}
			memset(taskName,0,sizeof(taskName));
			UnionReadXMLPackageValue("taskName", taskName, sizeof(taskName));
			if (strcmp((pgunionTaskClass+i)->name,taskName) == 0)
			{
				foundFlag = 1;
				break;
			}
		}
		
		if (!foundFlag)
		{
			if ((pgunionTaskClass+i)->currentNum <= 0)
				memset(pgunionTaskClass+i,0,sizeof(*ptaskClass));
		}
	}

	if( pgunionTaskTBL->maxClassNum > 1024 )
	{
		UnionUserErrLog("in UnionReloadTaskTBL:: MaxNumOfTaskClass is too large\n");
		return (errCodeTaskMDL_ConfFile);
	}

	for (i = 0,firstNullPos = 0; (i < iCnt) && (firstNullPos < pgunionTaskTBL->maxClassNum); i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionReloadTaskTBL:: UnionLocate\n");
			continue;
		}
		memset(taskName,0,sizeof(taskName));
		UnionReadXMLPackageValue("taskName", taskName, sizeof(taskName));
		memset(startCmd,0,sizeof(startCmd));
		UnionReadXMLPackageValue("startCmd", startCmd, sizeof(startCmd));
		memset(taskNum,0,sizeof(taskNum));
		UnionReadXMLPackageValue("taskNum", taskNum, sizeof(taskNum));
		memset(logFileName,0,sizeof(logFileName));
		UnionReadXMLPackageValue("logFileName", logFileName, sizeof(logFileName));

		// 读取名称
		memset(tmpBuf, 0, sizeof(tmpBuf));
		if( (ret = UnionFilterRubbisBlank(taskName, strlen(taskName), tmpBuf, sizeof(tmpBuf))) <= 0 )
		{
			UnionUserErrLog("in UnionReloadTaskTBL:: UnionFilterRubbisBlank [%s]\n", taskName);
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
		if (strlen(startCmd) > sizeof(ptaskClass->startCmd) - 1)
			continue;
		strcpy(ptaskClass->startCmd, startCmd);
			
		// 读取minNum
		if ((ptaskClass->minNum = atoi(taskNum)) < 0)
			ptaskClass->minNum = 1;
		
		// 读取logFileName

		if (strlen(logFileName) > sizeof(ptaskClass->logFileName) - 1)
			continue;
		strcpy(ptaskClass->logFileName, logFileName);
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
	int		ret = 0;
	int		index = 0;
	char		sql[128+1];
	char		startCmd[100+1];
	char		num[16+1];
	char		logFileName[100+1];
	TUnionTaskClass	taskClass;
	PUnionTaskClass	ptaskClass;

	if (taskName == NULL)
		return(UnionReloadTaskTBL());
	if (strlen(taskName) >= sizeof(taskClass.name))
	{
		UnionUserErrLog("in UnionReloadTaskClass:: taskName [%s] too long!\n",taskName);
		return(errCodeParameter);
	}

	memset(sql,0,sizeof(sql));
	sprintf(sql,"select name,startCmd,num,logFileName from unionTask where name = '%s'",taskName);
	
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionReloadTaskClass:: UnionSelectRealDBRecord\n");
		return (ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionReloadTaskClass:: ret == 0\n");
		return (errCodeDatabaseMDL_RecordNotFound);
	}
	
	if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
	{
		UnionUserErrLog("in UnionReloadTaskClass:: UnionLocate\n");
		return(ret);
	}
	memset(startCmd,0,sizeof(startCmd));
	UnionReadXMLPackageValue("startCmd", startCmd, sizeof(startCmd));
	memset(num,0,sizeof(num));
	UnionReadXMLPackageValue("num", num, sizeof(num));
	memset(logFileName,0,sizeof(logFileName));
	UnionReadXMLPackageValue("logFileName", logFileName, sizeof(logFileName));

	// 拼装任务
	memset(&taskClass, 0, sizeof(taskClass));
	strcpy(taskClass.name, taskName);
	if( strlen(startCmd) < sizeof(taskClass.startCmd) )
		strcpy(taskClass.startCmd, startCmd);
	if( strlen(taskClass.startCmd) != 0 )
		taskClass.minNum = atoi(num);
	if( strlen(logFileName) < sizeof(taskClass.logFileName) )
		strcpy(taskClass.logFileName, logFileName);
	else
		memcpy(taskClass.logFileName, logFileName, sizeof(taskClass.logFileName)-1);

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
