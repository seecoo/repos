//	Wolfgang Wang
//	2003/9/23

#define _UnionTask_3_x_	

#include <stdio.h>
#include <string.h>

#include "unionCopyrightRegistry.h"
#include "UnionTask.h"
#include "unionCommand.h"
#include "unionVersion.h"

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
	printf("Usage:: %s command [Arguments]\n",UnionGetApplicationName());
	printf(" Where command as follow:\n");
	printf(" -g abbrProductName	Generate a lisence configuration file\n");
	printf("	which will be stored in $UNIONPRODUCTREGISTRY/abbrProductName\n");
	printf("	with a name of LisenceYYYYMMDDHHMMSS.CFG, where YYYYMMDDHHMMSS is\n");
	printf("	the date and time when this file is created.\n");
	printf("	this command use a CFGFile name abbrProductName.Lisence.INI stored\n");
	printf("	in the directory $UNIONPRODUCTREGISTRY/abbrProductName\n");
	printf(" -v abbrProductName	Verify the lisence, where the lisence information is stored in the file\n");
	printf("	of a name unionRegistry.CFG, which is stored in the directory of\n");
	printf("	$UNIONETC.\n");
	return(1);
}


int main(int argc,char **argv)
{
	char	productName[80+1];
	int	ret;
	
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());

	if (argc >= 3)
	{	
		memset(productName,0,sizeof(productName));
		strcpy(productName,argv[2]);
	}
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"mngLisence")) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	//printf("ptaskInstance = [%x]\n",ptaskInstance);

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());

	if (argc < 3)
	{
		UnionHelp();
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"-G") == 0)
	{
loopGen:
		if ((ret = UnionGenerateUserCopyrightRegistry(productName)) < 0)
			printf("UnionGenerateUserCopyrightRegistry Failure! ret = [%d]\n",ret);
		else
			printf("UnionGenerateUserCopyrightRegistry OK!\n");
		if (UnionConfirm("Create another?"))
			goto loopGen;
		return(UnionTaskActionBeforeExit());		
	}
	if (strcasecmp(argv[1],"-V") == 0)
	{
loopVerify:
		if ((ret = UnionVerifyUserCopyrightRegistry(productName)) < 0)
			printf("UnionVerifyUserCopyrightRegistry Failure! ret = [%d]\n",ret);
		else
			printf("UnionVerifyUserCopyrightRegistry OK!\n");
		if (UnionConfirm("Verify another?"))
			goto loopVerify;
		return(UnionTaskActionBeforeExit());		
	}
	
	UnionHelp();
	return(UnionTaskActionBeforeExit());
}
