//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	1.0

#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_	
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
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

#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"

#include "unionCardBinTBL.h"

PUnionSharedMemoryModule	pgunionCardBinMDL = NULL;
PUnionCardBinTBL		pgunionCardBinTBL = NULL;

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
	int			i;
	int			realNum = 0;
	char			sql[256+1];
	char			tmpBuf[16+1];
	int			len = 0;
	int			iCnt = 0;
	
	if ((ret = UnionConnectCardBinTBL()) < 0)
	{
		UnionUserErrLog("in UnionReloadCardBinTBL:: UnionConnectCardBinTBL!\n");
		return(ret);
	}

	len = sprintf(sql,"select * from cardBin");	
	sql[len] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionReloadCardBinTBL:: UnionSelectRealDBRecord sql[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
		return 0;	
	
	// 读取总数量
	if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReloadCardBinTBL:: UnionReadXMLPackageValue[totalNum]\n");
		return(ret);
	}
	tmpBuf[ret] = 0;
	iCnt = atoi(tmpBuf);

	for (i = 0,realNum = 0; (i < iCnt) && (realNum < UnionGetMaxNumOfCardBin()); i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionReloadCardBinTBL:: UnionLocateXMLPackage!\n");
			continue;
		}

		if ((ret = UnionReadXMLPackageValue("cardNo",pgunionCardBinTBL->rec[realNum].cardBin,sizeof(pgunionCardBinTBL->rec[realNum].cardBin))) < 0)
		{
			UnionUserErrLog("in UnionReloadCardBinTBL:: UnionReadXMLPackageValue [%d] [cardBin]\n",i);
			continue;
		}

		// 读取cvkName
		if ((ret = UnionReadXMLPackageValue("cvkName",pgunionCardBinTBL->rec[realNum].cvkName,sizeof(pgunionCardBinTBL->rec[realNum].cvkName))) < 0)
		{
			UnionUserErrLog("in UnionReloadCardBinTBL:: UnionReadXMLPackageValue [%d] [cvkName]\n",i);
			continue;
		}

		// 读取algorithmOfCVV
		if ((ret = UnionReadXMLPackageValue("algorithmOfCVV",pgunionCardBinTBL->rec[realNum].algorithmOfCVV,sizeof(pgunionCardBinTBL->rec[realNum].algorithmOfCVV))) < 0)
		{
			UnionUserErrLog("in UnionReloadCardBinTBL:: UnionReadXMLPackageValue [%d] [algorithmOfCVV]\n",i);
			continue;
		}

		// 读取pvkName
		if ((ret = UnionReadXMLPackageValue("pvkName",pgunionCardBinTBL->rec[realNum].pvkName,sizeof(pgunionCardBinTBL->rec[realNum].pvkName))) < 0)
		{
			UnionUserErrLog("in UnionReloadCardBinTBL:: UnionReadXMLPackageValue [%d] [pvkName]\n",i);
			continue;
		}

		// 读取algorithmOfPVV
		if ((ret = UnionReadXMLPackageValue("algorithmOfPVV",pgunionCardBinTBL->rec[realNum].algorithmOfPVV,sizeof(pgunionCardBinTBL->rec[realNum].algorithmOfPVV))) < 0)
		{
			UnionUserErrLog("in UnionReloadCardBinTBL:: UnionReadXMLPackageValue [%d] [algorithmOfPVV]\n",i);
			continue;
		}

		// 读取remark
		if ((ret = UnionReadXMLPackageValue("remark",pgunionCardBinTBL->rec[realNum].remark,sizeof(pgunionCardBinTBL->rec[realNum].remark))) < 0)
		{
			UnionUserErrLog("in UnionReloadCardBinTBL:: UnionReadXMLPackageValue [%d] [remark]\n",i);
			continue;
		}

		if (strcasecmp(pgunionCardBinTBL->rec[realNum].cardBin,"default") == 0)
		{
			sprintf(pgunionCardBinTBL->defaultCvkName,"%s", pgunionCardBinTBL->rec[realNum].cvkName);
			sprintf(pgunionCardBinTBL->defaultPvkName,"%s", pgunionCardBinTBL->rec[realNum].pvkName);
			sprintf(pgunionCardBinTBL->defaultAlOfCVV,"%s", pgunionCardBinTBL->rec[realNum].algorithmOfCVV);
			sprintf(pgunionCardBinTBL->defaultAlOfPVV,"%s", pgunionCardBinTBL->rec[realNum].algorithmOfPVV);
		}
		realNum++;
	}
	pgunionCardBinTBL->realNum = realNum;

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
				UnionProgramerLog("in UnionGetPvkCvkByCardNo:: cardNo=[%s],cardBin=[%s],j=[%d]!\n",cardNo,pgunionCardBinTBL->rec[i].cardBin,j);
				sprintf(cvkName,"%s", pgunionCardBinTBL->rec[i].cvkName);
				sprintf(pvkName,"%s", pgunionCardBinTBL->rec[i].pvkName);
				sprintf(algorithmOfCVV,"%s", pgunionCardBinTBL->rec[i].algorithmOfCVV);
				sprintf(algorithmOfPVV,"%s", pgunionCardBinTBL->rec[i].algorithmOfPVV);
				return(0);
			}
		}
	}
	sprintf(cvkName,"%s", pgunionCardBinTBL->defaultCvkName);
	sprintf(pvkName,"%s", pgunionCardBinTBL->defaultPvkName);
	sprintf(algorithmOfCVV,"%s", pgunionCardBinTBL->defaultAlOfCVV);
	sprintf(algorithmOfPVV,"%s", pgunionCardBinTBL->defaultAlOfPVV);
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
/*
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
}*/
