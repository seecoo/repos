//      Date:           2004/11/16
//      Version:        3.0

#include <stdio.h>
#include <string.h>

#include "unionVersion.h"
#include "unionREC.h"

char    pgunionAppSysName[128];

char *UnionGetApplicationSystemName()
{
	sprintf(pgunionAppSysName,"���Ͽ��� ��̬����ϵͳ 2.x");
        return(pgunionAppSysName);
}
