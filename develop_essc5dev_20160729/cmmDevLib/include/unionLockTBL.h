// Wolfgang Wang
// 2008/11/03

#ifndef _unionLockTBL_
#define _unionLockTBL_

#include <stdio.h>

typedef struct
{
	char		resName[40+1];	// ��Դ����
	char		keyWord[256+1];	// �ؼ���,�ؼ���Ϊ��ʱ������Դ����
	unsigned long	readingLocksNum;	// ������Ŀ
	unsigned long	isWritingLocked;	// �Ƿ�д��ס�ˣ����ǣ�����
	char		writingTime[14+1];	// д��ʱ��
	char		readingTime[14+1];	// ���һ�ζ���ʱ��
} TUnionLockRec;
typedef TUnionLockRec	*PUnionLockRec;

#define conMaxNumOfLock	128
typedef struct
{
	//int		realNum;
	int		maxNum;
	TUnionLockRec	lockGrp[conMaxNumOfLock];
} TUnionLockTBL;
typedef TUnionLockTBL	*PUnionLockTBL;

#define conMDLNameOfLockTBL	"unionLockTBLMDL"

// ��ʾ������
/*
�������
	resID		�ϲ�Ӧ��Ϊ��������id��
�������
	��
����ֵ
	>=0		����Ŀ
	<0		������
*/
int UnionSpierAllLock(int resID);

// �ж������Ƿ�������
/*
�������
	��
�������
	��
����ֵ
	1	������
	0	δ����
*/
int UnionIsLockTBLConnected();

// ����������Ŀ
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionGetMaxNumOfLock();

// ��������
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionConnectLockTBL();

	
// �Ͽ������������
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionDisconnectLockTBL();

// ɾ������
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionRemoveLockTBL();

// ��װ����
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionReloadLockTBL();

// ��������
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionResetLockTBL();

// �ж�һ�����Ƿ�����Դ����
/*
�������
	prec		����¼
�������
	��
����ֵ
	1		��
	0		��
*/
int UnionIsResLevelLockedRec(PUnionLockRec prec);

// �ж�һ�����Ƿ��Ǽ�¼����
/*
�������
	prec		����¼
�������
	��
����ֵ
	1		��
	0		��
*/
int UnionIsRecLevelLockedRec(PUnionLockRec prec);

// �ж�һ�����Ƿ��ǺϷ���
/*
�������
	prec		����¼
�������
	��
����ֵ
	1		��
	0		��
*/
int UnionIsValidLockedRec(PUnionLockRec prec);

// ��ӡһ����
/*
�������
	prec		����¼ָ��
�������
	fp		�ļ����
����ֵ
	>=0		��ӡ��������
	<0		������
*/
int UnionPrintLockRecToFile(PUnionLockRec prec,FILE *fp);

// ��ӡָ����Դ�ļ�¼����
/*
�������
	resName		��Դ����
�������
	fp		�ļ����
����ֵ
	>=0		����Ŀ
	<0		������
*/
int UnionPrintAllRecLockOfSpecResToFile(char *resName,FILE *fp);

// ͳ��ָ����Դ����������¼��
/*
�������
	resName		��Դ����
�������
	fp		�ļ����
����ֵ
	>=0		����Ŀ
	<0		������
*/
int UnionCountAllRecLockOfSpecRes(char *resName,int *writingLockRecNum,int *readingLockRecNum);

// ��ӡ������Դ������
/*
�������
	��
�������
	fp		�ļ����
����ֵ
	>=0		����Ŀ
	<0		������
*/
int UnionPrintAllResLockToFile(FILE *fp);

// ��ӡ������
/*
�������
	��
�������
	fp		�ļ����
����ֵ
	>=0		����Ŀ
	<0		������
*/
int UnionPrintAllLockToFile(FILE *fp);

// ����ָ��������¼
/*
�������
	resName		��Դ����
	keyWord		�ؼ���
�������
	firstNullPos	��õĵ�һ����λ��
����ֵ
	�ɹ�		����¼ָ��
	ʧ��		��ָ��
*/
PUnionLockRec UnionFindSpecLockRec(char *resName,char *keyWord,int *firstNullPos);

// �ж�һ����Դ�Ƿ�д��ס��
/*
�������
	resName		��Դ����
�������
	��
����ֵ
	1		��
	0		��
*/
int UnionIsSpecResLocked(char *resName);

// �ж�һ��λ���Ƿ��ǺϷ�λ��
/*
�������
	pos		����¼λ��
�������
	��
����ֵ
	1		��
	0		��
*/
int UnionIsValidLockRecPos(int pos);

// ����һ����
/*
�������
	resName		��¼����
	keyWord		�ؼ���
	isWritingLock	������������д�������������Ƕ���
	timeout		������ʱ�ĳ�ʱ��>=0����ʱ��<0�������ó�ʱ
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionApplyLockWithTimeout(char *resName,char *keyWord,int isWritingLock,int timeout);

// �ͷ�һ����¼��
/*
�������
	resName		��¼����
	keyWord		�ؼ���
	isWritingLock	������������д�������������Ƕ�����<0 �������ж���
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionFreeLock(char *resName,char *keyWord,int isWritingLock);


// ������Դд��
/*
�������
	resName		��Դ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionApplyResWritingLockOfSpecRes(char *resName);

// ������Դ����
/*
�������
	resName		��Դ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionApplyResReadingLockOfSpecRes(char *resName);

// ��������ʽ������Դд��
/*
�������
	resName		��Դ����
	timeout		��ʱ
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionApplyResWritingLockOfSpecResWithTimeout(char *resName,int timeout);

// ��������ʽ������Դ����
/*
�������
	resName		��Դ����
	timeout		��ʱ
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionApplyResReadingLockOfSpecResWithTimeout(char *resName,int timeout);

// �ͷ���Դд��
/*
�������
	resName		��Դ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionFreeResWritingLockOfSpecRes(char *resName);

// �ͷ���Դ����
/*
�������
	resName		��Դ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionFreeResReadingLockOfSpecRes(char *resName);

// �ͷ�ָ����Դ�����ж���
/*
�������
	resName		��Դ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionFreeAllResReadingLockOfSpecRes(char *resName);


// �����¼д��
/*
�������
	resName		��¼����
	keyWord		�ؼ���
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionApplyRecWritingLockOfSpecRec(char *resName,char *keyWord);

// �����¼����
/*
�������
	resName		��¼����
	keyWord		�ؼ���
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionApplyRecReadingLockOfSpecRec(char *resName,char *keyWord);

// ��������ʽ�����¼д��
/*
�������
	resName		��¼����
	keyWord		�ؼ���
	timeout		��ʱ
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionApplyRecWritingLockOfSpecRecWithTimeout(char *resName,char *keyWord,int timeout);

// ��������ʽ�����¼����
/*
�������
	resName		��¼����
	keyWord		�ؼ���
	timeout		��ʱ
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionApplyRecReadingLockOfSpecRecWithTimeout(char *resName,char *keyWord,int timeout);

// �ͷż�¼д��
/*
�������
	resName		��¼����
	keyWord		�ؼ���
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionFreeRecWritingLockOfSpecRec(char *resName,char *keyWord);

// �ͷż�¼����
/*
�������
	resName		��¼����
	keyWord		�ؼ���
�������
	��
����ֵ
	>=0		�ɹ�
	<0		������
*/
int UnionFreeRecReadingLockOfSpecRec(char *resName,char *keyWord);

// ά��������,�Զ��ͷű���ס����ָ��ʱ�����
/*
�������
	maxLockedSeconds	����ס�����ʱ��
�������
	��
����ֵ
	>=0		�ɹ�,�������������
	<0		�������
*/
int UnionMaintainingAllDeadWritingLocks(int maxLockedSeconds);

// wangk add 2009-9-25
int UnionPrintLockRecInRecStrFormatToFile(PUnionLockRec prec,FILE *fp);

int UnionPrintAllLockInRecStrFormatToFile(FILE *fp);

int UnionPrintLockRecInRecStrFormatToSpecFile(char *fileName);
// wangk add end 2009-9-25

// add by wangk 2009-12-09
int UnionFreeAllWriteLock();

int UnionFreeAllReadLock();

int UnionFreeAllResWriteLock();

int UnionFreeAllResReadLock();

int UnionFreeAllRecWriteLock();

int UnionFreeAllRecReadLock();
// add end wangk 2009-12-09

#endif
