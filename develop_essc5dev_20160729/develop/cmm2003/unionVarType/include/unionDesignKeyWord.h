//	Wolfgang Wang
//	2008/10/6

#ifndef _unionDesignKeyWord_
#define _unionDesignKeyWord_

// 常量定义标识
#define conDesginKeyWordTagConst		1
// 函数定义标识
#define conDesginKeyWordTagFun			10
// 全局变量定义标识
#define conDesginKeyWordTagGlobalVar		11
// 类型定义的偏移，从该偏移保留50个给类型定义用
#define conDesginKeyWordTagVarTypeOffset	100

/*
功能	
	获得一个函数所在程序的全名
输入参数
	funName		函数名称
输入出数
	fullProgramName	程序的全名
	incFileName	程序应包括的头文件配置
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGetFullProgramFileNameOfFun(char *funName,char *fullProgramName,char *incFileName);

/*
功能	
	显示一个类型的定义
输入参数
	typeTag		指定的类型
	keyWord		类型名称
	fp		文件句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionDisplayKeyWordDefOfSpecTypeToFp(int typeTag,char *keyWord,FILE *fp);

/*
功能	
	显示一个类型的定义
输入参数
	keyWord		类型名称
	fp		文件句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionDisplayKeyWordDefToFp(char *keyWord,FILE *fp);

/*
功能	
	显示一个类型的定义
输入参数
	keyWord		类型名称
	fileName	将一个类型定义写入到文件中
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionOutputKeyWordDefToSpecFile(char *keyWord,char *fileName);

/*
功能	
	显示一个类型的所有定义
输入参数
	nameOfType	类型名称
	fp		文件句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionDisplayAllDefOfSpecKeyWordToFp(char *keyWord,FILE *fp);

#endif
