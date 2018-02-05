#ifndef _HighCached_H_
#define _HighCached_H_

#include <stdio.h>

int UnionIsUseHighCached();

void UnionSetIsUseHighCached();

void UnionSetIsNotUseHighCached();

int UnionGetKeyNameOfHighCached(char *tableName,char *sql,char *keyName);

// 连接缓存
int UnionConnectHighCachedService();

// 关闭缓存
int UnionCloseHighCachedService();

// 清空缓存
int UnionFlushHighCachedService2(int syncFlag);
int UnionFlushHighCachedService();

//检查是否存在
int UnionExistHighCachedValue(char *key);

// 设置缓存值
int UnionSetHighCachedValue2(int syncFlag,char *key,char *value,int lenOfValue,int expiration);
int UnionSetHighCachedValue(char *key,char *value,int lenOfValue,int expiration);

// 读取缓存值
int UnionGetHighCachedValue(char *key,char *value,int sizeofBuf);

// 删除缓存值
int UnionDeleteHighCachedValue2(int syncFlag,char *key);
int UnionDeleteHighCachedValue(char *key);

//检查是否存在
int UnionExistHighCachedValue(char *key);

/* 
功能：	返回与指定模式相匹配的所用的keys
参数：	reqKey[in]		请求键名
	resKeys[out]		响应建名列表，逗号分隔
	sizeofBuf[in]		响应建名列表大小
返回值：>=0			成功，键名的数量
	<0			失败，返回错误码
*/
int  UnionGetHighCachedKeysByAlikeName(char *reqKey,char *resKeys,int sizeofBuf);

/* 
功能：	删除与指定模式相匹配的所用的keys
参数：	key[in]		请求键名
返回值：>=0			成功，键名的数量
	<0			失败，返回错误码
*/
int  UnionDeleteHighCachedKeysByAlikeName(char *key);
/*
功能：  根据IP地址和端口,测试指定缓存的状态
参数：  ipAddr[in]              IP地址
        port[in]                端口号
返回值：>=0                     成功
        <0                      失败,返回错误码
*/
int UnionTestHighCachedStatus(char *ipAddr, int port);

int UnionPrintHighCachedKeysToFile(int isKey,FILE *fp,char *key);

#endif
