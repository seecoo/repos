//	Author:		wangk
//	Date:		2009-9-22
//	Version:	1.0

#define _UnionLogMDL_3_x_	
#define _UnionEnv_3_x_	
#define _LOG_FILE_NUM_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "UnionStr.h"
#include "unionCommand.h"
#include "unionUnionLogTBL.h"

PUnionSharedMemoryModule	pgunionLogFileMDL = NULL;
PUnionLogFileTBL		pgunionLogFileTBL = NULL;
PUnionLogFile			pgunionLogFile = NULL;

// 2006/08/08增加
PUnionLogFile UnionGetCurrentLogFileTBL()
{
	if (UnionConnectLogFileTBL() < 0)
		return(NULL);
	else
		return(pgunionLogFile);
}

// 2006/08/08增加
int UnionGetCurrentMaxNumOfLogFile()
{
	int	ret;
	
	if ((ret = UnionConnectLogFileTBL()) < 0)
		return(ret);
	else
		return(pgunionLogFileTBL->maxLogFileNum);
}

int UnionIsLogFileTBLConnected()
{
	if ((pgunionLogFile == NULL) || (pgunionLogFileTBL == NULL) || (!UnionIsSharedMemoryInited(conMDLNameOfUnionLogTBL)))
		return(0);
	else
		return(1);
}

int UnionConnectLogFileTBL()
{
	int				num;
	
	if (UnionIsLogFileTBLConnected())	// 已经连接
		return(0);
		
	if ((num = UnionGetMaxNumOfLogFile()) <= 0)
	{
		UnionUserErrLog("in UnionConnectLogFileTBL:: UnionGetMaxNumOfLogFile [%d]\n",num);
		return(num);
	}
	
	if ((pgunionLogFileMDL = UnionConnectSharedMemoryModule(conMDLNameOfUnionLogTBL,
			sizeof(TUnionLogFileTBL) + sizeof(TUnionLogFile) * num)) == NULL)
	{
		UnionUserErrLog("in UnionConnectLogFileTBL:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	//printf("UnionConnectSharedMemoryModule pgunionLogFileMDL = [%0x]!\n",pgunionLogFileMDL); 	
	if ((pgunionLogFileTBL = (PUnionLogFileTBL)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionLogFileMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectLogFileTBL:: PUnionLogFileTBL!\n");
		return(errCodeSharedMemoryModule);
	}
	
	//printf("UnionConnectSharedMemoryModule pgunionLogFileTBL = [%0x]!\n",pgunionLogFileTBL); 	
	if ((pgunionLogFileTBL->plogFile = (PUnionLogFile)((unsigned char *)pgunionLogFileTBL + sizeof(*pgunionLogFileTBL))) == NULL)
	{
		UnionUserErrLog("in UnionConnectLogFileTBL:: PUnionLogFile!\n");
		return(errCodeSharedMemoryModule);
	}
	pgunionLogFile = pgunionLogFileTBL->plogFile;

	
	//printf("UnionConnectSharedMemoryModule pgunionLogFile = [%0x]!\n",pgunionLogFile); 	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionLogFileMDL))
	{
		pgunionLogFileTBL->maxLogFileNum = num;
		return(UnionReloadLogFileTBL());
	}
	else
		return(0);
}

	
int UnionDisconnectLogFileTBL()
{
	pgunionLogFileTBL = NULL;
	UnionDestroyILOG();
	return(UnionDisconnectShareModule(pgunionLogFileMDL));
}

int UnionRemoveLogFileTBL()
{
	UnionDisconnectLogFileTBL();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfUnionLogTBL));
}

int UnionPrintLogFileTBLToFile(FILE *fp)
{
	int	i;
	int	num=0;
	int	ret;
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintLogFileTBLToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
		
	if ((ret = UnionConnectLogFileTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintLogFileTBLToFile:: UnionConnectLogFileTBL!\n");
		return(ret);
	}
	
	fprintf(fp,"%40s %4s %4s %6s %4s %12s %6s\n","名称","级别","个数","尺寸","属性","大小","用户数");
	for (i = 0; i < pgunionLogFileTBL->maxLogFileNum; i++)
	{
		if (UnionPrintLogFileToFile(pgunionLogFile+i,fp) < 0)
			continue;
		num++;
		if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
	}
	fprintf(fp,"logFileNum = [%d]\n",num);
	//if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
	//	return(0);
	//UnionPrintSharedMemoryModuleToFile(pgunionLogFileMDL,fp);
	return(0);
}

int UnionPrintLogFileTBL()
{
	return(UnionPrintLogFileTBLToFile(stdout));
}

int UnionAddLogFile(char *logFileName)
{
	int		i;
	PUnionLogFile	plogFile;
	TUnionLogFile	logFile;
	int		ret;
	
	if ((ret = UnionConnectLogFileTBL()) < 0)
	{
		UnionUserErrLog("in UnionAddLogFile:: UnionConnectLogFileTBL!\n");
		return(ret);
	}

	if ((plogFile = UnionConnectLogFile(logFileName)) != NULL)
	{
		UnionUserErrLog("in UnionAddLogFile:: [%s] exists!\n",logFileName);
		return(errCodeLogMDL_LogFileAlreadyExists);
	}
	
	memset(&logFile,0,sizeof(logFile));
	if ((ret = UnionReadLogFileDef(logFileName,&logFile)) < 0)
	{
		UnionUserErrLog("in UnionAddLogFile:: UnionReadLogFileDef [%s]!\n",logFileName);
		return(ret);
	}
	
	for (i = 0; i < pgunionLogFileTBL->maxLogFileNum; i++)
	{
		if (strlen((pgunionLogFile+i)->name) == 0)
		{
			memcpy(pgunionLogFile+i,&logFile,sizeof(logFile));
			return(0);
		}
	}
	
	UnionUserErrLog("in UnionAddLogFile:: logFileTBL is full!\n");
	
	return(errCodeLogMDL_LogFileTblFull);
}
	
int UnionDeleteLogFile(char *logFileName)
{
	int		i;
	int		ret;
	
	if ((ret = UnionConnectLogFileTBL()) < 0)
	{
		UnionUserErrLog("in UnionDeleteLogFile:: UnionConnectLogFileTBL!\n");
		return(ret);
	}

	for (i = 0; i < pgunionLogFileTBL->maxLogFileNum; i++)
	{
		if (strcmp((pgunionLogFile+i)->name,logFileName) == 0)
		{
			if ((pgunionLogFile+i)->users > 0)
			{
				UnionUserErrLog("in UnionDeleteLogFile:: there is still users for this logfile [%s]\n",logFileName);
				return(errCodeLogMDL_LogFileUsedByUsers);
			}
			memset(pgunionLogFile+i,0,sizeof(*(pgunionLogFile+i)));
			return(0);
		}
	}
	
	UnionUserErrLog("in UnionDeleteLogFile:: logFileName[%s] is not exists!\n",logFileName);
	
	return(errCodeLogMDL_LogFileNotExists);
}

PUnionLogFile UnionConnectLogFile(char *logFileName)
{
	int	i;
	int	ret;

	if ((ret = UnionConnectLogFileTBL()) < 0)
	{
		UnionUserErrLog("in UnionConnectLogFile:: UnionConnectLogFileTBL!\n");
		return(NULL);
	}
	for (i = 0; i < pgunionLogFileTBL->maxLogFileNum; i++)
	{
		if (strcmp(logFileName,(pgunionLogFile+i)->name) == 0)
		{
			(pgunionLogFile+i)->users += 1;
			return(pgunionLogFile+i);
		}
	}
	/*
	// 没有找到指定的日志文件，将日志定入unionlog.log
	for (i = 0; i < pgunionLogFileTBL->maxLogFileNum; i++)
	{
		if (strcmp("unionlog",(pgunionLogFile+i)->name) == 0)
		{
			(pgunionLogFile+i)->users += 1;
			return(pgunionLogFile+i);
		}
	}
	*/
	return(NULL);
}

int UnionDisconnectLogFile(PUnionLogFile plogFile)
{
	if (plogFile == NULL)
		return(0);
	UnionDeleteILOG(plogFile->name);
	if (plogFile->users > 0)
		--plogFile->users;
	plogFile = NULL;
	return(0);
}

// 20060825 增加
int UnionExistLogFile(char *logFileName)
{
	int	i;
	int	ret;

	if ((logFileName == NULL) || (strlen(logFileName) == 0))
		return(errCodeParameter);
		
	if ((ret = UnionConnectLogFileTBL()) < 0)
	{
		UnionUserErrLog("in UnionExistLogFile:: UnionConnectLogFileTBL!\n");
		return(ret);
	}
	for (i = 0; i < pgunionLogFileTBL->maxLogFileNum; i++)
	{
		if (strcmp(logFileName,(pgunionLogFile+i)->name) == 0)
		{
			return(1);
		}
	}
	return(errCodeLogMDL_LogFileNotExists);
}

// 20060825增加
PUnionLogFile UnionFindLogFile(char *logFileName)
{
	int	i;
	int	ret;

	if ((ret = UnionConnectLogFileTBL()) < 0)
	{
		UnionUserErrLog("in UnionFindLogFile:: UnionConnectLogFileTBL!\n");
		return(NULL);
	}
	for (i = 0; i < pgunionLogFileTBL->maxLogFileNum; i++)
	{
		if (strcmp(logFileName,(pgunionLogFile+i)->name) == 0)
		{
			return(pgunionLogFile+i);
		}
	}
	return(NULL);
}

// 20060825 增加
int UnionResetLogFileUsers(char *logFileName)
{
	int	i;
	int	ret;

	if ((logFileName == NULL) || (strlen(logFileName) == 0))
		return(errCodeParameter);
		
	if ((ret = UnionConnectLogFileTBL()) < 0)
	{
		UnionUserErrLog("in UnionResetLogFileUsers:: UnionConnectLogFileTBL!\n");
		return(ret);
	}
	for (i = 0; i < pgunionLogFileTBL->maxLogFileNum; i++)
	{
		if (strcmp(logFileName,(pgunionLogFile+i)->name) == 0)
		{
			(pgunionLogFile+i)->users = 0;
			return(1);
		}
	}
	return(errCodeLogMDL_LogFileNotExists);
}

int UnionPrintLogFileTBLToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
		
	if ((fileName == NULL) || (strlen(fileName) == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintLogFileTBLToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	ret = UnionPrintLogFileTBLToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

// wangk add 2009-9-24
int UnionPrintLogFileInRecStrFormatToFile(PUnionLogFile plogFile,FILE *fp)
{
	int	offset = 0;
	int ret = 0;
	char recStr[1024];
	int sizeOfBuf = 0;

	if ((fp == NULL) || (plogFile == NULL))
		return(errCodeParameter);

	if (strlen(plogFile->name) == 0)
		return(0);

	memset(recStr, 0, sizeof(recStr));
	sizeOfBuf = sizeof(recStr);

	ret = UnionPutRecFldIntoRecStr("name",plogFile->name,strlen(plogFile->name),recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionPrintLogFileInRecStrFormatToFile:: UnionPutRecFldIntoRecStr [name] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;

	ret = UnionPutIntTypeRecFldIntoRecStr("level",plogFile->level,recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionPrintLogFileInRecStrFormatToFile:: UnionPutIntTypeRecFldIntoRecStr [level] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutIntTypeRecFldIntoRecStr("maxSize",plogFile->maxSize/1000000,recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionPrintLogFileInRecStrFormatToFile:: UnionPutIntTypeRecFldIntoRecStr [maxSize] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutIntTypeRecFldIntoRecStr("rewriteAttr",plogFile->rewriteAttr,recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionPrintLogFileInRecStrFormatToFile:: UnionPutIntTypeRecFldIntoRecStr [rewriteAttr] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutIntTypeRecFldIntoRecStr("currentSize",plogFile->currentSize,recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionPrintLogFileInRecStrFormatToFile:: UnionPutIntTypeRecFldIntoRecStr [currentSize] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;
	ret = UnionPutIntTypeRecFldIntoRecStr("users",plogFile->users,recStr+offset,sizeOfBuf-offset);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionPrintLogFileInRecStrFormatToFile:: UnionPutIntTypeRecFldIntoRecStr [users] error!\n");
		return(UnionSetUserDefinedErrorCode(ret));
	}
	offset += ret;

	fprintf(fp, "%s\n", recStr);

	return(0);
}

int UnionPrintLogFileTBLInRecStrFormatToFile(FILE *fp)
{
	int	i;
	int	ret;
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintLogFileTBLInRecStrFormatToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
		
	if ((ret = UnionConnectLogFileTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintLogFileTBLInRecStrFormatToFile:: UnionConnectLogFileTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionLogFileTBL->maxLogFileNum; i++)
	{
		if (UnionPrintLogFileInRecStrFormatToFile(pgunionLogFile+i,fp) < 0)
			continue;
	}


	return(0);
}

int UnionPrintLogFileTBLInRecStrFormatToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
		
	if ((fileName == NULL) || (strlen(fileName) == 0))
	{
		UnionUserErrLog("in UnionPrintLogFileTBLInRecStrFormatToSpecFile:: NULL poionter!\n");
		return(errCodeParameter);
	}

	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintLogFileTBLInRecStrFormatToSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	ret = UnionPrintLogFileTBLInRecStrFormatToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}
// wangk add end 2009-9-24
