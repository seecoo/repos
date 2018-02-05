//	Author:		’≈”¿∂®
//	Date:		2011-11-15
//	Version:	1.0

#include <stdio.h>
#include <string.h>

#include "unionVersion.h"

char	gunionGetDataBaseType[32];

char *UnionGetDataBaseType()
{
	memset(gunionGetDataBaseType,0,sizeof(gunionGetDataBaseType));
	strcpy(gunionGetDataBaseType,"ORACLE");
	return(gunionGetDataBaseType);
}
char *UnionGetDLDataBaseType()
{
	memset(gunionGetDataBaseType,0,sizeof(gunionGetDataBaseType));
	strcpy(gunionGetDataBaseType,"ORACLE");
	return(gunionGetDataBaseType);
}
