//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2003/09/12
//	Version:	1.0

// 2004/06/22，Wolfgang Wang升级为2.0
// 升级是因为修改了TUnionSJL06StaticAttr的结构

// 2006/7/26,在sjl06Grp2.0.20050816.c基础上升级，修改了UnionAutoLoadSJL06IntoSJL06MDL
// 这个函数自动从$UNIONETC/HsmCfg/hsmGrp.conf定义的文件中加载密码机，如果密码机未定义，自动创建其定义

// 2007/12/22，在UnionCheckStatusOfWorkingSJL06中增加空测试指令。
// 	如果测试指令=null或NULL，那么在检测密码机状态时，只建立与密码机的连接，如果建连成功，就认为密码机在线。
// 2010-11-30 HuangBaoxin,	升级UnionPrintAllWorkingSJL06ToFile函数，打印加密机在内存中的索引
//				增加UnionClearWorkingSJL06函数，清除共享内存中加密机数据

#define _UnionLogMDL_3_x_
#define _UnionEnv_3_x_

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "UnionStr.h"
#include "unionVersion.h"
#include "sjl06.h"
#include "sjl06Grp.h"
#include "unionMenu.h"
#include "unionCommand.h"
#include "unionModule.h"
#include "unionErrCode.h"
#include "UnionLog.h"

#define _SJL06CmdForJK_IC_RSA_
#define _RacalCmdForNewRacal_
#include "sjl06Cmd.h"

#define _UnionTask_3_x_
#include "UnionTask.h"

#ifdef _useComplexDB_
#include "unionComplexDBRecord.h"
#include "unionHsm.h"
#endif

PUnionSharedMemoryModule	pgunionSJL06GrpMDL = NULL;
PUnionSJL06			pgunionSJL06Grp = NULL;

// 2006/08/08增加
PUnionSJL06 UnionGetCurrentSJL06Grp()
{
	if (UnionConnectWorkingSJL06MDL() < 0)
		return(NULL);
	else
		return(pgunionSJL06Grp);
}

// 2006/08/08增加
int UnionGetMaxNumOfSJL06InGrp()
{
	return(conMaxNumOfSJL06);
}

#ifndef _useComplexDB_
int UnionGetNameOfAutoRegisteredHsmConf(char *fileName)
{
	sprintf(fileName,"%s/HsmCfg/hsmGrp.conf",getenv("UNIONETC"));
	return(0);
}
#else
int UnionGetNameOfAutoRegisteredHsmConf(char *fileName)
{
	sprintf(fileName,"hsmGroup");
	return(0);
}
#endif

int UnionIsWorkingSJL06MDLConnected()
{
	if ((pgunionSJL06Grp == NULL) || (!UnionIsSharedMemoryInited(conMDLNameOfUnionSJL06TBL)))
		return(0);
	else
		return(1);
}

#ifndef _useComplexDB_
int UnionAutoLoadSJL06IntoSJL06MDL()
{
	char	fileName[512];
	FILE	*fp;
	int	ret;
	int	num=0;
	char	tmpBuf[1024+1];
	int	index;
	char	*ipAddr,*connNumPtr;
	int	connNum;
	char	hsmName[10];
	char	taskName[128];

	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		UnionUserErrLog("in UnionAutoLoadSJL06IntoSJL06MDL:: UnionConnectWorkingSJL06MDL\n");
		return(ret);
	}

	memset(fileName,0,sizeof(fileName));
	if ((ret = UnionGetNameOfAutoRegisteredHsmConf(fileName)) < 0)
	{
		UnionUserErrLog("in UnionAutoLoadSJL06IntoSJL06MDL:: UnionGetNameOfAutoRegisteredHsmConf!\n");
		return(ret);
	}

	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionAutoLoadSJL06IntoSJL06MDL:: fopen [%s]\n",fileName);
		return(ret);
	}

	while (!feof(fp))
	{
		// 从文件中读取一行
		memset(tmpBuf,0,sizeof(tmpBuf));
		if (UnionReadOneFileLine(fp,tmpBuf) == 0)
			continue;
		UnionFilterHeadAndTailBlank(tmpBuf);	// 去掉头尾的空格
		if (tmpBuf[0] == '#')	// 注释行
			continue;
		if ((connNumPtr = strstr(tmpBuf,"=")) == NULL)	// 未定义缺省启动连接数
			connNum = 1;
		else
		{
			connNum = atoi(connNumPtr+1);
			*connNumPtr = 0;
		}
		if (connNum <= 0)
			connNum = 1;
		ipAddr = tmpBuf;
		if (!UnionIsValidIPAddrStr(ipAddr))	// 判断是不是合法的IP地址
			continue;
		if ((ret = UnionCreateSJL06RecWhenNotExist(ipAddr)) < 0)	// 如果对应的配置文件不存在，创建这个配置文件
		{
			UnionUserErrLog("in UnionAutoLoadSJL06IntoSJL06MDL:: UnionCreateSJL06RecWhenNotExist [%s]\n",ipAddr);
			continue;
		}
		// 加载到密码机注册表中
		if ((ret = UnionAddWorkingSJL06(ipAddr)) < 0)
		{
			UnionUserErrLog("in UnionAutoLoadSJL06IntoSJL06MDL:: UnionAddWorkingSJL06 [%s]\n",ipAddr);
			continue;
		}
		UnionSetWorkingSJL06Online(UnionFindWorkingSJL06(ipAddr));
		num++;
#ifdef _KillHsmTaskAutomatically_
		// 检查是否定义了名称为密码机IP地址的任务类型
		if (UnionExistsTaskClassOfAlikeName(ipAddr) > 0)	// 已定义
			continue;
		// 未定义密码机任务，增加一个密码机任务
		sprintf(taskName,"hsmTask %s",ipAddr);
		if ((ret = UnionAddNewTaskClass(taskName,taskName,connNum,"hsmTask")) < 0)
		{
			UnionUserErrLog("in UnionAutoLoadSJL06IntoSJL06MDL:: UnionAddNewTaskClass [%s] error!\n",taskName);
			continue;
		}
#endif
	}
	fclose(fp);
	UnionSuccessLog("in UnionAutoLoadSJL06IntoSJL06MDL:: [%04d] hsm has been reloaded!\n",num);
	return(num);
}
#else
int UnionAutoLoadSJL06IntoSJL06MDL()
{
	int	ret;
	int	num=0;
	char	tmpBuf[1024+1];
	int	i = 0;
	int iCnt = 0;
	char	*ipAddr,*connNumPtr;
	int	connNum = 0;
	char	hsmName[10];
	char	taskName[128];
	TUnionHsm tHsm[conMaxNumOfSJL06];

	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		UnionUserErrLog("in UnionAutoLoadSJL06IntoSJL06MDL:: UnionConnectWorkingSJL06MDL\n");
		return(ret);
	}

	memset(tHsm, 0, conMaxNumOfSJL06 * sizeof(TUnionHsm));
	if( (iCnt = UnionBatchReadHsmRec("active=1|", tHsm, conMaxNumOfSJL06)) < 0 )
	{
		UnionUserErrLog("in UnionAutoLoadSJL06IntoSJL06MDL:: UnionBatchReadHsmRec\n");
		return (iCnt);
	}

	for(i = 0; i < iCnt; i++)
	{
		if (!UnionIsValidIPAddrStr(tHsm[i].ipAddr))	// 判断是不是合法的IP地址
			continue;

		// 加载到密码机注册表中
		if ((ret = UnionAddWorkingSJL06(tHsm[i].ipAddr)) < 0)
		{
			UnionUserErrLog("in UnionAutoLoadSJL06IntoSJL06MDL:: UnionAddWorkingSJL06 [%s]\n", tHsm[i].ipAddr);
			continue;
		}

		UnionSetWorkingSJL06Online(UnionFindWorkingSJL06(tHsm[i].ipAddr));
		num++;

#ifdef _KillHsmTaskAutomatically_
		if (connNum <= 0)
			connNum = 1;
		// 检查是否定义了名称为密码机IP地址的任务类型
		if (UnionExistsTaskClassOfAlikeName(tHsm[i].ipAddr) > 0)	// 已定义
			continue;
		// 未定义密码机任务，增加一个密码机任务
		sprintf(taskName, "hsmTask %s", tHsm[i].ipAddr);
		if ((ret = UnionAddNewTaskClass(taskName, taskName, connNum, "hsmTask")) < 0)
		{
			UnionUserErrLog("in UnionAutoLoadSJL06IntoSJL06MDL:: UnionAddNewTaskClass [%s] error!\n",taskName);
			continue;
		}
#endif

	}

	UnionSuccessLog("in UnionAutoLoadSJL06IntoSJL06MDL:: [%04d] hsm has been reloaded!\n", num);
	return(num);
}
#endif

// Module Layer Functions
int UnionConnectWorkingSJL06MDL()
{
	int	index;

	if (UnionIsWorkingSJL06MDLConnected())	// 已经连接
		return(0);

	if ((pgunionSJL06GrpMDL = UnionConnectSharedMemoryModule(conMDLNameOfUnionSJL06TBL,
			sizeof(TUnionSJL06) * conMaxNumOfSJL06)) == NULL)
	{
		UnionUserErrLog("in UnionConnectWorkingSJL06MDL:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgunionSJL06Grp = (PUnionSJL06)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionSJL06GrpMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectWorkingSJL06MDL:: PUnionSJL06!\n");
		return(errCodeSharedMemoryModule);
	}

	if (UnionIsNewCreatedSharedMemoryModule(pgunionSJL06GrpMDL))
		return(0);
	else
		return(0);
}

int UnionDisconnectWorkingSJL06MDL()
{
	pgunionSJL06Grp = NULL;

	return(UnionDisconnectShareModule(pgunionSJL06GrpMDL));
}

int UnionRemoveWorkingSJL06MDL()
{
	UnionDisconnectWorkingSJL06MDL();

	return(UnionRemoveSharedMemoryModule(conMDLNameOfUnionSJL06TBL));
}

PUnionSJL06 UnionFindWorkingSJL06(char *ipAddr)
{
	int		index;
	PUnionSJL06	pSJL06;
	int		ret;

	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		UnionUserErrLog("in UnionFindWorkingSJL06:: UnionConnectWorkingSJL06MDL!\n");
		return(NULL);
	}

	for (index = 0; index < conMaxNumOfSJL06; index++)
	{
		pSJL06 = pgunionSJL06Grp + index;
		if (strcmp(pSJL06->staticAttr.ipAddr,ipAddr) == 0)
			return(pSJL06);
	}
	return(NULL);
}

int UnionExistWorkingSJL06(PUnionSJL06 pSJL06)
{
	int		index;
	int		ret;
	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		UnionUserErrLog("in UnionExistWorkingSJL06:: UnionConnectWorkingSJL06MDL!\n");
		return(0);
	}

	for (index = 0; index < conMaxNumOfSJL06; index++)
	{
		if (strcmp(pSJL06->staticAttr.ipAddr,(pgunionSJL06Grp + index)->staticAttr.ipAddr) == 0)
			return(1);
	}
	return(0);
}


int UnionAddWorkingSJL06(char *ipAddr)
{
	int		index;
	TUnionSJL06	sjl06;
	int		ret;

	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		UnionUserErrLog("in UnionAddWorkingSJL06:: UnionConnectWorkingSJL06MDL!\n");
		return(ret);
	}

	if (!UnionIsValidIPAddrStr(ipAddr))
	{
		UnionUserErrLog("in UnionAddWorkingSJL06:: invalid ipaddr [%s]\n",ipAddr);
		return(errCodeParameter);
	}

	memset(&sjl06,0,sizeof(sjl06));
	strcpy(sjl06.staticAttr.ipAddr,ipAddr);

	if ((ret = UnionSelectSJL06Rec(&sjl06)) < 0)
	{
		UnionUserErrLog("in UnionAddWorkingSJL06::UnionSelectSJL06Rec [%s] non-exists!\n",ipAddr);
		return(ret);
	}

	if (UnionExistWorkingSJL06(&sjl06))
	{
		//UnionUserErrLog("in UnionAddWorkingSJL06:: [%s] exists already!\n",ipAddr);
		return(0);
	}

	sjl06.dynamicAttr.activeLongConn = 0;
	sjl06.dynamicAttr.normalCmdTimes = sjl06.dynamicAttr.timeoutTimes = sjl06.dynamicAttr.abnormalCmdTimes = sjl06.dynamicAttr.connFailTimes = 0;
	for (index = 0; index < conMaxNumOfSJL06; index++)
	{
		if (strlen((pgunionSJL06Grp + index)->staticAttr.ipAddr) == 0)
		{
			//memset(&(sjl06.dynamicAttr),0,sizeof(sjl06.dynamicAttr));
			//UnionPrintStatusOfWorkingSJL06(&sjl06);
			memcpy((pgunionSJL06Grp+index),&sjl06,sizeof(sjl06));
			return(0);
		}
	}
	UnionUserErrLog("in UnionAddWorkingSJL06:: SJL06 Buffer is Full!\n");

	return(errCodeSJL06MDL_WorkingTableFull);
}

int UnionDeleteWorkingSJL06(char *ipAddr)
{
	PUnionSJL06	pSJL06;
	int		ret;

	if ((pSJL06 = UnionFindWorkingSJL06(ipAddr)) == NULL)
	{
		UnionUserErrLog("in UnionDeleteWorkingSJL06:: [%s] non-exists!\n",ipAddr);
		return(errCodeSJL06MDL_SJL06NotExists);
	}

	if (pSJL06->dynamicAttr.status == conOnlineSJL06)
	{
		UnionUserErrLog("in UnionDeleteWorkingSJL06:: [%s] still online!\n",ipAddr);
		return(errCodeSJL06MDL_SJL06StillOnline);
	}

#ifdef _KillHsmTaskAutomatically_
	UnionCloseTaskInstanceByAlikeName(ipAddr);
#endif
	if ((ret = UnionUpdateSJL06DynamicAttr(pSJL06)) < 0)
	{
		UnionUserErrLog("in UnionDeleteWorkingSJL06:: UnionUpdateSJL06DynamicAttr [%s]\n",ipAddr);
		return(ret);
	}
	memset(pSJL06,0,sizeof(*pSJL06));
	return(0);
}

int UnionSetWorkingSJL06Abnormal(PUnionSJL06 pSJL06)
{
	int	ret;

	if (pSJL06 == NULL)
	{
		UnionUserErrLog("in UnionSetWorkingSJL06Abnormal:: NullPointer\n");
		return(errCodeParameter);
	}

	UnionAuditLog("in UnionSetWorkingSJL06Abnormal:: status=[%d]!\n",pSJL06->dynamicAttr.status);
	if (pSJL06->dynamicAttr.status == conAbnormalSJL06)
		return(errCodeSJL06MDL_SJL06AlreadyAbnormal);

	UnionAuditLog("in UnionSetWorkingSJL06Abnormal:: set [%s] abnormal!\n", pSJL06->staticAttr.ipAddr);
#ifdef _KillHsmTaskAutomatically_
	UnionAuditLog("in UnionSetWorkingSJL06Online:: UnionCloseTaskInstanceByAlikeName [%s]!\n",pSJL06->staticAttr.ipAddr);
	UnionKillTaskInstanceByAlikeName(pSJL06->staticAttr.ipAddr);
#endif
	pSJL06->dynamicAttr.status = conAbnormalSJL06;
	UnionGetFullSystemDateTime(pSJL06->dynamicAttr.lastDownDate);
	++pSJL06->dynamicAttr.downTimes;

	return(UnionUpdateSJL06DynamicAttr(pSJL06));
}

int UnionSetWorkingSJL06Online(PUnionSJL06 pSJL06)
{
	int	ret;

	if (pSJL06 == NULL)
	{
		UnionUserErrLog("in UnionSetWorkingSJL06Online:: NullPointer\n");
		return(errCodeParameter);
	}

	if (pSJL06->dynamicAttr.status == conOnlineSJL06)
		return(errCodeSJL06MDL_SJL06AlreadyOnline);
	pSJL06->dynamicAttr.status = conOnlineSJL06;
	pSJL06->dynamicAttr.continueFailTimes = 0;
#ifdef _KillHsmTaskAutomatically_
	UnionAuditLog("in UnionSetWorkingSJL06Online:: UnionReloadTaskTBL!\n");
	UnionReloadTaskTBL();
#endif
	return(UnionUpdateSJL06DynamicAttr(pSJL06));
}


int UnionSetWorkingSJL06ColdBackup(PUnionSJL06 pSJL06)
{
	int	ret;

	if (pSJL06 == NULL)
	{
		UnionUserErrLog("in UnionSetWorkingSJL06ColdBackup:: NullPointer\n");
		return(errCodeParameter);
	}
	if (pSJL06->dynamicAttr.status == conColdBackupSJL06)
		return(errCodeSJL06MDL_SJL06AlreadyBackup);

#ifdef _KillHsmTaskAutomatically_
	UnionCloseTaskInstanceByAlikeName(pSJL06->staticAttr.ipAddr);
#endif
	pSJL06->dynamicAttr.status = conColdBackupSJL06;

	return(UnionUpdateSJL06DynamicAttr(pSJL06));
}

int UnionIsOnlineWorkingSJL06(char *ipAddr)
{
	int	ret;

	PUnionSJL06	pSJL06;

	if ((pSJL06 = UnionFindWorkingSJL06(ipAddr)) == NULL)
		return(0);
	if (pSJL06->dynamicAttr.status == conOnlineSJL06)
		return(1);
	else
		return(0);
}

int UnionPrintStatusOfWorkingSJL06(PUnionSJL06 pSJL06)
{
	return(UnionPrintSJL06Rec(pSJL06));
}

int UnionPrintWorkingSJL06(char *ipAddr)
{
	PUnionSJL06	pSJL06;

	if ((pSJL06 = UnionFindWorkingSJL06(ipAddr)) == NULL)
	{
		printf("[%s] not exists in working hsm-spool!\n",ipAddr);
		return(errCodeSJL06MDL_SJL06NotExists);
	}

	return(UnionPrintStatusOfWorkingSJL06(pSJL06));
}

int UnionPrintAllWorkingSJL06ToFile(FILE *fp)
{
	int		index;
	PUnionSJL06	pSJL06;
	int		hsmNum = 0;
	int		ret;

	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAllWorkingSJL06ToFile:: UnionConnectWorkingSJL06MDL!\n");
		return(ret);
	}
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintAllWorkingSJL06ToFile:: fp is null!\n");
		return(errCodeParameter);
	}

	for (index = 0; index < conMaxNumOfSJL06; index++)
	{
		pSJL06 = pgunionSJL06Grp + index;
		if (strlen(pSJL06->staticAttr.ipAddr) == 0)
			continue;
		fprintf(fp,"[%d] %5s %15s %6d ",
			index,
			pSJL06->staticAttr.hsmGrpID,
			pSJL06->staticAttr.ipAddr,
			pSJL06->staticAttr.port);
		switch (pSJL06->dynamicAttr.status)
		{
			case	'0':
				fprintf(fp,"  异常 ");
				break;
			case	'1':
				fprintf(fp,"  在线 ");
				break;
			case	'2':
				fprintf(fp,"  备份 ");
				break;
			default:
				fprintf(fp,"  不明 ");
				break;
		}
		fprintf(fp,"%4d %12ld %12ld",
			pSJL06->dynamicAttr.activeLongConn,
			pSJL06->dynamicAttr.timeoutTimes + pSJL06->dynamicAttr.connFailTimes,
			pSJL06->dynamicAttr.normalCmdTimes + pSJL06->dynamicAttr.abnormalCmdTimes);
		fprintf(fp,"\n");
		hsmNum++;
		if ((hsmNum > 0) && (hsmNum % 23 == 0) && ((fp == stdout) || (fp == stderr)))
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
	}
	fprintf(fp,"hsmNum = [%d]\n",hsmNum);
	return(0);
}

int UnionPrintAllWorkingSJL06()
{
	return(UnionPrintAllWorkingSJL06ToFile(stdout));
}

int UnionRefreshDynamicAttrsOfAllWorkingSJL06()
{
	int		index;
	PUnionSJL06	pSJL06;
	int		ret;

	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		UnionUserErrLog("in UnionRefreshDynamicAttrsOfAllWorkingSJL06:: UnionConnectWorkingSJL06MDL!\n");
		return(ret);
	}

	for (index = 0; index < conMaxNumOfSJL06; index++)
	{
		pSJL06 = pgunionSJL06Grp + index;
		if (strlen(pSJL06->staticAttr.ipAddr) == 0)
			continue;
		if ((ret = UnionUpdateSJL06DynamicAttr(pSJL06)) < 0)
		{
			UnionUserErrLog("in UnionRefreshDynamicAttrsOfAllWorkingSJL06:: UnionUpdateSJL06DynamicAttr [%s]\n",pSJL06->staticAttr.ipAddr);
		}
	}
	return(0);
}


int UnionOutputWorkingSJL06(PUnionSJL06 pSJL06,int expectedNum)
{
	int		index;
	int		realNum;
	int		ret;

	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		UnionUserErrLog("in UnionOutputWorkingSJL06:: UnionConnectWorkingSJL06MDL!\n");
		return(ret);
	}

	for (index = 0,realNum = 0; index < conMaxNumOfSJL06; index++)
	{
		if (strlen((pgunionSJL06Grp + index)->staticAttr.ipAddr) != 0)
		{
			if (realNum >= expectedNum)
			{
				UnionUserErrLog("in UnionOutputWorkingSJL06:: real num [%d] > expected [%d]\n",realNum+1,expectedNum);
				return(errCodeSmallBuffer);
			}
			memcpy((pSJL06+realNum),(pgunionSJL06Grp+index),sizeof(TUnionSJL06));
			realNum++;
		}
	}
	return(realNum);
}

int UnionFindAllWorkingSJL06InGroup(char *hsmGrpID,PUnionSJL06 pSJL06[],int expectedNum)
{
	int		index;
	int		realNum;
	int		ret;

	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		UnionUserErrLog("in UnionFindAllWorkingSJL06InGroup:: UnionConnectWorkingSJL06MDL!\n");
		return(ret);
	}

	for (index = 0,realNum = 0; index < conMaxNumOfSJL06; index++)
	{
		if (strlen((pgunionSJL06Grp + index)->staticAttr.ipAddr) == 0)
			continue;
		if (strncmp((pgunionSJL06Grp + index)->staticAttr.hsmGrpID,hsmGrpID,3) != 0)
			continue;
		if ((pgunionSJL06Grp + index)->dynamicAttr.status != conOnlineSJL06)
			continue;
		if (realNum >= expectedNum)
		{
			UnionUserErrLog("in UnionFindAllWorkingSJL06InGroup:: real num [%d] > expected [%d]\n",realNum+1,expectedNum);
			return(errCodeSmallBuffer);
		}
		pSJL06[realNum] = pgunionSJL06Grp+index;
		realNum++;
	}
	return(realNum);
}

// is rewritten by Wolfgang Wang, 2004/06/22
int UnionCheckStatusOfWorkingSJL06()
{
	int		index;
	PUnionSJL06	pSJL06;
	int		ret;
	char		buf[128];
	char		resBuf[128];
	char		traceStr[1024];
	int		len;
	int		taskNum;

	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		UnionUserErrLog("in UnionCheckStatusOfWorkingSJL06:: UnionConnectWorkingSJL06MDL!\n");
		return(ret);
	}

	for (index = 0; index < conMaxNumOfSJL06; index++)

	{
		pSJL06 = pgunionSJL06Grp + index;

		/***del by xusj begin 20091231***
		// 2008/10/24,
		taskNum = UnionExistsTaskOfAlikeName(pSJL06->staticAttr.ipAddr);
		if ((taskNum >= 0) && (taskNum != pSJL06->dynamicAttr.activeLongConn))
			pSJL06->dynamicAttr.activeLongConn = taskNum;
		// end of addition of 2008/10/24
		***del by xusj end 20091231***/
		if (strlen(pSJL06->staticAttr.ipAddr) == 0)
			continue;
		if (pSJL06->dynamicAttr.status == conColdBackupSJL06)
			continue;

#ifdef _traceInfo_
		//add by xusj 2008-12-12
		memset(traceStr, 0, sizeof traceStr);
		UnionGetSjl06GrpTraceStr(pSJL06, traceStr);
		ret = UnionTraceInfo(UnionGetApplicationName(), traceStr);
		if (ret < 0)
		{
			UnionUserErrLog("in UnionCheckStatusOfWorkingSJL06WithCmdGrp:: UnionTraceInfo appName=[%s], str=[%s]\n",UnionGetApplicationName(), traceStr);
		}
		//add by xusj end
#endif

		// 2007/12/22，增加
		if ((strcmp(pSJL06->staticAttr.testCmdReq,"null") == 0) || (strcmp(pSJL06->staticAttr.testCmdReq,"NULL") == 0))
		{
			if ((ret = UnionCreateSocketClient(pSJL06->staticAttr.ipAddr,pSJL06->staticAttr.port)) < 0)
				UnionSetWorkingSJL06Abnormal(pSJL06);
			else
			{
				UnionCloseSocket(ret);
				if (pSJL06->dynamicAttr.status == conAbnormalSJL06)
				{
					UnionAuditLog("in UnionCheckStatusOfWorkingSJL06:: set [%s] online\n",pSJL06->staticAttr.ipAddr);
					UnionSetWorkingSJL06Online(pSJL06);
				}
			}
			continue;
		}
		// 2007/12/22，增加完成

		// 是二进制密码机
		if (strcmp(pSJL06->staticAttr.hsmCmdVersion,"SJL05") != 0)
		{
			strcpy(buf,pSJL06->staticAttr.testCmdReq);
			len = strlen(pSJL06->staticAttr.testCmdReq);
		}
		else	// 不是二进制密码机
		{
			aschex_to_bcdhex(pSJL06->staticAttr.testCmdReq,strlen(pSJL06->staticAttr.testCmdReq),buf);
			len = strlen(pSJL06->staticAttr.testCmdReq) / 2;
		}
		if ((ret = UnionShortConnSJL06CmdAnyway(pSJL06,buf,
			len,buf,sizeof(buf))) < 0)
		{
			UnionSetWorkingSJL06Abnormal(pSJL06);
/*
#ifdef _KillHsmTaskAutomatically_
			UnionUserErrLog("in UnionCheckStatusOfWorkingSJL06:: kill hsmTask for [%s]\n",pSJL06->staticAttr.ipAddr);
			UnionKillTaskInstanceByAlikeName(pSJL06->staticAttr.ipAddr);
#endif
*/
			continue;
		}
		memset(resBuf,0,sizeof(resBuf));
		if (strcmp(pSJL06->staticAttr.hsmCmdVersion,"SJL05") != 0)
			memcpy(resBuf,buf,strlen(pSJL06->staticAttr.testCmdSuccessRes));
		else
			bcdhex_to_aschex(buf,strlen(pSJL06->staticAttr.testCmdSuccessRes)/2,resBuf);
		if (strncmp(resBuf,pSJL06->staticAttr.testCmdSuccessRes,strlen(pSJL06->staticAttr.testCmdSuccessRes)) != 0)
		{
			UnionUserErrLog("in UnionCheckStatusOfWorkingSJL06:: response of Hsm [%s] = [%s] != defined [%s]\n",
					pSJL06->staticAttr.ipAddr,resBuf,pSJL06->staticAttr.testCmdSuccessRes);
			UnionSetWorkingSJL06Abnormal(pSJL06);
			continue;
		}
		else
		{
			//UnionAuditLog("in UnionCheckStatusOfWorkingSJL06:: [%s] status = [%c]\n",pSJL06->staticAttr.ipAddr,pSJL06->dynamicAttr.status);
			if (pSJL06->dynamicAttr.status == conAbnormalSJL06)
			{
				UnionAuditLog("in UnionCheckStatusOfWorkingSJL06:: set [%s] online\n",pSJL06->staticAttr.ipAddr);
				UnionSetWorkingSJL06Online(pSJL06);
			}
			continue;
		}
	}
	return(0);
}

PUnionSJL06 UnionFindIdleWorkingSJL06(char *hsmGrpID)
{
	int		index;
	PUnionSJL06	pSJL06,pIdleSJL06=NULL;
	int		isFirst = 1;
	int		ret;

	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		UnionUserErrLog("in UnionFindIdleWorkingSJL06:: UnionConnectWorkingSJL06MDL!\n");
		return(NULL);
	}

	for (index = 0; index < conMaxNumOfSJL06; index++)
	{
		pSJL06 = pgunionSJL06Grp + index;
		if ((strncmp(pSJL06->staticAttr.hsmGrpID,hsmGrpID,3) == 0) && (pSJL06->dynamicAttr.status == conOnlineSJL06))
		{
			if (!isFirst)
			{
				if (pIdleSJL06->dynamicAttr.normalCmdTimes > pSJL06->dynamicAttr.normalCmdTimes)
				pIdleSJL06 = pSJL06;
			}
			else
			{
				pIdleSJL06 = pSJL06;
				isFirst = 0;
			}
		}
	}
	return(pIdleSJL06);
}

PUnionSJL06 UnionFindFirstIdleWorkingSJL06()
{
	int		index;
	PUnionSJL06	pSJL06,pIdleSJL06=NULL;
	int		isFirst = 1;
	int		ret;

	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		UnionUserErrLog("in UnionFindIdleWorkingSJL06:: UnionConnectWorkingSJL06MDL!\n");
		return(NULL);
	}

	for (index = 0; index < conMaxNumOfSJL06; index++)
	{
		pSJL06 = pgunionSJL06Grp + index;
		if (pSJL06->dynamicAttr.status == conOnlineSJL06)
		{
			if (!isFirst)
			{
				if (pIdleSJL06->dynamicAttr.normalCmdTimes > pSJL06->dynamicAttr.normalCmdTimes)
				pIdleSJL06 = pSJL06;
			}
			else
			{
				pIdleSJL06 = pSJL06;
				isFirst = 0;
			}
		}
	}
	return(pIdleSJL06);
}

int UnionResetWorkingSJL06CmdTimes(char *ipAddr)
{
	PUnionSJL06	psjl06;

	if ((psjl06 = UnionFindWorkingSJL06(ipAddr)) == NULL)
	{
		UnionUserErrLog("in UnionResetWorkingSJL06CmdTimes:: UnionFindWorkingSJL06 [%s]\n",ipAddr);
		return(errCodeSJL06MDL_SJL06NotExists);
	}
	psjl06->dynamicAttr.normalCmdTimes = 0;
	psjl06->dynamicAttr.timeoutTimes = 0;
	psjl06->dynamicAttr.connFailTimes = 0;
	psjl06->dynamicAttr.abnormalCmdTimes = 0;
	psjl06->dynamicAttr.continueFailTimes = 0;
	psjl06->dynamicAttr.downTimes = 0;

	return(0);
}

int UnionResetAllWorkingSJL06CmdTimes()
{
	int		index;
	int		ret;

	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		UnionUserErrLog("in UnionResetAllWorkingSJL06CmdTimes:: UnionConnectWorkingSJL06MDL!\n");
		return(ret);
	}

	for (index = 0; index < conMaxNumOfSJL06; index++)
	{
		if (strlen((pgunionSJL06Grp + index)->staticAttr.ipAddr) != 0)
		{
			(pgunionSJL06Grp + index)->dynamicAttr.normalCmdTimes = 0;
			(pgunionSJL06Grp + index)->dynamicAttr.timeoutTimes = 0;
			(pgunionSJL06Grp + index)->dynamicAttr.connFailTimes = 0;
			(pgunionSJL06Grp + index)->dynamicAttr.abnormalCmdTimes = 0;
			(pgunionSJL06Grp + index)->dynamicAttr.continueFailTimes = 0;
			(pgunionSJL06Grp + index)->dynamicAttr.downTimes = 0;
		}
	}
	return(0);
}
// is rewritten by Wolfgang Wang, 2004/06/22
int UnionKillHsmTaskOfAllAbnormalHsm()
{
	int		index;
	PUnionSJL06	pSJL06;
	int		ret;
	char		buf[512];

	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		UnionUserErrLog("in UnionKillHsmTaskOfAllAbnormalHsm:: UnionConnectWorkingSJL06MDL!\n");
		return(ret);
	}

	for (index = 0; index < conMaxNumOfSJL06; index++)
	{
		pSJL06 = pgunionSJL06Grp + index;
		if (strlen(pSJL06->staticAttr.ipAddr) == 0)
			continue;
		if (pSJL06->dynamicAttr.status == conOnlineSJL06)
			continue;
		ret = UnionKillTaskInstanceByAlikeName(pSJL06->staticAttr.ipAddr);
		UnionSuccessLog("in UnionKillHsmTaskOfAllAbnormalHsm:: kill [%d] hsmTask for [%s]\n",ret,pSJL06->staticAttr.ipAddr);
	}
	return(0);
}

int UnionReloadWorkingSJL06(char *ipAddr)
{
	PUnionSJL06	pSJL06;
	int		ret;
	TUnionSJL06	sjl06;

	if (!UnionIsValidIPAddrStr(ipAddr))
		return(errCodeParameter);
	if ((pSJL06 = UnionFindWorkingSJL06(ipAddr)) == NULL)
	{
		return(UnionAddWorkingSJL06(ipAddr));
	}
#ifdef _KillHsmTaskAutomatically_
	UnionCloseTaskInstanceByAlikeName(ipAddr);
#endif
	memset(&sjl06,0,sizeof(sjl06));
	strcpy(sjl06.staticAttr.ipAddr,ipAddr);
	if ((ret = UnionSelectSJL06Rec(&sjl06)) < 0)
	{
		UnionUserErrLog("in UnionReloadWorkingSJL06:: UnionSelectSJL06Rec [%s]\n",ipAddr);
		return(ret);
	}
	memcpy(&(pSJL06->staticAttr),&(sjl06.staticAttr),sizeof(sjl06.staticAttr));
	return(0);
}

int UnionExistSJL06InHsmGrpConfFile(char *ipAddr)
{
	FILE	*fp = NULL;
	int	ret;
	int	num=0;
	char	tmpBuf[1024+1];
	int	index;
	char	*connNumPtr;

	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionGetNameOfAutoRegisteredHsmConf(tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionExistSJL06InHsmGrpConfFile:: UnionGetNameOfAutoRegisteredHsmConf!\n");
		return(ret);
	}
	if ((fp = fopen(tmpBuf,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionExistSJL06InHsmGrpConfFile:: fopen [%s]\n",tmpBuf);
		return(ret);
	}
	while (!feof(fp))
	{
		// 从文件中读取一行
		memset(tmpBuf,0,sizeof(tmpBuf));
		if (UnionReadOneFileLine(fp,tmpBuf) == 0)
			continue;
		if (UnionIsRemarkFileLine(tmpBuf))
			continue;
		if ((connNumPtr = strstr(tmpBuf,"=")) != NULL)
			*connNumPtr = 0;
		if (!UnionIsValidIPAddrStr(tmpBuf))	// 判断是不是合法的IP地址
			continue;
		if (strcmp(tmpBuf,ipAddr) != 0)		// 不是此密码机
			continue;
		fclose(fp);
		return(1);
	}
	fclose(fp);
	return(0);
}

int UnionAddSJL06IntoHsmGrpConfFile(char *ipAddr,int connNum)
{
	char	tmpBuf[1024+1];
	FILE	*fp = NULL;
	int	ret;

	if (UnionExistSJL06InHsmGrpConfFile(ipAddr) > 0)
	{
		UnionUserErrLog("in UnionAddSJL06IntoHsmGrpConfFile:: ipAddr [%s] already exists in hsmGrp file!\n",ipAddr);
		return(errCodeSJL06MDL_SJL06AlreadyInGrpFile);
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionGetNameOfAutoRegisteredHsmConf(tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionAddSJL06IntoHsmGrpConfFile:: UnionGetNameOfAutoRegisteredHsmConf!\n");
		return(ret);
	}
	if ((fp = fopen(tmpBuf,"a")) == NULL)
	{
		UnionSystemErrLog("in UnionAddSJL06IntoHsmGrpConfFile:: fopen [%s]\n",tmpBuf);
		return(ret);
	}
	if (connNum <= 0)
		connNum = 1;
	fprintf(fp,"%s=%d\n",ipAddr,connNum);
	fclose(fp);
	return(0);
}

int UnionDeleteSJL06FromHsmGrpConfFile(char *ipAddr)
{
	FILE	*fp1 = NULL;
	FILE	*fp2 = NULL;
	int	ret;
	char	tmpBuf[1024+1];
	char	fileName[512+1];
	char	*ptr;

	if ((ret = UnionExistSJL06InHsmGrpConfFile(ipAddr)) <= 0)
	{
		UnionUserErrLog("in UnionDeleteSJL06FromHsmGrpConfFile:: UnionExistSJL06InHsmGrpConfFile [%s]!\n",ipAddr);
		return(errCodeSJL06MDL_SJL06NotExistsInGrpFile);
	}
	memset(fileName,0,sizeof(fileName));
	if ((ret = UnionGetNameOfAutoRegisteredHsmConf(fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteSJL06FromHsmGrpConfFile:: UnionGetNameOfAutoRegisteredHsmConf!\n");
		return(ret);
	}
	if ((fp1 = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteSJL06FromHsmGrpConfFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	sprintf(tmpBuf,"%s/%d.tmp",getenv("UNIONTEMP"),getpid());
	if ((fp2 = fopen(tmpBuf,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteSJL06FromHsmGrpConfFile:: fopen [%s]\n",tmpBuf);
		fclose(fp1);
		return(errCodeUseOSErrCode);
	}
	for (;!feof(fp1);)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadOneFileLine(fp1,tmpBuf)) < 0)
			continue;
		if (ret == 0)
		{
			if (!feof(fp1))
				goto writeLine;
			else
				break;
		}
		if (UnionIsRemarkFileLine(tmpBuf))	// 注释行
			goto writeLine;
		if ((ptr = strstr(tmpBuf,"=")) != NULL)
			*ptr = 0;
		if (strcmp(tmpBuf,ipAddr) == 0)		// 是密码机定义
			continue;
		if (ptr != NULL)
			*ptr = '=';
writeLine:	// 写入
		fprintf(fp2,"%s\n",tmpBuf);
		continue;
	}
	fclose(fp1);
	fclose(fp2);
	sprintf(tmpBuf,"mv %s/%d.tmp %s",getenv("UNIONTEMP"),getpid(),fileName);
	system(tmpBuf);
	return(0);
}

int UnionPrintAllWorkingSJL06ToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;

	if ((fileName == NULL) || (strlen(fileName) == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintAllWorkingSJL06ToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	ret = UnionPrintAllWorkingSJL06ToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

int UnionSetSpecWorkingSJL06Abnormal(char *ipAddr)
{
	PUnionSJL06	psjl06;

	if ((ipAddr == NULL) || (!UnionIsValidIPAddrStr(ipAddr)))
		return(errCodeInvalidIPAddr);
	if ((psjl06 = UnionFindWorkingSJL06(ipAddr)) == NULL)
	{
		UnionUserErrLog("in UnionSetSpecWorkingSJL06Abnormal:: UnionFindWorkingSJL06 [%s]\n",ipAddr);
		return(errCodeSJL06MDL_SJL06NotExists);
	}
	return(UnionSetWorkingSJL06Abnormal(psjl06));
}

int UnionSetSpecWorkingSJL06Online(char *ipAddr)
{
	PUnionSJL06	psjl06;

	if ((ipAddr == NULL) || (!UnionIsValidIPAddrStr(ipAddr)))
		return(errCodeInvalidIPAddr);
	if ((psjl06 = UnionFindWorkingSJL06(ipAddr)) == NULL)
	{
		UnionUserErrLog("in UnionSetSpecWorkingSJL06Online:: UnionFindWorkingSJL06 [%s]\n",ipAddr);
		return(errCodeSJL06MDL_SJL06NotExists);
	}
	return(UnionSetWorkingSJL06Online(psjl06));
}

int UnionSetSpecWorkingSJL06ColdBackup(char *ipAddr)
{
	PUnionSJL06	psjl06;

	if ((ipAddr == NULL) || (!UnionIsValidIPAddrStr(ipAddr)))
		return(errCodeInvalidIPAddr);
	if ((psjl06 = UnionFindWorkingSJL06(ipAddr)) == NULL)
	{
		UnionUserErrLog("in UnionSetSpecWorkingSJL06ColdBackup:: UnionFindWorkingSJL06 [%s]\n",ipAddr);
		return(errCodeSJL06MDL_SJL06NotExists);
	}
	return(UnionSetWorkingSJL06ColdBackup(psjl06));
}

int UnionDeleteWorkingSJL06Anyway(char *ipAddr)
{
	PUnionSJL06	pSJL06;
	int		ret;

	if ((pSJL06 = UnionFindWorkingSJL06(ipAddr)) == NULL)
	{
		UnionUserErrLog("in UnionDeleteWorkingSJL06Anyway:: [%s] non-exists!\n",ipAddr);
		return(errCodeSJL06MDL_SJL06NotExists);
	}

	if (pSJL06->dynamicAttr.status == conOnlineSJL06)
	{
#ifdef _KillHsmTaskAutomatically_
		UnionCloseTaskInstanceByAlikeName(ipAddr);
#endif
	}

	if ((ret = UnionUpdateSJL06DynamicAttr(pSJL06)) < 0)
	{
		UnionUserErrLog("in UnionDeleteWorkingSJL06Anyway:: UnionUpdateSJL06DynamicAttr [%s]\n",ipAddr);
		//return(ret);
	}
	memset(pSJL06,0,sizeof(*pSJL06));
	return(0);
}
// 2008/6/3增加
// 根据一个指定的命令配置文件检查密码机的状态
int UnionCheckStatusOfWorkingSJL06WithCmdGrp()
{
	int		index;
	PUnionSJL06	pSJL06;
	int		ret;
	char		cmdReq[conMaxNumOfHsmCheckCmd][256+1],reqStr[256+1];
	char		cmdRes[conMaxNumOfHsmCheckCmd][256+1],resStr[256+1],ascResBuf[256+1];
	int		realCmdNum = 0;
	int		cmdIndex;
	int		len;
	char		currentHsmGrp[3+1];
	char		traceStr[1024];
	int		successCmdNum = 0;

	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		UnionUserErrLog("in UnionCheckStatusOfWorkingSJL06WithCmdGrp:: UnionConnectWorkingSJL06MDL!\n");
		return(ret);
	}

	memset(currentHsmGrp,0,sizeof(currentHsmGrp));
	for (index = 0; index < conMaxNumOfSJL06; index++)
	{
		pSJL06 = pgunionSJL06Grp + index;

		if (strlen(pSJL06->staticAttr.ipAddr) == 0)
			continue;
		if (pSJL06->dynamicAttr.status == conColdBackupSJL06)
			continue;
#ifdef _traceInfo_
		//add by xusj 2008-12-12
		memset(traceStr, 0, sizeof traceStr);
		UnionGetSjl06GrpTraceStr(pSJL06, traceStr);
		ret = UnionTraceInfo(UnionGetApplicationName(), traceStr);
		if (ret < 0)
		{
			UnionUserErrLog("in UnionCheckStatusOfWorkingSJL06WithCmdGrp:: UnionTraceInfo appName=[%s], str=[%s]\n",UnionGetApplicationName(), traceStr);
		}
		//add by xusj end
#endif
		// 读取该组要检测的密码机指令
		if ((strcmp(currentHsmGrp,pSJL06->staticAttr.hsmGrpID) == 0) && (realCmdNum > 0))	// 与上一台密码机同组，并且组指令已初始化
			goto checkNow;
		for (cmdIndex = 0; cmdIndex < conMaxNumOfHsmCheckCmd; cmdIndex++)
		{
			memset(cmdReq[cmdIndex],0,sizeof(cmdReq[cmdIndex]));
			memset(cmdRes[cmdIndex],0,sizeof(cmdRes[cmdIndex]));
		}
		if ((realCmdNum = UnionReadTestCmdGrpOfHsmGrp(pSJL06->staticAttr.hsmGrpID,cmdReq,cmdRes,conMaxNumOfHsmCheckCmd)) <= 0)
		{
			strcpy(cmdReq[0],pSJL06->staticAttr.testCmdReq);
			strcpy(cmdRes[0],pSJL06->staticAttr.testCmdSuccessRes);
			realCmdNum = 1;
		}
checkNow:
		// 逐一测试每条指令
		successCmdNum = 0;
		for (cmdIndex = 0; cmdIndex < realCmdNum; cmdIndex++)
		{

			if (strcmp(pSJL06->staticAttr.hsmCmdVersion,"SJL05") != 0)	// 不是二进制密码机
			{
				strcpy(reqStr,cmdReq[cmdIndex]);
				len = strlen(reqStr);
			}
			else	// 是二进制密码机
			{
				aschex_to_bcdhex(cmdReq[cmdIndex],strlen(cmdReq[cmdIndex]),reqStr);
				len = strlen(cmdReq[cmdIndex]) / 2;
			}
			if ((strcmp(reqStr,"null") == 0) || (strcmp(reqStr,"NULL") == 0))	// 不执行这条指令
			{
				// 与密码机建立连接
				if ((ret = UnionCreateSocketClient(pSJL06->staticAttr.ipAddr,pSJL06->staticAttr.port)) < 0)
				{
					break;
				}
				UnionCloseSocket(ret);
				successCmdNum++;
				continue;
			}
			// 执行这条指令
			memset(resStr,0,sizeof(resStr));
			if ((ret = UnionShortConnSJL06CmdAnyway(pSJL06,reqStr,len,resStr,sizeof(resStr))) < 0)	// 执行指令
			{
				break;
			}
			memset(ascResBuf,0,sizeof(ascResBuf));
			// 比较响应
			if (strcmp(pSJL06->staticAttr.hsmCmdVersion,"SJL05") != 0)
				memcpy(ascResBuf,resStr,ret);
			else
				bcdhex_to_aschex(resStr,ret,ascResBuf);
			if (strncmp(cmdRes[cmdIndex],ascResBuf,strlen(cmdRes[cmdIndex])) != 0)	// 响应不符合预期
			{
				//如果密码机未授权,add by LiangJie 2009-09-16
				if (strncmp(ascResBuf+2, "17", 2)==0)
				{
					UnionLog("in UnionCheckStatusOfWorkingSJL06WithCmdGrp:: Hsm [%s] in hsmGrpID[%s] is not authorize!\n", pSJL06->staticAttr.ipAddr, pSJL06->staticAttr.hsmGrpID);
					//向密码机发送授权指令
					memset(resStr,0,sizeof(resStr));
					if ((ret = UnionShortConnSJL06CmdAnyway(pSJL06,"0A",2,resStr,sizeof(resStr))) < 0)
					{
						break;
					}
				}
				else
				{
					UnionUserErrLog("in UnionCheckStatusOfWorkingSJL06WithCmdGrp:: response of Hsm [%s] of cmd [%s] = [%s] != defined [%s]\n",
						pSJL06->staticAttr.ipAddr,cmdReq[cmdIndex],ascResBuf,cmdRes[cmdIndex]);
					break;
				}
			}
			UnionLog("in UnionCheckStatusOfWorkingSJL06WithCmdGrp:: Hsm [%s] in hsmGrpID[%s] check [%s] OK!\n",pSJL06->staticAttr.ipAddr, pSJL06->staticAttr.hsmGrpID,cmdReq[cmdIndex]);
			successCmdNum++;
			continue;
		}
		if (successCmdNum < realCmdNum)	// 有指令的执行结果不符合预期
		{
			UnionSetWorkingSJL06Abnormal(pSJL06);
		}
		else	// 所有指令执行正确
		{
			if (pSJL06->dynamicAttr.status == conAbnormalSJL06)
			{
				UnionAuditLog("in UnionCheckStatusOfWorkingSJL06WithCmdGrp:: set [%s] online\n",pSJL06->staticAttr.ipAddr);
				UnionSetWorkingSJL06Online(pSJL06);
			}
		}
	}
	return(0);
}

// 2008/6/3，增加，从一个指令配置文件中读取定义的字令串
int UnionReadTestCmdGrpOfHsmGrp(char *hsmGrpID,char cmdReq[][256+1],char cmdRes[][256+1],int maxExpectedNum)
{
	char	tmpBuf[512+1];
	int	realNum=0;
	FILE	*fp;
	int	ret;
	int	reqExists;
	char	*ptr;

	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%s/HsmCfg/checkCmdGrpOf%s.conf",getenv("UNIONETC"),hsmGrpID);
	if ((fp = fopen(tmpBuf,"r")) == NULL)
	{
		UnionProgramerLog("in UnionReadTestCmdGrpOfHsmGrp:: fopen [%s] failure!\n",tmpBuf);
		return(0);
	}

	reqExists = 0;
	while (!feof(fp))
	{
		if (realNum >= maxExpectedNum)	// 已读了最大数量的测试指令
			break;
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadOneLineFromTxtStr(fp,tmpBuf,sizeof(tmpBuf))) <= 0)
			continue;
		if (UnionIsRemarkFileLine(tmpBuf))
			continue;
		if (!reqExists) // 还没有定义请求
		{
			if ((ptr=strstr(tmpBuf,"hsmCmdReq=")) == NULL)
				continue;
			if (strlen(ptr+strlen("hsmCmdReq=")) >= sizeof(cmdReq[realNum]))
			{
				UnionUserErrLog("in UnionReadTestCmdGrpOfHsmGrp:: [%s] is too long!\n",tmpBuf);
				continue;
			}
			strcpy(cmdReq[realNum],ptr+strlen("hsmCmdReq="));
			reqExists = 1;
		}
		else	// 读请求的对应响应
		{
			reqExists = 0;
			if ((ptr=strstr(tmpBuf,"hsmCmdRes=")) == NULL)
				continue;
			if (strlen(ptr+strlen("hsmCmdRes=")) >= sizeof(cmdRes[realNum]))
			{
				UnionUserErrLog("in UnionReadTestCmdGrpOfHsmGrp:: [%s] is too long!\n",tmpBuf);
				continue;
			}
			strcpy(cmdRes[realNum],ptr+strlen("hsmCmdRes="));
			realNum++;
		}
	}
	fclose(fp);
	return(realNum);
}


// 2008/6/6增加
// 根据一个指定的命令配置文件检查指定密码机是否正常
// 如果checkAnyway是0，则只检查异常的密码机，否则检查异常和正常的加密机
int UnionIsSpecHsmNormal(char *ipAddr,int checkAnyway)
{
	PUnionSJL06	pSJL06;
	int		ret;
	char		cmdReq[conMaxNumOfHsmCheckCmd][256+1],reqStr[256+1];
	char		cmdRes[conMaxNumOfHsmCheckCmd][256+1],resStr[256+1],ascResBuf[256+1];
	int		realCmdNum = 0;
	int		cmdIndex;
	int		len;
	int		successCmdNum = 0;

	if ((ipAddr == NULL) || (strlen(ipAddr) == 0))
		return(0);

	if ((pSJL06 = UnionFindWorkingSJL06(ipAddr)) == NULL)
	{
		UnionUserErrLog("in UnionIsSpecHsmNormal:: UnionFindWorkingSJL06 [%s]!\n",ipAddr);
		return(0);
	}

	if (pSJL06->dynamicAttr.status == conColdBackupSJL06)
		return(0);

	if (pSJL06->dynamicAttr.status == conColdBackupSJL06)
		return(0);
	if ((pSJL06->dynamicAttr.status == conOnlineSJL06) && (!checkAnyway))
	{
		return(1);
	}

	// 读取该组要检测的密码机指令
	for (cmdIndex = 0; cmdIndex < conMaxNumOfHsmCheckCmd; cmdIndex++)
	{
		memset(cmdReq[cmdIndex],0,sizeof(cmdReq[cmdIndex]));
		memset(cmdRes[cmdIndex],0,sizeof(cmdRes[cmdIndex]));
	}
	if ((realCmdNum = UnionReadTestCmdGrpOfHsmGrp(pSJL06->staticAttr.hsmGrpID,cmdReq,cmdRes,conMaxNumOfHsmCheckCmd)) <= 0)
	{
		strcpy(cmdReq[0],pSJL06->staticAttr.testCmdReq);
		strcpy(cmdRes[0],pSJL06->staticAttr.testCmdSuccessRes);
		realCmdNum = 1;
	}
	successCmdNum = 0;
	for (cmdIndex = 0; cmdIndex < realCmdNum; cmdIndex++)
	{
		if (strcmp(pSJL06->staticAttr.hsmCmdVersion,"SJL05") != 0)	// 不是二进制密码机
		{
			strcpy(reqStr,cmdReq[cmdIndex]);
			len = strlen(reqStr);
		}
		else	// 是二进制密码机
		{
			aschex_to_bcdhex(cmdReq[cmdIndex],strlen(cmdReq[cmdIndex]),reqStr);
			len = strlen(cmdReq[cmdIndex]) / 2;
		}
		if ((strcmp(reqStr,"null") == 0) || (strcmp(reqStr,"NULL") == 0))	// 不执行这条指令
		{
			// 与密码机建立连接
			if ((ret = UnionCreateSocketClient(pSJL06->staticAttr.ipAddr,pSJL06->staticAttr.port)) < 0)
			{
				break;
			}
			UnionCloseSocket(ret);
			successCmdNum++;
			continue;
		}
		// 执行这条指令
		memset(resStr,0,sizeof(resStr));
		if ((ret = UnionShortConnSJL06CmdAnyway(pSJL06,reqStr,len,resStr,sizeof(resStr))) < 0)	// 执行指令
		{
			break;
		}
		memset(ascResBuf,0,sizeof(ascResBuf));
		// 比较响应
		if (strcmp(pSJL06->staticAttr.hsmCmdVersion,"SJL05") != 0)
			memcpy(ascResBuf,resStr,ret);
		else
			bcdhex_to_aschex(resStr,ret,ascResBuf);
		if (strncmp(cmdRes[cmdIndex],ascResBuf,strlen(cmdRes[cmdIndex])) != 0)	// 响应不符合预期
		{
			UnionUserErrLog("in UnionIsSpecHsmNormal:: response of Hsm [%s] of cmd [%s] = [%s] != defined [%s]\n",
					pSJL06->staticAttr.ipAddr,cmdReq[cmdIndex],ascResBuf,cmdRes[cmdIndex]);
			break;
		}
		UnionNullLog("in UnionIsSpecHsmNormal:: check [%s] OK!\n",cmdReq[cmdIndex]);
		successCmdNum++;
		continue;
	}
	if (successCmdNum < realCmdNum)	// 有指令的执行结果不符合预期
	{
		UnionSetWorkingSJL06Abnormal(pSJL06);
		return(0);
	}
	else	// 所有指令执行正确
	{
		if (pSJL06->dynamicAttr.status == conAbnormalSJL06)
		{
			UnionAuditLog("in UnionIsSpecHsmNormal:: set [%s] online\n",pSJL06->staticAttr.ipAddr);
			UnionSetWorkingSJL06Online(pSJL06);
		}
		return(1);
	}
}

//add by xsj 2008-12-12
int UnionGetSjl06GrpTraceStr(PUnionSJL06 pSJL06, char *str)
{
	sprintf(str,"hsmGrpID=%5s|ipAddr=%15s|port=%6d|",
		pSJL06->staticAttr.hsmGrpID,
		pSJL06->staticAttr.ipAddr,
		pSJL06->staticAttr.port);
	switch (pSJL06->dynamicAttr.status)
	{
		case    '0':
			sprintf(str+strlen(str),"status=异常|");
			break;
		case    '1':
			sprintf(str+strlen(str),"status=在线|");
			break;
		case    '2':
			sprintf(str+strlen(str),"status=备份|");
			break;
		default:
			sprintf(str+strlen(str),"status=不明|");
			break;
	}
	sprintf(str+strlen(str),"activeLongConn=%4d|timeoutTimes=%12ld|normalCmdTimes=%12ld|",
		pSJL06->dynamicAttr.activeLongConn,
		pSJL06->dynamicAttr.timeoutTimes + pSJL06->dynamicAttr.connFailTimes,
		pSJL06->dynamicAttr.normalCmdTimes + pSJL06->dynamicAttr.abnormalCmdTimes);

	return 0;
}

// Add By HuangBaoxin, 2010-11-30
int UnionClearWorkingSJL06(int index)
{
	PUnionSJL06	pSJL06;
	int		ret;

	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		UnionUserErrLog("in UnionClearWorkingSJL06:: UnionConnectWorkingSJL06MDL!\n");
		return(ret);
	}
	
	if (index >= conMaxNumOfSJL06)
	{
		UnionUserErrLog("in UnionClearWorkingSJL06::index[%d] >= conMaxNumOfSJL06[%d]\n", index, conMaxNumOfSJL06);
		return(errCodeParameter);
	}
	
	pSJL06 = pgunionSJL06Grp + index;
	if (pSJL06 != NULL)
		memset(pSJL06, 0, sizeof(TUnionSJL06));
	
	return(0);
}

int UnionChangeWorkingSJL06(int index, char *ipAddr)
{
	PUnionSJL06	pSJL06;
	int		ret;
	
	if (strlen(ipAddr) > 15)
	{
		UnionUserErrLog("in UnionChangeWorkingSJL06::length of ipAddr [%d] is too large\n", strlen(ipAddr));
		return(errCodeParameter);
	}
	
	if (index >= conMaxNumOfSJL06)
	{
		UnionUserErrLog("in UnionChangeWorkingSJL06::index[%d] >= conMaxNumOfSJL06[%d]\n", index, conMaxNumOfSJL06);
		return(errCodeParameter);
	}
			
	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		UnionUserErrLog("in UnionChangeWorkingSJL06:: UnionConnectWorkingSJL06MDL!\n");
		return(ret);
	}
	
	pSJL06 = pgunionSJL06Grp + index;
	if (pSJL06 != NULL) {
		memcpy(pSJL06->staticAttr.ipAddr, ipAddr, strlen(ipAddr));
	}
}

// Add End By HuangBaoxin
