//	Wolfgang Wang, 2006/8/9

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_	
#endif

#define _realBaseDB_2_x_

#include "UnionProc.h"
#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "UnionStr.h"
#include "unionErrCode.h"

#include "highCached.h"
#include "unionRealBaseDB.h"
#include "unionCommand.h"

PUnionSharedMemoryModule	pgunionHighCachedMDL = NULL;
PUnionHighCachedTBL		pgunionHighCachedTBL = NULL;

int UnionGetNameOfHighCached(char *fileName)
{
	sprintf(fileName,"%s/%s",getenv("UNIONETC"),conConfFileNameOfHighCached);
	return(0);
}

int UnionIsHighCachedConnected()
{
	if ((pgunionHighCachedTBL == NULL) || (!UnionIsSharedMemoryInited(conMDLNameOfHighCachedTBL)))
		return(0);
	else
		return(1);
}

int UnionConnectHighCached()
{
	if (UnionIsHighCachedConnected())	// 已经连接
		return(0);
	
	if ((pgunionHighCachedMDL = UnionConnectSharedMemoryModule(conMDLNameOfHighCachedTBL,
			sizeof(TUnionHighCachedTBL))) == NULL)
	{
		UnionUserErrLog("in UnionConnectHighCached:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgunionHighCachedTBL = (PUnionHighCachedTBL)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionHighCachedMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectHighCached:: PUnionHighCached!\n");
		return(errCodeSharedMemoryModule);
	}
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionHighCachedMDL))
	{
		return(UnionReloadHighCached());
	}
	else
		return(0);
}

int UnionDisconnectHighCached()
{
	pgunionHighCachedTBL = NULL;
	return(UnionDisconnectShareModule(pgunionHighCachedMDL));
}

int UnionRemoveHighCached()
{
	UnionDisconnectHighCached();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfHighCachedTBL));
}

int UnionReloadHighCached()
{
	int			ret, i;
	char			fileName[128];
	char			*p = NULL;

	
	if ((ret = UnionConnectHighCached()) < 0)
	{
		UnionUserErrLog("in UnionReloadHighCached:: UnionConnectHighCached!\n");
		return(ret);
	}


	memset(fileName, 0, sizeof(fileName));
	UnionGetNameOfHighCached(fileName);

	pgunionHighCachedTBL->realNum = 0;

	if((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionReloadHighCached:: UnionInitEnvi ret = %d!\n", ret);
		return(ret);
	}

	for (i = 0, pgunionHighCachedTBL->realNum = 0; i < UnionGetEnviVarNum() && pgunionHighCachedTBL->realNum < conMaxNumOfHighCached; i++)
	{
		// ipAddr
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i, 0)) == NULL || !UnionIsValidIPAddrStr(p))
		{
			UnionUserErrLog("in UnionReloadHighCached:: i = %d  ipAddr = [invalid]!\n",i);
			continue;
		}
		strcpy(pgunionHighCachedTBL->rec[pgunionHighCachedTBL->realNum].ipAddr, p);

		// port
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i, 1)) == NULL || atoi(p) <= 0)
		{
			UnionUserErrLog("in UnionReloadHighCached:: i = %d  port = [invalid]!\n",i);
			continue;
		}
		pgunionHighCachedTBL->rec[pgunionHighCachedTBL->realNum].port = atoi(p);

		// master
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i, 2)) == NULL || (atoi(p) != 1 && atoi(p) != 0))
		{
			UnionUserErrLog("in UnionReloadHighCached:: i = %d  master = [invalid]!\n",i);
			continue;
		}
		pgunionHighCachedTBL->rec[pgunionHighCachedTBL->realNum].master = atoi(p);

		// enabled
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i, 3)) == NULL || (atoi(p) != 1 && atoi(p) != 0))
		{
			UnionUserErrLog("in UnionReloadHighCached:: i = %d  enabled = [invalid]!\n",i);
			continue;
		}
		pgunionHighCachedTBL->rec[pgunionHighCachedTBL->realNum].enabled = atoi(p);

		if ((p = UnionGetEnviVarOfTheIndexByIndex(i, 4)) != NULL)
		{
			strcpy(pgunionHighCachedTBL->rec[pgunionHighCachedTBL->realNum].remark, p);
		}
		
		UnionAuditLog("in UnionReloadHighCached:: index = %d ipAddr = [%s] port = [%d] master = [%d] enabled = [%d]  remark = [%s]!\n",
				i, pgunionHighCachedTBL->rec[pgunionHighCachedTBL->realNum].ipAddr,
				pgunionHighCachedTBL->rec[pgunionHighCachedTBL->realNum].port,
				pgunionHighCachedTBL->rec[pgunionHighCachedTBL->realNum].master,
				pgunionHighCachedTBL->rec[pgunionHighCachedTBL->realNum].enabled,
				pgunionHighCachedTBL->rec[pgunionHighCachedTBL->realNum].remark);

		pgunionHighCachedTBL->realNum++;

	}

	UnionClearEnvi();
	return(pgunionHighCachedTBL->realNum);
}

int UnionPrintHighCachedTBLToFile(FILE *fp)
{
	int	i;
	int	ret;
	int	num = 0;
	
	if (fp == NULL)
		fp = stdout;
		
	if ((ret = UnionConnectHighCached()) < 0)
	{
		UnionUserErrLog("in UnionPrintHighCachedTBLToFile:: UnionConnectHighCached!\n");
		return(ret);
	}
	
	fprintf(fp,"%16s %6s %8s %4s %s\n","IP地址","端口","主备标识","激活","备注");
	for (i = 0; i < pgunionHighCachedTBL->realNum; i++)
	{
		if (UnionPrintHighCachedToFile(&(pgunionHighCachedTBL->rec[i]),fp) < 0)
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
	fprintf(fp,"数量 = [%d]\n",num);
	return(0);
}

int UnionPrintHighCachedToFile(PUnionHighCached phighCached,FILE *fp)
{
	char	master[16+1];
	char	enabled[16+1];
	
	memset(enabled,0,sizeof(enabled));
	if (phighCached->enabled == 1)
		strcpy(enabled,"ON");
	else
		strcpy(enabled,"OFF");
	
	memset(master,0,sizeof(master));
	if (phighCached->master == 1)
		strcpy(master,"主");
	else
		strcpy(master,"备");

	if ((phighCached == NULL) || (fp == NULL))
	{
		UnionUserErrLog("in UnionPrintHighCachedToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
	fprintf(fp,"%16s %6d %8s %4s %s\n",phighCached->ipAddr,phighCached->port,master,enabled,phighCached->remark);
	return(0);
}

PUnionHighCachedTBL UnionGetHighCachedTBL()
{
	int	ret;
	
	if ((ret = UnionConnectHighCached()) < 0)
	{
		UnionUserErrLog("in UnionGetHighCachedTBL:: UnionConnectHighCached!\n");
		return(NULL);
	}
	
	return(pgunionHighCachedTBL);
}

