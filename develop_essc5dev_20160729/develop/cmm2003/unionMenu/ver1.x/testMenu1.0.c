//	Wolfgang Wang
//	2003/09/09

#include <stdio.h>
#include <string.h>

#include "unionMenu.h"

int UnionHelp()
{
	printf("unionMenu menuDefinitionFileName\n");
	printf(" where the menu DefinitionFile stored in $HOME/etc/unionCmd/menuDefinitionFileName.MNU\n");
	return(0);
}

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
	sprintf(fileName,"%s/log/unionMenu.log",getenv("HOME"));
	return(0);
}

int main(int argc,char **argv)
{
	if ((argc < 2) || ((strcmp(argv[1],"?") == 0) || (strcmp(argv[1],"help") == 0)))
		return(UnionHelp());
	
	return(UnionMenu(argv[1]));
}

	
