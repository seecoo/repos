// Author:	Wolfgang Wang
// Date:	2003/09/24

// Version 2.x 2003/09/24,Wolfgang Wang,��unionKeyDBInMemory.h�ĳ�

// 2004/11/20, �������뱸�ݷ�������صĺ���

// 2004/11/25������Ϊ 3.0��ʹ�� _UnionDesKey_3_x_ �汾��DesKey�ṹ
/*
	�����˼�������	UnionUpdateDesKeyValueInKeyDB();
			UnionUseDesKeyFromKeyDB();
*/

#ifndef _UnionDesKeyDB_
#define _UnionDesKeyDB_

#include "unionErrCode.h"
#include "unionDesKey.h"

#include "keyDBBackuper.h"

#ifdef _UnionDesKeyDB_2_x_

#define _UnionLogMDL_3_x_
#include "UnionLog.h"

#define errDesKeyNotEffective		-101


#define conMDLNameOfUnionDesKeyDB	"UnionDesKeyDBMDL"

typedef struct
{
	char		lastUpdatedTime[14+1];	// �������ʱ��
	long		num;			// ��Կ����
	long		maxKeyNum;		// ������Կ����
#ifdef _UnionDesKey_4_x_
	short		isWritingLocked;	// ��д��ס��
	short		readingLocks;		// ����д��ס��
#endif
	PUnionDesKey	pkey;			// ��Կ��;
} TUnionDesKeyDB;
typedef TUnionDesKeyDB		*PUnionDesKeyDB;


// added by wolfgang wang,20070611
int UnionGetReadingLocksOfDesKeyDB();
int UnionGetWritingLocksOfDesKeyDB();
int UnionResetReadingLocksOfDesKeyDB();
int UnionResetWritingLocksOfDesKeyDB();
// 20070611 ���ӽ���

// Added by Wolfgang Wang,2007/5/16
int UnionSetProgramControlDesKeyDB();
int UnionFreeProgramControlDesKeyDB();
int UnionIsProgramControlDesKeyDB();

// Added by Wolfgang Wang, 2004/11/25
int UnionMirrorDesKeyDBIntoDiskArtifically();
int UnionUpdateDesKeyValueInKeyDB(char *fullName,char *value,char *checkValue); 	
int UnionUpdateDesKeyAttrInKeyDB(PUnionDesKey pkey);
int UnionUpdateAllDesKeyFieldsInKeyDB(PUnionDesKey pkey); 	
int UnionUseDesKeyFromKeyDB(PUnionDesKey pkey);
int UnionPrintDesKeyNearOutofMaxUseTimesToFile(long times,FILE *fp);
int UnionExistDesKeyInKeyDB(char *fullName);
int UnionIsDesKeyDBConnected();
int UnionPrintDesKeyOfContainerInKeyDBToFile(char *container,FILE *fp);
int UnionPrintDesKeyInKeyDBUsedTimesToFile(long times,FILE *fp);
int UnionPrintDesKeyInKeyDBUsedDaysToFile(long days,FILE *fp);

// End of addition of 2004/11/25 by Wolfgang Wang

int UnionGetNameOfDesKeyDB(char *fileName);

long UnionGetMaxKeyNumFromKeyDBFile();


int UnionConnectDesKeyDB();
int UnionDisconnectDesKeyDB();
int UnionRemoveDesKeyDBInMemory();
int UnionDeleteDesKeyDB();
int UnionCreateDesKeyDB(int maxKeyNum);

int UnionLoadDesKeyDBIntoMemory();
int UnionPrintDesKeyDB();

int UnionPrintAllDesKeysInKeyDBToFile(FILE *fp);
int UnionPrintStatusOfDesKeyDBToFile(FILE *fp);
int UnionPrintDesKeyExpectedToFile(char *app,char *owner,char *name,FILE *fp);

int UnionInsertDesKeyIntoKeyDB(PUnionDesKey pkey);
int UnionDeleteDesKeyFromKeyDB(char *fullName);
int UnionReadDesKeyFromKeyDB(PUnionDesKey pkey);
int UnionReadDesKeyFromKeyDBAnyway(PUnionDesKey pkey);
long UnionFindDesKeyPosInKeyDB(char *fullName);
int UnionUpdateDesKeyInKeyDB(PUnionDesKey pkey);
int UnionMirrorDesKeyDBIntoDisk(PUnionDesKeyDB pdesKeyDB,PUnionDesKey pkey);

#ifdef _UnionDesKey_4_x_
int UnionPrintDesKeyDBToFile(FILE *fp);
int UnionPrintDesKeyExpectedInKeyDBToFile(char *app,char *owner,char *name,FILE *fp);
int UnionPrintDesKeyGroupInKeyDBToFile(PUnionDesKeyQueryCondition pcondition,FILE *fp);
int UnionPrintDesKeyGroupInKeyDBToSpecFile(PUnionDesKeyQueryCondition pcondition,char *fileName);
int UnionPrintDesKeyInKeyDBOutofDateToFile(FILE *fp);
int UnionPrintDesKeyInKeyDBNearOutofDateToFile(long days,FILE *fp);
int UnionPrintDesKeyInKeyDBNearOutofMaxUseTimesToFile(long times,FILE *fp);
int UnionUnlockDesKeyDB();
int UnionUnlockAllLockedDesKeyInDesKeyDB();
int UnionUnlockDesKeyInDesKeyDB(char *keyName);
int UnionPrintAllLockedDesKeyInDesKeyDBToFile(FILE *fp);
#else
int UnionPrintDesKeyDBToFile(PUnionDesKeyDB pdesKeyDB,FILE *fp);
int UnionPrintDesKeyExpectedInKeyDBToFile(PUnionDesKeyDB pdesKeyDB,char *app,char *owner,char *name,FILE *fp);
int UnionPrintDesKeyInKeyDBOutofDateToFile(PUnionDesKeyDB pdesKeyDB,FILE *fp);
int UnionPrintDesKeyInKeyDBNearOutofDateToFile(PUnionDesKeyDB pdesKeyDB,long days,FILE *fp);
int UnionPrintDesKeyInKeyDBNearOutofMaxUseTimesToFile(PUnionDesKeyDB pdesKeyDB,long times,FILE *fp);
#endif

int UnionGetKeyNumOfKeyDB();
char *UnionGetDesKeyDBLastUpdatedTime();

int UnionChangeSizeOfDesKeyDB(long nexMaxKeyNum);

int UnionPrintDesKeyOutofDateToFile(FILE *fp);
int UnionPrintDesKeyNearOutofDateToFile(long days,FILE *fp);

// Added by Wolfgang Wang, 2004/11/20
int UnionSynchronizeSpecifiedDesKeyToBackuper(char *fullName);
long UnionSynchronizeAllDesKeyToBackuper();
long UnionSynchronizeDesKeyOfSpecifiedAppToBackuper(char *appName);
int UnionDesKeyDBOperation(TUnionKeyDBOperation operation,PUnionDesKey pdesKey);
// End Of Addition Of 2004/11/20

// 2006/08/14�����˺���
// ����д����
int UnionWritingLockDesKeyTBL();
// ���д����
int UnionWritingUnlockDesKeyTBL();
// ���������
int UnionReadingLockDesKeyTBL();
// ���������
int UnionReadingUnlockDesKeyTBL();
// ����д��ס��¼
int UnionWritingLockDesKey(PUnionDesKey pkey);
// ������д��ס��¼
int UnionWritingUnlockDesKey(PUnionDesKey pkey);
// �������ס��¼
int UnionReadingLockDesKey(PUnionDesKey pkey);
// ����������ס��¼
int UnionReadingUnlockDesKey(PUnionDesKey pkey);

// ����Կ���л�ȡ��Կ����
int UnionGetContainerOfSpecKey(char *keyName,char *container);
int UnionGetCurrentActiveDesKeyNumInKeyDB();
PUnionDesKey UnionGetCurrentActiveDesKeyTBL();
PUnionDesKeyDB UnionGetCurrentActiveDesKeyDB();

int UnionDeleteAllDesKeyOfApp(char *app);

// 2007/11/29, Added by Wolfgang Wang
long UnionCompareAllDesKeyValueInKeyDBWithBrothers();

// 2007/12/11���ӣ��Զ��򱸷ݷ�����ͬ����һ�µ���Կ
int UnionAutoSynchronizeDesKeyToBrothers();

// 2008/7/18,������
int UnionPrintAllDesKeyToRecStrFile(FILE *fp);

// 2008/7/18,������
int UnionOutputAllDesKeyToRecStrFile(char *fileName);

// 2008/7/18,������
int UnionInputAllDesKeyFromRecStrFile(char *fileName);

#endif // ifdef _UnionDesKeyDBInMemory_2_x_

#endif
