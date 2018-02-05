//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2003/09/08

//	2004/11/20,Wolfgang Wang,修改了函数：UnionLogDesKeyDBOperation

//	2006/4/14 在unionDesKeyDBLog3.0.20041125.c基础上升级
//	增加了最大的DESKEYDBLOG的尺寸，为500M。

// 2007/11/29 打开这个文件中的UnionIsDesKeyValueDynamicUpdated控制。
// 将密钥库日志的大小上限设为30M。

// 2007/11/30 增加一个编译开关，_useKeySynchronizer_,控制同步密钥采用的机制。
// 当在unionREC.CFG中定义sizeOfDesKeyDBLogFile = 0时，不写密钥库日志

#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#ifndef _UnionDesKey_3_x_
#define _UnionDesKey_3_x_
#endif

#include "unionModule.h"
#include "UnionStr.h"
#include "unionREC.h"

#include "unionDesKeyDBLog.h"
#include "UnionLog.h"

#include "keyDBBackuper.h"	// 2004/11/20 Added by Wolfgang Wang
#ifdef _useKeySynchronizer_
#include "kdbSvrService.h" 
#endif

int	pgisStoring = 0;

int UnionGetNameOfDesKeyDBLog(char *fileName)
{
	sprintf(fileName,"%s/keyFile/unionDesKeyDB.LOG",getenv("UNIONETC"));
	return(0);
}

int UnionOpenRestoringDesKeyDBFromLog()
{
	pgisStoring = 1;
	return(0);
}

int UnionCloseRestoringDesKeyDBFromLog()
{
	pgisStoring = 0;
	return(0);
}

int UnionLogDesKeyDBOperation(TUnionDesKeyDBLogOperation operation,PUnionDesKey pdesKey)
{
	FILE	*fp;
	char	fileName[512];
	TUnionDesKeyDBLogRecord	logRec;
	long	sizeOfDBLogFile;
		
	if (pgisStoring)
		return(0);
		
	if (pdesKey == NULL)
		return(0);

	// 2007/11/30修改以下代码	
	if (!UnionIsKeyDBBackupServer())		// 不是密钥备份服务器，同步这个操作
	{
#ifdef _useKeySynchronizer_
		UnionApplyKeySynchronizeServiceNowait(operation,conIsDesKeyDB,(unsigned char *)pdesKey,sizeof(*pdesKey));
#else
		UnionSynchronizeDesKeyDBOperation(operation,pdesKey);
#endif
	}
	// 2007/11/30修改完成
	
	if (UnionIsDesKeyValueDynamicUpdated(pdesKey)) // 2007/11/29 打开
	{
		switch (operation)
		{
			case	conUpdateRecordValueFlds:
			case	conCompareKeyValue:
				return(0);
			default:
				break;
		}
	}
			
	// 2007/11/30，修改这一段
	if (((sizeOfDBLogFile = UnionReadIntTypeRECVar("sizeOfDesKeyDBLogFile") * 1000000) < 0) || 
		(sizeOfDBLogFile > 30000000))
		sizeOfDBLogFile = 30000000;
	else if (sizeOfDBLogFile == 0)	// 2007/11/30增加，当定义sizeOfDesKeyDBLogFile == 0时，不写日志
		return(0);
	// 2007/11/30，修改结束
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfDesKeyDBLog(fileName);
	if ((fp = fopen(fileName,"a")) == NULL)
	{
		UnionSystemErrLog("in UnionLogDesKeyDBOperation:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if (ftell(fp) > sizeOfDBLogFile)
	{
		fclose(fp);
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionLogDesKeyDBOperation:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}	
	memset(&logRec,0,sizeof(logRec));
	logRec.operation = operation;
	memcpy(&(logRec.desKey),pdesKey,sizeof(*pdesKey));
	UnionGetFullSystemDateTime(logRec.time);
	fwrite(&logRec,sizeof(logRec),1,fp);
	fclose(fp);
	return(0);
}

int UnionPrintDesKeyDBLog(char *startTime,char *endTime)
{
	FILE	*fp;
	char	fileName[512];
	TUnionDesKeyDBLogRecord	logRec;
	long	num = 0;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfDesKeyDBLog(fileName);
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintDesKeyDBLog:: fopen [%s]\n",fileName);
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
		printf("%14s::%20s::%s\n",logRec.time,logRec.desKey.fullName,logRec.desKey.value);
		/*
		if ((num != 0) && (num % 23 == 0))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
		*/
	}
	printf("total %d num!\n",num);
	fclose(fp);
	return(0);
}

int UnionRestoreDesKeyDBFromLog(char *startTime,char *endTime)
{
	FILE	*fp;
	char	fileName[512];
	TUnionDesKeyDBLogRecord	logRec;
	int	ret;
	char	currentTime[14+1];
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfDesKeyDBLog(fileName);
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionRestoreDesKeyDBFromLog:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	memset(currentTime,0,sizeof(currentTime));
	UnionGetFullSystemDateTime(currentTime);
	
	UnionOpenRestoringDesKeyDBFromLog();
	
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
				if ((ret = UnionInsertDesKeyIntoKeyDB(&(logRec.desKey))) < 0)
					UnionUserErrLog("in UnionRestoreDesKeyDBFromLog:: UnionInsertDesKeyIntoKeyDB [%s]\n",logRec.desKey.fullName);
				else
				{
					UnionSuccessLog("in UnionRestoreDesKeyDBFromLog:: UnionInsertDesKeyIntoKeyDB [%s] OK\n",logRec.desKey.fullName);
					printf("UnionInsertDesKeyIntoKeyDB [%s] OK\n",logRec.desKey.fullName);
				}
				continue;
			case	conRecordUpdate:
				if ((ret = UnionUpdateAllDesKeyFieldsInKeyDB(&(logRec.desKey))) < 0)
					UnionUserErrLog("in UnionRestoreDesKeyDBFromLog:: UnionUpdateAllDesKeyFieldsInKeyDB [%s]\n",logRec.desKey.fullName);
				else
				{
					UnionSuccessLog("in UnionRestoreDesKeyDBFromLog:: UnionUpdateAllDesKeyFieldsInKeyDB [%s] OK\n",logRec.desKey.fullName);
					printf("UnionUpdateAllDesKeyFieldsInKeyDB [%s] OK\n",logRec.desKey.fullName);
				}
				continue;
			case	conRecordDelete:
				if ((ret = UnionDeleteDesKeyFromKeyDB(logRec.desKey.fullName)) < 0)
					UnionUserErrLog("in UnionRestoreDesKeyDBFromLog:: UnionDeleteDesKeyFromKeyDB [%s]\n",logRec.desKey.fullName);
				else
				{
					UnionSuccessLog("in UnionRestoreDesKeyDBFromLog:: UnionDeleteDesKeyFromKeyDB [%s] OK\n",logRec.desKey.fullName);
					printf("UnionDeleteDesKeyFromKeyDB [%s] OK\n",logRec.desKey.fullName);
				}
				continue;
			default:
				printf("Unknown::");
				break;
		}
	}
	UnionCloseRestoringDesKeyDBFromLog();
	
	fclose(fp);
	return(0);
}

// Added by Wolfgang Wang, 2004/11/20
int UnionSynchronizeDesKeyDBOperation(TUnionDesKeyDBLogOperation operation,PUnionDesKey pdesKey)
{
	return(UnionSynchronizeKeyDBOperation(operation,conIsDesKeyDB,(unsigned char *)pdesKey,sizeof(*pdesKey)));
}
