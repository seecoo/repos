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

#include "unionMsgBuf.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 	reload the keyDBSvrBuf\n");
	printf("  -available 	print the available keyDBSvrBuf pos\n");
	printf("  -occupied 	print the occupied keyDBSvrBuf pos\n");
	printf("  -status	print the status of keyDBSvrBuf\n");
	printf("  -timeout timeout	reset timeout\n");
	printf("  -clear	clear rubbish message\n");
	printf("  -resetall	read all pos available\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectKDBSvrBufMDL();
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
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,UnionGetApplicationName())) == NULL)
	{
		UnionPrintf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	if (strcasecmp(argv[1],"-RELOAD") == 0)
	{
		if (UnionConfirm("Are you sure of reloading keyDBSvrBuf?"))
		{
			if ((ret = UnionReloadKDBSvrBufDef()) < 0)
				UnionPrintf("UnionReloadKDBSvrBufDef Error! ret = [%d]\n",ret);
			else
				UnionPrintf("UnionReloadKDBSvrBufDef OK!\n");
		}
	}
	else if (strcasecmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if ((ret = UnionReloadKDBSvrBufDef()) < 0)
			UnionPrintf("UnionReloadKDBSvrBufDef Error! ret = [%d]\n",ret);
		else
			UnionPrintf("UnionReloadKDBSvrBufDef OK!\n");
	}
	else if (strcasecmp(argv[1],"-AVAILABLE") == 0)
	{
		if ((ret = UnionPrintAvailablKDBSvrBufPosToFile(stdout)) < 0)
			UnionPrintf("UnionPrintAvailablKDBSvrBufPosToFile Error! ret = [%d]\n",ret);
	}
	else if (strcasecmp(argv[1],"-OCCUPIED") == 0)
	{
		if ((ret = UnionPrintInavailabeKDBSvrBufPosToFile(stdout)) < 0)
			UnionPrintf("UnionPrintInavailabeKDBSvrBufPosToFile Error! ret = [%d]\n",ret);
	}
	else if (strcasecmp(argv[1],"-TIMEOUT") == 0)
	{
		if (argc < 3)
		{
			UnionHelp();
		}
		else
		{
			if ((ret = UnionResetKDBServiceTimeout(atoi(argv[2]))) < 0)
				UnionPrintf("UnionResetKDBServiceTimeout Error! ret = [%d]\n",ret);
			else
				UnionPrintf("UnionResetKDBServiceTimeout OK!\n");
		}
	}
	else if (strcasecmp(argv[1],"-STATUS") == 0)
	{
		if ((ret = UnionPrintKDBSvrBufStatusToFile(stdout)) < 0)
			UnionPrintf("UnionPrintKDBSvrBufStatusToFile Error! ret = [%d]\n",ret);
	}
	else if (strcasecmp(argv[1],"-CLEAR") == 0)
		return(StartClearTask(argc-2,&argv[2]));
	else if (strcasecmp(argv[1],"-RESETALL") == 0)
	{
		if (UnionConfirm("Are you sure of resetting keyDBSvrBuf?"))
		{
			if ((ret = UnionResetAllKDBRecPos()) < 0)
				UnionPrintf("UnionResetAllKDBRecPos Error! ret = [%d]\n",ret);
			else
				UnionPrintf("UnionResetAllKDBRecPos OK!\n");
		}
	}
	else
		UnionHelp();
	return(UnionTaskActionBeforeExit());
}

int StartClearTask(int argc,char *argv[])
{
	int	ret;
	int	interval = 1;
	
	if (UnionCreateProcess() > 0)
		return(UnionTaskActionBeforeExit());
	
	if (argc >= 1)
		interval = atoi(argv[0]);
	if (interval <= 0)
		interval = 1;

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s -clear %d",UnionGetApplicationName(),interval)) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	for (;;)
	{
		
		if ((ret = UnionClearKDBSvrBufRubbish()) < 0)
		{
			UnionUserErrLog("in StartClearTask:: UnionClearKDBSvrBufRubbish!\n");
				return(UnionTaskActionBeforeExit());
		}
		sleep(interval);
	}
}

