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

#include "unionDBConf.h"        // add by huangbaoxin, 20081223

#include "unionUnionREC.h"

extern PUnionSharedMemoryModule	pgunionRECMDL;
extern PUnionREC		pgunionREC;
extern PUnionRECVar		pgunionRECVar;


int UnionGetConfFileNameOfREC(char *fileName)
{
	sprintf(fileName,"unionREC");
	return(0);
}

int UnionReloadREC()
{
	int		ret = 0;
	int 		iCnt = 0;
	int 		i = 0;
	TUnionUnionREC	tUnionREC[256];
	PUnionRECVar	pRECVar;

	if ((ret = UnionConnectREC()) < 0)
	{
		UnionUserErrLog("in UnionReloadREC:: UnionConnectREC!\n");
		return(ret);
	}

	if( (pgunionREC->maxNum = UnionGetMaxVarNumOfREC()) < 0 )
	{
		UnionUserErrLog("in UnionReloadREC:: UnionGetMaxVarNumOfREC\n");
		 return (errCodeRECMDL_ConfFile);
	}

	if(pgunionREC->maxNum > 256)
		pgunionREC->maxNum = 256;

	memset(tUnionREC,0,sizeof(tUnionREC));
	if( (iCnt = UnionBatchReadUnionRECRec("",tUnionREC, 256)) < 0 )
	{
		UnionUserErrLog("in UnionReloadREC:: UnionBatchReadUnionRECRec\n");
		return (iCnt);
	}

	pgunionREC->realNum = 0;
	for (i = 0; (i < iCnt) && (pgunionREC->realNum < pgunionREC->maxNum); i++)
	{
		pRECVar = pgunionRECVar + pgunionREC->realNum;
		// 读取变量名
		if (strcmp(tUnionREC[i].varName,"MaxNumOfVar") == 0)
			continue;
		if (strlen(tUnionREC[i].varName) > sizeof(pRECVar->name) - 1)
			continue;
		strcpy(pRECVar->name,tUnionREC[i].varName);
		
		// 读取变量类型
		UnionToUpperCase(tUnionREC[i].varType);
		if (strcmp(tUnionREC[i].varType,"CHAR") == 0)
			pRECVar->type = conChar;
		else if (strcmp(tUnionREC[i].varType,"STRING") == 0)
			pRECVar->type = conString;
		else if ((strcmp(tUnionREC[i].varType,"INT") == 0) || (strcmp(tUnionREC[i].varType,"INTEGER") == 0))
			pRECVar->type = conInt;
		else if (strcmp(tUnionREC[i].varType,"LONG") == 0)
			pRECVar->type = conLong;
		else if (strcmp(tUnionREC[i].varType,"SHORT") == 0)
			pRECVar->type = conShort;
		else if (strcmp(tUnionREC[i].varType,"DOUBLE") == 0)
			pRECVar->type = conDouble;
		else
		{
			UnionUserErrLog("in UnionReloadREC:: type [%s] invalid!\n",tUnionREC[i].varType);
			continue;
		}
			
		// 读取变量值
		switch (pRECVar->type)
		{
			case	conChar:
				pRECVar->value.charValue = *tUnionREC[i].varValue;
				break;
			case	conString:
				if (strlen(tUnionREC[i].varValue) > sizeof(pRECVar->value.strValue) - 1)
				{
					UnionUserErrLog("in UnionReloadREC:: [%d] string [%s] too long!\n",i,tUnionREC[i].varValue);
					continue;
				}
				strcpy(pRECVar->value.strValue,tUnionREC[i].varValue);
				break;
			case	conInt:
				pRECVar->value.intValue = atoi(tUnionREC[i].varValue);
				break;
			case	conLong:
				//UnionPrintf("in UnionReloadREC:: long = [%s]\n",p);
				pRECVar->value.longValue = atol(tUnionREC[i].varValue);
				break;
			case	conShort:
				pRECVar->value.shortValue = atoi(tUnionREC[i].varValue);
				break;
			case	conDouble:
				//UnionPrintf("in UnionReloadREC:: double = [%s]\n",p);
				pRECVar->value.doubleValue = atof(tUnionREC[i].varValue);
				break;
			default:
				UnionUserErrLog("in UnionReloadREC:: [%d] type invalid!\n",i);
				continue;
		}

		++pgunionREC->realNum;
	}

	return(pgunionREC->realNum);
}

int UnionGetMaxVarNumOfREC()
{
	char	*ptr;

	if ((ptr = getenv("maxNumOfRECVar")) == NULL)
		return(256);
	else
		return(atoi(ptr));
}	

