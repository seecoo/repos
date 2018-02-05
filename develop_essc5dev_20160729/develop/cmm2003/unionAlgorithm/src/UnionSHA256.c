#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

#include "UnionSHA256.h"

int UnionSHA256(const char *data, size_t len, unsigned char *digest)
{
	unsigned char md_value[EVP_MAX_MD_SIZE];	//���������ժҪֵ������
	unsigned int md_len, i;
	EVP_MD_CTX mdctx;				//EVP��ϢժҪ�ṹ��
	
	//��ʼ��ժҪ�ṹ�� 
        EVP_MD_CTX_init(&mdctx);			

	//����ժҪ�㷨�������㷨���棬���������㷨ʹ��sha256���㷨����ʹ��OpenSSLĬ�����漴���㷨
	EVP_DigestInit_ex(&mdctx, EVP_sha256(), NULL);	

	//����ժҪUpDate���� data ��ժҪ
	EVP_DigestUpdate(&mdctx, data, strlen(data));	

	//ժҪ���������ժҪֵ	
	EVP_DigestFinal_ex(&mdctx, md_value, &md_len);	

	//�ͷ��ڴ�
	EVP_MD_CTX_cleanup(&mdctx);			
	
	for (i = 0; i < md_len; i++)
	{
		sprintf((char *)(digest +(i * 2)), "%02X", md_value[i]);
	}
	return 0;
}
