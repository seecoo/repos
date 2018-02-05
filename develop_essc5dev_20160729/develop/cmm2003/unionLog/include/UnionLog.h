// Author:	Wolfgang Wang
// Date:	2002/01/18

/*
	2003/09/19,Wolfgang Wang
	增加了_UnionLogMDL_3_x_一段
	
	3.x版的日志模块操作一个名为：$HOME/etc/unionLogTBL.CFG的文件
	该文件中定义了每个日志的属性，每个日志的属性按如下方式定义
		[日志文件名]	[写日志的级别]		[日志的最大尺寸]	[满重写前备份]
		日志的最大尺寸以M计，如果为10，表示10M。
		满重写前备份 = 1，表示备份。0，表示不备份。
		写日志的级别：
			0，表示不写日志
			1，只写操作系统错误
			2，写程序错误
			3，审计级
			4，调试级
			5，编程级
			6，所有信息写日志
		该文件中还定义了以下两个变量：
			MaxNumOfLogFile			定义了日志文件的最大数目
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

// 计算一个时间距离当前时间的间隔
/*
输入参数：
	firstTime	时间
输出参数:
	seconds		间隔，假设当前时间为currentTime,计算的间隔为currentTime-fisrtTime
返回值
	>=0		成功
	<0		出错代码
*/
int UnionCalcuSecondsPassedAfterSpecTime(char *firstTime,long *seconds);

// 2010/5/15,王纯军增加
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

// 日志的级别由高到低降序排列，级别高的自动具有级别低的权限。
// conLogNoneLevel具有最高权限，conLogAlwaysLevel具有最低权限。
typedef enum 
{
	conLogNoneLogLevel = 0,	// 不写任何日志信息	
	conLogSystemErrLevel = 1,	// 调用系统函数出错级
	conLogUserErrLevel = 2,	// 程序自定义的错误级
	conLogAuditLevel = 3,	// 审计信息级		缺省级别
	conLogDebugLevel = 4,	// 程序调试级
	conLogProgramerLevel = 5,	// 程序员级，程序写的调试信息
	conLogAlwaysLevel = 6	// 总写日志级
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

// 日志满后的写策略
typedef enum
{
	conLogFullRewriteAtonce,	// 日志满立即重写，		缺省值
	conLogFullRewriteAfterBackup,	// 再重写日志前备份日志
	conLogNewEveryDay = 11,		// 每天一个新日志，日志名称加上日期，日志满后备份
	conLogNewEveryMonth = 21,	// 每月一个新日志，日志名称加上月份，日志满后备份
	conLogNewEverySeason = 31,	// 每季一个新日志，日志名称加上季度，日志满后备份
	conLogNewHalfYear = 41,		// 每季一个新日志，日志名称加上半年，日志满后备份
	conLogNewEveryYear = 51		// 每年一个新日志，日志名称加上半年，日志满后备份
} TUnionLogFullRewriteStrategy;
typedef TUnionLogFullRewriteStrategy	*PUnionLogFullRewriteStrategy;	

// 日志文件
typedef struct
{
	char		name[40+1];	// 最多40个字符的log文件名，不带后缀，不带路径
	TUnionLogLevel	level;		// 日志级别	
#ifdef	_LOG_FILE_NUM_
	int		logFileNum;
#endif
	long		maxSize;	// 日志最大尺寸
	TUnionLogFullRewriteStrategy	rewriteAttr;	// 日志满的写属性
	long		currentSize;	// 日志当前尺寸
	int		users;		// 日志使用者数目
} TUnionLogFile;
typedef TUnionLogFile		*PUnionLogFile;

// 定义日志模块
typedef struct
{
	int		maxLogFileNum;		// 最多可有的日志版本
	PUnionLogFile	plogFile;		// 日志文件列表
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

// 2006/08/08增加
PUnionLogFile UnionGetCurrentLogFileTBL();
// 2006/08/08增加
int UnionGetCurrentMaxNumOfLogFile();

// 2006/08/11 增加
int UnionSetUserDefinedErrorCode(int errCode);

// 2010/10/30 增加
void *UnionSetUserDefinedErrorCodeReturnNullPointer(int errCode);

// 2009/08/11 增加
int UnionGetUserDefinedErrorCode();

// 20060825 增加
int UnionExistLogFile(char *logFileName);

// 20060825 增加
int UnionExistLogFileDef(char *name);

// 20060825 增加
int UnionInsertLogFileDef(char *name,TUnionLogLevel level,long maxSize,TUnionLogFullRewriteStrategy rewriteAttr);

// 20060825 增加
int UnionDeleteLogFileDef(char *name);

// 20060825 增加
int UnionUpdateLogFileDef(char *name,TUnionLogLevel level,long maxSize,TUnionLogFullRewriteStrategy rewriteAttr);

// 20060825增加
PUnionLogFile UnionFindLogFile(char *logFileName);

// 20060825 增加
int UnionResetLogFileUsers(char *logFileName);
int UnionPrintLogFileTBLToSpecFile(char *fileName);

// 2006/11/15增加
void UnionMerelyNullLog(char *fmt,...);

// 2006/11/20增加
FILE *UnionOpenSpecLogFile(char *fileName);

// 2006/12/11增加
void UnionMemAuditNullLog(unsigned char *pBuffer,int iBufLen);

// wangk add 2009-9-24
int UnionPrintLogFileTBLInRecStrFormatToFile(FILE *fp);

int UnionPrintLogFileTBLInRecStrFormatToSpecFile(char *fileName);

int UnionPrintLogFileInRecStrFormatToFile(PUnionLogFile plogFile,FILE *fp);
// wangk add end 2009-9-24

#endif	// for _UnionLogMDL_3_x_

// 判断一个时间串是否合法
/*
输入参数：
	timeStr		时间采用HHMMSS
输入参数
	无
返回值
	1		合法
	0		不合法
*/
int UnionIsValidTimeStr(char *timeStr);

// 将一个时间串拼分成小时，分、秒
/*
输入参数：
	timeStr		时间采用HHMMSS
输入参数
	hour		小时
	minute		分
	second		秒
返回值
	>=0		成功
	<0		出错代码
*/
int UnionAnalysisTime(char *timeStr,int *hour,int *minute,int *second);
	
// 计算两个时间的时间间隔
/*
输入参数：
	firstTime	第一个时间
	secondTime	第二个时间
	时间采用HHMMSS
输入参数
	seconds		间隔，以秒计
返回值
	>=0		成功
	<0		出错代码
*/

// afterTime，输出参数：YYYYMMDDHHMMSS
int UnionCalcuSecondsBetweenTwoTime(char *firstTime,char *secondTime,long *seconds);

// 计算两个时间的时间间隔
/*
输入参数：
	firstTime	第一个时间
	secondTime	第二个时间
	时间采用YYMMDDHHMMSS
输入参数
	seconds		间隔，以秒计
返回值
	>=0		成功
	<0		出错代码
*/

// afterTime，输出参数：YYYYMMDDHHMMSS
int UnionCalcuSecondsBetweenTwoFullSystemDateTime(char *firstTime,char *secondTime,long *seconds);

// 计算指定日期过后几日是哪一天。
// specDate:YYYYMMDD，输入参数
// snapDays是过后的天数，输入参数
// afterDate::YYYYMMDD,输出参数
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

//设置fflush方式. 缺省(mode=0)为每写一次即fflush
void UnionSetFlushLogFileLinesMode(int mode);

//取fflush方式
int UnionGetFlushLogFileLinesMode();

//fflush日志文件组中的所有已打开的日志文件
void UnionFlushAllLogFileInGrpNoWait();

// 日志缓存，缓存开始
void UnionSetMultiLogBegin();

// 日志缓存，缓存结束
void UnionSetMultiLogEnd();

void UnionDestroyILOG();

int UnionDeleteILOG(char *name);

#endif
