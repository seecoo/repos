// Wolfgang Wang
// 2008/11/03

#ifndef _unionOsLockMachinism_
#define _unionOsLockMachinism_

#define  SEM_KEY  100168

#ifndef _WIN32_
typedef union
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
} UUnionSemun;
typedef UUnionSemun  *PUnionSemnu;
#endif

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
	resName	资源名称
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionApplyOsLock(char *resName);

// 释入系统互斥
/*
输入参数
	resName	资源名称
输出参数
	无
返回值
	成功		>=0
	失败		<0,错误码
*/
int UnionFreeOsLock(char *resName);

#endif
