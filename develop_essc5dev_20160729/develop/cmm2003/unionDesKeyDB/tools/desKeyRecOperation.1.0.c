#include <stdio.h>
#include <string.h>

#include "unionDesKeyDB.h"
#include "UnionLog.h"


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
