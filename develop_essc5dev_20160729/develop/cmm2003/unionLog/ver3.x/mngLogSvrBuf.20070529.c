//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	2.0

#define _UnionTask_3_x_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"

#include "unionLogSvrBuf.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 	reload the spierTransMsgBufDef\n");
	printf("  -available 	print the available spierTransMsgBuf pos\n");
	printf("  -occupied 	print the occupied spierTransMsgBuf pos\n");
	printf("  -status	print the status of spierTransMsgBuf\n");
	printf("  -printall	print the whole status of spierTransMsgBuf\n");
	printf("  -resetall	reset all index\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectLogSvrBufMDL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	char	*p;

	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,UnionGetApplicationName())) == NULL)
	{
		UnionPrintf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-RELOAD") == 0)
	{
		if (!UnionConfirm("Are you sure of reloading spierTransMsgBufDefinition?"))
			return(-1);
		if ((ret = UnionReloadLogSvrBufDef()) < 0)
			UnionPrintf("in mngLogSvrBuf:: UnionReloadLogSvrBufDef Error! ret = [%d]\n",ret);
		else
			UnionPrintf("in mngLogSvrBuf:: UnionReloadLogSvrBufDef OK!\n");
	}
	else if (strcasecmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if ((ret = UnionReloadLogSvrBufDef()) < 0)
			UnionPrintf("in mngLogSvrBuf:: UnionReloadLogSvrBufDef Error! ret = [%d]\n",ret);
		else
			UnionPrintf("in mngLogSvrBuf:: UnionReloadLogSvrBufDef OK!\n");
	}
	else if (strcasecmp(argv[1],"-AVAILABLE") == 0)
	{
		if ((ret = UnionPrintAvailablLogSvrBufPosToFile(stdout)) < 0)
			UnionPrintf("in mngLogSvrBuf:: UnionPrintAvailablLogSvrBufPosToFile Error! ret = [%d]\n",ret);
	}
	else if (strcasecmp(argv[1],"-OCCUPIED") == 0)
	{
		if ((ret = UnionPrintInavailabeLogSvrBufPosToFile(stdout)) < 0)
			UnionPrintf("in mngLogSvrBuf:: UnionPrintInavailabeLogSvrBufPosToFile Error! ret = [%d]\n",ret);
	}
	else if (strcasecmp(argv[1],"-STATUS") == 0)
	{
		if ((ret = UnionPrintLogSvrBufStatusToFile(stdout)) < 0)
			UnionPrintf("in mngLogSvrBuf:: UnionPrintLogSvrBufStatusToFile Error! ret = [%d]\n",ret);
	}
	else if (strcasecmp(argv[1],"-PRINTALL") == 0)
	{
		if ((ret = UnionPrintLogSvrBufToFile(stdout)) < 0)
			UnionPrintf("in mngLogSvrBuf:: UnionPrintLogSvrBufToFile Error! ret = [%d]\n",ret);
	}
	else if (strcasecmp(argv[1],"-RESETALL") == 0)
	{
		if (UnionConfirm("Are you sure of reset all the index available?"))
		{
			if ((ret = UnionResetAllLogSvrMsgPos()) < 0)
				printf("UnionResetAllLogSvrMsgPos failure! ret = [%d]\n",ret);
			else
				printf("UnionResetAllLogSvrMsgPos OK!\n");
		}
	}
	else
		UnionHelp();
	return(UnionTaskActionBeforeExit());
}
