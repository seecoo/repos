//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	1.0

#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_	
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "unionCommand.h"

#include "unionErrCodeTBL.h"

PUnionSharedMemoryModule	pgunionSoftErrCodeMDL = NULL;
PUnionSofterrCodeTBL		pgunionSoftErrCodeTBL = NULL;
char				pgunionDefaultCliErrCode[] = "UN";

int UnionGetNameOfSoftErrCodeTBL(char *fileName)
{
	sprintf(fileName,"%s/errCode/%s",getenv("UNIONETC"),conConfFileNameOfSoftErrCodeTBL);
	return(0);
}

int UnionIsSoftErrCodeTBLConnected()
{
	// 2007/10/26修改
	if ((pgunionSoftErrCodeTBL == NULL) || (!UnionIsSharedMemoryInited(conMDLNameOfSoftErrCodeTBL)))
		return(0);
	else
		return(1);
}

int UnionGetMaxNumOfSoftErrCode()
{
	return(conMaxNumOfSoftErrCode);	
}

int UnionConnectSoftErrCodeTBL()
{
	if (UnionIsSoftErrCodeTBLConnected())	// 已经连接
		return(0);
	
	if ((pgunionSoftErrCodeMDL = UnionConnectSharedMemoryModule(conMDLNameOfSoftErrCodeTBL,
			sizeof(TUnionSofterrCodeTBL))) == NULL)
	{
		UnionUserErrLog("in UnionConnectSoftErrCodeTBL:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgunionSoftErrCodeTBL = (PUnionSofterrCodeTBL)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionSoftErrCodeMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectSoftErrCodeTBL:: PUnionSofterrCodeTBL!\n");
		return(errCodeSharedMemoryModule);
	}
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionSoftErrCodeMDL))
	{
		return(UnionReloadSoftErrCodeTBL());
	}
	else
		return(0);
}

	
int UnionDisconnectSoftErrCodeTBL()
{
	pgunionSoftErrCodeTBL = NULL;
	return(UnionDisconnectShareModule(pgunionSoftErrCodeMDL));
}

int UnionRemoveSoftErrCodeTBL()
{
	UnionDisconnectSoftErrCodeTBL();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfSoftErrCodeTBL));
}

int UnionReloadSoftErrCodeTBL()
{
	int			ret;
	char			fileName[512];
	char			*p;
	int			i,j;
	int			realNum = 0;
	
	if ((ret = UnionConnectSoftErrCodeTBL()) < 0)
	{
		UnionUserErrLog("in UnionReloadSoftErrCodeTBL:: UnionConnectSoftErrCodeTBL!\n");
		return(ret);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfSoftErrCodeTBL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionReloadSoftErrCodeTBL:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	for (i = 0,j = 0,realNum = 0; (i < UnionGetEnviVarNum()) && (realNum < UnionGetMaxNumOfSoftErrCode()); i++,j = 0)
	{
		// 读取softErrCode
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,j)) == NULL)
		{
			UnionUserErrLog("in UnionReloadSoftErrCodeTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,j);
			continue;
		}
		if ((strcmp(p,"defaultCliErrCode") == 0) || (strcmp(p,"defaultSoftErrCode") == 0))
			continue;
		pgunionSoftErrCodeTBL->rec[realNum].softErrCode = atoi(p);
		j++;
		// 读取cliErrCode
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,j)) == NULL)
		{
			UnionUserErrLog("in UnionReloadSoftErrCodeTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,j);
			continue;
		}
		if (strlen(p) > sizeof(pgunionSoftErrCodeTBL->rec[realNum].cliErrCode) - 1)
			continue;
		strcpy(pgunionSoftErrCodeTBL->rec[realNum].cliErrCode,p);
		realNum++;
	}
	pgunionSoftErrCodeTBL->realNum = realNum;
	strcpy(pgunionSoftErrCodeTBL->defaultCliErrCode,"UN");
	if ((p = UnionGetEnviVarByName("defaultCliErrCode")) != NULL)
	{
		if (strlen(p) >= sizeof(pgunionSoftErrCodeTBL->defaultCliErrCode))
			memcpy(pgunionSoftErrCodeTBL->defaultCliErrCode,p,sizeof(pgunionSoftErrCodeTBL->defaultCliErrCode)-1);
		else
			strcpy(pgunionSoftErrCodeTBL->defaultCliErrCode,p);	
	}
	pgunionSoftErrCodeTBL->defaultSoftErrCode = errCodeDefaultErrCode;
	if ((p = UnionGetEnviVarByName("defaultSoftErrCode")) != NULL)
		pgunionSoftErrCodeTBL->defaultSoftErrCode = atoi(p);
	if (pgunionSoftErrCodeTBL->defaultSoftErrCode >= 0)
		pgunionSoftErrCodeTBL->defaultSoftErrCode = errCodeDefaultErrCode;
	
	UnionClearEnvi();
	return(realNum);
}

int UnionPrintSoftErrCodeTBLToFile(FILE *fp)
{
	int	i;
	int	ret;
	int	num = 0;
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintSoftErrCodeTBLToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
		
	if ((ret = UnionConnectSoftErrCodeTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintSoftErrCodeTBLToFile:: UnionConnectSoftErrCodeTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionSoftErrCodeTBL->realNum; i++)
	{
		if (UnionPrintSoftErrCodeToFile(&(pgunionSoftErrCodeTBL->rec[i]),fp) < 0)
			continue;
		num++;
		if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
	}
	fprintf(fp,"defaultCliErrCode  = [%d]\n",pgunionSoftErrCodeTBL->defaultSoftErrCode);
	fprintf(fp,"defaultSoftErrCode = [%s]\n",pgunionSoftErrCodeTBL->defaultCliErrCode);
	fprintf(fp,"SoftErrCodeNum     = [%d]\n",num);
	return(0);
}

int UnionPrintSoftErrCodeToFile(PUnionSoftErrCode pylqz_cliErrCode,FILE *fp)
{
	if ((pylqz_cliErrCode == NULL) || (fp == NULL))
	{
		UnionUserErrLog("in UnionPrintSoftErrCodeTBLToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
	fprintf(fp,"%06d %s\n",pylqz_cliErrCode->softErrCode,pylqz_cliErrCode->cliErrCode);
	return(0);
}

char *UnionFindCliErrCodeOfSoftErrCode(int softErrCode)
{
	int	i;
	int	ret;
		
	if ((ret = UnionConnectSoftErrCodeTBL()) < 0)
	{
		UnionUserErrLog("in UnionFindCliErrCodeOfSoftErrCode:: UnionConnectSoftErrCodeTBL!\n");
		return(pgunionDefaultCliErrCode);
	}
	
	for (i = 0; i < pgunionSoftErrCodeTBL->realNum; i++)
	{
		if (pgunionSoftErrCodeTBL->rec[i].softErrCode == softErrCode)
			return(pgunionSoftErrCodeTBL->rec[i].cliErrCode);
	}
	return(pgunionSoftErrCodeTBL->defaultCliErrCode);
}

int UnionFindSoftErrCodeOfCliErrCode(char *errCode)
{
	int	i;
	int	ret;
		
	if ((ret = UnionConnectSoftErrCodeTBL()) < 0)
	{
		UnionUserErrLog("in UnionFindSoftErrCodeOfCliErrCode:: UnionConnectSoftErrCodeTBL!\n");
		return(errCodeDefaultErrCode);
	}
	
	for (i = 0; i < pgunionSoftErrCodeTBL->realNum; i++)
	{
		if (strcmp(pgunionSoftErrCodeTBL->rec[i].cliErrCode,errCode) == 0)
			return(pgunionSoftErrCodeTBL->rec[i].softErrCode);
	}
	return(pgunionSoftErrCodeTBL->defaultSoftErrCode);
}

int UnionPrintRecOfSoftErrCodeToFile(int softErrCode,FILE *fp)
{
	char	*cliErrCode;
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionFindCliErrCodeOfSoftErrCode:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
	if ((cliErrCode = UnionFindCliErrCodeOfSoftErrCode(softErrCode)) == NULL)
		return(0);
	else
	{
		fprintf(fp,"%06d %s\n",softErrCode,cliErrCode);
		return(1);
	}	
}

int UnionPrintAllSoftErrCodeOfCliErrCodeToFile(char *cliErrCode,FILE *fp)
{
	int	i;
	int	ret;
	int	num = 0;
	
	if ((fp == NULL) || (cliErrCode == NULL))
	{
		UnionUserErrLog("in UnionPrintAllSoftErrCodeOfCliErrCodeToFile:: fp/cliErrCode is NULL!\n");
		return(errCodeParameter);
	}
		
	if ((ret = UnionConnectSoftErrCodeTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAllSoftErrCodeOfCliErrCodeToFile:: UnionConnectSoftErrCodeTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionSoftErrCodeTBL->realNum; i++)
	{
		if (strcmp(pgunionSoftErrCodeTBL->rec[i].cliErrCode,cliErrCode) != 0)
			continue;
		if (UnionPrintSoftErrCodeToFile(&(pgunionSoftErrCodeTBL->rec[i]),fp) < 0)
			continue;
		num++;
		if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
	}
	fprintf(fp,"num = [%d]\n",num);
	return(num);
}

int UnionTranslateSoftErrCodeIntoCliErrCode(int softErrCode,char *cliErrCode)
{
	UnionLog("in UnionTranslateSoftErrCodeIntoCliErrCode:: softErrCode = [%d]\n",softErrCode);
	if (cliErrCode == NULL)
	{
		UnionUserErrLog("in UnionTranslateSoftErrCodeIntoCliErrCode:: null pointer!\n");
		return(errCodeParameter);
	}
	strcpy(cliErrCode,UnionFindCliErrCodeOfSoftErrCode(softErrCode));
	return(0);
}
int UnionPrintSoftErrCodeTBLToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintSoftErrCodeTBLToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	ret = UnionPrintSoftErrCodeTBLToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

PUnionSoftErrCode UnionFindSoftErrCode(char *errCode)
{
	int	i;
	int	ret;
	int	tmpErrCode;
	
	if (errCode == NULL)
		return(NULL);
			
	if ((ret = UnionConnectSoftErrCodeTBL()) < 0)
	{
		UnionUserErrLog("in UnionFindSoftErrCode:: UnionConnectSoftErrCodeTBL!\n");
		return(NULL);
	}
	
	for (i = 0; i < pgunionSoftErrCodeTBL->realNum; i++)
	{
		if (strcmp(pgunionSoftErrCodeTBL->rec[i].cliErrCode,errCode) == 0)
			return(&(pgunionSoftErrCodeTBL->rec[i]));
	}
	if (errCode[0] != '-')
		return(NULL);
	tmpErrCode = atoi(errCode);
	for (i = 0; i < pgunionSoftErrCodeTBL->realNum; i++)
	{
		if (pgunionSoftErrCodeTBL->rec[i].softErrCode == tmpErrCode)
			return(&(pgunionSoftErrCodeTBL->rec[i]));
	}
	return(NULL);
}

// 20060825 增加
int UnionExistSoftErrCodeDef(char *name)
{
	char	fileName[512+1];
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfSoftErrCodeTBL(fileName);
	
	return(UnionExistEnviVarInFile(fileName,name));
}


int UnionInsertSoftErrCode(int softErrCode,char *cliErrCode,char *remark)
{
	int	ret;
	char	fileName[512+1];
	char	varName[40+1];
		
	if ((cliErrCode == NULL) || (strlen(cliErrCode) == 0))
		return(errCodeParameter);
	
	// 检查是否在定义文件中存在
	sprintf(varName,"%d",softErrCode);
	if ((ret = UnionExistSoftErrCodeDef(varName)) > 0)
	{
		UnionUserErrLog("in UnionInsertSoftErrCodeDef:: UnionExistSoftErrCodeDef [%s]\n",varName);
		return(errCodeOffsetOfErrCodeMDL_CodeAlreadyDefined);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfSoftErrCodeTBL(fileName);
	if ((ret = UnionInsertEnviVar(fileName,varName,"[%s][%s]",cliErrCode,remark)) < 0)
	{
		UnionUserErrLog("in UnionInsertSoftErrCodeDef:: UnionInsertEnviVar [%s]\n",varName);
		return(ret);
	}

	// 重新加载定义文件
	if ((ret = UnionReloadSoftErrCodeTBL()) < 0)
	{
		UnionUserErrLog("in UnionInsertSoftErrCodeDef:: UnionReloadSoftErrCodeTBL!\n");
		return(ret);
	}
	
	return(ret);
}

// 20060825 增加
int UnionDeleteSoftErrCodeDef(int softErrCode)
{
	char	fileName[512+1];
	int	ret;
	char	varName[40+1];
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfSoftErrCodeTBL(fileName);
	sprintf(varName,"%d",softErrCode);
	if ((ret = UnionDeleteEnviVar(fileName,varName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteSoftErrCodeDef:: UnionDeleteEnviVar [%s]\n",varName);
		return(ret);
	}
	// 重新加载定义文件
	if ((ret = UnionReloadSoftErrCodeTBL()) < 0)
	{
		UnionUserErrLog("in UnionInsertSoftErrCodeDef:: UnionReloadSoftErrCodeTBL!\n");
		return(ret);
	}
	return(ret);	
}

// 20060825 增加
int UnionUpdateSoftErrCodeDef(int softErrCode,char *cliErrCode,char *remark)
{
	int		ret;
	char		fileName[512+1];
	char		varName[40+1];
		
	if ((cliErrCode == NULL) || (strlen(cliErrCode) == 0))
		return(errCodeParameter);
	
	
	// 更新文件中的定义
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfSoftErrCodeTBL(fileName);
	sprintf(varName,"%d",softErrCode);
	if ((ret = UnionUpdateEnviVar(fileName,varName,"[%s][%s]",cliErrCode,remark)) < 0)
	{
		UnionUserErrLog("in UnionUpdateSoftErrCodeDef:: UnionUpdateEnviVar [%s]\n",varName);
		return(ret);
	}
	// 重新加载定义文件
	if ((ret = UnionReloadSoftErrCodeTBL()) < 0)
	{
		UnionUserErrLog("in UnionInsertSoftErrCodeDef:: UnionReloadSoftErrCodeTBL!\n");
		return(ret);
	}
	return(ret);
}


