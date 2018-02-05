#include <stdio.h>
#include <string.h>

#include "UnionLog.h"

int UnionIsDebug()
{
	return(1);
}

long UnionGetSizeOfLogFile()
{
	return(100000);
}

int UnionGetNameOfLogFile(char *logfilename)
{
	sprintf(logfilename,"%s/log/testLog.log",getenv("HOME"));
	return(0);
}

main()
{
	UnionLog("%s %d %ld %x %s %c %012d\n",
		"God who r u?\n",
		102,
		1000001,
		102,
		"I hate you!\n",
		'O',
		1000001);
	UnionSystemErrLog("in My Home! how dare u say that!\n %s %12ld\n",
		"11112222",
		111222);
	UnionUserErrLog("in Success!\n");
	UnionNullLog("dddaddfdfaadfda\n");
}

