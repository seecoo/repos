//	Author:	ChenJiaMei
//	Date:	2002/11/11

#ifndef _sjl06CmdRules_
#define _sjl06CmdRules_

#include "unionDesKey.h"

// ����Կ������Կ����������Կ���ĳ���
int UnionPutKeyIntoRacalKeyString(char *key,char *keyString,int sizeOfBuf);

// ����Կ���ж�ȡ��Կ��������Կ�ڴ�ռ�ĳ���
int UnionReadKeyFromRacalKeyString(char *keyString,int lenOfKeyString,char *key);

// ����Կ�����γ�һ����Կ��
// ������Կ���ĳ���
int UnionFormX917RacalKeyString(PUnionDesKey pDesKey,char *keyString);

// ����Կ���ж�ȡһ����Կ��������Կ����
// ������Կ���ĳ���
int UnionReadKeyFromX917RacalKeyString(PUnionDesKey pDesKey,char *keyString,int lenOfKeyString);

// ������Կ����Ķ��壬�γ�ת����Կָ����
int UnionFormCmdStringForTranslateKeyUnderZMKToLMK(PUnionDesKey pDesKey,char *cmdString);

// ������Կ����Ķ��壬�ж�ת����Կָ����Ӧָ�����Ƿ���ȷ
int UnionIsValidCmdResponseStringForTranslateKeyUnderZMKToLMK(PUnionDesKey pDesKey,char *cmdResString);

// ������Կ����Ķ��壬�γ�ָ����
int UnionFormCmdStringForGenerateDesKey(PUnionDesKey pDesKey,char *cmdString);

// ������Կ����Ķ��壬�ж�������Կ������Ӧ���Ƿ���ȷ
int UnionIsValidCmdResponseStringForGenerateDesKey(PUnionDesKey pDesKey,char *cmdResString);

// ��һ���˺��γ�һ��12λ���˺Ŵ�,֧�����һλУ��ֵ
int UnionForm12LenAccountNumber(char *accNo,int lenOfAccNo,char *accNoOf12Len);

// ��һ���˺��γ�һ��12λ���˺Ŵ�,ȥ�������λУ��ֵ
int UnionFormSpec12LenAccountNumber(char *accNo,int lenOfAccNo,char *accNoOf12Len);

// �γ������ָ���ϱ����Կ���ʹ������ش�ʵ�ʳ���
int UnionFormHsmKeyTypeString(PUnionDesKey pDesKey,char *keyTypeString);

// �γ������ָ��ʶ�����Կ���ȴ�,���ش�ʵ�ʳ���
int UnionFormHsmKeyLengthString(PUnionDesKey pDesKey,char *keyLengthString);

// ��ȡ��Կֵ,������Կ����
int UnionReadKeyValueString(PUnionDesKey pDesKey,char *keyString);

// ƴװһ��keyScheme��������keyScheme��ʶ���ĳ���
int UnionFormRacalKeySchemeString(PUnionDesKey pDesKey,char *keySchemeString);

int UnionTranslateHsmKeyKeyScheme(TUnionDesKeyLength length,char *keyScheme);
int UnionTranslateHsmKeyTypeString(TUnionDesKeyType type,char *keyTypeString);
int UnionCaculateHsmKeyKeyScheme(int length,char *keyScheme);

// ����һ����Կ��
int UnionGenerateX917RacalKeyString(int keyLength,char *keyValue,char *keyString);

// ������ת��ΪRACAL����Կ����ָʾλ
char UnionTranslateKeyLenIntoRacalKeyLenFlag(TUnionDesKeyLength length);

int UnionTranslateHsmKeyTypeStringForBU(TUnionDesKeyType type,char *keyTypeString1,char *keyTypeString2);
int UnionForm16LenAccountNumber(char *accNo,int lenOfAccNo,char *accNoOf16Len);
#endif
