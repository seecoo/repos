//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	1.0

#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_	
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "unionVersion.h"

#include "UnionLog.h"
#include "unionModule.h"
#include "unionErrCode.h"
#include "unionLockTBL.h"
#include "unionOSLockMachinism.h"

#define	CONMAXLOCKEDTIMES	60

PUnionSharedMemoryModule	pgunionLockMDL = NULL;
PUnionLockTBL			pgunionLockTBL = NULL;
int				gunionLockMDLConnected = 0;

int UnionSpierSpecRecOfSpecRes(char fldValue[][1024+1],int fldNum,int resID);

// �ж������Ƿ�������
/*
�������
	��
�������
	��
����ֵ
	1	������
	0	δ����
*/
int UnionIsLockTBLConnected()
{
	return(gunionLockMDLConnected);
}

// ����������Ŀ
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionGetMaxNumOfLock()
{
	return(conMaxNumOfLock);	
}

// ��������
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionConnectLockTBL()
{
	int				ret;

	if (UnionIsLockTBLConnected())	// �Ѿ�����
		return(0);
	
	if ((pgunionLockMDL = UnionConnectSharedMemoryModule(conMDLNameOfLockTBL,
			sizeof(*pgunionLockTBL))) == NULL)
	{
		UnionUserErrLog("in UnionConnectLockTBL:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgunionLockTBL = (PUnionLockTBL)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionLockMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectLockTBL:: PUnionLockTBL!\n");
		return(errCodeSharedMemoryModule);
	}
	
	// Begin Added By HuangBaoxin, 20090312
	pgunionLockTBL->maxNum = conMaxNumOfLock;
	// End Added By HuangBaoxin, 20090312
	
	gunionLockMDLConnected = 1;
	if (UnionIsNewCreatedSharedMemoryModule(pgunionLockMDL))
	{
		return(UnionReloadLockTBL());
	}
	else
		return(0);
}

	
// �Ͽ������������
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionDisconnectLockTBL()
{
	pgunionLockTBL = NULL;
	return(UnionDisconnectShareModule(pgunionLockMDL));
}

// ɾ������
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionRemoveLockTBL()
{
	UnionDisconnectLockTBL();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfLockTBL));
}

// ��װ����
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionReloadLockTBL()
{
	int	index;
	int	ret;
	
	if ((ret = UnionConnectLockTBL()) < 0)
	{
		UnionUserErrLog("in UnionReloadLockTBL:: UnionConnectLockTBL!\n");
		return(ret);
	}
	
	if ((ret = UnionApplyOsLock(NULL)) < 0)
	{
		UnionUserErrLog("in UnionReloadLockTBL:: UnionApplyOsLock!\n");
		return(ret);
	}

	//pgunionLockTBL->realNum = 0;
	pgunionLockTBL->maxNum = UnionGetMaxNumOfLock();
	for (index = 0; index < pgunionLockTBL->maxNum; index++);
	{
		memset(&(pgunionLockTBL->lockGrp[index]),0,sizeof(pgunionLockTBL->lockGrp[index]));
	}
	UnionFreeOsLock(NULL);
	return(0);
}

// ��������
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionResetLockTBL()
{
	return(UnionReloadLockTBL());
}

// �ж�һ�����Ƿ�����Դ����
/*
�������
	prec		����¼
�������
	��
����ֵ
	1		��
	0		��
*/
int UnionIsResLevelLockedRec(PUnionLockRec prec)
{
	if (!UnionIsValidLockedRec(prec))
		return(0);
	if (strlen(prec->keyWord) == 0)
		return(1);
	else
		return(0);
}

// �ж�һ�����Ƿ��Ǽ�¼����
/*
�������
	prec		����¼
�������
	��
����ֵ
	1		��
	0		��
*/
int UnionIsRecLevelLockedRec(PUnionLockRec prec)
{
	if (!UnionIsValidLockedRec(prec))
		return(0);
	if (strlen(prec->keyWord) == 0)
		return(0);
	else
		return(1);
}

// �ж�һ�����Ƿ��ǺϷ���
/*
�������
	prec		����¼
�������
	��
����ֵ
	1		��
	0		��
*/
int UnionIsValidLockedRec(PUnionLockRec prec)
{
	if (prec == NULL)
		return(0);
	if (strlen(prec->resName) == 0)
		return(0);
	return(1);
}

// ��ӡһ����
/*
�������
	prec		����¼ָ��
�������
	fp		�ļ����
����ֵ
	>=0		��ӡ��������
	<0		������
*/
int UnionPrintLockRecToFile(PUnionLockRec prec,FILE *fp)
{
	FILE	*outFp = stdout;
	
	if (!UnionIsValidLockedRec(prec))
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"%03d %d ",prec->readingLocksNum,prec->isWritingLocked);
	if (prec->readingLocksNum <= 0)
		fprintf(outFp,"%14s ","");
	else
		fprintf(outFp,"%14s ",prec->readingTime);
	if (prec->isWritingLocked <= 0)
		fprintf(outFp,"%14s ","");
	else
		fprintf(outFp,"%14s ",prec->writingTime);
	fprintf(outFp,"%20s %s\n",prec->resName,prec->keyWord);
	return(1);
}

#ifdef _openSpierAPI_
// ��ʾ������
/*
�������
	resID		�ϲ�Ӧ��Ϊ��������id��
�������
	��
����ֵ
	>=0		����Ŀ
	<0		������
*/
int UnionSpierAllLock(int resID)
{
	int	i,fldIndex;
	int	ret;
	int	num = 0;
	char	fldValue[20][1024+1];
		
	if ((ret = UnionConnectLockTBL()) < 0)
	{
		UnionUserErrLog("in UnionDisplayAllLock:: UnionConnectLockTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionLockTBL->maxNum; i++)
	{
		if (strlen(pgunionLockTBL->lockGrp[i].resName) == 0)
			continue;
		fldIndex = 0;
		strcpy(fldValue[fldIndex],pgunionLockTBL->lockGrp[i].resName);
		fldIndex++;
		strcpy(fldValue[fldIndex],pgunionLockTBL->lockGrp[i].keyWord);
		fldIndex++;
		sprintf(fldValue[fldIndex],"%d",pgunionLockTBL->lockGrp[i].readingLocksNum);
		fldIndex++;
		strcpy(fldValue[fldIndex],pgunionLockTBL->lockGrp[i].readingTime);
		fldIndex++;
		sprintf(fldValue[fldIndex],"%d",pgunionLockTBL->lockGrp[i].isWritingLocked);
		fldIndex++;
		strcpy(fldValue[fldIndex],pgunionLockTBL->lockGrp[i].writingTime);
		fldIndex++;
		UnionSpierSpecRecOfSpecRes(fldValue,fldIndex,resID);
		num++;
	}
	return(num);
}
#endif

// ��ӡ������
/*
�������
	��
�������
	fp		�ļ����
����ֵ
	>=0		����Ŀ
	<0		������
*/
int UnionPrintAllLockToFile(FILE *fp)
{
	UnionPrintAllRecLockOfSpecResToFile("",fp);
	UnionPrintAllResLockToFile(fp);
	return(0);
}

// ��ӡָ����Դ�ļ�¼����
/*
�������
	resName		��Դ����
�������
	fp		�ļ����
����ֵ
	>=0		����Ŀ
	<0		������
*/
int UnionPrintAllRecLockOfSpecResToFile(char *resName,FILE *fp)
{
	int	i;
	int	ret;
	int	num = 0;
	FILE	*outFp = stdout;
		
	if (fp != NULL)
		outFp = fp;
		
	if ((ret = UnionConnectLockTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAllRecLockOfSpecResToFile:: UnionConnectLockTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionLockTBL->maxNum; i++)
	{
		if (!UnionIsRecLevelLockedRec(&(pgunionLockTBL->lockGrp[i])))
			continue;
		if ((resName != NULL) && (strlen(resName) != 0))
		{
			if (strcmp(resName,pgunionLockTBL->lockGrp[i].resName) != 0)
				continue;
		}
		if (UnionPrintLockRecToFile(&(pgunionLockTBL->lockGrp[i]),fp) > 0)
			num++;
	}
	fprintf(fp,"TotalRecLockNum = [%d]\n",num);
	return(num);
}

// ͳ��ָ����Դ����������¼��
/*
�������
	resName		��Դ����
�������
	fp		�ļ����
����ֵ
	>=0		����Ŀ
	<0		������
*/
int UnionCountAllRecLockOfSpecRes(char *resName,int *writingLockRecNum,int *readingLockRecNum)
{
	int	i;
	int	ret;
	int	writingNum = 0,readingNum = 0;

	if ((ret = UnionConnectLockTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAllRecLockOfSpecResToFile:: UnionConnectLockTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionLockTBL->maxNum; i++)
	{
		if (!UnionIsRecLevelLockedRec(&(pgunionLockTBL->lockGrp[i])))
			continue;
		if ((resName != NULL) && (strlen(resName) != 0))
		{
			if (strcmp(resName,pgunionLockTBL->lockGrp[i].resName) != 0)
				continue;
		}
		if (pgunionLockTBL->lockGrp[i].isWritingLocked)
			writingNum++;
		if (pgunionLockTBL->lockGrp[i].readingLocksNum)
			readingNum++;
	}
	if (writingLockRecNum != NULL)
		*writingLockRecNum = writingNum;
	if (readingLockRecNum != NULL)
		*readingLockRecNum = readingNum;
	return(0);
}	

// ��ӡ������Դ������
/*
�������
	��
�������
	fp		�ļ����
����ֵ
	>=0		����Ŀ
	<0		������
*/
int UnionPrintAllResLockToFile(FILE *fp)
{
	int	i;
	int	ret;
	int	num = 0;
	FILE	*outFp = stdout;
		
	if (fp != NULL)
		outFp = fp;
		
	if ((ret = UnionConnectLockTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAllRecLockOfSpecResToFile:: UnionConnectLockTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionLockTBL->maxNum; i++)
	{
		if (!UnionIsResLevelLockedRec(&(pgunionLockTBL->lockGrp[i])))
			continue;
		if (UnionPrintLockRecToFile(&(pgunionLockTBL->lockGrp[i]),fp) > 0)
			num++;
	}
	fprintf(fp,"TotalResLockNum = [%d]\n",num);
	return(num);
}

// ����ָ��������¼
/*
�������
	resName		��Դ����
	keyWord		�ؼ���
�������
	firstNullPos	��õĵ�һ����λ��
����ֵ
	�ɹ�		����¼ָ��
	ʧ��		��ָ��
*/
PUnionLockRec UnionFindSpecLockRec(char *resName,char *keyWord,int *firstNullPos)
{
	int	i;
	int	ret;
	int	nullPosFound = 0;
	
	if ((resName == NULL) || (strlen(resName) == 0))
		return(NULL);
				
	if ((ret = UnionConnectLockTBL()) < 0)
	{
		UnionUserErrLog("in UnionFindSpecLockRec:: UnionConnectLockTBL!\n");
		return(NULL);
	}
	
	for (i = 0; i < pgunionLockTBL->maxNum; i++)
	{
		
		if (strcmp(pgunionLockTBL->lockGrp[i].resName, resName) != 0)
		{
			if ((strlen(pgunionLockTBL->lockGrp[i].resName) == 0) && (!nullPosFound))
			{
				if (firstNullPos != NULL)
					*firstNullPos = i;	// ��õ�һ����λ��
				nullPosFound = 1;
			}
			continue;
		}
		
		if ((keyWord != NULL) && (strlen(keyWord) != 0))	// Ҫ���ҵ��Ǽ�¼��
		{
			if (strcmp(pgunionLockTBL->lockGrp[i].keyWord,keyWord) != 0)
				continue;
			nullPosFound = i;
			return(&(pgunionLockTBL->lockGrp[i]));
		}
		else // Ҫ�������Դ��
		{
			if (strlen(pgunionLockTBL->lockGrp[i].keyWord) != 0)
				continue;
			nullPosFound = i;
			return(&(pgunionLockTBL->lockGrp[i]));
		}
	}
	return(NULL);
}

// ����ָ����Դ���������¼
/*
�������
	resName		��Դ����
	keyWord		�ؼ���
�������
	firstNullPos	��õĵ�һ����λ��
����ֵ
	�ɹ�		����¼ָ��
	ʧ��		��ָ��
*/
PUnionLockRec UnionFindLatestSpecLockRec(char *resName)
{
	int	i;
	int	ret;
	int	nullPosFound = 0;
	char	latestWritingTime[20+1];
	PUnionLockRec	pLockRec = NULL;
	
	if ((resName == NULL) || (strlen(resName) == 0))
		return(NULL);
				
	if ((ret = UnionConnectLockTBL()) < 0)
	{
		UnionUserErrLog("in UnionFindSpecLockRec:: UnionConnectLockTBL!\n");
		return(NULL);
	}
	
	for (i = 0; i < pgunionLockTBL->maxNum; i++)
	{		
		if ((strcmp(pgunionLockTBL->lockGrp[i].resName, resName) == 0)
			&& (strlen(pgunionLockTBL->lockGrp[i].writingTime) > 0))
		{
			if (strcmp(pgunionLockTBL->lockGrp[i].writingTime, latestWritingTime) > 0)
			{
				memset(latestWritingTime, 0, sizeof(latestWritingTime));
				strcpy(latestWritingTime, pgunionLockTBL->lockGrp[i].writingTime);
				pLockRec = &(pgunionLockTBL->lockGrp[i]);
			}
		}
	}
	return(pLockRec);
}

// ����ָ����Դ����������¼
/*
�������
	resName		��Դ����
	keyWord		�ؼ���
�������
	firstNullPos	��õĵ�һ����λ��
����ֵ
	�ɹ�		����¼ָ��
	ʧ��		��ָ��
*/
PUnionLockRec UnionFindForemostSpecLockRec(char *resName, int *firstNullPos)
{
	int	i;
	int	ret;
	int	nullPosFound = 0;
	char	foremostTime[20+1], lockTime[20+1];
	PUnionLockRec	pLockRec = NULL;
	
	if ((resName == NULL) || (strlen(resName) == 0))
		return(NULL);
				
	if ((ret = UnionConnectLockTBL()) < 0)
	{
		UnionUserErrLog("in UnionFindForemostSpecLockRec:: UnionConnectLockTBL!\n");
		return(NULL);
	}
	
	memset(foremostTime, 0, sizeof(foremostTime));
	
	for (i = 0; i < pgunionLockTBL->maxNum; i++)
	{		
		if ((strlen(pgunionLockTBL->lockGrp[i].resName) == 0) && (!nullPosFound))
		{
			if (firstNullPos != NULL)
				*firstNullPos = i;	// ��õ�һ����λ��
			nullPosFound = 1;
		}
		
		if (strcmp(pgunionLockTBL->lockGrp[i].resName, resName) == 0)
		{
			UnionProgramerLog("in UnionFindForemostSpecLockRec:: resName[%s]\n", resName);
			
			if (strlen(pgunionLockTBL->lockGrp[i].writingTime) > 0)
			{
				memset(lockTime, 0, sizeof(lockTime));
				strcpy(lockTime, pgunionLockTBL->lockGrp[i].writingTime);				
			}
			else if (strlen(pgunionLockTBL->lockGrp[i].readingTime) > 0)
			{
				memset(lockTime, 0, sizeof(lockTime));
				strcpy(lockTime, pgunionLockTBL->lockGrp[i].writingTime);
			}
			else
				continue;
			
			if (strlen(foremostTime) == 0)
			{
				strcpy(foremostTime, lockTime);
				pLockRec = &(pgunionLockTBL->lockGrp[i]);
			}
			else if (strcmp(lockTime, foremostTime) < 0)
			{	
				pLockRec = &(pgunionLockTBL->lockGrp[i]);
				strcpy(foremostTime, lockTime);
			}
		}
	}
	
	return(pLockRec);
}

// ����ָ�������͵���
/*
�������
	resName		��Դ����
	isWritingLock	�Ƿ�д��
�������
	firstNullPos	��õĵ�һ����λ��
����ֵ
	�ɹ�		����¼ָ��
	ʧ��		��ָ��
*/
PUnionLockRec UnionFindSpecLockRecWithLockType(char *resName, int isWritingLock, int *firstNullPos)
{
	int	i;
	int	ret;
	int	nullPosFound = 0;

	PUnionLockRec	pLockRec = NULL;
	
	if ((resName == NULL) || (strlen(resName) == 0))
	{
		UnionUserErrLog("in UnionFindSpecLockRecWithLockType:: parameter resName err!\n");
		return(NULL);
	}
						
	if ((ret = UnionConnectLockTBL()) < 0)
	{
		UnionUserErrLog("in UnionFindSpecLockRecWithLockType:: UnionConnectLockTBL!\n");
		return(NULL);
	}
	
	for (i = 0; i < pgunionLockTBL->maxNum; i++)
	{		
		if ((strlen(pgunionLockTBL->lockGrp[i].resName) == 0) && (!nullPosFound))
		{
			if (firstNullPos != NULL)
				*firstNullPos = i;	// ��õ�һ����λ��
			nullPosFound = 1;
		}
		
		if (strcmp(pgunionLockTBL->lockGrp[i].resName, resName) == 0)
		{
			if (isWritingLock)	// д��
			{
				if (pgunionLockTBL->lockGrp[i].isWritingLocked)
					return(&(pgunionLockTBL->lockGrp[i]));
			}
			else			// ����
			{
				if (!pgunionLockTBL->lockGrp[i].isWritingLocked)
					return(&(pgunionLockTBL->lockGrp[i]));
			}
		}
	}
	
	return(NULL);
}
// �ж�һ����Դ�Ƿ�д��ס��
/*
�������
	resName		��Դ����
�������
	��
����ֵ
	1		��
	0		��
*/
int UnionIsSpecResLocked(char *resName)
{
	PUnionLockRec 	prec;
	
	if ((prec = UnionFindSpecLockRec(resName,"",NULL)) == NULL)
		return(0);
	return(prec->isWritingLocked);
}

// �ж�һ��λ���Ƿ��ǺϷ�λ��
/*
�������
	pos		����¼λ��
�������
	��
����ֵ
	1		��
	0		��
*/
int UnionIsValidLockRecPos(int pos)
{
	if ((pos >= 0) && (pos < conMaxNumOfLock))
		return(1);
	else
		return(0);
}

// ����һ����
/*
�������
	resName		��¼����
	keyWord		�ؼ���
	isWritingLock	������������д�������������Ƕ���
	timeout		������ʱ�ĳ�ʱ��>=0����ʱ��<0�������ó�ʱ
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionApplyLockWithTimeout(char *resName,char *keyWord,int isWritingLock,int timeout)
{
	PUnionLockRec	prec,presRec;
	int		nullPos = -1, tmpPos;
	int		ret;
	int		recWritingLocked = 0;
	int		resLen,recLen;
	int		retryTimes = 0;
	time_t		start,now;
	long		alreadyLockedTime;
	int		applyRecLockAndHasResLock = 0;
	int		applyResourceLockAndHasRecordLock = 0;
	
	if ((resName == NULL) || ((resLen = strlen(resName)) == 0))
		return(errCodeParameter);
		
	if ((ret = UnionConnectLockTBL()) < 0)
	{
		UnionUserErrLog("in UnionApplyLockWithTimeout:: UnionConnectLockTBL!\n");
		return(ret);
	}
	
	if (keyWord == NULL)
		recLen = 0;
	else
		recLen = strlen(keyWord);
	
	if (recLen > 0)
		UnionProgramerLog("in UnionApplyLockWithTimeout:: %s begin lock resName = [%s]"
			" keyWord = [%s] isWritingLock = [%d] timeout = [%d]\n",
			UnionGetApplicationName(), resName, keyWord, isWritingLock, timeout);
	else
		UnionProgramerLog("in UnionApplyLockWithTimeout:: %s begin lock resName = [%s]"
			" keyWord = [null] isWritingLock = [%d] timeout = [%d]\n",
			UnionGetApplicationName(), resName, isWritingLock, timeout);
	
	time(&start);

loop:
	++retryTimes;
	if (retryTimes > 1)
	{
#ifdef _WIN32
		Sleep(1000);
#else
		usleep(10000);
#endif
	}
	if ((ret = UnionApplyOsLock(resName)) < 0)
	{
		UnionUserErrLog("in UnionApplyLockWithTimeout:: UnionApplyLockWithTimeout!\n");
		return(ret);
	}
	
	// Begin Mod By Huangbaoxin, 2009/03/17
FindForeMostLock:	
	if (recLen == 0)	// ��Դ��
	{		
		if (isWritingLock)	// ������Դд��
		{	
			if ((prec = UnionFindForemostSpecLockRec(resName, &nullPos)) == NULL)
				applyResourceLockAndHasRecordLock = 0;
			else
				applyResourceLockAndHasRecordLock = 1;
		}
		else			// ������Դ����
		{
			prec = UnionFindSpecLockRec(resName, NULL, &nullPos);
			applyResourceLockAndHasRecordLock = 0;
			if (prec == NULL)
			{
				if ((prec = UnionFindSpecLockRecWithLockType(resName, 1, &nullPos)) == NULL)
					applyResourceLockAndHasRecordLock = 0;
				else
					applyResourceLockAndHasRecordLock = 1;
			}
		}
		
		UnionProgramerLog("in UnionApplyLockWithTimeout::applyResourceLockAndHasRecordLock = [%d]\n", applyResourceLockAndHasRecordLock);
	}
	else			// ��¼��
	{	
		if (isWritingLock)	// �����¼д��
		{
			if ((prec = UnionFindSpecLockRec(resName, NULL, &nullPos)) == NULL)
			{
				prec = UnionFindSpecLockRec(resName, keyWord, &nullPos);
				applyRecLockAndHasResLock = 0;
			}
			else
				applyRecLockAndHasResLock = 1;				
		}
		else			// �����¼����
		{
			applyRecLockAndHasResLock = 0;
			prec = UnionFindSpecLockRec(resName, NULL, &nullPos);
			
			if ((prec == NULL) || (!prec->isWritingLocked))
			{
				prec = UnionFindSpecLockRec(resName, keyWord, &nullPos);
				//if ((prec != NULL) && (prec->isWritingLocked))
				//	applyRecLockAndHasResLock = 1;
			}
		}
		UnionProgramerLog("in UnionApplyLockWithTimeout::applyRecLockAndHasResLock 222 = [%d]\n", applyRecLockAndHasResLock);
	}
	// End Add By HuangBaoxin, 2009/03/17

InsertNewLockRec:
				
	if (prec == NULL)
	{	
		UnionProgramerLog("in UnionApplyLockWithTimeout::insert new lockrec\n");
		applyResourceLockAndHasRecordLock = 0;
		applyRecLockAndHasResLock = 0;
		
		if (!UnionIsValidLockRecPos(nullPos))
		{
			UnionUserErrLog("in UnionApplyLockWithTimeout:: UnionIsValidLockRecPos [%d] not valid!\n",nullPos);
			ret = errCodeLockMachanism_InvalidLockRecPos;
			goto errExit;
		}
		prec = &(pgunionLockTBL->lockGrp[nullPos]);
				
		if ((resLen >= sizeof(prec->resName)) || (recLen >= sizeof(prec->keyWord)))
		{
			if (recLen > 0)
				UnionUserErrLog("in UnionApplyLockWithTimeout:: resName [%s] too long or keyWord [%d] too long!\n",resName,keyWord);
			else
				UnionUserErrLog("in UnionApplyLockWithTimeout:: resName [%s] too long!\n",resName);
			UnionFreeOsLock(resName);
			return(ret = errCodeParameter);
		}
		memset(prec,0,sizeof(*prec));
		strcpy(prec->resName,resName);
		if (recLen > 0)
			strcpy(prec->keyWord,keyWord);
	}
					
	// �ѱ�д��ס
	if (prec->isWritingLocked)
	{
		// �ж��ѱ�д����ʱ��
		if ((ret = UnionCalcuSecondsPassedAfterSpecTime(prec->writingTime, &alreadyLockedTime)) < 0)
		{
			UnionUserErrLog("in UnionApplyLockWithTimeout:: UnionCalcuSecondsPassedAfterSpecTime!\n");
			goto errExit;
		}
		ret = errCodeLockMachanism_RecWritingLockedAlready;
		
		UnionProgramerLog("in UnionApplyLockWithTimeout::alreadyLockedTime writingTime[%d]\n", alreadyLockedTime);
		
		if (alreadyLockedTime < CONMAXLOCKEDTIMES)
			goto errExit;	// ����һ����
			
		// �ѱ�����һ����,�ͷ����д��
		if (keyWord != NULL)
			UnionAuditLog("in UnionApplyLockWithTimeout:: res-lock [%s] [%s] has been writing-locked since [%s], and now free it.\n",resName,keyWord,prec->writingTime);
		else
			UnionAuditLog("in UnionApplyLockWithTimeout:: rec-lock [%s] has been writing-locked since [%s], and now free it.\n",resName,prec->writingTime);
		
		/*
		prec->isWritingLocked = 0;
		memset(prec->writingTime,0,sizeof(prec->writingTime));
		*/
		memset(prec,0,sizeof(*prec));
		prec = NULL;
		goto FindForeMostLock;
	}
	
	if (isWritingLock)	// ����д��
	{
		if (prec->readingLocksNum > 0)	// �ѱ�����ס��
		{
			// �ж��ѱ�������ʱ��
			if ((ret = UnionCalcuSecondsPassedAfterSpecTime(prec->readingTime,&alreadyLockedTime)) < 0)
				goto errExit;
				
			ret = errCodeLockMachanism_WritingLockedNotPermittedWhenReadingLocked;
			
			UnionProgramerLog("in UnionApplyLockWithTimeout::alreadyLockedTime readingTime[%d]\n", alreadyLockedTime);
			
			if (alreadyLockedTime < CONMAXLOCKEDTIMES)
				goto errExit;	// ����һ����
				
			// �ѱ�����һ����,�ͷ����ж���
			if (keyWord != NULL)
				UnionAuditLog("in UnionApplyLockWithTimeout:: res-lock [%s] [%s] has been reading-locked since [%s], and now free it.\n",resName,keyWord,prec->readingTime);
			else
				UnionAuditLog("in UnionApplyLockWithTimeout:: rec-lock [%s] has been reading-locked since [%s], and now free it.\n",resName,prec->readingTime);
			
			/*
			prec->readingLocksNum = 0;
			memset(prec->readingTime,0,sizeof(prec->readingTime));
			*/
			memset(prec,0,sizeof(*prec));
			prec = NULL;
		}
		
		if (applyRecLockAndHasResLock)
			goto FindForeMostLock;
		
		if (applyResourceLockAndHasRecordLock)
			goto FindForeMostLock;
		
		if (prec == NULL)
			goto InsertNewLockRec;
			
		prec->isWritingLocked = 1;
		memset(prec->writingTime,0,sizeof(prec->writingTime));
		UnionGetFullSystemDateTime(prec->writingTime);
	}
	else	// �������
	{		
		if (applyResourceLockAndHasRecordLock)
		{
			UnionProgramerLog("in UnionApplyLockWithTimeout::goto FindForeMostLock[applyResourceLockAndHasRecordLock]\n");
			prec = NULL;
			goto FindForeMostLock;
		}
		
		if (prec == NULL)
			goto InsertNewLockRec;
						
		prec->readingLocksNum += 1;
		memset(prec->readingTime,0,sizeof(prec->readingTime));
		UnionGetFullSystemDateTime(prec->readingTime);
	}

	//UnionProgramerLog("in UnionApplyLockWithTimeout:: [%04d] [%04d] after [%d] [%d] on [%s] [%s]\n",prec->isWritingLocked,prec->readingLocksNum,isWritingLock,isApply,resName,keyWord);
	ret = 0;		
	UnionFreeOsLock(resName);
	if (recLen > 0)
		UnionProgramerLog("in UnionApplyLockWithTimeout:: success:: resName = [%s] keyWord = [%s] isWritingLock = [%d] timeout = [%d]\n",resName,keyWord,isWritingLock,timeout);
	else
		UnionProgramerLog("in UnionApplyLockWithTimeout:: sucess:: resName = [%s] keyWord = [null] isWritingLock = [%d] timeout = [%d]\n",resName,isWritingLock,timeout);
	return(ret);
	
errExit:
	UnionFreeOsLock(resName);
	// UnionProgramerLog("in UnionApplyLockWithTimeout:: apply lock of type [%d] for the %dth time!\n",isWritingLock,retryTimes);
	if (timeout < 0)
		goto loop;
	time(&now);
	if (now - start >= timeout)	// ����������ʱ��
		return(ret);
	else
		goto loop;
}

// �ͷ�һ����¼��
/*
�������
	resName		��¼����
	keyWord		�ؼ���
	isWritingLock	������������д�������������Ƕ�����<0 �������ж���
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionFreeLock(char *resName,char *keyWord,int isWritingLock)
{
	PUnionLockRec	prec,presRec;
	int		nullPos = -1;
	int		ret;
	int		recWritingLocked = 0;
	int		resLen,recLen;
	int		osLocked = 1;
	
	if ((resName == NULL) || ((resLen = strlen(resName)) == 0))
		return(errCodeParameter);

	if ((ret = UnionConnectLockTBL()) < 0)
	{
		UnionUserErrLog("in UnionFreeLock:: UnionConnectLockTBL!\n");
		return(ret);
	}
	if ((keyWord != NULL) && (strlen(keyWord) > 0))
		UnionProgramerLog("in UnionFreeLock:: begin resName = [%s] keyWord = [%s] isWritingLock = [%d]\n",
			resName,keyWord,isWritingLock);
	else
		UnionProgramerLog("in UnionFreeLock:: begin resName = [%s] keyWord = [null] isWritingLock = [%d]\n",
			resName,isWritingLock);
			
	if ((ret = UnionApplyOsLock(resName)) < 0)
	{
		UnionUserErrLog("in UnionFreeLock:: UnionFreeLock!\n");
		osLocked = 0;
		return(ret);
	}
	if ((prec = UnionFindSpecLockRec(resName,keyWord,&nullPos)) == NULL)
	{
		if (keyWord == NULL)
		{
			UnionUserErrLog("in UnionFreeLock:: resName [%s] not locked!\n",resName);
			ret = errCodeLockMachanism_ResNotLocked;
		}
		else
		{
			UnionUserErrLog("in UnionFreeLock:: rec [%s] of resName [%s] not locked!\n",keyWord,resName);
			ret = errCodeLockMachanism_RecNotLocked;
		}
		goto errExit;
	}
	if (isWritingLock > 0)	 // �ͷ�д��
	{
		if (!prec->isWritingLocked)	// δ��д��ס
		{
			ret = errCodeLockMachanism_RecWritingLockedNotExists;
			goto errExit;
		}
		prec->isWritingLocked = 0;
		memset(prec->writingTime,0,sizeof(prec->writingTime));
	}
	else	// �ͷŶ���
	{
		if (prec->readingLocksNum <= 0)	// δ������ס
		{
			ret = errCodeLockMachanism_RecReadingLockedNotExists;
			goto errExit;
		}
		//if (isWritingLock == 0)
			prec->readingLocksNum -= 1;
		//else
		//	prec->readingLocksNum = 0;
		if (prec->readingLocksNum < 0)
			prec->readingLocksNum = 0;
		if (prec->readingLocksNum == 0)
			memset(prec->readingTime,0,sizeof(prec->readingTime));
	}
	if (!prec->isWritingLocked && (prec->readingLocksNum <= 0))	// ��������
	{
		memset(prec,0,sizeof(prec));	// �ͷ�����¼λ��
	}
	ret = 0;		
errExit:
	if (osLocked)
		UnionFreeOsLock(resName);
	if (ret == 0)
	{
		if ((keyWord != NULL) && (strlen(keyWord) > 0))
			UnionProgramerLog("in UnionFreeLock:: success:: resName = [%s]"
				" keyWord = [%s] isWritingLock = [%d]\n",
				resName, keyWord, isWritingLock);
		else
			UnionProgramerLog("in UnionFreeLock:: success:: resName = [%s]"
				" keyWord = [null] isWritingLock = [%d]\n",
				resName, isWritingLock);
	}
	return(ret);
}


// ������Դд��
/*
�������
	resName		��Դ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionApplyResWritingLockOfSpecRes(char *resName)
{
	return(UnionApplyLockWithTimeout(resName,NULL,1,-1));
}

// ������Դ����
/*
�������
	resName		��Դ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionApplyResReadingLockOfSpecRes(char *resName)
{
	return(UnionApplyLockWithTimeout(resName,NULL,0,-1));
}

// ��������ʽ������Դд��
/*
�������
	resName		��Դ����
	timeout		��ʱ
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionApplyResWritingLockOfSpecResWithTimeout(char *resName,int timeout)
{
	return(UnionApplyLockWithTimeout(resName,NULL,1,timeout));
}

// ��������ʽ������Դ����
/*
�������
	resName		��Դ����
	timeout		��ʱ
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionApplyResReadingLockOfSpecResWithTimeout(char *resName,int timeout)
{
	return(UnionApplyLockWithTimeout(resName,NULL,0,timeout));
}

// �ͷ���Դд��
/*
�������
	resName		��Դ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionFreeResWritingLockOfSpecRes(char *resName)
{
	return(UnionFreeLock(resName,NULL,1));
}

// �ͷ���Դ����
/*
�������
	resName		��Դ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionFreeResReadingLockOfSpecRes(char *resName)
{
	return(UnionFreeLock(resName,NULL,0));
}

// �ͷ�ָ����Դ�����ж���
/*
�������
	resName		��Դ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionFreeAllResReadingLockOfSpecRes(char *resName)
{
	return(UnionFreeLock(resName,NULL,-1));
}

// �����¼д��
/*
�������
	resName		��¼����
	keyWord		�ؼ���
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionApplyRecWritingLockOfSpecRec(char *resName,char *keyWord)
{
	return(UnionApplyLockWithTimeout(resName,keyWord,1,-1));
}

// �����¼����
/*
�������
	resName		��¼����
	keyWord		�ؼ���
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionApplyRecReadingLockOfSpecRec(char *resName,char *keyWord)
{
	return(UnionApplyLockWithTimeout(resName,keyWord,0,-1));
}

// ��������ʽ�����¼д��
/*
�������
	resName		��¼����
	keyWord		�ؼ���
	timeout		��ʱ
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionApplyRecWritingLockOfSpecRecWithTimeout(char *resName,char *keyWord,int timeout)
{
	return(UnionApplyLockWithTimeout(resName,keyWord,1,timeout));
}

// ��������ʽ�����¼����
/*
�������
	resName		��¼����
	keyWord		�ؼ���
	timeout		��ʱ
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionApplyRecReadingLockOfSpecRecWithTimeout(char *resName,char *keyWord,int timeout)
{
	return(UnionApplyLockWithTimeout(resName,keyWord,0,timeout));
}

// �ͷż�¼д��
/*
�������
	resName		��¼����
	keyWord		�ؼ���
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionFreeRecWritingLockOfSpecRec(char *resName,char *keyWord)
{
	return(UnionFreeLock(resName,keyWord,1));
}

// �ͷż�¼����
/*
�������
	resName		��¼����
	keyWord		�ؼ���
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionFreeRecReadingLockOfSpecRec(char *resName,char *keyWord)
{
	return(UnionFreeLock(resName,keyWord,0));
}

// �ͷ�ָ����Դ�����м�¼����
/*
�������
	resName		��¼����
	keyWord		�ؼ���
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionFreeAllRecReadingLockOfSpecRec(char *resName,char *keyWord)
{
	return(UnionFreeLock(resName,keyWord,-1));
}

// ά��������,�Զ��ͷű���ס����ָ��ʱ�����
/*
�������
	maxLockedSeconds	����ס�����ʱ��
�������
	��
����ֵ
	>=0		�ɹ�,�������������
	<0		�������
*/
int UnionMaintainingAllDeadWritingLocks(int maxLockedSeconds)
{
	int	i;
	int	ret;
	int	writingNum = 0,readingNum = 0;
	long	seconds;
	//char	currentTime[14+1];
	
	if ((ret = UnionConnectLockTBL()) < 0)
	{
		UnionUserErrLog("in UnionMaintainingAllDeadWritingLocks:: UnionConnectLockTBL!\n");
		return(ret);
	}

	for (writingNum = 0,i = 0; i < pgunionLockTBL->maxNum; i++)
	{
		// �Ȳ�д��
		if (!pgunionLockTBL->lockGrp[i].isWritingLocked)	// δ����ס
			goto checkReadingLock;
		if ((ret = UnionCalcuSecondsPassedAfterSpecTime(pgunionLockTBL->lockGrp[i].writingTime,&seconds)) < 0)
		{
			UnionUserErrLog("in UnionMaintainingAllDeadWritingLocks:: UnionCalcuSecondsPassedAfterSpecTime!\n");
			goto checkReadingLock;
		}
		if (seconds < maxLockedSeconds)
			goto checkReadingLock;
		if ((ret = UnionApplyOsLock(pgunionLockTBL->lockGrp[i].resName)) < 0)
		{
			UnionUserErrLog("in UnionMaintainingAllDeadWritingLocks:: UnionApplyOsLock!\n");
			goto checkReadingLock;
		}
		if (pgunionLockTBL->lockGrp[i].isWritingLocked)	// �Ա���ס
		{
			UnionAuditLog("in UnionMaintainingAllDeadWritingLocks:: free [%s] [%s] which writing locked at [%s] ok!\n",pgunionLockTBL->lockGrp[i].resName,pgunionLockTBL->lockGrp[i].keyWord,
						pgunionLockTBL->lockGrp[i].writingTime);
			writingNum++;
			pgunionLockTBL->lockGrp[i].isWritingLocked = 0;
			if (pgunionLockTBL->lockGrp[i].readingLocksNum <= 0)
				memset(&(pgunionLockTBL->lockGrp[i]),0,sizeof(pgunionLockTBL->lockGrp[i]));
		}
		UnionFreeOsLock(pgunionLockTBL->lockGrp[i].resName);
		// �����
checkReadingLock:
		if (pgunionLockTBL->lockGrp[i].readingLocksNum <= 0) // δ������
			continue;
		if ((ret = UnionCalcuSecondsPassedAfterSpecTime(pgunionLockTBL->lockGrp[i].readingTime,&seconds)) < 0)
		{
			UnionUserErrLog("in UnionMaintainingAllDeadWritingLocks:: UnionCalcuSecondsPassedAfterSpecTime!\n");
			continue;
		}
		if (seconds < maxLockedSeconds)
			continue;
		if ((ret = UnionApplyOsLock(pgunionLockTBL->lockGrp[i].resName)) < 0)
		{
			UnionUserErrLog("in UnionMaintainingAllDeadWritingLocks:: UnionApplyOsLock!\n");
			continue;
		}
		if (pgunionLockTBL->lockGrp[i].readingLocksNum > 0)	// �Ա���ס
		{
			UnionAuditLog("in UnionMaintainingAllDeadWritingLocks:: free [%s] [%s] which reading locked at [%s] ok!\n",pgunionLockTBL->lockGrp[i].resName,pgunionLockTBL->lockGrp[i].keyWord,
						pgunionLockTBL->lockGrp[i].readingTime);
			readingNum++;
			pgunionLockTBL->lockGrp[i].readingLocksNum = 0;
			if (pgunionLockTBL->lockGrp[i].isWritingLocked == 0)
				memset(&(pgunionLockTBL->lockGrp[i]),0,sizeof(pgunionLockTBL->lockGrp[i]));
		}
		UnionFreeOsLock(pgunionLockTBL->lockGrp[i].resName);
	}
	if (writingNum > 0)
		UnionAuditLog("in UnionMaintainingAllDeadWritingLocks:: free [%d] dead writing locks!\n",writingNum);
	if (readingNum > 0)
		UnionAuditLog("in UnionMaintainingAllDeadWritingLocks:: free [%d] dead reading locks!\n",readingNum);
	return(writingNum+readingNum);
}	

// wangk add 2009-9-25
int UnionPrintLockRecInRecStrFormatToFile(PUnionLockRec prec,FILE *fp)
{
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintLockRecInRecStrFormatToFile:: fp is NULL pointer!\n");
		return(errCodeParameter);
	}

	if (!UnionIsValidLockedRec(prec))
		return(errCodeParameter);

	fprintf(fp,"readingLocksNum=%d|isWritingLocked=%d|",prec->readingLocksNum,prec->isWritingLocked);
	if (prec->readingLocksNum <= 0)
		fprintf(fp,"readingTime=|");
	else
		fprintf(fp,"readingTime=%s|",prec->readingTime);
	if (prec->isWritingLocked <= 0)
		fprintf(fp,"writingTime=|");
	else
		fprintf(fp,"writingTime=%s|",prec->writingTime);
	fprintf(fp,"resName=%s|keyWord=%s|\n",prec->resName,prec->keyWord);

	return(0);
}

int UnionPrintAllLockInRecStrFormatToFile(FILE *fp)
{
	int	i;
	int	ret;
	int	num = 0;
		
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintAllLockInRecStrFormatToFile:: fp is NULL pointer!\n");
		return(errCodeParameter);
	}
		
	if ((ret = UnionConnectLockTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAllLockToFile:: UnionConnectLockTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionLockTBL->maxNum; i++)
	{
		if (UnionPrintLockRecInRecStrFormatToFile(&(pgunionLockTBL->lockGrp[i]),fp) > 0)
			num++;
	}

	return(num);
}

int UnionPrintLockRecInRecStrFormatToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;

	if ((fileName == NULL) || (strlen(fileName) == 0))
	{
		UnionUserErrLog("in UnionPrintLockRecInRecStrFormatToSpecFile:: NULL poionter!\n");
		return(errCodeParameter);
	}

	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintLockRecInRecStrFormatToSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	ret = UnionPrintAllLockInRecStrFormatToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}
// wangk add end 2009-9-25

// add by wangk 2009-12-09
int UnionFreeAllWriteLock()
{
	int	ret;

	if ((ret = UnionIsLockTBLConnected()) == 0)
	{
		if ((ret = UnionConnectLockTBL()) < 0)
		{
			UnionUserErrLog("in UnionFreeAllWriteLock:: UnionConnectLockTBL!\n");
			return(ret);
		}
	}

	if ((ret = UnionFreeAllResWriteLock()) < 0)
	{
		UnionUserErrLog("in UnionFreeAllWriteLock:: UnionFreeAllResWriteLock error! ret = [%d]\n",ret);
		return (ret);
	}
	if ((ret = UnionFreeAllRecWriteLock()) < 0)
	{
		UnionUserErrLog("in UnionFreeAllWriteLock:: UnionFreeAllRecWriteLock error! ret = [%d]\n",ret);
		return (ret);
	}
	return (0);
}

int UnionFreeAllReadLock()
{
	int	ret;

	if ((ret = UnionIsLockTBLConnected()) == 0)
	{
		if ((ret = UnionConnectLockTBL()) < 0)
		{
			UnionUserErrLog("in UnionFreeAllReadLock:: UnionConnectLockTBL!\n");
			return(ret);
		}
	}

	if ((ret = UnionFreeAllResReadLock()) < 0)
	{
		UnionUserErrLog("in UnionFreeAllReadLock:: UnionFreeAllResReadLock error! ret = [%d]\n",ret);
		return (ret);
	}
	if ((ret = UnionFreeAllRecReadLock()) < 0)
	{
		UnionUserErrLog("in UnionFreeAllReadLock:: UnionFreeAllRecReadLock error! ret = [%d]\n",ret);
		return (ret);
	}
	return (0);
}

int UnionFreeAllResWriteLock()
{
	int	ret;
	int	i;

	if ((ret = UnionIsLockTBLConnected()) == 0)
	{
		if ((ret = UnionConnectLockTBL()) < 0)
		{
			UnionUserErrLog("in UnionFreeAllResWriteLock:: UnionConnectLockTBL!\n");
			return(ret);
		}
	}
	for (i = 0; i < pgunionLockTBL->maxNum; i++)
	{	
		if (!UnionIsResLevelLockedRec(&(pgunionLockTBL->lockGrp[i])))
			continue;

		if ((pgunionLockTBL->lockGrp[i].resName !=NULL) && pgunionLockTBL->lockGrp[i].isWritingLocked == 1)
		{
			UnionLog("%s\n",pgunionLockTBL->lockGrp[i].resName);
			if ((ret = UnionFreeLock(pgunionLockTBL->lockGrp[i].resName,NULL,1) < 0))
			{
				UnionUserErrLog("in UnionFreeAllResWriteLock:: UnionFreeLock error! ret = [%d]\n",ret);
				return (ret);
			}
		}
	}
	return (0);
}

int UnionFreeAllResReadLock()
{
	int	ret;
	int	i;

	if ((ret = UnionIsLockTBLConnected()) == 0)
	{
		if ((ret = UnionConnectLockTBL()) < 0)
		{
			UnionUserErrLog("in UnionFreeAllResReadLock:: UnionConnectLockTBL!\n");
			return(ret);
		}
	}

	for (i = 0; i < pgunionLockTBL->maxNum; i++)
	{	
		if (!UnionIsResLevelLockedRec(&(pgunionLockTBL->lockGrp[i])))
			continue;

		if ((pgunionLockTBL->lockGrp[i].resName !=NULL) && pgunionLockTBL->lockGrp[i].isWritingLocked == 0)
		{
			UnionLog("%s\n",pgunionLockTBL->lockGrp[i].resName);
			if ((ret = UnionFreeLock(pgunionLockTBL->lockGrp[i].resName,NULL,0) < 0))
			{
				UnionUserErrLog("in UnionFreeAllResReadLock:: UnionFreeLock error! ret = [%d]\n",ret);
				return (ret);
			}
		}
	}
	return (0);
}

int UnionFreeAllRecWriteLock()
{
	int	ret;
	int	i;

	if ((ret = UnionIsLockTBLConnected()) == 0)
	{
		if ((ret = UnionConnectLockTBL()) < 0)
		{
			UnionUserErrLog("in UnionFreeAllRecWriteLock:: UnionConnectLockTBL!\n");
			return(ret);
		}
	}

	for (i = 0; i < pgunionLockTBL->maxNum; i++)
	{	
		if (!UnionIsRecLevelLockedRec(&(pgunionLockTBL->lockGrp[i])))
			continue;
		if ((pgunionLockTBL->lockGrp[i].resName !=NULL) && (pgunionLockTBL->lockGrp[i].keyWord != NULL) && (pgunionLockTBL->lockGrp[i].isWritingLocked == 1))
		{
			UnionLog("%s\n",pgunionLockTBL->lockGrp[i].resName,pgunionLockTBL->lockGrp[i].keyWord);
			if ((ret = UnionFreeLock(pgunionLockTBL->lockGrp[i].resName,pgunionLockTBL->lockGrp[i].keyWord,1) < 0))
			{
				UnionUserErrLog("in UnionFreeAllRecWriteLock:: UnionFreeLock error! ret = [%d]\n",ret);
				return (ret);
			}
		}
	}
	return (0);
}

int UnionFreeAllRecReadLock()
{
	int	ret;
	int	i;

	if ((ret = UnionIsLockTBLConnected()) == 0)
	{
		if ((ret = UnionConnectLockTBL()) < 0)
		{
			UnionUserErrLog("in UnionFreeAllRecReadLock:: UnionConnectLockTBL!\n");
			return(ret);
		}
	}

	for (i = 0; i < pgunionLockTBL->maxNum; i++)
	{	
		if (!UnionIsRecLevelLockedRec(&(pgunionLockTBL->lockGrp[i])))
			continue;
		if ((pgunionLockTBL->lockGrp[i].resName !=NULL) && (pgunionLockTBL->lockGrp[i].keyWord != NULL) && pgunionLockTBL->lockGrp[i].isWritingLocked == 0)
		{
			UnionLog("%s\n",pgunionLockTBL->lockGrp[i].resName,pgunionLockTBL->lockGrp[i].keyWord);
			if ((ret = UnionFreeLock(pgunionLockTBL->lockGrp[i].resName,pgunionLockTBL->lockGrp[i].keyWord,0) < 0))
			{
				UnionUserErrLog("in UnionFreeAllRecReadLock:: UnionFreeLock error! ret = [%d]\n",ret);
				return (ret);
			}
		}
	}
	return (0);
}

// add end wangk 2009-12-09
