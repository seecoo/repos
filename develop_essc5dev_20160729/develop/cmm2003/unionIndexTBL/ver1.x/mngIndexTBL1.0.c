//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	2.0

#define _UnionLogMDL_3_x_
#define _UnionTask_3_x_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"

#include "unionIndexTBL.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  remove userID	remove the index table\n");
	printf("  removeanyway userID	remove the index table\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	// UnionDisconnectIndexStatusTBL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int RemoveIndexTBL(int argc,char *argv[])
{
	int	ret;
	int	i;
	
	for (i = 0; i < argc; i++)
	{
		if (!UnionConfirm("Are you sure of delete IndexTBL [%s]?",argv[i]))
			continue;
		if ((ret = UnionRemoveIndexStatusTBL(atoi(argv[i]))) < 0)
			printf("Delete Index Table [%s] Error! ret = [%d]\n",argv[i],ret);
		else
			printf("Delete Index Table [%s] OK!\n",argv[i]);
	}
	return(UnionTaskActionBeforeExit());
}

int RemoveIndexTBLAnyway(int argc,char *argv[])
{
	int	ret;
	int	i;
	
	for (i = 0; i < argc; i++)
	{
		if ((ret = UnionRemoveIndexStatusTBL(atoi(argv[i]))) < 0)
			printf("Delete Index Table [%s] Error! ret = [%d]\n",argv[i],ret);
		else
			printf("Delete Index Table [%s] OK!\n",argv[i]);
	}
	return(UnionTaskActionBeforeExit());
}
	
int main(int argc,char *argv[])
{

	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"mngIndexTBL")) == NULL)
	{
		UnionPrintf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	
	if (strcasecmp(argv[1],"REMOVE") == 0)
		return(RemoveIndexTBL(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"REMOVEANYWAY") == 0)
		return(RemoveIndexTBLAnyway(argc-2,&argv[2]));
	UnionHelp();

	return(UnionTaskActionBeforeExit());
}

