//	Wolfgang Wang, 2006/8/9

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_	
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include "UnionProc.h"
#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "unionCommand.h"
#include "errCodeTranslater.h"
#include "unionErrCode.h"
#include "unionWorkingDir.h"
#include "unionRecFile.h"
#include "UnionStr.h"


/*读取错误代码到共享内存   20081204 by changzx */
#ifdef _useComplexDB_
#include "unionComplexDBRecord.h"
#endif

/* end  */

PUnionSharedMemoryModule	pgunionErrCodeMDL = NULL;
PUnionErrCodeTranslater		pgunionErrCodeTranslater = NULL;
char pgunionDefaultErrCodeRemark[] = "can't find remark for this code";

int UnionGetNameOfErrCodeTranslater(char *fileName)
{
#ifdef _WIN32
        char    mainDir[512+1];
        memset(mainDir,0,sizeof(mainDir));
        UnionGetMainWorkingDir(mainDir);
	sprintf(fileName,"%s/errCode/%s",mainDir,conConfFileNameOfErrCodeTranslater);
#else
	sprintf(fileName,"%s/errCode/%s",getenv("UNIONETC"),conConfFileNameOfErrCodeTranslater);
#endif
	return(0);
}

int UnionIsErrCodeTranslaterConnected()
{
	// 2007/10/26修改
	if ((pgunionErrCodeTranslater == NULL) || (!UnionIsSharedMemoryInited(conMDLNameOfErrCodeTranslater)))
		return(0);
	else
		return(1);
}

int UnionGetMaxNumOfErrCode()
{
	return(conMaxNumOfErrCode);	
}

int UnionConnectErrCodeTranslater()
{
	if (UnionIsErrCodeTranslaterConnected())	// 已经连接
		return(0);
	
	if ((pgunionErrCodeMDL = UnionConnectSharedMemoryModule(conMDLNameOfErrCodeTranslater,
			sizeof(TUnionErrCodeTranslater))) == NULL)
	{
		UnionUserErrLog("in UnionConnectErrCodeTranslater:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgunionErrCodeTranslater = (PUnionErrCodeTranslater)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionErrCodeMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectErrCodeTranslater:: PUnionErrCodeTranslater!\n");
		return(errCodeSharedMemoryModule);
	}
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionErrCodeMDL))
	{
		return(UnionReloadErrCodeTranslater());
	}
	else
		return(0);
}

	
int UnionDisconnectErrCodeTranslater()
{
	pgunionErrCodeTranslater = NULL;
	return(UnionDisconnectShareModule(pgunionErrCodeMDL));
}

int UnionRemoveErrCodeTranslater()
{
	UnionDisconnectErrCodeTranslater();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfErrCodeTranslater));
}

#ifndef _useComplexDB_
int UnionReloadErrCodeTranslater()
{
	int			ret;
	char			fileName[512];
	char			*p;
	int			i,j;
	int			realNum = 0;
	
	if ((ret = UnionConnectErrCodeTranslater()) < 0)
	{
		UnionUserErrLog("in UnionReloadErrCodeTranslater:: UnionConnectErrCodeTranslater!\n");
		return(ret);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfErrCodeTranslater(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionReloadErrCodeTranslater:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	for (i = 0,j = 0,realNum = 0; (i < UnionGetEnviVarNum()) && (realNum < UnionGetMaxNumOfErrCode()); i++,j = 0)
	{
		// 读取ErrCode
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,j)) == NULL)
		{
			UnionUserErrLog("in UnionReloadErrCodeTranslater:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,j);
			continue;
		}
		pgunionErrCodeTranslater->errCodeTBL[realNum].errCode = atoi(p);
		j++;
		// 读取remark
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,j)) == NULL)
		{
			UnionUserErrLog("in UnionReloadErrCodeTranslater:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,j);
			continue;
		}
		if (strlen(p) >= sizeof(pgunionErrCodeTranslater->errCodeTBL[realNum].remark))
		{
			memcpy(pgunionErrCodeTranslater->errCodeTBL[realNum].remark,p,sizeof(pgunionErrCodeTranslater->errCodeTBL[realNum].remark)-1);
			pgunionErrCodeTranslater->errCodeTBL[realNum].remark[sizeof(pgunionErrCodeTranslater->errCodeTBL[realNum].remark)-1] = 0;
		}
		else
			strcpy(pgunionErrCodeTranslater->errCodeTBL[realNum].remark,p);
		j++;
#ifdef _unionErrCodeTranslater_2_x_
		// 读取ErrCode
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,j)) == NULL)
			pgunionErrCodeTranslater->errCodeTBL[realNum].alaisCode = pgunionErrCodeTranslater->errCodeTBL[realNum].errCode;
		else
			pgunionErrCodeTranslater->errCodeTBL[realNum].alaisCode = atol(p);
#endif
		realNum++;
	}
	pgunionErrCodeTranslater->realNum = realNum;
	UnionClearEnvi();
	return(realNum);
}
#else
// 从complexDB中读取配置
/*读取错误代码到共享内存   20081204 by changzx */
int UnionReloadErrCodeTranslater()
{
	int		ret,iCnt,i = 0;
	char		fileName[512];
	char		tmpBuf[1024+1];
	int		tmpBufLen;
	FILE		*fp;

	//char	deleteFilename[1024+512];

	//UnionLog("in complexDB UnionReloadErrCodeTranslater!!\n");
	if ((ret = UnionConnectErrCodeTranslater()) < 0)
	{
		UnionUserErrLog("in UnionReloadErrCodeTranslater:: UnionConnectErrCodeTranslater!\n");
		return(ret);
	}

	memset(fileName,0,sizeof(fileName));
	ret = UnionGetTmpFileName((unsigned int)UnionGetPID(),fileName);
	if ( ret < 0 )
	{
		UnionUserErrLog("in UnionReloadErrCodeTranslater:: UnionGetTmpFileName!\n");
		return(ret);
	}

	iCnt = UnionBatchSelectObjectRecord("errCodeTranslater","",fileName);	
	if ( iCnt < 0 )
	{
		UnionUserErrLog("in UnionReloadErrCodeTranslater:UnionBatchSelectObjectRecord  error!!\n");
		UnionDeleteFile(fileName);
		return(iCnt);
	}
	
	//if ( iCnt > UnionGetMaxNumOfErrCode() )
	{
		iCnt = UnionGetMaxNumOfErrCode();
	}

	fp = fopen(fileName,"r");
	if ( fp == NULL )
	{
		UnionUserErrLog("in UnionReloadErrCodeTranslater:open file [%s]  error!!\n",fileName);
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
		ret = UnionReadIntTypeRecFldFromRecStr(tmpBuf,tmpBufLen,"errCode",&(pgunionErrCodeTranslater->errCodeTBL[i].errCode));
		if ( ret < 0 )
		{
			continue;	
		}
		ret = UnionReadRecFldFromRecStr(tmpBuf,tmpBufLen,"remark",pgunionErrCodeTranslater->errCodeTBL[i].remark,
			sizeof(pgunionErrCodeTranslater->errCodeTBL[i].remark));
		if ( ret < 0 )
		{
			memset(pgunionErrCodeTranslater->errCodeTBL[i].remark,0,sizeof(pgunionErrCodeTranslater->errCodeTBL[i].remark));
		}
		//UnionLog("[%8d]	[%s]\n",pgunionErrCodeTranslater->errCodeTBL[i].errCode,pgunionErrCodeTranslater->errCodeTBL[i].remark);
#ifdef _unionErrCodeTranslater_2_x_
		ret = UnionReadLongTypeRecFldFromRecStr(tmpBuf,tmpBufLen,"alaisCode",&(pgunionErrCodeTranslater->errCodeTBL[i].alaisCode));
		if ( ret < 0 )
		{
			UnionUserErrLog("in UnionReloadErrCodeTranslater: UnionReadLongTypeRecFldFromRecStr  read  pkcs11ErrCode error!!\n");
			pgunionErrCodeTranslater->errCodeTBL[i].alaisCode = -10000;
			return ret;
		}
#endif	
		i++;
	}
	
	fclose(fp);	
	UnionDeleteFile(fileName);
	pgunionErrCodeTranslater->realNum = i;
	//UnionLog("in UnionReloadErrCodeTranslater:: totalNum = [%d]\n",pgunionErrCodeTranslater->realNum);
	return(pgunionErrCodeTranslater->realNum);
}
#endif
/*end*/

int UnionPrintErrCodeTranslaterToFile(FILE *fp)
{
	int	i;
	int	ret;
	int	num = 0;
	
	if (fp == NULL)
		fp = stdout;
		
	if ((ret = UnionConnectErrCodeTranslater()) < 0)
	{
		UnionUserErrLog("in UnionPrintErrCodeTranslaterToFile:: UnionConnectErrCodeTranslater!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionErrCodeTranslater->realNum; i++)
	{
		if (UnionPrintErrCodeToFile(&(pgunionErrCodeTranslater->errCodeTBL[i]),fp) < 0)
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
	fprintf(fp,"ErrCodeNum = [%d]\n",num);
	return(0);
}

int UnionPrintErrCodeToFile(PUnionErrCode errCodeRec,FILE *fp)
{
	if ((errCodeRec == NULL) || (fp == NULL))
	{
		UnionUserErrLog("in UnionPrintErrCodeTranslaterToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
#ifndef _unionErrCodeTranslater_2_x_
	fprintf(fp,"%06d %s\n",errCodeRec->errCode,errCodeRec->remark);
#else
	fprintf(fp,"%06d %06ld %s\n",errCodeRec->errCode,errCodeRec->alaisCode,errCodeRec->remark);
#endif
	return(0);
}

PUnionErrCode UnionFindErrCode(int errCode)
{
	int	i;
	int	ret;
		
	if ((ret = UnionConnectErrCodeTranslater()) < 0)
	{
		UnionUserErrLog("in UnionFindErrCode:: UnionConnectErrCodeTranslater!\n");
		return(NULL);
	}
	
	for (i = 0; i < pgunionErrCodeTranslater->realNum; i++)
	{
		if (pgunionErrCodeTranslater->errCodeTBL[i].errCode == errCode)
			return(&(pgunionErrCodeTranslater->errCodeTBL[i]));
	}
	return(NULL);
}

// 2008/12/1，王纯军
long UnionFindAlaisCodeOfErrCode(int errCode)
{
#ifdef _unionErrCodeTranslater_2_x_
	PUnionErrCode	pcode;
	
	if ((pcode = UnionFindErrCode(errCode)) == NULL)
		return(errCode);
	else
		return(pcode->alaisCode);
#else
		return(errCode);
#endif
}

char *UnionFindRemarkOfErrCode(int ErrCode)
{
	int	i;
	int	ret;
		
	if ((ret = UnionConnectErrCodeTranslater()) < 0)
	{
		UnionUserErrLog("in UnionFindRemarkOfErrCode:: UnionConnectErrCodeTranslater!\n");
		return(pgunionDefaultErrCodeRemark);
	}
	
	for (i = 0; i < pgunionErrCodeTranslater->realNum; i++)
	{
#ifndef _unionErrCodeTranslater_2_x_
		if (pgunionErrCodeTranslater->errCodeTBL[i].errCode == ErrCode)
#else
		if ((pgunionErrCodeTranslater->errCodeTBL[i].errCode == ErrCode) || (pgunionErrCodeTranslater->errCodeTBL[i].alaisCode == ErrCode))
#endif
			return(pgunionErrCodeTranslater->errCodeTBL[i].remark);
	}
	return(pgunionDefaultErrCodeRemark);
}

int UnionPrintRecOfErrCodeToFile(int ErrCode,FILE *fp)
{
	char	*remark;
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionFindRemarkOfErrCode:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
	if ((remark = UnionFindRemarkOfErrCode(ErrCode)) == NULL)
		return(0);
	else
	{
#ifndef _unionErrCodeTranslater_2_x_
		fprintf(fp,"%06d %s\n",ErrCode,remark);
#else
		fprintf(fp,"%06d %06ld %s\n",ErrCode,UnionFindAlaisCodeOfErrCode(ErrCode),remark);
#endif
		return(1);
	}	
}

int UnionPrintAllErrCodeOfRemarkToFile(char *remark,FILE *fp)
{
	int	i;
	int	ret;
	int	num = 0;
	
	if ((fp == NULL) || (remark == NULL))
	{
		UnionUserErrLog("in UnionPrintAllErrCodeOfRemarkToFile:: fp/remark is NULL!\n");
		return(errCodeParameter);
	}
		
	if ((ret = UnionConnectErrCodeTranslater()) < 0)
	{
		UnionUserErrLog("in UnionPrintAllErrCodeOfRemarkToFile:: UnionConnectErrCodeTranslater!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionErrCodeTranslater->realNum; i++)
	{
		if (strcmp(pgunionErrCodeTranslater->errCodeTBL[i].remark,remark) != 0)
			continue;
		if (UnionPrintErrCodeToFile(&(pgunionErrCodeTranslater->errCodeTBL[i]),fp) < 0)
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

int UnionTranslateErrCodeIntoRemark(int ErrCode,char *remark,int sizeOfRemark)
{
	if (ErrCode == 0)
		return(0);
		
	if (remark == NULL)
	{
		UnionUserErrLog("in UnionTranslateErrCodeIntoRemark:: null pointer!\n");
		return(errCodeParameter);
	}
	strcpy(remark,UnionFindRemarkOfErrCode(ErrCode));
	return(strlen(remark));
}
int UnionPrintErrCodeTranslaterToSpecFile(char *fileName)
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
	ret = UnionPrintErrCodeTranslaterToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

// 20060825 增加
int UnionExistErrCodeDef(char *name)
{
	char	fileName[512+1];
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfErrCodeTranslater(fileName);
	
	return(UnionExistEnviVarInFile(fileName,name));
}


int UnionInsertErrCode(int errCode,char *remark)
{
	int	ret;
	char	fileName[512+1];
	char	varName[40+1];
		
	if ((remark == NULL) || (strlen(remark) == 0))
		return(errCodeParameter);
	
	// 检查是否在定义文件中存在
	sprintf(varName,"%d",errCode);
	if ((ret = UnionExistErrCodeDef(varName)) > 0)
	{
		UnionUserErrLog("in UnionInsertErrCodeDef:: UnionExistErrCodeDef [%s]\n",varName);
		return(errCodeOffsetOfErrCodeMDL_CodeAlreadyDefined);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfErrCodeTranslater(fileName);
	if ((ret = UnionInsertEnviVar(fileName,varName,"[%s]",remark)) < 0)
	{
		UnionUserErrLog("in UnionInsertErrCodeDef:: UnionInsertEnviVar [%s]\n",varName);
		return(ret);
	}

	// 重新加载定义文件
	if ((ret = UnionReloadErrCodeTranslater()) < 0)
	{
		UnionUserErrLog("in UnionInsertErrCodeDef:: UnionReloadErrCodeTranslater!\n");
		return(ret);
	}
	
	return(ret);
}

// 20060825 增加
int UnionDeleteErrCodeDef(int errCode)
{
	char	fileName[512+1];
	int	ret;
	char	varName[40+1];
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfErrCodeTranslater(fileName);
	sprintf(varName,"%d",errCode);
	if ((ret = UnionDeleteEnviVar(fileName,varName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteErrCodeDef:: UnionDeleteEnviVar [%s]\n",varName);
		return(ret);
	}
	// 重新加载定义文件
	if ((ret = UnionReloadErrCodeTranslater()) < 0)
	{
		UnionUserErrLog("in UnionInsertErrCodeDef:: UnionReloadErrCodeTranslater!\n");
		return(ret);
	}
	return(ret);	
}

// 20060825 增加
int UnionUpdateErrCodeDef(int errCode,char *remark)
{
	int		ret;
	char		fileName[512+1];
	char		varName[40+1];
		
	if ((remark == NULL) || (strlen(remark) == 0))
		return(errCodeParameter);
	
	
	// 更新文件中的定义
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfErrCodeTranslater(fileName);
	sprintf(varName,"%d",errCode);
	if ((ret = UnionUpdateEnviVar(fileName,varName,"[%s]",remark)) < 0)
	{
		UnionUserErrLog("in UnionUpdateErrCodeDef:: UnionUpdateEnviVar [%s]\n",varName);
		return(ret);
	}
	// 重新加载定义文件
	if ((ret = UnionReloadErrCodeTranslater()) < 0)
	{
		UnionUserErrLog("in UnionInsertErrCodeDef:: UnionReloadErrCodeTranslater!\n");
		return(ret);
	}
	return(ret);
}

// wangk add 2009-9-24
int UnionPrintErrCodeInRecStrToFile(PUnionErrCode errCodeRec,FILE *fp)
{
	if ((errCodeRec == NULL) || (fp == NULL))
	{
		UnionUserErrLog("in UnionPrintErrCodeInRecStrToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}

	fprintf(fp,"errCode=%d|remark=%s|\n",errCodeRec->errCode,errCodeRec->remark);

	return(0);
}

int UnionPrintErrCodeTranslaterInRecStrToFile(FILE *fp)
{
	int	i;
	int	ret;
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintErrCodeTranslaterInRecStrToFile:: Null pointer!\n");
		return(errCodeParameter);
	}

	if ((ret = UnionConnectErrCodeTranslater()) < 0)
	{
		UnionUserErrLog("in UnionPrintErrCodeTranslaterInRecStrToFile:: UnionConnectErrCodeTranslater!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionErrCodeTranslater->realNum; i++)
	{
		if (UnionPrintErrCodeInRecStrToFile(&(pgunionErrCodeTranslater->errCodeTBL[i]),fp) < 0)
			continue;

	}
	return(0);
}


int UnionPrintErrCodeTranslaterInRecStrToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
	{
		UnionUserErrLog("in UnionPrintErrCodeTranslaterInRecStrToSpecFile:: Null pointer!\n");
		return(errCodeParameter);
	}

	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintErrCodeTranslaterInRecStrToSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	ret = UnionPrintErrCodeTranslaterInRecStrToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}
// wangk add end 2009-9-24

