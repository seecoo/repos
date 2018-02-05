// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2008/11/03
// Version:	1.0

#include <stdio.h>
#include <string.h>

#include "unionVersion.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionOSLockMachinism.h"
#include "unionIndexTBL.h"
#include "unionLockTBL.h"

int			gunionLockInitialized = 0;
PUnionIndexStatusTBL	pgunionOsLock = NULL;
long			gunionLockIndex = 0;

// ��ϵͳ�������
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionOpenOSLockMachinism()
{
	if (gunionLockInitialized)
		return(0);
	//UnionProgramerLog("in UnionOpenOSLockMachinism:: %s begin to open...\n",UnionGetApplicationName());
	if ((pgunionOsLock = UnionConnectIndexStatusTBL(UnionReadUserIDOfSharedMemoryModule(conMDLNameOfLockTBL),1)) == NULL)
	{
		UnionUserErrLog("in UnionOpenOSLockMachinism:: UnionConnectIndexStatusTBL!\n");
		return(errCodeUseOSErrCode);
	}
	//UnionProgramerLog("in UnionOpenOSLockMachinism:: %s open [%0x] [%d] finished!\n",UnionGetApplicationName(),pgunionOsLock,UnionReadUserIDOfSharedMemoryModule(conMDLNameOfLockTBL));
	gunionLockInitialized = 1;
	return(0);
}

// �ر�ϵͳ�������
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionCloseOSLockMachinism()
{
	if (!gunionLockInitialized)
		return(0);
	return(UnionDisconnectIndexStatusTBL(pgunionOsLock));
}

// ����һ��ϵͳ����
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionApplyOsLock(char *resName)
{
	int    ret = 0;
	
	if ((ret = UnionOpenOSLockMachinism()) < 0)
	{
		UnionUserErrLog("in UnionApplyOsLock:: UnionOpenOSLockMachinism!\n");
		return(ret);
	}
	//UnionProgramerLog("in UnionApplyOsLock:: begin apply from [%0x]...\n",pgunionOsLock);
	if ((ret = UnionGetAvailableIndex(pgunionOsLock)) < 0)
	{
		UnionUserErrLog("in UnionApplyOsLock:: UnionGetAvailableIndex!\n");
		return(ret);
	}
	//UnionProgramerLog("in UnionApplyOsLock:: lockID = [%d]\n",ret);
	gunionLockIndex = ret;
	return(0);
}	

// �ͷ�ϵͳ����
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionFreeOsLock(char *resName)
{
	int    ret;
	
	if ((ret = UnionOpenOSLockMachinism()) < 0)
	{
		UnionUserErrLog("in UnionFreeOsLock:: UnionOpenOSLockMachinism!\n");
		return(ret);
	}
	if ((ret = UnionSetIndexAvailable(pgunionOsLock,gunionLockIndex)) < 0)
	{
		UnionUserErrLog("in UnionFreeOsLock:: UnionSetIndexAvailable!\n");
		return(ret);
	}
	return(0);
}
