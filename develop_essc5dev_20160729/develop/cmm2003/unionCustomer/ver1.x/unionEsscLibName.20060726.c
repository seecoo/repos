//	Author:		Wolfgang Wang
//	Date:		2004/11/16
//	Version:	3.0

#include <stdio.h>
#include <string.h>

#include "unionVersion.h"
#include "unionREC.h"

char	pgunionAppSysName[128];

char *UnionGetApplicationSystemName()
{
	sprintf(pgunionAppSysName,"%s 密码服务系统 3.x",UnionReadStringTypeRECVar("nameOfMyself"));
	return(pgunionAppSysName);
}
