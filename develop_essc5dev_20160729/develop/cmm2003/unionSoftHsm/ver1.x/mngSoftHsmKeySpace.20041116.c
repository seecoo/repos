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

#include "softHsmKeySpace.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 	reload the running enviroment configuration\n");
	printf("  -print [hsmGrp]\n");
	printf("   print the running enviroment configuration\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectSoftHsmKeySpace();
	UnionDisconnectTaskTBL();
	UnionDisconnectSoftHsmKeySpace();
	//return(exit(0));
	exit(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	//char	*p;
	int	i;
	
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	if ((ret = UnionConnectSoftHsmKeySpace()) < 0)
	{
		UnionPrintf("in %s:: UnionConnectSoftHsmKeySpace Failure!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"-RELOAD") == 0)
	{
		if (!UnionConfirm("Are you sure of reloading running enviroment configuration?"))
			return(-1);
		if ((ret = UnionReloadSoftHsmKeySpace()) < 0)
			UnionPrintf("in %s:: UnionReloadSoftHsmKeySpace Error! ret = [%d]\n",UnionGetApplicationName(),ret);
		else
			UnionPrintf("in %s:: UnionReloadSoftHsmKeySpace OK!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-PRINT") == 0)
	{
		if (argc == 2)
		{
			if ((ret = UnionPrintSoftHsmKeySpaceToFile(stdout)) < 0)
				UnionPrintf("in %s:: UnionPrintSoftHsmKeySpaceToFile Error! ret = [%d]\n",UnionGetApplicationName(),ret);
		}
		else
		{
			for (i = 2; i < argc; i++)
				if ((ret = UnionPrintSoftHsmKeySpaceOfSpecifiedHsmGrpToFile(argv[i],stdout)) < 0)
					UnionPrintf("in %s:: UnionPrintSoftHsmKeySpaceOfSpecifiedHsmGrpToFile Error! ret = [%d]\n",UnionGetApplicationName(),ret);
		}		
		return(UnionTaskActionBeforeExit());
	}

		
	UnionHelp();
	return(UnionTaskActionBeforeExit());
}

