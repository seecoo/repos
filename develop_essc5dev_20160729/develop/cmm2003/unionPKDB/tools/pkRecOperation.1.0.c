#include <stdio.h>
#include <string.h>

#include "unionPKDB.h"
#include "UnionLog.h"

// 将PK记录写入到文件中
int UnionPrintPKToRecStrTxtFile(PUnionPK pPK,FILE *fp)
{
	char		recStr[2048+1];
	int		ret = 0;
	FILE		*outputFp;
		
	memset(recStr,0,sizeof(recStr));
	if ((ret = UnionPutPKIntoRecStr(pPK,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionPrintPKToRecStrTxtFile:: UnionPutPKIntoRecStr!\n");
		return(ret);
	}
	if (fp == NULL)
		outputFp = stdout;
	else
		outputFp = fp;
	fprintf(outputFp,"%s\n",recStr);
	return(ret);
}	

// 将一个PK密钥写入到串中
int UnionPutPKIntoRecStr(PUnionPK pPK,char *recStr,int sizeOfRecStr)
{
	char		tmpBuf[100+1];
	int		ret = 0,len = 0;
		
	if (pPK == NULL)
	{
		UnionUserErrLog("in UnionPutPKIntoRecStr:: null pointer!\n");
		return(errCodeParameter);
	}

	memset(recStr,0,sizeOfRecStr);
	if ((ret = UnionPutRecFldIntoRecStr("fullName",pPK->fullName,strlen(pPK->fullName),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutPKIntoRecStr:: UnionPutRecFldIntoRecStr fullName!\n");
		return(ret);
	}
	len += ret;
	
	// 将不可见的 二进制 转化为 ASCII 码
	memset(tmpBuf, 0, sizeof(tmpBuf));
	bcdhex_to_aschex(pPK->value, UnionGetAscCharLengthOfPK(pPK->length) / 2, tmpBuf);		
	if ((ret = UnionPutRecFldIntoRecStr("value",tmpBuf, strlen(tmpBuf), recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutPKIntoRecStr:: UnionPutRecFldIntoRecStr value!\n");
		return(ret);
	}
	len += ret;
	if ((ret = UnionPutRecFldIntoRecStr("checkValue",pPK->checkValue,strlen(pPK->checkValue),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutPKIntoRecStr:: UnionPutRecFldIntoRecStr checkValue!\n");
		return(ret);
	}
	len += ret;
	if ((ret = UnionPutRecFldIntoRecStr("activeDate",pPK->activeDate,strlen(pPK->activeDate),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutPKIntoRecStr:: UnionPutRecFldIntoRecStr activeDate!\n");
		return(ret);
	}
	len += ret;
	
	// 将不可见的 二进制 转化为 ASCII 码
	memset(tmpBuf, 0, sizeof(tmpBuf));
	bcdhex_to_aschex(pPK->oldValue, UnionGetAscCharLengthOfPK(pPK->length) / 2, tmpBuf);
	if ((ret = UnionPutRecFldIntoRecStr("oldValue", tmpBuf,strlen(tmpBuf),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutPKIntoRecStr:: UnionPutRecFldIntoRecStr oldValue!\n");
		return(ret);
	}
	len += ret;
	if ((ret = UnionPutRecFldIntoRecStr("oldCheckValue",pPK->oldCheckValue,strlen(pPK->oldCheckValue),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutPKIntoRecStr:: UnionPutRecFldIntoRecStr oldCheckValue!\n");
		return(ret);
	}
	len += ret;
	if ((ret = UnionPutRecFldIntoRecStr("container",pPK->container,strlen(pPK->container),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutPKIntoRecStr:: UnionPutRecFldIntoRecStr container!\n");
		return(ret);
	}
	len += ret;
	sprintf(tmpBuf,"%ld",pPK->maxUseTimes);
	if ((ret = UnionPutRecFldIntoRecStr("maxUseTimes",tmpBuf,strlen(tmpBuf),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutPKIntoRecStr:: UnionPutRecFldIntoRecStr maxUseTimes!\n");
		return(ret);
	}
	len += ret;
	sprintf(tmpBuf,"%ld",pPK->useTimes);
	if ((ret = UnionPutRecFldIntoRecStr("useTimes",tmpBuf,strlen(tmpBuf),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutPKIntoRecStr:: UnionPutRecFldIntoRecStr useTimes!\n");
		return(ret);
	}
	len += ret;
	sprintf(tmpBuf,"%ld",pPK->maxEffectiveDays);
	if ((ret = UnionPutRecFldIntoRecStr("maxEffectiveDays",tmpBuf,strlen(tmpBuf),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutPKIntoRecStr:: UnionPutRecFldIntoRecStr maxEffectiveDays!\n");
		return(ret);
	}
	len += ret;
	sprintf(tmpBuf,"%ld",pPK->lastUpdateTime);
	if ((ret = UnionPutRecFldIntoRecStr("lastUpdateTime",tmpBuf,strlen(tmpBuf),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutPKIntoRecStr:: UnionPutRecFldIntoRecStr lastUpdateTime!\n");
		return(ret);
	}
	len += ret;
	sprintf(tmpBuf,"%ld",pPK->oldVerEffective);
	if ((ret = UnionPutRecFldIntoRecStr("oldVerEffective",tmpBuf,strlen(tmpBuf),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutPKIntoRecStr:: UnionPutRecFldIntoRecStr oldVerEffective!\n");
		return(ret);
	}
	len += ret;
	sprintf(tmpBuf,"%ld",pPK->windowBetweenKeyVer);
	if ((ret = UnionPutRecFldIntoRecStr("windowBetweenKeyVer",tmpBuf,strlen(tmpBuf),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutPKIntoRecStr:: UnionPutRecFldIntoRecStr windowBetweenKeyVer!\n");
		return(ret);
	}
	len += ret;
#ifdef _UnionPK_4_x_
	sprintf(tmpBuf,"%ld",pPK->isWritingLocked);
#else
	sprintf(tmpBuf,"0");
#endif
	if ((ret = UnionPutRecFldIntoRecStr("isWritingLocked",tmpBuf,strlen(tmpBuf),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutPKIntoRecStr:: UnionPutRecFldIntoRecStr isWritingLocked!\n");
		return(ret);
	}
	len += ret;
	sprintf(tmpBuf,"%ld",pPK->length);
	if ((ret = UnionPutRecFldIntoRecStr("length",tmpBuf,strlen(tmpBuf),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutPKIntoRecStr:: UnionPutRecFldIntoRecStr length!\n");
		return(ret);
	}
	len += ret;
	sprintf(tmpBuf,"%ld",pPK->type);
	if ((ret = UnionPutRecFldIntoRecStr("type",tmpBuf,strlen(tmpBuf),recStr+len,sizeOfRecStr-1-len)) < 0)
	{
		UnionUserErrLog("in UnionPutPKIntoRecStr:: UnionPutRecFldIntoRecStr type!\n");
		return(ret);
	}
	len += ret;
	return(len);
}

// 2008/7/18增加
// 从一个串中读 PK 密钥
int UnionReadPKFromRecStr(char *recStr,int lenOfRecStr,PUnionPK pPK)
{
	char		tmpBuf[100+1];
	int		ret = 0;
	int		len;
		
	if (pPK == NULL)
	{
		UnionUserErrLog("in UnionReadPKFromRecStr:: null pointer!\n");
		return(errCodeParameter);
	}

	memset(pPK,0,sizeof(*pPK));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"fullName",pPK->fullName,sizeof(pPK->fullName))) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromRecStr:: UnionReadRecFldFromRecStr fullName!\n");
		return(ret);
	}
		
	// 将 ASCII 码 转化为 二进制
	memset(tmpBuf, 0, sizeof(tmpBuf));	
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"value",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromRecStr:: UnionReadRecFldFromRecStr value!\n");
		return(ret);
	}
	
	switch (strlen(tmpBuf))
	{
		case	0:
		case	128:
			len = con512RSAPair;
			break;
		case	256:
			len = con1024RSAPair;
			break;
		case	512:
			len = con2048RSAPair;
			break;
		case	1024:
			len = con4096RSAPair;
			break;
		default:
			UnionUserErrLog("in UnionReadPKFromRecStr:: pkey->value [%s]\n", tmpBuf);
			return(errCodeParameter);
	}
	aschex_to_bcdhex(tmpBuf, UnionGetAscCharLengthOfPK(len), pkey->value);
	
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"checkValue",pPK->checkValue,sizeof(pPK->checkValue))) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromRecStr:: UnionReadRecFldFromRecStr checkValue!\n");
		return(ret);
	}
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"activeDate",pPK->activeDate,sizeof(pPK->activeDate))) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromRecStr:: UnionReadRecFldFromRecStr activeDate!\n");
		return(ret);
	}
	
	memset(tmpBuf, 0, sizeof(tmpBuf));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"oldValue",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromRecStr:: UnionReadRecFldFromRecStr oldValue!\n");
		return(ret);
	}
	
	switch (strlen(tmpBuf))
	{
		case	0:
		case	128:
			len = con512RSAPair;
			break;
		case	256:
			len = con1024RSAPair;
			break;
		case	512:
			len = con2048RSAPair;
			break;
		case	1024:
			len = con4096RSAPair;
			break;
		default:
			UnionUserErrLog("in UnionReadPKFromRecStr:: pkey->value [%s]\n", tmpBuf);
			return(errCodeParameter);
	}
	aschex_to_bcdhex(tmpBuf, UnionGetAscCharLengthOfPK(len), pkey->value);
	
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"oldCheckValue",pPK->oldCheckValue,sizeof(pPK->oldCheckValue))) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromRecStr:: UnionReadRecFldFromRecStr oldCheckValue!\n");
		return(ret);
	}
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"container",pPK->container,sizeof(pPK->container))) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromRecStr:: UnionReadRecFldFromRecStr container!\n");
		return(ret);
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"maxUseTimes",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromRecStr:: UnionReadRecFldFromRecStr maxUseTimes!\n");
		return(ret);
	}
	pPK->maxUseTimes = atol(tmpBuf);
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"useTimes",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromRecStr:: UnionReadRecFldFromRecStr useTimes!\n");
		return(ret);
	}
	pPK->useTimes = atol(tmpBuf);
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"maxEffectiveDays",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromRecStr:: UnionReadRecFldFromRecStr maxEffectiveDays!\n");
		return(ret);
	}
	pPK->maxEffectiveDays = atol(tmpBuf);
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"lastUpdateTime",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromRecStr:: UnionReadRecFldFromRecStr lastUpdateTime!\n");
		return(ret);
	}
	pPK->lastUpdateTime = atol(tmpBuf);
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"oldVerEffective",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromRecStr:: UnionReadRecFldFromRecStr oldVerEffective!\n");
		return(ret);
	}
	pPK->oldVerEffective = atoi(tmpBuf);
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"windowBetweenKeyVer",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromRecStr:: UnionReadRecFldFromRecStr windowBetweenKeyVer!\n");
		return(ret);
	}
	pPK->windowBetweenKeyVer = atoi(tmpBuf);
#ifdef _UnionPK_4_x_
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"isWritingLocked",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromRecStr:: UnionReadRecFldFromRecStr isWritingLocked!\n");
		return(ret);
	}
	pPK->isWritingLocked = atoi(tmpBuf);
#endif
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"length",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromRecStr:: UnionReadRecFldFromRecStr length!\n");
		return(ret);
	}
	pPK->length = atoi(tmpBuf);
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"type",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadPKFromRecStr:: UnionReadRecFldFromRecStr type!\n");
		return(ret);
	}
	pPK->type = atoi(tmpBuf);
	return(0);
}
