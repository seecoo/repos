// Author:	Wolfgang Wang
// Date:	2003/10/09

// 2004-3-9，陈家梅，在1.0版本的基础上修改，增加了拼装以下指令的函数
// DG、CA、JE、JC、JG、JA、EE、DE、BA、NG、DA、EA、DC、EC、BC、BE、CW、CY、EW、EY

#define _RacalCmdForNewRacal_

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "sjl06.h"
#include "sjl06Protocol.h"
#include "sjl06Cmd.h"
#include "3DesRacalSyntaxRules.h"
#include "unionDesKey.h"
#include "unionSJL06API.h"
#include "unionErrCode.h"
#include "UnionLog.h"


// 将一个ZMK加密的密钥转换为LMK加密
int RacalCmdA6(PUnionSJL06Server psjl06Server,
	TUnionDesKeyType keyType,char *zmk,char *keyByZmk,
	char *keyByLmk,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	int		keyByLmkStrLen;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"A6",2);
	hsmCmdLen = 2;
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA6:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionPutKeyIntoRacalKeyString(zmk,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA6:: UnionPutKeyIntoRacalKeyString [%s]!\n",zmk);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionPutKeyIntoRacalKeyString(keyByZmk,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-1-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA6:: UnionPutKeyIntoRacalKeyString [%s]!\n",keyByZmk);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionCaculateHsmKeyKeyScheme(strlen(keyByZmk),hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA6:: UnionCaculateHsmKeyKeyScheme [%s]!\n",keyByZmk);
		return(ret);
	}
	hsmCmdLen++;
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdA6:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"A7",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdA6:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdA6:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	if ((keyByLmkStrLen = UnionReadKeyFromRacalKeyString(hsmCmdBuf+4,ret-4,keyByLmk)) < 0)
	{
		UnionUserErrLog("in RacalCmdA6:: UnionReadKeyFromRacalKeyString! hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(keyByLmkStrLen);
	}
	if (keyByLmkStrLen >= ret)	// 没有校验值
		return(0);
	if (ret - keyByLmkStrLen > 16)
		memcpy(checkValue,hsmCmdBuf+4+keyByLmkStrLen,16);
	else
		memcpy(checkValue,hsmCmdBuf+4+keyByLmkStrLen,ret-keyByLmkStrLen-4);
	return(0);
}

// 将ZMK加密的ZPK转换为LMK加密
int RacalCmdFA(PUnionSJL06Server psjl06Server,char *zmk,char *zpkByZmk,int variant,char keyLenFlag,char *zpkByLmk,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	int		offset;
	int		len;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"FA",2);
	hsmCmdLen = 2;
	if ((ret = UnionPutKeyIntoRacalKeyString(zmk,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdFA:: UnionPutKeyIntoRacalKeyString zmk [%s]error!\n",zmk);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionPutKeyIntoRacalKeyString(zpkByZmk,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdFA:: UnionPutKeyIntoRacalKeyString zpkByZmk [%s]error!\n",zpkByZmk);
		return(ret);
	}
	hsmCmdLen += ret;
	if (variant > 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%02d",variant % 100);
		hsmCmdLen += 2;
	}
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen++;
	hsmCmdBuf[hsmCmdLen] = keyLenFlag;
	hsmCmdLen++;
	hsmCmdBuf[hsmCmdLen] = keyLenFlag;
	hsmCmdLen++;
	hsmCmdBuf[hsmCmdLen] = '0';
	hsmCmdLen++;
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdFA:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"FB",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdFA:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdFA:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	offset = 4;
	if ((len = UnionReadKeyFromRacalKeyString(hsmCmdBuf+offset,ret-offset,zpkByLmk)) < 0)
	{
		UnionUserErrLog("in RacalCmdFA:: UnionReadKeyFromRacalKeyString [%s]!\n",hsmCmdBuf);
		return(len);
	}
	UnionDebugLog("in RacalCmdFA:: zpkByLmk = [%s]\n",zpkByLmk);
	offset += len;
	if (offset + 16 > ret)
		ret = ret - offset;
	else
		ret = 16;
	if (ret < 0)
		ret = 0;
	memcpy(checkValue,hsmCmdBuf+offset,ret);
	return(strlen(zpkByZmk));
}

// 生成ZAK/ZEK
int RacalCmdFI(PUnionSJL06Server psjl06Server,char keyFlag,char *zmk,int variant,char keyLenFlag,
	char *keyByZmk,char *keyByLmk,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	int		offset;
	int		len;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"FI",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = keyFlag;
	hsmCmdLen++;
	if ((ret = UnionPutKeyIntoRacalKeyString(zmk,hsmCmdBuf+hsmCmdLen,sizeof(hsmCmdBuf)-hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdFI:: UnionPutKeyIntoRacalKeyString zmk [%s]error!\n",zmk);
		return(ret);
	}
	hsmCmdLen += ret;
	if (variant > 0)
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%02d",variant % 100);
		hsmCmdLen += 2;
	}
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen++;
	hsmCmdBuf[hsmCmdLen] = keyLenFlag;
	hsmCmdLen++;
	hsmCmdBuf[hsmCmdLen] = keyLenFlag;
	hsmCmdLen++;
	hsmCmdBuf[hsmCmdLen] = '0';
	hsmCmdLen++;
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdFI:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"FJ",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdFI:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdFI:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	offset = 4;
	if ((len = UnionReadKeyFromRacalKeyString(hsmCmdBuf+offset,ret-offset,keyByZmk)) < 0)
	{
		UnionUserErrLog("in RacalCmdFI:: UnionReadKeyFromRacalKeyString [%s]!\n",hsmCmdBuf);
		return(len);
	}
	offset += len;
	if ((len = UnionReadKeyFromRacalKeyString(hsmCmdBuf+offset,ret-offset,keyByLmk)) < 0)
	{
		UnionUserErrLog("in RacalCmdFI:: UnionReadKeyFromRacalKeyString [%s]!\n",hsmCmdBuf);
		return(len);
	}
	offset += len;
	if (offset + 16 > ret)
		ret = ret - offset;
	else
		ret = 16;
	if (ret < 0)
		ret = 0;
	memcpy(checkValue,hsmCmdBuf+offset,ret);
	return(strlen(keyByZmk));
}

// 20051206，王纯军增加
// 将ZMK加密的密钥转换为LMK加密的ZAK/ZEK
int RacalCmdFK(PUnionSJL06Server psjl06Server,char type,char *zmk,char *keyByZMK,char *keyByLMK,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	int		keyLen;
	int		offset;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"FK",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = type;
	hsmCmdLen++;
	switch (keyLen = strlen(zmk))
	{
		case	16:
			break;
		case	32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case	48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in RacalCmdFK:: zmk [%s] length error!\n",zmk);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,zmk,keyLen);
	hsmCmdLen += keyLen;
	switch (keyLen = strlen(keyByZMK))
	{
		case	16:
			break;
		case	32:
			memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
			hsmCmdLen++;
			break;
		case	48:
			memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
			hsmCmdLen++;
			break;
		default:
			UnionUserErrLog("in RacalCmdFK:: keyByZMK [%s] length error!\n",keyByZMK);
			return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,keyByZMK,keyLen);
	hsmCmdLen += keyLen;
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen++;
	switch (keyLen)
	{
		case	16:
			memcpy(hsmCmdBuf+hsmCmdLen,"ZZ0",3);
			break;
		case	32:
			memcpy(hsmCmdBuf+hsmCmdLen,"XX0",3);
			break;
		case	48:
			memcpy(hsmCmdBuf+hsmCmdLen,"YY0",3);
			break;
	}
	hsmCmdLen += 3;
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdFK:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"FL",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdFK:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdFK:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	switch (keyLen)
	{
		case	16:
			offset = 0;
			break;
		case	32:
		case	48:
			offset = 1;
			break;
		default:
			UnionUserErrLog("in RacalCmdFK:: keyByZMK [%s] length error!\n",keyByZMK);
			return(errCodeParameter);
	}
	if (offset + keyLen + 4 > ret)
	{
		UnionUserErrLog("in RacalCmdFK:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(keyByLMK,hsmCmdBuf+4+offset,keyLen);
	memcpy(checkValue,hsmCmdBuf+4+offset+keyLen,ret-4-offset-keyLen);
	return(keyLen);
}

int RacalCmdA0(PUnionSJL06Server psjl06Server,
	int outputByZMK,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,
	char *zmk,char *keyByLMK,char *keyByZMK,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	int		zmkLength,keyLen;
	int		offsetOfKeyByLMK,offsetOfKeyByZMK,offsetOfCheckValue;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"A0",2);
	hsmCmdLen = 2;
	if (outputByZMK)
		hsmCmdBuf[hsmCmdLen] = '1';
	else
		hsmCmdBuf[hsmCmdLen] = '0';
	hsmCmdLen += 1;
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA0:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA0:: UnionTranslateHsmKeyKeyScheme!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	if (outputByZMK)
	{
		/*
		if ((ret = UnionCaculateHsmKeyKeyScheme(zmkLength=strlen(zmk),hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in RacalCmdA0:: UnionCaculateHsmKeyKeyScheme! zmk=[%s]zmkLen=[%d]\n",zmk,strlen(zmk));
			return(ret);
		}
		*/
		switch (zmkLength=strlen(zmk))
		{
			case	16:
				ret = 0;
				break;
			case	32:
				memcpy(hsmCmdBuf+hsmCmdLen,"X",1);
				ret = 1;
				break;
			case	48:
				memcpy(hsmCmdBuf+hsmCmdLen,"Y",1);
				ret = 1;
				break;
			default:
				UnionUserErrLog("in RacalCmdA0:: UnionCaculateHsmKeyKeyScheme! zmk = [%s] zmkLen = [%d]\n",zmk,zmkLength);
				return(errCodeParameter);
		}
		hsmCmdLen += ret;
		memcpy(hsmCmdBuf+hsmCmdLen,zmk,zmkLength);
		hsmCmdLen += zmkLength;
		if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
		//if ((ret = UnionCaculateHsmKeyKeyScheme(zmkLength,hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in RacalCmdA0:: UnionTranslateHsmKeyKeyScheme!\n");
			//UnionUserErrLog("in RacalCmdA0:: UnionCaculateHsmKeyKeyScheme!\n");
			return(ret);
		}
		hsmCmdLen += ret;
	}		

	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdA0:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"A1",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdA0:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdA0:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}

	offsetOfKeyByZMK = -1;
	switch (keyLength)
	{
		case	con64BitsDesKey:
			offsetOfKeyByLMK = 4;
			if (outputByZMK)
			{
				offsetOfKeyByZMK = 4 + 16;
				offsetOfCheckValue = 4 + 16 + 16;
			}
			else
				offsetOfCheckValue = 4 + 16;
			keyLen = 16;
			break;
			/*if (outputByZMK)
			{
				memcpy(keyByZMK,hsmCmdBuf+4,16);
				memcpy(keyByLMK,hsmCmdBuf+4+16,16);
				memcpy(checkValue,hsmCmdBuf+4+16+16,6);
			}
			else
			{
				memcpy(keyByLMK,hsmCmdBuf+4,16);
				memcpy(checkValue,hsmCmdBuf+4+16,6);
			}
			return(16);
			*/
		case	con128BitsDesKey:
			offsetOfKeyByLMK = 4 + 1;
			if (outputByZMK)
			{
				offsetOfKeyByZMK = 4 + 1 + 32 + 1;
				offsetOfCheckValue = 4 + 1 + 32 + 1 + 32;
			}
			else
				offsetOfCheckValue = 4 + 1 + 32;
			keyLen = 32;
			break;
			/*
			if (outputByZMK)
			{
				memcpy(keyByZMK,hsmCmdBuf+4+1,32);
				memcpy(keyByLMK,hsmCmdBuf+4+1+32+1,32);
				memcpy(checkValue,hsmCmdBuf+4+1+32+1+32,6);
			}
			else
			{
				memcpy(keyByLMK,hsmCmdBuf+4+1,32);
				memcpy(checkValue,hsmCmdBuf+4+1+32,6);
			}
			return(32);
			*/
		case	con192BitsDesKey:
			offsetOfKeyByLMK = 4 + 1;
			if (outputByZMK)
			{
				offsetOfKeyByZMK = 4 + 1 + 48 + 1;
				offsetOfCheckValue = 4 + 1 + 48 + 1 + 48;
			}
			else
				offsetOfCheckValue = 4 + 1 + 48;
			keyLen = 48;
			break;		// Mary add, 2004-4-12
			/*
			if (outputByZMK)
			{
				memcpy(keyByZMK,hsmCmdBuf+4+1,48);
				memcpy(keyByLMK,hsmCmdBuf+4+1+48+1,48);
				memcpy(checkValue,hsmCmdBuf+4+1+48+1+48,6);
			}
			else
			{
				memcpy(keyByLMK,hsmCmdBuf+4+1,48);
				memcpy(checkValue,hsmCmdBuf+4+1+48,6);
			}
			return(48);
			*/
		default:
			UnionUserErrLog("in RacalCmdA0:: invalid keyLength type [%d]\n",keyLength);
			return(errCodeParameter);
	}
	memcpy(keyByLMK,hsmCmdBuf+offsetOfKeyByLMK,keyLen);
	if (outputByZMK)
		memcpy(keyByZMK,hsmCmdBuf+offsetOfKeyByZMK,keyLen);
	memcpy(checkValue,hsmCmdBuf+offsetOfCheckValue,6);
	return(keyLen);
}

int RacalCmd0A(PUnionSJL06Server psjl06Server,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"0A",2);
	hsmCmdLen = 2;

	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmd0A:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"0B",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmd0A:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	return(0);
}

int RacalCmdRA(PUnionSJL06Server psjl06Server,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"RA",2);
	hsmCmdLen = 2;

	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdRA:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"RB",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdRA:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	return(0);
}

int RacalCmdPA(PUnionSJL06Server psjl06Server,char *format,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024+1];
	int		hsmCmdLen = 0;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"PA",2);
	hsmCmdLen = 2;
	strcpy(hsmCmdBuf+hsmCmdLen,format);
	hsmCmdLen += strlen(format);
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdPA:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"PB",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdPA:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	return(0);
}

int RacalCmdA2(PUnionSJL06Server psjl06Server,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int fldNum,char fld[][80],char *component,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	int		i;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"A2",2);
	hsmCmdLen = 2;
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA2:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	// Mary add begin, 2004-4-15
	if (memcmp(hsmCmdBuf+hsmCmdLen,"000",3) == 0)
		memcpy(hsmCmdBuf+hsmCmdLen,"100",3);
	// Mary add end, 2004-4-15
	hsmCmdLen += ret;
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA2:: UnionTranslateHsmKeyKeyScheme!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	for (i = 0; i < fldNum; i++)
	{
		if (i > 0)
		{
			hsmCmdBuf[hsmCmdLen] = ';';
			hsmCmdLen++;
		}
		//printf("fld[%d] = [%s]\n",i,fld[i]);
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",fld[i]);
		hsmCmdLen += strlen(fld[i]);
	}	

	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdA2:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"A3",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdA2:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdA2:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}

	switch (keyLength)
	{
		case	con64BitsDesKey:
			memcpy(component,hsmCmdBuf+4,16);
			return(16);
		case	con128BitsDesKey:
			memcpy(component,hsmCmdBuf+4+1,32);
			return(32);
		case	con192BitsDesKey:
			memcpy(component,hsmCmdBuf+4+1,48);
			return(48);
		default:
			UnionUserErrLog("in RacalCmdA2:: unknown key length!\n");
			return(errCodeParameter);
	}
}

int RacalCmdA2SpecForZmk(PUnionSJL06Server psjl06Server,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int fldNum,char fld[][80],char *component,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	int		i;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"A2",2);
	hsmCmdLen = 2;
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA2SpecForZmk:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA2SpecForZmk:: UnionTranslateHsmKeyKeyScheme!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	for (i = 0; i < fldNum; i++)
	{
		if (i > 0)
		{
			hsmCmdBuf[hsmCmdLen] = ';';
			hsmCmdLen++;
		}
		//printf("fld[%d] = [%s]\n",i,fld[i]);
		sprintf(hsmCmdBuf+hsmCmdLen,"%s",fld[i]);
		hsmCmdLen += strlen(fld[i]);
	}	

	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdA2SpecForZmk:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"A3",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdA2SpecForZmk:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdA2SpecForZmk:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}

	switch (keyLength)
	{
		case	con64BitsDesKey:
			memcpy(component,hsmCmdBuf+4,16);
			return(16);
		case	con128BitsDesKey:
			memcpy(component,hsmCmdBuf+4+1,32);
			return(32);
		case	con192BitsDesKey:
			memcpy(component,hsmCmdBuf+4+1,48);
			return(48);
		default:
			UnionUserErrLog("in RacalCmdA2SpecForZmk:: unknown key length!\n");
			return(errCodeParameter);
	}
}

int RacalCmdNC(PUnionSJL06Server psjl06Server,char *version,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	int		i;
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"NC",2);
	hsmCmdLen = 2;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdNC:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"ND",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdNC:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdNC:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(version,hsmCmdBuf+4,ret - 4);
	return(ret - 4);
}

int RacalCmdCC(PUnionSJL06Server psjl06Server,int zpk1Length,char *zpk1,int zpk2Length,char * zpk2,int maxPinLen,char *pinFormat1,char *pinBlockByZPK1,
		char *accNo,int lenOfAccNo,char *pinFormat2,char *pinBlockByZPK2,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	int		i;
	
	if ((zpk1 == NULL) || (zpk2 == NULL) || (pinFormat1 == NULL) || (pinFormat2 == NULL) || (accNo == NULL) ||
		(pinBlockByZPK1 == NULL) || (pinBlockByZPK2 == NULL))
	{
		UnionUserErrLog("in RacalCmdCC:: wrong parameters!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"CC",2);
	hsmCmdLen = 2;
	
	if ((ret = UnionGenerateX917RacalKeyString(zpk1Length,zpk1,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdCC:: UnionGenerateX917RacalKeyString for [%s]\n",zpk1);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionGenerateX917RacalKeyString(zpk2Length,zpk2,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdCC:: UnionGenerateX917RacalKeyString for [%s]\n",zpk2);
		return(ret);
	}
	hsmCmdLen += ret;
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",maxPinLen);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK1,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat1,2);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat2,2);
	hsmCmdLen += 2;
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdCC:: UnionForm12LenAccountNumber for [%s]\n",accNo);
		return(ret);
	}
	hsmCmdLen += ret;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdNC:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"CD",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdNC:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdNC:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(pinBlockByZPK2,hsmCmdBuf+4+2,16);
	return(16);
}

/*
输  出  消  息  格  式
输入域	长度	类型	说　明
命令代码		2A	值为"TI"。
源KEY标志	1	N	1：TPK           2：ZPK
源KEY	16
1A+32
1A+48	H	当KEY标志为1时为TPK，LMK对（14-15）下加密；
当KEY标志为2时为ZPK，LMK对（06-07）下加密。
目的KEY标志	1	N	1：TPK           2：ZPK
目的KEY	16
1A+32
1A+48	H	当KEY标志为1时为TPK，LMK对（14-15）下加密；
当KEY标志为2时为ZPK，LMK对（06-07）下加密。
源PIN块	16	H	源ZPK下加密的源PIN块。
源PIN格式	1	N	1：ANSI9.8格式    2：IBM格式
源PAN	16	H	用户主帐号，仅用到最右12位; 当源PIN格式为1时有此域
目的PIN格式	1	N	1：ANSI9.8格式    2：IBM格式
目的PAN	16	H	用户主帐号，仅用到最右12位; 当源PIN格式为1时有此域

输  出  消  息  格  式
输出域	长度	类型	说　明
响应代码	2	A	值"TJ"
错误代码	2	H	
PIN密文	16	H	TPK或ZPK下加密
*/
int RacalCmdTI(PUnionSJL06Server psjl06Server,char zpk1type,int zpk1Length,char *zpk1,char zpk2type,int zpk2Length,char * zpk2,
		int maxPinLen,char *pinFormat1,char *pinBlockByZPK1,
		char *accNo1,int lenOfAccNo1,char *accNo2,int lenOfAccNo2,
		char *pinFormat2,char *pinBlockByZPK2,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	int		i;
	
	if ((zpk1 == NULL) || (zpk2 == NULL) || (pinFormat1 == NULL) || (pinFormat2 == NULL) || (accNo1 == NULL) ||
		(pinBlockByZPK1 == NULL) || (pinBlockByZPK2 == NULL) || (accNo2 == NULL))
	{
		UnionUserErrLog("in RacalCmdTI:: wrong parameters!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"TI",2);
	hsmCmdLen = 2;
	hsmCmdBuf[hsmCmdLen] = zpk1type;
	hsmCmdLen++;
	if ((ret = UnionGenerateX917RacalKeyString(zpk1Length,zpk1,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdTI:: UnionGenerateX917RacalKeyString for [%s]\n",zpk1);
		return(ret);
	}
	hsmCmdLen += ret;
	hsmCmdBuf[hsmCmdLen] = zpk2type;
	hsmCmdLen++;
	if ((ret = UnionGenerateX917RacalKeyString(zpk2Length,zpk2,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdTI:: UnionGenerateX917RacalKeyString for [%s]\n",zpk2);
		return(ret);
	}
	hsmCmdLen += ret;
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK1,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat1,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,"0000",4);
	hsmCmdLen += 4;
	if ((ret = UnionForm12LenAccountNumber(accNo1,lenOfAccNo1,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdTI:: UnionForm12LenAccountNumber for [%s]\n",accNo1);
		return(ret);
	}
	hsmCmdLen += ret;
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat2,1);
	hsmCmdLen += 1;
	memcpy(hsmCmdBuf+hsmCmdLen,"0000",4);
	hsmCmdLen += 4;
	if ((ret = UnionForm12LenAccountNumber(accNo2,lenOfAccNo2,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdTI:: UnionForm12LenAccountNumber for [%s]\n",accNo2);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdTI:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"TJ",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdTI:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdTI:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(pinBlockByZPK2,hsmCmdBuf+4,16);
	return(16);
}

int RacalCmdMS(PUnionSJL06Server psjl06Server,
		char keyType,char keyLength,char *key,
		char msgType,int msgLen,char *msg,
		char *mac,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	int		i;
	
	if ((key == NULL) || (msg == NULL) || (mac == NULL))
	{
		UnionUserErrLog("in RacalCmdMS:: wrong parameters!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"MS0",3);
	hsmCmdLen = 3;
	hsmCmdBuf[hsmCmdLen] = keyType;
	++hsmCmdLen;
	hsmCmdBuf[hsmCmdLen] = keyLength;
	++hsmCmdLen;
	hsmCmdBuf[hsmCmdLen] = msgType;
	++hsmCmdLen;
	switch (keyLength)
	{
		case	'0':
			memcpy(hsmCmdBuf+hsmCmdLen,key,16);
			hsmCmdLen += 16;
			break;
		case	'1':
			memcpy(hsmCmdBuf+hsmCmdLen,key,32);
			hsmCmdLen += 32;
			break;
		default:
			UnionUserErrLog("in RacalCmdMS:: wrong keyLength flag [%c]!\n",keyLength);
			return(errCodeParameter);
	}
	switch (msgType)
	{
		case	'0':
			sprintf(hsmCmdBuf+hsmCmdLen,"%04X",msgLen);
			break;
		case	'1':
			sprintf(hsmCmdBuf+hsmCmdLen,"%04X",msgLen/2);
			break;
		default:
			UnionUserErrLog("in RacalCmdMS:: wrong msgtype flag [%c]!\n",msgType);
			return(errCodeParameter);
	}
	hsmCmdLen += 4;
	if (hsmCmdLen + msgLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in RacalCmdMS:: msg length [%d] too long!\n",msgLen);
		return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,msg,msgLen);
	hsmCmdLen += msgLen;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdNC:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"MT",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdNC:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdNC:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(mac,hsmCmdBuf+4,16);
	return(16);
}

int RacalCmdMU(PUnionSJL06Server psjl06Server,
		char keyType,char keyLength,char *key,
		char msgType,int msgLen,char *msg,
		char *mac,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	int		i;
	
	if ((key == NULL) || (msg == NULL) || (mac == NULL))
	{
		UnionUserErrLog("in RacalCmdMU:: wrong parameters!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"MU0",3);
	hsmCmdLen = 3;
	hsmCmdBuf[hsmCmdLen] = keyType;
	++hsmCmdLen;
	hsmCmdBuf[hsmCmdLen] = keyLength;
	++hsmCmdLen;
	hsmCmdBuf[hsmCmdLen] = msgType;
	++hsmCmdLen;
	switch (keyLength)
	{
		case	'0':
			memcpy(hsmCmdBuf+hsmCmdLen,key,16);
			hsmCmdLen += 16;
			break;
		case	'1':
			memcpy(hsmCmdBuf+hsmCmdLen,key,32);
			hsmCmdLen += 32;
			break;
		default:
			UnionUserErrLog("in RacalCmdMU:: wrong keyLength flag [%c]!\n",keyLength);
			return(errCodeParameter);
	}
	switch (msgType)
	{
		case	'0':
			sprintf(hsmCmdBuf+hsmCmdLen,"%04X",msgLen);
			break;
		case	'1':
			sprintf(hsmCmdBuf+hsmCmdLen,"%04X",msgLen/2);
			break;
		default:
			UnionUserErrLog("in RacalCmdMU:: wrong msgtype flag [%c]!\n",msgType);
			return(errCodeParameter);
	}
	hsmCmdLen += 4;
	if (hsmCmdLen + msgLen > sizeof(hsmCmdBuf))
	{
		UnionUserErrLog("in RacalCmdMU:: msg length [%d] too long!\n",msgLen);
		return(errCodeParameter);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,msg,msgLen);
	hsmCmdLen += msgLen;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdNC:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"MV",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdNC:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdNC:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(mac,hsmCmdBuf+4,16);
	return(16);
}

// Mary add begin, 2004-3-9
/*
函数功能：
	DG指令，用PVK生成PIN的PVV(PIN Verification Value)，
	采用的加密标准为Visa Method
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinLength：LMK加密的PIN密文长度
	pin：LMK加密的PIN密文
	pvkLength：LMK加密的PVK对长度
	pvk：LMK加密的PVK对
	lenOfAccNo：客户帐号长度
	accNo：客户帐号
输出参数：
	pvv：产生的PVV
	errCode：错误代码
返回值：
	<0，函数执行失败，无PVV
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回PVV的长度
*/
int RacalCmdDG(PUnionSJL06Server psjl06Server,int pinLength,char *pin,int pvkLength,char *pvk,\
		int lenOfAccNo,char *accNo,char *pvv,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;

	if ((pin == NULL) || (pvk == NULL) || (accNo == NULL) || (pvv == NULL))
	{
		UnionUserErrLog("in RacalCmdDG:: wrong parameters!\n");
		return(errCodeParameter);
	}
	
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"DG",2);
	hsmCmdLen = 2;
	// LMK加密的PVK对
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDG:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PIN
	memcpy(hsmCmdBuf+hsmCmdLen,pin,pinLength);
	hsmCmdLen += pinLength;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDG:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PVKI（0－6之间）
	memcpy(hsmCmdBuf+hsmCmdLen,gPVKI,1);
	hsmCmdLen += 1;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdDG:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"DH",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdDG:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// 产生的PVV
	memcpy(pvv,hsmCmdBuf+4,4);
	return(4);
}

/*
函数功能：
	CA指令，将一个TPK加密的PIN转换为由ZPK加密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	tpkLength：LMK加密的TPK长度
	tpk：LMK加密的TPK
	zpkLength：LMK加密的ZPK长度
	zpk：LMK加密的ZPK
	pinFormat1：转换前的PIN格式
	pinBlockByTPK：转换前由TPK加密的PIN密文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
	pinFormat2：转换后的PIN格式
输出参数：
	pinBlockByZPK：转换后由ZPK加密的PIN密文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByZPK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByZPK的长度
*/
int RacalCmdCA(PUnionSJL06Server psjl06Server,int tpkLength,char *tpk,int zpkLength,\
		char *zpk,char *pinFormat1,char *pinBlockByTPK,char *accNo,\
		int lenOfAccNo,char *pinFormat2,char *pinBlockByZPK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;

	if ((tpk == NULL) || (zpk == NULL) || (pinFormat1 == NULL) || (pinFormat2 == NULL) || \
		(accNo == NULL) || (pinBlockByTPK == NULL) || (pinBlockByZPK == NULL))
	{
		UnionUserErrLog("in RacalCmdCA:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"CA",2);
	hsmCmdLen = 2;
	// LMK加密的TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdCA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdCA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN最大长度
	memcpy(hsmCmdBuf+hsmCmdLen,"12",2);
	hsmCmdLen += 2;
	// 由TPK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// 转换前的PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat1,2);
	hsmCmdLen += 2;
	// 转换后的PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat2,2);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdCA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdCA:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"CB",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdCA:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// 转换后ZPK加密的PIN
	memcpy(pinBlockByZPK,hsmCmdBuf+4+2,16);
	return(16);
}

/*
函数功能：
	JE指令，将一个ZPK加密的PIN转换为由LMK加密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	zpkLength：LMK加密的ZPK长度
	zpk：LMK加密的ZPK
	pinFormat：PIN格式
	pinBlockByZPK：转换前由ZPK加密的PIN密文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByLMK：转换后由LMK加密的PIN密文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdJE(PUnionSJL06Server psjl06Server,int zpkLength,char *zpk,\
		char *pinFormat,char *pinBlockByZPK,char *accNo,\
		int lenOfAccNo,char *pinBlockByLMK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;

	if ((zpk == NULL) || (pinFormat == NULL) || (accNo == NULL) || \
		(pinBlockByZPK == NULL) || (pinBlockByLMK == NULL))
	{
		UnionUserErrLog("in RacalCmdJE:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"JE",2);
	hsmCmdLen = 2;
	// LMK加密的ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdJE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// 由ZPK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,16);
	hsmCmdLen += 16;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdJE:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdJE:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"JF",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdJE:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// 转换后LMK加密的PIN
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	return(ret-4);
}

/*
函数功能：
	JC指令，将一个TPK加密的PIN转换为由LMK加密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	tpkLength：LMK加密的TPK长度
	tpk：LMK加密的TPK
	pinFormat：PIN格式
	pinBlockByTPK：转换前由TPK加密的PIN密文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByLMK：转换后由LMK加密的PIN密文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdJC(PUnionSJL06Server psjl06Server,int tpkLength,char *tpk,\
		char *pinFormat,char *pinBlockByTPK,char *accNo,\
		int lenOfAccNo,char *pinBlockByLMK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;

	if ((tpk == NULL) || (pinFormat == NULL) || (accNo == NULL) || \
		(pinBlockByTPK == NULL) || (pinBlockByLMK == NULL))
	{
		UnionUserErrLog("in RacalCmdJC:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"JC",2);
	hsmCmdLen = 2;
	// LMK加密的TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdJC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// 由TPK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdJC:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdJC:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"JD",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdJC:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// 转换后LMK加密的PIN
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	return(ret-4);
}

/*
函数功能：
	JG指令，将一个LMK加密的PIN转换为由ZPK加密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	zpkLength：LMK加密的ZPK长度
	zpk：LMK加密的ZPK
	pinFormat：PIN格式
	pinLength：LMK加密的PIN密文长度
	pinBlockByZPK：转换前由ZPK加密的PIN密文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByZPK：转换后由LMK加密的PIN密文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByZPK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByZPK的长度
*/
int RacalCmdJG(PUnionSJL06Server psjl06Server,int zpkLength,char *zpk,\
		char *pinFormat,int pinLength,char *pinBlockByLMK,char *accNo,\
		int lenOfAccNo,char *pinBlockByZPK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((zpk == NULL) || (pinFormat == NULL) || (accNo == NULL) || \
		(pinBlockByZPK == NULL) || (pinBlockByLMK == NULL))
	{
		UnionUserErrLog("in RacalCmdJG:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"JG",2);
	hsmCmdLen = 2;
	// LMK加密的ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdJG:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdJG:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// 由LMK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByLMK,pinLength);
	hsmCmdLen += pinLength;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdJG:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"JH",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdJG:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// 转换后ZPK加密的PIN
	memcpy(pinBlockByZPK,hsmCmdBuf+4,16);
	return(16);
}

/*
函数功能：
	JA指令，随机产生一个PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinLength：要求随机生成的PIN明文的长度
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByLMK：随机产生的PIN的密文，由LMK加密
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdJA(PUnionSJL06Server psjl06Server,int pinLength,char *accNo,\
		int lenOfAccNo,char *pinBlockByLMK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((accNo == NULL) || (pinBlockByLMK == NULL))
	{
		UnionUserErrLog("in RacalCmdJA:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"JA",2);
	hsmCmdLen = 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdJA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",pinLength);
	hsmCmdLen += 2;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdJA:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"JB",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdJA:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// 随机产生的PIN的密文，由LMK加密
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	return(ret-4);
}

/*
函数功能：
	EE指令，用IBM方式产生一个PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	minPINLength：最小PIN长度
	pinValidData：用户自定义数据
	decimalizationTable：十六进制数到十进制数的转换表
	pinOffset：PIN Offset，左对齐，右补'F'
	pvkLength：LMK加密的PVK长度
	pvk：LMK加密的PVK
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByLMK：随机产生的PIN的密文，由LMK加密
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdEE(PUnionSJL06Server psjl06Server,int minPINLength,char *pinValidData,\
		char *decimalizationTable,char *pinOffset,int pvkLength,char *pvk,\
		char *accNo,int lenOfAccNo,char *pinBlockByLMK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((pinValidData == NULL) || (pinOffset == NULL) || (pvk == NULL) || \
		(accNo == NULL) || (pinBlockByLMK == NULL) || (decimalizationTable == NULL))
	{
		UnionUserErrLog("in RacalCmdEE:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"EE",2);
	hsmCmdLen = 2;
	// LMK加密的PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// PIN Offset
	memcpy(hsmCmdBuf+hsmCmdLen,pinOffset,12);
	hsmCmdLen += 12;
	// 最小PIN长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEE:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// 用户自定义数据
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidData,12);
	hsmCmdLen += 12;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdEE:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"EF",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdEE:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"02",2) == 0)
	{
		UnionSuccessLog("in RacalCmdEE:: Warning--The racal HSM result error code is [%s](PVK not single length)\n",errCode);
		memcpy(errCode,"00",2);
	}
	if (memcmp(errCode,"00",2) != 0 && memcmp(errCode,"02",2) != 0)
	{
		return(0);
	}
	// 随机产生的PIN的密文，由LMK加密
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	return(ret-4);
}

/*
函数功能：
	DE指令，用IBM方式产生一个PIN的Offset
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	minPINLength：最小PIN长度
	pinValidData：用户自定义数据
	decimalizationTable：十六进制数到十进制数的转换表
	pinLength：LMK加密的PIN密文长度
	pinBlockByLMK：由LMK加密的PIN密文
	pvkLength：LMK加密的PVK长度
	pvk：LMK加密的PVK
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinOffset：PIN Offset，左对齐，右补'F'
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinOffset
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinOffset的长度
*/
int RacalCmdDE(PUnionSJL06Server psjl06Server,int minPINLength,char *pinValidData,\
		char *decimalizationTable,int pinLength,char *pinBlockByLMK,int pvkLength,\
		char *pvk,char *accNo,int lenOfAccNo,char *pinOffset,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((pinValidData == NULL) || (pinOffset == NULL) || (pvk == NULL) || \
		(accNo == NULL) || (pinBlockByLMK == NULL) || (decimalizationTable == NULL))
	{
		UnionUserErrLog("in RacalCmdDE:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"DE",2);
	hsmCmdLen = 2;
	// LMK加密的PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByLMK,pinLength);
	hsmCmdLen += pinLength;
	// 最小PIN长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDE:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// 用户自定义数据
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidData,12);
	hsmCmdLen += 12;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdDE:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"DF",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdDE:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"02",2) == 0)
	{
		UnionSuccessLog("in RacalCmdDE:: Warning--The racal HSM result error code is [%s](PVK not single length)\n",errCode);
		memcpy(errCode,"00",2);
	}
	if (memcmp(errCode,"00",2) != 0 && memcmp(errCode,"02",2) != 0)
	{
		return(0);
	}
	// 产生的PIN的Offset
	memcpy(pinOffset,hsmCmdBuf+4,12);
	return(12);
}

/*
函数功能：
	BA指令，使用本地主密钥加密一个PIN明文
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinCryptogramLen：加密后的PIN密文的长度
	pinTextLength：PIN明文的长度
	pinText：PIN明文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByLMK：LMK加密的PIN密文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdBA(PUnionSJL06Server psjl06Server,int pinCryptogramLen,int pinTextLength,\
		char *pinText,char *accNo,int lenOfAccNo,char *pinBlockByLMK,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1],tmpBuf[100];
	int		hsmCmdLen = 0;
	
	if ((pinText == NULL) || (accNo == NULL) || (pinBlockByLMK == NULL) || (pinTextLength < 0) || (pinCryptogramLen < 0))
	{
		UnionUserErrLog("in RacalCmdBA:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"BA",2);
	hsmCmdLen = 2;
	
	// 填充PIN明文
	if ((pinCryptogramLen >= sizeof(tmpBuf)) || (pinTextLength >= sizeof(tmpBuf)))
	{
		UnionUserErrLog("in RacalCmdBA:: pinCryptogramLen = [%d] or pinTextLength = [%d] too long!\n",pinCryptogramLen,pinTextLength);
		return(errCodeParameter);
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
	memset(tmpBuf,'F',pinCryptogramLen);
	memcpy(tmpBuf,pinText,pinTextLength);

	// 填充后的PIN明文
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,pinCryptogramLen);
	hsmCmdLen += pinCryptogramLen;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdBA:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"BB",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdBA:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// LMK加密的PIN密文
	memcpy(pinBlockByLMK,hsmCmdBuf+4,ret-4);
	return(ret-4);
}

/*
函数功能：
	NG指令，使用本地主密钥解密一个PIN密文
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinCryptogramLen：LMK加密的PIN密文的长度
	pinCryptogram：LMK加密的PIN密文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	referenceNumber：用LMK18-19加密帐号得到的偏移值
	pinText：PIN明文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinText
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinText的长度
*/
int RacalCmdNG(PUnionSJL06Server psjl06Server,int pinCryptogramLen,char *pinCryptogram,\
		char *accNo,int lenOfAccNo,char *referenceNumber,char *pinText,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1],tmpBuf[100];
	int		hsmCmdLen = 0,i;
	
	if ((pinCryptogram == NULL) || (accNo == NULL) || (referenceNumber == NULL) || (pinText == NULL))
	{
		UnionUserErrLog("in RacalCmdNG:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"NG",2);
	hsmCmdLen = 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdNG:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinCryptogram,pinCryptogramLen);
	hsmCmdLen += pinCryptogramLen;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdNG:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"NH",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdNG:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// 用LMK18-19加密帐号得到的偏移值
	memcpy(referenceNumber,hsmCmdBuf+4+pinCryptogramLen,12);
	// 填充的PIN明文
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf,hsmCmdBuf+4,pinCryptogramLen);
	for (i=0;i<pinCryptogramLen;i++)
	{
		if (tmpBuf[i] == 'F')
		{
			tmpBuf[i] = '\0';
			break;
		}
	}
	// PIN明文
	memcpy(pinText,tmpBuf,i);
	return(i);
}

/*
函数功能：
	DA指令，用IBM方式验证终端的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinBlockByTPK：TPK加密的PIN密文
	minPINLength：最小PIN长度
	pinValidData：用户自定义数据
	decimalizationTable：十六进制数到十进制数的转换表
	pinOffset：IBM Offset，左对齐，右补'F'
	tpkLength：LMK加密的TPK长度
	tpk：LMK加密的TPK
	pvkLength：LMK加密的PVK长度
	pvk：LMK加密的PVK
	pinFormat：PIN格式
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdDA(PUnionSJL06Server psjl06Server,char *pinBlockByTPK,int minPINLength,\
		char *pinValidData,char *decimalizationTable,char *pinOffset,int tpkLength,\
		char *tpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((pinBlockByTPK == NULL) || (pinValidData == NULL) || (pinOffset == NULL) || \
		(decimalizationTable == NULL) || (tpk == NULL) || (pvk == NULL) || \
		(pinFormat == NULL) || (accNo == NULL))
	{
		UnionUserErrLog("in RacalCmdDA:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"DA",2);
	hsmCmdLen = 2;
	// LMK加密的TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// 最大PIN长度
	memcpy(hsmCmdBuf+hsmCmdLen,"12",2);
	hsmCmdLen += 2;
	// TPK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 最小PIN长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// 用户自定义数据
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidData,12);
	hsmCmdLen += 12;
	// IBM Offset
	memcpy(hsmCmdBuf+hsmCmdLen,pinOffset,12);
	hsmCmdLen += 12;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdDA:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"DB",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdDA:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"02",2) == 0)
	{
		UnionSuccessLog("in RacalCmdDA:: Warning--The racal HSM result error code is [%s](PVK not single length)\n",errCode);
		memcpy(errCode,"00",2);
	}
	if (memcmp(errCode,"00",2) != 0 && memcmp(errCode,"02",2) != 0)
	{
		return(0);
	}

	return(0);
}

/*
函数功能：
	EA指令，用IBM方式验证交换中心的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinBlockByZPK：ZPK加密的PIN密文
	minPINLength：最小PIN长度
	pinValidData：用户自定义数据
	decimalizationTable：十六进制数到十进制数的转换表
	pinOffset：IBM Offset，左对齐，右补'F'
	zpkLength：LMK加密的ZPK长度
	zpk：LMK加密的ZPK
	pvkLength：LMK加密的TPK长度
	pvk：LMK加密的PVK
	pinFormat：PIN格式
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdEA(PUnionSJL06Server psjl06Server,char *pinBlockByZPK,int minPINLength,\
		char *pinValidData,char *decimalizationTable,char *pinOffset,int zpkLength,\
		char *zpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((pinBlockByZPK == NULL) || (pinValidData == NULL) || (pinOffset == NULL) || \
		(zpk == NULL) || (pvk == NULL) || (decimalizationTable == NULL) || \
		(pinFormat == NULL) || (accNo == NULL))
	{
		UnionUserErrLog("in RacalCmdEA:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"EA",2);
	hsmCmdLen = 2;
	// LMK加密的ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEA:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// 最大PIN长度
	memcpy(hsmCmdBuf+hsmCmdLen,"12",2);
	hsmCmdLen += 2;
	// ZPK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,16);
	hsmCmdLen += 16;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 最小PIN长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",minPINLength);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEA:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// Decimalization table
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	// 用户自定义数据
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidData,12);
	hsmCmdLen += 12;
	// IBM Offset
	memcpy(hsmCmdBuf+hsmCmdLen,pinOffset,12);
	hsmCmdLen += 12;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdEA:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"EB",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdEA:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"02",2) == 0)
	{
		UnionSuccessLog("in RacalCmdDE:: Warning--The racal HSM result error code is [%s](PVK not single length)\n",errCode);
		memcpy(errCode,"00",2);
	}
	if (memcmp(errCode,"00",2) != 0 && memcmp(errCode,"02",2) != 0)
	{
		return(0);
	}

	return(0);
}

/*
函数功能：
	DC指令，用VISA方式验证终端的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinBlockByTPK：TPK加密的PIN密文
	pvv：终端PIN的4位VISA PVV
	tpkLength：LMK加密的TPK长度
	tpk：LMK加密的TPK
	pvkLength：LMK加密的PVK长度
	pvk：LMK加密的PVK
	pinFormat：PIN格式
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdDC(PUnionSJL06Server psjl06Server,char *pinBlockByTPK,\
		char *pvv,int tpkLength,char *tpk,int pvkLength,char *pvk,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((pinBlockByTPK == NULL) || (pvv == NULL) || (tpk == NULL) || \
		(pvk == NULL) || (pinFormat == NULL) || (accNo == NULL))
	{
		UnionUserErrLog("in RacalCmdDC:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"DC",2);
	hsmCmdLen = 2;
	// LMK加密的TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// TPK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDC:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PVKI
	memcpy(hsmCmdBuf+hsmCmdLen,gPVKI,1);
	hsmCmdLen += 1;
	// 终端PIN的4位VISA PVV
	memcpy(hsmCmdBuf+hsmCmdLen,pvv,4);
	hsmCmdLen += 4;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdDC:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"DD",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdDC:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}

	return(0);
}

/*
函数功能：
	EC指令，用VISA方式验证交换中心的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinBlockByZPK：ZPK加密的PIN密文
	pvv：终端PIN的4位VISA PVV
	zpkLength：LMK加密的ZPK
	zpk：LMK加密的ZPK
	pvkLength：LMK加密的PVK长度
	pvk：LMK加密的PVK
	pinFormat：PIN格式
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdEC(PUnionSJL06Server psjl06Server,char *pinBlockByZPK,\
		char *pvv,int zpkLength,char *zpk,int pvkLength,char *pvk,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((pinBlockByZPK == NULL) || (pvv == NULL) || (zpk == NULL) || \
		(pvk == NULL) || (pinFormat == NULL) || (accNo == NULL))
	{
		UnionUserErrLog("in RacalCmdEC:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"EC",2);
	hsmCmdLen = 2;
	// LMK加密的ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的PVK
	if ((ret = UnionGenerateX917RacalKeyString(pvkLength,pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",pvk,pvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// ZPK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,16);
	hsmCmdLen += 16;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdEC:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// PVKI
	memcpy(hsmCmdBuf+hsmCmdLen,gPVKI,1);
	hsmCmdLen += 1;
	// 终端PIN的4位VISA PVV
	memcpy(hsmCmdBuf+hsmCmdLen,pvv,4);
	hsmCmdLen += 4;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdEC:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"ED",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdEC:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}

	return(0);
}

/*
函数功能：
	BC指令，用比较方式验证终端的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinBlockByTPK：TPK加密的PIN密文
	tpkLength：LMK加密的TPK
	tpk：LMK加密的TPK
	pinByLMKLength：主机PIN密文长度
	pinByLMK：主机PIN密文，由LMK02-03加密
	pinFormat：PIN格式
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdBC(PUnionSJL06Server psjl06Server,char *pinBlockByTPK,\
		int tpkLength,char *tpk,int pinByLMKLength,char *pinByLMK,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((pinBlockByTPK == NULL) || (pinByLMK == NULL) || (tpk == NULL) || \
		(pinFormat == NULL) || (accNo == NULL))
	{
		UnionUserErrLog("in RacalCmdBC:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"BC",2);
	hsmCmdLen = 2;
	// LMK加密的TPK
	if ((ret = UnionGenerateX917RacalKeyString(tpkLength,tpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBC:: UnionGenerateX917RacalKeyString for [%s][%d]\n",tpk,tpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// TPK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByTPK,16);
	hsmCmdLen += 16;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBC:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// 主机PIN密文，由LMK02-03加密
	memcpy(hsmCmdBuf+hsmCmdLen,pinByLMK,pinByLMKLength);
	hsmCmdLen += pinByLMKLength;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdBC:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"BD",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdBC:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}

	return(0);
}

/*
函数功能：
	BE指令，用比较方式验证交换中心的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinBlockByZPK：ZPK加密的PIN密文
	zpkLength：LMK加密的ZPK长度
	zpk：LMK加密的ZPK
	pinByLMKLength：主机PIN密文长度
	pinByLMK：主机PIN密文，由LMK02-03加密
	pinFormat：PIN格式
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdBE(PUnionSJL06Server psjl06Server,char *pinBlockByZPK,\
		int zpkLength,char *zpk,int pinByLMKLength,char *pinByLMK,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((pinBlockByZPK == NULL) || (pinByLMK == NULL) || (zpk == NULL) || \
		(pinFormat == NULL) || (accNo == NULL))
	{
		UnionUserErrLog("in RacalCmdBE:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"BE",2);
	hsmCmdLen = 2;
	// LMK加密的ZPK
	if ((ret = UnionGenerateX917RacalKeyString(zpkLength,zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBE:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zpk,zpkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// ZPK加密的PIN密文
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlockByZPK,16);
	hsmCmdLen += 16;
	// PIN格式
	memcpy(hsmCmdBuf+hsmCmdLen,pinFormat,2);
	hsmCmdLen += 2;
	// 12位客户帐号
	if ((ret = UnionForm12LenAccountNumber(accNo,lenOfAccNo,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBE:: UnionForm12LenAccountNumber for [%s][%d]\n",accNo,lenOfAccNo);
		return(ret);
	}
	hsmCmdLen += ret;
	// 主机PIN密文，由LMK02-03加密
	memcpy(hsmCmdBuf+hsmCmdLen,pinByLMK,pinByLMKLength);
	hsmCmdLen += pinByLMKLength;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdBE:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"BF",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdBE:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}

	return(0);
}

/*
函数功能：
	CW指令，产生VISA卡校验值CVV 
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	cardValidDate：VISA卡的有效期
	cvkLength：CVK的长度
	cvk：CVK
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	cvv：生成的VISA卡的CVV
	errCode：错误代码
返回值：
	<0，函数执行失败，无cvv
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回cvv的长度
*/
int RacalCmdCW(PUnionSJL06Server psjl06Server,char *cardValidDate,int cvkLength,char *cvk,\
		char *accNo,int lenOfAccNo,char *serviceCode,char *cvv,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((cardValidDate == NULL) || (cvk == NULL) || (accNo == NULL) || (cvv == NULL))
	{
		UnionUserErrLog("in RacalCmdCW:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"CW",2);
	hsmCmdLen = 2;
	// LMK加密的CVK
	if ((ret = UnionGenerateX917RacalKeyString(cvkLength,cvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdCW:: UnionGenerateX917RacalKeyString for [%s][%d]\n",cvk,cvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// 客户帐号
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,lenOfAccNo);
	hsmCmdLen += lenOfAccNo;
	// 分隔符
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// VISA卡的有效期
	memcpy(hsmCmdBuf+hsmCmdLen,cardValidDate,4);
	hsmCmdLen += 4;
	// VISA卡服务商代码
	memcpy(hsmCmdBuf+hsmCmdLen,serviceCode,3);
	hsmCmdLen += 3;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdCW:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"CX",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdCW:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// 生成的VISA卡的CVV
	memcpy(cvv,hsmCmdBuf+4,3);
	return(3);
}

/*
函数功能：
	CY指令，验证VISA卡的CVV 
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	cvv：待验证的VISA卡的CVV
	cardValidDate：VISA卡的有效期
	cvkLength：CVK的长度
	cvk：CVK
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdCY(PUnionSJL06Server psjl06Server,char *cvv,char *cardValidDate,\
		int cvkLength,char *cvk,char *accNo,int lenOfAccNo,char *serviceCode,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((cardValidDate == NULL) || (cvk == NULL) || (accNo == NULL) || (cvv == NULL))
	{
		UnionUserErrLog("in RacalCmdCY:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"CY",2);
	hsmCmdLen = 2;
	// LMK加密的CVK
	if ((ret = UnionGenerateX917RacalKeyString(cvkLength,cvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdCY:: UnionGenerateX917RacalKeyString for [%s][%d]\n",cvk,cvkLength);
		return(ret);
	}
	hsmCmdLen += ret;
	// 待验证的VISA卡的CVV
	memcpy(hsmCmdBuf+hsmCmdLen,cvv,3);
	hsmCmdLen += 3;
	// 客户帐号
	memcpy(hsmCmdBuf+hsmCmdLen,accNo,lenOfAccNo);
	hsmCmdLen += lenOfAccNo;
	// 分隔符
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// VISA卡的有效期
	memcpy(hsmCmdBuf+hsmCmdLen,cardValidDate,4);
	hsmCmdLen += 4;
	// VISA卡服务商代码
	memcpy(hsmCmdBuf+hsmCmdLen,serviceCode,3);
	hsmCmdLen += 3;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdCY:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"CZ",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdCY:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}

	return(0);
}

/*
函数功能：
	EW指令，用私钥签名 
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	indexOfVK：私钥索引号
	signDataLength：待签名数据的长度
	signData：待签名的数据
	vkLength：LMK加密的私钥长度
	vk：LMK加密的私钥
输出参数：
	signature：生成的签名
	errCode：错误代码
返回值：
	<0，函数执行失败，无signature
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回signature的长度

int RacalCmdEW(PUnionSJL06Server psjl06Server,char *indexOfVK,int signDataLength,\
		char *signData,int vkLength,char *vk,char *signature,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1],tmpBuf[10];
	int		hsmCmdLen = 0;
	
	if ((indexOfVK == NULL) || (signData == NULL) || (vk == NULL) || (signature == NULL))
	{
		UnionUserErrLog("in RacalCmdEW:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"EW",2);
	hsmCmdLen = 2;
	// Hash Identifier
	memcpy(hsmCmdBuf+hsmCmdLen,gHashIdentifier,2);
	hsmCmdLen += 2;
	// Signature Identifier
	memcpy(hsmCmdBuf+hsmCmdLen,gSignatureIdentifier,2);
	hsmCmdLen += 2;
	// Pad Mode Identifier
	memcpy(hsmCmdBuf+hsmCmdLen,gPadModeIdentifier,2);
	hsmCmdLen += 2;
	// 待签名数据的长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",signDataLength);
	hsmCmdLen += 4;
	// 待签名的数据
	memcpy(hsmCmdBuf+hsmCmdLen,signData,signDataLength);
	hsmCmdLen += signDataLength;
	// 分隔符
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// 私钥索引号
	memcpy(hsmCmdBuf+hsmCmdLen,indexOfVK,2);
	hsmCmdLen += 2;
	// LMK加密的私钥长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",vkLength);
	hsmCmdLen += 4;
	// LMK加密的私钥
	memcpy(hsmCmdBuf+hsmCmdLen,vk,vkLength);
	hsmCmdLen += vkLength;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdEW:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"EX",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdEW:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}
	// 生成的签名长度
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf,hsmCmdBuf+4,4);
	// 生成的签名
	memcpy(signature,hsmCmdBuf+4+4,atoi(tmpBuf));
	return(atoi(tmpBuf));
}


函数功能：
	EY指令，用公钥验证签名
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	macOfPK：公钥的MAC值
	signatureLength：待验证的签名的长度
	signature：待验证的签名
	signDataLength：待签名数据的长度
	signData：待签名的数据
	publicKeyLength：公钥的长度
	publicKey：公钥
输出参数：
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码

int RacalCmdEY(PUnionSJL06Server psjl06Server,char *macOfPK,int signatureLength,\
		char *signature,int signDataLength,char *signData,int publicKeyLength,\
		char *publicKey,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((macOfPK == NULL) || (signData == NULL) || (publicKey == NULL) || (signature == NULL))
	{
		UnionUserErrLog("in RacalCmdEY:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"EY",2);
	hsmCmdLen = 2;
	// Hash Identifier
	memcpy(hsmCmdBuf+hsmCmdLen,gHashIdentifier,2);
	hsmCmdLen += 2;
	// Signature Identifier
	memcpy(hsmCmdBuf+hsmCmdLen,gSignatureIdentifier,2);
	hsmCmdLen += 2;
	// Pad Mode Identifier
	memcpy(hsmCmdBuf+hsmCmdLen,gPadModeIdentifier,2);
	hsmCmdLen += 2;
	// 待验证的签名长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",signatureLength);
	hsmCmdLen += 4;
	// 待验证的签名
	memcpy(hsmCmdBuf+hsmCmdLen,signature,signatureLength);
	hsmCmdLen += signatureLength;
	// 分隔符
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// 待签名数据的长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",signDataLength);
	hsmCmdLen += 4;
	// 待签名的数据
	memcpy(hsmCmdBuf+hsmCmdLen,signData,signDataLength);
	hsmCmdLen += signDataLength;
	// 分隔符
	memcpy(hsmCmdBuf+hsmCmdLen,";",1);
	hsmCmdLen += 1;
	// 公钥的MAC值
	memcpy(hsmCmdBuf+hsmCmdLen,macOfPK,4);
	hsmCmdLen += 4;
	// 公钥
	memcpy(hsmCmdBuf+hsmCmdLen,publicKey,publicKeyLength);
	hsmCmdLen += publicKeyLength;

	UnionProgramerMemLog("in RacalCmdEY:: req =",hsmCmdBuf, hsmCmdLen);
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdEY:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"EZ",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdEY:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		return(0);
	}

	return(0);
}
*/
// Mary add end, 2004-3-9

// Mary add begin, 2004-3-24
/*
函数功能：
	BU指令，产生一把密钥的校验值
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	keyType：密钥的类型
	keyLength：密钥的长度
	key：LMK加密的密钥密文
输出参数：
	checkValue：生成的密钥校验值
	errCode：错误代码
返回值：
	<0，函数执行失败，无checkValue
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回checkValue的长度
*/
int RacalCmdBU(PUnionSJL06Server psjl06Server,TUnionDesKeyType keyType,\
		TUnionDesKeyLength keyLength,char *key,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	
	if ((key == NULL) || (checkValue == NULL))
	{
		UnionUserErrLog("in RacalCmdBU:: wrong parameters!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	// 命令字
	memcpy(hsmCmdBuf,"BU",2);
	hsmCmdLen = 2;
	// 加密密钥的LMK对指示符，即密钥类型代码的最后2位
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,tmpBuf)) < 0)
	{
		UnionUserErrLog("in RacalCmdBU:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	// Mary add begin, 2004-4-15
	if (memcmp(tmpBuf,"000",3) == 0)
		memcpy(tmpBuf,"010",3);
	// Mary add end, 2004-4-15
	memcpy(hsmCmdBuf+hsmCmdLen,&tmpBuf[1],2);
	hsmCmdLen += 2;
	// 密钥长度标志
	switch (keyLength)
	{
		case	con64BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '0';
			break;
		case	con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '1';
			break;
		case	con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '2';
			break;
		default:
			UnionUserErrLog("in RacalCmdBU:: unknown key length!\n");
			return(errCodeParameter);
	}
	hsmCmdLen += 1;
	// LMK加密的密钥密文
	if ((ret = UnionGenerateX917RacalKeyString(keyLength,key,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBU:: UnionGenerateX917RacalKeyString for [%s][%d]\n",key,keyLength);
		return(ret);
	}
	hsmCmdLen += ret;
	/*
	// 分隔符
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen += 1;
	// 密钥类型代码
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,3);
	hsmCmdLen += 3;
	// 分隔符
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen += 1;
	// ZMK加密的密钥密文长度标志
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBU:: UnionTranslateHsmKeyKeyScheme for key by ZMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的密钥密文长度标志
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBU:: UnionTranslateHsmKeyKeyScheme for key by LMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// 校验值类型标志
	hsmCmdBuf[hsmCmdLen] = '1';
	hsmCmdLen += 1;
	*/

	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdBU:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"BV",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdBU:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdBU:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(checkValue,hsmCmdBuf+4,16);
	return(16);
}

int RacalCmdBUSpecForZmk(PUnionSJL06Server psjl06Server,TUnionDesKeyType keyType,\
		TUnionDesKeyLength keyLength,char *key,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024+1];
	int		hsmCmdLen = 0;
	char		tmpBuf[100];
	
	if ((key == NULL) || (checkValue == NULL))
	{
		UnionUserErrLog("in RacalCmdBUSpecForZmk:: wrong parameters!\n");
		return(errCodeParameter);
	}
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	// 命令字
	memcpy(hsmCmdBuf,"BU",2);
	hsmCmdLen = 2;
	// 加密密钥的LMK对指示符，即密钥类型代码的最后2位
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,tmpBuf)) < 0)
	{
		UnionUserErrLog("in RacalCmdBUSpecForZmk:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	memcpy(hsmCmdBuf+hsmCmdLen,&tmpBuf[1],2);
	hsmCmdLen += 2;
	// 密钥长度标志
	switch (keyLength)
	{
		case	con64BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '0';
			break;
		case	con128BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '1';
			break;
		case	con192BitsDesKey:
			hsmCmdBuf[hsmCmdLen] = '2';
			break;
		default:
			UnionUserErrLog("in RacalCmdBUSpecForZmk:: unknown key length!\n");
			return(errCodeParameter);
	}
	hsmCmdLen += 1;
	// LMK加密的密钥密文
	if ((ret = UnionGenerateX917RacalKeyString(keyLength,key,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBUSpecForZmk:: UnionGenerateX917RacalKeyString for [%s][%d]\n",key,keyLength);
		return(ret);
	}
	hsmCmdLen += ret;
	/*
	// 分隔符
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen += 1;
	// 密钥类型代码
	memcpy(hsmCmdBuf+hsmCmdLen,tmpBuf,3);
	hsmCmdLen += 3;
	// 分隔符
	hsmCmdBuf[hsmCmdLen] = ';';
	hsmCmdLen += 1;
	// ZMK加密的密钥密文长度标志
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBUSpecForZmk:: UnionTranslateHsmKeyKeyScheme for key by ZMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// LMK加密的密钥密文长度标志
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdBUSpecForZmk:: UnionTranslateHsmKeyKeyScheme for key by LMK!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	// 校验值类型标志
	hsmCmdBuf[hsmCmdLen] = '1';
	hsmCmdLen += 1;
	*/

	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdBUSpecForZmk:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"BV",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdBUSpecForZmk:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdBUSpecForZmk:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}
	memcpy(checkValue,hsmCmdBuf+4,16);
	return(16);
}


/*
函数功能：
	A4指令，用几把密钥的密文成分合成一把密钥，并生成校验值
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	keyType：密钥的类型
	keyLength：密钥的长度
	partKeyNum：密钥成分的数量
	partKey：存放各个密钥成分的数组缓冲，为LMK加密的密钥密文
输出参数：
	keyByLMK：合成的密钥密文，由LMK加密
	checkValue：合成的密钥的校验值
	errCode：错误代码
返回值：
	<0，函数执行失败，无keyByLMK和checkValue
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回keyByLMK和checkValue的总长度
*/
int RacalCmdA4(PUnionSJL06Server psjl06Server,TUnionDesKeyType keyType,\
		TUnionDesKeyLength keyLength,int partKeyNum,char partKey[][50],\
		char *keyByLMK,char *checkValue,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[1024+1];
	int		hsmCmdLen = 0;
	int		i;
	
	if ((keyByLMK == NULL) || (checkValue == NULL))
	{
		UnionUserErrLog("in RacalCmdA4:: wrong parameters!\n");
		return(errCodeParameter);
	}
	if (partKeyNum < 2 || partKeyNum > 9)
	{
		UnionUserErrLog("in RacalCmdA4:: partKeyNum [%d] must between 2 and 9!\n",partKeyNum);
		return(errCodeParameter);
	}
	
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	// 命令字
	memcpy(hsmCmdBuf,"A4",2);
	hsmCmdLen = 2;
	// 密钥成分数量
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",partKeyNum);
	hsmCmdLen += 1;
	// 密钥类型代码
	if ((ret = UnionTranslateHsmKeyTypeString(keyType,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA4:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	// Mary add begin, 2004-4-15
	if (memcmp(hsmCmdBuf+hsmCmdLen,"000",3) == 0)
		memcpy(hsmCmdBuf+hsmCmdLen,"100",3);
	// Mary add end, 2004-4-15
	hsmCmdLen += ret;
	// 密钥长度标志
	if ((ret = UnionTranslateHsmKeyKeyScheme(keyLength,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA4:: UnionTranslateHsmKeyKeyScheme!\n");
		return(ret);
	}
	hsmCmdLen += ret;

	for (i = 0; i < partKeyNum; i++)
	{
		// LMK加密的密钥密文
		if ((ret = UnionGenerateX917RacalKeyString(keyLength,partKey[i],hsmCmdBuf+hsmCmdLen)) < 0)
		{
			UnionUserErrLog("in RacalCmdA4:: UnionGenerateX917RacalKeyString for [%s][%d]\n",partKey[i],keyLength);
			return(ret);
		}
		hsmCmdLen += ret;
	}	

	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdA4:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"A5",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdA4:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdA4:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(0);
	}

	switch (keyLength)
	{
		case	con64BitsDesKey:
			// 合成的密钥，LMK加密
			memcpy(keyByLMK,hsmCmdBuf+4,16);
			// 合成的密钥的校验值
			memcpy(checkValue,hsmCmdBuf+4+16,6);
			return(16+6);
		case	con128BitsDesKey:
			// 合成的密钥，LMK加密
			memcpy(keyByLMK,hsmCmdBuf+4+1,32);
			// 合成的密钥的校验值
			memcpy(checkValue,hsmCmdBuf+4+1+32,6);
			return(32+6);
		case	con192BitsDesKey:
			// 合成的密钥，LMK加密
			memcpy(keyByLMK,hsmCmdBuf+4+1,48);
			// 合成的密钥的校验值
			memcpy(checkValue,hsmCmdBuf+4+1+48,6);
			return(48+6);
		default:
			UnionUserErrLog("in RacalCmdA4:: unknown key length!\n");
			return(errCodeParameter);
	}
}
// Mary add end, 2004-3-24


/*
功能	由源帐号生成的PINBLOCK转换为由目的帐号生成的PINBLOCK，并受指定的密钥加密保护
说明	根据密钥长度确定算法

指令消息格式
消息头	mA	
命令码	2A	DF
ZPK	16H or
	1A+32H or
	1A+48H	被 LMK 06-07 加密的ZPK
PVK	16H or
	1A+32H or
	1A+48H	被 LMK 14-15加密的ZPK
Check length	2N	最大PIN长度
十进制数表	16N	16进制转换表
PIN Validation data	12A	User-defined data consisting of hexadecimal characters and
the character N, which indicates to the HSM where to insert
the last 5 digits of the account number.
PIN block	16 H	被ZPK采用ANSI X9.8标准加密
源账号	12 N	12位有效源账号
目标账号	12 N	12位有效目标账号
*/

int RacalCmdDF(PUnionSJL06Server psjl06Server,char *zpk,char *pvk,int maxPinLen,char *decimalizationTable,
		char *pinValidationData,char *pinBlock1,char *accNo1,char *accNo2,
		char *pinBlock2,char *pinOffset,char *errCode)
{
	int		ret;
	char		hsmCmdBuf[512+1];
	int		hsmCmdLen = 0;
	
	if ((zpk == NULL) || (pvk == NULL) || (decimalizationTable == NULL) || 
		(pinValidationData == NULL) || (pinBlock1 == NULL) || (accNo1 == NULL) || (accNo2 == NULL) ||
		(pinBlock2 == NULL) || (pinOffset == NULL) || (errCode == NULL))
	{
		UnionUserErrLog("in RacalCmdDF:: wrong parameters!\n");
		return(errCodeParameter);
	}
	// 命令字
	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));
	memcpy(hsmCmdBuf,"DF",2);
	hsmCmdLen = 2;
	if ((ret = UnionGenerateX917RacalKeyString(strlen(zpk),zpk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDF:: UnionGenerateX917RacalKeyString for [%s]\n",zpk);
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionGenerateX917RacalKeyString(strlen(pvk),pvk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdDF:: UnionGenerateX917RacalKeyString for [%s]\n",pvk);
		return(ret);
	}
	hsmCmdLen += ret;
	sprintf(hsmCmdBuf+hsmCmdLen,"%02d",maxPinLen);
	hsmCmdLen += 2;
	memcpy(hsmCmdBuf+hsmCmdLen,decimalizationTable,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,pinValidationData,12);
	hsmCmdLen += 12;
	memcpy(hsmCmdBuf+hsmCmdLen,pinBlock1,16);
	hsmCmdLen += 16;
	memcpy(hsmCmdBuf+hsmCmdLen,accNo1,12);
	hsmCmdLen += 12;
	memcpy(hsmCmdBuf+hsmCmdLen,accNo2,12);
	hsmCmdLen += 12;
	
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdDF:: UnionSJL06ServerService! return=[%d]\n",ret);
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"DG",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdDF:: Command is error, hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if ((memcmp(errCode,"02",2) == 0) || (memcmp(errCode,"00",2) == 0))
	{
		memcpy(errCode,"00",2);
		memcpy(pinBlock2,hsmCmdBuf+2+2,16);
		memcpy(pinOffset,hsmCmdBuf+2+2+16,12);
	}
	return(0);
}

int RacalCmdA8(PUnionSJL06Server psjl06Server,
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
		UnionUserErrLog("in RacalCmdA8:: UnionTranslateHsmKeyTypeString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionGenerateX917RacalKeyString(strlen(zmk),zmk,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA8:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
	if ((ret = UnionGenerateX917RacalKeyString(strlen(key),key,hsmCmdBuf+hsmCmdLen)) < 0)
	{
		UnionUserErrLog("in RacalCmdA8:: UnionGenerateX917RacalKeyString!\n");
		return(ret);
	}
	hsmCmdLen += ret;
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
	if ((ret = UnionSJL06ServerService(psjl06Server,hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in RacalCmdA8:: UnionSJL06ServerService!\n");
		return(ret);
	}
	hsmCmdBuf[ret] = 0;
	if ((memcmp(hsmCmdBuf,"A9",2) != 0) || (ret < 4))
	{
		UnionUserErrLog("in RacalCmdA8:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
		return(errSJL06Abnormal);
	}
	memcpy(errCode,hsmCmdBuf+2,2);
	if (memcmp(errCode,"00",2) != 0)
	{
		//UnionUserErrLog("in RacalCmdA8:: hsmCmdBuf = [%s]!\n",hsmCmdBuf);
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
			UnionUserErrLog("in RacalCmdA8:: invalid keyLen[%d]\n",strlen(key));
			return(errCodeParameter);
	}
	memcpy(keyByZMK,hsmCmdBuf+offsetOfKeyByZMK,keyLen);
	memcpy(checkValue,hsmCmdBuf+offsetOfCheckValue,6);
	return(keyLen);
}
