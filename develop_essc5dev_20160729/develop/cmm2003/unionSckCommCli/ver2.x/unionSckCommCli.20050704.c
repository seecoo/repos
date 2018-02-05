//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2004/11/23

#include <stdio.h>
#include <string.h>
#include <signal.h>

#ifndef _UnionSckCli_2_x_
#define _UnionSckCli_2_x_
#endif

#include "unionSckCommCli.h"
#include "unionModule.h"
#include "unionErrCode.h"
#include "UnionLog.h"

#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_
#include "UnionEnv.h"
#endif

PUnionSharedMemoryModule	pgunionCommSvrDefMDL = NULL;
PUnionCommSvrDef		pgunionCommSvrDef = NULL;

int UnionSetCommSvrStatus(TUnionCommSvrWorkingMode workingMode)
{
	int	ret;
	int	i;
	
	switch (workingMode)
	{
		case	conCommSvrHotBackupWorkingMode:
		case	conCommSvrBalanceWorkingMode:
		case	conSckSvrMaintaining:
			break;
		default:
			UnionUserErrLog("in UnionSetCommSvrStatus:: invalid workingMode [%d]!\n",workingMode);
			return(errCodeSckCommMDL_InvalidWorkingMode);
	}
	if ((ret = UnionConnectCommSvrDefMDL()) < 0)
	{
		UnionUserErrLog("in UnionSetCommSvrStatus:: UnionConnectCommSvrDefMDL!\n");
		return(ret);
	}
	pgunionCommSvrDef->workingMode = workingMode;
	return(0);
}

int UnionSetSckSvrStatus(char *ipAddr,TUnionSckSvrStatus status)
{
	int	ret;
	int	i;
	
	switch (status)
	{
		case	conSckSvrAbnormal:
		case	conSckSvrNormal:
		case	conSckSvrMaintaining:
		case	conSckSvrColdBackup:
			break;
		default:
			UnionUserErrLog("in UnionSetSckSvrStatus:: invalid status [%d]!\n",status);
			return(errCodeSckCommMDL_InvalidWorkingMode);
	}
	if ((ret = UnionConnectCommSvrDefMDL()) < 0)
	{
		UnionUserErrLog("in UnionSetSckSvrStatus:: UnionConnectCommSvrDefMDL!\n");
		return(ret);
	}
	UnionToUpperCase(ipAddr);
	if (strcmp(ipAddr,"BOTH") == 0)
	{
		pgunionCommSvrDef->sckSvrDef[0].status = status;
		pgunionCommSvrDef->sckSvrDef[1].status = status;
		return(0);
	}
	for (i = 0; i < 2; i++)
	{
		if (strcmp(ipAddr,pgunionCommSvrDef->sckSvrDef[i].ipAddr) == 0)
		{
			pgunionCommSvrDef->sckSvrDef[i].status = status;
			return(0);
		}
	}
	UnionUserErrLog("in UnionSetSckSvrStatus:: ipAddr [%s] server not defined!\n",ipAddr);
	return(errCodeSckCommMDL_ServerNotDefined);
}	

int UnionZerolizeSckSvrCommTimes(char *ipAddr)
{
	int	ret;
	int	i;
	
	if ((ret = UnionConnectCommSvrDefMDL()) < 0)
	{
		UnionUserErrLog("in UnionZerolizeSckSvrCommTimes:: UnionConnectCommSvrDefMDL!\n");
		return(ret);
	}
	UnionToUpperCase(ipAddr);
	if (strcmp(ipAddr,"BOTH") == 0)
	{
		pgunionCommSvrDef->sckSvrDef[0].successTimes = 0;
		pgunionCommSvrDef->sckSvrDef[1].successTimes = 0;
		pgunionCommSvrDef->sckSvrDef[0].failTimes = 0;
		pgunionCommSvrDef->sckSvrDef[1].failTimes = 0;
		pgunionCommSvrDef->sckSvrDef[0].activeNum = 0;
		pgunionCommSvrDef->sckSvrDef[1].activeNum = 0;
		return(0);
	}
	for (i = 0; i < 2; i++)
	{
		if (strcmp(ipAddr,pgunionCommSvrDef->sckSvrDef[i].ipAddr) == 0)
		{
			pgunionCommSvrDef->sckSvrDef[i].successTimes = 0;
			pgunionCommSvrDef->sckSvrDef[i].activeNum = 0;
			pgunionCommSvrDef->sckSvrDef[i].failTimes = 0;
			return(0);
		}
	}
	UnionUserErrLog("in UnionZerolizeSckSvrCommTimes:: ipAddr [%s] server not defined!\n",ipAddr);
	return(errCodeSckCommMDL_ServerNotDefined);
}	

int UnionReversePrimaryAndSecondarySckSvr()
{
	int	ret;
	int	i;
	
	if ((ret = UnionConnectCommSvrDefMDL()) < 0)
	{
		UnionUserErrLog("in UnionReversePrimaryAndSecondarySckSvr:: UnionConnectCommSvrDefMDL!\n");
		return(ret);
	}
	return(UnionSetBackupSckSvrAsPrimary(pgunionCommSvrDef));
}	

int UnionSetBackupSckSvrAsPrimary(PUnionCommSvrDef pcommSvrDef)
{
	if (pcommSvrDef == NULL)
	{
		UnionUserErrLog("in UnionSetBackupSckSvrAsPrimary:: pcommSvrDef is NULL!\n");
		return(errCodeParameter);
	}
	if (pcommSvrDef->sckSvrDef[0].isPrimaryServer)
	{
		switch (pcommSvrDef->sckSvrDef[1].status)
		{
			case	conSckSvrAbnormal:
				pcommSvrDef->sckSvrDef[1].status = conSckSvrNormal;
				break;
			case	conSckSvrMaintaining:
			case	conSckSvrColdBackup:
				return(errCodeSckCommMDL_InvalidWorkingMode);
			default:
				break;
		}
		pcommSvrDef->sckSvrDef[0].isPrimaryServer = 0;
		pcommSvrDef->sckSvrDef[1].isPrimaryServer = 1;
	}
	else
	{
		switch (pcommSvrDef->sckSvrDef[0].status)
		{
			case	conSckSvrAbnormal:
				pcommSvrDef->sckSvrDef[0].status = conSckSvrNormal;
				break;
			case	conSckSvrMaintaining:
			case	conSckSvrColdBackup:
				return(errCodeSckCommMDL_InvalidWorkingMode);
			default:
				break;
		}
		pcommSvrDef->sckSvrDef[1].isPrimaryServer = 0;
		pcommSvrDef->sckSvrDef[0].isPrimaryServer = 1;
	}
	return(0);
}

PUnionSckSvrDef UnionSelectPrimarySckSvr(PUnionCommSvrDef pcommSvrDef)
{
	if (pcommSvrDef == NULL)
	{
		UnionUserErrLog("in UnionSelectPrimarySckSvr:: pcommSvrDef is NULL!\n");
		return(NULL);
	}
	if ((pcommSvrDef->workingMode != conCommSvrHotBackupWorkingMode) &&
	    (pcommSvrDef->workingMode != conCommSvrBalanceWorkingMode))
	{
		UnionUserErrLog("in UnionSelectPrimarySckSvr:: pgunionCommSvrDef->workingMode = [%d]\n",pgunionCommSvrDef->workingMode);
		return(NULL);
	}
	if (pcommSvrDef->sckSvrDef[0].isPrimaryServer)
	{
		if (pcommSvrDef->sckSvrDef[0].status == conSckSvrNormal)
			return(&(pcommSvrDef->sckSvrDef[0]));
		else
		{
			UnionSetBackupSckSvrAsPrimary(pcommSvrDef);
			return(&(pcommSvrDef->sckSvrDef[1]));
		}
	}
	else
	{
		if (pcommSvrDef->sckSvrDef[1].status == conSckSvrNormal)
			return(&(pcommSvrDef->sckSvrDef[1]));
		else
		{
			UnionSetBackupSckSvrAsPrimary(pcommSvrDef);
			return(&(pcommSvrDef->sckSvrDef[0]));
		}
	}
}

PUnionSckSvrDef UnionSelectAnotherSckSvr(PUnionCommSvrDef pcommSvrDef,PUnionSckSvrDef psckSvrDef)
{
	if (pcommSvrDef == NULL)
	{
		UnionUserErrLog("in UnionSelectAnotherSckSvr:: pcommSvrDef is NULL!\n");
		return(NULL);
	}
	if ((pcommSvrDef->workingMode != conCommSvrHotBackupWorkingMode) &&
	    (pcommSvrDef->workingMode != conCommSvrBalanceWorkingMode))
	{
		UnionUserErrLog("in UnionSelectAnotherSckSvr:: pgunionCommSvrDef->workingMode = [%d]\n",pgunionCommSvrDef->workingMode);
		return(NULL);
	}
	if (&(pcommSvrDef->sckSvrDef[0]) == psckSvrDef)
		return(&(pcommSvrDef->sckSvrDef[1]));
	else
		return(&(pcommSvrDef->sckSvrDef[0]));
}

PUnionSckSvrDef UnionSelectIdleSckSvr(PUnionCommSvrDef pcommSvrDef)
{
	if (pcommSvrDef == NULL)
	{
		UnionUserErrLog("in UnionSelectIdleSckSvr:: pcommSvrDef is NULL!\n");
		return(NULL);
	}
	if ((pcommSvrDef->workingMode != conCommSvrHotBackupWorkingMode) &&
	    (pcommSvrDef->workingMode != conCommSvrBalanceWorkingMode))
	{
		UnionUserErrLog("in UnionSelectIdleSckSvr:: pgunionCommSvrDef->workingMode = [%d]\n",pgunionCommSvrDef->workingMode);
		return(NULL);
	}
	//if ((pcommSvrDef->sckSvrDef[0].successTimes + pcommSvrDef->sckSvrDef[0].failTimes) 
	//	<= (pcommSvrDef->sckSvrDef[1].successTimes + pcommSvrDef->sckSvrDef[1].failTimes))
	if (pcommSvrDef->sckSvrDef[0].activeNum <= pcommSvrDef->sckSvrDef[1].activeNum)
	{
		if (pcommSvrDef->sckSvrDef[0].status == conSckSvrNormal)
			return(&(pcommSvrDef->sckSvrDef[0]));
		else
		{
			switch (pcommSvrDef->sckSvrDef[1].status)
			{
				case	conSckSvrAbnormal:
					pcommSvrDef->sckSvrDef[1].status = conSckSvrNormal;
					//UnionLog("in UnionSelectIdleSckSvr:: both abnormal set the second normal!\n");
					return(&(pcommSvrDef->sckSvrDef[1]));
				case	conSckSvrNormal:
					return(&(pcommSvrDef->sckSvrDef[1]));
				default:
					return(NULL);
			}
		}
	}
	else
	{
		if (pcommSvrDef->sckSvrDef[1].status == conSckSvrNormal)
			return(&(pcommSvrDef->sckSvrDef[1]));
		else
		{
			switch (pcommSvrDef->sckSvrDef[0].status)
			{
				case	conSckSvrAbnormal:
					pcommSvrDef->sckSvrDef[0].status = conSckSvrNormal;
					//UnionLog("in UnionSelectIdleSckSvr:: both abnormal set the first normal!\n");
					return(&(pcommSvrDef->sckSvrDef[0]));
				case	conSckSvrNormal:
					return(&(pcommSvrDef->sckSvrDef[0]));
				default:
					return(NULL);
			}
		}
	}
}

int UnionGetNameOfCommSvrDef(char *tmpBuf)
{
	sprintf(tmpBuf,"%s/unionCommSvr.Def",getenv("UNIONETC"));
	return(0);
}

int UnionSetCommSvrAsHotBackupWorkingMode()
{
	int	ret;
	if ((ret = UnionConnectCommSvrDefMDL()) < 0)
	{
		UnionUserErrLog("in UnionSetCommSvrAsHotBackupWorkingMode:: UnionConnectCommSvrDefMDL!\n");
		return(ret);
	}
	pgunionCommSvrDef->workingMode = conCommSvrHotBackupWorkingMode;
	return(0);
}

int UnionSetCommSvrAsBalanceWorkingMode()
{
	int	ret;
	if ((ret = UnionConnectCommSvrDefMDL()) < 0)
	{
		UnionUserErrLog("in UnionSetCommSvrAsBalanceWorkingMode:: UnionConnectCommSvrDefMDL!\n");
		return(ret);
	}
	pgunionCommSvrDef->workingMode = conCommSvrBalanceWorkingMode;
	return(0);
}

int UnionSetCommSvrAsMaintainWorkingMode()
{
	int	ret;
	if ((ret = UnionConnectCommSvrDefMDL()) < 0)
	{
		UnionUserErrLog("in UnionSetCommSvrAsMaintainWorkingMode:: UnionConnectCommSvrDefMDL!\n");
		return(ret);
	}
	pgunionCommSvrDef->workingMode = conCommSvrMaintainWorkingMode;
	return(0);
}

int UnionReloadCommSvrDef()
{
	int		ret;
	char		tmpBuf[512];
	char		*p;
	int		i,j;
	
	if ((ret = UnionConnectCommSvrDefMDL()) < 0)
	{
		UnionUserErrLog("in UnionReloadCommSvrDef:: UnionConnectCommSvrDefMDL!\n");
		return(ret);
	}
	
	if ((ret = UnionSetCommSvrAsMaintainWorkingMode()) < 0)
	{
		UnionUserErrLog("in UnionReloadCommSvrDef:: UnionSetCommSvrAsMaintainWorkingMode!\n");
		return(ret);
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	UnionGetNameOfCommSvrDef(tmpBuf);
	if ((ret = UnionInitEnvi(tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionReloadCommSvrDef:: UnionInitEnvi [%s]!\n",tmpBuf);
		return(ret);
	}

	for (i = 0,j = 0; (i < UnionGetEnviVarNum()) && (j < 2); i++)
	{
		// 读取名称
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
		{
			UnionUserErrLog("in UnionReloadCommSvrDef:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,0);
			continue;
		}
		if (strcmp(p,"sckSvrDef") != 0)
			continue;
		// 读取IP地址
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
		{
			UnionUserErrLog("in UnionReloadCommSvrDef:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
			continue;
		}
		if (!UnionIsValidIPAddrStr(p))
		{
			UnionUserErrLog("in UnionReloadCommSvrDef:: !UnionIsValidIPAddrStr [%s]\n",p);
			continue;
		}
		else
		{
			memset(pgunionCommSvrDef->sckSvrDef[j].ipAddr,0,sizeof(pgunionCommSvrDef->sckSvrDef[j].ipAddr));
			strcpy(pgunionCommSvrDef->sckSvrDef[j].ipAddr,p);
		}
		// 读取端口号
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,2)) == NULL)
		{
			UnionUserErrLog("in UnionReloadCommSvrDef:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,2);
			continue;
		}
		if ((pgunionCommSvrDef->sckSvrDef[j].port = atoi(p)) < 0)
		{
			UnionUserErrLog("in UnionReloadCommSvrDef:: port [%s] error\n",p);
			continue;
		}
		// 读取主、从标志
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,3)) == NULL)
		{
			UnionUserErrLog("in UnionReloadCommSvrDef:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,3);
			continue;
		}
		pgunionCommSvrDef->sckSvrDef[j].isPrimaryServer = atoi(p);
		// 读取状态标志
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,4)) == NULL)
		{
			UnionUserErrLog("in UnionReloadCommSvrDef:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,4);
			continue;
		}
		switch ((pgunionCommSvrDef->sckSvrDef[j].status = atoi(p)))
		{
			case 	conSckSvrAbnormal:
			case	conSckSvrNormal:
			case	conSckSvrMaintaining:
			case	conSckSvrColdBackup:
				break;
			default:
				pgunionCommSvrDef->sckSvrDef[j].status = conSckSvrNormal;
				break;
		}
		pgunionCommSvrDef->sckSvrDef[j].status = conSckSvrNormal;
		pgunionCommSvrDef->sckSvrDef[j].activeNum = 0;
		pgunionCommSvrDef->sckSvrDef[j].successTimes = 0;
		pgunionCommSvrDef->sckSvrDef[j].failTimes = 0;
		pgunionCommSvrDef->sckSvrDef[j].activeConnNum = 0;
		j++;
	}

	pgunionCommSvrDef->workingMode = conCommSvrHotBackupWorkingMode;
	if ((p = UnionGetEnviVarByName("workingMode")) == NULL)
		UnionLog("in UnionReloadCommSvrDef:: UnionGetEnviVarByName for [%s],default mode used\n!","workingMode");
	else
		pgunionCommSvrDef->workingMode = atoi(p);
	switch (pgunionCommSvrDef->workingMode)
	{
		case conCommSvrHotBackupWorkingMode:
		case conCommSvrBalanceWorkingMode:
		case conCommSvrMaintainWorkingMode:
			break;
		default:
			UnionUserErrLog("in UnionReloadCommSvrDef:: wrong working mode [%d],default mode used\n!",pgunionCommSvrDef->workingMode);
			pgunionCommSvrDef->workingMode = conCommSvrHotBackupWorkingMode;
			break;
	}
	pgunionCommSvrDef->timeout = 5;
	if ((p = UnionGetEnviVarByName("timeout")) == NULL)
		UnionLog("in UnionReloadCommSvrDef:: UnionGetEnviVarByName for [%s],default mode used\n!","timeout");
	else
	{
		if ((pgunionCommSvrDef->timeout = atoi(p)) < 0)
			pgunionCommSvrDef->timeout = 5;
	}
			
	UnionClearEnvi();

	if (j == 2)		
		return(0);
	else
		return(1);
}

int UnionIsConnectedCommSvrDefMDL()
{
	if ((pgunionCommSvrDef != NULL)	&& (pgunionCommSvrDefMDL != NULL))// 已经连接
		return(1);
	else
		return(0);
}

int UnionConnectCommSvrDefMDL()
{
	int	index;
	int	ret;
		
	if (UnionIsConnectedCommSvrDefMDL())// 已经连接
		return(0);
		
	if ((pgunionCommSvrDefMDL = UnionConnectSharedMemoryModule(conMDLNameOfCommSvrDef,
			sizeof(TUnionCommSvrDef))) == NULL)
	{
		UnionUserErrLog("in UnionConnectCommSvrDefMDL:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgunionCommSvrDef = (PUnionCommSvrDef)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionCommSvrDefMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectCommSvrDefMDL:: PUnionCommSvrDef!\n");
		UnionDisconnectShareModule(pgunionCommSvrDefMDL);
		return(errCodeSharedMemoryModule);
	}
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionCommSvrDefMDL))
	{
		if ((ret = UnionReloadCommSvrDef()) < 0)
		{
			UnionUserErrLog("in UnionConnectCommSvrDefMDL:: UnionReloadCommSvrDef!\n");
			UnionDisconnectCommSvrDefMDL();
		}
		return(ret);
	}
	else
		return(0);
}

int UnionDisconnectCommSvrDefMDL()
{
	pgunionCommSvrDef = NULL;
	
	return(UnionDisconnectShareModule(pgunionCommSvrDefMDL));
}

int UnionRemoveCommSvrDefMDL()
{
	pgunionCommSvrDef = NULL;
	
	return(UnionRemoveSharedMemoryModule(conMDLNameOfCommSvrDef));
}

PUnionSckSvrConn UnionCreateSckSvrConn(PUnionSckSvrDef psckSvrDef)
{
	PUnionSckSvrConn	psckSvrConn;
	
	if (psckSvrDef == NULL)
	{
		UnionUserErrLog("in UnionCreateSckSvrConn:: psckSvrDef is null!\n");
		return(NULL);
	}
	if (psckSvrDef->status != conSckSvrNormal)
	{
		UnionUserErrLog("in UnionCreateSckSvrConn:: conSckSvrNormal is not normal!\n");
		return(NULL);
	}
	if ((psckSvrConn = (PUnionSckSvrConn)malloc(sizeof(*psckSvrConn))) == NULL)
	{
		UnionSystemErrLog("in UnionCreateSckSvrConn:: malloc!\n");
		return(NULL);
	}
	psckSvrConn->psckSvrDef = psckSvrDef;
	if ((psckSvrConn->sckHDL = UnionCreateSocketClient(psckSvrConn->psckSvrDef->ipAddr,
							psckSvrConn->psckSvrDef->port)) < 0)
	{
		UnionUserErrLog("in UnionCreateSckSvrConn:: UnionCreateSocketClient [%s] [%d]!\n",psckSvrConn->psckSvrDef->ipAddr,
							psckSvrConn->psckSvrDef->port);
		free(psckSvrConn);
		return(NULL);
	}
	++(psckSvrConn->psckSvrDef->activeConnNum);
	return(psckSvrConn);
}
	
int UnionCloseSckSvrConn(PUnionSckSvrConn psckSvrConn)
{
	if (psckSvrConn == NULL)
		return(0);
	if (psckSvrConn->sckHDL >= 0)
	{
		UnionCloseSocket(psckSvrConn->sckHDL);
		psckSvrConn->sckHDL = -1;
	}
	if (psckSvrConn->psckSvrDef)
	{
		if (psckSvrConn->psckSvrDef->activeConnNum > 0)
			--psckSvrConn->psckSvrDef->activeConnNum;
		psckSvrConn->psckSvrDef = NULL;
	}
	free(psckSvrConn);
	psckSvrConn = NULL;
	return(0);
}

PUnionCommSvrConn UnionConnectCommSvr()
{
	int			ret;
	int			i;
	
	PUnionCommSvrConn	psckCommSvrConn;
	
	if ((ret = UnionConnectCommSvrDefMDL()) < 0)
	{
		UnionUserErrLog("in UnionConnectCommSvr:: UnionConnectCommSvrDefMDL!\n");
		return(NULL);
	}
	
	if ((pgunionCommSvrDef->workingMode != conCommSvrHotBackupWorkingMode) &&
	    (pgunionCommSvrDef->workingMode != conCommSvrBalanceWorkingMode))
	{
		UnionUserErrLog("in UnionConnectCommSvr:: pgunionCommSvrDef->workingMode = [%d]\n",pgunionCommSvrDef->workingMode);
		return(NULL);
	}
	
	if ((psckCommSvrConn = (PUnionCommSvrConn)malloc(sizeof(*psckCommSvrConn))) == NULL)
	{
		UnionSystemErrLog("in UnionConnectCommSvr:: malloc!\n");
		return(NULL);
	}
	memset(psckCommSvrConn,0,sizeof(*psckCommSvrConn));
	psckCommSvrConn->pcommSvrDef = pgunionCommSvrDef;
	for (i = 0; i < 2; i++)
	{
		psckCommSvrConn->psckCommConn[i] = NULL;
	}
	
	psckCommSvrConn->workingMode = psckCommSvrConn->pcommSvrDef->workingMode;
	//UnionLog("in UnionConnectCommSvr:: psckCommSvrConn address = [%x]\npsckCommConn[0] = [%x]\npsckCommConn[1] = [%x]\n",
	//					psckCommSvrConn,psckCommSvrConn->psckCommConn[0],psckCommSvrConn->psckCommConn[1]);
	//UnionLog("in UnionConnectCommSvr:: psckCommSvrConn->pcommSvrDef = [%x]\n",psckCommSvrConn->pcommSvrDef);
	return(psckCommSvrConn);	
}

int UnionDisconnectCommSvr(PUnionCommSvrConn psckCommSvrConn)
{
	int	ret;
	int	i;
	
	if (psckCommSvrConn == NULL)
		return(0);
	
	for (i = 0; i < 2; i++)
	{
		UnionCloseSckSvrConn(psckCommSvrConn->psckCommConn[i]);
		psckCommSvrConn->psckCommConn[i] = NULL;
	}					
	
	psckCommSvrConn->pcommSvrDef = NULL;		
	free(psckCommSvrConn);
	psckCommSvrConn = NULL;
	
	UnionDisconnectCommSvrDefMDL();

	return(0);
}

int UnionCommSvrConnService(PUnionCommSvrConn psckCommSvrConn,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr)
{
	int		i;
		
	if (psckCommSvrConn == NULL)
	{
		UnionUserErrLog("in UnionCommSvrConnService:: psckCommSvrConn is null!\n");
		return(errCodeParameter);
	}

	if (psckCommSvrConn->workingMode != psckCommSvrConn->pcommSvrDef->workingMode)
	{
		UnionLog("in UnionCommSvrConnService:: workingMode changed!\n");
		for (i = 0; i < 2; i++)
		{
			UnionCloseSckSvrConn(psckCommSvrConn->psckCommConn[i]);
			psckCommSvrConn->psckCommConn[i] = NULL;
		}
		psckCommSvrConn->workingMode = psckCommSvrConn->pcommSvrDef->workingMode;
	}
		
	switch (psckCommSvrConn->pcommSvrDef->workingMode)
	{
		case	conCommSvrHotBackupWorkingMode:
			return(UnionCommSvrConnHotBackupService(psckCommSvrConn,reqStr,lenOfReqStr,resStr,sizeOfResStr));
		case	conCommSvrBalanceWorkingMode:
			return(UnionCommSvrConnBalanceService(psckCommSvrConn,reqStr,lenOfReqStr,resStr,sizeOfResStr));
		case	conCommSvrMaintainWorkingMode:
			for (i = 0; i < 2; i++)
			{
				UnionCloseSckSvrConn(psckCommSvrConn->psckCommConn[i]);
				psckCommSvrConn->psckCommConn[i] = NULL;
			}
			UnionUserErrLog("in UnionCommSvrConnService:: CommSvr is now maintaining...\n");
			return(-2);					
		default:
			return(UnionCommSvrConnHotBackupService(psckCommSvrConn,reqStr,lenOfReqStr,resStr,sizeOfResStr));
	}
}	
	
int UnionCommSvrConnHotBackupService(PUnionCommSvrConn psckCommSvrConn,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr)
{
	PUnionSckSvrDef	psckSvrDef = NULL;
	int		ret;
	int		retryTimes = 0;
	
	if (psckCommSvrConn == NULL)
	{
		UnionUserErrLog("in UnionCommSvrConnHotBackupService:: psckCommSvrConn is null!\n");
		return(errCodeParameter);
	}
selectPrimary:	
	if (retryTimes >= 2)
	{
		UnionUserErrLog("in UnionCommSvrConnHotBackupService:: retryTimes = 2!\n");
		return(errCodeSckCommMDL_ServiceFailure);
	}
		
	//UnionLog("in UnionCommSvrConnHotBackupService:: psckCommSvrConn->pcommSvrDef = [%x]\n",psckCommSvrConn->pcommSvrDef);

	if ((psckSvrDef = UnionSelectPrimarySckSvr(psckCommSvrConn->pcommSvrDef)) == NULL)
	{
		UnionUserErrLog("in UnionCommSvrConnHotBackupService:: UnionSelectPrimarySckSvr!\n");
		return(errCodeSckCommMDL_NoWorkingServer);
	}

	if (psckCommSvrConn->psckCommConn[0] == NULL)
	{
		if ((psckCommSvrConn->psckCommConn[0] = UnionCreateSckSvrConn(psckSvrDef)) == NULL)
		{
			UnionUserErrLog("in UnionCommSvrConnHotBackupService:: UnionCreateSckSvrConn!\n");
			psckSvrDef->status = conSckSvrAbnormal;
			UnionSetBackupSckSvrAsPrimary(psckCommSvrConn->pcommSvrDef);
			retryTimes++;
			goto selectPrimary;
		}
	}
	else
	{	
		if (psckCommSvrConn->psckCommConn[0]->psckSvrDef != psckSvrDef)
		{
			//UnionLog("in UnionCommSvrConnHotBackupService:: primary sck svr changed!\n");
			UnionCloseSckSvrConn(psckCommSvrConn->psckCommConn[0]);
			psckCommSvrConn->psckCommConn[0] = NULL;
			if ((psckCommSvrConn->psckCommConn[0] = UnionCreateSckSvrConn(psckSvrDef)) == NULL)
			{
				UnionUserErrLog("in UnionCommSvrConnHotBackupService:: UnionCreateSckSvrConn!\n");
				psckSvrDef->status = conSckSvrAbnormal;
				UnionSetBackupSckSvrAsPrimary(psckCommSvrConn->pcommSvrDef);
				retryTimes++;
				goto selectPrimary;
			}
		}
	}

	//UnionLog("in UnionCommSvrConnHotBackupService:: lenOfReqStr = [%d] reqStr = [%s]\n",lenOfReqStr,reqStr);
	
	++psckSvrDef->activeNum;
	if ((ret = UnionCommWithSckSvr(psckCommSvrConn->psckCommConn[0],reqStr,lenOfReqStr,resStr,sizeOfResStr)) < 0)
	{
		if (psckSvrDef->activeNum > 0)
			--psckSvrDef->activeNum;
		++psckSvrDef->failTimes;
		UnionUserErrLog("in UnionCommSvrConnHotBackupService:: UnionCommWithSckSvr!\n");
		if (ret == errSocketError)
		{
			UnionUserErrLog("in UnionCommSvrConnHotBackupService:: UnionCommWithSckSvr socketError!\n");
			UnionCloseSckSvrConn(psckCommSvrConn->psckCommConn[0]);
			psckCommSvrConn->psckCommConn[0] = NULL;
			goto selectPrimary;
		}
	}
	if (psckSvrDef->activeNum > 0)
		--psckSvrDef->activeNum;
	++psckSvrDef->successTimes;
	return(ret);
}

int UnionCommSvrConnBalanceService(PUnionCommSvrConn psckCommSvrConn,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr)
{
	PUnionSckSvrDef	psckSvrDef = NULL;
	int		i;
	int		ret;
	int		index = -1;
	int		retryTimes = 0;
	
	if (psckCommSvrConn == NULL)
	{
		UnionUserErrLog("in UnionCommSvrConnBalanceService:: psckCommSvrConn is null!\n");
		return(errCodeParameter);
	}

	for (i = 0; i < 2; i++)
	{
		if (psckCommSvrConn->psckCommConn[i] != NULL)
		{
			if (psckCommSvrConn->psckCommConn[i]->psckSvrDef->status != conSckSvrNormal)
			{
				UnionCloseSckSvrConn(psckCommSvrConn->psckCommConn[i]);
				psckCommSvrConn->psckCommConn[i] = NULL;
			}
		}
	}
selectIdle:	
	if (retryTimes >= 2)
	{
		UnionUserErrLog("in UnionCommSvrConnBalanceService:: retryTimes = 2!\n");
		return(errCodeSckCommMDL_ServiceFailure);
	}
	index = -1;
	if (psckSvrDef == NULL)
	{
		if ((psckSvrDef = UnionSelectIdleSckSvr(psckCommSvrConn->pcommSvrDef)) == NULL)
		{
			UnionUserErrLog("in UnionCommSvrConnHotBackupService:: UnionSelectIdleSckSvr!\n");
			return(errCodeSckCommMDL_NoWorkingServer);
		}
	}
	else
	{
		if ((psckSvrDef = UnionSelectAnotherSckSvr(psckCommSvrConn->pcommSvrDef,psckSvrDef)) == NULL)
		{
			UnionUserErrLog("in UnionCommSvrConnHotBackupService:: UnionSelectIdleSckSvr!\n");
			return(errCodeSckCommMDL_NoWorkingServer);
		}
	}
	
	/*
	UnionLog("in UnionCommSvrConnBalanceService:: psckSvrDef = [%x] [%s] [%d]\n",
			psckSvrDef,psckSvrDef->ipAddr,psckSvrDef->port);
	if (psckCommSvrConn->psckCommConn[0] != NULL)
		UnionLog("in UnionCommSvrConnBalanceService:: psckSvrDef[0] = [%x]\n",
					psckCommSvrConn->psckCommConn[0]->psckSvrDef);
	if (psckCommSvrConn->psckCommConn[1] != NULL)
		UnionLog("in UnionCommSvrConnBalanceService:: psckSvrDef[1] = [%x]\n",
					psckCommSvrConn->psckCommConn[1]->psckSvrDef);
	*/
reinit:
	if (psckCommSvrConn->psckCommConn[0] != NULL)
	{
		if (psckCommSvrConn->psckCommConn[0]->psckSvrDef == psckSvrDef)
			index = 0;
	}
	else if (psckCommSvrConn->psckCommConn[1] != NULL)
	{
		if (psckCommSvrConn->psckCommConn[1]->psckSvrDef == psckSvrDef)
			index = 1;
	}
	if (index < 0)
	{
		//UnionLog("in UnionCommSvrConnBalanceService:: create connect psckSvrDef = [%x] [%s] [%d]\n",
		//		psckSvrDef,psckSvrDef->ipAddr,psckSvrDef->port);
		for (i = 0; i < 2; i++)
		{
			if (psckCommSvrConn->psckCommConn[i] != NULL)
				continue;
			if ((psckCommSvrConn->psckCommConn[i] = UnionCreateSckSvrConn(psckSvrDef)) == NULL)
			{
				UnionUserErrLog("in UnionCommSvrConnBalanceService:: create connect failure psckSvrDef = [%x] [%s] [%d]\n",
						psckSvrDef,psckSvrDef->ipAddr,psckSvrDef->port);
				psckSvrDef->status = conSckSvrAbnormal;
				retryTimes++;
				goto selectIdle;
			}
			else
			{
				index = i;
				UnionLog("in UnionCommSvrConnBalanceService:: create connect ok psckSvrDef = [%x] [%s] [%d]\n",
						psckSvrDef,psckSvrDef->ipAddr,psckSvrDef->port);
				break;
			}
		}
	}
	if (index < 0)
	{
		UnionUserErrLog("in UnionCommSvrConnBalanceService:: no working server!\n");
		return(errCodeSckCommMDL_NoWorkingServer);
	}
	//UnionLog("in UnionCommSvrConnBalanceService:: index = [%d]\n",index);
	++(psckSvrDef->activeNum);
	if ((ret = UnionCommWithSckSvr(psckCommSvrConn->psckCommConn[index],reqStr,lenOfReqStr,resStr,sizeOfResStr)) < 0)
	{
		if (psckSvrDef->activeNum > 0)
			--(psckSvrDef->activeNum);
		++(psckSvrDef->failTimes);
		UnionUserErrLog("in UnionCommSvrConnBalanceService:: UnionCommWithSckSvr psckSvrDef = [%x] [%s] [%d]\n",
					psckCommSvrConn->psckCommConn[index]->psckSvrDef,
					psckCommSvrConn->psckCommConn[index]->psckSvrDef->ipAddr,
					psckCommSvrConn->psckCommConn[index]->psckSvrDef->port);
		if (ret == errSocketError)
		{
			UnionUserErrLog("in UnionCommSvrConnBalanceService:: errSocketError!\n");
			UnionCloseSckSvrConn(psckCommSvrConn->psckCommConn[index]);
			psckCommSvrConn->psckCommConn[index] = NULL;
			retryTimes++;
			goto selectIdle;
		}
	}
	if (psckSvrDef->activeNum > 0)
		--(psckSvrDef->activeNum);
	++(psckSvrDef->successTimes);
	return(ret);
}

int UnionPrintCommSvrDef()
{
	PUnionCommSvrConn	pcommSvrConn;
	int			i;
	
	if ((pcommSvrConn = UnionConnectCommSvr()) == NULL)
	{
		UnionUserErrLog("in UnionPrintCommSvrDef:: UnionConnectCommSvr!\n");
		return(errCodeParameter);
	}
	
	switch (pcommSvrConn->pcommSvrDef->workingMode)
	{
		case	conCommSvrHotBackupWorkingMode:
			printf("Hot backup working mode\n");
			break;
		case	conCommSvrBalanceWorkingMode:
			printf("Balance working mode\n");
			break;
		case	conCommSvrMaintainWorkingMode:
			printf("Maintaining working mode\n");
			break;
		default:
			printf("Unknown working mode\n");
			break;
	}
	printf("Timeout = [%d]\n",pgunionCommSvrDef->timeout);
	for (i = 0; i < 2; i++)
	{
		printf("%15s %05d ",pcommSvrConn->pcommSvrDef->sckSvrDef[i].ipAddr,pcommSvrConn->pcommSvrDef->sckSvrDef[i].port);
		switch (pcommSvrConn->pcommSvrDef->sckSvrDef[i].status)
		{
			case	conSckSvrAbnormal:
				printf("%10s","abnormal");
				break;
			case	conSckSvrNormal:
				printf("%10s","normal");
				break;
			case	conSckSvrMaintaining:
				printf("%10","maintain");
				break;
			case	conSckSvrColdBackup:
				printf("%10s","coldbck");
				break;
		}
		if (pcommSvrConn->pcommSvrDef->sckSvrDef[i].isPrimaryServer)
			printf(" Primary ");
		else
			printf(" backup  ");
		printf("%10ld %10ld %10ld %ld\n",
			pcommSvrConn->pcommSvrDef->sckSvrDef[i].activeConnNum,
			pcommSvrConn->pcommSvrDef->sckSvrDef[i].successTimes,
			pcommSvrConn->pcommSvrDef->sckSvrDef[i].failTimes,
			pcommSvrConn->pcommSvrDef->sckSvrDef[i].activeNum);
	}
	return(0);	
}

int UnionGetTimeoutOfCommSvr()
{
	int	ret;
	
	if ((ret = UnionConnectCommSvrDefMDL()) < 0)
		return(5);
	else
		return(pgunionCommSvrDef->timeout);
}

int UnionSetTimeoutOfCommSvr(int timeout)
{
	int	ret;
	
	if ((ret = UnionConnectCommSvrDefMDL()) < 0)
		return(ret);
	else
	{
		if (timeout < 0)
			return(errCodeParameter);
		else
		{
			pgunionCommSvrDef->timeout = timeout;
			return(0);
		}
	}
}

int UnionCheckCommSvr()
{
	int	ret;
	int	sckHDL;
	int	i;	
	
	if ((ret = UnionConnectCommSvrDefMDL()) < 0)
	{
		UnionUserErrLog("in UnionCheckCommSvr:: UnionConnectCommSvrDefMDL!\n");
		return(ret);
	}
	
	switch (pgunionCommSvrDef->workingMode)
	{
		case	conCommSvrHotBackupWorkingMode:
		case	conCommSvrBalanceWorkingMode:
			break;
		case	conCommSvrMaintainWorkingMode:
			return(0);
		default:
			UnionUserErrLog("in UnionCheckCommSvr:: Unknown working word!\n");
			return(errCodeSckCommMDL_InvalidWorkingMode);
	}
	for (i = 0; i < 2; i++)
	{	
		if ((sckHDL = UnionCreateSocketClient(pgunionCommSvrDef->sckSvrDef[i].ipAddr,
						pgunionCommSvrDef->sckSvrDef[i].port)) < 0)
		{
			UnionUserErrLog("in UnionCheckCommSvr:: UnionCreateSocketClient [%s] [%d]!\n",
					pgunionCommSvrDef->sckSvrDef[i].ipAddr,
					pgunionCommSvrDef->sckSvrDef[i].port);
			if (pgunionCommSvrDef->sckSvrDef[i].status == conSckSvrNormal)
			{
				UnionSuccessLog("in UnionCheckCommSvr:: set [%s] [%d] abnormal!\n",
					pgunionCommSvrDef->sckSvrDef[i].ipAddr,
					pgunionCommSvrDef->sckSvrDef[i].port);
				pgunionCommSvrDef->sckSvrDef[i].status = conSckSvrAbnormal;
			}
		}
		else
		{
			UnionLog("in UnionCheckCommSvr:: UnionCreateSocketClient [%s] [%d] OK!\n",
					pgunionCommSvrDef->sckSvrDef[i].ipAddr,
					pgunionCommSvrDef->sckSvrDef[i].port);
			UnionCloseSocket(sckHDL);
			if (pgunionCommSvrDef->sckSvrDef[i].status == conSckSvrAbnormal)
			{
				UnionSuccessLog("in UnionCheckCommSvr:: set [%s] [%d] normal!\n",
					pgunionCommSvrDef->sckSvrDef[i].ipAddr,
					pgunionCommSvrDef->sckSvrDef[i].port);
				/*
				pgunionCommSvrDef->sckSvrDef[0].successTimes = 0;
				pgunionCommSvrDef->sckSvrDef[0].failTimes = 0;
				pgunionCommSvrDef->sckSvrDef[1].successTimes = 0;
				pgunionCommSvrDef->sckSvrDef[1].failTimes = 0;
				*/
				pgunionCommSvrDef->sckSvrDef[i].status = conSckSvrNormal;
			}
		}
	}
}

PUnionSckSvrDef UnionSelectSpecifiedSckSvr(char *ipAddr)
{
	int	i;
	int	ret;
		
	if ((ret = UnionConnectCommSvrDefMDL()) < 0)
	{
		UnionUserErrLog("in UnionSelectSpecifiedSckSvr:: UnionConnectCommSvrDefMDL!\n");
		return(NULL);
	}

	for (i = 0; i < 2; i++)
	{
		if (strcmp(pgunionCommSvrDef->sckSvrDef[i].ipAddr,ipAddr) == 0)
			return(&(pgunionCommSvrDef->sckSvrDef[i]));
	}
	UnionUserErrLog("in UnionSelectSpecifiedSckSvr:: [%s] not defined in commSvr\n",ipAddr);
	return(NULL);
}

PUnionSckSvrDef UnionSelectSpecifiedSckSvrOfIndex(int index)
{
	int	i;
	int	ret;
		
	if ((ret = UnionConnectCommSvrDefMDL()) < 0)
	{
		UnionUserErrLog("in UnionSelectSpecifiedSckSvrOfIndex:: UnionConnectCommSvrDefMDL!\n");
		return(NULL);
	}

	switch (index)
	{
		case	1:
		case	2:
			return(&(pgunionCommSvrDef->sckSvrDef[index-1]));
	}
	UnionUserErrLog("in UnionSelectSpecifiedSckSvrOfIndex:: [%d] not defined in commSvr\n",index);
	return(NULL);
}
