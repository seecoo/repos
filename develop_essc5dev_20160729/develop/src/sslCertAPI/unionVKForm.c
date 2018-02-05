/*
	create by hzh in 2010.3.9
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionVKForm.h"
#include "unionRSA.h"
#include "UnionStr.h"

/*
���ܣ�ȡDER��ʽ��ָʾ�ĳ���,��֮���������ڵ�ַ��λ��
���룺
	derByte��der��ʽ���ַ���
	derByteLen��derByte�ĳ���
�����
	���ִ��ĵ�һ������ֵ���ڵ�ַ��λ��
���أ�
	>= �ɹ�����ʾDER��ʽ��ָʾ�ĳ���
	< 0. ʧ��
*/
int UnionFetchLenOfDerVal(unsigned char *derByte,int derByteLen,int *locationOfVal) 
{
		// �ж���һ���ֵĳ���
		int lenOfNextPart = 0;
		int bitsOfLenFlag = 0;
		int i = 0;
		int lenOfStr = 0;
		int offset = 0;

		if (derByte == NULL)
			return -1;

		if ((derByte[offset] & 0xFF) <= 0x80) {
			lenOfNextPart = (derByte[offset] & 0xFF);
			offset++;
		} else {
			// ����ָʾλ
			bitsOfLenFlag = (derByte[offset] & 0xFF) - 0x80;
			offset++;
			if (offset + bitsOfLenFlag > derByteLen)
				return -2;

			for (i = 0, lenOfNextPart = 0; i < bitsOfLenFlag; i++, offset++)
				lenOfNextPart += ((derByte[offset] & 0xFF) << ((bitsOfLenFlag - i - 1) * 8));
		}
		lenOfStr = lenOfNextPart + offset;
		if (lenOfStr > derByteLen) {
			return -3;
		}
		*locationOfVal = offset;
		return lenOfNextPart;
}

/*
���ܣ�ȡDER��ʽ��ָʾ�����Գ���,�����Ե�ֵ
���룺
	derByte��der��ʽ���ַ���
	derByteLen��derByte�ĳ���
	derType��	���Ե�����
�����
	pval��ָ������ֵ��ַ��ָ��
	valLen������ֵ�ĳ���
���أ�
	>=0 �ɹ�����ʾ��һ�����Ե�λ��
	<0 ʧ��
*/
int UnionGetNextValOfDer(unsigned char *derByte,int derByteLen,int derType,unsigned char **pval,int *valLen)
{
		int lenOfNextPart = 0;
		int offset = 0;
		int locationOfVal = 0;
	
		if ((derByte[offset] & 0xFF) != derType) {
			return -1;
		}
		offset++;

		if ( (lenOfNextPart = UnionFetchLenOfDerVal(derByte+offset,derByteLen-1,&locationOfVal)) < 0)
		{
			printf("in UnionGetNextValOfDer::UnionFetchLenOfDerVal failed !lenOfNextPart = [%d]\n",lenOfNextPart);
			return lenOfNextPart;
		}
		offset += locationOfVal;
		*pval = (unsigned char *)malloc(lenOfNextPart);
		memcpy(*pval,derByte + offset,lenOfNextPart);
		
		offset += lenOfNextPart;
		*valLen = lenOfNextPart;

		return offset;
}

/*
���ܣ�����DER��ʽ��˽Կ(��չ��ascHex)��ȡ��DER��ʽ�Ĺ�Կ(��չ��ascHex)
���룺
	derVkAscHex��DER��ʽ��˽Կ(��չ��ascHex)
�����
	derPkAscHex��DER��ʽ�Ĺ�Կ(��չ��ascHex)
���أ�
	>=0 �ɹ���
	<0 ʧ��
*/
int UnionGetPKFromVK(char *derVkAscHex,char *derPkAscHex)
{
	unsigned char *version = NULL;
	unsigned char *modulues = NULL;
	unsigned char *publicExponent = NULL;
	unsigned char *privateExponent = NULL;
	unsigned char *primeP = NULL;
	unsigned char *primeQ = NULL;
	unsigned char *primeExponentP = NULL;
	unsigned char *primeExponentQ = NULL;
	unsigned char *crtCoefficient = NULL;
	int  offset = 0;
	unsigned char vkByte[8192];
	char pkMod[2048];
	char pkEval[10];
	int len = 0;
	int lenOfvk = 0;
	int location = 0;
	if (derVkAscHex == NULL || derPkAscHex == NULL) {
		printf("UnionGetPKFromVK param is err!\n");
		return -1;
	}
	aschex_to_bcdhex(derVkAscHex,strlen(derVkAscHex),(char *)vkByte);
	lenOfvk = strlen(derVkAscHex)/2;

	offset = 0;
	if (vkByte[offset] != 0x30) {
			return -2;
	}
	offset++;
	if (UnionFetchLenOfDerVal(vkByte+offset,lenOfvk-offset,&location) < 0)
	{
		return -3;
	}
	offset += location;
	if ((location = UnionGetNextValOfDer(vkByte+offset,lenOfvk-offset,DerByteType,&version,&len)) < 0)
			goto UnionGetPKFromVKexit;
	offset += location;

	if ((location = UnionGetNextValOfDer(vkByte+offset,lenOfvk-offset,DerByteType,&modulues,&len)) < 0)
			goto UnionGetPKFromVKexit;

	memset(pkMod,0,sizeof(pkMod));
	if (*modulues == 0x00) {
		memcpy(modulues,modulues+1,len-1);
		len--;
	}
	bcdhex_to_aschex((char *)modulues,len,pkMod);
	offset += location;

	if ((location = UnionGetNextValOfDer(vkByte+offset,lenOfvk-offset,DerByteType,&publicExponent,&len)) < 0)
			goto UnionGetPKFromVKexit;
	memset(pkEval,0,sizeof(pkEval));
	if (*publicExponent == 0x00) {
		memcpy(publicExponent,publicExponent+1,len-1);
		len--;
	}
	bcdhex_to_aschex((char *)publicExponent,len,pkEval);
	offset += location;

	if ((location = UnionGetNextValOfDer(vkByte+offset,lenOfvk-offset,DerByteType,&privateExponent,&len)) < 0)
			goto UnionGetPKFromVKexit;

	offset += location;

	if ((location = UnionGetNextValOfDer(vkByte+offset,lenOfvk-offset,DerByteType,&primeP,&len)) < 0)
			goto UnionGetPKFromVKexit;

	offset += location;

	if ((location = UnionGetNextValOfDer(vkByte+offset,lenOfvk-offset,DerByteType,&primeQ,&len)) < 0)
			goto UnionGetPKFromVKexit;

	offset += location;
	if ((location = UnionGetNextValOfDer(vkByte+offset,lenOfvk-offset,DerByteType,&primeExponentP,&len)) < 0)
			goto UnionGetPKFromVKexit;

	offset += location;
	if ((location = UnionGetNextValOfDer(vkByte+offset,lenOfvk-offset,DerByteType,&primeExponentQ,&len)) < 0)
			goto UnionGetPKFromVKexit;

	offset += location;
	if ((location = UnionGetNextValOfDer(vkByte+offset,lenOfvk-offset,DerByteType,&crtCoefficient,&len)) < 0)
			goto UnionGetPKFromVKexit;

	offset += location;

	UnionPKCSASN1DER_Encode(pkMod,pkEval,derPkAscHex);

UnionGetPKFromVKexit:
	if (version != NULL) free(version);
	if (modulues != NULL) free(modulues);
	if (publicExponent != NULL) free(publicExponent);
	if (privateExponent != NULL) free(privateExponent);
	if (primeP != NULL) free(primeP);
	if (primeQ != NULL) free(primeQ);
	if (primeExponentP != NULL) free(primeExponentP);
	if (primeExponentQ != NULL) free(primeExponentQ);
	if (crtCoefficient != NULL) free(crtCoefficient);
	if (location < 0)
		return location;

	return 0;
}

/*
���ܣ�����DER��ʽ��˽Կ(��չ��ascHex)��ȡ��������(��չ��ascHex)
���룺
	derVkAscHex��DER��ʽ��˽Կ(��չ��ascHex)
�����
	moduluesAscHex����Կģ(��չ��ascHex)
	publicExponentAscHex: ��Կָ��(��չ��ascHex)
	privateExponentAscHex: ˽Կ��ָ��(��չ��ascHex)
	primePAscHex:	˽Կ��P(��չ��ascHex)
	primeQAscHex: ˽Կ��Q(��չ��ascHex)
	dmp1AscHex: ˽Կ��dmp1(��չ��ascHex)
	dmq1AscHex: ˽Կ��dmq1(��չ��ascHex)
	coefAscHex: ˽Կ��coef(��չ��ascHex)
���أ�
	>=0 �ɹ���
	<0 ʧ��
*/
int UnionUnPackFromVK(char *derVkAscHex,char *moduluesAscHex, char *publicExponentAscHex, char *privateExponentAscHex,char *primePAscHex,char *primeQAscHex,char *dmp1AscHex,char *dmq1AscHex,char *coefAscHex)
{
	unsigned char *version = NULL;
	unsigned char *modulues = NULL;
	unsigned char *publicExponent = NULL;
	unsigned char *privateExponent = NULL;
	unsigned char *primeP = NULL;
	unsigned char *primeQ = NULL;
	unsigned char *dmp1 = NULL;
	unsigned char *dmq1 = NULL;
	unsigned char *coef = NULL;
	int  offset = 0;
	unsigned char vkByte[8192];
	
	int len = 0;
	int lenOfvk = 0;
	int location = 0;
	if (derVkAscHex == NULL || moduluesAscHex == NULL || publicExponentAscHex == NULL 
		|| privateExponentAscHex == NULL || primeQAscHex == NULL || dmp1AscHex == NULL || dmq1AscHex == NULL
		|| coefAscHex == NULL)
	{
		printf("UnionUnPackFromVK param is err!\n");
		return -1;
	}
	aschex_to_bcdhex(derVkAscHex,strlen(derVkAscHex),(char *)vkByte);
	lenOfvk = strlen(derVkAscHex)/2;

	offset = 0;
	if (vkByte[offset] != 0x30) {
			return -2;
	}
	offset++;
	if (UnionFetchLenOfDerVal(vkByte+offset,lenOfvk-offset,&location) < 0)
	{
		return -3;
	}
	offset += location;
	if ((location = UnionGetNextValOfDer(vkByte+offset,lenOfvk-offset,DerByteType,&version,&len)) < 0)
			goto UnionGetPKFromVKexit;
	offset += location;

	if ((location = UnionGetNextValOfDer(vkByte+offset,lenOfvk-offset,DerByteType,&modulues,&len)) < 0)
			goto UnionGetPKFromVKexit;

	if (*modulues == 0x00) {
		memcpy(modulues,modulues+1,len-1);
		len--;
	}
	bcdhex_to_aschex((char *)modulues,len,moduluesAscHex);
	moduluesAscHex[2*len] = '\0';
	offset += location;

	if ((location = UnionGetNextValOfDer(vkByte+offset,lenOfvk-offset,DerByteType,&publicExponent,&len)) < 0)
			goto UnionGetPKFromVKexit;
	if (*publicExponent == 0x00) {
		memcpy(publicExponent,publicExponent+1,len-1);
		len--;
	}
	bcdhex_to_aschex((char *)publicExponent,len,publicExponentAscHex);
	publicExponentAscHex[2*len] = '\0';
	offset += location;

	if ((location = UnionGetNextValOfDer(vkByte+offset,lenOfvk-offset,DerByteType,&privateExponent,&len)) < 0)
			goto UnionGetPKFromVKexit;

	if (*privateExponent == 0x00) {
		memcpy(privateExponent,privateExponent+1,len-1);
		len--;
	}
	bcdhex_to_aschex((char *)privateExponent,len,privateExponentAscHex);
	privateExponentAscHex[2*len] = '\0';

	offset += location;

	if ((location = UnionGetNextValOfDer(vkByte+offset,lenOfvk-offset,DerByteType,&primeP,&len)) < 0)
			goto UnionGetPKFromVKexit;
	if (*primeP == 0x00) {
		memcpy(primeP,primeP+1,len-1);
		len--;
	}
	bcdhex_to_aschex((char *)primeP,len,primePAscHex);
	primePAscHex[2*len] = '\0';

	offset += location;

	if ((location = UnionGetNextValOfDer(vkByte+offset,lenOfvk-offset,DerByteType,&primeQ,&len)) < 0)
			goto UnionGetPKFromVKexit;

	if (*primeQ == 0x00) {
		memcpy(primeQ,primeQ+1,len-1);
		len--;
	}
	bcdhex_to_aschex((char *)primeQ,len,primeQAscHex);
	primeQAscHex[2*len] = '\0';
	offset += location;

	if ((location = UnionGetNextValOfDer(vkByte+offset,lenOfvk-offset,DerByteType,&dmp1,&len)) < 0)
			goto UnionGetPKFromVKexit;

	if (*dmp1 == 0x00) {
		memcpy(dmp1,dmp1+1,len-1);
		len--;
	}
	bcdhex_to_aschex((char *)dmp1,len,dmp1AscHex);
	dmp1AscHex[2*len] = '\0';
	offset += location;
	
	if ((location = UnionGetNextValOfDer(vkByte+offset,lenOfvk-offset,DerByteType,&dmq1,&len)) < 0)
			goto UnionGetPKFromVKexit;
	if (*dmq1 == 0x00) {
		memcpy(dmq1,dmq1+1,len-1);
		len--;
	}
	bcdhex_to_aschex((char *)dmq1,len,dmq1AscHex);
	dmq1AscHex[2*len] = '\0';

	offset += location;
	if ((location = UnionGetNextValOfDer(vkByte+offset,lenOfvk-offset,DerByteType,&coef,&len)) < 0)
			goto UnionGetPKFromVKexit;

	if (*coef == 0x00) {
		memcpy(coef,coef+1,len-1);
		len--;
	}
	bcdhex_to_aschex((char *)coef,len,coefAscHex);
	coefAscHex[2*len] = '\0';
	offset += location;

UnionGetPKFromVKexit:
	if (version != NULL) free(version);
	if (modulues != NULL) free(modulues);
	if (publicExponent != NULL) free(publicExponent);
	if (privateExponent != NULL) free(privateExponent);
	if (primeP != NULL) free(primeP);
	if (primeQ != NULL) free(primeQ);
	if (dmp1 != NULL) free(dmp1);
	if (dmq1 != NULL) free(dmq1);
	if (coef != NULL) free(coef);
	if (location < 0)
		return location;

	return 0;
}
