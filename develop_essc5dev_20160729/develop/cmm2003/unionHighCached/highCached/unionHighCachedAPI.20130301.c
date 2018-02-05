//	Author: zhangyongding
//	Date: 2012-10-19

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionREC.h"
#include "UnionMD5.h"
#include "unionCommand.h"

#include "unionHighCachedAPI.h"
#include "unionMemcached.h"
#include "unionErrCode.h"
#include "unionDataSync.h"

typedef enum 
{
	HIGH_MEMCACHED = 0,
	HIGH_REDIS = 1
} TUnionHighCachedVersion;

//extern TUnionDataSyncIni	gunionDataSyncIni;
#define	maxTableNum		8

#ifndef __hpux
int	gunionIsUseHighCached = 1;
int UnionIsUseHighCached()
{
	if (gunionIsUseHighCached && (UnionReadIntTypeRECVar("isUseHighCached") > 0))
		return(1);
	else
		return(0);
}

void UnionSetIsUseHighCached()
{
	gunionIsUseHighCached = 1;
	return;
}

void UnionSetIsNotUseHighCached()
{
	gunionIsUseHighCached = 0;
	return;
}

int UnionGetKeyNameOfHighCached(char *tableName,char *sql,char *keyName)
{
	int	i;
	int	len = 0;
	int	tableNum = 0;
	char	tableGrp[maxTableNum][128];
	
	// ƴ�ֱ��崮
	if ((tableNum = UnionSeprateVarStrIntoVarGrp(tableName,strlen(tableName),',',tableGrp,maxTableNum)) < 0)
	{
		UnionUserErrLog("in UnionGetKeyNameOfHighCached:: UnionSeprateVarStrIntoVarGrp[%s]!\n",tableName);
		return(tableNum);
	}
	
	for (i = 0; i < tableNum; i ++)
		len += sprintf(keyName + len,"%s:",tableGrp[i]);
	
	if ((sql != NULL) && (strlen(sql) > 0))
		len += UnionMD5((unsigned char *)sql, strlen(sql), (unsigned char *)keyName+len);
	return(len);
}

int UnionGetHighCachedVersion()
{
	char	*version = NULL;
	
	if ((version = UnionReadStringTypeRECVar("versionOfHighCached")) == NULL)
		return(HIGH_MEMCACHED);
	
	if (strcasecmp(version,"memcached") == 0)
		return(HIGH_MEMCACHED);
	else if (strcasecmp(version,"redis") == 0)
		return(HIGH_REDIS);
	else
		return(HIGH_MEMCACHED);
}

// ���ӻ���
int UnionConnectHighCachedService()
{
	// ����Ƿ�ʹ�ø��ٻ���
	if (UnionReadIntTypeRECVar("isUseHighCached") <= 0)
		return(0);

	switch(UnionGetHighCachedVersion())
	{
		case	HIGH_MEMCACHED:
			return(UnionConnectMemcached());
		default:
			return(UnionConnectMemcached());
	}
}

// �رջ���
int UnionCloseHighCachedService()
{
	// ����Ƿ�ʹ�ø��ٻ���
	if (UnionReadIntTypeRECVar("isUseHighCached") <= 0)
		return(0);

	switch(UnionGetHighCachedVersion())
	{
		case	HIGH_MEMCACHED:
			return(UnionCloseMemcached());
		default:
			return(UnionCloseMemcached());
	}
}

// ��ջ���
int UnionFlushHighCachedService2(int syncFlag)
{
	int	ret;
	int	len = 0;
	char	msg[1024];

	// ����Ƿ�ʹ�ø��ٻ���
	if (UnionReadIntTypeRECVar("isUseHighCached") <= 0)
		return(0);

	if (syncFlag)
	{
		msg[0] = '0' + conCache;
		msg[1] = '0' + conWaitResult;
		len = 2;
		len += snprintf(msg + len,sizeof(msg) - len,"flush:");
	}
	
	if (syncFlag && UnionDataSyncIsWaitResult())
	{
		if ((ret = UnionDataSyncSendToSocket(1,(unsigned char *)msg,len)) < 0)
		{
			UnionUserErrLog("in UnionFlushHighCachedService2:: UnionDataSyncSendToSocket msg = [%s] !\n",msg);
			return(ret);
		}
	}

	switch(UnionGetHighCachedVersion())
	{
		case	HIGH_MEMCACHED:
			ret = UnionFlushMemcached();
			break;
		default:
			ret = UnionFlushMemcached();
			break;
	}
	if (syncFlag && !UnionDataSyncIsWaitResult())
	{
		if (UnionDataSyncSendMsg((unsigned char *)msg + 2,len - 2,conCache,conNotWaitResult) < 0 )
			UnionUserErrLog("in UnionFlushHighCachedService2:: UnionDataSyncSendMsg msg = [%s] !\n",msg + 2);
	}
	return(ret);
}
int UnionFlushHighCachedService()
{
	return(UnionFlushHighCachedService2(1));
}
// ���û���ֵ
int UnionSetHighCachedValue2(int syncFlag,char *key,char *value,int lenOfValue,int expiration)
{
	int	ret;
	int	len = 0;
	char	msg[81920];
	char	tmpBuf[32];
	
	// ����Ƿ�ʹ�ø��ٻ���
	if (UnionReadIntTypeRECVar("isUseHighCached") <= 0)
		return(0);

	if (syncFlag)
	{
		msg[0] = '0' + conCache;
		msg[1] = '0' + conWaitResult;
		len = 2;
		// key+lenOfValue+value+expiration
		len += snprintf(msg + len,sizeof(msg) - len,"set:%04zu%s%04d",strlen(key),key,lenOfValue);
		memcpy(msg + len,value,lenOfValue);
		len += lenOfValue;
		snprintf(tmpBuf,sizeof(tmpBuf),"%d",expiration);
		len += snprintf(msg + len,sizeof(msg) - len,"%04zu%s",strlen(tmpBuf),tmpBuf);
	}
	
	if (syncFlag && UnionDataSyncIsWaitResult())
	{
		if ((ret = UnionDataSyncSendToSocket(1,(unsigned char *)msg,len)) < 0)
		{
			UnionUserErrLog("in UnionSetHighCachedValue2:: UnionDataSyncSendToSocket msg = [%s] !\n",msg);
			return(ret);
		}
	}
	
	switch(UnionGetHighCachedVersion())
	{
		case	HIGH_MEMCACHED:
			ret = UnionSetMemcachedValue(key,value,lenOfValue,expiration);
			break;
		default:
			ret = UnionSetMemcachedValue(key,value,lenOfValue,expiration);
			break;
	}
	
	if (syncFlag && !UnionDataSyncIsWaitResult())
	{
		if (UnionDataSyncSendMsg((unsigned char *)msg + 2,len - 2,conCache,conNotWaitResult) < 0 )
			UnionUserErrLog("in UnionSetHighCachedValue2:: UnionDataSyncSendMsg msg = [%s] !\n",msg + 2);
	}

	return(ret);
}
int UnionSetHighCachedValue(char *key,char *value,int lenOfValue,int expiration)
{
	//modify by linxj  20150430 ֻ������֧�ֱ�����Դͬ�����׳�ͬ����Ϣ
	int	i = 0;
	UnionReloadDataSyncIni();
	//for (i=0; i<gunionDataSyncIni.syncNum; i++)
	for (i=0; i<getDataSyncIniPtr()->syncNum; i++)
	{
		//if (gunionDataSyncIni.syncAddr[i].highCachedNeedSync == 1)
		if (getDataSyncIniPtr()->syncAddr[i].highCachedNeedSync == 1)
			return(UnionSetHighCachedValue2(1,key,value,lenOfValue,expiration));
	}
	return(UnionSetHighCachedValue2(0,key,value,lenOfValue,expiration));
	//return(UnionSetHighCachedValue2(1,key,value,lenOfValue,expiration));
	//modify end 20150430
}

// ��ȡ����ֵ
int UnionGetHighCachedValue(char *key,char *value,int sizeofBuf)
{
	// ����Ƿ�ʹ�ø��ٻ���
	if (UnionReadIntTypeRECVar("isUseHighCached") <= 0)
		return(0);

	switch(UnionGetHighCachedVersion())
	{
		case	HIGH_MEMCACHED:
			return(UnionGetMemcachedValue(key,value,sizeofBuf));
		default:
			return(UnionGetMemcachedValue(key,value,sizeofBuf));
	}
}

// ɾ������ֵ
int UnionDeleteHighCachedValue2(int syncFlag,char *key)
{
	int	ret;
	int	len = 0;
	char	msg[1024];

	// ����Ƿ�ʹ�ø��ٻ���
	if (UnionReadIntTypeRECVar("isUseHighCached") <= 0)
		return(0);

	if (syncFlag)
	{
		msg[0] = '0' + conCache;
		msg[1] = '0' + conWaitResult;
		len = 2;
		len += snprintf(msg + len,sizeof(msg) - len,"delete:%s",key);
	}
	
	if (syncFlag && UnionDataSyncIsWaitResult())
	{
		if ((ret = UnionDataSyncSendToSocket(1,(unsigned char *)msg,len)) < 0)
		{
			UnionUserErrLog("in UnionDeleteHighCachedValue2:: UnionDataSyncSendToSocket msg = [%s] !\n",msg);
			return(ret);
		}
	}

	switch(UnionGetHighCachedVersion())
	{
		case	HIGH_MEMCACHED:
			ret = UnionDeleteMemcachedValue(key);
			break;
		default:
			ret = UnionDeleteMemcachedValue(key);
			break;
	}

	if (syncFlag && !UnionDataSyncIsWaitResult())
	{
		if (UnionDataSyncSendMsg((unsigned char *)msg + 2,len - 2,conCache,conNotWaitResult) < 0 )
			UnionUserErrLog("in UnionDeleteHighCachedValue2:: UnionDataSyncSendMsg msg = [%s] !\n",msg + 2);
	}
	return(ret);
}
int UnionDeleteHighCachedValue(char *key)
{
	return(UnionDeleteHighCachedValue2(1,key));
}

//����Ƿ����
int UnionExistHighCachedValue(char *key)
{
	// ����Ƿ�ʹ�ø��ٻ���
	if (UnionReadIntTypeRECVar("isUseHighCached") <= 0)
		return(0);

	switch(UnionGetHighCachedVersion())
	{
		case	HIGH_MEMCACHED:
			return(UnionExistMemcachedValue(key));
		default:
			return(UnionExistMemcachedValue(key));
	}
}

/* 
���ܣ�	������ָ��ģʽ��ƥ������õ�keys
������	reqKey[in]		�������
	resKeys[out]		��Ӧ�����б����ŷָ�
	sizeofBuf[in]		��Ӧ�����б��С
����ֵ��>=0			�ɹ�������������
	<0			ʧ�ܣ����ش�����
*/
int  UnionGetHighCachedKeysByAlikeName(char *reqKey,char *resKeys,int sizeofBuf)
{
	// ����Ƿ�ʹ�ø��ٻ���
	if (UnionReadIntTypeRECVar("isUseHighCached") <= 0)
		return(0);

	switch(UnionGetHighCachedVersion())
	{
		case	HIGH_MEMCACHED:
			return(UnionGetMemcachedKeysByAlikeName(reqKey,resKeys,sizeofBuf));
		default:
			return(UnionGetMemcachedKeysByAlikeName(reqKey,resKeys,sizeofBuf));
	}
}

/* 
���ܣ�	ɾ����ָ��ģʽ��ƥ������õ�keys
������	key[in]		�������
����ֵ��>=0		�ɹ�������������
	<0		ʧ�ܣ����ش�����
*/
int  UnionDeleteHighCachedKeysByAlikeName(char *key)
{
	// ����Ƿ�ʹ�ø��ٻ���
	if (UnionReadIntTypeRECVar("isUseHighCached") <= 0)
		return(0);

	switch(UnionGetHighCachedVersion())
	{
		case	HIGH_MEMCACHED:
			return(UnionDeleteMemcachedKeysByAlikeName(key));
		default:
			return(UnionDeleteMemcachedKeysByAlikeName(key));
	}
}

/*
���ܣ�  ����IP��ַ�Ͷ˿�,����ָ�������״̬
������  ipAddr[in]              IP��ַ
        port[in]                �˿ں�
����ֵ��>=0                     �ɹ�
        <0                      ʧ��,���ش�����
*/
int UnionTestHighCachedStatus(char *ipAddr, int port)
{
	switch(UnionGetHighCachedVersion())
	{
		case	HIGH_MEMCACHED:
			return(UnionTestMemcachedStatus(ipAddr,port));
		default:
			return(UnionTestMemcachedStatus(ipAddr,port));
	}
}

int UnionPrintHighCachedKeysToFile(int isKey,FILE *fp,char *key)
{
	int	ret;
	int	backup_flag = -1;
	int	master_num = 0;
	int	backup_num = 0;
	int	num = 0;
	char	tmpBuf[8192000];
	char	*ptr1 = NULL;
	char	*ptr2 = NULL;
	
	if (fp == NULL)
		fp = stdout;
		
	if ((ret = UnionConnectHighCachedService()) < 0)
	{
		UnionUserErrLog("in UnionPrintHighCachedKeysToFile:: UnionConnectHighCachedService!\n");
		return(ret);
	}
	
	if ((ret = UnionGetHighCachedKeysByAlikeName(key,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionPrintHighCachedKeysToFile:: UnionGetHighCachedKeysByAlikeName key[%s]!\n",key);
		return(ret);
	}

	if (isKey)
		fprintf(fp,"%s\n","[  ���  ][*** ��Կ���� ***]");
	else
		fprintf(fp,"%s\n","[  ���  ][*** ���� ***]");
	ptr1 = tmpBuf;
	while ((ptr2 = strchr(ptr1,',')))
	{
		*ptr2 = 0;
		if (strncmp(ptr1,"***",3) == 0)
		{
			backup_flag ++;
			fprintf(fp,"***%s***\n",ptr1);
		}
		else	
		{
			if (backup_flag)
			{
				if (isKey)
					fprintf(fp,"[%8d][%s]\n",++backup_num,strchr(ptr1,':')+1);
				else
					fprintf(fp,"[%8d][%s]\n",++backup_num,ptr1);
			}
			else
			{
				if (isKey)
					fprintf(fp,"[%8d][%s]\n",++master_num,strchr(ptr1,':')+1);
				else
					fprintf(fp,"[%8d][%s]\n",++master_num,ptr1);
			}
		}
		num = master_num + backup_num;
		ptr1 = ptr2+1;
#ifndef _WIN32
		if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
#endif
	}
	fprintf(fp,"���������� = [%d] ���������� = [%d] ������ = [%d]\n",master_num,backup_num,num);
	return(0);
}

#else // __hpux #if ( defined __hpux )
int	gunionIsUseHighCached = 0;

int UnionIsUseHighCached()
{
	return(0);
}

void UnionSetIsUseHighCached()
{
	return;
}

void UnionSetIsNotUseHighCached()
{
	return;
}

int UnionGetKeyNameOfHighCached(char *tableName,char *sql,char *keyName)
{
	return(0);
}

int UnionGetHighCachedVersion()
{
	return(0);
}

// ���ӻ���
int UnionConnectHighCachedService()
{
	return(0);
}

// �رջ���
int UnionCloseHighCachedService()
{
	return(0);
}

// ��ջ���
int UnionFlushHighCachedService2(int syncFlag)
{
	return(0);
}
int UnionFlushHighCachedService()
{
	return(0);
}
// ���û���ֵ
int UnionSetHighCachedValue2(int syncFlag,char *key,char *value,int lenOfValue,int expiration)
{
	return(0);
}
int UnionSetHighCachedValue(char *key,char *value,int lenOfValue,int expiration)
{
	return(0);
}

// ��ȡ����ֵ
int UnionGetHighCachedValue(char *key,char *value,int sizeofBuf)
{
	return(0);
}

// ɾ������ֵ
int UnionDeleteHighCachedValue2(int syncFlag,char *key)
{
	return(0);
}
int UnionDeleteHighCachedValue(char *key)
{
	return(0);
}

//����Ƿ����
int UnionExistHighCachedValue(char *key)
{
	return(0);
}

int  UnionGetHighCachedKeysByAlikeName(char *reqKey,char *resKeys,int sizeofBuf)
{
	return(0);
}

int  UnionDeleteHighCachedKeysByAlikeName(char *key)
{
	return(0);
}

int UnionTestHighCachedStatus(char *ipAddr, int port)
{
	return(0);
}

int UnionPrintHighCachedKeysToFile(int isKey,FILE *fp,char *key)
{
	return(0);
}
#endif // __hpux

