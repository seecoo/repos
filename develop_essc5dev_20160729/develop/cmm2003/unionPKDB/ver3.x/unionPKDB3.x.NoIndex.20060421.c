//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2004/11/11

#define _UnionLogMDL_3_x_

#ifndef _UnionPK_3_x_
#define _UnionPK_3_x_
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
	pgunionPKDBMDL->readingLocks = 0;
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
	pgunionPKDBMDL->writingLocks = 0;
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
	return(pgunionPKDBMDL->readingLocks);
}

int UnionGetWritingLocksOfPKDB()
{
	int	ret;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionGetWritingLocksOfPKDB:: UnionConnectPKDB!\n");
		return(ret);
	}
	return(pgunionPKDBMDL->writingLocks);
}
// 20070611 Ôö¼Ó½áÊø

int UnionIsPKDBConnected()
{
	if ((!UnionIsSharedMemoryInited(conMDLNameOfUnionPKDB)) || (pgunionPKDB == NULL) || (pgunionPK == NULL))
		return(0);
	else
		return(1);
}

int UnionGetNameOfPKDB(char *fileName)
{
	sprintf(fileName,"%s/unionPKDB.KDB",getenv("UNIONETC"));
	return(0);
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

int UnionMirrorPKDBIntoDisk(PUnionPKDB pdesKeyDB,PUnionPK pkey)
{
	FILE			*fp;
	char			fileName[256];

	if (pdesKeyDB == NULL)
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
	
	fwrite(pdesKeyDB,sizeof(*pdesKeyDB),1,fp);
	if (pdesKeyDB->num < 0)
	{
		UnionUserErrLog("in UnionMirrorPKDBIntoDisk:: pdesKeyDB->num [%d] Error!\n",pdesKeyDB->num);
		fclose(fp);
		return(errCodePKDBMDL_KeyNum);
	}
	
	if ((pdesKeyDB->num == 0) || (pkey == NULL))
	{
		fclose(fp);
		return(0);
	}
	
	fwrite(pkey,sizeof(TUnionPK),pdesKeyDB->num,fp);	
	
	fclose(fp);

	return(0);
}

int UnionLoadPKDBIntoMemory()
{
	int	ret;
	FILE	*fp;
	char	fileName[256];
	TUnionPKDB	keyDB;
	
	//UnionNullLog("in UnionLoadPKDBIntoMemory::pgunionPKDB = [%0x]\n",pgunionPKDB);

	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionLoadPKDBIntoMemory:: UnionConnectPKDB !\n");
		return(ret);
	}
		
	if ((ret = UnionApplyWritingLocks(pgunionPKDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionLoadPKDBIntoMemory:: UnionApplyWritingLocks !\n");
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
	}
	ret = 0;
	
exitRightNow:
	UnionReleaseWritingLocks(pgunionPKDBMDL);
	fclose(fp);

	return(ret);
}

int UnionPrintPKDBToFile(PUnionPKDB pdesKeyDB,FILE *fp)
{
	int	ret;
	long	i;
	char	nameOfType[40];
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKDBToFile:: UnionConnectPKDB !\n");
		return(ret);
	}
		
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintPKDBToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
	for (i = 0; i < pgunionPKDB->num; i++)
	{
		memset(nameOfType,0,sizeof(nameOfType));
		UnionGetNameOfPKType((pgunionPK+i)->type,nameOfType);
		fprintf(fp,"%30s %20s %20s %04d\n",
			(pgunionPK+i)->fullName,
			(pgunionPK+i)->container,
			nameOfType,
			UnionGetBitsLengthOfRSAPair((pgunionPK+i)->length));
		if (((fp == stdout) || (fp == stderr)) && (i % 20 == 0) && (i != 0))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press enter to continue or exit/quit to exit")))
				return(0);
		}
	}
	fprintf(fp,"totolKeyNum = [%d]\n",pgunionPKDB->num);
	fprintf(fp,"maxKeyNum = [%d]\n",pgunionPKDB->maxKeyNum);
	ret = 0;
	
exitRightNow:
	//UnionReleaseReadingLocks(pdesKeyDB);
	return(ret);
}

int UnionPrintPKDB()
{
	int	ret;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKDB:: UnionConnectPKDB !\n");
		return(ret);
	}
		
	UnionPrintPKDBToFile(pgunionPKDB,stdout);
	UnionPrintSharedMemoryModuleToFile(pgunionPKDBMDL,stdout);
	return(0);
}

int UnionPrintAllPKsInKeyDBToFile(FILE *fp)
{
	int	ret;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintAllPKsInKeyDBToFile:: UnionConnectPKDB !\n");
		return(ret);
	}
		
	return(UnionPrintPKDBToFile(pgunionPKDB,fp));
}

int UnionPrintStatusOfPKDBToFile(FILE *fp)
{
	int	ret;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintStatusOfPKDBToFile:: UnionLoadPKDBIntoMemory !\n");
		return(ret);
	}
	if (fp == NULL)
		return(errCodeParameter);
	fprintf(fp,"totolKeyNum = [%d]\n",pgunionPKDB->num);
	fprintf(fp,"maxKeyNum = [%d]\n",pgunionPKDB->maxKeyNum);
	fprintf(fp,"lastUpdatedTime = [%s]\n",pgunionPKDB->lastUpdatedTime);
	return(UnionPrintSharedMemoryModuleToFile(pgunionPKDBMDL,fp));
}

int UnionInsertPKIntoKeyDB(PUnionPK pkey)
{
	int	ret;
	long    index;
	int	check;
	int	pos;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionInsertPKIntoKeyDB:: UnionConnectPKDB !\n");
		return(ret);
	}
	
	if (!UnionIsValidPK(pkey))
	{
		UnionUserErrLog("in UnionInsertPKIntoKeyDB:: UnionIsValidPK [%s]\n",pkey->fullName);
		return(errCodeParameter);
	}
	
	if ((index = UnionFindPKPosInKeyDB(pkey->fullName)) >= 0)
	{
		UnionUserErrLog("in UnionInsertPKIntoKeyDB:: [%s] already exists!\n",pkey->fullName);
		return(errCodePKDBMDL_KeyAlreadyExists);
	}

	if (pgunionPKDB->num == pgunionPKDB->maxKeyNum)
	{
		UnionUserErrLog("in UnionInsertPKIntoKeyDB:: PKDB is full!\n");
		return(errCodePKDBMDL_KeyDBFull);
	}
	
	if (pgunionPK == NULL)
	{
		UnionUserErrLog("in UnionInsertPKIntoKeyDB:: pkey is NULL!\n");
		return(errCodePKDBMDL_KeyDBNonConnected);
	}
	
	if ((ret = UnionApplyWritingLocks(pgunionPKDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionInsertPKIntoKeyDB:: UnionApplyWritingLocks !\n");
		return(ret);
	}
	
	memcpy(pgunionPK+pgunionPKDB->num,pkey,sizeof(TUnionPK));
	pgunionPKDB->num += 1;

	UnionLogPKDBOperation(conRecordInsert,pkey);
	UnionGetFullSystemDateTime(pgunionPKDB->lastUpdatedTime);
	if ((ret = UnionMirrorPKDBIntoDisk(pgunionPKDB,pgunionPK)) < 0)
		UnionUserErrLog("in UnionInsertPKIntoKeyDB:: UnionMirrorPKDBIntoDisk!\n");
	
exitRightNow:
	UnionReleaseWritingLocks(pgunionPKDBMDL);
	return(ret);
}

int UnionDeletePKFromKeyDB(char *fullName)
{
	int		ret;
	long    	index;
	TUnionPK	tmpPK;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionInsertPKIntoKeyDB:: UnionConnectPKDB !\n");
		return(ret);
	}
	
	if ((ret = UnionApplyWritingLocks(pgunionPKDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionDeletePKFromKeyDB:: UnionApplyWritingLocks !\n");
		return(ret);
	}
	
	for (index = 0; index < pgunionPKDB->num; index++)
	{
		if (strcmp(fullName,(pgunionPK + index)->fullName) == 0)
			break;
	}
	if ((index == pgunionPKDB->num) || (pgunionPKDB->num <= 0))
	{
		ret = errCodePKDBMDL_KeyNonExists;
		goto exitRightNow;
	}
	memcpy(&tmpPK,pgunionPK+index,sizeof(tmpPK));
	UnionLog("in UnionDeletePKFromKeyDB:: %s [%04d]\n",(pgunionPK + (pgunionPKDB->num - 1))->fullName,sizeof(tmpPK));
	memcpy(pgunionPK+index,pgunionPK + (pgunionPKDB->num - 1),sizeof(TUnionPK));
	pgunionPKDB->num--;
	UnionLogPKDBOperation(conRecordDelete,&tmpPK);	
	UnionGetFullSystemDateTime(pgunionPKDB->lastUpdatedTime);
	if ((ret = UnionMirrorPKDBIntoDisk(pgunionPKDB,pgunionPK)) < 0)
		UnionUserErrLog("in UnionDeletePKFromKeyDB:: UnionMirrorPKDBIntoDisk!\n");
	
exitRightNow:
	UnionReleaseWritingLocks(pgunionPKDBMDL);
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
	
	if ((ret = UnionApplyReadingLocks(pgunionPKDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromKeyDBAnyway:: UnionApplyReadingLocks !\n");
		return(ret);
	}
	
	memcpy(pkey,pgunionPK+index,sizeof(TUnionPK));

	UnionReleaseReadingLocks(pgunionPKDBMDL);

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
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionUpdatePKInKeyDB:: UnionConnectPKDB !\n");
		return(ret);
	}
		
	if (pkey == NULL)
	{
		UnionUserErrLog("in UnionUpdatePKInKeyDB:: null Pointer!\n");
		return(errCodeParameter);
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	bcdhex_to_aschex(pkey->value,UnionGetAscCharLengthOfPK(pkey->length)/2,tmpBuf);	
	return(UnionUpdatePKValueInKeyDB(pkey->fullName,tmpBuf,pkey->checkValue));
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
	for (index = 0; index < pgunionPKDB->num; index++)
	{
		if (strcmp(fullName,(pgunionPK+index)->fullName) == 0)
		{
			UnionReleaseReadingLocks(pgunionPKDBMDL);
			return(index);
		}
	}		
		
	UnionReleaseReadingLocks(pgunionPKDBMDL);
	return(errCodePKDBMDL_KeyNonExists);
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
	TUnionPK	desKey;
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

int UnionPrintPKExpectedInKeyDBToFile(PUnionPKDB pdesKeyDB,char *app,char *owner,char *name,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	short	appEQExpected=0,ownerEQExpected=0,nameEQExpected=0;
	char	thisApp[40+1],thisOwner[40+1],thisName[40+1];
	char	nameOfType[20];
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKExpectedInKeyDBToFile:: UnionConnectPKDB !\n");
		return(ret);
	}
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintPKExpectedInKeyDBToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
	/*
	if ((ret = UnionApplyReadingLocks(pdesKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionPrintPKExpectedInKeyDBToFile:: UnionApplyReadingLocks !\n");
		return(ret);
	}
	*/
	
	if (pgunionPK == NULL)
	{
		UnionUserErrLog("in UnionPrintPKExpectedInKeyDBToFile:: pkey is NULL!\n");
		ret = errCodePKDBMDL_KeyDBNonConnected;
		goto exitRightNow;
	}
	
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
	//UnionReleaseReadingLocks(pdesKeyDB);
	return(ret);
}

int UnionPrintPKExpectedToFile(char *app,char *owner,char *name,FILE *fp)
{
	int	ret;
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKExpectedToFile:: UnionConnectPKDB !\n");
		return(ret);
	}
	return(UnionPrintPKExpectedInKeyDBToFile(pgunionPKDB,app,owner,name,fp));
}

int UnionPrintPKInKeyDBOutofDateToFile(PUnionPKDB pdesKeyDB,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBOutofDateToFile:: UnionConnectPKDB !\n");
		return(ret);
	}
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBOutofDateToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
	if (pgunionPK == NULL)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBOutofDateToFile:: pkey is NULL!\n");
		ret = errCodePKDBMDL_KeyDBNonConnected;
		goto exitRightNow;
	}
	
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
	//UnionReleaseReadingLocks(pdesKeyDB);
	return(ret);
}


int UnionPrintPKOutofDateToFile(FILE *fp)
{
	int	ret;
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKOutofDateToFile:: UnionConnectPKDB !\n");
		return(ret);
	}
	return(UnionPrintPKInKeyDBOutofDateToFile(pgunionPKDB,fp));
}

int UnionPrintPKInKeyDBNearOutofDateToFile(PUnionPKDB pdesKeyDB,long days,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	long	tmpDays;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBNearOutofDateToFile:: UnionConnectPKDB !\n");
		return(ret);
	}
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBNearOutofDateToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
	if (pgunionPK == NULL)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBNearOutofDateToFile:: pkey is NULL!\n");
		ret = errCodePKDBMDL_KeyDBNonConnected;
		goto exitRightNow;
	}
	
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
	//UnionReleaseReadingLocks(pdesKeyDB);
	return(ret);
}

int UnionPrintPKNearOutofDateToFile(long days,FILE *fp)
{
	int	ret;
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKNearOutofDateToFile:: UnionConnectPKDB !\n");
		return(ret);
	}
	return(UnionPrintPKInKeyDBNearOutofDateToFile(pgunionPKDB,days,fp));
}



// 2004/11/20, Added by Wolfgang Wang
int UnionSynchronizeSpecifiedPKToBackuper(char *fullName)
{
	long	index=0;
	int	ret;

	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeSpecifiedPKToBackuper:: UnionConnectPKDB !\n");
		return(ret);
	}
	if ((index = UnionFindPKPosInKeyDB(fullName)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeSpecifiedPKToBackuper:: UnionFindPKPosInKeyDB! [%s]\n",fullName);
		return((int)index);
	}

	if ((ret = UnionApplyWritingLocks(pgunionPKDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeAllPKToBackuper:: UnionApplyWritingLocks !\n");
		return(ret);
	}

	if ((ret = UnionSynchronizePKDBOperation(conRecordUpdate,pgunionPK+index)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeAllPKToBackuper:: UnionSynchronizePKDBOperation !\n");
	}
	
	UnionReleaseWritingLocks(pgunionPKDBMDL);
	
	return(ret);
}

// 2004/11/20, Added by Wolfgang Wang
long UnionSynchronizeAllPKToBackuper()
{
	int	ret;
	long	i;
	long	synchronizedNum = 0;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeAllPKToBackuper:: UnionConnectPKDB !\n");
		return(ret);
	}
		
	if ((ret = UnionApplyWritingLocks(pgunionPKDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeAllPKToBackuper:: UnionApplyWritingLocks !\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionPKDB->num; i++)
	{
		if ((ret = UnionSynchronizePKDBOperation(conRecordUpdate,pgunionPK+i)) < 0)
			UnionUserErrLog("in UnionSynchronizeAllPKToBackuper:: UnionSynchronizePKDBOperation !\n");
		else
			synchronizedNum++;
	}
	ret = synchronizedNum;
	
exitRightNow:
	UnionReleaseWritingLocks(pgunionPKDBMDL);
	return(ret);
}

// 2004/11/20, Added by Wolfgang Wang
long UnionSynchronizePKOfSpecifiedAppToBackuper(char *appName)
{
	int	ret;
	long	i;
	char	thisApp[40+1],thisOwner[40+1],thisName[40+1];
	long	synchronizedNum = 0;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionSynchronizePKOfSpecifiedAppToBackuper:: UnionConnectPKDB !\n");
		return(ret);
	}
		
	if ((ret = UnionApplyWritingLocks(pgunionPKDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizePKOfSpecifiedAppToBackuper:: UnionApplyWritingLocks !\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionPKDB->num; i++)
	{
		memset(thisApp,0,sizeof(thisApp));
		UnionAnalysisPKFullName((pgunionPK+i)->fullName,thisApp,thisOwner,thisName);
		if (strcmp(appName,thisApp) != 0)
			continue;
		if ((ret = UnionSynchronizePKDBOperation(conRecordUpdate,pgunionPK+i)) < 0)
			UnionUserErrLog("in UnionSynchronizePKOfSpecifiedAppToBackuper:: UnionSynchronizePKDBOperation !\n");
		else
			synchronizedNum++;
	}
	ret = synchronizedNum;
	
exitRightNow:
	UnionReleaseWritingLocks(pgunionPKDBMDL);
	return(ret);
}

// added by Wolfgang Wang, 2004/11/20
int UnionPKDBOperation(TUnionKeyDBOperation operation,PUnionPK pkey)
{
	int	ret = -1;
	
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
	
	if ((ret = UnionApplyReadingLocks(pgunionPKDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionUsePKFromKeyDB:: UnionApplyReadingLocks !\n");
		return(ret);
	}
	
	if (!UnionIsPKStillEffective(pgunionPK+index))
	{
		UnionReleaseReadingLocks(pgunionPKDBMDL);
		return(errPKNotEffective);
	}
	
	(pgunionPK+index)->useTimes += 1;
	
	memcpy(pkey,pgunionPK+index,sizeof(TUnionPK));

	UnionReleaseReadingLocks(pgunionPKDBMDL);

	return(0);
}

// 2004/11/25, Wolfgang Wang
int UnionUpdatePKValueInKeyDB(char *fullName,char *value,char *checkValue)
{
	int	ret;
	long	index=0;

	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionUpdatePKValueInKeyDB:: UnionConnectPKDB !\n");
		return(ret);
	}
	if ((index = UnionFindPKPosInKeyDB(fullName)) < 0)
	{
		UnionUserErrLog("in UnionUpdatePKValueInKeyDB:: UnionFindPKPosInKeyDB [%s]!\n",fullName);
		return(errCodeParameter);
	}
	
	if ((ret = UnionApplyWritingLocks(pgunionPKDBMDL)) < 0)
	//if ((ret = UnionApplyReadingLocks(pgunionPKDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionUpdatePKValueInKeyDB:: UnionApplyWritingLocks !\n");
		return(ret);
	}
	
	if ((ret = UnionUpdatePKValue(pgunionPK+index,value,checkValue)) < 0)
	{
		UnionUserErrLog("in UnionUpdatePKValueInKeyDB:: UnionUpdatePKValue [%s]\n",fullName);
		UnionReleaseReadingLocks(pgunionPKDBMDL);
		return(ret);
	}
	
	UnionLogPKDBOperation(conRecordUpdate,pgunionPK+index);	

	UnionGetFullSystemDateTime(pgunionPKDB->lastUpdatedTime);

	if ((ret = UnionMirrorPKDBIntoDisk(pgunionPKDB,pgunionPK)) < 0)
		UnionUserErrLog("in UnionUpdatePKValueInKeyDB:: UnionMirrorPKDBIntoDisk!\n");
	
	UnionReleaseWritingLocks(pgunionPKDBMDL);
	//UnionReleaseReadingLocks(pgunionPKDBMDL);
	
	return(0);
}


// 2004/11/25, Wolfgang Wang
int UnionPrintPKInKeyDBNearOutofMaxUseTimesToFile(PUnionPKDB pdesKeyDB,long times,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBNearOutofMaxUseTimesToFile:: UnionConnectPKDB !\n");
		return(ret);
	}
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBNearOutofMaxUseTimesToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
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
	//UnionReleaseReadingLocks(pdesKeyDB);
	return(ret);
}

// 2004/11/25, Wolfgang Wang
int UnionPrintPKNearOutofMaxUseTimesToFile(long times,FILE *fp)
{
	int	ret;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKNearOutofMaxUseTimesToFile:: UnionConnectPKDB !\n");
		return(ret);
	}
	return(UnionPrintPKInKeyDBNearOutofMaxUseTimesToFile(pgunionPKDB,times,fp));
}


// Wolfgang Wang, 2004/11/25
int UnionUpdatePKAttrInKeyDB(PUnionPK pkey)
{
	int	ret;
	long	index=0;
	TUnionPK	pk;

	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionUpdatePKAttrInKeyDB(:: UnionConnectPKDB !\n");
		return(ret);
	}
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
	
	if ((ret = UnionApplyWritingLocks(pgunionPKDBMDL)) < 0)
	//if ((ret = UnionApplyReadingLocks(pgunionPKDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionUpdatePKAttrInKeyDB:: UnionApplyWritingLocks !\n");
		return(ret);
	}
	
	memcpy(pk.value,(pgunionPK+index)->value,sizeof(pk.value));
	memcpy(pk.oldValue,(pgunionPK+index)->oldValue,sizeof(pk.oldValue));
	
	memcpy(pgunionPK+index,&pk,sizeof(TUnionPK));
	UnionLogPKDBOperation(conRecordUpdate,&pk);	

	UnionGetFullSystemDateTime(pgunionPKDB->lastUpdatedTime);

	if ((ret = UnionMirrorPKDBIntoDisk(pgunionPKDB,pgunionPK)) < 0)
		UnionUserErrLog("in UnionUpdatePKAttrInKeyDB:: UnionMirrorPKDBIntoDisk!\n");
	
	UnionReleaseWritingLocks(pgunionPKDBMDL);
	//UnionReleaseReadingLocks(pgunionPKDBMDL);
	
	return(0);
}

// Wolfgang Wang, 2004/11/25
int UnionUpdateAllPKFieldsInKeyDB(PUnionPK pkey)
{
	int	ret;
	long	index=0;

	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionUpdateAllPKFieldsInKeyDB:: UnionConnectPKDB !\n");
		return(ret);
	}
	if (pkey == NULL)
	{
		UnionUserErrLog("in UnionUpdateAllPKFieldsInKeyDB:: null Pointer!\n");
		return(errCodeParameter);
	}
	
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
	
	//if ((ret = UnionApplyWritingLocks(pgunionPKDBMDL)) < 0)
	if ((ret = UnionApplyReadingLocks(pgunionPKDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionUpdateAllPKFieldsInKeyDB:: UnionApplyWritingLocks !\n");
		return(ret);
	}
	
	memcpy(pgunionPK+index,pkey,sizeof(TUnionPK));
	UnionLogPKDBOperation(conRecordUpdate,pkey);	

	UnionGetFullSystemDateTime(pgunionPKDB->lastUpdatedTime);

	if ((ret = UnionMirrorPKDBIntoDisk(pgunionPKDB,pgunionPK)) < 0)
		UnionUserErrLog("in UnionUpdateAllPKFieldsInKeyDB:: UnionMirrorPKDBIntoDisk!\n");
	
	//UnionReleaseWritingLocks(pgunionPKDBMDL);
	UnionReleaseReadingLocks(pgunionPKDBMDL);
	
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
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBUsedDaysToFile:: UnionConnectPKDB!\n");
		return(ret);
	}
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBUsedDaysToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
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
	//UnionReleaseReadingLocks(pdesKeyDB);
	return(ret);
}

int UnionPrintPKInKeyDBUsedTimesToFile(long times,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	//long	tmpDays;
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBUsedTimesToFile:: UnionConnectPKDB!\n");
		return(ret);
	}
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBUsedTimesToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
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
	//UnionReleaseReadingLocks(pdesKeyDB);
	return(ret);
}

