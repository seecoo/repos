//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	1.0

#define _UnionLogMDL_3_x_	

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"

PUnionLogFile	plogFile = NULL;

int InitResource()
{
	int	ret;
	
	if ((ret = UnionConnectLogFileTBL()) < 0)
	{
		printf("UnionConnectLogFileTBL Error! ret = [%d]\n",ret);
		return(ret);
	}
	plogFile = UnionConnectLogFile("mngUnionLog");
	//UnionSetProductName("mngUnionLog");
	//UnionResetProductVersionNumber();
	UnionSetProductVersion("mngUnionLog",UnionReadPredfinedProductVersionNumber());
	return(0);
}

int ReleaseResource()
{
	UnionDisconnectLogFile(plogFile);
	UnionDisconnectLogFileTBL();
	return(0);
}

PUnionLogFile UnionGetLogFile()
{
	return(plogFile);
}
	
int UnionHelp()
{
	printf("mngUnionLog command\n");
	printf(" Where command as:\n");
	printf("  -xa		remove the logfiletbl\n");
	printf("  -reload 	reload the logfiletbl\n");
	printf("  -add 		add a logfile\n");
	printf("  -delete 	delete a logfile\n");
	printf("  -print 	print the logfiletbl\n");
	return(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	char	logFileName[80];
	char	*p;

	if (argc < 2)
		return(UnionHelp());
		
	if ((ret = InitResource()) < 0)
		return(ret);

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);
	
	if (strcasecmp(argv[1],"-XA") == 0)
	{
		if (!UnionConfirm("Are you sure of removing logFileTBL?"))
			return(-1);
		if ((ret = UnionRemoveLogFileTBL()) < 0)
			UnionPrintf("in mngUnionLog:: UnionRemoveLogFileTBL Error! ret = [%d]\n",ret);
		else
			UnionPrintf("in mngUnionLog:: UnionRemoveLogFileTBL OK!\n");
		return(ret);
	}

	if (strcasecmp(argv[1],"-RELOAD") == 0)
	{
		if (!UnionConfirm("Are you sure of reloading logFileTBL?"))
			return(-1);
		if ((ret = UnionReloadLogFileTBL()) < 0)
			UnionPrintf("in mngUnionLog:: UnionReloadLogFileTBL Error! ret = [%d]\n",ret);
		else
			UnionPrintf("in mngUnionLog:: UnionReloadLogFileTBL OK!\n");
		return(ReleaseResource());
	}

	if (strcasecmp(argv[1],"-PRINT") == 0)
	{
		if ((ret = UnionPrintLogFileTBL()) < 0)
			UnionPrintf("in mngUnionLog:: UnionPrintLogFileTBL Error! ret = [%d]\n",ret);
		return(ReleaseResource());
	}

	if (strcasecmp(argv[1],"-ADD") == 0)
	{
loopAdd:
		memset(logFileName,0,sizeof(logFileName));
		p = UnionInput("Add>Input logFileName (quit/exit to exit)::");
		strcpy(logFileName,p);
		if (UnionIsQuit(p))
			return(ReleaseResource());
		if ((ret = UnionAddLogFile(logFileName)) < 0)
			UnionPrintf("in mngUnionLog:: UnionAddLogFile [%s] Error! ret = [%d]\n",logFileName,ret);
		else
			UnionPrintf("in mngUnionLog:: UnionAddLogFile [%s] OK!\n",logFileName);
		goto loopAdd;
	}

	if (strcasecmp(argv[1],"-DELETE") == 0)
	{
loopDelete:
		memset(logFileName,0,sizeof(logFileName));
		p = UnionInput("Delete>Input logFileName (quit/exit to exit)::");
		strcpy(logFileName,p);
		if (UnionIsQuit(p))
			return(ReleaseResource());
		if ((ret = UnionDeleteLogFile(logFileName)) < 0)
			UnionPrintf("in mngUnionLog:: UnionDeleteLogFile [%s] Error! ret = [%d]\n",logFileName,ret);
		else
			UnionPrintf("in mngUnionLog:: UnionDeleteLogFile [%s] OK!\n",logFileName);
		goto loopDelete;
	}

	ReleaseResource();
	return(UnionHelp());
}

		
