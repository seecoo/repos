//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	3.0

/*
	2003/09/19,Wolfgang Wang,由2.0升级为3.0.
	升级时将原系统日期时间操作函数剔除形成一个名为systemTime1.0.c的程序
*/

// 2006/08/11 在20050909基础上升级为目前版本

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif
#define _UnionEnv_3_x_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionEnv.h"

#ifdef _WIN32
#	include <process.h>
#endif //_WIN32

// 2003/09/19 Wolfgang Wang
#include "UnionLog.h"

extern PUnionLogFile UnionGetLogFile(); 

int	gunionDynamicLogLevel = -1;
char	gunionSuffixOfMyLogFile[15] = "";
long	gunionCountOfMyLogFile = 1;
int	gunionLogFileMDLConnected = 0;

int	gunionUserSuffixOfMyLogFileUsed = 0;
char	gunionUserSuffixOfMyLogFile[64+1];

// 2006/08/11 增加
int gunionUserDefinedErrorCode = errCodeDefaultErrCode;

void UnionDynamicSetLogLevel(TUnionLogLevel level)
{
	gunionDynamicLogLevel = level;
}

// 2006/08/11 增加
int UnionSetUserDefinedErrorCode(int errCode)
{
	return(gunionUserDefinedErrorCode = errCode);
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
	
	if (plogFile == NULL)
		return(errCodeParameter);

	//printf("[%d] [%d] [%d]\n",plogFile->rewriteAttr,conLogFullRewriteAfterBackup,conLogNewEveryDay);
	switch (plogFile->rewriteAttr)
	{
		case	conLogFullRewriteAtonce:
			if (gunionUserSuffixOfMyLogFileUsed)
				sprintf(fileName,"%s-%s",plogFile->name,gunionUserSuffixOfMyLogFile);
			else
				sprintf(fileName,"%s",plogFile->name);
			return(0);
		case	conLogFullRewriteAfterBackup:
			if (gunionUserSuffixOfMyLogFileUsed)
				sprintf(fileName,"%s-%s.%04ld",plogFile->name,gunionUserSuffixOfMyLogFile,gunionCountOfMyLogFile);
			else
				sprintf(fileName,"%s.%04ld",plogFile->name,gunionCountOfMyLogFile);
			return(0);
		case	conLogNewEveryDay:
		case	conLogNewEveryMonth:
		case	conLogNewEverySeason:
		case	conLogNewHalfYear:
		case	conLogNewEveryYear:
			break;
		default:
			if (gunionUserSuffixOfMyLogFileUsed)
				sprintf(fileName,"%s-%s",plogFile->name,gunionUserSuffixOfMyLogFile);
			else
				sprintf(fileName,"%s",plogFile->name);
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
		sprintf(fileName,"%s-%s.%s.%04ld",plogFile->name,gunionUserSuffixOfMyLogFile,gunionSuffixOfMyLogFile,gunionCountOfMyLogFile);
	else
		sprintf(fileName,"%s.%s.%04ld",plogFile->name,gunionSuffixOfMyLogFile,gunionCountOfMyLogFile);
	return(0);
}
	
void UnionIncreaseCountOfMyLogFile()
{
	++gunionCountOfMyLogFile;
	return;
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
	
// SystemErrLevel
void UnionSystemErrLog(char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	char		logInfo[8096+1];
	int		len = 0;
		
	if (!UnionIsLogSystemErrLevel(UnionGetLogFile()))
		return;
		
	memset(tmpBuf,0,sizeof(tmpBuf));
	UnionGetFullSystemDateTime(tmpBuf);
	sprintf(logInfo,"%10s::[OSError][%05d][%06d][%s][%d]\n",
		tmpBuf,getpid(),errno,strerror(errno),gunionUserDefinedErrorCode);
	len = strlen(logInfo);

	va_start(args,fmt);
	vsprintf(logInfo+len,fmt,args);
	va_end(args);

	len += strlen(logInfo+len);

	UnionBufferLogSvrInfo(logInfo,len);	
/*
#ifdef _useSpier_
	sprintf(errMsg,"%10s::[OSError][%05d][%s][%06d][%d]\n",
		tmpBuf,getpid(),errno,strerror(errno),gunionUserDefinedErrorCode);
	len = strlen(errMsg);
	va_start(args,fmt);
	vsprintf(errMsg+len,fmt,args);
	va_end(args);
	UnionSendInfoToErrorSpier(strlen(errMsg),errMsg);
#endif
*/
}

// UserErrLevel
void UnionUserErrLog(char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	char		logInfo[8096+1];
	int		len = 0;
	
	if (!UnionIsLogUserErrLevel(UnionGetLogFile()))
		return;
		
	memset(tmpBuf,0,40);
	UnionGetFullSystemDateTime(tmpBuf);

	sprintf(logInfo,"%10s::[UserErr][%05d][%d]\n",tmpBuf,getpid(),gunionUserDefinedErrorCode);
	len = strlen(logInfo);
	
	va_start(args,fmt);
	vsprintf(logInfo+len,fmt,args);
	va_end(args);

	len += strlen(logInfo+len);

	UnionBufferLogSvrInfo(logInfo,len);
/*
#ifdef _useSpier_
	sprintf(errMsg,"%10s::[UserErr][%05d][%d]\n",tmpBuf,getpid(),gunionUserDefinedErrorCode);
	len = strlen(errMsg);
	va_start(args,fmt);
	vsprintf(errMsg+len,fmt,args);
	va_end(args);
	UnionSendInfoToErrorSpier(strlen(errMsg),errMsg);
#endif
*/
}

void UnionMemErrLog(char *pTitle,unsigned char *pBuffer,int iBufLen)
{
	char		tmpBuf[40];
	char		logInfo[8096+1];
	int		len = 0;
	
	if (!UnionIsLogUserErrLevel(UnionGetLogFile()))
		return;
		
	if (pBuffer == NULL || iBufLen <= 0)
		return;

	memset(tmpBuf,0,40);
	UnionGetFullSystemDateTime(tmpBuf);

	sprintf(logInfo,"%10s::[UserErr][%05d][%d]\n%s\n",tmpBuf,getpid(),gunionUserDefinedErrorCode,pTitle);
	len = strlen(logInfo);
	bcdhex_to_aschex(pBuffer,iBufLen,logInfo+len);
	len += (iBufLen * 2);
	
	UnionBufferLogSvrInfo(logInfo,len);
}

// AuditLevel
void UnionSuccessLog(char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	char		logInfo[8096+1];
	int		len = 0;
		
	if (!UnionIsLogAuditLevel(UnionGetLogFile()))
		return;

	memset(tmpBuf,0,40);
	UnionGetFullSystemDateTime(tmpBuf);
	sprintf(logInfo,"%10s::[Audit][%05d]\n",tmpBuf,getpid());
	len = strlen(logInfo);
	
	va_start(args,fmt);
	vsprintf(logInfo+len,fmt,args);
	va_end(args);

	len += strlen(logInfo+len);

	UnionBufferLogSvrInfo(logInfo,len);
}

void UnionAuditLog(char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	char		logInfo[8096+1];
	int		len = 0;
		
	if (!UnionIsLogAuditLevel(UnionGetLogFile()))
		return;

	memset(tmpBuf,0,40);
	UnionGetFullSystemDateTime(tmpBuf);
	sprintf(logInfo,"%10s::[Audit][%05d]\n",tmpBuf,getpid());
	len = strlen(logInfo);
	
	va_start(args,fmt);
	vsprintf(logInfo+len,fmt,args);
	va_end(args);

	len += strlen(logInfo+len);

	UnionBufferLogSvrInfo(logInfo,len);
}

void UnionAuditNullLog(char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	char		logInfo[8096+1];
	int		len = 0;
		
	if (!UnionIsLogAuditLevel(UnionGetLogFile()))
		return;

	sprintf(logInfo,"%08d::",getpid());
	len = strlen(logInfo);
	
	va_start(args,fmt);
	vsprintf(logInfo+len,fmt,args);
	va_end(args);

	len += strlen(logInfo+len);

	
	UnionBufferLogSvrInfo(logInfo,len);
}

void UnionAuditNullLogWithTime(char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	char		logInfo[8096+1];
	int		len = 0;
	
	if (!UnionIsLogAuditLevel(UnionGetLogFile()))
		return;

	memset(tmpBuf,0,40);
	UnionGetFullSystemDateTime(tmpBuf);
	sprintf(logInfo,"%08d::%10s::",getpid(),tmpBuf);
	len = strlen(logInfo);
	
	va_start(args,fmt);
	vsprintf(logInfo+len,fmt,args);
	va_end(args);

	len += strlen(logInfo+len);

	UnionBufferLogSvrInfo(logInfo,len);
}

// DebugLevel
void UnionLog(char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	char		logInfo[8096+1];
	int		len = 0;
	
	if (!UnionIsLogDebugLevel(UnionGetLogFile()))
		return;

	memset(tmpBuf,0,40);
	UnionGetFullSystemDateTime(tmpBuf);
	sprintf(logInfo,"%10s::[Debug][%05d]\n",tmpBuf,getpid());
	len = strlen(logInfo);
	
	va_start(args,fmt);
	vsprintf(logInfo+len,fmt,args);
	va_end(args);

	len += strlen(logInfo+len);

	UnionBufferLogSvrInfo(logInfo,len);
}

void UnionMemLog(char *pTitle,unsigned char *pBuffer,int iBufLen)
{
	char		tmpBuf[40];
	char		logInfo[8096+1];
	int		len = 0;
		
	if (!UnionIsLogDebugLevel(UnionGetLogFile()))
		return;
		
	if (pBuffer == NULL || iBufLen <= 0)
		return;

	memset(tmpBuf,0,40);
	UnionGetFullSystemDateTime(tmpBuf);
	sprintf(logInfo,"%10s::[Debug][%05d]\n%s\n",tmpBuf,getpid(),pTitle);
	len = strlen(logInfo);
	bcdhex_to_aschex(pBuffer,iBufLen,logInfo+len);
	len += (iBufLen * 2);

	UnionBufferLogSvrInfo(logInfo,len);
}

void UnionMemAuditNullLog(unsigned char *pBuffer,int iBufLen)
{
	char		logInfo[8096+1];
	int		len = 0;
	
	if (!UnionIsLogAuditLevel(UnionGetLogFile()))
		return;
		
	if (pBuffer == NULL || iBufLen <= 0)
		return;

	bcdhex_to_aschex(pBuffer,iBufLen,logInfo);
	len = iBufLen * 2;
		
	UnionBufferLogSvrInfo(logInfo,len);
}

void UnionMemNullLog(unsigned char *pBuffer,int iBufLen)
{
	char		logInfo[8096+1];
	int		len = 0;
		
	if (!UnionIsLogDebugLevel(UnionGetLogFile()))
		return;
		
	if (pBuffer == NULL || iBufLen <= 0)
		return;

	bcdhex_to_aschex(pBuffer,iBufLen,logInfo);
	len = iBufLen * 2;
	
	UnionBufferLogSvrInfo(logInfo,len);
}

void UnionNoTimeLog(char *fmt,...)
{
	va_list 	args;
	char		logInfo[8096+1];
	int		len = 0;
			
	if (!UnionIsLogDebugLevel(UnionGetLogFile()))
		return;

	va_start(args,fmt);
	vsprintf(logInfo+len,fmt,args);
	va_end(args);
	len += strlen(logInfo+len);

	UnionBufferLogSvrInfo(logInfo,len);
}

void UnionProgramerLog(char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	char		logInfo[8096+1];
	int		len = 0;
	
	if (!UnionIsLogProgramerLevel(UnionGetLogFile()))
		return;

	memset(tmpBuf,0,40);
	UnionGetFullSystemDateTime(tmpBuf);
	sprintf(logInfo,"%10s::[Debug][%05d]\n",tmpBuf,getpid());
	len = strlen(logInfo);
	
	va_start(args,fmt);
	vsprintf(logInfo+len,fmt,args);
	va_end(args);
	len += strlen(logInfo+len);

	UnionBufferLogSvrInfo(logInfo,len);
}

void UnionDebugLog(char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	char		logInfo[8096+1];
	int		len = 0;
	
	if (!UnionIsLogProgramerLevel(UnionGetLogFile()))
		return;

	memset(tmpBuf,0,40);
	UnionGetFullSystemDateTime(tmpBuf);
	sprintf(logInfo,"%10s::[Debug][%05d]\n",tmpBuf,getpid());
	len = strlen(logInfo);
	
	va_start(args,fmt);
	vsprintf(logInfo+len,fmt,args);
	va_end(args);
	len += strlen(logInfo+len);

	UnionBufferLogSvrInfo(logInfo,len);
}

void UnionDebugNullLog(char *fmt,...)
{
	va_list 	args;
	char		logInfo[8096+1];
	int		len = 0;
		
	if (!UnionIsLogProgramerLevel(UnionGetLogFile()))
		return;

	sprintf(logInfo,"%08d::",getpid());
	len = strlen(logInfo);
	
	va_start(args,fmt);
	vsprintf(logInfo+len,fmt,args);
	va_end(args);
	len += strlen(logInfo+len);
	
	UnionBufferLogSvrInfo(logInfo,len);
}

void UnionMerelyNullLog(char *fmt,...)
{
	va_list 	args;
	char		logInfo[8096+1];
	int		len = 0;
	
	if (!UnionIsLogDebugLevel(UnionGetLogFile()))
		return;

	va_start(args,fmt);
	vsprintf(logInfo+len,fmt,args);
	va_end(args);
	len += strlen(logInfo+len);

	UnionBufferLogSvrInfo(logInfo,len);
}

void UnionMerelyNullLogAnyway(char *fmt,...)
{
	va_list 	args;
	char		logInfo[8096+1];
	int		len = 0;
		
	va_start(args,fmt);
	vsprintf(logInfo+len,fmt,args);
	va_end(args);
	len += strlen(logInfo+len);

	UnionBufferLogSvrInfo(logInfo,len);
}

void UnionNullLog(char *fmt,...)
{
	va_list 	args;
	char		logInfo[8096+1];
	int		len = 0;
	
	if (!UnionIsLogDebugLevel(UnionGetLogFile()))
		return;

	sprintf(logInfo,"%08d::",getpid());
	len = strlen(logInfo);
	
	va_start(args,fmt);
	vsprintf(logInfo+len,fmt,args);
	va_end(args);
	len += strlen(logInfo+len);
	
	UnionBufferLogSvrInfo(logInfo,len);
}

void UnionMemNullLogWithTime(unsigned char *pBuffer,int iBufLen)
{
	char		tmpBuf[40+1];
	char		logInfo[8096+1];
	int		len = 0;
		
	if (!UnionIsLogDebugLevel(UnionGetLogFile()))
		return;
		
	if (pBuffer == NULL || iBufLen <= 0)
		return;

	memset(tmpBuf,0,40);
	UnionGetFullSystemDateTime(tmpBuf);
	sprintf(logInfo,"%10s::%08d::",tmpBuf,getpid());
	len = strlen(logInfo);
	
	bcdhex_to_aschex(pBuffer,iBufLen,logInfo+len);
	len += (iBufLen * 2);

	UnionBufferLogSvrInfo(logInfo,len);
}

void UnionNullLogWithTime(char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	char		logInfo[8096+1];
	int		len = 0;
	
	if (!UnionIsLogDebugLevel(UnionGetLogFile()))
		return;
	
	memset(tmpBuf,0,40);
	UnionGetFullSystemDateTime(tmpBuf);
	sprintf(logInfo,"%10s::%08d::",tmpBuf,getpid());
	len = strlen(logInfo);

	va_start(args,fmt);
	vsprintf(logInfo+len,fmt,args);
	va_end(args);

	len += strlen(logInfo+len);
	
	UnionBufferLogSvrInfo(logInfo,len);
}

void UnionNullLogWithTimeAnyway(char *fmt,...)
{
	va_list 	args;
	char		tmpBuf[40];
	char		logInfo[8096+1];
	int		len = 0;
	
	memset(tmpBuf,0,40);
	UnionGetFullSystemDateTime(tmpBuf);
	sprintf(logInfo,"%10s::%08d::",tmpBuf,getpid());
	len = strlen(logInfo);

	va_start(args,fmt);
	vsprintf(logInfo+len,fmt,args);
	va_end(args);

	len += strlen(logInfo+len);
	
	UnionBufferLogSvrInfo(logInfo,len);
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
	fprintf(fp,"%40s %02d %06d %02d %12d %05d\n",
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
	sprintf(fileName,"%s",plogFile->name);
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

void UnionProgramerMemLog(char *pTitle,unsigned char *pBuffer,int iBufLen)
{
	char		tmpBuf[40];
	char		logInfo[8096+1];
	int		len = 0;
		
	if (!UnionIsLogProgramerLevel(UnionGetLogFile()))
		return;
		
	if (pBuffer == NULL || iBufLen <= 0)
		return;

	memset(tmpBuf,0,40);
	UnionGetFullSystemDateTime(tmpBuf);
	sprintf(logInfo,"%10s::[Debug][%05d]\n%s\n",tmpBuf,getpid(),pTitle);
	len = strlen(logInfo);
	
	bcdhex_to_aschex(pBuffer,iBufLen,logInfo + len);
	len += (iBufLen * 2);

	
	UnionBufferLogSvrInfo(logInfo,len);
}
