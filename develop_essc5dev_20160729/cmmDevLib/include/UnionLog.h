// Author:	Wolfgang Wang
// Date:	2002/01/18

/*
	2003/09/19,Wolfgang Wang
	������_UnionLogMDL_3_x_һ��
	
	3.x�����־ģ�����һ����Ϊ��$HOME/etc/unionLogTBL.CFG���ļ�
	���ļ��ж�����ÿ����־�����ԣ�ÿ����־�����԰����·�ʽ����
		[��־�ļ���]	[д��־�ļ���]		[��־�����ߴ�]	[����дǰ����]
		��־�����ߴ���M�ƣ����Ϊ10����ʾ10M��
		����дǰ���� = 1����ʾ���ݡ�0����ʾ�����ݡ�
		д��־�ļ���
			0����ʾ��д��־
			1��ֻд����ϵͳ����
			2��д�������
			3����Ƽ�
			4�����Լ�
			5����̼�
			6��������Ϣд��־
		���ļ��л���������������������
			MaxNumOfLogFile			��������־�ļ��������Ŀ
*/

#ifndef _UnionLogMDL
#define _UnionLogMDL

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <zlog.h>
#include <LOGS.h>

#define gUnionErrMsgIssureID		"00000000"
#define gUnionErrMsgIssureName		"Union Error Message Issure"

char *UnionGetCurrentFullSystemDateTime();

char *UnionGetCurrentFullSystemDate();

char *UnionGetCurrentSystemTime();

int UnionGetSystemTime(char *strtime);
int UnionGetSystemDate(char *strdate);
int UnionGetSystemYear(char *stryear);
int UnionGetFullSystemDate(char *strdate);
int UnionGetSystemDateTime(char *datetime);
int UnionGetFullSystemDateTime(char *datetime);
int UnionGetTimeBeforeSpecTime(char *specTime, long seconds,char *beforeTime);

/*
void UnionDebugNullLog(char *fmt,...);
void UnionAuditNullLog(char *fmt,...);
void UnionAuditNullLogWithTime(char *fmt,...);
void UnionNullLogWithTime(char *fmt,...);
*/

// ����һ��ʱ����뵱ǰʱ��ļ��
/*
���������
	firstTime	ʱ��
�������:
	seconds		��������赱ǰʱ��ΪcurrentTime,����ļ��ΪcurrentTime-fisrtTime
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionCalcuSecondsPassedAfterSpecTime(char *firstTime,long *seconds);

// 2010/5/15,����������
void UnionSetUserSpecLogFileName(char *fileName);

#ifndef _UnionLogMDL_3_x_
// The two following functions must be defined outside the UnionLogMDL.
int UnionGetNameOfLogFile(char *NameOfLogFile);
long UnionGetSizeOfLogFile();
// *************************************************************

// Anytime before calling functions of the UnionLogMDL the first time,
// please call this function to connect to the UnionLogMDL.

// Anytime when not calling functions of the UnionLogMDL any more,
// please call this function to disconnect to the UnionLogMDL.
int UnionDisconnectLogMDL();

FILE *UnionOpenLogFile();
#endif

int UnionConnectLogMDL();
int UnionConnectLogMDLAnyway();

int UnionCloseLogFile(FILE *UnionLogfile);

void WriteMemory(FILE *fp,unsigned char *pBuffer,int iBufLen);	//
/*
void UnionSystemErrLog(char *,...);				// SystemErrLevel
void UnionUserErrLog(char *,...);				// UserErrLevel
void UnionSuccessLog(char *,...);				// AuditLevel
void UnionLog(char *,...);					// AuditLevel
void UnionAuditLog(char *,...);		                        // AuditLevel
void UnionNoTimeLog(char *,...);				// DebugLevel
void UnionMemErrLog(char *pTitle,unsigned char *pBuffer,int iBufLen);	// UserErrLevel
void UnionMemLog(char *pTitle,unsigned char *pBuffer,int iBufLen);	// DebugLevel
void UnionNullLog(char *,...);					// DebugLevel
*/

void UnionPrintf(char *,...);					// AlwaysLevel
long UnionDecideDaysBeforeToday(char *date);
long UnionDecideDaysAfterToday(char *date);

// Added by Wolfgang Wang, 2003/09/19
#ifdef _UnionLogMDL_3_x_	

#include "unionErrCode.h"

#define conMDLNameOfUnionLogTBL		"UnionLogTBLMDL"

// ��־�ļ����ɸߵ��ͽ������У�����ߵ��Զ����м���͵�Ȩ�ޡ�
// conLogNoneLevel�������Ȩ�ޣ�conLogAlwaysLevel�������Ȩ�ޡ�
typedef enum 
{
	conLogNoneLogLevel = 0,	// ��д�κ���־��Ϣ	
	conLogSystemErrLevel = 1,	// ����ϵͳ��������
	conLogUserErrLevel = 2,	// �����Զ���Ĵ���
	conLogAuditLevel = 3,	// �����Ϣ��		ȱʡ����
	conLogDebugLevel = 4,	// ������Լ�
	conLogProgramerLevel = 5,	// ����Ա��������д�ĵ�����Ϣ
	conLogAlwaysLevel = 6	// ��д��־��
} TUnionLogLevel;
typedef TUnionLogLevel		*PUnionLogLevel;

enum
{
	//DEBUG = 20, LOG_DEBUG
	//INFO = 40, LOG_INFO
	//NOTICE = 60, LOG_NOTICE
	//WARN = 80, LOG_WARNING
	//ERROR = 100, LOG_ERR
	//FATAL = 120, LOG_ALERT
	//UNKNOWN = 254, LOG_ERR

	ZLOG_LEVEL_NONELOG = 1,
	ZLOG_LEVEL_PGM = 10,
	//ZLOG_LEVEL_DEBUG = 20,
	//ZLOG_LEVEL_AUDIT = 30,
	ZLOG_LEVEL_USERERR = 110,
	ZLOG_LEVEL_OSERR = 130
};

// ��־�����д����
typedef enum
{
	conLogFullRewriteAtonce,	// ��־��������д��		ȱʡֵ
	conLogFullRewriteAfterBackup,	// ����д��־ǰ������־
	conLogNewEveryDay = 11,		// ÿ��һ������־����־���Ƽ������ڣ���־���󱸷�
	conLogNewEveryMonth = 21,	// ÿ��һ������־����־���Ƽ����·ݣ���־���󱸷�
	conLogNewEverySeason = 31,	// ÿ��һ������־����־���Ƽ��ϼ��ȣ���־���󱸷�
	conLogNewHalfYear = 41,		// ÿ��һ������־����־���Ƽ��ϰ��꣬��־���󱸷�
	conLogNewEveryYear = 51		// ÿ��һ������־����־���Ƽ��ϰ��꣬��־���󱸷�
} TUnionLogFullRewriteStrategy;
typedef TUnionLogFullRewriteStrategy	*PUnionLogFullRewriteStrategy;	

// ��־�ļ�
typedef struct
{
	char		name[40+1];	// ���40���ַ���log�ļ�����������׺������·��
	TUnionLogLevel	level;		// ��־����	
#ifdef	_LOG_FILE_NUM_
	int		logFileNum;
#endif
	long		maxSize;	// ��־���ߴ�
	TUnionLogFullRewriteStrategy	rewriteAttr;	// ��־����д����
	long		currentSize;	// ��־��ǰ�ߴ�
	int		users;		// ��־ʹ������Ŀ
} TUnionLogFile;
typedef TUnionLogFile		*PUnionLogFile;

// ������־ģ��
typedef struct
{
	int		maxLogFileNum;		// �����е���־�汾
	PUnionLogFile	plogFile;		// ��־�ļ��б�
} TUnionLogFileTBL;
typedef TUnionLogFileTBL	*PUnionLogFileTBL;

int UnionConnectLogFileTBL();
int UnionDisconnectLogFileTBL();
int UnionRemoveLogFileTBL();
int UnionReloadLogFileTBL();
int UnionPrintLogFileTBL();

int UnionAddLogFile(char *logFileName);
int UnionDeleteLogFile(char *logFileName);

PUnionLogFile UnionConnectLogFile(char *logFileName);
int UnionDisconnectLogFile(PUnionLogFile plogFile);

int UnionIsLogSystemErrLevel(PUnionLogFile plogFile);
int UnionIsLogUserErrLevel(PUnionLogFile plogFile);
int UnionIsLogAuditLevel(PUnionLogFile plogFile);
int UnionIsLogDebugLevel(PUnionLogFile plogFile);
int UnionIsLogProgramerLevel(PUnionLogFile plogFile);
int UnionIsLogAlwaysLevel(PUnionLogFile plogFile);
int UnionIsLogBackupBeforeRewrite(PUnionLogFile plogFile);

/*
void UnionAuditLog(char *,...);		// AuditLevel
void UnionDebugLog(char *,...);		// DebugLevel
void UnionProgramerLog(char *fmt,...);
void UnionProgramerMemLog(char *pTitle,unsigned char *pBuffer,int iBufLen);
*/

FILE *UnionOpenLogFile(PUnionLogFile plogFile);

int UnionReadLogFileDef(char *logFileName,PUnionLogFile plogFile);
int UnionPrintLogFileToFile(PUnionLogFile plogFile,FILE *fp);
int UnionPrintLogFile(PUnionLogFile plogFile);

int UnionGetNameOfLogFileTBL(char *fileName);
int UnionGetMaxNumOfLogFile();
int UnionCalSizeOfLogFile(PUnionLogFile plogFile);
int UnionPrintLogFileTBLToFile(FILE *fp);

int UnionGetSuffixForLogFile(TUnionLogFullRewriteStrategy rewriteAttr,char *suffix);
int UnionGetNameOfMyLogFile(PUnionLogFile plogFile,char *fileName);
void UnionIncreaseCountOfMyLogFile();

int UnionSetSuffixOfMyLogFile(char *logFileSuffix);	// Added by Wolfgang Wang,20051009
int UnionCloseSuffixOfMyLogFile();
void UnionDynamicSetLogLevel(TUnionLogLevel level);

// 2006/08/08����
PUnionLogFile UnionGetCurrentLogFileTBL();
// 2006/08/08����
int UnionGetCurrentMaxNumOfLogFile();

// 2006/08/11 ����
int UnionSetUserDefinedErrorCode(int errCode);

// 2010/10/30 ����
void *UnionSetUserDefinedErrorCodeReturnNullPointer(int errCode);

// 2009/08/11 ����
int UnionGetUserDefinedErrorCode();

// 20060825 ����
int UnionExistLogFile(char *logFileName);

// 20060825 ����
int UnionExistLogFileDef(char *name);

// 20060825 ����
int UnionInsertLogFileDef(char *name,TUnionLogLevel level,long maxSize,TUnionLogFullRewriteStrategy rewriteAttr);

// 20060825 ����
int UnionDeleteLogFileDef(char *name);

// 20060825 ����
int UnionUpdateLogFileDef(char *name,TUnionLogLevel level,long maxSize,TUnionLogFullRewriteStrategy rewriteAttr);

// 20060825����
PUnionLogFile UnionFindLogFile(char *logFileName);

// 20060825 ����
int UnionResetLogFileUsers(char *logFileName);
int UnionPrintLogFileTBLToSpecFile(char *fileName);

// 2006/11/15����
void UnionMerelyNullLog(char *fmt,...);

// 2006/11/20����
FILE *UnionOpenSpecLogFile(char *fileName);

// 2006/12/11����
void UnionMemAuditNullLog(unsigned char *pBuffer,int iBufLen);

// wangk add 2009-9-24
int UnionPrintLogFileTBLInRecStrFormatToFile(FILE *fp);

int UnionPrintLogFileTBLInRecStrFormatToSpecFile(char *fileName);

int UnionPrintLogFileInRecStrFormatToFile(PUnionLogFile plogFile,FILE *fp);
// wangk add end 2009-9-24

#endif	// for _UnionLogMDL_3_x_

// �ж�һ��ʱ�䴮�Ƿ�Ϸ�
/*
���������
	timeStr		ʱ�����HHMMSS
�������
	��
����ֵ
	1		�Ϸ�
	0		���Ϸ�
*/
int UnionIsValidTimeStr(char *timeStr);

// ��һ��ʱ�䴮ƴ�ֳ�Сʱ���֡���
/*
���������
	timeStr		ʱ�����HHMMSS
�������
	hour		Сʱ
	minute		��
	second		��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionAnalysisTime(char *timeStr,int *hour,int *minute,int *second);
	
// ��������ʱ���ʱ����
/*
���������
	firstTime	��һ��ʱ��
	secondTime	�ڶ���ʱ��
	ʱ�����HHMMSS
�������
	seconds		����������
����ֵ
	>=0		�ɹ�
	<0		�������
*/

// afterTime�����������YYYYMMDDHHMMSS
int UnionCalcuSecondsBetweenTwoTime(char *firstTime,char *secondTime,long *seconds);

// ��������ʱ���ʱ����
/*
���������
	firstTime	��һ��ʱ��
	secondTime	�ڶ���ʱ��
	ʱ�����YYMMDDHHMMSS
�������
	seconds		����������
����ֵ
	>=0		�ɹ�
	<0		�������
*/

// afterTime�����������YYYYMMDDHHMMSS
int UnionCalcuSecondsBetweenTwoFullSystemDateTime(char *firstTime,char *secondTime,long *seconds);

// ����ָ�����ڹ���������һ�졣
// specDate:YYYYMMDD���������
// snapDays�ǹ�����������������
// afterDate::YYYYMMDD,�������
int UnionDecideDateAfterSpecDate(char *specDate,int snapDays,char *afterDate);

int UnionGetDateBeforSpecDays(int days, char *date);

# if defined __GNUC__
#   define UNIONLOG_CHECK_PRINTF(m,n) __attribute__((format(printf,m,n)))
# else
#   define UNIONLOG_CHECK_PRINTF(m,n)
# endif

// add by hzh in 2009.10.26
void UnionSystemErrLogW(char *fileName,char *funName,int line,char *fmt,...) UNIONLOG_CHECK_PRINTF(4,5);
void UnionUserErrLogW(char *fileName,char *funName,int line,char *fmt,...) UNIONLOG_CHECK_PRINTF(4,5);
void UnionMemErrLogW(char *fileName,char *funName,int line,char *pTitle,unsigned char *pBuffer,int iBufLen);
void UnionSuccessLogW(char *fileName,char *funName,int line,char *fmt,...) UNIONLOG_CHECK_PRINTF(4,5);
void UnionAuditLogW(char *fileName,char *funName,int line,char *fmt,...) UNIONLOG_CHECK_PRINTF(4,5);
void UnionAuditNullLogW(char *fileName,char *funName,int line,char *fmt,...) UNIONLOG_CHECK_PRINTF(4,5);
void UnionAuditNullLogWithTimeW(char *fileName,char *funName,int line,char *fmt,...) UNIONLOG_CHECK_PRINTF(4,5);
void UnionLogW(char *fileName,char *funName,int line,char *fmt,...) UNIONLOG_CHECK_PRINTF(4,5);
void UnionMemLogW(char *fileName,char *funName,int line,char *pTitle,unsigned char *pBuffer,int iBufLen);
void UnionProgramerLogW(char *fileName,char *funName,int line,char *fmt,...) UNIONLOG_CHECK_PRINTF(4,5);
void UnionDebugLogW(char *fileName,char *funName,int line,char *fmt,...) UNIONLOG_CHECK_PRINTF(4,5);
void UnionDebugNullLogW(char *fileName,char *funName,int line,char *fmt,...) UNIONLOG_CHECK_PRINTF(4,5);
void UnionNullLogW(char *fileName,char *funName,int line,char *fmt,...) UNIONLOG_CHECK_PRINTF(4,5);
void UnionMemNullLogWithTimeW(char *fileName,char *funName,int line,unsigned char *pBuffer,int iBufLen);
void UnionNullLogWithTimeW(char *fileName,char *funName,int line,char *fmt,...) UNIONLOG_CHECK_PRINTF(4,5);
void UnionNullLogWithTimeAnywayW(char *fileName,char *funName,int line,char *fmt,...) UNIONLOG_CHECK_PRINTF(4,5);
void UnionProgramerMemLogW(char *fileName,char *funName,int line,char *pTitle,unsigned char *pBuffer,int iBufLen);
void UnionRealNullLogW(char *fileName,char *funName,int line,char *fmt,...) UNIONLOG_CHECK_PRINTF(4,5);

#define UnionSystemErrLogD(...)	UnionSystemErrLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionUserErrLogD(...)  UnionUserErrLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionMemErrLogD(pTitle,pBuffer,iBufLen) UnionMemErrLogW(__FILE__,(char *)__FUNCTION__,__LINE__,pTitle,pBuffer,iBufLen)
#define UnionSuccessLogD(...)	UnionSuccessLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionAuditLogD(...)	UnionAuditLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionAuditNullLogD(...)	UnionAuditNullLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionAuditNullLogWithTimeD(...)	UnionAuditNullLogWithTimeW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionLogD(...)		UnionLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionMemLogD(pTitle,pBuffer,iBufLen)	UnionMemLogW(__FILE__,(char *)__FUNCTION__,__LINE__,pTitle,pBuffer,iBufLen)
#define UnionProgramerLogD(...)	UnionProgramerLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionDebugLogD(...)	UnionDebugLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionDebugNullLogD(...)	UnionDebugNullLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionNullLogD(...)	UnionNullLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionMemNullLogWithTimeD(pBuffer,iBufLen) UnionMemNullLogWithTimeW(__FILE__,(char *)__FUNCTION__,__LINE__,pBuffer,iBufLen)
#define UnionNullLogWithTimeD(...)	UnionNullLogWithTimeW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionNullLogWithTimeAnywayD(...)	UnionNullLogWithTimeAnywayW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionProgramerMemLogD(pTitle,pBuffer,iBufLen) UnionProgramerMemLogW(__FILE__,(char *)__FUNCTION__,__LINE__,pTitle,pBuffer,iBufLen)

#define UnionSystemErrLog(...)	UnionSystemErrLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionUserErrLog(...)  UnionUserErrLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionMemErrLog(pTitle,pBuffer,iBufLen) UnionMemErrLogW(__FILE__,(char *)__FUNCTION__,__LINE__,pTitle,pBuffer,iBufLen)
#define UnionSuccessLog(...)	UnionSuccessLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionAuditLog(...)	UnionAuditLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionAuditNullLog(...)	UnionAuditNullLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionRealNullLog(...)	UnionRealNullLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionAuditNullLogWithTime(...)	UnionAuditNullLogWithTimeW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionLog(...)		UnionLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionMemLog(pTitle,pBuffer,iBufLen)	UnionMemLogW(__FILE__,(char *)__FUNCTION__,__LINE__,pTitle,pBuffer,iBufLen)
#define UnionProgramerLog(...)	UnionProgramerLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionDebugLog(...)	UnionDebugLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionDebugNullLog(...)	UnionDebugNullLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionNullLog(...)	UnionNullLogW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionMemNullLogWithTime(pBuffer,iBufLen) UnionMemNullLogWithTimeW(__FILE__,(char *)__FUNCTION__,__LINE__,pBuffer,iBufLen)
#define UnionNullLogWithTime(...)	UnionNullLogWithTimeW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionNullLogWithTimeAnyway(...)	UnionNullLogWithTimeAnywayW(__FILE__,(char *)__FUNCTION__,__LINE__,__VA_ARGS__)
#define UnionProgramerMemLog(pTitle,pBuffer,iBufLen) UnionProgramerMemLogW(__FILE__,(char *)__FUNCTION__,__LINE__,pTitle,pBuffer,iBufLen)

int UnionGetCategoryNameOfZlog(char *logFileName,char *categoryName);

int UnionSetSuffixAndMDCOfMyLogFile(char *mdc,char *logFileSuffix);

int UnionZlogWriteRules(int logFileNum[]);

//����fflush��ʽ. ȱʡ(mode=0)Ϊÿдһ�μ�fflush
void UnionSetFlushLogFileLinesMode(int mode);

//ȡfflush��ʽ
int UnionGetFlushLogFileLinesMode();

//fflush��־�ļ����е������Ѵ򿪵���־�ļ�
void UnionFlushAllLogFileInGrpNoWait();

// ��־���棬���濪ʼ
void UnionSetMultiLogBegin();

// ��־���棬�������
void UnionSetMultiLogEnd();

void UnionDestroyILOG();

int UnionDeleteILOG(char *name);

#endif
