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

// Mary add begin, 2004-2-3
int UnionBackupLogFileBeforeRewriteIt()
{
	return(0);
}
// Mary add end, 2004-2-3

main()
{
	UnionSystemErrLog("in My Home! how dare u say that!\n %s %12ld\n",
		"11112222",
		111222);
	UnionUserErrLog("in Success!\n");
	UnionMemErrLog("Test Memory Error Log:",(unsigned char *)"87614ytyrjwebgvuydsfier7yfi",27);
	UnionLog("%s %d %ld %x %s %c %012d\n",
		"God who r u?\n",
		102,
		1000001,
		102,
		"I hate you!\n",
		'O',
		1000001);
	UnionNullLog("dddaddfdfaadfda\n");
}

