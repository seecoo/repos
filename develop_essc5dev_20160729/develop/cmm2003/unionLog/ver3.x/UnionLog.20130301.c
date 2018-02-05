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
int Union_zlog_fini_inner();

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

typedef struct
{
	zlog_category_t	*category;	// ͨ�÷���
	zlog_category_t	*category_N;	// ��ǰ׺����
	zlog_category_t	*category_H;	// Hex����
}TUnionZlogCategory;
typedef TUnionZlogCategory	*PUnionZlogCategory;

TUnionZlogCategory	gunionZlogCategory;

int	gunionIsInitZlog = 0;
int	gunionForceInitZlog = 0;

char	gunionLogConfigureFile[256] = "";
int	gunionLogConfigureFileLastMtime = 0;

int UnionGetFileMtime(const char *fullname)
{
	struct stat	     buf;

	if(stat(fullname, &buf) == 0)
	{
		return((int)buf.st_mtime);
	}
	else
	{
		return(-1);
	}
}
int UnionForceInitZlog()
{
	return(gunionForceInitZlog = 1);
}
int UnionIsInitZlog()
{
	int		i;
	char		fileName[512+1];
	char		categoryName[3][128+1];
	char		logFileName[256+1];
	int		lastTime;
	
	PUnionLogFile 	plogFile = NULL;
	
	if (gunionIsInitZlog == 1)
	{
		if(gunionForceInitZlog)
		{
			zlog_fini();
			gunionForceInitZlog = 0;
		}
		else
		{
			// �ļ����޸Ĺ�
			if((lastTime = UnionGetFileMtime(gunionLogConfigureFile)) > gunionLogConfigureFileLastMtime)
			{
				gunionLogConfigureFileLastMtime = lastTime;
				zlog_fini();
			}
			else
			{
				return(1);
			}
		}
	}

	if (gunionIsInitZlog == -1)
		zlog_fini();
	
	if(gunionIsInitZlog != 1)
	{
		memset(fileName,0,sizeof(fileName));
		sprintf(fileName,"%s/%s",getenv("UNIONETC"),"zlog_default.conf");
		strncpy(gunionLogConfigureFile, fileName, sizeof(gunionLogConfigureFile));
		gunionLogConfigureFileLastMtime = UnionGetFileMtime(gunionLogConfigureFile);
	}
	
	
	if (zlog_init(gunionLogConfigureFile) != 0)
	{
		printf("UnionIsInitZlog[%s] error\n",gunionLogConfigureFile);
		return(0);
	}
	
	for (i = 0; i < 3; i++)
		memset(categoryName[i],0,sizeof(categoryName[i]));
	
	if ((plogFile = UnionGetLogFile()) == NULL)
	{
		sprintf(categoryName[0],"unknown");
		sprintf(categoryName[1],"unknown");
		sprintf(categoryName[2],"unknown");
		gunionIsInitZlog = -1;
	}
	else
	{
		memset(logFileName,0,sizeof(logFileName));
		UnionGetCategoryNameOfZlog(plogFile->name,logFileName);		
		sprintf(categoryName[0],"%s",logFileName);
		sprintf(categoryName[1],"%s_N",logFileName);
		sprintf(categoryName[2],"%s_H",logFileName);
		gunionIsInitZlog = 1;
	}
	
	if ((gunionZlogCategory.category = zlog_get_category(categoryName[0])) == NULL)
	{
		printf("in UnionIsInitZlog:: zlog_get_category[0][%s] error\n",categoryName[0]);
		gunionIsInitZlog = 0;
		zlog_fini();
		return(0);
	}
	if ((gunionZlogCategory.category_N = zlog_get_category(categoryName[1])) == NULL)
	{
		printf("in UnionIsInitZlog:: zlog_get_category[1][%s] error\n",categoryName[1]);
		gunionIsInitZlog = 0;
		zlog_fini();
		return(0);
	}
	if ((gunionZlogCategory.category_H = zlog_get_category(categoryName[2])) == NULL)
	{
		printf("in UnionIsInitZlog:: zlog_get_category[2][%s] error\n",categoryName[2]);
		gunionIsInitZlog = 0;
		zlog_fini();
		return(0);
	}
	return(1);
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
	zlog_put_mdc(mdc,logFileSuffix);
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

void UnionSystemErrLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitZlog() == 0)
		return;
	
	va_start(args,fmt);
	
	vzlog(gunionZlogCategory.category,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_OSERR,fmt,args);
	
	va_end(args);

	return;
}

// UserErrLevel
void UnionUserErrLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitZlog() == 0)
		return;
	
	va_start(args,fmt);
	
	vzlog(gunionZlogCategory.category,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_USERERR,fmt,args);
	
	va_end(args);

	return;
}

void UnionMemErrLogW(char *fileName,char *funName,int line,char *pTitle,unsigned char *pBuffer,int iBufLen)
{
	if (UnionIsInitZlog() == 0)
		return;
	
	zlog(gunionZlogCategory.category,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_USERERR,pTitle);

	hzlog(gunionZlogCategory.category_H,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_USERERR,pBuffer,iBufLen);
	return;
}

// AuditLevel
void UnionSuccessLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitZlog() == 0)
		return;
	
	va_start(args,fmt);
	
	vzlog(gunionZlogCategory.category,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_INFO,fmt,args);
	
	va_end(args);

	return;
}

void UnionAuditLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitZlog() == 0)
		return;
	
	va_start(args,fmt);
	
	vzlog(gunionZlogCategory.category,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_INFO,fmt,args);
	
	va_end(args);

	return;
}

void UnionRealNullLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitZlog() == 0)
		return;
	
	va_start(args,fmt);
	
	vzlog(gunionZlogCategory.category_N,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_INFO,fmt,args);
	
	va_end(args);

	return;
}
void UnionAuditNullLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitZlog() == 0)
		return;
	
	va_start(args,fmt);
	
	vzlog(gunionZlogCategory.category_N,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_INFO,fmt,args);
	
	va_end(args);

	return;
}

void UnionAuditNullLogWithTimeW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitZlog() == 0)
		return;
	
	va_start(args,fmt);
	
	vzlog(gunionZlogCategory.category_N,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_INFO,fmt,args);
	
	va_end(args);

	return;
}

// DebugLevel
void UnionLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitZlog() == 0)
		return;
	
	va_start(args,fmt);
	
	vzlog(gunionZlogCategory.category,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_INFO,fmt,args);
	
	va_end(args);

	return;
}

void UnionMemLogW(char *fileName,char *funName,int line,char *pTitle,unsigned char *pBuffer,int iBufLen)
{
	if (UnionIsInitZlog() == 0)
		return;
	
	zlog(gunionZlogCategory.category,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_INFO,pTitle);

	hzlog(gunionZlogCategory.category_H,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_INFO,pBuffer,iBufLen);
	return;
}

void UnionProgramerLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitZlog() == 0)
		return;
	
	va_start(args,fmt);
	
	vzlog(gunionZlogCategory.category,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_PGM,fmt,args);
	
	va_end(args);

	return;
}

void UnionDebugLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitZlog() == 0)
		return;
	
	va_start(args,fmt);
	
	vzlog(gunionZlogCategory.category,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_DEBUG,fmt,args);
	
	va_end(args);

	return;
}


void UnionDebugNullLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitZlog() == 0)
		return;
	
	va_start(args,fmt);
	
	vzlog(gunionZlogCategory.category_N,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_DEBUG,fmt,args);
	
	va_end(args);

	return;
}

void UnionNullLogW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitZlog() == 0)
		return;
	
	va_start(args,fmt);
	
	vzlog(gunionZlogCategory.category_N,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_INFO,fmt,args);
	
	va_end(args);

	return;
}

void UnionMemNullLogWithTimeW(char *fileName,char *funName,int line,unsigned char *pBuffer,int iBufLen)
{
	if (UnionIsInitZlog() == 0)
		return;
	
	hzlog(gunionZlogCategory.category_H,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_INFO,pBuffer,iBufLen);
	return;
}

void UnionNullLogWithTimeW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitZlog() == 0)
		return;
	
	va_start(args,fmt);
	
	vzlog(gunionZlogCategory.category_N,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_INFO,fmt,args);
	
	va_end(args);

	return;
}

void UnionNullLogWithTimeAnywayW(char *fileName,char *funName,int line,char *fmt,...)
{
	va_list 	args;
	
	if (UnionIsInitZlog() == 0)
		return;
	
	va_start(args,fmt);
	
	vzlog(gunionZlogCategory.category_N,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_INFO,fmt,args);
	
	va_end(args);

	return;
}

void UnionProgramerMemLogW(char *fileName,char *funName,int line,char *pTitle,unsigned char *pBuffer,int iBufLen)
{
	if (UnionIsInitZlog() == 0)
		return;
	
	zlog(gunionZlogCategory.category,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_PGM,pTitle);

	hzlog(gunionZlogCategory.category_H,fileName, strlen(fileName),funName,strlen(funName),line,ZLOG_LEVEL_PGM,pBuffer,iBufLen);
	return;
}

