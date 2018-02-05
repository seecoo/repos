
#include <stdio.h>
#include <string.h>
#include "global.h"
#include "UnionLog.h"
#include "rsaeuro.h"
#include "r_random.h"
#include "rsa.h"
//#include "nn.h"

#include "md5.h"
#include "UnionRSAEuroFunc.h"
#define PUBE "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010001"


void UnionAscBcd(char *buf, int len, char *result)
{
	int i;

	for( i = 0; i < (len+1)/2; i++)
	{
		result[i] = (buf[i*2] >= 'A')?(buf[i*2]+10-'A'):(buf[i*2]-'0');
		result[i] <<= 4;
		if(len%2 != 0 && len == (i*2+1))
			result[i] |= 0;
		else
			result[i] |= (buf[i*2+1] >= 'A')?(buf[i*2+1]+10-'A'):(buf[i*2+1]-'0');
	}
}

void UnionBcdAsc(char *buf, int len, char *result)
{
	int i;
	for( i = 0; i < len; i++)
	{
		result[i*2]   = (((buf[i]&0xf0) >> 4)> 9)? (((buf[i]&0xf0)>>4)+'A'-10):(((buf[i]&0xf0)>>4)+'0');
		result[i*2+1] = ((buf[i]&0x0f) > 9)?((buf[i]&0x0f)+'A'-10):((buf[i]&0x0f)+'0');
	}
	result[len*2] = 0;
}

int UnionCompress128BCDInto64Bits(char *p128BCDKey,unsigned char *p64BitsKey)
{
	int	i;

	for (i = 0; i < 8; i++)
	{
		if (p128BCDKey[2*i] >= 'A')
			p64BitsKey[i] = (char)((p128BCDKey[2*i] - 0x37) << 4);
		else
			p64BitsKey[i] = (char)((p128BCDKey[2*i] - '0') << 4);
		if (p128BCDKey[2*i+1] >= 'A')
			p64BitsKey[i] |= p128BCDKey[2*i+1] - 0x37;
		else
			p64BitsKey[i] |= p128BCDKey[2*i+1] - '0';
	}
	return(0);
}

int UnionUncompress64BitsInto128BCD(unsigned char *p64BitsKey,char *p128BCDKey)
{
	int	i;
	unsigned char xxc;

	for (i = 0; i < 8; i++)
	{
		xxc = p64BitsKey[i] / 16;
		if (xxc < 0x0a)
			p128BCDKey[2*i] = xxc + '0';
		else
			p128BCDKey[2*i] = xxc + 0x37;

		xxc = p64BitsKey[i] % 16;
		if (xxc < 0x0a)
			p128BCDKey[2*i+1] = xxc + '0';
		else
			p128BCDKey[2*i+1] = xxc + 0x37;
	}
	return(0);
}

int UnionCompressBCDIntoBits(char *pBCDData,int LenOfBCDData,unsigned char *pBitsData)
{
	int	i;

	for (i = 0; i < LenOfBCDData/2; i++)
	{
		if (pBCDData[2*i] >= 'A')
			pBitsData[i] = (char)((pBCDData[2*i] - 0x37) << 4);
		else
			pBitsData[i] = (char)((pBCDData[2*i] - '0') << 4);
		if (pBCDData[2*i+1] >= 'A')
			pBitsData[i] |= pBCDData[2*i+1] - 0x37;
		else
			pBitsData[i] |= pBCDData[2*i+1] - '0';
	}
	return(0);
}

int UnionUncompressBitsIntoBCD(unsigned char *pBitsData,int LenOfBitsData,char *pBCDData)
{
	int	i;
	unsigned char xxc;

	for (i = 0; i < LenOfBitsData; i++)
	{
		xxc = pBitsData[i] / 16;
		if (xxc < 0x0a)
			pBCDData[2*i] = xxc + '0';
		else
			pBCDData[2*i] = xxc + 0x37;

		xxc = pBitsData[i] % 16;
		if (xxc < 0x0a)
			pBCDData[2*i+1] = xxc + '0';
		else
			pBCDData[2*i+1] = xxc + 0x37;
	}
	return(0);
}

int  UnionMD5(unsigned char *pData, unsigned long cbData, unsigned char *pDigest)
{
        MD5_CTX Ctx;
	char	tmpDigest[128];

	MD5Init(&Ctx);
	MD5Update(&Ctx, pData, cbData);
	MD5Final((unsigned char *)tmpDigest,&Ctx);

	UnionUncompressBitsIntoBCD((unsigned char *)tmpDigest,16,(char *)pDigest);

	return(0);
}

static R_RANDOM_STRUCT *InitRandomStruct(void)
{
	static unsigned char seedByte = 0;
	unsigned int bytesNeeded;
	static R_RANDOM_STRUCT randomStruct;

	R_RandomInit(&randomStruct);

	/* Initialize with all zero seed bytes, which will not yield an actual
			 random number output. */

	while (1) {
		R_GetRandomBytesNeeded(&bytesNeeded, &randomStruct);
		if(bytesNeeded == 0)
			break;

		R_RandomUpdate(&randomStruct, &seedByte, 1);
	}

	return(&randomStruct);
}
int UnionGenRSAPair(char *pk,char *vk)
{
        R_RANDOM_STRUCT *randomStruct;  /* random structure */
        R_RSA_PUBLIC_KEY publicKey;
        R_RSA_PRIVATE_KEY privateKey;
	R_RSA_PROTO_KEY protoKey;
	char command[80];
        char pkmodulus[512+1];
        char pkexponent[512+1];
        char vkmodulus[512+1];
        char vkpublicExponent[512+1];
        char vkexponent[512+1];
        char vkprime1[512+1];
        char vkprime2[512+1];
        char vkprimeExponent1[512+1];
        char vkprimeExponent2[512+1];
        char vkcoefficient[512+1];
	int status, keySize;
        int ret;
      	randomStruct = InitRandomStruct();		/* setup random object */
        protoKey.bits = 1024;
        protoKey.useFermat4 = 1;
        status = R_GeneratePEMKeys(&publicKey, &privateKey, &protoKey, randomStruct);
        if(status)
        {
		return -1;
	}
        memset(pkmodulus,0,sizeof pkmodulus);
        memset(pkexponent,0,sizeof pkexponent);
        memset(vkmodulus,0,sizeof vkmodulus);
        memset(vkpublicExponent,0,sizeof vkpublicExponent);
        memset(vkexponent,0,sizeof vkexponent);
        memset(vkprime1,0,sizeof vkprime1);
        memset(vkprime2,0,sizeof vkprime2);
        memset(vkprimeExponent1,0,sizeof vkprimeExponent1);
        memset(vkprimeExponent2,0,sizeof vkprimeExponent2);
        memset(vkcoefficient,0,sizeof vkcoefficient);

        UnionUncompressBitsIntoBCD(publicKey.modulus,128,pkmodulus);
        UnionUncompressBitsIntoBCD(publicKey.exponent,128,pkexponent);
        UnionUncompressBitsIntoBCD(privateKey.modulus,128,vkmodulus);
        UnionUncompressBitsIntoBCD(privateKey.publicExponent,128,vkpublicExponent);
        UnionUncompressBitsIntoBCD(privateKey.exponent,128,vkexponent);
        UnionUncompressBitsIntoBCD(privateKey.prime[1],128,vkprime1);
        UnionUncompressBitsIntoBCD(privateKey.prime[2],128,vkprime2);
        UnionUncompressBitsIntoBCD(privateKey.primeExponent[1],128,vkprimeExponent1);
        UnionUncompressBitsIntoBCD(privateKey.primeExponent[2],128,vkprimeExponent2);
        UnionUncompressBitsIntoBCD(privateKey.coefficient,128,vkcoefficient);

        ret = strlen(pkmodulus);
        ret = strlen(pkexponent);
        ret = strlen(vkmodulus);
        ret = strlen(vkpublicExponent);
        ret = strlen(vkexponent);
        ret = strlen(vkprime1);
        ret = strlen(vkprime2);
        ret = strlen(vkprimeExponent1);
        ret = strlen(vkprimeExponent2);
        ret = strlen(vkcoefficient);

//        UnionUncompressBitsIntoBCD((unsigned char *)&publicKey,sizeof(R_RSA_PUBLIC_KEY),pk);
        memcpy(pk,pkmodulus,strlen(pkmodulus));
        UnionUncompressBitsIntoBCD((unsigned char *)&privateKey,sizeof(R_RSA_PRIVATE_KEY),vk);
/*
        char encData[512];
        unsigned int enclen=0;
        char plainData[512];
        unsigned int plalen=0;
        char tmpBuf[512];
        memset(tmpBuf,0,sizeof tmpBuf);
        memset(encData,0,sizeof encData);
        memset(plainData,0,sizeof plainData);
        strcpy(plainData,"11111111111111111111111111111111");
        ret = RSAPrivateEncrypt(encData, &enclen, plainData, strlen(plainData), &privateKey);
        memset(plainData,0,sizeof plainData);
        ret = UnionUncompressBitsIntoBCD(encData,enclen,tmpBuf);
        memset(encData,0,sizeof encData);
        ret = UnionCompressBCDIntoBits(tmpBuf,strlen(tmpBuf),encData);
        ret = RSAPublicDecrypt(plainData, &plalen, encData, enclen, &publicKey);
*/
        return 0;
}
/*
int UnionEncByVK(char *input,char *output,char *vk)
{
        int ret;
        char tmpBuf[512+1];
        unsigned int tmpBufLen;

        R_RSA_PRIVATE_KEY privateKey;
        ret = UnionCompressBCDIntoBits(vk,strlen(vk),(unsigned char *)&privateKey);
        if(ret < 0)
        {
                return ret;
        }
        ret = RSAPrivateEncrypt(tmpBuf, &tmpBufLen, input, strlen(input), &privateKey);
        if(ret != 0)
        {
                return -1;
        }

        ret = UnionUncompressBitsIntoBCD(tmpBuf,tmpBufLen,output);
        if(ret < 0)
        {
                return ret;
        }
}

int UnionDecByPK(char *input,char *output,char *pk)
{
        int ret;
        unsigned int inputLen = strlen(input);
        char tmpBuf[512+1];
        char pkBuf[512];
        char encBuf[512];
	char derPK[512];
        unsigned int encBufLen;
        unsigned int outputLen;
	unsigned int derPKLen;
        memset(tmpBuf,0,sizeof tmpBuf);
        memset(pkBuf,0,sizeof pkBuf);
        memset(encBuf,0,sizeof encBuf);
	memset(derPK,0,sizeof derPK);


	derPKLen = UnionFormANSIDERRSAPK(pk,strlen(pk),derPK,sizeof derPK);
	if(derPKLen < 0)
       	{
       		UnionUserErrLog("UnionFormANSIDERRSAPK Err ret = [%d]",ret);
       		return derPKLen;
       	}
	bcdhex_to_aschex(derPK,derPKLen,tmpBuf);
       	aschex_to_bcdhex(input,strlen(input),encBuf);
        ret =  UnionPKDecByPKCS1(encBuf,128,tmpBuf,output);
        if(ret < 0)
       	{
		UnionUserErrLog("UnionPKDecByPKCS1 Err ret = [%d]\n",ret);
       		return ret;        		
       	}
	return(ret);
}

int UnionDecUserNameAndPasswd(char *input,char *userName,char *passwd,char *pk)
{
        int ret;
        char tmpBuf[2048];
	memset(tmpBuf,0,sizeof tmpBuf);
        if((userName == NULL)||(passwd == NULL))
        {
                return -1;
        }
        ret = UnionDecByPK(input,tmpBuf,pk);
        if(ret < 0)
        {
                return ret;
        }
        userName = strtok(tmpBuf, "|");
        passwd = strtok(NULL, "|");

}
*/