//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2004/11/11

// 20060822在，20041125基础上升级

// 2006/8/14，在unionPKDB3.0.20041125.c基础上升级为本程序
/*
// 2006/08/14增加了函数
// 申请锁表
int UnionWritingLockPKTBL();
// 解除锁表
int UnionWritingUnlockPKTBL();
// 申请写锁住记录
int UnionWritingLockPK(char *keyName);
// 申请解除写锁住记录
int UnionWritingUnlockPK(char *keyName);
*/

#define _UnionLogMDL_3_x_

#ifndef _UnionPK_3_x_
#define _UnionPK_3_x_
#endif

#ifndef _UnionPK_4_x_
#define _UnionPK_4_x_
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
//#include <signal.h>
//#include <sys/ipc.h>
//#include <sys/msg.h>
//#include <sys/shm.h>

#include "unionModule.h"
#include "UnionStr.h"

#include "unionPKDB.h"
#include "unionPKDBLog.h"
#include "unionErrCode.h"
#include "UnionLog.h"

PUnionSharedMemoryModule	pgunionPKDBMDL = NULL;
PUnionPKDB			pgunionPKDB = NULL;
PUnionPK			pgunionPK = NULL;

// added by wolfgang wang,20070611
int UnionResetReadingLocksOfPKDB()
{
	int	ret;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionResetReadingLocksOfPKDB:: UnionConnectPKDB!\n");
		return(ret);
	}
	pgunionPKDB->readingLocks = 0;
	return(0);
}

int UnionResetWritingLocksOfPKDB()
{
	int	ret;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionResetWritingLocksOfPKDB:: UnionConnectPKDB!\n");
		return(ret);
	}
	pgunionPKDB->isWritingLocked = 0;
	return(0);
}

int UnionGetReadingLocksOfPKDB()
{
	int	ret;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionGetReadingLocksOfPKDB:: UnionConnectPKDB!\n");
		return(ret);
	}
	return(pgunionPKDB->readingLocks);
}

int UnionGetWritingLocksOfPKDB()
{
	int	ret;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionGetWritingLocksOfPKDB:: UnionConnectPKDB!\n");
		return(ret);
	}
	return(pgunionPKDB->isWritingLocked);
}

// 20070611 增加结束

PUnionPKDB UnionGetCurrentActivePKDB()
{
	int	ret;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionGetCurrentActivePKDB:: UnionConnectPKDB!\n");
		return(NULL);
	}
	return(pgunionPKDB);
}

PUnionPK UnionGetCurrentActivePKTBL()
{
	int	ret;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionGetCurrentActivePKTBL:: UnionConnectPKDB!\n");
		return(NULL);
	}
	return(pgunionPK);
}

int UnionGetCurrentActivePKNumInKeyDB()
{
	int	ret;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionGetCurrentActivePKNumInKeyDB:: UnionConnectPKDB!\n");
		return(ret);
	}
	else
		return(pgunionPKDB->num);
}


// 申请写锁表
int UnionWritingLockPKTBL()
{
	int	ret;
	int	index;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionWritingLockPKTBL:: UnionConnectPKDB!\n");
		return(ret);
	}
	if ((pgunionPKDB->isWritingLocked <= 0) && (pgunionPKDB->readingLocks <= 0))	// 表未被锁住，置锁住标志
		pgunionPKDB->isWritingLocked = 1;
	else							// 表已被锁住
		return(errCodePKDBMDL_TBLLocked);
		
	for (index = 0; index <  pgunionPKDB->num; index++)
	{
		if ((pgunionPK+index)->isWritingLocked)	// 有记录被写锁住了，不能置写锁
		{	
			pgunionPKDB->isWritingLocked = 0;
			return(errCodePKDBMDL_SomeRecIsWritingLocked);
		}
	}
	// 没有记录被写锁住
	return(0);
}

// 解除写锁表
int UnionWritingUnlockPKTBL()
{
	int	ret;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionWritingUnlockPKTBL:: UnionConnectPKDB!\n");
		return(ret);
	}
	pgunionPKDB->isWritingLocked = 0;
	return(0);
}

// 申请读锁表
int UnionReadingLockPKTBL()
{
	int	ret;
	int	index;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionReadingLockPKTBL:: UnionConnectPKDB!\n");
		return(ret);
	}
	if (pgunionPKDB->readingLocks <= 0)	// 表未被写锁住，置锁住标志
	{
		pgunionPKDB->readingLocks++;
		if (pgunionPKDB->readingLocks <= 0)
			pgunionPKDB->readingLocks = 1;
		return(0);
	}
	else							// 表已被锁住
		return(errCodePKDBMDL_TBLLocked);
}

// 解除读锁表
int UnionReadingUnlockPKTBL()
{
	int	ret;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionReadingUnlockPKTBL:: UnionConnectPKDB!\n");
		return(ret);
	}
	--pgunionPKDB->readingLocks;
	if (pgunionPKDB->readingLocks < 0)
		pgunionPKDB->readingLocks = 0;
	return(0);
}


// 申请写锁住记录
int UnionWritingLockPK(PUnionPK pkey)
{
	int	ret;
	
	if (pkey == NULL)
		return(errCodeParameter);

	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionWritingLockPK:: UnionConnectPKDB!\n");
		return(ret);
	}
	if (pgunionPKDB->isWritingLocked > 0)			// 表已被锁住
		return(errCodePKDBMDL_TBLLocked);
	
	if (pkey->isWritingLocked)	// 记录被写锁住了，不能置写锁
	{	
		UnionUserErrLog("in UnionWritingLockPK:: [%s] is writing-locked!\n",pkey->fullName);
		return(errCodePKDBMDL_SomeRecIsWritingLocked);
	}
	
	// 置记录为写锁住
	pkey->isWritingLocked = 1;
	UnionProgramerLog("in UnionWritingLockPK:: lock [%s] ok!\n",pkey->fullName);
	return(0);
}

// 申请解除写锁住记录
int UnionWritingUnlockPK(PUnionPK pkey)
{
	int	ret;
	int	index;
	int	i;
	
	if (pkey == NULL)
		return(errCodeParameter);

	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionWritingUnlockPK:: UnionConnectPKDB!\n");
		return(ret);
	}
	for (i = 0; i < 1000; i++)
	{
		if (pgunionPKDB->isWritingLocked > 0)			// 表已被锁住
		{
			usleep(1000);
			continue;
		}
		else
		{
			pkey->isWritingLocked = 0;
			UnionProgramerLog("in UnionWritingUnlockPK:: unlock [%s] ok!\n",pkey->fullName);
			return(0);
		}
	}
	UnionUserErrLog("in UnionWritingUnlockPK:: PKDB is writingLocked!\n");
	UnionProgramerLog("in UnionWritingUnlockPK:: unlock [%s] failure!\n",pkey->fullName);
	return(errCodePKDBMDL_TBLLocked);
}

// 申请读锁住记录
int UnionReadingLockPK(PUnionPK pkey)
{
	int	ret;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionReadingLockPK:: UnionConnectPKDB!\n");
		return(ret);
	}
	if (pgunionPKDB->isWritingLocked > 0)			// 表已被锁住
		return(errCodePKDBMDL_TBLLocked);
	
	if (pkey == NULL)
		return(errCodeParameter);

	if (pkey->isWritingLocked)
	{
		//UnionPrintPK(pkey);	
		UnionUserErrLog("in UnionReadingLockPK:: [%s] is writing-locked!\n",pkey->fullName);
		return(errCodePKDBMDL_SomeRecIsWritingLocked);
	}
		
	return(0);
}

// 申请解除读锁住记录
int UnionReadingUnlockPK(PUnionPK pkey)
{
	return(0);
}

int UnionGetNameOfPKDB(char *fileName)
{
	sprintf(fileName,"%s/keyFile/unionPKDB.KDB",getenv("UNIONETC"));
	return(0);
}

int UnionIsPKDBConnected()
{
	if ((!UnionIsSharedMemoryInited(conMDLNameOfUnionPKDB)) || (pgunionPKDB == NULL) || (pgunionPK == NULL))
		return(0);
	else
		return(1);
}

int UnionGetMaxKeyNumFromPKDBFile()
{
	int	ret;
	FILE	*fp;
	char	fileName[256];
	TUnionPKDB	keyDB;

	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfPKDB(fileName);
	
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionGetMaxKeyNumFromPKDBFile:: fopen [%s]\n",fileName);
		fclose(fp);
		return(errCodeUseOSErrCode);
	}
	
	memset(&keyDB,0,sizeof(keyDB));	
	fread(&keyDB,sizeof(keyDB),1,fp);
	
	fclose(fp);
	
	return(keyDB.maxKeyNum);
}

// Module Layer Functions
int UnionConnectPKDB()
{
	int	ret;
	int	maxKeyNum;

	if (UnionIsPKDBConnected())
		return(0);
	
	if ((maxKeyNum = UnionGetMaxKeyNumFromPKDBFile()) <= 0)
	{
		UnionUserErrLog("in UnionConnectPKDB:: UnionGetMaxKeyNumFromPKDBFile!\n");
		return(errCodePKDBMDL_KeyNum);
	}
	
	if ((pgunionPKDBMDL = UnionConnectSharedMemoryModule(conMDLNameOfUnionPKDB,
			sizeof(TUnionPKDB) + sizeof(TUnionPK) * maxKeyNum)
			) == NULL)
	{
		UnionUserErrLog("in UnionConnectPKDB:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}
	if ((pgunionPKDB = (PUnionPKDB)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionPKDBMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectPKDB:: PUnionPKDB!\n");
		UnionRemovePKDBInMemory();
		return(errCodeSharedMemoryModule);
	}
	
	if ((pgunionPK = (PUnionPK)((unsigned char *)pgunionPKDB + sizeof(*pgunionPKDB))) == NULL)
	{
		UnionUserErrLog("in UnionConnectPKDB:: PUnionPK!\n");
		UnionRemovePKDBInMemory();
		return(errCodeSharedMemoryModule);
	}
	pgunionPKDB->pkey = pgunionPK;
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionPKDBMDL))
	{
		pgunionPKDB->num = 0;
		pgunionPKDB->maxKeyNum = maxKeyNum;
		pgunionPKDB->isWritingLocked = 0;
		pgunionPKDB->readingLocks = 0;
		if ((ret = UnionLoadPKDBIntoMemory()) < 0)
		{
			UnionUserErrLog("in UnionConnectPKDB:: UnionLoadPKDBIntoMemory!\n");
			UnionRemovePKDBInMemory();
		}
		return(ret);
	}
	else
		return(0);
}

int UnionDisconnectPKDB()
{
	pgunionPK = NULL;
	pgunionPKDB = NULL;
	
	return(UnionDisconnectShareModule(pgunionPKDBMDL));
}

int UnionRemovePKDBInMemory()
{
	UnionDisconnectPKDB();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfUnionPKDB));
}

int UnionCreatePKDB(int maxKeyNum)
{
	TUnionPKDB		rec;
	int			ret;
	
	if (maxKeyNum <= 0)
	{
		UnionUserErrLog("in UnionCreatePKDB:: maxKeyNum [%d] error!\n",maxKeyNum);
		return(errCodePKDBMDL_KeyNum);
	}
	
	memset(&rec,0,sizeof(rec));
	rec.num = 0;
	rec.pkey = NULL;
	rec.maxKeyNum = maxKeyNum;
	UnionGetFullSystemDateTime(rec.lastUpdatedTime);
		
	return(UnionMirrorPKDBIntoDisk(&rec,NULL));
}

int UnionDeletePKDB()
{
	char			fileName[256];
	char			cmd[256];
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfPKDB(fileName);
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"rm %s",fileName);
	
	return(system(cmd));
}	

int UnionMirrorPKDBIntoDisk(PUnionPKDB ppkDB,PUnionPK pkey)
{
	FILE			*fp;
	char			fileName[256];

	if (ppkDB == NULL)
	{
		UnionUserErrLog("in UnionMirrorPKDBIntoDisk:: NullPointer!\n");
		return(errCodeParameter);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfPKDB(fileName);
	
	if ((fp = fopen(fileName,"wb")) == NULL)
	{
		UnionSystemErrLog("in UnionMirrorPKDBIntoDisk:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	fwrite(ppkDB,sizeof(*ppkDB),1,fp);
	if (ppkDB->num < 0)
	{
		UnionUserErrLog("in UnionMirrorPKDBIntoDisk:: ppkDB->num [%d] Error!\n",ppkDB->num);
		fclose(fp);
		return(errCodePKDBMDL_KeyNum);
	}
	
	if ((ppkDB->num == 0) || (pkey == NULL))
	{
		fclose(fp);
		return(0);
	}
	
	fwrite(pkey,sizeof(TUnionPK),ppkDB->num,fp);	
	
	fclose(fp);

	return(0);
}

int UnionLoadPKDBIntoMemory()
{
	int	ret;
	FILE	*fp;
	char	fileName[256];
	TUnionPKDB	keyDB;
	int	index;
		
	if ((ret = UnionWritingLockPKTBL()) < 0)
	{
		UnionUserErrLog("in UnionLoadPKDBIntoMemory:: UnionWritingLockPKTBL!\n");
		return(ret);
	}

	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfPKDB(fileName);
	
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionLoadPKDBIntoMemory:: fopen [%s]\n",fileName);
		ret = errCodeUseOSErrCode;
		goto exitRightNow;
	}
	
	memset(&keyDB,0,sizeof(keyDB));	
	fread(&keyDB,sizeof(keyDB),1,fp);
	pgunionPKDB->maxKeyNum = keyDB.maxKeyNum;
	pgunionPKDB->num = keyDB.num;
	memcpy(pgunionPKDB->lastUpdatedTime,keyDB.lastUpdatedTime,sizeof(keyDB.lastUpdatedTime));

	if ((pgunionPKDB->num = keyDB.num) > 0)
	{
		if (pgunionPK == NULL)
		{
			UnionUserErrLog("in UnionLoadPKDBIntoMemory:: pkey is NULL!\n");
			ret = errCodePKDBMDL_KeyDBNonConnected;
			goto exitRightNow;
		}
		if (pgunionPKDB->num > pgunionPKDB->maxKeyNum)
		{
			UnionUserErrLog("in UnionLoadPKDBIntoMemory:: pgunionPKDB->num [%d] > pgunionPKDB->maxKeyNum [%d]\n",pgunionPKDB->num,pgunionPKDB->maxKeyNum);
			ret = errCodePKDBMDL_KeyNum;
			goto exitRightNow;
		}
		fread(pgunionPK,sizeof(TUnionPK),pgunionPKDB->num,fp);
		for (index = 0; index < pgunionPKDB->num; index++)
		{
			(pgunionPK+index)->isWritingLocked = 0;
		}
	}
	ret = 0;
	
exitRightNow:
	UnionWritingUnlockPKTBL();
	fclose(fp);
	return(ret);
}

int UnionPrintPKDBToFile(FILE *fp)
{
	int	ret;
	long	i;
	char	nameOfType[40];
	
	if ((ret = UnionReadingLockPKTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKDBToFile:: UnionReadingLockPKTBL!\n");
		return(ret);
	}
	
	if (fp == NULL)
		fp = stdout;
	
	for (i = 0; i < pgunionPKDB->num; i++)
	{
		memset(nameOfType,0,sizeof(nameOfType));
		UnionGetNameOfPKType((pgunionPK+i)->type,nameOfType);
		fprintf(fp,"%30s %20s %18s %04d %d\n",
			(pgunionPK+i)->fullName,
			(pgunionPK+i)->container,
			nameOfType,
			UnionGetBitsLengthOfRSAPair((pgunionPK+i)->length),
			(pgunionPK+i)->isWritingLocked);
		if (((fp == stdout) || (fp == stderr)) && (i % 20 == 0) && (i != 0))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press enter to continue or exit/quit to exit")))
				return(0);
		}
	}
	fprintf(fp,"totolKeyNum = [%d]\n",pgunionPKDB->num);
	fprintf(fp,"maxKeyNum       = [%d]\n",pgunionPKDB->maxKeyNum);
	fprintf(fp,"isWritingLocked = [%d]\n",pgunionPKDB->isWritingLocked);
	fprintf(fp,"readingLocks    = [%d]\n",pgunionPKDB->readingLocks);
	fprintf(fp,"lastUpdatedTime = [%s]\n",pgunionPKDB->lastUpdatedTime);
	UnionReadingUnlockPKTBL();
	ret = 0;
	
	return(0);
}

int UnionPrintPKDB()
{
	int	ret;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKDB:: UnionConnectPKDB !\n");
		return(ret);
	}
		
	UnionPrintPKDBToFile(stdout);
	UnionPrintSharedMemoryModuleToFile(pgunionPKDBMDL,stdout);
	return(0);
}

int UnionPrintAllPKsInKeyDBToFile(FILE *fp)
{
	return(UnionPrintPKDBToFile(fp));
}

int UnionPrintStatusOfPKDBToFile(FILE *fp)
{
	int	ret;
	
	if ((ret = UnionReadingLockPKTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintStatusOfPKDBToFile:: UnionReadingLockPKTBL! ret = [%d]\n",ret);
		return(ret);
	}
	if (fp == NULL)
		fp = stdout;
	fprintf(fp,"totolKeyNum     = [%d]\n",pgunionPKDB->num);
	fprintf(fp,"maxKeyNum       = [%d]\n",pgunionPKDB->maxKeyNum);
	fprintf(fp,"isWritingLocked = [%d]\n",pgunionPKDB->isWritingLocked);
	fprintf(fp,"readingLocks    = [%d]\n",pgunionPKDB->readingLocks);
	fprintf(fp,"lastUpdatedTime = [%s]\n",pgunionPKDB->lastUpdatedTime);
	UnionReadingUnlockPKTBL();
	return(UnionPrintSharedMemoryModuleToFile(pgunionPKDBMDL,fp));
}

int UnionInsertPKIntoKeyDB(PUnionPK pkey)
{
	int	ret;
	long    index;
	int	check;
	int	pos;
	
#ifdef _KDBClient_
	if ((ret = UnionApplyKDBService(conInsertRecord,conIsPKDB,(unsigned char *)pkey,sizeof(*pkey))) < 0)
	{
		UnionUserErrLog("in UnionInsertPKIntoKeyDB:: UnionApplyKDBService !\n");
		return(ret);
	}
	return(ret);
#endif
	if (!UnionIsValidPK(pkey))
	{
		UnionUserErrLog("in UnionInsertPKIntoKeyDB:: UnionIsValidPK [%s]\n",pkey->fullName);
		return(errCodeParameter);
	}
	if ((ret = UnionWritingLockPKTBL()) < 0)
	{
		UnionUserErrLog("in UnionInsertPKIntoKeyDB:: UnionWritingLockPKTBL !\n");
		return(ret);
	}
	pkey->useTimes = 0;
	pkey->isWritingLocked = 0;
	time(&(pkey->lastUpdateTime));
	if (pgunionPKDB->num == pgunionPKDB->maxKeyNum)
	{
		UnionUserErrLog("in UnionInsertPKIntoKeyDB:: PKDB is full!\n");
		UnionWritingUnlockPKTBL();
		return(errCodePKDBMDL_KeyDBFull);
	}
	for (index = 0; index < pgunionPKDB->num; index++)
	{
		if ((check = strcmp(pkey->fullName,(pgunionPK + index)->fullName)) == 0)
		{
			UnionUserErrLog("in UnionInsertPKIntoKeyDB:: [%s] already exists!\n",pkey->fullName);
			UnionWritingUnlockPKTBL();
			return(errCodePKDBMDL_KeyAlreadyExists);
		}
		if (check > 0)
			continue;
		else
			break;
	}
	pos = index;	// 新记录将插在pos指示的位置
	for (index = pgunionPKDB->num - 1; index >= pos; index--)
	{
		memcpy(pgunionPK+index+1,pgunionPK+index,sizeof(TUnionPK));
	}
	memcpy(pgunionPK+pos,pkey,sizeof(TUnionPK));
	pgunionPKDB->num++;
	UnionGetFullSystemDateTime(pgunionPKDB->lastUpdatedTime);
	if ((ret = UnionMirrorPKDBIntoDisk(pgunionPKDB,pgunionPK)) < 0)
		UnionUserErrLog("in UnionInsertPKIntoKeyDB:: UnionMirrorPKDBIntoDisk!\n");
	UnionWritingUnlockPKTBL();
	UnionLogPKDBOperation(conRecordInsert,pkey);
	return(ret);
}

int UnionDeletePKFromKeyDB(char *fullName)
{
	int	ret;
	long    index;
	int	pos;
	TUnionPK	pk;
	
#ifdef _KDBClient_
	
	memset(&pk,0,sizeof(pk));
	strcpy(pk.fullName,fullName);
	if ((ret = UnionApplyKDBService(conDeleteRecord,conIsPKDB,(unsigned char *)(&pk),sizeof(pk))) < 0)
	{
		UnionUserErrLog("in UnionDeletePKFromKeyDB:: UnionApplyKDBService !\n");
		return(ret);
	}
	return(ret);
#endif
	if ((ret = UnionWritingLockPKTBL()) < 0)
	{
		UnionUserErrLog("in UnionDeletePKFromKeyDB:: UnionWritingLockPKTBL !\n");
		return(ret);
	}
	
	for (index = 0; index < pgunionPKDB->num; index++)
	{
		if (strcmp(fullName,(pgunionPK + index)->fullName) == 0)
			break;
	}
	if (index == pgunionPKDB->num)
	{
		UnionWritingUnlockPKTBL();
		return(errCodePKDBMDL_KeyNonExists);
	}
	pos = index;
	memcpy(&pk,pgunionPK+index,sizeof(pk));
	for (index = pos+1; index < pgunionPKDB->num; index++)
	{
		memcpy(pgunionPK+index-1,pgunionPK + index,sizeof(TUnionPK));
	}
	pgunionPKDB->num--;
	UnionGetFullSystemDateTime(pgunionPKDB->lastUpdatedTime);
	if ((ret = UnionMirrorPKDBIntoDisk(pgunionPKDB,pgunionPK)) < 0)
		UnionUserErrLog("in UnionDeletePKFromKeyDB:: UnionMirrorPKDBIntoDisk!\n");
	UnionWritingUnlockPKTBL();
	UnionLogPKDBOperation(conRecordDelete,&pk);	
	return(ret);
}

int UnionReadPKFromKeyDBAnyway(PUnionPK pkey)
{
	int	ret;
	long	index=0;

	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromKeyDBAnyway:: UnionConnectPKDB !\n");
		return(ret);
	}
		
	if (pkey == NULL)
	{
		UnionUserErrLog("in UnionReadPKFromKeyDBAnyway:: null Pointer!\n");
		return(errCodeParameter);
	}
	
	if ((index = UnionFindPKPosInKeyDB(pkey->fullName)) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromKeyDBAnyway:: UnionFindPKPosInKeyDB [%s]!\n",pkey->fullName);
		return((int)index);
	}
	
	if ((ret = UnionReadingLockPK(pgunionPK+index)) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromKeyDBAnyway:: UnionReadingLockPK [%s]!\n",pkey->fullName);
		return(ret);
	}
	memcpy(pkey,pgunionPK+index,sizeof(TUnionPK));
	UnionReadingUnlockPK(pgunionPK+index);

	return(0);
}

// 2004/11/25, Wolfgang Wang
int UnionUsePKFromKeyDB(PUnionPK pkey)
{
	int	ret;
	long	index=0;

	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionUsePKFromKeyDB:: UnionConnectPKDB !\n");
		return(ret);
	}
	if (pkey == NULL)
	{
		UnionUserErrLog("in UnionUsePKFromKeyDB:: null Pointer!\n");
		return(errCodeParameter);
	}
	
	if ((index = UnionFindPKPosInKeyDB(pkey->fullName)) < 0)
	{
		UnionUserErrLog("in UnionUsePKFromKeyDB:: UnionFindPKPosInKeyDB [%s]!\n",pkey->fullName);
		return((int)index);
	}
	
	if ((ret = UnionReadingLockPK(pgunionPK+index)) < 0)
	{
		UnionUserErrLog("in UnionUsePKFromKeyDB:: UnionReadingLockPK [%s]!\n",pkey->fullName);
		return(ret);
	}
	
	if (!UnionIsPKStillEffective(pgunionPK+index))
	{
		UnionReadingUnlockPK(pgunionPK+index);
		UnionUserErrLog("in UnionUsePKFromKeyDB:: [%s] outdate!\n",pkey->fullName);
		return(errCodePKDBMDL_KeyNotEffective);
	}
	
	(pgunionPK+index)->useTimes += 1;
	
	memcpy(pkey,pgunionPK+index,sizeof(TUnionPK));

	UnionReadingUnlockPK(pgunionPK+index);

	return(0);
}

int UnionReadPKFromKeyDB(PUnionPK pkey)
{
	return(UnionUsePKFromKeyDB(pkey));
}

int UnionUpdatePKInKeyDB(PUnionPK pkey)
{
	int	ret;
	long	index=0;
	char	tmpBuf[2048+1];
	
	if (pkey == NULL)
	{
		UnionUserErrLog("in UnionUpdatePKInKeyDB:: null Pointer!\n");
		return(errCodeParameter);
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	bcdhex_to_aschex(pkey->value,UnionGetAscCharLengthOfPK(pkey->length)/2,tmpBuf);	
	return(UnionUpdatePKValueInKeyDB(pkey->fullName,tmpBuf,pkey->checkValue));
}

// 2004/11/25, Wolfgang Wang
int UnionUpdatePKValueInKeyDB(char *fullName,char *value,char *checkValue)
{
	int	ret;
	long	index=0;
	TUnionPK	pk;

#ifdef _KDBClient_
	if ((fullName == NULL) || (strlen(fullName) >= sizeof(pk.fullName)) || (value == NULL) || 
		(strlen(value)/2 >= sizeof(pk.value)))
	{
		UnionUserErrLog("in UnionUpdatePKValueInKeyDB:: wrong parameter!\n");
		return(errCodeParameter);
	}
	memset(&pk,0,sizeof(pk));
	strcpy(pk.fullName,fullName);
	if ((ret = UnionReadPKFromKeyDBAnyway(&pk)) < 0)
	{
		UnionUserErrLog("in UnionUpdatePKValueInKeyDB:: UnionReadPKFromKeyDBAnyway [%s]!\n");
		return(errCodeParameter);
	}
	aschex_to_bcdhex(value,strlen(value),pk.value);
	if ((checkValue != NULL) && (strlen(checkValue) < sizeof(pk.checkValue)))
		strcpy(pk.checkValue,checkValue);
	if ((ret = UnionApplyKDBService(conUpdateRecordValueFlds,conIsPKDB,(unsigned char *)(&pk),sizeof(pk))) < 0)
	{
		UnionUserErrLog("in UnionUpdatePKValueInKeyDB:: UnionApplyKDBService !\n");
		return(ret);
	}
	return(ret);
#endif
	if ((index = UnionFindPKPosInKeyDB(fullName)) < 0)
	{
		UnionUserErrLog("in UnionUpdatePKValueInKeyDB:: UnionFindPKPosInKeyDB [%s]!\n",fullName);
		return((int)index);
	}
	
	if ((ret = UnionWritingLockPK(pgunionPK+index)) < 0)
	{
		UnionUserErrLog("in UnionUpdatePKValueInKeyDB:: UnionWritingLockPK!\n");
		return(ret);
	}
	
	if ((ret = UnionUpdatePKValue(pgunionPK+index,value,checkValue)) < 0)
	{
		UnionUserErrLog("in UnionUpdatePKValueInKeyDB:: UnionUpdatePKValue [%s]\n",fullName);
		UnionWritingUnlockPK(pgunionPK+index);
		return(ret);
	}
	
	UnionGetFullSystemDateTime(pgunionPKDB->lastUpdatedTime);

	if ((ret = UnionMirrorPKDBIntoDisk(pgunionPKDB,pgunionPK)) < 0)
		UnionUserErrLog("in UnionUpdatePKValueInKeyDB:: UnionMirrorPKDBIntoDisk!\n");
	
	memcpy(&pk,pgunionPK+index,sizeof(pk));

	UnionWritingUnlockPK(pgunionPK+index);
	
	UnionLogPKDBOperation(conRecordUpdate,&pk);	

	return(0);
}

long UnionFindPKPosInKeyDB(char *fullName)
{
	int	ret;
	long    index=0;
	//int	pos;
	long	left;
	long	right;
	int	check;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionFindPKPosInKeyDB:: UnionConnectPKDB !\n");
		return(ret);
	}
		
	if ((ret = UnionApplyReadingLocks(pgunionPKDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionFindPKPosInKeyDB:: UnionApplyReadingLocks !\n");
		return(ret);
	}
	/*
	for (index = 0; index < pgunionPKDB->num; index++)
	{
		UnionNullLog("[%06d] [%s]\n",index,(pgunionPK+index)->fullName);
	}

	UnionLog("in UnionFindPKPosInKeyDB:: [%s] [%d]\n",fullName,pgunionPKDB->num);
	*/
	left = 0;
	right = pgunionPKDB->num - 1;
	index = (left+right)/2;
	while (1) 
	{
		if ((left > right) || (left >= pgunionPKDB->num) || (right < 0))
		{
			ret = errCodePKDBMDL_KeyNonExists;
			goto exitRightNow;
		}
		if ((strcmp(fullName,(pgunionPK+left)->fullName) < 0) || 
			(strcmp(fullName,(pgunionPK+right)->fullName) > 0))
		{
			ret = errCodePKDBMDL_KeyNonExists;
			//UnionNullLog("fullName = [%s] left = [%s] right = [%s] len = [%d]\n",fullName,(pgunionPK+left)->fullName,(pgunionPK+right)->fullName,40);
			goto exitRightNow;
		}
		index = (left + right) / 2;
		//UnionNullLog("index = [%d] fullName = [%s] index = [%s] len = [%d]\n",index,fullName,(pgunionPK+index)->fullName,40);	
		if ((check = strcmp(fullName,(pgunionPK+index)->fullName)) == 0)
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
		
	UnionReleaseReadingLocks(pgunionPKDBMDL);
	return(index);
	
exitRightNow:
	//UnionNullLog("in UnionFindPKPosInKeyDB:: index = [%ld] left = [%ld] right = [%ld]\n",index,left,right);
	UnionReleaseReadingLocks(pgunionPKDBMDL);
	return(ret);
}

int UnionGetPKNumOfKeyDB()
{
	int	ret;
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionGetPKNumOfKeyDB:: UnionConnectPKDB !\n");
		return(ret);
	}
		
	if (pgunionPKDB == NULL)
	{
		UnionUserErrLog("in UnionGetPKNumOfKeyDB:: null Pointer!\n");
		return(errCodePKDBMDL_KeyDBNonConnected);
	}
	return(pgunionPKDB->num);
}

char *UnionGetPKDBLastUpdatedTime()
{
	int	ret;
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionGetPKDBLastUpdatedTime:: UnionConnectPKDB !\n");
		return(NULL);
	}
		
	if (pgunionPKDB == NULL)
	{
		UnionUserErrLog("in UnionGetPKDBLastUpdatedTime:: null Pointer!\n");
		return(NULL);
	}
	return(pgunionPKDB->lastUpdatedTime);
}

int UnionChangeSizeOfPKDB(long newMaxKeyNum)
{
	int	ret;
	FILE	*fp1,*fp2;
	char	fileName1[256],fileName2[256];
	TUnionPKDB	oldKeyDB,newKeyDB;
	long	copyNum;
	TUnionPK	pk;
	char	cmd[512];
	
	memset(fileName1,0,sizeof(fileName1));
	UnionGetNameOfPKDB(fileName1);
	
	if ((fp1 = fopen(fileName1,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionChangeSizeOfPKDB:: fopen [%s]\n",fileName1);
		return(errCodeUseOSErrCode);
	}
	
	memset(&oldKeyDB,0,sizeof(oldKeyDB));	
	fread(&oldKeyDB,sizeof(oldKeyDB),1,fp1);
	if (oldKeyDB.maxKeyNum == newMaxKeyNum)
	{
		UnionUserErrLog("in UnionChangeSizeOfPKDB:: original newMaxKeyNum [%ld] = newMaxNum [%ld]\n",oldKeyDB.maxKeyNum,newMaxKeyNum);
		fclose(fp1);
		return(errCodePKDBMDL_KeyNum);
	}
	if (oldKeyDB.maxKeyNum > newMaxKeyNum)
	{
		if (oldKeyDB.num > newMaxKeyNum)
		{
			UnionUserErrLog("in UnionChangeSizeOfPKDB:: real Num [%ld] > newMaxNum [%ld]\n",oldKeyDB.num,newMaxKeyNum);
			fclose(fp1);
			return(errCodePKDBMDL_KeyNum);
		}
	}
	memset(fileName2,0,sizeof(fileName2));
	sprintf(fileName2,"%s.tmp",fileName1);
	if ((fp2 = fopen(fileName2,"wb")) == NULL)
	{
		UnionSystemErrLog("in UnionChangeSizeOfPKDB:: fopen [%s]\n",fileName2);
		fclose(fp1);
		return(errCodeUseOSErrCode);
	}
	
	memcpy(&newKeyDB,&oldKeyDB,sizeof(newKeyDB));
	newKeyDB.maxKeyNum = newMaxKeyNum;
	fwrite(&newKeyDB,sizeof(newKeyDB),1,fp2);
	
	for (copyNum = 0; copyNum < oldKeyDB.num; copyNum++)
	{
		fread(&pk,sizeof(pk),1,fp1);
		fwrite(&pk,sizeof(pk),1,fp2);
	}
	memset(&pk,0,sizeof(pk));
	for (;copyNum < newKeyDB.maxKeyNum;copyNum++)
	{
		fwrite(&pk,sizeof(pk),1,fp2);
	}
	fclose(fp1);
	fclose(fp2);

	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"mv %s %s",fileName2,fileName1);
	system(cmd);
	
	return(0);
}

int UnionPrintPKExpectedInKeyDBToFile(char *app,char *owner,char *name,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	short	appEQExpected=0,ownerEQExpected=0,nameEQExpected=0;
	char	thisApp[40+1],thisOwner[40+1],thisName[40+1];
	char	nameOfType[20];
	
	if ((ret = UnionReadingLockPKTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKExpectedInKeyDBToFile:: UnionReadingLockPKTBL!\n");
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
			
	for (i = 0; i < pgunionPKDB->num; i++)
	{
		memset(thisApp,0,sizeof(thisApp));
		memset(thisOwner,0,sizeof(thisOwner));
		memset(thisName,0,sizeof(thisName));
		UnionAnalysisPKFullName((pgunionPK+i)->fullName,thisApp,thisOwner,thisName);
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
		UnionGetNameOfPKType((pgunionPK+i)->type,nameOfType);
		fprintf(fp,"%30s %20s %20s %04d\n",
			(pgunionPK+i)->fullName,
			(pgunionPK+i)->container,
			nameOfType,
			UnionGetAscCharLengthOfPK((pgunionPK+i)->length));
		if ((fp == stdout) && (totalNum % 20 == 0) && (totalNum != 0))
		{
			printf("Press any key to continue...\n");
			getchar();
			getchar();
		}
	}
	fprintf(fp,"totolNum = [%d]\n",totalNum);
	ret = 0;
	
exitRightNow:
	UnionReadingUnlockPKTBL();
	return(ret);
}

int UnionPrintPKExpectedToFile(char *app,char *owner,char *name,FILE *fp)
{
	return(UnionPrintPKExpectedInKeyDBToFile(app,owner,name,fp));
}

int UnionPrintPKInKeyDBOutofDateToFile(FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	
	if ((ret = UnionReadingLockPKTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBOutofDateToFile:: UnionReadingLockPKTBL!\n");
		return(ret);
	}
	if (fp == NULL)
		fp = stdout;
	
	for (i = 0; i < pgunionPKDB->num; i++)
	{
		if (UnionIsPKStillEffective(pgunionPK+i))
			continue;
		totalNum++;
		fprintf(fp,"%30s %20s %s %05ld %05ld %05ld\n",
			(pgunionPK+i)->fullName,
			(pgunionPK+i)->container,
			(pgunionPK+i)->activeDate,
			(pgunionPK+i)->maxEffectiveDays,
			(pgunionPK+i)->useTimes,
			(pgunionPK+i)->maxUseTimes);
		if ((fp == stdout) && (totalNum % 20 == 0) && (totalNum != 0))
		{
			printf("Press any key to continue...\n");
			getchar();
			getchar();
		}
	}
	fprintf(fp,"totolNum = [%d]\n",totalNum);
	ret = 0;
	
exitRightNow:
	UnionReadingUnlockPKTBL();
	return(ret);
}


int UnionPrintPKOutofDateToFile(FILE *fp)
{
	return(UnionPrintPKInKeyDBOutofDateToFile(fp));
}

int UnionPrintPKInKeyDBNearOutofDateToFile(long days,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	long	tmpDays;
	
	if ((ret = UnionReadingLockPKTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBNearOutofDateToFile:: UnionReadingLockPKTBL!\n");
		return(ret);
	}
	if (fp == NULL)
		fp = stdout;

	for (i = 0; i < pgunionPKDB->num; i++)
	{
		if ((tmpDays = UnionDecideDaysBeforeToday((pgunionPK+i)->activeDate) + days) < (pgunionPK+i)->maxEffectiveDays)
			continue;
		totalNum++;
		fprintf(fp,"%30s %20s %s %05ld\n",(pgunionPK+i)->fullName,(pgunionPK+i)->container,(pgunionPK+i)->activeDate,(pgunionPK+i)->maxEffectiveDays);
		if ((fp == stdout) && (totalNum % 20 == 0) && (totalNum != 0))
		{
			printf("Press any key to continue...\n");
			getchar();
			getchar();
		}
	}
	fprintf(fp,"totolNum = [%d]\n",totalNum);
	ret = 0;
	
exitRightNow:
	UnionReadingUnlockPKTBL();
	return(ret);
}

int UnionPrintPKNearOutofDateToFile(long days,FILE *fp)
{
	return(UnionPrintPKInKeyDBNearOutofDateToFile(days,fp));
}


// 2004/11/25, Wolfgang Wang
int UnionPrintPKInKeyDBNearOutofMaxUseTimesToFile(long times,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	
	if ((ret = UnionReadingLockPKTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBNearOutofMaxUseTimesToFile:: UnionReadingLockPKTBL!\n");
		return(ret);
	}
	if (fp == NULL)
		fp = stdout;
	for (i = 0; i < pgunionPKDB->num; i++)
	{
		if ( (pgunionPK+i)->useTimes + times < (pgunionPK+i)->maxUseTimes)
			continue;
		totalNum++;
		fprintf(fp,"%30s %20s %05ld %05ld\n",(pgunionPK+i)->fullName,(pgunionPK+i)->container,(pgunionPK+i)->useTimes,(pgunionPK+i)->maxUseTimes);
		if ((fp == stdout) && (totalNum % 20 == 0) && (totalNum != 0))
		{
			printf("Press any key to continue...\n");
			getchar();
			getchar();
		}
	}
	fprintf(fp,"totolNum = [%d]\n",totalNum);
	ret = 0;
	
exitRightNow:
	UnionReadingUnlockPKTBL();
	return(ret);
}

// 2004/11/25, Wolfgang Wang
int UnionPrintPKNearOutofMaxUseTimesToFile(long times,FILE *fp)
{
	return(UnionPrintPKInKeyDBNearOutofMaxUseTimesToFile(times,fp));
}

// 2004/11/20, Added by Wolfgang Wang
int UnionSynchronizeSpecifiedPKToBackuper(char *fullName)
{
	long	index=0;
	int	ret;

	if ((index = UnionFindPKPosInKeyDB(fullName)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeSpecifiedPKToBackuper:: UnionFindPKPosInKeyDB! [%s]\n",fullName);
		return((int)index);
	}

	if ((ret = UnionWritingLockPK(pgunionPK+index)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeAllPKToBackuper:: UnionWritingLockPK !\n");
		return(ret);
	}

	if ((ret = UnionSynchronizePKDBOperation(conRecordUpdate,pgunionPK+index)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeAllPKToBackuper:: UnionSynchronizePKDBOperation !\n");
	}
	
	UnionWritingUnlockPK(pgunionPK+index);
	
	return(ret);
}

// 2004/11/20, Added by Wolfgang Wang
long UnionSynchronizeAllPKToBackuper()
{
	int	ret;
	long	i;
	long	synchronizedNum = 0;
	
	if ((ret = UnionReadingLockPKTBL()) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeAllPKToBackuper:: UnionReadingLockPKTBL !\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionPKDB->num; i++)
	{
		if ((ret = UnionSynchronizePKDBOperation(conRecordUpdate,pgunionPK+i)) < 0)
		{
			UnionUserErrLog("synchronize Err::[%s]\n",(pgunionPK+i)->fullName);
			printf("synchronize Err::[%s]\n",(pgunionPK+i)->fullName);
		}
		else
		{
			UnionAuditLog("synchronize OK ::[%s]\n",(pgunionPK+i)->fullName);
			printf("synchronize OK ::[%s]\n",(pgunionPK+i)->fullName);
			synchronizedNum++;
		}
	}
	ret = synchronizedNum;
	
exitRightNow:
	UnionReadingUnlockPKTBL();
	return(ret);
}

// 2004/11/20, Added by Wolfgang Wang
long UnionSynchronizePKOfSpecifiedAppToBackuper(char *appName)
{
	int	ret;
	long	i;
	char	thisApp[40+1],thisOwner[40+1],thisName[40+1];
	long	synchronizedNum = 0;
	
	if ((ret = UnionReadingLockPKTBL()) < 0)
	{
		UnionUserErrLog("in UnionSynchronizePKOfSpecifiedAppToBackuper:: UnionReadingLockPKTBL !\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionPKDB->num; i++)
	{
		memset(thisApp,0,sizeof(thisApp));
		UnionAnalysisPKFullName((pgunionPK+i)->fullName,thisApp,thisOwner,thisName);
		if (strcmp(appName,thisApp) != 0)
			continue;
		if ((ret = UnionSynchronizePKDBOperation(conRecordUpdate,pgunionPK+i)) < 0)
		{
			UnionUserErrLog("synchronize Err::[%s]\n",(pgunionPK+i)->fullName);
			printf("synchronize Err::[%s]\n",(pgunionPK+i)->fullName);
		}
		else
		{
			UnionAuditLog("synchronize OK ::[%s]\n",(pgunionPK+i)->fullName);
			printf("synchronize OK ::[%s]\n",(pgunionPK+i)->fullName);
			synchronizedNum++;
		}
	}
	ret = synchronizedNum;
	
exitRightNow:
	UnionReadingUnlockPKTBL();
	return(ret);
}

// added by Wolfgang Wang, 2004/11/20
int UnionPKDBOperation(TUnionKeyDBOperation operation,PUnionPK pkey)
{
	int	ret = -1;
	char	tmpBuf[8096+100];
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionPKDBOperation:: UnionConnectPKDB !\n");
		return(ret);
	}
	
	switch (operation)
	{
		case	conInsertRecord:
			if ((ret = UnionInsertPKIntoKeyDB(pkey)) < 0)
				UnionUserErrLog("in UnionPKDBOperation:: UnionInsertPKIntoKeyDB [%s]\n",pkey->fullName);
			else
				UnionSuccessLog("in UnionPKDBOperation:: UnionInsertPKIntoKeyDB [%s] OK\n",pkey->fullName);
			break;
		case	conUpdateRecord:
		case	conUpdateRecordAllFlds:
			if (UnionFindPKPosInKeyDB(pkey->fullName) < 0)
			{
				if ((ret = UnionInsertPKIntoKeyDB(pkey)) < 0)
					UnionUserErrLog("in UnionPKDBOperation:: UnionInsertPKIntoKeyDB [%s]\n",pkey->fullName);
				else
					UnionSuccessLog("in UnionPKDBOperation:: UnionInsertPKIntoKeyDB [%s] OK\n",pkey->fullName);
			}
			else
			{
				if ((ret = UnionUpdateAllPKFieldsInKeyDB(pkey)) < 0)
					UnionUserErrLog("in UnionPKDBOperation:: UnionUpdateAllPKFieldsInKeyDB [%s]\n",pkey->fullName);
				else
					UnionSuccessLog("in UnionPKDBOperation:: UnionUpdateAllPKFieldsInKeyDB [%s] OK\n",pkey->fullName);
			}
			break;
		case	conDeleteRecord:
			if ((ret = UnionDeletePKFromKeyDB(pkey->fullName)) < 0)
				UnionUserErrLog("in UnionPKDBOperation:: UnionDeletePKFromKeyDB [%s]\n",pkey->fullName);
			else
				UnionSuccessLog("in UnionPKDBOperation:: UnionDeletePKFromKeyDB [%s] OK\n",pkey->fullName);
			break;
		case	conUpdateRecordValueFlds:
			memset(tmpBuf,0,sizeof(tmpBuf));
			bcdhex_to_aschex(pkey->value,UnionGetAscCharLengthOfPK(pkey->length)/2,tmpBuf);
			//UnionDebugLog("[%04d][%s]\n",pkey->length,tmpBuf);
			if ((ret = UnionUpdatePKValueInKeyDB(pkey->fullName,tmpBuf,pkey->checkValue)) < 0)
				UnionUserErrLog("in UnionPKDBOperation:: UnionUpdatePKValueInKeyDB [%s]\n",pkey->fullName);
			else
				UnionSuccessLog("in UnionPKDBOperation:: UnionUpdatePKValueInKeyDB [%s] OK\n",pkey->fullName);
			break;
		case	conUpdateRecordAttrFlds:
			if ((ret = UnionUpdatePKAttrInKeyDB(pkey)) < 0)
				UnionUserErrLog("in UnionPKDBOperation:: UnionUpdatePKAttrInKeyDB [%s]\n",pkey->fullName);
			else
				UnionSuccessLog("in UnionPKDBOperation:: UnionUpdatePKAttrInKeyDB [%s] OK\n",pkey->fullName);
			break;
		default:
			UnionUserErrLog("in UnionPKDBOperation::Unknown operation!");
			break;
	}
	return(ret);
}


// 2004/11/25, Wolfgang Wang
int UnionMirrorPKDBIntoDiskArtifically()
{
	int	ret;
	
	if ((ret = UnionConnectExistingPKDB()) < 0)
	{
		UnionUserErrLog("in UnionMirrorPKDBIntoDiskArtifically:: UnionConnectExistingPKDB !\n");
		return(ret);
	}
	return(UnionMirrorPKDBIntoDisk(pgunionPKDB,pgunionPK));
}

// Module Layer Functions
int UnionConnectExistingPKDB()
{
	int	ret;
	
	if ((pgunionPKDBMDL = UnionConnectExistingSharedMemoryModule(conMDLNameOfUnionPKDB,
			sizeof(TUnionPKDB))) == NULL)
	{
		UnionUserErrLog("in UnionConnectExistingPKDB:: UnionConnectExistingSharedMemoryModule!\n");
		return(-1);
	}
	if ((pgunionPKDB = (PUnionPKDB)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionPKDBMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectExistingPKDB:: PUnionPKDB!\n");
		UnionRemovePKDBInMemory();
		return(-1);
	}
	
	if ((pgunionPK = (PUnionPK)((unsigned char *)pgunionPKDB + sizeof(*pgunionPKDB))) == NULL)
	{
		UnionUserErrLog("in UnionConnectExistingPKDB:: PUnionPK!\n");
		UnionRemovePKDBInMemory();
		return(-1);
	}
	pgunionPKDB->pkey = pgunionPK;
	
	return(0);
}

// Wolfgang Wang, 2004/11/25
int UnionUpdatePKAttrInKeyDB(PUnionPK pkey)
{
	int	ret;
	long	index=0;
	TUnionPK	pk;

#ifdef _KDBClient_
	if ((ret = UnionApplyKDBService(conUpdateRecordAttrFlds,conIsPKDB,(unsigned char *)(pkey),sizeof(*pkey))) < 0)
	{
		UnionUserErrLog("in UnionUpdatePKAttrInKeyDB:: UnionApplyKDBService !\n");
		return(ret);
	}
	return(ret);
#endif
	if (pkey == NULL)
	{
		UnionUserErrLog("in UnionUpdatePKAttrInKeyDB:: null Pointer!\n");
		return(errCodeParameter);
	}
	
	memset(&pk,0,sizeof(pk));
	memcpy(&pk,pkey,sizeof(pk));
	memset(pk.value,0,sizeof(pk.value));	
	if (!UnionIsValidPK(pkey))
	{
		UnionUserErrLog("in UnionUpdatePKAttrInKeyDB:: UnionIsValidPK [%s]\n",pkey->fullName);
		return(errCodeParameter);
	}
	
	if ((index = UnionFindPKPosInKeyDB(pkey->fullName)) < 0)
	{
		UnionUserErrLog("in UnionUpdatePKAttrInKeyDB:: UnionFindPKPosInKeyDB [%s]!\n",pkey->fullName);
		return((int)index);
	}
	
	if ((ret = UnionWritingLockPK(pgunionPK+index)) < 0)
	{
		UnionUserErrLog("in UnionUpdatePKAttrInKeyDB:: UnionWritingLockPK !\n");
		return(ret);
	}
	
	memcpy(pk.value,(pgunionPK+index)->value,sizeof(pk.value));
	memcpy(pk.oldValue,(pgunionPK+index)->oldValue,sizeof(pk.oldValue));
	memcpy(pk.checkValue,(pgunionPK+index)->checkValue,sizeof(pk.checkValue));
	memcpy(pk.oldCheckValue,(pgunionPK+index)->oldCheckValue,sizeof(pk.oldCheckValue));
	pk.isWritingLocked = 0;
	memcpy(pgunionPK+index,&pk,sizeof(TUnionPK));

	UnionGetFullSystemDateTime(pgunionPKDB->lastUpdatedTime);

	if ((ret = UnionMirrorPKDBIntoDisk(pgunionPKDB,pgunionPK)) < 0)
		UnionUserErrLog("in UnionUpdatePKAttrInKeyDB:: UnionMirrorPKDBIntoDisk!\n");
	
	//UnionReleaseWritingLocks(pgunionPKDBMDL);
	UnionWritingUnlockPK(pgunionPK+index);
	UnionLogPKDBOperation(conRecordUpdate,&pk);	
	
	return(0);
}

// Wolfgang Wang, 2004/11/25
int UnionUpdateAllPKFieldsInKeyDB(PUnionPK pkey)
{
	int	ret;
	long	index=0;

#ifdef _KDBClient_
	if ((ret = UnionApplyKDBService(conUpdateRecordAllFlds,conIsPKDB,(unsigned char *)(pkey),sizeof(*pkey))) < 0)
	{
		UnionUserErrLog("in UnionUpdateAllPKFieldsInKeyDB:: UnionApplyKDBService !\n");
		return(ret);
	}
	return(ret);
#endif
	if (!UnionIsValidPK(pkey))
	{
		UnionUserErrLog("in UnionUpdateAllPKFieldsInKeyDB:: UnionIsValidPK [%s]\n",pkey->fullName);
		return(errCodeParameter);
	}
	
	if ((index = UnionFindPKPosInKeyDB(pkey->fullName)) < 0)
	{
		UnionUserErrLog("in UnionUpdateAllPKFieldsInKeyDB:: UnionFindPKPosInKeyDB [%s]!\n",pkey->fullName);
		return((int)index);
	}
	
	if ((ret = UnionWritingLockPK(pgunionPK+index)) < 0)
	{
		UnionUserErrLog("in UnionUpdateAllPKFieldsInKeyDB:: UnionWritingLockPK !\n");
		return(ret);
	}
	
	memcpy(pgunionPK+index,pkey,sizeof(TUnionPK));

	UnionGetFullSystemDateTime(pgunionPKDB->lastUpdatedTime);

	if ((ret = UnionMirrorPKDBIntoDisk(pgunionPKDB,pgunionPK)) < 0)
		UnionUserErrLog("in UnionUpdateAllPKFieldsInKeyDB:: UnionMirrorPKDBIntoDisk!\n");
	
	UnionWritingUnlockPK(pgunionPK+index);

	UnionLogPKDBOperation(conRecordUpdate,pkey);	
	
	return(0);
}

// Wolfgang Wang, 2004/11/25
int UnionExistPKInKeyDB(char *fullName)
{
	if (UnionFindPKPosInKeyDB(fullName) < 0)
		return(0);
	else
		return(1);
}

int UnionPrintPKInKeyDBUsedDaysToFile(long days,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	//long	tmpDays;
	
	if ((ret = UnionReadingLockPKTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBUsedDaysToFile:: UnionReadingLockPKTBL!\n");
		return(ret);
	}
	if (fp == NULL)
		fp = stdout;
		
	for (i = 0; i < pgunionPKDB->num; i++)
	{
		if (UnionDecideDaysBeforeToday((pgunionPK+i)->activeDate) < days)
			continue;
		totalNum++;
		fprintf(fp,"%30s %20s %8s %05ld\n",(pgunionPK+i)->fullName,(pgunionPK+i)->container,(pgunionPK+i)->activeDate,(pgunionPK+i)->maxEffectiveDays);
		if (((fp == stdout) || (fp == stderr)) && (totalNum % 20 == 0) && (totalNum != 0))
		{
			printf("Press any key to continue...\n");
			getchar();
			getchar();
		}
	}
	fprintf(fp,"totolNum = [%d]\n",totalNum);
	ret = 0;
	
exitRightNow:
	UnionReadingUnlockPKTBL();
	return(ret);
}

int UnionPrintPKInKeyDBUsedTimesToFile(long times,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	//long	tmpDays;
	
	if ((ret = UnionReadingLockPKTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBUsedTimesToFile:: UnionReadingLockPKTBL!\n");
		return(ret);
	}
	if (fp == NULL)
		fp = stdout;
	
	for (i = 0; i < pgunionPKDB->num; i++)
	{
		if (((pgunionPK+i)->useTimes < times) || ((pgunionPK+i)->useTimes < 0))
			continue;
		totalNum++;
		fprintf(fp,"%30s %20s %12ld %12ld\n",(pgunionPK+i)->fullName,(pgunionPK+i)->container,(pgunionPK+i)->useTimes,(pgunionPK+i)->maxUseTimes);
		if (((fp == stdout) || (fp == stderr)) && (totalNum % 20 == 0) && (totalNum != 0))
		{
			printf("Press any key to continue...\n");
			getchar();
			getchar();
		}
	}
	fprintf(fp,"totolNum = [%d]\n",totalNum);
	ret = 0;
	
exitRightNow:
	UnionReadingUnlockPKTBL();
	return(ret);
}

int UnionPrintPKOfContainerInKeyDBToFile(char *container,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	char	nameOfType[40];
		
	if ((ret = UnionReadingLockPKTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKOfContainerInKeyDBToFile:: UnionReadingLockPKTBL!\n");
		return(ret);
	}
	if (fp == NULL)
		fp = stdout;
		
	for (i = 0; i < pgunionPKDB->num; i++)
	{
		if (strcmp(container,(pgunionPK+i)->container) != 0)
			continue;
		totalNum++;
		memset(nameOfType,0,sizeof(nameOfType));
		UnionGetNameOfPKType((pgunionPK+i)->type,nameOfType);
		fprintf(fp,"%40s %20s %3s %04d\n",
			(pgunionPK+i)->fullName,(pgunionPK+i)->container,
			nameOfType,
			(pgunionPK+i)->length);
		if ((fp == stdout) && (totalNum % 23 == 0) && (totalNum != 0))
		{
			printf("Press any key to continue...\n");
			getchar();
			getchar();
		}
	}
	fprintf(fp,"totolNum = [%d]\n",totalNum);
	ret = 0;
	
	UnionReadingUnlockPKTBL();
	return(ret);
}

// 从密钥库中获取密钥容器
int UnionGetContainerOfSpecPK(char *keyName,char *container)
{
	TUnionPK	pk;
	int		ret;
	
	memset(&pk,0,sizeof(pk));
	if (strlen(keyName) >= sizeof(pk.fullName))
	{
		UnionUserErrLog("in UnionGetContainerOfSpecPK:: keyName [%s] error!\n",keyName);
		return(errCodeParameter);
	}
	strcpy(pk.fullName,keyName);
	if ((ret = UnionReadPKFromKeyDBAnyway(&pk)) < 0)
	{
		UnionUserErrLog("in UnionGetContainerOfSpecPK:: UnionReadPKFromKeyDBAnyway [%s] error!\n",keyName);
		return(ret);
	}
	strcpy(container,pk.container);
	return(0);
}

int UnionUnlockPKDB()
{
	int	ret;
	
	if ((ret = UnionWritingUnlockPKTBL()) < 0)
	{
		UnionUserErrLog("in UnionUnlockPKDB:: UnionWritingUnlockPKTBL!\n");
		return(ret);
	}
	if ((ret = UnionReadingUnlockPKTBL()) < 0)
	{
		UnionUserErrLog("in UnionUnlockPKDB:: UnionReadingUnlockPKTBL!\n");
		return(ret);
	}
	return(ret);
}

int UnionUnlockAllLockedPKInPKDB()
{
	int	ret;
	long	i;
	long	totalNum = 0;

	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionUnlockAllLockedPKInPKDB: UnionConnectPKDB!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionPKDB->num; i++)
	{
		if (!(pgunionPK+i)->isWritingLocked)
			continue;
		(pgunionPK+i)->isWritingLocked = 0;
		totalNum++;
	}
	if (totalNum > 0)
		UnionMirrorPKDBIntoDiskArtifically();
	return(totalNum);
}
	
int UnionUnlockPKInPKDB(char *keyName)
{
	long	index;
	
	if ((index = UnionFindPKPosInKeyDB(keyName)) < 0)
	{
		UnionUserErrLog("in UnionUnlockPKInPKDB:: UnionFindPKPosInKeyDB [%s]\n",keyName);
		return((int)index);
	}
	if ((pgunionPK+index)->isWritingLocked)
	{
		(pgunionPK+index)->isWritingLocked = 0;
		UnionMirrorPKDBIntoDiskArtifically();
		return(0);
	}
	else
		return(errCodePKDBMDL_KeyNotLocked);
}

int UnionPrintAllLockedPKInPKDBToFile(FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
		
	if ((ret = UnionReadingLockPKTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAllLockedPKInPKDBToFile:: UnionReadingLockPKTBL!\n");
		return(ret);
	}
	if (fp == NULL)
		fp = stdout;
	
	fprintf(fp,"Locked key::\n");	
	for (i = 0; i < pgunionPKDB->num; i++)
	{
		if (!(pgunionPK+i)->isWritingLocked)
			continue;
		totalNum++;
		fprintf(fp,"%40s\n",(pgunionPK+i)->fullName);
		if ((fp == stdout) && (totalNum % 23 == 0) && (totalNum != 0))
		{
			printf("Press any key to continue...\n");
			getchar();
			getchar();
		}
	}
	fprintf(fp,"totolNum = [%d]\n",totalNum);
	ret = 0;
	
	UnionReadingUnlockPKTBL();
	return(ret);
}


// 2009/1/18,王纯军
int UnionPrintAllPKToRecStrFile(FILE *fp)
{
	int	ret;
	long	i;
	int	realNum = 0;
	
	if ((ret = UnionReadingLockPKTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAllPKToRecStrFile:: UnionReadingLockPKTBL!\n");
		return(ret);
	}
	
	if (fp == NULL)
		fp = stdout;
	
	for (i = 0; i < pgunionPKDB->num; i++)
	{
		if ((ret = UnionPrintPKToRecStrTxtFile(pgunionPK+i,fp)) < 0)
		{
			UnionUserErrLog("in UnionPrintAllDesKeyToRecStrFile:: UnionPrintDesKeyToRecStrTxtFile [%04d] [%s]\n",i,(pgunionPK+i)->fullName);
			continue;
		}
		UnionLog("in UnionPrintAllPKToRecStrFile:: UnionPrintPKToRecStrTxtFile [%s] OK\n",(pgunionPK+i)->fullName);
		realNum++;
	}
	// UnionReadingUnlockDesKeyTBL();
	UnionReadingUnlockPKTBL();
	
	return(realNum);
}

// 2009/1/18,王纯军
int UnionOutputAllPKToRecStrFile(char *fileName)
{
	FILE	*fp;
	int	realNum = 0;
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionOutputAllPKToRecStrFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	realNum = UnionPrintAllPKToRecStrFile(fp);
	fflush(fp);
	fclose(fp);
	return(realNum);
}

// 2007/11/29, Added by Wolfgang Wang
long UnionCompareAllPKValueInKeyDBWithBrothers()
{
	int	ret;
	long	i;
	long	checkErrNum = 0;

	if ((ret = UnionReadingLockPKTBL()) < 0)
	{
		UnionUserErrLog("in UnionCompareAllPKValueInKeyDBWithBrothers:: UnionReadingLockPKTBL !\n");
		return(ret);
	}

	for (i = 0; i < pgunionPKDB->num; i++)
	{
		if ((ret = UnionComparePKValueWithBrothers((pgunionPK+i)->fullName,(pgunionPK+i)->value)) < 0)
		{
			UnionUserErrLog("check Err::[%s]\n",(pgunionPK+i)->fullName);
			printf("check Err::[%s]\n",(pgunionPK+i)->fullName);
			checkErrNum++;
		}
		else
		{
			UnionAuditLog("check OK ::[%s]\n",(pgunionPK+i)->fullName);
			printf("check OK ::[%s]\n",(pgunionPK+i)->fullName);
		}
	}
	ret = checkErrNum;

	UnionReadingUnlockPKTBL();
	return(ret);
}

int UnionCheckAllUnsamePKInImageAndFileToFp(FILE *fp)
{
	return(0);
}
