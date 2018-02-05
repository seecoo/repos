//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2009/6/2

#define _UnionDesKeyDB_2_x_
#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "unionModule.h"
#include "UnionStr.h"

#ifndef _UnionDesKey_3_x_
#define _UnionDesKey_3_x_
#endif
#ifndef _UnionDesKey_4_x_
#define _UnionDesKey_4_x_
#endif

#include "unionDesKey.h"
#include "UnionLog.h"

PUnionSharedMemoryModule	pgunionDesKeyDBMDL = NULL;
PUnionDesKeyDB			pgunionDesKeyDB = NULL;
PUnionDesKey			pgunionDesKey = NULL;

int UnionUnlockDesKeyDB()
{
	int	ret;
	
	if ((ret = UnionWritingUnlockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionUnlockDesKeyDB:: UnionWritingUnlockDesKeyTBL!\n");
		return(ret);
	}
	if ((ret = UnionReadingUnlockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionUnlockDesKeyDB:: UnionReadingUnlockDesKeyTBL!\n");
		return(ret);
	}
	return(ret);
}

int UnionUnlockAllLockedDesKeyInDesKeyDB()
{
	int	ret;
	long	i;
	long	totalNum = 0;

	if ((ret = UnionConnectDesKeyDBImage()) < 0)
	{
		UnionUserErrLog("in UnionUnlockAllLockedDesKeyInDesKeyDB: UnionConnectDesKeyDBImage!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		if (!(pgunionDesKey+i)->isWritingLocked)
			continue;
		(pgunionDesKey+i)->isWritingLocked = 0;
		totalNum++;
	}
	return(totalNum);
}
	
int UnionUnlockDesKeyInDesKeyDB(char *keyName)
{
	long	index;
	
	if ((index = UnionFindDesKeyPosInKeyDBImage(keyName)) < 0)
	{
		UnionUserErrLog("in UnionUnlockDesKeyInDesKeyDB:: UnionFindDesKeyPosInKeyDBImage [%s]\n",keyName);
		return((int)index);
	}
	if ((pgunionDesKey+index)->isWritingLocked)
	{
		(pgunionDesKey+index)->isWritingLocked = 0;
		return(0);
	}
	else
		return(errCodeDesKeyDBMDL_KeyNotLocked);
}

// 2008/05/15增加
int UnionLockDesKeyInDesKeyDB(char *keyName)
{
	long	index;
	
	if ((index = UnionFindDesKeyPosInKeyDBImage(keyName)) < 0)
	{
		UnionUserErrLog("in UnionUnlockDesKeyInDesKeyDB:: UnionFindDesKeyPosInKeyDBImage [%s]\n",keyName);
		return((int)index);
	}
	if (!((pgunionDesKey+index)->isWritingLocked))
		(pgunionDesKey+index)->isWritingLocked = 1;
	return(0);
}

int UnionResetReadingLocksOfDesKeyDB()
{
	int	ret;
	
	if ((ret = UnionConnectDesKeyDBImage()) < 0)
	{
		UnionUserErrLog("in UnionResetReadingLocksOfDesKeyDB:: UnionConnectDesKeyDBImage!\n");
		return(ret);
	}
	pgunionDesKeyDB->readingLocks = 0;
	return(0);
}

int UnionResetWritingLocksOfDesKeyDB()
{
	int	ret;
	
	if ((ret = UnionConnectDesKeyDBImage()) < 0)
	{
		UnionUserErrLog("in UnionResetWritingLocksOfDesKeyDB:: UnionConnectDesKeyDBImage!\n");
		return(ret);
	}
	pgunionDesKeyDB->isWritingLocked = 0;
	return(0);
}

int UnionGetReadingLocksOfDesKeyDB()
{
	int	ret;
	
	if ((ret = UnionConnectDesKeyDBImage()) < 0)
	{
		UnionUserErrLog("in UnionGetReadingLocksOfDesKeyDB:: UnionConnectDesKeyDBImage!\n");
		return(ret);
	}
	return(pgunionDesKeyDB->readingLocks);
}

int UnionGetWritingLocksOfDesKeyDB()
{
	int	ret;
	
	if ((ret = UnionConnectDesKeyDBImage()) < 0)
	{
		UnionUserErrLog("in UnionGetWritingLocksOfDesKeyDB:: UnionConnectDesKeyDBImage!\n");
		return(ret);
	}
	return(pgunionDesKeyDB->isWritingLocked);
}

// 申请写锁表
int UnionWritingLockDesKeyTBL()
{
	int	ret;
	int	index;
	
	if ((ret = UnionConnectDesKeyDBImage()) < 0)
	{
		UnionUserErrLog("in UnionWritingLockDesKeyTBL:: UnionConnectDesKeyDBImage!\n");
		return(ret);
	}
	if ((pgunionDesKeyDB->isWritingLocked <= 0) && (pgunionDesKeyDB->readingLocks <= 0))	// 表未被锁住，置锁住标志
		pgunionDesKeyDB->isWritingLocked = 1;
	else							// 表已被锁住
		return(errCodeDesKeyDBMDL_TBLLocked);
		
	for (index = 0; index <  pgunionDesKeyDB->num; index++)
	{
		if ((pgunionDesKey+index)->isWritingLocked)	// 有记录被写锁住了，不能置写锁
		{	
			pgunionDesKeyDB->isWritingLocked = 0;
			return(errCodeDesKeyDBMDL_SomeRecIsWritingLocked);
		}
	}
	// 没有记录被写锁住
	return(0);
}

// 解除写锁表
int UnionWritingUnlockDesKeyTBL()
{
	int	ret;
	
	if ((ret = UnionConnectDesKeyDBImage()) < 0)
	{
		UnionUserErrLog("in UnionWritingUnlockDesKeyTBL:: UnionConnectDesKeyDBImage!\n");
		return(ret);
	}
	pgunionDesKeyDB->isWritingLocked = 0;
	return(0);
}

// 申请读锁表
int UnionReadingLockDesKeyTBL()
{
	int	ret;
	int	index;
	
	if ((ret = UnionConnectDesKeyDBImage()) < 0)
	{
		UnionUserErrLog("in UnionReadingLockDesKeyTBL:: UnionConnectDesKeyDBImage!\n");
		return(ret);
	}
	//if (pgunionDesKeyDB->readingLocks <= 0)	// 表未被写锁住，置锁住标志,2007/12/11屏蔽这一句
	if (!pgunionDesKeyDB->isWritingLocked)	// 表未被写锁住，置锁住标志，2007/12/11改为这一句。
	{
		pgunionDesKeyDB->readingLocks++;
		if (pgunionDesKeyDB->readingLocks <= 0)
			pgunionDesKeyDB->readingLocks = 1;
		return(0);
	}
	else							// 表已被锁住
		return(errCodeDesKeyDBMDL_TBLLocked);
}

// 解除读锁表
int UnionReadingUnlockDesKeyTBL()
{
	int	ret;
	
	if ((ret = UnionConnectDesKeyDBImage()) < 0)
	{
		UnionUserErrLog("in UnionReadingUnlockDesKeyTBL:: UnionConnectDesKeyDBImage!\n");
		return(ret);
	}
	--pgunionDesKeyDB->readingLocks;
	if (pgunionDesKeyDB->readingLocks < 0)
		pgunionDesKeyDB->readingLocks = 0;
	return(0);
}


// 申请写锁住记录
int UnionWritingLockDesKey(PUnionDesKey pkey)
{
	int	ret;
	
	if ((ret = UnionConnectDesKeyDBImage()) < 0)
	{
		UnionUserErrLog("in UnionWritingLockDesKey:: UnionConnectDesKeyDBImage!\n");
		return(ret);
	}
	if (pgunionDesKeyDB->isWritingLocked > 0)			// 表已被锁住
		return(errCodeDesKeyDBMDL_TBLLocked);
	
	if (pkey == NULL)
		return(errCodeParameter);

	if (pkey->isWritingLocked)	// 记录被写锁住了，不能置写锁
	{	
		UnionUserErrLog("in UnionWritingLockDesKey:: [%s] is writing-locked!\n",pkey->fullName);
		return(errCodeDesKeyDBMDL_SomeRecIsWritingLocked);
	}
	
	// 置记录为写锁住
	pkey->isWritingLocked = 1;
	return(0);
}

// 申请解除写锁住记录
int UnionWritingUnlockDesKey(PUnionDesKey pkey)
{
	int	ret;
	int	index;
	
	if ((ret = UnionConnectDesKeyDBImage()) < 0)
	{
		UnionUserErrLog("in UnionWritingUnlockDesKey:: UnionConnectDesKeyDBImage!\n");
		return(ret);
	}
	if (pgunionDesKeyDB->isWritingLocked > 0)			// 表已被锁住
		return(errCodeDesKeyDBMDL_TBLLocked);
	
	if (pkey == NULL)
		return(errCodeParameter);

	pkey->isWritingLocked = 0;

	return(0);
}

// 申请读锁住记录
int UnionReadingLockDesKey(PUnionDesKey pkey)
{
	int	ret;
	time_t	start,finish;
	int	timeoutOfDesKeyDBLock;
	
	if ((ret = UnionConnectDesKeyDBImage()) < 0)
	{
		UnionUserErrLog("in UnionReadingLockDesKey:: UnionConnectDesKeyDBImage!\n");
		return(ret);
	}
	if (pgunionDesKeyDB->isWritingLocked > 0)			// 表已被锁住
		return(errCodeDesKeyDBMDL_TBLLocked);
	
	if (pkey == NULL)
		return(errCodeParameter);

	if (pkey->isWritingLocked)
	{
		// 2008/05/15增加
		time(&start);
		if ((timeoutOfDesKeyDBLock = UnionReadIntTypeRECVar("timeoutOfWaitingDesKeyDBUnlocked")) > 10)
			timeoutOfDesKeyDBLock = 10;
		for (;;)
		{
			usleep(1000);
			if (!pkey->isWritingLocked)
				return(0);
			time(&finish);
			if (finish - start >= timeoutOfDesKeyDBLock)
			{
				UnionUserErrLog("in UnionReadingLockDesKey:: [%s] is writing-locked!\n",pkey->fullName);
				return(errCodeDesKeyDBMDL_SomeRecIsWritingLocked);
			}
		}
		// 2008/5/15增加结束
	}
		
	return(0);
}

// 申请解除读锁住记录
int UnionReadingUnlockDesKey(PUnionDesKey pkey)
{
	return(0);
}

PUnionDesKeyDB UnionGetCurrentActiveDesKeyDB()
{
	int	ret;
	
	if ((ret = UnionConnectDesKeyDBImage()) < 0)
	{
		UnionUserErrLog("in UnionGetCurrentActiveDesKeyDB:: UnionConnectDesKeyDBImage!\n");
		return(NULL);
	}
	return(pgunionDesKeyDB);
}

PUnionDesKey UnionGetCurrentActiveDesKeyTBL()
{
	int	ret;
	
	if ((ret = UnionConnectDesKeyDBImage()) < 0)
	{
		UnionUserErrLog("in UnionGetCurrentActiveDesKeyTBL:: UnionConnectDesKeyDBImage!\n");
		return(NULL);
	}
	return(pgunionDesKey);
}

int UnionGetCurrentActiveDesKeyNumInKeyDB()
{
	int	ret;
	
	if ((ret = UnionConnectDesKeyDBImage()) < 0)
	{
		UnionUserErrLog("in UnionGetCurrentActiveDesKeyNumInKeyDB:: UnionConnectDesKeyDBImage!\n");
		return(ret);
	}
	else
		return(pgunionDesKeyDB->num);
}


long UnionGetMaxKeyNumOfDesKeyDBImage()
{
	int	maxNum;
	
	if ((maxNum = UnionReadIntTypeRECVar("maxNumOfDesKeyDBImage")) <= 0)
		maxNum = 1000;
	return(maxNum);
}

// Added by Wolfgang Wang, 2004/11/25
int UnionIsDesKeyDBImageConnected()
{
	if ((!UnionIsSharedMemoryInited(conMDLNameOfUnionDesKeyDB)) || (pgunionDesKeyDB == NULL) || (pgunionDesKey == NULL))
		return(0);
	else
		return(1);
}

// Module Layer Functions
int UnionConnectDesKeyDBImage()
{
	int	ret;
	long	maxKeyNum;

	if (UnionIsDesKeyDBImageConnected())
		return(0);
		
	if ((maxKeyNum = UnionGetMaxKeyNumOfDesKeyDBImage()) < 0)
	{
		UnionUserErrLog("in UnionConnectDesKeyDBImage:: UnionGetMaxKeyNumOfDesKeyDBImage!\n");
		return(maxKeyNum);
	}
	
	if ((pgunionDesKeyDBMDL = UnionConnectSharedMemoryModule(conMDLNameOfUnionDesKeyDB,
			sizeof(TUnionDesKeyDB) + sizeof(TUnionDesKey) * maxKeyNum)) == NULL)
	{
		UnionUserErrLog("in UnionConnectDesKeyDBImage:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}
	if ((pgunionDesKeyDB = (PUnionDesKeyDB)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionDesKeyDBMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectDesKeyDBImage:: PUnionDesKeyDB!\n");
		return(errCodeSharedMemoryModule);
	}
	if ((pgunionDesKey = (PUnionDesKey)((unsigned char *)pgunionDesKeyDB + sizeof(*pgunionDesKeyDB))) == NULL)
	{
		UnionUserErrLog("in UnionConnectDesKeyDBImage:: PUnionDesKey!\n");
		return(errCodeSharedMemoryModule);
	}
	pgunionDesKeyDB->pkey = pgunionDesKey;
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionDesKeyDBMDL))
	{
		pgunionDesKeyDB->num = 0;
		pgunionDesKeyDB->maxKeyNum = maxKeyNum;
		pgunionDesKeyDB->isWritingLocked = 0;
		pgunionDesKeyDB->readingLocks = 0;
		return(ret);
	}
	else
		return(0);
}

int UnionDisconnectDesKeyDBImage()
{
	if (!UnionIsDesKeyDBImageConnected())
		return(0);
	pgunionDesKey = NULL;
	pgunionDesKeyDB = NULL;
	return(UnionDisconnectShareModule(pgunionDesKeyDBMDL));
}

int UnionRemoveDesKeyDBInMemory()
{
	UnionDisconnectDesKeyDBImage();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfUnionDesKeyDB));
}

int UnionConnectExistingDesKeyDBImage()
{
	int	ret;

	if (UnionIsDesKeyDBImageConnected())
		return(0);

	if ((pgunionDesKeyDBMDL = UnionConnectExistingSharedMemoryModule(conMDLNameOfUnionDesKeyDB,
			sizeof(TUnionDesKeyDB))) == NULL)
	{
		UnionUserErrLog("in UnionConnectExistingDesKeyDBImage:: UnionConnectExistingSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}
	if ((pgunionDesKeyDB = (PUnionDesKeyDB)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionDesKeyDBMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectExistingDesKeyDBImage:: PUnionDesKeyDB!\n");
		return(errCodeSharedMemoryModule);
	}
	
	
	if ((pgunionDesKey = (PUnionDesKey)((unsigned char *)pgunionDesKeyDB + sizeof(*pgunionDesKeyDB))) == NULL)
	{
		UnionUserErrLog("in UnionConnectExistingDesKeyDBImage:: PUnionDesKey!\n");
		return(errCodeSharedMemoryModule);
	}
	pgunionDesKeyDB->pkey = pgunionDesKey;
	
	return(0);
}

int UnionInsertDesKeyIntoKeyDBImage(PUnionDesKey pkey)
{
	int	ret;
	long    index;
	int	check;
	int	pos;
	
	if (!UnionIsValidDesKey(pkey))
	{
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDBImage:: UnionIsValidDesKey [%s]\n",pkey->fullName);
		return(errCodeParameter);
	}
	if ((ret = UnionWritingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDBImage:: UnionWritingLockDesKeyTBL !\n");
		return(ret);
	}
	pkey->useTimes = 0;
	pkey->isWritingLocked = 0;
	time(&(pkey->lastUpdateTime));
	if (pgunionDesKeyDB->num == pgunionDesKeyDB->maxKeyNum)
	{
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDBImage:: DesKeyDB is full!\n");
		UnionWritingUnlockDesKeyTBL();
		return(errCodeDesKeyDBMDL_KeyDBFull);
	}
	for (index = 0; index < pgunionDesKeyDB->num; index++)
	{
		if ((check = strcmp(pkey->fullName,(pgunionDesKey + index)->fullName)) == 0)
		{
			UnionUserErrLog("in UnionInsertDesKeyIntoKeyDBImage:: [%s] already exists!\n",pkey->fullName);
			UnionWritingUnlockDesKeyTBL();
			return(errCodeDesKeyDBMDL_KeyAlreadyExists);
		}
		if (check > 0)
			continue;
		else
			break;
	}
	pos = index;	// 新记录将插在pos指示的位置
	for (index = pgunionDesKeyDB->num - 1; index >= pos; index--)
	{
		memcpy(pgunionDesKey+index+1,pgunionDesKey+index,sizeof(TUnionDesKey));
	}
	memcpy(pgunionDesKey+pos,pkey,sizeof(TUnionDesKey));
	pgunionDesKeyDB->num++;
	UnionGetFullSystemDateTime(pgunionDesKeyDB->lastUpdatedTime);

	UnionWritingUnlockDesKeyTBL();
	return(0);
}

int UnionDeleteDesKeyFromKeyDBImage(char *fullName)
{
	int	ret;
	long    index;
	int	pos;
	TUnionDesKey	key;

	if ((ret = UnionWritingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionDeleteDesKeyFromKeyDBImage:: UnionWritingLockDesKeyTBL !\n");
		return(ret);
	}
	
	for (index = 0; index < pgunionDesKeyDB->num; index++)
	{
		if (strcmp(fullName,(pgunionDesKey + index)->fullName) == 0)
			break;
	}
	if (index == pgunionDesKeyDB->num)
	{
		UnionWritingUnlockDesKeyTBL();
		return(errCodeDesKeyDBMDL_KeyNonExists);
	}
	pos = index;
	memcpy(&key,pgunionDesKey+pos,sizeof(key));
	for (index = pos+1; index < pgunionDesKeyDB->num; index++)
	{
		memcpy(pgunionDesKey+index-1,pgunionDesKey + index,sizeof(TUnionDesKey));
	}
	pgunionDesKeyDB->num--;
	UnionGetFullSystemDateTime(pgunionDesKeyDB->lastUpdatedTime);

	UnionWritingUnlockDesKeyTBL();

	return(0);
}

int UnionReadDesKeyFromKeyDBImageAnyway(PUnionDesKey pkey)
{
	int	ret;
	long	index=0;

	if ((ret = UnionConnectDesKeyDBImage()) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromKeyDBImageAnyway:: UnionConnectDesKeyDBImage!\n");
		return(ret);
	}
	if (pkey == NULL)
	{
		UnionUserErrLog("in UnionReadDesKeyFromKeyDBImageAnyway:: null Pointer!\n");
		return(errCodeParameter);
	}
	
	if ((index = UnionFindDesKeyPosInKeyDBImage(pkey->fullName)) < 0)
	{
		ret = (int)index;
		if (ret == errCodeDesKeyDBMDL_KeyNonExists)
			UnionAuditLog("in UnionReadDesKeyFromKeyDBImageAnyway:: UnionFindDesKeyPosInKeyDBImage [%s]!\n",pkey->fullName);
		else
			UnionUserErrLog("in UnionReadDesKeyFromKeyDBImageAnyway:: UnionFindDesKeyPosInKeyDBImage [%s]!\n",pkey->fullName);
		return(ret);
	}
	if ((ret = UnionReadingLockDesKey(pgunionDesKey+index)) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromKeyDBImageAnyway:: UnionReadingLockDesKey [%s]!\n",pkey->fullName);
		return(ret);
	}
	memcpy(pkey,pgunionDesKey+index,sizeof(TUnionDesKey));
	UnionReadingUnlockDesKey(pgunionDesKey+index);
	return(0);
}

int UnionUseDesKeyFromKeyDBImage(PUnionDesKey pkey)
{
	return(UnionReadDesKeyFromKeyDBImage(pkey));
}

int UnionReadDesKeyFromKeyDBImage(PUnionDesKey pkey)
{
	int	ret;
	
	if ((ret = UnionReadDesKeyFromKeyDBImageAnyway(pkey)) < 0)
	{
		if (ret == errCodeDesKeyDBMDL_KeyNonExists)
			UnionAuditLog("in UnionReadDesKeyFromKeyDBImageAnyway:: UnionReadDesKeyFromKeyDBImageAnyway [%s]!\n",pkey->fullName);
		else
			UnionUserErrLog("in UnionReadDesKeyFromKeyDBImageAnyway:: UnionReadDesKeyFromKeyDBImageAnyway [%s]!\n",pkey->fullName);
		return(ret);
	}
	if (!UnionIsDesKeyStillEffective(pkey))
	{
		UnionUserErrLog("in UnionReadDesKeyFromKeyDBImage:: [%s] not effective!!\n",pkey->fullName);
		return(errCodeDesKeyDBMDL_KeyNotEffective);
	}
	return(0);
}	

int UnionUpdateDesKeyInKeyDBImage(PUnionDesKey pkey)
{
	if (pkey == NULL)
	{
		UnionUserErrLog("in UnionUpdateDesKeyInKeyDBImage:: null Pointer!\n");
		return(errCodeParameter);
	}

	return(UnionUpdateDesKeyValueInKeyDBImage(pkey->fullName,pkey->value,pkey->checkValue));
}
	
int UnionUpdateDesKeyValueInKeyDBImage(char *fullName,char *value,char *checkValue)
{
	int			ret;
	long			index=0;
	TUnionDesKey		key;
	char			idOfApp[40+1];
	char			varName[100];

	
	if ((index = UnionFindDesKeyPosInKeyDBImage(fullName)) < 0)
	{
		ret = (int)index;
		if (ret == errCodeDesKeyDBMDL_KeyNonExists)
			UnionAuditLog("in UnionUpdateDesKeyValueInKeyDBImage:: UnionFindDesKeyPosInKeyDBImage [%s]!\n",fullName);
		else
			UnionUserErrLog("in UnionUpdateDesKeyValueInKeyDBImage:: UnionFindDesKeyPosInKeyDBImage [%s]!\n",fullName);
		return(ret);
	}
	
	if ((ret = UnionWritingLockDesKey(pgunionDesKey+index)) < 0)
	{
		UnionUserErrLog("in UnionUpdateDesKeyValueInKeyDBImage:: UnionWritingLockDesKey!\n");
		return(ret);
	}
	if ((ret = UnionUpdateDesKeyValue(pgunionDesKey+index,value,checkValue)) < 0)
	{
		UnionUserErrLog("in UnionUpdateDesKeyValueInKeyDBImage:: UnionUpdateDesKeyValue [%s]\n",fullName);
		UnionWritingUnlockDesKey(pgunionDesKey+index);
		return(ret);
	}
	memcpy(&key,pgunionDesKey+index,sizeof(key));
	UnionGetFullSystemDateTime(pgunionDesKeyDB->lastUpdatedTime);
	UnionWritingUnlockDesKey(pgunionDesKey+index);
	return(0);
}

long UnionFindDesKeyPosInKeyDBImage(char *fullName)
{
	int	ret;
	long    index=0;
	//int	pos;
	long	left;
	long	right;
	int	check;
	
	if ((ret = UnionConnectDesKeyDBImage()) < 0)
	{
		UnionUserErrLog("in UnionFindDesKeyPosInKeyDBImage:: UnionConnectDesKeyDBImage!\n");
		return(ret);
	}
	
	left = 0;
	right = pgunionDesKeyDB->num - 1;
	index = (left+right)/2;
	while (1) 
	{
		if ((left > right) || (left >= pgunionDesKeyDB->num) || (right < 0))
		{
			ret = errCodeDesKeyDBMDL_KeyNonExists;
			goto exitRightNow;
		}
		if ((strcmp(fullName, (pgunionDesKey+left)->fullName) < 0) || 
			(strcmp(fullName, (pgunionDesKey+right)->fullName) > 0))
		{
			ret = errCodeDesKeyDBMDL_KeyNonExists;
			UnionNullLog("fullName = [%s] left = [%s] right = [%s] len = [%d]\n",
				fullName, (pgunionDesKey+left)->fullName, (pgunionDesKey+right)->fullName, 40);
			goto exitRightNow;
		}
		index = (left + right) / 2;
		//UnionNullLog("index = [%d] fullName = [%s] index = [%s] len = [%d]\n",index,fullName,(pgunionDesKey+index)->fullName,40);	
		if ((check = strcmp(fullName,(pgunionDesKey+index)->fullName)) == 0)
			break;
		//UnionNullLog("check = [%d]\n",check);
		if (check > 0)
		{
			left = index+1;
			continue;
		}
		if (check < 0)
		{
			right = index-1;
			continue;
		}
	}
		
	return(index);
	
exitRightNow:
	//UnionNullLog("in UnionFindDesKeyPosInKeyDBImage:: index = [%ld] left = [%ld] right = [%ld]\n",index,left,right);
	return(ret);
}

char *UnionGetDesKeyDBImageLastUpdatedTime()
{
	int	ret;
	
	if ((ret = UnionConnectDesKeyDBImage()) < 0)
	{
		UnionUserErrLog("in UnionGetDesKeyDBImageLastUpdatedTime:: UnionConnectDesKeyDBImage!\n");
		return(NULL);
	}
	
	return(pgunionDesKeyDB->lastUpdatedTime);
}

int UnionUpdateDesKeyAttrInKeyDBImage(PUnionDesKey pkey)
{
	int	ret;
	long	index=0;
	TUnionDesKey	desKey;

	if (!UnionIsValidDesKey(pkey))
	{
		UnionUserErrLog("in UnionUpdateDesKeyAttrInKeyDBImage:: UnionIsValidDesKey [%s]\n",pkey->fullName);
		return(errCodeParameter);
	}
	memset(&desKey,0,sizeof(desKey));
	memcpy(&desKey,pkey,sizeof(desKey));
	memset(desKey.value,0,sizeof(desKey.value));	
	
	if ((index = UnionFindDesKeyPosInKeyDBImage(pkey->fullName)) < 0)
	{
		ret = (int)index;
		if (ret == errCodeDesKeyDBMDL_KeyNonExists)
			UnionAuditLog("in UnionUpdateDesKeyAttrInKeyDBImage:: UnionFindDesKeyPosInKeyDBImage [%s]!\n",pkey->fullName);
		else
			UnionUserErrLog("in UnionUpdateDesKeyAttrInKeyDBImage:: UnionFindDesKeyPosInKeyDBImage [%s]!\n",pkey->fullName);
		return(ret);
	}
	
	if ((ret = UnionWritingLockDesKey(pgunionDesKey+index)) < 0)
	{
		UnionUserErrLog("in UnionUpdateDesKeyAttrInKeyDBImage:: UnionWritingLockDesKey !\n");
		return(ret);
	}
	
	memcpy(desKey.value,(pgunionDesKey+index)->value,sizeof(desKey.value));
	memcpy(desKey.oldValue,(pgunionDesKey+index)->oldValue,sizeof(desKey.oldValue));
	memcpy(desKey.checkValue,(pgunionDesKey+index)->checkValue,sizeof(desKey.checkValue));
	memcpy(desKey.oldCheckValue,(pgunionDesKey+index)->oldCheckValue,sizeof(desKey.oldCheckValue));

	desKey.useTimes = (pgunionDesKey+index)->useTimes;
	desKey.lastUpdateTime = (pgunionDesKey+index)->lastUpdateTime;
	desKey.isWritingLocked = 0;
	memcpy(pgunionDesKey+index,&desKey,sizeof(TUnionDesKey));

	UnionGetFullSystemDateTime(pgunionDesKeyDB->lastUpdatedTime);
	UnionWritingUnlockDesKey(pgunionDesKey+index);
	return(0);
}

// Wolfgang Wang, 2004/11/25
int UnionUpdateAllDesKeyFieldsInKeyDBImage(PUnionDesKey pkey)
{
	int		ret;
	long		index=0;
	
	if (!UnionIsValidDesKey(pkey))
	{
		UnionUserErrLog("in UnionUpdateAllDesKeyFieldsInKeyDBImage:: UnionIsValidDesKey [%s]\n",pkey->fullName);
		return(errCodeParameter);
	}
	
	if ((index = UnionFindDesKeyPosInKeyDBImage(pkey->fullName)) < 0)
	{
		ret = (int)index;
		if (ret == errCodeDesKeyDBMDL_KeyNonExists)
			UnionAuditLog("in UnionUpdateAllDesKeyFieldsInKeyDBImage:: UnionFindDesKeyPosInKeyDBImage [%s]!\n",pkey->fullName);
		else
			UnionUserErrLog("in UnionUpdateAllDesKeyFieldsInKeyDBImage:: UnionFindDesKeyPosInKeyDBImage [%s]!\n",pkey->fullName);
		return(ret);
	}

	if ((ret = UnionWritingLockDesKey(pgunionDesKey+index)) < 0)
	{
		UnionUserErrLog("in UnionUpdateAllDesKeyFieldsInKeyDBImage:: UnionWritingLockDesKey !\n");
		return(ret);
	}
	
	memcpy(pgunionDesKey+index,pkey,sizeof(TUnionDesKey));

	// UnionLog("in UnionUpdateAllDesKeyFieldsInKeyDBImage: (pgunionDesKey+index)->isWritingLocked is: [%d].\n", (pgunionDesKey+index)->isWritingLocked);
	
	UnionGetFullSystemDateTime(pgunionDesKeyDB->lastUpdatedTime);
	
	UnionWritingUnlockDesKey(pgunionDesKey+index);
	
	return(0);
}


int UnionExistDesKeyInKeyDBImage(char *fullName)
{
	if (UnionFindDesKeyPosInKeyDBImage(fullName) < 0)
		return(0);
	else
		return(1);
}


