//	Author:		Wolfgang Wang
//	Date:		2003/09/20
//	Version:	1.0

// 2004/11/25, Wolfgang Wang 增加了函数
// UnionVerifyProductVersion

#include <stdio.h>
#include <string.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "unionVersion.h"
#ifdef _verifyLiscence_
#include "unionLisence.h"
#endif

TUnionVersion	gunionVersion;
char		gunionApplicationName[128] = "unknown";
int		gunionIsValidLiscence = 0;

// Added by Wolfang Wang, 2004/11/25
int UnionIsProductStillEffective()
{
	//char	date[20];

#ifdef _verifyLiscence_		// 2008/7/16，王纯军增加
	int	ret;

	if (gunionIsValidLiscence <= 0)
	{	
		if ((ret = UnionVerifyLisenceKeyUseSpecInputData(NULL,NULL,NULL)) <= 0)
		{
			printf("fatal error:: invalid liscence code!\n");
			return(0);
		}
		gunionIsValidLiscence = 1;
	}
#endif	// 2008/7/16，王纯军增加结束
	// 2014-01-22 张永定删除
	/*
	strcpy(date,UnionGetProductGeneratedTime());
	date[8] = 0;
	//printf("%ld %ld\n",UnionDecideDaysBeforeToday(date),UnionReadPredefinedProductEffectiveDays());
	if ((UnionDecideDaysBeforeToday(date) < 0) || (UnionDecideDaysBeforeToday(date) > UnionReadPredefinedProductEffectiveDays()))
		return(0);
	else
	*/
		return(1);
}
// Added by Wolfang Wang, 2004/11/25	
long UnionGetStillEffectiveDaysOfProduct()
{
	char	date[20];

	strcpy(date,UnionGetProductGeneratedTime());
	date[8] = 0;
	
	return(UnionReadPredefinedProductEffectiveDays() - UnionDecideDaysBeforeToday(date));
}

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

/*
int UnionSetProductDate(char *date)
{
	if (date == NULL)
		return(errCodeParameter);
	if (strlen(date) > sizeof(gunionVersion.date))
		return(errCodeParameter);
	memset(gunionVersion.date,0,sizeof(gunionVersion.date));
	strcpy(gunionVersion.date,date);
	return(0);
}
int UnionReadProductDate(char *date)
{
	if (date == NULL)
		return(errCodeParameter);
	strcpy(date,gunionVersion.date);
	return(0);
}

int UnionSetProductAuthor(char *author)
{
	if (author == NULL)
		return(errCodeParameter);
	if (strlen(author) > sizeof(gunionVersion.author))
		return(errCodeParameter);
	memset(gunionVersion.author,0,sizeof(gunionVersion.author));
	strcpy(gunionVersion.author,author);
	return(0);
}
int UnionReadProductAuthor(char *author)
{
	if (author == NULL)
		return(errCodeParameter);
	strcpy(author,gunionVersion.author);
	return(0);
}
*/

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
	fprintf(fp,"	ExcutableName      %s\n",UnionGetApplicationName());
	fprintf(fp,"	VersionNumber	   %s\n",UnionReadPredfinedProductVersionNumber());
	fprintf(fp,"	Generated Time     %s\n",UnionGetProductGeneratedTime());
	// 2011-11-14 张永定增加
	fprintf(fp,"	DataBase Type      %s\n",UnionGetDataBaseType());
	// 2011-11-14 End
	// 2014-01-22 张永定删除
	//fprintf(fp,"	Maximum Effective Days %ld\n",UnionReadPredefinedProductEffectiveDays());
	//fprintf(fp,"	Still left %ld days effective\n",UnionGetStillEffectiveDaysOfProduct());
	fprintf(fp,"\n");
	fprintf(fp,"	Union Tech. Limited. GuangZhou\n");
	fprintf(fp,"	Tel: 400-008-0466\n");
	fprintf(fp,"	http://www.keyou.cn\n");
	fprintf(fp,"\n");	
	return(0);
}
	
