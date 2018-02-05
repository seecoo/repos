//	Wolfgang Wang
//	2004/6/17

#include <stdio.h>
#include <string.h>

#include "unionLisence.h"

int UnionHelp(char *appName)
{
	printf("Usage:: %s command\n",appName);
	printf(" Where command as follows::\n");
	printf("  -c create a lisence registry file.\n");
	printf("  -v verify lisence.\n");
	printf("  -p print lisence.\n");
	printf("  -reload reload lisence module.\n");
	printf("  -remove remove lisence module.\n");
	return(0);
}

int main(int argc,char **argv)
{
	int	ret;
	
	if (argc < 2)
		return(UnionHelp(argv[0]));
	if (strcasecmp(argv[1],"-c")==0)
		return(UnionCreateLisenceFile());
	if (strcasecmp(argv[1],"-v")==0)
	{
		if ((ret = UnionVerifyLisenceCode()) < 0)
			printf("Verify Lisence Failure!\n");
		else
			printf("Verify Lisence OK!\n");
		return(ret);
	}		
	if (strcasecmp(argv[1],"-p")==0)
	{
		if ((ret = UnionPrintLisence()) < 0)
			printf("Print Lisence Failure!\n");
		return(ret);
	}		
	if (strcasecmp(argv[1],"-reload") == 0)
	{
		if ((ret = UnionReloadLisenceModule()) < 0)
			printf("Reload Lisence Module Failure!\n");
		else
			printf("Reload Lisence Module OK!\n");
		return(ret);
	}
	if (strcasecmp(argv[1],"-remove") == 0)
	{
		if ((ret = UnionRemoveLisenceModule()) < 0)
			printf("Remove Lisence Module Failure!\n");
		else
			printf("Remove Lisence Module OK!\n");
		return(ret);
	}
				
	UnionHelp(argv[0]);
	return(-1);
}

int UnionIsDebug()
{
	return(1);
}

long UnionGetSizeOfLogFile()
{
	return(10000000);
}

int UnionGetNameOfLogFile(char *fileName)
{
	sprintf(fileName,"%s/mngLisence.log",getenv("UNIONLOG"));
	return(0);
}
