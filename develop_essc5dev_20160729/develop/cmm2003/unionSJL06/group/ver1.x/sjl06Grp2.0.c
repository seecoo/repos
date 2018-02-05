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

// Module Layer Functions
int UnionConnectWorkingSJL06MDL()
{
	int	index;
	
	if (pgunionSJL06Grp != NULL)	// 已经连接
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
	pgunionSJL06Grp = NULL;
	
	return(UnionRemoveSharedMemoryModule(conMDLNameOfUnionSJL06TBL));
}

PUnionSJL06 UnionFindWorkingSJL06(char *ipAddr)
{
	int		index;
	PUnionSJL06	pSJL06;
	
	if (pgunionSJL06Grp == NULL)
	{
		UnionUserErrLog("in UnionFindWorkingSJL06:: null Pointer!\n");
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

	if (pgunionSJL06Grp == NULL)
		return(0);
	
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
	
	if (pgunionSJL06Grp == NULL)
		return(errCodeSJL06MDL_NotConnected);
	
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

int UnionPrintAllWorkingSJL06()
{
	int		index;
	PUnionSJL06	pSJL06;
	int		hsmNum = 0;
	
	if (pgunionSJL06Grp == NULL)
		return(errCodeSJL06MDL_NotConnected);
	
	for (index = 0; index < conMaxNumOfSJL06; index++)
	{
		pSJL06 = pgunionSJL06Grp + index;
		if (strlen(pSJL06->staticAttr.ipAddr) == 0)
			continue;
		UnionPrintStatusOfWorkingSJL06(pSJL06);
		hsmNum++;
		if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
			break;
		printf("\n\n");
	}
	printf("hsmNum = [%d]\n",hsmNum);
	return(0);
}

int UnionRefreshDynamicAttrsOfAllWorkingSJL06()
{
	int		index;
	PUnionSJL06	pSJL06;
	int		ret;
	
	if (pgunionSJL06Grp == NULL)
		return(errCodeSJL06MDL_NotConnected);
	
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
	
	if (pgunionSJL06Grp == NULL)
		return(errCodeSJL06MDL_NotConnected);
	
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
	
	if ((pgunionSJL06Grp == NULL) || (hsmGrpID == NULL))
		return(errCodeSJL06MDL_NotConnected);
	
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
	
	if (pgunionSJL06Grp == NULL)
		return(errCodeSJL06MDL_NotConnected);
	
	for (index = 0; index < conMaxNumOfSJL06; index++)
	{
		pSJL06 = pgunionSJL06Grp + index;
		if (strlen(pSJL06->staticAttr.ipAddr) == 0)
			continue;
		if (pSJL06->dynamicAttr.status == conColdBackupSJL06)
			continue;
		if ((ret = UnionShortConnSJL06CmdAnyway(pSJL06,pSJL06->staticAttr.testCmdReq,
			strlen(pSJL06->staticAttr.testCmdReq),buf,sizeof(buf))) < 0)
		{
			UnionSetWorkingSJL06Abnormal(pSJL06);
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
	
	if (pgunionSJL06Grp == NULL)
	{
		UnionUserErrLog("in UnionFindIdleWorkingSJL06:: null Pointer!\n");
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
	
	if (pgunionSJL06Grp == NULL)
		return(errCodeSJL06MDL_NotConnected);
	
	for (index = 0; index < conMaxNumOfSJL06; index++)
	{
		if (strlen((pgunionSJL06Grp + index)->staticAttr.ipAddr) != 0)
		{
			(pgunionSJL06Grp + index)->dynamicAttr.normalCmdTimes = 0;
		}
	}
	return(0);
}
