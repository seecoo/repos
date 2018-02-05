//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2003/09/08

// 	Version:	2.0

//	2003/09/24, 在1.0基础上升级为2.0

//	2004/11/20, 增加了一些与备份服务器相关的函数

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

#include "unionDesKeyDB.h"
#include "unionDesKeyDBLog.h"

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
	pgunionDesKeyDBMDL->readingLocks = 0;
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
	pgunionDesKeyDBMDL->writingLocks = 0;
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
	return(pgunionDesKeyDBMDL->readingLocks);
}

int UnionGetWritingLocksOfDesKeyDB()
{
	int	ret;
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionGetWritingLocksOfDesKeyDB:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	return(pgunionDesKeyDBMDL->writingLocks);
}
// 20070611 增加结束

int UnionGetNameOfDesKeyDB(char *fileName)
{
	sprintf(fileName,"%s/unionDesKeyDB.KDB",getenv("UNIONETC"));
	return(0);
}

long UnionGetMaxKeyNumFromKeyDBFile()
{
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

	if ((maxKeyNum = UnionGetMaxKeyNumFromKeyDBFile()) <= 0)
	{
		UnionUserErrLog("in UnionConnectDesKeyDB:: UnionGetMaxKeyNumFromKeyDBFile!\n");
		return((int)maxKeyNum);
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
		UnionRemoveDesKeyDBInMemory();
		return(errCodeSharedMemoryModule);
	}
	
	if ((pgunionDesKey = (PUnionDesKey)((unsigned char *)pgunionDesKeyDB + sizeof(*pgunionDesKeyDB))) == NULL)
	{
		UnionUserErrLog("in UnionConnectDesKeyDB:: PUnionDesKey!\n");
		UnionRemoveDesKeyDBInMemory();
		return(errCodeSharedMemoryModule);
	}
	pgunionDesKeyDB->pkey = pgunionDesKey;
	
	//UnionNullLog("in UnionConnectDesKeyDB::pgunionDesKeyDB = [%0x]\n",pgunionDesKeyDB);
	if (UnionIsNewCreatedSharedMemoryModule(pgunionDesKeyDBMDL))
	{
		pgunionDesKeyDB->num = 0;
		pgunionDesKeyDB->maxKeyNum = maxKeyNum;
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
		
	return(UnionMirrorDesKeyDBIntoDisk(&rec,NULL));
}

int UnionDeleteDesKeyDB()
{
	char			fileName[256];
	char			cmd[256];
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfDesKeyDB(fileName);
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"rm %s",fileName);
	
	return(system(cmd));
}	

int UnionMirrorDesKeyDBIntoDiskArtifically()
{
	int	ret;
	
	if ((ret = UnionConnectExistingDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionMirrorDesKeyDBIntoDiskArtifically:: UnionConnectExistingDesKeyDB!\n");
		return(ret);
	}

	return(UnionMirrorDesKeyDBIntoDisk(pgunionDesKeyDB,pgunionDesKey));
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

	return(0);
}

int UnionLoadDesKeyDBIntoMemory()
{
	int	ret;
	FILE	*fp;
	char	fileName[256];
	TUnionDesKeyDB	keyDB;
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionLoadDesKeyDBIntoMemory:: UnionConnectDesKeyDB!\n");
		return(ret);
	}

	if ((ret = UnionApplyWritingLocks(pgunionDesKeyDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionLoadDesKeyDBIntoMemory:: UnionApplyWritingLocks !\n");
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
	pgunionDesKeyDB->num = keyDB.num;
	memcpy(pgunionDesKeyDB->lastUpdatedTime,keyDB.lastUpdatedTime,sizeof(keyDB.lastUpdatedTime));

	if ((pgunionDesKeyDB->num = keyDB.num) > 0)
	{
		if (pgunionDesKey == NULL)
		{
			UnionUserErrLog("in UnionLoadDesKeyDBIntoMemory:: pkey is NULL!\n");
			ret = errCodeDesKeyDBMDL_KeyDBNonConnected;
			goto exitRightNow;
		}
		if (pgunionDesKeyDB->num > pgunionDesKeyDB->maxKeyNum)
		{
			UnionUserErrLog("in UnionLoadDesKeyDBIntoMemory:: pgunionDesKeyDB->num [%d] > pgunionDesKeyDB->maxKeyNum [%d]\n",pgunionDesKeyDB->num,pgunionDesKeyDB->maxKeyNum);
			ret = errCodeDesKeyDBMDL_KeyNum;
			goto exitRightNow;
		}
		fread(pgunionDesKey,sizeof(TUnionDesKey),pgunionDesKeyDB->num,fp);
	}
	ret = 0;
	
exitRightNow:
	UnionReleaseWritingLocks(pgunionDesKeyDBMDL);
	fclose(fp);

	return(ret);
}

int UnionPrintDesKeyDBToFile(PUnionDesKeyDB pdesKeyDB,FILE *fp)
{
	int	ret;
	long	i;
	char	nameOfType[20];
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyDBToFile:: UnionConnectDesKeyDB!\n");
		return(ret);
	}

	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKeyDBToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
	/*
	if ((ret = UnionApplyReadingLocks(pdesKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyDBToFile:: UnionApplyReadingLocks !\n");
		return(ret);
	}
	*/
	
	if (pgunionDesKey == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKeyDBToFile:: pkey is NULL!\n");
		ret = errCodeParameter;
		goto exitRightNow;
	}
		
	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		memset(nameOfType,0,sizeof(nameOfType));
		UnionGetNameOfDesKeyType((pgunionDesKey+i)->type,nameOfType);
		fprintf(fp,"%40s %20s %3s %03d\n",
			(pgunionDesKey+i)->fullName,(pgunionDesKey+i)->container,
			nameOfType,
			UnionGetDesKeyBitsLength((pgunionDesKey+i)->length));
		if (((fp == stdout) || (fp == stderr)) && (i % 20 == 0) && (i != 0))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press enter to continue or exit/quit to exit")))
				return(0);
		}
	}
	fprintf(fp,"totolKeyNum = [%d]\n",pgunionDesKeyDB->num);
	fprintf(fp,"maxKeyNum = [%d]\n",pgunionDesKeyDB->maxKeyNum);
	ret = 0;
	
exitRightNow:
	//UnionReleaseReadingLocks(pdesKeyDB);
	return(ret);
}

int UnionPrintDesKeyDB()
{
	int	ret;
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyDB:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	UnionPrintDesKeyDBToFile(pgunionDesKeyDB,stdout);
	UnionPrintSharedMemoryModuleToFile(pgunionDesKeyDBMDL,stdout);
	return(0);
}

int UnionPrintAllDesKeysInKeyDBToFile(FILE *fp)
{
	int	ret;
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintAllDesKeysInKeyDBToFile:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	return(UnionPrintDesKeyDBToFile(pgunionDesKeyDB,fp));
}

int UnionPrintStatusOfDesKeyDBToFile(FILE *fp)
{
	int	ret;
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintStatusOfDesKeyDBToFile:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	if (fp == NULL)
		return(errCodeParameter);
	fprintf(fp,"totolKeyNum = [%d]\n",pgunionDesKeyDB->num);
	fprintf(fp,"maxKeyNum = [%d]\n",pgunionDesKeyDB->maxKeyNum);
	fprintf(fp,"lastUpdatedTime = [%s]\n",pgunionDesKeyDB->lastUpdatedTime);
	return(UnionPrintSharedMemoryModuleToFile(pgunionDesKeyDBMDL,fp));
}

int UnionInsertDesKeyIntoKeyDB(PUnionDesKey pkey)
{
	int	ret;
	long    index;
	int	check;
	int	pos;
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	
	if (!UnionIsValidDesKey(pkey))
	{
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: UnionIsValidDesKey [%s]\n",pkey->fullName);
		return(errCodeParameter);
	}
	
	if ((index = UnionFindDesKeyPosInKeyDB(pkey->fullName)) >= 0)
	{
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: [%s] already exists!\n",pkey->fullName);
		return(errCodeDesKeyDBMDL_KeyAlreadyExists);
	}
	pkey->useTimes = 0;
	time(&(pkey->lastUpdateTime));

	if (pgunionDesKeyDB->num == pgunionDesKeyDB->maxKeyNum)
	{
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: DesKeyDB is full!\n");
		return(errCodeDesKeyDBMDL_KeyDBFull);
	}
	
	if (pgunionDesKey == NULL)
	{
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: pkey is NULL!\n");
		return(errCodeDesKeyDBMDL_KeyDBNonConnected);
	}
	
	if ((ret = UnionApplyWritingLocks(pgunionDesKeyDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: UnionApplyWritingLocks !\n");
		return(ret);
	}

	memcpy(pgunionDesKey+pgunionDesKeyDB->num,pkey,sizeof(TUnionDesKey));
	pgunionDesKeyDB->num += 1;
		
	UnionLogDesKeyDBOperation(conRecordInsert,pkey);
	UnionGetFullSystemDateTime(pgunionDesKeyDB->lastUpdatedTime);
	if ((ret = UnionMirrorDesKeyDBIntoDisk(pgunionDesKeyDB,pgunionDesKey)) < 0)
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: UnionMirrorDesKeyDBIntoDisk!\n");
	
exitRightNow:
	UnionReleaseWritingLocks(pgunionDesKeyDBMDL);
	return(ret);
}

int UnionDeleteDesKeyFromKeyDB(char *fullName)
{
	int	ret;
	long    index;
	int	pos;
	TUnionDesKey	tmpDesKey;
		
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionDeleteDesKeyFromKeyDB:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	
	if ((ret = UnionApplyWritingLocks(pgunionDesKeyDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionDeleteDesKeyFromKeyDB:: UnionApplyWritingLocks !\n");
		return(ret);
	}
	
	for (index = 0; index < pgunionDesKeyDB->num; index++)
	{
		if (strcmp(fullName,(pgunionDesKey + index)->fullName) == 0)
			break;
	}
	if ((index == pgunionDesKeyDB->num) || (pgunionDesKeyDB->num <= 0))
	{
		ret = errCodeDesKeyDBMDL_KeyNonExists;
		goto exitRightNow;
	}
	memcpy(&tmpDesKey,pgunionDesKey+index,sizeof(tmpDesKey));
	UnionLog("in UnionDeleteDesKeyFromKeyDB:: %s\n",(pgunionDesKey + (pgunionDesKeyDB->num - 1))->fullName);
	memcpy(pgunionDesKey+index,pgunionDesKey + (pgunionDesKeyDB->num - 1),sizeof(TUnionDesKey));
	pgunionDesKeyDB->num--;
	UnionLogDesKeyDBOperation(conRecordDelete,&tmpDesKey);
	UnionGetFullSystemDateTime(pgunionDesKeyDB->lastUpdatedTime);
	if ((ret = UnionMirrorDesKeyDBIntoDisk(pgunionDesKeyDB,pgunionDesKey)) < 0)
		UnionUserErrLog("in UnionDeleteDesKeyFromKeyDB:: UnionMirrorDesKeyDBIntoDisk!\n");
	
exitRightNow:
	UnionReleaseWritingLocks(pgunionDesKeyDBMDL);
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
		return(index);
	}
	
	if ((ret = UnionApplyReadingLocks(pgunionDesKeyDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromKeyDBAnyway:: UnionApplyReadingLocks !\n");
		return(ret);
	}
	
	memcpy(pkey,pgunionDesKey+index,sizeof(TUnionDesKey));

	UnionReleaseReadingLocks(pgunionDesKeyDBMDL);

	return(0);
}

int UnionUseDesKeyFromKeyDB(PUnionDesKey pkey)
{
	int	ret;
	long	index=0;

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionUseDesKeyFromKeyDB:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	
	if (pkey == NULL)
	{
		UnionUserErrLog("in UnionUseDesKeyFromKeyDB:: null Pointer!\n");
		return(errCodeParameter);
	}
	
	if ((index = UnionFindDesKeyPosInKeyDB(pkey->fullName)) < 0)
	{
		UnionUserErrLog("in UnionUseDesKeyFromKeyDB:: UnionFindDesKeyPosInKeyDB [%s]!\n",pkey->fullName);
		return(index);
	}
	
	if ((ret = UnionApplyReadingLocks(pgunionDesKeyDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionUseDesKeyFromKeyDB:: UnionApplyReadingLocks !\n");
		return(ret);
	}
	
	if (!UnionIsDesKeyStillEffective(pgunionDesKey+index))
	{
		UnionReleaseReadingLocks(pgunionDesKeyDBMDL);
		return(errDesKeyNotEffective);
	}
	
	(pgunionDesKey+index)->useTimes += 1;
	
	memcpy(pkey,pgunionDesKey+index,sizeof(TUnionDesKey));

	UnionReleaseReadingLocks(pgunionDesKeyDBMDL);

	return(0);
}

int UnionReadDesKeyFromKeyDB(PUnionDesKey pkey)
{
	return(UnionUseDesKeyFromKeyDB(pkey));
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
	int	ret;
	long	index=0;

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionUpdateDesKeyValueInKeyDB:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	
	if ((index = UnionFindDesKeyPosInKeyDB(fullName)) < 0)
	{
		UnionUserErrLog("in UnionUpdateDesKeyValueInKeyDB:: UnionFindDesKeyPosInKeyDB [%s]!\n",fullName);
		return(ret);
	}
	
	if ((ret = UnionApplyWritingLocks(pgunionDesKeyDBMDL)) < 0)
	//if ((ret = UnionApplyReadingLocks(pgunionDesKeyDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionUpdateDesKeyValueInKeyDB:: UnionApplyWritingLocks !\n");
		return(ret);
	}
	
	if ((ret = UnionUpdateDesKeyValue(pgunionDesKey+index,value,checkValue)) < 0)
	{
		UnionUserErrLog("in UnionUpdateDesKeyValueInKeyDB:: UnionUpdateDesKeyValue [%s]\n",fullName);
		UnionReleaseWritingLocks(pgunionDesKeyDBMDL);
		return(ret);
	}
	
	UnionLogDesKeyDBOperation(conRecordUpdate,pgunionDesKey+index);	

	UnionGetFullSystemDateTime(pgunionDesKeyDB->lastUpdatedTime);

	if ((ret = UnionMirrorDesKeyDBIntoDisk(pgunionDesKeyDB,pgunionDesKey)) < 0)
		UnionUserErrLog("in UnionUpdateDesKeyValueInKeyDB:: UnionMirrorDesKeyDBIntoDisk!\n");
	
	UnionReleaseWritingLocks(pgunionDesKeyDBMDL);
	//UnionReleaseReadingLocks(pgunionDesKeyDBMDL);
	
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
	
	if ((ret = UnionApplyReadingLocks(pgunionDesKeyDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionFindDesKeyPosInKeyDB:: UnionApplyReadingLocks !\n");
		return(ret);
	}
	//UnionDebugLog("in UnionFindDesKeyPosInKeyDB:: [%s] [%d]\n",fullName,pgunionDesKeyDB->num);
	for (index = 0; index < pgunionDesKeyDB->num; index++)
	{
		if (strcmp(fullName,(pgunionDesKey+index)->fullName) == 0)
		{
			UnionReleaseReadingLocks(pgunionDesKeyDBMDL);
			return(index);
		}
	}		
	UnionReleaseReadingLocks(pgunionDesKeyDBMDL);
	return(errCodeDesKeyDBMDL_KeyNonExists);
}

int UnionGetKeyNumOfKeyDB()
{
	int	ret;
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionGetKeyNumOfKeyDB:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	
	return(pgunionDesKeyDB->num);
}

char *UnionGetDesKeyDBLastUpdatedTime()
{
	int	ret;
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionGetDesKeyDBLastUpdatedTime:: UnionConnectDesKeyDB!\n");
		return(NULL);
	}
	
	return(pgunionDesKeyDB->lastUpdatedTime);
}

int UnionChangeSizeOfDesKeyDB(long newMaxKeyNum)
{
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
}

int UnionPrintDesKeyExpectedInKeyDBToFile(PUnionDesKeyDB pdesKeyDB,char *app,char *owner,char *name,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	short	appEQExpected=0,ownerEQExpected=0,nameEQExpected=0;
	char	thisApp[40+1],thisOwner[40+1],thisName[40+1];
	char	nameOfType[20];
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyExpectedInKeyDBToFile:: UnionConnectDesKeyDB!\n");
		return(ret);
	}

	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKeyExpectedInKeyDBToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
	/*
	if ((ret = UnionApplyReadingLocks(pdesKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyExpectedInKeyDBToFile:: UnionApplyReadingLocks !\n");
		return(ret);
	}
	*/
	
	if (pgunionDesKey == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKeyExpectedInKeyDBToFile:: pkey is NULL!\n");
		ret = errCodeDesKeyDBMDL_KeyDBNonConnected;
		goto exitRightNow;
	}
	
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
	//UnionReleaseReadingLocks(pdesKeyDB);
	return(ret);
}

int UnionPrintDesKeyExpectedToFile(char *app,char *owner,char *name,FILE *fp)
{
	int	ret;
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyExpectedToFile:: UnionConnectDesKeyDB!\n");
		return(ret);
	}

	return(UnionPrintDesKeyExpectedInKeyDBToFile(pgunionDesKeyDB,app,owner,name,fp));
}

int UnionPrintDesKeyInKeyDBOutofDateToFile(PUnionDesKeyDB pdesKeyDB,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBOutofDateToFile:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBOutofDateToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
	if (pgunionDesKey == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBOutofDateToFile:: pkey is NULL!\n");
		ret = errCodeDesKeyDBMDL_KeyDBNonConnected;
		goto exitRightNow;
	}
	
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
	//UnionReleaseReadingLocks(pdesKeyDB);
	return(ret);
}

int UnionPrintDesKeyOutofDateToFile(FILE *fp)
{
	int	ret;
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyOutofDateToFile:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	return(UnionPrintDesKeyInKeyDBOutofDateToFile(pgunionDesKeyDB,fp));
}

int UnionPrintDesKeyInKeyDBNearOutofDateToFile(PUnionDesKeyDB pdesKeyDB,long days,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	long	tmpDays;
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBNearOutofDateToFile:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBNearOutofDateToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
	if (pgunionDesKey == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBNearOutofDateToFile:: pkey is NULL!\n");
		ret = errCodeDesKeyDBMDL_KeyDBNonConnected;
		goto exitRightNow;
	}
	
	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		if ((tmpDays = UnionDecideDaysBeforeToday((pgunionDesKey+i)->activeDate) + days) < (pgunionDesKey+i)->maxEffectiveDays)
			continue;
		totalNum++;
		fprintf(fp,"%35s %8s %05ld\n",(pgunionDesKey+i)->fullName,(pgunionDesKey+i)->activeDate,(pgunionDesKey+i)->maxEffectiveDays);
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
	//UnionReleaseReadingLocks(pdesKeyDB);
	return(ret);
}

int UnionPrintDesKeyNearOutofDateToFile(long days,FILE *fp)
{
	int	ret;
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyNearOutofDateToFile:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	return(UnionPrintDesKeyInKeyDBNearOutofDateToFile(pgunionDesKeyDB,days,fp));
}

int UnionPrintDesKeyInKeyDBNearOutofMaxUseTimesToFile(PUnionDesKeyDB pdesKeyDB,long times,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBNearOutofMaxUseTimesToFile:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBNearOutofMaxUseTimesToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
	if (pgunionDesKey == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBNearOutofMaxUseTimesToFile:: pkey is NULL!\n");
		ret = errCodeDesKeyDBMDL_KeyDBNonConnected;
		goto exitRightNow;
	}
	
	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		if ( (pgunionDesKey+i)->useTimes + times < (pgunionDesKey+i)->maxUseTimes)
			continue;
		totalNum++;
		fprintf(fp,"%35s %05ld %05ld\n",(pgunionDesKey+i)->fullName,(pgunionDesKey+i)->useTimes,(pgunionDesKey+i)->maxUseTimes);
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
	//UnionReleaseReadingLocks(pdesKeyDB);
	return(ret);
}

int UnionPrintDesKeyNearOutofMaxUseTimesToFile(long times,FILE *fp)
{
	int	ret;
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyNearOutofMaxUseTimesToFile:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	return(UnionPrintDesKeyInKeyDBNearOutofMaxUseTimesToFile(pgunionDesKeyDB,times,fp));
}



// 2004/11/20, Added by Wolfgang Wang
int UnionSynchronizeSpecifiedDesKeyToBackuper(char *fullName)
{
	long	index=0;
	int	ret;

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeSpecifiedDesKeyToBackuper:: UnionConnectDesKeyDB!\n");
		return(ret);
	}

	if ((index = UnionFindDesKeyPosInKeyDB(fullName)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeSpecifiedDesKeyToBackuper:: UnionFindDesKeyPosInKeyDB! [%s]\n",fullName);
		return((int)index);
	}

	if ((ret = UnionApplyWritingLocks(pgunionDesKeyDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeAllDesKeyToBackuper:: UnionApplyWritingLocks !\n");
		return(ret);
	}

	if ((ret = UnionSynchronizeDesKeyDBOperation(conRecordUpdate,pgunionDesKey+index)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeAllDesKeyToBackuper:: UnionSynchronizeDesKeyDBOperation !\n");
	}
	
	UnionReleaseWritingLocks(pgunionDesKeyDBMDL);
	
	return(ret);
}

// 2004/11/20, Added by Wolfgang Wang
long UnionSynchronizeAllDesKeyToBackuper()
{
	int	ret;
	long	i;
	long	synchronizedNum = 0;
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeAllDesKeyToBackuper:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
		
	if ((ret = UnionApplyWritingLocks(pgunionDesKeyDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeAllDesKeyToBackuper:: UnionApplyWritingLocks !\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		if ((ret = UnionSynchronizeDesKeyDBOperation(conRecordUpdate,pgunionDesKey+i)) < 0)
			UnionUserErrLog("in UnionSynchronizeAllDesKeyToBackuper:: UnionSynchronizeDesKeyDBOperation !\n");
		else
			synchronizedNum++;
	}
	ret = synchronizedNum;
	
exitRightNow:
	UnionReleaseWritingLocks(pgunionDesKeyDBMDL);
	return(ret);
}

// 2004/11/20, Added by Wolfgang Wang
long UnionSynchronizeDesKeyOfSpecifiedAppToBackuper(char *appName)
{
	int	ret;
	long	i;
	char	thisApp[40+1],thisOwner[40+1],thisName[40+1];
	long	synchronizedNum = 0;
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeDesKeyOfSpecifiedAppToBackuper:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
		
	if ((ret = UnionApplyWritingLocks(pgunionDesKeyDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeDesKeyOfSpecifiedAppToBackuper:: UnionApplyWritingLocks !\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		memset(thisApp,0,sizeof(thisApp));
		UnionAnalysisDesKeyFullName((pgunionDesKey+i)->fullName,thisApp,thisOwner,thisName);
		if (strcmp(appName,thisApp) != 0)
			continue;
		if ((ret = UnionSynchronizeDesKeyDBOperation(conRecordUpdate,pgunionDesKey+i)) < 0)
			UnionUserErrLog("in UnionSynchronizeDesKeyOfSpecifiedAppToBackuper:: UnionSynchronizeDesKeyDBOperation !\n");
		else
			synchronizedNum++;
	}
	ret = synchronizedNum;
	
exitRightNow:
	UnionReleaseWritingLocks(pgunionDesKeyDBMDL);
	return(ret);
}

// added by Wolfgang Wang, 2004/11/20
int UnionDesKeyDBOperation(TUnionKeyDBOperation operation,PUnionDesKey pdesKey)
{
	int	ret = -1;
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionDesKeyDBOperation:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	
	switch (operation)
	{
		case	conInsertRecord:
			if ((ret = UnionInsertDesKeyIntoKeyDB(pdesKey)) < 0)
				UnionUserErrLog("in UnionDesKeyDBOperation:: UnionInsertDesKeyIntoKeyDB [%s]\n",pdesKey->fullName);
			else
				UnionSuccessLog("in UnionDesKeyDBOperation:: UnionInsertDesKeyIntoKeyDB [%s] OK\n",pdesKey->fullName);
			break;
		case	conUpdateRecord:
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
		default:
			UnionUserErrLog("in UnionDesKeyDBOperation::Unknown operation!");
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

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionUpdateDesKeyAttrInKeyDB:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	
	if (pkey == NULL)
	{
		UnionUserErrLog("in UnionUpdateDesKeyAttrInKeyDB:: null Pointer!\n");
		return(errCodeParameter);
	}
	
	memset(&desKey,0,sizeof(desKey));
	memcpy(&desKey,pkey,sizeof(desKey));
	memset(desKey.value,0,sizeof(desKey.value));	
	if (!UnionIsValidDesKey(pkey))
	{
		UnionUserErrLog("in UnionUpdateDesKeyAttrInKeyDB:: UnionIsValidDesKey [%s]\n",pkey->fullName);
		return(errCodeParameter);
	}
	
	if ((index = UnionFindDesKeyPosInKeyDB(pkey->fullName)) < 0)
	{
		UnionUserErrLog("in UnionUpdateDesKeyAttrInKeyDB:: UnionFindDesKeyPosInKeyDB [%s]!\n",pkey->fullName);
		return(ret);
	}
	
	if ((ret = UnionApplyWritingLocks(pgunionDesKeyDBMDL)) < 0)
	//if ((ret = UnionApplyReadingLocks(pgunionDesKeyDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionUpdateDesKeyAttrInKeyDB:: UnionApplyWritingLocks !\n");
		return(ret);
	}
	
	memcpy(desKey.value,(pgunionDesKey+index)->value,sizeof(desKey.value));
	memcpy(desKey.oldValue,(pgunionDesKey+index)->oldValue,sizeof(desKey.oldValue));
	memcpy(desKey.checkValue,(pgunionDesKey+index)->checkValue,sizeof(desKey.checkValue));
	memcpy(desKey.oldCheckValue,(pgunionDesKey+index)->oldCheckValue,sizeof(desKey.oldCheckValue));

	desKey.useTimes = (pgunionDesKey+index)->useTimes;
	desKey.lastUpdateTime = (pgunionDesKey+index)->lastUpdateTime;
	memcpy(pgunionDesKey+index,&desKey,sizeof(TUnionDesKey));
	UnionLogDesKeyDBOperation(conRecordUpdate,&desKey);	

	UnionGetFullSystemDateTime(pgunionDesKeyDB->lastUpdatedTime);

	if ((ret = UnionMirrorDesKeyDBIntoDisk(pgunionDesKeyDB,pgunionDesKey)) < 0)
		UnionUserErrLog("in UnionUpdateDesKeyAttrInKeyDB:: UnionMirrorDesKeyDBIntoDisk!\n");
	
	UnionReleaseWritingLocks(pgunionDesKeyDBMDL);
	//UnionReleaseReadingLocks(pgunionDesKeyDBMDL);
	
	return(0);
}

// Wolfgang Wang, 2004/11/25
int UnionUpdateAllDesKeyFieldsInKeyDB(PUnionDesKey pkey)
{
	int	ret;
	long	index=0;

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionUpdateAllDesKeyFieldsInKeyDB:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	
	if (pkey == NULL)
	{
		UnionUserErrLog("in UnionUpdateAllDesKeyFieldsInKeyDB:: null Pointer!\n");
		return(errCodeParameter);
	}
	
	if (!UnionIsValidDesKey(pkey))
	{
		UnionUserErrLog("in UnionUpdateAllDesKeyFieldsInKeyDB:: UnionIsValidDesKey [%s]\n",pkey->fullName);
		return(errCodeParameter);
	}
	
	if ((index = UnionFindDesKeyPosInKeyDB(pkey->fullName)) < 0)
	{
		UnionUserErrLog("in UnionUpdateAllDesKeyFieldsInKeyDB:: UnionFindDesKeyPosInKeyDB [%s]!\n",pkey->fullName);
		return(ret);
	}
	
	//if ((ret = UnionApplyWritingLocks(pgunionDesKeyDBMDL)) < 0)
	if ((ret = UnionApplyReadingLocks(pgunionDesKeyDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionUpdateAllDesKeyFieldsInKeyDB:: UnionApplyWritingLocks !\n");
		return(ret);
	}
	
	memcpy(pgunionDesKey+index,pkey,sizeof(TUnionDesKey));
	UnionLogDesKeyDBOperation(conRecordUpdate,pkey);	

	UnionGetFullSystemDateTime(pgunionDesKeyDB->lastUpdatedTime);

	if ((ret = UnionMirrorDesKeyDBIntoDisk(pgunionDesKeyDB,pgunionDesKey)) < 0)
		UnionUserErrLog("in UnionUpdateAllDesKeyFieldsInKeyDB:: UnionMirrorDesKeyDBIntoDisk!\n");
	
	//UnionReleaseWritingLocks(pgunionDesKeyDBMDL);
	UnionReleaseReadingLocks(pgunionDesKeyDBMDL);
	
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
		
	if ((fp == NULL) || ((ret = UnionConnectDesKeyDB()) < 0))
	{
		UnionUserErrLog("in UnionPrintDesKeyOfContainerInKeyDBToFile:: fp is NULL or UnionConnectDesKeyDB!\n");
		return(errCodeParameter);
	}
	
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
	//UnionReleaseReadingLocks(pdesKeyDB);
	return(ret);
}

int UnionPrintDesKeyInKeyDBUsedDaysToFile(long days,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	//long	tmpDays;
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBUsedDaysToFile:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBUsedDaysToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		if (UnionDecideDaysBeforeToday((pgunionDesKey+i)->activeDate) < days)
			continue;
		totalNum++;
		fprintf(fp,"%40s %20s %8s %05ld\n",(pgunionDesKey+i)->fullName,(pgunionDesKey+i)->container,(pgunionDesKey+i)->activeDate,(pgunionDesKey+i)->maxEffectiveDays);
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
	//UnionReleaseReadingLocks(pdesKeyDB);
	return(ret);
}

int UnionPrintDesKeyInKeyDBUsedTimesToFile(long times,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	//long	tmpDays;
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBUsedTimesToFile:: UnionConnectDesKeyDB!\n");
		return(ret);
	}
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBUsedTimesToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		if (((pgunionDesKey+i)->useTimes < times) || ((pgunionDesKey+i)->useTimes < 0))
			continue;
		totalNum++;
		fprintf(fp,"%40s %20s %12ld %12ld\n",(pgunionDesKey+i)->fullName,(pgunionDesKey+i)->container,(pgunionDesKey+i)->useTimes,(pgunionDesKey+i)->maxUseTimes);
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
	//UnionReleaseReadingLocks(pdesKeyDB);
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
