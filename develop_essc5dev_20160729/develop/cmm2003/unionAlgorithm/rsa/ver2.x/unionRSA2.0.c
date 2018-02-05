#include <stdio.h>
#include <string.h>
#include "unionRSA.h"
#include "UnionStr.h"

#include "UnionLog.h"
#include "unionErrCode.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>


// 产生RSA密钥对
/*
输入参数：
	bits： 密钥强度，512/1024/1152/1408/1984/2048等
	pkExponent: 公钥指数 65537,3
	sizeofPK：公钥的缓存大小
	sizeofVK: 私钥的缓存大小
输出参数：
	pk:	公钥(可见字符串，扩张的DER格式BCD码)
	vk：    私钥(可见字符串，扩张的DER格式BCD码)
返回：
	>=0 成功
	<0 失败
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

	// 取公钥
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
    	//取私钥信息
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
	EVP_PKEY_free(pkey);  //由于EVP_PKEY_assign_RSA，调用这个已经释放了RSA, 故不能做RSA_free了.

	return 1;
}


// 产生RSA密钥对,指数为65537
/*
输入参数：
	bits： 密钥强度，512/1024/2048等
	sizeofPK：公钥的缓存大小
	sizeofVK: 私钥的缓存大小
输出参数：
	pk:	公钥(可见字符串，扩张的DER格式BCD码)
	vk：    私钥(可见字符串，扩张的DER格式BCD码)
返回：
	>=0 成功
	<0 失败
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

	// 取公钥
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
    	//取私钥信息
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
	EVP_PKEY_free(pkey);  //由于EVP_PKEY_assign_RSA，调用这个已经释放了RSA, 故不能做RSA_free了.

	return 1;
}


// 按PKCS1的填充方式，进行私钥加密
/*
输入参数：
	plaintxt： 二进制明文
	plaintxtLen：明文长度
	vk：       私钥(可见字符串，扩张的DER格式BCD码)
输出参数：
	encTxt：加密的密文数据
返回：
	加密的密文数据长度
*/
int UnionVKEncByPKCS1(unsigned char *plaintxt,int plaintxtLen,char *vk,unsigned char *encTxt)
{
	return UnionVKEnc(plaintxt,plaintxtLen,vk,1,encTxt);
}

// 用私钥加密，无填充方式
/*
输入参数：
	plaintxt： 二进制明文
	plaintxtLen：明文长度
	vk：       私钥(可见字符串，扩张的DER格式BCD码)
输出参数：
	encTxt：加密的密文数据
返回：
	加密的密文数据长度
*/
int UnionVKEncNoPADDING(unsigned char *plaintxt,int plaintxtLen,char *vk,unsigned char *encTxt)
{
	return UnionVKEnc(plaintxt,plaintxtLen,vk,0,encTxt);
}


// 用私钥加密
/*
输入参数：
	plaintxt： 二进制明文
	plaintxtLen：明文长度
	vk：       私钥(可见字符串，扩张的DER格式BCD码)
	fillFlag:  填充方式，0无填充，1按pkcs1格式填充
输出参数：
	encTxt：加密的密文数据
返回：
	加密的密文数据长度

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

// 按PKCS1的填充方式，进行公钥解密
/*
输入参数：
	crypk： 二进制密文数据
	crypklen：密文数据长度
	pk：       公钥(可见字符串，扩张的DER格式BCD码)
输出参数：
	plaintxt：明文数据
返回：
	解密后的明文数据长度
*/
int UnionPKDecByPKCS1(unsigned char *crypk,int crypklen,char *pk,unsigned char *plaintxt)
{
	return UnionPKDec(crypk,crypklen,pk,1,plaintxt);
}


// 进行公钥解密,无填充方式
/*
输入参数：
	crypk： 二进制密文数据
	crypklen：密文数据长度
	pk：       公钥(可见字符串，扩张的DER格式BCD码)
输出参数：
	plaintxt：明文数据
返回：
	解密后的明文数据长度
*/
int UnionPKDecNoPADDING(unsigned char *crypk,int crypklen,char *pk,unsigned char *plaintxt)
{
	return UnionPKDec(crypk,crypklen,pk,0,plaintxt);
}

// 进行公钥解密
/*
输入参数：
	crypk： 二进制密文数据
	crypklen：密文数据长度
	pk：       公钥(可见字符串，扩张的DER格式BCD码)
	fillFlag:  填充方式，0无填充，1按pkcs1格式填充
输出参数：
	plaintxt：明文数据
返回：
	解密后的明文数据长度
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



// 进行公钥加密
/*
输入参数：
	plaintxt： 明文二进制数据
	plaintxtlen：明文数据长度
	pk：       公钥(可见字符串，扩张的DER格式BCD码)
	fillFlag:  填充方式，0无填充，1按pkcs1格式填充
输出参数：
	crypktxt：密文数据
返回：
	加密后的密文数据长度
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

// 按PKCS1的填充方式，进行公钥加密
/*
输入参数：
	plaintxt： 明文二进制数据
	plaintxtlen：明文数据长度
	pk：       公钥(可见字符串，扩张的DER格式BCD码)
输出参数：
	crypktxt：密文数据
返回：
	加密后的密文数据长度
*/
int UnionPKEncByPKCS1(unsigned char *plaintxt,int plaintxtlen,char *pk,unsigned char *crypktxt)
{
	return UnionPKEnc(plaintxt,plaintxtlen,pk,1,crypktxt);
}

// 进行公钥加密,无填充方式
/*
输入参数：
	plaintxt： 明文二进制数据
	plaintxtlen：明文数据长度
	pk：       公钥(可见字符串，扩张的DER格式BCD码)
输出参数：
	crypktxt：密文数据
返回：
	加密后的密文数据长度
*/
int UnionPKEncNoPADDING(unsigned char *plaintxt,int plaintxtlen,char *pk,unsigned char *crypktxt)
{
	return UnionPKEnc(plaintxt,plaintxtlen,pk,0,crypktxt);
}


// 按PKCS1的填充方式，进行私钥解密
/*
输入参数：
	encTxt： 二进制密文
	encTxtLen：  密文长度
	vk：       私钥(可见字符串，扩张的DER格式BCD码)
输出参数：
	plaintxt：解密后的明文数据
返回：
	解密的明文数据长度
*/
int UnionVKDecByPKCS1(unsigned char *encTxt,int encTxtLen,char *vk,unsigned char *plaintxt)
{
	return UnionVKDec(encTxt,encTxtLen,vk,1,plaintxt);
}

// 用私钥解密，无填充方式
/*
输入参数：
	encTxt： 二进制密文
	encTxtLen：  密文长度
	vk：       私钥(可见字符串，扩张的DER格式BCD码)
输出参数：
	plaintxt：解密后的明文数据
返回：
	解密的明文数据长度
*/
int UnionVKDecNoPADDING(unsigned char *encTxt,int encTxtLen,char *vk,unsigned char *plaintxt)
{
	return UnionVKDec(encTxt,encTxtLen,vk,0,plaintxt);
}

// 用私钥解密
/*
输入参数：
	encTxt： 二进制密文
	encTxtLen：  密文长度
	vk：       私钥(可见字符串，扩张的DER格式BCD码)
	fillFlag:  填充方式，0无填充，1按pkcs1格式填充
输出参数：
	plaintxt：解密后的明文数据
返回：
	解密的明文数据长度
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

//公钥 PKCS#1 ASN.1 DER 解码；简单返回其中的标签内容（16进制的ASCII字符串），不根据类型返回；
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
			st1 = pdest - buf + 1;					//找到的第一个位置+1！
		} else {
			return -1;
		}
		pnext= buf + st1;

		ch=')';
		pdest = strchr( pnext, ch );
		if( pdest != NULL )
		{
			st2 = pdest - pnext + 1;					//找到的第2个位置+1！
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
			st1 = pdest - pnext + 1;					//找到的第一个位置+1！
		} else {
			return -1;
		}
		pnext= pnext + st1;

		ch=')';
		pdest = strchr( pnext, ch );
		if( pdest != NULL )
		{
			st2 = pdest - pnext + 1;					//找到的第2个位置+1！
		} else {
			return -1;
		}
		memcpy(Evalue,pnext,st2-1);
		Evalue[st2-1]=0x00;

		// add in 2009.3.2 去调公钥前面的'00'
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

//ASN.1 DER解码，将各种定长型结构、标签分割开来——〉outbuf！
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
		//开始ASN.1 DER解码；
		while(i<(len-1))
		{
			flag=ASC2TOINT(encode[i+1],encode[i]);			//取标签类型字节
			type=flag;
			i+=2;
			if((flag & 0x20)>0)
			{	//结构化定长方法
				flag=ASC2TOINT(encode[i+1],encode[i]);	//取标签长度，判断bit8=0/1？0：短字节长度；1：长字节长度；
				i+=2;
				if((flag & 0x80) > 0)
				{	//长字节长度
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
				{	//短字节长度
					lenA=flag & 0x7F;
					lenB=lenA;
					//i+=2;
				}
				//i+=2;
				//读取lenB长度的字节到字符串中；
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
			{	//简单定长方法；
				flag=ASC2TOINT(encode[i+1],encode[i]);
				//type=flag;
				i+=2;
				//LL=flag & 0x80;
				if((flag & 0x80)>0)
				{	//长字节长度
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
				{	//短字节长度
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
					//读取lenB长度的字节到字符串中；
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
功能：将长度转为asn1格式的长度
输入：
	len:长度
输出：
	asn1Len:在asn1中表示的长度
返回:
	asn1Len二进制字符串的输出长度
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
功能:将公钥模与指数拼装为DER编码格式的公钥
输入:
	lenOfPkModule:公钥模的二进制字符长度
	pkModule:公钥模的二进制字符
	lenOfExp:公钥指数的二进制字符长度
	exp:公钥指数的二进制字符
输出:
	derPK:DER编码格式的公钥二进制字符串
返回:
	DER编码格式的公钥二进制字符串长度
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

// 将裸公钥，编码成DER格式
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
//公钥 PKCS#1 ASN.1 DER 解码；简单返回其中的标签内容（16进制的ASCII字符串），不根据类型返回；
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
			st1 = pdest - buf + 1;					//找到的第一个位置+1！
		} else {
			return -1;
		}
		pnext= buf + st1;

		ch=')';
		pdest = strchr( pnext, ch );
		if( pdest != NULL )
		{
			st2 = pdest - pnext + 1;					//找到的第2个位置+1！
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
			st1 = pdest - pnext + 1;					//找到的第一个位置+1！
		} else {
			return -1;
		}
		pnext= pnext + st1;

		ch=')';
		pdest = strchr( pnext, ch );
		if( pdest != NULL )
		{
			st2 = pdest - pnext + 1;					//找到的第2个位置+1！
		} else {
			return -1;
		}
		memcpy(Evalue,pnext,st2-1);
		Evalue[st2-1]=0x00;

		// add in 2009.3.2 去调公钥前面的'00'
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

