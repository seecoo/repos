#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/pkcs12.h>


#include "UnionLog.h"
#include "unionCertFun.h"
#include "unionRSA.h"
#include "unionCharCode.h"
#include "unionVKForm.h"
#include "unionJKSFun.h"
#include "unionErrCode.h"

#define JKSPriKeyOID "300E060A2B060104012A021101010500"
#define PKCS8PriKeyOID "300D06092A864886F70D0101010500"
#define JKS_MAGIC  0xFEEDFEED
#define JKS_PRIVATE_KEY_FLAG   1
#define JKS_TRUSTED_CERT_FLAG  2
#define JKS_MIGHTY	"Mighty Aphrodite"

#define IS_FILE_EXIST(fileName)	if(access(fileName,0)) {	\
											UnionUserErrLog("fileName[%s]	is not exist\n",fileName);	\
											return errCodeOffsetOfCertMDL_FileNotExist;	\
										}
										
extern int aschex_to_bcdhex(char aschex[],int len,char bcdhex[]);
extern int bcdhex_to_aschex(char bcdhex[],int len,char aschex[]);
extern int UnionAsn1Len(int len,unsigned char *asn1Len);
extern int UnionToLowerCase(char *str);
extern void UnionLoadLoadOpenSSLAlg();

int UnionNum2Buf(int num,int tolen,unsigned char *outbuff);
int UnionJksLocalTime(unsigned char *outbuff);
int UnionJKSWriteCertBuf(char *cert,unsigned char *jskCertBuf,int sizeofJksCertBuf);
int UnionGetAliasFromCertAscHexBuf(char *certAscHex,char *alias);
int UnionPackEncryptedPrivateKeyInfo(unsigned char *encrypted,int len,unsigned char *encryptedWithOid,int *olen);

/*
功能：将私钥加密成JKS私钥密文
输入:
	VK: PKCS8格式私钥
	passwd: 加密口令
	encryptVK: JKS私钥密文
返回：
	>=0 成功
	<0  失败
*/
int UnionEncrytJKSPrivate(char *VKOfPKcs8,char *passwd,char *encryptVK)
{
	unsigned char keystream[100];
	int keystreamLen = 20;
	char tbuf[8196];
	unsigned char buffer[4096];
	unsigned char *passwdUni;
	unsigned char passwdUniBuf[100];
	int passwdUniLen = 0;
	unsigned char encrypted[4096];
	int len = 0;
	int count = 0;
	int i = 0;
	int encryptedLen = 0;
	EVP_MD_CTX ctx;
	const EVP_MD      *md = NULL;
	int rnd = 0;
	md = EVP_sha1();

	//asc2uni(passwd,strlen(passwd),&passwdUni,&passwdUniLen);
	OPENSSL_asc2uni(passwd,strlen(passwd),&passwdUni,&passwdUniLen);	// modify by leipp 20141022
	memcpy(passwdUniBuf,passwdUni,passwdUniLen);
	free(passwdUni);
	passwdUniLen -= 2;
	memset(tbuf,0,sizeof(tbuf));
	bcdhex_to_aschex((char *)passwdUniBuf,passwdUniLen,tbuf);
	
	
	//产生20字节的随机数
	memset(keystream,0,sizeof(keystream));
	keystreamLen = 20;
	srand( (unsigned)time( NULL ) ); 
	for(i=0;i<keystreamLen/4;i++) 
	{ 
		rnd = rand(); 
		sprintf(tbuf,"%04d",rnd);
		memcpy(keystream+i*4,tbuf,4);
	} 
	keystream[keystreamLen] = '\0';  

	memset(encrypted,0,sizeof(encrypted));
	memcpy(encrypted,keystream,keystreamLen);
	memset(buffer,0,sizeof(buffer));
	aschex_to_bcdhex(VKOfPKcs8,strlen(VKOfPKcs8),(char *)buffer);
	len = strlen(VKOfPKcs8)/2;
	
	
	while (count < len)
    {
			EVP_MD_CTX_init(&ctx);
			EVP_DigestInit_ex(&ctx,md, NULL);
			EVP_DigestUpdate(&ctx,passwdUniBuf,passwdUniLen);
			
			EVP_DigestUpdate(&ctx,keystream,keystreamLen);
			EVP_DigestFinal_ex(&ctx,keystream,(unsigned int *)&keystreamLen);
			//EVP_MD_CTX_destroy(&ctx);
			EVP_MD_CTX_cleanup(&ctx);
			memset(tbuf,0,sizeof(tbuf));
			bcdhex_to_aschex((char *)keystream,keystreamLen,tbuf);
            for (i = 0; i < keystreamLen && count < len; i++)
            {
                encrypted[count+20] = (keystream[i] ^ buffer[count]);
                count++;
            }
    }

	EVP_MD_CTX_init(&ctx);
	EVP_DigestInit_ex(&ctx,md, NULL);
    EVP_DigestUpdate(&ctx,passwdUniBuf,passwdUniLen);
	EVP_DigestUpdate(&ctx,buffer,len);
    EVP_DigestFinal_ex(&ctx,keystream,(unsigned int *)&keystreamLen);   
	encryptedLen = len + 40;
    memcpy(encrypted+encryptedLen-keystreamLen, keystream, keystreamLen);
	//EVP_MD_CTX_destroy(&ctx);
	EVP_MD_CTX_cleanup(&ctx);
	memset(buffer,0,sizeof(buffer));
	UnionPackEncryptedPrivateKeyInfo(encrypted,encryptedLen,buffer,&len);
	memset(tbuf,0,sizeof(tbuf));
	bcdhex_to_aschex((char *)buffer,len,tbuf);
	strcpy(encryptVK,tbuf);
	return strlen(encryptVK);
}

/*
功能:
	将pkcs8格式的私钥密文，打包成JKS私钥DER
输入：
	encrypted: pkcs8格式的私钥密文
	len： 私钥密文长度
输出：
	encryptedWithOid: JKS私钥DER的二进制字符串，含JKS私钥OID
	olen: encryptedWithOid的长度
*/
int UnionPackEncryptedPrivateKeyInfo(unsigned char *encrypted,int len,unsigned char *encryptedWithOid,int *olen)
{
	unsigned char Kbuffer[4096];
	int kbuffserlen = 0;
	unsigned char JKSPriKeyOIDBuf[40];
	unsigned char tbuffer[4096];
	int ret = 0;
	int offset = 0;
	int bufLen = 0;

	// JKSPriKeyOID
	memset(JKSPriKeyOIDBuf,0,sizeof(JKSPriKeyOIDBuf));
	aschex_to_bcdhex(JKSPriKeyOID,strlen(JKSPriKeyOID),(char *)JKSPriKeyOIDBuf);
	bufLen = strlen(JKSPriKeyOID)/2;

	// 私钥密文
	Kbuffer[0] = 0x04;
	offset = 1;
	ret = UnionAsn1Len(len,Kbuffer+offset);
	offset += ret;
	memcpy(Kbuffer+offset,encrypted,len);
	offset += len;
	kbuffserlen = offset;
	bufLen += kbuffserlen;

	tbuffer[0] = 0x30;
	offset = 1;
	ret = UnionAsn1Len(bufLen,tbuffer+offset);
	offset += ret;
	memcpy(tbuffer+offset,JKSPriKeyOIDBuf,strlen(JKSPriKeyOID)/2);
	offset += strlen(JKSPriKeyOID)/2;
	memcpy(tbuffer+offset,Kbuffer,kbuffserlen);
	offset += kbuffserlen;
	memcpy(encryptedWithOid,tbuffer,offset);
	*olen = offset;
	return *olen;
}

/*
功能：将DER格式的私钥打包成PKCS8的格式
输入：
	vkAschex: der格式的私钥,扩展的可见字符
输出：
	pkcs8VkAschex：	PKCS8的格式的私钥,扩展的可见字符
返回：
	>=0 成功
	<0  失败
*/
int UnionPackPkcs8PrivateKeyInfo(char *vkAschex,char *pkcs8VkAschex)
{
	unsigned char vk[4096];
	int len = 0;
	unsigned char pkcs8Vk[4096];
	unsigned char Kbuffer[4096];
	unsigned char Pkcs8PriKeyOIDBuf[40];
	unsigned char tbuffer[4096];
	int ret = 0;
	int offset = 0;
	int bufLen = 0;
	int olen = 0;

	aschex_to_bcdhex(vkAschex,strlen(vkAschex),(char *)vk);
	len = strlen(vkAschex)/2;

	// Pkcs8PriKeyOID
	memset(Pkcs8PriKeyOIDBuf,0,sizeof(Pkcs8PriKeyOIDBuf));
	aschex_to_bcdhex(PKCS8PriKeyOID,strlen(PKCS8PriKeyOID),(char *)Pkcs8PriKeyOIDBuf);
	bufLen = strlen(PKCS8PriKeyOID)/2;

	// 版本号
	memcpy(Kbuffer,"\x02\x01\x00",3);
	offset += 3;
	memcpy(Kbuffer+offset,Pkcs8PriKeyOIDBuf,bufLen);
	offset += bufLen;

	Kbuffer[offset] = 0x04;
	offset += 1;
	ret = UnionAsn1Len(len,Kbuffer+offset);
	offset += ret;
	memcpy(Kbuffer+offset,vk,len);
	offset += len;
	bufLen = offset;
	
	tbuffer[0] = 0x30;
	offset = 1;
	ret = UnionAsn1Len(bufLen,tbuffer+offset);
	offset += ret;
	memcpy(tbuffer+offset,Kbuffer,bufLen);
	offset += bufLen;
	
	memcpy(pkcs8Vk,tbuffer,offset);
	olen = offset;

	bcdhex_to_aschex((char *)pkcs8Vk,olen,pkcs8VkAschex);
	pkcs8VkAschex[olen*2] = '\0';
	return olen*2;
}


/*
功能：将一个整数转成指令长度的字符串.如输入0x123,4,输出"\x00\x00\x01\x23"
输入：
	num: 整数
	tolen: 需要转为字符串的长度
输出：
	outbuff: 转换后的字符串
返回：
	>=0 转换后的字符串长度
	<0 失败
*/
int UnionNum2Buf(int num,int tolen,unsigned char *outbuff)
{
	BIGNUM bnum;
	unsigned char buffer[4096];
	int len = 0;
	if (outbuff == NULL) 
	{
		UnionUserErrLog("in UnionNum2Buf::para is err!\n");
		return errCodeOffsetOfCertMDL_CodeParameter;
	}
	BN_init(&bnum);
	BN_add_word(&bnum,num);
	len = BN_bn2bin(&bnum,buffer);
	BN_clear_free(&bnum);
	if (len > tolen)
	{
		UnionUserErrLog("in UnionNum2Buf::tolen[%d] is smaller than [%d]!\n",tolen,len);
		return errCodeOffsetOfCertMDL_SmallBuffer;
	}
	memset(outbuff,0,tolen);
	memcpy(outbuff+tolen-len,buffer,len);
	return tolen;
}

/*
功能：取JKS格式的当前时间
*/
int UnionJksLocalTime(unsigned char *outbuff)
{
	time_t t;
	int len = 0;
	int totallen = 8;
	unsigned char buffer[20];
	BIGNUM bnum;
	time(&t);  
	BN_init(&bnum);
	BN_add_word(&bnum,t);
	BN_mul_word(&bnum,1000);
	len = BN_bn2bin(&bnum,buffer);
	BN_clear_free(&bnum);
	memset(outbuff,0,totallen);
	memcpy(outbuff+totallen-len,buffer,len);
	return totallen;
}

/*
功能：将扩展的DER格式X509证书,组成JKS所需的格式串
*/
int UnionJKSWriteCertBuf(char *cert,unsigned char *jskCertBuf,int sizeofJksCertBuf)
{
	char typeBuf[20];
	int certlen = 0;
	unsigned char certBuf[4096];
	int len = 0;
	unsigned char buffer[8192];
	int offset = 0;

	memset(certBuf,0,sizeof(certBuf));
	aschex_to_bcdhex(cert,strlen(cert),(char *)certBuf);
	certlen = strlen(cert)/2;
	//writeUTF(certType);
	strcpy(typeBuf,"X.509");
	len = strlen(typeBuf);
	buffer[0] = len/256;
	buffer[1] = len%256;
	memcpy(buffer+2,typeBuf,len);
	offset = 2 + len;
	len = UnionNum2Buf(certlen,4,buffer+offset);
	offset += len;
	memcpy(buffer+offset,certBuf,certlen);
	offset += certlen;
	if (sizeofJksCertBuf < offset)
	{
		UnionUserErrLog("in UnionJKSWriteCertBuf::sizeofJksCertBuf[%d] smaller than [%d]\n",sizeofJksCertBuf,offset);
		return errCodeOffsetOfCertMDL_SmallBuffer;
	}
	memcpy(jskCertBuf,buffer,offset);
	return offset;
}

/*
功能：从扩展的DER格式X509证书字符串里获取别名,当别名不存在时,取common name名
*/
int UnionGetAliasFromCertAscHexBuf(char *certAscHex,char *alias)
{
	int ret = 0;
	unsigned char cert[4096];
	int certlen = 0;
	TUnionX509Cer tCertInfo;
	memset(cert,0,sizeof(cert));
	aschex_to_bcdhex(certAscHex,strlen(certAscHex),(char *)cert);
	certlen = strlen(certAscHex)/2;
	memset(&tCertInfo,0,sizeof(tCertInfo));
	if ((ret = UnionGetCertificateInfoFromBuf((char *)cert,certlen,&tCertInfo)) < 0)
	{
		UnionUserErrLog("in UnionGetAliasFromCertAscHexBuf::UnionGetCertificateInfoFromBuf failed ret=[%d]\n",ret);
		return ret;
	}
	
	if (strlen(tCertInfo.userInfo.alias) != 0)
		strcpy(alias,tCertInfo.userInfo.alias);
	else
		strcpy(alias,tCertInfo.userInfo.commonName);
	UnionToLowerCase(alias);
	return strlen(alias);
}

/*
功能：将私钥，证书，信任证书以JKS文件格式保存
输入：
	alias: 存储到JKS(keystore)的别名
	passwd: 私钥保护口令
	certAscHex: 证书,扩展der格式的X509证书
	certChain: 证书链数组, 每组证书的格式为扩展der格式的X509证书
	certChainNum：证书链实际包含的证书数
	jksFileName: 要生成的JKS文件名，含路径
	jskPasswd: jks的密码

返回：
	>=0 成功
	<0  失败
*/
int UnionJksStore(char *alias,char *vk,char *passwd, char *cert, char srcCertChain[MAX_CERTCHAIN_NUM][4096],int certChainNum,char *jksFileName,char *jksPasswd)
{
	FILE *fp = NULL;
	char pkcs8ofVk[4096];
	unsigned char buffer[4096];
	char JKSprivateEncrypt[4096];
	unsigned char jksBuffer[19200];
	int LenOfJKSprivate = 0;
	unsigned char JKSprivateBuf[2048];
	int len = 0;
	unsigned char aliasUtf8[100];
	int i = 0;
	int offset = 0;
	EVP_MD_CTX ctx;
	const EVP_MD      *md = NULL;
	unsigned char jksPasswdUniBuf[100];
	unsigned char *passwdUni = NULL;
	int passwdUniLen = 0;
	unsigned char digestBuf[50];
	int num = 0;
	char aliasBuf[100];
	int aliasCnt = 0;
	TUnionX509Cer cerInfo;
	int ret = 0;
	char certChain[MAX_CERTCHAIN_NUM][4096];
	
	//add by hzh in 2010.6.13 对证书链先排一下序
	if ((ret = UnionSortCert(srcCertChain,certChainNum,1,certChain)) < 0)
	{
		UnionUserErrLog("in UnionJksStore:srcCertChain format err!\n");
		return errCodeOffsetOfCertMDL_FileFormat;
	}

	//vk===>pkcs8ofVk
	UnionPackPkcs8PrivateKeyInfo(vk,pkcs8ofVk);
	
	memset(JKSprivateEncrypt,0,sizeof(JKSprivateEncrypt));
	UnionEncrytJKSPrivate(pkcs8ofVk,passwd,JKSprivateEncrypt);
	memset(JKSprivateBuf,0,sizeof(JKSprivateBuf));
	aschex_to_bcdhex(JKSprivateEncrypt,strlen(JKSprivateEncrypt),(char *)JKSprivateBuf);
	LenOfJKSprivate = strlen(JKSprivateEncrypt)/2;


	// JKS头
	len = UnionNum2Buf(JKS_MAGIC,4,buffer);
	memcpy(jksBuffer+offset,buffer,len);
	offset += len;

	// 版本
	len = UnionNum2Buf(2,4,buffer);
	memcpy(jksBuffer+offset,buffer,len);
	offset += len;

	if (cert != NULL)
		aliasCnt += 1;
	aliasCnt += certChainNum;

	// 别名数量
	len = UnionNum2Buf(aliasCnt,4,buffer);
	memcpy(jksBuffer+offset,buffer,len);
	offset += len;

	if (certChain != NULL)
	{
		for(i=0;i<certChainNum && i<MAX_CERTCHAIN_NUM;i++)  
		{
			//JKStrustedCerts
			len = UnionNum2Buf(JKS_TRUSTED_CERT_FLAG,4,buffer);
			memcpy(jksBuffer+offset,buffer,len);
			offset += len;

			//writeUTF(alias);
			memset(aliasBuf,0,sizeof(aliasBuf));
			if ((ret = UnionGetAliasFromCertAscHexBuf(certChain[i],aliasBuf)) < 0)
				return ret;
			if (ret == 0)
				sprintf(aliasBuf,"r%d",i);
			memset(aliasUtf8,0,sizeof(aliasUtf8));
			len = UnionAnsiToUtf8((unsigned char *)aliasBuf,strlen(aliasBuf),aliasUtf8,sizeof(aliasUtf8));
			memcpy(buffer+2,aliasUtf8,len);
			buffer[0] = len/256;
			buffer[1] = len%256;
			memcpy(jksBuffer+offset,buffer,len+2);
			offset += (len+2);

			//date
			len = UnionJksLocalTime(buffer);
			memcpy(jksBuffer+offset,buffer,len);
			offset += len;
			
			if ((len = UnionJKSWriteCertBuf(certChain[i],buffer,sizeof(buffer))) < 0)
			{
					return len;
			}
			memcpy(jksBuffer+offset,buffer,len);
			offset += len;
		}
	}
	

	// 私钥
	len = UnionNum2Buf(JKS_PRIVATE_KEY_FLAG,4,buffer);
	memcpy(jksBuffer+offset,buffer,len);
	offset += len;

	memset(&cerInfo,0,sizeof(cerInfo));

	//writeUTF(alias);
	if (alias != NULL && strlen(alias) != 0)
		strcpy(aliasBuf,alias);
	else {
		if ((ret = UnionGetAliasFromCertAscHexBuf(cert,aliasBuf)) < 0)
			return ret;
	}
	UnionToLowerCase(aliasBuf);
	memset(aliasUtf8,0,sizeof(aliasUtf8));
	len = UnionAnsiToUtf8((unsigned char *)aliasBuf,strlen(aliasBuf),aliasUtf8,sizeof(aliasUtf8));
	memcpy(buffer+2,aliasUtf8,len);
	buffer[0] = len/256;
	buffer[1] = len%256;
	memcpy(jksBuffer+offset,buffer,len+2);
	offset += (len+2);

	//date
	len = UnionJksLocalTime(buffer);
	memcpy(jksBuffer+offset,buffer,len);
	offset += len;

	//JKSprivate
	len = UnionNum2Buf(LenOfJKSprivate,4,buffer);
	memcpy(jksBuffer+offset,buffer,len);
	offset += len;
	memcpy(jksBuffer+offset,JKSprivateBuf,LenOfJKSprivate);
	offset += LenOfJKSprivate;

	num = certChainNum;
	if (cert != NULL) 
		num = certChainNum + 1;
	len = UnionNum2Buf(num,4,buffer);
	memcpy(jksBuffer+offset,buffer,len);
	offset += len;
	
	
	//先写用户证书
	if (cert != NULL)
	{
		if ((len = UnionJKSWriteCertBuf(cert,buffer,sizeof(buffer))) < 0)
		{
			return len;
		}
		memcpy(jksBuffer+offset,buffer,len);
		offset += len;
	}
	

	if (certChain != NULL)
	{
		for(i=0;i<certChainNum && i<MAX_CERTCHAIN_NUM;i++)
		{
			if ((len = UnionJKSWriteCertBuf(certChain[i],buffer,sizeof(buffer))) < 0)
			{
				return len;
			}
			memcpy(jksBuffer+offset,buffer,len);
			offset += len;
		}
	}

	//asc2uni(jksPasswd,strlen(jksPasswd),&passwdUni,&passwdUniLen);
	OPENSSL_asc2uni(jksPasswd,strlen(jksPasswd),&passwdUni,&passwdUniLen); // modify by leipp 20141022
	memcpy(jksPasswdUniBuf,passwdUni,passwdUniLen);
	free(passwdUni);
	passwdUniLen -= 2;

	md = EVP_sha1();
	EVP_MD_CTX_init(&ctx);
	EVP_DigestInit_ex(&ctx,md, NULL);
	EVP_DigestUpdate(&ctx,jksPasswdUniBuf,passwdUniLen);
	EVP_DigestUpdate(&ctx,JKS_MIGHTY,strlen(JKS_MIGHTY));
	EVP_DigestUpdate(&ctx,jksBuffer,offset);
	memset(digestBuf,0,sizeof(digestBuf));
	len = 0;
	EVP_DigestFinal_ex(&ctx,digestBuf,(unsigned int *)&len);
	//EVP_MD_CTX_destroy(&ctx);
	EVP_MD_CTX_cleanup(&ctx);
	
	if ((fp = fopen(jksFileName,"wb")) == NULL)
	{
		UnionUserErrLog("fopen jksFileName[%s] failed!\n",jksFileName);
		return errCodeOffsetOfCertMDL_WriteFile;
	}
	fwrite(jksBuffer,1,offset,fp);
	fwrite(digestBuf,1,len,fp);

	fclose(fp);
	
	return 0;
}

/*
功能：将pfx证书文件转换为JKS文件格式
输入：
	pfxFileName: pfx证书文件,含路径
	passwd: pfx证书保护口令
	jksFileName: 要转换生成的JKS文件名，含路径
	jskPasswd: jks的密码
返回：
	>=0 成功
	<0  失败
*/
int UnionPfx2JKS(char *pfxFileName,char *passwd, char *jksFileName,char *jksPasswd)
{
	STACK_OF(X509) *ca = NULL; 
	X509 * x509=NULL;
	EVP_PKEY *pkey = NULL;
	BIO *bioCert = NULL,*bioCa = NULL;
	BUF_MEM *bptr = NULL;
	PKCS12 *p12cert = NULL;
	int rc = 0;
	int len = 0;
	unsigned char *buf = NULL;
	char tbuf[8196]; 
	FILE *fp=NULL;   
	int ret = 0;
	char VK[4096];
	int i = 0;
	char certAscHex[4096];
	char certChain[MAX_CERTCHAIN_NUM][4096];
	int certChainNum = 0;
	char alias[100];

	UnionLoadLoadOpenSSLAlg();  
	IS_FILE_EXIST(pfxFileName);
	if ((fp = fopen(pfxFileName, "rb") ) == NULL)   
    {   
        UnionUserErrLog("UnionPfx2JKS::打开文件[%s]错误!\n",pfxFileName);   
        return errCodeOffsetOfCertMDL_ReadFile;   
    }   

	
    p12cert = d2i_PKCS12_fp(fp, NULL);   
    fclose (fp);   
	fp = NULL;
    if (!p12cert)    
    {   
        UnionUserErrLog("UnionPfx2JKS::d2i_PKCS12_fp failed!\n");   
        return errCodeOffsetOfCertMDL_FileFormat;   
    }   

	rc = PKCS12_parse(p12cert, passwd, &pkey, &x509, &ca);
	if (pkey == NULL || x509 == NULL)
	{
		UnionUserErrLog("UnionPfx2JKS pfxfile format err or passwd not right!\n");
		return errCodeOffsetOfCertMDL_Passwd;
	}
	
	len = i2d_PrivateKey(pkey,&buf);

	memset(tbuf,0,sizeof(tbuf));
	bcdhex_to_aschex((char *)buf,len,tbuf);
	strcpy(VK,tbuf);
	free(buf);
	buf = NULL;

	//输出x509格式的缓冲	
	 memset(tbuf,0,sizeof(tbuf));
		
	bioCert = BIO_new( BIO_s_mem() ); 
	if (bioCert == NULL)
	{
			UnionUserErrLog("UnionPfx2JKS BIO_new(BIO_s_mem) failed!\n");
			ret = errCodeOffsetOfCertMDL_OpenSSl;
			goto cleanup;
	}
	
	memset(alias,0,sizeof(alias));
	if (X509_alias_get0(x509,NULL) != NULL)
		strcpy(alias,(char *)X509_alias_get0(x509,NULL));

	i2d_X509_bio(bioCert,x509);
	X509_free(x509);
	x509 = NULL;

	BIO_get_mem_ptr(bioCert, &bptr); 
	memcpy(tbuf, bptr->data, bptr->length); 
	len = bptr->length;
	BUF_MEM_free(bptr);
	bptr = NULL;

	bioCert->ptr = NULL;
	BIO_free(bioCert);
	bioCert = NULL;
	memset(certAscHex,0,sizeof(certAscHex));
	bcdhex_to_aschex(tbuf,len,(char *)certAscHex);

	 //保存根证书到缓冲
	 for(i=0;i<MAX_CERTCHAIN_NUM;i++)
     {
            X509 *x5 = sk_X509_pop(ca);
            if (!x5)
                break;
			memset(tbuf,0,sizeof(tbuf));
		
			bioCa = BIO_new( BIO_s_mem() ); 
			if (bioCa == NULL)
			{
				UnionUserErrLog("UnionPfx2JKS BIO_new(BIO_s_mem) failed!\n");
				ret = errCodeOffsetOfCertMDL_OpenSSl;
				goto cleanup;
			}
		
			i2d_X509_bio(bioCa,x5);
			X509_free(x5);
			x5 = NULL;

			BIO_get_mem_ptr(bioCa, &bptr); 
			memcpy(tbuf, bptr->data, bptr->length); 
			len = bptr->length;
			BUF_MEM_free(bptr);
			bptr = NULL;

			bioCa->ptr = NULL;
			BIO_free(bioCa);
			bioCa = NULL;

			memset(certChain[i],0,sizeof(certChain[i]));
			bcdhex_to_aschex(tbuf,len,(char *)certChain[i]);
     }
	 certChainNum = i;
	 ret = UnionJksStore(alias,VK,passwd,certAscHex,certChain,certChainNum,jksFileName,jksPasswd);
cleanup:
	if (bioCert != NULL) BIO_free(bioCert);
	if (bioCa != NULL) BIO_free(bioCa);
	if (pkey != NULL) EVP_PKEY_free(pkey);
	if (p12cert != NULL) PKCS12_free(p12cert);
	if (x509 != NULL)  X509_free(x509);
	if (bptr != NULL) BUF_MEM_free(bptr);
	if (ca != NULL) 
	{
        for(;;)
        {
            X509 *x5 = sk_X509_pop(ca);
            if (!x5)
                break;
            X509_free(x5);
        }
        sk_X509_free (ca);
    } 

	return ret;
}


/*
功能：将证书链里的证书按顺序排列
输入：
	certChain: 证书字符串数组
	certChainNum: 证书个数
	descFlag: 排序方向。0：从上到下顺序（从根CA到下一级CA）, 1: 从下往上逆序（从最后一级CA到根CA）
*/
int UnionSortCert(char certChain[MAX_CERTCHAIN_NUM][4096],int certChainNum,int descFlag,char orderCertChain[MAX_CERTCHAIN_NUM][4096])
{
	int i = 0;
	char	buffer[8192];
	int ret = 0;
	int	localcate[MAX_CERTCHAIN_NUM];
	int destlocal = -1;

	X509 *cer = NULL;
	BIO *pbio = NULL;
	X509_NAME *pSignerName[MAX_CERTCHAIN_NUM];
	X509_NAME *pOwnerName[MAX_CERTCHAIN_NUM];
	X509_NAME *pName = NULL;
	UnionLoadLoadOpenSSLAlg();
	if (certChainNum > MAX_CERTCHAIN_NUM)
	{
		UnionUserErrLog("in UnionSortCert::certChainNum[%d] over MAX_CERTCHAIN_NUM[%d] !\n",certChainNum,MAX_CERTCHAIN_NUM);
		return errCodeOffsetOfCertMDL_FileFormat;
	}
	for(i=0;i<MAX_CERTCHAIN_NUM;i++)
	{
		pSignerName[i] = NULL;
		pOwnerName[i] = NULL;
		localcate[i] = -1;
	}

	for(i=0;i<certChainNum;i++)
	{
		memset(buffer,0,sizeof(buffer));
		aschex_to_bcdhex(certChain[i],strlen(certChain[i]),buffer);
		pbio = BIO_new_mem_buf(buffer,strlen(certChain[i])/2);
		if((cer = d2i_X509_bio(pbio,NULL)) == NULL)
		{
			BIO_free(pbio);
			pbio = BIO_new_mem_buf(buffer,strlen(certChain[i])/2);
			if((cer = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
			{
				BIO_free(pbio);
				UnionPemStrPreHandle(certChain[i],buffer,"CERTIFICATE",3000);
				pbio = BIO_new_mem_buf(buffer,strlen(buffer));
		 		cer = PEM_read_bio_X509(pbio,NULL,NULL,NULL);
			}
		}

		BIO_free(pbio);
		pbio = NULL;

		if (cer == NULL)
		{
			UnionUserErrLog("in UnionGetCertificateInfoFromBuf::read cert No[%d] failed!\n",i);
			return errCodeOffsetOfCertMDL_FileFormat;
		}

		pSignerName[i] = X509_NAME_dup(X509_get_issuer_name(cer)); 
		pOwnerName[i] = X509_NAME_dup(X509_get_subject_name(cer));

		if (X509_name_cmp(pSignerName[i],pOwnerName[i]) == 0)  //取根CA
		{
			destlocal = 0;
			strcpy(orderCertChain[destlocal],certChain[i]);
			localcate[i] = destlocal;
			pName = pOwnerName[i];
		}

		X509_free(cer);
	}

	
	if (-1 == destlocal)  //无根CA,不排序
	{
		for(i=0;i<certChainNum;i++)
			strcpy(orderCertChain[i],certChain[i]);
		ret = 0;
		goto cleanup;
	}

	while(1)
	{
		for(i=0;i<certChainNum;i++)
		{
			if (localcate[i] != -1)
				continue;
		
			if (X509_name_cmp(pName,pSignerName[i]) == 0)   //用签名者证书的owner与使用者证书的singer比较
			{
				destlocal++;
				strcpy(orderCertChain[destlocal],certChain[i]);
				localcate[i] = destlocal;
				pName = pOwnerName[i];           //选改证书的主题owner，做为下次查询的singer
				break;
			}
		}
		if (i >= certChainNum)
			break;
	}

	if ((destlocal+1) != certChainNum)
	{
		UnionUserErrLog("in UnionSortCert::certLink format is err!\n");
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}

	if (descFlag)  //逆序
	{
		for(i=0;i<certChainNum/2;i++)
		{
			strcpy(buffer,orderCertChain[i]);
			strcpy(orderCertChain[i],orderCertChain[destlocal-i]);
			strcpy(orderCertChain[destlocal-i],buffer);
		}
	}

	ret = 0;

cleanup:
	for(i=0;i<certChainNum;i++)
	{
		X509_NAME_free(pSignerName[i]);
		X509_NAME_free(pOwnerName[i]);
	}

	return ret;
}
