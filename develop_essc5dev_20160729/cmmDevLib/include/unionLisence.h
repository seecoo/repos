//	Wolfgang Wang
//	2004/6/17

#ifndef _UnionLisence_
#define _UnionLisence_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define conMDLNameOfUnionLisence	"UnionLisenceMDL"

typedef struct
{
	// 以下为固定属性
	char	userName[40+1];		// 产品简称
	char	serialNumber[16+1];	// 产品的序列号
	char	lisenceData[16+1];	// 产品的验证码
	long	effectiveDays;		// 产品有效使用日期
	long	lisenceTime;		// 颁布产品最早的使用日期
	// 以下为动态属性
	time_t	registerTime;		// 产品登记的开始使用日期
	char	lisenceCode[16+1];	// 产品的验证码，由上述各项生成
} TUnionLisence;
typedef TUnionLisence	*PUnionLisence;

int UnionGetLisenceKey(char *key);
int UnionGenerateLisenceCode(PUnionLisence plisence,char *lisenceCode);
int UnionCreateLisenceFile();
int UnionVerifyLisenceCode();
int UnionPrintLisence();

int UnionGetFixedLisence(PUnionLisence plisence);

int UnionConnectLisenceModule();
int UnionDisconnectLisenceModule();
int UnionRemoveLisenceModule();
int UnionReloadLisenceModule();
int UnionVerifyLisenceCode();


// 获得版本登记文件的名称
int UnionGetCopyrightRegistryFileName(char *fileName);

/*
功能
	根据输入数据和序列号生成授权码
输入参数
	inputData	输入数据，如果为空指针，则取unionREC.CFG中的nameOfMyself的值
	serialNumber	序列号，如果为空指针，则取unionREC.CFG中的序列号的值
	isVerify	是不是验证
输出参数
	key		生成的授权码
返回值
	>=0		成功，生成的授权码长度
	<0		出错代码
*/
int UnionGenerateLisenceKey(char *inputData,char *serialNumber,char *key,int isVerify);

/*
功能
	根据输入数据和序列号生成授权码
输入参数
	inputData	输入数据，如果为空指针，则取unionREC.CFG中的nameOfMyself的值
	conSerialNumberStr	序列号，如果为空指针，则取unionREC.CFG中的序列号的值
	isVerify	是不是验证
输出参数
	key		生成的授权码
返回值
	>=0		成功，生成的授权码长度
	<0		出错代码
*/
int UnionGenerateLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *key,int isVerify);
/*
功能
	根据输入数据和序列号以及授权码校验授权码
输入参数
	inputData	输入数据，如果为空指针，则取unionREC.CFG中的nameOfMyself的值
	conSerialNumberStr	序列号，如果为空指针，则取unionREC.CFG中的序列号的值
	key		要验校的授权码，如果为空指针，则取unionREC.CFG中的liscenceCode
输出参数
	无
返回值
	>0		校验成功
	=0		校验失败
	<0		出错代码
*/
int UnionVerifyLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *key);

/*
功能
	输入产品数据和序列号以及授权码
输入参数
	inputData	输入数据
	conSerialNumberStr	序列号
	key		要验校的授权码
输出参数
	无
返回值
	>0		校验成功
	=0		校验失败
	<0		出错代码
*/
int UnionInputLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *key);

/*
功能
	存储产品数据和序列号以及授权码
输入参数
	inputData	输入数据
	serialNumber	序列号
	key		要验校的授权码
输出参数
	无
返回值
	>0		校验成功
	=0		校验失败
	<0		出错代码
*/
int UnionStoreLisenceInfo(char *inputData,char *serialNumber,char *key);

/*
功能
	读取产品数据和序列号以及授权码
输入参数
	无
输出参数
	inputData	输入数据
	serialNumber	序列号
	key		要验校的授权码
返回值
	>0		校验成功
	=0		校验失败
	<0		出错代码
*/
int UnionReadLiscenceInfo(char *inputData,char *serialNumber,char *key);

/*
功能
	校验产品授权信息
输入参数
	无
输出参数
	无
返回值
	>0		校验成功
	=0		校验失败
	<0		出错代码
*/
int UnionVerifyLisenceInfo();

/*
功能
	根据输入数据和序列号生成前16位授权码
输入参数
	inputData	输入数据，如果为空指针，则取unionREC.CFG中的nameOfMyself的值
	serialNumber	序列号，如果为空指针，则取unionREC.CFG中的序列号的值
	isVerify	是不是验证
输出参数
	key		生成的授权码
返回值
	>=0		成功，生成的授权码长度
	<0		出错代码
*/
int UnionGenerateFrontLisenceKey(char *inputData,char *serialNumber,char *key,int isVerify);

/*
功能
	根据输入数据和序列号生成后16位授权码
输入参数
	date1	维护到期日期
	date2	停止使用日期
	isVerify	是不是验证
输出参数
	key		生成的授权码
返回值
	>=0		成功，生成的授权码长度
	<0		出错代码
*/
int UnionGenerateAfterLisenceKey(char *date1,char *date2,char *key,int isVerify);

/*
功能
	根据输入数据和序列号生成最终授权码
输入参数
	inputData	输入数据，如果为空指针，则取unionREC.CFG中的nameOfMyself的值
	serialNumber	序列号，如果为空指针，则取unionREC.CFG中的序列号的值
	date1		维护到期日期
	date2		使用到期时期
	isVerify	是不是验证
输出参数
	key		生成的授权码
返回值
	>=0		成功，生成的授权码长度
	<0		出错代码
*/
int UnionGenerateFinalLisenceKey(char *inputData,char *serialNumber,char *date1,char *date2,char *key,int isVerify);

/*
功能
	根据输入数据和序列号生成最终授权码
输入参数
	inputData	输入数据，如果为空指针，则取unionREC.CFG中的nameOfMyself的值
	serialNumber	序列号，如果为空指针，则取unionREC.CFG中的序列号的值
	date1		维护到期日期，如果为空指针，则取00000000的值
	date2		使用到期日期，如果为空指针，则取00000000的值
	isVerify	是不是验证
输出参数
	key		生成的授权码
返回值
	>=0		成功，生成的授权码长度
	<0		出错代码
*/
int UnionGenerateFinalLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *date1,char *date2,char *key,int isVerify);

/*
功能
	根据输入数据和序列号生成前16授权码
输入参数
	inputData	输入数据，如果为空指针，则取unionREC.CFG中的nameOfMyself的值
	serialNumber	序列号，如果为空指针，则取unionREC.CFG中的序列号的值
	isVerify	是不是验证
输出参数
	key		生成的授权码
返回值
	>=0		成功，生成的授权码长度
	<0		出错代码
*/
int UnionGenerateFrontLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *key,int isVerify);

/*
功能
	根据输入数据和序列号以及授权码校验授权码
输入参数
	inputData	输入数据，如果为空指针，则取unionREC.CFG中的nameOfMyself的值
	serialNumber	序列号，如果为空指针，则取unionREC.CFG中的序列号的值
	key		要验校的授权码，如果为空指针，则取unionREC.CFG中的liscenceCode
输出参数
	date1		维护到期日期
	date2		使用到期日期
返回值
	=2		校验成功
	=1		到维护期
	=0		校验失败
	=-1		到截止使用日期
	<-1		出错代码
*/
int UnionVerifyFinalLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *date1,char *date2,char *key);

/*
功能
	输入产品数据和序列号以及授权码
输入参数
	inputData	输入数据
	serialNumber	序列号
	key		要验校的授权码
输出参数
	date1		维护到期日期
	date2		使用到期日期
返回值
	>0		校验成功
	=0		校验失败
	<0		出错代码
*/
int UnionInputFinalLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *date1,char *date2,char *key);

/*
功能
	存储产品数据和序列号以及授权码
输入参数
	inputData	输入数据
	serialNumber	序列号
	key		要验校的授权码
输出参数
	无
返回值
	>0		校验成功
	=0		校验失败
	<0		出错代码
*/
int UnionStoreFinalLisenceInfo(char *inputData,char *serialNumber,char *key);

/*
功能
	读取产品数据和序列号以及授权码
输入参数
	无
输出参数
	inputData	输入数据
	serialNumber	序列号
	key		要验校的授权码
返回值
	>0		校验成功
	=0		校验失败
	<0		出错代码
*/
int UnionReadFinalLiscenceInfo(char *inputData,char *serialNumber,char *key);

/*
功能
	校验产品授权信息
输入参数
	无
输出参数
	无
返回值
	=2		校验成功
	=1		到维护期
	=0		校验失败
	=-1		到截止使用日期
	<-1		出错代码
*/
int UnionVerifyFinalLisenceInfo();

#endif
