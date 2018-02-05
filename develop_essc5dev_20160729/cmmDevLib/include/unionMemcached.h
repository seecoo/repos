#ifndef _MemCached_H
#define _MemCached_H

int UnionConnectMemcached();

// 关闭Memcached
int UnionCloseMemcached_Conf();

// 清空Memcached
int UnionFlushMemcached();

//检查是否存在
int UnionExistMemcachedValue(char *key);

// 设置Memcached值
int UnionSetMemcachedValue(char *key,char *value,int lenOfValue,int expiration);

// 读取Memcached值
int UnionGetMemcachedValue(char *key,char *value,int sizeofBuf);

// 删除Memcached值
int UnionDeleteMemcachedValue(char *key);

//检查是否存在
int UnionExistMemcachedValue(char *key);


/* 
功能：	返回与指定模式相匹配的所用的keys
参数：	reqKey[in]		请求键名
	resKeys[out]		响应建名列表，逗号分隔
	sizeofBuf[in]		响应建名列表大小
返回值：>=0			成功，键名的数量
	<0			失败，返回错误码
*/
int  UnionGetMemcachedKeysByAlikeName(char *reqKey,char *resKeys,int sizeofBuf);

/* 
功能：	删除与指定模式相匹配的所用的keys
参数：	reqKey[in]		请求键名
返回值：>=0			成功，键名的数量
	<0			失败，返回错误码
*/
int  UnionDeleteMemcachedKeysByAlikeName(char *key);

/*
功能：  根据IP地址和端口,测试指定memcached的状态
参数：  ipAddr[in]              IP地址
        port[in]                端口号
返回值：>=0                     成功
        <0                      失败，返回错误码
*/
int UnionTestMemcachedStatus(char *ipAddr, int port);

#endif
