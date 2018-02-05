//	Wolfgang Wang
//	2003/05/01
//	Version 2.0

// Wolfgang Wang, 2003/09/24, 在1.0基础上升级为2.0 删除了原创建/删除密钥对象类函数。
// 重写了：UnionIsValidDesKeyFullName

#include <stdio.h>
#include <string.h>

#include "unionKeyDB.h"
#include "unionDesKey.h"
#include "UnionLog.h"

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
		case	conDesKeyAttrPassiveDate:
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
		if (!(isdigit(fullName[i]) || isalpha(fullName[i]) || (fullName[i] == '_') || (fullName[i] == '.') || (fullName[i] == '-')))
			return(0);
		if (fullName[i] != '.')
			continue;
		pointNum++;
		i++;
		if (i >= len)
			return(0);
		if (fullName[i] == '.')
			return(0);
	}
	
	if (pointNum != 2)
		return(0);
	else
		return(1);
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
		case	conSelfDefinedKey:
			return(1);
		default:
			return(0);
	}
}

int UnionPrintDesKey(PUnionDesKey pDesKey)
{
	char	keyName[40];
	
	if (pDesKey == NULL)
	{
		UnionUserErrLog("in UnionPrintDesKey:: null pointer!\n");
		return(-1);
	}
	printf("[fullName]		[%s]\n",pDesKey->fullName);
	printf("[cryptogram]		[%s]\n",pDesKey->value);
	printf("[checkValue]		[%s]\n",pDesKey->checkValue);
	printf("[activeDate]		[%s]\n",pDesKey->activeDate);
	printf("[passiveDate]		[%s]\n",pDesKey->passiveDate);
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
	char	currentDate[10+1];
	
	if (pDesKey == NULL)
		return(1);
	memset(currentDate,0,sizeof(currentDate));
	UnionGetFullSystemDate(currentDate);
	if ((strncmp(currentDate,pDesKey->passiveDate,8) >= 0) || (strncmp(currentDate,pDesKey->activeDate,8) < 0))
		return(0);
	else
		return(1);
}

int UnionFormDefaultDesKey(PUnionDesKey pkey,char *fullName,char *keyValue,TUnionDesKeyType type,char *checkValue)
{
	if ((pkey == NULL) || (fullName == NULL))
	{
		UnionUserErrLog("in UnionFormDefaultDesKey:: null parameter!\n");
		return(-1);
	}
	memset(pkey,0,sizeof(*pkey));
	if (strlen(fullName) > sizeof(pkey->fullName) - 1)
	{
		UnionUserErrLog("in UnionFormDefaultDesKey:: fullName [%s] too long!\n",fullName);
		return(-1);
	}
	strcpy(pkey->fullName,fullName);
	
	if (keyValue != NULL)
	{
		if (UnionIsValidDesKeyCryptogram(keyValue))
		{
			UnionUserErrLog("in UnionFormDefaultDesKey:: keyValue [%s]\n",keyValue);
			return(-1);
		}
		switch (strlen(keyValue))
		{
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
				UnionUserErrLog("in UnionFormDefaultDesKey:: keyValue [%s]\n",keyValue);
				return(-1);
		}
		strcpy(pkey->value,keyValue);
	}
	if (checkValue != NULL)
	{
		if (!UnionIsValidDesKeyCheckValue(checkValue))
		{
			UnionUserErrLog("in UnionFormDefaultDesKey:: checkValue [%s]\n",checkValue);
			return(-1);
		}
		strcpy(pkey->checkValue,checkValue);
	}
	if (!UnionIsValidDesKeyType(type))
	{
		UnionUserErrLog("in UnionFormDefaultDesKey:: type = [%d]\n",type);
		return(-1);
	}
	pkey->type = type;
	UnionGetFullSystemDate(pkey->activeDate);
	memcpy(pkey->passiveDate,"21000101",8);
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
	int		i;
	char		*p;
	
	if (!UnionIsValidDesKeyFullName(fullName))
		return(-1);	
	
	memcpy(appID,fullName,2);
	for (i = 3,len = strlen(fullName); i < len; i++)
	{
		if (fullName[i] == '.') 
		{
			memcpy(ownerName,fullName+3,i-3);
			memcpy(keyName,fullName+i+1,len - i - 1);
			return(0);
		}
	}
	return(-1);
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
			return(-1);
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
	if (strcmp(keyTypeName,"USER") == 0)
		return(conSelfDefinedKey);
	return(-1);
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
		default:
			return(-1);
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
		case	conBDK:
			strcpy(nameOfType,"BDK");
			break;
		case	conSelfDefinedKey:
			strcpy(nameOfType,"USER");
			break;
		default:
			return(-1);
	}
	return(0);
}

