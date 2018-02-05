//	Wolfgang Wang
//	2008/10/08

#ifndef _unionGenSvrFunUsingCmmPack_
#define _unionGenSvrFunUsingCmmPack_

#include "unionDatagramDef.h"

/*
功能	
	拼装函数名称
输入参数
	appID		应用用途
	datagramID	报文标识
输入出数
	funName		函数名称
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateSvrCmmPackFunNameOfService(char *appID,char *datagramID,char *funName);

/*
功能	
	拼装函数名称
输入参数
	appID		应用用途
	datagramID	报文标识
输入出数
	funName		函数名称
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateSvrCmmPackFunNameOfService(char *appID,char *datagramID,char *funName);

/*
功能	
	拼装程序名称
输入参数
	appID		应用用途
	datagramID	报文标识
	version		程序版本
输入出数
	programName	程序全名名称
	incFileConf	配置头文件的文件名称
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateSvrCmmPackProgramNameOfService(char *appID,char *datagramID,char *version,char *incFileConf,char *programName);

/*
功能	
	产生一个对报文域进行赋值的函数
输入参数
	funName		函数名称
	pdef		报文指针
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateSvrCmmPackFldSetCodes(char *funName,PUnionDatagramDef pdef,FILE *fp);

/*
功能	
	产生一个从报文域读值代码的函数
输入参数
	funName		函数名称
	pdef		报文指针
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateSvrCmmPackFldReadCodes(char *funName,PUnionDatagramDef pdef,FILE *fp);

/*
功能	
	产生变量声明
输入参数
	pdef		报文指针
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateSvrCmmPackVarDeclarionCodes(PUnionDatagramDef pdef,FILE *fp);

/*
功能	
	产生与服务器通讯的代码
输入参数
	fp		文件句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateNullCmmPackFunCodes(FILE *fp);

/*
功能	
	生成一个服务端函数
输入参数
	appID		应用用途
	datagramID	报文标识
	incConfFileName	生成的函数要使用的头文件
	fp		文件名柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateSvrFunUsingCmmPackToFp(char *appID,char *datagramID,char *incConfFileName,FILE *fp);

/*
功能	
	生成一个服务端函数
输入参数
	appID		应用用途
	datagramID	报文标识
	version		程序版本
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateSvrFunUsingCmmPack(char *appID,char *datagramID,char *version);

#endif

