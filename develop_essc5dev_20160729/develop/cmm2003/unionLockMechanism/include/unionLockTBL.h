// Wolfgang Wang
// 2008/11/03

#ifndef _unionLockTBL_
#define _unionLockTBL_

#include <stdio.h>

typedef struct
{
	char		resName[40+1];	// 资源名称
	char		keyWord[256+1];	// 关键字,关键字为空时，是资源锁。
	unsigned long	readingLocksNum;	// 读锁数目
	unsigned long	isWritingLocked;	// 是否被写锁住了，１是，０否
	char		writingTime[14+1];	// 写锁时间
	char		readingTime[14+1];	// 最后一次读锁时间
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

// 显示所有锁
/*
输入参数
	resID		上层应用为锁表分配的id号
输出参数
	无
返回值
	>=0		锁数目
	<0		错误码
*/
int UnionSpierAllLock(int resID);

// 判断锁表是否已连接
/*
输入参数
	无
输出参数
	无
返回值
	1	已连接
	0	未连接
*/
int UnionIsLockTBLConnected();

// 获得最大锁数目
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionGetMaxNumOfLock();

// 连接锁表
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionConnectLockTBL();

	
// 断开与锁表的连接
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionDisconnectLockTBL();

// 删除锁表
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionRemoveLockTBL();

// 重装锁表
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionReloadLockTBL();

// 重置锁表
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionResetLockTBL();

// 判断一个锁是否是资源级锁
/*
输入参数
	prec		锁记录
输出参数
	无
返回值
	1		是
	0		否
*/
int UnionIsResLevelLockedRec(PUnionLockRec prec);

// 判断一个锁是否是记录级锁
/*
输入参数
	prec		锁记录
输出参数
	无
返回值
	1		是
	0		否
*/
int UnionIsRecLevelLockedRec(PUnionLockRec prec);

// 判断一个锁是否是合法锁
/*
输入参数
	prec		锁记录
输出参数
	无
返回值
	1		是
	0		否
*/
int UnionIsValidLockedRec(PUnionLockRec prec);

// 打印一个锁
/*
输入参数
	prec		锁记录指针
输出参数
	fp		文件句柄
返回值
	>=0		打印的锁数量
	<0		错误码
*/
int UnionPrintLockRecToFile(PUnionLockRec prec,FILE *fp);

// 打印指定资源的记录级锁
/*
输入参数
	resName		资源名称
输出参数
	fp		文件句柄
返回值
	>=0		锁数目
	<0		错误码
*/
int UnionPrintAllRecLockOfSpecResToFile(char *resName,FILE *fp);

// 统计指定资源的所有锁记录数
/*
输入参数
	resName		资源名称
输出参数
	fp		文件句柄
返回值
	>=0		锁数目
	<0		错误码
*/
int UnionCountAllRecLockOfSpecRes(char *resName,int *writingLockRecNum,int *readingLockRecNum);

// 打印所有资源级的锁
/*
输入参数
	无
输出参数
	fp		文件句柄
返回值
	>=0		锁数目
	<0		错误码
*/
int UnionPrintAllResLockToFile(FILE *fp);

// 打印所有锁
/*
输入参数
	无
输出参数
	fp		文件句柄
返回值
	>=0		锁数目
	<0		错误码
*/
int UnionPrintAllLockToFile(FILE *fp);

// 查找指定的锁记录
/*
输入参数
	resName		资源名称
	keyWord		关键字
输出参数
	firstNullPos	获得的第一个空位置
返回值
	成功		锁记录指针
	失败		空指针
*/
PUnionLockRec UnionFindSpecLockRec(char *resName,char *keyWord,int *firstNullPos);

// 判断一个资源是否被写锁住了
/*
输入参数
	resName		资源名称
输出参数
	无
返回值
	1		是
	0		否
*/
int UnionIsSpecResLocked(char *resName);

// 判断一个位置是否是合法位置
/*
输入参数
	pos		锁记录位置
输出参数
	无
返回值
	1		是
	0		否
*/
int UnionIsValidLockRecPos(int pos);

// 申请一个锁
/*
输入参数
	resName		记录名称
	keyWord		关键字
	isWritingLock	１，操作的是写锁；０操作的是读锁
	timeout		申请锁时的超时，>=0，超时，<0，不设置超时
输出参数
	无
返回值
	>=0		成功
	<0		错误码
*/
int UnionApplyLockWithTimeout(char *resName,char *keyWord,int isWritingLock,int timeout);

// 释放一个记录锁
/*
输入参数
	resName		记录名称
	keyWord		关键字
	isWritingLock	１，操作的是写锁；０操作的是读锁，<0 操作所有读锁
输出参数
	无
返回值
	>=0		成功
	<0		错误码
*/
int UnionFreeLock(char *resName,char *keyWord,int isWritingLock);


// 申请资源写锁
/*
输入参数
	resName		资源名称
输出参数
	无
返回值
	>=0		成功
	<0		错误码
*/
int UnionApplyResWritingLockOfSpecRes(char *resName);

// 申请资源读锁
/*
输入参数
	resName		资源名称
输出参数
	无
返回值
	>=0		成功
	<0		错误码
*/
int UnionApplyResReadingLockOfSpecRes(char *resName);

// 非阻塞方式申请资源写锁
/*
输入参数
	resName		资源名称
	timeout		超时
输出参数
	无
返回值
	>=0		成功
	<0		错误码
*/
int UnionApplyResWritingLockOfSpecResWithTimeout(char *resName,int timeout);

// 非阻塞方式申请资源读锁
/*
输入参数
	resName		资源名称
	timeout		超时
输出参数
	无
返回值
	>=0		成功
	<0		错误码
*/
int UnionApplyResReadingLockOfSpecResWithTimeout(char *resName,int timeout);

// 释放资源写锁
/*
输入参数
	resName		资源名称
输出参数
	无
返回值
	>=0		成功
	<0		错误码
*/
int UnionFreeResWritingLockOfSpecRes(char *resName);

// 释放资源读锁
/*
输入参数
	resName		资源名称
输出参数
	无
返回值
	>=0		成功
	<0		错误码
*/
int UnionFreeResReadingLockOfSpecRes(char *resName);

// 释放指定资源的所有读锁
/*
输入参数
	resName		资源名称
输出参数
	无
返回值
	>=0		成功
	<0		错误码
*/
int UnionFreeAllResReadingLockOfSpecRes(char *resName);


// 申请记录写锁
/*
输入参数
	resName		记录名称
	keyWord		关键字
输出参数
	无
返回值
	>=0		成功
	<0		错误码
*/
int UnionApplyRecWritingLockOfSpecRec(char *resName,char *keyWord);

// 申请记录读锁
/*
输入参数
	resName		记录名称
	keyWord		关键字
输出参数
	无
返回值
	>=0		成功
	<0		错误码
*/
int UnionApplyRecReadingLockOfSpecRec(char *resName,char *keyWord);

// 非阻塞方式申请记录写锁
/*
输入参数
	resName		记录名称
	keyWord		关键字
	timeout		超时
输出参数
	无
返回值
	>=0		成功
	<0		错误码
*/
int UnionApplyRecWritingLockOfSpecRecWithTimeout(char *resName,char *keyWord,int timeout);

// 非阻塞方式申请记录读锁
/*
输入参数
	resName		记录名称
	keyWord		关键字
	timeout		超时
输出参数
	无
返回值
	>=0		成功
	<0		错误码
*/
int UnionApplyRecReadingLockOfSpecRecWithTimeout(char *resName,char *keyWord,int timeout);

// 释放记录写锁
/*
输入参数
	resName		记录名称
	keyWord		关键字
输出参数
	无
返回值
	>=0		成功
	<0		错误码
*/
int UnionFreeRecWritingLockOfSpecRec(char *resName,char *keyWord);

// 释放记录读锁
/*
输入参数
	resName		记录名称
	keyWord		关键字
输出参数
	无
返回值
	>=0		成功
	<0		错误码
*/
int UnionFreeRecReadingLockOfSpecRec(char *resName,char *keyWord);

// 维护所有锁,自动释放被锁住超过指定时间的锁
/*
输入参数
	maxLockedSeconds	被锁住的最大时间
输出参数
	无
返回值
	>=0		成功,解除的死锁数量
	<0		出错代码
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
