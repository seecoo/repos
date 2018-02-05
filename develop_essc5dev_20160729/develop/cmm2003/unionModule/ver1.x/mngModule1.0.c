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
#include "unionModule.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -removeall\n");
	printf("  -removeallanyway\n");
	printf("  -remove nameOfModule	...\n");
	printf("  -print  nameOfModule  ...\n");
	printf("  -resetwritinglocks nameOfModule ...\n");
	printf("  -resetreadinglocks nameOfModule ...\n");
	return(0);
}

int PrintModule(int argc,char *argv[])
{
	int	i;
	int	ret;
	char	mdlName[80+1];
	char	*p;
	
	for (i = 0; i < argc; i++)
	{
		if ((ret = UnionPrintSharedMemoryModuleToFileByModuleName(argv[i],stderr)) < 0)
			printf("UnionPrintSharedMemoryModuleToFileByModuleName [%s] Error! ret = [%d]\n",argv[i],ret);
	}
	
	if (argc > 0)
		return(UnionTaskActionBeforeExit());

loop:
	memset(mdlName,0,sizeof(mdlName));
	p = UnionInput("Print>Input mdlName (quit/exit to exit)::");
	strcpy(mdlName,p);
	if (UnionIsQuit(p))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionPrintSharedMemoryModuleToFileByModuleName(mdlName,stderr)) < 0)
		printf("UnionPrintSharedMemoryModuleToFileByModuleName [%s] Error! ret = [%d]\n",mdlName,ret);
	goto loop;

}

int RemoveModule(int argc,char *argv[])
{
	int	i;
	int	ret;
	char	mdlName[80+1];
	char	*p;
	
	for (i = 0; i < argc; i++)
	{
		if (!UnionConfirm("Are you sure remove module [%s]\n",argv[i]))
			continue;
		if ((ret = UnionRemoveSharedMemoryModule(argv[i])) < 0)
			printf("UnionRemoveSharedMemoryModule [%s] Error! ret = [%d]\n",argv[i],ret);
		else
			printf("UnionRemoveSharedMemoryModule [%s] OK!\n",argv[i]);
	}

	if (argc > 0)
		return(UnionTaskActionBeforeExit());

loop:
	memset(mdlName,0,sizeof(mdlName));
	p = UnionInput("Print>Input mdlName (quit/exit to exit)::");
	strcpy(mdlName,p);
	if (UnionIsQuit(p))
		return(UnionTaskActionBeforeExit());
	if (!UnionConfirm("Are you remove module [%s]\n",mdlName))
		goto loop;
	if ((ret = UnionRemoveSharedMemoryModule(argv[i])) < 0)
		printf("UnionRemoveSharedMemoryModule [%s] Error! ret = [%d]\n",mdlName,ret);
	else
		printf("UnionRemoveSharedMemoryModule [%s] OK!\n",mdlName);
	goto loop;

}		

int ResetWritingLocks(int argc,char *argv[])
{
	int	i;
	int	ret;
	char	mdlName[80+1];
	char	*p;
	
	for (i = 0; i < argc; i++)
	{
		if (!UnionConfirm("Are you sure reset writing locks for module [%s]\n",argv[i]))
			continue;
		if ((ret = UnionResetWritingLocks(argv[i])) < 0)
			printf("UnionResetWritingLocks [%s] Error! ret = [%d]\n",argv[i],ret);
		else
			printf("UnionResetWritingLocks [%s] OK!\n",argv[i]);
	}

	if (argc > 0)
		return(UnionTaskActionBeforeExit());

loop:
	memset(mdlName,0,sizeof(mdlName));
	p = UnionInput("Print>Input mdlName (quit/exit to exit)::");
	strcpy(mdlName,p);
	if (UnionIsQuit(p))
		return(UnionTaskActionBeforeExit());
	if (!UnionConfirm("Are you sure reset writing locks for module [%s]\n",mdlName))
		goto loop;
	if ((ret = UnionResetWritingLocks(argv[i])) < 0)
		printf("UnionResetWritingLocks [%s] Error! ret = [%d]\n",mdlName,ret);
	else
		printf("UnionResetWritingLocks [%s] OK!\n",mdlName);
	goto loop;

}		

int ResetReadingLocks(int argc,char *argv[])
{
	int	i;
	int	ret;
	char	mdlName[80+1];
	char	*p;
	
	for (i = 0; i < argc; i++)
	{
		if (!UnionConfirm("Are you sure reset reading locks for module [%s]\n",argv[i]))
			continue;
		if ((ret = UnionResetReadingLocks(argv[i])) < 0)
			printf("UnionResetReadingLocks [%s] Error! ret = [%d]\n",argv[i],ret);
		else
			printf("UnionResetReadingLocks [%s] OK!\n",argv[i]);
	}

	if (argc > 0)
		return(UnionTaskActionBeforeExit());

loop:
	memset(mdlName,0,sizeof(mdlName));
	p = UnionInput("Print>Input mdlName (quit/exit to exit)::");
	strcpy(mdlName,p);
	if (UnionIsQuit(p))
		return(UnionTaskActionBeforeExit());
	if (!UnionConfirm("Are you sure reset reading locks for module [%s]\n",mdlName))
		goto loop;
	if ((ret = UnionResetReadingLocks(argv[i])) < 0)
		printf("UnionResetReadingLocks [%s] Error! ret = [%d]\n",mdlName,ret);
	else
		printf("UnionResetReadingLocks [%s] OK!\n",mdlName);
	goto loop;

}		
int main(int argc,char *argv[])
{
	int	ret;

	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"mngModule")) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	if (strcasecmp(argv[1],"-REMOVEALLANYWAY") == 0)
	{
		if ((ret = UnionRemoveAllSharedMemoryModule()) < 0)
			UnionPrintf("in mngModule:: UnionRemoveAllSharedMemoryModule Error! ret = [%d]\n",ret);
		else
			UnionPrintf("in mngModule:: UnionRemoveAllSharedMemoryModule OK! [%d] removed\n",ret);
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-REMOVEALL") == 0)
	{
		if (!UnionConfirm("Are you sure of remove all the module?"))
			return(UnionTaskActionBeforeExit());
		if ((ret = UnionRemoveAllSharedMemoryModule()) < 0)
			UnionPrintf("in mngModule:: UnionRemoveAllSharedMemoryModule Error! ret = [%d]\n",ret);
		else
			UnionPrintf("in mngModule:: UnionRemoveAllSharedMemoryModule OK! [%d] removed\n",ret);
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-PRINT") == 0)
		return(PrintModule(argc-2,&argv[2]));
	
	if (strcasecmp(argv[1],"-REMOVE") == 0)
		return(RemoveModule(argc-2,&argv[2]));
	
	if (strcasecmp(argv[1],"-RESETWRITINGLOCKS") == 0)
		return(ResetWritingLocks(argc-2,&argv[2]));
		
	if (strcasecmp(argv[1],"-RESETREADINGLOCKS") == 0)
		return(ResetReadingLocks(argc-2,&argv[2]));
		
	UnionHelp();
	return(UnionTaskActionBeforeExit());
}

