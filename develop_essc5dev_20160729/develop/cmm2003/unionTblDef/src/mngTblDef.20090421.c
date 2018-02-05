//	Author:		Union tech.
//	Date:		2012-05-07
//	Version:	1.0

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

#include "unionREC.h"
#include "unionTblDef.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage:: %s command\n", UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -print 		print the all table def from image \n");
	printf("  -print  tableName 	print spec table's desc from image, 'ALL' for all table. \n");
	printf("  -clear 		remove all table def from image \n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectREC();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//UnionDisconnectTblDef();
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
	
	if ((ret = UnionConnectTblDef()) < 0)
	{
		UnionPrintf("in mngTblDef:: UnionConnectTblDef Failure!\n");
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"-CLEAR") == 0)
	{
		if (!UnionConfirm("Are you sure of clearing all table def from image?"))
			return(-1);
		if ((ret = UnionRemoveTblDef()) < 0)
			UnionPrintf("in mngTblDef:: UnionRemoveTblDef Error! ret = [%d]\n",ret);
		else
			UnionPrintf("in mngTblDef:: UnionRemoveTblDef OK!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (argc == 2 && strcasecmp(argv[1],"-PRINT" ) == 0)
	{
		if ((ret = UnionPrintTblDefToFile(stdout)) < 0)
			UnionPrintf("in mngTblDef:: UnionPrintTblDefToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}

	if (argc > 2 && strcasecmp(argv[1],"-PRINT") == 0)
	{
		if ((ret = UnionPrintDetailTblDefToFile(stdout, argv[2])) < 0)
			UnionPrintf("in mngTblDef:: UnionPrintDetailTblDefToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
		
	UnionHelp();
	return(UnionTaskActionBeforeExit());
}

