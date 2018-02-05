//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	2.0

#define _UnionLogMDL_3_x_
#define _UnionTask_3_x_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 	reload the logfiletbl\n");
	printf("  -print 	print the logfiletbl\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int main(int argc,char *argv[])
{
	int	ret;

	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	/*
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"mngUnionLog")) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	*/

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	if (strcasecmp(argv[1],"-RELOAD") == 0)
	{
		if (!UnionConfirm("确定要加载共享内存[日志表]吗?"))
			return(-1);
		if ((ret = UnionReloadLogFileTBL()) < 0)
			printf("***** %-30s Error!\n","加载共享内存[日志表]");
		else
			printf("***** %-30s OK!\n","加载共享内存[日志表]");
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if ((ret = UnionReloadLogFileTBL()) < 0)
			printf("***** %-30s Error!\n","加载共享内存[日志表]");
		else
			printf("***** %-30s OK!\n","加载共享内存[日志表]");
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-PRINT") == 0)
	{
		if ((ret = UnionPrintLogFileTBL()) < 0)
			UnionPrintf("in mngUnionLog:: UnionPrintLogFileTBL Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}

	UnionHelp();
	return(UnionTaskActionBeforeExit());
}

		
