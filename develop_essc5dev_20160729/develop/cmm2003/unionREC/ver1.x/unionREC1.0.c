//	Author:		Wolfgang Wang
//	Date:		2003/10/10
//	Version:	1.0

#define _UnionLogMDL_3_x_	
#define _UnionEnv_3_x_	

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "unionCommand.h"
#include "unionErrCode.h"

#include "unionREC.h"
#include "UnionStr.h"

PUnionSharedMemoryModule	pgunionRECMDL = NULL;
PUnionREC			pgunionREC = NULL;
PUnionRECVar			pgunionRECVar = NULL;

int UnionGetConfFileNameOfREC(char *fileName)
{
	sprintf(fileName,"%s/unionREC.CFG",getenv("UNIONETC"));
	return(0);
}

int UnionIsRECConnected()
{
	if ((pgunionRECVar == NULL) || (pgunionREC == NULL) || (!UnionIsSharedMemoryInited(conMDLNameOfREC)))
		return(0);
	else
		return(1);
}

int UnionConnectREC()
{
	int				num;
	
	if (UnionIsRECConnected())	// 已经连接
		return(0);
		
	if ((num = UnionGetMaxVarNumOfREC()) <= 0)
	{
		UnionUserErrLog("in UnionConnectREC:: UnionGetMaxVarNumOfREC [%d]\n",num);
		return(num);
	}
	
	if ((pgunionRECMDL = UnionConnectSharedMemoryModule(conMDLNameOfREC,
			sizeof(TUnionREC) + sizeof(TUnionRECVar) * num)) == NULL)
	{
		UnionUserErrLog("in UnionConnectREC:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgunionREC = (PUnionREC)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionRECMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectREC:: PUnionREC!\n");
		return(errCodeSharedMemoryModule);
	}
	
	if ((pgunionREC->pvarTBL = (PUnionRECVar)((unsigned char *)pgunionREC + sizeof(*pgunionREC))) == NULL)
	{
		UnionUserErrLog("in UnionConnectREC:: PUnionREC!\n");
		return(errCodeSharedMemoryModule);
	}
	pgunionRECVar = pgunionREC->pvarTBL;
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionRECMDL))
	{
		pgunionREC->maxNum = num;
		pgunionREC->realNum = 0;
		return(UnionReloadREC());
	}
	else
		return(0);
}


int UnionDisconnectREC()
{
	pgunionRECVar = NULL;
	pgunionREC = NULL;
	return(UnionDisconnectShareModule(pgunionRECMDL));
}

int UnionRemoveREC()
{
	UnionDisconnectREC();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfREC));
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
		else if (strcmp(p,"INT") == 0)
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

int UnionPrintRECVarToFile(PUnionRECVar pvar,FILE *fp)
{
	if ((pvar == NULL) || (fp == NULL))
		return(errCodeParameter);
	fprintf(fp,"[%40s] ",pvar->name);
	switch (pvar->type)
	{
		case	conChar:
			fprintf(fp,"%8s %c","char",pvar->value.charValue);
			break;
		case	conString:
			fprintf(fp,"%8s %s","string",pvar->value.strValue);
			break;
		case	conInt:
			fprintf(fp,"%8s %d","int",pvar->value.intValue);
			break;
		case	conLong:
			fprintf(fp,"%8s %ld","long",pvar->value.longValue);
			break;
		case	conShort:
			fprintf(fp,"%8s %d","short",pvar->value.shortValue);
			break;
		case	conDouble:
			fprintf(fp,"%8s %-12.2f","double",pvar->value.doubleValue);
			break;
		default:
			fprintf(fp,"Unknown");
			break;
	}
	fprintf(fp,"\n");
	return(0);
}

int UnionPrintRECToFile(FILE *fp)
{
	int	i;
	int	ret;
	
	if ((ret = UnionConnectREC()) < 0)
	{
		UnionUserErrLog("in UnionPrintRECToFile:: UnionConnectREC!\n");
		return(ret);
	}
	if (fp == NULL)
		return(errCodeParameter);
	for (i = 0; i < pgunionREC->realNum; i++)
	{
		if ((i != 0) && (i % 23 == 0))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
		UnionPrintRECVarToFile((pgunionRECVar+i),fp);
	}
	fprintf(fp,"RECVarNum = [%d]\n",pgunionREC->realNum);
	//if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
	//	return(0);
	//UnionPrintSharedMemoryModuleToFile(pgunionRECMDL,stdout);
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

PUnionRECVar UnionReadRECVar(char *varName)
{
	int	i;
	int	ret;
	if ((ret = UnionConnectREC()) < 0)
	{
		UnionUserErrLog("in UnionReadRECVar:: UnionConnectREC!\n");
		return(NULL);
	}
	for (i = 0; i < pgunionREC->realNum; i++)
	{
		if (strcmp(varName,(pgunionRECVar+i)->name) == 0)
			return(pgunionRECVar+i);
	}
	return(NULL);
}

int UnionReadIntTypeRECVar(char *varName)
{
	PUnionRECVar	pvar = NULL;
	
	if ((pvar = UnionReadRECVar(varName)) == NULL)
		return(errCodeRECMDL_VarNotExists);
	if (pvar->type == conInt)
		return(pvar->value.intValue);
	return(errCodeRECMDL_VarType);
}

long UnionReadLongTypeRECVar(char *varName)
{
	PUnionRECVar	pvar = NULL;
	
	if ((pvar = UnionReadRECVar(varName)) == NULL)
		return(errCodeRECMDL_VarNotExists);
	if (pvar->type == conLong)
		return(pvar->value.longValue);
	return(errCodeRECMDL_VarType);
}

short UnionReadShortTypeRECVar(char *varName)
{
	PUnionRECVar	pvar = NULL;
	
	if ((pvar = UnionReadRECVar(varName)) == NULL)
		return(errCodeRECMDL_VarNotExists);
	if (pvar->type == conShort)
		return(pvar->value.shortValue);
	return(errCodeRECMDL_VarType);
}

double UnionReadDoubleTypeRECVar(char *varName)
{
	PUnionRECVar	pvar = NULL;
	
	if ((pvar = UnionReadRECVar(varName)) == NULL)
		return(errCodeRECMDL_VarNotExists);
	if (pvar->type == conDouble)
		return(pvar->value.doubleValue);
	return(errCodeRECMDL_VarType);
}

char UnionReadCharTypeRECVar(char *varName)
{
	PUnionRECVar	pvar = NULL;
	
	if ((pvar = UnionReadRECVar(varName)) == NULL)
		return(0);
	if (pvar->type == conChar)
		return(pvar->value.charValue);
	return(0);
}

char *UnionReadStringTypeRECVar(char *varName)
{
	PUnionRECVar	pvar = NULL;
	
	if ((pvar = UnionReadRECVar(varName)) == NULL)
		return(NULL);
	if (pvar->type == conString)
		return(pvar->value.strValue);
	return(NULL);
}

int UnionUpdateRECVar(char *varName,unsigned char *value)
{
	PUnionRECVar	pvar;
		
	if ((pvar = UnionReadRECVar(varName)) == NULL)
	{
		UnionUserErrLog("in UnionUpdateRECVar:: UnionReadRECVar [%s]\n",varName);
		return(errCodeRECMDL_VarNotExists);
	}
	switch (pvar->type)
	{
		case	conInt:
			memcpy(&(pvar->value.intValue),value,sizeof(int));
			break;
		case	conShort:
			memcpy(&(pvar->value.shortValue),value,sizeof(short));
			break;
		case	conLong:
			memcpy(&(pvar->value.longValue),value,sizeof(long));
			break;
		case	conChar:
			memcpy(&(pvar->value.charValue),value,sizeof(char));
			break;
		case	conString:
			if (strlen((char *)value) > sizeof(pvar->value.strValue) - 1)
			{
				UnionUserErrLog("in UnionUpdateRECVar:: string [%s] too long!\n",value);
				return(errCodeParameter);
			}
			strcpy(pvar->value.strValue,(char *)value);
			break;
		case	conDouble:
			memcpy(&(pvar->value.doubleValue),value,sizeof(double));
			break;
		default:
			UnionUserErrLog("in UnionUpdateRECVar:: unknown var type!\n");
			return(errCodeRECMDL_VarType);
	}
	return(UnionMirrorREC());
}

int UnionMirrorREC()
{
	FILE	*fp,*tmpFp;
	char	fileName[512];
	char	tmpFileName[512];
	char	tmpBuf[2048];
	char	varName[100];
	PUnionRECVar	pvar;
	int	ret;
	int	len;
	
	if (!UnionIsRECConnected())
		return(0);
		
	memset(fileName,0,sizeof(fileName));
	UnionGetConfFileNameOfREC(fileName);
	memset(tmpFileName,0,sizeof(tmpFileName));
	sprintf(tmpFileName,"%s/tmp/%d.tmp",getenv("HOME"),getpid());
	
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionMirrorREC:: fopen [%s]\n",fileName);
		return(errCodeParameter);
	}
	if ((tmpFp = fopen(tmpFileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionMirrorREC:: fopen [%s]\n",tmpFileName);
		fclose(fp);
		return(errCodeUseOSErrCode);
	}
	while (!feof(fp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadOneFileLine(fp,tmpBuf)) < 0)
			break;
		//if (ret == 0)
		//	continue;
		//printf("%s\n",tmpBuf);
		memset(varName,0,sizeof(varName));
		if ((ret = UnionReadVarNameFromLine(tmpBuf,varName)) < 0)
		{
			fprintf(tmpFp,"%s\n",tmpBuf);
			continue;
		}
		if (!UnionIsValidRECVar(pvar = UnionReadRECVar(varName)))
		{
			fprintf(tmpFp,"%s\n",tmpBuf);
			continue;
		}
		
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"[%s]",pvar->name);
		len = strlen(tmpBuf);
		if (42 - len > 0)
			memset(tmpBuf+len,' ',42-len);
		len = strlen(tmpBuf);
		switch (pvar->type)
		{
			case	conInt:
				sprintf(tmpBuf+len,"[int]            [%d]",pvar->value.intValue);
				break;
			case	conShort:
				sprintf(tmpBuf+len,"[short]          [%d]",pvar->value.shortValue);
				break;
			case	conLong:
				sprintf(tmpBuf+len,"[long]           [%ld]",pvar->value.longValue);
				break;
			case	conString:
				sprintf(tmpBuf+len,"[string]         [%s]",pvar->value.strValue);
				break;
			case	conChar:
				sprintf(tmpBuf+len,"[char]           [%d]",pvar->value.charValue);
				break;
			case	conDouble:
				sprintf(tmpBuf+len,"[double]         [%f]",pvar->value.doubleValue);
				break;
		}
		fprintf(tmpFp,"%s\n",tmpBuf);
	}
	fclose(fp);
	fclose(tmpFp);
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"cp %s %s",tmpFileName,fileName);
	system(tmpBuf);
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"rm %s",tmpFileName);
	system(tmpBuf);
	return(0);
}

int UnionIsValidRECVar(PUnionRECVar pvar)
{
	if (pvar == NULL)
		return(0);
	switch (pvar->type)
	{
		case	conInt:
		case	conString:
		case	conChar:
		case	conLong:
		case	conShort:
		case	conDouble:
			return(1);
		default:
			return(0);
	}
}
