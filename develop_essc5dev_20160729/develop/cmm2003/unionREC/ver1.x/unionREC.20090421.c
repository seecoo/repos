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
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "unionCommand.h"
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

PUnionSharedMemoryModule	pgunionRECMDL = NULL;
PUnionREC			pgunionREC = NULL;
PUnionRECVar			pgunionRECVar = NULL;
char				gunionRECTmpStr[20];		// 2005/9/16增加

// 20060808
int UnionGetCurrentRECVarNum()
{
	int	ret;
	
	if ((ret = UnionConnectREC()) < 0)
		return(ret);
	else
		return(pgunionREC->realNum);
}

// 20060808
PUnionRECVar UnionGetCurrentRECVarGrp()
{
	if (UnionConnectREC() < 0)
		return(NULL);
	else
		return(pgunionRECVar);
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

int UnionPrintRECVarToFile(PUnionRECVar pvar,FILE *fp)
{
	if ((pvar == NULL) || (fp == NULL))
		return(errCodeParameter);
	fprintf(fp,"%40s ",pvar->name);
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
#ifndef _WIN32
		if ((i != 0) && (i % 23 == 0))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
#endif
		UnionPrintRECVarToFile((pgunionRECVar+i),fp);
	}
	fprintf(fp,"RECVarNum = [%d]\n",pgunionREC->realNum);
	//if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
	//	return(0);
	//UnionPrintSharedMemoryModuleToFile(pgunionRECMDL,stdout);
	return(0);
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
	switch (pvar->type)
	{
		case	conChar:
			sprintf(gunionRECTmpStr,"%c",pvar->value.charValue);
			break;
		case	conString:
			sprintf(gunionRECTmpStr,"%s",pvar->value.strValue);
			break;
		case	conInt:
			sprintf(gunionRECTmpStr,"%d",pvar->value.intValue);
			break;
		case	conLong:
			sprintf(gunionRECTmpStr,"%ld",pvar->value.longValue);
			break;
		case	conShort:
			sprintf(gunionRECTmpStr,"%d",pvar->value.shortValue);
			break;
		case	conDouble:
			sprintf(gunionRECTmpStr,"%-12.2f",pvar->value.doubleValue);
			break;
		default:
			sprintf(gunionRECTmpStr,"UnknownType");
			break;
	}
	return(gunionRECTmpStr);
}

int UnionUpdateRECVar(char *varName,unsigned char *value)
{
	PUnionRECVar	pvar;
	char		nameOfType[64];
	char		tmpBuf[128];		
	char		fileName[512];
	int		ret;
		
	if ((pvar = UnionReadRECVar(varName)) == NULL)
	{
		UnionUserErrLog("in UnionUpdateRECVar:: UnionReadRECVar [%s]\n",varName);
		return(errCodeRECMDL_VarNotExists);
	}
	switch (pvar->type)
	{
		case	conInt:
			memcpy(&(pvar->value.intValue),value,sizeof(int));
			sprintf(tmpBuf,"%d",pvar->value.intValue);
			break;
		case	conShort:
			memcpy(&(pvar->value.shortValue),value,sizeof(short));
			sprintf(tmpBuf,"%d",pvar->value.shortValue);
			break;
		case	conLong:
			memcpy(&(pvar->value.longValue),value,sizeof(long));
			sprintf(tmpBuf,"%ld",pvar->value.longValue);
			break;
		case	conChar:
			memcpy(&(pvar->value.charValue),value,sizeof(char));
			sprintf(tmpBuf,"%c",pvar->value.charValue);
			break;
		case	conString:
			if (strlen((char *)value) > sizeof(pvar->value.strValue) - 1)
			{
				UnionUserErrLog("in UnionUpdateRECVar:: string [%s] too long!\n",value);
				return(errCodeParameter);
			}
			strcpy(pvar->value.strValue,(char *)value);
			sprintf(tmpBuf,"%s",pvar->value.strValue);
			break;
		case	conDouble:
			memcpy(&(pvar->value.doubleValue),value,sizeof(double));
			sprintf(tmpBuf,"%.2f",pvar->value.doubleValue);
			break;
		default:
			UnionUserErrLog("in UnionUpdateRECVar:: unknown var type!\n");
			return(errCodeRECMDL_VarType);
	}
	memset(nameOfType,0,sizeof(nameOfType));
	if ((ret = UnionConvertRECVarTypeIntoTypeName(pvar->type,nameOfType)) < 0)
	{
		UnionUserErrLog("in UnionUpdateRECVar:: UnionConvertRECVarTypeIntoTypeName!\n");
		return(ret);
	}
	memset(fileName,0,sizeof(fileName));
	UnionGetConfFileNameOfREC(fileName);
	if ((ret = UnionUpdateEnviVar(fileName,varName,"[%s][%s]",nameOfType,tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionUpdateRECVar:: UnionUpdateEnviVar [%s]!\n",varName);
		return(ret);
	}
	return(0);
}

int UnionMirrorREC()
{
	FILE	*fp,*tmpFp;
	char	fileName[512];
	char	tmpFileName[512];
	char	tmpBuf[2048];
	char	varName[128];
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

// 2005/06/16 王纯军增加
int UnionUpdateRECVarAsString(char *varName,char *value,int len)
{
	int		ret;
	PUnionRECVar	pvar;
	char		nameOfType[64];
	char		tmpBuf[128];		
	char		fileName[512];
		
	if ((pvar = UnionReadRECVar(varName)) == NULL)
	{
		UnionUserErrLog("in UnionUpdateRECVarAsString:: UnionReadRECVar [%s]\n",varName);
		return(errCodeRECMDL_VarNotExists);
	}
	if (len < 0)
	{
		UnionUserErrLog("in UnionUpdateRECVarAsString:: len = [%d]\n",len);
		return(errCodeParameter);
	}
	if (pvar->type == conString)
	{
		if (len > sizeof(pvar->value.strValue) - 1)
		{
			UnionUserErrLog("in UnionUpdateRECVarAsString:: string  too long [%d]!\n",len);
			return(errCodeParameter);
		}
		strcpy(pvar->value.strValue,value);
	}
	else
	{
		if (len >= sizeof(tmpBuf))
		{
			UnionUserErrLog("in UnionUpdateRECVarAsString:: string  too long [%d]!\n",len);
			return(errCodeParameter);
		}
		memcpy(tmpBuf,value,len);
		tmpBuf[len] = 0;
		switch (pvar->type)
		{
			case	conInt:
				pvar->value.intValue = atoi(tmpBuf);
				break;
			case	conShort:
				pvar->value.shortValue = atoi(tmpBuf);
				break;
			case	conLong:
				pvar->value.longValue = atol(tmpBuf);
				break;
			case	conChar:
				pvar->value.charValue = tmpBuf[0];
				break;
			case	conString:
				break;
			case	conDouble:
				pvar->value.doubleValue = atof(tmpBuf);
				break;
			default:
				UnionUserErrLog("in UnionUpdateRECVarAsString:: unknown var type!\n");
				return(errCodeRECMDL_VarType);
		}
	}
	memset(nameOfType,0,sizeof(nameOfType));
	if ((ret = UnionConvertRECVarTypeIntoTypeName(pvar->type,nameOfType)) < 0)
	{
		UnionUserErrLog("in UnionUpdateRECVar:: UnionConvertRECVarTypeIntoTypeName!\n");
		return(ret);
	}
	memset(fileName,0,sizeof(fileName));
	UnionGetConfFileNameOfREC(fileName);
	value[len] = 0;
	if ((ret = UnionUpdateEnviVar(fileName,varName,"[%s][%s]",nameOfType,value)) < 0)
	{
		UnionUserErrLog("in UnionUpdateRECVarAsString:: UnionUpdateEnviVar [%s]!\n",varName);
		return(ret);
	}
	return(ret);
}

int UnionPrintRECToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintRECToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	ret = UnionPrintRECToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

// 2006/8/25
#ifndef _WIN32
TUnionRECVarType UnionConvertTypeNameIntoRECVarType(char *nameOfType)
#else
int UnionConvertTypeNameIntoRECVarType(char *nameOfType)
#endif
{
	if (nameOfType == NULL)
		return(errCodeParameter);
	UnionToUpperCase(nameOfType);
	if ((strcmp(nameOfType,"STRING") == 0) || (strcmp(nameOfType,"STR") == 0))
		return(conString);
	if ((strcmp(nameOfType,"CHAR") == 0) || (strcmp(nameOfType,"CHARACTER") == 0))
		return(conChar);
	if ((strcmp(nameOfType,"INTEGER") == 0) || (strcmp(nameOfType,"INT") == 0))
		return(conInt);
	if (strcmp(nameOfType,"LONG") == 0)
		return(conLong);
	if (strcmp(nameOfType,"SHORT") == 0)
		return(conShort);
	if (strcmp(nameOfType,"DOUBLE") == 0)
		return(conDouble);
	UnionUserErrLog("in UnionConvertTypeNameIntoRECVarType:: wrong type name [%s]\n",nameOfType);
	return(errCodeRECMDL_VarType);
}

// 2006/8/25
int UnionConvertRECVarTypeIntoTypeName(TUnionRECVarType type,char *nameOfType)
{
	if (nameOfType == NULL)
		return(errCodeParameter);
	switch (type)
	{
		case	conString:
			strcpy(nameOfType,"string");
			break;
		case	conChar:
			strcpy(nameOfType,"char");
			break;
		case	conInt:
			strcpy(nameOfType,"int");
			break;
		case	conLong:
			strcpy(nameOfType,"long");
			break;
		case	conShort:
			strcpy(nameOfType,"short");
			break;
		case	conDouble:
			strcpy(nameOfType,"double");
			break;
		default:
			UnionUserErrLog("in UnionInsertRECVar:: type [%d] error!\n",type);
			return(errCodeRECMDL_VarType);
	}
	return(0);
}

// 检查变量是否已在文件中定义了
int UnionExistRECVarInConfFile(char *name)
{
	char	fileName[512];
	
	memset(fileName,0,sizeof(fileName));
	UnionGetConfFileNameOfREC(fileName);
	return(UnionExistEnviVarInFile(fileName,name));
}

//2006/8/25 插入一个记录
int UnionInsertRECVar(char *name,char *type,char *value)
{
	TUnionRECVar	recVar;
	int		ret;
	PUnionRECVar	precVar;
	char		fileName[512];
	char		nameOfType[64];
		
	if ((name == NULL) || (type == NULL) || (value == NULL))
		return(errCodeParameter);
		
	if ((ret = UnionConnectREC()) < 0)	
	{
		UnionUserErrLog("in UnionInsertRECVar:: UnionConnectREC!\n");
		return(ret);
	}
	if (((precVar = UnionReadRECVar(name)) != NULL)	||
		(UnionExistRECVarInConfFile(name) > 0)) // 检查变量是否已存在
	{
		UnionUserErrLog("in UnionInsertRECVar:: [%s] already exists!\n",name);
		return(errCodeRECMDL_VarAlreadyExists);
	}
	// 以下进行变量赋值
	memset(&recVar,0,sizeof(recVar));
	if ((strlen(name) == 0) || (strlen(name) >= sizeof(recVar.name)))
	{
		UnionUserErrLog("in UnionInsertRECVar:: name [%s] length error!\n",name);
		return(errCodeRECMDL_VarNameLength);
	}
	strcpy(recVar.name,name);
#ifndef _WIN32
	switch (recVar.type = UnionConvertTypeNameIntoRECVarType(type))
#else
	switch (recVar.type = (TUnionRECVarType)UnionConvertTypeNameIntoRECVarType(type))
#endif
	{
		case	conString:
			if (strlen(value) >= sizeof(recVar.value.strValue))
			{
				UnionUserErrLog("in UnionInsertRECVar:: value [%s] too long!\n",value);
				return(errCodeRECMDL_VarValueTooLong);
			}			
			strcpy(recVar.value.strValue,value);
			break;
		case	conChar:
			recVar.value.charValue = value[0];
			break;
		case	conInt:
			recVar.value.intValue = atoi(value);
			break;
		case	conLong:
			recVar.value.longValue = atol(value);
			break;
		case	conShort:
			recVar.value.shortValue = atoi(value);
			break;
		case	conDouble:
			recVar.value.doubleValue = atof(value);
			break;
		default:
			UnionUserErrLog("in UnionInsertRECVar:: type [%s] error!\n",type);
			return(errCodeRECMDL_VarType);
	}
	// 将变量拷贝到内存中
	if (pgunionREC->realNum >= pgunionREC->maxNum)
	{
		UnionUserErrLog("in UnionInsertRECVar:: rec table is full!\n");
		return(errCodeRECMDL_VarTBLIsFull);
	}
	if (pgunionREC->realNum < 0)
	{
		UnionUserErrLog("in UnionInsertRECVar:: realNum = [%d]\n",pgunionREC->realNum);
		return(errCodeRECMDL_RealNumError);
	}
	memcpy(pgunionRECVar+pgunionREC->realNum,&recVar,sizeof(recVar));
	pgunionREC->realNum += 1;
	// 将变量插入到文件中
	memset(nameOfType,0,sizeof(nameOfType));
	if ((ret = UnionConvertRECVarTypeIntoTypeName(recVar.type,nameOfType)) < 0)
	{
		UnionUserErrLog("in UnionInsertRECVar:: UnionConvertRECVarTypeIntoTypeName!\n");
		return(ret);
	}
	memset(fileName,0,sizeof(fileName));
	UnionGetConfFileNameOfREC(fileName);
	if ((ret = UnionInsertEnviVar(fileName,name,"[%s][%s]",nameOfType,value)) < 0)
	{
		UnionUserErrLog("in UnionInsertRECVar:: UnionInsertEnviVar [%s]\n",name);
		return(ret);
	}
	return(ret);
}

//2006/8/25 删除一个记录
int UnionDeleteRECVar(char *name)
{
	int		ret;
	char		fileName[512];
	
	if (name == NULL)
		return(errCodeParameter);
	
	// 先从文件删除记录	
	memset(fileName,0,sizeof(fileName));
	UnionGetConfFileNameOfREC(fileName);
	if ((ret = UnionDeleteEnviVar(fileName,name)) < 0)
	{
		UnionUserErrLog("in UnionDeleteRECVar:: UnionDeleteEnviVar [%s]!\n",name);
		return(ret);
	}
	
	return(UnionReloadREC());
}

// wangk add 2009-9-24
int UnionPrintRECInRecStrFormatToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
	{
		UnionUserErrLog("in UnionPrintRECInRecStrFormatToSpecFile:: NULL poionter!\n");
		return(errCodeParameter);
	}

	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintRECInRecStrFormatToSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	ret = UnionPrintRECInRecStrFormatToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

int UnionPrintRECInRecStrFormatToFile(FILE *fp)
{
	int	i;
	int	ret;
	
	if ((ret = UnionConnectREC()) < 0)
	{
		UnionUserErrLog("in UnionPrintRECInRecStrFormatToFile:: UnionConnectREC!\n");
		return(ret);
	}
	if (fp == NULL)
		return(errCodeParameter);
	for (i = 0; i < pgunionREC->realNum; i++)
	{
		UnionPrintRECVarInRecStrToFile((pgunionRECVar+i),fp);
	}

	return(0);
}

int UnionPrintRECVarInRecStrToFile(PUnionRECVar pvar,FILE *fp)
{
	char recStr[1024];
	int sizeOfBuf = 0;
	int offSet = 0;
	int ret = 0;

	if ((pvar == NULL) || (fp == NULL))
		return(errCodeParameter);

	sizeOfBuf = sizeof(recStr);
	memset(recStr, 0, sizeof(recStr));
	ret = UnionPutRecFldIntoRecStr("varName", pvar->name, strlen(pvar->name), recStr+offSet, sizeOfBuf-offSet);
	if( ret < 0 )
	{
		UnionUserErrLog("in UnionPrintRECVarInRecStrToFile:: UnionPutRecFldIntoRecStr [varName] error!\n");
		return (ret);
	}
	offSet += ret;

	switch (pvar->type)
	{
		case	conChar:
		{
			ret = UnionPutRecFldIntoRecStr("varType", "char", strlen("char"), recStr+offSet, sizeOfBuf-offSet);
			if( ret < 0 )
			{
				UnionUserErrLog("in UnionPrintRECVarInRecStrToFile:: UnionPutRecFldIntoRecStr [varType] error!\n");
				return (ret);
			}
			offSet += ret;

			ret = UnionPutCharTypeRecFldIntoRecStr("varValue", pvar->value.charValue, recStr+offSet, sizeOfBuf-offSet);
			if( ret < 0 )
			{
				UnionUserErrLog("in UnionPrintRECVarInRecStrToFile:: UnionPutCharTypeRecFldIntoRecStr [varValue] error!\n");
				return (ret);
			}
			offSet += ret;

			break;
		}
		case	conString:
		{
			ret = UnionPutRecFldIntoRecStr("varType", "string", strlen("string"), recStr+offSet, sizeOfBuf-offSet);
			if( ret < 0 )
			{
				UnionUserErrLog("in UnionPrintRECVarInRecStrToFile:: UnionPutRecFldIntoRecStr [varType] error!\n");
				return (ret);
			}
			offSet += ret;

			ret = UnionPutRecFldIntoRecStr("varValue", pvar->value.strValue, strlen(pvar->value.strValue), recStr+offSet, sizeOfBuf-offSet);
			if( ret < 0 )
			{
				UnionUserErrLog("in UnionPrintRECVarInRecStrToFile:: UnionPutRecFldIntoRecStr [varValue] error!\n");
				return (ret);
			}
			offSet += ret;

			break;
		}
		case	conInt:
		{
			ret = UnionPutRecFldIntoRecStr("varType", "int", strlen("int"), recStr+offSet, sizeOfBuf-offSet);
			if( ret < 0 )
			{
				UnionUserErrLog("in UnionPrintRECVarInRecStrToFile:: UnionPutRecFldIntoRecStr [varType] error!\n");
				return (ret);
			}
			offSet += ret;

			ret = UnionPutIntTypeRecFldIntoRecStr("varValue", pvar->value.intValue, recStr+offSet, sizeOfBuf-offSet);
			if( ret < 0 )
			{
				UnionUserErrLog("in UnionPrintRECVarInRecStrToFile:: UnionPutIntTypeRecFldIntoRecStr [varValue] error!\n");
				return (ret);
			}
			offSet += ret;

			break;
		}
		case	conLong:
		{
			ret = UnionPutRecFldIntoRecStr("varType", "long", strlen("long"), recStr+offSet, sizeOfBuf-offSet);
			if( ret < 0 )
			{
				UnionUserErrLog("in UnionPrintRECVarInRecStrToFile:: UnionPutRecFldIntoRecStr [varType] error!\n");
				return (ret);
			}
			offSet += ret;

			ret = UnionPutLongTypeRecFldIntoRecStr("varValue", pvar->value.longValue, recStr+offSet, sizeOfBuf-offSet);
			if( ret < 0 )
			{
				UnionUserErrLog("in UnionPrintRECVarInRecStrToFile:: UnionPutLongTypeRecFldIntoRecStr [varValue] error!\n");
				return (ret);
			}
			offSet += ret;

			break;
		}
		case	conShort:
		{
			ret = UnionPutRecFldIntoRecStr("varType", "short", strlen("short"), recStr+offSet, sizeOfBuf-offSet);
			if( ret < 0 )
			{
				UnionUserErrLog("in UnionPrintRECVarInRecStrToFile:: UnionPutRecFldIntoRecStr [varType] error!\n");
				return (ret);
			}
			offSet += ret;

			ret = UnionPutIntTypeRecFldIntoRecStr("varValue", pvar->value.shortValue, recStr+offSet, sizeOfBuf-offSet);
			if( ret < 0 )
			{
				UnionUserErrLog("in UnionPrintRECVarInRecStrToFile:: UnionPutIntTypeRecFldIntoRecStr [varValue] error!\n");
				return (ret);
			}
			offSet += ret;

			break;
		}
		case	conDouble:
		{
			ret = UnionPutRecFldIntoRecStr("varType", "double", strlen("double"), recStr+offSet, sizeOfBuf-offSet);
			if( ret < 0 )
			{
				UnionUserErrLog("in UnionPrintRECVarInRecStrToFile:: UnionPutRecFldIntoRecStr [varType] error!\n");
				return (ret);
			}
			offSet += ret;

			ret = UnionPutDoubleTypeRecFldIntoRecStr("varValue", pvar->value.doubleValue, recStr+offSet, sizeOfBuf-offSet);
			if( ret < 0 )
			{
				UnionUserErrLog("in UnionPrintRECVarInRecStrToFile:: UnionPutDoubleTypeRecFldIntoRecStr [varValue] error!\n");
				return (ret);
			}
			offSet += ret;

			break;
		}
		default:
		{
			ret = UnionPutRecFldIntoRecStr("varType", "Unknown", strlen("Unknown"), recStr+offSet, sizeOfBuf-offSet);
			if( ret < 0 )
			{
				UnionUserErrLog("in UnionPrintRECVarInRecStrToFile:: UnionPutRecFldIntoRecStr [varType] error!\n");
				return (ret);
			}
			offSet += ret;

			break;
		}
	}
	fprintf(fp, "%s\n", recStr);

	return(0);
}
// wangk add end 2009-9-24

// added 2012-11
int UnionUpdateRECImageValue(char *varName,unsigned char *value)
{
	PUnionRECVar	pvar;
	char		tmpBuf[128];		
		
	if ((pvar = UnionReadRECVar(varName)) == NULL)
	{
		UnionUserErrLog("in UnionUpdateRECImageValue:: UnionReadRECVar [%s]\n",varName);
		return(errCodeRECMDL_VarNotExists);
	}
	switch (pvar->type)
	{
		case	conInt:
			memcpy(&(pvar->value.intValue),value,sizeof(int));
			sprintf(tmpBuf,"%d",pvar->value.intValue);
			break;
		case	conShort:
			memcpy(&(pvar->value.shortValue),value,sizeof(short));
			sprintf(tmpBuf,"%d",pvar->value.shortValue);
			break;
		case	conLong:
			memcpy(&(pvar->value.longValue),value,sizeof(long));
			sprintf(tmpBuf,"%ld",pvar->value.longValue);
			break;
		case	conChar:
			memcpy(&(pvar->value.charValue),value,sizeof(char));
			sprintf(tmpBuf,"%c",pvar->value.charValue);
			break;
		case	conString:
			if (strlen((char *)value) > sizeof(pvar->value.strValue) - 1)
			{
				UnionUserErrLog("in UnionUpdateRECImageValue:: string [%s] too long!\n",value);
				return(errCodeParameter);
			}
			strcpy(pvar->value.strValue,(char *)value);
			sprintf(tmpBuf,"%s",pvar->value.strValue);
			break;
		case	conDouble:
			memcpy(&(pvar->value.doubleValue),value,sizeof(double));
			sprintf(tmpBuf,"%.2f",pvar->value.doubleValue);
			break;
		default:
			UnionUserErrLog("in UnionUpdateRECImageValue:: unknown var type!\n");
			return(errCodeRECMDL_VarType);
	}
	return(0);
}

