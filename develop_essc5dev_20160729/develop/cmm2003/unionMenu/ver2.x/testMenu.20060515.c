//	Wolfgang Wang
//	2003/09/09

#define _UnionTask_3_x_	

#include <stdio.h>
#include <string.h>

#include "unionMenu.h"
#include "UnionLog.h"

int UnionHelp()
{
	//printf("%s menuDefinitionFileName\n",UnionGetApplicationName());
	printf("unionMenu menuDefinitionFileName\n");
	printf(" where the menu DefinitionFile stored in $UNIONETC/unionCmd/menuDefinitionFileName.MNU\n");
	return(0);
}
/*
void UnionLog(char fmt,...)
{
	return;
}

void UnionDebugLog(char fmt,...)
{
	return;
}

void UnionUserErrLog(char fmt,...)
{
	return;
}

void UnionSystemErrLog(char fmt,...)
{
	return;
}

void UnionNullLog(char fmt,...)
{
	return;
}
*/
int main(int argc,char **argv)
{
	int	ret;
	int	seconds;
	char	menuName[40+1];
	
	//UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
	/*
	if (strcmp("version",argv[1]) == 0)
	{
		UnionPrintProductVersionToFile(stderr);
		return(0);
	}
	*/
	if (strlen(argv[1]) > sizeof(menuName))
	{
		printf("menuName too long!\n");
		return(-1);
	}
	strcpy(menuName,argv[1]);
	
	UnionMenu(menuName);
	
	return(0);
}

	
