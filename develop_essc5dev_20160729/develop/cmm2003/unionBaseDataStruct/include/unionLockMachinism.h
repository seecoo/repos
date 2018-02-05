// Wolfgang Wang
// 2008/11/03

#ifndef _unionLockMachinism_
#define _unionLockMachinism_

// 打开关键字锁机制
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionOpenKeyWordLockMachinism();

// 关闭关键字锁机制
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionCloseKeyWordLockMachinism();

// 对一个关键字加锁
/*
输入参数
	key		关键字
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionLockKeyWord(char *key);

// 对一个关键字解锁
/*
输入参数
	key		关键字
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionUnlockKeyWord(char *key);

// 对所有关键字解锁
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0，解锁的数量
	失败		<0,错误码
*/
int UnionUnlockAllKeyWord();

// 显示所有锁住了的关键字
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionPrintAllLockedKeyWordToFile(FILE *fp);


#endif
