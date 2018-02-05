//	Wolfgang Wang
//	2003/05/01
//	Version 2.0

// Wolfgang Wang, 2003/09/24, 在1.0基础上升级为2.0 删除了原创建/删除密钥对象类函数。
// 重写了：UnionIsValidDesKeyFullName

// 在2.0基础上升级为20040714,Wolfgang Wang, 2004/07/21
// 应用编号长度由用户定，密钥名称里可以包含任何字符

// 2004/11/25 在 unionDesKey.20040714.c升级为 unionDesKey3.0.20041125.c

#include <stdio.h>
#include <string.h>
#include <time.h>

#ifndef _UnionDesKey_3_x_
#define _UnionDesKey_3_x_
#endif

#include "unionKeyDB.h"
#include "unionDesKey.h"
#include "UnionLog.h"

int UnionGetDesKeyBitsLength(TUnionDesKeyLength length)
{
	switch (length)
	{
		case	con64BitsDesKey:
			return(64);
		case	con128BitsDesKey:
			return(128);
		case	con192BitsDesKey:
			return(192);
		default:
			return(errCodeDesKeyDBMDL_InvalidKeyLength);
	}
}

// Wolfgang Wang, 2004/11/25
int UnionIsOldVerDesKeyEffective(PUnionDesKey pkey)
{
	time_t	now;
	long	snap;
	
	if (pkey == NULL)
	{
		UnionUserErrLog("in UnionIsOldVerDesKeyEffective:: null pointer!\n");
		return(0);
	}
	time(&now);
	if (pkey->oldVerEffective && 
		( (((snap = now - pkey->lastUpdateTime) < pkey->windowBetweenKeyVer) && (snap > 0)) || (pkey->windowBetweenKeyVer < 0) ) )
	{
		//UnionLog("in UnionIsOldVerDesKeyEffective:: oldVerEffective = [%d] snap = [%ld] windowBetweenKeyVer = [%ld]\n",pkey->oldVerEffective,snap,pkey->windowBetweenKeyVer);
		return(1);
	}
	else
	{
		//UnionUserErrLog("in UnionIsOldVerDesKeyEffective:: oldVerEffective = [%d] snap = [%ld] windowBetweenKeyVer = [%ld]\n",pkey->oldVerEffective,snap,pkey->windowBetweenKeyVer);
		return(0);
	}
}

int UnionIsValidDesKeyLength(TUnionDesKeyLength len)
{
	switch (len)
	{
		case	con64BitsDesKey:
		case	con128BitsDesKey:
		case	con192BitsDesKey:
			return(1);
		default:
			return(0);
	}
}

int UnionIsValidDesKeyProtectMethod(TUnionDesKeyProtectMethod method)
{
	switch (method)
	{
		case	conProtectedByLMK0001:
		case	conProtectedByLMK0203:
		case	conProtectedByLMK0405:
		case	conProtectedByLMK0607:
		case	conProtectedByLMK0809:
		case	conProtectedByLMK1011:
		case	conProtectedByLMK1213:
		case	conProtectedByLMK1415:
		case	conProtectedByLMK1617:
		case	conProtectedByLMK1819:
		case	conProtectedByLMK2021:
		case	conProtectedByLMK2223:
		case	conProtectedByLMK2425:
		case	conProtectedByLMK2627:
		case	conProtectedByLMK2829:
		case	conProtectedByLMK3031:
		case	conProtectedByLMK3233:
		case	conProtectedByLMK3435:
		case	conProtectedByLMK3637:
		case	conProtectedByLMK3839:
			return(1);
		default:
			return(0);
	}
}

// 判断是否是合法的密钥属性
int UnionIsValidDesKeyAttribute(TUnionDesKeyAttribute attr)
{
	switch (attr)
	{
		case	conDesKeyAttrValue:
		case	conDesKeyAttrCheckValue:
		case	conDesKeyAttrLength:
		//case	conDesKeyAttrProtectMethod:
		case	conDesKeyAttrType:
		case	conDesKeyAttrActiveDate:
		case	conDesKeyFullName:
		case	conDesKeyAttrMaxEffectiveDays:
		case	conDesKeyAttrMaxUseTimes:
		case	conDesKeyAttrUseTimes:
		case	conDesKeyAttrLastUpdateTime:
		case	conDesKeyAttrOldVerEffective:
		case	conDesKeyAttrOldValue:
		case	conDesKeyAttrOldCheckValue:
		case	conDesKeyAttrWindowBetweenKeyVer:
			return(1);
		default:
			return(0);
	}
}

// 判断是否是合法的密钥密文
int UnionIsValidDesKeyCryptogram(char *keyCryptogram)
{
	int	len;
	int	i;
	
	len = strlen(keyCryptogram);
	if (len == 0)
		return(1);
	if ((len % 16 != 0) || (len / 16 <= 0) || (len / 16 > 3))
		return(0);
	else
	{
		for (i = 0; i < len; i++)
		{
			keyCryptogram[i] = toupper(keyCryptogram[i]);
		}
		return(UnionIsBCDStr(keyCryptogram));
	}
}

// 判断是否是合法的密钥校验值
int UnionIsValidDesKeyCheckValue(char *checkValue)
{
	int	len;
	int	i;
	
	len = strlen(checkValue);
	if (len == 0)
		return(1);
	
	if ((len > 16) || (len < 4))
		return(0);	
	else
	{
		for (i = 0; i < len; i++)
		{
			checkValue[i] = toupper(checkValue[i]);
		}
		return(UnionIsBCDStr(checkValue));
	}
}

// 判断是否是合法的密钥全称
int UnionIsValidDesKeyFullName(char *fullName)
{
	PUnionDesKey	pDesKey;
	int		pointNum = 0;
	int		len;
	int		i;
	
	if ((len = strlen(fullName)) > sizeof(pDesKey->fullName) - 1)
		return(0);
	
	for (i = 0; i < len; i++)
	{
		if (fullName[i] == '.')
			pointNum++;
			
	}
	
	if (pointNum != 2)
		return(0);
	else
	{
		if (fullName[len-1] == '.')
			return(0);
		else
			return(1);
	}
}

// 判断是否是合法的密钥类型
int UnionIsValidDesKeyType(TUnionDesKeyType type)
{
	switch (type)
	{
		case	conZPK:
		case	conZAK:
		case	conZMK:
		case	conTMK:
		case	conTPK:
		case	conTAK:
		case	conPVK:
		case	conCVK:
		case	conZEK:
		case	conWWK:
		case	conBDK:
		case	conEDK:
		case	conSelfDefinedKey:
			return(1);
		default:
			return(0);
	}
}

int UnionPrintDesKey(PUnionDesKey pDesKey)
{
	char	keyName[40];
	//time_t	now;
		
	if (pDesKey == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKey:: null pointer!\n");
		return(errCodeParameter);
	}
	//time(&now);
	printf("[fullName]		[%s]\n",pDesKey->fullName);
	printf("[cryptogram]		[%s]\n",pDesKey->value);
	printf("[checkValue]		[%s]\n",pDesKey->checkValue);
	printf("[activeDate]		[%s]\n",pDesKey->activeDate);
	printf("[maxUseTimes]		[%ld]\n",pDesKey->maxUseTimes);
	printf("[useTimes]		[%ld]\n",pDesKey->useTimes);
	printf("[maxEffectiveDays]	[%ld]\n",pDesKey->maxEffectiveDays);
	printf("[lastUpdateTime]	[%ld]\n",pDesKey->lastUpdateTime);
	printf("[oldVerEffective]	[%d]\n",pDesKey->oldVerEffective);
	printf("[oldValue]		[%s]\n",pDesKey->oldValue);
	printf("[oldCheckValue]		[%s]\n",pDesKey->oldCheckValue);
	printf("[windowBetweenKeyVer]	[%d]\n",pDesKey->windowBetweenKeyVer);
	printf("[container]		[%s]\n",pDesKey->container);
	switch (pDesKey->length)
	{
		case	con64BitsDesKey:
			printf("this is a 64 bits des key.\n");
			break;
		case	con128BitsDesKey:
			printf("this is a 128 bits des key.\n");
			break;
		case	con192BitsDesKey:
			printf("this is a 192 bits des key.\n");
			break;
		default:
			printf("length of this des key error!\n");
			break;
	}
	memset(keyName,0,sizeof(keyName));
	UnionGetNameOfDesKey(pDesKey->type,keyName);
	printf("this is a %s!\n",keyName);
		
	return(0);
}

int UnionIsValidDesKey(PUnionDesKey pDesKey)
{
	if (pDesKey == NULL)
	{
		UnionUserErrLog("in UnionIsValidDesKey:: null pointer!\n");
		return(0);
	}

	if (!UnionIsValidDesKeyLength(pDesKey->length))
		return(0);
	if (!UnionIsValidDesKeyCryptogram(pDesKey->value))
		return(0);
	if (!UnionIsValidDesKeyCheckValue(pDesKey->checkValue))
		return(0);
	if (!UnionIsValidDesKeyFullName(pDesKey->fullName))
		return(0);
	if (!UnionIsValidDesKeyType(pDesKey->type))
		return(0);
	
	return(1);
}

int UnionIsDesKeyStillEffective(PUnionDesKey pDesKey)
{
	long	days;
		
	if (pDesKey == NULL)
		return(0);
	if (	((days = UnionDecideDaysBeforeToday(pDesKey->activeDate)) < 0) || 
		(pDesKey->maxEffectiveDays - days <= 0) || 
		((pDesKey->useTimes >= pDesKey->maxUseTimes) && (pDesKey->maxUseTimes > 0)))
		return(0);
	else
		return(1);
}

int UnionFormDefaultDesKey(PUnionDesKey pkey,char *fullName,char *keyValue,TUnionDesKeyType type,char *checkValue)
{
	char	owner[100];
	
	if ((pkey == NULL) || (fullName == NULL))
	{
		UnionUserErrLog("in UnionFormDefaultDesKey:: null parameter!\n");
		return(errCodeParameter);
	}
	memset(pkey,0,sizeof(*pkey));
	
	if (!UnionIsValidDesKeyFullName(fullName))
	{
		UnionUserErrLog("in UnionFormDefaultDesKey:: UnionIsValidDesKeyFullName [%s]!\n",fullName);
		return(errCodeParameter);
	}
	strcpy(pkey->fullName,fullName);
	
	if (keyValue != NULL)
	{
		if (!UnionIsValidDesKeyCryptogram(keyValue))
		{
			UnionUserErrLog("in UnionFormDefaultDesKey:: 1. keyValueLen = [%d] keyValue [%s]\n",strlen(keyValue),keyValue);
			return(errCodeParameter);
		}
		switch (strlen(keyValue))
		{
			case	0:
			case	16:
				pkey->length = con64BitsDesKey;
				break;
			case	32:
				pkey->length = con128BitsDesKey;
				break;
			case	48:
				pkey->length = con192BitsDesKey;
				break;
			default:
				UnionUserErrLog("in UnionFormDefaultDesKey:: 2. keyValue [%s]\n",keyValue);
				return(errCodeDesKeyDBMDL_InvalidKeyLength);
		}
		if (strlen(keyValue) != 0)
			strcpy(pkey->value,keyValue);
	}
	if (checkValue != NULL)
	{
		if (!UnionIsValidDesKeyCheckValue(checkValue))
		{
			UnionUserErrLog("in UnionFormDefaultDesKey:: checkValue [%s]\n",checkValue);
			return(errCodeParameter);
		}
		if (strlen(checkValue) != 0)
			strcpy(pkey->checkValue,checkValue);
	}
	if (!UnionIsValidDesKeyType(type))
	{
		UnionUserErrLog("in UnionFormDefaultDesKey:: type = [%d]\n",type);
		return(errCodeParameter);
	}
	pkey->type = type;
	UnionGetFullSystemDate(pkey->activeDate);
	pkey->maxEffectiveDays = 3650;
	pkey->maxUseTimes = 1000000000;
	pkey->useTimes = 0;
	pkey->windowBetweenKeyVer = 600;
	pkey->oldVerEffective = 1;
	time(&pkey->lastUpdateTime);
	memset(owner,0,sizeof(owner));
	UnionGetOwnerNameOutOfDesKeyFullName(fullName,owner);
	if (strlen(owner) < sizeof(pkey->container) - 1)
		strcpy(pkey->container,owner);
	return(0);
}

int UnionGetNameOfDesKey(TUnionDesKeyType type,char *keyName)
{
	switch (type)
	{
		case	conZPK:
			strcpy(keyName,"ZPK");
			break;
		case	conZAK:
			strcpy(keyName,"ZAK");
			break;
		case	conZMK:
			strcpy(keyName,"ZMK");
			break;
		case	conTMK:
			strcpy(keyName,"TMK");
			break;
		case	conTPK:
			strcpy(keyName,"TPK");
			break;
		case	conTAK:
			strcpy(keyName,"TAK");
			break;
		case	conPVK:
			strcpy(keyName,"PVK");
			break;
		case	conCVK:
			strcpy(keyName,"CVK");
			break;
		case	conZEK:
			strcpy(keyName,"ZEK");
			break;
		case	conWWK:
			strcpy(keyName,"WWK");
			break;
		case	conBDK:
			strcpy(keyName,"BDK");
			break;
		case	conEDK:
			strcpy(keyName,"EDK");
			break;
		case	conSelfDefinedKey:
			strcpy(keyName,"Self defined key");
			break;
		default:
			strcpy(keyName,"Invalid key type");
			break;
	}
	return(0);
}

// 从密钥名称中获取属主名称
int UnionAnalysisDesKeyFullName(char *fullName,char *appID,char *ownerName,char *keyName)
{
	int		pointNum = 0;
	int		len;
	int		i,j;
	char		*p;
	
	if (!UnionIsValidDesKeyFullName(fullName))
		return(errCodeParameter);	
	
	for (i = 0,j = 0,len = strlen(fullName); i < len; i++,j++)
	{
		if (fullName[i] == '.') 
		{
			appID[j] = 0;
			break;
		}
		else
			appID[j] = fullName[i];
	}
				
	for (j = 0,++i; i < len; i++,j++)
	{
		if (fullName[i] == '.') 
		{
			ownerName[j] = 0;
			break;
		}
		else
			ownerName[j] = fullName[i];
	}

	++i;
	memcpy(keyName,fullName+i,len - i);	
	return(0);
}

int UnionGetAppIDOutOfDesKeyFullName(char *fullName,char *appID)
{
	char	tmpBuf[100];
	
	return(UnionAnalysisDesKeyFullName(fullName,appID,tmpBuf,tmpBuf));
}

int UnionGetKeyNameOutOfDesKeyFullName(char *fullName,char *keyName)
{
	char	tmpBuf[100];
	
	return(UnionAnalysisDesKeyFullName(fullName,tmpBuf,tmpBuf,keyName));
}

int UnionGetOwnerNameOutOfDesKeyFullName(char *fullName,char *owner)
{
	char	tmpBuf[100];
	
	return(UnionAnalysisDesKeyFullName(fullName,tmpBuf,owner,tmpBuf));
}

TUnionDesKeyLength UnionConvertDesKeyLength(int keyLen)
{
	switch (keyLen)
	{
		case 16:
		case 64:
			return(con64BitsDesKey);
		case 32:
		case 128:
			return(con128BitsDesKey);
		case 48:
		case 192:
			return(con192BitsDesKey);
		default:
			return(errCodeDesKeyDBMDL_InvalidKeyLength);
	}
}

TUnionDesKeyType UnionConvertDesKeyType(char *keyTypeName)
{
	UnionToUpperCase(keyTypeName);
	if (strcmp(keyTypeName,"ZPK") == 0)
		return(conZPK);
	if (strcmp(keyTypeName,"ZAK") == 0)
		return(conZAK);
	if (strcmp(keyTypeName,"ZMK") == 0)
		return(conZMK);
	if (strcmp(keyTypeName,"ZEK") == 0)
		return(conZEK);
	if (strcmp(keyTypeName,"TPK") == 0)
		return(conTPK);
	if (strcmp(keyTypeName,"TAK") == 0)
		return(conTAK);
	if (strcmp(keyTypeName,"TMK") == 0)
		return(conTMK);
	if (strcmp(keyTypeName,"PVK") == 0)
		return(conPVK);
	if (strcmp(keyTypeName,"CVK") == 0)
		return(conCVK);
	if (strcmp(keyTypeName,"WWK") == 0)
		return(conWWK);
	if (strcmp(keyTypeName,"BDK") == 0)
		return(conBDK);
	if (strcmp(keyTypeName,"EDK") == 0)
		return(conEDK);
	if (strcmp(keyTypeName,"USER") == 0)
		return(conSelfDefinedKey);
	return(errCodeParameter);
}

TUnionSJL06LMKPairIndex UnionGetProtectLMKPairOfDesKeyType(TUnionDesKeyType type)
{
	switch (type)
	{
		case	conZPK:
			return(conLMK0607);
		case	conZAK:
			return(conLMK2627);
		case	conZMK:
			return(conLMK0405);
		case	conTMK:
		case	conTPK:
		case	conPVK:
			return(conLMK1415);
		case	conTAK:
			return(conLMK1617);
		case	conCVK:
			return(conLMK1415);
		case	conZEK:
			return(conLMK3031);
		case	conWWK:
			return(conLMK2223);
		case	conBDK:
			return(conLMK2829);
		case	conEDK:
			return(conLMK2425);
		default:
			return(errCodeParameter);
	}
}

int UnionGetNameOfDesKeyType(TUnionDesKeyType type,char *nameOfType)
{
	switch (type)
	{
		case	conZPK:
			strcpy(nameOfType,"ZPK");
			break;
		case	conZAK:
			strcpy(nameOfType,"ZAK");
			break;
		case	conZMK:
			strcpy(nameOfType,"ZMK");
			break;
		case	conTPK:
			strcpy(nameOfType,"TPK");
			break;
		case	conTAK:
			strcpy(nameOfType,"TAK");
			break;
		case	conTMK:
			strcpy(nameOfType,"TMK");
			break;
		case	conPVK:
			strcpy(nameOfType,"PVK");
			break;
		case	conCVK:
			strcpy(nameOfType,"CVK");
			break;
		case	conWWK:
			strcpy(nameOfType,"WWK");
			break;
		case	conZEK:
			strcpy(nameOfType,"ZEK");
			break;
		case	conBDK:
			strcpy(nameOfType,"BDK");
			break;
		case	conEDK:
			strcpy(nameOfType,"EDK");
			break;
		case	conSelfDefinedKey:
			strcpy(nameOfType,"USER");
			break;
		default:
			return(errCodeParameter);
	}
	return(0);
}

int UnionUseDesKey(PUnionDesKey pkey)
{
	if (pkey == NULL)
		return(errCodeParameter);
	if (!UnionIsDesKeyStillEffective(pkey))
	{
		UnionUserErrLog("in UnionUseDesKey:: UnionIsDesKeyStillEffective [%s]\n",pkey->fullName);
		return(errCodeDesKeyDBMDL_KeyOutdate);
	}
	++pkey->useTimes;
	return(0);
}
	
int UnionUpdateDesKeyValue(PUnionDesKey pkey,char *newValue,char *newCheckValue)
{
	if (pkey == NULL)
		return(-1);
	if (!UnionIsValidDesKeyCryptogram(newValue))
	{
		UnionUserErrLog("in UnionUpdateDesKeyValue:: newValue [%s] error!\n",newValue);
		return(errCodeParameter);
	}
	if (UnionConvertDesKeyLength(strlen(newValue)) != pkey->length)
	{
		UnionUserErrLog("in UnionUpdateDesKeyValue:: key [%s] newValue length [%d]!\n",pkey->fullName,strlen(newValue));
		return(errCodeParameter);
	}
	if (pkey->oldVerEffective)
	{
		memcpy(pkey->oldValue,pkey->value,sizeof(pkey->oldValue));		
		memcpy(pkey->oldCheckValue,pkey->checkValue,sizeof(pkey->oldCheckValue));
	}
	memset(pkey->value,0,sizeof(pkey->value));
	strcpy(pkey->value,newValue);
	memset(pkey->checkValue,0,sizeof(pkey->checkValue));
	strcpy(pkey->checkValue,newCheckValue);
	pkey->useTimes = 0;
	time(&(pkey->lastUpdateTime));
	UnionGetFullSystemDate(pkey->activeDate);
	return(0);
}	
