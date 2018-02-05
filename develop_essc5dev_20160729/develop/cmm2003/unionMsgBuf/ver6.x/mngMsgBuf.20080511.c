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

#include "unionMsgBuf6.x.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 	reload the msgBufDef\n");
	printf("  -available 	print the available msgBuf pos\n");
	printf("  -occupied 	print the occupied msgBuf pos\n");
	printf("  -status	print the status of msgBuf\n");
	printf("  -printall	print the whole status of msgBuf\n");
	printf("  -clear	clear rubbish message\n");
	printf("  -resetall	reset all index\n");
	printf("  -setlostindex index ...\n");
	printf("  -repair index ...\n");
	printf("  -repairall\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectMsgBufMDL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	char	*p;
	int	i;

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

	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionPrintf("UnionConnectMsgBufMDL Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"-RELOAD") == 0)
	{
		if (!UnionConfirm("确定加载共享内存[消息表]吗?"))
			return(-1);
		if ((ret = UnionReloadMsgBufDef()) < 0)
			printf("***** %-30s Error!\n","加载共享内存[消息表]");
		else
			printf("***** %-30s OK!\n","加载共享内存[消息表]");
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if ((ret = UnionReloadMsgBufDef()) < 0)
			printf("***** %-30s Error!\n","加载共享内存[消息表]");
		else
			printf("***** %-30s OK!\n","加载共享内存[消息表]");
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-AVAILABLE") == 0)
	{
		if ((ret = UnionPrintAvailablMsgBufPosToFile(stdout)) < 0)
			UnionPrintf("in mngMsgBuf:: UnionPrintAvailablMsgBufPosToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"-OCCUPIED") == 0)
	{
		if ((ret = UnionPrintInavailabeMsgBufPosToFile(stdout)) < 0)
			UnionPrintf("in mngMsgBuf:: UnionPrintInavailabeMsgBufPosToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-STATUS") == 0)
	{
		if ((ret = UnionPrintMsgBufStatusToFile(stdout)) < 0)
			UnionPrintf("in mngMsgBuf:: UnionPrintMsgBufStatusToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-PRINTALL") == 0)
	{
		if ((ret = UnionPrintMsgBufToFile(stdout)) < 0)
			UnionPrintf("in mngMsgBuf:: UnionPrintMsgBufToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-CLEAR") == 0)
		return(StartClearTask(argc-2,&argv[2]));

	if (strcasecmp(argv[1],"-RESETALL") == 0)
	{
		if (UnionConfirm("Are you sure of reset all the index available?"))
		{
			if ((ret = UnionResetAllMsgIndexAvailable()) < 0)
				printf("UnionResetAllMsgIndexAvailable failure! ret = [%d]\n",ret);
			else
				printf("UnionResetAllMsgIndexAvailable OK!\n");
		}
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"-SETLOSTINDEX") == 0)
	{
		for (i = 2; i < argc; i++)
		{
			if (!UnionConfirm("Are you sure of set the index [%04d] of msgBuf losted?",atoi(argv[i])))
				continue;
			if ((ret = UnionSetSpecMsgBufIndexLosted(atoi(argv[i]))) < 0)
				printf("UnionSetSpecMsgBufIndexLosted [%04d] failure! ret = [%d]\n",atoi(argv[i]),ret);
			else
				printf("UnionSetSpecMsgBufIndexLosted [%04d] OK!\n",atoi(argv[i]));
		}
		return(UnionTaskActionBeforeExit());		
	}
	
	if (strcasecmp(argv[1],"-REPAIR") == 0)
	{
		for (i = 2; i < argc; i++)
		{
			if (!UnionConfirm("Are you sure of set the index [%04d] of msgBuf free?",atoi(argv[i])))
				continue;
			if ((ret = UnionRepairSpecMsgBufIndex(atoi(argv[i]))) < 0)
				printf("UnionRepairSpecMsgBufIndex [%04d] failure! ret = [%d]\n",atoi(argv[i]),ret);
			else
				printf("UnionRepairSpecMsgBufIndex [%04d] OK!\n",atoi(argv[i]));
		}
		return(UnionTaskActionBeforeExit());		
	}

	if (strcasecmp(argv[1],"-REPAIRALL") == 0)
	{
		if (UnionConfirm("Are you sure of repair all the indexes?"))
		{
			if ((ret = UnionRepairAllMsgBufIndex()) < 0)
				printf("UnionRepairAllMsgBufIndex failure! ret = [%d]\n",ret);
			else
				printf("UnionRepairAllMsgBufIndex OK!\n");
		}
		return(UnionTaskActionBeforeExit());		
	}

	printf("wrong command = [%s]\n",argv[1]);
	UnionHelp();
	return(UnionTaskActionBeforeExit());
}

int StartClearTask(int argc,char *argv[])
{
	int	ret;
	int	interval;
	
	if (UnionCreateProcess() > 0)
		return(UnionTaskActionBeforeExit());
	
	if (argc <= 0)
		interval = 1;
	else
	{
		if ((interval = atoi(argv[0])) <= 0)
			interval = 1;
	}
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"mngMsgBuf -clear %d",interval)) == NULL)
	{
		UnionPrintf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	for (;;)
	{
		
		if ((ret = UnionFreeRubbishMsg()) < 0)
		{
			UnionUserErrLog("in StartClearTask:: UnionFreeRubbishMsg!\n");
				return(UnionTaskActionBeforeExit());
		}
		sleep(interval);
	}
}
