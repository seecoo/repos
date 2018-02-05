//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2003/09/12
//	Version:	1.0

// 2004/06/22，Wolfgang Wang升级为2.0
// 升级是因为修改了TUnionSJL06StaticAttr的结构

#define _UnionLogMDL_3_x_	
#define _UnionEnv_3_x_	

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "UnionStr.h"

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

PUnionSharedMemoryModule	pgunionSJL06GrpMDL = NULL;
PUnionSJL06			pgunionSJL06Grp = NULL;

int UnionGetNameOfAutoRegisteredHsmConf(char *fileName)
{
	sprintf(fileName,"%s/HsmCfg/autoRegisterHsm.Def",getenv("UNIONETC"));
	return(0);
}

int UnionIsWorkingSJL06MDLConnected()
{
	if ((pgunionSJL06Grp == NULL) || (!UnionIsSharedMemoryInited(conMDLNameOfUnionSJL06TBL)))
		return(0);
	else
		return(1);
}

int UnionAutoLoadSJL06IntoSJL06MDL()
{
	char	fileName[512];
	FILE	*fp;
	char	ipAddr[100];
	int	ret;
	int	num=0;
	
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
		memset(ipAddr,0,sizeof(ipAddr));
		fscanf(fp,"%s",ipAddr);
		if (!UnionIsValidIPAddrStr(ipAddr))
			continue;
		if ((ret = UnionAddWorkingSJL06(ipAddr)) < 0)
		{
			UnionUserErrLog("in UnionAutoLoadSJL06IntoSJL06MDL:: UnionAddWorkingSJL06 [%s]\n",ipAddr);
			continue;
		}
		UnionSetWorkingSJL06Online(UnionFindWorkingSJL06(ipAddr));
		num++;
	}
	fclose(fp);
	UnionSuccessLog("in UnionAutoLoadSJL06IntoSJL06MDL:: [%s] hsm has been reloaded!\n",num);
	return(num);
}

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
	
	pSJL06->dynamicAttr.status = conAbnormalSJL06;
	UnionGetFullSystemDateTime(pSJL06->dynamicAttr.lastDownDate);
	++pSJL06->dynamicAttr.downTimes;
	UnionKillTaskInstanceByName(pSJL06->staticAttr.ipAddr);
	
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
	
	pSJL06->dynamicAttr.status = conOnlineSJL06;
	pSJL06->dynamicAttr.continueFailTimes = 0;
	
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
	
	pSJL06->dynamicAttr.status = conColdBackupSJL06;
	UnionKillTaskInstanceByName(pSJL06->staticAttr.ipAddr);
	
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
		fprintf(fp,"%5s %15s %6d ",
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
			pSJL06->dynamicAttr.timeoutTimes + pSJL06->dynamicAttr.abnormalCmdTimes + pSJL06->dynamicAttr.connFailTimes,
			pSJL06->dynamicAttr.normalCmdTimes);
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
	char		buf[512];
	
	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		UnionUserErrLog("in UnionCheckStatusOfWorkingSJL06:: UnionConnectWorkingSJL06MDL!\n");
		return(ret);
	}
	
	for (index = 0; index < conMaxNumOfSJL06; index++)
	{
		pSJL06 = pgunionSJL06Grp + index;
		if (strlen(pSJL06->staticAttr.ipAddr) == 0)
			continue;
		if (pSJL06->dynamicAttr.status == conColdBackupSJL06)
			continue;
		if (strcmp(pSJL06->staticAttr.hsmCmdVersion,"SJL05") == 0)
			continue;
		if ((ret = UnionShortConnSJL06CmdAnyway(pSJL06,pSJL06->staticAttr.testCmdReq,
			strlen(pSJL06->staticAttr.testCmdReq),buf,sizeof(buf))) < 0)
		{
			UnionSetWorkingSJL06Abnormal(pSJL06);
#ifdef _KillHsmTaskAutomatically_
			UnionUserErrLog("in UnionCheckStatusOfWorkingSJL06:: kill hsmTask for [%s]\n",pSJL06->staticAttr.ipAddr);
			UnionKillTaskInstanceByAlikeName(pSJL06->staticAttr.ipAddr);
#endif
			continue;
		}
		if (strncmp(pSJL06->staticAttr.testCmdSuccessRes,buf,strlen(pSJL06->staticAttr.testCmdSuccessRes)) != 0)
		{
			UnionUserErrLog("in UnionCheckStatusOfWorkingSJL06:: response of Hsm [%s] = [%s] != defined [%s]\n",
					pSJL06->staticAttr.ipAddr,buf,pSJL06->staticAttr.testCmdSuccessRes);
			UnionSetWorkingSJL06Abnormal(pSJL06);
			continue;
		}
		else
		{
			if (pSJL06->dynamicAttr.status == conAbnormalSJL06)
			{
				UnionSuccessLog("in UnionCheckStatusOfWorkingSJL06:: set [%s] online\n",pSJL06->staticAttr.ipAddr);
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

int UnionResetWorkingSJL06CmdTimes(char *ipAddr)
{
	PUnionSJL06	psjl06;
	
	if ((psjl06 = UnionFindWorkingSJL06(ipAddr)) == NULL)
	{
		UnionUserErrLog("in UnionResetWorkingSJL06CmdTimes:: UnionFindWorkingSJL06 [%s]\n",ipAddr);
		return(errCodeSJL06MDL_SJL06NotExists);
	}
	psjl06->dynamicAttr.normalCmdTimes = 0;
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
