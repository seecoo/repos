// Author:	Wolfgang Wang
// Date:	2004/11/11

// 2004/11/25，升级为 3.0，使用 _UnionPK_3_x_ 版本的PK结构
/*
	增加了几个函数	UnionUpdatePKValueInKeyDB();
			UnionUsePKFromKeyDB();
*/


#ifndef _UnionPKDB_
#define _UnionPKDB_

#include "unionPK.h"
#include "keyDBBackuper.h"

#define _UnionLogMDL_3_x_
#include "UnionLog.h"

#define errPKNotEffective		-101

#define conMDLNameOfUnionPKDB	"UnionPKDBMDL"

typedef struct
{
	char		lastUpdatedTime[14+1];	// 最近更新时间
	long		num;			// 密钥数量
	long		maxKeyNum;		// 最大的密钥数量
#ifdef _UnionPK_4_x_
	short		isWritingLocked;	// 表被写锁住了
	short		readingLocks;	// 表被读写锁住了
#endif
	PUnionPK	pkey;			// 密钥表;
} TUnionPKDB;
typedef TUnionPKDB	*PUnionPKDB;

// added by wolfgang wang,20070611
int UnionGetReadingLocksOfPKDB();
int UnionGetWritingLocksOfPKDB();
int UnionResetReadingLocksOfPKDB();
int UnionResetWritingLocksOfPKDB();
// 20070611 增加结束

// Added by Wolfgang Wang, 2004/11/25
int UnionMirrorPKDBIntoDiskArtifically();
int UnionUpdatePKValueInKeyDB(char *fullName,char *value,char *checkValue); 	
int UnionUpdatePKAttrInKeyDB(PUnionPK pkey);
int UnionUpdateAllPKFieldsInKeyDB(PUnionPK pkey); 	
int UnionUsePKFromKeyDB(PUnionPK pkey);
int UnionExistPKInKeyDB(char *fullName);
// End of addition of 2004/11/25 by Wolfgang Wang

int UnionGetNameOfPKDB(char *fileName);

int UnionGetMaxKeyNumFromPKDBFile();


int UnionConnectPKDB();
int UnionDisconnectPKDB();
int UnionRemovePKDBInMemory();
int UnionDeletePKDB();
int UnionCreatePKDB(int maxKeyNum);

int UnionLoadPKDBIntoMemory();
int UnionPrintPKDB();
int UnionMirrorPKDBIntoDisk(PUnionPKDB pkeyDB,PUnionPK pkey);
int UnionPrintAllPKsInKeyDBToFile(FILE *fp);
int UnionPrintStatusOfPKDBToFile(FILE *fp);

int UnionInsertPKIntoKeyDB(PUnionPK pkey);
int UnionDeletePKFromKeyDB(char *fullName);
int UnionReadPKFromKeyDB(PUnionPK pkey);
int UnionReadPKFromKeyDBAnyway(PUnionPK pkey);
long UnionFindPKPosInKeyDB(char *fullName);
int UnionUpdatePKInKeyDB(PUnionPK pkey);

int UnionGetPKNumOfKeyDB();
char *UnionGetPKDBLastUpdatedTime();

int UnionChangeSizeOfPKDB(long nexMaxKeyNum);

#ifdef _UnionPK_4_x_
int UnionPrintPKNearOutofMaxUseTimesToFile(long times,FILE *fp);
int UnionPrintPKInKeyDBNearOutofMaxUseTimesToFile(long times,FILE *fp);
int UnionPrintPKDBToFile(FILE *fp);
int UnionPrintPKExpectedInKeyDBToFile(char *app,char *owner,char *name,FILE *fp);
int UnionPrintPKExpectedToFile(char *app,char *owner,char *name,FILE *fp);
int UnionPrintPKInKeyDBOutofDateToFile(FILE *fp);
int UnionPrintPKOutofDateToFile(FILE *fp);
int UnionPrintPKInKeyDBNearOutofDateToFile(long days,FILE *fp);
int UnionPrintPKNearOutofDateToFile(long days,FILE *fp);
int UnionPrintPKOfContainerInKeyDBToFile(char *container,FILE *fp);
int UnionUnlockPKDB();
int UnionUnlockAllLockedPKInPKDB();
int UnionUnlockPKInPKDB(char *keyName);
int UnionPrintAllLockedPKInPKDBToFile(FILE *fp);
#else
int UnionPrintPKNearOutofMaxUseTimesToFile(long times,FILE *fp);
int UnionPrintPKInKeyDBNearOutofMaxUseTimesToFile(PUnionPKDB pdesKeyDB,long times,FILE *fp);
int UnionPrintPKDBToFile(PUnionPKDB pkeyDB,FILE *fp);
int UnionPrintPKExpectedInKeyDBToFile(PUnionPKDB pkeyDB,char *app,char *owner,char *name,FILE *fp);
int UnionPrintPKExpectedToFile(char *app,char *owner,char *name,FILE *fp);
int UnionPrintPKInKeyDBOutofDateToFile(PUnionPKDB pkeyDB,FILE *fp);
int UnionPrintPKOutofDateToFile(FILE *fp);
int UnionPrintPKInKeyDBNearOutofDateToFile(PUnionPKDB pkeyDB,long days,FILE *fp);
int UnionPrintPKNearOutofDateToFile(long days,FILE *fp);
int UnionPrintPKOfContainerInKeyDBToFile(char *container,FILE *fp);
#endif

// Added by Wolfgang Wang, 2004/11/20
int UnionSynchronizeSpecifiedPKToBackuper(char *fullName);
long UnionSynchronizeAllPKToBackuper();
long UnionSynchronizePKOfSpecifiedAppToBackuper(char *appName);
int UnionPKDBOperation(TUnionKeyDBOperation operation,PUnionPK pkey);
// End Of Addition Of 2004/11/20

// 2006/08/14增加了函数
// 申请写锁表
int UnionWritingLockPKTBL();
// 解除写锁表
int UnionWritingUnlockPKTBL();
// 申请读锁表
int UnionReadingLockPKTBL();
// 解除读锁表
int UnionReadingUnlockPKTBL();
// 申请写锁住记录
int UnionWritingLockPK(PUnionPK pkey);
// 申请解除写锁住记录
int UnionWritingUnlockPK(PUnionPK pkey);
// 申请读锁住记录
int UnionReadingLockPK(PUnionPK pkey);
// 申请解除读锁住记录
int UnionReadingUnlockPK(PUnionPK pkey);

int UnionGetCurrentActivePKNumInKeyDB();
PUnionPK UnionGetCurrentActivePKTBL();
PUnionPKDB UnionGetCurrentActivePKDB();
// 从密钥库中获取密钥容器
int UnionGetContainerOfSpecPK(char *keyName,char *container);

// add by chenliang, 2009-01-09

// 2008/7/18,王纯军
int UnionPrintAllPKToRecStrFile(FILE *fp);

// 2008/7/18,王纯军
int UnionOutputAllPKToRecStrFile(char *fileName);

// add end.

#endif
