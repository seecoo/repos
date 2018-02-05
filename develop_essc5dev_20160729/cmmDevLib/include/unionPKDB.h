// Author:	Wolfgang Wang
// Date:	2004/11/11

// 2004/11/25������Ϊ 3.0��ʹ�� _UnionPK_3_x_ �汾��PK�ṹ
/*
	�����˼�������	UnionUpdatePKValueInKeyDB();
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
	char		lastUpdatedTime[14+1];	// �������ʱ��
	long		num;			// ��Կ����
	long		maxKeyNum;		// ������Կ����
#ifdef _UnionPK_4_x_
	short		isWritingLocked;	// ��д��ס��
	short		readingLocks;	// ����д��ס��
#endif
	PUnionPK	pkey;			// ��Կ��;
} TUnionPKDB;
typedef TUnionPKDB	*PUnionPKDB;

// added by wolfgang wang,20070611
int UnionGetReadingLocksOfPKDB();
int UnionGetWritingLocksOfPKDB();
int UnionResetReadingLocksOfPKDB();
int UnionResetWritingLocksOfPKDB();
// 20070611 ���ӽ���

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

// 2006/08/14�����˺���
// ����д����
int UnionWritingLockPKTBL();
// ���д����
int UnionWritingUnlockPKTBL();
// ���������
int UnionReadingLockPKTBL();
// ���������
int UnionReadingUnlockPKTBL();
// ����д��ס��¼
int UnionWritingLockPK(PUnionPK pkey);
// ������д��ס��¼
int UnionWritingUnlockPK(PUnionPK pkey);
// �������ס��¼
int UnionReadingLockPK(PUnionPK pkey);
// ����������ס��¼
int UnionReadingUnlockPK(PUnionPK pkey);

int UnionGetCurrentActivePKNumInKeyDB();
PUnionPK UnionGetCurrentActivePKTBL();
PUnionPKDB UnionGetCurrentActivePKDB();
// ����Կ���л�ȡ��Կ����
int UnionGetContainerOfSpecPK(char *keyName,char *container);

// add by chenliang, 2009-01-09

// 2008/7/18,������
int UnionPrintAllPKToRecStrFile(FILE *fp);

// 2008/7/18,������
int UnionOutputAllPKToRecStrFile(char *fileName);

// add end.

#endif
