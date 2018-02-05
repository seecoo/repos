// Author:	ChenJiaMei
// Date:	2002-12-11

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "3DesRacalSyntaxRules.h"
#include "unionErrCode.h"

#include "UnionLog.h"
#include "unionDesKey.h"
#include "unionREC.h"
#include "UnionStr.h"

// ��һ���˺��γ�һ��12λ���˺Ŵ�,֧�����һλУ��ֵ
int UnionForm12LenAccountNumber(char *accNo,int lenOfAccNo,char *accNoOf12Len)
{
	if (lenOfAccNo < 0)
	{
		UnionUserErrLog("in UnionForm12LenAccountNumber:: lenOfAccNo [%d] too short!\n",lenOfAccNo);
		return(errCodeParameter);
	}
	if (lenOfAccNo >= 13)
		memcpy(accNoOf12Len,accNo+lenOfAccNo-13,12);
	else
	{
		memset(accNoOf12Len,'0',12);
		memcpy(accNoOf12Len+12-lenOfAccNo,accNo,lenOfAccNo);
	}
	return(12);
}
// ��һ���˺��γ�һ��12λ���˺Ŵ�,ȥ�������λУ��ֵ
int UnionFormSpec12LenAccountNumber(char *accNo,int lenOfAccNo,char *accNoOf12Len)
{
        if (lenOfAccNo < 0)
        {
                UnionUserErrLog("in UnionFormSpec12LenAccountNumber:: lenOfAccNo [%d] too short!\n",lenOfAccNo);
                return(errCodeParameter);
        }
        if (lenOfAccNo >= 14)
	{
                memcpy(accNoOf12Len,accNo+lenOfAccNo-14,12);
	}
        else if (lenOfAccNo == 13)
	{
                memset(accNoOf12Len,'0',12);
                memcpy(accNoOf12Len+1,accNo, 11);
	}
	else
        {
                memset(accNoOf12Len,'0',12);
                memcpy(accNoOf12Len+12-lenOfAccNo,accNo,lenOfAccNo);
        }
        return(12);
}

// ��һ���˺��γ�һ��16λ���˺�,֧�����һλУ��ֵ
int UnionForm16LenAccountNumber(char *accNo,int lenOfAccNo,char *accNoOf16Len)
{
	if (lenOfAccNo < 0)
	{
		UnionUserErrLog("in UnionForm16LenAccountNumber:: lenOfAccNo [%d] too short!\n",lenOfAccNo);
		return(errCodeParameter);
	}
	memset(accNoOf16Len,'0',16);
	if (lenOfAccNo >= 13)
		memcpy(accNoOf16Len+4,accNo+lenOfAccNo-13,12);
	else
		memcpy(accNoOf16Len+16-lenOfAccNo,accNo,lenOfAccNo);
	return(16);
}

// ����Կ���ж�ȡ��Կ��������Կ�ڴ���ռ�ĳ���
int UnionReadKeyFromRacalKeyString(char *keyString,int lenOfKeyString,char *key)
{
	int	len = 0;
	int	offset = 0;
	char	flag;
	
	if ((key == NULL) || (keyString == NULL) || (lenOfKeyString <= 0))
	{
		UnionUserErrLog("in UnionReadKeyFromRacalKeyString:: null pointer!\n");
		return(errCodeParameter);
	}
	switch (keyString[0])
	{
		case	'X':
			len = 32;
			offset = 1;
			break;
		case	'Y':
			len = 48;
			offset = 1;
			break;
		case	'Z':
			len = 16;
			offset = 1;
			break;
		default:
			len = 16;
			break;
	}
	if (offset + len > lenOfKeyString)
	{
		UnionUserErrLog("in UnionPutKeyIntoRacalKeyString:: lenOfKeyString [%d] too short!\n",lenOfKeyString);
		return(errCodeParameter);
	}
	memcpy(key,keyString+offset,len);
	key[len] = 0;
	return(len+offset);
}

// ����Կ������Կ����������Կ���ĳ���
int UnionPutKeyIntoRacalKeyString(char *key,char *keyString,int sizeOfBuf)
{
	int	len = 0;
	int	offset = 0;
	char	flag;
	
	if ((key == NULL) || (keyString == NULL))
	{
		UnionUserErrLog("in UnionPutKeyIntoRacalKeyString:: null pointer!\n");
		return(errCodeParameter);
	}
	switch (len = strlen(key))
	{
		case	16:
			break;
		case	32:
			offset = 1;
			flag = 'X';
			break;
		case	48:
			offset = 1;
			flag = 'Y';
			break;
		default:
			UnionUserErrLog("in UnionPutKeyIntoRacalKeyString:: invalid [%s]\n",key);
			return(errCodeParameter);
	}
	if (offset + len > sizeOfBuf)
	{
		UnionUserErrLog("in UnionPutKeyIntoRacalKeyString:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeParameter);
	}
	if (offset == 1)
		keyString[0] = flag;
	memcpy(keyString+offset,key,len);
	return(len+offset);
}

// ����Կ�����γ�һ����Կ��
// ������Կ���ĳ���
int UnionFormX917RacalKeyString(PUnionDesKey pDesKey,char *keyString)
{
	int	len = 0;
	
	switch (pDesKey->length)
	{
		case	con64BitsDesKey:
			memcpy(keyString,pDesKey->value,16);
			len = 16;
			break;
		case	con128BitsDesKey:
			memcpy(keyString,"X",1);
			memcpy(keyString+1,pDesKey->value,32);
			len = 32 + 1;
			break;
		case	con192BitsDesKey:
			memcpy(keyString,"Y",1);
			memcpy(keyString+1,pDesKey->value,48);
			len = 48 + 1;
			break;
		default:
			UnionUserErrLog("in UnionFormX917RacalKeyString:: invalid keyLength [%d]\n",pDesKey->length);
			return(errCodeParameter);
	}
	return(len);
}

// ����Կ���ж�ȡһ����Կ��������Կ����
// ������Կ���ĳ���
int UnionReadKeyFromX917RacalKeyString(PUnionDesKey pDesKey,char *keyString,int lenOfKeyString)
{
	int	len = 0;
	int	offset = 0;	
	switch (pDesKey->length)
	{
		case	con64BitsDesKey:
			len = 16;
			break;
		case	con128BitsDesKey:
			len = 32;
			if ((keyString[0] == 'X') || (keyString[0] == 'U'))
			{
				//UnionUserErrLog("in UnionReadKeyFromX917RacalKeyString:: Invalid flag [%c] for 128 bits key!\n",keyString[0]);
				offset = 1;
			}
			else
			{
				if (((keyString[0] >= '0') && (keyString[0] <= '9')) || ((keyString[0] >= 'A') && (keyString[0] <= 'F')))
					offset = 0;
				else
				{
					UnionUserErrLog("in UnionReadKeyFromX917RacalKeyString:: Invalid flag [%c] for 128 bits key!\n",keyString[0]);
					return(errCodeParameter);
				}
			}
			break;
		case	con192BitsDesKey:
			if ((keyString[0] != 'Y') && (keyString[0] != 'T'))
			{
				UnionUserErrLog("in UnionReadKeyFromX917RacalKeyString:: Invalid flag [%c] for 192 bits key!\n",keyString[0]);
				return(errCodeParameter);
			}
			len = 48;
			offset = 1;
			break;
		default:
			UnionUserErrLog("in UnionReadKeyFromX917RacalKeyString:: invalid keyLength [%d]\n",pDesKey->length);
			return(errCodeParameter);
	}
	if (len+offset > lenOfKeyString)
	{
		UnionUserErrLog("in UnionReadKeyFromX917RacalKeyString:: lenOfKeyString is too short! [%d]\n",lenOfKeyString);
		return(errCodeParameter);
	}
	memcpy(pDesKey->value,keyString+offset,len);
	len += offset;
	return(len);
}

// ������Կ����Ķ��壬�γ�ָ����,���������ֵĳ���
int UnionFormCmdStringForTranslateKeyUnderZMKToLMK(PUnionDesKey pDesKey,char *cmdString)
{
	switch (pDesKey->type)
	{
		case	conCVK:
			strcpy(cmdString,"AW");
			return(2);
		case	conTMK:
		case	conTPK:
		case	conPVK:
			strcpy(cmdString,"FC");
			return(2);
		case	conTAK:
			strcpy(cmdString,"MI");
			return(2);
		case	conWWK:
			strcpy(cmdString,"FS");
			return(2);
		case	conZEK:
			strcpy(cmdString,"FK0");
			return(3);
		case	conZAK:
			strcpy(cmdString,"FK1");
			return(3);
		case	conZPK:
			strcpy(cmdString,"FA");
			return(2);
		case	conZMK:
			strcpy(cmdString,"BY");
			return(2);
		default:
			UnionUserErrLog("in UnionFormCmdStringForTranslateKeyUnderZMKToLMK:: invalid key type [%d]!\n",pDesKey->type);
			return(errCodeSJL06MDL_InvalidKeyExchange);
	}	
}

// ������Կ����Ķ��壬�ж���Ӧָ�����Ƿ���ȷ
int UnionIsValidCmdResponseStringForTranslateKeyUnderZMKToLMK(PUnionDesKey pDesKey,char *cmdResString)
{
	char	response[2+1];
	
	switch (pDesKey->type)
	{
		case	conCVK:
			strcpy(response,"AX");
			break;
		case	conTMK:
		case	conTPK:
		case	conPVK:
			strcpy(response,"FD");
			break;
		case	conTAK:
			strcpy(response,"MJ");
			break;
		case	conWWK:
			strcpy(response,"FT");
			break;
		case	conZEK:
		case	conZAK:
			strcpy(response,"FL");
			break;
		case	conZPK:
			strcpy(response,"FB");
			break;
		case	conZMK:
			strcpy(response,"BZ");
			break;
		default:
			UnionUserErrLog("in UnionIsValidCmdResponseStringForTranslateKeyUnderZMKToLMK:: invalid key type [%d]!\n",pDesKey->type);
			return(0);
	}
	if (strncmp(response,cmdResString,2) != 0)
	{
		UnionUserErrLog("in UnionIsValidCmdResponseStringForTranslateKeyUnderZMKToLMK:: response [%s] != expected [%s]\n",cmdResString,response);
		return(0);
	}
	else
		return(1);
}

// ������Կ����Ķ��壬�γ�������Կ��ָ���֣�����ָ���ֵĳ���
int UnionFormCmdStringForGenerateDesKey(PUnionDesKey pDesKey,char *cmdString)
{
	switch (pDesKey->type)
	{
		//case	conCVK:
		//	strcpy(cmdString,"AW");
		//	return(2);
		case	conTMK:
		case	conTPK:
		case	conPVK:
			strcpy(cmdString,"HC");
			return(2);
		case	conTAK:
			strcpy(cmdString,"HA");
			return(2);
		//case	conWWK:
		//	strcpy(cmdString,"FS");
		//	return(2);
		case	conZEK:
			strcpy(cmdString,"FI0");
			return(3);
		case	conZAK:
			strcpy(cmdString,"FI1");
			return(3);
		case	conZPK:
			strcpy(cmdString,"IA");
			return(2);
		//case	conZMK:
		//	strcpy(cmdString,"BY");
		//	return(2);
		default:
			UnionUserErrLog("in UnionFormCmdStringForGenerateDesKey:: invalid key type [%d]!\n",pDesKey->type);
			return(errCodeSJL06MDL_InvalidKeyExchange);
	}
}	

// ������Կ����Ķ��壬�ж�������Կ������Ӧ���Ƿ���ȷ
int UnionIsValidCmdResponseStringForGenerateDesKey(PUnionDesKey pDesKey,char *cmdResString)
{
	switch (pDesKey->type)
	{
		case	conTMK:
		case	conTPK:
		case	conPVK:
			if (strncmp(cmdResString,"HD",2) == 0)
				return(1);
			else
				return(0);
		case	conTAK:
			if (strncmp(cmdResString,"HB",2) == 0)
				return(1);
			else
				return(0);
		case	conZEK:
			if (strncmp(cmdResString,"FJ",2) == 0)
				return(1);
			else
				return(0);
		case	conZAK:
			if (strncmp(cmdResString,"FJ",2) == 0)
				return(1);
			else
				return(0);
		case	conZPK:
			if (strncmp(cmdResString,"IB",2) == 0)
				return(1);
			else
				return(0);
		default:
			UnionUserErrLog("in UnionIsValidCmdResponseStringForGenerateDesKey:: invalid key type [%d]!\n",pDesKey->type);
			return(errCodeSJL06MDL_InvalidHsmResponseCode);
	}
}	

// �γ������ָ���ϱ����Կ���ʹ������ش�ʵ�ʳ���
int UnionFormHsmKeyTypeString(PUnionDesKey pDesKey,char *keyTypeString)
{
	switch (pDesKey->type)
	{
		case	conCVK:
			strcpy(keyTypeString,"402");
			return(3);
		case	conTMK:
		case	conTPK:
		case	conPVK:
			strcpy(keyTypeString,"002");
			return(3);
		case	conTAK:
			strcpy(keyTypeString,"003");
			return(3);
		case	conWWK:
			strcpy(keyTypeString,"006");
			return(3);
		case	conZEK:
			strcpy(keyTypeString,"00A");
			return(3);
		case	conZAK:
			strcpy(keyTypeString,"008");
			return(3);
		case	conZPK:
			strcpy(keyTypeString,"001");
			return(3);
		case	conZMK:
			strcpy(keyTypeString,"000");
			return(3);
		case	conBDK:
			strcpy(keyTypeString,"009");
			return(3);
		case	conEDK:
			strcpy(keyTypeString,"007");
			return(3);
		case	conMKAC:
			strcpy(keyTypeString,"109");
			return(3);
		case	conMKSMC:
			strcpy(keyTypeString,"309");
			return(3);
		case	conMKSMI:	// add by leipp 20151023
			strcpy(keyTypeString,"209");
			return(3);	// add end 
		default:
			UnionUserErrLog("in UnionFormHsmKeyTypeString:: invalid key type [%d]!\n",pDesKey->type);
			return(errCodeSJL06MDL_InvalidKeyExchange);
	}	
}

// �γ������ָ��ʶ�����Կ���ȴ�,���ش�ʵ�ʳ���
int UnionFormHsmKeyLengthString(PUnionDesKey pDesKey,char *keyLengthString)
{
	switch (pDesKey->length)
	{
		case	con64BitsDesKey:
			strcpy(keyLengthString,"Z");
			return(1);
		case	con128BitsDesKey:
			strcpy(keyLengthString,"X");
			return(1);
		case	con192BitsDesKey:
			strcpy(keyLengthString,"Y");
			return(1);
		default:
			UnionUserErrLog("in UnionFormHsmKeyLengthString:: invalid key type [%d]!\n",pDesKey->type);
			return(errCodeParameter);
	}	
}

		
// ��ȡ��Կֵ��������Կ����
int UnionReadKeyValueString(PUnionDesKey pDesKey,char *keyString)
{
	switch (pDesKey->length)
	{
		case	con64BitsDesKey:
			memcpy(keyString,pDesKey->value,16);
			return(16);
		case	con128BitsDesKey:
			memcpy(keyString,pDesKey->value,32);
			return(32);
		case	con192BitsDesKey:
			memcpy(keyString,pDesKey->value,48);
			return(48);
		default:
			UnionUserErrLog("in UnionReadKeyValueString:: unknown pDesKey length! [%s]\n",pDesKey->fullName);
			return(errCodeParameter);
	}
}


// ƴװһ��keyScheme��������keyScheme��ʶ���ĳ���
int UnionFormRacalKeySchemeString(PUnionDesKey pDesKey,char *keySchemeString)
{
	switch (pDesKey->length)
	{
		case	con64BitsDesKey:
			memcpy(keySchemeString,"Z",1);
			return(1);
		case	con128BitsDesKey:
			memcpy(keySchemeString,"X",1);
			return(1);
		case	con192BitsDesKey:
			memcpy(keySchemeString,"Y",1);
			return(1);
		default:
			UnionUserErrLog("in UnionReadKeyValueString:: unknown pDesKey length! [%s]\n",pDesKey->fullName);
			return(errCodeParameter);
	}
}

int UnionTranslateHsmKeyTypeString(TUnionDesKeyType type,char *keyTypeString)
{
        switch (type)
        {
                case    conCVK:
                        strcpy(keyTypeString,"402");
                        return(3);
                case    conTMK:
                case    conTPK:
                case    conPVK:
                        strcpy(keyTypeString,"002");
                        return(3);
                case    conTAK:
                        strcpy(keyTypeString,"003");
                        return(3);
                case    conWWK:
                        strcpy(keyTypeString,"006");
                        return(3);
                case    conZEK:
                        strcpy(keyTypeString,"00A");
                        return(3);
                case    conZAK:
                        strcpy(keyTypeString,"008");
                        return(3);
                case    conZPK:
                        strcpy(keyTypeString,"001");
                        return(3);
                case    conZMK:
                case    conKMUSeed:
                case    conKMCSeed:
                case    conMDKSeed:
                       strcpy(keyTypeString,"000");
                        return(3);
		case	conEDK:
			strcpy(keyTypeString,"007");
			return(3);
		case	conBDK:
			strcpy(keyTypeString,"009");
			return(3);
		case	conMKAC:
			strcpy(keyTypeString,"109");
			return(3);
		case	conMKSMC:
			strcpy(keyTypeString,"309");
			return(3);
		case	conMKSMI:	// add by leipp 20151023
			strcpy(keyTypeString,"209");
			return(3);	// add by leipp end
                default:
                        UnionUserErrLog("in UnionTranslateHsmKeyTypeString:: invalid key type [%d]!\n",type);
                        return(errCodeSJL06MDL_InvalidKeyExchange);
        }
}

// 2011-4-19 xusj add begin
int UnionTranslateHsmKeyTypeStringForBU(TUnionDesKeyType type,char *keyTypeString1,char *keyTypeString2)
{
        switch (type)
        {
                case    conCVK:
                        strcpy(keyTypeString1,"FFF");
                        strcpy(keyTypeString2,"402");
                        return(3);
                case    conTMK:
                case    conTPK:
                case    conPVK:
                        strcpy(keyTypeString1,"002");
                        return(3);
                case    conTAK:
                        strcpy(keyTypeString1,"003");
                        return(3);
                case    conWWK:
                        strcpy(keyTypeString1,"006");
                        return(3);
                case    conZEK:
                        strcpy(keyTypeString1,"00A");
                        return(3);
                case    conZAK:
                        strcpy(keyTypeString1,"008");
                        return(3);
                case    conZPK:
                        strcpy(keyTypeString1,"001");
                        return(3);
                case    conZMK:
                case    conKMUSeed:
                case    conKMCSeed:
                case    conMDKSeed:
                       strcpy(keyTypeString1,"000");
                        return(3);
		case	conEDK:
			strcpy(keyTypeString1,"007");
			return(3);
		case	conBDK:
			strcpy(keyTypeString1,"009");
			return(3);
		case	conMKAC:
			strcpy(keyTypeString1,"FFF");
			strcpy(keyTypeString2,"109");
			return(3);
		case	conMKSMC:
			strcpy(keyTypeString1,"FFF");
			strcpy(keyTypeString2,"309");
			return(3);
		case	conMKSMI:	//add by leipp 20151023
			strcpy(keyTypeString1,"FFF");
			strcpy(keyTypeString2,"209");
			return(3);	//add by 201510223 end
                default:
                        UnionUserErrLog("in UnionTranslateHsmKeyTypeString:: invalid key type [%d]!\n",type);
                        return(errCodeSJL06MDL_InvalidKeyExchange);
        }
}
// 2011-04-19 xusj add end

int UnionTranslateHsmKeyKeyScheme(TUnionDesKeyLength length,char *keyScheme)
{
	switch (length)
	{
		case	con64BitsDesKey:
		case	64:
		case	16:
			memcpy(keyScheme,"Z",1);
			return(1);
		case	con128BitsDesKey:
		case	128:
		case	32:
			memcpy(keyScheme,"X",1);
			return(1);
		case	con192BitsDesKey:
		case	192:
		case	48:
			memcpy(keyScheme,"Y",1);
			return(1);
                default:
                        UnionUserErrLog("in UnionTranslateHsmKeyKeyScheme:: invalid key length [%d]!\n",length);
                        return(errCodeParameter);
	}
}

char UnionTranslateKeyLenIntoRacalKeyLenFlag(TUnionDesKeyLength length)
{
	switch (length)
	{
		case	con64BitsDesKey:
			return('Z');
		case	con128BitsDesKey:
			return('X');
		case	con192BitsDesKey:
			return('Y');
                default:
			return(' ');
	}
}

int UnionCaculateHsmKeyKeyScheme(int length,char *keyScheme)
{
	switch (length)
	{
		case	16:
			memcpy(keyScheme,"Z",1);
			return(1);
		case	32:
			memcpy(keyScheme,"X",1);
			return(1);
		case	48:
			memcpy(keyScheme,"Y",1);
			return(1);
                default:
                        UnionUserErrLog("in UnionCaculateHsmKeyKeyScheme:: invalid key length [%d]\n",length);
                        return(errCodeParameter);
	}
}

int UnionGenerateX917RacalKeyString(int keyLength,char *keyValue,char *keyString)
{
	switch (keyLength)
	{
		case	16:
		case	64:
		case	con64BitsDesKey:
			memcpy(keyString,keyValue,16);
			return(16);
		case	32:
		case	128:
		case	con128BitsDesKey:
			memcpy(keyString,"X",1);
			memcpy(keyString+1,keyValue,32);
			return(33);
		case	48:
		case	192:
		case	con192BitsDesKey:
			memcpy(keyString,"Y",1);
			memcpy(keyString+1,keyValue,48);
			return(49);
		default:
			return(errCodeParameter);
	}
}

/*����Կȫ�����Ϊ�����֣�Ӧ�á��ڵ㡢��Կ����*/
int UnionSplitFullKeyNameToThreeComp(char *fullKeyName, char *appNo, char *nodeID, char *keyType)
{
	int	ret;
	char    varGrp[3][128];
	
	memset(varGrp, 0, sizeof varGrp);
	
	ret = UnionSeprateVarStrIntoVarGrp(fullKeyName, strlen(fullKeyName), '.',varGrp, 3);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionSplitToThreeComp:: UnionSeprateVarStrIntoVarGrp err fullKeyName=[%s]\n", fullKeyName);
		return(ret);
	}
	if (ret == 1)
	{
		strcpy(keyType, varGrp[0]);
	}
	else
	{
		strcpy(appNo, varGrp[0]);
		strcpy(nodeID, varGrp[1]);
		strcpy(keyType, varGrp[2]);
	}

	return 0;
}

// ����Կ������ȡӦ��
int UnionGetAppNoFromFullKeyName(char *fullKeyName, char *appNo)
{
	char 	nodeID[64];
	char 	keyType[64];
	
	memset(nodeID, 0, sizeof nodeID);
	memset(keyType, 0, sizeof keyType);

	return(UnionSplitFullKeyNameToThreeComp(fullKeyName, appNo, nodeID, keyType));
}

// ����Կ������ȡ�ڵ�
int UnionGetNodeIDFromFullKeyName(char *fullKeyName, char *nodeID)
{
	char 	appNo[64];
	char 	keyType[64];
	
	memset(appNo, 0, sizeof appNo);
	memset(keyType, 0, sizeof keyType);

	return(UnionSplitFullKeyNameToThreeComp(fullKeyName, appNo, nodeID, keyType));
}

// ����Կ������ȡ��Կģ��
int UnionGetKeyModelFromFullKeyName(char *fullKeyName, char *keyType)
{
	char 	appNo[64];
	char 	nodeID[64];
	
	memset(appNo, 0, sizeof appNo);
	memset(nodeID, 0, sizeof nodeID);

	return(UnionSplitFullKeyNameToThreeComp(fullKeyName, appNo, nodeID, keyType));
}

// ��(01.test.zpk)��ʽ����Կ����װ����(01.test-�汾��.zpk)����ʽ
int UnionFormKeyNameForDerivedKey(char *inName, int version, char *outName)
{
        int             ret;
        char            keyName[128];
        char            appNo[64];
        char            nodeID[64];
        char            keyType[64];

        memset(keyName, 0, sizeof keyName);
	strcpy(keyName, inName);

        memset(appNo, 0, sizeof appNo);
        memset(nodeID, 0, sizeof nodeID);
        memset(keyType, 0, sizeof keyType);

        if ( (ret=UnionSplitFullKeyNameToThreeComp(inName, appNo, nodeID, keyType))<0 )
        {
                UnionUserErrLog("in UnionFormKeyNameForDerivedKey:: UnionSplitFullKeyNameToThreeComp err fullKeyName=[%s]\n", inName);
                return (ret);
        }
        sprintf(outName,"%s.%s-%03d.%s", appNo, nodeID, version, keyType);

	if (UnionReadStringTypeRECVar(keyName) != NULL)
		strcpy(outName, (char *)UnionReadStringTypeRECVar(keyName));

        return 0;
}
