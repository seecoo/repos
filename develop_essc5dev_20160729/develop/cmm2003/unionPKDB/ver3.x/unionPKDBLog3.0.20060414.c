//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2004/11/25

// 2004/11/25�� unionPKDBLog.20041111.c����������

// 2006/4/14��unionPKDBLog3.0.20041125����������
//	����������KEYDBLOG�ĳߴ磬Ϊ500M��

#ifndef _UnionPK_3_x_
#define _UnionPK_3_x_
#endif

#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "UnionStr.h"
#include "unionErrCode.h"

#include "keyDBBackuper.h"

#include "unionPKDBLog.h"
#include "UnionLog.h"

int pgisPKDBRestoring = 0;

int UnionGetNameOfPKDBLog(char *fileName)
{
	sprintf(fileName,"%s/unionPKDB.LOG",getenv("UNIONETC"));
	return(0);
}

int UnionOpenRestoringPKDBFromLog()
{
	pgisPKDBRestoring = 1;
	return(0);
}

int UnionCloseRestoringPKDBFromLog()
{
	pgisPKDBRestoring = 0;
	return(0);
}

int UnionLogPKDBOperation(TUnionPKDBLogOperation operation,PUnionPK pkey)
{
	FILE	*fp;
	char	fileName[512];
	TUnionPKDBLogRecord	logRec;
	
	if (pgisPKDBRestoring)
		return(0);
		
	if (pkey == NULL)
		return(0);

	// Wolfgang Wang, 2004/11/20
	UnionSynchronizePKDBOperation(operation,pkey);
	// End of addition of 2004/11/20

	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfPKDBLog(fileName);
	if ((fp = fopen(fileName,"a")) == NULL)
	{
		UnionSystemErrLog("in UnionLogPKDBOperation:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if (ftell(fp) >= 500000000)
	{
		fclose(fp);
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionLogPKDBOperation:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}	
	memset(&logRec,0,sizeof(logRec));
	logRec.operation = operation;
	memcpy(&(logRec.key),pkey,sizeof(*pkey));
	UnionGetFullSystemDateTime(logRec.time);
	fwrite(&logRec,sizeof(logRec),1,fp);
	fclose(fp);
	return(0);
}

int UnionPrintPKDBLog(char *startTime,char *endTime)
{
	FILE	*fp;
	char	fileName[512];
	TUnionPKDBLogRecord	logRec;
	long	num = 0;
	char	tmpBuf[2048+1];
		
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfPKDBLog(fileName);
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintPKDBLog:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	while (!feof(fp))
	{
		memset(&logRec,0,sizeof(logRec));
		if (fread(&logRec,sizeof(logRec),1,fp) != 1)
			continue;
		if (endTime != NULL)
		{
			if (strcmp(logRec.time,endTime) > 0)
				break;
		}
		if (startTime != NULL)
		{
			if (strcmp(logRec.time,startTime) < 0)
				continue;
		}
		num++;
		switch (logRec.operation)
		{
			case	conRecordInsert:
				printf("Insert::");
				break;	
			case	conRecordUpdate:
				printf("Update::");
				break;	
			case	conRecordDelete:
				printf("Delete::");
				break;	
			default:
				printf("Unknown::");
				break;
		}
		memset(tmpBuf,0,sizeof(tmpBuf));
		bcdhex_to_aschex(logRec.key.value,UnionGetAscCharLengthOfPK(logRec.key.length)/2,tmpBuf);
		if (UnionIsStringContainingOnlyZero(tmpBuf,UnionGetAscCharLengthOfPK(logRec.key.length)))
		{
			memset(tmpBuf,0,sizeof(tmpBuf));
		}
		printf("%14s::%20s::%04d::%s\n",logRec.time,logRec.key.fullName,UnionGetAscCharLengthOfPK(logRec.key.length),tmpBuf);
		if ((num != 0) && (num % 23 == 0))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
	}
	printf("total %d num!\n",num);
	fclose(fp);
	return(0);
}

int UnionRestorePKDBFromLog(char *startTime,char *endTime)
{
	FILE	*fp;
	char	fileName[512];
	TUnionPKDBLogRecord	logRec;
	int	ret;
	char	currentTime[14+1];
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfPKDBLog(fileName);
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionRestorePKDBFromLog:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	memset(currentTime,0,sizeof(currentTime));
	UnionGetFullSystemDateTime(currentTime);
	
	UnionOpenRestoringPKDBFromLog();
	
	while (!feof(fp))
	{
		memset(&logRec,0,sizeof(logRec));
		if (fread(&logRec,sizeof(logRec),1,fp) != 1)
			continue;
		if (startTime != NULL)
		{
			if (strcmp(logRec.time,startTime) < 0)
				continue;
		}
		if (endTime != NULL)
		{
			if (strcmp(logRec.time,endTime) > 0)
				break;
		}
		if (strcmp(logRec.time,currentTime) >= 0)
			break;
		switch (logRec.operation)
		{
			case	conRecordInsert:
				if ((ret = UnionInsertPKIntoKeyDB(&(logRec.key))) < 0)
					UnionUserErrLog("in UnionRestorePKDBFromLog:: UnionInsertPKIntoKeyDB [%s]\n",logRec.key.fullName);
				else
				{
					UnionSuccessLog("in UnionRestorePKDBFromLog:: UnionInsertPKIntoKeyDB [%s] OK\n",logRec.key.fullName);
					printf("UnionInsertPKIntoKeyDB [%s] OK\n",logRec.key.fullName);
				}
				continue;
			case	conRecordUpdate:
				if ((ret = UnionUpdateAllPKFieldsInKeyDB(&(logRec.key))) < 0)
					UnionUserErrLog("in UnionRestorePKDBFromLog:: UnionUpdateAllPKFieldsInKeyDB [%s]\n",logRec.key.fullName);
				else
				{
					UnionSuccessLog("in UnionRestorePKDBFromLog:: UnionUpdateAllPKFieldsInKeyDB [%s] OK\n",logRec.key.fullName);
					printf("UnionUpdateAllPKFieldsInKeyDB [%s] OK\n",logRec.key.fullName);
				}
				continue;
			case	conRecordDelete:
				if ((ret = UnionDeletePKFromKeyDB(logRec.key.fullName)) < 0)
					UnionUserErrLog("in UnionRestorePKDBFromLog:: UnionDeletePKFromKeyDB [%s]\n",logRec.key.fullName);
				else
				{
					UnionSuccessLog("in UnionRestorePKDBFromLog:: UnionDeletePKFromKeyDB [%s] OK\n",logRec.key.fullName);
					printf("UnionDeletePKFromKeyDB [%s] OK\n",logRec.key.fullName);
				}
				continue;
			default:
				printf("Unknown::");
				break;
		}
	}
	UnionCloseRestoringPKDBFromLog();
	
	fclose(fp);
	return(0);
}

int UnionSynchronizePKDBOperation(TUnionPKDBLogOperation operation,PUnionPK ppk)
{
	return(UnionSynchronizeKeyDBOperation(operation,conIsPKDB,(unsigned char *)ppk,sizeof(*ppk)));
}
