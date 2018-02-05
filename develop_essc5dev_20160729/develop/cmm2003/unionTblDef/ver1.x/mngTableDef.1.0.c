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

#include "unionTableDef.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 		reload the tableDefTBL\n");
	printf("  -all			print all tableDefTBL definition\n");
	printf("  -table tableName	print the records of tableName\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTableDefTBL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int PrintRecOfTableDef(int num,char *tableName[])
{
	int	i;
	int	ret = 0;
	char	*p;
		
	for (i = 0; i < num; i++)
	{
		if ((ret = UnionPrintRecOfTableDefToFile(tableName[i],stdout)) < 0)
			printf("UnionPrintRecOfTableDefToFile [%s] Error! ret = [%d]\n",tableName[i],ret);
	}
	
	if (num > 0)
		return(0);
	
	while(1)
	{
		if (UnionIsQuit(p = UnionInput("Input tableName::")))
			break;
		if ((ret = UnionPrintRecOfTableDefToFile(p,stdout)) < 0)
			printf("UnionPrintRecOfTableDefToFile [%s] Error! ret = [%d]\n",p,ret);
	}
	return(ret);
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
		if (!UnionConfirm("确定加载数据库定义吗?"))
			return(-1);
		if ((ret = UnionReloadTableDefTBL()) < 0)
			printf("***** %-30s Error!\n","加载共享内存[数据库表定义]");
		else
			printf("***** %-30s OK!\n","加载共享内存[数据库表定义]");
	}
	else if (strcasecmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if ((ret = UnionReloadTableDefTBL()) < 0)
			printf("***** %-30s Error!\n","加载共享内存[数据库表定义]");
		else
			printf("***** %-30s OK!\n","加载共享内存[数据库表定义]");
	}
	else if (strcasecmp(argv[1],"-ALL") == 0)
		UnionPrintTableDefTBLToFile(stdout);
	else if (strcasecmp(argv[1],"-TABLE") == 0)
		PrintRecOfTableDef(argc-2,&argv[2]);			
	else 
		return(UnionHelp());
	
	return(0);
}


