// Author:	ChenJiaMei
// Date:	2004-7-15

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif
#include "UnionTask.h"
#include "commCheck.h"
#include "UnionLog.h"
#include "unionVersion.h"
#include "unionCommand.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf("Where command as follows\n");
	printf(" -check, start a checker\n");
	printf(" -forgds,start a temp checker for GDS\n");
	printf(" -listen,start a listener\n");
	printf(" -print, print status of host groups\n");
	printf(" -openexitwhenalldown\n");
	printf(" -closeexitwhenalldown\n");
	printf(" -reload\n");
	return(0);
}

int main(int argc,char *argv[])
{
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
		
	if (strcasecmp(argv[1],"-CHECK") == 0)
		UnionCheckAllHostsAlways();
	else if (strcasecmp(argv[1],"-PRINT") == 0)
		UnionPrintHostGroup();
	else if (strcasecmp(argv[1],"-OPENEXITWHENALLDOWN") == 0)
		UnionOpenExitWhenAllHostsDown();
	else if (strcasecmp(argv[1],"-CLOSEEXITWHENALLDOWN") == 0)
		UnionCloseExitWhenAllHostsDown();
	else if (strcasecmp(argv[1],"-RELOAD") == 0)
		UnionReloadHostGroup();
	else if (strcasecmp(argv[1],"-LISTEN") == 0)
		UnionStartNullHostListener();
	else if (strcasecmp(argv[1],"-FORGDS") == 0)
	{
		if (UnionCheckAllHostsUntilAllHostsDown() <= 0)
			UnionUserErrLog("in main:: UnionCheckAllHostsUntilAllHostsDown!\n");
	}
	else
		UnionHelp();
	return(UnionTaskActionBeforeExit());
}
