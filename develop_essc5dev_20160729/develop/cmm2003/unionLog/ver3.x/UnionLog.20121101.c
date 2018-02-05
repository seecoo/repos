//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	3.0

/*
	2003/09/19,Wolfgang Wang,��2.0����Ϊ3.0.
	����ʱ��ԭϵͳ����ʱ����������޳��γ�һ����ΪsystemTime1.0.c�ĳ���
*/

// 2006/08/11 ��20050909����������ΪĿǰ�汾

// 2003/09/19 Wolfgang Wang

/* 2012/11/1 hzh ��20060811����������ΪĿǰ�汾
	ԭ����ÿдһ����־����򿪡��ر�һ���ļ���
	Ϊ����д��־�����ܣ��ָ�Ϊ�Դ򿪵���־�ļ�����д�����־�ٹرա�
	֧��д������־�󣬽���һ��fflushд�ļ���ģʽ
	����ӿ�,��UnionLog,UnionUserErrLog�Ⱥ������ñ��ֲ���
	���ӽӿ�(֧��д������־�󣬽���һ��fflushд�ļ���ģʽ)
	UnionSetFlushLogFileLinesMode(int mode);
	UnionFlushAllLogFileInGrpNoWait();
*/

#define _UnionLogMDL_3_x_	
#define _UnionEnv_3_x_


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>

#include <stdlib.h>
#include <sys/stat.h>
#include "UnionStr.h"


#include "UnionEnv.h"

#ifdef _WIN32
#	include <process.h>
#endif //_WIN32

// 2003/09/19 Wolfgang Wang
#include "UnionLog.h"

void UnionSetUserSpecLogFileName(char *fileName);
extern PUnionLogFile UnionGetLogFile(); 

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

//add by hzh in 2012.11.1
#define MAX_LOGFILE_NUM  5   //ͬһ���������ɴ򿪵���־�ļ���

//��ǰ��־�ļ��ṹ
typedef struct {
	char logFileName[256+1];  //��־�ļ���(��·��)
	FILE *logfp;             //�򿪵���־�ļ�ָ��
	time_t  openTime;        //�������־��ʱ��
	time_t  flushTime;       //����fflushд����־��ʱ��
} TUnionElementLogFile;
typedef TUnionElementLogFile *PUnionElementLogFile;

TUnionElementLogFile gCurrentLogFileGrp[MAX_LOGFILE_NUM] = {
		{"",NULL,0,0},
		{"",NULL,0,0},
		{"",NULL,0,0},
		{"",NULL,0,0},
		{"",NULL,0,0}
};
	
char gCurrentUsingFileName[256+1]="";   //��ǰ����ʹ�õ���־�ļ�
int  giFlushLinesMode = 0;   //fflush��ʽ��0--д����Logfile,fflushһ�Σ�1---����(���5��)дLogFile,��ÿ��һ��fflush



typedef struct
{
	zlog_category_t	*category;	// ͨ�÷���
	zlog_category_t	*category_N;	// ��ǰ׺����
	zlog_category_t	*category_H;	// Hex����
}TUnionZlogCategory;
typedef TUnionZlogCategory	*PUnionZlogCategory;
TUnionZlogCategory	gunionZlogCategory;

//����fflush��ʽ. ȱʡ(mode=0)Ϊÿдһ�μ�fflush
void UnionSetFlushLogFileLinesMode(int mode)
{
	giFlushLinesMode = mode;
}

//ȡfflush��ʽ
int UnionGetFlushLogFileLinesMode()
{
	return giFlushLinesMode;
}

//�ļ��Ƿ�����־�ļ���,����: 0--�� 1--��
int UnionIsExistLogFileInGrp(char *fileName)
{
	int i = 0;
	if (fileName == NULL || strlen(fileName) == 0)
		return 0;
	for(i=0;i<MAX_LOGFILE_NUM;i++)
	{
		if (strlen(gCurrentLogFileGrp[i].logFileName) == 0)
			continue;
		if (strcmp(fileName,gCurrentLogFileGrp[i].logFileName) == 0)
			return 1;
	}
	return 0;
}

/*
	fflush��־�ļ����е���־�ļ�����giFlushLinesModeֵΪ0ʱ��ʵʱˢ�¡�
	��ѡ����giFlushLinesMode��0ģʽ�£��ϴ�fflush��1��ǰ���ѵ���5�β�ˢ��
*/
void UnionFlushLogFileInGrp(char *fileName)
{
	int i = 0;
	time_t mt = 0;
	static int iCount = 0;
	if (fileName == NULL || strlen(fileName) == 0)
		return;
	for(i=0;i<MAX_LOGFILE_NUM;i++)
	{
		if (strlen(gCurrentLogFileGrp[i].logFileName) == 0)
			continue;
		if (strcmp(fileName,gCurrentLogFileGrp[i].logFileName) == 0)
		{
			if(giFlushLinesMode)
			{
				time(&mt);
		 		if(mt - gCurrentLogFileGrp[i].flushTime > 1)
				{
					if (gCurrentLogFileGrp[i].logfp != NULL || iCount >= 5)
					{
						fflush(gCurrentLogFileGrp[i].logfp);
						time(&gCurrentLogFileGrp[i].flushTime);
						iCount = 0;
					}
					iCount++;
				}
				return;
			}
			fflush(gCurrentLogFileGrp[i].logfp);
			return;
		}
	}
	return;
}

//fflush��־�ļ����е���־�ļ�, ʵʱˢ��
void UnionFlushLogFileInGrpNoWait(char *fileName)
{
	int i = 0;
	if (fileName == NULL || strlen(fileName) == 0)
		return;
	for(i=0;i<MAX_LOGFILE_NUM;i++)
	{
		if (strlen(gCurrentLogFileGrp[i].logFileName) == 0)
			continue;
		if (strcmp(fileName,gCurrentLogFileGrp[i].logFileName) == 0)
		{
			if (gCurrentLogFileGrp[i].logfp != NULL)
			{
				fflush(gCurrentLogFileGrp[i].logfp);
				time(&gCurrentLogFileGrp[i].flushTime);
			}
			return;
		}
	}
	return;
}

//fflush��־�ļ����е������Ѵ򿪵���־�ļ�
void UnionFlushAllLogFileInGrpNoWait()
{
	int i = 0;
	for(i=0;i<MAX_LOGFILE_NUM;i++)
	{
		if (gCurrentLogFileGrp[i].logfp != NULL)
		{
			fflush(gCurrentLogFileGrp[i].logfp);
			time(&gCurrentLogFileGrp[i].flushTime);
		}
	}
	return;
}


//����־�ļ�����ѡȡ�ļ��򿪲���ȡ��־�ļ���С.
FILE *UnionOpenLogFileInGrp(char *fileName,long *fileSize)
{
	int i = 0;
	time_t mt = 0;
	struct stat statbuf;
	int index = 0;
	
	if (fileName == NULL || strlen(fileName) == 0)
		return NULL;
		
	*fileSize = 0;
	for(i=0;i<MAX_LOGFILE_NUM;i++)
	{
		if (strlen(gCurrentLogFileGrp[i].logFileName) == 0)
			continue;
		if (strcmp(fileName,gCurrentLogFileGrp[i].logFileName) == 0)
		{
			if (gCurrentLogFileGrp[i].logfp == NULL) 
			{
	  		time(&gCurrentLogFileGrp[i].openTime);
	  		stat(gCurrentLogFileGrp[i].logFileName,&statbuf);
	  		*fileSize = statbuf.st_size;
	  		gCurrentLogFileGrp[i].logfp = fopen(gCurrentLogFileGrp[i].logFileName,"a");
	  		if (gCurrentLogFileGrp[i].logfp == NULL)
					return NULL;
			}
			
			//gCurrentLogFileGrp[i].logfp��Ϊ��
			memset(&statbuf,0,sizeof(statbuf));
			fstat(fileno(gCurrentLogFileGrp[i].logfp), &statbuf);
			if((statbuf.st_nlink == 0)) //�ļ����ӱ�ɾ
			{
				fclose(gCurrentLogFileGrp[i].logfp);
	  			time(&gCurrentLogFileGrp[i].openTime);
				gCurrentLogFileGrp[i].logfp = fopen(gCurrentLogFileGrp[i].logFileName,"a");
				if(gCurrentLogFileGrp[i].logfp == NULL)
				{
					return(NULL);
				}
				memset(&statbuf,0,sizeof(statbuf));
				fstat(fileno(gCurrentLogFileGrp[i].logfp), &statbuf);
				*fileSize = statbuf.st_size;
				return gCurrentLogFileGrp[i].logfp;
			}
			else //������
			{
				*fileSize = statbuf.st_size;
				return gCurrentLogFileGrp[i].logfp;
			}
		}
	}
	
	//�ļ���δ�ҵ�,ѡһ�����еĴ�
	for(i=0;i<MAX_LOGFILE_NUM;i++)
	{
		if (strlen(gCurrentLogFileGrp[i].logFileName) == 0)
		{
			snprintf(gCurrentLogFileGrp[i].logFileName,sizeof(gCurrentLogFileGrp[i].logFileName)-1,fileName);
			time(&gCurrentLogFileGrp[i].openTime);
			if(gCurrentLogFileGrp[i].logfp != NULL)
				fclose(gCurrentLogFileGrp[i].logfp);
			gCurrentLogFileGrp[i].logfp = fopen(gCurrentLogFileGrp[i].logFileName,"a");
			if(gCurrentLogFileGrp[i].logfp == NULL)
			{
				return(NULL);
			}
			memset(&statbuf,0,sizeof(statbuf));
			fstat(fileno(gCurrentLogFileGrp[i].logfp), &statbuf);
			*fileSize = statbuf.st_size;
			return gCurrentLogFileGrp[i].logfp;
		}
	}

	//λ�ñ�ռ����������򿪵��ļ�λ��
	index = 0;
	mt = gCurrentLogFileGrp[0].openTime;
	for(i=1;i<MAX_LOGFILE_NUM;i++)
	{
		if(gCurrentLogFileGrp[i].openTime < mt)
		{
			index = i;
			mt = gCurrentLogFileGrp[i].openTime;
		}
	}
	if(gCurrentLogFileGrp[index].logfp != NULL)
		fclose(gCurrentLogFileGrp[index].logfp);
	snprintf(gCurrentLogFileGrp[index].logFileName,sizeof(gCurrentLogFileGrp[index].logFileName)-1,fileName);
	time(&gCurrentLogFileGrp[index].openTime);
	gCurrentLogFileGrp[index].logfp = fopen(gCurrentLogFileGrp[index].logFileName,"a");
	if(gCurrentLogFileGrp[index].logfp == NULL)
	{
		return(NULL);
	}	
	memset(&statbuf,0,sizeof(statbuf));
	fstat(fileno(gCurrentLogFileGrp[index].logfp), &statbuf);
	*fileSize = statbuf.st_size;
	return gCurrentLogFileGrp[index].logfp;
}

//����־�ļ�����ѡȡ�ļ�,���´���־�ļ�
FILE *UnionReWriteLogFileInGrp(char *fileName)
{
	int i = 0;
	int index = 0;
	time_t mt = 0;
	
	if (fileName == NULL || strlen(fileName) == 0)
		return NULL;
		
	for(i=0;i<MAX_LOGFILE_NUM;i++)
	{
		if (strlen(gCurrentLogFileGrp[i].logFileName) == 0)
			continue;
		if (strcmp(fileName,gCurrentLogFileGrp[i].logFileName) == 0)
		{
				if(gCurrentLogFileGrp[i].logfp != NULL)
	   			fclose(gCurrentLogFileGrp[i].logfp);
				unlink(gCurrentLogFileGrp[i].logFileName);
				time(&gCurrentLogFileGrp[i].openTime);
				gCurrentLogFileGrp[i].logfp = fopen(gCurrentLogFileGrp[i].logFileName,"a");
				if(gCurrentLogFileGrp[i].logfp == NULL)
				{
					return(NULL);
				}
				return gCurrentLogFileGrp[i].logfp;
		}
	}
	//λ�ñ�ռ����������򿪵��ļ�λ��
	index = 0;
	mt = gCurrentLogFileGrp[0].openTime;
	for(i=1;i<MAX_LOGFILE_NUM;i++)
	{
		if(gCurrentLogFileGrp[i].openTime < mt)
		{
			index = i;
			mt = gCurrentLogFileGrp[i].openTime;
		}
	}
	if(gCurrentLogFileGrp[index].logfp != NULL)
		fclose(gCurrentLogFileGrp[index].logfp);
	snprintf(gCurrentLogFileGrp[index].logFileName,sizeof(gCurrentLogFileGrp[index].logFileName)-1,fileName);
	unlink(gCurrentLogFileGrp[index].logFileName);
	time(&gCurrentLogFileGrp[index].openTime);
	gCurrentLogFileGrp[index].logfp = fopen(gCurrentLogFileGrp[index].logFileName,"a");
	if(gCurrentLogFileGrp[index].logfp == NULL)
	{
		return(NULL);
	}
	return gCurrentLogFileGrp[index].logfp;
}

//�ر���־�ļ�(���Ѵ�ʱ�䳬50��Ĳ�ִ��)
int UnionCloseLogFileInGrp(char *fileName)
{
	int i = 0;
	time_t mt;
	if (fileName == NULL || strlen(fileName) == 0)
		return 0;
	for(i=0;i<MAX_LOGFILE_NUM;i++)
	{
		if (strlen(gCurrentLogFileGrp[i].logFileName) == 0)
			continue;
		if (strcmp(fileName,gCurrentLogFileGrp[i].logFileName) == 0)
		{
			time(&mt);
			if(mt - gCurrentLogFileGrp[i].openTime > 50)
			{
				if (gCurrentLogFileGrp[i].logfp != NULL)
				{
					fflush(gCurrentLogFileGrp[i].logfp);
					fclose(gCurrentLogFileGrp[i].logfp);
					gCurrentLogFileGrp[i].logfp = NULL;
					gCurrentLogFileGrp[i].logFileName[0] = '\0';
				}
			}
			return 1;
		}
	}
	return 0;
}

//��ʱ�ر���־�ļ�
int UnionCloseLogFileInGrpNoWait(char *fileName)
{
	int i = 0;
	if (fileName == NULL || strlen(fileName) == 0)
		return 0;
	for(i=0;i<MAX_LOGFILE_NUM;i++)
	{
		if (strlen(gCurrentLogFileGrp[i].logFileName) == 0)
			continue;
		if (strcmp(fileName,gCurrentLogFileGrp[i].logFileName) == 0)
		{
				if (gCurrentLogFileGrp[i].logfp != NULL)
				{
					fflush(gCurrentLogFileGrp[i].logfp);
					fclose(gCurrentLogFileGrp[i].logfp);
					gCurrentLogFileGrp[i].logfp = NULL;
					gCurrentLogFileGrp[i].logFileName[0] = '\0';
					return 1;
				}
		}
	}
	return 0;
}
//add end by hzh in 2012.11.1


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

int UnionGetFullSystemDateExt(char *systemDate)
{	
	char buf[50];	
	UnionGetCurrentDateTimeInMacroFormat(buf);	
	memcpy(systemDate,buf,4);	
	memcpy(systemDate+4,buf+5,2);	
	memcpy(systemDate+6,buf+8,2);	
	systemDate[8] = '\0';	
	return(0);
}

int UnionGetSuffixForLogFile(TUnionLogFullRewriteStrategy rewriteAttr,char *suffix)
{
	char	systemDate[15];
	
	memset(systemDate,0,sizeof(systemDate));
	//UnionGetFullSystemDate(systemDate);
	UnionGetFullSystemDateExt(systemDate);
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

	// 2010/5/15,�������޸�	
	if (gunionIsUserSpecLogFileSet)	// 2010/5/15,����������
	{
		if (!gunionIsUserSpecLogFileWithFullDir)
			sprintf(fileName,"%s/%s",getenv("UNIONLOG"),gunionUserSpecLogFileName); // 2010/5/15,����������
		else
		{
			strcpy(fileName,gunionUserSpecLogFileName);
			return(0);
		}
	}
	else	// 2010/5/15,����������
	{
		if (plogFile == NULL)
		{
			sprintf(fileName,"%s/unknown.log",getenv("UNIONLOG"));
			return(0);
			//return(errCodeParameter);
		}
	}
	// 2010/5/15,�������޸Ľ���

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

//modify by hzh in 2012.11.1
FILE *UnionOpenLogFile(PUnionLogFile plogFile)
{
	char 	fileName[512];
	FILE 	*logFilePtr;
	int	ret;
	long	maxSize = 0;
	long  curFileSize = 0;
	struct stat statBuf;
	
reOpenLogFile:
	memset(fileName,0,512);
	if ((ret = UnionGetNameOfMyLogFile(plogFile,fileName)) < 0)
		return(stderr);
		
	snprintf(gCurrentUsingFileName,sizeof(gCurrentUsingFileName)-1,fileName);
		
	/*
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
	*/
	
	if (plogFile == NULL || maxSize <= 0)
	{
		maxSize = 1024*1024*10;
		//return(logFilePtr);
	}
	else
		maxSize = plogFile->maxSize;
		
	//������־�ļ���
	if ( !UnionIsExistLogFileInGrp(fileName) )
	{
		memset(&statBuf,0,sizeof(statBuf));
		stat(fileName,&statBuf);
		if (statBuf.st_size > maxSize)    //׼���򿪵��ļ�����
		{
			if (!UnionIsLogBackupBeforeRewrite(plogFile))   //����д
			{
				if ((logFilePtr = UnionReWriteLogFileInGrp(fileName)) == NULL)
					return(stderr);
				else
					return(logFilePtr);
			}
			//����д����ѡ��һ���ļ�
			UnionIncreaseCountOfMyLogFile();
			goto reOpenLogFile;
		}
	}
	
	//���ļ�
	if ((logFilePtr = UnionOpenLogFileInGrp(fileName,&curFileSize)) == NULL)
		return(stderr);
		
	
	if (curFileSize < maxSize)
		return(logFilePtr);
	
	//�Ѿ�д��
	UnionCloseLogFileInGrpNoWait(fileName);

	if (!UnionIsLogBackupBeforeRewrite(plogFile))   //����д
	{
		if ((logFilePtr = UnionReWriteLogFileInGrp(fileName)) == NULL)
			return(stderr);
		else
			return(logFilePtr);
	}
	
	//����д����ѡ��һ���ļ�
	UnionIncreaseCountOfMyLogFile();
	
	goto reOpenLogFile;
}

int UnionCloseLogFile(FILE *logFilePtr)
{
	if ((logFilePtr == stderr) || (logFilePtr == stdout))
		return(0);
	/*
	return(fclose(logFilePtr));  //modify by hzh in 2012.11.1
	*/
	UnionCloseLogFileInGrp(gCurrentUsingFileName);
	return 0;
}

//add by hzh in 2012.11.1
int UnionFlushLogFile(FILE *logFilePtr)
{
	if ((logFilePtr == stderr) || (logFilePtr == stdout))
		return(0);
	UnionFlushLogFileInGrp(gCurrentUsingFileName);
	return 0;
}

int UnionFlushLogFileNoWait(FILE *logFilePtr)
{
	if ((logFilePtr == stderr) || (logFilePtr == stdout))
		return(0);
	UnionFlushLogFileInGrpNoWait(gCurrentUsingFileName);
	return 0;
}
//add end 

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
	struct stat statBuf;
	if ((plogFile == NULL) || (logFilePtr == NULL) || (logFilePtr == stderr) || (logFilePtr == stdout))
		return(errCodeParameter);
	else
	{
		//plogFile->currentSize = ftell(logFilePtr);  //modify by hzh in 2012.11.1 Ϊ��������
		if (stat(gCurrentUsingFileName,&statBuf) != 0)
			return(errCodeParameter);
		plogFile->currentSize = statBuf.st_size;
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
	//fflush(fp);
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
	char		fileName[512];
	struct stat statBuf;
	
	if (plogFile == NULL)
		return(errCodeParameter);
		
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/%s.log",getenv("UNIONLOG"),plogFile->name);
	/*
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
	*/ //modify by hzh in 2012.11.1
	if (stat(fileName,&statBuf) != 0)
			return(0);
	plogFile->currentSize = statBuf.st_size;
	return(0);
}

// add by hzh in 20091026 ���ӵĺ���
void UnionSystemErrLogW(char *fileName __attribute__((unused)),char *funName __attribute__((unused)),int line __attribute__((unused)),char *fmt,...)
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
	//fprintf(logFilePtr,"%10s::[OSError][%05d][%06d][%s][%d] fileName[%s]funName[%s]line[%d]::\n",
	//		tmpBuf,getpid(),errno,strerror(errno),gunionUserDefinedErrorCode,fileName,funName,line);
	fprintf(logFilePtr,"%10s::[OSError][%05d][%06d][%s][%d]\n",tmpBuf,getpid(),errno,strerror(errno),gunionUserDefinedErrorCode);
	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	//1fflush(logFilePtr);
	UnionFlushLogFileNoWait(logFilePtr);
	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);
	
	UnionCloseLogFile(logFilePtr);
}

// UserErrLevel
void UnionUserErrLogW(char *fileName __attribute__((unused)),char *funName __attribute__((unused)),int line __attribute__((unused)),char *fmt,...)
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

	//fprintf(logFilePtr,"%10s::[UserErr][%05d][%d] fileName[%s]funName[%s]line[%d]::\n",tmpBuf,getpid(),gunionUserDefinedErrorCode,fileName,funName,line);
	fprintf(logFilePtr,"%10s::[UserErr][%05d][%d]\n",tmpBuf,getpid(),gunionUserDefinedErrorCode);

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	//1fflush(logFilePtr);
	UnionFlushLogFileNoWait(logFilePtr);

	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);

}

void UnionMemErrLogW(char *fileName __attribute__((unused)),char *funName __attribute__((unused)),int line __attribute__((unused)),char *pTitle,unsigned char *pBuffer,int iBufLen)
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

	//fprintf(logFilePtr,"%10s::[UserErr][%05d][%d] fileName[%s]funName[%s]line[%d]::\n",tmpBuf,getpid(),gunionUserDefinedErrorCode,fileName,funName,line);
	fprintf(logFilePtr,"%10s::[UserErr][%05d][%d]\n",tmpBuf,getpid(),gunionUserDefinedErrorCode);
	fprintf(logFilePtr,"%s\n",pTitle);

	WriteMemory(logFilePtr,pBuffer,iBufLen);

	//1fflush(logFilePtr);
	UnionFlushLogFileNoWait(logFilePtr);
	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

// AuditLevel
void UnionSuccessLogW(char *fileName __attribute__((unused)),char *funName __attribute__((unused)),int line __attribute__((unused)),char *fmt,...)
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

	//fprintf(logFilePtr,"%10s::[Audit][%05d] fileName[%s]funName[%s]line[%d]::\n",tmpBuf,getpid(),fileName,funName,line);
	fprintf(logFilePtr,"%10s::[Audit][%05d]\n",tmpBuf,getpid());

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	//fflush(logFilePtr);
	UnionFlushLogFile(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionAuditLogW(char *fileName __attribute__((unused)),char *funName __attribute__((unused)),int line __attribute__((unused)),char *fmt,...)
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

	//fprintf(logFilePtr,"%10s::[Audit][%05d] fileName[%s]funName[%s]line[%d]::\n",tmpBuf,getpid(),fileName,funName,line);
	fprintf(logFilePtr,"%10s::[Audit][%05d]\n",tmpBuf,getpid());

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	//fflush(logFilePtr);
	UnionFlushLogFile(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionAuditNullLogW(char *fileName __attribute__((unused)),char *funName __attribute__((unused)),int line __attribute__((unused)),char *fmt,...)
{
	va_list 	args;
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
	
	if (!UnionIsLogAuditLevel(plogFile = UnionGetLogFile()))
		return;

	logFilePtr = UnionOpenLogFile(plogFile);
	if (logFilePtr == NULL)
		return;

	//fprintf(logFilePtr,"%08d:: fileName[%s]funName[%s]line[%d]::",getpid(),fileName,funName,line);
	fprintf(logFilePtr,"%08d::",getpid());
	
	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	//fflush(logFilePtr);
	UnionFlushLogFile(logFilePtr);

	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionAuditNullLogWithTimeW(char *fileName __attribute__((unused)),char *funName __attribute__((unused)),int line __attribute__((unused)),char *fmt,...)
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
	//fprintf(logFilePtr,"%08d::%10s:: fileName[%s]funName[%s]line[%d]::",getpid(),tmpBuf,fileName,funName,line);
	fprintf(logFilePtr,"%08d::%10s::",getpid(),tmpBuf);

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	//fflush(logFilePtr);
	UnionFlushLogFile(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

// DebugLevel
void UnionLogW(char *fileName __attribute__((unused)),char *funName __attribute__((unused)),int line __attribute__((unused)),char *fmt,...)
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

	//fprintf(logFilePtr,"%10s::[Debug][%05d] fileName[%s]funName[%s]line[%d]::\n",tmpBuf,getpid(),fileName,funName,line);
	fprintf(logFilePtr,"%10s::[Debug][%05d]\n",tmpBuf,getpid());

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	//fflush(logFilePtr);
	UnionFlushLogFile(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionMemLogW(char *fileName __attribute__((unused)),char *funName __attribute__((unused)),int line __attribute__((unused)),char *pTitle,unsigned char *pBuffer,int iBufLen)
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

	//fprintf(logFilePtr,"%10s::[Debug][%05d] fileName[%s]funName[%s]line[%d]::\n",tmpBuf,getpid(),fileName,funName,line);
	fprintf(logFilePtr,"%10s::[Debug][%05d]\n",tmpBuf,getpid());
	fprintf(logFilePtr,"%s\n",pTitle);

	WriteMemory(logFilePtr,pBuffer,iBufLen);

	//fflush(logFilePtr);
	UnionFlushLogFile(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionProgramerLogW(char *fileName __attribute__((unused)),char *funName __attribute__((unused)),int line __attribute__((unused)),char *fmt,...)
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

	//fprintf(logFilePtr,"%10s::[Debug][%05d] fileName[%s]funName[%s]line[%d]::\n",tmpBuf,getpid(),fileName,funName,line);
	fprintf(logFilePtr,"%10s::[Debug][%05d]\n",tmpBuf,getpid());

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	//fflush(logFilePtr);
	UnionFlushLogFile(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionDebugLogW(char *fileName __attribute__((unused)),char *funName __attribute__((unused)),int line __attribute__((unused)),char *fmt,...)
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

	//fprintf(logFilePtr,"%10s::[Debug][%05d] fileName[%s]funName[%s]line[%d]::\n",tmpBuf,getpid(),fileName,funName,line);
	fprintf(logFilePtr,"%10s::[Debug][%05d]\n",tmpBuf,getpid());

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	//fflush(logFilePtr);
	UnionFlushLogFile(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}


void UnionDebugNullLogW(char *fileName __attribute__((unused)),char *funName __attribute__((unused)),int line __attribute__((unused)),char *fmt,...)
{
	va_list 	args;
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
	
	if (!UnionIsLogProgramerLevel(plogFile = UnionGetLogFile()))
		return;

	logFilePtr = UnionOpenLogFile(plogFile);
	if (logFilePtr == NULL)
		return;

	//fprintf(logFilePtr,"%08d:: fileName[%s]funName[%s]line[%d]::",getpid(),fileName,funName,line);
	fprintf(logFilePtr,"%08d::",getpid());
	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	//fflush(logFilePtr);
	UnionFlushLogFile(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionNullLogW(char *fileName __attribute__((unused)),char *funName __attribute__((unused)),int line __attribute__((unused)),char *fmt,...)
{
	va_list 	args;
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
	
	if (!UnionIsLogDebugLevel(plogFile = UnionGetLogFile()))
		return;

	logFilePtr = UnionOpenLogFile(plogFile);
	if (logFilePtr == NULL)
		return;

	//fprintf(logFilePtr,"%08d:: fileName[%s]funName[%s]line[%d]::",getpid(),fileName,funName,line);
	fprintf(logFilePtr,"%08d::",getpid());
	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	//fflush(logFilePtr);
	UnionFlushLogFile(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionMemNullLogWithTimeW(char *fileName __attribute__((unused)),char *funName __attribute__((unused)),int line __attribute__((unused)),unsigned char *pBuffer,int iBufLen)
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
	//fprintf(logFilePtr,"%10s::%08d:: fileName[%s]funName[%s]line[%d]::",tmpBuf,getpid(),fileName,funName,line);
	fprintf(logFilePtr,"%10s::%08d::",tmpBuf,getpid());
	fprintf(logFilePtr,"[%04d][%s]\n",iBufLen*2,tmpBuf);
	//fflush(logFilePtr);
	UnionFlushLogFile(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionNullLogWithTimeW(char *fileName __attribute__((unused)),char *funName __attribute__((unused)),int line __attribute__((unused)),char *fmt,...)
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

	//fprintf(logFilePtr,"%10s::%08d:: fileName[%s]funName[%s]line[%d]::",tmpBuf,getpid(),fileName,funName,line);
	fprintf(logFilePtr,"%10s::%08d::",tmpBuf,getpid());
	//fprintf(logFilePtr,"%10s::",tmpBuf);

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	//fflush(logFilePtr);
	UnionFlushLogFile(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionNullLogWithTimeAnywayW(char *fileName __attribute__((unused)),char *funName __attribute__((unused)),int line __attribute__((unused)),char *fmt,...)
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

	//fprintf(logFilePtr,"%10s::%08d:: fileName[%s]funName[%s]line[%d]::",tmpBuf,getpid(),fileName,funName,line);
	fprintf(logFilePtr,"%10s::%08d::",tmpBuf,getpid());
	//fprintf(logFilePtr,"%10s::",tmpBuf);

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	//fflush(logFilePtr);
	UnionFlushLogFile(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionProgramerMemLogW(char *fileName __attribute__((unused)),char *funName __attribute__((unused)),int line __attribute__((unused)),char *pTitle,unsigned char *pBuffer,int iBufLen)
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

	//fprintf(logFilePtr,"%10s::[Debug][%05d] fileName[%s]funName[%s]line[%d]::\n",tmpBuf,getpid(),fileName,funName,line);
	fprintf(logFilePtr,"%10s::[Debug][%05d]\n",tmpBuf,getpid());
	fprintf(logFilePtr,"%s\n",pTitle);

	WriteMemory(logFilePtr,pBuffer,iBufLen);

	//fflush(logFilePtr); 
	UnionFlushLogFile(logFilePtr);
	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);
}

void UnionRealNullLogW(char *fileName __attribute__((unused)),char *funName __attribute__((unused)),int line __attribute__((unused)),char *fmt,...)
{
	va_list 	args;
	FILE		*logFilePtr;
	PUnionLogFile	plogFile;
	
	if (!UnionIsLogDebugLevel(plogFile = UnionGetLogFile()))
		return;

	logFilePtr = UnionOpenLogFile(plogFile);

	va_start(args,fmt);
	vfprintf(logFilePtr,fmt,args);
	va_end(args);

	fflush(logFilePtr);

	
	UnionGetCurrentSizeOfLogFile(logFilePtr,plogFile);

	UnionCloseLogFile(logFilePtr);

}

