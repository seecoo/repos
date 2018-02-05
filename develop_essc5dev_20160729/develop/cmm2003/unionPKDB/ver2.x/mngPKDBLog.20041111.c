//	Wolfgang Wang
//	2003/09/09

#define _UnionTask_3_x_
#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"
#include "unionModule.h"

#include "unionPKDBLog.h"
#include "unionPKDB.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectPKDB();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command:\n");
	printf(" print	[startTime [endTime]]\n");
	printf(" restore [starttime [endTime]\n");

	return(0);
}


int Print(int argc,char *argv[])
{
	int	ret;
	long	gmaxKeyNum;
	char	*p;
	
	if (argc >= 2)
		UnionPrintPKDBLog(argv[0],argv[1]);
	else if (argc == 1)
		UnionPrintPKDBLog(argv[0],NULL);
	else
	{
		if (UnionConfirm("Are you want to see all the logFile?"))
			UnionPrintPKDBLog(NULL,NULL);
		else
		{
			if (!UnionIsQuit(p = UnionInput("Input start time(exit/quit to exit)::")))
				UnionPrintPKDBLog(p,NULL);
		}
	}
	return(UnionTaskActionBeforeExit());
}

int Restore(int argc,char *argv[])
{
	int	ret;
	long	gmaxKeyNum;
	char	*p;

	if (argc >= 2)
		UnionRestorePKDBFromLog(argv[0],argv[1]);
	else if (argc == 1)
		UnionRestorePKDBFromLog(argv[0],NULL);
	else 
	{
		if (UnionConfirm("Are you want to restore all from the whole logFile?"))
			UnionRestorePKDBFromLog(NULL,NULL);
		else
		{
			if (!UnionIsQuit(p = UnionInput("Input start time(exit/quit to exit)::")))
				UnionRestorePKDBFromLog(p,NULL);
		}
	}
		
	return(UnionTaskActionBeforeExit());
}
int main(int argc,char **argv)
{
	int	ret;
	
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"mngPKDBLog")) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
		
	if ((ret = UnionConnectPKDB()) < 0)
	{
		printf("in mngPKDBLog:: UnionConnectPKDB Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"PRINT") == 0)
		return(Print(argc-2,&argv[2]));
	
	if (strcasecmp(argv[1],"RESTORE") == 0)
		return(Restore(argc-2,&argv[2]));
	
	UnionHelp();
	return(UnionTaskActionBeforeExit());
}

