//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	3.0

/*
	2003/09/19,Wolfgang Wang,��2.0����Ϊ3.0.
	����ʱ��ԭϵͳ����ʱ����������޳��γ�һ����ΪsystemTime1.0.c�ĳ���
*/

// 2006/08/11 ��20050909����������ΪĿǰ�汾

// 2003/09/19 Wolfgang Wang
#define _UnionLogMDL_3_x_	
#define _UnionEnv_3_x_
#define _LOG_FILE_NUM_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/stat.h>
#ifdef _WIN32
#	include <process.h>
#endif //_WIN32

#include "UnionStr.h"
#include "UnionEnv.h"
#include "UnionLog.h"

PUnionLogFile UnionGetLogFile();

int	gunionDynamicLogLevel = -1;
char	gunionSuffixOfMyLogFile[15];
long	gunionCountOfMyLogFile = 1;
int	gunionLogFileMDLConnected = 0;

int	gunionUserSuffixOfMyLogFileUsed = 0;
char	gunionUserSuffixOfMyLogFile[64+1];

// 2006/08/11 ����
int gunionUserDefinedErrorCode = errCodeDefaultErrCode;

// 2010/5/15,����������
char	gunionUserSpecLogFileName[256+1] = "";
int	gunionIsUserSpecLogFileSet = 0;
int	gunionIsUserSpecLogFileWithFullDir = 0;	// ��ʶһ���û���־�ļ��Ƿ���ȫ��

LOGS	*gs = NULL;
#define LOG_STYLES_UNION	( LOG_STYLE_DATETIMEMS | LOG_STYLE_LOGLEVEL | LOG_STYLE_PID | LOG_STYLE_SOURCE | LOG_STYLE_FORMAT )
#define UNIONLOG_SIZE_DEFAULT	1024*1024*1024

void UnionDestroyILOG()
{
	DestroyLogsHandle(gs);
	gs = NULL;
}

int UnionDeleteILOG(char *name)
{
	return(DeleteOutLogFromLogs(gs , name));
}

int UnionSetILOG(LOG *g , int output)
{
	int	len;
	char	fileName[MAXLEN_FILENAME];
	PUnionLogFile	plogFile = UnionGetLogFile();

	if (plogFile == NULL)
	{
		len = sprintf(fileName,"%s/unknown.log",getenv("UNIONLOG"));
		fileName[len] = 0;
		SetLogOutput(g, output , fileName , LOG_NO_OUTPUTFUNC );
		SetLogLevel(g , LOG_LEVEL_ALL );
		SetLogStyles(g , LOG_STYLES_UNION , LOG_NO_STYLEFUNC );
		SetLogRotateSize(g , UNIONLOG_SIZE_DEFAULT);
		SetLogRotateMode(g, LOG_ROTATEMODE_SIZE);
		SetLogRotateFileCount(g, 1);
	}
	else	
	{
		len = sprintf(fileName,"%s/%s.log",getenv("UNIONLOG"),plogFile->name);
		fileName[len] = 0;
		SetLogOutput(g, output , fileName , LOG_NO_OUTPUTFUNC );
		SetLogLevel(g , plogFile->level );
		SetLogStyles(g , LOG_STYLES_UNION , LOG_NO_STYLEFUNC );
		if (plogFile->maxSize > 0)
		{
			SetLogRotateSize(g , plogFile->maxSize);
			if (plogFile->rewriteAttr == conLogFullRewriteAtonce)	// ��־����д
			{
				SetLogRotateMode( g , LOG_ROTATEMODE_SIZE);
				SetLogRotateFileCount(g, 1);
			}
			else if (plogFile->rewriteAttr == conLogFullRewriteAfterBackup)	// ����д
			{
				SetLogRotateMode( g , LOG_ROTATEMODE_SIZE);
				SetLogRotateFileCount(g, plogFile->logFileNum);
			}
			else if ((plogFile->rewriteAttr == conLogNewEveryDay) ||	// �ձ���
				(plogFile->rewriteAttr == conLogNewEveryMonth) ||	// �±���
				(plogFile->rewriteAttr == conLogNewEverySeason) ||	// ���ȱ���
				(plogFile->rewriteAttr == conLogNewHalfYear) ||		// ���걸��
				(plogFile->rewriteAttr == conLogNewEveryYear))		// �걸��
			{
				SetLogRotateMode( g , LOG_ROTATEMODE_PER_DAY|LOG_ROTATEMODE_SIZE);
				SetLogRotateFileCount(g, plogFile->logFileNum);
			}
		}
		else
		{
			//SetLogRotateSize(g , 0);
			SetLogRotateSize(g , UNIONLOG_SIZE_DEFAULT);
			// ��ת��
			if ((plogFile->rewriteAttr == conLogNewEveryDay) ||	// �ձ���
				(plogFile->rewriteAttr == conLogNewEveryMonth) ||	// �±���
				(plogFile->rewriteAttr == conLogNewEverySeason) ||	// ���ȱ���
				(plogFile->rewriteAttr == conLogNewHalfYear) ||		// ���걸��
				(plogFile->rewriteAttr == conLogNewEveryYear))		// �걸��
			{
				SetLogRotateMode( g , LOG_ROTATEMODE_PER_DAY|LOG_ROTATEMODE_SIZE);
				SetLogRotateFileCount(g, 1);
			}
			else
			{
				SetLogRotateMode( g , LOG_ROTATEMODE_SIZE);
				SetLogRotateFileCount(g, 1);
			}
		}
	}
	return(0);
}

int UnionIsInitILOG()
{
	int	i;
	int	ret;
	LOG	*g = NULL;
	PUnionLogFile	plogFile = UnionGetLogFile();
	
	if (gs)
		return(1);
	
	if ((gs = CreateLogsHandle()) == NULL)
	{
		fprintf(stderr, "������־�����ʧ��errno[%d]\n" , errno );
		return(0);
	}
	
	for (i = 0; ; i++)
	{
		if (i > 3)
		{
			fprintf(stderr, "������־���ʧ��errno[%d]\n" , errno );
			UnionDestroyILOG();
			return(0);
		}
	
		if ((g = CreateLogHandle()) == NULL)
		{
			usleep(200);
			continue;
		}
		else
			break;
	}
	
	UnionSetILOG(g,LOG_OUTPUT_FILE);
	SetTimingInitFunc( g , LOG_TIMING_TIME_DEFAULT, UnionSetILOG);
	if (plogFile == NULL)
		ret = AddLogToLogs( gs , "unknown" , g );
	else
		ret = AddLogToLogs( gs , plogFile->name , g );

	if (ret < 0)
	{
		fprintf(stderr,"in UnionIsInitILOG:: AddLogToLogs ret[%d]\n",ret);
		return(ret);
	}
	
	return(1);
}

void UnionSetMultiLogBegin()
{
	SetMultiLogBegin(GetLogFromLogs(gs,mdc_get()));
	return;
}

void UnionSetMultiLogEnd()
{
	SetMultiLogEnd(GetLogFromLogs(gs,mdc_get()));
	return;
}

//add by hzh in 2012.12.13 Ϊ֧�ּ���20121101���������
void UnionSetFlushLogFileLinesMode(int mode __attribute__((unused)))
{
	return;
}

//ȡfflush��ʽ
int UnionGetFlushLogFileLinesMode()
{
	return 0;
}

//fflush��־�ļ����е������Ѵ򿪵���־�ļ�
void UnionFlushAllLogFileInGrpNoWait()
{
	return;
}

//add end

// 2010/12/2,����������
// ����־�ļ���ʱ����ָ���ļ�
void UnionRedirectLogFileToSpecFile(char *fileName)
{
	UnionSetUserSpecLogFileName(fileName);
	gunionIsUserSpecLogFileWithFullDir = 1;
}

// 2010/12/2,����������
// �ָ�ȱʡ��־����
int UnionRestoreDefaultLogFile()
{
	gunionIsUserSpecLogFileWithFullDir = 0;
	gunionIsUserSpecLogFileSet = 0;
	return(0);
}

// 2010/5/15,����������
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

// 2006/08/11 ����
int UnionSetUserDefinedErrorCode(int errCode)
{
	return(gunionUserDefinedErrorCode = errCode);
}

// 2010/10/30 ����
void *UnionSetUserDefinedErrorCodeReturnNullPointer(int errCode)
{
	gunionUserDefinedErrorCode = errCode;
	return(NULL);
}

// 2009/08/11 ����
int UnionGetUserDefinedErrorCode()
{
	return(gunionUserDefinedErrorCode);
}

int UnionSetSuffixOfMyLogFile(char *logFileSuffix)
{
	UnionSetSuffixAndMDCOfMyLogFile("mdc",logFileSuffix);
	return(0);
}

int UnionSetSuffixAndMDCOfMyLogFile(char *mdc,char *logFileSuffix)
{
	int	i;
	int	ret;
	LOG	*g = NULL;
	PUnionLogFile	plogFile = UnionGetLogFile();
		
	if (plogFile == NULL)
		return(0);

	if (UnionIsInitILOG() == 0)
		return(0);

	mdc_put(logFileSuffix);

	if (FindLogFromLogs(gs,logFileSuffix))
		return(0);	

	// �����ӵ�
	for (i = 0 ; ; i++)
	{
		if (i > 3)
			return(0);
		if ((g = CreateLogHandle()) == NULL)
			continue;
		else
			break;
	}

	UnionSetILOG(g,LOG_OUTPUT_FILE);
	SetTimingInitFunc( g , LOG_TIMING_TIME_DEFAULT, UnionSetILOG);
	if ((ret = AddLogToLogs( gs , logFileSuffix , g )) < 0)
	{
		fprintf(stderr,"in UnionSetSuffixAndMDCOfMyLogFile:: AddLogToLogs ret[%d]\n",ret);
		return(ret);
	}
	
	return(0);
}

int UnionCloseSuffixOfMyLogFile()
{
	gunionUserSuffixOfMyLogFileUsed = 0;
	return(0);
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
	fprintf(fp,"%40s %4d %4d %6ld %4d %12ld %6d\n",
		plogFile->name,plogFile->level,plogFile->logFileNum,plogFile->maxSize/1000000,
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

void UnionSystemErrLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitILOG() == 0)
		return;
	
	va_start(args,fmt);
	
	WriteLogsV( gs , fileName , line , LOG_LEVEL_OSERR ,fmt ,args);
	
	va_end(args);

	return;
}

// UserErrLevel
void UnionUserErrLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitILOG() == 0)
		return;
	
	va_start(args,fmt);
	
	WriteLogsV( gs , fileName , line , LOG_LEVEL_ERROR ,fmt ,args);
	
	va_end(args);

	return;
}

void UnionMemErrLogW(char *fileName,char *funName,int line,char *pTitle,unsigned char *pBuffer,int iBufLen)
{
	if (UnionIsInitILOG() == 0)
		return;
	
	WriteHexLogs( gs , fileName , line , LOG_LEVEL_ERROR , (char *)pBuffer , iBufLen , pTitle);
	
	return;
}

// AuditLevel
void UnionSuccessLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitILOG() == 0)
		return;
	
	va_start(args,fmt);
	
	WriteLogsV( gs , fileName , line , LOG_LEVEL_INFO ,fmt ,args);
	
	va_end(args);

	return;
}

void UnionAuditLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitILOG() == 0)
		return;
	
	va_start(args,fmt);
	
	WriteLogsV( gs , fileName , line , LOG_LEVEL_INFO ,fmt ,args);
	
	va_end(args);

	return;
}

void UnionRealNullLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitILOG() == 0)
		return;
	
	va_start(args,fmt);
	
	WriteLogsV( gs , fileName , line , LOG_LEVEL_INFO ,fmt ,args);
	
	va_end(args);

	return;
}
void UnionAuditNullLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitILOG() == 0)
		return;
	
	va_start(args,fmt);
	
	WriteLogsV( gs , fileName , line , LOG_LEVEL_INFO ,fmt ,args);
	
	va_end(args);

	return;
}

void UnionAuditNullLogWithTimeW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitILOG() == 0)
		return;
	
	va_start(args,fmt);
	
	WriteLogsV( gs , fileName , line , LOG_LEVEL_INFO ,fmt ,args);
	
	va_end(args);

	return;
}

// DebugLevel
void UnionLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitILOG() == 0)
		return;
	
	va_start(args,fmt);
	
	WriteLogsV( gs , fileName , line , LOG_LEVEL_INFO ,fmt ,args);
	
	va_end(args);

	return;
}

void UnionMemLogW(char *fileName,char *funName,int line,char *pTitle,unsigned char *pBuffer,int iBufLen)
{
	if (UnionIsInitILOG() == 0)
		return;
	
	WriteHexLogs( gs , fileName , line , LOG_LEVEL_INFO , (char *)pBuffer , iBufLen , pTitle);

	return;
}

void UnionProgramerLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitILOG() == 0)
		return;
	
	va_start(args,fmt);
	
	WriteLogsV( gs , fileName , line , LOG_LEVEL_PGM ,fmt ,args);
	
	va_end(args);

	return;
}

void UnionDebugLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitILOG() == 0)
		return;
	
	va_start(args,fmt);
	
	WriteLogsV( gs , fileName , line , LOG_LEVEL_DEBUG ,fmt ,args);
	
	va_end(args);

	return;
}


void UnionDebugNullLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitILOG() == 0)
		return;
	
	va_start(args,fmt);
	
	WriteLogsV( gs , fileName , line , LOG_LEVEL_DEBUG ,fmt ,args);
	
	va_end(args);

	return;
}

void UnionNullLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitILOG() == 0)
		return;
	
	va_start(args,fmt);
	
	WriteLogsV( gs , fileName , line , LOG_LEVEL_INFO ,fmt ,args);
	
	va_end(args);

	return;
}

void UnionMemNullLogWithTimeW(char *fileName,char *funName,int line,unsigned char *pBuffer,int iBufLen)
{
	if (UnionIsInitILOG() == 0)
		return;
	
	WriteHexLogs( gs , fileName , line , LOG_LEVEL_INFO , (char *)pBuffer , iBufLen,"");

	return;
}

void UnionNullLogWithTimeW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitILOG() == 0)
		return;
	
	va_start(args,fmt);
	
	WriteLogsV( gs , fileName , line , LOG_LEVEL_INFO ,fmt ,args);
	
	va_end(args);

	return;
}

void UnionNullLogWithTimeAnywayW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitILOG() == 0)
		return;
	
	va_start(args,fmt);
	
	WriteLogsV( gs , fileName , line , LOG_LEVEL_INFO ,fmt ,args);
	
	va_end(args);

	return;
}

void UnionProgramerMemLogW(char *fileName,char *funName,int line,char *pTitle,unsigned char *pBuffer,int iBufLen)
{
	if (UnionIsInitILOG() == 0)
		return;
	
	WriteHexLogs( gs , fileName , line , LOG_LEVEL_INFO , (char *)pBuffer , iBufLen , pTitle);

	return;
}

