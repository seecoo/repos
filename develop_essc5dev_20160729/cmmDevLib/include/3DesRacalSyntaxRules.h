//	Author:	ChenJiaMei
//	Date:	2002/11/11

#ifndef _sjl06CmdRules_
#define _sjl06CmdRules_

#include "unionDesKey.h"

// 将密钥置入密钥串，返回密钥串的长度
int UnionPutKeyIntoRacalKeyString(char *key,char *keyString,int sizeOfBuf);

// 从密钥串中读取密钥，返回密钥在串占的长度
int UnionReadKeyFromRacalKeyString(char *keyString,int lenOfKeyString,char *key);

// 从密钥对象形成一个密钥串
// 返回密钥串的长度
int UnionFormX917RacalKeyString(PUnionDesKey pDesKey,char *keyString);

// 从密钥串中读取一个密钥，放入密钥对象
// 返回密钥串的长度
int UnionReadKeyFromX917RacalKeyString(PUnionDesKey pDesKey,char *keyString,int lenOfKeyString);

// 根据密钥对象的定义，形成转换密钥指令字
int UnionFormCmdStringForTranslateKeyUnderZMKToLMK(PUnionDesKey pDesKey,char *cmdString);

// 根据密钥对象的定义，判断转换密钥指令响应指令字是否正确
int UnionIsValidCmdResponseStringForTranslateKeyUnderZMKToLMK(PUnionDesKey pDesKey,char *cmdResString);

// 根据密钥对象的定义，形成指令字
int UnionFormCmdStringForGenerateDesKey(PUnionDesKey pDesKey,char *cmdString);

// 根据密钥对象的定义，判断生成密钥批令响应字是否正确
int UnionIsValidCmdResponseStringForGenerateDesKey(PUnionDesKey pDesKey,char *cmdResString);

// 从一个账号形成一个12位的账号串,支掉最后一位校验值
int UnionForm12LenAccountNumber(char *accNo,int lenOfAccNo,char *accNoOf12Len);

// 从一个账号形成一个12位的账号串,去掉最后两位校验值
int UnionFormSpec12LenAccountNumber(char *accNo,int lenOfAccNo,char *accNoOf12Len);

// 形成密码机指令认别的密钥类型串，返回串实际长度
int UnionFormHsmKeyTypeString(PUnionDesKey pDesKey,char *keyTypeString);

// 形成密码机指令识别的密钥长度串,返回串实际长度
int UnionFormHsmKeyLengthString(PUnionDesKey pDesKey,char *keyLengthString);

// 读取密钥值,返回密钥长度
int UnionReadKeyValueString(PUnionDesKey pDesKey,char *keyString);

// 拼装一个keyScheme串，返回keyScheme标识串的长度
int UnionFormRacalKeySchemeString(PUnionDesKey pDesKey,char *keySchemeString);

int UnionTranslateHsmKeyKeyScheme(TUnionDesKeyLength length,char *keyScheme);
int UnionTranslateHsmKeyTypeString(TUnionDesKeyType type,char *keyTypeString);
int UnionCaculateHsmKeyKeyScheme(int length,char *keyScheme);

// 生成一个密钥串
int UnionGenerateX917RacalKeyString(int keyLength,char *keyValue,char *keyString);

// 将长度转换为RACAL的密钥长度指示位
char UnionTranslateKeyLenIntoRacalKeyLenFlag(TUnionDesKeyLength length);

int UnionTranslateHsmKeyTypeStringForBU(TUnionDesKeyType type,char *keyTypeString1,char *keyTypeString2);
int UnionForm16LenAccountNumber(char *accNo,int lenOfAccNo,char *accNoOf16Len);
#endif
