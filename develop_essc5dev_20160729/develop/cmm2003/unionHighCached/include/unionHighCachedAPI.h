#ifndef _HighCached_H_
#define _HighCached_H_

#include <stdio.h>

int UnionIsUseHighCached();

void UnionSetIsUseHighCached();

void UnionSetIsNotUseHighCached();

int UnionGetKeyNameOfHighCached(char *tableName,char *sql,char *keyName);

// ���ӻ���
int UnionConnectHighCachedService();

// �رջ���
int UnionCloseHighCachedService();

// ��ջ���
int UnionFlushHighCachedService2(int syncFlag);
int UnionFlushHighCachedService();

//����Ƿ����
int UnionExistHighCachedValue(char *key);

// ���û���ֵ
int UnionSetHighCachedValue2(int syncFlag,char *key,char *value,int lenOfValue,int expiration);
int UnionSetHighCachedValue(char *key,char *value,int lenOfValue,int expiration);

// ��ȡ����ֵ
int UnionGetHighCachedValue(char *key,char *value,int sizeofBuf);

// ɾ������ֵ
int UnionDeleteHighCachedValue2(int syncFlag,char *key);
int UnionDeleteHighCachedValue(char *key);

//����Ƿ����
int UnionExistHighCachedValue(char *key);

/* 
���ܣ�	������ָ��ģʽ��ƥ������õ�keys
������	reqKey[in]		�������
	resKeys[out]		��Ӧ�����б����ŷָ�
	sizeofBuf[in]		��Ӧ�����б��С
����ֵ��>=0			�ɹ�������������
	<0			ʧ�ܣ����ش�����
*/
int  UnionGetHighCachedKeysByAlikeName(char *reqKey,char *resKeys,int sizeofBuf);

/* 
���ܣ�	ɾ����ָ��ģʽ��ƥ������õ�keys
������	key[in]		�������
����ֵ��>=0			�ɹ�������������
	<0			ʧ�ܣ����ش�����
*/
int  UnionDeleteHighCachedKeysByAlikeName(char *key);
/*
���ܣ�  ����IP��ַ�Ͷ˿�,����ָ�������״̬
������  ipAddr[in]              IP��ַ
        port[in]                �˿ں�
����ֵ��>=0                     �ɹ�
        <0                      ʧ��,���ش�����
*/
int UnionTestHighCachedStatus(char *ipAddr, int port);

int UnionPrintHighCachedKeysToFile(int isKey,FILE *fp,char *key);

#endif
