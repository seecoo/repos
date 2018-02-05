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
	//char	date[32];

#ifdef _verifyLiscence_		// 2008/7/16，王纯军增加
	int	ret;
	char	maintainDate[8+1];
	char	endDate[8+1];
	
	if (gunionIsValidLiscence <= 0)
	{	
		/*
		if ((ret = UnionVerifyLisenceKeyUseSpecInputData(NULL,NULL,NULL)) <= 0)
		{
			printf("fatal error:: invalid liscence code!\n");
			return(0);
		}
		*/
		
		// 20120919 add
		//	=2		校验成功
		//	=1		到维护期
		//	=0		校验失败
		//	=-1		到截止使用日期
		//	<-1		出错代码
		memset(maintainDate,0,sizeof(maintainDate));
		memset(endDate,0,sizeof(endDate));
		ret = UnionVerifyFinalLisenceKeyUseSpecInputData(NULL,NULL,maintainDate,endDate,NULL);
		if (ret == 1)		// 到维护期了
		{
			printf("授权维护到期日期[%s]，已过期，请获取新的授权码!\n",maintainDate);
			gunionIsValidLiscence = 1;
		}
		else if (ret == -1)	// 到使用截止日期
		{
			printf("授权使用到期日期[%s]，已过期，请获取新的授权码!\n",endDate);
			return(0);
		}
		else if (ret == 2)	// 验证成功
		{
			gunionIsValidLiscence = 1;
		}
		else
		{
			printf("授权码验证失败，请获取新的授权码!\n");
			return(0);
		}
		// 20120919 end
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
/*
long UnionGetStillEffectiveDaysOfProduct()
{
	char	date[32];

	strcpy(date,UnionGetProductGeneratedTime());
	date[8] = 0;
	
	return(UnionReadPredefinedProductEffectiveDays() - UnionDecideDaysBeforeToday(date));
}
*/

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
	char	*pTime = NULL;
	
	if (fp == NULL)
		return(errCodeParameter);
	//fprintf(fp,"Name	%s\n",gunionVersion.name);
	//fprintf(fp,"Version	%s\n",gunionVersion.version);
	//fprintf(fp,"Author	%s\n",gunionVersion.author);
	//fprintf(fp,"Date	%s\n",gunionVersion.date);
	fprintf(fp,"\n");
	fprintf(fp,"	系统名称:        %s\n",UnionGetApplicationSystemName());
	//fprintf(fp,"	版本号:          %s\n",UnionReadPredfinedProductVersionNumber());
	fprintf(fp,"	程序名:          %s\n",UnionGetApplicationName());
	pTime = UnionGetProductGeneratedTime();
	fprintf(fp,"	生成时间:        %.4s-%.2s-%.2s %.2s:%.2s:%.2s\n",pTime,pTime+4,pTime+6,pTime+8,pTime+10,pTime+12);
	//fprintf(fp,"	生成时间:        %s %s\n",__DATE__,__TIME__);
	// 2011-11-14 张永定增加
	fprintf(fp,"	数据库:          %s\n",UnionGetDataBaseType());
	// 2011-11-14 End
	// 2014-01-22 张永定删除
	//fprintf(fp,"	最大有效期(天):  %ld\n",UnionReadPredefinedProductEffectiveDays());
	//fprintf(fp,"	剩余有效期(天):  %ld \n",UnionGetStillEffectiveDaysOfProduct());
	fprintf(fp,"\n");
	fprintf(fp,"	版权所有:        Copyright 江南科友\n");
	fprintf(fp,"	服务热线:        400-008-0466\n");
	fprintf(fp,"	公司网址:        http://www.keyou.cn\n");
	fprintf(fp,"\n");	
	return(0);
}
	
