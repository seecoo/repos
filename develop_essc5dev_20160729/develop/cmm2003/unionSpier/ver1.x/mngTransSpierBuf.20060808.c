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

#include "transSpierBuf.h"

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
	UnionDisconnectTransSpierBufMDL();
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
		if ((ret = UnionReloadTransSpierBufDef()) < 0)
			UnionPrintf("in mngTransSpierBuf:: UnionReloadTransSpierBufDef Error! ret = [%d]\n",ret);
		else
			UnionPrintf("in mngTransSpierBuf:: UnionReloadTransSpierBufDef OK!\n");
	}
	else if (strcasecmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if ((ret = UnionReloadTransSpierBufDef()) < 0)
			UnionPrintf("in mngTransSpierBuf:: UnionReloadTransSpierBufDef Error! ret = [%d]\n",ret);
		else
			UnionPrintf("in mngTransSpierBuf:: UnionReloadTransSpierBufDef OK!\n");
	}
	else if (strcasecmp(argv[1],"-AVAILABLE") == 0)
	{
		if ((ret = UnionPrintAvailablTransSpierBufPosToFile(stdout)) < 0)
			UnionPrintf("in mngTransSpierBuf:: UnionPrintAvailablTransSpierBufPosToFile Error! ret = [%d]\n",ret);
	}
	else if (strcasecmp(argv[1],"-OCCUPIED") == 0)
	{
		if ((ret = UnionPrintInavailabeTransSpierBufPosToFile(stdout)) < 0)
			UnionPrintf("in mngTransSpierBuf:: UnionPrintInavailabeTransSpierBufPosToFile Error! ret = [%d]\n",ret);
	}
	else if (strcasecmp(argv[1],"-STATUS") == 0)
	{
		if ((ret = UnionPrintTransSpierBufStatusToFile(stdout)) < 0)
			UnionPrintf("in mngTransSpierBuf:: UnionPrintTransSpierBufStatusToFile Error! ret = [%d]\n",ret);
	}
	else if (strcasecmp(argv[1],"-PRINTALL") == 0)
	{
		if ((ret = UnionPrintTransSpierBufToFile(stdout)) < 0)
			UnionPrintf("in mngTransSpierBuf:: UnionPrintTransSpierBufToFile Error! ret = [%d]\n",ret);
	}
	else if (strcasecmp(argv[1],"-RESETALL") == 0)
	{
		if (UnionConfirm("Are you sure of reset all the index available?"))
		{
			if ((ret = UnionResetAllTransSpierMsgPos()) < 0)
				printf("UnionResetAllTransSpierMsgPos failure! ret = [%d]\n",ret);
			else
				printf("UnionResetAllTransSpierMsgPos OK!\n");
		}
	}
	else
		UnionHelp();
	return(UnionTaskActionBeforeExit());
}
