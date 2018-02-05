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

#include "errCodeTranslater.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 		reload the transDefTBL\n");
	printf("  -all			print all transDef definition\n");
	printf("  -errcode errCode	print the records of errCode\n");
	printf("  -remark remark	print the records of remark\n");
	printf("  errCode\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectErrCodeTranslater();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int PrintRecOfCode(int num,char *errCode[])
{
	int	i;
	int	ret;
	char	*p;
		
	for (i = 0; i < num; i++)
	{
		if ((ret = UnionPrintRecOfErrCodeToFile(atoi(errCode[i]),stdout)) < 0)
			printf("UnionPrintRecOfErrCodeToFile [%s] Error! ret = [%d]\n",errCode[i],ret);
	}
	
	if (num > 0)
		return(0);
	
loopInput:
	if (UnionIsQuit(p = UnionInput("Input ErrCode::")))
		return(0);
	if ((ret = UnionPrintRecOfErrCodeToFile(atoi(p),stdout)) < 0)
		printf("UnionPrintRecOfErrCodeToFile [%s] Error! ret = [%d]\n",p,ret);
	goto loopInput;
}

int PrintRecOfRemark(int num,char *errCode[])
{
	int	i;
	int	ret;
	char	*p;
		
	for (i = 0; i < num; i++)
	{
		if ((ret = UnionPrintAllErrCodeOfRemarkToFile(errCode[i],stdout)) < 0)
			printf("UnionPrintAllErrCodeOfRemarkToFile [%s] Error! ret = [%d]\n",errCode[i],ret);
	}
	
	if (num > 0)
		return(0);
	
loopInput:
	if (UnionIsQuit(p = UnionInput("Input remark::")))
		return(0);
	if ((ret = UnionPrintAllErrCodeOfRemarkToFile(p,stdout)) < 0)
		printf("UnionPrintAllErrCodeOfRemarkToFile [%s] Error! ret = [%d]\n",p,ret);
	goto loopInput;
}

int main(int argc,char *argv[])
{
	int	ret;
		
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

	if (strcasecmp(argv[1],"-RELOAD") == 0)
	{
		if (!UnionConfirm("确定加载共享内存[错误码表]吗?"))
			return(-1);
		if ((ret = UnionReloadErrCodeTranslater()) < 0)
			printf("***** %-30s Error!\n","加载共享内存[错误码表]");
		else
			printf("***** %-30s OK!\n","加载共享内存[错误码表]");
	}
	else if (strcasecmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if ((ret = UnionReloadErrCodeTranslater()) < 0)
			printf("***** %-30s Error!\n","加载共享内存[错误码表]");
		else
			printf("***** %-30s OK!\n","加载共享内存[错误码表]");
	}
	else if (strcasecmp(argv[1],"-ALL") == 0)
		UnionPrintErrCodeTranslaterToFile(stdout);
	else if (strcasecmp(argv[1],"-ERRCODE") == 0)
		PrintRecOfCode(argc-2,&argv[2]);			
	else if (strcasecmp(argv[1],"-REMARK") == 0)
		PrintRecOfRemark(argc-2,&argv[2]);			
	else 
		PrintRecOfCode(argc-1,&argv[1]);			
	return(UnionTaskActionBeforeExit());
}

