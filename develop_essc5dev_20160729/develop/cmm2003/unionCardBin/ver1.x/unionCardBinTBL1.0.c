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

#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "UnionProc.h"
#include "unionRecFile.h"
#include "UnionStr.h"
#include "unionWorkingDir.h"
#include "unionErrCode.h"
#ifndef _WIN32
#include "unionCommand.h"
#endif

#include "unionCardBinTBL.h"

PUnionSharedMemoryModule	pgunionCardBinMDL = NULL;
PUnionCardBinTBL		pgunionCardBinTBL = NULL;

int UnionGetNameOfCardBinTBL(char *fileName)
{
	sprintf(fileName,"%s/cardBin/%s",getenv("UNIONETC"),conConfFileNameOfCardBinTBL);
	return(0);
}

int UnionIsCardBinTBLConnected()
{
	// 2007/10/26修改
	if ((pgunionCardBinTBL == NULL) || (!UnionIsSharedMemoryInited(conMDLNameOfCardBinTBL)))
	//if ((pgunionCardBinTBL == NULL) || (pgunionCardBinMDL == NULL))
		return(0);
	else
		return(1);
}

int UnionGetMaxNumOfCardBin()
{
	return(conMaxNumOfCardBin);	
}

int UnionConnectCardBinTBL()
{
	int				ret;
	
	if (UnionIsCardBinTBLConnected())	// 已经连接
		return(0);
	
	if ((pgunionCardBinMDL = UnionConnectSharedMemoryModule(conMDLNameOfCardBinTBL, sizeof(TUnionCardBinTBL))) == NULL)
	{
		UnionUserErrLog("in UnionConnectCardBinTBL:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgunionCardBinTBL = (PUnionCardBinTBL)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionCardBinMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectCardBinTBL:: PUnionCardBinTBL!\n");
		return(errCodeSharedMemoryModule);
	}
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionCardBinMDL))
	{
		return(UnionReloadCardBinTBL());
	}
	else
		return(0);
}


int UnionDisconnectCardBinTBL()
{
	pgunionCardBinTBL = NULL;
	return(UnionDisconnectShareModule(pgunionCardBinMDL));
}

int UnionRemoveCardBinTBL()
{
	UnionDisconnectCardBinTBL();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfCardBinTBL));
}


int UnionReloadCardBinTBL()
{
	int			ret;
	char			fileName[512];
	char			*p;
	int			i,j;
	int			realNum = 0;
	
	if ((ret = UnionConnectCardBinTBL()) < 0)
	{
		UnionUserErrLog("in UnionReloadCardBinTBL:: UnionConnectCardBinTBL!\n");
		return(ret);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfCardBinTBL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionReloadCardBinTBL:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	for (i = 0,j = 0,realNum = 0; (i < UnionGetEnviVarNum()) && (realNum < UnionGetMaxNumOfCardBin()); i++,j = 0)
	{
		// 读取cardBin
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,j)) == NULL)
		{
			UnionUserErrLog("in UnionReloadCardBinTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,j);
			continue;
		}

		if ( (strcmp(p,"defaultCvkName") == 0) || 
		     (strcmp(p,"defaultPvkName") == 0) ||
		     (strcmp(p,"defaultAlOfCVV") == 0) ||
		     (strcmp(p,"defaultAlOfPVV") == 0) )
                        continue;

		if (strlen(p) > sizeof(pgunionCardBinTBL->rec[realNum].cardBin) - 1)
			continue;
		strcpy(pgunionCardBinTBL->rec[realNum].cardBin,p);
		j++;

		// 读取cvkName
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,j)) == NULL)
		{
			UnionUserErrLog("in UnionReloadCardBinTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,j);
			continue;
		}
		if (strlen(p) > sizeof(pgunionCardBinTBL->rec[realNum].cvkName) - 1)
			continue;
		strcpy(pgunionCardBinTBL->rec[realNum].cvkName,p);
		j++;

		// 读取algorithmOfCVV
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,j)) == NULL)
		{
			UnionUserErrLog("in UnionReloadCardBinTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,j);
			continue;
		}
		if (strlen(p) > sizeof(pgunionCardBinTBL->rec[realNum].algorithmOfCVV) - 1)
			continue;
		strcpy(pgunionCardBinTBL->rec[realNum].algorithmOfCVV,p);
		j++;

		// 读取pvkName
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,j)) == NULL)
		{
			UnionUserErrLog("in UnionReloadCardBinTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,j);
			continue;
		}
		if (strlen(p) > sizeof(pgunionCardBinTBL->rec[realNum].pvkName) - 1)
			continue;
		strcpy(pgunionCardBinTBL->rec[realNum].pvkName,p);
		j++;

		// 读取algorithmOfPVV
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,j)) == NULL)
		{
			UnionUserErrLog("in UnionReloadCardBinTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,j);
			continue;
		}
		if (strlen(p) > sizeof(pgunionCardBinTBL->rec[realNum].algorithmOfPVV) - 1)
			continue;
		strcpy(pgunionCardBinTBL->rec[realNum].algorithmOfPVV,p);
		j++;

		// 读取remark
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,j)) == NULL)
		{
			UnionUserErrLog("in UnionReloadCardBinTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,j);
			memset(pgunionCardBinTBL->rec[realNum].remark, 0, sizeof pgunionCardBinTBL->rec[realNum].remark);
		}
		else
		{
			if (strlen(p) > sizeof(pgunionCardBinTBL->rec[realNum].remark) - 1)
				continue;
			strcpy(pgunionCardBinTBL->rec[realNum].remark,p);
		}

		realNum++;
	}
	pgunionCardBinTBL->realNum = realNum;

	if ((p = UnionGetEnviVarByName("defaultCvkName")) != NULL)
	{
		if (strlen(p) >= sizeof(pgunionCardBinTBL->defaultCvkName))
			memcpy(pgunionCardBinTBL->defaultCvkName,p,sizeof(pgunionCardBinTBL->defaultCvkName)-1);
		else
			strcpy(pgunionCardBinTBL->defaultCvkName,p);
	}
	else
	{
		UnionUserErrLog("in UnionReloadCardBinTBL:: UnionGetEnviVarByName [%s]!\n", "defaultCvkName");
		return (-1);
	}

	if ((p = UnionGetEnviVarByName("defaultPvkName")) != NULL)
	{
		if (strlen(p) >= sizeof(pgunionCardBinTBL->defaultPvkName))
			memcpy(pgunionCardBinTBL->defaultPvkName,p,sizeof(pgunionCardBinTBL->defaultPvkName)-1);
		else
			strcpy(pgunionCardBinTBL->defaultPvkName,p);
	}
	else
	{
		UnionUserErrLog("in UnionReloadCardBinTBL:: UnionGetEnviVarByName [%s]!\n", "defaultPvkName");
		return (-1);
	}

	if ((p = UnionGetEnviVarByName("defaultAlOfCVV")) != NULL)
	{
		if (strlen(p) >= sizeof(pgunionCardBinTBL->defaultAlOfCVV))
			memcpy(pgunionCardBinTBL->defaultAlOfCVV,p,sizeof(pgunionCardBinTBL->defaultAlOfCVV)-1);
		else
			strcpy(pgunionCardBinTBL->defaultAlOfCVV,p);
	}
	else
	{
		UnionUserErrLog("in UnionReloadCardBinTBL:: UnionGetEnviVarByName [%s]!\n", "defaultAlOfCVV");
		return (-1);
	}

	if ((p = UnionGetEnviVarByName("defaultAlOfPVV")) != NULL)
	{
		if (strlen(p) >= sizeof(pgunionCardBinTBL->defaultAlOfPVV))
			memcpy(pgunionCardBinTBL->defaultAlOfPVV,p,sizeof(pgunionCardBinTBL->defaultAlOfPVV)-1);
		else
			strcpy(pgunionCardBinTBL->defaultAlOfPVV,p);
	}
	else
	{
		UnionUserErrLog("in UnionReloadCardBinTBL:: UnionGetEnviVarByName [%s]!\n", "defaultAlOfPVV");
		return (-1);
	}
	
	UnionClearEnvi();
	return(realNum);
}

int UnionPrintCardBinTBLToFile(FILE *fp)
{
	int	i;
	int	ret;
	int	num = 0;
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintCardBinTBLToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
		
	if ((ret = UnionConnectCardBinTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintCardBinTBLToFile:: UnionConnectCardBinTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionCardBinTBL->realNum; i++)
	{
		if (UnionPrintCardBinToFile(&(pgunionCardBinTBL->rec[i]),fp) < 0)
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
	fprintf(fp,"defaultCvkName  = [%s]\n",pgunionCardBinTBL->defaultCvkName);
	fprintf(fp,"defaultPvkName  = [%s]\n",pgunionCardBinTBL->defaultPvkName);
	fprintf(fp,"defaultAlOfCVV  = [%s]\n",pgunionCardBinTBL->defaultAlOfCVV);
	fprintf(fp,"defaultAlOfPVV  = [%s]\n",pgunionCardBinTBL->defaultAlOfPVV);
	fprintf(fp,"CardBinNum     = [%d]\n",num);
	return(0);
}

int UnionPrintCardBinToFile(PUnionCardBin cardBin,FILE *fp)
{
	int			i;
	
	if ((cardBin == NULL) || (fp == NULL))
	{
		UnionUserErrLog("in UnionPrintCardBinTBLToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}

	fprintf(fp,"%s %s %s %s %s %s\n", cardBin->cardBin, cardBin->cvkName, cardBin->algorithmOfCVV, cardBin->pvkName, cardBin->algorithmOfPVV, cardBin->remark);

	return(0);
}

/* 根据卡号取cvk */
int UnionGetCvkByCardNo(char *cardNo, char *cvkName, char *algorithmOfCVV)
{
	char 	pvkName[40+1];
	char 	algorithmOfPVV[2+1];
	
	memset(pvkName, 0, sizeof pvkName);
	memset(algorithmOfPVV, 0, sizeof algorithmOfPVV);

	return (UnionGetPvkCvkByCardNo(cardNo, cvkName, pvkName, algorithmOfCVV,algorithmOfPVV));
}

/* 根据卡号取pvk */
int UnionGetPvkByCardNo(char *cardNo, char *pvkName, char *algorithmOfPVV)
{
	char 	cvkName[40+1];
	char 	algorithmOfCVV[40+1];
	
	memset(cvkName, 0, sizeof cvkName);
	memset(algorithmOfCVV, 0, sizeof algorithmOfCVV);

	return (UnionGetPvkCvkByCardNo(cardNo, cvkName, pvkName, algorithmOfCVV,algorithmOfPVV));
}

/* 根据卡号取cvk和pvk */
int UnionGetPvkCvkByCardNo(char *cardNo, char *cvkName, char *pvkName, char *algorithmOfCVV, char *algorithmOfPVV)
{
	int	i,j;
	int	ret;
		
	if ((ret = UnionConnectCardBinTBL()) < 0)
	{
		UnionUserErrLog("in UnionGetPvkCvkByCardNo:: UnionConnectCardBinTBL!\n");
		return(errCodeDefaultErrCode);
	}
	
	for(j=9; j>=6; j--)
	{
		for (i = 0; i < pgunionCardBinTBL->realNum; i++)
		{
			if ( (strlen(pgunionCardBinTBL->rec[i].cardBin)==j) 
			&& (memcmp(cardNo, pgunionCardBinTBL->rec[i].cardBin, j) == 0) )
			{
	//	UnionUserErrLog("1111111cardNo=[%s],cardBin=[%s],j=[%d]!\n",cardNo,pgunionCardBinTBL->rec[i].cardBin,j);
		UnionProgramerLog("1111111cardNo=[%s],cardBin=[%s],j=[%d]!\n",cardNo,pgunionCardBinTBL->rec[i].cardBin,j);
				strcpy(cvkName, pgunionCardBinTBL->rec[i].cvkName);
				strcpy(pvkName, pgunionCardBinTBL->rec[i].pvkName);
				strcpy(algorithmOfCVV, pgunionCardBinTBL->rec[i].algorithmOfCVV);
				strcpy(algorithmOfPVV, pgunionCardBinTBL->rec[i].algorithmOfPVV);
				return(0);
			}
		}
	}
	strcpy(cvkName, pgunionCardBinTBL->defaultCvkName);
	strcpy(pvkName, pgunionCardBinTBL->defaultPvkName);
	strcpy(algorithmOfCVV, pgunionCardBinTBL->defaultAlOfCVV);
	strcpy(algorithmOfPVV, pgunionCardBinTBL->defaultAlOfPVV);
	return(0);
}

/* 功能：根据二磁取卡信息
  输入：stack2 - 二磁
  输出：cardNo - 卡号；period - 有效期；service - 服务码；
        CVN - 卡CVN
  返回：0 - 成功；<0 - 失败
*/
int UnionGetCardInfoFromStack2(char * Stack2, int lenOfStack2, char *cardNo,char *period,char *service,char *CVN)
{
	char	*p;
	if( (Stack2==NULL) || (cardNo==NULL) || (period==NULL) || (service==NULL) || (CVN==NULL) )
		return(errCodeParameter);
	
	p=strchr(Stack2,'=');
	if (p == NULL)
	{
		UnionUserErrLog("in UnionGetCardInfoFromStack2:: Stack2 err\n");
		return(errCodeParameter);
	}
	memcpy(cardNo, Stack2, p-Stack2);
	p++;
	if ( (p-Stack2+10) > lenOfStack2 )
	{
		UnionUserErrLog("in unionGetCardInfoFromStack2:: Stack2 err\n");
		return(errCodeParameter);
	}
	memcpy(period, p, 4);
	p += 4;
	memcpy(service, p, 3);
	p += 3;
	memcpy(CVN, p, 3);

	return 0;
}


int UnionPrintCardBinTBLToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintCardBinTBLToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	ret = UnionPrintCardBinTBLToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

// 20060825 增加
int UnionExistCardBinDef(char *name)
{
	char	fileName[512+1];
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfCardBinTBL(fileName);
	
	return(UnionExistEnviVarInFile(fileName,name));
}


int UnionInsertCardBin(char *cardBin,char *cvkName,char *pvkName, char *algorithmOfCVV, char *algorithmOfPVV, char *remark)
{
	int	ret;
	char	fileName[512+1];
	char	varName[40+1];
		
	if ((cardBin == NULL) || (cvkName == NULL) || (pvkName == NULL) || (algorithmOfCVV == NULL) || (algorithmOfPVV == NULL))
		return(errCodeParameter);
	
	// 检查是否在定义文件中存在
	strcpy(varName, cardBin);
	if ((ret = UnionExistCardBinDef(varName)) > 0)
	{
		UnionUserErrLog("in UnionInsertCardBinDef:: UnionExistCardBinDef [%s]\n",varName);
		return(errCodeOffsetOfErrCodeMDL_CodeAlreadyDefined);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfCardBinTBL(fileName);
	if ((ret = UnionInsertEnviVar(fileName,varName,"[%s][%s][%s][%s][%s]",cvkName,pvkName,algorithmOfCVV,algorithmOfPVV,remark)) < 0)
	{
		UnionUserErrLog("in UnionInsertCardBinDef:: UnionInsertEnviVar [%s]\n",varName);
		return(ret);
	}

	// 重新加载定义文件
	if ((ret = UnionReloadCardBinTBL()) < 0)
	{
		UnionUserErrLog("in UnionInsertCardBinDef:: UnionReloadCardBinTBL!\n");
		return(ret);
	}
	
	return(ret);
}

// 20060825 增加
int UnionDeleteCardBinDef(char *cardBin)
{
	char	fileName[512+1];
	int	ret;
	char	varName[40+1];
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfCardBinTBL(fileName);
	strcpy(varName,cardBin);
	if ((ret = UnionDeleteEnviVar(fileName,varName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteCardBinDef:: UnionDeleteEnviVar [%s]\n",varName);
		return(ret);
	}
	// 重新加载定义文件
	if ((ret = UnionReloadCardBinTBL()) < 0)
	{
		UnionUserErrLog("in UnionInsertCardBinDef:: UnionReloadCardBinTBL!\n");
		return(ret);
	}
	return(ret);	
}

// 20060825 增加
int UnionUpdateCardBinDef(char *cardBin,char *cvkName,char *pvkName, char *algorithmOfCVV, char *algorithmOfPVV, char *remark)
{
	int		ret;
	char		fileName[512+1];
	char		varName[40+1];
		
	if ((cardBin == NULL) || (cvkName == NULL) || (pvkName == NULL) || (algorithmOfCVV == NULL) || (algorithmOfPVV == NULL))
		return(errCodeParameter);
	
	
	// 更新文件中的定义
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfCardBinTBL(fileName);
	strcpy(varName, cardBin);
	if ((ret = UnionUpdateEnviVar(fileName,varName,"[%s][%s][%s][%s][%s]",cvkName,pvkName,algorithmOfCVV,algorithmOfPVV,remark)) < 0)

	{
		UnionUserErrLog("in UnionUpdateCardBinDef:: UnionUpdateEnviVar [%s]\n",varName);
		return(ret);
	}
	// 重新加载定义文件
	if ((ret = UnionReloadCardBinTBL()) < 0)
	{
		UnionUserErrLog("in UnionInsertCardBinDef:: UnionReloadCardBinTBL!\n");
		return(ret);
	}
	return(ret);
}
