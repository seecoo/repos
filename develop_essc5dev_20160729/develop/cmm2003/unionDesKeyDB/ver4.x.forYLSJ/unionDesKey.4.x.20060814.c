//	Wolfgang Wang
//	2003/05/01
//	Version 2.0

// Wolfgang Wang, 2003/09/24, 在1.0基础上升级为2.0 删除了原创建/删除密钥对象类函数。
// 重写了：UnionIsValidDesKeyFullName

// 在2.0基础上升级为20040714,Wolfgang Wang, 2004/07/21
// 应用编号长度由用户定，密钥名称里可以包含任何字符

// 2004/11/25 在 unionDesKey.20040714.c升级为 unionDesKey3.0.20041125.c

// 2006/8/14，在unionDesKey3.0.20041125.c基础上升级为本程序

#include <stdio.h>
#include <string.h>
#include <time.h>

#ifndef _UnionDesKey_3_x_
#define _UnionDesKey_3_x_
#endif

#ifndef _UnionDesKey_4_x_
#define _UnionDesKey_4_x_
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
		case	conMKAC:
		case	conMKSMC:
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
	printf("[fullName]              [%s]\n",pDesKey->fullName);
	printf("[cryptogram]            [%s]\n",pDesKey->value);
	printf("[checkValue]            [%s]\n",pDesKey->checkValue);
	printf("[activeDate]            [%s]\n",pDesKey->activeDate);
	printf("[maxUseTimes]           [%ld]\n",pDesKey->maxUseTimes);
	printf("[useTimes]              [%ld]\n",pDesKey->useTimes);
	printf("[maxEffectiveDays]      [%ld]\n",pDesKey->maxEffectiveDays);
	printf("[lastUpdateTime]        [%ld]\n",pDesKey->lastUpdateTime);
	printf("[oldVerEffective]       [%d]\n",pDesKey->oldVerEffective);
	printf("[oldValue]              [%s]\n",pDesKey->oldValue);
	printf("[oldCheckValue]         [%s]\n",pDesKey->oldCheckValue);
	printf("[windowBetweenKeyVer]   [%d]\n",pDesKey->windowBetweenKeyVer);
	printf("[container]             [%s]\n",pDesKey->container);
	printf("[isWritingLocked]       [%d]\n",pDesKey->isWritingLocked);
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
	{
		UnionUserErrLog("in UnionIsValidDesKey:: UnionIsValidDesKeyLength!\n");
		return(0);
	}
	if (!UnionIsValidDesKeyCryptogram(pDesKey->value))
	{
		UnionUserErrLog("in UnionIsValidDesKey:: UnionIsValidDesKeyCryptogram!\n");
		return(0);
	}
	if (!UnionIsValidDesKeyCheckValue(pDesKey->checkValue))
	{
		UnionUserErrLog("in UnionIsValidDesKey:: UnionIsValidDesKeyCheckValue!\n");
		return(0);
	}
	if (!UnionIsValidDesKeyFullName(pDesKey->fullName))
	{
		UnionUserErrLog("in UnionIsValidDesKey:: UnionIsValidDesKeyFullName!\n");
		return(0);
	}
	if (!UnionIsValidDesKeyType(pDesKey->type))
	{
		UnionUserErrLog("in UnionIsValidDesKey:: UnionIsValidDesKeyType!\n");
		return(0);
	}
	
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
	pkey->maxEffectiveDays = 36500;
	pkey->maxUseTimes = -1;
	pkey->useTimes = 0;
	pkey->windowBetweenKeyVer = 600;
	pkey->oldVerEffective = 1;
	pkey->isWritingLocked = 0;
	time(&pkey->lastUpdateTime);
	memset(owner,0,sizeof(owner));
	UnionGetOwnerNameOutOfDesKeyFullName(fullName,owner);
	//UnionAuditLog("in UnionFormDefaultDesKey:: owner = [%s]\n",owner);
	if (strlen(owner) < sizeof(pkey->container) - 1)
		strcpy(pkey->container,owner);
	//UnionAuditLog("in UnionFormDefaultDesKey:: owner = [%s][%s]\n",owner,pkey->container);
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
		
// 判断某一密钥是否属于动态更新的密钥
int UnionIsDesKeyValueDynamicUpdated(PUnionDesKey pkey)
{
	char	idOfApp[40+1];
	char	varName[100+1];

	if (pkey == NULL)
		return(1);
		
	// 不是功能程序控制密钥库，即是密钥管理类程序控制密钥库	
	// if (!UnionIsProgramControlDesKeyDB())
	//	return(0);
	
	// 根据类型判断是否动态更新的密钥
	switch (pkey->type)
	{
		case	conZPK:
		case	conZAK:
		case	conZEK:
		case	conEDK:
			break;
		default:
			return(0);
	}
	
	
	memset(idOfApp,0,sizeof(idOfApp));
	UnionGetAppIDOutOfDesKeyFullName(pkey->fullName,idOfApp);
	sprintf(varName,"logUpdateZoneWKOf%s",idOfApp);
	// 判断是否定义了某应用的工作密钥不是动态更新类型
	if (UnionReadIntTypeRECVar(varName) <= 0)
		return(1);
	else
		return(0);
}

#ifndef _useSharedDesKeyPrint_
// 2008/7/17增加
// 将DES记录写入到文件中
int UnionPrintDesKeyToRecStrTxtFile(PUnionDesKey pDesKey,FILE *fp)
{
	char		recStr[2048+1];
	int		ret = 0;
	FILE		*outputFp;
		
	memset(recStr,0,sizeof(recStr));
	if ((ret = UnionPutDesKeyIntoRecStr(pDesKey,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionPrintDesKeyToRecStrTxtFile:: UnionPutDesKeyIntoRecStr!\n");
		return(ret);
	}
	if (fp == NULL)
		outputFp = stdout;
	else
		outputFp = fp;
	fprintf(outputFp,"%s\n",recStr);
	return(ret);
}	

// 2008/7/18增加
// 将一个des密钥写入到串中
int UnionPutDesKeyIntoRecStr(PUnionDesKey pDesKey,char *recStr,int sizeOfRecStr)
{
	char		tmpBuf[100+1];
	int		ret = 0,len = 0;
		
	if (pDesKey == NULL)
	{
		UnionUserErrLog("in UnionPutDesKeyIntoRecStr:: null pointer!\n");
		return(errCodeParameter);
	}

	memset(recStr,0,sizeOfRecStr);
	if ((ret = UnionPutRecFldIntoRecStr("fullName",pDesKey->fullName,strlen(pDesKey->fullName),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutDesKeyIntoRecStr:: UnionPutRecFldIntoRecStr fullName!\n");
		return(ret);
	}
	len += ret;
	if ((ret = UnionPutRecFldIntoRecStr("value",pDesKey->value,strlen(pDesKey->value),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutDesKeyIntoRecStr:: UnionPutRecFldIntoRecStr value!\n");
		return(ret);
	}
	len += ret;
	if ((ret = UnionPutRecFldIntoRecStr("checkValue",pDesKey->checkValue,strlen(pDesKey->checkValue),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutDesKeyIntoRecStr:: UnionPutRecFldIntoRecStr checkValue!\n");
		return(ret);
	}
	len += ret;
	if ((ret = UnionPutRecFldIntoRecStr("activeDate",pDesKey->activeDate,strlen(pDesKey->activeDate),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutDesKeyIntoRecStr:: UnionPutRecFldIntoRecStr activeDate!\n");
		return(ret);
	}
	len += ret;
	if ((ret = UnionPutRecFldIntoRecStr("oldValue",pDesKey->oldValue,strlen(pDesKey->oldValue),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutDesKeyIntoRecStr:: UnionPutRecFldIntoRecStr oldValue!\n");
		return(ret);
	}
	len += ret;
	if ((ret = UnionPutRecFldIntoRecStr("oldCheckValue",pDesKey->oldCheckValue,strlen(pDesKey->oldCheckValue),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutDesKeyIntoRecStr:: UnionPutRecFldIntoRecStr oldCheckValue!\n");
		return(ret);
	}
	len += ret;
	if ((ret = UnionPutRecFldIntoRecStr("container",pDesKey->container,strlen(pDesKey->container),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutDesKeyIntoRecStr:: UnionPutRecFldIntoRecStr container!\n");
		return(ret);
	}
	len += ret;
	sprintf(tmpBuf,"%ld",pDesKey->maxUseTimes);
	if ((ret = UnionPutRecFldIntoRecStr("maxUseTimes",tmpBuf,strlen(tmpBuf),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutDesKeyIntoRecStr:: UnionPutRecFldIntoRecStr maxUseTimes!\n");
		return(ret);
	}
	len += ret;
	sprintf(tmpBuf,"%ld",pDesKey->useTimes);
	if ((ret = UnionPutRecFldIntoRecStr("useTimes",tmpBuf,strlen(tmpBuf),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutDesKeyIntoRecStr:: UnionPutRecFldIntoRecStr useTimes!\n");
		return(ret);
	}
	len += ret;
	sprintf(tmpBuf,"%ld",pDesKey->maxEffectiveDays);
	if ((ret = UnionPutRecFldIntoRecStr("maxEffectiveDays",tmpBuf,strlen(tmpBuf),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutDesKeyIntoRecStr:: UnionPutRecFldIntoRecStr maxEffectiveDays!\n");
		return(ret);
	}
	len += ret;
	sprintf(tmpBuf,"%ld",pDesKey->lastUpdateTime);
	if ((ret = UnionPutRecFldIntoRecStr("lastUpdateTime",tmpBuf,strlen(tmpBuf),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutDesKeyIntoRecStr:: UnionPutRecFldIntoRecStr lastUpdateTime!\n");
		return(ret);
	}
	len += ret;
	sprintf(tmpBuf,"%ld",pDesKey->oldVerEffective);
	if ((ret = UnionPutRecFldIntoRecStr("oldVerEffective",tmpBuf,strlen(tmpBuf),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutDesKeyIntoRecStr:: UnionPutRecFldIntoRecStr oldVerEffective!\n");
		return(ret);
	}
	len += ret;
	sprintf(tmpBuf,"%ld",pDesKey->windowBetweenKeyVer);
	if ((ret = UnionPutRecFldIntoRecStr("windowBetweenKeyVer",tmpBuf,strlen(tmpBuf),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutDesKeyIntoRecStr:: UnionPutRecFldIntoRecStr windowBetweenKeyVer!\n");
		return(ret);
	}
	len += ret;
#ifdef _UnionDesKey_4_x_
	sprintf(tmpBuf,"%ld",pDesKey->isWritingLocked);
#else
	sprintf(tmpBuf,"0");
#endif
	if ((ret = UnionPutRecFldIntoRecStr("isWritingLocked",tmpBuf,strlen(tmpBuf),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutDesKeyIntoRecStr:: UnionPutRecFldIntoRecStr isWritingLocked!\n");
		return(ret);
	}
	len += ret;
	sprintf(tmpBuf,"%ld",pDesKey->length);
	if ((ret = UnionPutRecFldIntoRecStr("length",tmpBuf,strlen(tmpBuf),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutDesKeyIntoRecStr:: UnionPutRecFldIntoRecStr length!\n");
		return(ret);
	}
	len += ret;
	sprintf(tmpBuf,"%ld",pDesKey->type);
	if ((ret = UnionPutRecFldIntoRecStr("type",tmpBuf,strlen(tmpBuf),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutDesKeyIntoRecStr:: UnionPutRecFldIntoRecStr type!\n");
		return(ret);
	}
	len += ret;
	return(len);
}

// 2008/7/18增加
// 从一个串中读密钥
int UnionReadDesKeyFromRecStr(char *recStr,int lenOfRecStr,PUnionDesKey pDesKey)
{
	char		tmpBuf[100+1];
	int		ret = 0;
		
	if (pDesKey == NULL)
	{
		UnionUserErrLog("in UnionReadDesKeyFromRecStr:: null pointer!\n");
		return(errCodeParameter);
	}

	memset(pDesKey,0,sizeof(*pDesKey));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"fullName",pDesKey->fullName,sizeof(pDesKey->fullName))) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromRecStr:: UnionReadRecFldFromRecStr fullName!\n");
		return(ret);
	}
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"value",pDesKey->value,sizeof(pDesKey->value))) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromRecStr:: UnionReadRecFldFromRecStr value!\n");
		return(ret);
	}
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"checkValue",pDesKey->checkValue,sizeof(pDesKey->checkValue))) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromRecStr:: UnionReadRecFldFromRecStr checkValue!\n");
		return(ret);
	}
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"activeDate",pDesKey->activeDate,sizeof(pDesKey->activeDate))) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromRecStr:: UnionReadRecFldFromRecStr activeDate!\n");
		return(ret);
	}
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"oldValue",pDesKey->oldValue,sizeof(pDesKey->oldValue))) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromRecStr:: UnionReadRecFldFromRecStr oldValue!\n");
		return(ret);
	}
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"oldCheckValue",pDesKey->oldCheckValue,sizeof(pDesKey->oldCheckValue))) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromRecStr:: UnionReadRecFldFromRecStr oldCheckValue!\n");
		return(ret);
	}
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"container",pDesKey->container,sizeof(pDesKey->container))) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromRecStr:: UnionReadRecFldFromRecStr container!\n");
		return(ret);
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"maxUseTimes",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromRecStr:: UnionReadRecFldFromRecStr maxUseTimes!\n");
		return(ret);
	}
	pDesKey->maxUseTimes = atol(tmpBuf);
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"useTimes",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromRecStr:: UnionReadRecFldFromRecStr useTimes!\n");
		return(ret);
	}
	pDesKey->useTimes = atol(tmpBuf);
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"maxEffectiveDays",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromRecStr:: UnionReadRecFldFromRecStr maxEffectiveDays!\n");
		return(ret);
	}
	pDesKey->maxEffectiveDays = atol(tmpBuf);
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"lastUpdateTime",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromRecStr:: UnionReadRecFldFromRecStr lastUpdateTime!\n");
		return(ret);
	}
	pDesKey->lastUpdateTime = atol(tmpBuf);
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"oldVerEffective",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromRecStr:: UnionReadRecFldFromRecStr oldVerEffective!\n");
		return(ret);
	}
	pDesKey->oldVerEffective = atoi(tmpBuf);
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"windowBetweenKeyVer",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromRecStr:: UnionReadRecFldFromRecStr windowBetweenKeyVer!\n");
		return(ret);
	}
	pDesKey->windowBetweenKeyVer = atoi(tmpBuf);
#ifdef _UnionDesKey_4_x_
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"isWritingLocked",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromRecStr:: UnionReadRecFldFromRecStr isWritingLocked!\n");
		return(ret);
	}
	pDesKey->isWritingLocked = atoi(tmpBuf);
#endif
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"length",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromRecStr:: UnionReadRecFldFromRecStr length!\n");
		return(ret);
	}
	pDesKey->length = atoi(tmpBuf);
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"type",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadDesKeyFromRecStr:: UnionReadRecFldFromRecStr type!\n");
		return(ret);
	}
	pDesKey->type = atoi(tmpBuf);
	return(0);
}
#endif
