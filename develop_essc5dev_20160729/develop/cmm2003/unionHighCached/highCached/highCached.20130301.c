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
#include "unionXMLPackage.h"
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
	int		ret,iCnt,i = 0;
	char		sql[128];
	char		tmpBuf[1024+1];

	if ((ret = UnionConnectHighCached()) < 0)
	{
		UnionUserErrLog("in UnionReloadHighCached:: UnionConnectHighCached!\n");
		return(ret);
	}

	memset(sql,0,sizeof(sql));
	strcpy(sql,"select * from highCached order by ipAddr,port");
	
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionReloadHighCached:: UnionSelectRealDBRecord\n");
		return(ret);
	}

	// 读取总数量
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReloadHighCached:: UnionReadXMLPackageValue[totalNum]\n");
		return(ret);
	}
	iCnt = atoi(tmpBuf);

	pgunionHighCachedTBL->realNum = 0;
	for (i = 0; (i < iCnt) && (pgunionHighCachedTBL->realNum < conMaxNumOfHighCached); i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionReloadHighCached:: UnionLocateXMLPackage\n");
			continue;
		}

		UnionReadXMLPackageValue("ipAddr", pgunionHighCachedTBL->rec[i].ipAddr, sizeof(pgunionHighCachedTBL->rec[i].ipAddr));

		memset(tmpBuf,0,sizeof(tmpBuf));
		UnionReadXMLPackageValue("port", tmpBuf, sizeof(tmpBuf));
		pgunionHighCachedTBL->rec[i].port = atoi(tmpBuf);
		
		memset(tmpBuf,0,sizeof(tmpBuf));
		UnionReadXMLPackageValue("master", tmpBuf, sizeof(tmpBuf));
		pgunionHighCachedTBL->rec[i].master = atoi(tmpBuf);

		memset(tmpBuf,0,sizeof(tmpBuf));
		UnionReadXMLPackageValue("enabled", tmpBuf, sizeof(tmpBuf));
		pgunionHighCachedTBL->rec[i].enabled = atoi(tmpBuf);

		UnionReadXMLPackageValue("remark", pgunionHighCachedTBL->rec[i].remark, sizeof(pgunionHighCachedTBL->rec[i].remark));

		pgunionHighCachedTBL->realNum ++;
	}
	
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

