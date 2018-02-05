// Wolfgang Wang
// 2008/11/03

#ifndef _unionOsLockMachinism_
#define _unionOsLockMachinism_

// 打开系统互斥机制
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionOpenOSLockMachinism();

// 关闭系统互斥机制
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionCloseOSLockMachinism();

// 申请一个系统互斥
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionApplyOsLock();

// 释入系统互斥
/*
输入参数
	无
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionFreeOsLock();

#endif
