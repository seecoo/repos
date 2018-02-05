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

#include "unionCardBinTBL.h"
#include "unionErrCodeTBL.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 		reload the transDefTBL\n");
	printf("  -all			print all transDef definition\n");
	printf("  cardBin\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectCardBinTBL();
	UnionDisconnectTaskTBL();
	exit(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	char	*p;
		
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
		if (!UnionConfirm("Are you sure of reloading cardBinTBL?"))
			return(-1);
		if ((ret = UnionReloadCardBinTBL()) < 0)
			printf("UnionReloadCardBinTBL Error! ret = [%d]\n",ret);
		else
			printf("UnionReloadCardBinTBL OK!\n");
	}
	else if (strcasecmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if ((ret = UnionReloadCardBinTBL()) < 0)
			printf("UnionReloadCardBinTBL Error! ret = [%d]\n",ret);
		else
			printf("UnionReloadCardBinTBL OK!\n");
	}
	else if (strcasecmp(argv[1],"-ALL") == 0)
		UnionPrintCardBinTBLToFile(stdout);
	else
		UnionHelp();

	return(UnionTaskActionBeforeExit());
}
