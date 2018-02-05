//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2003/09/08

// 	Version:	2.0

//	2003/09/24, 在1.0基础上升级为2.0

//	2004/11/20, 增加了一些与备份服务器相关的函数

// 2006/8/14，在unionDesKeyDB3.0.20041125.c基础上升级为本程序
/*
// 2006/08/14增加了函数
// 申请锁表
int UnionWritingLockDesKeyTBL();
// 解除锁表
int UnionWritingUnlockDesKeyTBL();
// 申请写锁住记录
int UnionWritingLockDesKey(char *keyName);
// 申请解除写锁住记录
int UnionWritingUnlockDesKey(char *keyName);
*/

/* *** 2007/11/29,Wolfgang Wang
// 2007/11/29修改，增加了对conCompareKeyValue的处理
int UnionDesKeyDBOperation(TUnionKeyDBOperation operation,PUnionDesKey pdesKey)
// 2007/11/29, Added by Wolfgang Wang
long UnionCompareAllDesKeyValueInKeyDBWithBrothers()

关闭这个文件中的UnionIsDesKeyValueDynamicUpdated控制。
*/

/* 2007/12/11 修改
   UnionPrintStatusOfDesKeyDBToFile
   	修改之前，会读锁密钥库；当密钥库被锁住时，想通过这个函数查密钥库状态，实际不可行。
   	修改之后，取消了这个函数对密钥的写锁。
   UnionReadingLockDesKeyTBL:
   	修改之前，如果已被读锁住了，则申请读锁失败；修改之后，改为如果已被写锁住了，则申请读锁失败。
   增加：
   UnionAutoSynchronizeDesKeyToBrothers
   	自动向备份服务器同步不一致的密钥
*/
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
#include "UnionLog.h"

#ifndef _UnionDesKey_3_x_
#define _UnionDesKey_3_x_
#endif
#ifndef _UnionDesKey_4_x_
#define _UnionDesKey_4_x_
#endif

/*
#ifndef _useOracle_
#define _useOracle_
#endif
*/
#include "unionDesKeyDB.h"
#include "unionDesKeyDBLog.h"
#include "unionDesKeyDB.useOracle.h"
#include "unionREC.h"

#include "keyDBBackuper.h"	// 2004/11/20, Added by Wolfgang Wang
#include "UnionLog.h"

PUnionSharedMemoryModule	pgunionDesKeyDBMDL = NULL;
PUnionDesKeyDB			pgunionDesKeyDB = NULL;
PUnionDesKey			pgunionDesKey = NULL;

// added by wolfgang wang,20070611
int UnionResetReadingLocksOfDesKeyDB()
{
	int	ret;

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionResetReadingLocksOfDesKeyDB:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	pgunionDesKeyDB->readingLocks = 0;
	return(0);
}

int UnionResetWritingLocksOfDesKeyDB()
{
	int	ret;

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionResetWritingLocksOfDesKeyDB:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	pgunionDesKeyDB->isWritingLocked = 0;
	return(0);
}

int UnionGetReadingLocksOfDesKeyDB()
{
	int	ret;

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionGetReadingLocksOfDesKeyDB:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	return(pgunionDesKeyDB->readingLocks);
}

int UnionGetWritingLocksOfDesKeyDB()
{
	int	ret;

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionGetWritingLocksOfDesKeyDB:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	return(pgunionDesKeyDB->isWritingLocked);
}
// 20070611 增加结束

/* 20070517,Wolfgang Wang */
int gunionIsProgramControlDesKeyDB = 0;
int UnionSetProgramControlDesKeyDB()
{
	gunionIsProgramControlDesKeyDB = 1;
	return(0);
}
int UnionFreeProgramControlDesKeyDB()
{
	gunionIsProgramControlDesKeyDB = 0;
	return(0);
}
int UnionIsProgramControlDesKeyDB()
{
	return(gunionIsProgramControlDesKeyDB);
}
/* 20070517 Wolfgang Wang 增加结束 */

PUnionDesKeyDB UnionGetCurrentActiveDesKeyDB()
{
	int	ret;

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionGetCurrentActiveDesKeyDB:: UnionConnectDesKeyDB!\n");
		return(NULL);
	}
	return(pgunionDesKeyDB);
}

PUnionDesKey UnionGetCurrentActiveDesKeyTBL()
{
	int	ret;

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionGetCurrentActiveDesKeyTBL:: UnionConnectDesKeyDB!\n");
		return(NULL);
	}
	return(pgunionDesKey);
}

int UnionGetCurrentActiveDesKeyNumInKeyDB()
{
	int	ret;

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionGetCurrentActiveDesKeyNumInKeyDB:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	else
		return(pgunionDesKeyDB->num);
}


// 申请写锁表
int UnionWritingLockDesKeyTBL()
{
	int	ret;
	int	index;

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionWritingLockDesKeyTBL:: UnionConnectDesKeyDB!\n");
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

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionWritingUnlockDesKeyTBL:: UnionConnectDesKeyDB!\n");
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

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionReadingLockDesKeyTBL:: UnionConnectDesKeyDB!\n");
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

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionReadingUnlockDesKeyTBL:: UnionConnectDesKeyDB!\n");
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

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionWritingLockDesKey:: UnionConnectDesKeyDB!\n");
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

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionWritingUnlockDesKey:: UnionConnectDesKeyDB!\n");
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

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionReadingLockDesKey:: UnionConnectDesKeyDB!\n");
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

int UnionGetNameOfDesKeyDB(char *fileName)
{
	sprintf(fileName,"%s/keyFile/unionDesKeyDB.KDB",getenv("UNIONETC"));
	return(0);
}

long UnionGetMaxKeyNumFromKeyDBFile()
{
#ifndef _useOracle_
	int	ret;
	FILE	*fp;
	char	fileName[256];
	TUnionDesKeyDB	keyDB;

	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfDesKeyDB(fileName);

	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionGetMaxKeyNumFromKeyDBFile:: fopen [%s]\n",fileName);
		fclose(fp);
		return(errCodeUseOSErrCode);
	}

	memset(&keyDB,0,sizeof(keyDB));
	fread(&keyDB,sizeof(keyDB),1,fp);

	fclose(fp);

	return(keyDB.maxKeyNum);
#else
	return UnionGetMaxKeyNumFromKeyDBOnOracle();
#endif
}

// Added by Wolfgang Wang, 2004/11/25
int UnionIsDesKeyDBConnected()
{
	if ((!UnionIsSharedMemoryInited(conMDLNameOfUnionDesKeyDB)) || (pgunionDesKeyDB == NULL) || (pgunionDesKey == NULL))
		return(0);
	else
		return(1);
}

// Module Layer Functions
int UnionConnectDesKeyDB()
{
	int	ret;
	long	maxKeyNum;

	if (UnionIsDesKeyDBConnected())
		return(0);

#ifdef _useOracle_
	if ((ret = UnionConnectDBOnOracle(3)) < 0)
	{
		UnionUserErrLog("in UnionConnectDesKeyDB:: UnionConnectDBOnOracle!\n");
		return(ret);
	}
#endif

	if ((maxKeyNum = UnionGetMaxKeyNumFromKeyDBFile()) < 0)
	{
		UnionUserErrLog("in UnionConnectDesKeyDB:: UnionGetMaxKeyNumFromKeyDBFile!\n");
		return((int)maxKeyNum);
	}
	else if (maxKeyNum == 0)
	{
		UnionUserErrLog("in UnionConnectDesKeyDB:: maxKeyNum=[%d]!\n",maxKeyNum);
		return -1;
	}


	if ((pgunionDesKeyDBMDL = UnionConnectSharedMemoryModule(conMDLNameOfUnionDesKeyDB,
			sizeof(TUnionDesKeyDB) + sizeof(TUnionDesKey) * maxKeyNum)
			) == NULL)
	{
		UnionUserErrLog("in UnionConnectDesKeyDB:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}
	if ((pgunionDesKeyDB = (PUnionDesKeyDB)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionDesKeyDBMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectDesKeyDB:: PUnionDesKeyDB!\n");
		return(errCodeSharedMemoryModule);
	}


	if ((pgunionDesKey = (PUnionDesKey)((unsigned char *)pgunionDesKeyDB + sizeof(*pgunionDesKeyDB))) == NULL)
	{
		UnionUserErrLog("in UnionConnectDesKeyDB:: PUnionDesKey!\n");
		return(errCodeSharedMemoryModule);
	}
	pgunionDesKeyDB->pkey = pgunionDesKey;

	//UnionNullLog("in UnionConnectDesKeyDB::pgunionDesKeyDB = [%0x]\n",pgunionDesKeyDB);
	if (UnionIsNewCreatedSharedMemoryModule(pgunionDesKeyDBMDL))
	{
		pgunionDesKeyDB->num = 0;
		pgunionDesKeyDB->maxKeyNum = maxKeyNum;
		pgunionDesKeyDB->isWritingLocked = 0;
		pgunionDesKeyDB->readingLocks = 0;
		if ((ret = UnionLoadDesKeyDBIntoMemory()) < 0)
		{
			UnionUserErrLog("in UnionConnectDesKeyDB:: UnionLoadDesKeyDBIntoMemory!\n");
			UnionRemoveDesKeyDBInMemory();
		}
		return(ret);
	}
	else
		return(0);
}

int UnionDisconnectDesKeyDB()
{
#ifdef _useOracle
	UnionCommitAndDisconnectOnOracle();
#endif
	pgunionDesKey = NULL;
	pgunionDesKeyDB = NULL;
	return(UnionDisconnectShareModule(pgunionDesKeyDBMDL));
}

int UnionRemoveDesKeyDBInMemory()
{
	UnionDisconnectDesKeyDB();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfUnionDesKeyDB));
}

int UnionCreateDesKeyDB(int maxKeyNum)
{
	TUnionDesKeyDB		rec;
	int			ret;

	if (maxKeyNum <= 0)
	{
		UnionUserErrLog("in UnionCreateDesKeyDB:: maxKeyNum [%d] error!\n",maxKeyNum);
		return(errCodeParameter);
	}

	memset(&rec,0,sizeof(rec));
	rec.num = 0;
	rec.pkey = NULL;
	rec.maxKeyNum = maxKeyNum;
	UnionGetFullSystemDateTime(rec.lastUpdatedTime);

#ifndef _useOracle_
	return(UnionMirrorDesKeyDBIntoDisk(&rec,NULL));
#else
	return UnionCreateDesKeyDBOnOracle(&rec);
#endif
}

int UnionDeleteDesKeyDB()
{
#ifndef _useOracle_
	char			fileName[256];
	char			cmd[256];

	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfDesKeyDB(fileName);
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"rm %s",fileName);

	return(system(cmd));
#else
	return(UnionDeleteDesKeyDBOnOracle());
#endif
}

int UnionMirrorDesKeyDBIntoDiskArtifically()
{
	int	ret;

	if ((ret = UnionConnectExistingDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionMirrorDesKeyDBIntoDiskArtifically:: UnionConnectExistingDesKeyDB!\n");
		return(ret);
	}
#ifndef _useOracle_
	return(UnionMirrorDesKeyDBIntoDisk(pgunionDesKeyDB,pgunionDesKey));
#else
	return 0;
#endif
}

// Module Layer Functions
int UnionConnectExistingDesKeyDB()
{
	int	ret;

	if (UnionIsDesKeyDBConnected())
		return(0);

	if ((pgunionDesKeyDBMDL = UnionConnectExistingSharedMemoryModule(conMDLNameOfUnionDesKeyDB,
			sizeof(TUnionDesKeyDB))) == NULL)
	{
		UnionUserErrLog("in UnionConnectExistingDesKeyDB:: UnionConnectExistingSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}
	if ((pgunionDesKeyDB = (PUnionDesKeyDB)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionDesKeyDBMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectExistingDesKeyDB:: PUnionDesKeyDB!\n");
		return(errCodeSharedMemoryModule);
	}


	if ((pgunionDesKey = (PUnionDesKey)((unsigned char *)pgunionDesKeyDB + sizeof(*pgunionDesKeyDB))) == NULL)
	{
		UnionUserErrLog("in UnionConnectExistingDesKeyDB:: PUnionDesKey!\n");
		return(errCodeSharedMemoryModule);
	}
	pgunionDesKeyDB->pkey = pgunionDesKey;

	return(0);
}


int UnionMirrorDesKeyDBIntoDisk(PUnionDesKeyDB pdesKeyDB,PUnionDesKey pkey)
{
#ifndef _useOracle_
	FILE			*fp;
	char			fileName[256];

	if (pdesKeyDB == NULL)
	{
		UnionUserErrLog("in UnionMirrorDesKeyDBIntoDisk:: NullPointer!\n");
		return(errCodeParameter);
	}

	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfDesKeyDB(fileName);

	if ((fp = fopen(fileName,"wb")) == NULL)
	{
		UnionSystemErrLog("in UnionMirrorDesKeyDBIntoDisk:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	fwrite(pdesKeyDB,sizeof(*pdesKeyDB),1,fp);
	if (pdesKeyDB->num < 0)
	{
		UnionUserErrLog("in UnionMirrorDesKeyDBIntoDisk:: pdesKeyDB->num [%d] Error!\n",pdesKeyDB->num);
		fclose(fp);
		return(errCodeDesKeyDBMDL_KeyNum);
	}

	if ((pdesKeyDB->num == 0) || (pkey == NULL))
	{
		fclose(fp);
		return(0);
	}

	//fseek(fp,0,SEEK_SET);
	fwrite(pkey,sizeof(TUnionDesKey),pdesKeyDB->num,fp);

	fclose(fp);
#endif
	return(0);
}

int UnionLoadDesKeyDBIntoMemory()
{
#ifndef _useOracle_
	int	ret;
	FILE	*fp;
	char	fileName[256];
	TUnionDesKeyDB	keyDB;
	int	index;

	if ((ret = UnionWritingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionLoadDesKeyDBIntoMemory:: UnionWritingLockDesKeyTBL!\n");
		return(ret);
	}

	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfDesKeyDB(fileName);

	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionLoadDesKeyDBIntoMemory:: fopen [%s]\n",fileName);
		ret = errCodeUseOSErrCode;
		goto exitRightNow;
	}

	memset(&keyDB,0,sizeof(keyDB));
	fread(&keyDB,sizeof(keyDB),1,fp);
	pgunionDesKeyDB->maxKeyNum = keyDB.maxKeyNum;
	memcpy(pgunionDesKeyDB->lastUpdatedTime,keyDB.lastUpdatedTime,sizeof(keyDB.lastUpdatedTime));

	if ((pgunionDesKeyDB->num = keyDB.num) > pgunionDesKeyDB->maxKeyNum)
	{
		pgunionDesKeyDB->num = pgunionDesKeyDB->maxKeyNum;
	}
	if (pgunionDesKeyDB->num > 0)
	{
		fread(pgunionDesKey,sizeof(TUnionDesKey),pgunionDesKeyDB->num,fp);
		for (index = 0; index < pgunionDesKeyDB->num; index++)
		{
			(pgunionDesKey+index)->isWritingLocked = 0;
		}
	}
	ret = 0;

exitRightNow:
	UnionWritingUnlockDesKeyTBL();
	fclose(fp);
	return(ret);
#else
	return(UnionLoadDesKeyDBIntoMemoryOnOracle());
#endif
}

// 2008/5/20,王纯军
int UnionPrintAllDesKeyValueToFile(FILE *fp)
{
	int	ret;
	long	i;
	char	nameOfType[20];

	if ((ret = UnionReadingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAllDesKeyValueToFile:: UnionReadingLockDesKeyTBL!\n");
		return(ret);
	}

	if (fp == NULL)
		fp = stdout;

	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		memset(nameOfType,0,sizeof(nameOfType));
		UnionGetNameOfDesKeyType((pgunionDesKey+i)->type,nameOfType);
		fprintf(fp,"%40s %s %s\n",(pgunionDesKey+i)->fullName,(pgunionDesKey+i)->value,(pgunionDesKey+i)->checkValue);
		if (((fp == stdout) || (fp == stderr)) && (i % 23 == 0) && (i != 0))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press enter to continue or exit/quit to exit")))
				return(UnionReadingUnlockDesKeyTBL());
		}
	}
	fprintf(fp,"totolKeyNum     = [%d]\n",pgunionDesKeyDB->num);
	fprintf(fp,"maxKeyNum       = [%d]\n",pgunionDesKeyDB->maxKeyNum);
	fprintf(fp,"isWritingLocked = [%d]\n",pgunionDesKeyDB->isWritingLocked);
	fprintf(fp,"readingLocks    = [%d]\n",pgunionDesKeyDB->readingLocks);
	fprintf(fp,"lastUpdatedTime = [%s]\n",pgunionDesKeyDB->lastUpdatedTime);
	UnionReadingUnlockDesKeyTBL();
	return(0);
}

int UnionPrintDesKeyDBToFile(FILE *fp)
{
	int	ret;
	long	i;
	char	nameOfType[20];

	if ((ret = UnionReadingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyDBToFile:: UnionReadingLockDesKeyTBL!\n");
		return(ret);
	}

	if (fp == NULL)
		fp = stdout;

	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		memset(nameOfType,0,sizeof(nameOfType));
		UnionGetNameOfDesKeyType((pgunionDesKey+i)->type,nameOfType);
		fprintf(fp,"%40s %20s %3s %03d %d\n",
			(pgunionDesKey+i)->fullName,(pgunionDesKey+i)->container,
			nameOfType,
			UnionGetDesKeyBitsLength((pgunionDesKey+i)->length),
			(pgunionDesKey+i)->isWritingLocked);
		if (((fp == stdout) || (fp == stderr)) && (i % 23 == 0) && (i != 0))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press enter to continue or exit/quit to exit")))
				return(UnionReadingUnlockDesKeyTBL());
		}
	}
	fprintf(fp,"totolKeyNum     = [%d]\n",pgunionDesKeyDB->num);
	fprintf(fp,"maxKeyNum       = [%d]\n",pgunionDesKeyDB->maxKeyNum);
	fprintf(fp,"isWritingLocked = [%d]\n",pgunionDesKeyDB->isWritingLocked);
	fprintf(fp,"readingLocks    = [%d]\n",pgunionDesKeyDB->readingLocks);
	fprintf(fp,"lastUpdatedTime = [%s]\n",pgunionDesKeyDB->lastUpdatedTime);
	UnionReadingUnlockDesKeyTBL();
	return(0);
}

int UnionPrintDesKeyDB()
{
	UnionPrintDesKeyDBToFile(stdout);
	UnionPrintSharedMemoryModuleToFile(pgunionDesKeyDBMDL,stdout);
	return(0);
}

int UnionPrintAllDesKeysInKeyDBToFile(FILE *fp)
{
	return(UnionPrintDesKeyDBToFile(fp));
}

int UnionPrintStatusOfDesKeyDBToFile(FILE *fp)
{
	int	ret;

	/* 2007/12/11删除
	if ((ret = UnionReadingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintStatusOfDesKeyDBToFile:: UnionReadingLockDesKeyTBL! ret = [%d]\n",ret);
		return(ret);
	}
	*/
	// 2007/12/11增加
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintStatusOfDesKeyDBToFile:: UnionConnectDesKeyDB! ret = [%d]\n",ret);
		return(ret);
	}
	// 2007/12/11增加结束

	if (fp == NULL)
		fp = stdout;
	fprintf(fp,"totolKeyNum     = [%d]\n",pgunionDesKeyDB->num);
	fprintf(fp,"maxKeyNum       = [%d]\n",pgunionDesKeyDB->maxKeyNum);
	fprintf(fp,"isWritingLocked = [%d]\n",pgunionDesKeyDB->isWritingLocked);
	fprintf(fp,"readingLocks    = [%d]\n",pgunionDesKeyDB->readingLocks);
	fprintf(fp,"lastUpdatedTime = [%s]\n",pgunionDesKeyDB->lastUpdatedTime);
	//UnionReadingUnlockDesKeyTBL(); 2007/12/11删除
	return(UnionPrintSharedMemoryModuleToFile(pgunionDesKeyDBMDL,fp));
}

int UnionInsertDesKeyIntoKeyDB(PUnionDesKey pkey)
{
	int	ret;
	long    index;
	int	check;
	int	pos;
	char	v_tmpbuf[256];

#ifdef _KDBClient_
	if ((ret = UnionApplyKDBService(conInsertRecord,conIsDesKeyDB,(unsigned char *)pkey,sizeof(*pkey))) < 0)
	{
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: UnionApplyKDBService !\n");
		return(ret);
	}
	return(ret);
#endif
	if (!UnionIsValidDesKey(pkey))
	{
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: UnionIsValidDesKey [%s]\n",pkey->fullName);
		return(errCodeParameter);
	}
	if ((ret = UnionWritingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: UnionWritingLockDesKeyTBL !\n");
		return(ret);
	}
	pkey->useTimes = 0;
	pkey->isWritingLocked = 0;
	time(&(pkey->lastUpdateTime));
	if (pgunionDesKeyDB->num == pgunionDesKeyDB->maxKeyNum)
	{
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: DesKeyDB is full!\n");
		UnionWritingUnlockDesKeyTBL();
		return(errCodeDesKeyDBMDL_KeyDBFull);
	}
	for (index = 0; index < pgunionDesKeyDB->num; index++)
	{
		if ((check = strcmp(pkey->fullName,(pgunionDesKey + index)->fullName)) == 0)
		{
			UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: [%s] already exists!\n",pkey->fullName);
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

#ifndef _useOracle_
	if ((ret = UnionMirrorDesKeyDBIntoDisk(pgunionDesKeyDB,pgunionDesKey)) < 0)
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: UnionMirrorDesKeyDBIntoDisk!\n");
#else
	if ((ret = UnionInsertDesKeyOnOracle(pkey)) < 0)
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: UnionInsertDesKeyOnOracle!\n");

	memset(v_tmpbuf, 0, sizeof(v_tmpbuf));
	sprintf(v_tmpbuf, "%ld", pgunionDesKeyDB->num);
	if ((ret = UnionUpdateUnionParamOnOracle("NUM", v_tmpbuf, NULL)) < 0)
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: UnionUpdateUnionParamOnOracle [NUM]!\n");
	else if (ret == 0)
	{
		if ((ret = UnionInsertParamValueOnOracle("NUM", v_tmpbuf, NULL)) < 0)
			UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: UnionInsertParamValueOnOracle [NUM]!\n");
	}

	if ((ret = UnionUpdateUnionParamOnOracle("LASTUPDATEDTIME", pgunionDesKeyDB->lastUpdatedTime, NULL)) < 0)
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: UnionUpdateUnionParamOnOracle [LASTUPDATEDTIME]!\n");
	else if (ret == 0)
	{
		if ((ret = UnionInsertParamValueOnOracle("LASTUPDATEDTIME", pgunionDesKeyDB->lastUpdatedTime, NULL)) < 0)
			UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: UnionInsertParamValueOnOracle [LASTUPDATEDTIME]!\n");
	}
#endif

	UnionWritingUnlockDesKeyTBL();
	UnionLogDesKeyDBOperation(conRecordInsert,pkey);
	return(ret);
}

int UnionDeleteDesKeyFromKeyDB(char *fullName)
{
	int	ret;
	long    index;
	int	pos;
	TUnionDesKey	key;
	char	v_tmpbuf[256];

#ifdef _KDBClient_

	memset(&key,0,sizeof(key));
	strcpy(key.fullName,fullName);
	if ((ret = UnionApplyKDBService(conDeleteRecord,conIsDesKeyDB,(unsigned char *)(&key),sizeof(key))) < 0)
	{
		UnionUserErrLog("in UnionDeleteDesKeyFromKeyDB:: UnionApplyKDBService !\n");
		return(ret);
	}
	return(ret);
#endif
	if ((ret = UnionWritingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionDeleteDesKeyFromKeyDB:: UnionWritingLockDesKeyTBL !\n");
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

#ifndef _useOracle_
	if ((ret = UnionMirrorDesKeyDBIntoDisk(pgunionDesKeyDB,pgunionDesKey)) < 0)
		UnionUserErrLog("in UnionDeleteDesKeyFromKeyDB:: UnionMirrorDesKeyDBIntoDisk!\n");
#else
	if ((ret = UnionDeleteDesKeyOnOracle(fullName)) < 0)
		UnionUserErrLog("in UnionDeleteDesKeyFromKeyDB:: UnionDeleteDesKeyOnOracle!\n");

	memset(v_tmpbuf, 0, sizeof(v_tmpbuf));
	sprintf(v_tmpbuf, "%ld", pgunionDesKeyDB->num);
	if ((ret = UnionUpdateUnionParamOnOracle("NUM", v_tmpbuf, NULL)) <= 0)
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: UnionUpdateUnionParamOnOracle [NUM]!\n");

	if ((ret = UnionUpdateUnionParamOnOracle("LASTUPDATEDTIME", pgunionDesKeyDB->lastUpdatedTime, NULL)) <= 0)
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: UnionUpdateUnionParamOnOracle [LASTUPDATEDTIME]!\n");
#endif

	UnionWritingUnlockDesKeyTBL();
	UnionLogDesKeyDBOperation(conRecordDelete,&key);
	return(ret);
}

int UnionReadDesKeyFromKeyDBAnyway(PUnionDesKey pkey)
{
	int	ret;
	long	index=0;

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromKeyDBAnyway:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	if (pkey == NULL)
	{
		UnionUserErrLog("in UnionReadDesKeyFromKeyDBAnyway:: null Pointer!\n");
		return(errCodeParameter);
	}

	if ((index = UnionFindDesKeyPosInKeyDB(pkey->fullName)) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromKeyDBAnyway:: UnionFindDesKeyPosInKeyDB [%s]!\n",pkey->fullName);
		return((int)index);
	}
	if ((ret = UnionReadingLockDesKey(pgunionDesKey+index)) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromKeyDBAnyway:: UnionReadingLockDesKey [%s]!\n",pkey->fullName);
		return(ret);
	}
	memcpy(pkey,pgunionDesKey+index,sizeof(TUnionDesKey));
	UnionReadingUnlockDesKey(pgunionDesKey+index);
	return(0);
}

int UnionUseDesKeyFromKeyDB(PUnionDesKey pkey)
{
	return(UnionReadDesKeyFromKeyDB(pkey));
}

int UnionReadDesKeyFromKeyDB(PUnionDesKey pkey)
{
	int	ret;
	long	index=0;

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromKeyDBAnyway:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	if (pkey == NULL)
	{
		UnionUserErrLog("in UnionReadDesKeyFromKeyDBAnyway:: null Pointer!\n");
		return(errCodeParameter);
	}

	if ((index = UnionFindDesKeyPosInKeyDB(pkey->fullName)) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromKeyDBAnyway:: UnionFindDesKeyPosInKeyDB [%s]!\n",pkey->fullName);
		return((int)index);
	}
	if ((ret = UnionReadingLockDesKey(pgunionDesKey+index)) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromKeyDBAnyway:: UnionReadingLockDesKey [%s]!\n",pkey->fullName);
		return(ret);
	}
	if (!UnionIsDesKeyStillEffective(pgunionDesKey+index))
	{
		UnionUserErrLog("in UnionReadDesKeyFromKeyDB:: [%s] not effective!!\n",pkey->fullName);
		UnionReadingUnlockDesKey(pgunionDesKey+index);
		return(errCodeDesKeyDBMDL_KeyNotEffective);
	}
	memcpy(pkey,pgunionDesKey+index,sizeof(TUnionDesKey));
	(pgunionDesKey+index)->useTimes += 1;
	UnionReadingUnlockDesKey(pgunionDesKey+index);
	return(0);
}

int UnionUpdateDesKeyInKeyDB(PUnionDesKey pkey)
{
	if (pkey == NULL)
	{
		UnionUserErrLog("in UnionUpdateDesKeyInKeyDB:: null Pointer!\n");
		return(errCodeParameter);
	}

	return(UnionUpdateDesKeyValueInKeyDB(pkey->fullName,pkey->value,pkey->checkValue));
}

int UnionUpdateDesKeyValueInKeyDB(char *fullName,char *value,char *checkValue)
{
	int			ret;
	long			index=0;
	TUnionDesKey		key;
	char			idOfApp[40+1];
	char			varName[100];
	int			mirrorIntoFile = 1;

#ifdef _KDBClient_
	if ((fullName == NULL) || (strlen(fullName) >= sizeof(key.fullName)) || (value == NULL) ||
		(strlen(value) >= sizeof(key.value)))
	{
		UnionUserErrLog("in UnionUpdateDesKeyValueInKeyDB:: wrong parameter!\n");
		return(errCodeParameter);
	}
	memset(&key,0,sizeof(key));
	strcpy(key.fullName,fullName);
	if ((checkValue != NULL) && (strlen(checkValue) < sizeof(key.checkValue)))
		strcpy(key.checkValue,checkValue);
	if ((value != NULL) && (strlen(value) < sizeof(key.value)))
		strcpy(key.value,value);
	if ((ret = UnionApplyKDBService(conUpdateRecordValueFlds,conIsDesKeyDB,(unsigned char *)(&key),sizeof(key))) < 0)
	{
		UnionUserErrLog("in UnionUpdateDesKeyValueInKeyDB:: UnionApplyKDBService !\n");
		return(ret);
	}
	return(ret);
#endif

	if ((index = UnionFindDesKeyPosInKeyDB(fullName)) < 0)
	{
		UnionUserErrLog("in UnionUpdateDesKeyValueInKeyDB:: UnionFindDesKeyPosInKeyDB [%s]!\n",fullName);
		return((int)index);
	}

	if ((ret = UnionWritingLockDesKey(pgunionDesKey+index)) < 0)
	{
		UnionUserErrLog("in UnionUpdateDesKeyValueInKeyDB:: UnionWritingLockDesKey!\n");
		return(ret);
	}

	if ((ret = UnionUpdateDesKeyValue(pgunionDesKey+index,value,checkValue)) < 0)
	{
		UnionUserErrLog("in UnionUpdateDesKeyValueInKeyDB:: UnionUpdateDesKeyValue [%s]\n",fullName);
		UnionWritingUnlockDesKey(pgunionDesKey+index);
		return(ret);
	}
	memcpy(&key,pgunionDesKey+index,sizeof(key));

	UnionGetFullSystemDateTime(pgunionDesKeyDB->lastUpdatedTime);

	// 2008/9/27,王纯军增加
	switch ((pgunionDesKey+index)->type)
	{
		case	conZAK:
		case	conZPK:
		case	conEDK:
		case	conZEK:
			memset(idOfApp,0,sizeof(idOfApp));
			UnionGetAppIDOutOfDesKeyFullName((pgunionDesKey+index)->fullName,idOfApp);
			sprintf(varName,"noMirrorWKIntoDBOf%s",idOfApp);
			if (UnionReadIntTypeRECVar(varName) > 0)
			{
				UnionDebugLog("in UnionUpdateDesKeyValueInKeyDB:: no mirror key value of [%s] into KDB\n",(pgunionDesKey+index)->fullName);
				mirrorIntoFile = 0;
			}
			break;
		default:
			break;
	}
	// 2008/9/27,王纯军增加结束
	if (mirrorIntoFile)	// 2008/9/27,增加此句，王纯军
	{
	#ifndef _useOracle_
		if ((ret = UnionMirrorDesKeyDBIntoDisk(pgunionDesKeyDB,pgunionDesKey)) < 0)
			UnionUserErrLog("in UnionUpdateDesKeyValueInKeyDB:: UnionMirrorDesKeyDBIntoDisk!\n");
	#else
		if ((ret = UnionUpdateDesKeyOnOracle(&key)) < 0)
			UnionUserErrLog("in UnionUpdateDesKeyValueInKeyDB:: UnionUpdateDesKeyOnOracle!\n");
		if ((ret = UnionUpdateUnionParamOnOracle("LASTUPDATEDTIME", pgunionDesKeyDB->lastUpdatedTime, NULL)) <= 0)
			UnionUserErrLog("in UnionUpdateDesKeyValueInKeyDB:: UnionUpdateUnionParamOnOracle [LASTUPDATEDTIME]!\n");
	#endif
	}

	UnionWritingUnlockDesKey(pgunionDesKey+index);

	//if (UnionIsDesKeyValueDynamicUpdated(pgunionDesKey+index))	// 2007/11/29 删除
	//	return(0);
	UnionLogDesKeyDBOperation(conRecordUpdate,&key);

	return(0);
}

long UnionFindDesKeyPosInKeyDB(char *fullName)
{
	int	ret;
	long    index=0;
	//int	pos;
	long	left;
	long	right;
	int	check;

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionFindDesKeyPosInKeyDB:: UnionConnectDesKeyDB!\n");
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
	//UnionNullLog("in UnionFindDesKeyPosInKeyDB:: index = [%ld] left = [%ld] right = [%ld]\n",index,left,right);
	return(ret);
}

int UnionGetKeyNumOfKeyDB()
{
	int	ret;
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionGetKeyNumOfKeyDB:: UnionConnectDesKeyDB!\n");
		return(ret);
	}

#ifndef _useOracle_
	return(pgunionDesKeyDB->num);
#else
	return(UnionGetNumFromKeyDBOnOracle());
#endif
}

char *UnionGetDesKeyDBLastUpdatedTime()
{
	int	ret;
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionGetDesKeyDBLastUpdatedTime:: UnionConnectDesKeyDB!\n");
		return(NULL);
	}

#ifndef _useOracle_
	return(pgunionDesKeyDB->lastUpdatedTime);
#else
	return(UnionGetDesKeyDBLastUpdatedTimeOnOracle());
#endif
}

int UnionChangeSizeOfDesKeyDB(long newMaxKeyNum)
{
#ifndef _useOracle_
	int	ret;
	FILE	*fp1,*fp2;
	char	fileName1[256],fileName2[256];
	TUnionDesKeyDB	oldKeyDB,newKeyDB;
	long	copyNum;
	TUnionDesKey	desKey;
	char	cmd[512];

	memset(fileName1,0,sizeof(fileName1));
	UnionGetNameOfDesKeyDB(fileName1);

	if ((fp1 = fopen(fileName1,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionChangeSizeOfDesKeyDB:: fopen [%s]\n",fileName1);
		return(errCodeUseOSErrCode);
	}

	memset(&oldKeyDB,0,sizeof(oldKeyDB));
	fread(&oldKeyDB,sizeof(oldKeyDB),1,fp1);
	if (oldKeyDB.maxKeyNum == newMaxKeyNum)
	{
		UnionUserErrLog("in UnionChangeSizeOfDesKeyDB:: original newMaxKeyNum [%ld] = newMaxNum [%ld]\n",oldKeyDB.maxKeyNum,newMaxKeyNum);
		fclose(fp1);
		return(errCodeDesKeyDBMDL_KeyNum);
	}
	if (oldKeyDB.maxKeyNum > newMaxKeyNum)
	{
		if (oldKeyDB.num > newMaxKeyNum)
		{
			UnionUserErrLog("in UnionChangeSizeOfDesKeyDB:: real Num [%ld] > newMaxNum [%ld]\n",oldKeyDB.num,newMaxKeyNum);
			fclose(fp1);
			return(errCodeDesKeyDBMDL_KeyNum);
		}
	}
	memset(fileName2,0,sizeof(fileName2));
	sprintf(fileName2,"%s.tmp",fileName1);
	if ((fp2 = fopen(fileName2,"wb")) == NULL)
	{
		UnionSystemErrLog("in UnionChangeSizeOfDesKeyDB:: fopen [%s]\n",fileName2);
		fclose(fp1);
		return(errCodeUseOSErrCode);
	}

	memcpy(&newKeyDB,&oldKeyDB,sizeof(newKeyDB));
	newKeyDB.maxKeyNum = newMaxKeyNum;
	fwrite(&newKeyDB,sizeof(newKeyDB),1,fp2);

	for (copyNum = 0; copyNum < oldKeyDB.num; copyNum++)
	{
		fread(&desKey,sizeof(desKey),1,fp1);
		fwrite(&desKey,sizeof(desKey),1,fp2);
	}
	memset(&desKey,0,sizeof(desKey));
	for (;copyNum < newKeyDB.maxKeyNum;copyNum++)
	{
		fwrite(&desKey,sizeof(desKey),1,fp2);
	}
	fclose(fp1);
	fclose(fp2);

	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"mv %s %s",fileName2,fileName1);
	system(cmd);
	return(0);
#else
	char v_maxKeyNum[16];
	memset(v_maxKeyNum, 0, sizeof(v_maxKeyNum));
	sprintf(v_maxKeyNum, "%ld", newMaxKeyNum);
	return(UnionUpdateUnionParamOnOracle("MAXKEYNUM", v_maxKeyNum, NULL));
#endif
}

int UnionPrintDesKeyExpectedInKeyDBToFile(char *app,char *owner,char *name,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	short	appEQExpected=0,ownerEQExpected=0,nameEQExpected=0;
	char	thisApp[40+1],thisOwner[40+1],thisName[40+1];
	char	nameOfType[20];

	if ((ret = UnionReadingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyExpectedInKeyDBToFile:: UnionReadingLockDesKeyTBL!\n");
		return(ret);
	}

	if (fp == NULL)
		fp = stdout;

	if ((app != NULL) && (strlen(app) > 0))
		appEQExpected = 1;
	if ((owner != NULL) && (strlen(owner) > 0))
		ownerEQExpected = 1;
	if ((name != NULL) && (strlen(name) > 0))
		nameEQExpected = 1;

	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		memset(thisApp,0,sizeof(thisApp));
		memset(thisOwner,0,sizeof(thisOwner));
		memset(thisName,0,sizeof(thisName));
		UnionAnalysisDesKeyFullName((pgunionDesKey+i)->fullName,thisApp,thisOwner,thisName);
		if (appEQExpected)
		{
			if (strcmp(thisApp,app) != 0)
				continue;
		}
		if (ownerEQExpected)
		{
			if (strcmp(thisOwner,owner) != 0)
				continue;
		}
		if (nameEQExpected)
		{
			if (strcmp(thisName,name) != 0)
				continue;
		}
		totalNum++;
		memset(nameOfType,0,sizeof(nameOfType));
		UnionGetNameOfDesKeyType((pgunionDesKey+i)->type,nameOfType);
		fprintf(fp,"%40s %20s %3s %03d\n",
			(pgunionDesKey+i)->fullName,(pgunionDesKey+i)->container,
			nameOfType,
			UnionGetDesKeyBitsLength((pgunionDesKey+i)->length));
		if ((fp == stdout) && (totalNum % 23 == 0) && (totalNum != 0))
		{
			printf("Press any key to continue...\n");
			getchar();
			getchar();
		}
	}
	fprintf(fp,"totolNum = [%d]\n",totalNum);
	ret = 0;

exitRightNow:
	UnionReadingUnlockDesKeyTBL();
	return(ret);
}

int UnionPrintDesKeyExpectedToFile(char *app,char *owner,char *name,FILE *fp)
{
	return(UnionPrintDesKeyExpectedInKeyDBToFile(app,owner,name,fp));
}

int UnionPrintDesKeyInKeyDBOutofDateToFile(FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;

	if ((ret = UnionReadingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBOutofDateToFile:: UnionReadingLockDesKeyTBL!\n");
		return(ret);
	}
	if (fp == NULL)
		fp = stdout;

	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		if (UnionIsDesKeyStillEffective(pgunionDesKey+i))
			continue;
		totalNum++;
		fprintf(fp,"%35s %8s %05ld %05ld %05ld\n",
			(pgunionDesKey+i)->fullName,
			(pgunionDesKey+i)->activeDate,
			(pgunionDesKey+i)->maxEffectiveDays,
			(pgunionDesKey+i)->useTimes,
			(pgunionDesKey+i)->maxUseTimes);
		if ((fp == stdout) && (totalNum % 23 == 0) && (totalNum != 0))
		{
			printf("Press any key to continue...\n");
			getchar();
			getchar();
		}
	}
	fprintf(fp,"totolNum = [%d]\n",totalNum);
	ret = 0;

exitRightNow:
	UnionReadingUnlockDesKeyTBL();
	return(ret);
}

int UnionPrintDesKeyOutofDateToFile(FILE *fp)
{
	return(UnionPrintDesKeyInKeyDBOutofDateToFile(fp));
}

int UnionPrintDesKeyInKeyDBNearOutofDateToFile(long days,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	long	tmpDays;

	if ((ret = UnionReadingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBNearOutofDateToFile:: UnionReadingLockDesKeyTBL!\n");
		return(ret);
	}
	if (fp == NULL)
		fp = stdout;
	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		if ((tmpDays = UnionDecideDaysBeforeToday((pgunionDesKey+i)->activeDate) + days) < (pgunionDesKey+i)->maxEffectiveDays)
			continue;
		totalNum++;
		fprintf(fp,"%35s %8s %05ld\n",(pgunionDesKey+i)->fullName,(pgunionDesKey+i)->activeDate,(pgunionDesKey+i)->maxEffectiveDays);
		if ((fp == stdout) && (totalNum % 23 == 0) && (totalNum != 0))
		{
			printf("Press any key to continue...\n");
			getchar();
			getchar();
		}
	}
	fprintf(fp,"totolNum = [%d]\n",totalNum);
	ret = 0;

exitRightNow:
	UnionReadingUnlockDesKeyTBL();
	return(ret);
}

int UnionPrintDesKeyNearOutofDateToFile(long days,FILE *fp)
{
	return(UnionPrintDesKeyInKeyDBNearOutofDateToFile(days,fp));
}

int UnionPrintDesKeyInKeyDBNearOutofMaxUseTimesToFile(long times,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;

	if ((ret = UnionReadingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBNearOutofMaxUseTimesToFile:: UnionReadingLockDesKeyTBL!\n");
		return(ret);
	}
	if (fp == NULL)
		fp = stdout;
	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		if ( (pgunionDesKey+i)->useTimes + times < (pgunionDesKey+i)->maxUseTimes)
			continue;
		totalNum++;
		fprintf(fp,"%35s %05ld %05ld\n",(pgunionDesKey+i)->fullName,(pgunionDesKey+i)->useTimes,(pgunionDesKey+i)->maxUseTimes);
		if ((fp == stdout) && (totalNum % 23 == 0) && (totalNum != 0))
		{
			printf("Press any key to continue...\n");
			getchar();
			getchar();
		}
	}
	fprintf(fp,"totolNum = [%d]\n",totalNum);
	ret = 0;

exitRightNow:
	UnionReadingUnlockDesKeyTBL();
	return(ret);
}

int UnionPrintDesKeyNearOutofMaxUseTimesToFile(long times,FILE *fp)
{
	return(UnionPrintDesKeyInKeyDBNearOutofMaxUseTimesToFile(times,fp));
}



// 2004/11/20, Added by Wolfgang Wang
int UnionSynchronizeSpecifiedDesKeyToBackuper(char *fullName)
{
	long	index=0;
	int	ret;

	if ((index = UnionFindDesKeyPosInKeyDB(fullName)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeSpecifiedDesKeyToBackuper:: UnionFindDesKeyPosInKeyDB! [%s]\n",fullName);
		return((int)index);
	}

	if ((ret = UnionWritingLockDesKey(pgunionDesKey+index)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeSpecifiedDesKeyToBackuper:: UnionWritingLockDesKey !\n");
		return(ret);
	}

	if ((ret = UnionSynchronizeDesKeyDBOperation(conRecordUpdate,pgunionDesKey+index)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeSpecifiedDesKeyToBackuper:: UnionSynchronizeDesKeyDBOperation !\n");
	}

	UnionWritingUnlockDesKey(pgunionDesKey+index);

	return(ret);
}

// 2004/11/20, Added by Wolfgang Wang
long UnionSynchronizeAllDesKeyToBackuper()
{
	int	ret;
	long	i;
	long	synchronizedNum = 0;

	if ((ret = UnionReadingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeAllDesKeyToBackuper:: UnionReadingLockDesKeyTBL !\n");
		return(ret);
	}

	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		if ((ret = UnionSynchronizeDesKeyDBOperation(conRecordUpdate,pgunionDesKey+i)) < 0)
		{
			UnionUserErrLog("synchronize Err::[%s]\n",(pgunionDesKey+i)->fullName);
			printf("synchronize Err::[%s]\n",(pgunionDesKey+i)->fullName);
		}
		else
		{
			UnionAuditLog("synchronize OK ::[%s]\n",(pgunionDesKey+i)->fullName);
			printf("synchronize OK ::[%s]\n",(pgunionDesKey+i)->fullName);
			synchronizedNum++;
		}
	}
	ret = synchronizedNum;

exitRightNow:
	UnionReadingUnlockDesKeyTBL();
	return(ret);
}

// 2004/11/20, Added by Wolfgang Wang
long UnionSynchronizeDesKeyOfSpecifiedAppToBackuper(char *appName)
{
	int	ret;
	long	i;
	char	thisApp[40+1],thisOwner[40+1],thisName[40+1];
	long	synchronizedNum = 0;

	if ((ret = UnionReadingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeDesKeyOfSpecifiedAppToBackuper:: UnionReadingLockDesKeyTBL !\n");
		return(ret);
	}

	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		memset(thisApp,0,sizeof(thisApp));
		UnionAnalysisDesKeyFullName((pgunionDesKey+i)->fullName,thisApp,thisOwner,thisName);
		if (strcmp(appName,thisApp) != 0)
			continue;
		if ((ret = UnionSynchronizeDesKeyDBOperation(conRecordUpdate,pgunionDesKey+i)) < 0)
		{
			UnionUserErrLog("synchronize Err::[%s]\n",(pgunionDesKey+i)->fullName);
			printf("synchronize Err::[%s]\n",(pgunionDesKey+i)->fullName);
		}
		else
		{
			UnionAuditLog("synchronize OK ::[%s]\n",(pgunionDesKey+i)->fullName);
			printf("synchronize OK ::[%s]\n",(pgunionDesKey+i)->fullName);
			synchronizedNum++;
		}
	}
	ret = synchronizedNum;

exitRightNow:
	UnionReadingUnlockDesKeyTBL();
	return(ret);
}

// added by Wolfgang Wang, 2004/11/20
// 2007/11/29修改，增加了对conCompareKeyValue的处理
int UnionDesKeyDBOperation(TUnionKeyDBOperation operation,PUnionDesKey pdesKey)
{
	int		ret = -1;
	TUnionDesKey	desKey;

	switch (operation)
	{
		case	conInsertRecord:
			if ((ret = UnionInsertDesKeyIntoKeyDB(pdesKey)) < 0)
				UnionUserErrLog("in UnionDesKeyDBOperation:: UnionInsertDesKeyIntoKeyDB [%s]\n",pdesKey->fullName);
			else
				UnionSuccessLog("in UnionDesKeyDBOperation:: UnionInsertDesKeyIntoKeyDB [%s] OK\n",pdesKey->fullName);
			break;
		case	conUpdateRecord:
		case	conUpdateRecordAllFlds:
			if (UnionFindDesKeyPosInKeyDB(pdesKey->fullName) < 0)
			{
				if ((ret = UnionInsertDesKeyIntoKeyDB(pdesKey)) < 0)
					UnionUserErrLog("in UnionDesKeyDBOperation:: UnionInsertDesKeyIntoKeyDB [%s]\n",pdesKey->fullName);
				else
					UnionSuccessLog("in UnionDesKeyDBOperation:: UnionInsertDesKeyIntoKeyDB [%s] OK\n",pdesKey->fullName);
			}
			else
			{
				if ((ret = UnionUpdateAllDesKeyFieldsInKeyDB(pdesKey)) < 0)
					UnionUserErrLog("in UnionDesKeyDBOperation:: UnionUpdateAllDesKeyFieldsInKeyDB [%s]\n",pdesKey->fullName);
				else
					UnionSuccessLog("in UnionDesKeyDBOperation:: UnionUpdateAllDesKeyFieldsInKeyDB [%s] OK\n",pdesKey->fullName);
			}
			break;
		case	conDeleteRecord:
			if ((ret = UnionDeleteDesKeyFromKeyDB(pdesKey->fullName)) < 0)
				UnionUserErrLog("in UnionDesKeyDBOperation:: UnionDeleteDesKeyFromKeyDB [%s]\n",pdesKey->fullName);
			else
				UnionSuccessLog("in UnionDesKeyDBOperation:: UnionDeleteDesKeyFromKeyDB [%s] OK\n",pdesKey->fullName);
			break;
		case	conUpdateRecordValueFlds:
			if ((ret = UnionUpdateDesKeyValueInKeyDB(pdesKey->fullName,pdesKey->value,pdesKey->checkValue)) < 0)
				UnionUserErrLog("in UnionDesKeyDBOperation:: UnionUpdateDesKeyValueInKeyDB [%s]\n",pdesKey->fullName);
			else
				UnionSuccessLog("in UnionDesKeyDBOperation:: UnionUpdateDesKeyValueInKeyDB [%s] OK\n",pdesKey->fullName);
			break;
		case	conUpdateRecordAttrFlds:
			if ((ret = UnionUpdateDesKeyAttrInKeyDB(pdesKey)) < 0)
				UnionUserErrLog("in UnionDesKeyDBOperation:: UnionUpdateDesKeyAttrInKeyDB [%s]\n",pdesKey->fullName);
			else
				UnionSuccessLog("in UnionDesKeyDBOperation:: UnionUpdateDesKeyAttrInKeyDB [%s] OK\n",pdesKey->fullName);
			break;
		case	conCompareKeyValue:	// 2007/11/29增加了该功能
			memset(&desKey,0,sizeof(desKey));
			strcpy(desKey.fullName,pdesKey->fullName);
			if ((ret = UnionReadDesKeyFromKeyDBAnyway(&desKey)) < 0)
			{
				UnionUserErrLog("in UnionDesKeyDBOperation:: UnionReadDesKeyFromKeyDBAnyway [%s]\n",desKey.fullName);
				break;
			}
			if (strcmp(desKey.value,pdesKey->value) != 0)
				ret = errCodeDesKeyDBMDL_KeyValueNotSameAsExpected;
			else
				ret = 0;
			break;
		default:
			UnionUserErrLog("in UnionDesKeyDBOperation::Unknown operation! [%02d]\n",operation);
			break;
	}
	return(ret);
}

// Wolfgang Wang, 2004/11/25
int UnionUpdateDesKeyAttrInKeyDB(PUnionDesKey pkey)
{
	int	ret;
	long	index=0;
	TUnionDesKey	desKey;

#ifdef _KDBClient_
	if ((ret = UnionApplyKDBService(conUpdateRecordAttrFlds,conIsDesKeyDB,(unsigned char *)(pkey),sizeof(*pkey))) < 0)
	{
		UnionUserErrLog("in UnionUpdateDesKeyAttrInKeyDB:: UnionApplyKDBService !\n");
		return(ret);
	}
	return(ret);
#endif

	if (!UnionIsValidDesKey(pkey))
	{
		UnionUserErrLog("in UnionUpdateDesKeyAttrInKeyDB:: UnionIsValidDesKey [%s]\n",pkey->fullName);
		return(errCodeParameter);
	}
	memset(&desKey,0,sizeof(desKey));
	memcpy(&desKey,pkey,sizeof(desKey));
	memset(desKey.value,0,sizeof(desKey.value));

	if ((index = UnionFindDesKeyPosInKeyDB(pkey->fullName)) < 0)
	{
		UnionUserErrLog("in UnionUpdateDesKeyAttrInKeyDB:: UnionFindDesKeyPosInKeyDB [%s]!\n",pkey->fullName);
		return((int)index);
	}

	if ((ret = UnionWritingLockDesKey(pgunionDesKey+index)) < 0)
	{
		UnionUserErrLog("in UnionUpdateDesKeyAttrInKeyDB:: UnionWritingLockDesKey !\n");
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

#ifndef _useOracle_
	if ((ret = UnionMirrorDesKeyDBIntoDisk(pgunionDesKeyDB,pgunionDesKey)) < 0)
		UnionUserErrLog("in UnionUpdateDesKeyAttrInKeyDB:: UnionMirrorDesKeyDBIntoDisk!\n");
#else
	if ((ret = UnionUpdateDesKeyOnOracle(&desKey)) < 0)
		UnionUserErrLog("in UnionUpdateDesKeyAttrInKeyDB:: UnionUpdateDesKeyOnOracle!\n");
	if ((ret = UnionUpdateUnionParamOnOracle("LASTUPDATEDTIME", pgunionDesKeyDB->lastUpdatedTime, NULL)) <= 0)
		UnionUserErrLog("in UnionUpdateDesKeyAttrInKeyDB:: UnionUpdateUnionParamOnOracle [LASTUPDATEDTIME]!\n");
#endif

	//UnionWritingUnlockDesKeyTBL();
	UnionWritingUnlockDesKey(pgunionDesKey+index);

	UnionLogDesKeyDBOperation(conRecordUpdate,&desKey);

	return(0);
}

// Wolfgang Wang, 2004/11/25
int UnionUpdateAllDesKeyFieldsInKeyDB(PUnionDesKey pkey)
{
	int		ret;
	long		index=0;

#ifdef _KDBClient_
	if ((ret = UnionApplyKDBService(conUpdateRecordAllFlds,conIsDesKeyDB,(unsigned char *)(pkey),sizeof(*pkey))) < 0)
	{
		UnionUserErrLog("in UnionUpdateAllDesKeyFieldsInKeyDB:: UnionApplyKDBService !\n");
		return(ret);
	}
	return(ret);
#endif

	if (!UnionIsValidDesKey(pkey))
	{
		UnionUserErrLog("in UnionUpdateAllDesKeyFieldsInKeyDB:: UnionIsValidDesKey [%s]\n",pkey->fullName);
		return(errCodeParameter);
	}

	if ((index = UnionFindDesKeyPosInKeyDB(pkey->fullName)) < 0)
	{
		UnionUserErrLog("in UnionUpdateAllDesKeyFieldsInKeyDB:: UnionFindDesKeyPosInKeyDB [%s]!\n",pkey->fullName);
		return((int)index);
	}

	if ((ret = UnionWritingLockDesKey(pgunionDesKey+index)) < 0)
	{
		UnionUserErrLog("in UnionUpdateAllDesKeyFieldsInKeyDB:: UnionWritingLockDesKey !\n");
		return(ret);
	}

	memcpy(pgunionDesKey+index,pkey,sizeof(TUnionDesKey));

	// UnionLog("in UnionUpdateAllDesKeyFieldsInKeyDB: (pgunionDesKey+index)->isWritingLocked is: [%d].\n", (pgunionDesKey+index)->isWritingLocked);

	UnionGetFullSystemDateTime(pgunionDesKeyDB->lastUpdatedTime);

#ifdef _useOracle_
	if ((ret = UnionMirrorDesKeyDBIntoDisk(pgunionDesKeyDB,pgunionDesKey)) < 0)
		UnionUserErrLog("in UnionUpdateAllDesKeyFieldsInKeyDB:: UnionMirrorDesKeyDBIntoDisk!\n");
#else
	if ((ret = UnionUpdateUnionParamOnOracle("LASTUPDATEDTIME", pgunionDesKeyDB->lastUpdatedTime, NULL)) <= 0)
	{
		UnionUserErrLog("in UnionUpdateAllDesKeyFieldsInKeyDB:: UnionUpdateUnionParamOnOracle!\n");
	}
	if ((ret = UnionUpdateDesKeyOnOracle(pkey)) <= 0)
	{
		UnionUserErrLog("in UnionUpdateAllDesKeyFieldsInKeyDB:: UnionUpdateDesKeyOnOracle!\n");
	}
#endif

	UnionWritingUnlockDesKey(pgunionDesKey+index);

	UnionLogDesKeyDBOperation(conRecordUpdate,pkey);

	return(0);
}


// Wolfgang Wang, 2004/11/25
int UnionExistDesKeyInKeyDB(char *fullName)
{
	if (UnionFindDesKeyPosInKeyDB(fullName) < 0)
		return(0);
	else
		return(1);
}

int UnionPrintDesKeyOfContainerInKeyDBToFile(char *container,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	char	nameOfType[40];

	if ((ret = UnionReadingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyOfContainerInKeyDBToFile:: UnionReadingLockDesKeyTBL!\n");
		return(ret);
	}
	if (fp == NULL)
		fp = stdout;

	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		if (strcmp(container,(pgunionDesKey+i)->container) != 0)
			continue;
		totalNum++;
		memset(nameOfType,0,sizeof(nameOfType));
		UnionGetNameOfDesKeyType((pgunionDesKey+i)->type,nameOfType);
		fprintf(fp,"%40s %20s %3s %03d\n",
			(pgunionDesKey+i)->fullName,(pgunionDesKey+i)->container,
			nameOfType,
			UnionGetDesKeyBitsLength((pgunionDesKey+i)->length));
		if ((fp == stdout) && (totalNum % 23 == 0) && (totalNum != 0))
		{
			printf("Press any key to continue...\n");
			getchar();
			getchar();
		}
	}
	fprintf(fp,"totolNum = [%d]\n",totalNum);
	ret = 0;

	UnionReadingUnlockDesKeyTBL();
	return(ret);
}

int UnionPrintDesKeyInKeyDBUsedDaysToFile(long days,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;

	if ((ret = UnionReadingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBUsedDaysToFile:: UnionReadingLockDesKeyTBL!\n");
		return(ret);
	}
	if (fp == NULL)
		fp = stdout;

	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		if (UnionDecideDaysBeforeToday((pgunionDesKey+i)->activeDate) < days)
			continue;
		totalNum++;
		fprintf(fp,"%40s %20s %8s %05ld\n",(pgunionDesKey+i)->fullName,(pgunionDesKey+i)->container,(pgunionDesKey+i)->activeDate,(pgunionDesKey+i)->maxEffectiveDays);
		if (((fp == stdout) || (fp == stderr)) && (totalNum % 23 == 0) && (totalNum != 0))
		{
			printf("Press any key to continue...\n");
			getchar();
			getchar();
		}
	}
	fprintf(fp,"totolNum = [%d]\n",totalNum);

	UnionReadingUnlockDesKeyTBL();
	return(0);
}

int UnionPrintDesKeyInKeyDBUsedTimesToFile(long times,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;

	if ((ret = UnionReadingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBUsedTimesToFile:: UnionReadingLockDesKeyTBL!\n");
		return(ret);
	}
	if (fp == NULL)
		fp = stdout;

	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		if (((pgunionDesKey+i)->useTimes < times) || ((pgunionDesKey+i)->useTimes < 0))
			continue;
		totalNum++;
		fprintf(fp,"%40s %20s %12ld %12ld\n",(pgunionDesKey+i)->fullName,(pgunionDesKey+i)->container,(pgunionDesKey+i)->useTimes,(pgunionDesKey+i)->maxUseTimes);
		if (((fp == stdout) || (fp == stderr)) && (totalNum % 23 == 0) && (totalNum != 0))
		{
			printf("Press any key to continue...\n");
			getchar();
			getchar();
		}
	}
	fprintf(fp,"totolNum = [%d]\n",totalNum);
	UnionReadingUnlockDesKeyTBL();
	return(0);
}


// 从密钥库中获取密钥容器
int UnionGetContainerOfSpecKey(char *keyName,char *container)
{
	TUnionDesKey	desKey;
	int		ret;

	memset(&desKey,0,sizeof(desKey));
	if (strlen(keyName) >= sizeof(desKey.fullName))
	{
		UnionUserErrLog("in UnionGetContainerOfSpecKey:: keyName [%s] error!\n",keyName);
		return(errCodeParameter);
	}
	strcpy(desKey.fullName,keyName);
	if ((ret = UnionReadDesKeyFromKeyDBAnyway(&desKey)) < 0)
	{
		UnionUserErrLog("in UnionGetContainerOfSpecKey:: UnionReadDesKeyFromKeyDBAnyway [%s] error!\n",keyName);
		return(ret);
	}
	strcpy(container,desKey.container);
	return(0);
}
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

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionUnlockAllLockedDesKeyInDesKeyDB: UnionConnectDesKeyDB!\n");
		return(ret);
	}

	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		if (!(pgunionDesKey+i)->isWritingLocked)
			continue;
		(pgunionDesKey+i)->isWritingLocked = 0;
		totalNum++;
	}
	if (totalNum > 0)
		UnionMirrorDesKeyDBIntoDiskArtifically();
	return(totalNum);
}

int UnionUnlockDesKeyInDesKeyDB(char *keyName)
{
	long	index;

	if ((index = UnionFindDesKeyPosInKeyDB(keyName)) < 0)
	{
		UnionUserErrLog("in UnionUnlockDesKeyInDesKeyDB:: UnionFindDesKeyPosInKeyDB [%s]\n",keyName);
		return((int)index);
	}
	if ((pgunionDesKey+index)->isWritingLocked)
	{
		(pgunionDesKey+index)->isWritingLocked = 0;
		UnionMirrorDesKeyDBIntoDiskArtifically();
		return(0);
	}
	else
		return(errCodeDesKeyDBMDL_KeyNotLocked);
}

// 2008/05/15增加
int UnionLockDesKeyInDesKeyDB(char *keyName)
{
	long	index;

	if ((index = UnionFindDesKeyPosInKeyDB(keyName)) < 0)
	{
		UnionUserErrLog("in UnionUnlockDesKeyInDesKeyDB:: UnionFindDesKeyPosInKeyDB [%s]\n",keyName);
		return((int)index);
	}
	if (!((pgunionDesKey+index)->isWritingLocked))
		(pgunionDesKey+index)->isWritingLocked = 1;
	return(0);
}

int UnionPrintAllLockedDesKeyInDesKeyDBToFile(FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;

	if ((ret = UnionReadingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAllLockedDesKeyInDesKeyDBToFile:: UnionReadingLockDesKeyTBL!\n");
		return(ret);
	}
	if (fp == NULL)
		fp = stdout;

	fprintf(fp,"Locked key::\n");
	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		if (!(pgunionDesKey+i)->isWritingLocked)
			continue;
		totalNum++;
		fprintf(fp,"%40s\n",(pgunionDesKey+i)->fullName);
		if ((fp == stdout) && (totalNum % 23 == 0) && (totalNum != 0))
		{
			printf("Press any key to continue...\n");
			getchar();
			getchar();
		}
	}
	fprintf(fp,"totolNum = [%d]\n",totalNum);
	ret = 0;

	UnionReadingUnlockDesKeyTBL();
	return(ret);
}

int UnionPrintDesKeyGroupInKeyDBToFile(PUnionDesKeyQueryCondition pcondition,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	char	thisApp[40+1],thisOwner[40+1],thisName[40+1];
	char	nameOfType[20];
	long	usedDays;

	if ((ret = UnionReadingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyGroupInKeyDBToFile:: UnionReadingLockDesKeyTBL!\n");
		return(ret);
	}

	if (fp == NULL)
		fp = stdout;

	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		memset(thisApp,0,sizeof(thisApp));
		memset(thisOwner,0,sizeof(thisOwner));
		memset(thisName,0,sizeof(thisName));
		UnionAnalysisDesKeyFullName((pgunionDesKey+i)->fullName,thisApp,thisOwner,thisName);
		if (pcondition == NULL)
			goto writeNow;
		if ((pcondition->app != NULL) && (strlen(pcondition->app) > 0))
		{
			if (strcmp(thisApp,pcondition->app) != 0)
				continue;
		}
		if ((pcondition->owner != NULL) && (strlen(pcondition->owner) > 0))
		{
			if (strcmp(thisOwner,pcondition->owner) != 0)
				continue;
		}
		if ((pcondition->name != NULL) && (strlen(pcondition->name) > 0))
		{
			if (strcmp(thisName,pcondition->name) != 0)
				continue;
		}
		if ((pcondition->activeDate != NULL) && (strlen(pcondition->activeDate) > 0))
		{
			if (strcmp((pgunionDesKey+i)->activeDate,pcondition->activeDate) != 0)
				continue;
		}
		if ((pcondition->container != NULL) && (strlen(pcondition->container) > 0))
		{
			if (strcmp((pgunionDesKey+i)->container,pcondition->container) != 0)
				continue;
		}
		if (pcondition->checkWritingLocked)
		{
			if (!(pgunionDesKey+i)->isWritingLocked)
				continue;
		}
		if (pcondition->checkOutdate > 0)	// 检查是否过期
		{
			if (UnionIsDesKeyStillEffective(pgunionDesKey+i))
				continue;
		}
		else if (pcondition->checkOutdate < 0)	// 检查快过期密钥
		{
			if (abs(pcondition->usedTimes) > 0)	// 检查使用次数超过条件规定的
			{
				if ((pgunionDesKey+i)->useTimes + abs(pcondition->usedTimes) < (pgunionDesKey+i)->maxUseTimes)
					continue;
			}
			if (abs(pcondition->usedDays) > 0)	// 检查使用天数超过条件规定的
			{
				usedDays = UnionDecideDaysBeforeToday((pgunionDesKey+i)->activeDate);
				if (usedDays + abs(pcondition->usedDays) < (pgunionDesKey+i)->maxEffectiveDays)
					continue;
			}
		}
		else	// 检查使用条件
		{
			if (pcondition->usedTimes > 0)	// 检查使用次数超过条件规定的
			{
				if ((pgunionDesKey+i)->useTimes < pcondition->usedTimes)
					continue;
			}
			else if (pcondition->usedTimes < 0)	// 检查使用次数不足条件规定的
			{
				if ((pgunionDesKey+i)->useTimes > abs(pcondition->usedTimes))
					continue;
			}
			if (pcondition->usedDays > 0)	// 检查使用天数超过条件规定的
			{
				usedDays = UnionDecideDaysBeforeToday((pgunionDesKey+i)->activeDate);
				if (usedDays < pcondition->usedDays)
					continue;
			}
			else if (pcondition->usedDays < 0)	// 检查使用天数不足条件规定的
			{
				usedDays = UnionDecideDaysBeforeToday((pgunionDesKey+i)->activeDate);
				if (usedDays > abs(pcondition->usedDays))
					continue;
			}
		}
		if (UnionIsValidDesKeyLength(pcondition->length))
		{
			if ((pgunionDesKey+i)->length != pcondition->length)
				continue;
		}
		if (UnionIsValidDesKeyType(pcondition->type))
		{
			if ((pgunionDesKey+i)->type != pcondition->type)
				continue;
		}
writeNow:
		totalNum++;
		memset(nameOfType,0,sizeof(nameOfType));
		UnionGetNameOfDesKeyType((pgunionDesKey+i)->type,nameOfType);
		fprintf(fp,"%40s %20s %3s %03d\n",
			(pgunionDesKey+i)->fullName,(pgunionDesKey+i)->container,
			nameOfType,
			UnionGetDesKeyBitsLength((pgunionDesKey+i)->length));
		if ((fp == stdout) && (totalNum % 23 == 0) && (totalNum != 0))
		{
			printf("Press any key to continue...\n");
			getchar();
			getchar();
		}
	}
	fprintf(fp,"totolNum = [%d]\n",totalNum);
	ret = 0;

exitRightNow:
	UnionReadingUnlockDesKeyTBL();
	return(ret);
}
int UnionPrintDesKeyGroupInKeyDBToSpecFile(PUnionDesKeyQueryCondition pcondition,char *fileName)
{
	FILE	*fp;
	int	ret;

	if ((fileName == NULL) || (strlen(fileName) == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintDesKeyGroupInKeyDBToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	ret = UnionPrintDesKeyGroupInKeyDBToFile(pcondition,fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}
int UnionDeleteAllDesKeyOfApp(char *app)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	char	thisApp[40+1],thisOwner[40+1],thisName[40+1];
	//int	keyOfAppExists = 0;
	char	fullName[100+1];

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllDesKeyOfApp:: UnionConnectDesKeyDB !\n");
		return(ret);
	}

	for (i = 0;;i++)
	{
		if (i >= pgunionDesKeyDB->num)
			break;
		memset(thisApp,0,sizeof(thisApp));
		memset(thisOwner,0,sizeof(thisOwner));
		memset(thisName,0,sizeof(thisName));
		UnionAnalysisDesKeyFullName((pgunionDesKey+i)->fullName,thisApp,thisOwner,thisName);
		if (strcmp(thisApp,app) != 0)
			continue;
		strcpy(fullName,(pgunionDesKey+i)->fullName);
		if ((ret = UnionDeleteDesKeyFromKeyDB(fullName)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllDesKeyOfApp:: UnionDeleteDesKeyFromKeyDB [%s]\n",fullName);
			continue;
		}
		UnionLog("in UnionDeleteAllDesKeyOfApp:: delete [%s] ok!\n",fullName);
		totalNum++;
		i = 0;
	}
	return(totalNum);
}

// 2007/11/29, Added by Wolfgang Wang
long UnionCompareAllDesKeyValueInKeyDBWithBrothers()
{
	int	ret;
	long	i;
	long	checkErrNum = 0;

	if ((ret = UnionReadingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionCompareAllDesKeyValueInKeyDBWithBrothers:: UnionReadingLockDesKeyTBL !\n");
		return(ret);
	}

	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		if ((ret = UnionCompareDesKeyValueWithBrothers((pgunionDesKey+i)->fullName,(pgunionDesKey+i)->value)) < 0)
		{
			UnionUserErrLog("check Err::[%s]\n",(pgunionDesKey+i)->fullName);
			printf("check Err::[%s]\n",(pgunionDesKey+i)->fullName);
			checkErrNum++;
		}
		else
		{
			UnionAuditLog("check OK ::[%s]\n",(pgunionDesKey+i)->fullName);
			printf("check OK ::[%s]\n",(pgunionDesKey+i)->fullName);
		}
	}
	ret = checkErrNum;

	UnionReadingUnlockDesKeyTBL();
	return(ret);
}

// 2007/12/11增加，自动向备份服务器同步不一致的密钥
int UnionAutoSynchronizeDesKeyToBrothers()
{
	int		keyNum = 0;
	char		ipAddr[20][15+1];
	int		unsameNum;
	int		index;
	int		failNum = 0,successNum = 0;
	int		ret;
	PUnionDesKey	pdesKey;

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionAutoSynchronizeDesKeyToBrothers:: UnionConnectDesKeyDB!\n");
		return(ret);
	}

	for (successNum = 0,failNum = 0;keyNum < pgunionDesKeyDB->num; keyNum++,index = 0)
	{
		pdesKey = pgunionDesKey+keyNum;
		if ((unsameNum = UnionCountAllUnsameDesKeyBrothers(pdesKey->fullName,pdesKey->value,ipAddr,20)) <= 0)
			continue;
		for (index = 0;	(index < unsameNum) && (index < 20); index++)
		{
			if ((ret = UnionSynchronizeKeyDBOperationToSpecBrother(ipAddr[index],conUpdateRecord,conIsDesKeyDB,
						(unsigned char *)pdesKey,sizeof(*pdesKey))) < 0)
			{
				UnionUserErrLog("in UnionAutoSynchronizeDesKeyToBrothers:: UnionSynchronizeKeyDBOperationToSpecBrother! [%s] [%s] ret = [%d]\n",ipAddr[index],pdesKey->fullName,ret);
				failNum++;
			}
			else
				successNum++;
		}
	}
	if (successNum > 0)
		UnionAuditLog("in UnionAutoSynchronizeDesKeyToBrothers:: [%d] keys synchronized successfully!\n",successNum);
	if (failNum > 0)
		UnionAuditLog("in UnionAutoSynchronizeDesKeyToBrothers:: [%d] keys synchronized failure!\n",failNum);
	return(successNum);
}

// 2008/7/18,王纯军
int UnionPrintAllDesKeyToRecStrFile(FILE *fp)
{
	int	ret;
	long	i;
	int	realNum = 0;

	if ((ret = UnionReadingLockDesKeyTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAllDesKeyToRecStrFile:: UnionReadingLockDesKeyTBL!\n");
		return(ret);
	}

	if (fp == NULL)
		fp = stdout;

	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		if ((ret = UnionPrintDesKeyToRecStrTxtFile(pgunionDesKey+i,fp)) < 0)
		{
			UnionUserErrLog("in UnionPrintAllDesKeyToRecStrFile:: UnionPrintDesKeyToRecStrTxtFile [%s]\n",(pgunionDesKey+i)->fullName);
			printf("output [%s] failure!\n",(pgunionDesKey+i)->fullName);
			continue;
		}
		UnionLog("in UnionPrintAllDesKeyToRecStrFile:: UnionPrintDesKeyToRecStrTxtFile [%s] OK\n",(pgunionDesKey+i)->fullName);
		// printf("output [%s] ok!\n",(pgunionDesKey+i)->fullName);
		realNum++;
	}
	UnionReadingUnlockDesKeyTBL();
	return(realNum);
}

// 2008/7/18,王纯军
int UnionOutputAllDesKeyToRecStrFile(char *fileName)
{
	FILE	*fp;
	int	realNum = 0;
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionOutputAllDesKeyToRecStrFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	realNum = UnionPrintAllDesKeyToRecStrFile(fp);
	fflush(fp);
	fclose(fp);
	return(realNum);
}

// 2008/7/18,王纯军
int UnionInputAllDesKeyFromRecStrFile(char *fileName)
{
	FILE		*fp;
	int		realNum = 0;
	char		recStr[4096+1];
	int		len,ret;
	TUnionDesKey	desKey;

	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionInputAllDesKeyFromRecStrFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(fp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((len = UnionReadOneLineFromTxtStr(fp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(&desKey,0,sizeof(desKey));
		if ((ret = UnionReadDesKeyFromRecStr(recStr,len,&desKey)) < 0)
		{
			UnionUserErrLog("in UnionInputAllDesKeyFromRecStrFile:: UnionReadDesKeyFromRecStr [%s]\n",recStr);
			// printf("UnionReadDesKeyFromRecStr [%s] error\n",recStr);
			continue;
		}
		if ((ret = UnionInsertDesKeyIntoKeyDB(&desKey)) < 0)
		{
			UnionUserErrLog("in UnionInputAllDesKeyFromRecStrFile:: UnionInsertDesKeyIntoKeyDB [%s]\n",desKey.fullName);
			// printf("UnionInsertDesKeyIntoKeyDB [%s] failure\n",desKey.fullName);
			continue;
		}
		UnionLog("in UnionInputAllDesKeyFromRecStrFile:: UnionInsertDesKeyIntoKeyDB [%s] ok!\n",desKey.fullName);
		// printf("UnionInsertDesKeyIntoKeyDB [%s] ok!\n",desKey.fullName);
		realNum++;
	}
	fclose(fp);
	return(realNum);
}
