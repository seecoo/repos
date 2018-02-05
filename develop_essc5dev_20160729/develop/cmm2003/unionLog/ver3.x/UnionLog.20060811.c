//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	3.0

/*
	2003/09/19,Wolfgang Wang,由2.0升级为3.0.
	升级时将原系统日期时间操作函数剔除形成一个名为systemTime1.0.c的程序
*/

// 2006/08/11 在20050909基础上升级为目前版本

// 2003/09/19 Wolfgang Wang
#define _UnionLogMDL_3_x_	
#define _UnionEnv_3_x_


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include "UnionStr.h"

#include "UnionEnv.h"

#ifdef _WIN32
#	include <process.h>
#endif //_WIN32

// 2003/09/19 Wolfgang Wang
#include "UnionLog.h"

extern PUnionLogFile UnionGetLogFile(); 

int	gunionDynamicLogLevel = -1;
char	gunionSuffixOfMyLogFile[15];
long	gunionCountOfMyLogFile = 1;
int	gunionLogFileMDLConnected = 0;

int	gunionUserSuffixOfMyLogFileUsed = 0;
char	gunionUserSuffixOfMyLogFile[64+1];

// 2006/08/11 增加
int gunionUserDefinedErrorCode = errCodeDefaultErrCode;

// 2010/5/15,王纯军增加
char	gunionUserSpecLogFileName[256+1] = "";
int	gunionIsUserSpecLogFileSet = 0;
int	gunionIsUserSpecLogFileWithFullDir = 0;	// 标识一个用户日志文件是否是全名


//add by hzh in 2012.12.13 为支持兼容20121101版编译问题
void UnionSetFlushLogFileLinesMode(int mode __attribute__((unused)))
{
	return;
}

//取fflush方式
int UnionGetFlushLogFileLinesMode()
{
	return 0;
}
//add end

// 2010/12/2,王纯军增加
// 将日志文件暂时改向到指定文件
void UnionRedirectLogFileToSpecFile(char *fileName)
{
	UnionSetUserSpecLogFileName(fileName);
	gunionIsUserSpecLogFileWithFullDir = 1;
}

// 2010/12/2,王纯军增加
// 恢复缺省日志配置
int UnionRestoreDefaultLogFile()
{
	gunionIsUserSpecLogFileWithFullDir = 0;
	gunionIsUserSpecLogFileSet = 0;
	return(0);
}

// 2010/5/15,王纯军增加
void UnionSetUserSpecLogFileName(char *fileName)
{
	int	len;
	
	memset(gunionUserSpecLogFileName,0,sizeof(gunionUserSpecLogFileName));
	if ((len = strlen(fileName)) == 0)
	{
		gunionIsUserSpecLogFileSet = 0;
		return;
	}
	if (len >= (int)sizeof(fileName))
		len = sizeof(gunionUserSpecLogFileName) - 1;
	memcpy(gunionUserSpecLogFileName,fileName,len);
	gunionIsUserSpecLogFileSet = 1;
	return;
}

void UnionDynamicSetLogLevel(TUnionLogLevel level)
{
	gunionDynamicLogLevel = level;
}

// 2006/08/11 增加
int UnionSetUserDefinedErrorCode(int errCode)
{
	return(gunionUserDefinedErrorCode = errCode);
}

// 2010/10/30 增加
void *UnionSetUserDefinedErrorCodeReturnNullPointer(int errCode)
{
	gunionUserDefinedErrorCode = errCode;
	return(NULL);
}

// 2009/08/11 增加
int UnionGetUserDefinedErrorCode()
{
	return(gunionUserDefinedErrorCode);
}

int UnionSetSuffixOfMyLogFile(char *logFileSuffix)
{
	if (logFileSuffix == NULL)
		return(0);
	gunionUserSuffixOfMyLogFileUsed = 1;
	if (strlen(logFileSuffix) >= sizeof(gunionUserSuffixOfMyLogFile))
	{
		memset(gunionUserSuffixOfMyLogFile,0,sizeof(gunionUserSuffixOfMyLogFile));
		memcpy(gunionUserSuffixOfMyLogFile,logFileSuffix,sizeof(gunionUserSuffixOfMyLogFile)-1);
	}
	else
		strcpy(gunionUserSuffixOfMyLogFile,logFileSuffix);
	return(0);
}

int UnionCloseSuffixOfMyLogFile()
{
	gunionUserSuffixOfMyLogFileUsed = 0;
	return(0);
}

int UnionGetSuffixForLogFile(TUnionLogFullRewriteStrategy rewriteAttr,char *suffix)
{
	char	systemDate[15];
	
	memset(systemDate,0,sizeof(systemDate));
	UnionGetFullSystemDate(systemDate);
	switch (rewriteAttr)
	{
		case	conLogFullRewriteAtonce:
		case	conLogFullRewriteAfterBackup:
			break;
		case	conLogNewEveryDay:
			strcpy(suffix,systemDate);
			break;
		case	conLogNewEveryMonth:
			systemDate[6] = 0;
			strcpy(suffix,systemDate);
			break;
		case	conLogNewEverySeason:
			systemDate[6] = 0;
			switch (atoi(systemDate+4))
			{
				case 1:
				case 2:
				case 3:
					systemDate[4] = 0;
					sprintf(suffix,"%s01-03",systemDate);
					break;
				case 4:
				case 5:
				case 6:
					systemDate[4] = 0;
					sprintf(suffix,"%s04-05",systemDate);
					break;
				case 7:
				case 8:
				case 9:
					systemDate[4] = 0;
					sprintf(suffix,"%s07-09",systemDate);
					break;
				case 10:
				case 11:
				case 12:
					systemDate[4] = 0;
					sprintf(suffix,"%s10-12",systemDate);
					break;
			}
			break;
		case	conLogNewHalfYear:
			systemDate[6] = 0;
			switch (atoi(systemDate+4))
			{
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
					systemDate[4] = 0;
					sprintf(suffix,"%s01-06",systemDate);
					break;
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
					systemDate[4] = 0;
					sprintf(suffix,"%s07-12",systemDate);
					break;
			}
			break;
		case	conLogNewEveryYear:
			systemDate[4] = 0;
			strcpy(suffix,systemDate);
			break;
		default:
			break;
	}
	return(0);
}

int UnionGetNameOfMyLogFile(PUnionLogFile plogFile,char *fileName)
{
	char	tmpSuffix[40];

	// 2010/5/15,王纯军修改	
	if (gunionIsUserSpecLogFileSet)	// 2010/5/15,王纯军新增
	{
		if (!gunionIsUserSpecLogFileWithFullDir)
			sprintf(fileName,"%s/%s",getenv("UNIONLOG"),gunionUserSpecLogFileName); // 2010/5/15,王纯军新增
		else
		{
			strcpy(fileName,gunionUserSpecLogFileName);
			return(0);
		}
	}
	else	// 2010/5/15,王纯军新增
	{
		if (plogFile == NULL)
		{
			sprintf(fileName,"%s/unknown.log",getenv("UNIONLOG"));
			return(0);
			//return(errCodeParameter);
		}
	}
	// 2010/5/15,王纯军修改结束

	//printf("[%d] [%d] [%d]\n",plogFile->rewriteAttr,conLogFullRewriteAfterBackup,conLogNewEveryDay);
	switch (plogFile->rewriteAttr)
	{
		case	conLogFullRewriteAtonce:
			if (gunionUserSuffixOfMyLogFileUsed)
				sprintf(fileName,"%s/%s-%s.log",getenv("UNIONLOG"),plogFile->name,gunionUserSuffixOfMyLogFile);
			else
				sprintf(fileName,"%s/%s.log",getenv("UNIONLOG"),plogFile->name);
			return(0);
		case	conLogFullRewriteAfterBackup:
			if (gunionUserSuffixOfMyLogFileUsed)
				sprintf(fileName,"%s/%s-%s.%04ld.log",getenv("UNIONLOG"),plogFile->name,gunionUserSuffixOfMyLogFile,gunionCountOfMyLogFile);
			else
				sprintf(fileName,"%s/%s.%04ld.log",getenv("UNIONLOG"),plogFile->name,gunionCountOfMyLogFile);
			return(0);
		case	conLogNewEveryDay:
		case	conLogNewEveryMonth:
		case	conLogNewEverySeason:
		case	conLogNewHalfYear:
		case	conLogNewEveryYear:
			break;
		default:
			if (gunionUserSuffixOfMyLogFileUsed)
				sprintf(fileName,"%s/%s-%s.log",getenv("UNIONLOG"),plogFile->name,gunionUserSuffixOfMyLogFile);
			else
				sprintf(fileName,"%s/%s.log",getenv("UNIONLOG"),plogFile->name);
			return(0);
	}
	
	if (!gunionLogFileMDLConnected)
	{
		gunionLogFileMDLConnected = 1;
		memset(gunionSuffixOfMyLogFile,0,sizeof(gunionSuffixOfMyLogFile));
		UnionGetSuffixForLogFile(plogFile->rewriteAttr,gunionSuffixOfMyLogFile);
	}
	
	//printf("in UnionGetNameOfMyLogFile:: suffix = [%s]\n",gunionSuffixOfMyLogFile);
	memset(tmpSuffix,0,sizeof(tmpSuffix));
	UnionGetSuffixForLogFile(plogFile->rewriteAttr,tmpSuffix);
	if (strcmp(tmpSuffix,gunionSuffixOfMyLogFile) != 0)
	{
		strcpy(gunionSuffixOfMyLogFile,tmpSuffix);
		gunionCountOfMyLogFile = 1;
	}
	if (gunionUserSuffixOfMyLogFileUsed)
		sprintf(fileName,"%s/%s-%s.%s.%04ld.log",getenv("UNIONLOG"),plogFile->name,gunionUserSuffixOfMyLogFile,gunionSuffixOfMyLogFile,gunionCountOfMyLogFile);
	else
		sprintf(fileName,"%s/%s.%s.%04ld.log",getenv("UNIONLOG"),plogFile->name,gunionSuffixOfMyLogFile,gunionCountOfMyLogFile);
	return(0);
}
	
void UnionIncreaseCountOfMyLogFile()
{
	++gunionCountOfMyLogFile;
	return;
}

FILE *UnionOpenSpecLogFile(char *logFileName)
{
	PUnionLogFile 	plogFile;
	char 		fileName[512];
	FILE 		*logFilePtr;
	int		ret;


	if ((plogFile = UnionFindLogFile(logFileName)) == NULL)
		return(stderr);

reOpenLogFile:
	memset(fileName,0,sizeof(fileName));
	if ((ret = UnionGetNameOfMyLogFile(plogFile,fileName)) < 0)
		return(stderr);
		
	if ((logFilePtr = fopen(fileName,"a")) == NULL)
		return(stderr);
		
	if (ftell(logFilePtr) < plogFile->maxSize)
		return(logFilePtr);
	
	UnionCloseLogFile(logFilePtr);

	if (!UnionIsLogBackupBeforeRewrite(plogFile))
	{
		if ((logFilePtr = fopen(fileName,"w")) == NULL)
			return(stderr);
		else
			return(logFilePtr);
	}
	
	UnionIncreaseCountOfMyLogFile();
	
	goto reOpenLogFile;
}

FILE *UnionOpenLogFile(PUnionLogFile plogFile)
{
	char 	fileName[512];
	FILE 	*logFilePtr;
	int	ret;
	long	maxSize;
	
reOpenLogFile:
	memset(fileName,0,512);
	if ((ret = UnionGetNameOfMyLogFile(plogFile,fileName)) < 0)
		return(stderr);
		
	if ((logFilePtr = fopen(fileName,"a")) == NULL)
		return(stderr);
	
	if (plogFile == NULL)
	{
		maxSize = 1024*1024*10;
		//return(logFilePtr);
	}
	else
		maxSize = plogFile->maxSize;
		
	if (ftell(logFilePtr) < maxSize)
		return(logFilePtr);
	
	UnionCloseLogFile(logFilePtr);

	if (!UnionIsLogBackupBeforeRewrite(plogFile))
	{
		if ((logFilePtr = fopen(fileName,"w")) == NULL)
			return(stderr);
		else
			return(logFilePtr);
	}
	
	UnionIncreaseCountOfMyLogFile();
	
	goto reOpenLogFile;
}

int UnionCloseLogFile(FILE *logFilePtr)
{
	if ((logFilePtr == stderr) || (logFilePtr == stdout))
		return(0);
	return(fclose(logFilePtr));
}

// Added by Wolfgang Wang, 2003/09/19
int UnionIsLogSystemErrLevel(PUnionLogFile plogFile)
{
	if (gunionDynamicLogLevel >= 0)
	{
		if (gunionDynamicLogLevel >= conLogSystemErrLevel)
			return(1);
		else
			return(0);
	}
	else if (plogFile == NULL)
		return(1);
	else
	{
		if (plogFile->level >= conLogSystemErrLevel)
			return(1);
		else
			return(0);
	}
}

int UnionIsLogUserErrLevel(PUnionLogFile plogFile)
{
	if (gunionDynamicLogLevel >= 0)
	{
		if (gunionDynamicLogLevel >= conLogUserErrLevel)
			return(1);
		else
			return(0);
	}
	else if (plogFile == NULL)
		return(1);
	else
	{
		if (plogFile->level >= conLogUserErrLevel)
			return(1);
		else
			return(0);
	}
}

int UnionIsLogAuditLevel(PUnionLogFile plogFile)
{
	if (gunionDynamicLogLevel >= 0)
	{
		if (gunionDynamicLogLevel >= conLogAuditLevel)
			return(1);
		else
			return(0);
	}
	else if (plogFile == NULL)
		return(1);
	else
	{
		if (plogFile->level >= conLogAuditLevel)
			return(1);
		else
			return(0);
	}
}

int UnionIsLogDebugLevel(PUnionLogFile plogFile)
{
	if (gunionDynamicLogLevel >= 0)
	{
		if (gunionDynamicLogLevel >= conLogDebugLevel)
			return(1);
		else
			return(0);
	}
	else if (plogFile == NULL)
		return(1);
	else
	{
		if (plogFile->level >= conLogDebugLevel)
			return(1);
		else
			return(0);
	}
}

int UnionIsLogProgramerLevel(PUnionLogFile plogFile)
{
	if (gunionDynamicLogLevel >= 0)
	{
		if (gunionDynamicLogLevel >= conLogProgramerLevel)
			return(1);
		else
			return(0);
	}
	else if (plogFile == NULL)
		return(1);
	else
	{
		if (plogFile->level >= conLogProgramerLevel)
			return(1);
		else
			return(0);
	}
}

int UnionIsLogAlwaysLevel(PUnionLogFile plogFile)
{
	if (gunionDynamicLogLevel >= 0)
	{
		if (gunionDynamicLogLevel >= conLogAlwaysLevel)
			return(1);
		else
			return(0);
	}
	else if (plogFile == NULL)
		return(1);
	else
	{
		if (plogFile->level >= conLogAlwaysLevel)
			return(1);
		else
			return(0);
	}
}

int UnionIsLogBackupBeforeRewrite(PUnionLogFile plogFile)
{
	if (plogFile == NULL)
		return(0);
	switch (plogFile->rewriteAttr)
	{
		case	conLogFullRewriteAtonce:
			return(0);
		case	conLogFullRewriteAfterBackup:
		case	conLogNewEveryDay:
		case	conLogNewEveryMonth:
		case	conLogNewEverySeason:
		case	conLogNewHalfYear:
		case	conLogNewEveryYear:
			return(1);
		default:
			return(0);
	}
}

int UnionGetCurrentSizeOfLogFile(FILE* logFilePtr,PUnionLogFile plogFile)
{
	if ((plogFile == NULL) || (logFilePtr == NULL) || (logFilePtr == stderr) || (logFilePtr == stdout))
		return(errCodeParameter);
	else
	{
		plogFile->currentSize = ftell(logFilePtr);
		return(0);
	}
}

void WriteMemory(FILE *fp,unsigned char *pBuffer,int iBufLen)
{
	int	i,j,iLine,iBytes,iLineBytes,iSpaceNum;
	char	caHint[]="Displacement -1--2--3--4--5--6--7--8-Hex-0--1--2--3--4--5--6  --ASCII Value--\n";

	if ((iBufLen % 16) != 0)
		iLine = iBufLen / 16 + 1;
	else
		iLine = iBufLen / 16;

	for (i=0;i<iLine;i++)
	{
		if (i % 20 == 0)
			fprintf(fp,caHint);

		// Write total bytes
		iBytes = i*16;
		fprintf(fp,"%05d(%05X) ",iBytes,iBytes);

		// Write 16 bytes as HEX mode
		if (i != (iLine-1))
			iLineBytes = 16;
		else
			iLineBytes = iBufLen-iBytes;

		for (j=0;j<iLineBytes;j++)
		{
			fprintf(fp,"%02X ",pBuffer[iBytes+j]);
		}

		// Write space
		if (i != (iLine-1))
			iSpaceNum = 1;
		else
			iSpaceNum = 1+(16-iLineBytes)*3;
		for (j=0;j<iSpaceNum;j++)
			fprintf(fp," ");

		// Write 16 bytes as ASCII mode
		for (j=0;j<iLineBytes;j++)
		{
			fprintf(fp,"%c",pBuffer[iBytes+j]);
		}
		fprintf(fp,"\n");
	}
}


void UnionPrintf(char *fmt,...)
{
	va_list args;

	printf("(%d)",getpid());
	va_start(args,fmt);
	vprintf(fmt,args);
	va_end(args);
	
}

int UnionPrintLogFileToFile(PUnionLogFile plogFile,FILE *fp)
{
	if ((fp == NULL) || (plogFile == NULL))
		return(errCodeParameter);
	
	if (strlen(plogFile->name) == 0)
		return(errCodeLogMDL_LogFileNotExists);

	/*
	fprintf(fp,"\nLogFile\n");
	fprintf(fp,"[name]		[%s]\n",plogFile->name);
	fprintf(fp,"[level]		[%d]\n",plogFile->level);
	fprintf(fp,"[maxSize]	[%ld]\n",plogFile->maxSize);
	fprintf(fp,"[rewriteAttr]	[%02d]\n",plogFile->rewriteAttr);
	fprintf(fp,"[currentSize]	[%ld]\n",plogFile->currentSize);
	fprintf(fp,"[users]		[%d]\n",plogFile->users);
	fflush(fp);
	*/
	fprintf(fp,"%40s %02d %06ld %02d %12ld %05d\n",
		plogFile->name,plogFile->level,plogFile->maxSize/1000000,
		plogFile->rewriteAttr,plogFile->currentSize,plogFile->users);
	return(0);
}

int UnionPrintLogFile(PUnionLogFile plogFile)
{
	return(UnionPrintLogFileToFile(plogFile,stdout));
}

int UnionCalSizeOfLogFile(PUnionLogFile plogFile)
{
	FILE		*fp;
	char		fileName[512];
	
	if (plogFile == NULL)
		return(errCodeParameter);
		
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/%s.log",getenv("UNIONLOG"),plogFile->name);
	if ((fp = fopen(fileName,"a")) == NULL)
	{
		//UnionPrintf("in UnionCalSizeOfLogFile:: fopen [%s]\n",fileName);
		plogFile->currentSize = 0;
	}
	else
	{
		plogFile->currentSize = ftell(fp);
		fclose(fp);
	}
	
	return(0);
}		

// add by hzh in 20091026 增加的函数
void UnionSystemErrLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
		
	if (!UnionIsLogSystemErrLevel(plogFile = UnionGetLogFile()))
		return;
		
	logFilePtr = UnionOpenLogFile(plogFile);
	if (logFilePtr == NULL)
		return;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	UnionGetCurrentDateTimeInMacroFormat(tmpBuf);
	fprintf(logFilePtr,"%10s::[OSError][%05d][%06d][%s][%d] fileName[%s]funName[%s]line[%d]::\n",
		tmpBuf,getpid(),errno,strerror(errno),gunionUserDefinedErrorCode,fileName,funName,line);

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	fflush(logFilePtr);
	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);
	
	UnionCloseLogFile(logFilePtr);
}

// UserErrLevel
void UnionUserErrLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
	
	if (!UnionIsLogUserErrLevel(plogFile = UnionGetLogFile()))
		return;
		
	logFilePtr = UnionOpenLogFile(plogFile);
	if (logFilePtr == NULL)
		return;

	memset(tmpBuf,0,40);
	UnionGetCurrentDateTimeInMacroFormat(tmpBuf);

	fprintf(logFilePtr,"%10s::[UserErr][%05d][%d] fileName[%s]funName[%s]line[%d]::\n",tmpBuf,getpid(),gunionUserDefinedErrorCode,fileName,funName,line);

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	fflush(logFilePtr);

	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);

}

void UnionMemErrLogW(char *fileName,char *funName,int line,char *pTitle,unsigned char *pBuffer,int iBufLen)
{
	char		tmpBuf[40];
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
	
	if (!UnionIsLogUserErrLevel(plogFile = UnionGetLogFile()))
		return;
		
	if (pBuffer == NULL || iBufLen <= 0)
		return;

	logFilePtr = UnionOpenLogFile(plogFile);
	if (logFilePtr == NULL)
		return;
	
	memset(tmpBuf,0,40);
	UnionGetCurrentDateTimeInMacroFormat(tmpBuf);

	fprintf(logFilePtr,"%10s::[UserErr][%05d][%d] fileName[%s]funName[%s]line[%d]::\n",tmpBuf,getpid(),gunionUserDefinedErrorCode,fileName,funName,line);
	fprintf(logFilePtr,"%s\n",pTitle);

	WriteMemory(logFilePtr,pBuffer,iBufLen);

	fflush(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

// AuditLevel
void UnionSuccessLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
	
	if (!UnionIsLogAuditLevel(plogFile = UnionGetLogFile()))
		return;

	logFilePtr = UnionOpenLogFile(plogFile);
	if (logFilePtr == NULL)
		return;

	memset(tmpBuf,0,40);
	UnionGetCurrentDateTimeInMacroFormat(tmpBuf);

	fprintf(logFilePtr,"%10s::[Audit][%05d] fileName[%s]funName[%s]line[%d]::\n",tmpBuf,getpid(),fileName,funName,line);

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	fflush(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionAuditLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
	
	if (!UnionIsLogAuditLevel(plogFile = UnionGetLogFile()))
		return;

	logFilePtr = UnionOpenLogFile(plogFile);
	if (logFilePtr == NULL)
		return;

	memset(tmpBuf,0,40);
	UnionGetCurrentDateTimeInMacroFormat(tmpBuf);

	fprintf(logFilePtr,"%10s::[Audit][%05d] fileName[%s]funName[%s]line[%d]::\n",tmpBuf,getpid(),fileName,funName,line);

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	fflush(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionAuditNullLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
	
	if (!UnionIsLogAuditLevel(plogFile = UnionGetLogFile()))
		return;

	logFilePtr = UnionOpenLogFile(plogFile);
	if (logFilePtr == NULL)
		return;

	fprintf(logFilePtr,"%08d:: fileName[%s]funName[%s]line[%d]::",getpid(),fileName,funName,line);
	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	fflush(logFilePtr);

	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionAuditNullLogWithTimeW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
	
	if (!UnionIsLogAuditLevel(plogFile = UnionGetLogFile()))
		return;

	logFilePtr = UnionOpenLogFile(plogFile);
	if (logFilePtr == NULL)
		return;

	memset(tmpBuf,0,40);
	UnionGetCurrentDateTimeInMacroFormat(tmpBuf);

	//fprintf(logFilePtr,"%10s::",tmpBuf);
	fprintf(logFilePtr,"%08d::%10s:: fileName[%s]funName[%s]line[%d]::",getpid(),tmpBuf,fileName,funName,line);

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	fflush(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

// DebugLevel
void UnionLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
	
	if (!UnionIsLogDebugLevel(plogFile = UnionGetLogFile()))
		return;
	
	logFilePtr = UnionOpenLogFile(plogFile);
	
	if (logFilePtr == NULL)
		return;

	memset(tmpBuf,0,40);
	UnionGetCurrentDateTimeInMacroFormat(tmpBuf);

	fprintf(logFilePtr,"%10s::[Debug][%05d] fileName[%s]funName[%s]line[%d]::\n",tmpBuf,getpid(),fileName,funName,line);

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	fflush(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionMemLogW(char *fileName,char *funName,int line,char *pTitle,unsigned char *pBuffer,int iBufLen)
{
	char		tmpBuf[40];
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
	
	if (!UnionIsLogDebugLevel(plogFile = UnionGetLogFile()))
		return;
		
	if (pBuffer == NULL || iBufLen <= 0)
		return;

	logFilePtr = UnionOpenLogFile(plogFile);
	if (logFilePtr == NULL)
		return;
	
	memset(tmpBuf,0,40);
	UnionGetCurrentDateTimeInMacroFormat(tmpBuf);

	fprintf(logFilePtr,"%10s::[Debug][%05d] fileName[%s]funName[%s]line[%d]::\n",tmpBuf,getpid(),fileName,funName,line);
	fprintf(logFilePtr,"%s\n",pTitle);

	WriteMemory(logFilePtr,pBuffer,iBufLen);

	fflush(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionProgramerLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
	
	if (!UnionIsLogProgramerLevel(plogFile = UnionGetLogFile()))
		return;

	logFilePtr = UnionOpenLogFile(plogFile);
	if (logFilePtr == NULL)
		return;

	memset(tmpBuf,0,40);
	UnionGetCurrentDateTimeInMacroFormat(tmpBuf);

	fprintf(logFilePtr,"%10s::[Debug][%05d] fileName[%s]funName[%s]line[%d]::\n",tmpBuf,getpid(),fileName,funName,line);

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	fflush(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionDebugLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
	
	if (!UnionIsLogProgramerLevel(plogFile = UnionGetLogFile()))
		return;

	logFilePtr = UnionOpenLogFile(plogFile);
	if (logFilePtr == NULL)
		return;

	memset(tmpBuf,0,40);
	UnionGetCurrentDateTimeInMacroFormat(tmpBuf);

	fprintf(logFilePtr,"%10s::[Debug][%05d] fileName[%s]funName[%s]line[%d]::\n",tmpBuf,getpid(),fileName,funName,line);

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	fflush(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}


void UnionDebugNullLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
	
	if (!UnionIsLogProgramerLevel(plogFile = UnionGetLogFile()))
		return;

	logFilePtr = UnionOpenLogFile(plogFile);
	if (logFilePtr == NULL)
		return;

	fprintf(logFilePtr,"%08d:: fileName[%s]funName[%s]line[%d]::",getpid(),fileName,funName,line);
	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	fflush(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionNullLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
	
	if (!UnionIsLogDebugLevel(plogFile = UnionGetLogFile()))
		return;

	logFilePtr = UnionOpenLogFile(plogFile);
	if (logFilePtr == NULL)
		return;

	fprintf(logFilePtr,"%08d:: fileName[%s]funName[%s]line[%d]::",getpid(),fileName,funName,line);
	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	fflush(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionMemNullLogWithTimeW(char *fileName,char *funName,int line,unsigned char *pBuffer,int iBufLen)
{
	char		tmpBuf[4096+1];
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
	
	if (!UnionIsLogDebugLevel(plogFile = UnionGetLogFile()))
		return;
		
	if (pBuffer == NULL || iBufLen <= 0)
		return;

	if (iBufLen * 2 >= (int)sizeof(tmpBuf))
		iBufLen = (sizeof(tmpBuf) - 1) / 2;
	bcdhex_to_aschex((char *)pBuffer,iBufLen,tmpBuf);
	tmpBuf[iBufLen*2] = 0;
	
	logFilePtr = UnionOpenLogFile(plogFile);
	if (logFilePtr == NULL)
		return;
	
	memset(tmpBuf,0,40);
	UnionGetCurrentDateTimeInMacroFormat(tmpBuf);
	fprintf(logFilePtr,"%10s::%08d:: fileName[%s]funName[%s]line[%d]::",tmpBuf,getpid(),fileName,funName,line);
	fprintf(logFilePtr,"[%04d][%s]\n",iBufLen*2,tmpBuf);
	fflush(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionNullLogWithTimeW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
	
	if (!UnionIsLogDebugLevel(plogFile = UnionGetLogFile()))
		return;
	
	logFilePtr = UnionOpenLogFile(plogFile);
	if (logFilePtr == NULL)
		return;

	memset(tmpBuf,0,40);
	UnionGetCurrentDateTimeInMacroFormat(tmpBuf);

	fprintf(logFilePtr,"%10s::%08d:: fileName[%s]funName[%s]line[%d]::",tmpBuf,getpid(),fileName,funName,line);
	//fprintf(logFilePtr,"%10s::",tmpBuf);

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	fflush(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionNullLogWithTimeAnywayW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;

	plogFile = UnionGetLogFile();
	logFilePtr = UnionOpenLogFile(plogFile);
	if (logFilePtr == NULL)
		return;

	memset(tmpBuf,0,40);
	UnionGetCurrentDateTimeInMacroFormat(tmpBuf);

	fprintf(logFilePtr,"%10s::%08d:: fileName[%s]funName[%s]line[%d]::",tmpBuf,getpid(),fileName,funName,line);
	//fprintf(logFilePtr,"%10s::",tmpBuf);

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	fflush(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionProgramerMemLogW(char *fileName,char *funName,int line,char *pTitle,unsigned char *pBuffer,int iBufLen)
{
	char		tmpBuf[40];
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
	
	if (!UnionIsLogProgramerLevel(plogFile = UnionGetLogFile()))
		return;
		
	if (pBuffer == NULL || iBufLen <= 0)
		return;

	logFilePtr = UnionOpenLogFile(plogFile);
	if (logFilePtr == NULL)
		return;
	
	memset(tmpBuf,0,40);
	UnionGetCurrentDateTimeInMacroFormat(tmpBuf);

	fprintf(logFilePtr,"%10s::[Debug][%05d] fileName[%s]funName[%s]line[%d]::\n",tmpBuf,getpid(),fileName,funName,line);
	fprintf(logFilePtr,"%s\n",pTitle);

	WriteMemory(logFilePtr,pBuffer,iBufLen);

	fflush(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

// add end
