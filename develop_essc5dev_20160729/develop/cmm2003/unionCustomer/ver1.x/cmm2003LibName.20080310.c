//      Date:           2004/11/16
//      Version:        3.0

#include <stdio.h>
#include <string.h>

#include "unionVersion.h"
#include "unionREC.h"

char    pgunionAppSysName[128];

char *UnionGetApplicationSystemName()
{
	char	*ptr = NULL;
	char	nameOfMyself[64+1];
	char	nameOfSystem[64+1];

	if (UnionReadStringTypeRECVar("nameOfMyself") == NULL)
		UnionReloadREC();

	memset(nameOfMyself,0,sizeof(nameOfMyself));
	if ((ptr = UnionReadStringTypeRECVar("nameOfMyself")) != NULL)
		strcpy(nameOfMyself,ptr);
	
	memset(nameOfSystem,0,sizeof(nameOfSystem));
	if ((ptr = UnionReadStringTypeRECVar("nameOfSystem")) != NULL)
		strcpy(nameOfSystem,ptr);
	
	sprintf(pgunionAppSysName,"%s %s",nameOfMyself,nameOfSystem);
        return(pgunionAppSysName);
}
