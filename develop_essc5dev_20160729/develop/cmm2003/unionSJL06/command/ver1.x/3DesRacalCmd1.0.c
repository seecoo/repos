// Author:	Wolfgang Wang
// Date:	2003/10/09

// 2005/04/19,Wolfgang Wang, ��3DesRacalCmd1.0.c����������
// �޸� UnionTranslateKeyUnderLMKToZMK
// ������ת��ZMK�Ĺ���

// 2005/06/10,Wolfgang Wang
// �޸�����CVKת���ĺ���
// ��ΪCVKת���ĺ�����У��ֵ��6λ������16λ

#define _RacalCmdForNewRacal_

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "sjl06.h"
#include "sjl06Protocol.h"
//#include "sjl06Cmd.h"
#include "3DesRacalSyntaxRules.h"
#include "3DesRacalCmd.h"
#include "UnionLog.h"

// ��һ����ԿKey��ZMK����ת��Ϊ��LMK����
// pZMK�Ǳ���Key��ZMK����
// ����ʱ��pKeyObject�д�ŵ���pZMK���ܵ�Key����
// ���ʱ��pKeyObject�д�ŵ���LMK���ܵ�Key���ģ�checkValue�洢����У��ֵ
// Key�ı������ԡ�ǿ�ȶ���pKeyObject����
// �������ת��ʧ��ʱ��errCode��ŵ��Ǵ����룬��ʱ����ֵΪ0
// ֻ�е�����ֵΪ0����errCodeΪ00ʱ����ת���ɹ�
int UnionTranslateKeyUnderZMKToLMK(int hsmSckHDL,PUnionSJL06 pSJL06,PUnionDesKey pZMK,PUnionDesKey pKeyObject,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512];
	int		hsmCmdLen;
		
	// 1��ƴװ��Կת��ָ��
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

	// ƴװ������
	if ((ret = UnionFormCmdStringForTranslateKeyUnderZMKToLMK(pKeyObject,hsmCmdBuf)) < 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderZMKToLMK:: UnionFormCmdStringForTranslateKeyUnderZMKToLMK!\n");
		return(ret);
	}
	hsmCmdLen = ret;
	
	// ƴװZMK under LMK04~05
	if ((ret = UnionFormX917RacalKeyString(pZMK,hsmCmdBuf+hsmCmdLen)) <= 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderZMKToLMK:: UnionFormX917RacalKeyString for [%s]!\n",pZMK->fullName);
		return(ret);
	}
	hsmCmdLen += ret;
	
	// ƴװ��Կ under ZMK
	if ((ret = UnionFormX917RacalKeyString(pKeyObject,hsmCmdBuf+hsmCmdLen)) <= 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderZMKToLMK:: UnionFormX917RacalKeyString for [%s]!\n",pKeyObject->fullName);
		return(ret);
	}
	hsmCmdLen += ret;
	
	// �ָ���
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen++;
	// ZMK���ܷ�ʽ
	//if ((ret = UnionFormRacalKeySchemeString(pZMK,hsmCmdBuf+hsmCmdLen)) < 0)
	if ((ret = UnionFormRacalKeySchemeString(pKeyObject,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderZMKToLMK:: UnionFormRacalKeySchemeString for [%s]!\n",pZMK->fullName);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܷ�ʽ
	if ((ret = UnionFormRacalKeySchemeString(pKeyObject,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderZMKToLMK:: UnionFormRacalKeySchemeString for [%s]!\n",pKeyObject->fullName);
		return(ret);
	}
	hsmCmdLen += ret;
	// У��ֵ���ɷ�ʽ
	// Modified by Wolfgang Wang,2005/06/10
	if (pKeyObject->type == conCVK)
		hsmCmdBuf[hsmCmdLen] = '1';
	else
		hsmCmdBuf[hsmCmdLen] = '0';
	// end of modification of 2005/6/10
	hsmCmdLen++;
	
	// 1��ƴװ��Կת��ָ�����
	
	// 2���������������
	if ((hsmCmdLen = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderZMKToLMK:: UnionLongConnSJL06Cmd!\n");
		return(hsmCmdLen);
	}
	hsmCmdBuf[hsmCmdLen] = 0;
	
	// 3���жϽ��յ��������Ƿ���ȷ
	// �ж�������Ӧ���Ƿ���ȷ
	if (!UnionIsValidCmdResponseStringForTranslateKeyUnderZMKToLMK(pKeyObject,hsmCmdBuf))
	{
		UnionUserErrLog("in UnionTranslateKeyUnderZMKToLMK:: UnionIsValidCmdResponseStringForTranslateKeyUnderZMKToLMK!\n");
		return(errCodeSJL06MDL_InvalidHsmResponseCode);
	}
	// �ж���Ӧ���Ƿ���00
	if (strncmp(hsmCmdBuf+2,"00",2) != 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderZMKToLMK:: hsmReturnBuf = [%s]\n",hsmCmdBuf);
		memcpy(errCode,hsmCmdBuf+2,2);
		return(0);
	}
	
	// 4����ȡ��Կ
	if ((ret = UnionReadKeyFromX917RacalKeyString(pKeyObject,hsmCmdBuf+4,hsmCmdLen-4)) < 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderZMKToLMK:: UnionReadKeyFromX917RacalKeyString! hsmReturnBuf = [%s]\n",hsmCmdBuf);
		return(ret);
	}
	
	// Modified by Wolfgang Wang,2005/06/10
	if (pKeyObject->type == conCVK)
	{
		if (ret + 4 + 6 > hsmCmdLen)
		{
			UnionUserErrLog("in UnionTranslateKeyUnderZMKToLMK:: No enough checkValue! hsmReturnBuf = [%s]\n",hsmCmdBuf);
			return(errCodeParameter);
		}
		// ��ȡУ��ֵ
		memcpy(pKeyObject->checkValue,hsmCmdBuf+4+ret,6);
	}
	else
	{
		if (ret + 4 + 16 > hsmCmdLen)
		{
			UnionUserErrLog("in UnionTranslateKeyUnderZMKToLMK:: No enough checkValue! hsmReturnBuf = [%s]\n",hsmCmdBuf);
			return(errCodeParameter);
		}
		// ��ȡУ��ֵ
		memcpy(pKeyObject->checkValue,hsmCmdBuf+4+ret,16);
	}
	// End of modification of 2005/06/01
	
	memcpy(errCode,"00",2);
	
	return(0);
}

// ��һ����ԿKey��LMK����ת��Ϊ��ZMK����
// pZMK�Ǳ���Key��ZMK����
// ����ʱ��pKeyObject�д�ŵ���LMK���ܵ�Key����
// ���ʱ��pKeyObject�д�ŵ���ZMK���ܵ�Key���ģ�checkValue�洢����У��ֵ
// Key�ı������ԡ�ǿ�ȶ���pKeyObject����
// �������ת��ʧ��ʱ��errCode��ŵ��Ǵ����룬��ʱ����ֵΪ0
// ֻ�е�����ֵΪ0����errCodeΪ00ʱ����ת���ɹ�
int UnionTranslateKeyUnderLMKToZMK(int hsmSckHDL,PUnionSJL06 pSJL06,PUnionDesKey pZMK,PUnionDesKey pKeyObject,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512];
	int		hsmCmdLen;

	// Added by Wolfgang Wang, 2005/04/19
	if (pKeyObject->type == conZMK)
	{
		return(UnionTranslateKeyUnderLMKToZMKUseA8(hsmSckHDL,pSJL06,pKeyObject->type,pKeyObject->value,pZMK->value,
				pKeyObject->value,pKeyObject->checkValue,errCode));
	}	
	// End of addition by Wolfgang Wang, 2005/04/19

	// 1��ƴװ��Կת��ָ��
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

	// ƴװ������
	if ((ret = UnionFormCmdStringForTranslateKeyUnderLMKToZMK(pKeyObject,hsmCmdBuf)) < 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMK:: UnionFormCmdStringForTranslateKeyUnderLMKToZMK!\n");
		return(ret);
	}
	hsmCmdLen = ret;
	
	// ƴװZMK under LMK04~05
	if ((ret = UnionFormX917RacalKeyString(pZMK,hsmCmdBuf+hsmCmdLen)) <= 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMK:: UnionFormX917RacalKeyString for [%s]!\n",pZMK->fullName);
		return(ret);
	}
	hsmCmdLen += ret;
	
	// ƴװ��Կ under ZMK
	if ((ret = UnionFormX917RacalKeyString(pKeyObject,hsmCmdBuf+hsmCmdLen)) <= 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMK:: UnionFormX917RacalKeyString for [%s]!\n",pKeyObject->fullName);
		return(ret);
	}
	hsmCmdLen += ret;
	
	// �ָ���
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen++;
	// ZMK���ܷ�ʽ
	if ((ret = UnionFormRacalKeySchemeString(pKeyObject,hsmCmdBuf+hsmCmdLen)) < 0)
	//if ((ret = UnionFormRacalKeySchemeString(pKeyObject,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMK:: UnionFormRacalKeySchemeString for [%s]!\n",pZMK->fullName);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK���ܷ�ʽ
	if ((ret = UnionFormRacalKeySchemeString(pKeyObject,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMK:: UnionFormRacalKeySchemeString for [%s]!\n",pKeyObject->fullName);
		return(ret);
	}
	hsmCmdLen += ret;
	// У��ֵ���ɷ�ʽ
	// Modified by Wolfgang Wang,2005/06/10
	if (pKeyObject->type == conCVK)
		hsmCmdBuf[hsmCmdLen] = '1';
	else
		hsmCmdBuf[hsmCmdLen] = '0';
	// end of modification of 2005/6/10
	hsmCmdLen++;
	
	// 1��ƴװ��Կת��ָ�����
	
	// 2���������������
	if ((hsmCmdLen = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMK:: UnionLongConnSJL06Cmd!\n");
		return(hsmCmdLen);
	}
	hsmCmdBuf[hsmCmdLen] = 0;
	
	// 3���жϽ��յ��������Ƿ���ȷ
	// �ж�������Ӧ���Ƿ���ȷ
	if (!UnionIsValidCmdResponseStringForTranslateKeyUnderLMKToZMK(pKeyObject,hsmCmdBuf))
	{
		UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMK:: UnionIsValidCmdResponseStringForTranslateKeyUnderLMKToZMK!\n");
		return(errCodeSJL06MDL_InvalidHsmResponseCode);
	}
	// �ж���Ӧ���Ƿ���00
	if (strncmp(hsmCmdBuf+2,"00",2) != 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMK:: hsmReturnBuf = [%s]\n",hsmCmdBuf);
		memcpy(errCode,hsmCmdBuf+2,2);
		return(0);
	}
	
	// 4����ȡ��Կ
	if ((ret = UnionReadKeyFromX917RacalKeyString(pKeyObject,hsmCmdBuf+4,hsmCmdLen-4)) < 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMK:: UnionReadKeyFromX917RacalKeyString! hsmReturnBuf = [%s]\n",hsmCmdBuf);
		return(ret);
	}
	
	// Modified by Wolfgang Wang,2005/06/10
	if (pKeyObject->type == conCVK)
	{
		if (ret + 4 + 6 > hsmCmdLen)
		{
			UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMK:: No enough checkValue! hsmReturnBuf = [%s]\n",hsmCmdBuf);
			return(errCodeParameter);
		}
		// ��ȡУ��ֵ
		memcpy(pKeyObject->checkValue,hsmCmdBuf+4+ret,6);
	}
	else
	{
		if (ret + 4 + 16 > hsmCmdLen)
		{
			UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMK:: No enough checkValue! hsmReturnBuf = [%s]\n",hsmCmdBuf);
			return(errCodeParameter);
		}
		// ��ȡУ��ֵ
		memcpy(pKeyObject->checkValue,hsmCmdBuf+4+ret,16);
	}
	// End of modification of 2005/06/01
	
	memcpy(errCode,"00",2);
	
	return(0);
}

// ������Կ����Ķ��壬�γ�ָ����,���������ֵĳ���
int UnionFormCmdStringForTranslateKeyUnderLMKToZMK(PUnionDesKey pDesKey,char *cmdString)
{
	switch (pDesKey->type)
	{
		case	conCVK:
			strcpy(cmdString,"AU");
			return(2);
		case	conTMK:
		case	conTPK:
		case	conPVK:
			strcpy(cmdString,"FE");
			return(2);
		case	conTAK:
			strcpy(cmdString,"MG");
			return(2);
		case	conWWK:
			strcpy(cmdString,"FQ");
			return(2);
		case	conZEK:
			strcpy(cmdString,"FM0");
			return(3);
		case	conZAK:
			strcpy(cmdString,"FM1");
			return(3);
		case	conZPK:
			strcpy(cmdString,"GC");
			return(2);
		case	conZMK:
			UnionUserErrLog("in UnionFormCmdStringForTranslateKeyUnderLMKToZMK:: not support zmk translation from lmk to zmk!\n");
			return(errCodeSJL06MDL_InvalidKeyExchange);
		default:
			UnionUserErrLog("in UnionFormCmdStringForTranslateKeyUnderLMKToZMK:: invalid key type [%d]!\n",pDesKey->type);
			return(errCodeSJL06MDL_InvalidKeyExchange);
	}	
}

// ������Կ����Ķ��壬�ж���Ӧָ�����Ƿ���ȷ
int UnionIsValidCmdResponseStringForTranslateKeyUnderLMKToZMK(PUnionDesKey pDesKey,char *cmdResString)
{
	char	response[2+1];
	
	switch (pDesKey->type)
	{
		case	conCVK:
			strcpy(response,"AV");
			break;
		case	conTMK:
		case	conTPK:
		case	conPVK:
			strcpy(response,"FF");
			break;
		case	conTAK:
			strcpy(response,"MH");
			break;
		case	conWWK:
			strcpy(response,"FR");
			break;
		case	conZEK:
		case	conZAK:
			strcpy(response,"FN");
			break;
		case	conZPK:
			strcpy(response,"GD");
			break;
		case	conZMK:
			UnionUserErrLog("in UnionIsValidCmdResponseStringForTranslateKeyUnderLMKToZMK:: not support zmk translation from lmk to zmk!\n");
			return(0);
		default:
			UnionUserErrLog("in UnionIsValidCmdResponseStringForTranslateKeyUnderLMKToZMK:: invalid key type [%d]!\n",pDesKey->type);
			return(0);
	}
	if (strncmp(response,cmdResString,2) != 0)
	{
		UnionUserErrLog("in UnionIsValidCmdResponseStringForTranslateKeyUnderLMKToZMK:: response [%s] != expected [%s]\n",cmdResString,response);
		return(0);
	}
	else
		return(1);
}

int UnionTranslateKeyUnderLMKToZMKUseA8(int hsmSckHDL,PUnionSJL06 pSJL06,
	TUnionDesKeyType keyType,char *key,char *zmk,
	char *keyByZMK,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	int		zmkLength,keyLen;
	int		offsetOfKeyByZMK,offsetOfCheckValue;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"A8",2);
	hsmCmdLen = 2;
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMKUseA8:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionGenerateX917RacalKeyString(strlen(zmk),zmk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMKUseA8:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionGenerateX917RacalKeyString(strlen(key),key,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMKUseA8:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	//UnionLog("in UnionTranslateKeyUnderLMKToZMKUseA8:: keyType = [%d]\n",keyType);
	switch (strlen(key))
	{
		case	16:
			memcpy(hsmCmdBuf+hsmCmdLen,"Z",1);
			break;
		case	32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			break;
		case	48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			break;
		default:
			UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMKUseA8:: keylen [%d] error!\n",strlen(key));
			return(errCodeParameter);
	}
	hsmCmdLen++;
	if ((ret = UnionLongConnSJL06Cmd(hsmSckHDL,pSJL06,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMKUseA8:: UnionLongConnSJL06Cmd!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"A9",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMKUseA8:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMKUseA8:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}

	switch (strlen(key))
	{
		case	16:
			offsetOfKeyByZMK = 4;
			offsetOfCheckValue = 4 + 16;
			keyLen = 16;
			break;
		case	32:
			offsetOfKeyByZMK = 4 + 1;
			offsetOfCheckValue = 4 + 1 + 32;
			keyLen = 32;
			break;
		case	48:
			offsetOfKeyByZMK = 4 + 1;
			offsetOfCheckValue = 4 + 1 + 48;
			keyLen = 48;
			break;		// Mary add, 2004-4-12
		default:
			UnionUserErrLog("in UnionTranslateKeyUnderLMKToZMKUseA8:: invalid keyLen[%d]\n",strlen(key));
			return(errCodeParameter);
	}
	memcpy(keyByZMK,hsmCmdBuf+offsetOfKeyByZMK,keyLen);
	memcpy(checkValue,hsmCmdBuf+offsetOfCheckValue,6);
	return(keyLen);
}
