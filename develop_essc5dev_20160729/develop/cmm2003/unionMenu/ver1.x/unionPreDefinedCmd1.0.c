//	Wolfgang Wang
//	2003/09/09

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "UnionStr.h"
#include "unionCommand.h"
#include "unionVersion.h"

void UnionHelp();

int UnionExcutePreDefinedCommand(int argc,char *argv[])
{
	if (argc <= 0)
		return(0);
	
	UnionToUpperCase(argv[0]);

	if ((strcasecmp(argv[0],"HELP") == 0) || (strcasecmp(argv[0],"-?") == 0))
	{
		UnionHelp();
		return(1);
	}

	if (strcasecmp(argv[0],"VERSION") == 0)
	{
		UnionPrintProductVersionToFile(stderr);
		return(1);
	}
	
	return(0);
}

int UnionExcutePreDefinedCommand2(int argc,char *argv[])
{
	if (argc <= 0)
		return(0);
	
	//UnionToUpperCase(argv[0]);

	if ((strcasecmp(argv[0],"HELP") == 0) || (strcasecmp(argv[0],"-?") == 0))
	{
		UnionHelp();
		return(1);
	}

	if (strcasecmp(argv[0],"VERSION") == 0)
	{
		UnionPrintProductVersionToFile(stderr);
		return(1);
	}
	
	return(0);
}