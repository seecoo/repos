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
#include <stdlib.h>
#include <unistd.h>

#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "UnionProc.h"
#include "unionRecFile.h"
#include "UnionStr.h"
#include "unionWorkingDir.h"
#include "unionREC.h"
#ifndef _WIN32
#include "unionCommand.h"
#endif

#include "unionErrCodeTBL.h"


/*读取错误代码到共享内存   20081204 by changzx */
#ifdef _useComplexDB_
#include "unionComplexDBRecord.h"
#endif
/* end  */

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
	//if ((pgunionSoftErrCodeTBL == NULL) || (pgunionSoftErrCodeMDL == NULL))
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


#ifndef _useComplexDB_
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
		j++;
#ifdef _withSoftErrCodeRemark_
		// 读取remark
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,j)) == NULL)
		{
			UnionUserErrLog("in UnionReloadSoftErrCodeTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,j);
			memset(pgunionSoftErrCodeTBL->rec[realNum].remark,0,sizeof(pgunionSoftErrCodeTBL->rec[realNum].remark));
		}
		else
		{
			if (strlen(p) >= sizeof(pgunionSoftErrCodeTBL->rec[realNum].remark))
			{
				memcpy(pgunionSoftErrCodeTBL->rec[realNum].remark,p,sizeof(pgunionSoftErrCodeTBL->rec[realNum].remark)-1);
				pgunionSoftErrCodeTBL->rec[realNum].remark[sizeof(pgunionSoftErrCodeTBL->rec[realNum].remark)-1] = 0;
			}
			else
				strcpy(pgunionSoftErrCodeTBL->rec[realNum].remark,p);
		}
#endif
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
#else
// 从complexDB中读取配置
/*读取错误代码到共享内存   20081204 by changzx */
int UnionReloadSoftErrCodeTBL()
{
	int	ret,iCnt,i = 0;
	char	fileName[512];
	char	tmpBuf[1024+1];
	char	remark[100];
	int	tmpBufLen;
	char	errCode[100];
	FILE	*fp;
	char	*p;
	
	//char	deleteFilename[1024+512];

	if ((ret = UnionConnectSoftErrCodeTBL()) < 0)
	{
		UnionUserErrLog("in UnionReloadSoftErrCodeTBL:: UnionConnectSoftErrCodeTBL!\n");
		return(ret);
	}

	memset(fileName,0,sizeof(fileName));
	ret = UnionGetTmpFileName((unsigned int)getpid(),fileName);
	if ( ret < 0 )
	{
		UnionUserErrLog("in UnionReloadSoftErrCodeTBL:: UnionGetTmpFileName!\n");
		return(ret);
	}

 
	iCnt = UnionBatchSelectObjectRecord("softErrCodeTBL","",fileName);	
	if ( iCnt < 0 )
	{
		UnionUserErrLog("in UnionReloadSoftErrCodeTBL:UnionBatchSelectObjectRecord  error!!\n");
		UnionDeleteFile(fileName);
		return(iCnt);
	}
	//UnionLog("in UnionReloadErrCodeTranslater tmpFileName :[%s] iCnt[%d]\n",fileName,iCnt);

	//if ( iCnt >  UnionGetMaxNumOfSoftErrCode())
	{
		iCnt = UnionGetMaxNumOfSoftErrCode();
	}

	fp = fopen(fileName,"r");
	if ( fp == NULL )
	{
		UnionUserErrLog("in UnionReloadSoftErrCodeTBL:open file [%s]  error!!\n",fileName);
		UnionDeleteFile(fileName);
		return(errCodeUseOSErrCode);	
	}

	while (!feof(fp))
	{
		if (i >= iCnt)
			break;
		memset(tmpBuf,0,sizeof(tmpBuf));
		
		tmpBufLen = UnionReadOneDataLineFromTxtFile(fp,tmpBuf,sizeof(tmpBuf));
		
		if (tmpBufLen <= 0)
			continue;
		memset(errCode,0,sizeof(errCode));
		ret = UnionReadRecFldFromRecStr(tmpBuf,tmpBufLen,"softErrCode",errCode,sizeof(errCode));
		if ( ret < 0 )
		{
			UnionUserErrLog("in UnionReloadSoftErrCodeTBL : UnionReadRecFldFromRecStr [%s]\n",tmpBuf);
			continue;
		}
		
		pgunionSoftErrCodeTBL->rec[i].softErrCode = atoi(errCode);
		ret = UnionReadRecFldFromRecStr(tmpBuf,tmpBufLen,"cliErrCode",errCode,sizeof(errCode));
		if ( ret < 0 )
		{
			memset(pgunionSoftErrCodeTBL->rec[i].cliErrCode,0,sizeof(pgunionSoftErrCodeTBL->rec[i].cliErrCode));
		}
		
		strcpy(pgunionSoftErrCodeTBL->rec[i].cliErrCode,errCode);
		//UnionLog("[%s] [%d]\n",	pgunionSoftErrCodeTBL->rec[i].cliErrCode,pgunionSoftErrCodeTBL->rec[i].softErrCode);
		
#ifdef _withSoftErrCodeRemark_
		memset(remark,0,sizeof(remark));
		ret = UnionReadRecFldFromRecStr(tmpBuf,tmpBufLen,"remark",remark,sizeof(remark));
		if ( ret < 0 )
		{
			memset(pgunionSoftErrCodeTBL->rec[i].remark,0,sizeof(pgunionSoftErrCodeTBL->rec[i].remark));
		}
		else
		{
		
			if (strlen(remark) >= sizeof(pgunionSoftErrCodeTBL->rec[i].remark))
			{
				memcpy(pgunionSoftErrCodeTBL->rec[i].remark,remark,sizeof(pgunionSoftErrCodeTBL->rec[i].remark)-1);
				pgunionSoftErrCodeTBL->rec[i].remark[sizeof(pgunionSoftErrCodeTBL->rec[i].remark)-1] = 0;
			}
			else
				strcpy(pgunionSoftErrCodeTBL->rec[i].remark,remark);
		}
#endif
		i++;
	
	}

	fclose(fp);
	UnionDeleteFile(fileName); 
	pgunionSoftErrCodeTBL->realNum = i;
	UnionLog("in UnionReloadSoftErrCodeTBL:: num = [%d]\n",pgunionSoftErrCodeTBL->realNum);

	// add by lix,20091116
	if ((p = UnionReadStringTypeRECVar("defaultCliErrCode")) == NULL)
		pgunionSoftErrCodeTBL->defaultSoftErrCode = errCodeDefaultErrCode;
	else
		pgunionSoftErrCodeTBL->defaultSoftErrCode = atoi(p);
	strcpy(pgunionSoftErrCodeTBL->defaultCliErrCode,"UN");
	return(pgunionSoftErrCodeTBL->realNum);
}
#endif
/*add end*/



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
#ifndef _WIN32
		if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
#endif
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

#ifdef _withSoftErrCodeRemark_	
	fprintf(fp,"%06d %s %s\n",pylqz_cliErrCode->softErrCode,pylqz_cliErrCode->cliErrCode,pylqz_cliErrCode->remark);
#else
	fprintf(fp,"%06d %s\n",pylqz_cliErrCode->softErrCode,pylqz_cliErrCode->cliErrCode);
#endif
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
#ifndef _WIN32
		if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
#endif
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

// wangk add 2009-9-24
int UnionPrintSoftErrCodeInRecStrToFile(PUnionSoftErrCode pylqz_cliErrCode,FILE *fp)
{
	if ((pylqz_cliErrCode == NULL) || (fp == NULL))
	{
		UnionUserErrLog("in UnionPrintSoftErrCodeInRecStrToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}

	fprintf(fp,"softErrCode=%d|cliErrCode=%s|remark=%s|\n",pylqz_cliErrCode->softErrCode,pylqz_cliErrCode->cliErrCode,pylqz_cliErrCode->remark);

	return(0);
}

int UnionPrintSoftErrCodeTBLInRecStrToFile(FILE *fp)
{
	int	i;
	int	ret;
	int	num = 0;
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintSoftErrCodeTBLInRecStrToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
		
	if ((ret = UnionConnectSoftErrCodeTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintSoftErrCodeTBLInRecStrToFile:: UnionConnectSoftErrCodeTBL!\n");
		return(ret);
	}

	for (i = 0; i < pgunionSoftErrCodeTBL->realNum; i++)
	{
		if (UnionPrintSoftErrCodeInRecStrToFile(&(pgunionSoftErrCodeTBL->rec[i]),fp) < 0)
			continue;
		num++;

	}
	return(0);
}

int UnionPrintSoftErrCodeTBLInRecStrToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
	{
		UnionUserErrLog("in UnionPrintSoftErrCodeTBLInRecStrToSpecFile:: NULL poionter!\n");
		return(errCodeParameter);
	}

	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintSoftErrCodeTBLInRecStrToSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	ret = UnionPrintSoftErrCodeTBLInRecStrToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}
// wangk add end 2009-9-24
