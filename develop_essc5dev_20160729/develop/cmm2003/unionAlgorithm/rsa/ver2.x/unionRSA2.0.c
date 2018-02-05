#include <stdio.h>
#include <string.h>
#include "unionRSA.h"
#include "UnionStr.h"

#include "UnionLog.h"
#include "unionErrCode.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>


// ����RSA��Կ��
/*
���������
	bits�� ��Կǿ�ȣ�512/1024/1152/1408/1984/2048��
	pkExponent: ��Կָ�� 65537,3
	sizeofPK����Կ�Ļ����С
	sizeofVK: ˽Կ�Ļ����С
���������
	pk:	��Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
	vk��    ˽Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���أ�
	>=0 �ɹ�
	<0 ʧ��
*/
int UnionGenRSAByBitsAndPKExponent(int bits,int pkExponent,char *pk,int sizeofPK,char *vk,int sizeofVK)
{
	RSA           *rsa;
	EVP_PKEY      *pkey;
	unsigned long  e;
	int len = 0;
	    unsigned char *buf_out=NULL;
	char		buffer[8192];

/*	if(bits != 512 && bits != 1024 && bits != 1152 && bits != 1408 && bits != 1984 && bits != 2048)
	{
		UnionUserErrLog("UnionGenRSAByBitsAndPKExponent bits[%d] err!\n",bits);
		return -1;
	}
*/
	pkey=EVP_PKEY_new();

	if (pkExponent == 65537)
		e = RSA_F4;
	else if (pkExponent == 3)
		e = RSA_3;
	else
	{
		UnionUserErrLog("UnionGenRSAByBitsAndPKExponent pkExponent[%d] != 65537 or != 3 err!\n",pkExponent);
		return -1;
	}
	
	if ((rsa=RSA_generate_key(bits,e,NULL,NULL)) == NULL)
	{
		UnionUserErrLog("UnionGenRSAByBitsAndPKExponent RSA_generate_key[%d] failed!\n",bits);
		return -1;
	}

	EVP_PKEY_assign_RSA(pkey,rsa);

	// ȡ��Կ
	len=i2d_PublicKey(pkey,&buf_out);
	if (len <= 0)
	{
		UnionUserErrLog("UnionGenRSAByBitsAndPKExponent i2d_PublicKey failed[%d] err!\n",len);
		return -1;
	}
	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)buf_out,len,buffer);
	free(buf_out);
	
	if (sizeofPK < 2*len + 1)
	{
		UnionUserErrLog("UnionGenRSAByBitsAndPKExponent sizeofPK [%d] too small than[%d]!\n",sizeofPK,2*len+1);
		return -1;
	}
	strcpy(pk,buffer);
    	//ȡ˽Կ��Ϣ
	buf_out = NULL;

	len=i2d_PrivateKey(pkey,&buf_out);
	if (len <= 0)
	{
		UnionUserErrLog("UnionGenRSAByBitsAndPKExponent i2d_PrivateKey failed[%d] err!\n",len);
		return -1;
	}
	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)buf_out,len,buffer);
	free(buf_out);
	
	if (sizeofVK < 2*len + 1)
	{
		UnionUserErrLog("UnionGenRSAByBitsAndPKExponent sizeofVK [%d] too small than[%d]!\n",sizeofVK,2*len+1);
		return -1;
	}
	strcpy(vk,buffer);
	EVP_PKEY_free(pkey);  //����EVP_PKEY_assign_RSA����������Ѿ��ͷ���RSA, �ʲ�����RSA_free��.

	return 1;
}


// ����RSA��Կ��,ָ��Ϊ65537
/*
���������
	bits�� ��Կǿ�ȣ�512/1024/2048��
	sizeofPK����Կ�Ļ����С
	sizeofVK: ˽Կ�Ļ����С
���������
	pk:	��Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
	vk��    ˽Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���أ�
	>=0 �ɹ�
	<0 ʧ��
*/
int UnionGenRSABySS(int bits,char *pk,int sizeofPK,char *vk,int sizeofVK)
{
	RSA           *rsa;
	EVP_PKEY      *pkey;
	unsigned long  e = RSA_F4;
	int len = 0;
	    unsigned char *buf_out=NULL;
	char		buffer[8192];

	//if(bits != 512 && bits != 1024 && bits != 2048)
	if ((bits % 8) != 0)
	{
		UnionUserErrLog("UnionGenRSABySS bits[%d] err!\n",bits);
		return -1;
	}
	pkey=EVP_PKEY_new();

	if ((rsa=RSA_generate_key(bits,e,NULL,NULL)) == NULL)
	{
		UnionUserErrLog("UnionGenRSABySS RSA_generate_key[%d] failed!\n",bits);
		return -1;
	}

	EVP_PKEY_assign_RSA(pkey,rsa);

	// ȡ��Կ
	len=i2d_PublicKey(pkey,&buf_out);
	if (len <= 0)
	{
		UnionUserErrLog("UnionGenRSABySS i2d_PublicKey failed[%d] err!\n",len);
		return -1;
	}
	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)buf_out,len,buffer);
	free(buf_out);
	
	if (sizeofPK < 2*len + 1)
	{
		UnionUserErrLog("UnionGenRSABySS sizeofPK [%d] too small than[%d]!\n",sizeofPK,2*len+1);
		return -1;
	}
	strcpy(pk,buffer);
    	//ȡ˽Կ��Ϣ
    buf_out = NULL;

    len=i2d_PrivateKey(pkey,&buf_out);
    if (len <= 0)
	{
		UnionUserErrLog("UnionGenRSABySS i2d_PrivateKey failed[%d] err!\n",len);
		return -1;
	}
	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)buf_out,len,buffer);
	free(buf_out);
	
	if (sizeofVK < 2*len + 1)
	{
		UnionUserErrLog("UnionGenRSABySS sizeofVK [%d] too small than[%d]!\n",sizeofVK,2*len+1);
		return -1;
	}
	strcpy(vk,buffer);
	EVP_PKEY_free(pkey);  //����EVP_PKEY_assign_RSA����������Ѿ��ͷ���RSA, �ʲ�����RSA_free��.

	return 1;
}


// ��PKCS1����䷽ʽ������˽Կ����
/*
���������
	plaintxt�� ����������
	plaintxtLen�����ĳ���
	vk��       ˽Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���������
	encTxt�����ܵ���������
���أ�
	���ܵ��������ݳ���
*/
int UnionVKEncByPKCS1(unsigned char *plaintxt,int plaintxtLen,char *vk,unsigned char *encTxt)
{
	return UnionVKEnc(plaintxt,plaintxtLen,vk,1,encTxt);
}

// ��˽Կ���ܣ�����䷽ʽ
/*
���������
	plaintxt�� ����������
	plaintxtLen�����ĳ���
	vk��       ˽Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���������
	encTxt�����ܵ���������
���أ�
	���ܵ��������ݳ���
*/
int UnionVKEncNoPADDING(unsigned char *plaintxt,int plaintxtLen,char *vk,unsigned char *encTxt)
{
	return UnionVKEnc(plaintxt,plaintxtLen,vk,0,encTxt);
}


// ��˽Կ����
/*
���������
	plaintxt�� ����������
	plaintxtLen�����ĳ���
	vk��       ˽Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
	fillFlag:  ��䷽ʽ��0����䣬1��pkcs1��ʽ���
���������
	encTxt�����ܵ���������
���أ�
	���ܵ��������ݳ���

*/
int UnionVKEnc(unsigned char *plaintxt,int plaintxtLen,char *vk,int fillFlag,unsigned char *encTxt)
{
	EVP_PKEY        *pkey;
	long 		inl=0;
	char		buf[8192+1];
	const unsigned char *const_buf;
	RSA                  *rsa;
	int		len = 0;

	pkey=EVP_PKEY_new();
	inl = strlen(vk)/2;
	memset(buf,0,sizeof(buf));
	aschex_to_bcdhex(vk,strlen(vk),buf);
	const_buf = (const unsigned char *)buf;
	d2i_PrivateKey(EVP_PKEY_RSA,&pkey,&const_buf,inl);
	rsa = EVP_PKEY_get1_RSA(pkey);
	EVP_PKEY_free(pkey);
	RSA_size(rsa);
	if (fillFlag == 0)
		len = RSA_private_encrypt(plaintxtLen,plaintxt,encTxt,rsa,RSA_NO_PADDING);
	else
		len = RSA_private_encrypt(plaintxtLen,plaintxt,encTxt,rsa,RSA_PKCS1_PADDING);
	RSA_free(rsa);
	return len;
}

// ��PKCS1����䷽ʽ�����й�Կ����
/*
���������
	crypk�� ��������������
	crypklen���������ݳ���
	pk��       ��Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���������
	plaintxt����������
���أ�
	���ܺ���������ݳ���
*/
int UnionPKDecByPKCS1(unsigned char *crypk,int crypklen,char *pk,unsigned char *plaintxt)
{
	return UnionPKDec(crypk,crypklen,pk,1,plaintxt);
}


// ���й�Կ����,����䷽ʽ
/*
���������
	crypk�� ��������������
	crypklen���������ݳ���
	pk��       ��Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���������
	plaintxt����������
���أ�
	���ܺ���������ݳ���
*/
int UnionPKDecNoPADDING(unsigned char *crypk,int crypklen,char *pk,unsigned char *plaintxt)
{
	return UnionPKDec(crypk,crypklen,pk,0,plaintxt);
}

// ���й�Կ����
/*
���������
	crypk�� ��������������
	crypklen���������ݳ���
	pk��       ��Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
	fillFlag:  ��䷽ʽ��0����䣬1��pkcs1��ʽ���
���������
	plaintxt����������
���أ�
	���ܺ���������ݳ���
*/
int UnionPKDec(unsigned char *crypk,int crypklen,char *pk,int fillFlag,unsigned char *plaintxt)
{
	EVP_PKEY        *pkey;
	long 		inl=0;
	char		buf[8192+1];
	const unsigned char *const_buf;
	RSA                  *rsa;
	int		len = 0;

	pkey=EVP_PKEY_new();
	inl = strlen(pk)/2;
	memset(buf,0,sizeof(buf));
	aschex_to_bcdhex(pk,strlen(pk),buf);
	const_buf = (const unsigned char *)buf;
	d2i_PublicKey(EVP_PKEY_RSA,&pkey,&const_buf,inl);
	rsa = EVP_PKEY_get1_RSA(pkey);
	EVP_PKEY_free(pkey);
	RSA_size(rsa);
	if (fillFlag == 0)
		len = RSA_public_decrypt(crypklen,crypk,plaintxt,rsa,RSA_NO_PADDING);
	else
		len = RSA_public_decrypt(crypklen,crypk,plaintxt,rsa,RSA_PKCS1_PADDING);
	RSA_free(rsa);
	return len;
}



// ���й�Կ����
/*
���������
	plaintxt�� ���Ķ���������
	plaintxtlen���������ݳ���
	pk��       ��Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
	fillFlag:  ��䷽ʽ��0����䣬1��pkcs1��ʽ���
���������
	crypktxt����������
���أ�
	���ܺ���������ݳ���
*/
int UnionPKEnc(unsigned char *plaintxt,int plaintxtlen,char *pk,int fillFlag,unsigned char *crypktxt)
{
	EVP_PKEY        *pkey;
	long 		inl=0;
	char		buf[8192+1];
	const unsigned char *const_buf;
	RSA                  *rsa;
	int		len = 0;

	pkey=EVP_PKEY_new();
	inl = strlen(pk)/2;
	memset(buf,0,sizeof(buf));
	aschex_to_bcdhex(pk,strlen(pk),buf);

	const_buf = (const unsigned char *)buf;
	d2i_PublicKey(EVP_PKEY_RSA,&pkey,&const_buf,inl);
	rsa = EVP_PKEY_get1_RSA(pkey);
	EVP_PKEY_free(pkey);
	RSA_size(rsa);
	if (fillFlag == 0)
		len = RSA_public_encrypt(plaintxtlen,plaintxt,crypktxt,rsa,RSA_NO_PADDING);
	else
		len = RSA_public_encrypt(plaintxtlen,plaintxt,crypktxt,rsa,RSA_PKCS1_PADDING);
	RSA_free(rsa);
	return len;
}

// ��PKCS1����䷽ʽ�����й�Կ����
/*
���������
	plaintxt�� ���Ķ���������
	plaintxtlen���������ݳ���
	pk��       ��Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���������
	crypktxt����������
���أ�
	���ܺ���������ݳ���
*/
int UnionPKEncByPKCS1(unsigned char *plaintxt,int plaintxtlen,char *pk,unsigned char *crypktxt)
{
	return UnionPKEnc(plaintxt,plaintxtlen,pk,1,crypktxt);
}

// ���й�Կ����,����䷽ʽ
/*
���������
	plaintxt�� ���Ķ���������
	plaintxtlen���������ݳ���
	pk��       ��Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���������
	crypktxt����������
���أ�
	���ܺ���������ݳ���
*/
int UnionPKEncNoPADDING(unsigned char *plaintxt,int plaintxtlen,char *pk,unsigned char *crypktxt)
{
	return UnionPKEnc(plaintxt,plaintxtlen,pk,0,crypktxt);
}


// ��PKCS1����䷽ʽ������˽Կ����
/*
���������
	encTxt�� ����������
	encTxtLen��  ���ĳ���
	vk��       ˽Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���������
	plaintxt�����ܺ����������
���أ�
	���ܵ��������ݳ���
*/
int UnionVKDecByPKCS1(unsigned char *encTxt,int encTxtLen,char *vk,unsigned char *plaintxt)
{
	return UnionVKDec(encTxt,encTxtLen,vk,1,plaintxt);
}

// ��˽Կ���ܣ�����䷽ʽ
/*
���������
	encTxt�� ����������
	encTxtLen��  ���ĳ���
	vk��       ˽Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
���������
	plaintxt�����ܺ����������
���أ�
	���ܵ��������ݳ���
*/
int UnionVKDecNoPADDING(unsigned char *encTxt,int encTxtLen,char *vk,unsigned char *plaintxt)
{
	return UnionVKDec(encTxt,encTxtLen,vk,0,plaintxt);
}

// ��˽Կ����
/*
���������
	encTxt�� ����������
	encTxtLen��  ���ĳ���
	vk��       ˽Կ(�ɼ��ַ��������ŵ�DER��ʽBCD��)
	fillFlag:  ��䷽ʽ��0����䣬1��pkcs1��ʽ���
���������
	plaintxt�����ܺ����������
���أ�
	���ܵ��������ݳ���
*/
int UnionVKDec(unsigned char *encTxt,int encTxtLen,char *vk,int fillFlag,unsigned char *plaintxt)
{
	EVP_PKEY        *pkey;
	long 		inl=0;
	char		buf[8192+1];
	const unsigned char *const_buf;
	RSA                  *rsa;
	int		len = 0;

	pkey=EVP_PKEY_new();
	inl = strlen(vk)/2;
	memset(buf,0,sizeof(buf));
	aschex_to_bcdhex(vk,strlen(vk),buf);

	const_buf = (const unsigned char *)buf;
	d2i_PrivateKey(EVP_PKEY_RSA,&pkey,&const_buf,inl);
	rsa = EVP_PKEY_get1_RSA(pkey);
	EVP_PKEY_free(pkey);
	RSA_size(rsa);
	if (fillFlag == 0)
		len = RSA_private_decrypt(encTxtLen,encTxt,plaintxt,rsa,RSA_NO_PADDING);
	else
		len = RSA_private_decrypt(encTxtLen,encTxt,plaintxt,rsa,RSA_PKCS1_PADDING);
	RSA_free(rsa);
	return len;
}

//��Կ PKCS#1 ASN.1 DER ���룻�򵥷������еı�ǩ���ݣ�16���Ƶ�ASCII�ַ����������������ͷ��أ�
int UnionPKCSCerASN1DER_Decode(char *encode,char *outpubkey,char *Evalue)
{
	char buf[2048];
	char pubkey[2048];
	char *pdest,*pnext;
	int i,st1,st2;
	int ch;

	//strcpy(buf,encode);
	i=ASN1DER_Decode(encode,buf);
	if(i<0)
	{
		return -1;
	} else
	{
		ch='(';
		pdest = strchr( buf, ch );
		if( pdest != NULL )
		{
			st1 = pdest - buf + 1;					//�ҵ��ĵ�һ��λ��+1��
		} else {
			return -1;
		}
		pnext= buf + st1;

		ch=')';
		pdest = strchr( pnext, ch );
		if( pdest != NULL )
		{
			st2 = pdest - pnext + 1;					//�ҵ��ĵ�2��λ��+1��
		} else {
			return -1;
		}
		memcpy(pubkey,pnext,st2-1);
		pubkey[st2-1]=0x00;

		pnext= pnext + st2;

		ch='(';
		pdest = strchr( pnext, ch );
		if( pdest != NULL )
		{
			st1 = pdest - pnext + 1;					//�ҵ��ĵ�һ��λ��+1��
		} else {
			return -1;
		}
		pnext= pnext + st1;

		ch=')';
		pdest = strchr( pnext, ch );
		if( pdest != NULL )
		{
			st2 = pdest - pnext + 1;					//�ҵ��ĵ�2��λ��+1��
		} else {
			return -1;
		}
		memcpy(Evalue,pnext,st2-1);
		Evalue[st2-1]=0x00;

		// add in 2009.3.2 ȥ����Կǰ���'00'
		if (strncmp(pubkey,"00",2) == 0)
		{
			strcpy(outpubkey,pubkey+2);
		}
		else
		{
			strcpy(outpubkey,pubkey);
		}

		return 0;
	}
}
int ASC2TOINT(char ALL,char AHH)
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

//ASN.1 DER���룬�����ֶ����ͽṹ����ǩ�ָ��������outbuf��
int ASN1DER_Decode(char *encode,char *outbuf)
{
	char buf[2048];
	int i,j,l,len,flag,lenA,type;
	long lenB,lflag;
	int RetFalse;

	RetFalse=0;
	len=strlen(encode);
	i=0;
	j=0;

	if((len>1) && (len<2036))
	{
		//��ʼASN.1 DER���룻
		while(i<(len-1))
		{
			flag=ASC2TOINT(encode[i+1],encode[i]);			//ȡ��ǩ�����ֽ�
			type=flag;
			i+=2;
			if((flag & 0x20)>0)
			{	//�ṹ����������
				flag=ASC2TOINT(encode[i+1],encode[i]);	//ȡ��ǩ���ȣ��ж�bit8=0/1��0�����ֽڳ��ȣ�1�����ֽڳ��ȣ�
				i+=2;
				if((flag & 0x80) > 0)
				{	//���ֽڳ���
					lenA=flag & 0x7F;
					if(lenA>4)
					{
						RetFalse=-1;
						break;
					}
					lenB=0;
					for(l=0;l<lenA;l++)
					{
						lflag=ASC2TOINT(encode[i+l*2+1],encode[i+l*2]);
						lenB=lenB+(lflag<<((lenA-l-1)*8));
					}
					i+=lenA*2;
				} else
				{	//���ֽڳ���
					lenA=flag & 0x7F;
					lenB=lenA;
					//i+=2;
				}
				//i+=2;
				//��ȡlenB���ȵ��ֽڵ��ַ����У�
				//memcpy(buf+j,encode+i,lenB*2);
				//j+=lenB*2;
				//i+=lenB*2;
				if(lenB>(len-i))
				{
					RetFalse=-1;
					break;
				}
				buf[j]='{';
				sprintf(buf+j+1,"%02X;%04X",type,(unsigned int)lenB);
				buf[j+8]='}';
				j+=9;
			} else
			{	//�򵥶���������
				flag=ASC2TOINT(encode[i+1],encode[i]);
				//type=flag;
				i+=2;
				//LL=flag & 0x80;
				if((flag & 0x80)>0)
				{	//���ֽڳ���
					lenA=flag & 0x7F;
					if(lenA>4)
					{
						RetFalse=-1;
						break;
					}
					lenB=0;
					for(l=0;l<lenA;l++)
					{
						lflag=ASC2TOINT(encode[i+l*2+1],encode[i+l*2]);
						lenB=lenB+(lflag<<((lenA-l-1)*8));
					}
					i+=lenA*2;
				} else
				{	//���ֽڳ���
					lenA=flag & 0x7F;
					//lenB=ASC2TOINT(encode[i+1],encode[i]);
					lenB=lenA;
				}
				//i+=2;
				//j+=lenB*2;

				buf[j]='[';
				sprintf(buf+j+1,"%02X;%04X",type,(unsigned int)lenB);
				buf[j+8]=']';
				buf[j+9]='(';
				j+=10;

				if(lenB>(len-i))
				{
					RetFalse=-1;
					break;
				} else
				{
					//��ȡlenB���ȵ��ֽڵ��ַ����У�
					memcpy(buf+j,encode+i,lenB*2);
					i+=lenB*2;
					j+=lenB*2;
					buf[j]=')';
					j+=1;
				}
			}
		}
		if(RetFalse==-1)
		{
			return -1;
		} else
		{
			buf[j]=0x00;
			strcpy(outbuf,buf);
			return 0;
		}
	} else {
		return -1;
	}
}

/*
int ASC2TOINT(char ALL,char AHH)
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
*/
/*
���ܣ�������תΪasn1��ʽ�ĳ���
���룺
	len:����
�����
	asn1Len:��asn1�б�ʾ�ĳ���
����:
	asn1Len�������ַ������������
*/
int UnionAsn1Len(int len,unsigned char *asn1Len)
{
	unsigned char buff[10];
	int ret = 0;
	memset(buff,0,sizeof(buff));
	if(len > 65535)
	{
		return -1;
	}
	if (len > 255)
	{
		buff[0] = 0x82;
		buff[1] = ((len & 0xFF00) >> 8);
		buff[2] = (len & 0x00FF);
		ret  =3;
	}
	else {
		if ((len & 0x80) != 0)
		{
			buff[0] = 0x81;
			buff[1] = len;
			ret = 2;
		}
		else {
			buff[0] = len;
			ret = 1;
		}
	}
	memcpy(asn1Len,buff,ret);
	return ret;
}

/*
����:����Կģ��ָ��ƴװΪDER�����ʽ�Ĺ�Կ
����:
	lenOfPkModule:��Կģ�Ķ������ַ�����
	pkModule:��Կģ�Ķ������ַ�
	lenOfExp:��Կָ���Ķ������ַ�����
	exp:��Կָ���Ķ������ַ�
���:
	derPK:DER�����ʽ�Ĺ�Կ�������ַ���
����:
	DER�����ʽ�Ĺ�Կ�������ַ�������
*/
int UnionFormDerPK(int lenOfPkModule,unsigned char *pkModule,int lenOfExp,unsigned char *exp,unsigned char *derPK)
{
	char buff[4096];
	char tbuff[4096];

	int offset = 0;
	int len = 0;
	int 	ret=0;

	if (pkModule == NULL || exp == NULL || derPK == NULL)
	{
		return -1;
	}
	memset(buff,0,sizeof(buff));
	memset(tbuff,0,sizeof(tbuff));
	offset = 0;
	tbuff[offset] = 0x02;
	offset += 1;
	ret = UnionAsn1Len(lenOfPkModule+1,(unsigned char *)tbuff+offset);
	offset += ret;
	tbuff[offset] = 0x00;
	offset += 1;
	memcpy(tbuff+offset,pkModule,lenOfPkModule);
	offset += lenOfPkModule;
	tbuff[offset] = 0x02;
	offset += 1;
	ret = UnionAsn1Len(lenOfExp,(unsigned char *)tbuff+offset);
	offset += ret;
	memcpy(tbuff+offset,exp,lenOfExp);
	offset += lenOfExp;
	len = offset;

	offset = 0;
	memset(buff,0,sizeof(buff));
	buff[offset] = 0x30; 
	offset += 1;
	ret = UnionAsn1Len(len,(unsigned char *)buff+offset);
	offset += ret;
	memcpy(buff+offset,tbuff,len);
	offset += len;
	
	memcpy(derPK,buff,offset);
	return offset;
}

// ���㹫Կ�������DER��ʽ
int UnionPKCSASN1DER_Encode(char *pkModule,char *exp, char *derPK)
{
	int ret = 0;
	int len = 0;
	unsigned char pkModule_bcdhex[4096];
	unsigned char exp_bcdhex[50];
	unsigned char derPK_bcdhex[4096];
	int lenOfPkModule = 0;
	int lenOfExp = 0;
	
	if (pkModule == NULL || exp == NULL || derPK == NULL)
		return -1;
	memset(pkModule_bcdhex,0,sizeof(pkModule_bcdhex));
	memset(exp_bcdhex,0,sizeof(exp_bcdhex));
	memset(derPK_bcdhex,0,sizeof(derPK_bcdhex));
	aschex_to_bcdhex(pkModule,strlen(pkModule),(char *)pkModule_bcdhex);
	lenOfPkModule = strlen(pkModule)/2;
	aschex_to_bcdhex(exp,strlen(exp),(char *)exp_bcdhex);
	lenOfExp = strlen(exp)/2;
	ret = UnionFormDerPK(lenOfPkModule,pkModule_bcdhex,lenOfExp,exp_bcdhex,derPK_bcdhex);
	bcdhex_to_aschex((char *)derPK_bcdhex,ret,(char *)derPK);
	len = ret*2;
	derPK[len] = 0;
	return len;
}

/*********************jh531 ***************/
//��Կ PKCS#1 ASN.1 DER ���룻�򵥷������еı�ǩ���ݣ�16���Ƶ�ASCII�ַ����������������ͷ��أ�
int UnionPKCSASN1DER_Decode(char *encode,char *outpubkey,char *Evalue)
{
	char buf[2048];
	char pubkey[2048];
	char *pdest,*pnext;
	int i,st1,st2;
	int ch;

	//strcpy(buf,encode);
	i=ASN1DER_Decode(encode,buf);
	if(i<0)
	{
		return -1;
	} else
	{
		ch='(';
		pdest = strchr( buf, ch );
		if( pdest != NULL )
		{
			st1 = pdest - buf + 1;					//�ҵ��ĵ�һ��λ��+1��
		} else {
			return -1;
		}
		pnext= buf + st1;

		ch=')';
		pdest = strchr( pnext, ch );
		if( pdest != NULL )
		{
			st2 = pdest - pnext + 1;					//�ҵ��ĵ�2��λ��+1��
		} else {
			return -1;
		}
		memcpy(pubkey,pnext,st2-1);
		pubkey[st2-1]=0x00;

		pnext= pnext + st2;

		ch='(';
		pdest = strchr( pnext, ch );
		if( pdest != NULL )
		{
			st1 = pdest - pnext + 1;					//�ҵ��ĵ�һ��λ��+1��
		} else {
			return -1;
		}
		pnext= pnext + st1;

		ch=')';
		pdest = strchr( pnext, ch );
		if( pdest != NULL )
		{
			st2 = pdest - pnext + 1;					//�ҵ��ĵ�2��λ��+1��
		} else {
			return -1;
		}
		memcpy(Evalue,pnext,st2-1);
		Evalue[st2-1]=0x00;

		// add in 2009.3.2 ȥ����Կǰ���'00'
		if (strncmp(pubkey,"00",2) == 0)
		{
			strcpy(outpubkey,pubkey+2);
		}
		else
		{
			strcpy(outpubkey,pubkey);
		}

		return 0;
	}
}

