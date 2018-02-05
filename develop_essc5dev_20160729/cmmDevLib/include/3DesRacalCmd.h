/* Copyright:	Union Tech.
   Author:	Wolfgang Wang
   Date:	2003/05/02
*/

#ifndef _Union3DesRacalCmd_
#define _Union3DesRacalCmd_

#include "unionDesKey.h"

// ��һ����ԿKey��ZMK����ת��Ϊ��LMK����
// pZMK�Ǳ���Key��ZMK����
// ����ʱ��pKeyObject�д�ŵ���pZMK���ܵ�Key����
// ���ʱ��pKeyObject�д�ŵ���LMK���ܵ�Key���ģ�checkValue�洢����У��ֵ
// Key�ı������ԡ�ǿ�ȶ���pKeyObject����
// �������ת��ʧ��ʱ��errCode��ŵ��Ǵ����룬��ʱ����ֵΪ0
// ֻ�е�����ֵΪ0����errCodeΪ00ʱ����ת���ɹ�
int UnionTranslateKeyUnderZMKToLMK(int hsmSckHDL,PUnionSJL06 pSJL06,PUnionDesKey pZMK,PUnionDesKey pKeyObject,char *errCode);

// ��һ����ԿKey��LMK����ת��Ϊ��ZMK����
// pZMK�Ǳ���Key��ZMK����
// ����ʱ��pKeyObject�д�ŵ���LMK���ܵ�Key����
// ���ʱ��pKeyObject�д�ŵ���ZMK���ܵ�Key���ģ�checkValue�洢����У��ֵ
// Key�ı������ԡ�ǿ�ȶ���pKeyObject����
// �������ת��ʧ��ʱ��errCode��ŵ��Ǵ����룬��ʱ����ֵΪ0
// ֻ�е�����ֵΪ0����errCodeΪ00ʱ����ת���ɹ�
int UnionTranslateKeyUnderLMKToZMK(int hsmSckHDL,PUnionSJL06 pSJL06,PUnionDesKey pZMK,PUnionDesKey pKeyObject,char *errCode);

// ������Կ����Ķ��壬�γ�ָ����,���������ֵĳ���
int UnionFormCmdStringForTranslateKeyUnderLMKToZMK(PUnionDesKey pDesKey,char *cmdString);

// ������Կ����Ķ��壬�ж���Ӧָ�����Ƿ���ȷ
int UnionIsValidCmdResponseStringForTranslateKeyUnderLMKToZMK(PUnionDesKey pDesKey,char *cmdResString);

#endif
