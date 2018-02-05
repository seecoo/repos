//	Wolfgang Wang
//	2003/09/09

#define _UnionTask_3_x_	

#include <stdio.h>
#include <string.h>

#include "unionMenu.h"
#include "UnionTask.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("%s menuDefinitionFileName\n",UnionGetApplicationName());
	printf(" where the menu DefinitionFile stored in $UNIONETC/unionCmd/menuDefinitionFileName.MNU\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int main(int argc,char **argv)
{
	int	ret;
	int	seconds;
	char	menuName[40+1];
	
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
	if (strlen(argv[1]) > sizeof(menuName))
	{
		printf("menuName too long!\n");
		return(-1);
	}
	strcpy(menuName,argv[1]);
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,argv[0])) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	//printf("ptaskInstance = [%x]\n",ptaskInstance);
	
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	UnionMenu(menuName);
	
	return(UnionTaskActionBeforeExit());
}

	
