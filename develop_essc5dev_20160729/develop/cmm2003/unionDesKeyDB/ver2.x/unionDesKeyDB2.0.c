//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2003/09/08

// 	Version:	2.0

//	2003/09/24, 在1.0基础上升级为2.0

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

#include "unionDesKeyDB.h"
#include "unionDesKeyDBLog.h"
#include "UnionLog.h"

PUnionSharedMemoryModule	pgunionDesKeyDBMDL = NULL;
PUnionDesKeyDB			pgunionDesKeyDB = NULL;
PUnionDesKey			pgunionDesKey = NULL;

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
		return(-1);
	}
	
	memset(&keyDB,0,sizeof(keyDB));	
	fread(&keyDB,sizeof(keyDB),1,fp);
	
	fclose(fp);
	
	return(keyDB.maxKeyNum);
}

// Module Layer Functions
int UnionConnectDesKeyDB()
{
	int	ret;
	long	maxKeyNum;
	
	if ((maxKeyNum = UnionGetMaxKeyNumFromKeyDBFile()) <= 0)
	{
		UnionUserErrLog("in UnionConnectDesKeyDB:: UnionGetMaxKeyNumFromKeyDBFile!\n");
		return(-1);
	}
	
	if ((pgunionDesKeyDBMDL = UnionConnectSharedMemoryModule(conMDLNameOfUnionDesKeyDB,
			sizeof(TUnionDesKeyDB) + sizeof(TUnionDesKey) * maxKeyNum)
			) == NULL)
	{
		UnionUserErrLog("in UnionConnectDesKeyDB:: UnionConnectSharedMemoryModule!\n");
		return(-1);
	}
	if ((pgunionDesKeyDB = (PUnionDesKeyDB)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionDesKeyDBMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectDesKeyDB:: PUnionDesKeyDB!\n");
		UnionRemoveDesKeyDBInMemory();
		return(-1);
	}
	
	/*
	if ((pgunionDesKeyDB->pkey = (PUnionDesKey)((unsigned char *)pgunionDesKeyDB + sizeof(*pgunionDesKeyDB))) == NULL)
	{
		UnionUserErrLog("in UnionConnectDesKeyDB:: PUnionDesKey!\n");
		return(-1);
	}
	pgunionDesKey = pgunionDesKeyDB->pkey;
	*/
	
	if ((pgunionDesKey = (PUnionDesKey)((unsigned char *)pgunionDesKeyDB + sizeof(*pgunionDesKeyDB))) == NULL)
	{
		UnionUserErrLog("in UnionConnectDesKeyDB:: PUnionDesKey!\n");
		UnionRemoveDesKeyDBInMemory();
		return(-1);
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
	pgunionDesKeyDB = NULL;
	
	return(UnionDisconnectShareModule(pgunionDesKeyDBMDL));
}

int UnionRemoveDesKeyDBInMemory()
{
	pgunionDesKeyDB = NULL;
	return(UnionRemoveSharedMemoryModule(conMDLNameOfUnionDesKeyDB));
}

int UnionCreateDesKeyDB(int maxKeyNum)
{
	TUnionDesKeyDB		rec;
	int			ret;
	
	if (maxKeyNum <= 0)
	{
		UnionUserErrLog("in UnionCreateDesKeyDB:: maxKeyNum [%d] error!\n",maxKeyNum);
		return(-1);
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

int UnionMirrorDesKeyDBIntoDisk(PUnionDesKeyDB pdesKeyDB,PUnionDesKey pkey)
{
	FILE			*fp;
	char			fileName[256];

	if (pdesKeyDB == NULL)
	{
		UnionUserErrLog("in UnionMirrorDesKeyDBIntoDisk:: NullPointer!\n");
		return(-1);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfDesKeyDB(fileName);
	
	if ((fp = fopen(fileName,"wb")) == NULL)
	{
		UnionSystemErrLog("in UnionMirrorDesKeyDBIntoDisk:: fopen [%s]\n",fileName);
		return(-1);
	}
	
	fwrite(pdesKeyDB,sizeof(*pdesKeyDB),1,fp);
	if (pdesKeyDB->num < 0)
	{
		UnionUserErrLog("in UnionMirrorDesKeyDBIntoDisk:: pdesKeyDB->num [%d] Error!\n",pdesKeyDB->num);
		fclose(fp);
		return(-1);
	}
	
	if ((pdesKeyDB->num == 0) || (pkey == NULL))
	{
		fclose(fp);
		return(0);
	}
	
	/*
	if (pgunionDesKey == NULL)
	{
		UnionUserErrLog("in UnionMirrorDesKeyDBIntoDisk:: pgunionDesKey is null!\n");
		fclose(fp);
		return(-1);
	}
	*/
		
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
	
	//UnionNullLog("in UnionLoadDesKeyDBIntoMemory::pgunionDesKeyDB = [%0x]\n",pgunionDesKeyDB);
	
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
		ret = -1;
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
			ret = -1;
			goto exitRightNow;
		}
		if (pgunionDesKeyDB->num > pgunionDesKeyDB->maxKeyNum)
		{
			UnionUserErrLog("in UnionLoadDesKeyDBIntoMemory:: pgunionDesKeyDB->num [%d] > pgunionDesKeyDB->maxKeyNum [%d]\n",pgunionDesKeyDB->num,pgunionDesKeyDB->maxKeyNum);
			ret = -1;
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
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKeyDBToFile:: fp is NULL!\n");
		return(-1);
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
		ret = -1;
		goto exitRightNow;
	}
		
	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		memset(nameOfType,0,sizeof(nameOfType));
		UnionGetNameOfDesKeyType((pgunionDesKey+i)->type,nameOfType);
		fprintf(fp,"[%30s][%s][%s][%s]\n",
			(pgunionDesKey+i)->fullName,
			nameOfType,
			(pgunionDesKey+i)->value,
			(pgunionDesKey+i)->checkValue);
		if ((fp == stdout) && (i % 20 == 0) && (i != 0))
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
	UnionPrintDesKeyDBToFile(pgunionDesKeyDB,stdout);
	UnionPrintSharedMemoryModuleToFile(pgunionDesKeyDBMDL,stdout);
	return(0);
}

int UnionPrintAllDesKeysInKeyDBToFile(FILE *fp)
{
	return(UnionPrintDesKeyDBToFile(pgunionDesKeyDB,fp));
}

int UnionPrintStatusOfDesKeyDBToFile(FILE *fp)
{
	if (fp == NULL)
		return(-1);
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
	
	if ((pgunionDesKeyDB == NULL) || (pkey == NULL))
	{
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: Null pointer!\n");
		return(-1);
	}
	
	if (!UnionIsValidDesKey(pkey))
	{
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: UnionIsValidDesKey [%s]\n",pkey->fullName);
		return(-1);
	}
	
	if (pgunionDesKeyDB->num == pgunionDesKeyDB->maxKeyNum)
	{
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: DesKeyDB is full!\n");
		return(-1);
	}
	
	if (pgunionDesKey == NULL)
	{
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: pkey is NULL!\n");
		return(-1);
	}
	
	if ((ret = UnionApplyWritingLocks(pgunionDesKeyDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: UnionApplyWritingLocks !\n");
		return(ret);
	}
	
	for (index = 0; index < pgunionDesKeyDB->num; index++)
	{
		if ((check = strcmp(pkey->fullName,(pgunionDesKey + index)->fullName)) == 0)
		{
			UnionUserErrLog("in UnionInsertDesKeyIntoKeyDB:: [%s] already exists!\n",pkey->fullName);
			ret = -1;
			goto exitRightNow;
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
	if (index == pgunionDesKeyDB->num)
	{
		ret = -1;
		goto exitRightNow;
	}
	pos = index;
	UnionLogDesKeyDBOperation(conRecordDelete,pgunionDesKey+pos);	
	for (index = pos+1; index < pgunionDesKeyDB->num; index++)
	{
		memcpy(pgunionDesKey+index-1,pgunionDesKey + index,sizeof(TUnionDesKey));
	}
	pgunionDesKeyDB->num--;
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

	if (pkey == NULL)
	{
		UnionUserErrLog("in UnionReadDesKeyFromKeyDBAnyway:: null Pointer!\n");
		return(-1);
	}
	
	if ((index = UnionFindDesKeyPosInKeyDB(pkey->fullName)) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromKeyDBAnyway:: UnionFindDesKeyPosInKeyDB [%s]!\n",pkey->fullName);
		return((int)index);
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

int UnionReadDesKeyFromKeyDB(PUnionDesKey pkey)
{
	int	ret;
	
	if ((ret = UnionReadDesKeyFromKeyDBAnyway(pkey)) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromKeyDB:: UnionReadDesKeyFromKeyDBAnyway !\n");
		return(ret);
	}

	if (UnionIsDesKeyStillEffective(pkey))	
		return(0);
	else
		return(errDesKeyNotEffective);
}

int UnionUpdateDesKeyInKeyDB(PUnionDesKey pkey)
{
	int	ret=-1;
	long	index=0;

	if (pkey == NULL)
	{
		UnionUserErrLog("in UnionUpdateDesKeyInKeyDB:: null Pointer!\n");
		return(-1);
	}
	
	if (!UnionIsValidDesKey(pkey))
	{
		UnionUserErrLog("in UnionUpdateDesKeyInKeyDB:: UnionIsValidDesKey [%s]\n",pkey->fullName);
		return(-1);
	}
	
	if ((index = UnionFindDesKeyPosInKeyDB(pkey->fullName)) < 0)
	{
		UnionUserErrLog("in UnionUpdateDesKeyInKeyDB:: UnionFindDesKeyPosInKeyDB! [%s]\n",pkey->fullName);
		return((int)index);
	}
	
	//if ((ret = UnionApplyWritingLocks(pgunionDesKeyDBMDL)) < 0)
	if ((ret = UnionApplyReadingLocks(pgunionDesKeyDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionUpdateDesKeyInKeyDB:: UnionApplyWritingLocks !\n");
		return(ret);
	}
	
	memcpy(pgunionDesKey+index,pkey,sizeof(TUnionDesKey));
	UnionLogDesKeyDBOperation(conRecordUpdate,pkey);	

	UnionGetFullSystemDateTime(pgunionDesKeyDB->lastUpdatedTime);

	if ((ret = UnionMirrorDesKeyDBIntoDisk(pgunionDesKeyDB,pgunionDesKey)) < 0)
		UnionUserErrLog("in UnionDeleteDesKeyFromKeyDB:: UnionMirrorDesKeyDBIntoDisk!\n");
	
	//UnionReleaseWritingLocks(pgunionDesKeyDBMDL);
	UnionReleaseReadingLocks(pgunionDesKeyDBMDL);
	
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
	
	if ((ret = UnionApplyReadingLocks(pgunionDesKeyDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionFindDesKeyPosInKeyDB:: UnionApplyReadingLocks !\n");
		return(ret);
	}
	/*
	for (index = 0; index < pgunionDesKeyDB->num; index++)
	{
		UnionNullLog("[%06d] [%s]\n",index,(pgunionDesKey+index)->fullName);
	}

	UnionLog("in UnionFindDesKeyPosInKeyDB:: [%s] [%d]\n",fullName,pgunionDesKeyDB->num);
	*/
	left = 0;
	right = pgunionDesKeyDB->num - 1;
	index = (left+right)/2;
	while (1) 
	{
		if ((left > right) || (left >= pgunionDesKeyDB->num) || (right < 0))
		{
			ret = -1;
			goto exitRightNow;
		}
		if ((strcmp(fullName,(pgunionDesKey+left)->fullName) < 0) || 
			(strcmp(fullName,(pgunionDesKey+right)->fullName) > 0))
		{
			ret = -1;
			UnionNullLog("fullName = [%s] left = [%s] right = [%s] len = [%d]\n",fullName,(pgunionDesKey+left)->fullName,(pgunionDesKey+right)->fullName,40);
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
		
	UnionReleaseReadingLocks(pgunionDesKeyDBMDL);
	return(index);
	
exitRightNow:
	//UnionNullLog("in UnionFindDesKeyPosInKeyDB:: index = [%ld] left = [%ld] right = [%ld]\n",index,left,right);
	UnionReleaseReadingLocks(pgunionDesKeyDBMDL);
	return(ret);
}

int UnionGetKeyNumOfKeyDB()
{
	if (pgunionDesKeyDB == NULL)
	{
		UnionUserErrLog("in UnionGetKeyNumOfKeyDB:: null Pointer!\n");
		return(-1);
	}
	return(pgunionDesKeyDB->num);
}

char *UnionGetDesKeyDBLastUpdatedTime()
{
	if (pgunionDesKeyDB == NULL)
	{
		UnionUserErrLog("in UnionGetDesKeyDBLastUpdatedTime:: null Pointer!\n");
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
		return(-1);
	}
	
	memset(&oldKeyDB,0,sizeof(oldKeyDB));	
	fread(&oldKeyDB,sizeof(oldKeyDB),1,fp1);
	if (oldKeyDB.maxKeyNum == newMaxKeyNum)
	{
		UnionUserErrLog("in UnionChangeSizeOfDesKeyDB:: original newMaxKeyNum [%ld] = newMaxNum [%ld]\n",oldKeyDB.maxKeyNum,newMaxKeyNum);
		fclose(fp1);
		return(-1);
	}
	if (oldKeyDB.maxKeyNum > newMaxKeyNum)
	{
		if (oldKeyDB.num > newMaxKeyNum)
		{
			UnionUserErrLog("in UnionChangeSizeOfDesKeyDB:: real Num [%ld] > newMaxNum [%ld]\n",oldKeyDB.num,newMaxKeyNum);
			fclose(fp1);
			return(-1);
		}
	}
	memset(fileName2,0,sizeof(fileName2));
	sprintf(fileName2,"%s.tmp",fileName1);
	if ((fp2 = fopen(fileName2,"wb")) == NULL)
	{
		UnionSystemErrLog("in UnionChangeSizeOfDesKeyDB:: fopen [%s]\n",fileName2);
		fclose(fp1);
		return(-1);
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
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKeyExpectedInKeyDBToFile:: fp is NULL!\n");
		return(-1);
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
		ret = -1;
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
		fprintf(fp,"[%30s][%s][%s][%s]\n",
			(pgunionDesKey+i)->fullName,
			nameOfType,
			(pgunionDesKey+i)->value,
			(pgunionDesKey+i)->checkValue);
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
	return(UnionPrintDesKeyExpectedInKeyDBToFile(pgunionDesKeyDB,app,owner,name,fp));
}

int UnionPrintDesKeyInKeyDBOutofDateToFile(PUnionDesKeyDB pdesKeyDB,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	char	today[10];
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBOutofDateToFile:: fp is NULL!\n");
		return(-1);
	}
	
	if (pgunionDesKey == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBOutofDateToFile:: pkey is NULL!\n");
		ret = -1;
		goto exitRightNow;
	}
	
	memset(today,0,sizeof(today));
	UnionGetFullSystemDate(today);
	//printf("Today = [%s]\n",today);			
	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		//printf("(pgunionDesKey+i)->passiveDate = [%s]\n",(pgunionDesKey+i)->passiveDate);
		if (strncmp(today,(pgunionDesKey+i)->passiveDate,8) < 0)
			continue;
		totalNum++;
		fprintf(fp,"[%35s][%s][%s]\n",(pgunionDesKey+i)->fullName,(pgunionDesKey+i)->activeDate,(pgunionDesKey+i)->passiveDate);
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
	return(UnionPrintDesKeyInKeyDBOutofDateToFile(pgunionDesKeyDB,fp));
}

int UnionPrintDesKeyInKeyDBNearOutofDateToFile(PUnionDesKeyDB pdesKeyDB,long days,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	long	tmpDays;
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBNearOutofDateToFile:: fp is NULL!\n");
		return(-1);
	}
	
	if (pgunionDesKey == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKeyInKeyDBNearOutofDateToFile:: pkey is NULL!\n");
		ret = -1;
		goto exitRightNow;
	}
	
	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		if ((tmpDays = UnionDecideDaysAfterToday((pgunionDesKey+i)->passiveDate)) < 0)
			continue;
		if (tmpDays > days)
			continue;
		totalNum++;
		fprintf(fp,"[%35s][%s][%s]\n",(pgunionDesKey+i)->fullName,(pgunionDesKey+i)->activeDate,(pgunionDesKey+i)->passiveDate);
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
	return(UnionPrintDesKeyInKeyDBNearOutofDateToFile(pgunionDesKeyDB,days,fp));
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
	
	if ((pgunionDesKeyDBMDL = UnionConnectExistingSharedMemoryModule(conMDLNameOfUnionDesKeyDB,
			sizeof(TUnionDesKeyDB))) == NULL)
	{
		UnionUserErrLog("in UnionConnectExistingDesKeyDB:: UnionConnectExistingSharedMemoryModule!\n");
		return(-1);
	}
	if ((pgunionDesKeyDB = (PUnionDesKeyDB)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionDesKeyDBMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectExistingDesKeyDB:: PUnionDesKeyDB!\n");
		UnionRemoveDesKeyDBInMemory();
		return(-1);
	}
	if ((pgunionDesKey = (PUnionDesKey)((unsigned char *)pgunionDesKeyDB + sizeof(*pgunionDesKeyDB))) == NULL)
	{
		UnionUserErrLog("in UnionConnectExistingDesKeyDB:: PUnionDesKey!\n");
		UnionRemoveDesKeyDBInMemory();
		return(-1);
	}
	pgunionDesKeyDB->pkey = pgunionDesKey;
	return(0);
}

int UnionDeleteAllDesKeyOfApp(char *app)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	char	thisApp[40+1],thisOwner[40+1],thisName[40+1];
	//int	keyOfAppExists = 0;
	char	fullName[100+1];
	TUnionDesKey	desKey;
	
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
		memcpy(&desKey,(pgunionDesKey+i),sizeof(desKey));
		if ((ret = UnionDeleteDesKeyFromKeyDB(fullName)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllDesKeyOfApp:: UnionDeleteDesKeyFromKeyDB [%s] by [%s] failure!\n",fullName,getenv("LOGNAME"));
			continue;
		}
		UnionLog("in UnionDeleteAllDesKeyOfApp:: delete [%s] [%s] by [%s] ok!\n",fullName,desKey.activeDate,getenv("LOGNAME"));
		totalNum++;
		i = 0;
	}
	UnionLog("in UnionDeleteAllDesKeyOfApp:: total delete keys [%04d] by [%s] ok!\n",totalNum,getenv("LOGNAME"));
	return(totalNum);
}
