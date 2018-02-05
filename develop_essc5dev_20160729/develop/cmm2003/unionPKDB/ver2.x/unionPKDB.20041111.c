//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2004/11/11

#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <string.h>
//#include <signal.h>
//#include <sys/ipc.h>
//#include <sys/msg.h>
//#include <sys/shm.h>

#include "unionModule.h"
#include "UnionStr.h"

#include "unionPKDB.h"
#include "unionPKDBLog.h"
#include "UnionLog.h"

PUnionSharedMemoryModule	pgunionPKDBMDL = NULL;
PUnionPKDB			pgunionPKDB = NULL;
PUnionPK			pgunionPK = NULL;

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
		return(-1);
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
	
	if ((maxKeyNum = UnionGetMaxKeyNumFromPKDBFile()) <= 0)
	{
		UnionUserErrLog("in UnionConnectPKDB:: UnionGetMaxKeyNumFromPKDBFile!\n");
		return(-1);
	}
	
	if ((pgunionPKDBMDL = UnionConnectSharedMemoryModule(conMDLNameOfUnionPKDB,
			sizeof(TUnionPKDB) + sizeof(TUnionPK) * maxKeyNum)
			) == NULL)
	{
		UnionUserErrLog("in UnionConnectPKDB:: UnionConnectSharedMemoryModule!\n");
		return(-1);
	}
	if ((pgunionPKDB = (PUnionPKDB)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionPKDBMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectPKDB:: PUnionPKDB!\n");
		UnionRemovePKDBInMemory();
		return(-1);
	}
	
	if ((pgunionPK = (PUnionPK)((unsigned char *)pgunionPKDB + sizeof(*pgunionPKDB))) == NULL)
	{
		UnionUserErrLog("in UnionConnectPKDB:: PUnionPK!\n");
		UnionRemovePKDBInMemory();
		return(-1);
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
	pgunionPKDB = NULL;
	
	return(UnionDisconnectShareModule(pgunionPKDBMDL));
}

int UnionRemovePKDBInMemory()
{
	pgunionPKDB = NULL;
	return(UnionRemoveSharedMemoryModule(conMDLNameOfUnionPKDB));
}

int UnionCreatePKDB(int maxKeyNum)
{
	TUnionPKDB		rec;
	int			ret;
	
	if (maxKeyNum <= 0)
	{
		UnionUserErrLog("in UnionCreatePKDB:: maxKeyNum [%d] error!\n",maxKeyNum);
		return(-1);
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
		return(-1);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfPKDB(fileName);
	
	if ((fp = fopen(fileName,"wb")) == NULL)
	{
		UnionSystemErrLog("in UnionMirrorPKDBIntoDisk:: fopen [%s]\n",fileName);
		return(-1);
	}
	
	fwrite(pdesKeyDB,sizeof(*pdesKeyDB),1,fp);
	if (pdesKeyDB->num < 0)
	{
		UnionUserErrLog("in UnionMirrorPKDBIntoDisk:: pdesKeyDB->num [%d] Error!\n",pdesKeyDB->num);
		fclose(fp);
		return(-1);
	}
	
	if ((pdesKeyDB->num == 0) || (pkey == NULL))
	{
		fclose(fp);
		return(0);
	}
	
	/*
	if (pgunionPK == NULL)
	{
		UnionUserErrLog("in UnionMirrorPKDBIntoDisk:: pgunionPK is null!\n");
		fclose(fp);
		return(-1);
	}
	*/
		
	//fseek(fp,0,SEEK_SET);
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
		ret = -1;
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
			ret = -1;
			goto exitRightNow;
		}
		if (pgunionPKDB->num > pgunionPKDB->maxKeyNum)
		{
			UnionUserErrLog("in UnionLoadPKDBIntoMemory:: pgunionPKDB->num [%d] > pgunionPKDB->maxKeyNum [%d]\n",pgunionPKDB->num,pgunionPKDB->maxKeyNum);
			ret = -1;
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
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintPKDBToFile:: fp is NULL!\n");
		return(-1);
	}
	
	if (pgunionPK == NULL)
	{
		UnionUserErrLog("in UnionPrintPKDBToFile:: pkey is NULL!\n");
		ret = -1;
		goto exitRightNow;
	}
		
	fprintf(fp,"[%30s][%20s][%4s][%s]\n",
		"密钥名称","密钥类型","长度","密钥校验值");
	for (i = 0; i < pdesKeyDB->num; i++)
	{
		memset(nameOfType,0,sizeof(nameOfType));
		UnionGetNameOfPKType((pgunionPK+i)->type,nameOfType);
		fprintf(fp,"[%30s][%20s][%04d][%s]\n",
			(pgunionPK+i)->fullName,
			nameOfType,
			//UnionGetAscCharLengthOfPK((pgunionPK+i)->length),
			UnionGetBitsLengthOfRSAPair((pgunionPK+i)->length),
			(pgunionPK+i)->checkValue);
		if ((fp == stdout) && (i % 20 == 0) && (i != 0))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press enter to continue or exit/quit to exit")))
				return(0);
		}
	}
	fprintf(fp,"totolKeyNum = [%d]\n",pdesKeyDB->num);
	fprintf(fp,"maxKeyNum = [%d]\n",pdesKeyDB->maxKeyNum);
	ret = 0;
	
exitRightNow:
	//UnionReleaseReadingLocks(pdesKeyDB);
	return(ret);
}

int UnionPrintPKDB()
{
	UnionPrintPKDBToFile(pgunionPKDB,stdout);
	UnionPrintSharedMemoryModuleToFile(pgunionPKDBMDL,stdout);
	return(0);
}

int UnionPrintAllPKsInKeyDBToFile(FILE *fp)
{
	return(UnionPrintPKDBToFile(pgunionPKDB,fp));
}

int UnionPrintStatusOfPKDBToFile(FILE *fp)
{
	if (fp == NULL)
		return(-1);
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
	
	if ((pgunionPKDB == NULL) || (pkey == NULL))
	{
		UnionUserErrLog("in UnionInsertPKIntoKeyDB:: Null pointer!\n");
		return(-1);
	}
	
	if (!UnionIsValidPK(pkey))
	{
		UnionUserErrLog("in UnionInsertPKIntoKeyDB:: UnionIsValidPK [%s]\n",pkey->fullName);
		return(-1);
	}
	
	if (pgunionPKDB->num == pgunionPKDB->maxKeyNum)
	{
		UnionUserErrLog("in UnionInsertPKIntoKeyDB:: PKDB is full!\n");
		return(-1);
	}
	
	if (pgunionPK == NULL)
	{
		UnionUserErrLog("in UnionInsertPKIntoKeyDB:: pkey is NULL!\n");
		return(-1);
	}
	
	if ((ret = UnionApplyWritingLocks(pgunionPKDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionInsertPKIntoKeyDB:: UnionApplyWritingLocks !\n");
		return(ret);
	}
	
	for (index = 0; index < pgunionPKDB->num; index++)
	{
		if ((check = strcmp(pkey->fullName,(pgunionPK + index)->fullName)) == 0)
		{
			UnionUserErrLog("in UnionInsertPKIntoKeyDB:: [%s] already exists!\n",pkey->fullName);
			ret = -1;
			goto exitRightNow;
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
	int	ret;
	long    index;
	int	pos;
	
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
	if (index == pgunionPKDB->num)
	{
		ret = -1;
		goto exitRightNow;
	}
	pos = index;
	UnionLogPKDBOperation(conRecordDelete,pgunionPK+pos);	
	for (index = pos+1; index < pgunionPKDB->num; index++)
	{
		memcpy(pgunionPK+index-1,pgunionPK + index,sizeof(TUnionPK));
	}
	pgunionPKDB->num--;
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

	if (pkey == NULL)
	{
		UnionUserErrLog("in UnionReadPKFromKeyDBAnyway:: null Pointer!\n");
		return(-1);
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
	int	ret;
	
	if ((ret = UnionReadPKFromKeyDBAnyway(pkey)) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromKeyDB:: UnionReadPKFromKeyDBAnyway !\n");
		return(ret);
	}

	if (UnionIsPKStillEffective(pkey))	
		return(0);
	else
		return(errPKNotEffective);
}

int UnionUpdatePKInKeyDB(PUnionPK pkey)
{
	int	ret=-1;
	long	index=0;

	if (pkey == NULL)
	{
		UnionUserErrLog("in UnionUpdatePKInKeyDB:: null Pointer!\n");
		return(-1);
	}
	
	if (!UnionIsValidPK(pkey))
	{
		UnionUserErrLog("in UnionUpdatePKInKeyDB:: UnionIsValidPK [%s]\n",pkey->fullName);
		return(-1);
	}
	
	if ((index = UnionFindPKPosInKeyDB(pkey->fullName)) < 0)
	{
		UnionUserErrLog("in UnionUpdatePKInKeyDB:: UnionFindPKPosInKeyDB! [%s]\n",pkey->fullName);
		return((int)index);
	}
	
	//if ((ret = UnionApplyWritingLocks(pgunionPKDBMDL)) < 0)
	if ((ret = UnionApplyReadingLocks(pgunionPKDBMDL)) < 0)
	{
		UnionUserErrLog("in UnionUpdatePKInKeyDB:: UnionApplyWritingLocks !\n");
		return(ret);
	}
	
	memcpy(pgunionPK+index,pkey,sizeof(TUnionPK));
	UnionLogPKDBOperation(conRecordUpdate,pkey);	

	UnionGetFullSystemDateTime(pgunionPKDB->lastUpdatedTime);

	if ((ret = UnionMirrorPKDBIntoDisk(pgunionPKDB,pgunionPK)) < 0)
		UnionUserErrLog("in UnionDeletePKFromKeyDB:: UnionMirrorPKDBIntoDisk!\n");
	
	//UnionReleaseWritingLocks(pgunionPKDBMDL);
	UnionReleaseReadingLocks(pgunionPKDBMDL);
	
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
			ret = -1;
			goto exitRightNow;
		}
		if ((strcmp(fullName,(pgunionPK+left)->fullName) < 0) || 
			(strcmp(fullName,(pgunionPK+right)->fullName) > 0))
		{
			ret = -1;
			UnionNullLog("fullName = [%s] left = [%s] right = [%s] len = [%d]\n",fullName,(pgunionPK+left)->fullName,(pgunionPK+right)->fullName,40);
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
	if (pgunionPKDB == NULL)
	{
		UnionUserErrLog("in UnionGetPKNumOfKeyDB:: null Pointer!\n");
		return(-1);
	}
	return(pgunionPKDB->num);
}

char *UnionGetPKDBLastUpdatedTime()
{
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
		return(-1);
	}
	
	memset(&oldKeyDB,0,sizeof(oldKeyDB));	
	fread(&oldKeyDB,sizeof(oldKeyDB),1,fp1);
	if (oldKeyDB.maxKeyNum == newMaxKeyNum)
	{
		UnionUserErrLog("in UnionChangeSizeOfPKDB:: original newMaxKeyNum [%ld] = newMaxNum [%ld]\n",oldKeyDB.maxKeyNum,newMaxKeyNum);
		fclose(fp1);
		return(-1);
	}
	if (oldKeyDB.maxKeyNum > newMaxKeyNum)
	{
		if (oldKeyDB.num > newMaxKeyNum)
		{
			UnionUserErrLog("in UnionChangeSizeOfPKDB:: real Num [%ld] > newMaxNum [%ld]\n",oldKeyDB.num,newMaxKeyNum);
			fclose(fp1);
			return(-1);
		}
	}
	memset(fileName2,0,sizeof(fileName2));
	sprintf(fileName2,"%s.tmp",fileName1);
	if ((fp2 = fopen(fileName2,"wb")) == NULL)
	{
		UnionSystemErrLog("in UnionChangeSizeOfPKDB:: fopen [%s]\n",fileName2);
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

int UnionPrintPKExpectedInKeyDBToFile(PUnionPKDB pdesKeyDB,char *app,char *owner,char *name,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	short	appEQExpected=0,ownerEQExpected=0,nameEQExpected=0;
	char	thisApp[40+1],thisOwner[40+1],thisName[40+1];
	char	nameOfType[20];
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintPKExpectedInKeyDBToFile:: fp is NULL!\n");
		return(-1);
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
		ret = -1;
		goto exitRightNow;
	}
	
	if ((app != NULL) && (strlen(app) > 0))
		appEQExpected = 1;
	if ((owner != NULL) && (strlen(owner) > 0))
		ownerEQExpected = 1;
	if ((name != NULL) && (strlen(name) > 0))
		nameEQExpected = 1;
			
	fprintf(fp,"[%30s][%20s][%4s][%s]\n",
		"密钥名称","密钥类型","长度","密钥校验值");
	for (i = 0; i < pdesKeyDB->num; i++)
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
		fprintf(fp,"[%30s][%20s][%04d][%s]\n",
			(pgunionPK+i)->fullName,
			nameOfType,
			UnionGetAscCharLengthOfPK((pgunionPK+i)->length),
			(pgunionPK+i)->checkValue);
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
	return(UnionPrintPKExpectedInKeyDBToFile(pgunionPKDB,app,owner,name,fp));
}

int UnionPrintPKInKeyDBOutofDateToFile(PUnionPKDB pdesKeyDB,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	char	today[10];
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBOutofDateToFile:: fp is NULL!\n");
		return(-1);
	}
	
	if (pgunionPK == NULL)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBOutofDateToFile:: pkey is NULL!\n");
		ret = -1;
		goto exitRightNow;
	}
	
	memset(today,0,sizeof(today));
	UnionGetFullSystemDate(today);
	//printf("Today = [%s]\n",today);			
	for (i = 0; i < pdesKeyDB->num; i++)
	{
		//printf("(pgunionPK+i)->passiveDate = [%s]\n",(pgunionPK+i)->passiveDate);
		if (strncmp(today,(pgunionPK+i)->passiveDate,8) < 0)
			continue;
		totalNum++;
		fprintf(fp,"[%35s][%s][%s]\n",(pgunionPK+i)->fullName,(pgunionPK+i)->activeDate,(pgunionPK+i)->passiveDate);
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
	return(UnionPrintPKInKeyDBOutofDateToFile(pgunionPKDB,fp));
}

int UnionPrintPKInKeyDBNearOutofDateToFile(PUnionPKDB pdesKeyDB,long days,FILE *fp)
{
	int	ret;
	long	i;
	long	totalNum = 0;
	long	tmpDays;
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBNearOutofDateToFile:: fp is NULL!\n");
		return(-1);
	}
	
	if (pgunionPK == NULL)
	{
		UnionUserErrLog("in UnionPrintPKInKeyDBNearOutofDateToFile:: pkey is NULL!\n");
		ret = -1;
		goto exitRightNow;
	}
	
	for (i = 0; i < pdesKeyDB->num; i++)
	{
		if ((tmpDays = UnionDecideDaysAfterToday((pgunionPK+i)->passiveDate)) < 0)
			continue;
		if (tmpDays > days)
			continue;
		totalNum++;
		fprintf(fp,"[%35s][%s][%s]\n",(pgunionPK+i)->fullName,(pgunionPK+i)->activeDate,(pgunionPK+i)->passiveDate);
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
	return(UnionPrintPKInKeyDBNearOutofDateToFile(pgunionPKDB,days,fp));
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
	
	if ((pgunionPK == NULL) || (pgunionPKDB == NULL))
	{
		UnionUserErrLog("in UnionSynchronizeAllPKToBackuper:: pkey is NULL!\n");
		return(-1);
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
	
	if ((pgunionPK == NULL) || (pgunionPKDB == NULL))
	{
		UnionUserErrLog("in UnionSynchronizePKOfSpecifiedAppToBackuper:: pkey is NULL!\n");
		return(-1);
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
	
	if ((pgunionPK == NULL) || (pgunionPKDB == NULL))
	{
		if ((ret = UnionConnectPKDB()) < 0)
		{
			UnionUserErrLog("in UnionPKDBOperation:: UnionConnectPKDB Error!\n");
			return(ret);
		}
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
				if ((ret = UnionUpdatePKInKeyDB(pkey)) < 0)
					UnionUserErrLog("in UnionPKDBOperation:: UnionUpdatePKInKeyDB [%s]\n",pkey->fullName);
				else
					UnionSuccessLog("in UnionPKDBOperation:: UnionUpdatePKInKeyDB [%s] OK\n",pkey->fullName);
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

