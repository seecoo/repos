//	Wolfgang Wang
//	2004/11/11

#include <stdio.h>
#include <string.h>

#include "unionPKDB.h"
#include "unionPK.h"
#include "UnionLog.h"

int UnionIsValidPKLength(TUnionPKLength len)
{
	switch (len)
	{
		case	con512RSAPair:
		case	con1024RSAPair:
		case	con2048RSAPair:
		case	con4096RSAPair:
			return(1);
		default:
			return(0);
	}
}

// 判断是否是合法的密钥属性
int UnionIsValidPKAttribute(TUnionPKAttribute attr)
{
	switch (attr)
	{
		case	conPKAttrValue:
		case	conPKAttrCheckValue:
		case	conPKAttrLength:
		//case	conPKAttrProtectMethod:
		case	conPKAttrType:
		case	conPKAttrActiveDate:
		case	conPKAttrPassiveDate:
			return(1);
		default:
			return(0);
	}
}

// 判断是否是合法的密钥密文
int UnionIsValidPKValue(char *keyCryptogram)
{
	int	len;
	int	i;
	
	switch (len = strlen(keyCryptogram))
	{
		case	128:
		case	256:
		case	512:
		case	1024:
			break;
		case	0:
			return(1);
		default:
			return(0);
	}
		
	for (i = 0; i < len; i++)
	{
		keyCryptogram[i] = toupper(keyCryptogram[i]);
	}
	return(UnionIsBCDStr(keyCryptogram));
}

// 判断是否是合法的密钥校验值
int UnionIsValidPKCheckValue(char *checkValue)
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
int UnionIsValidPKFullName(char *fullName)
{
	PUnionPK	pPK;
	int		pointNum = 0;
	int		len;
	int		i;
	
	if ((len = strlen(fullName)) > sizeof(pPK->fullName) - 1)
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
int UnionIsValidPKType(TUnionPKType type)
{
	switch (type)
	{
		case	conSignature:
		case	conEncryption:
		case	conSelfDefinedPK:
			return(1);
		default:
			return(0);
	}
}

int UnionPrintPK(PUnionPK pPK)
{
	char	keyName[40];
	char	tmpBuf[2048+1];
		
	if (pPK == NULL)
	{
		UnionUserErrLog("in UnionPrintPK:: null pointer!\n");
		return(-1);
	}
	printf("[fullName]		[%s]\n",pPK->fullName);
	memset(tmpBuf,0,sizeof(tmpBuf));
	bcdhex_to_aschex(pPK->value,UnionGetAscCharLengthOfPK(pPK->length)/2,tmpBuf);
	if (UnionIsStringContainingOnlyZero(tmpBuf,UnionGetAscCharLengthOfPK(pPK->length)))
		printf("[cryptogram]		[]\n");
	else
		printf("[cryptogram]		[%s]\n",tmpBuf);
	printf("[checkValue]		[%s]\n",pPK->checkValue);
	printf("[activeDate]		[%s]\n",pPK->activeDate);
	printf("[passiveDate]		[%s]\n",pPK->passiveDate);
	printf("this is a public key for [%d] bits RSA pair.\n",UnionGetBitsLengthOfRSAPair(pPK->length));
	memset(keyName,0,sizeof(keyName));
	UnionGetNameOfPK(pPK->type,keyName);
	printf("this is a %s PK!\n",keyName);
		
	return(0);
}

int UnionIsValidPK(PUnionPK pPK)
{
	char	tmpBuf[2048+1];
	
	if (pPK == NULL)
	{
		UnionUserErrLog("in UnionIsValidPK:: null pointer!\n");
		return(0);
	}

	if (UnionIsValidPKLength(pPK->length) &&
		UnionIsValidPKCheckValue(pPK->checkValue) &&
		UnionIsValidPKFullName(pPK->fullName) &&
		UnionIsValidPKType(pPK->type))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		bcdhex_to_aschex(pPK->value,UnionGetAscCharLengthOfPK(pPK->length)/2,tmpBuf);
		if (UnionIsValidPKValue(tmpBuf))
			return(1);
		else
		{
			UnionUserErrLog("in UnionIsValidPK:: pk length = [%04d]\npk = [%s]\n",UnionGetAscCharLengthOfPK(pPK->length),tmpBuf);
			return(0);
		}
	}
	else
		return(0);
}

int UnionIsPKStillEffective(PUnionPK pPK)
{
	char	currentDate[10+1];
	
	if (pPK == NULL)
		return(0);
	memset(currentDate,0,sizeof(currentDate));
	UnionGetFullSystemDate(currentDate);
	if ((strncmp(currentDate,pPK->passiveDate,8) >= 0) || (strncmp(currentDate,pPK->activeDate,8) < 0))
		return(0);
	else
		return(1);
}

int UnionFormDefaultPK(PUnionPK pkey,char *fullName,char *keyValue,TUnionPKType type,char *checkValue)
{
	if ((pkey == NULL) || (fullName == NULL))
	{
		UnionUserErrLog("in UnionFormDefaultPK:: null parameter!\n");
		return(-1);
	}
	memset(pkey,0,sizeof(*pkey));
	if (strlen(fullName) > sizeof(pkey->fullName) - 1)
	{
		UnionUserErrLog("in UnionFormDefaultPK:: fullName [%s] too long!\n",fullName);
		return(-1);
	}
	strcpy(pkey->fullName,fullName);
	
	if (keyValue != NULL)
	{
		if (UnionIsValidPKValue(keyValue))
		{
			UnionUserErrLog("in UnionFormDefaultPK:: keyValue [%s]\n",keyValue);
			return(-1);
		}
		switch (strlen(keyValue))
		{
			case	128:
				pkey->length = con512RSAPair;
				break;
			case	256:
				pkey->length = con1024RSAPair;
				break;
			case	512:
				pkey->length = con2048RSAPair;
				break;
			case	1024:
				pkey->length = con4096RSAPair;
				break;
			default:
				UnionUserErrLog("in UnionFormDefaultPK:: keyValue [%s]\n",keyValue);
				return(-1);
		}
		aschex_to_bcdhex(keyValue,UnionGetAscCharLengthOfPK(pkey->length),pkey->value);
	}
	if (checkValue != NULL)
	{
		if (!UnionIsValidPKCheckValue(checkValue))
		{
			UnionUserErrLog("in UnionFormDefaultPK:: checkValue [%s]\n",checkValue);
			return(-1);
		}
		strcpy(pkey->checkValue,checkValue);
	}
	if (!UnionIsValidPKType(type))
	{
		UnionUserErrLog("in UnionFormDefaultPK:: type = [%d]\n",type);
		return(-1);
	}
	pkey->type = type;
	UnionGetFullSystemDate(pkey->activeDate);
	memcpy(pkey->passiveDate,"21000101",8);
	return(0);
}

int UnionGetNameOfPK(TUnionPKType type,char *keyName)
{
	switch (type)
	{
		case	conSignature:
			strcpy(keyName,"Signature");
			break;
		case	conEncryption:
			strcpy(keyName,"Encryption");
			break;
		case	conSelfDefinedPK:
			strcpy(keyName,"Self defined key");
			break;
		default:
			strcpy(keyName,"Invalid key type");
			break;
	}
	return(0);
}

// 从密钥名称中获取属主名称
int UnionAnalysisPKFullName(char *fullName,char *appID,char *ownerName,char *keyName)
{
	int		pointNum = 0;
	int		len;
	int		i,j;
	char		*p;
	
	if (!UnionIsValidPKFullName(fullName))
		return(-1);	
	
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

int UnionGetAppIDOutOfPKFullName(char *fullName,char *appID)
{
	char	tmpBuf[100];
	
	return(UnionAnalysisPKFullName(fullName,appID,tmpBuf,tmpBuf));
}

int UnionGetKeyNameOutOfPKFullName(char *fullName,char *keyName)
{
	char	tmpBuf[100];
	
	return(UnionAnalysisPKFullName(fullName,tmpBuf,tmpBuf,keyName));
}

int UnionGetOwnerNameOutOfPKFullName(char *fullName,char *owner)
{
	char	tmpBuf[100];
	
	return(UnionAnalysisPKFullName(fullName,tmpBuf,owner,tmpBuf));
}

TUnionPKLength UnionConvertPKLength(int keyLen)
{
	switch (keyLen)
	{
		case 128:
			return(con512RSAPair);
		case 256:
			return(con1024RSAPair);
		case 512:
			return(con2048RSAPair);
		case 1024:
			return(con4096RSAPair);
		default:
			return(-1);
	}
}

TUnionPKType UnionConvertPKType(char *keyTypeName)
{
	UnionToUpperCase(keyTypeName);
	if (strcmp(keyTypeName,"SIGNATURE") == 0)
		return(conSignature);
	if (strcmp(keyTypeName,"ENCRYPTION") == 0)
		return(conEncryption);
	if (strcmp(keyTypeName,"USER") == 0)
		return(conSelfDefinedPK);
	return(-1);
}

int UnionGetNameOfPKType(TUnionPKType type,char *nameOfType)
{
	switch (type)
	{
		case	conSignature:
			strcpy(nameOfType,"Signature");
			break;
		case	conEncryption:
			strcpy(nameOfType,"Encryption");
			break;
		case	conSelfDefinedPK:
			strcpy(nameOfType,"User Defined");
			break;
		default:
			return(-1);
	}
	return(0);
}


int UnionGetAscCharLengthOfPK(TUnionPKLength length)
{
	switch (length)
	{
		case	con512RSAPair:
			return(128);
		case	con1024RSAPair:
			return(256);
		case	con2048RSAPair:
			return(512);
		case	con4096RSAPair:
			return(1024);
		default:
			return(0);
	}
}

int UnionGetBitsLengthOfRSAPair(TUnionPKLength length)
{
	switch (length)
	{
		case	con512RSAPair:
			return(512);
		case	con1024RSAPair:
			return(1024);
		case	con2048RSAPair:
			return(2048);
		case	con4096RSAPair:
			return(4096);
		default:
			return(-1);
	}
}
