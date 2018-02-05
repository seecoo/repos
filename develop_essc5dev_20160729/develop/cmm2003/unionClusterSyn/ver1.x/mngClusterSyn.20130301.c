#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif
#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"
#include "UnionStr.h"
#include "unionDataSync.h"
#include "clusterSyn.h"

PUnionTaskInstance	ptaskInstance = NULL;
#define	numOfClusterSyn		128

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -syn [间隔时间] 	间隔一段时间自动加载\n");
	printf("  -clear 		删除本机所有的同步记录\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int StartSynTask(int argc,char *argv[])
{
	int	i,j;
	int	cmdNum = 0;
	int	ret;
	int	num = 0;
	int	interval;
	char	taskName[128];
	char	*cmdPtr[8];
	char	tmpBuf[81920];
	TUnionClusterDefTBL	clusterDefTBL;
	TUnionClusterSyn	clusterSyn[numOfClusterSyn];
	
	if (UnionCreateProcess() > 0)
		return(UnionTaskActionBeforeExit());
	
	if (argc <= 0)
	{
		UnionPrintf("argc <= 0 Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	if ((interval = atoi(argv[0])) <= 0)
	{
		UnionPrintf("argv <= 0 Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	
	memset(taskName,0,sizeof(taskName));
	sprintf(taskName,"%s -syn %d",UnionGetApplicationName(),interval);
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,taskName)) == NULL)
	{
		UnionPrintf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if ((ret = UnionInitClusterDef(&clusterDefTBL)) < 0)
	{
		UnionUserErrLog("in StartSynTask:: UnionInitClusterDef!\n");
		return(UnionTaskActionBeforeExit());
	}

	UnionSetDataSyncIsWaitResult();
	for (;;)
	{
		if ((num = UnionReadClusterSyn(clusterDefTBL.clusterNo,clusterSyn,numOfClusterSyn)) < 0)
		{
			UnionUserErrLog("in StartSynTask:: UnionReadClusterSyn!\n");
			return(UnionTaskActionBeforeExit());
		}
		
		for (i = 0; i < num ; i++)
		{
			if ((cmdNum = UnionSeprateVarStrIntoPtr(clusterSyn[i].cmd,strlen(clusterSyn[i].cmd),';',cmdPtr,8)) < 0)
			{
				UnionUserErrLog("in StartSynTask:: UnionSeprateVarStrIntoPtr[%s]!\n",clusterSyn[i].cmd);
				return(cmdNum);
			}
		
			for (j = 0; j < cmdNum; j++)
			{
				// 多条命令顺序处理
				if (memcmp(cmdPtr[j],"mng",3) == 0)
					snprintf(tmpBuf,sizeof(tmpBuf),"%s >/dev/null 2>&1",cmdPtr[j]);
				else
					snprintf(tmpBuf,sizeof(tmpBuf),"%s",cmdPtr[j]);

				system(tmpBuf);	
				UnionProgramerLog("in StartSynTask:: cmd[%s]\n",tmpBuf);
				usleep(200);
				continue;
			}
			UnionDeleteClusterSynBySN(clusterSyn[i].sn);
		}
		

		sleep(interval);
	}
}

int StartClearTask()
{
	int	ret;
	TUnionClusterDefTBL	clusterDefTBL;
	
	if ((ret = UnionInitClusterDef(&clusterDefTBL)) < 0)
	{
		UnionUserErrLog("in StartClearTask:: UnionInitClusterDef!\n");
		return(UnionTaskActionBeforeExit());
	}
	// 删除本机所有的同步记录
	UnionDeleteClusterSynByClusterNo(clusterDefTBL.clusterNo);
	return(UnionTaskActionBeforeExit());
}

int main(int argc,char *argv[])
{
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);
	
	/*
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	*/

	if (strcasecmp(argv[1],"-SYN") == 0)
		return(StartSynTask(argc-2,&argv[2]));
	else if (strcasecmp(argv[1],"-CLEAR") == 0)
		return(StartClearTask());
	else 
		return(UnionHelp());
	return(UnionTaskActionBeforeExit());
}

