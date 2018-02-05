#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char gunionPredefinedProductVersionNumber[20+1];
char gunionProductGeneratedTime[20+1];


long UnionReadPredefinedProductEffectiveDays()
{
	return(36500);
}

char *UnionReadPredfinedProductVersionNumber()
{
	memset(gunionPredefinedProductVersionNumber,0,sizeof(gunionPredefinedProductVersionNumber));
	strcpy(gunionPredefinedProductVersionNumber,"1.0-aix-nodb-64bits");
	return(gunionPredefinedProductVersionNumber);
}

char *UnionGetProductGeneratedTime()
{
	memset(gunionProductGeneratedTime,0,sizeof(gunionProductGeneratedTime));
	strcpy(gunionProductGeneratedTime,"20110101163911");
	return(gunionProductGeneratedTime);
}
