//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#ifndef _hsmDefaultFunction_
#define _hsmDefaultFunction_

#include "unionHsm.h"

/*
功能
	判断指定IP的加密机是否正常
输入参数
	ipAddr		加密机IP
输入出数
	hsmGrpID	组号
返回值
	1		正常
	<=0		异常
*/
int UnionIsHsmNormal(char *ipAddr,char *hsmGrpID);

/*                                                           
功能                                                         
	获取一个密码机组中的可用密码机                       
输入参数                                                     
	hsmGrpID	密码机组号                           
输入出数                                                     
	hsmGrp		密码机结构体                         
返回值                                                       
	>=0 成功                                             
	<0 失败                                              
*/                                                           
int UnionFindHsmOfSpecHsmGrp(char *hsmGrpID,TUnionHsm hsmGrp);

/*
功能
	获取一个密码机组中的所有可用密码机
输入参数
	hsmGrpID	密码机组号
	maxNum		所能返回的最大密码机台数
输入出数
	hsmRecGrp	密码机所组成的数组
返回值
	>=0		实际读取的密码机台数
	<0		错误码
*/
int UnionFindAllWorkingHsmOfSpecHsmGrp(char *hsmGrpID, TUnionHsm hsmRecGrp[], int maxNum);

/*
功能
	获取密码机组内的打印密码机
输入参数
	hsmGrpID	密码机组ID
输入出数
	hsmRec		密码机结构体
返回值
	>=0 成功
	<0 失败
*/
int UnionFindDefaultPrinterHsmOfHsmGroup(char *hsmGrpID,PUnionHsm hsmRec);

// 使用指定加密机组内的密码机，打印一把密钥,
/*
功能
	使用指定加密机组内的密码机，打印一把密钥
输入参数
	hsmGrpID		密码机组ID
	keyType			密钥类型
	keyLen			密钥长度
	printFormat		密钥的打印格式
	checkValueFormat	密钥校验值的打印格式
	numOfComponent		分量的数量
	keyName			密钥名称
输入出数
	value			密钥值
	checkValue		密钥校验值
返回值
	>=0 成功
	<0 失败
*/
int UnionPrintDesKeyUsingSpecHsmGroup(char *hsmGrpID, int keyType, int keyLen, char *printFormat, char *checkValueFormat, int numOfComponent, char *keyName, char *value, char *checkValue);

#endif


