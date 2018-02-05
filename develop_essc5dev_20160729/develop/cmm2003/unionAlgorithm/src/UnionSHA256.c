#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

#include "UnionSHA256.h"

int UnionSHA256(const char *data, size_t len, unsigned char *digest)
{
	unsigned char md_value[EVP_MAX_MD_SIZE];	//保存输出的摘要值的数组
	unsigned int md_len, i;
	EVP_MD_CTX mdctx;				//EVP消息摘要结构体
	
	//初始化摘要结构体 
        EVP_MD_CTX_init(&mdctx);			

	//设置摘要算法和密码算法引擎，这里密码算法使用sha256，算法引擎使用OpenSSL默认引擎即软算法
	EVP_DigestInit_ex(&mdctx, EVP_sha256(), NULL);	

	//调用摘要UpDate计算 data 的摘要
	EVP_DigestUpdate(&mdctx, data, strlen(data));	

	//摘要结束，输出摘要值	
	EVP_DigestFinal_ex(&mdctx, md_value, &md_len);	

	//释放内存
	EVP_MD_CTX_cleanup(&mdctx);			
	
	for (i = 0; i < md_len; i++)
	{
		sprintf((char *)(digest +(i * 2)), "%02X", md_value[i]);
	}
	return 0;
}
