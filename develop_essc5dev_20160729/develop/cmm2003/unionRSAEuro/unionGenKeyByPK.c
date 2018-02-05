// create by hzh in 2009.6.16
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "rsaeuro.h"
#include "rsa.h"
#include "unionGenKeyByPK.h"

char *UnionGenDESKey(char *outKeybuf);
char *UnionGen2DESKey(char *outKeybuf);
char *UnionGen3DESKey(char *outKeybuf);
char *UnionRandKeyData(int keylen,char *outKeybuf);
int UnionASC2TOINT(char ALL,char AHH);
int UnionOddCheck(int KK);

#define PUBE "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010001"
#define PBKPRE "0002FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00301C0410"
#define PBKBAC "04089999999999999999"

//����������des��Կ
char *UnionGenDESKey(char *outKeybuf)
{
	return UnionRandKeyData(16,outKeybuf);
}

//����˫����des��Կ
char *UnionGen2DESKey(char *outKeybuf)
{
	return UnionRandKeyData(2*16,outKeybuf);
}

//����3����des��Կ
char *UnionGen3DESKey(char *outKeybuf)
{
	return UnionRandKeyData(3*16,outKeybuf);
}

// ����ָ�����ȵ���Կ����,���Ϊ48(3����)����
char *UnionRandKeyData(int keylen,char *outKeybuf)
{
	int rnd = 0;
	char buf[50],tbuf[50];
	int i = 0;
	int n,m;
	
	memset(buf,0,sizeof(buf));
	memset(tbuf,0,sizeof(tbuf));
	srand( (unsigned)time( NULL ) ); 
	for(i=0;i<12;i++) 
	{ 
		rnd = rand(); 
		sprintf(buf,"%04X",rnd);
		memcpy(tbuf+i*4,buf,4);
	} 
	memcpy(buf,tbuf,48);
	
	for(i=0;i<49;i++)
	{
		n=UnionASC2TOINT(buf[i+1],buf[i]);
		m=UnionOddCheck(n);
		sprintf(tbuf,"%02X",m);
		tbuf[2]='\0';
		memcpy(buf+i*1,tbuf,2);
		i++;
	}
	
	tbuf[48] = '\0';  
	if (keylen < 49)
	 memcpy(outKeybuf,buf,keylen);
	else 
	 memcpy(outKeybuf,buf,49);
	 
	return outKeybuf;
}

// ��2�ֽ�ѹ��Ϊһ����
int UnionASC2TOINT(char ALL,char AHH)
{
	int ll,hh;
	if(ALL<'A')
		ll=ALL-'0';
	else
		ll=ALL-'A'+10;
	if(AHH<'A')
		hh=AHH-'0';
	else
		hh=AHH-'A'+10;
	hh=hh<<4;
	return ll|hh;
}

// ��KK��Ϊ������У�����
int UnionOddCheck(int KK)
{
	int i=0,j=0,rr;

	rr=KK;
	if(KK<2)
	{
		rr=1;
	} 
	else {
		j=0;
		for(i=0;i<7;i++)
		{
			j+=KK & 1;
			KK>>=1;
		}
		j=j % 2;

		if(j>0)
		{
			rr=rr & 0x7F;
		} else {
			rr=rr | 0x80;
		}
	}
	return rr;
}


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

/*�������2����des,����ǿ��Ϊ1024��PK�������*/
/*
���룺1024bits��PK�㹫Կ,��չ�Ŀɼ��ַ���
�����DesKey, �漴������des��Կ����
���: DesKeyByPK,����PK���ܵ�des��Կ,��չ�Ŀɼ��ַ���
���أ�
    >= 0  �ɹ�
    <0   ʧ��
*/
int UnionGen2DesKeyBy1024PK(char *PK,char *pDesKey,char *DesKeyByPK)
{
	char deskey[33];
	int ret = 0;
	if (PK == NULL || DesKeyByPK == NULL || pDesKey == NULL)
		return -1;
	if (strlen(PK) != 256)
		return -2;	
	
	memset(deskey,0,sizeof(deskey));
	UnionGen2DESKey(deskey);
	strcpy(pDesKey,deskey);
	
	return UnionEncDesKeyBy1024PK(PK,pDesKey,DesKeyByPK);
}

//��PK����DES��Կ
/*
���룺1024bits��PK�㹫Կ,��չ�Ŀɼ��ַ���
���룺DesKey, 2des��Կ����
���: DesKeyByPK,����PK���ܵ�des��Կ,��չ�Ŀɼ��ַ���
���أ�
    >= 0  �ɹ�
    <0   ʧ��
*/
int UnionEncDesKeyBy1024PK(char *PK,char *pDesKey,char *DesKeyByPK)
{
	R_RSA_PUBLIC_KEY pkey;
	unsigned char pkbuf[2048];
	unsigned char data[1024];
	unsigned char output[1024];
	unsigned int outputLen = 0;
	unsigned char input[1024];
	unsigned int inputLen = 0;
	char deskey[33];
	int ret = 0;
	if (PK == NULL || DesKeyByPK == NULL || pDesKey == NULL)
		return -1;
	if (strlen(PK) != 256 || strlen(pDesKey) != 32)
		return -2;	
	memset(&pkey,0,sizeof(pkey));
	pkey.bits = 1024;
	UnionAscBcd(PK,strlen(PK),pkey.modulus);
	UnionAscBcd(PUBE,strlen(PUBE),pkey.exponent);
	memset(deskey,0,sizeof(deskey));
	strcpy(deskey,pDesKey);
	strcpy(data,PBKPRE);
	strcat(data,deskey);
	strcat(data,PBKBAC);
	
	memset(input,0,sizeof(input));
	UnionAscBcd(data,256,input);
	inputLen = 128;
	memset(output,0,sizeof(output));
	if (RSAPublicEnc(output, &outputLen, input, inputLen, &pkey) != 0)
		return -3;
	memset(data,0,sizeof(data));
	UnionBcdAsc(output,outputLen,data);
	strcpy(DesKeyByPK,data);
	return strlen(DesKeyByPK);
}
