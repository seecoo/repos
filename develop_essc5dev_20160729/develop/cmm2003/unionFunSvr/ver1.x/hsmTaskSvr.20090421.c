// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2005/12/30
// Version:	1.0

// 2008/3/25,������sig_pipe�ź���
//2009-04-21����������hsmTask.20060808.c�����ϸ�д

#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "unionErrCode.h"
#include "unionFunSvrName.h"

#ifndef _UnionSJL06_2_x_Above_
#define _UnionSJL06_2_x_Above_
#endif
#include "sjl06.h"
#include "sjl06Grp.h"
#include "UnionLog.h"

PUnionSJL06	pgunionSJL06OfMine;
int		gunionHsmSckHDL = -1;

void TestPipe();

// 2009-12-5������������
int UnionUserSpecFunForFunSvr()
{
	int	ret;
	int	sckHDL;
	
	if ((ret = UnionCheckHsmStatusUsedByHsmTask()) < 0)
	{
		UnionUserErrLog("in UnionUserSpecFunForFunSvr:: UnionCheckHsmStatusUsedByHsmTask!\n");
		return(ret);
	}
	
	// �������������������
	if ((sckHDL = UnionCreateSocketClient(pgunionSJL06OfMine->staticAttr.ipAddr,pgunionSJL06OfMine->staticAttr.port)) < 0)
	{
		UnionUserErrLog("in UnionUserSpecFunForFunSvr:: UnionCreateSocketClient [%s] [%d]\n",pgunionSJL06OfMine->staticAttr.ipAddr,pgunionSJL06OfMine->staticAttr.port);
		pgunionSJL06OfMine->dynamicAttr.status = conAbnormalSJL06;
		UnionKillTaskInstanceByAlikeName(pgunionSJL06OfMine->staticAttr.ipAddr);
		return(sckHDL);
	}
	UnionCloseSocket(sckHDL);
	UnionAuditLog("in UnionUserSpecFunForFunSvr:: [%s] [%d] is OK!\n",pgunionSJL06OfMine->staticAttr.ipAddr,pgunionSJL06OfMine->staticAttr.port);
	return(0);
}

// 2009-12-5������������
int UnionCheckHsmStatusUsedByHsmTask()
{
	int	ret;
	 
	// ���������
	if (pgunionSJL06OfMine == NULL)
	{
		if ((pgunionSJL06OfMine = UnionFindWorkingSJL06(UnionGetFunSvrStartVar())) == NULL)
		{
			UnionUserErrLog("in UnionCheckHsmStatusUsedByHsmTask:: UnionFindWorkingSJL06 [%s]\n",UnionGetFunSvrStartVar());
			return(errCodeSJL06MDL_SJL06NotExists);
		}
	}
	/***modify by xusj 20100226 begin ***/
	// ��������״̬
	/*********
	if (pgunionSJL06OfMine->dynamicAttr.status == conColdBackupSJL06)	// 2008/3/26,�޸�
	*********/
	if ( (pgunionSJL06OfMine->dynamicAttr.status == conColdBackupSJL06)
          || (pgunionSJL06OfMine->dynamicAttr.status == conAbnormalSJL06) )	// 2010/2/26,�޸�
	{
		UnionUserErrLog("in UnionCheckHsmStatusUsedByHsmTask:: [%s] not online!\n",UnionGetFunSvrStartVar());
		return(errCodeSJL06MDL_SJL06StillNotOnline);
	}
	/***modify by xusj 20100226 end ***/
	
	return(0);
}

char *UnionGetHsmIPAddrOfThisHsmTask()
{
	return(UnionGetFunSvrStartVar());
}

// 2009/3/3,������
PUnionSJL06 UnionGetCurrentSJL06()
{
	return(pgunionSJL06OfMine);
}

int UnionGetCurrentHsmSckHDL()
{
	return(gunionHsmSckHDL);
}
// 2009/3/3,������

int UnionDisconnectHsmTask()
{
	if (gunionHsmSckHDL < 0)
		return(0);
	if (pgunionSJL06OfMine == NULL)
		return(0);
	UnionCloseSocket(gunionHsmSckHDL);
	if ((--pgunionSJL06OfMine->dynamicAttr.activeLongConn) < 0)
		pgunionSJL06OfMine->dynamicAttr.activeLongConn = 0;
	gunionHsmSckHDL = -1;
	if (pgunionSJL06OfMine->dynamicAttr.status == conAbnormalSJL06)
		UnionCloseTaskInstanceByAlikeName(pgunionSJL06OfMine->staticAttr.ipAddr);
	return(0);
}
	
int UnionFunSvrInterprotor(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	int			ret;
	int			retryTimes = 0;
	
	// 2009-12-5,�޸�,������
	if ((ret = UnionCheckHsmStatusUsedByHsmTask()) < 0)
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: UnionCheckHsmStatusUsedByHsmTask!\n");
		return(ret);
	}
	// 2009-12-5,�޸Ľ���

	//UnionSetMyTransInfoResID(conEssResIDHsmCmd);
	// 2008/3/24, added by Wolfgang Wang
	//signal(SIGPIPE,TestPipe);
	signal(SIGPIPE,SIG_IGN);	// ���Ը��ź���
	// 2008/3/24, end of addition

retryCreateSckConn:
	if (++retryTimes > 2)
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: create SckConn retry Times > 2!\n");
		return(conAbnormalSJL06);
	}
	// �������������
	if (gunionHsmSckHDL < 0)
	{
		if ((gunionHsmSckHDL = UnionCreateSocketClient(pgunionSJL06OfMine->staticAttr.ipAddr,pgunionSJL06OfMine->staticAttr.port)) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: UnionCreateSocketClient [%s] [%d]\n",pgunionSJL06OfMine->staticAttr.ipAddr,pgunionSJL06OfMine->staticAttr.port);
			pgunionSJL06OfMine->dynamicAttr.status = conAbnormalSJL06;
			UnionKillTaskInstanceByAlikeName(pgunionSJL06OfMine->staticAttr.ipAddr);
			return(gunionHsmSckHDL);
		}
		// 2008/3/26������
		else
		{
			pgunionSJL06OfMine->dynamicAttr.status = conOnlineSJL06;
		}
		UnionSetThisSJL06LongConnOK();	// 2008/6/6 ����
		// 2008/3/26������
		if ((++pgunionSJL06OfMine->dynamicAttr.activeLongConn) <= 0)
			pgunionSJL06OfMine->dynamicAttr.activeLongConn = 1;
	}
	else
	{
		// 2008/6/6 ����
		if (!UnionIsThisSJL06LongConnAbnormal())	
			goto longConnStillOK;
			// �Ͳ�ͨѶʱ�������������쳣
		UnionCloseSocket(gunionHsmSckHDL);	// �رյ�ǰ����
		if ((--pgunionSJL06OfMine->dynamicAttr.activeLongConn) < 0)
			pgunionSJL06OfMine->dynamicAttr.activeLongConn = 0;
		// ��������״̬
		if (!UnionIsSpecHsmNormal(pgunionSJL06OfMine->staticAttr.ipAddr,0))	// ״̬������
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: [%s] is abnormal!\n",pgunionSJL06OfMine->staticAttr.ipAddr);
			ret = errCodeSJL06MDL_SJL06StillNotOnline;
			UnionCloseSocket(gunionHsmSckHDL);
			if ((--pgunionSJL06OfMine->dynamicAttr.activeLongConn) < 0)
				pgunionSJL06OfMine->dynamicAttr.activeLongConn = 0;
			gunionHsmSckHDL = -1;
			goto retryCreateSckConn;
		}
		// ���´������������
		UnionAuditLog("in UnionFunSvrInterprotor:: sckConn error or hsm error! try to recreate socket to [%s] [%d]!\n",pgunionSJL06OfMine->staticAttr.ipAddr,pgunionSJL06OfMine->staticAttr.port);
		if ((gunionHsmSckHDL = UnionCreateSocketClient(pgunionSJL06OfMine->staticAttr.ipAddr,pgunionSJL06OfMine->staticAttr.port)) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: UnionCreateSocketClient [%s] [%d]\n",pgunionSJL06OfMine->staticAttr.ipAddr,pgunionSJL06OfMine->staticAttr.port);
			pgunionSJL06OfMine->dynamicAttr.status = conAbnormalSJL06;
			goto retryCreateSckConn;
		}
		UnionAuditLog("in UnionFunSvrInterprotor:: recreate socket to [%s] [%d] ok!\n",pgunionSJL06OfMine->staticAttr.ipAddr,pgunionSJL06OfMine->staticAttr.port);
		// ���½����ɹ�
		pgunionSJL06OfMine->dynamicAttr.status = conOnlineSJL06;	// �������״̬����
		if ((++pgunionSJL06OfMine->dynamicAttr.activeLongConn) <= 0)
			pgunionSJL06OfMine->dynamicAttr.activeLongConn = 1;
		UnionSetThisSJL06LongConnOK();
		// 2008/6/6 ��������
	}
	
longConnStillOK:
	// ��������
	if ((ret = UnionHsmTaskInterprotor(gunionHsmSckHDL,pgunionSJL06OfMine,reqStr,lenOfReqStr,resStr,sizeOfResStr)) < 0)
	{
		UnionUserErrLog("in UnionFunSvrInterprotor::UnionHsmTaskInterprotor Error ret = [%d]!\n",ret);
		goto retryCreateSckConn;
	}
	return(ret);
}

// 2008/3/24, ����
void TestPipe()
{
	// �ر�ר�ŵ���־
	UnionCloseSuffixOfMyLogFile();
	UnionDynamicSetLogLevel(-1);
	UnionUserErrLog("in TestPipe:: pipe error occurs...\n");
	UnionDisconnectHsmTask();
	exit(UnionTaskActionBeforeExit());
}
