//	Author:		Wolfgang Wang
//	Date:		2003/10/10
//	Version:	1.0

// 2005/9/16修改了UnionReadStringTypeRECVar
// 使任何类型的变量都能作为字符串类型读出
// 增加了UnionUpdateRECVarAsString，使任何类型的变量都接受字符串类型的赋值

#define _UnionLogMDL_3_x_	
#define _UnionEnv_3_x_	

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
//#include "unionCommand.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "unionWorkingDir.h"
#include "unionRecFile.h"

#ifndef _WIN32
#include "unionREC.h"
#else
#include <process.h>
#include "unionRECVar.h"
#endif

extern PUnionSharedMemoryModule	pgunionRECMDL;
extern PUnionREC		pgunionREC;
extern PUnionRECVar		pgunionRECVar;

int UnionGetConfFileNameOfREC(char *fileName)
{
#ifdef _WIN32
        char    mainDir[512+1];
        memset(mainDir,0,sizeof(mainDir));
        UnionGetMainWorkingDir(mainDir);
	sprintf(fileName,"%s/unionREC.CFG",mainDir);
#else
	sprintf(fileName,"%s/unionREC.CFG",getenv("UNIONETC"));
#endif
	return(0);
}

int UnionReloadREC()
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		i;
	PUnionRECVar	pRECVar;
	
	if ((ret = UnionConnectREC()) < 0)
	{
		UnionUserErrLog("in UnionReloadREC:: UnionConnectREC!\n");
		return(ret);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetConfFileNameOfREC(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionReloadREC:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("MaxNumOfVar")) == NULL)
	{
		UnionUserErrLog("in UnionReloadREC:: UnionGetEnviVarByName for [%s]\n!","MaxNumOfVar");
		UnionClearEnvi();
		return(errCodeRECMDL_ConfFile);
	}
	pgunionREC->maxNum = atoi(p);
	pgunionREC->realNum = 0;
		
	for (i = 0; (i < UnionGetEnviVarNum()) && (pgunionREC->realNum < pgunionREC->maxNum); i++)
	{
		pRECVar = pgunionRECVar + pgunionREC->realNum;
		// 读取变量名
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
		{
			UnionUserErrLog("in UnionReloadREC:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,0);
			continue;
		}
		if (strcmp(p,"MaxNumOfVar") == 0)
			continue;
		if (strlen(p) > sizeof(pRECVar->name) - 1)
			continue;
		strcpy(pRECVar->name,p);
		
		// 读取变量类型
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
		{
			UnionUserErrLog("in UnionReloadREC:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
			continue;
		}
		UnionToUpperCase(p);
		if (strcmp(p,"CHAR") == 0)
			pRECVar->type = conChar;
		else if (strcmp(p,"STRING") == 0)
			pRECVar->type = conString;
		else if ((strcmp(p,"INT") == 0) || (strcmp(p,"INTEGER") == 0))
			pRECVar->type = conInt;
		else if (strcmp(p,"LONG") == 0)
			pRECVar->type = conLong;
		else if (strcmp(p,"SHORT") == 0)
			pRECVar->type = conShort;
		else if (strcmp(p,"DOUBLE") == 0)
			pRECVar->type = conDouble;
		else
		{
			UnionUserErrLog("in UnionReloadREC:: type [%s] invalid!\n",p);
			continue;
		}
			
		// 读取变量值
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,2)) == NULL)
		{
			UnionUserErrLog("in UnionReloadREC:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,2);
			continue;
		}
		switch (pRECVar->type)
		{
			case	conChar:
				pRECVar->value.charValue = *p;
				break;
			case	conString:
				if (strlen(p) > sizeof(pRECVar->value.strValue) - 1)
				{
					UnionUserErrLog("in UnionReloadREC:: [%d] string [%s] too long!\n",i,p);
					continue;
				}
				strcpy(pRECVar->value.strValue,p);
				break;
			case	conInt:
				pRECVar->value.intValue = atoi(p);
				break;
			case	conLong:
				//UnionPrintf("in UnionReloadREC:: long = [%s]\n",p);
				pRECVar->value.longValue = atol(p);
				break;
			case	conShort:
				pRECVar->value.shortValue = atoi(p);
				break;
			case	conDouble:
				//UnionPrintf("in UnionReloadREC:: double = [%s]\n",p);
				pRECVar->value.doubleValue = atof(p);
				break;
			default:
				UnionUserErrLog("in UnionReloadREC:: [%d] type invalid!\n",i);
				continue;
		}
		
		++pgunionREC->realNum;
	}
	
	UnionClearEnvi();
		
	return(0);
}

int UnionGetMaxVarNumOfREC()
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		num;
		
	memset(fileName,0,sizeof(fileName));
	UnionGetConfFileNameOfREC(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionGetMaxVarNumOfREC:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("MaxNumOfVar")) == NULL)
	{
		UnionUserErrLog("in UnionGetMaxVarNumOfREC:: UnionGetEnviVarByName for [%s]\n!","MaxNumOfVar");
		num = -1;
	}
	else
		num = atoi(p);
	
	UnionClearEnvi();

	return(num);	
}

