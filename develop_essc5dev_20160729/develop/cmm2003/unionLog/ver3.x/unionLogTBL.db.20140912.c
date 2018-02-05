//	Author:		wangk
//	Date:		2009-9-22
//	Version:	1.0

#define _UnionLogMDL_3_x_	
#define _UnionEnv_3_x_	
#define _realBaseDB_2_x_
#define _LOG_FILE_NUM_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "unionModule.h"
#include "unionCommand.h"
#include "unionREC.h"
#include "UnionStr.h"

#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"

extern PUnionSharedMemoryModule	pgunionLogFileMDL;
extern PUnionLogFileTBL		pgunionLogFileTBL;
extern PUnionLogFile		pgunionLogFile;

int UnionGetNameOfLogFileTBL(char *fileName)
{
	sprintf(fileName,"unionLog");
	return(0);
}

int UnionReloadLogFileTBL()
{
	int	ret = 0;
	int	i = 0;
	int	j = 0;
	char	tmpBuf[128+1];
	char	sql[128+1];
	int	realNum = 0;
	int	iCnt = 0;
	char	logName[40+1];
	char	logLevel[16+1];
	char	maxSize[16+1];
	char	rewriteAttr[16+1];
	char	logFileNum[16+1];
	PUnionLogFile	plogFile;
	
	if ((ret = UnionConnectLogFileTBL()) < 0)
	{
		UnionUserErrLog("in UnionReloadLogFileTBL:: UnionConnectLogFileTBL!\n");
		return(ret);
	}

	if( (pgunionLogFileTBL->maxLogFileNum = UnionGetMaxNumOfLogFile()) < 0 )
	{
		UnionUserErrLog("in UnionReloadLogFileTBL:: UnionGetMaxNumOfLogFile\n");
		return (errCodeLogMDL_ConfFile);
	}

	if( pgunionLogFileTBL->maxLogFileNum > 1024 )
	{
		UnionUserErrLog("in UnionReloadLogFileTBL:: MaxNumOfLogFile is too large\n");
		return (errCodeDefaultErrCode);
	}

	memset(sql,0,sizeof(sql));
	strcpy(sql,"select logName,logLevel,maxSize,rewriteAttr,logFileNum from unionLog");
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionReloadLogFileTBL:: UnionSelectRealDBRecord\n");
		return (ret);
	}
	
	// 读取总数量
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReloadLogFileTBL:: UnionReadXMLPackageValue[totalNum]\n");
		return (ret);
	}
	iCnt = atoi(tmpBuf);

	for (i = 0; i < pgunionLogFileTBL->maxLogFileNum; i++)
	{

		if (strlen((pgunionLogFile+i)->name) == 0)
			continue;

		// 查找是否有定义文件名为(pgunionLogFile+i)->name的日志
		for( j = 0; j < iCnt; j++ )
		{
			if ((ret = UnionLocateXMLPackage("detail", j+1)) < 0)
			{
				UnionUserErrLog("in UnionReloadLogFileTBL:: UnionLocateXMLPackage\n");
				continue;
			}
			
			memset(logName,0,sizeof(logName));
			UnionReadXMLPackageValue("logName", logName, sizeof(logName));
			
			if( strcmp((pgunionLogFile+i)->name, logName) == 0 )
				break;
		}
		if( j >= iCnt )
		{
			if ((pgunionLogFile+i)->users > 0)
				continue;
			//UnionNullLog("[%s] not used and defined,delete it\n",(pgunionLogFile+i)->name);
			memset(pgunionLogFile+i,0,sizeof(*plogFile));
		}
	}
	
	realNum = 0;
	for (i = 0; (i < iCnt) && (realNum < pgunionLogFileTBL->maxLogFileNum); i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionReloadLogFileTBL:: UnionLocate\n");
			continue;
		}
			
		memset(logName,0,sizeof(logName));
		UnionReadXMLPackageValue("logName", logName, sizeof(logName));
		memset(logLevel,0,sizeof(logLevel));
		UnionReadXMLPackageValue("logLevel", logLevel, sizeof(logLevel));
		memset(maxSize,0,sizeof(maxSize));
		UnionReadXMLPackageValue("maxSize", maxSize, sizeof(maxSize));
		memset(rewriteAttr,0,sizeof(rewriteAttr));
		UnionReadXMLPackageValue("rewriteAttr", rewriteAttr, sizeof(rewriteAttr));

		memset(logFileNum,0,sizeof(logFileNum));
		UnionReadXMLPackageValue("logFileNum", logFileNum, sizeof(logFileNum));
		
		// 读取名称
		if (strlen(logName) > sizeof(plogFile->name) - 1)
			continue;
		if ((plogFile = UnionConnectLogFile(logName)) == NULL)
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
			strcpy(plogFile->name, logName);
		}
		else
			--plogFile->users;
		
	
		// 读取Level
		plogFile->level = atoi(logLevel);
		//if ((plogFile->level < conLogNoneLogLevel) || (plogFile->level > conLogAlwaysLevel))
		if (plogFile->level > conLogAlwaysLevel)
			plogFile->level = conLogAuditLevel;
			
		// 读取日志的最大尺寸
		plogFile->maxSize = atoi(maxSize) * 1000000;	// 日志文件中的尺寸以M计
		if (plogFile->maxSize < 0)
			plogFile->maxSize = 1000000;
		
		// 读取重写标志
		switch (plogFile->rewriteAttr = atoi(rewriteAttr))
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
		
		plogFile->logFileNum = atoi(logFileNum);
		
		UnionCalSizeOfLogFile(plogFile);
		
		++realNum;
	}

	return(0);
}

int UnionGetMaxNumOfLogFile()
{
	int	num = 0;

	if( (num=UnionReadIntTypeRECVar("maxNumOfLogFile")) < 0 )
	{
		UnionUserErrLog("in UnionGetMaxNumOfLogFile:: UnionReadIntTypeRECVar for [%s]\n!","MaxNumOfLogFile");
		num = -1;
	}

	return(num);	
}

int UnionReadLogFileDef(char *logFileName,PUnionLogFile plogFile)
{
	int	ret;
	char	sql[128+1];
	//char	tmpBuf[128+1];
	char	logLevel[16+1];
	char	maxSize[16+1];
	char	rewriteAttr[16+1];
	
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

	memset(sql,0,sizeof(sql));
	sprintf(sql,"select taskName,logLevel,maxSize,rewriteAttr logFileNum from unionLog where taskName = '%s'",logFileName);
	
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionReadLogFileDef:: UnionSelectRealDBRecord\n");
		return (ret);
	}
	
	// 读取总数量
	/*
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadLogFileDef:: UnionReadXMLPackageValue[totalNum]\n");
		return (ret);
	}
	iCnt = atoi(tmpBuf);
	*/
	
	if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
	{
		UnionUserErrLog("in UnionReadLogFileDef:: UnionLocate\n");
		return (ret);
	}
			
	memset(logLevel,0,sizeof(logLevel));
	UnionReadXMLPackageValue("logLevel", logLevel, sizeof(logLevel));
	memset(maxSize,0,sizeof(maxSize));
	UnionReadXMLPackageValue("maxSize", maxSize, sizeof(maxSize));
	memset(rewriteAttr,0,sizeof(rewriteAttr));
	UnionReadXMLPackageValue("rewriteAttr", rewriteAttr, sizeof(rewriteAttr));

	// 找到了该日志的定义
	// 读取名称
	memset(plogFile,0,sizeof(*plogFile));
	strcpy(plogFile->name,logFileName);

	// 读取Level
	plogFile->level = atoi(logLevel);
	//if ((plogFile->level < conLogNoneLogLevel) || (plogFile->level > conLogAlwaysLevel))
	if (plogFile->level > conLogAlwaysLevel)
		plogFile->level = conLogAuditLevel;
		
	// 读取日志的最大尺寸
	plogFile->maxSize = atoi(maxSize) * 1000000;	// 日志文件中的尺寸以M计
	if (plogFile->maxSize < 0)
		plogFile->maxSize = 1000000;
	
	// 读取重写标志
	switch (plogFile->rewriteAttr = atoi(rewriteAttr))
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

	plogFile->users = 0;
	
	UnionLog("in UnionReadLogFileDef:: [%s] [%d] [%ld] [%d]\n",plogFile->name,plogFile->level,plogFile->maxSize,plogFile->rewriteAttr);
	return(0);
}
