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
#include "UnionStr.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"

#include "UnionEnv.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -insert fileName var1 var2 ...\n");
	printf("  -update fileName var1 var2 ...\n");
	printf("  -delete fileName var1\n");
	printf("  -addline fileName str\n");
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
	char	fileName[256+1];
	char	varName[128+1];
	char	format[2048+1];
	int	index;
	int	offset = 0;
	
	UnionSetApplicationName(argv[0]);
	if (argc < 4)
		return(UnionHelp());
		
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	// 文件名称
	memset(fileName,0,sizeof(fileName));
	UnionReadDirFromStr(argv[2],0,fileName);
	
	// 第一个变量名称
	strcpy(varName,argv[3]);
	
	// 拼变量串
	memset(format,0,sizeof(format));
	for (index = 4; index < argc; index++)
	{
		sprintf(format+offset,"[%s]",argv[index]);
		offset = strlen(format);
	}
	
	if (strcasecmp(argv[1],"-INSERT") == 0)
	{
		if ((ret = UnionInsertEnviVar(fileName,varName,format)) < 0)
			UnionPrintf("UnionInsertEnviVar Error! ret = [%d]\n",ret);
		else
			UnionPrintf("UnionInsertEnviVar OK!\n");
	}
	else if (strcasecmp(argv[1],"-UPDATE") == 0)
	{
		if ((ret = UnionUpdateEnviVar(fileName,varName,format)) < 0)
			UnionPrintf("UnionUpdateEnviVar Error! ret = [%d]\n",ret);
		else
			UnionPrintf("UnionUpdateEnviVar OK!\n");
	}
	else if (strcasecmp(argv[1],"-DELETE") == 0)
	{
		if ((ret = UnionDeleteEnviVar(fileName,varName)) < 0)
			UnionPrintf("UnionDeleteEnviVar Error! ret = [%d]\n",ret);
		else
			UnionPrintf("UnionDeleteEnviVar OK!\n");
	}
	else if (strcasecmp(argv[1],"-ADDLINE") == 0)
	{
		if ((ret = UnionAddStrToFile(fileName,varName)) < 0)
			UnionPrintf("UnionAddStrToFile Error! ret = [%d]\n",ret);
		else
			UnionPrintf("UnionAddStrToFile OK!\n");
	}
	else
		UnionHelp();
	return(UnionTaskActionBeforeExit());
}

