//	Wolfgang Wang
//	2008/10/08

#ifndef _unionGenClientFunUsingCmmPack_
#define _unionGenClientFunUsingCmmPack_

#include "unionPackAndFunRelation.h"

/*
功能	
	设置请求报文和响应报文变量名称
输入参数
	isClientFun	标识是否是客户端函数
输入出数
	putFldIntVarName	请求报文名称
	reaFldFromVarName	响应报文名称
返回值
	>=0		成功
	<0		出错代码
*/
void UnionSetCmmPackReqAndResVarName(int isClientFun,char *putFldIntVarName,char *reaFldFromVarName);

/*
功能	
	读取一个函数的输入参数的值类型
输入参数
	funName		函数名称
	varValueTag	值类型
	inputOrOutput	是读输入还是输出，1是输入参数，2是输出参数，3是返回值
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadTypeOfSpecVarValueTag(char *funName,char *varValueTag,int inputOrOutput);

/*
功能	
	产生一段对报文域进行赋值的代码
输入参数
	isClientFun	标识是否是客户端函数
	funName		函数名称
	errExit		出错退出的断点
	ppackAndFunDef	域赋值表
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateCmmPackSetCodes(int isClientFun,char *funName,char *errExit,PUnionPackAndFunRelation ppackAndFunDef,FILE *fp);

/*
功能	
	产生一个对报文域进行赋值的函数
输入参数
	isClientFun	标识是否是客户端函数
	funName		函数名称
	errExit		出错退出的断点
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateCmmPackFldSetCodesForFun(int isClientFun,char *funName,char *errExit,FILE *fp);

/*
功能	
	产生一个从报文域读值代码的代码
输入参数
	isClientFun	标识是否是客户端函数
	funName		函数名称
	errExit		出错退出的断点
	ppackAndFunDef	域赋值表
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateCmmPackReadCodes(int isClientFun,char *funName,char *errExit,PUnionPackAndFunRelation ppackAndFunDef,FILE *fp);

/*
功能	
	产生一个从报文域读值代码的函数
输入参数
	isClientFun	标识是否是客户端函数
	funName		函数名称
	errExit		出错退出的断点
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateCmmPackFldReadCodesForFun(int isClientFun,char *funName,char *errExit,FILE *fp);

/*
功能	
	产生变量声明
输入参数
	fp		文件句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateCmmPackAndFunVarDeclareCodes(FILE *fp);

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
int UnionGenerateCommWithCmmPackSvrCodes(char *funName,FILE *fp);

/*
功能	
	生成一个客户端函数
输入参数
	funName		函数名称
	incConfFileName	生成的函数要使用的头文件
	fp		文件名柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateClientFunUsingCmmPackToFp(char *funName,char *incConfFileName,FILE *fp);

/*
功能	
	生成一个客户端函数
输入参数
	funName		函数名称
	toNewFile	是否写到新文件中
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateClientFunUsingCmmPack(char *funName,int toNewFile);

/*
功能	
	为指定的程序生成所有客户端API
输入参数
	nameOfProgram	程序名称
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateAllCmmPackClientAPIInSpecProgram(char *nameOfProgram);

/*
功能	
	生成一个服务端函数
输入参数
	funName		函数名称
	incConfFileName	生成的函数要使用的头文件
	fp		文件名柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateServerFunUsingCmmPackToFp(char *funName,char *incConfFileName,FILE *fp);

/*
功能	
	生成一个服务端函数
输入参数
	funName		函数名称
	toNewFile	是否写到新文件中
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateServerFunUsingCmmPack(char *funName,int toNewFile);

/*
功能	
	为指定的程序生成所有服务端API
输入参数
	nameOfProgram	程序名称
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateAllCmmPackServerAPIInSpecProgram(char *nameOfProgram);

#endif

