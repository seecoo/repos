#ifndef _MemCached_H
#define _MemCached_H

int UnionConnectMemcached();

// �ر�Memcached
int UnionCloseMemcached_Conf();

// ���Memcached
int UnionFlushMemcached();

//����Ƿ����
int UnionExistMemcachedValue(char *key);

// ����Memcachedֵ
int UnionSetMemcachedValue(char *key,char *value,int lenOfValue,int expiration);

// ��ȡMemcachedֵ
int UnionGetMemcachedValue(char *key,char *value,int sizeofBuf);

// ɾ��Memcachedֵ
int UnionDeleteMemcachedValue(char *key);

//����Ƿ����
int UnionExistMemcachedValue(char *key);


/* 
���ܣ�	������ָ��ģʽ��ƥ������õ�keys
������	reqKey[in]		�������
	resKeys[out]		��Ӧ�����б����ŷָ�
	sizeofBuf[in]		��Ӧ�����б��С
����ֵ��>=0			�ɹ�������������
	<0			ʧ�ܣ����ش�����
*/
int  UnionGetMemcachedKeysByAlikeName(char *reqKey,char *resKeys,int sizeofBuf);

/* 
���ܣ�	ɾ����ָ��ģʽ��ƥ������õ�keys
������	reqKey[in]		�������
����ֵ��>=0			�ɹ�������������
	<0			ʧ�ܣ����ش�����
*/
int  UnionDeleteMemcachedKeysByAlikeName(char *key);

/*
���ܣ�  ����IP��ַ�Ͷ˿�,����ָ��memcached��״̬
������  ipAddr[in]              IP��ַ
        port[in]                �˿ں�
����ֵ��>=0                     �ɹ�
        <0                      ʧ�ܣ����ش�����
*/
int UnionTestMemcachedStatus(char *ipAddr, int port);

#endif
