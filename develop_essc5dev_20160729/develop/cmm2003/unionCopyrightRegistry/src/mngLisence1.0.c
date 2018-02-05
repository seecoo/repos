//	Wolfgang Wang
//	2003/1/19

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionCopyrightRegistry.h"

int UnionIsDebug()
{
	return(1);
}

long UnionGetSizeOfLogFile()
{
	return(1000000);
}

int UnionGetNameOfLogFile(char *fileName)
{
	sprintf(fileName,"%s/log/mngLisence.log",getenv("HOME"));
	return(0);
}

int UnionHelp()
{
	printf("Usage:: mngLisence1.x command [Arguments]\n");
	printf(" Where command as follow:\n");
	printf("	-g abbrProductName	Generate a lisence configuration file\n");
	printf("	which will be stored in $HOME/UnionProductRegistry/abbrProductName\n");
	printf("	with a name of LisenceYYYYMMDDHHMMSS.CFG, where YYYYMMDDHHMMSS is\n");
	printf("	the date and time when this file is created.\n");
	printf("	-v abbrProductName	Verify the lisence, where the lisence information is stored in the file\n");
	printf("	of a name UnionProductRegistry.CFG, which is stored in the directory of\n");
	printf("	$HOME/etc.\n");
	return(1);
}


int main(int argc,char **argv)
{
	int	ret;
	
	if (argc < 3)
		return(UnionHelp());
	
	ret = 200;
	if (strcasecmp(argv[1],"-g") == 0)
	{
		if ((ret = UnionGenerateUserCopyrightRegistry(argv[2])) < 0)
			printf("UnionGenerateUserCopyrightRegistry Failure! ret = [%d]\n",ret);
		else
			printf("UnionGenerateUserCopyrightRegistry OK!\n");
	}
	if (strcasecmp(argv[1],"-v") == 0)
	{
		if ((ret = UnionVerifyUserCopyrightRegistry(argv[2])) < 0)
			printf("UnionVerifyUserCopyrightRegistry Failure! ret = [%d]\n",ret);
		else
			printf("UnionVerifyUserCopyrightRegistry OK!\n");
	}
	
	if (ret == 200)
		goto wrongParameter;
	return(ret);

wrongParameter:
	UnionHelp();
	return(ret);
}
