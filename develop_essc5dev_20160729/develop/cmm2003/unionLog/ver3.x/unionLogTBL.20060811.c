//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	1.0

#define _UnionLogMDL_3_x_	
#define _UnionEnv_3_x_	

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "unionCommand.h"

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

int UnionGetNameOfLogFileTBL(char *fileName)
{
	sprintf(fileName,"%s/unionLogTBL.CFG",getenv("UNIONETC"));
	return(0);
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
	return(UnionDisconnectShareModule(pgunionLogFileMDL));
}

int UnionRemoveLogFileTBL()
{
	UnionDisconnectLogFileTBL();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfUnionLogTBL));
}

int UnionReloadLogFileTBL()
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		i;
	PUnionLogFile	plogFile;
	int		realNum;
	
	if ((ret = UnionConnectLogFileTBL()) < 0)
	{
		UnionUserErrLog("in UnionReloadLogFileTBL:: UnionConnectLogFileTBL!\n");
		return(ret);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfLogFileTBL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionReloadLogFileTBL:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("MaxNumOfLogFile")) == NULL)
	{
		UnionUserErrLog("in UnionReloadLogFileTBL:: UnionGetEnviVarByName for [%s]\n!","MaxNumOfLogFile");
		UnionClearEnvi();
		return(errCodeLogMDL_ConfFile);
	}
	pgunionLogFileTBL->maxLogFileNum = atoi(p);
	for (i = 0; i < pgunionLogFileTBL->maxLogFileNum; i++)
	{
		if (strlen((pgunionLogFile+i)->name) == 0)
			continue;
		if ((p = UnionGetEnviVarByName((pgunionLogFile+i)->name)) == NULL)
		{
			if ((pgunionLogFile+i)->users > 0)
				continue;
			//UnionNullLog("[%s] not used and defined,delete it\n",(pgunionLogFile+i)->name);
			memset(pgunionLogFile+i,0,sizeof(*plogFile));
		}
	}
	
	realNum = 0;
	for (i = 0; (i < UnionGetEnviVarNum()) && (realNum < pgunionLogFileTBL->maxLogFileNum); i++)
	{
		// 读取名称
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
		{
			UnionUserErrLog("in UnionReloadLogFileTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,0);
			continue;
		}
		if (strlen(p) > sizeof(plogFile->name) - 1)
			continue;
		if (strcmp(p,"MaxNumOfLogFile") == 0)
			continue;
		if ((plogFile = UnionConnectLogFile(p)) == NULL)
		{
			//UnionNullLog("in UnionReloadLogFileTBL:: [%s] not in logfiletable, load it!\n",p);
			for (;realNum < pgunionLogFileTBL->maxLogFileNum;realNum++)
			{
				if (strlen((plogFile = pgunionLogFile + realNum)->name) == 0)
					break;
				else
					continue;
			}
			if (realNum >=  pgunionLogFileTBL->maxLogFileNum)
			{
				UnionUserErrLog("in UnionReloadLogFileTBL:: logfiletbl is full!\n");
				break;
			}
			memset(plogFile,0,sizeof(*plogFile));
			plogFile->users = 0;
			strcpy(plogFile->name,p);
		}
		else
			--plogFile->users;
		
	
		// 读取Level
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
		{
			UnionUserErrLog("in UnionReloadLogFileTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
			continue;
		}
		plogFile->level = atoi(p);
		//if ((plogFile->level < conLogNoneLogLevel) || (plogFile->level > conLogAlwaysLevel))
		if (plogFile->level > conLogAlwaysLevel)
			plogFile->level = conLogAuditLevel;
			
		// 读取日志的最大尺寸
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,2)) == NULL)
		{
			UnionUserErrLog("in UnionReloadLogFileTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,2);
			continue;
		}
		plogFile->maxSize = atol(p) * 1000000;	// 日志文件中的尺寸以M计
		if (plogFile->maxSize <= 0)
			plogFile->maxSize = 1000000;
		
		// 读取重写标志
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,3)) == NULL)
		{
			UnionUserErrLog("in UnionReloadLogFileTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,3);
			continue;
		}
		switch (plogFile->rewriteAttr = atoi(p))
		{	
			case	conLogFullRewriteAtonce:
			case	conLogFullRewriteAfterBackup:
			case	conLogNewEveryDay:
			case	conLogNewEveryMonth:
			case	conLogNewEverySeason:
			case	conLogNewHalfYear:
			case	conLogNewEveryYear:
				break;
			default:
				plogFile->rewriteAttr = conLogFullRewriteAtonce;
				break;
		}
		
		UnionCalSizeOfLogFile(plogFile);
		
		++realNum;
	}
	
	UnionClearEnvi();
		
	return(0);
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
	
	UnionUserErrLog("in UnionDeleteLogFile:: logFileName [%s] is not exists!\n",logFileName);
	
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
	if (plogFile->users > 0)
		--plogFile->users;
	plogFile = NULL;
	return(0);
}

int UnionReadLogFileDef(char *logFileName,PUnionLogFile plogFile)
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		i;
	
	if ((logFileName == NULL) || (plogFile == NULL))
	{
		UnionUserErrLog("in UnionReadLogFileDef:: logFileName or plogFile is NULL!\n");
		return(errCodeParameter);
	}
	if (strlen(logFileName) > sizeof(plogFile->name) - 1)
	{
		UnionUserErrLog("in UnionReadLogFileDef:: logFileName [%s] longer than expected [%d]!\n",logFileName,(int)sizeof(plogFile->name)-1);
		return(errCodeParameter);
	}

	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfLogFileTBL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionReadLogFileDef:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((i = UnionGetVarIndexOfTheVarName(logFileName)) < 0)
	{
		UnionUserErrLog("in UnionReadLogFileDef:: UnionGetVarIndexOfTheVarName for [%s]\n!",logFileName);
		goto abnormalExit;
	}
	
	// 找到了该日志的定义
	// 读取名称
	
	memset(plogFile,0,sizeof(*plogFile));
	strcpy(plogFile->name,logFileName);
	// 读取Level
	if ((p = UnionGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
	{
		UnionUserErrLog("in UnionReadLogFileDef:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
		goto abnormalExit;
	}
	plogFile->level = atoi(p);
	//if ((plogFile->level < conLogNoneLogLevel) || (plogFile->level > conLogAlwaysLevel))
	if (plogFile->level > conLogAlwaysLevel)
		plogFile->level = conLogAuditLevel;
			
	// 读取日志的最大尺寸
	if ((p = UnionGetEnviVarOfTheIndexByIndex(i,2)) == NULL)
	{
		UnionUserErrLog("in UnionReadLogFileDef:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,2);
		goto abnormalExit;
	}
	plogFile->maxSize = atol(p) * 1000000;	// 日志文件中的尺寸以M计
	if (plogFile->maxSize <= 0)
		plogFile->maxSize = 1000000;
		
	// 读取重写标志
	if ((p = UnionGetEnviVarOfTheIndexByIndex(i,3)) == NULL)
	{
		UnionUserErrLog("in UnionReadLogFileDef:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,3);
		goto abnormalExit;
	}
	switch (plogFile->rewriteAttr = atoi(p))
	{
		case	conLogFullRewriteAtonce:
		case	conLogFullRewriteAfterBackup:
		case	conLogNewEveryDay:
		case	conLogNewEveryMonth:
		case	conLogNewEverySeason:
		case	conLogNewHalfYear:
		case	conLogNewEveryYear:
			break;
		default:
			plogFile->rewriteAttr = conLogFullRewriteAtonce;
			break;
	}
		
	UnionClearEnvi();
	
	UnionCalSizeOfLogFile(plogFile);

	plogFile->users = 0;
	
	UnionLog("in UnionReadLogFileDef:: [%s] [%d] [%ld] [%d] [%ld]\n",plogFile->name,plogFile->level,plogFile->maxSize,plogFile->rewriteAttr,plogFile->currentSize);
		
	return(0);
	
abnormalExit:
	UnionUserErrLog("in UnionReadLogFileDef:: [%s] not defined in file [%s]\n",logFileName,fileName);
	UnionClearEnvi();
	return(errCodeLogMDL_LogFileNotExists);
	
}

int UnionGetMaxNumOfLogFile()
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		num;
		
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfLogFileTBL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionGetMaxNumOfLogFile:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("MaxNumOfLogFile")) == NULL)
	{
		UnionUserErrLog("in UnionGetMaxNumOfLogFile:: UnionGetEnviVarByName for [%s]\n!","MaxNumOfLogFile");
		num = -1;
	}
	else
		num = atoi(p);
	
	UnionClearEnvi();

	return(num);	
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

// 20060825 增加
int UnionExistLogFileDef(char *name)
{
	char	fileName[512+1];
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfLogFileTBL(fileName);
	
	return(UnionExistEnviVarInFile(fileName,name));
}

// 20060825 增加
int UnionInsertLogFileDef(char *name,TUnionLogLevel level,long maxSize,TUnionLogFullRewriteStrategy rewriteAttr)
{
	int	ret;
	char	fileName[512+1];
		
	if ((name == NULL) || (strlen(name) == 0))
		return(errCodeParameter);
	
	// 检查日志是否已经在内存中存在	
	if ((ret = UnionExistLogFile(name)) < 0)
	{
		if (ret != errCodeLogMDL_LogFileNotExists)	// 错误，并且不是日志不存在错误
		{
			UnionUserErrLog("in UnionInsertLogFileDef:: UnionExistLogFile [%s]\n",name);
			return(ret);
		}
	}
	// 检查日志是否已经在日志定义文件中存在
	if ((ret = UnionExistLogFileDef(name)) > 0)
	{
		UnionUserErrLog("in UnionInsertLogFileDef:: UnionExistLogFileDef [%s]\n",name);
		return(errCodeLogMDL_LogFileAlreadyExists);
	}
	
	UnionDebugNullLog("in UnionInsertLogFileDef:: [%s] [%08ld] [%d] [%02d]\n",name,maxSize,level,rewriteAttr);
	// 先在日志文件中增加
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfLogFileTBL(fileName);
	if ((maxSize = maxSize % 1000) <= 0)
		maxSize = 1;
	//if (level < 0)
	//	level = 2;
	//if (rewriteAttr < 0)
	//	rewriteAttr = 0;
	if ((ret = UnionInsertEnviVar(fileName,name,"[%d][%03ld][%02d]",level,maxSize,rewriteAttr)) < 0)
	{
		UnionUserErrLog("in UnionInsertLogFileDef:: UnionInsertEnviVar [%s]\n",name);
		return(ret);
	}
	
	// 再在日志注册表中增加
	if ((ret = UnionAddLogFile(name)) < 0)
	{
		UnionUserErrLog("in UnionInsertLogFileDef:: UnionAddLogFile [%s]\n",name);
		return(ret);
	}
	
	return(ret);
}

// 20060825 增加
int UnionDeleteLogFileDef(char *name)
{
	char	fileName[512+1];
	int	ret;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfLogFileTBL(fileName);
	
	if ((ret = UnionDeleteLogFile(name)) < 0)
	{
		UnionUserErrLog("in UnionDeleteLogFileDef:: UnionDeleteLogFile [%s]\n",name);
		return(ret);
	}
	
	return(UnionDeleteEnviVar(fileName,name));
}

// 20060825 增加
int UnionUpdateLogFileDef(char *name,TUnionLogLevel level,long maxSize,TUnionLogFullRewriteStrategy rewriteAttr)
{
	int		ret;
	char		fileName[512+1];
	PUnionLogFile	plogFile;
	
	if ((name == NULL) || (strlen(name) == 0))
		return(errCodeParameter);
	
	// 查找日志	
	if ((plogFile = UnionFindLogFile(name)) == NULL)
	{	
		UnionUserErrLog("in UnionUpdateLogFileDef:: UnionFindLogFile [%s]\n",name);
		return(errCodeLogMDL_LogFileNotExists);
	}
	// 更新内存中的数据
	//if ((level >= 0) && (level != plogFile->level))
	if (level != plogFile->level)
		plogFile->level = level;
	
	if ((maxSize > 0) && (maxSize % 1000 != plogFile->maxSize / 1000000))
		plogFile->maxSize = maxSize % 1000 * 1000000;
	
	//if ((rewriteAttr >= 0) && (rewriteAttr != plogFile->rewriteAttr))
	if (rewriteAttr != plogFile->rewriteAttr)
		plogFile->rewriteAttr = rewriteAttr;
	
	// 更新日志文件定义中的数据
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfLogFileTBL(fileName);
	if ((ret = UnionUpdateEnviVar(fileName,name,"[%d][%03d][%02d]",plogFile->level,plogFile->maxSize/1000000,plogFile->rewriteAttr)) < 0)
	{
		UnionUserErrLog("in UnionUpdateLogFileDef:: UnionUpdateEnviVar [%s]\n",name);
		return(ret);
	}
	return(ret);
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
