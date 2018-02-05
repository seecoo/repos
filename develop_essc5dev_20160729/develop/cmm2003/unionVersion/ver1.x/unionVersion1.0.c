//	Author:		Wolfgang Wang
//	Date:		2003/09/20
//	Version:	1.0

#include <stdio.h>
#include <string.h>

#include "unionVersion.h"
#include "unionErrCode.h"
#include "UnionLog.h"

TUnionVersion	gunionVersion;
char		gunionApplicationName[128] = "unknown";

int UnionSetApplicationName(char *appName)
{
	memset(gunionApplicationName,0,sizeof(gunionApplicationName));
	strcpy(gunionApplicationName,appName);
	return(0);
}

char *UnionGetApplicationName()
{
	return(gunionApplicationName);
}

int UnionSetProductVersion(char *name,char *version)
{
	int	ret;
	
	if ((ret = UnionSetProductName(name)) < 0)
		return(ret);
	if ((ret = UnionSetProductVersionNumber(version)) < 0)
		return(ret);
	return(ret);
}

int UnionSetProductName(char *name)
{
	if (name == NULL)
		return(errCodeParameter);
	if (strlen(name) > sizeof(gunionVersion.name))
		return(errCodeParameter);
	memset(gunionVersion.name,0,sizeof(gunionVersion.name));
	strcpy(gunionVersion.name,name);
	//UnionLog("in UnionSetProductName:: product Name = [%s]\n",gunionVersion.name);
	return(0);
}
int UnionReadProductName(char *name)
{
	if (name == NULL)
		return(errCodeParameter);
	strcpy(name,gunionVersion.name);
	return(0);
}

int UnionSetProductVersionNumber(char *version)
{
	if (version == NULL)
		return(errCodeParameter);
	if (strlen(version) > sizeof(gunionVersion.version))
		return(errCodeParameter);
	memset(gunionVersion.version,0,sizeof(gunionVersion.version));
	strcpy(gunionVersion.version,version);
	//UnionLog("in UnionSetProductVersionNumber:: versionNumber = [%s]\n",gunionVersion.version);
	return(0);
}
int UnionReadProductVersionNumber(char *version)
{
	if (version == NULL)
		return(errCodeParameter);
	strcpy(version,gunionVersion.version);
	return(0);
}


int UnionPrintProductVersionToFile(FILE *fp)
{
	if (fp == NULL)
		return(errCodeParameter);
	//fprintf(fp,"Name	%s\n",gunionVersion.name);
	//fprintf(fp,"Version	%s\n",gunionVersion.version);
	//fprintf(fp,"Author	%s\n",gunionVersion.author);
	//fprintf(fp,"Date	%s\n",gunionVersion.date);
	fprintf(fp,"\n");
	fprintf(fp,"	Application System %s\n",UnionGetApplicationSystemName());
	fprintf(fp,"	ExcutableName      %s\n",gunionVersion.name);
	fprintf(fp,"	VersionNumber	   %s\n",gunionVersion.version);
	fprintf(fp,"	Generated Time     %s\n",UnionGetProductGeneratedTime());
	fprintf(fp,"\n");
	fprintf(fp,"	Union Tech. Limited. GuangZhou\n");
	fprintf(fp,"	Tel: 020 8553 3289\n");
	fprintf(fp,"	Fax: 020 8553 0160\n");
	fprintf(fp,"	email:union@union-tech.net.cn\n");
	fprintf(fp,"	http://www.union-tech.net.cn\n");
	fprintf(fp,"\n");	
	return(0);
}
	
