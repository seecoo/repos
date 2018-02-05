//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	2.0

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif
#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"

#include "unionErrCodeTBL.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 		reload the transDefTBL\n");
	printf("  -all			print all transDef definition\n");
	printf("  -soft softErrCode	print the records of softErrCode\n");
	printf("  -cli cliErrCode	print the records of cliErrCode\n");
	printf("  errCode\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectSoftErrCodeTBL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int PrintRecOfSoftCode(int num,char *errCode[])
{
	int	i;
	int	ret;
	char	*p;
		
	for (i = 0; i < num; i++)
	{
		if ((ret = UnionPrintRecOfSoftErrCodeToFile(atoi(errCode[i]),stdout)) < 0)
			printf("UnionPrintRecOfSoftErrCodeToFile [%s] Error! ret = [%d]\n",errCode[i],ret);
	}
	
	if (num > 0)
		return(0);
	
loopInput:
	if (UnionIsQuit(p = UnionInput("Input softErrCode::")))
		return(0);
	if ((ret = UnionPrintRecOfSoftErrCodeToFile(atoi(p),stdout)) < 0)
		printf("UnionPrintRecOfSoftErrCodeToFile [%s] Error! ret = [%d]\n",p,ret);
	goto loopInput;
}

int PrintRecOfCliCode(int num,char *errCode[])
{
	int	i;
	int	ret;
	char	*p;
		
	for (i = 0; i < num; i++)
	{
		if ((ret = UnionPrintAllSoftErrCodeOfCliErrCodeToFile(errCode[i],stdout)) < 0)
			printf("UnionPrintAllSoftErrCodeOfCliErrCodeToFile [%s] Error! ret = [%d]\n",errCode[i],ret);
	}
	
	if (num > 0)
		return(0);
	
loopInput:
	if (UnionIsQuit(p = UnionInput("Input cliErrCode::")))
		return(0);
	if ((ret = UnionPrintAllSoftErrCodeOfCliErrCodeToFile(p,stdout)) < 0)
		printf("UnionPrintAllSoftErrCodeOfCliErrCodeToFile [%s] Error! ret = [%d]\n",p,ret);
	goto loopInput;
}

int main(int argc,char *argv[])
{
	int	ret;
		
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-RELOAD") == 0)
	{
		if (!UnionConfirm("Are you sure of reloading softErrCodeTBL?"))
			return(-1);
		if ((ret = UnionReloadSoftErrCodeTBL()) < 0)
			printf("UnionReloadSoftErrCodeTBL Error! ret = [%d]\n",ret);
		else
			printf("UnionReloadSoftErrCodeTBL OK!\n");
	}
	else if (strcasecmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if ((ret = UnionReloadSoftErrCodeTBL()) < 0)
			printf("UnionReloadSoftErrCodeTBL Error! ret = [%d]\n",ret);
		else
			printf("UnionReloadSoftErrCodeTBL OK!\n");
	}
	else if (strcasecmp(argv[1],"-ALL") == 0)
		UnionPrintSoftErrCodeTBLToFile(stdout);
	else if (strcasecmp(argv[1],"-ESSC") == 0)
		PrintRecOfSoftCode(argc-2,&argv[2]);			
	else if (strcasecmp(argv[1],"-CLI") == 0)
		PrintRecOfCliCode(argc-2,&argv[2]);			
	else 
		PrintRecOfCliCode(argc-1,&argv[1]);			
	return(UnionTaskActionBeforeExit());
}

