//	Author:		Wolfgang Wang
//	Date:		2003/09/22


#define _UnionTask_3_x_	

#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "UnionTask.h"
#include "unionCommand.h"
#include "unionVersion.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage: %s command\n",UnionGetApplicationName());
	printf(" Where command as follow:\n");
	printf("	-kaa	Kill all the process in the task registry, before killing, user's confirm is required.\n");
	printf("	-ka	Kill all the process in the task registry, whereas no user's confirm is required.\n");
	printf("	-reload reload taskClass Definition.\n");
	printf("	-pclass	Print all the task class.\n");
	printf("	-ptaskInstance Print all the task instance.\n");
	printf("	-pstatus Print status of the registry.\n");
	printf("	-m Seconds	Every Interval of Seconds the task registry will be scanned once.\n");
	printf("	-mp Seconds	Every Interval of Seconds all process defined in unionTaskTBL.CFG will be scanned once.\n");
	printf("	-kn	taskName\n");
	printf("	-kln	taskName Kill task by name which contains the specified string\n");
	printf("	-kp	ProcID\n");
	
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int KillAllProcessAnyway()
{
	int	ret;
	
	if ((ret = UnionKillAllTaskInstance()) < 0)
		printf("UnionKillAllTaskInstance Error! ret = [%d]\n",ret);
	else
		printf("UnionKillAllTaskInstance OK! [%d] killed!\n",ret);
	return(UnionTaskActionBeforeExit());
}

int KillAllProcess()
{
	if (!UnionConfirm("Are you sure of killing all task instance?"))
		return(UnionTaskActionBeforeExit());	
	return(KillAllProcessAnyway());
}

int KillTaskByName(int argc,char **taskName)
{
	int	ret;
	int	i;
	char	*p;
	char	tmpBuf[128];
	
	for (i = 0; i < argc; i++)
	{
		if (!UnionConfirm("Kill all the tasks of name [%s]?",taskName[i]))
			continue;
		if ((ret = UnionKillTaskInstanceByName(taskName[i])) < 0)
			printf("Fail to Kill Task [%s]. ret = [%d]\n",taskName[i],ret);
		else
			printf("Kill [%d] Tasks [%s].\n",ret,taskName[i]);
	}
	if (argc >0 )
		return(UnionTaskActionBeforeExit());

	for (;;)
	{
		p = UnionInput("KillByName>Input task name (exit/quit to exit)::");
		memset(tmpBuf,0,sizeof(tmpBuf));
		strcpy(tmpBuf,p);
		if (UnionIsQuit(p))
			return(UnionTaskActionBeforeExit());
		if (!UnionConfirm("Kill all the tasks of name [%s]?",tmpBuf))
			continue;
		if ((ret = UnionKillTaskInstanceByName(tmpBuf)) < 0)
			printf("Fail to Kill Task [%s]. ret = [%d]\n",tmpBuf,ret);
		else
			printf("Kill [%d] Tasks [%s].\n",ret,tmpBuf);
	}
}

int KillTaskByAlikeName(int argc,char **taskName)
{
	int	ret;
	int	i;
	char	*p;
	char	tmpBuf[128];
	
	for (i = 0; i < argc; i++)
	{
		if (!UnionConfirm("Kill all the tasks of name like [%s]?",taskName[i]))
			continue;
		if ((ret = UnionKillTaskInstanceByAlikeName(taskName[i])) < 0)
			printf("Fail to Kill Task like [%s]. ret = [%d]\n",taskName[i],ret);
		else
			printf("Kill [%d] Tasks like [%s].\n",ret,taskName[i]);
	}
	if (argc >0 )
		return(UnionTaskActionBeforeExit());

	for (;;)
	{
		p = UnionInput("KillByName>Input task name (exit/quit to exit)::");
		memset(tmpBuf,0,sizeof(tmpBuf));
		strcpy(tmpBuf,p);
		if (UnionIsQuit(p))
			return(UnionTaskActionBeforeExit());
		if (!UnionConfirm("Kill all the tasks of name like [%s]?",tmpBuf))
			continue;
		if ((ret = UnionKillTaskInstanceByAlikeName(tmpBuf)) < 0)
			printf("Fail to Kill Task of name like [%s] . ret = [%d]\n",tmpBuf,ret);
		else
			printf("Kill [%d] Tasks of name like [%s].\n",ret,tmpBuf);
	}
}

int KillTaskByProcID(int argc,char **taskProcID)
{
	int	ret;
	int	i;
	char	*p;
	
	for (i = 0; i < argc; i++)
	{
		if (!UnionConfirm("Kill the task of procID [%d]?",atoi(taskProcID[i])))
			continue;
		if ((ret = UnionKillTaskInstanceByProcID(atoi(taskProcID[i]))) < 0)
			printf("Fail to Kill Task [%s]. ret = [%d]\n",taskProcID[i],ret);
		else
			printf("Kill Task [%s] OK.\n",taskProcID[i]);
	}
	if (argc >0 )
		return(UnionTaskActionBeforeExit());

	for (;;)
	{
		if (UnionIsQuit(p = UnionInput("KillByProcID>Input task procID (exit/quit to exit)::")))
			return(UnionTaskActionBeforeExit());
		if (!UnionConfirm("Kill the task of procID [%d]?",atoi(p)))
			continue;
		if ((ret = UnionKillTaskInstanceByProcID(atoi(p))) < 0)
			printf("Fail to Kill Task [%s]. ret = [%d]\n",p,ret);
		else
			printf("Kill Task [%s].\n",p);
	}
}
int main(int argc,char **argv)
{
	int	ret;
	int	seconds;
	
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	//printf("ptaskInstance = [%x]\n",ptaskInstance);
	
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	if (strcasecmp(argv[1],"-KAA") == 0)
		return(KillAllProcessAnyway());
	
	if (strcasecmp(argv[1],"-KA") == 0)
		return(KillAllProcess());
		
	if (strcasecmp(argv[1],"-PCLASS") == 0)
	{
		ret = UnionPrintTaskClassInTBL();
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"-PINSTANCE") == 0)
	{
		ret = UnionPrintTaskInstanceInTBL();
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"-P") == 0)
	{
		ret = UnionPrintTaskInstanceInTBL();
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-PSTATUS") == 0)
	{
		ret = UnionPrintStatusOfTaskTBL();
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"-M") == 0)
	{
		if (UnionCreateProcess() > 0)
			return(UnionTaskActionBeforeExit());
		if (argc >= 3)
			seconds = atoi(argv[2]);
		else
			seconds = 1;
		if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s -m %d",UnionGetApplicationName(),seconds)) == NULL)
		{
			printf("UnionCreateTaskInstance Error!\n");
			return(UnionTaskActionBeforeExit());
		}
		ret = UnionClearRubbishTaskInstance(seconds);
		return(UnionTaskActionBeforeExit());
	}		

	if (strcasecmp(argv[1],"-MP") == 0)
	{
		if (UnionCreateProcess() > 0)
			return(UnionTaskActionBeforeExit());
		if (argc >= 3)
			seconds = atoi(argv[2]);
		else
			seconds = 1;
		if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s -mp %d",UnionGetApplicationName(),seconds)) == NULL)
		{
			printf("UnionCreateTaskInstance Error!\n");
			return(UnionTaskActionBeforeExit());
		}
		ret = UnionStartTaskGuard(seconds);
		return(UnionTaskActionBeforeExit());
	}		
	
	if (strcasecmp(argv[1],"-RELOAD") == 0)
	{
		if (UnionConfirm("Reload taskClass Defintion?"))
		{
			if ((ret = UnionReloadTaskTBL()) < 0)
				printf("UnionReloadTaskTBL Error! ret = [%d]\n",ret);
			else
				printf("UnionReloadTaskTBL OK!\n");
		}
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"-KN") == 0)
		return(KillTaskByName(argc-2,&argv[2]));
	
	if (strcasecmp(argv[1],"-KLN") == 0)
		return(KillTaskByAlikeName(argc-2,&argv[2]));
	
	if (strcasecmp(argv[1],"-KP") == 0)
		return(KillTaskByProcID(argc - 2,&argv[2]));

	UnionHelp();
	return(UnionTaskActionBeforeExit());
}

