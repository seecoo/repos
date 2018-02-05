#include <stdio.h>
#include <string.h>

#include "UnionLog.h"
int gunionIsDebug = 1;
int gunionDebugSet = 0;
int UnionIsDebug()
{
	char	*ptr;
	if (gunionDebugSet)
		return(gunionIsDebug);
	if ((ptr = getenv("openDebug")) != NULL)
		gunionIsDebug = atoi(ptr);
	return(gunionIsDebug);
}

long UnionGetSizeOfLogFile()
{
	return(1000000);
}

int UnionGetNameOfLogFile(char *logfilename)
{
	sprintf(logfilename,"%s/unionlog.log",getenv("UNIONLOG"));
	return(0);
}

