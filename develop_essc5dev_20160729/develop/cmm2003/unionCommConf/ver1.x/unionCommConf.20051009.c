//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2005/08/18

#ifndef _UnionEnv_3_x_	
#define _UnionEnv_3_x_	
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "UnionStr.h"
#include "unionModule.h"
#include "unionErrCode.h"
#include "UnionEnv.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionREC.h"
#include "unionCommand.h"

#include "unionCommConf.h"

PUnionSharedMemoryModule	pgunionCommConfTBLMDL = NULL;
PUnionCommConfTBL		pgunionCommConfTBL = NULL;

// 2006/08/08
int UnionGetMaxNumOfCommConfTBL()
{
	return(conMaxNumOfConn);
}

// 2006/08/08
PUnionCommConf UnionGetCurrentCommConfGrp()
{
	int	ret;
	
	if ((ret = UnionConnectCommConfTBL()) < 0)
		return(NULL);
	else
		return(pgunionCommConfTBL->rec);
}

int UnionIsCommConfNormal(PUnionCommConf pcommConf)
{
	if (pcommConf == NULL)
		return(0);
	if (pcommConf->connNum > 0)
		return(1);
	if (pcommConf->connType == conCommShortConn)
		return(1);
#ifndef _longConnServerMustHaveConns_
	if (pcommConf->procType == conCommServer)
		return(1);
#endif
	return(0);
}

int UnionIncreaseCommConfWorkingTimes(PUnionCommConf pcommConf)
{
	if (pcommConf == NULL)
		return(errCodeParameter);
	if (++(pcommConf->totalNum) <= 0)
		pcommConf->totalNum = 1;
	time(&(pcommConf->lastWorkingTime));
	return(0);
}


int UnionSetCommConfOK(PUnionCommConf pcommConf)
{
	if (pcommConf == NULL)
		return(errCodeParameter);
	if (++(pcommConf->connNum) <= 0)
		pcommConf->connNum = 1;
	if (pcommConf->connType == conCommLongConn)
		time(&(pcommConf->lastWorkingTime));
	return(0);
}

int UnionSetCommConfAbnormal(PUnionCommConf pcommConf)
{
	if (pcommConf == NULL)
		return(errCodeParameter);
	if (--(pcommConf->connNum) < 0)
		pcommConf->connNum = 0;
	return(0);
}

int UnionIsValidCommProcType(TUnionCommProcType commType)
{
	switch (commType)
	{
		case	conCommClient:
		case	conCommServer:
			return(1);
		default:
			return(0);
	}
}

int UnionIsValidCommConnType(TUnionCommConnType commType)
{
	switch (commType)
	{
		case	conCommShortConn:
		case	conCommLongConn:
			return(1);
		default:
			return(0);
	}
}
int UnionGetNameOfCommConfTBL(char *fileName)
{
	sprintf(fileName,"%s/%s",getenv("UNIONETC"),conConfFileNameOfComm);
	return(0);
}

int UnionIsCommConfTBLConnected()
{
	// 2007/10/26修改
	if ((pgunionCommConfTBL == NULL) || (!UnionIsSharedMemoryInited(conMDLNameOfCommConfTBL)))
		return(0);
	else
		return(1);
}

// Module Layer Functions
int UnionConnectCommConfTBL()
{
	PUnionCommConf		punionCommConf;
	int	i;
	
	if (UnionIsCommConfTBLConnected())	// 已经连接
		return(0);
		
	if ((pgunionCommConfTBLMDL = UnionConnectSharedMemoryModule(conMDLNameOfCommConfTBL,
			sizeof(TUnionCommConfTBL))) == NULL)
	{
		UnionUserErrLog("in UnionConnectCommConfTBL:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgunionCommConfTBL = (PUnionCommConfTBL)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionCommConfTBLMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectCommConfTBL:: PUnionSJL06!\n");
		return(errCodeSharedMemoryModule);
	}
	

	if (UnionIsNewCreatedSharedMemoryModule(pgunionCommConfTBLMDL))
	{
		for (i = 0; i < conMaxNumOfConn; i++)
		{
			punionCommConf = &(pgunionCommConfTBL->rec[i]);
			memset(punionCommConf,0,sizeof(*punionCommConf));
		}
	}
	return(0);
}

int UnionDisconnectCommConfTBL()
{
	pgunionCommConfTBL = NULL;
	return(UnionDisconnectShareModule(pgunionCommConfTBLMDL));
}

int UnionRemoveCommConfTBL()
{
	UnionDisconnectCommConfTBL();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfCommConfTBL));
}

int UnionReloadCommConfTBL()
{
	return(UnionConnectCommConfTBL());
}

int UnionPrintCommConfToFile(PUnionCommConf punionCommConf,FILE *fp)
{
	time_t		now;
	
	if ((punionCommConf == NULL) || (fp == NULL))
		return(errCodeParameter);
	if (strlen(punionCommConf->ipAddr) == 0)
		return(errCodeParameter);
	switch (punionCommConf->procType)
	{
		case	conCommServer:
			fprintf(fp,"服务器");
			break;
		case	conCommClient:
			fprintf(fp,"客户端");
			break;
		default:
			fprintf(fp,"      ");
			break;
	}
	switch (punionCommConf->connType)
	{
		case	conCommShortConn:
			fprintf(fp," 短连接");
			break;
		case	conCommLongConn:
			fprintf(fp," 长连接");
			break;
		default:
			fprintf(fp,"       ");
			break;
	}
	time(&now);
	fprintf(fp," %15s %5d %10s %6ld %10ld %6ld\n",punionCommConf->ipAddr,punionCommConf->port,punionCommConf->remark,punionCommConf->connNum,
						punionCommConf->totalNum,now - punionCommConf->lastWorkingTime);
	return(0);
}

int UnionPrintCommConfTBLToFile(FILE *fp)
{
	int		index;
	int		ret;
	int		num = 0;
		
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintCommConfTBLToFile:: fp is null!\n");
		return(errCodeParameter);
	}
		
	if ((ret = UnionConnectCommConfTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintCommConfTBLToFile:: UnionConnectCommConfTBL!\n");
		return(ret);
	}
	for (index = 0; index < conMaxNumOfConn; index++)
	{
		if (UnionPrintCommConfToFile(&(pgunionCommConfTBL->rec[index]),fp) < 0)
			continue;
		num++;
		if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
	}
	fprintf(fp,"num = [%d]\n",num);
	return(num);
}

int UnionPrintCommConfOfIPAddrToFile(char *ipAddr,FILE *fp)
{
	int		index;
	int		num = 0;
	int		ret;
	
	if ((fp == NULL) || (ipAddr == NULL))
	{
		UnionUserErrLog("in UnionPrintCommConfOfIPAddrToFile:: fp is null!\n");
		return(errCodeParameter);
	}
		
	if ((ret = UnionConnectCommConfTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintCommConfOfIPAddrToFile:: UnionConnectCommConfTBL!\n");
		return(ret);
	}
	for (index = 0; index < conMaxNumOfConn; index++)
	{
		if (strcmp(pgunionCommConfTBL->rec[index].ipAddr,ipAddr) == 0)
		{	
			if (UnionPrintCommConfToFile(&(pgunionCommConfTBL->rec[index]),fp) < 0)
				continue;
			num++;
		}
	}
	fprintf(fp,"num = [%d]\n",num);
	return(num);
}

int UnionPrintCommConfOfRemarkToFile(char *remark,FILE *fp)
{
	int		index;
	int		num = 0;
	int		ret;
	
	if ((fp == NULL) || (remark == NULL) || (strlen(remark) == 0))
	{
		UnionUserErrLog("in UnionPrintCommConfOfRemarkToFile:: fp is null!\n");
		return(errCodeParameter);
	}
		
	if ((ret = UnionConnectCommConfTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintCommConfOfRemarkToFile:: UnionConnectCommConfTBL!\n");
		return(ret);
	}
	for (index = 0; index < conMaxNumOfConn; index++)
	{
		if (strlen(pgunionCommConfTBL->rec[index].ipAddr) == 0)
			continue;
		if (strcmp(pgunionCommConfTBL->rec[index].remark,remark) == 0)
		{	
			if (UnionPrintCommConfToFile(&(pgunionCommConfTBL->rec[index]),fp) < 0)
				continue;
			num++;
			if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
				if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
					break;
		}
	}
	fprintf(fp,"num = [%d]\n",num);
	return(num);
}

int UnionPrintCommConfOfPortToFile(int port,FILE *fp)
{
	int		index;
	int		num = 0;
	int		ret;
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintCommConfOfPortToFile:: fp is null!\n");
		return(errCodeParameter);
	}
		
	if ((ret = UnionConnectCommConfTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintCommConfOfPortToFile:: UnionConnectCommConfTBL!\n");
		return(ret);
	}
	for (index = 0; index < conMaxNumOfConn; index++)
	{
		if (strlen(pgunionCommConfTBL->rec[index].ipAddr) == 0)
			continue;
		if (pgunionCommConfTBL->rec[index].port == port)
		{	
			if (UnionPrintCommConfToFile(&(pgunionCommConfTBL->rec[index]),fp) < 0)
				continue;
			num++;
			if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
				if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
					break;
		}
	}
	fprintf(fp,"num = [%d]\n",num);
	return(num);
}

int UnionPrintAllServerCommConfToFile(FILE *fp)
{
	int		index;
	int		num = 0;
	int		ret;
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintAllServerCommConfToFile:: fp is null!\n");
		return(errCodeParameter);
	}
		
	if ((ret = UnionConnectCommConfTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAllServerCommConfToFile:: UnionConnectCommConfTBL!\n");
		return(ret);
	}
	for (index = 0; index < conMaxNumOfConn; index++)
	{
		if (pgunionCommConfTBL->rec[index].procType == conCommServer)
		{	
			if (UnionPrintCommConfToFile(&(pgunionCommConfTBL->rec[index]),fp) < 0)
				continue;
			num++;
			if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
				if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
					break;
		}
	}
	fprintf(fp,"num = [%d]\n",num);
	return(num);
}

int UnionPrintAllClientCommConfToFile(FILE *fp)
{
	int		index;
	int		num = 0;
	int		ret;
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintAllClientCommConfToFile:: fp is null!\n");
		return(errCodeParameter);
	}
		
	if ((ret = UnionConnectCommConfTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAllClientCommConfToFile:: UnionConnectCommConfTBL!\n");
		return(ret);
	}
	for (index = 0; index < conMaxNumOfConn; index++)
	{
		if (pgunionCommConfTBL->rec[index].procType == conCommClient)
		{	
			if (UnionPrintCommConfToFile(&(pgunionCommConfTBL->rec[index]),fp) < 0)
				continue;
			num++;
			if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
				if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
					break;
		}
	}
	fprintf(fp,"num = [%d]\n",num);
	return(num);
}

PUnionCommConf UnionFindServerCommConf(char *ipAddr,int port)
{
	int		index;
	int		ret;
	
	if (port <= 0)
	{
		UnionUserErrLog("in UnionFindYLListenerCommConf:: wrong par!\n");
		return(NULL);
	}
	
	if ((ret = UnionConnectCommConfTBL()) < 0)
	{
		UnionUserErrLog("in UnionFindYLListenerCommConf:: UnionConnectCommConfTBL!\n");
		return(NULL);
	}
	
	for (index = 0; index < conMaxNumOfConn; index++)
	{
		if (strlen(pgunionCommConfTBL->rec[index].ipAddr) == 0)
			continue;
		if ((pgunionCommConfTBL->rec[index].port != port) || (pgunionCommConfTBL->rec[index].procType != conCommServer))
			continue;
		if (ipAddr != NULL)
		{
			if (strcmp(pgunionCommConfTBL->rec[index].ipAddr,ipAddr) != 0)
				continue;
		}	
		return(&(pgunionCommConfTBL->rec[index]));
	}
	
	return(NULL);
}

PUnionCommConf UnionFindClientCommConf(char *ipAddr,int port)
{
	int		index;
	int		ret;
	
	if ((ipAddr == NULL) || (port <= 0))
	{
		UnionUserErrLog("in UnionFindYLListenerCommConf:: wrong par!\n");
		return(NULL);
	}
	
	if ((ret = UnionConnectCommConfTBL()) < 0)
	{
		UnionUserErrLog("in UnionFindYLListenerCommConf:: UnionConnectCommConfTBL!\n");
		return(NULL);
	}
	
	for (index = 0; index < conMaxNumOfConn; index++)
	{
		if (strlen(pgunionCommConfTBL->rec[index].ipAddr) == 0)
			continue;
		if ((strcmp(pgunionCommConfTBL->rec[index].ipAddr,ipAddr) != 0) || 
			(pgunionCommConfTBL->rec[index].port != port) || (pgunionCommConfTBL->rec[index].procType != conCommClient))
				continue;
		return(&(pgunionCommConfTBL->rec[index]));
	}
	
	return(NULL);
}

//UnionFindClientCommConfWithType函数add by hzh in 2012.2.22
PUnionCommConf UnionFindClientCommConfWithType(char *ipAddr,int port,TUnionCommConnType connType)
{
	int		index;
	int		ret;
	
	if ((ipAddr == NULL) || (port <= 0))
	{
		UnionUserErrLog("in UnionFindYLListenerCommConf:: wrong par!\n");
		return(NULL);
	}
	
	if ((ret = UnionConnectCommConfTBL()) < 0)
	{
		UnionUserErrLog("in UnionFindYLListenerCommConf:: UnionConnectCommConfTBL!\n");
		return(NULL);
	}
	
	for (index = 0; index < conMaxNumOfConn; index++)
	{
		if (strlen(pgunionCommConfTBL->rec[index].ipAddr) == 0)
			continue;
		if ((strcmp(pgunionCommConfTBL->rec[index].ipAddr,ipAddr) != 0) || 
			(pgunionCommConfTBL->rec[index].port != port) || (pgunionCommConfTBL->rec[index].procType != connType))
				continue;
		return(&(pgunionCommConfTBL->rec[index]));
	}
	
	return(NULL);
}


PUnionCommConf UnionAddCommConf(char *ipAddr,int port,TUnionCommProcType procType,TUnionCommConnType connType,char *remark)
{
	int		index;
	int		ret;
	TUnionCommConf	commConf;
	int		nullPos = -1;
	char		tmpIPAddr[15+1];
	char		varName[100+1];
	
	if ((ret = UnionConnectCommConfTBL()) < 0)
	{
		UnionUserErrLog("in UnionAddCommConf:: UnionConnectCommConfTBL!\n");
		return(NULL);
	}
	
	memset(&commConf,0,sizeof(commConf));
	// 进程类型
	if (!UnionIsValidCommProcType(procType))
	{
		UnionUserErrLog("in UnionAddCommConf:: UnionIsValidCommProcType [%d]\n",procType);
		return(NULL);
	}
	commConf.procType = procType;
	// 连接的类型
	if (!UnionIsValidCommConnType(connType))
	{
		UnionUserErrLog("in UnionAddCommConf:: UnionIsValidCommConnType [%d]\n",connType);
		return(NULL);
	}
	commConf.connType = connType;
	// 2008/5/22,王纯军修改
	/*
	if (!UnionIsValidIPAddrStr(ipAddr))
	{
		UnionUserErrLog("in UnionAddCommConf:: not validIPAddr [%s]\n",ipAddr);
		return(NULL);
	}
	strcpy(commConf.ipAddr,ipAddr);
	*/
	memset(tmpIPAddr,0,sizeof(tmpIPAddr));
	sprintf(varName,"commConfSpierIPLevel%d",port);
	if ((ret = UnionDealWithIPAddr(ipAddr,UnionReadIntTypeRECVar(varName),tmpIPAddr)) < 0)
	{
		UnionUserErrLog("in UnionAddCommConf:: UnionDealWithIPAddr [%s]\n",ipAddr);
		return(NULL);
	}
	strcpy(commConf.ipAddr,tmpIPAddr);
	// 2008/5/22，修改结束
	if (port <= 0)
	{
		UnionUserErrLog("in UnionAddCommConf:: not validPort [%d]\n",port);
		return(NULL);
	}
	commConf.port = port;
	if (remark != NULL)
	{	
		if (strlen(remark) >= sizeof(commConf.remark))
			memcpy(commConf.remark,remark,sizeof(commConf.remark)-1);
		else
			strcpy(commConf.remark,remark);
	}
	time(&(commConf.lastWorkingTime));

	for (index = 0; index < conMaxNumOfConn; index++)
	{
		if (strlen(pgunionCommConfTBL->rec[index].ipAddr) == 0)
		{
			if (nullPos < 0)
				nullPos = index;
			continue;
		}
		// 2008/5/22,王纯军修改
		/*	
		if (	(pgunionCommConfTBL->rec[index].port == port) &&
			(pgunionCommConfTBL->rec[index].procType == procType) &&
			(strcmp(pgunionCommConfTBL->rec[index].ipAddr,ipAddr) == 0))
		*/
		if (	(pgunionCommConfTBL->rec[index].port == port) &&
			(pgunionCommConfTBL->rec[index].procType == procType) &&
			(strcmp(pgunionCommConfTBL->rec[index].ipAddr,tmpIPAddr) == 0))
		// 2008/5/22，修改结束
			return(&(pgunionCommConfTBL->rec[index]));
	}
	if (nullPos < 0)
	{
		UnionUserErrLog("in UnionAddCommConf:: table is full num = [%d] nullPos = [%d]\n",conMaxNumOfConn,nullPos);
		return(NULL);
	}
	memcpy(&(pgunionCommConfTBL->rec[nullPos]),&commConf,sizeof(pgunionCommConfTBL->rec[nullPos]));
	return(&(pgunionCommConfTBL->rec[nullPos]));
}

PUnionCommConf UnionAddServerCommConf(char *ipAddr,int port,TUnionCommConnType connType,char *remark)
{
	return(UnionAddCommConf(ipAddr,port,conCommServer,connType,remark));
}

PUnionCommConf UnionAddClientCommConf(char *ipAddr,int port,TUnionCommConnType connType,char *remark)
{
	return(UnionAddCommConf(ipAddr,port,conCommClient,connType,remark));
}

int UnionDeleteSpecifiedCommConf(char *ipAddr,int port,TUnionCommProcType procType)
{
	int		index;
	int		ret;

	if ((ret = UnionConnectCommConfTBL()) < 0)
	{
		UnionUserErrLog("in UnionDeleteSpecifiedCommConf:: UnionConnectCommConfTBL!\n");
		return(ret);
	}

	for (index = 0; index < conMaxNumOfConn; index++)
	{
		if (	(pgunionCommConfTBL->rec[index].port == port) &&
			(pgunionCommConfTBL->rec[index].procType == procType) &&
			(strcmp(pgunionCommConfTBL->rec[index].ipAddr,ipAddr) == 0))
		{
			memset(&(pgunionCommConfTBL->rec[index]),0,sizeof(pgunionCommConfTBL->rec[index]));
			return(0);
		}
	}
	if (procType == conCommClient)
		UnionUserErrLog("in UnionDeleteSpecifiedCommConf:: [%s] [%d] [client] not exists!\n",ipAddr,port);
	else if (procType == conCommServer)
		UnionUserErrLog("in UnionDeleteSpecifiedCommConf:: [%s] [%d] [server] not exists!\n",ipAddr,port);
	else
		UnionUserErrLog("in UnionDeleteSpecifiedCommConf:: [%s] [%d] [unknown] not exists!\n",ipAddr,port);	
	return(errCodeYLCommConfMDL_CommConfNotDefined);
}

int UnionResetAllCommConf()
{
	return(UnionResetSpecifiedCommConf(NULL,-1,-1));
}

int UnionResetSpecifiedCommConf(char *ipAddr,int port,TUnionCommProcType procType)
{
	int		index;
	int		ret;
	int		totalNum = 0;
	
	if ((ret = UnionConnectCommConfTBL()) < 0)
	{
		UnionUserErrLog("in UnionResetSpecifiedCommConf:: UnionConnectCommConfTBL!\n");
		return(ret);
	}

	for (index = 0; index < conMaxNumOfConn; index++)
	{
		if ((ipAddr != NULL) && (strlen(ipAddr) != 0))	// 比较IP地址
		{
			if (strcmp(pgunionCommConfTBL->rec[index].ipAddr,ipAddr) != 0)
				continue;
		}
		if (port > 0)	// 比较端口
		{
			if (pgunionCommConfTBL->rec[index].port != port)
				continue;
		}
		if (procType >= 0)	// 比较类型
		{
			if (pgunionCommConfTBL->rec[index].procType != procType)
				continue;
		}
		if (strlen(pgunionCommConfTBL->rec[index].ipAddr) == 0)
			continue;
		pgunionCommConfTBL->rec[index].connNum = 0;
		pgunionCommConfTBL->rec[index].totalNum = 0;
		time(&(pgunionCommConfTBL->rec[index].lastWorkingTime));
		totalNum++;
	}
	return(totalNum);
}

int UnionDeleteIdleCommConf(long idleTime)
{
	int		index;
	int		ret;
	time_t		now;
	int		totalNum = 0;	
	
	if ((ret = UnionConnectCommConfTBL()) < 0)
	{
		UnionUserErrLog("in UnionDeleteIdleCommConf:: UnionConnectCommConfTBL!\n");
		return(ret);
	}

	time(&now);
	for (index = 0; index < conMaxNumOfConn; index++)
	{
		if ((now - pgunionCommConfTBL->rec[index].lastWorkingTime >= idleTime) &&
			(pgunionCommConfTBL->rec[index].lastWorkingTime > 0))
		{
			memset(&(pgunionCommConfTBL->rec[index]),0,sizeof(pgunionCommConfTBL->rec[index]));
			totalNum++;
		}
	}
	return(totalNum);
}

int UnionDeleteSpecifiedCommConfByRemark(char *remark)
{
	int		index;
	int		ret;
	int		num = 0;

	if ((ret = UnionConnectCommConfTBL()) < 0)
	{
		UnionUserErrLog("in UnionDeleteSpecifiedCommConfByRemark:: UnionConnectCommConfTBL!\n");
		return(ret);
	}

	for (index = 0; index < conMaxNumOfConn; index++)
	{
		if (strcmp(pgunionCommConfTBL->rec[index].remark,remark) == 0)
		{
			memset(&(pgunionCommConfTBL->rec[index]),0,sizeof(pgunionCommConfTBL->rec[index]));
			num++;
		}
	}
	return(num);
}

int UnionPrintCommConfTBLToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintCommConfTBLToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	ret = UnionPrintCommConfTBLToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

int UnionPrintCommConfOfIPAddrToSpecFile(char *ipAddr,char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintCommConfOfIPAddrToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	ret = UnionPrintCommConfOfIPAddrToFile(ipAddr,fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

int UnionPrintCommConfOfRemarkToSpecFile(char *remark,char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintCommConfOfRemarkToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	ret = UnionPrintCommConfOfRemarkToFile(remark,fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

int UnionPrintCommConfOfPortToSpecFile(int port,char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintCommConfOfPortToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	ret = UnionPrintCommConfOfPortToFile(port,fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

int UnionPrintAllServerCommConfToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintAllServerCommConfToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	ret = UnionPrintAllServerCommConfToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

int UnionPrintAllClientCommConfToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintAllClientCommConfToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	ret = UnionPrintAllClientCommConfToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

// wangk add 2009-9-25
int UnionPrintCommConfInRecStrFormatToFile(PUnionCommConf punionCommConf,FILE *fp)
{
	time_t		now;
	
	if ((punionCommConf == NULL) || (fp == NULL))
		return(errCodeParameter);
	if (strlen(punionCommConf->ipAddr) == 0)
		return(errCodeParameter);
	switch (punionCommConf->procType)
	{
		case	conCommServer:
			fprintf(fp,"procType=服务器|");
			break;
		case	conCommClient:
			fprintf(fp,"procType=客户端|");
			break;
		default:
			fprintf(fp,"procType=|");
			break;
	}
	switch (punionCommConf->connType)
	{
		case	conCommShortConn:
			fprintf(fp,"connType=短连接|");
			break;
		case	conCommLongConn:
			fprintf(fp,"connType=长连接|");
			break;
		default:
			fprintf(fp,"connType=|");
			break;
	}
	time(&now);
	fprintf(fp,"connType=%s|port=%d|remark=%s|connNum=%ld|totalNum=%ld|lastWorkingTime=%ld|\n",
		punionCommConf->ipAddr,punionCommConf->port,punionCommConf->remark,punionCommConf->connNum,
		punionCommConf->totalNum,now - punionCommConf->lastWorkingTime);
	return(0);
}

int UnionPrintCommConfTBLInRecStrFormatToFile(FILE *fp)
{
	int		index;
	int		ret;
	int		num = 0;
		
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintCommConfTBLInRecStrFormatToFile:: fp is null!\n");
		return(errCodeParameter);
	}
		
	if ((ret = UnionConnectCommConfTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintCommConfTBLInRecStrFormatToFile:: UnionConnectCommConfTBL!\n");
		return(ret);
	}
	for (index = 0; index < conMaxNumOfConn; index++)
	{
		if (UnionPrintCommConfInRecStrFormatToFile(&(pgunionCommConfTBL->rec[index]),fp) < 0)
			continue;
		num++;
	}

	return(num);
}

int UnionPrintCommConfTBLInRecStrFormatToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
	{
		UnionUserErrLog("in UnionPrintCommConfTBLInRecStrFormatToSpecFile:: NULL poionter!\n");
		return(errCodeParameter);
	}

	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintCommConfTBLInRecStrFormatToSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	ret = UnionPrintCommConfTBLInRecStrFormatToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}
// wangk add end 2009-9-25

