// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2005/12/30
// Version:	1.0

// 2008/3/25,屏蔽了sig_pipe信号量
//2009-04-21王纯军，在hsmTask.20060808.c基础上改写

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

// 2009-12-5，王纯军增加
int UnionUserSpecFunForFunSvr()
{
	int	ret;
	int	sckHDL;
	
	if ((ret = UnionCheckHsmStatusUsedByHsmTask()) < 0)
	{
		UnionUserErrLog("in UnionUserSpecFunForFunSvr:: UnionCheckHsmStatusUsedByHsmTask!\n");
		return(ret);
	}
	
	// 尝试与密码机建立连接
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

// 2009-12-5，王纯军增加
int UnionCheckHsmStatusUsedByHsmTask()
{
	int	ret;
	 
	// 查找密码机
	if (pgunionSJL06OfMine == NULL)
	{
		if ((pgunionSJL06OfMine = UnionFindWorkingSJL06(UnionGetFunSvrStartVar())) == NULL)
		{
			UnionUserErrLog("in UnionCheckHsmStatusUsedByHsmTask:: UnionFindWorkingSJL06 [%s]\n",UnionGetFunSvrStartVar());
			return(errCodeSJL06MDL_SJL06NotExists);
		}
	}
	/***modify by xusj 20100226 begin ***/
	// 检查密码机状态
	/*********
	if (pgunionSJL06OfMine->dynamicAttr.status == conColdBackupSJL06)	// 2008/3/26,修改
	*********/
	if ( (pgunionSJL06OfMine->dynamicAttr.status == conColdBackupSJL06)
          || (pgunionSJL06OfMine->dynamicAttr.status == conAbnormalSJL06) )	// 2010/2/26,修改
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

// 2009/3/3,王纯军
PUnionSJL06 UnionGetCurrentSJL06()
{
	return(pgunionSJL06OfMine);
}

int UnionGetCurrentHsmSckHDL()
{
	return(gunionHsmSckHDL);
}
// 2009/3/3,王纯军

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
	
	// 2009-12-5,修改,王纯军
	if ((ret = UnionCheckHsmStatusUsedByHsmTask()) < 0)
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: UnionCheckHsmStatusUsedByHsmTask!\n");
		return(ret);
	}
	// 2009-12-5,修改结束

	//UnionSetMyTransInfoResID(conEssResIDHsmCmd);
	// 2008/3/24, added by Wolfgang Wang
	//signal(SIGPIPE,TestPipe);
	signal(SIGPIPE,SIG_IGN);	// 忽略该信号量
	// 2008/3/24, end of addition

retryCreateSckConn:
	if (++retryTimes > 2)
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: create SckConn retry Times > 2!\n");
		return(conAbnormalSJL06);
	}
	// 创建密码机连接
	if (gunionHsmSckHDL < 0)
	{
		if ((gunionHsmSckHDL = UnionCreateSocketClient(pgunionSJL06OfMine->staticAttr.ipAddr,pgunionSJL06OfMine->staticAttr.port)) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: UnionCreateSocketClient [%s] [%d]\n",pgunionSJL06OfMine->staticAttr.ipAddr,pgunionSJL06OfMine->staticAttr.port);
			pgunionSJL06OfMine->dynamicAttr.status = conAbnormalSJL06;
			UnionKillTaskInstanceByAlikeName(pgunionSJL06OfMine->staticAttr.ipAddr);
			return(gunionHsmSckHDL);
		}
		// 2008/3/26，增加
		else
		{
			pgunionSJL06OfMine->dynamicAttr.status = conOnlineSJL06;
		}
		UnionSetThisSJL06LongConnOK();	// 2008/6/6 增加
		// 2008/3/26，增加
		if ((++pgunionSJL06OfMine->dynamicAttr.activeLongConn) <= 0)
			pgunionSJL06OfMine->dynamicAttr.activeLongConn = 1;
	}
	else
	{
		// 2008/6/6 新增
		if (!UnionIsThisSJL06LongConnAbnormal())	
			goto longConnStillOK;
			// 低层通讯时，发现了连接异常
		UnionCloseSocket(gunionHsmSckHDL);	// 关闭当前连接
		if ((--pgunionSJL06OfMine->dynamicAttr.activeLongConn) < 0)
			pgunionSJL06OfMine->dynamicAttr.activeLongConn = 0;
		// 检查密码机状态
		if (!UnionIsSpecHsmNormal(pgunionSJL06OfMine->staticAttr.ipAddr,0))	// 状态不正常
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: [%s] is abnormal!\n",pgunionSJL06OfMine->staticAttr.ipAddr);
			ret = errCodeSJL06MDL_SJL06StillNotOnline;
			UnionCloseSocket(gunionHsmSckHDL);
			if ((--pgunionSJL06OfMine->dynamicAttr.activeLongConn) < 0)
				pgunionSJL06OfMine->dynamicAttr.activeLongConn = 0;
			gunionHsmSckHDL = -1;
			goto retryCreateSckConn;
		}
		// 重新创建密码机连接
		UnionAuditLog("in UnionFunSvrInterprotor:: sckConn error or hsm error! try to recreate socket to [%s] [%d]!\n",pgunionSJL06OfMine->staticAttr.ipAddr,pgunionSJL06OfMine->staticAttr.port);
		if ((gunionHsmSckHDL = UnionCreateSocketClient(pgunionSJL06OfMine->staticAttr.ipAddr,pgunionSJL06OfMine->staticAttr.port)) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: UnionCreateSocketClient [%s] [%d]\n",pgunionSJL06OfMine->staticAttr.ipAddr,pgunionSJL06OfMine->staticAttr.port);
			pgunionSJL06OfMine->dynamicAttr.status = conAbnormalSJL06;
			goto retryCreateSckConn;
		}
		UnionAuditLog("in UnionFunSvrInterprotor:: recreate socket to [%s] [%d] ok!\n",pgunionSJL06OfMine->staticAttr.ipAddr,pgunionSJL06OfMine->staticAttr.port);
		// 重新建连成功
		pgunionSJL06OfMine->dynamicAttr.status = conOnlineSJL06;	// 置密码机状态正常
		if ((++pgunionSJL06OfMine->dynamicAttr.activeLongConn) <= 0)
			pgunionSJL06OfMine->dynamicAttr.activeLongConn = 1;
		UnionSetThisSJL06LongConnOK();
		// 2008/6/6 新增结束
	}
	
longConnStillOK:
	// 处理请求
	if ((ret = UnionHsmTaskInterprotor(gunionHsmSckHDL,pgunionSJL06OfMine,reqStr,lenOfReqStr,resStr,sizeOfResStr)) < 0)
	{
		UnionUserErrLog("in UnionFunSvrInterprotor::UnionHsmTaskInterprotor Error ret = [%d]!\n",ret);
		goto retryCreateSckConn;
	}
	return(ret);
}

// 2008/3/24, 增加
void TestPipe()
{
	// 关闭专门的日志
	UnionCloseSuffixOfMyLogFile();
	UnionDynamicSetLogLevel(-1);
	UnionUserErrLog("in TestPipe:: pipe error occurs...\n");
	UnionDisconnectHsmTask();
	exit(UnionTaskActionBeforeExit());
}
