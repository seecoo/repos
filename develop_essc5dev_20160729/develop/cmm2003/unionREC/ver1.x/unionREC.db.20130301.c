//	Author:		Wolfgang Wang
//	Date:		2003/10/10
//	Version:	1.0

// 2005/9/16修改了UnionReadStringTypeRECVar
// 使任何类型的变量都能作为字符串类型读出
// 增加了UnionUpdateRECVarAsString，使任何类型的变量都接受字符串类型的赋值

#define _UnionLogMDL_3_x_	
#define _UnionEnv_3_x_	
#define _realBaseDB_2_x_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
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

#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"

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
	//char		tmpBuf[128];
	char		varSysName[64];
	char		varType[32];
	char		varValue[128];
	char		sql[128];;
	PUnionRECVar	pRECVar;

	if ((ret = UnionConnectREC()) < 0)
	{
		UnionUserErrLog("in UnionReloadREC:: UnionConnectREC!\n");
		return(ret);
	}

	if( (pgunionREC->maxNum = UnionGetMaxVarNumOfREC()) < 0 )
	{
		UnionUserErrLog("in UnionReloadREC:: UnionGetMaxVarNumOfREC\n");
		return(errCodeRECMDL_ConfFile);
	}

	if(pgunionREC->maxNum > 256)
		pgunionREC->maxNum = 256;

	memset(sql,0,sizeof(sql));
	strcpy(sql,"select varSysName,varType,varValue from unionREC order by varSysName");
	
	if ((iCnt = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionReloadREC:: UnionSelectRealDBRecord\n");
		return(iCnt);
	}

	UnionLogXMLPackage();
	
	/*
	// 读取总数量
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReloadREC:: UnionReadXMLPackageValue[totalNum]\n");
		return(ret);
	}
	iCnt = atoi(tmpBuf);
	*/
	
	pgunionREC->realNum = 0;
	for (i = 0; (i < iCnt) && (pgunionREC->realNum < pgunionREC->maxNum); i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionReloadREC:: UnionLocateXMLPackage!\n");
			continue;
		}
		
		memset(varSysName,0,sizeof(varSysName));
		UnionReadXMLPackageValue("varSysName", varSysName, sizeof(varSysName));
		
		memset(varType,0,sizeof(varType));
		UnionReadXMLPackageValue("varType", varType, sizeof(varType));
		memset(varValue,0,sizeof(varValue));
		UnionReadXMLPackageValue("varValue", varValue, sizeof(varValue));
		
		pRECVar = pgunionRECVar + pgunionREC->realNum;
		// 读取变量名
		if (strcmp(varSysName,"maxNumOfVar") == 0)
			continue;
		if (strlen(varSysName) > sizeof(pRECVar->name) - 1)
			continue;
		strcpy(pRECVar->name,varSysName);
		
		// 读取变量类型
		UnionToUpperCase(varType);
		if (strcmp(varType,"CHAR") == 0)
			pRECVar->type = conChar;
		else if (strcmp(varType,"STRING") == 0)
			pRECVar->type = conString;
		else if ((strcmp(varType,"INT") == 0) || (strcmp(varType,"INTEGER") == 0) || (strcmp(varType,"BOOL") == 0))
			pRECVar->type = conInt;
		else if (strcmp(varType,"LONG") == 0)
			pRECVar->type = conLong;
		else if (strcmp(varType,"SHORT") == 0)
			pRECVar->type = conShort;
		else if (strcmp(varType,"DOUBLE") == 0)
			pRECVar->type = conDouble;
		else
		{
			UnionUserErrLog("in UnionReloadREC:: type [%s] invalid!\n",varType);
			continue;
		}
			
		// 读取变量值
		switch (pRECVar->type)
		{
			case	conChar:
				pRECVar->value.charValue = *varValue;
				break;
			case	conString:
				if (strlen(varValue) > sizeof(pRECVar->value.strValue) - 1)
				{
					UnionUserErrLog("in UnionReloadREC:: [%d] string [%s] too long!\n",i,varValue);
					continue;
				}
				strcpy(pRECVar->value.strValue,varValue);
				break;
			case	conInt:
				pRECVar->value.intValue = atoi(varValue);
				break;
			case	conLong:
				//UnionPrintf("in UnionReloadREC:: long = [%s]\n",p);
				pRECVar->value.longValue = atol(varValue);
				break;
			case	conShort:
				pRECVar->value.shortValue = atoi(varValue);
				break;
			case	conDouble:
				//UnionPrintf("in UnionReloadREC:: double = [%s]\n",p);
				pRECVar->value.doubleValue = atof(varValue);
				break;
			default:
				UnionUserErrLog("in UnionReloadREC:: [%d] type invalid!\n",i);
				continue;
		}

		++pgunionREC->realNum;
	}

	if (pgunionREC->realNum == 0)
	{
		UnionUserErrLog("in UnionReloadREC:: realNum[%d] error!\n",pgunionREC->realNum);
		return(errCodeRECMDL_ConfFile);
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

