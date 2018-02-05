#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/des.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/pkcs12.h>

#include "UnionSHA1.h"
#include "UnionLog.h"
#include "unionCertFun.h"
#include "unionCertFunSM2.h"
#include "unionRSA.h"
#include "unionCharCode.h"
#include "unionVKForm.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"

int setSignData(int type, const unsigned char *m, unsigned int m_len,
	 unsigned char *sigret, unsigned int *siglen, RSA *rsa);
int setAlgor(X509_ALGOR *algor1,const EVP_MD *type);
int UnionConvertCrtTimeZtoBeiJin(char *timeZ,char *bjTime);

#define IS_FILE_EXIST(fileName)	if(access(fileName,0)) {	\
											UnionUserErrLog("fileName[%s]	is not exist\n",fileName);	\
											return errCodeOffsetOfCertMDL_FileNotExist;	\
										}
	
void UnionTrimCtrlMN(char *inbuf,char *outbuf)
{
	//char input[19200];
	char *input = NULL;
	char *p = NULL;
	//int offset = 0;
	//int len = 0;

	input = malloc(strlen(inbuf)+1);
	strcpy(input,inbuf);

	while ((p = strstr(input,"\x0D")) != NULL || (p = strstr(input,"\x0A")) != NULL )
	{
		*p = 0x00;
		p++;
		if (*p == 0x00)
			break;
		strcat(input,p);
	}
	strcpy(outbuf,input);
	free(input);
	input = NULL;
}
/*
void UnionTrimPemHeadTail(char *inbuf,char *outbuf,char *flagID)
{
	//char input[19200];
	//char *input = NULL;
	char *p = NULL;
	int offset = 0;
	char header[100];
	char ender[100];
	char *p1 = NULL;

	
	//memset(input,0,sizeof(input));
	//strcpy(input,inbuf);
	//p1 = input;
	p1 = inbuf;
	sprintf(header,"-----BEGIN %s-----",flagID);
	sprintf(ender,"-----END %s-----",flagID);
	if ((p = strstr(inbuf,header)) != NULL)
	{
		offset = (p - p1) + strlen(header);
	}
	if ((p = strstr(inbuf+offset,ender)) != NULL)
		*p = '\0';
	
	UnionTrimCtrlMN(inbuf+offset,outbuf);
}

int UnionPemStrPreHandle(char *inbuf,char *outbuf,char *flagID,int buflen)
{
	//char input[19200];
	//char buf[19200];
	char *input = NULL;
	char *buf = NULL;
	char header[100];
	char ender[100];
	int offset = 0;
	int  neendHeadFlag = 0;
	int  neendHeadCtrlFlag = 0;
	int  needTextCtrlFlag = 0;
	int intextlen = 0;
	int i = 0;
	int n = 0;
	char *p = NULL;
	int	len = 0;

	input = malloc(sizeof(char)*(buflen+1));
	buf= malloc(sizeof(char)*(buflen+1));
	if(!input||!buf)
	{
		UnionUserErrLog("UnionPemStrPreHandle::malloc failed!\n");
		return -1;
	}
	strcpy(input,inbuf);

	memset(buf,0,sizeof(char)*(buflen+1));
	UnionTrimCtrlMN(input,buf);
	strcpy(input,buf);
	//printf("input=[%s]\n",input);

	len = sprintf(header,"-----BEGIN %s-----",flagID);
	len += sprintf(ender,"-----END %s-----",flagID);
	len += strlen(inbuf);
	if (buflen < len+3)
	{
		UnionUserErrLog("UnionPemStrPreHandle:: buflen[%d] < len[%d]!\n",buflen,len+3);
		return (errCodeSmallBuffer);
	}
	if (strncmp(input,header,strlen(header)) != 0)
	{
		offset = 0;
		neendHeadFlag = 1;
	}
	else {
		offset += strlen(header);
	}

	if (input[offset] != 0x0A) 
	{
		neendHeadCtrlFlag = 1;
	}
	else {
		offset += 1;
	}

	if (input[offset+64] != 0x0A)
		needTextCtrlFlag = 1;

	if (neendHeadFlag == 0 && neendHeadCtrlFlag == 0 && needTextCtrlFlag == 0)
	{
		strcpy(outbuf,input);
		return strlen(input);
	}

	memset(buf,0,sizeof(char)*(buflen+1));
	offset = 0;
	if (neendHeadFlag)
	{
		strcpy(buf,header);
	}
	else {
		strncpy(buf,input,strlen(header));
		offset += strlen(header);
	}

	if (neendHeadCtrlFlag) 
	{
		strcat(buf,"\n");
	}
	else {
		strcat(buf,"\n");
		offset += 1;
	}

	p = strstr(input+offset,ender);
	if (p != NULL)
	{
		*p = 0x00;
		p--;
		if (*p == 0x0A)
		{
			*p = 0x00;
		}
	}

	intextlen = strlen(input+offset);
	if (needTextCtrlFlag)
	{	
		p = strstr(input+offset,ender);
		n = 64;
		for(i=0;i<intextlen;)
		{
			if ((intextlen - i) < 64)
				n = intextlen - i;
			strncat(buf,input+offset,n);
			strcat(buf,"\n");
			offset += 64;
			i += 64;
		}
	}
	else {
			strncat(buf,input+offset,intextlen);
	}
	strcat(outbuf,"\n");
	strcpy(outbuf,buf);
	strcat(outbuf,ender);


	free(input);
	free(buf);
	input = NULL;
	buf = NULL;
	return strlen(outbuf);
}
*/

/*
功能:加载OpenSSL的算法
*/
/*
void UnionLoadLoadOpenSSLAlg()
{
	if (gLoadOpenSSLAlgFlag)
		return;
	OpenSSL_add_all_algorithms();
	ERR_load_crypto_strings();
	gLoadOpenSSLAlgFlag = 1;
}
*/

/*
功能:产生用口令加密(des_ede3_cbc)的私钥文件
输入:
	keybits:RSA密钥强度512/1024/2048等
	vk:私钥(DER格式)
	passwd:私钥保护口令
	keyFileName:要生成的私钥名(带路径)
返回:
	>=0 成功
	<0 失败
*/

int UnionGenerateKeyPemWith3DesCBC(char *vk,char *passwd,char *keyFileName)
{
//	char pk[1024];
//	char vk[4096];
	EVP_PKEY    *pkey;
	long 		inl=0;
	char		buf[8192+1];
	unsigned char *const_buf;
	//int		keysize = 0;
	//int		len = 0;
	BIO *pbio;

	UnionLoadLoadOpenSSLAlg();
//	memset(pk,0,sizeof(pk));
//	memset(vk,0,sizeof(vk));
/*	if ((ret = UnionGenRSABySS(keybits,pk,sizeof(pk),vk,sizeof(vk))) < 0)
	{
		UnionUserErrLog("UnionGenerateSSLPemKey::UnionGenRSABySS failed! ret=[%d]\n",ret);
		return ret;
	}
*/
	pkey=EVP_PKEY_new();
	inl = strlen(vk)/2;
	memset(buf,0,sizeof(buf));

	aschex_to_bcdhex(vk,strlen(vk),buf);
	const_buf = (unsigned char *)buf;
	d2i_PrivateKey(EVP_PKEY_RSA,&pkey,(const unsigned char **)&const_buf,inl);
	
	pbio = BIO_new_file(keyFileName,"wb");
			
	if(!PEM_write_bio_PrivateKey(pbio,pkey,EVP_des_ede3_cbc(),
			(unsigned char*)passwd,strlen(passwd),0,NULL)) 
	{
		BIO_free(pbio);
		EVP_PKEY_free(pkey);
		return errCodeOffsetOfCertMDL_WriteFile;
	}

	BIO_free(pbio);
	pbio = NULL;

	EVP_PKEY_free(pkey);

	return 0;
}

/*
功能:私钥文件获取私钥
输入:
	keyFileName: 私钥文件
	passwd: 私钥保护口令
输出:
	VK: DER格式的私钥(扩展的AscHex)明文
	PK: DER格式的公钥(扩展的AscHex)明文
*/

int UnionGetPrivateKeyFromPEMFile(char *keyFileName,char *passwd,char *VK,int sizeofVK,char *PK,int sizeofPK)
{
	BIO *pbio = NULL;
	int len = 0;
	unsigned char *buf = NULL;
	char buffer[4096];
	EVP_PKEY	*pKey;
	int ret = 0;
	UnionLoadLoadOpenSSLAlg();
	
	IS_FILE_EXIST(keyFileName);
	pbio = BIO_new_file(keyFileName,"rb");
	
	pKey = PEM_read_bio_PrivateKey(pbio,NULL,0,(unsigned char*)passwd);
	if(pKey == NULL) {
		UnionUserErrLog("PEM_read_bio_PrivateKey failed! keyFileName[%s]\n",keyFileName);
		BIO_free(pbio);
		return errCodeOffsetOfCertMDL_FileFormat;
	}

	BIO_free(pbio);
	pbio = NULL;

	len = i2d_PrivateKey(pKey,&buf);
	if (len*2 > sizeofVK)
	{
		UnionUserErrLog("in UnionGetPrivateKeyFromPEMFile::sizeofVK too small!\n");
		EVP_PKEY_free(pKey);
		free(buf);
		return errCodeOffsetOfCertMDL_SmallBuffer;
	}
	EVP_PKEY_free(pKey);

	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)buf,len,buffer);
	free(buf);
	strcpy(VK,buffer);

	memset(buffer,0,sizeof(buffer));
	if ( (ret = UnionGetPKFromVK(VK,buffer)) < 0){
		UnionUserErrLog("in UnionGetPrivateKeyFromPEMFile::UnionGetPKFromVK failed! ret=[%d]\n",ret);
		return errCodeOffsetOfCertMDL_FileFormat;
	}

	if ( strlen(buffer) > sizeofPK)
	{
		UnionUserErrLog("in UnionGetPrivateKeyFromPEMFile::sizeofPK too small!\n");
		return errCodeOffsetOfCertMDL_SmallBuffer;
	}
	strcpy(PK,buffer);

	return len*2;
	
}


/*
功能:用私钥文件生成p10证书请求文件
输入:
	keyFileName: 私钥文件
	passwd: 私钥保护口令
	hashID: 摘要标志. 0--MD5,1--SHA1
	pReqUserInfo: 请求用户信息
	fileFormat: 生成文件的格式 0--PEM或1--DER
	fileName:生成的p10证书请求文件名(带路径)
返回:
	>=0 成功
	<0 失败
*/
/*
int UnionGeneratePKCS10(char *vkeyFileNamek,char *passwd,int hashID,PUnionX509UserInfo pReqUserInfo,int fileFormat,char *fileName)
 {
	 X509_REQ            *req = NULL;
	 int                  ret;
	 long               version;
	 X509_NAME         *name;
	 EVP_PKEY            *pkey = NULL;
	 RSA                  *rsa;
	 X509_NAME_ENTRY   *entry=NULL;
	
	 unsigned char      bytes[100];
	 char				mdout[40];
	 int                len,mdlen;
	 //int                bits=1024;
	 
	 const EVP_MD      *md = NULL;
	
	 BIO                      *b;
	 //int i = 0;
	 unsigned char		buf[8192+1];
	 const unsigned char* const_buf;
	 
     int inl=0;
     const ASN1_ITEM *it;
     void *asn = NULL;
     unsigned char *buf_in=NULL; 
     ASN1_BIT_STRING *signature;
     //FILE *mfp = NULL;
	 char vk[8192];
	 char pk[2048];
	 
	 char signdata[1024];

	 int  reqinfolen = 0;
	 
	 //STACK_OF(X509_EXTENSION) *exts;
	 
	 UnionLoadLoadOpenSSLAlg();

	 if (vkeyFileNamek == NULL || passwd == NULL || pReqUserInfo == NULL || fileName == NULL)
	 {
		 UnionUserErrLog("in UnionGeneratePKCS10::para err!\n");
		 return errCodeOffsetOfCertMDL_CodeParameter;
	 }

	 memset(vk,0,sizeof(vk));
	 memset(pk,0,sizeof(pk));

	 if ((ret = UnionGetPrivateKeyFromPEMFile(vkeyFileNamek,passwd,vk,sizeof(vk),pk,sizeof(pk))) < 0)
	 {
		 UnionUserErrLog("in UnionGeneratePKCS10::UnionGetPrivateKeyFromPEMFile failed ret=[%d]!\n",ret);
		 return ret;
	 }

	 req=X509_REQ_new();
	 
	 version=1;
	 ret=X509_REQ_set_version(req,version);
	 name=X509_NAME_new();
	 
	 if(pReqUserInfo->commonName != NULL && strlen(pReqUserInfo->commonName) != 0)
	 {
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pReqUserInfo->commonName,strlen(pReqUserInfo->commonName),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"commonName",V_ASN1_UTF8STRING,(unsigned char *)bytes,len);
		 X509_NAME_add_entry(name,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	 }
	 
	 if(pReqUserInfo->email != NULL && strlen(pReqUserInfo->email) != 0)
	 {
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pReqUserInfo->email,strlen(pReqUserInfo->email),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"emailAddress",V_ASN1_UTF8STRING,(unsigned char *)bytes,len);
		 X509_NAME_add_entry(name,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	 }
	 
	 if(pReqUserInfo->organizationalUnitName != NULL && strlen(pReqUserInfo->organizationalUnitName) != 0)
	 {
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pReqUserInfo->organizationalUnitName,strlen(pReqUserInfo->organizationalUnitName),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"organizationalUnitName",V_ASN1_UTF8STRING,bytes,len);
		 X509_NAME_add_entry(name,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	 }
	 
	 if(pReqUserInfo->organizationName != NULL && strlen(pReqUserInfo->organizationName) != 0)
	 {
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pReqUserInfo->organizationName,strlen(pReqUserInfo->organizationName),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"organizationName",V_ASN1_UTF8STRING,bytes,len);
		 X509_NAME_add_entry(name,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	 }
	 
	 if(pReqUserInfo->localityName != NULL && strlen(pReqUserInfo->localityName) != 0)
	 {
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pReqUserInfo->localityName,strlen(pReqUserInfo->localityName),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"localityName",V_ASN1_UTF8STRING,bytes,len);
		 X509_NAME_add_entry(name,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	 }
	 
	 if(pReqUserInfo->stateOrProvinceName != NULL && strlen(pReqUserInfo->stateOrProvinceName) != 0)
	 {
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pReqUserInfo->stateOrProvinceName,strlen(pReqUserInfo->stateOrProvinceName),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"stateOrProvinceName",V_ASN1_UTF8STRING,bytes,len);
		 X509_NAME_add_entry(name,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	 }
	 
	 if(pReqUserInfo->countryName != NULL && strlen(pReqUserInfo->countryName) != 0)
	 {
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pReqUserInfo->countryName,strlen(pReqUserInfo->countryName),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"countryName",V_ASN1_UTF8STRING,bytes,len);
		 X509_NAME_add_entry(name,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	 }
	 
	 // subject name 
	 ret=X509_REQ_set_subject_name(req,name);
	 
	 X509_NAME_free(name);
	 // pub key 
	 pkey=EVP_PKEY_new();
	 
	 aschex_to_bcdhex(pk,strlen(pk),(char *)buf);
	 inl = strlen(pk)/2;
	 const_buf = (const unsigned char *)buf;
	 d2i_PublicKey(EVP_PKEY_RSA,&pkey,&const_buf,inl);
	 
	 ret=X509_REQ_set_pubkey(req,pkey);
	 
	 if (hashID == 0)
		md = EVP_md5();
	 else 
		md = EVP_sha1();

	 asn = req->req_info;
	 it = ASN1_ITEM_rptr(X509_REQ_INFO);
	 reqinfolen=ASN1_item_i2d((struct ASN1_VALUE_st *)asn,&buf_in, it);
	 
	 setAlgor(req->sig_alg,md);
	 mdlen = 0;
	 memset(mdout,0,sizeof(mdout));
	 EVP_Digest(buf_in,reqinfolen,(unsigned char *)mdout,(unsigned int *)&mdlen,md, NULL);
	 free(buf_in);
	 
	 memset(signdata,0,sizeof(signdata));
	 inl = 0;
	 rsa = EVP_PKEY_get1_RSA(pkey);
	 
	 setSignData(md->type,(unsigned char *)mdout,mdlen,(unsigned char *)signdata,(unsigned int *)&inl,rsa);
	 RSA_free(rsa);
	 
	 memset(buf,0,sizeof(buf));
	 if ((len = UnionVKEncByPKCS1((unsigned char *)signdata,inl,vk,buf)) < 0)
	 {
		EVP_PKEY_free(pkey);
		X509_REQ_free(req);
		return errCodeOffsetOfCertMDL_Arithmetic;
	 }
	  //签名数据
	 signature = req->signature;
	 buf_in = (unsigned char *)buf;
	 signature->data=buf_in;
	 signature->length=len;
	 signature->flags&= ~(ASN1_STRING_FLAG_BITS_LEFT|0x07);
	 signature->flags|=ASN1_STRING_FLAG_BITS_LEFT;

	 // 写入文件PEM格式 
	 
	 b=BIO_new_file(fileName,"wb");
	 if (fileFormat == PEM) {
		PEM_write_bio_X509_REQ(b,req);
	 }
	 else {
		 i2d_X509_REQ_bio(b,req);
	 }
	 BIO_free(b);
	 
	 EVP_PKEY_free(pkey);
	 signature->data = NULL;
	 X509_REQ_free(req);
	 
	 return 0;
 }
*/

 int setSignData(int type, const unsigned char *m, unsigned int m_len,
	 unsigned char *sigret, unsigned int *siglen, RSA *rsa)
 {
	 X509_SIG sig;
	 ASN1_TYPE parameter;
	 int i,j,ret=1;
	 unsigned char *p, *tmps = NULL;
	 const unsigned char *s = NULL;
	 X509_ALGOR algor;
	 ASN1_OCTET_STRING digest;
	 //FILE *mfp =NULL;
	 
	 if(type == NID_md5_sha1) {
		 if(m_len != 36) {
			 UnionUserErrLog("m_len err,NID_md5_sha1 len must 36\n");
			 return(0);
		 }
		 i = 36;
		 s = m;
	 } else {
		 sig.algor= &algor;
		 sig.algor->algorithm=OBJ_nid2obj(type);
		 if (sig.algor->algorithm == NULL)
		 {
			 UnionUserErrLog("type[%d] is not surpport\n",type);
			 return(0);
		 }
		 if (sig.algor->algorithm->length == 0)
		 {
			 UnionUserErrLog("algorithm length is zero\n");
			 return(0);
		 }
		 parameter.type=V_ASN1_NULL;
		 parameter.value.ptr=NULL;
		 sig.algor->parameter= &parameter;
		 
		 sig.digest= &digest;
		 sig.digest->data=(unsigned char *)m; 
		 sig.digest->length=m_len;
		 
		 i=i2d_X509_SIG(&sig,NULL);
	 }
	 j=RSA_size(rsa);
	 if (i > (j-RSA_PKCS1_PADDING_SIZE))
	 {
		 UnionUserErrLog("input data too long\n");
		 return(0);
	 }
	 if(type != NID_md5_sha1) {
		 tmps=(unsigned char *)OPENSSL_malloc((unsigned int)j+1);
		 if (tmps == NULL)
		 {
			 UnionUserErrLog("OPENSSL_malloc failed\n");
			 return(0);
		 }
		 p=tmps;
		 i2d_X509_SIG(&sig,&p);
		 s=tmps;
	 }
	 
	 if (i <= 0)
	 {
		 ret=0;
		 return ret;
	 }
	 else
		 *siglen=i;
	 memcpy(sigret,s,i);
	 
	 if(type != NID_md5_sha1) {
		 OPENSSL_cleanse(tmps,(unsigned int)j+1);
		 OPENSSL_free(tmps);
	 }
	 
	 return(i);
 }

 int setAlgor(X509_ALGOR *algor1,const EVP_MD *type)
 {
	 X509_ALGOR *a;
	 a=algor1;
	 
	 if (a == NULL) {
		 UnionUserErrLog("algor1 is null\n");
		 return 0;
	 }
	 if (type->pkey_type == NID_dsaWithSHA1 ||
		 type->pkey_type == NID_ecdsa_with_SHA1)
	 {
		 ASN1_TYPE_free(a->parameter);
		 a->parameter = NULL;
	 }
	 else if ((a->parameter == NULL) || 
		 (a->parameter->type != V_ASN1_NULL))
	 {
		 ASN1_TYPE_free(a->parameter);
		 if ((a->parameter=ASN1_TYPE_new()) == NULL) 
		 {
			 UnionUserErrLog("ASN1_TYPE_new err\n"); 
			 return -1;
		 }
		 a->parameter->type=V_ASN1_NULL;
	 }
	 ASN1_OBJECT_free(a->algorithm);
	 a->algorithm=OBJ_nid2obj(type->pkey_type);
	 if (a->algorithm == NULL)
	 {
		 UnionUserErrLog("a->algorithm is NULL\n"); 
		 return -1;
	 }
	 if (a->algorithm->length == 0)
	 {
		 UnionUserErrLog("a->algorithm->length is 0\n"); 
		 return -1;
	 }
	 return 0;
	 
 }

 /*
 功能：验证证书请求文件格式及签名是否正确
输入：
	CerReqFileName, 证书请求文件名
返回：
	>0：验证成功
	<0：证书请求文件验证失败
 */
 int UnionVerifyCertificateREQ(char *CerReqFileName)
 {
	X509_REQ *req = NULL;
	char buffer[8192];
	EVP_PKEY * pk = NULL;
	BIO *pbio = NULL;
	int pass = 0;
	//int len = 0;

	UnionLoadLoadOpenSSLAlg();
	
	IS_FILE_EXIST(CerReqFileName);
	
	pbio = BIO_new_file(CerReqFileName,"rb");
	
	memset(buffer,0,sizeof(buffer));

	if ((req = PEM_read_bio_X509_REQ(pbio,NULL,NULL,buffer)) == NULL) {
		BIO_free(pbio);
		pbio = BIO_new_file(CerReqFileName,"rb");
		req = d2i_X509_REQ_bio(pbio,NULL);
	}

	BIO_free(pbio);
	pbio = NULL;
	
	if (req == NULL)
	{
		UnionUserErrLog("read_bio_X509_REQ failed!\n");
		return errCodeOffsetOfCertMDL_FileFormat;
	}
	//取公钥
    pk = X509_REQ_get_pubkey(req);
    if(pk == NULL)
    {
        UnionUserErrLog("in UnionVerifyCertificateREQ::X509_REQ_get_pubkey failed!");
        return errCodeOffsetOfCertMDL_OpenSSl;
    }

    pass=0;
    pass = X509_REQ_verify(req,pk); //验证，出现问题
    if(pass < 0)   
    {
        UnionUserErrLog("in UnionVerifyCertificateREQ::X509_REQ_verify err");
        EVP_PKEY_free(pk);
        X509_REQ_free(req);
        return errCodeOffsetOfCertMDL_OpenSSl;
    }
    if(pass == 0)
    {
        UnionUserErrLog("in UnionVerifyCertificateREQ:: X509_REQ_verify failed");
        EVP_PKEY_free(pk);
        X509_REQ_free(req);
        return errCodeOffsetOfCertMDL_VerifyCert;
    }
	EVP_PKEY_free(pk);
	X509_REQ_free(req);
	return 1;
 }

/*
 功能：从证书请求文件取国家、省、城市、单位、部门、用户名、Email、公钥、摘要算法、签名等属性值
输入：
	CerReqFileName, 证书请求文件名
输出:
	pCerReqInfo: 证书请求信息结构指针
 */
int UnionGetCertificateREQInfo(char *CerReqFileName,PUnionP10CerReq pCerReqInfo)
{
	X509_REQ *req;
	char buffer[8192];
	EVP_PKEY * pk = NULL;
	BIO *pbio;
	int pass = 0;
	unsigned char *p = NULL;
	int len = 0;
	ASN1_BIT_STRING *signature;
	unsigned char *buf_in = NULL;
	int type = 0;
	X509_NAME *pName;
	int ret = 0;

	UnionLoadLoadOpenSSLAlg();

	if (CerReqFileName == NULL || pCerReqInfo == NULL)
	{
		UnionUserErrLog("in UnionGetCertificateREQ::paramters err!\n");
		return errCodeOffsetOfCertMDL_CodeParameter;
	}

	IS_FILE_EXIST(CerReqFileName);
	pbio = BIO_new_file(CerReqFileName,"rb");
	memset(buffer,0,sizeof(buffer));
	if ((req = PEM_read_bio_X509_REQ(pbio,NULL,NULL,buffer)) == NULL)
	{	
		BIO_free(pbio);
		pbio = BIO_new_file(CerReqFileName,"rb");
		req = d2i_X509_REQ_bio(pbio,NULL);
	}
	BIO_free(pbio);
	if (req == NULL)
	{
		UnionUserErrLog("PEM_read_bio_X509_REQ failed!\n");
		return errCodeOffsetOfCertMDL_FileFormat;
	}
	//取版本信息
	pCerReqInfo->version = ASN1_INTEGER_get(req->req_info->version);

	//取公钥
    pk = X509_REQ_get_pubkey(req);
    if(pk == NULL)
    {
        UnionUserErrLog("in UnionGetCertificateREQInfo::X509_REQ_get_pubkey failed!");
        return errCodeOffsetOfCertMDL_OpenSSl;
    }
	len = i2d_PublicKey(pk,&p);
	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)p,len,buffer);
	free(p);
	p = NULL;
	strcpy(pCerReqInfo->derPK,buffer);

	pName = X509_REQ_get_subject_name(req);
	if (pName == NULL ) 
	{
		UnionUserErrLog("in UnionGetCertificateREQInfo::X509_REQ_get_subject_name failed!");
		EVP_PKEY_free(pk);
    X509_REQ_free(req);
		return errCodeOffsetOfCertMDL_OpenSSl;
	}

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_commonName, buffer, sizeof(buffer));
    if (ret >= 1)  {
			UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)(*pCerReqInfo).reqUserInfo.commonName,sizeof((*pCerReqInfo).reqUserInfo.commonName));
	}

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_organizationalUnitName, buffer, sizeof(buffer));
    if (ret >= 1) {
			UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)(*pCerReqInfo).reqUserInfo.organizationalUnitName,sizeof((*pCerReqInfo).reqUserInfo.organizationalUnitName));
	}

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_organizationName, buffer, sizeof(buffer));
    if (ret >= 1)  {
			UnionUtf8ToAnsi((unsigned char *)buffer,strlen(buffer),(unsigned char *)(*pCerReqInfo).reqUserInfo.organizationName,sizeof((*pCerReqInfo).reqUserInfo.organizationName));
	}

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_localityName, buffer, sizeof(buffer));
    if (ret >= 1)  {
			UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)(*pCerReqInfo).reqUserInfo.localityName,sizeof((*pCerReqInfo).reqUserInfo.localityName));
	}

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_stateOrProvinceName, buffer, sizeof(buffer));
    if (ret >= 1)  {
		UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)(*pCerReqInfo).reqUserInfo.stateOrProvinceName,sizeof((*pCerReqInfo).reqUserInfo.stateOrProvinceName));
	}

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_countryName, buffer, sizeof(buffer));
    if (ret >= 1)  {
			UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)(*pCerReqInfo).reqUserInfo.countryName,sizeof((*pCerReqInfo).reqUserInfo.countryName));
	}

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_pkcs9_emailAddress, buffer, sizeof(buffer));
    if (ret >= 1)  {
			UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)(*pCerReqInfo).reqUserInfo.email,sizeof((*pCerReqInfo).reqUserInfo.email));
	}

    pass=0;
    pass = X509_REQ_verify(req,pk); //验证，出现问题
    if(pass < 0)   
    {
        UnionUserErrLog("in in UnionGetCertificateREQInfo::X509_REQ_verify err!");
        EVP_PKEY_free(pk);
        X509_REQ_free(req);
        return errCodeOffsetOfCertMDL_OpenSSl;
    }
    if(pass == 0)
    {
        UnionUserErrLog("in UnionGetCertificateREQInfo::X509_REQ_verify failed!");
        EVP_PKEY_free(pk);
        X509_REQ_free(req);
        return errCodeOffsetOfCertMDL_VerifyCertReq;
    }
	signature = req->signature;
	buf_in = signature->data;
	len = signature->length;
	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)buf_in,len,buffer);

	strcpy(pCerReqInfo->sign,buffer);

	type = OBJ_obj2nid(req->sig_alg->algorithm);
	
	if (type == NID_md5WithRSAEncryption)
		pCerReqInfo->hashID = 0;
	else if (type == NID_sha1WithRSAEncryption)
		pCerReqInfo->hashID = 1;
	else
		pCerReqInfo->hashID = 2;

	EVP_PKEY_free(pk);
	X509_REQ_free(req);
    
	return 1;
}

/*
功能：给自己(根CA的角色)签发证书
输入:
	vkeyFileNamek: 私钥文件
	passwd: 私钥保护口令
	hashID: 摘要标志. 0--MD5,1--SHA1
	pUserInfo: 用户信息结构指针
	sn: 证书序列号(唯一值)
	days: 证书有效天数
	fileFormat: 生成文件的格式 0--PEM或1--DER
	fileName:生成的p10证书请求文件名(带路径)
返回:
	>=0 成功
	<0 失败
*/
int UnionMakeSelfCert(char *vkeyFileNamek,char *passwd,int hashID,PUnionX509UserInfo pUserInfo,long sn,int days,int fileFormat,char *selfCertfileName)
{
	X509 *pCert=NULL;
	EVP_PKEY *pubkey;
	
	//X509  *CACer;
	X509_NAME *name;

	//STACK_OF(X509_EXTENSION) *req_exts;
	const EVP_MD *digest;

	EVP_PKEY *CAVKey;
	unsigned char *const_buf = NULL;
	unsigned char buffer[8192];
	int		ret = 0;
	int		inl = 0;
	int		len = 0;
	unsigned char   bytes[100];
	X509_NAME_ENTRY   *entry=NULL;
	char vk[8192],pk[2048];
	BIO *pbio;

	UnionLoadLoadOpenSSLAlg();

	if (vkeyFileNamek == NULL || passwd == NULL || pUserInfo == NULL || selfCertfileName == NULL)
	{
		UnionUserErrLog("in UnionMakeSelfCert::paramter err!\n");
		return errCodeOffsetOfCertMDL_CodeParameter;
	}

	if (hashID == 0)
		digest = EVP_md5();
	else 
		digest = EVP_sha1();

	pCert = X509_new();
	if(pCert == NULL ) 
	{
		UnionUserErrLog("UnionMakeSelfCert::create X509 object memory err!\n");
		return errCodeOffsetOfCertMDL_OpenSSl;
	}

	memset(vk,0,sizeof(vk));
	memset(pk,0,sizeof(pk));
	if ((ret = UnionGetPrivateKeyFromPEMFile(vkeyFileNamek,passwd,vk,sizeof(vk),pk,sizeof(pk))) < 0)
	{
		UnionUserErrLog("UnionMakeSelfCert::UnionGetPrivateKeyFromPEMFile [%s] err! ret=[%d]\n",vkeyFileNamek,ret);
		return ret;
	}

	 pubkey = EVP_PKEY_new();
	 memset(buffer,0,sizeof(buffer));
	 
	 aschex_to_bcdhex(pk,strlen(pk),(char *)buffer);
	 inl = strlen(pk)/2;
	 const_buf = (unsigned char *)buffer;
	 d2i_PublicKey(EVP_PKEY_RSA,&pubkey,(const unsigned char **)&const_buf,inl);

	// 设置证书基本项目

	//设置版本号
	X509_set_version(pCert, 2);
	
	//设置序列号
	ASN1_INTEGER_set(X509_get_serialNumber(pCert),sn); 
	
	//设置起始时间
	X509_gmtime_adj(X509_get_notBefore(pCert),0);

	//设置终止时间
	X509_gmtime_adj(X509_get_notAfter(pCert), (long)60*60*24*days);

	//设置主体名(从证书请求文件取得)
	name=X509_NAME_new();
	 
	if(pUserInfo->commonName != NULL && strlen(pUserInfo->commonName) != 0)
	{
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pUserInfo->commonName,strlen(pUserInfo->commonName),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"commonName",V_ASN1_UTF8STRING,(unsigned char *)bytes,len);
		 X509_NAME_add_entry(name,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	}
	 
	if(pUserInfo->email != NULL && strlen(pUserInfo->email) != 0)
	{
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pUserInfo->email,strlen(pUserInfo->email),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"emailAddress",V_ASN1_UTF8STRING,(unsigned char *)bytes,len);
		 X509_NAME_add_entry(name,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	}
	 
	if(pUserInfo->organizationalUnitName != NULL && strlen(pUserInfo->organizationalUnitName) != 0)
	{
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pUserInfo->organizationalUnitName,strlen(pUserInfo->organizationalUnitName),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"organizationalUnitName",V_ASN1_UTF8STRING,bytes,len);
		 X509_NAME_add_entry(name,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	}
	 
	if(pUserInfo->organizationName != NULL && strlen(pUserInfo->organizationName) != 0)
	{
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pUserInfo->organizationName,strlen(pUserInfo->organizationName),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"organizationName",V_ASN1_UTF8STRING,bytes,len);
		 X509_NAME_add_entry(name,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	}
	 
	if(pUserInfo->localityName != NULL && strlen(pUserInfo->localityName) != 0)
	{
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pUserInfo->localityName,strlen(pUserInfo->localityName),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"localityName",V_ASN1_UTF8STRING,bytes,len);
		 X509_NAME_add_entry(name,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	}
	 
	if(pUserInfo->stateOrProvinceName != NULL && strlen(pUserInfo->stateOrProvinceName) != 0)
	{
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pUserInfo->stateOrProvinceName,strlen(pUserInfo->stateOrProvinceName),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"stateOrProvinceName",V_ASN1_UTF8STRING,bytes,len);
		 X509_NAME_add_entry(name,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	}
	 
	if(pUserInfo->countryName != NULL && strlen(pUserInfo->countryName) != 0)
	{
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pUserInfo->countryName,strlen(pUserInfo->countryName),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"countryName",V_ASN1_UTF8STRING,bytes,len);
		 X509_NAME_add_entry(name,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	}

	if (!X509_set_subject_name(pCert, name))
	{
		X509_NAME_free(name);
		X509_free(pCert);
		EVP_PKEY_free(pubkey);
		return -5;
	}
	
	//设置公钥(从证书请求文件取得)
	if (!X509_set_pubkey(pCert,pubkey))
	{
		X509_NAME_free(name);
		X509_free(pCert);
		EVP_PKEY_free(pubkey);
		return errCodeOffsetOfCertMDL_OpenSSl;
	}
	EVP_PKEY_free(pubkey);

	X509_set_issuer_name(pCert, name);

	X509_NAME_free(name);

	CAVKey = EVP_PKEY_new();
	memset(buffer,0,sizeof(buffer));
	aschex_to_bcdhex(vk,strlen(vk),(char *)buffer);
	const_buf = buffer;
	d2i_PrivateKey(EVP_PKEY_RSA,&CAVKey,(const unsigned char **)&const_buf,strlen(vk)/2);
	//签名
	if (!X509_sign(pCert,CAVKey,digest))
	{
		X509_free(pCert);
		EVP_PKEY_free(CAVKey);
		return errCodeOffsetOfCertMDL_Sign;
	}
	EVP_PKEY_free(CAVKey);
	// 将X509证书，保存到文件
	pbio = BIO_new_file(selfCertfileName,"wb");
	if(fileFormat == DER) {
		ret = i2d_X509_bio(pbio,pCert);
	}
	else {
		ret = PEM_write_bio_X509(pbio,pCert);
	}
	X509_free(pCert);
	BIO_free(pbio);
	if (!ret) {
		UnionUserErrLog("write certfile failed!\n");
		return errCodeOffsetOfCertMDL_WriteFile;
	}
	
	return ret;
}

/*
功能：根据证书请求文件，给请求者签发证书
输入:
	CACerFileName: CA证书文件
	vkeyFileName: CA证书对应的私钥文件，用于给证书签名
	passwd: 私钥文件的保护口令
	hashID: 签名时用的摘要标志 0--MD5,1--SHA1
	CerReqFileName: 证书请求文件名
	sn: 生成证书的序列号
	days：证书的有效天数
	fileFormat: 生成文件的格式 0--PEM或1--DER
	CertfileName: 要生成证书的文件名(带路径)
返回:
	>= 0 成功
	< 0 失败
*/
int UnionMakeCert(char *CACerFileName,char *vkeyFileName,char *passwd,int hashID,char *CerReqFileName,long sn,int days,int fileFormat,char *CertfileName)
{
	X509_REQ *req = NULL;
	X509 *pCert=NULL;
	EVP_PKEY *pubkey = NULL;
	
	X509  *CACer = NULL;
	X509_NAME *pName = NULL;
	int		  san_pos = -100;
	X509_EXTENSION *tmpsan = NULL;
	STACK_OF(X509_EXTENSION) *req_exts;

	const EVP_MD *digest;

	EVP_PKEY *CAVKey = NULL,*CAPKey = NULL;
	unsigned char *const_buf = NULL;
	char buffer[4096];
	int ret = 0;
	BIO *pbio = NULL;
	int pass;
	char derCAVK[8192],derCAPK[2048];
	int len = 0;
	unsigned char *buf = NULL;
	
	UnionLoadLoadOpenSSLAlg();

	IS_FILE_EXIST(CerReqFileName);
	IS_FILE_EXIST(CACerFileName);
	
	pCert = X509_new();
	if(pCert == NULL ) 
	{
		UnionUserErrLog("in UnionMakeCert::X509_new failed!\n");
		return errCodeOffsetOfCertMDL_OpenSSl;
	}

	if (hashID == 0)
		digest = EVP_md5();
	 else 
		digest = EVP_sha1();

	 //读请求文件
	pbio = BIO_new_file(CerReqFileName,"rb");
	memset(buffer,0,sizeof(buffer));

	if ((req = PEM_read_bio_X509_REQ(pbio,NULL,NULL,buffer)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_file(CerReqFileName,"rb");
		req = d2i_X509_REQ_bio(pbio,NULL);
	}
	if (req == NULL)
	{
		UnionUserErrLog("PEM_read_bio_X509_REQ failed!\n");
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto makeCertExit;
	}
	BIO_free(pbio);
	pbio = NULL;

	pubkey = X509_REQ_get_pubkey(req);
	if (pubkey == NULL)
	{
		UnionUserErrLog("in UnionMakeCert::X509_REQ_get_pubkey from CerReqFileName[%s] failed!\n",CerReqFileName);
		ret = errCodeOffsetOfCertMDL_OpenSSl;
		goto makeCertExit;
	}

	pass=0;
    pass = X509_REQ_verify(req,pubkey); //验证，出现问题
    if(pass < 0)   
    {
        UnionUserErrLog("in UnionMakeCert::X509_REQ_verify err\n");
        ret = errCodeOffsetOfCertMDL_OpenSSl;
		goto makeCertExit;
    }
    if(pass == 0)
    {
        UnionUserErrLog("in UnionMakeCert::X509_REQ_verify failed\n");
        ret = errCodeOffsetOfCertMDL_VerifyCertReq;
		goto makeCertExit;
    }

	//取私钥文件
	memset(derCAVK,0,sizeof(derCAVK));
	memset(derCAPK,0,sizeof(derCAPK));
	if ((ret = UnionGetPrivateKeyFromPEMFile(vkeyFileName,passwd,derCAVK,sizeof(derCAVK),derCAPK,sizeof(derCAPK))) < 0)
	{
		 UnionUserErrLog("in UnionMakeCert::UnionGetPrivateKeyFromPEMFile failed ret=[%d]!\n",ret);
		 goto makeCertExit;
	}

	CAVKey = EVP_PKEY_new();
	memset(buffer,0,sizeof(buffer));
	aschex_to_bcdhex(derCAVK,strlen(derCAVK),buffer);
	const_buf = (unsigned char *)buffer;
	d2i_PrivateKey(EVP_PKEY_RSA,&CAVKey,(const unsigned char **)&const_buf,strlen(derCAVK)/2);
	
	// 读CA证书文件
	pbio = BIO_new_file(CACerFileName,"rb");
	if((CACer = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
	{
			BIO_free(pbio);
			pbio = BIO_new_file(CACerFileName,"rb");
			CACer = d2i_X509_bio(pbio,NULL);
	}
	
	if (CACer == NULL)
	{
		UnionUserErrLog("in UnionMakeCert:: read CACerFile[%s] falied!\n",CACerFileName);
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto makeCertExit;
	}

	BIO_free(pbio);
	pbio = NULL;

	CAPKey = X509_get_pubkey(CACer);
	len = i2d_PublicKey(CAPKey,&buf);
	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)buf,len,(char *)buffer);
	free(buf);
    if(strcmp(derCAPK,buffer) != 0)
    {
        UnionUserErrLog("in UnionMakeCert:: PK Not Match!\n");
        ret = errCodeOffsetOfCertMDL_PKNotMatchVK;
		goto makeCertExit;
    }

	// 设置证书基本项目

	//设置版本号
	X509_set_version(pCert, 2);
	
	//设置序列号
	ASN1_INTEGER_set(X509_get_serialNumber(pCert),sn); 
	
	//设置起始时间
	X509_gmtime_adj(X509_get_notBefore(pCert),0);

	//设置终止时间
	X509_gmtime_adj(X509_get_notAfter(pCert), (long)60*60*24*days);

	//设置主体名(从证书请求文件取得)
	if (!X509_set_subject_name(pCert, X509_REQ_get_subject_name(req)))
	{
		UnionUserErrLog("in Makecert::X509_set_subject_name failed\n");
		ret = errCodeOffsetOfCertMDL_OpenSSl;
		goto makeCertExit;
	}
	
	//设置公钥(从证书请求文件取得)
	if (!X509_set_pubkey(pCert,pubkey))
	{
		UnionUserErrLog("in Makecert::X509_set_pubkey failed\n");
		ret = errCodeOffsetOfCertMDL_OpenSSl;
		goto makeCertExit;
	}

	//设置证书签发者(从CA证书取)
	pName = X509_get_subject_name(CACer);
	if (pName == NULL ) 
	{
		UnionUserErrLog("in Makecert::X509_get_subject_name failed\n");
		ret = errCodeOffsetOfCertMDL_OpenSSl;
		goto makeCertExit;
	}
	X509_set_issuer_name(pCert, pName);
	
	//设置扩展项目
	//加入可选名 SN_subject_alt_name
	req_exts = X509_REQ_get_extensions(req);
	while(req_exts != NULL) 
	{
		san_pos = X509v3_get_ext_by_NID(req_exts,OBJ_sn2nid(SN_subject_alt_name),-1);
		tmpsan =X509v3_get_ext(req_exts,san_pos);
		break;
	}
	if(tmpsan != NULL)
	{
		if(!X509_add_ext(pCert,tmpsan,-1))
		{
			ret = errCodeOffsetOfCertMDL_OpenSSl;
			UnionUserErrLog("in Makecert::X509_add_ext failed!\n");
			goto makeCertExit;;
		}
	}

	//签名
	if (!X509_sign(pCert,CAVKey,digest))
	{
		UnionUserErrLog("in Makecert::X509_sign failed!\n");
		ret = errCodeOffsetOfCertMDL_Sign;
		goto makeCertExit;;
	}

	// 将X509证书，保存到文件
	pbio = BIO_new_file(CertfileName,"wb");
	if(fileFormat == DER) {
		ret = i2d_X509_bio(pbio,pCert);
	}
	else {
		ret = PEM_write_bio_X509(pbio,pCert);
	}
	BIO_free(pbio);
	pbio = NULL;
	if (!ret) {
		UnionUserErrLog("in Makecert::WriteFile [%s] failed!\n",CertfileName);
		ret = errCodeOffsetOfCertMDL_WriteFile;
		goto makeCertExit;
	}
	ret = 0;

makeCertExit:
	if (req != NULL) X509_REQ_free(req); 
	if (pubkey != NULL) EVP_PKEY_free(pubkey);
	if (CACer != NULL) 	X509_free(CACer);
	if (CAVKey != NULL)	EVP_PKEY_free(CAVKey);
	if (CAPKey != NULL)	EVP_PKEY_free(CAPKey);
	if (pCert != NULL) X509_free(pCert);
	if (pbio != NULL)	BIO_free(pbio);
	return ret;
}

/*
功能：从证书文件获取公钥
输入：
	CerFileName：证书文件
输出：
	derPK：der格式的公钥(扩展的aschex可见字符串)
返回:
	>= 0 成功
	< 0 失败
*/
int UnionGetPKOfCertificate(char *CerFileName,char *derPK,int sizeofderPK)
{
	X509 *cer = NULL;
	//const unsigned char* const_buf;
	unsigned char buffer[8192];
	int len = 0;
	EVP_PKEY *PKey;
	//int ret = 0;
	BIO *pbio = NULL;
	unsigned char *buf = NULL;
	
	UnionLoadLoadOpenSSLAlg();
	if (CerFileName == NULL || derPK == NULL)
		return errCodeOffsetOfCertMDL_CodeParameter;

	IS_FILE_EXIST(CerFileName);
	pbio = BIO_new_file(CerFileName,"rb");
	if (pbio == NULL)
	{
		return errCodeOffsetOfCertMDL_ReadFile;
	}
	if((cer = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_file(CerFileName,"rb");
		cer = d2i_X509_bio(pbio,NULL);
	}
	
	BIO_free(pbio);
	pbio = NULL;
	if (cer == NULL)
	{
		return errCodeOffsetOfCertMDL_FileFormat;
	}

	PKey = X509_get_pubkey(cer);
	len = i2d_PublicKey(PKey,&buf);
	if (len*2 > sizeofderPK)
	{
		free(buf);
		X509_free(cer);
		return errCodeOffsetOfCertMDL_SmallBuffer;
	}
	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)buf,len,(char *)buffer);
	free(buf);
	strcpy(derPK,(char *)buffer);
	EVP_PKEY_free(PKey);
	X509_free(cer);
	return len*2;
}


/*
功能：用上一级证书验证看是否为自己颁发的证书，注意要认证CertfileName证书，首先要信用CACerFileName证书
输入：
	CACerFileName：CA证书文件
	CertfileName: 要认证的证书文件
返回:
	>= 0 成功
	< 0 失败
*/
int UnionVerifyCertificate(char *CACerFileName,char *CertfileName)
{
	X509 *cer = NULL;
	const unsigned char* const_buf = NULL;
	unsigned char buffer[4096];
	//int len;
	EVP_PKEY *pkey = NULL;
	int ret = 0;
	BIO *pbio = NULL;
	char caPK[2048];
	
	UnionLoadLoadOpenSSLAlg();
	IS_FILE_EXIST(CACerFileName);
	IS_FILE_EXIST(CertfileName);
	
	if (CACerFileName == NULL || CertfileName == NULL)
		return errCodeOffsetOfCertMDL_CodeParameter;

	// 读证书
	pbio = BIO_new_file(CertfileName,"rb");
	if (pbio == NULL)
	{
		return errCodeOffsetOfCertMDL_ReadFile;
	}
	if((cer = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_file(CertfileName,"rb");
		cer = d2i_X509_bio(pbio,NULL);
	}
	
	BIO_free(pbio);
	pbio = NULL;
	if (cer == NULL)
	{
		UnionUserErrLog("in UnionVerifyCertificate::read CertfileName failed!\n");
		return errCodeOffsetOfCertMDL_FileFormat;
	}

	//取CA证书的公钥
	if ((ret = UnionGetPKOfCertificate(CACerFileName,caPK,sizeof(caPK))) < 0)
	{
		UnionUserErrLog("in UnionVerifyCertificate::UnionGetPKOfCertificate failed!\n");
		return ret;
	}
	
	memset(buffer,0,sizeof(buffer));
	aschex_to_bcdhex(caPK,strlen(caPK),(char *)buffer);
	const_buf = buffer;
	pkey=EVP_PKEY_new();
	d2i_PublicKey(EVP_PKEY_RSA,&pkey,&const_buf,strlen(caPK)/2);

	ret = X509_verify(cer,pkey);
	if(ret < 0)   
    {
        UnionUserErrLog("in UnionVerifyCertificate::X509_verify err!\n");
        EVP_PKEY_free(pkey);
        X509_free(cer);
        return errCodeOffsetOfCertMDL_OpenSSl;
    }
    if(ret == 0)
    {
        UnionUserErrLog("in UnionVerifyCertificate::X509_verify failed!\n");
        EVP_PKEY_free(pkey);
        X509_free(cer);
        return errCodeOffsetOfCertMDL_VerifyCert;
    }
	X509_free(cer);
	EVP_PKEY_free(pkey);
	return 1;
}

// 转换证书时间为北京时间,返回time_t
int UnionConvertCrtTimeZtoBeiJin(char *timeZ,char *bjTime)
{
	struct tm tm,*loctm;
	time_t t;
	int offset = 0;
	 
	if (timeZ == NULL || bjTime == NULL)
		return -1;
	if (strlen(timeZ) != 13 )
	{
		return -2;
	}
	memset(&tm,0,sizeof(struct tm));
#define g2(p) (((p)[0]-'0')*10+(p)[1]-'0')
	tm.tm_year=g2(timeZ);
	if(tm.tm_year < 50)
		tm.tm_year+=100;
	tm.tm_mon=g2(timeZ+2)-1;
	tm.tm_mday=g2(timeZ+4);
	tm.tm_hour=g2(timeZ+6);
	tm.tm_min=g2(timeZ+8);
	tm.tm_sec=g2(timeZ+10);
#undef g2
	if ( timeZ[12] == 'Z')
		offset = 8*60*60;
	t = mktime(&tm) + offset;

	loctm = localtime(&t);
	sprintf(bjTime,"%04d%02d%02d%02d%02d%02d",1900+loctm->tm_year,loctm->tm_mon+1,loctm->tm_mday,loctm->tm_hour,loctm->tm_min,loctm->tm_sec);
	return t;
}


/*
功能：从证书文件获取证书信息属性值
输入：
	CerFileName：证书文件
	pCertInfo：证书信息结构指针
返回:
	>= 0 成功
	< 0 失败
*/
int UnionGetCertificateInfo(char *CerFileName,PUnionX509Cer pCertInfo)
{
	X509 *cer = NULL;
	//const unsigned char* const_buf;
	char buffer[8192];
	int len = 0;
	EVP_PKEY *PKey = NULL;
	int ret = 0;
	BIO *pbio = NULL;
	unsigned char *buf = NULL;
	ASN1_BIT_STRING *signature;
	X509_NAME *pName = NULL;
	PUnionX509UserInfo pUserInfo = NULL;
	int i = 0;
	long type = 0;
	unsigned char tbuf[1024];
	BIGNUM bnum;

	UnionLoadLoadOpenSSLAlg();
	
	if (CerFileName == NULL || pCertInfo == NULL) {
		UnionUserErrLog("in UnionGetCertificateInfo::paramters err!\n");
		return errCodeOffsetOfCertMDL_CodeParameter;
	}

	IS_FILE_EXIST(CerFileName);
	pbio = BIO_new_file(CerFileName,"rb");
	if (pbio == NULL)
	{
		return errCodeOffsetOfCertMDL_ReadFile;
	}
	if((cer = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_file(CerFileName,"rb");
		cer = d2i_X509_bio(pbio,NULL);
	}

	BIO_free(pbio);
	pbio = NULL;
	if (cer == NULL)
	{
		return errCodeOffsetOfCertMDL_FileFormat;
	}

	PKey = X509_get_pubkey(cer);

	len = i2d_PublicKey(PKey,&buf);
	if (len*2 > sizeof(pCertInfo->derPK))
	{
		X509_free(cer);
		EVP_PKEY_free(PKey);
		free(buf);
		return errCodeOffsetOfCertMDL_SmallBuffer;
	}
	//公钥
	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)buf,len,(char *)buffer);
	free(buf);
	buf = NULL;
	strcpy(pCertInfo->derPK,(char *)buffer);
	
	if (X509_alias_get0(cer,NULL) != NULL) {
		strcpy(pCertInfo->userInfo.alias,(char *)X509_alias_get0(cer,NULL));
	}

	for(i=0;i<2;i++)
	{
	//issuer
	if (i == 0) {
		pName = X509_get_issuer_name(cer); 
		pUserInfo = &(pCertInfo->issuerInfo);
	}
	else if (i == 1) {  // 取用户信息
		pName = X509_get_subject_name(cer);
		pUserInfo = &(pCertInfo->userInfo);
	}
	else
		break;

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_commonName, buffer, sizeof(buffer));
    if (ret >= 1)  {
			UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)pUserInfo->commonName,sizeof(pUserInfo->commonName));
	}

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_organizationalUnitName, buffer, sizeof(buffer));
    if (ret >= 1) {
			UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)pUserInfo->organizationalUnitName,sizeof(pUserInfo->organizationalUnitName));
	}

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_organizationName, buffer, sizeof(buffer));
    if (ret >= 1)  {
			UnionUtf8ToAnsi((unsigned char *)buffer,strlen(buffer),(unsigned char *)pUserInfo->organizationName,sizeof(pUserInfo->organizationName));
	}

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_localityName, buffer, sizeof(buffer));
    if (ret >= 1)  {
			UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)pUserInfo->localityName,sizeof(pUserInfo->localityName));
	}

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_stateOrProvinceName, buffer, sizeof(buffer));
    if (ret >= 1)  {
		UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)pUserInfo->stateOrProvinceName,sizeof(pUserInfo->stateOrProvinceName));
	}

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_countryName, buffer, sizeof(buffer));
    if (ret >= 1)  {
			UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)pUserInfo->countryName,sizeof(pUserInfo->countryName));
	}

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_pkcs9_emailAddress, buffer, sizeof(buffer));
    if (ret >= 1)  {
			UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)pUserInfo->email,sizeof(pUserInfo->email));
	}
	}

	//证书起始日期
	memset(buffer,0,sizeof(buffer));
	len = ASN1_STRING_length(cer->cert_info->validity->notBefore);
	memcpy(buffer,ASN1_STRING_data(cer->cert_info->validity->notBefore),len);
	UnionConvertCrtTimeZtoBeiJin((char *)buffer,pCertInfo->startDateTime);

	//证书终止日期
	memset(buffer,0,sizeof(buffer));
	len = ASN1_STRING_length(cer->cert_info->validity->notAfter);
	memcpy(buffer,ASN1_STRING_data(cer->cert_info->validity->notAfter),len);
	UnionConvertCrtTimeZtoBeiJin((char *)buffer,pCertInfo->endDataTime);

	//版本号
	pCertInfo->version = ASN1_INTEGER_get(cer->cert_info->version);
	//序列号
	pCertInfo->serialNumber = ASN1_INTEGER_get(cer->cert_info->serialNumber);
	
	//add by hzh in 2010.6.2 序列号可能为一个大整数
	BN_init(&bnum);
	ASN1_INTEGER_to_BN(cer->cert_info->serialNumber,&bnum);
	memset(tbuf,0,sizeof(tbuf));
	len = BN_bn2bin(&bnum,tbuf);
	BN_clear_free(&bnum);
	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)tbuf,len,buffer);
	strcpy(pCertInfo->serialNumberAscBuf,buffer);
	//end
	
	//摘要算法标记
	type = OBJ_obj2nid(cer->sig_alg->algorithm);
	
	if (type == NID_md5WithRSAEncryption)
		pCertInfo->hashID = 0;
	else if (type == NID_sha1WithRSAEncryption)
		pCertInfo->hashID = 1;
	else
		pCertInfo->hashID = 2;
	// 签名
	signature = cer->signature;
	buf = signature->data;
	len = signature->length;
	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)buf,len,(char *)buffer);
	strcpy(pCertInfo->sign,(char *)buffer);
	X509_free(cer);
	EVP_PKEY_free(PKey);
	return 1;
}

/*
功能：将私钥文件及X509证书合并成pfx文件
输入：
	keyFilename：私钥文件
	passwd：私钥文件保护口令
	crtFileName: x509证书文件
	caCrtFileName :ca证书文件
	alias:	别名
	pfxFileName: 要生成的pfx文件
返回：
	>= 0 成功
	< 0 失败
*/
int UnionCombinationPfx(char *keyFilename,char *passwd,char *crtFileName,char *caCrtFileName,char *alias,char *pfxFileName)
{
	 BIO *pbio = NULL;
	 PKCS12 *p12 = NULL;
	 EVP_PKEY *vKey = NULL,*pKey = NULL;
	 X509 *crt = NULL;
	 X509 *caCrt = NULL;
	 char vkBuf[8192],pkBuf[2048];
	 unsigned char buffer[4096];
	 int ret = 0;
	 unsigned char *const_buf = NULL;
	 unsigned char *buf = NULL;
	 int len = 0;
	 STACK_OF (X509) *ca = NULL;

	 
	if(passwd == NULL || keyFilename == NULL || crtFileName == NULL)
	{
		return errCodeOffsetOfCertMDL_CodeParameter;
	}


	UnionLoadLoadOpenSSLAlg();
	IS_FILE_EXIST(keyFilename);
	IS_FILE_EXIST(crtFileName);
	pbio = BIO_new_file(crtFileName,"rb");
	if (pbio == NULL)
	{
		return -2;
	}
	if((crt = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_file(crtFileName,"rb");
		crt = d2i_X509_bio(pbio,NULL);
	}
	
	BIO_free(pbio);
	pbio = NULL;
	if (crt == NULL)
	{
		return errCodeOffsetOfCertMDL_FileFormat;
	}

	// 读CA文件
	//if (caCrtFileName != NULL)
		//UnionFilterRightBlank(caCrtFileName);
	// 当caCrtFileName不为NULL，且长度不为0，才合并CA证书
	if (caCrtFileName != NULL && strlen(caCrtFileName) != 0) {
		
		pbio = BIO_new_file(caCrtFileName,"rb");
		if (pbio == NULL)
		{
			return errCodeOffsetOfCertMDL_ReadFile;
		}
		if((caCrt = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
		{
			BIO_free(pbio);
			pbio = BIO_new_file(caCrtFileName,"rb");
			caCrt = d2i_X509_bio(pbio,NULL);
		}
		
		BIO_free(pbio);
		pbio = NULL;
		if (caCrt == NULL)
		{
			return errCodeOffsetOfCertMDL_FileFormat;
		}

		ca = sk_X509_new_null();
		if (ca == NULL)
		{
			UnionUserErrLog("UnionCombinationPfx: sk_X509_new_null Error!\n");
			return errCodeOffsetOfCertMDL_OpenSSl;
		}

		if (!sk_X509_push(ca, caCrt))
		{
			UnionUserErrLog("UnionCombinationPfx: sk_X509_push Error!\n");
			return errCodeOffsetOfCertMDL_OpenSSl;
		}
	}

	memset(vkBuf,0,sizeof(vkBuf));
	memset(pkBuf,0,sizeof(pkBuf));
	ret = UnionGetPrivateKeyFromPEMFile(keyFilename,passwd,vkBuf,sizeof(vkBuf),pkBuf,sizeof(pkBuf));
	if (ret < 0) {
		UnionUserErrLog("CombinationPfx::UnionGetPrivateKeyFromPEMFile failed! \n");
		goto Cleanup;
	}

	vKey = EVP_PKEY_new();
	memset(buffer,0,sizeof(buffer));
	aschex_to_bcdhex(vkBuf,strlen(vkBuf),(char *)buffer);
	const_buf = buffer;
	d2i_PrivateKey(EVP_PKEY_RSA,&vKey,(const unsigned char **)&const_buf,strlen(vkBuf)/2);


	pKey = X509_get_pubkey(crt);
	len = i2d_PublicKey(pKey,&buf);
	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)buf,len,(char *)buffer);
	free(buf);
    if(strcmp(pkBuf,(char *)buffer) != 0)
    {
        UnionUserErrLog("keyFilename and crtFileName do not match!");
		ret = errCodeOffsetOfCertMDL_PKNotMatchVK;
		goto Cleanup;
    }

	pbio = BIO_new_file(pfxFileName,"wb");
	if ((p12 = PKCS12_create(passwd,
		alias, vKey, crt,
		ca, 0,0,0,0,0)) == NULL)
	{
		ret = errCodeOffsetOfCertMDL_WriteFile;
		goto Cleanup;
	}
	i2d_PKCS12_bio(pbio,p12);	
	ret = 0;
	
Cleanup:
	if (p12 != NULL) PKCS12_free(p12);
	if (pbio != NULL) BIO_free(pbio);
	if (vKey != NULL) EVP_PKEY_free(vKey);
	if (pKey != NULL) EVP_PKEY_free(pKey);
	if (crt != NULL)  X509_free(crt);
//	if (caCrt != NULL) X509_free(caCrt);  由于sk_X509_free，这里不能X509_free(caCrt)
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
功能：倒入根证书(或二级CA证书)到pfx文件
输入：
	pfxFileName: pfx证书文件
	passwd: pfx证书口令
	caCrtFileName: CA证书
返回：
	>= 0 成功
	< 0 失败
*/
int UnionImportCAToPfx(char *pfxFileName,char *passwd,char *caCrtFileName)
{
	BIO *pbio = NULL;
	STACK_OF(X509) *ca = NULL; 
	X509 *crt=NULL;
	X509 *caCrt=NULL;
	EVP_PKEY *pkey = NULL;
	//BIO * in=NULL;
	PKCS12 *p12cert = NULL;
	//int rc = 0;
	//int len = 0;
	//char *buf = NULL;
	//char buffer[4096];
	//char tbuf[4096];
	FILE *fp=NULL;   
	int ret = 0;

	 if (pfxFileName == NULL || passwd == NULL || caCrtFileName == NULL)
	 {
		 UnionUserErrLog("UnionImportCAToPfx paramters err!\n");
		 return errCodeOffsetOfCertMDL_CodeParameter;
	 }

	 UnionLoadLoadOpenSSLAlg();
	 IS_FILE_EXIST(pfxFileName);
	 IS_FILE_EXIST(caCrtFileName);
	 //读CA文件
	 pbio = BIO_new_file(caCrtFileName,"rb");
	 if (pbio == NULL)
	 {
			return errCodeOffsetOfCertMDL_ReadFile;
	 }
	 if((caCrt = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
	 {
		 BIO_free(pbio);
		 pbio = BIO_new_file(caCrtFileName,"rb");
		 caCrt = d2i_X509_bio(pbio,NULL);
	 }
	
	  BIO_free(pbio);
	  pbio = NULL;
	  if (caCrt == NULL)
	  {
			return errCodeOffsetOfCertMDL_FileFormat;
	  }

	 /// 读入P12，并分解
    if (!(fp = fopen (pfxFileName, "rb")))
    {
         UnionUserErrLog("UnionImportCAToPfx open pfx file[%s] failed!\n",pfxFileName);
		 return errCodeOffsetOfCertMDL_ReadFile;
    }

    p12cert = d2i_PKCS12_fp (fp, NULL);
    fclose (fp);
    if (!p12cert)
    {
        UnionUserErrLog("UnionImportCAToPfx open d2i_PKCS12_fp file[%s] failed!\n",pfxFileName);
		return errCodeOffsetOfCertMDL_FileFormat;
    }

    if (!PKCS12_parse (p12cert, passwd, &pkey, &crt, &ca))
    {
        ret = errCodeOffsetOfCertMDL_Passwd;
		UnionUserErrLog("UnionImportCAToPfx::PKCS12_parse failed!\n");
		goto cleanup;
    }
    PKCS12_free (p12cert);
    p12cert = NULL;

	if (!sk_X509_push(ca, caCrt))
	{
		UnionUserErrLog("UnionCombinationPfx: sk_X509_push Error!\n");
		ret = errCodeOffsetOfCertMDL_OpenSSl;
		goto cleanup;
	}


	pbio = BIO_new_file(pfxFileName,"wb");
	if ((p12cert = PKCS12_create(passwd,
		(char *)X509_alias_get0(crt,NULL), pkey, crt,
		ca, 0,0,0,0,0)) == NULL)
	{
		ret = errCodeOffsetOfCertMDL_WriteFile;
		goto cleanup;
	}
	i2d_PKCS12_bio(pbio,p12cert);	
	ret = 0;

cleanup:
	if (pbio != NULL) BIO_free(pbio);
	if (pkey != NULL) EVP_PKEY_free(pkey);
	if (p12cert != NULL) PKCS12_free(p12cert);
	if (crt != NULL)  X509_free(crt);
//	if (caCrt != NULL) X509_free(caCrt);  // 由于sk_X509_free，这里不能X509_free(caCrt)
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
	函数功能：从pfx证书获取公、私钥对
	输入参数：
	pfxFilname：pfx证书文件名
	pfxPassword: 私钥密码

	输出参数：
	VK: pfx证书的私钥(DER编码的可见字符串)
	PK:  pfx证书的公钥(DER编码的可见字符串)

	返回：>=0 成功， < 0 失败
*/
int UnionGetRSAPairFromPfx(char *pfxFilname,char *pfxPassword, char *VK, char *PK)
{
	STACK_OF(X509) *ca = NULL; 
	X509 * x509=NULL;
	EVP_PKEY *pkey = NULL;
	//BIO * in=NULL;
	PKCS12 *p12cert = NULL;
	int rc = 0;
	int len = 0;
	unsigned char *buf = NULL;
	char tbuf[4096];
	FILE *fp=NULL;   

	UnionProgramerLog("in UnionGetRSAPairFromPfx::start SSLeay_add_all_algorithms...\n");
	UnionLoadLoadOpenSSLAlg();  
	IS_FILE_EXIST(pfxFilname);
	if (!(fp = fopen(pfxFilname, "rb")))   
	{   
		UnionUserErrLog("UnionGetRSAPairFromPfx::打开文件[%s]错误!\n",pfxFilname);   
		return errCodeOffsetOfCertMDL_ReadFile;   
	}   
	UnionProgramerLog("in UnionGetRSAPairFromPfx::start d2i_PKCS12_fp...\n");
	p12cert = d2i_PKCS12_fp(fp, NULL);   
	fclose (fp);   
	if (!p12cert)    
	{   
		UnionUserErrLog("UnionGetRSAPairFromPfx::读取包文件错误\n");   
		return errCodeOffsetOfCertMDL_FileFormat;   
	}

	//printf("in UnionGetRSAPairFromPfx::start PKCS12_parse...\n");
	rc = PKCS12_parse(p12cert, pfxPassword, &pkey, &x509, &ca);
	// 解析公钥
	// 解析x509
	if (pkey == NULL || x509 == NULL)
	{
		UnionUserErrLog("UnionGetRSAPairFromPfx pkey or x509 is NULL!\n");
		return errCodeOffsetOfCertMDL_Passwd;
	}

	len = i2d_PrivateKey(pkey,&buf);

	memset(tbuf,0,sizeof(tbuf));
	bcdhex_to_aschex((char *)buf,len,tbuf);
	strcpy(VK,tbuf);
	free(buf);
	
	buf = NULL;
	len = i2d_PublicKey(pkey,&buf);
	
	memset(tbuf,0,sizeof(tbuf));
	bcdhex_to_aschex((char *)buf,len,tbuf);
	strcpy(PK,tbuf);
	free(buf);

//cleanup:
	if (pkey != NULL) EVP_PKEY_free(pkey);
	if (p12cert != NULL) PKCS12_free(p12cert);
	if (x509 != NULL)  X509_free(x509);
//	if (caCrt != NULL) X509_free(caCrt);  // 由于sk_X509_free，这里不能X509_free(caCrt)
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

	return 0;
}

/*
功能: 检查证书是否在撤消列表中
输入：
	tCRLFileName： 证书撤消列表文件名
	tCertFileName: 被检查的证书名
返回:
	<0 出错 
	=0,不在黑名单 
	=1,在黑名单 
*/
/*
int UnionCertCRLCheck(char *tCRLFileName,char *tCertFileName) 
{ 
	int i; 
	int iret=0; 
	BIO *pbio = NULL;
	X509 *x509_cert =NULL; 
	X509_CRL *x509_crl=NULL; 
	STACK_OF(X509_REVOKED) *revoked = NULL;
	int num = 0;
	X509_REVOKED *rc = NULL;
	ASN1_INTEGER *serial = NULL;

	UnionLoadLoadOpenSSLAlg();
	IS_FILE_EXIST(tCRLFileName);
	//读取CRL文件
	pbio = BIO_new_file(tCRLFileName,"rb");
	x509_crl = d2i_X509_CRL_bio(pbio,NULL); //默认先读取der编码的crl文件
  if(x509_crl == NULL)
  {
     BIO_free(pbio);  
     pbio = BIO_new_file(tCRLFileName,"rb");
     x509_crl = PEM_read_bio_X509_CRL(pbio, NULL, NULL, NULL); //BASE64编码的crl文件
	if(x509_crl == NULL)
	{
		iret = errCodeOffsetOfCertMDL_ReadFile;
		goto cleanup;
	}
  }

	BIO_free(pbio);
	pbio = NULL;

	//打开X509证书文件*
	pbio = BIO_new_file(tCertFileName,"rb");
	if((x509_cert = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
	{
			BIO_free(pbio);
			pbio = BIO_new_file(tCertFileName,"rb");
			x509_cert = d2i_X509_bio(pbio,NULL);
	}
	
	if (x509_cert == NULL)
	{
		iret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}

	BIO_free(pbio);
	pbio = NULL;
	
	revoked = x509_crl->crl->revoked; 
	 
	serial=X509_get_serialNumber(x509_cert); 
	num=revoked->num; 
	for(i=0;i<num;i++)
	{ 
		rc=sk_X509_REVOKED_pop(revoked); 
		if(ASN1_INTEGER_cmp(serial,rc->serialNumber)==0)
		{ 
			iret=1; 
			break; 
		} 
	} 


cleanup:
	if (pbio != NULL) BIO_free(pbio);
	if (x509_crl != NULL) X509_CRL_free(x509_crl); 
	if (x509_cert != NULL) X509_free(x509_cert); 
	return(iret); 
 } 
*/
#define CERT_MAX_LEN 4096
/*
	函数功能：从pfx证书获取私钥,公钥证书，CA证书
	输入参数：
	pfxFilname：pfx证书文件名
	pfxPassword: 私钥密码
	caCertFileName：CA根证书文件名
输出参数：
	VK: pfx证书的私钥(DER编码的可见字符串)
	x509FileName:  保存公钥证书文件名

	返回：>=0 成功， < 0 失败
*/
int UnionGetVKAndCertFromPfx(char *pfxFilname,char *pfxPassword, char *VK, char *x509FileName,char *caCertFileName,char* cerBuf,char* caCerBuf)
{
	STACK_OF(X509) *ca = NULL; 
	X509 * x509=NULL;
	X509 * caCert=NULL;
	EVP_PKEY *pkey = NULL;
	BIO *bioCert = NULL,*bioCa = NULL,*pbio=NULL;
	BIO *bioMemCert = NULL;
	BUF_MEM *bptr = NULL;
	PKCS12 *p12cert = NULL;
	int rc = 0;
	int len = 0;
	unsigned char *buf = NULL;
	char tbuf[CERT_MAX_LEN+1]; 
	FILE *fp=NULL;   
	int ret = 0;

	UnionLoadLoadOpenSSLAlg();  
	IS_FILE_EXIST(pfxFilname);
	if (!(fp = fopen(pfxFilname, "rb")))   
    {   
        UnionUserErrLog("UnionGetVKAndCertFromPfx::open[%s] err!\n",pfxFilname);   
        return errCodeOffsetOfCertMDL_ReadFile;   
    }   
	
    p12cert = d2i_PKCS12_fp(fp, NULL);   
    fclose (fp);   
    if (!p12cert)    
    {   
        UnionUserErrLog("UnionGetVKAndCertFromPfx::d2i_PKCS12_fp failed\n");   
        return errCodeOffsetOfCertMDL_FileFormat;   
    }   

	rc = PKCS12_parse(p12cert, pfxPassword, &pkey, &x509, &ca);
	
	if (pkey == NULL || x509 == NULL)
	{
		UnionUserErrLog("UnionGetVKAndCertFromPfx pfxfile format err or passwd not right\n");
		return errCodeOffsetOfCertMDL_Passwd;
	}

	len = i2d_PrivateKey(pkey,&buf);

	memset(tbuf,0,sizeof(tbuf));
	bcdhex_to_aschex((char *)buf,len,tbuf);
	strcpy(VK,tbuf);
	free(buf);
	buf = NULL;

	//输出文件x509	
     if ((bioCert=BIO_new_file(x509FileName, "wb")) == NULL)
     {
           UnionUserErrLog("UnionGetVKAndCertFromPfx pkey or x509 is NULL!\n");
		   ret = -2;
		   goto cleanup;
     }
	 PEM_write_bio_X509(bioCert,x509);
	 BIO_free(bioCert);
	 bioCert = NULL;
	bioMemCert = BIO_new(BIO_s_mem()); 
	int cLen=PEM_write_bio_X509(bioMemCert,x509);
	cLen=BIO_ctrl_pending(bioMemCert);
	BIO_read(bioMemCert,cerBuf,cLen);
	cerBuf[cLen]=0;
	//UnionUserErrLog("cerBuf:[%d][%s]\n",cLen,cerBuf);
	BIO_free(bioMemCert);
	bioMemCert=NULL;	

	 if (caCertFileName != NULL)
	 {
	 	if ((fp = fopen(caCertFileName,"rb")) == NULL)
	 	{
			  UnionUserErrLog("UnionGetVKAndCertFromPfx fopen caCertFileName[%s] failed!\n",caCertFileName);
			  ret = errCodeOffsetOfCertMDL_WriteFile;
			  goto cleanup;
		 }
		memset(tbuf,0,sizeof(tbuf));
		if ((cLen=fread(tbuf,1,5000,fp))<0)
			return -1;
		
		UnionUserErrLog("UnionGetVKAndCertFromPfx fopen caCertFileName cLen[%d] !\n",cLen);
                pbio = BIO_new_mem_buf(tbuf,cLen);
                if((caCert = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
                {
                        BIO_free(pbio);
                        pbio = BIO_new_mem_buf(tbuf,cLen);
                        caCert = d2i_X509_bio(pbio,NULL);
                }
		bioMemCert = BIO_new(BIO_s_mem());
	        int cLen=PEM_write_bio_X509(bioMemCert,caCert);
       	 	cLen=BIO_ctrl_pending(bioMemCert);
       		BIO_read(bioMemCert,caCerBuf,cLen);
		UnionUserErrLog("caCerBuf:[%d][%s]\n",cLen,caCerBuf);
		BIO_free(bioMemCert);
		bioMemCert=NULL;
		BIO_free(pbio);
		pbio=NULL;
		fclose(fp);
		ret = 0;
	}
	
	 
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
函数功能: 生成PKCS#7格式的签名
输入参数：
	vkindex: 0-20表示私钥索引号, 99表示外带LMK加密的VK, -1表示将采用vk明文私钥
	vk: 私钥二进制字符串. 当vkindex为0-20时，该参数无意义。当vkindex==99时，为LMK加密的密文，当vkindex==-1时，为明文私钥DER格式
	vklen: vk的长度
	data: 待签名数据
	dataLen: 数据长度
	x509FileName: 公钥证书文件
	caCertFileName: CA根证书
输出参数：
	pkcs7Sign: pkcs#7格式的签名
返回：>0 成功， 签名的长度
	  < 0 失败
*/
int UnionSignWithPkcs7(int vkindex,unsigned char *vk,int vklen,unsigned char *data,int dataLen,char *x509FileName,char *caCertFileName,unsigned char *pkcs7Sign,int sizeofPkcs7Sign)
{
	BIO *pbio = NULL;
	PKCS7* p7 = NULL;
	PKCS7_SIGNER_INFO* info = NULL;
	int i = 0;
	BIO *p7bio = NULL;
	STACK_OF(X509) *ca = NULL; 
	X509 *x509=NULL;
	X509 *caCert = NULL;
	EVP_PKEY *pkey = NULL;
	int ret = 0;
	FILE *fp = NULL;
	char tbuf[CERT_MAX_LEN+1]; 
	unsigned char *const_buf = NULL;
	int inl = 0;
	char buf[4096];
	BUF_MEM *bptr = NULL;

	if (data == NULL || x509FileName == NULL || caCertFileName == NULL || pkcs7Sign == NULL)
	{
		UnionUserErrLog("in UnionSignWithPkcs7::params err!\n");
		return errCodeOffsetOfCertMDL_CodeParameter;
	}
	UnionLoadLoadOpenSSLAlg();  
	IS_FILE_EXIST(x509FileName);
	IS_FILE_EXIST(caCertFileName);
	
	if (vkindex == -1) {
		pkey=EVP_PKEY_new();
		memcpy(buf,vk,vklen);
		const_buf = (unsigned char *)buf;
		d2i_PrivateKey(EVP_PKEY_RSA,&pkey,(const unsigned char **)&const_buf,vklen);
	}

	//将证书文件转x509	
	pbio = BIO_new_file(x509FileName,"rb");
	if (pbio == NULL)
	{
		return errCodeOffsetOfCertMDL_ReadFile;
	}
	if((x509 = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_file(x509FileName,"rb");
		x509 = d2i_X509_bio(pbio,NULL);
	}
	
	BIO_free(pbio);
	pbio = NULL;
	if (x509 == NULL)
	{
		return errCodeOffsetOfCertMDL_FileFormat;
	}

	if ((ca=sk_X509_new_null()) == NULL)
	{
			UnionUserErrLog("in UnionSignWithPkcs7::open sk_X509_new_null failure\n");
			ret = errCodeOffsetOfCertMDL_OpenSSl;
			goto cleanup;
	}

	//从根CA证书文件取证书 
	if ((fp = fopen(caCertFileName,"rb")) == NULL)
	{
		UnionUserErrLog("in UnionSignWithPkcs7::open caCertFileName[%s] failed!\n",caCertFileName);
		ret = errCodeOffsetOfCertMDL_ReadFile;
		goto cleanup;
	}
	for(;;)
	{
		memset(tbuf,0,sizeof(tbuf));
		if (fread(tbuf,1,CERT_MAX_LEN,fp) != CERT_MAX_LEN)
			break;

		pbio = BIO_new_mem_buf(tbuf,CERT_MAX_LEN);
		if((caCert = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
		{
			BIO_free(pbio);
			pbio = BIO_new_mem_buf(tbuf,CERT_MAX_LEN);
			caCert = d2i_X509_bio(pbio,NULL);
		}

		if (caCert == NULL)
		{
			UnionUserErrLog("in UnionSignWithPkcs7::get caCert failed!\n");
			ret = errCodeOffsetOfCertMDL_FileFormat;
			goto cleanup;
		}
		if (!sk_X509_push(ca,caCert))
		{
				UnionUserErrLog("in UnionSignWithPkcs7::sk_X509_push failed!\n");
				ret = errCodeOffsetOfCertMDL_OpenSSl;
				goto cleanup;
		}
		// add
		BIO_free(pbio);
		pbio = NULL;
		caCert = NULL;
		// end add 
	}
	fclose(fp);
	fp = NULL;

	p7 = PKCS7_new();
	PKCS7_set_type(p7, NID_pkcs7_signed); //设置类型为NID_pkcs7_signed
	PKCS7_content_new(p7, NID_pkcs7_data);
	PKCS7_set_detached(p7, 0);
	//添加签名者信息，
	//x509：签名证书，pkey：签名者私钥。EVP_sha1()签名者摘要算法。
	info = PKCS7_add_signature(p7, x509, pkey, EVP_sha1());
	//添加签名者证书
	PKCS7_add_certificate(p7, x509);

	//添加签名者的CA证书链
	for (i=0; i<sk_X509_num(ca); i++)
	{
		PKCS7_add_certificate(p7, sk_X509_value(ca, i));
	}

	//p7bio = PKCS7_dataInit(p7, NULL);  //该函数有内存泄露

	pbio = BIO_new(BIO_s_mem());
	BIO_set_mem_eof_return(pbio,0);
	p7bio = PKCS7_dataInit(p7, pbio);

	BIO_write(p7bio, data, dataLen); //加入原始数据，
	PKCS7_dataFinal(p7, p7bio); //处理数据。

	BIO_free(pbio);
	pbio = NULL;

	pbio = BIO_new( BIO_s_mem() ); 
	PEM_write_bio_PKCS7(pbio,p7);
//	bptr = BUF_MEM_new();
	BIO_get_mem_ptr(pbio, &bptr); 
	
	memcpy(tbuf, bptr->data, bptr->length); 
	inl = bptr->length;
	BUF_MEM_free(bptr);
	bptr = NULL;

	pbio->ptr = NULL;
	BIO_free(pbio);
	pbio = NULL;

	if (inl > sizeofPkcs7Sign)
	{
		UnionUserErrLog("in UnionSignWithPkcs7::sizeofPkcs7Sign[%d] too small!\n",sizeofPkcs7Sign);
		ret = errCodeOffsetOfCertMDL_SmallBuffer;
		goto cleanup;
	}
	memcpy(pkcs7Sign, tbuf, inl); 

	ret = inl;
cleanup:
	if (pbio != NULL) BIO_free(pbio);
	if (pkey != NULL) EVP_PKEY_free(pkey);
	if (x509 != NULL)  X509_free(x509);
	if (caCert != NULL) X509_free(caCert);  //add
	if (p7 != NULL) PKCS7_free(p7);
	if (p7bio != NULL) BIO_free(p7bio);
	if (fp != NULL) fclose(fp);
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
函数功能: 生成PKCS#7格式的签名
输入参数：
	vkindex: 0-20表示私钥索引号, 99表示外带LMK加密的VK, -1表示将采用vk明文私钥
	vk: 私钥字符串,扩展的aschex. 当vkindex为0-20时，该参数无意义。当vkindex==99时，为LMK加密的密文，当vkindex==-1时，为明文私钥DER格式
	data: 待签名数据
	dataLen: 数据长度
	x509FileName: 公钥证书文件
	caCertFileName: CA根证书
输出参数：
	pkcs7Sign: pkcs#7格式的签名
返回：>0 成功， 签名的长度
	  < 0 失败
*/
/*
int UnionSignWithPkcs7Hsm(int vkindex,char *vk,unsigned char *data,int dataLen,char *x509FileName,char *caCertFileName,unsigned char *pkcs7Sign,int sizeofPkcs7Sign)
{
	BIO *pbio = NULL;
	PKCS7* p7 = NULL;
	PKCS7_SIGNER_INFO* info = NULL;
	int i = 0;
	BIO *p7bio = NULL;
	STACK_OF(X509) *ca = NULL; 
	X509 *x509=NULL;
	X509 *caCert = NULL;
	EVP_PKEY *pkey = NULL;
	int ret = 0;
	FILE *fp = NULL;
	char tbuf[CERT_MAX_LEN+1]; 
	//unsigned char *const_buf = NULL;
	int inl = 0;
	char buf[4096];
	unsigned char sign[1024];
	BUF_MEM *bptr = NULL;
	int hashID = 1;
	STACK_OF(PKCS7_SIGNER_INFO) *si_sk=NULL;

	//int j = 0;

	if (data == NULL || x509FileName == NULL || caCertFileName == NULL || pkcs7Sign == NULL)
	{
		UnionUserErrLog("in UnionSignWithPkcs7Hsm::params err!\n");
		return errCodeOffsetOfCertMDL_CodeParameter;
	}
	UnionLoadLoadOpenSSLAlg();  
	IS_FILE_EXIST(x509FileName);
	//将证书文件转x509	
	pbio = BIO_new_file(x509FileName,"rb");
	if (pbio == NULL)
	{
		return errCodeOffsetOfCertMDL_ReadFile;
	}
	if((x509 = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_file(x509FileName,"rb");
		x509 = d2i_X509_bio(pbio,NULL);
	}
	
	BIO_free(pbio);
	pbio = NULL;
	if (x509 == NULL)
	{
		return errCodeOffsetOfCertMDL_FileFormat;
	}

	pkey = X509_get_pubkey(x509);

	if ((ca=sk_X509_new_null()) == NULL)
	{
			UnionUserErrLog("in UnionSignWithPkcs7Hsm::open sk_X509_new_null failure\n");
			ret = errCodeOffsetOfCertMDL_OpenSSl;
			goto cleanup;
	}

	//从根CA证书文件取证书 
	if ((fp = fopen(caCertFileName,"rb")) == NULL)
	{
		UnionUserErrLog("in UnionSignWithPkcs7Hsm::open caCertFileName[%s] failed!\n",caCertFileName);
		ret = errCodeOffsetOfCertMDL_ReadFile;
		goto cleanup;
	}
	for(;;)
	{
		memset(tbuf,0,sizeof(tbuf));
		if (fread(tbuf,1,CERT_MAX_LEN,fp) != CERT_MAX_LEN)
			break;

		pbio = BIO_new_mem_buf(tbuf,CERT_MAX_LEN);
		if((caCert = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
		{
			BIO_free(pbio);
			pbio = BIO_new_mem_buf(tbuf,CERT_MAX_LEN);
			caCert = d2i_X509_bio(pbio,NULL);
		}

		if (caCert == NULL)
		{
			UnionUserErrLog("in UnionSignWithPkcs7Hsm::get caCert failed!\n");
			ret = errCodeOffsetOfCertMDL_FileFormat;
			goto cleanup;
		}
		if (!sk_X509_push(ca,caCert))
		{
				UnionUserErrLog("in UnionSignWithPkcs7Hsm::sk_X509_push failed!\n");
				ret = errCodeOffsetOfCertMDL_OpenSSl;
				goto cleanup;
		}
		// add
		BIO_free(pbio);
		pbio = NULL;
		caCert = NULL;
		// end add 
	}
	fclose(fp);
	fp = NULL;

	p7 = PKCS7_new();
	PKCS7_set_type(p7, NID_pkcs7_signed); //设置类型为NID_pkcs7_signed
	PKCS7_content_new(p7, NID_pkcs7_data);
	PKCS7_set_detached(p7, 0);
	//添加签名者信息，
	//x509：签名证书，pkey：签名者私钥。EVP_sha1()签名者摘要算法。
	info = PKCS7_add_signature(p7, x509, pkey, EVP_sha1());
	//添加签名者证书
	PKCS7_add_certificate(p7, x509);

	//添加签名者的CA证书链
	for (i=0; i<sk_X509_num(ca); i++)
	{
		PKCS7_add_certificate(p7, sk_X509_value(ca, i));
	}

	
	if (!ASN1_STRING_set(p7->d.sign->contents->d.data,data, dataLen))
	{
		ret = errCodeOffsetOfCertMDL_OpenSSl;
		goto cleanup;
	}
	
	memset(buf,0,sizeof(buf));
	if (vkindex == -1) {
		if ((ret = UnionSignWithSoft(vk,1,data,dataLen,buf)) < 0)
		{
			ret = errCodeOffsetOfCertMDL_Arithmetic;
			goto cleanup;
		}
	}
	else {
		if ((ret = UnionSignWithHsm(vkindex,vk,hashID,data,dataLen,buf)) < 0)
			goto cleanup;
	}
	aschex_to_bcdhex(buf,strlen(buf),(char *)sign);
	si_sk = p7->d.sign->signer_info;
	info=sk_PKCS7_SIGNER_INFO_value(si_sk,0);
	if (!ASN1_STRING_set(info->enc_digest,sign,strlen(buf)/2))
	{
		ret = errCodeOffsetOfCertMDL_OpenSSl;
		goto cleanup;
	}
	pbio = BIO_new( BIO_s_mem() ); 
	PEM_write_bio_PKCS7(pbio,p7);
//	bptr = BUF_MEM_new();
	BIO_get_mem_ptr(pbio, &bptr); 
	
	memcpy(tbuf, bptr->data, bptr->length); 
	inl = bptr->length;
	BUF_MEM_free(bptr);
	bptr = NULL;

	pbio->ptr = NULL;
	BIO_free(pbio);
	pbio = NULL;

	if (inl > sizeofPkcs7Sign)
	{
		UnionUserErrLog("in UnionSignWithPkcs7Hsm::sizeofPkcs7Sign[%d] too small!\n",sizeofPkcs7Sign);
		ret = errCodeOffsetOfCertMDL_SmallBuffer;
		goto cleanup;
	}
	memcpy(pkcs7Sign, tbuf, inl); 

	ret = inl;
cleanup:
	if (pbio != NULL) BIO_free(pbio);
	if (pkey != NULL) EVP_PKEY_free(pkey);
	if (x509 != NULL)  X509_free(x509);
	if (caCert != NULL) X509_free(caCert);  //add
	if (p7 != NULL) PKCS7_free(p7);
	if (p7bio != NULL) BIO_free(p7bio);
	if (fp != NULL) fclose(fp);
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
*/
/*
功能：用软算法进行数字签名
输入：
	vk：私钥,DER格式的扩展字符串 
	hashID：摘要算法0-md5,1-sha1
	data: 待签名的数据
	len: 数据长度
输出：
	sign: 签名
返回：
	>= 0 成功
	< 0 失败
*/
/*
int UnionSignWithSoft(char *vk,int hashID,unsigned char *data,int len,char *sign)
{
	const EVP_MD      *md = NULL;
	unsigned char tbuf[1024];
	unsigned char *p = NULL;
	unsigned char mdout[50];
	unsigned int mdlen = 0;
	unsigned char encTxt[4096];
	int ret = 0;
	int shalen = 0;
	X509_SIG sig;
	ASN1_TYPE parameter;
	X509_ALGOR algor;
	ASN1_OCTET_STRING digest;
	 
	UnionLoadLoadOpenSSLAlg();
	if (hashID == 0)
		md = EVP_md5();
	 else 
		md = EVP_sha1();
	
	 memset(mdout,0,sizeof(mdout));
	 EVP_Digest(data,len,(unsigned char *)mdout,(unsigned int *)&mdlen,md, NULL);

	 sig.algor= &algor;
	 sig.algor->algorithm=OBJ_nid2obj(md->type);
	 
	 parameter.type=V_ASN1_NULL;
	 parameter.value.ptr=NULL;
	 sig.algor->parameter= &parameter;
		 
	 sig.digest= &digest;
	 sig.digest->data=(unsigned char *)mdout; 
	 sig.digest->length=mdlen;
	
	 memset(tbuf,0,sizeof(tbuf));
	 p = tbuf;
	 shalen=i2d_X509_SIG(&sig,&p);

	 memset(encTxt,0,sizeof(encTxt));
	 if ((ret = UnionVKEncByPKCS1(tbuf,shalen,vk,encTxt)) < 0)
	 {
		 return errCodeOffsetOfCertMDL_Arithmetic;
	 }
	 bcdhex_to_aschex((char *)encTxt,ret,sign);
	 sign[ret*2] = '\0';
	 return ret*2;
}
*/
/*
功能:用软算法验证签名
输入：
vk：公钥,DER格式的扩展字符串 
	hashID：摘要算法0-md5,1-sha1
	data: 待签名的数据
	len: 数据长度
	sign: 签名

返回：
	=1 成功
	=0 验证失败
	<0 出错
*/
/*
int UnionVerifySignWithSoft(char *pk,int hashID,unsigned char *data,int len,char *sign)
{
	const EVP_MD      *md = NULL;
	unsigned char tbuf[1024];
	unsigned char *p = NULL;
	unsigned char mdout[50];
	unsigned int mdlen = 0;
	unsigned char encTxt[4096];
	int ret = 0;
	int shalen = 0;
	X509_SIG sig;
	ASN1_TYPE parameter;
	X509_ALGOR algor;
	ASN1_OCTET_STRING digest;
	unsigned char usignbuf[1024];
	 
	UnionLoadLoadOpenSSLAlg();
	if (hashID == 0)
		md = EVP_md5();
	 else 
		md = EVP_sha1();
	
	 memset(mdout,0,sizeof(mdout));
	 EVP_Digest(data,len,(unsigned char *)mdout,(unsigned int *)&mdlen,md, NULL);

	 sig.algor= &algor;
	 sig.algor->algorithm=OBJ_nid2obj(md->type);
	 
	 parameter.type=V_ASN1_NULL;
	 parameter.value.ptr=NULL;
	 sig.algor->parameter= &parameter;
		 
	 sig.digest= &digest;
	 sig.digest->data=(unsigned char *)mdout; 
	 sig.digest->length=mdlen;
	
	 memset(tbuf,0,sizeof(tbuf));
	 p = tbuf;
	 shalen=i2d_X509_SIG(&sig,&p);

	 memset(encTxt,0,sizeof(encTxt));

	 memset(usignbuf,0,sizeof(usignbuf));
	 aschex_to_bcdhex(sign,strlen(sign),(char *)usignbuf);
	 
	 if ((ret = UnionPKDecByPKCS1(usignbuf,strlen(sign)/2,pk,encTxt)) < 0)
	 {
		 return errCodeOffsetOfCertMDL_Arithmetic;
	 }
	 if (memcmp((char *)encTxt,tbuf,shalen) == 0)
		 return 1;
	 else 
		return 0;
}
*/

/*
功能：根据证书请求，给请求者签发证书,生成X509证书PEM格式串
输入:
	PUnionX509UserInfo: 平台用户信息
	vkindex: 0-20表示私钥索引号, 99表示外带LMK加密的VK, -1表示将采用vk明文私钥
	vk: 私钥字符串,扩展的ASCHEX. 当vkindex为0-20时，该参数无意义。当vkindex==99时，为LMK加密的密文，当vkindex==-1时，为DER格式的明文私钥
	hashID: 签名时用的摘要标志 0--MD5,1--SHA1
	CerReq: 证书请求字串
	CerReqlen: CerReq长度
	sn: 生成证书的序列号
	days：证书的有效天数
	Cert: 生成PEM格式证书字串
返回:
	>= 0 成功
	< 0 失败
*/
/*
int UnionMakeCertWithHsmSign(PUnionX509UserInfo pEsscInfo,int vkindex,char *vk,int hashID,char *CerReq,int CerReqlen,long sn,int days,char *Cert,int sizeofCert)
{
	X509_REQ *req = NULL;
	X509 *pCert=NULL;
	EVP_PKEY *pubkey = NULL;
	
	X509_NAME *pName = NULL;
	int		  san_pos = -100;
	X509_EXTENSION *tmpsan = NULL;
	STACK_OF(X509_EXTENSION) *req_exts;

	const EVP_MD *digest;

	//unsigned char *const_buf = NULL;
	char buffer[4096];
	int ret = 0;
	BIO *pbio = NULL;
	int pass;
	int len = 0;
	//unsigned char *buf = NULL;
	void *asn = NULL;
	int certinfolen = 0;
	unsigned char *buf_in=NULL;
	const ASN1_ITEM *it = NULL;
	unsigned char sign[1024];
	ASN1_BIT_STRING *signature;
	BUF_MEM *bptr = NULL;
	unsigned char   bytes[4096];
	X509_NAME_ENTRY   *entry=NULL;
	
	UnionLoadLoadOpenSSLAlg();

	pCert = X509_new();
	if(pCert == NULL ) 
	{
		UnionUserErrLog("in UnionMakeCertWithHsmSign::X509_new failed!\n");
		return errCodeOffsetOfCertMDL_OpenSSl;
	}

	if (hashID == 0)
		digest = EVP_md5();
	 else 
		digest = EVP_sha1();

	 //读请求文件
	pbio = BIO_new_mem_buf(CerReq,CerReqlen);
	if((req = PEM_read_bio_X509_REQ(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		memset(buffer,0,sizeof(buffer));
		UnionPemStrPreHandle(CerReq,buffer,"CERTIFICATE REQUEST",4096);
		pbio = BIO_new_mem_buf(buffer,strlen(buffer));
		if ((req = PEM_read_bio_X509_REQ(pbio,NULL,NULL,NULL)) == NULL)
		{
			pbio = BIO_new_mem_buf(CerReq,CerReqlen);
			req = d2i_X509_REQ_bio(pbio,NULL);
		}
	}

	if (req == NULL)
	{
		UnionUserErrLog("in UnionMakeCertWithHsmSign::PEM_read_bio_X509_REQ failed!\n");
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto makeCertExit;
	}
	BIO_free(pbio);
	pbio = NULL;

	pubkey = X509_REQ_get_pubkey(req);
	if (pubkey == NULL)
	{
		UnionUserErrLog("in UnionMakeCertWithHsmSign::X509_REQ_get_pubkey[%s] failed!\n",CerReq);
		ret = errCodeOffsetOfCertMDL_OpenSSl;
		goto makeCertExit;
	}

	pass=0;
    pass = X509_REQ_verify(req,pubkey); //验证，出现问题
    if(pass < 0)   
    {
        UnionUserErrLog("in UnionMakeCertWithHsmSign::X509_REQ_verify err!\n");
        ret = errCodeOffsetOfCertMDL_OpenSSl;
		goto makeCertExit;
    }
    if(pass == 0)
    {
        UnionUserErrLog("in UnionMakeCertWithHsmSign::X509_REQ_verify failed!\n");
        ret = errCodeOffsetOfCertMDL_VerifyCertReq;
		goto makeCertExit;
    }
	
	// 设置证书基本项目

	//设置版本号
	X509_set_version(pCert, 2);
	
	//设置序列号
	ASN1_INTEGER_set(X509_get_serialNumber(pCert),sn); 
	
	//设置起始时间
	X509_gmtime_adj(X509_get_notBefore(pCert),0);

	//设置终止时间
	X509_gmtime_adj(X509_get_notAfter(pCert), (long)60*60*24*days);

	//设置主体名(从证书请求文件取得)
	if (!X509_set_subject_name(pCert, X509_REQ_get_subject_name(req)))
	{
		ret = errCodeOffsetOfCertMDL_OpenSSl;
		goto makeCertExit;
	}
	
	//设置公钥(从证书请求文件取得)
	if (!X509_set_pubkey(pCert,pubkey))
	{
		ret = errCodeOffsetOfCertMDL_OpenSSl;
		goto makeCertExit;
	}

	//设置主体名
	pName=X509_NAME_new();
	if(pEsscInfo->commonName != NULL && strlen(pEsscInfo->commonName) != 0)
	{
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pEsscInfo->commonName,strlen(pEsscInfo->commonName),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"commonName",V_ASN1_UTF8STRING,(unsigned char *)bytes,len);
		 X509_NAME_add_entry(pName,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	}
	 
	if(pEsscInfo->email != NULL && strlen(pEsscInfo->email) != 0)
	{
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pEsscInfo->email,strlen(pEsscInfo->email),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"emailAddress",V_ASN1_UTF8STRING,(unsigned char *)bytes,len);
		 X509_NAME_add_entry(pName,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	}
	 
	if(pEsscInfo->organizationalUnitName != NULL && strlen(pEsscInfo->organizationalUnitName) != 0)
	{
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pEsscInfo->organizationalUnitName,strlen(pEsscInfo->organizationalUnitName),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"organizationalUnitName",V_ASN1_UTF8STRING,bytes,len);
		 X509_NAME_add_entry(pName,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	}
	 
	if(pEsscInfo->organizationName != NULL && strlen(pEsscInfo->organizationName) != 0)
	{
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pEsscInfo->organizationName,strlen(pEsscInfo->organizationName),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"organizationName",V_ASN1_UTF8STRING,bytes,len);
		 X509_NAME_add_entry(pName,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	}
	 
	if(pEsscInfo->localityName != NULL && strlen(pEsscInfo->localityName) != 0)
	{
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pEsscInfo->localityName,strlen(pEsscInfo->localityName),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"localityName",V_ASN1_UTF8STRING,bytes,len);
		 X509_NAME_add_entry(pName,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	}
	 
	if(pEsscInfo->stateOrProvinceName != NULL && strlen(pEsscInfo->stateOrProvinceName) != 0)
	{
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pEsscInfo->stateOrProvinceName,strlen(pEsscInfo->stateOrProvinceName),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"stateOrProvinceName",V_ASN1_UTF8STRING,bytes,len);
		 X509_NAME_add_entry(pName,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	}
	 
	if(pEsscInfo->countryName != NULL && strlen(pEsscInfo->countryName) != 0)
	{
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pEsscInfo->countryName,strlen(pEsscInfo->countryName),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"countryName",V_ASN1_UTF8STRING,bytes,len);
		 X509_NAME_add_entry(pName,entry,0,-1);
		 X509_NAME_ENTRY_free(entry);
		 entry = NULL;
	}

	X509_set_issuer_name(pCert, pName);
	X509_NAME_free(pName);

	//设置扩展项目
	
	//加入可选名 SN_subject_alt_name
	req_exts = X509_REQ_get_extensions(req);
	while(req_exts != NULL) 
	{
		san_pos = X509v3_get_ext_by_NID(req_exts,OBJ_sn2nid(SN_subject_alt_name),-1);
		tmpsan =X509v3_get_ext(req_exts,san_pos);
		break;
	}
	if(tmpsan != NULL)
	{
		if(!X509_add_ext(pCert,tmpsan,-1))
		{
			ret = errCodeOffsetOfCertMDL_OpenSSl;
			UnionUserErrLog("in UnionMakeCertWithHsmSign::X509_add_ext failed\n");
			goto makeCertExit;
		}
	}
	

	setAlgor(pCert->cert_info->signature,digest);
	setAlgor(pCert->sig_alg,digest);
	asn = pCert->cert_info;
	it = ASN1_ITEM_rptr(X509_CINF);
	certinfolen=ASN1_item_i2d((struct ASN1_VALUE_st *)asn,&buf_in, it);
	memcpy(bytes,buf_in,certinfolen);
	free(buf_in);
	memset(buffer,0,sizeof(buffer));
	if (vkindex == -1) 
	{
		if ((ret = UnionSignWithSoft(vk,hashID,bytes,certinfolen,buffer)) < 0)
			goto makeCertExit;
	}
	else {
		//if ((ret = UnionSignWithHsm(vkindex,vk,hashID,bytes,certinfolen,buffer)) < 0)
			goto makeCertExit;
	}
	memset(sign,0,sizeof(sign));
	aschex_to_bcdhex(buffer,strlen(buffer),(char *)sign);
	len = strlen(buffer)/2;
	
	 //签名数据
	 signature = pCert->signature;
	 buf_in = (unsigned char *)sign;
	 signature->data=buf_in;
	 signature->length=len;
	 signature->flags&= ~(ASN1_STRING_FLAG_BITS_LEFT|0x07);
	 signature->flags|=ASN1_STRING_FLAG_BITS_LEFT;

	// 将X509证书，保存到缓冲
	pbio = BIO_new( BIO_s_mem() ); 
	PEM_write_bio_X509(pbio,pCert);

//	bptr = BUF_MEM_new();
	BIO_get_mem_ptr(pbio, &bptr); 
	
	memset(buffer,0,sizeof(buffer));
	memcpy(buffer, bptr->data, bptr->length); 
	len = bptr->length;
	BUF_MEM_free(bptr);
	bptr = NULL;

	pbio->ptr = NULL;
	BIO_free(pbio);
	pbio = NULL;

	signature->data = NULL;

	if (len > sizeofCert)
	{
		UnionUserErrLog("in UnionMakeCertWithHsmSign::sizeofCert[%d] too small!\n",sizeofCert);
		ret = errCodeOffsetOfCertMDL_SmallBuffer;
		goto makeCertExit;
	}
	//memcpy(Cert, buffer, len); 
	UnionTrimCtrlMN(buffer,Cert);
	ret = strlen(Cert);

makeCertExit:
	if (req != NULL) X509_REQ_free(req); 
	if (pubkey != NULL) EVP_PKEY_free(pubkey);
	if (pCert != NULL) X509_free(pCert);
	if (pbio != NULL)	BIO_free(pbio);
	if (tmpsan) X509_EXTENSION_free(tmpsan);
	return ret;
}
*/

/*
功能：验证看是否为(平台)自己颁发的证书串
输入：
	esscPK：发证机构(如平台)的公钥
	cert:   要认证的证书字符串
	certlen: cert长度
返回:
	>= 0 成功
	< 0 失败
*/
int UnionVerifyCertificateWithPK(char *esscPK,char *cert,int certlen)
{
	X509 *cer = NULL;
	const unsigned char* const_buf = NULL;
	unsigned char buffer[4096];
	//int len;
	EVP_PKEY *pkey = NULL;
	int ret = 0;
	BIO *pbio = NULL;
	char caPK[2048];
	
	UnionLoadLoadOpenSSLAlg();

	if (esscPK == NULL || cert == NULL)
		return errCodeOffsetOfCertMDL_CodeParameter;

	// 读证书
	memset(buffer,0,sizeof(buffer));
	UnionPemStrPreHandle(cert,(char *)buffer,"CERTIFICATE",4096);
	pbio = BIO_new_mem_buf(buffer,strlen((char *)buffer));
	if((cer = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_mem_buf(cert,certlen);
		if ((cer = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
		{
			BIO_free(pbio);
			pbio = BIO_new_mem_buf(cert,certlen);
			cer = d2i_X509_bio(pbio,NULL);
		}
	}

	if (cer == NULL)
	{
		UnionUserErrLog("in UnionVerifyCertificateWithPK::read CertfileName failed!\n");
		return errCodeOffsetOfCertMDL_FileFormat;
	}
	BIO_free(pbio);
	pbio = NULL;

	//取CA证书的公钥
	strcpy(caPK,esscPK);

	memset(buffer,0,sizeof(buffer));
	aschex_to_bcdhex(caPK,strlen(caPK),(char *)buffer);
	const_buf = buffer;
	pkey=EVP_PKEY_new();
	d2i_PublicKey(EVP_PKEY_RSA,&pkey,&const_buf,strlen(caPK)/2);

	ret = X509_verify(cer,pkey);
	if(ret < 0)   
    {
        UnionUserErrLog("in UnionVerifyCertificateWithPK::X509_verify err!\n");
        EVP_PKEY_free(pkey);
        X509_free(cer);
        return errCodeOffsetOfCertMDL_OpenSSl;
    }
    if(ret == 0)
    {
        UnionUserErrLog("in UnionVerifyCertificateWithPK::X509_verify failed!!\n");
        EVP_PKEY_free(pkey);
        X509_free(cer);
        return errCodeOffsetOfCertMDL_VerifyCert;
    }
	X509_free(cer);
	EVP_PKEY_free(pkey);
	return 1;
}


/*
功能：验证看是否为(平台)自己颁发的证书串
输入：
	hsmFlag: 使用加密机验签标志。 1-使用加密机验证证书里的签名，0-用软算法验证证书里的签名
	esscPK：发证机构(如平台)的公钥
	cert:   要认证的证书字符串
	certlen: cert长度
返回:
	>= 0 成功
	< 0 失败
*/
/*
int UnionVerifyCertificateWithPKHsm(int hsmFlag,char *esscPK,char *cert,int certlen)
{
	X509 *cer = NULL;
	//const unsigned char* const_buf = NULL;
	unsigned char buffer[4096];
	//int len;
	int ret = 0;
	BIO *pbio = NULL;
	char caPK[2048];
	unsigned char bytes[4096];
	unsigned char *buf_in=NULL;
	const ASN1_ITEM *it = NULL;
	void *asn = NULL;
	int certinfolen = 0;
	char sign[4096];
	int type=0, hashID = 0;
	UnionLoadLoadOpenSSLAlg();
	ASN1_BIT_STRING *signature;

	if (esscPK == NULL || cert == NULL)
		return errCodeOffsetOfCertMDL_CodeParameter;

	// 读证书
	memset(buffer,0,sizeof(buffer));
	UnionPemStrPreHandle(cert,(char *)buffer,"CERTIFICATE",4096);
	pbio = BIO_new_mem_buf(buffer,strlen((char *)buffer));
	if((cer = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_mem_buf(cert,certlen);
		if ((cer = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
		{
			BIO_free(pbio);
			pbio = BIO_new_mem_buf(cert,certlen);
			cer = d2i_X509_bio(pbio,NULL);
		}
	}

	if (cer == NULL)
	{
		UnionUserErrLog("in UnionVerifyCertificateWithPKHsm::read CertfileName failed!\n");
		return errCodeOffsetOfCertMDL_FileFormat;
	}
	BIO_free(pbio);
	pbio = NULL;

	//取CA证书的公钥
	strcpy(caPK,esscPK);

	// 证书信息
	memset(bytes,0,sizeof(bytes));
	asn = cer->cert_info;
	it = ASN1_ITEM_rptr(X509_CINF);
	certinfolen=ASN1_item_i2d((struct ASN1_VALUE_st *)asn,&buf_in, it);
	memcpy(bytes,buf_in,certinfolen);
	free(buf_in);

	// 签名
	signature = cer->signature;
	memset(sign,0,sizeof(sign));
	bcdhex_to_aschex((char *)signature->data,signature->length,sign);
	
	// 摘要类型
	type = OBJ_obj2nid(cer->sig_alg->algorithm);
	
	if (type == NID_md5WithRSAEncryption)
		hashID = 0;
	else if (type == NID_sha1WithRSAEncryption)
		hashID = 1;
	else
		hashID = 2;

	if (hsmFlag == 1) //使用加密机
	{
		//ret = UnionVerifySignWithHsm(caPK, hashID, bytes, certinfolen, sign);
	}
	else  // 使用软算法
	{
		ret = UnionVerifySignWithSoft(caPK,hashID,bytes,certinfolen,sign);
	}

	if(ret < 0)   
    {
        UnionUserErrLog("in UnionVerifyCertificateWithPKHsm::UnionVerifySign ret=[%d]!\n",ret);
        X509_free(cer);
        return ret;
    }
    if(ret == 0)
    {
        UnionUserErrLog("in UnionVerifyCertificateWithPKHsm::UnionVerifySign failed!\n");
        X509_free(cer);
        return errCodeOffsetOfCertMDL_VerifySign;
    }
	X509_free(cer);
	
	return 1;
}
*/
/*
功能：从证书字符串获取证书信息属性值
输入：
	cert：证书字符串
	certlen: 证书字符串长度
输出:
	PK：证书公钥der格式的扩展字符串
返回:
	>= 0 成功
	< 0 失败
*/
int UnionGetCertificatePKFromBuf(char *cert,int certlen,char *PK,int sizeofPK)
{
	X509 *cer = NULL;
	//const unsigned char* const_buf;
	char buffer[8192];
	int len = 0;
	EVP_PKEY *PKey = NULL;
	//int ret = 0;
	BIO *pbio = NULL;
	unsigned char *buf = NULL;
	
	//int i = 0;
	//long type = 0;

	UnionLoadLoadOpenSSLAlg();
	if (cert == NULL || PK == NULL) {
		printf("in UnionGetCertificatePKFromBuf::paramters err!\n");
		return errCodeOffsetOfCertMDL_CodeParameter;
	}

	memset(buffer,0,sizeof(buffer));
	UnionPemStrPreHandle(cert,buffer,"CERTIFICATE",4096);
	pbio = BIO_new_mem_buf(buffer,strlen(buffer));
	
	if((cer = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_mem_buf(cert,certlen);
		if((cer = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
		{
			BIO_free(pbio);
			pbio = BIO_new_mem_buf(cert,certlen);
			cer = d2i_X509_bio(pbio,NULL);
		}
	}

	if (cer == NULL)
	{
		printf("in UnionVerifyCertificate::read CertfileName failed!\n");
		return errCodeOffsetOfCertMDL_FileFormat;
	}
	BIO_free(pbio);
	pbio = NULL;

	PKey = X509_get_pubkey(cer);

	len = i2d_PublicKey(PKey,&buf);
	if (len*2 > sizeofPK)
	{
		X509_free(cer);
		EVP_PKEY_free(PKey);
		free(buf);
		return errCodeOffsetOfCertMDL_SmallBuffer;
	}
	//1???
	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)buf,len,(char *)buffer);
	free(buf);
	buf = NULL;
	strcpy(PK,(char *)buffer);

	X509_free(cer);
	EVP_PKEY_free(PKey);
	return 1;
}

/*
功能：从证书字符串获取证书信息属性值
输入：
	cert：证书字符串
	certlen: 证书字符串长度
输出:
	pCertInfo：证书信息结构指针
返回:
	>= 0 成功
	< 0 失败
*/
int UnionGetCertificateInfoFromBuf(char *cert,int certlen,PUnionX509Cer pCertInfo)
{
	X509 *cer = NULL;
	//const unsigned char* const_buf;
	char buffer[8192];
	int len = 0;
	EVP_PKEY *PKey = NULL;
	int ret = 0;
	BIO *pbio = NULL;
	unsigned char *buf = NULL;
	ASN1_BIT_STRING *signature;
	X509_NAME *pName = NULL;
	PUnionX509UserInfo pUserInfo = NULL;
	int i = 0;
	long type = 0;
	unsigned char tbuf[1024];
	BIGNUM bnum;
	
	UnionLoadLoadOpenSSLAlg();
	if (cert == NULL || pCertInfo == NULL) {
		UnionUserErrLog("in UnionGetCertificateInfoFromBuf::paramters err!\n");
		return errCodeOffsetOfCertMDL_CodeParameter;
	}

	memset(buffer,0,sizeof(buffer));
	UnionPemStrPreHandle(cert,buffer,"CERTIFICATE",8192);
	pbio = BIO_new_mem_buf(buffer,strlen(buffer));
	if((cer = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_mem_buf(cert,certlen);
		if((cer = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
		{
		 	pbio = BIO_new_mem_buf(cert,certlen);
			cer = d2i_X509_bio(pbio,NULL);
		}
	}

	if (cer == NULL)
	{
		UnionUserErrLog("in UnionGetCertificateInfoFromBuf::read cert[%s] certlen=[%d] failed!\n",cert,certlen);
		return errCodeOffsetOfCertMDL_FileFormat;
	}
	BIO_free(pbio);
	pbio = NULL;

	PKey = X509_get_pubkey(cer);

	len = i2d_PublicKey(PKey,&buf);
	if (len*2 > sizeof(pCertInfo->derPK))
	{
		X509_free(cer);
		EVP_PKEY_free(PKey);
		free(buf);
		return errCodeOffsetOfCertMDL_SmallBuffer;
	}
	//公钥
	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)buf,len,(char *)buffer);
	free(buf);
	buf = NULL;
	strcpy(pCertInfo->derPK,(char *)buffer);

	if (X509_alias_get0(cer,NULL) != NULL) {
		strcpy(pCertInfo->userInfo.alias,(char *)X509_alias_get0(cer,NULL));
	}
	
	for(i=0;i<2;i++)
	{
	//issuer
	if (i == 0) {
		pName = X509_get_issuer_name(cer); 
		pUserInfo = &(pCertInfo->issuerInfo);
	}
	else if (i == 1) {  // 取用户信息
		pName = X509_get_subject_name(cer);
		pUserInfo = &(pCertInfo->userInfo);
	}
	else
		break;

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_commonName, buffer, sizeof(buffer));
    if (ret >= 1)  {
			UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)pUserInfo->commonName,sizeof(pUserInfo->commonName));
	}

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_organizationalUnitName, buffer, sizeof(buffer));
    if (ret >= 1) {
			UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)pUserInfo->organizationalUnitName,sizeof(pUserInfo->organizationalUnitName));
	}

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_organizationName, buffer, sizeof(buffer));
    if (ret >= 1)  {
			UnionUtf8ToAnsi((unsigned char *)buffer,strlen(buffer),(unsigned char *)pUserInfo->organizationName,sizeof(pUserInfo->organizationName));
	}

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_localityName, buffer, sizeof(buffer));
    if (ret >= 1)  {
			UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)pUserInfo->localityName,sizeof(pUserInfo->localityName));
	}

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_stateOrProvinceName, buffer, sizeof(buffer));
    if (ret >= 1)  {
		UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)pUserInfo->stateOrProvinceName,sizeof(pUserInfo->stateOrProvinceName));
	}

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_countryName, buffer, sizeof(buffer));
    if (ret >= 1)  {
			UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)pUserInfo->countryName,sizeof(pUserInfo->countryName));
	}

	memset(buffer,0,sizeof(buffer));
	ret = X509_NAME_get_text_by_NID(pName, NID_pkcs9_emailAddress, buffer, sizeof(buffer));
    if (ret >= 1)  {
			UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)pUserInfo->email,sizeof(pUserInfo->email));
	}
	}

	//证书起始日期
	memset(buffer,0,sizeof(buffer));
	len = ASN1_STRING_length(cer->cert_info->validity->notBefore);
	memcpy(buffer,ASN1_STRING_data(cer->cert_info->validity->notBefore),len);
	UnionConvertCrtTimeZtoBeiJin((char *)buffer,pCertInfo->startDateTime);

	//证书终止日期
	memset(buffer,0,sizeof(buffer));
	len = ASN1_STRING_length(cer->cert_info->validity->notAfter);
	memcpy(buffer,ASN1_STRING_data(cer->cert_info->validity->notAfter),len);
	UnionConvertCrtTimeZtoBeiJin((char *)buffer,pCertInfo->endDataTime);

	//版本号
	pCertInfo->version = ASN1_INTEGER_get(cer->cert_info->version);
	//序列号
	pCertInfo->serialNumber = ASN1_INTEGER_get(cer->cert_info->serialNumber);
	
	//add by hzh in 2010.6.2 序列号可能为一个大整数
	BN_init(&bnum);
	ASN1_INTEGER_to_BN(cer->cert_info->serialNumber,&bnum);
	memset(tbuf,0,sizeof(tbuf));
	len = BN_bn2bin(&bnum,tbuf);
	BN_clear_free(&bnum);
	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)tbuf,len,buffer);
	strcpy(pCertInfo->serialNumberAscBuf,buffer);
	//end
	//摘要算法标记
	type = OBJ_obj2nid(cer->sig_alg->algorithm);
	
	if (type == NID_md5WithRSAEncryption)
		pCertInfo->hashID = 0;
	else if (type == NID_sha1WithRSAEncryption)
		pCertInfo->hashID = 1;
	else
		pCertInfo->hashID = 2;
	// 签名
	signature = cer->signature;
	buf = signature->data;
	len = signature->length;
	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)buf,len,(char *)buffer);
	strcpy(pCertInfo->sign,(char *)buffer);
	X509_free(cer);
	EVP_PKEY_free(PKey);
	return 1;
}

/*
函数功能: 解析KCS#7格式的签名
输入: 
	pkcs7Sign: pkcs7格式签名字串
	pkcs7Signlen: pkcs7Sign长度
输出:
	cert: 签名证书(x509)pem格式字符串
	certlen: 证书长度
	data: 原始数据
	datalen: 数据长度
	sign: 签名字串
	signlen: sign长度
返回：
	>=0  解析KCS#7格式的签名成功
	<0  失败
*/
int UnionParseSignWithPkcs7(unsigned char *pkcs7Sign,int pkcs7Signlen,unsigned char *cert,int *certlen,unsigned char *data,int *datalen,unsigned char *sign,int *signlen)
{
	BIO *pbio = NULL;
	PKCS7* p7 = NULL;
	//PKCS7_SIGNER_INFO* info = NULL;
	//int i = 0;
	BIO *p7bio = NULL;
	X509 *x509=NULL;
	int ret = 0;
	//char tbuf[CERT_MAX_LEN+1]; 
	//unsigned char *const_buf = NULL;
	int inl = 0;
	char buffer[4096];
	BUF_MEM *bptr = NULL;
	STACK_OF(PKCS7_SIGNER_INFO) *sk = NULL;
	int signCount = 0;
	PKCS7_SIGNER_INFO *signInfo = NULL;
	ASN1_OCTET_STRING	*asndata = NULL;
	ASN1_OCTET_STRING	*encDigest = NULL;
	X509_ALGOR *xa = NULL;
	STACK_OF(X509_ALGOR) *md_sk=NULL;
	const EVP_MD *evp_md = NULL;
	BIO *btmp = NULL;
	//BIO *btmp1 = NULL;
	int j = 0;

	if (pkcs7Sign == NULL || certlen == NULL  || datalen == NULL || signlen == NULL )
	{
		UnionUserErrLog("in UnionParseSignWithPkcs7::params err!\n");
		return errCodeOffsetOfCertMDL_CodeParameter;
	}
	UnionLoadLoadOpenSSLAlg();  

	// 读pkcs7
	pbio = BIO_new_mem_buf(pkcs7Sign,pkcs7Signlen);
	if((p7 = PEM_read_bio_PKCS7(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_mem_buf(pkcs7Sign,pkcs7Signlen);
		p7 =  d2i_PKCS7_bio(pbio,NULL);
	}

	if (p7 == NULL)
	{
		UnionUserErrLog("in UnionParseSignWithPkcs7::get p7 failed!\n");
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}
	BIO_free(pbio);
	pbio = NULL;

//	p7bio= PKCS7_dataDecode(p7,NULL,NULL,NULL);  // 用这个函数有内存泄露

	//读取原始数据
	md_sk=p7->d.sign->md_algs;
	xa=sk_X509_ALGOR_value(md_sk,0);

	j=OBJ_obj2nid(xa->algorithm);
	evp_md=EVP_get_digestbynid(j);

	btmp=BIO_new(BIO_f_md());
	BIO_set_md(btmp,evp_md);
	p7bio = btmp;

	asndata = p7->d.sign->contents->d.data;
	memcpy(data,asndata->data,asndata->length);
	*datalen = asndata->length;
	btmp = BIO_new_mem_buf(asndata->data,asndata->length);
	BIO_push(p7bio,btmp);

	memset(buffer,0,sizeof(buffer));
	inl = BIO_read(p7bio,buffer,sizeof(buffer));  //BIO_read不能省，否则PKCS7_signatureVerify过不去

	BIO_free(btmp);
	btmp = NULL;

	//获得签名者信息stack
	sk = PKCS7_get_signer_info(p7);
	//获得签名者个数(本例只有1个)
	signCount = sk_PKCS7_SIGNER_INFO_num(sk);
	if (signCount != 1) {
		UnionUserErrLog("signCount=[%d]\n",signCount);
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}

	//获得签名者信息
	signInfo = sk_PKCS7_SIGNER_INFO_value(sk,0);
	encDigest = signInfo->enc_digest;
	//memcpy(sign,encDigest->data,encDigest->length);
	//*signlen = encDigest->length;
	bcdhex_to_aschex((char*)encDigest->data,encDigest->length,(char*)sign);
	*signlen = encDigest->length * 2;
	sign[encDigest->length * 2] = '\0';
	
	//获得签名者证书
	x509= PKCS7_cert_from_signer_info(p7,signInfo);

	// 将X509证书，保存到缓冲
	pbio = BIO_new( BIO_s_mem() ); 
	PEM_write_bio_X509(pbio,x509);
	BIO_get_mem_ptr(pbio, &bptr); 
	
	memset(buffer,0,sizeof(buffer));
	memcpy(buffer, bptr->data, bptr->length); 
	inl = bptr->length;
	BUF_MEM_free(bptr);
	bptr = NULL;

	pbio->ptr = NULL;
	BIO_free(pbio);
	pbio = NULL;

	memcpy(cert, buffer, inl); 
	*certlen = inl;

	//验证签名
	if(PKCS7_signatureVerify(p7bio,p7,signInfo,x509) != 1)
	{
		UnionUserErrLog("signatureVerify Err\n");
		ret = errCodeOffsetOfCertMDL_VerifySign;
	}
	else
		ret = 1;
cleanup:
	if (pbio != NULL) BIO_free(pbio);
	if (p7 != NULL) PKCS7_free(p7);
	if (p7bio != NULL) BIO_free(p7bio);
	
	return ret;

}




/*
函数功能: 解析KCS#7格式的签名
输入:
	pkcs7Sign: KCS#7格式字符串
	pkcs7Signlen: pkcs7Sign的长度
输出:
	cert: pem格式的签名者证书
	sign: 签名
	data: 原始数据
	datalen: 原始数据长度,注意一些pkcs7的签名文件里不含原始签名数据，此时的datalen为0
	hashID: 签名时用的摘要算法
返回:
	>= 0 解析KCS#7格式的签名成功
	< 0 失败
*/
int UnionParaseJHSignWithPkcs7(unsigned char *pkcs7Sign,int pkcs7Signlen,char *cert,char *sign,unsigned char *data,int *datalen,int *hashID)
{
	BIO *pbio = NULL;
	PKCS7* p7 = NULL;
	BIO *p7bio = NULL;
	X509 *x509=NULL;
	int ret = 0;
	int inl = 0;
	char *buffer = NULL;
	BUF_MEM *bptr = NULL;
	STACK_OF(PKCS7_SIGNER_INFO) *sk = NULL;
	int signCount = 0;
	PKCS7_SIGNER_INFO *signInfo = NULL;
	ASN1_STRING	*asndata = NULL;
	ASN1_OCTET_STRING	*encDigest = NULL;
	X509_ALGOR *xa = NULL;
	STACK_OF(X509_ALGOR) *md_sk=NULL;
	int buflen=0;
	int j = 0;

	if (pkcs7Sign == NULL || cert == NULL || sign == NULL || hashID == NULL || datalen == NULL)
	{
		UnionUserErrLog("in UnionParaseJHSignWithPkcs7::params err!\n");
		return errCodeOffsetOfCertMDL_CodeParameter;
	}
	UnionLoadLoadOpenSSLAlg();  
	
	// 读pkcs7
	buflen=strlen((char*)pkcs7Sign)*2+1;
	buffer =malloc(sizeof(char)*buflen);
	if(!buffer)
	{
		UnionUserErrLog("in UnionParaseJHSignWithPkcs7::malloc failed!\n");
		return -1;
	}
	memset(buffer,0,buflen);

	int myret=0;
	myret=UnionPemStrPreHandle((char *)pkcs7Sign,buffer,"PKCS7",buflen);
	UnionLog("in UnionPemStrPreHandle::ret=%d\n",myret);
	pbio = BIO_new_mem_buf(buffer,strlen(buffer));
	
	if((p7 = PEM_read_bio_PKCS7(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_mem_buf(pkcs7Sign,pkcs7Signlen);
		if((p7 = PEM_read_bio_PKCS7(pbio,NULL,NULL,NULL)) == NULL)
		{
			BIO_free(pbio);
			pbio = BIO_new_mem_buf(pkcs7Sign,pkcs7Signlen);
			p7 =  d2i_PKCS7_bio(pbio,NULL);
		}
	}

	if (p7 == NULL)
	{
		UnionUserErrLog("in UnionParaseJHSignWithPkcs7::get p7 failed!\n");
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}
	BIO_free(pbio);
	pbio = NULL;

	 UnionLog("in UnionPemStrPreHandle1::ret=%d\n",myret);

	//读取原始数据
	asndata = p7->d.sign->contents->d.data;

	if (asndata != 0)
	{
		if (data != NULL)
		{
			memcpy(data,asndata->data,asndata->length);
			*datalen = asndata->length;
		}
	}
	 UnionLog("in UnionPemStrPreHandle 1.1::ret=%d\n",myret);

	md_sk=p7->d.sign->md_algs;
	xa=sk_X509_ALGOR_value(md_sk,0);

	j=OBJ_obj2nid(xa->algorithm);

	if (j == NID_md5)
		*hashID = 0;
	else if (j == NID_sha1)
		*hashID = 1;
	else
		*hashID = 2;
	 UnionLog("in UnionPemStrPreHandle2::ret=%d\n",myret);

	//获得签名者信息stack
	sk = PKCS7_get_signer_info(p7);
	//获得签名者个数(本例只有1个)
	signCount = sk_PKCS7_SIGNER_INFO_num(sk);
	if (signCount != 1) {
		UnionUserErrLog("in UnionParaseJHSignWithPkcs7::signCount=[%d]\n",signCount);
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}

	//获得签名者信息
	signInfo = sk_PKCS7_SIGNER_INFO_value(sk,0);
	encDigest = signInfo->enc_digest;
	bcdhex_to_aschex((char *)encDigest->data,encDigest->length,sign);
	sign[encDigest->length * 2] = '\0';
	
	//获得签名者证书
	x509= PKCS7_cert_from_signer_info(p7,signInfo);

	// 将X509证书，保存到缓冲
	pbio = BIO_new( BIO_s_mem() ); 
	PEM_write_bio_X509(pbio,x509);
	BIO_get_mem_ptr(pbio, &bptr); 
	 UnionLog("in UnionPemStrPreHandle3::ret=%d\n",myret);

	memset(buffer,0,buflen);
	memcpy(buffer, bptr->data, bptr->length); 
	inl = bptr->length;
	BUF_MEM_free(bptr);
	bptr = NULL;

	pbio->ptr = NULL;
	BIO_free(pbio);
	pbio = NULL;

	memcpy(cert, buffer, inl); 
	cert[inl] = '\0';

	ret = 1;

cleanup:
	if(buffer != NULL)
	{
		free(buffer);
		buffer = NULL;
	}
	if (pbio != NULL) BIO_free(pbio);
	if (p7 != NULL) PKCS7_free(p7);
	if (p7bio != NULL) BIO_free(p7bio);

	return ret;

}

/*
函数功能: 生成PKCS#7格式的签名
输入参数：
	vk:为LMK加密的密文，�
	dataLen: 数据长度
	NIDDataFlag: PKCS#7中是否要含签名数据的标志。0，不包含， 1--含签名数据
	bankCertBuf: PEM格式公钥证书 
	caCertBuf:PEM格式CA根证书,
输出参数：
	pkcs7Sign: pkcs#7格式的签名
返回：>0 成功， 签名的长度
	  < 0 失败
*/

int UnionJHSignWithPkcs7Hsm(int vkIndex,char *vk,unsigned char *data,int dataLen,int NIDDataFlag,char *bankCertBuf,char *caCertBuf, char *pkcs7Sign,int sizeofPkcs7Sign)
{
	BIO *pbio = NULL;
	PKCS7* p7 = NULL;
	PKCS7_SIGNER_INFO* info = NULL;
	BIO *p7bio = NULL;
	STACK_OF(X509) *ca = NULL; 
	X509 *x509=NULL;
	X509 *caCert = NULL;
	EVP_PKEY *pkey = NULL;
	int ret = 0;
	FILE *fp = NULL;
	//char tbuf[CERT_MAX_LEN+1]; 
	char *tbuf = NULL; 
	//unsigned char *const_buf = NULL;
	int inl = 0;
	char buf[4096];
	unsigned char sign[2048];
	BUF_MEM *bptr = NULL;
	int hashID = 1;
	int	i = 0;
	STACK_OF(PKCS7_SIGNER_INFO) *si_sk=NULL;

	if ( bankCertBuf == NULL ||  pkcs7Sign == NULL || data == NULL || dataLen <= 0)
	{
		UnionUserErrLog("in UnionJHSignWithPkcs7Hsm::params err!\n");
		return errCodeOffsetOfCertMDL_CodeParameter;
	}
	UnionLoadLoadOpenSSLAlg();  
	
	//将证书文件转x509	
	pbio = BIO_new_mem_buf(bankCertBuf,strlen(bankCertBuf));
	size_t nSize = BIO_ctrl_pending(pbio);
	//UnionUserErrLog("in UnionJHSignWithPkcs7Hsm::bankCertBuf len [%d]!\n",nSize);
	if (pbio == NULL)
	{
		return errCodeOffsetOfCertMDL_ReadFile;
	}
	if((x509 = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_mem_buf(bankCertBuf,strlen(bankCertBuf));
		x509 = d2i_X509_bio(pbio,NULL);
	}
	
	BIO_free(pbio);
	pbio = NULL;
	if (x509 == NULL)
	{
		UnionUserErrLog("in UnionJHSignWithPkcs7Hsm::bankCert err!\n");
		return errCodeOffsetOfCertMDL_FileFormat;
	}

	pkey = X509_get_pubkey(x509);

	tbuf = malloc(sizeof(char)*(1024*10+1));
	if(!tbuf)
	{
			UnionUserErrLog("in UnionJHSignWithPkcs7Hsm::malloc failed\n");
			ret = -1;
			goto cleanup;
	}
	
	if ((ca=sk_X509_new_null()) == NULL)
        {
                        UnionUserErrLog("in UnionJHSignWithPkcs7Hsm::open sk_X509_new_null failure\n");
                        ret = errCodeOffsetOfCertMDL_OpenSSl;
                        goto cleanup;
        }
	
	if (caCertBuf != NULL)
	{
		pbio = BIO_new_mem_buf(caCertBuf,CERT_MAX_LEN);
		if((caCert = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
		{
			BIO_free(pbio);
			pbio = BIO_new_mem_buf(tbuf,CERT_MAX_LEN);
			caCert = d2i_X509_bio(pbio,NULL);
		}

		if (caCert == NULL)
		{
			UnionUserErrLog("in UnionJHSignWithPkcs7Hsm::get caCert failed!\n");
			ret = errCodeOffsetOfCertMDL_FileFormat;
			goto cleanup;
		}
		if (!sk_X509_push(ca,caCert))
                {
                        UnionUserErrLog("in UnionJHSignWithPkcs7Hsm::sk_X509_push failed!\n");
                        ret = errCodeOffsetOfCertMDL_OpenSSl;
                        goto cleanup;
                }

		BIO_free(pbio);
		pbio = NULL;
		caCert = NULL;
	}

	p7 = PKCS7_new();
	PKCS7_set_type(p7, NID_pkcs7_signed); //设置类型为NID_pkcs7_signed

	if (NIDDataFlag == 1)
	{
		PKCS7_content_new(p7, NID_pkcs7_data);
		PKCS7_set_detached(p7, 0);
	}
	else
	{
		PKCS7_content_new(p7, NID_pkcs7_data);
		PKCS7_set_detached(p7, 1);
	}
	//添加签名者信息，
	//x509：签名证书，pkey：签名者私钥。EVP_sha1()签名者摘要算法。
	info = PKCS7_add_signature(p7, x509, pkey, EVP_sha1());
	//添加签名者证书
	PKCS7_add_certificate(p7, x509);

	// begin  2014-01-08  leipp 添加:对ca数量进行判断
	//添加签名者的CA证书链
	for (i = 0; i < sk_X509_num(ca); i++)	
	{
		PKCS7_add_certificate(p7,  sk_X509_value(ca, 0));
	}
	// end

	if (NIDDataFlag == 1)
	{
		if (!ASN1_STRING_set(p7->d.sign->contents->d.data,data, dataLen))
		{
			ret = errCodeOffsetOfCertMDL_OpenSSl;
			goto cleanup;
		}
	}
	
	memset(buf,0,sizeof(buf));
	memset(sign,0,sizeof(sign));
	if ((ret = UnionSignWithHsm(vkIndex,vk,hashID,data,dataLen,buf)) < 0)
	{	
		goto cleanup;
	}
	aschex_to_bcdhex(buf,strlen(buf),(char *)sign);
	si_sk = p7->d.sign->signer_info;
	info=sk_PKCS7_SIGNER_INFO_value(si_sk,0);
	if (!ASN1_STRING_set(info->enc_digest,sign,strlen(buf)/2))
	{
		ret = errCodeOffsetOfCertMDL_OpenSSl;
		goto cleanup;
	}
	pbio = BIO_new( BIO_s_mem() ); 
	PEM_write_bio_PKCS7(pbio,p7);
	BIO_get_mem_ptr(pbio, &bptr); 
	
	memset(tbuf,0,1024*10+1);
	memcpy(tbuf, bptr->data, bptr->length); 
	inl = bptr->length;

	BUF_MEM_free(bptr);
	bptr = NULL;

	pbio->ptr = NULL;
	BIO_free(pbio);
	pbio = NULL;

	if (inl > sizeofPkcs7Sign)
	{
		UnionUserErrLog("in UnionJHSignWithPkcs7Hsm::sizeofPkcs7Sign[%d] too small!\n",sizeofPkcs7Sign);
		ret = errCodeOffsetOfCertMDL_SmallBuffer;
		goto cleanup;
	}
	UnionDebugLog("in UnionJHSignWithPkcs7Hsm::pkcs7Sign=[%s]!\n", tbuf);
	
	UnionTrimPemHeadTail(tbuf,pkcs7Sign,"PKCS7");

	ret = strlen(pkcs7Sign);
cleanup:
	if(tbuf != NULL)
	{
		free(tbuf);
		tbuf=NULL;
	}
	if (pbio != NULL) BIO_free(pbio);
	if (pkey != NULL) EVP_PKEY_free(pkey);
	if (x509 != NULL)  X509_free(x509);
	if (caCert != NULL) X509_free(caCert);  //add
	if (p7 != NULL) PKCS7_free(p7);
	if (p7bio != NULL) BIO_free(p7bio);
	if (fp != NULL) fclose(fp);
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
函数功能: 初始化pb7格式的证书链文件
输入: 
	pb7FileName: pb7格式证书链文件名
返回：
	>=0  成功
	<0  失败
*/
int UnionInitPb7File(char *pb7FileName)
{
	int ret = 0;
	BIO *pbio = NULL;
	//BIO *bioCa = NULL;
	PKCS7* p7 = NULL;
	
	if (pb7FileName == NULL)
	 {
		 UnionUserErrLog("in UnionInitPb7File:: paramters err!\n");
		 return errCodeOffsetOfCertMDL_CodeParameter;
	 }
	UnionLoadLoadOpenSSLAlg();

	p7 = PKCS7_new();
	PKCS7_set_type(p7, NID_pkcs7_signed); //设置类型为NID_pkcs7_signed

	pbio =  BIO_new_file(pb7FileName,"wb");
	if (pbio == NULL)
	{
		UnionUserErrLog("in UnionInitPb7File::BIO_new_file[%s] wb failed!\n",pb7FileName);
		ret = errCodeOffsetOfCertMDL_WriteFile;
		goto cleanup;
	}
	if ((ret = i2d_PKCS7_bio(pbio,p7)) <= 0)
	{
		printf("in UnionInitPb7File::BIO_new_file[%s] i2d_PKCS7_bio failed!\n",pb7FileName);
		ret = errCodeOffsetOfCertMDL_WriteFile;
		goto cleanup;
	}
	BIO_free(pbio);
	pbio = NULL;

	ret = 0;

cleanup:
	if (pbio != NULL) BIO_free(pbio);
	if (p7 != NULL) PKCS7_free(p7);
	return ret;
}

/*
函数功能: 将X509证书导入到pb7格式的证书链文件
输入: 
	caCertFileName：X509证书文件名
	pb7FileName: pb7格式证书链文件名
返回：
	>=0  成功
	<0  失败
*/
int UnionImportCACert2Pb7File(char *caCertFileName,char *pb7FileName)
{
	X509 *cer = NULL;
	int ret = 0;
	BIO *pbio = NULL;
	//BIO *bioCa = NULL;
	PKCS7* p7 = NULL;

	
	UnionLoadLoadOpenSSLAlg();

	if (pb7FileName == NULL || caCertFileName == NULL)
	{
		 UnionUserErrLog("in UnionImportCACert2Pb7File:: paramters err!\n");
		 return errCodeOffsetOfCertMDL_CodeParameter;
	}

	// 读证书
	pbio = BIO_new_file(caCertFileName,"rb");
	if (pbio == NULL)
	{
		UnionUserErrLog("in UnionImportCACert2Pb7File::read caCertFileName[%s] failed!\n",caCertFileName);
		return errCodeOffsetOfCertMDL_ReadFile;
	}
	if((cer = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_file(caCertFileName,"rb");
		cer = d2i_X509_bio(pbio,NULL);
	}
	
	BIO_free(pbio);
	pbio = NULL;
	if (cer == NULL)
	{
		UnionUserErrLog("in UnionImportCACert2Pb7File::read caCertFileName[%s] format err!\n",caCertFileName);
		return errCodeOffsetOfCertMDL_FileFormat;
	}

	pbio = BIO_new_file(pb7FileName,"rb");
	if (pbio == NULL)
	{
		UnionUserErrLog("in UnionImportCACert2Pb7File::read pb7FileName[%s] failed!\n",pb7FileName);
		return errCodeOffsetOfCertMDL_ReadFile;
	}
	// 读pkcs7
	if((p7 = d2i_PKCS7_bio(pbio,NULL)) == NULL)
	{
		BIO_free(pbio);
		p7 = PEM_read_bio_PKCS7(pbio,NULL,NULL,NULL);
	}

	if (p7 == NULL)
	{
		printf("in UnionImportCACert2Pb7File::get p7 failed!\n");
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}
	BIO_free(pbio);
	pbio = NULL;

	PKCS7_add_certificate(p7, cer);


	pbio =  BIO_new_file(pb7FileName,"wb");
	if (pbio == NULL)
	{
		printf("in UnionParsePkcs7CALinks::BIO_new_file[%s] wb failed!\n",pb7FileName);
		ret = errCodeOffsetOfCertMDL_WriteFile;
		goto cleanup;
	}
	if ((ret = i2d_PKCS7_bio(pbio,p7)) <= 0)
	{
		printf("in UnionParsePkcs7CALinks::BIO_new_file[%s] i2d_PKCS7_bio failed!\n",pb7FileName);
		ret = errCodeOffsetOfCertMDL_WriteFile;
		goto cleanup;
	}
	BIO_free(pbio);
	pbio = NULL;


cleanup:
	if (cer != NULL) X509_free(cer);
	if (pbio != NULL) BIO_free(pbio);
	if (p7 != NULL) PKCS7_free(p7);
	
	return ret;

}

/*
函数功能: 用pb7证书链文件校验X509证书的合法性
输入: 
	CertfileName: X509证书文件
	pb7FileName: pb7格式证书链文件名
返回：
	>=0  成功
	<0  失败
*/
int UnionVerifyCertByP7bCAlinks(char *CertfileName,char *p7bFileName)
{
	X509 *cer = NULL;
	int ret = 0;
	BIO *pbio = NULL;
	STACK_OF(X509) *caChain = NULL; 
	//BIO *bioCa = NULL;
	PKCS7* p7 = NULL;
	X509_STORE *store=NULL; 	
	X509_STORE_CTX ctx; 	
	
	UnionLoadLoadOpenSSLAlg();

	if (CertfileName == NULL || p7bFileName == NULL)
	{
		 UnionUserErrLog("in UnionVerifyCertByP7bCAlinks:: paramters err!\n");
		 return errCodeOffsetOfCertMDL_CodeParameter;
	}

	// 读证书
	pbio = BIO_new_file(CertfileName,"rb");
	if (pbio == NULL)
	{
		UnionUserErrLog("in UnionVerifyCertByP7bCAlinks:: read CertfileName[%s] err!\n",CertfileName);
		return errCodeOffsetOfCertMDL_ReadFile;
	}
	if((cer = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_file(CertfileName,"rb");
		cer = d2i_X509_bio(pbio,NULL);
	}
	
	BIO_free(pbio);
	pbio = NULL;
	if (cer == NULL)
	{
		UnionUserErrLog("in UnionVerifyCertificate::CertfileName[%s] format err!\n",CertfileName);
		return errCodeOffsetOfCertMDL_FileFormat;
	}

	pbio = BIO_new_file(p7bFileName,"rb");
	if (pbio == NULL)
	{
		UnionUserErrLog("in UnionVerifyCertificate::BIO_new_file[%s] rb failed!\n",p7bFileName);
		ret = errCodeOffsetOfCertMDL_WriteFile;
		goto cleanup;
	}
	// 读pkcs7
	if((p7 = d2i_PKCS7_bio(pbio,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_file(p7bFileName,"rb");
		p7 = PEM_read_bio_PKCS7(pbio,NULL,NULL,NULL);
	}

	if (p7 == NULL)
	{
		UnionUserErrLog("in UnionVerifyCertificate:: p7 format err!\n");
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}
	BIO_free(pbio);
	pbio = NULL;

	if (p7->d.sign->cert == NULL)
	{
		UnionUserErrLog("in UnionParsePkcs7CALinks::cert is null!\n");
		ret = errCodeOffsetOfCertMDL_CodeParameter;
		goto cleanup;
	}

	caChain = p7->d.sign->cert;

	//创建X509_store对象，用来存储证书、撤销列表等 	
	store=X509_STORE_new();  	
  	
	//设置验证标记 都验证那些项 X509_V_FLAG_CRL_CHECK_ALL表示全部验证 	
	X509_STORE_set_flags(store,X509_V_FLAG_CRL_CHECK_ALL); 	
	//初始化CTX 这个类就是所谓的上下文 该类收集完必要的信息数据 可以进行验证 	
	// 此处X509_STORE_CTX_init最后一个参数为NULL，表示不加载证书撤销列表CPL 	
	if(!X509_STORE_CTX_init(&ctx,store ,cer,NULL)) 	
	{ 		
		ret = errCodeOffsetOfCertMDL_OpenSSl; 		
		goto cleanup; 	
	}  	
  	
	//将证书链存入CTX 		
	X509_STORE_CTX_trusted_stack(&ctx, caChain); 	
  
	//证书链式验证 	
	ret = X509_verify_cert(&ctx);
  
	if (ret != 1)
	{
		if(ret == 0) 	
		{
			UnionUserErrLog("in UnionParsePkcs7CALinks::X509_verify_cert failed!error msg [%s]\n", X509_verify_cert_error_string(ctx.error));
			printf("in UnionParsePkcs7CALinks::X509_verify_cert failed!error msg [%s]\n", X509_verify_cert_error_string(ctx.error));
			ret = errCodeOffsetOfCertMDL_VerifyCert; 
		} 
		else
		{		
			UnionUserErrLog("in UnionParsePkcs7CALinks::X509_verify_cert err!\n");
			ret = errCodeOffsetOfCertMDL_OpenSSl; 
		}
	}
	X509_STORE_CTX_cleanup(&ctx); 
cleanup:
	if (cer) X509_free(cer);
	if (pbio != NULL) BIO_free(pbio);
	if (p7 != NULL) PKCS7_free(p7);
	
		
	if(store) X509_STORE_free(store); 	
	
	return ret;
}


int UnionStoreP7File(char *data,int datalen,char *fileName)
{
	FILE *fp;
	
	if (data == NULL)
	{
		UnionLog("store pkcs#7 data is null \n");
		return -1;
	}
		
	if ( (fp = fopen(fileName,"wb")) == NULL )
	{
		UnionLog("store pkcs#7 fopen  error! \n");
		return -1;
	}	
	
	fwrite(data, 1, datalen,fp);
	fflush(fp);
	fclose(fp);
	return 0;
}


/*
功能: 检查证书是否在撤消列表中
输入：
	tCRLFileName： 证书撤消列表文件名
	tCertFileName: 被检查的证书名
返回:
	<0 出错 
	=0,不在黑名单 
	=1,在黑名单 
*/
/*
int UnionCertCRLCheckUseID(char *serialNumber,char *tCRLFileName) 
{ 
	int i; 
	int iret=0; 
	BIO *pbio = NULL;
	X509_CRL *x509_crl=NULL; 
	STACK_OF(X509_REVOKED) *revoked = NULL;
	char 	v_crlSerial[100+1];
	
	unsigned char tbuf[1024];
	char 	buffer[1024+1];
	BIGNUM bnum;
	int 	len;
	int num = 0;
	X509_REVOKED *rc = NULL;
	//ASN1_INTEGER *serial = NULL;

	UnionLoadLoadOpenSSLAlg();
	IS_FILE_EXIST(tCRLFileName);
	//读取CRL文件

	pbio = BIO_new_file(tCRLFileName,"rb");
	x509_crl = d2i_X509_CRL_bio(pbio,NULL); //默认先读取der编码的crl文件
    	if(x509_crl == NULL)
    	{
        	BIO_free(pbio); 
        	pbio = BIO_new_file(tCRLFileName,"rb"); 
        	x509_crl = PEM_read_bio_X509_CRL(pbio, NULL, NULL, NULL); //BASE64编码的crl文件
        	if(x509_crl == NULL)
		{
			iret = errCodeOffsetOfCertMDL_ReadFile;
            		goto cleanup;
		}
   	}

	BIO_free(pbio);
	pbio = NULL;

	revoked = x509_crl->crl->revoked; 
	num=revoked->num; 
	for(i=0;i<num;i++)
	{
		rc=sk_X509_REVOKED_pop(revoked); 
		if (rc==NULL)
			break;
		memset(v_crlSerial,0,sizeof(v_crlSerial));
		BN_init(&bnum);
		ASN1_INTEGER_to_BN(rc->serialNumber,&bnum);
		memset(tbuf,0,sizeof(tbuf));
		len = BN_bn2bin(&bnum,tbuf);
		BN_clear_free(&bnum);
		memset(buffer,0,sizeof(buffer));
		bcdhex_to_aschex((char *)tbuf,len,buffer);
		strcpy(v_crlSerial,buffer);
		X509_REVOKED_free(rc);
		rc = NULL;
		if (strcmp(v_crlSerial,serialNumber) == 0)
		//if(ASN1_INTEGER_cmp(serial,rc->serialNumber)==0)
		{ 
			iret=1; 
			break; 
		}
	} 


	if (x509_crl != NULL) X509_CRL_free(x509_crl); 
cleanup:
	if (pbio != NULL) BIO_free(pbio);

	return(iret); 
}
*/
/*
功能:用证书链校验证书撤销列表的合法性
	输入：
		p7bFileName:证书链文件名
		tCRLFileName:证书撤销列表的文件名
	返回：
		>=0 成功
		<0  失败
*/
int UnionVerifyCRL(char *p7bFileName,char *tCRLFileName)
{
	X509_CRL 	*x509_crl = NULL;
	//const unsigned char* const_buf = NULL;
	//unsigned char buffer[4096];
	STACK_OF(X509) *caChain = NULL; 
	PKCS7* p7 = NULL;
	//int len;
	EVP_PKEY *pkey = NULL;
	int ret = 0;
	BIO *pbio = NULL;
	//char caPK[2048];
	
	X509_NAME *pownerName = NULL;
	X509_NAME *pcrlName = NULL;
	X509 *x5 = NULL;
	int findSignerFlag = 0;
	//unsigned char *buf = NULL;
	
	UnionLoadLoadOpenSSLAlg();
	IS_FILE_EXIST(p7bFileName);
	IS_FILE_EXIST(tCRLFileName);
	
	if (p7bFileName == NULL || tCRLFileName == NULL)
		return errCodeOffsetOfCertMDL_CodeParameter;

	/*读取CRL文件*/ 
	pbio = BIO_new_file(tCRLFileName,"rb");
	x509_crl = d2i_X509_CRL_bio(pbio,NULL); //默认先读取der编码的crl文件
    	if(x509_crl == NULL)
	{
		BIO_free(pbio);  
		pbio = BIO_new_file(tCRLFileName,"rb");
		x509_crl = PEM_read_bio_X509_CRL(pbio, NULL, NULL, NULL); //BASE64编码的crl文件
		if(x509_crl == NULL)
		{
			ret = errCodeOffsetOfCertMDL_ReadFile;
            		BIO_free(pbio);
			pbio = NULL;
			return ret;
            	}
	}

	BIO_free(pbio);
	pbio = NULL;
	
	pbio = BIO_new_file(p7bFileName,"rb");
	if (pbio == NULL)
	{
		UnionUserErrLog("in UnionVerifyCRL::BIO_new_file[%s] rb failed!\n",p7bFileName);
		ret = errCodeOffsetOfCertMDL_WriteFile;
		return ret;
	}
	// 读pkcs7
	if((p7 = d2i_PKCS7_bio(pbio,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_file(p7bFileName,"rb");
		p7 = PEM_read_bio_PKCS7(pbio,NULL,NULL,NULL);
	}

	if (p7 == NULL)
	{
		UnionUserErrLog("in UnionVerifyCRL:: p7 format err!\n");
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}
	BIO_free(pbio);
	pbio = NULL;

	if (p7->d.sign->cert == NULL)
	{
		UnionUserErrLog("in UnionVerifyCRL::cert is null!\n");
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}


	caChain = p7->d.sign->cert;
	pcrlName = X509_CRL_get_issuer(x509_crl);
        for(;;)
    	{
		x5 = sk_X509_pop(caChain);
		if (!x5)
			break;
		pownerName = X509_get_subject_name(x5);
		// 比较crl的签名者与证书的拥有者，如果相同，即为crl的签名者的证书
		if (X509_name_cmp(pownerName,pcrlName) == 0)
		{
			findSignerFlag = 1;
			pkey = X509_get_pubkey(x5);
			X509_free(x5);
			break;
		}
		X509_free(x5);
	}
	if (!findSignerFlag)
	{
		UnionUserErrLog("in UnionVerifyCRL::not found Signer of crl!\n");
        ret = errCodeOffsetOfCertMDL_VerifyCert;
		goto cleanup;
	}
  
	//用签名者的证书，校验crl
	ret = X509_CRL_verify(x509_crl,pkey);
	if(ret < 0)   
	{
        	UnionUserErrLog("in UnionVerifyCRL::X509_CRL_verify err!\n");
        	ret = errCodeOffsetOfCertMDL_OpenSSl;
		goto cleanup;
	}
	if(ret == 0)
	{
		UnionUserErrLog("in UnionVerifyCRL::X509_CRL_verify failed!\n");
		ret = errCodeOffsetOfCertMDL_VerifyCert;
		goto cleanup;
	}

cleanup:
	if (pbio != NULL) BIO_free(pbio);
	if (x509_crl) X509_CRL_free(x509_crl);
	if (pkey) EVP_PKEY_free(pkey);
	if (p7 != NULL) PKCS7_free(p7);
	
	return ret;
}

//add by linxj 20130622
//制作信封
int UnionTrimPadPkcs5(unsigned char *padbuf,int len,unsigned char *buf,int bufsize)
{
	int padnum = 0;
	int rlen = 0;
	unsigned char pad[9];
	if (padbuf == NULL || buf == NULL || len < 8 || (len%8 != 0))
	{
		return -1;
	}
	padnum = padbuf[len-1];
	memset(pad,0,sizeof(pad));
	rlen = len - padnum;
	memset(pad,padnum,padnum);
	if (memcmp(padbuf+rlen,pad,padnum) != 0)
	{
		return -2;
	}
	if (bufsize < rlen)
	{
		return -3;
	}
	memcpy(buf,padbuf,rlen);
	return (rlen);
}

int UnionEncryptEnvelopeWithoutSign(char *data, char *bankCertBuf, char *envelopStr, int sizeOfEnvelopStr)
{
        //char buf[4096];
        //unsigned char *p;
        BIO *pbio = NULL;
        X509 *x509=NULL;
        BUF_MEM *bptr = NULL;
	PKCS7* p7 = NULL;
	//char tbuf[4096+1];
	char *tbuf = NULL;
	EVP_CIPHER *evp_cipher = NULL;
	PKCS7_RECIP_INFO *p7recipinfo = NULL;
	BIO *p7bio = NULL;
	int	ret = 0;
 
	UnionLoadLoadOpenSSLAlg(); 
        pbio = BIO_new_mem_buf(bankCertBuf,strlen(bankCertBuf));
        if (pbio == NULL)
        {
		UnionUserErrLog("in UnionEncryptEnvelopeWithoutSign:: pbio is null!\n");
		return errCodeOffsetOfCertMDL_ReadFile;
        }
        if((x509 = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
        {
                BIO_free(pbio);
                pbio = BIO_new_mem_buf(bankCertBuf,strlen(bankCertBuf)); 
                x509 = d2i_X509_bio(pbio,NULL);
        }
        BIO_free(pbio);
        pbio = NULL;
        if (x509 == NULL)
        {
		UnionUserErrLog("in UnionEncryptEnvelopeWithoutSign:: x509 is null!\n");
		return errCodeOffsetOfCertMDL_FileFormat;
        }
        if((p7 = PKCS7_new()) == NULL)
	{
		UnionUserErrLog("in UnionEncryptEnvelopeWithoutSign:: PKCS7_new err!\n");
		ret = errCodeOffsetOfCertMDL_ReadFile;
		goto cleanup;
	}
 
        if((ret = PKCS7_set_type(p7, NID_pkcs7_enveloped)) < 0)
	{
		UnionUserErrLog("in UnionEncryptEnvelopeWithoutSign:: PKCS7_new err!\n");
		goto cleanup;;
	}
 
        evp_cipher = EVP_des_ede3_cbc();
 
        PKCS7_set_cipher(p7,evp_cipher);
 

        p7recipinfo = PKCS7_add_recipient(p7,x509);
	if(p7recipinfo == NULL)
	{
		UnionUserErrLog("in UnionEncryptEnvelopeWithoutSign:: PKCS7_add_recipient err!\n");
		goto cleanup;
	}

        p7bio = PKCS7_dataInit(p7, NULL);
        BIO_write(p7bio, data,strlen(data));
        BIO_flush(p7bio);
 
        PKCS7_dataFinal(p7, p7bio);
 
        pbio = BIO_new( BIO_s_mem() );
        PEM_write_bio_PKCS7(pbio,p7);
        BIO_get_mem_ptr(pbio, &bptr);
 
	tbuf = malloc(sizeof(char)*(1024*1024*4+1));
        memset(tbuf,0,sizeof(char)*(1024*1024*4+1));
        memcpy(tbuf, bptr->data, bptr->length);

	if(sizeOfEnvelopStr < bptr->length)
	{
		UnionUserErrLog("in UnionEncryptEnvelopeWithoutSign:: sizeOfEnvelopStr is too short!\n");
		ret = errCodeOffsetOfCertMDL_SmallBuffer;
		goto cleanup;
	}
        
        UnionTrimPemHeadTail(tbuf,envelopStr,"PKCS7");
        ret = strlen(envelopStr);


cleanup:
	free(tbuf);
	tbuf = NULL;

	if (x509 != NULL)  X509_free(x509);
	if(pbio != NULL)
	{
		pbio->ptr = NULL;
		BIO_free(pbio);
		pbio = NULL;
	}
	if(bptr != NULL)
	{
		BUF_MEM_free(bptr);
		bptr = NULL;
	}
	if(p7bio != NULL)
	{
		BIO_free_all(p7bio);
		p7bio = NULL;
	}
	if(p7 != NULL)
	{
		PKCS7_free(p7);
		p7 = NULL;
	}
        return(ret);
}

//解密信封
int UnionDecryptEnvelopWithoutSign(int vkIndex, char *VK, char* bankCertBuf, char *envelopStr, int lenOfenvelop, char *data, int sizeOfData)
{
	BIO *pbio = NULL;
	PKCS7* p7 = NULL;
	int i = 0;
	BIO *p7bio = NULL;
	X509 *x509=NULL;
	int ret = 0;
	char *buffer = NULL;
	ASN1_STRING *data_body=NULL;
	X509_ALGOR *enc_alg=NULL;
	int	algFlag;
	STACK_OF(PKCS7_RECIP_INFO) *rsk=NULL;
	PKCS7_RECIP_INFO *ri=NULL;
	char desKeyByPK[2048+1];
	int lenOfDesKeyByPK = 0;
	char keyValue[48+1];
	int keyLen = 0;
	char iv[48+1];
	int	 lenOfIv = 0;
	char *encdata = NULL;
	int  encdataLen = 0;
	des_key_schedule ks,ks2,ks3;
	char addBufLen[3];
	char	*tmpBuf;

	if (envelopStr == NULL || data == NULL  || lenOfenvelop <= 0 || sizeOfData <= 0 || bankCertBuf == NULL)
	{
		UnionUserErrLog("in UnionDecryptEnvelopWithoutSign::params err!\n");
		return errCodeOffsetOfCertMDL_CodeParameter;
	}
	UnionLoadLoadOpenSSLAlg(); 
	// 读pkcs7
	int buflen=lenOfenvelop*2+4096;
	buffer = malloc(sizeof(char)*(buflen));
	UnionPemStrPreHandle((char *)envelopStr,buffer,"PKCS7",buflen);
	pbio = BIO_new_mem_buf(buffer,strlen(buffer));
	
	if((p7 = PEM_read_bio_PKCS7(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_mem_buf(envelopStr,lenOfenvelop);
		if((p7 = PEM_read_bio_PKCS7(pbio,NULL,NULL,NULL)) == NULL)
		{
			BIO_free(pbio);
			pbio = BIO_new_mem_buf(envelopStr,lenOfenvelop);
			p7 =  d2i_PKCS7_bio(pbio,NULL);
		}
	}
	BIO_free(pbio);
	pbio = NULL;

	//数据不为PKCS#7格式
	if (p7 == NULL)
	{
		UnionUserErrLog("in UnionDecryptEnvelopWithoutSign::get p7 failed!\n");
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}
	//检查是否为信封数据
	if(!PKCS7_type_is_enveloped(p7)) 
	{
		UnionUserErrLog("in UnionDecryptEnvelopWithoutSign::PKCS7_type_is_enveloped data is not envelopedata!\n");
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}
	

	i=OBJ_obj2nid(p7->type);
	p7->state=PKCS7_S_HEADER;
	rsk=p7->d.enveloped->recipientinfo;
	enc_alg=p7->d.enveloped->enc_data->algorithm;
	data_body=p7->d.enveloped->enc_data->enc_data;

	encdata = malloc(sizeof(char)*(1024*1024*4+1));
	memset(encdata, 0, sizeof(char)*(1024*1024*4+1));
	memcpy(encdata, data_body->data,data_body->length);
	encdataLen = data_body->length;
	algFlag = OBJ_obj2nid(enc_alg->algorithm);//算法标识
	
	data_body =(ASN1_STRING *)(enc_alg->parameter->value.ptr);
	memset(iv, 0, sizeof(iv));
	memcpy(iv,data_body->data,data_body->length);
	lenOfIv = data_body->length;
	
	//此处需要初始化证书
	//将证书文件转x509	
	pbio = BIO_new_mem_buf(bankCertBuf,strlen(bankCertBuf)); 
	if (pbio == NULL)
	{
		return errCodeOffsetOfCertMDL_ReadFile;
	}
	if((x509 = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_mem_buf(bankCertBuf,strlen(bankCertBuf)); 
		x509 = d2i_X509_bio(pbio,NULL);
	}
	
	BIO_free(pbio);
	pbio = NULL;
	if (x509 == NULL)
	{
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}

	if (x509)
	{
		ri=sk_PKCS7_RECIP_INFO_value(rsk,0);
		if ((ret = X509_NAME_cmp(ri->issuer_and_serial->issuer,x509->cert_info->issuer)))
		{
			ret = M_ASN1_INTEGER_cmp(x509->cert_info->serialNumber,ri->issuer_and_serial->serial);
		}
		if (ri == NULL)
		{
			UnionUserErrLog("in UnionDecryptEnvelopWithoutSign::sk_PKCS7_RECIP_INFO_value is null!\n");
			goto cleanup;
		}
	}

	memset(desKeyByPK, 0, sizeof(desKeyByPK));
	bcdhex_to_aschex((char*)(ri->enc_key->data), ri->enc_key->length, desKeyByPK);
	lenOfDesKeyByPK = ri->enc_key->length*2;
	

	//使用私钥对数据进行解密并获取des密钥
	memset(keyValue, 0, sizeof(keyValue));
	if((keyLen = UnionGetDesKeyValue(vkIndex, '1', desKeyByPK, lenOfDesKeyByPK, keyValue, sizeof(keyValue))) < 0)
	{
		UnionUserErrLog("in UnionDecryptEnvelopWithoutSign::UnionGetDesKeyValue ret=[%d]!\n", ret);
		goto cleanup;
	}

	DES_set_key_unchecked((const_DES_cblock *)keyValue,&ks);
	DES_set_key_unchecked((const_DES_cblock *)(keyValue+8),&ks2); 
	DES_set_key_unchecked((const_DES_cblock *)(keyValue+16),&ks3); 
	
	memset(buffer,0,sizeof(buffer));
	des_ede3_cbc_encrypt((unsigned char*)encdata,(unsigned char*)buffer,encdataLen,ks,ks2,ks3,(DES_cblock*)&iv,DES_DECRYPT);

	//清除PKCS#5的补位
	tmpBuf = malloc(sizeof(char)*(1024*1024*4+1));
	memset(tmpBuf,0, sizeof(char)*(1024*1024*4+1));
	bcdhex_to_aschex(buffer, encdataLen, tmpBuf);
	memset(addBufLen, 0, sizeof(addBufLen));
	memcpy(addBufLen, tmpBuf+encdataLen*2-2, 2);
	memcpy(data, buffer, encdataLen-atoi(addBufLen));
	ret = strlen(data);

cleanup:
	if(buffer != NULL) free(buffer); buffer=NULL;
	if(encdata != NULL) free(encdata); encdata=NULL;
	if(tmpBuf != NULL) free(tmpBuf); tmpBuf=NULL;
	if (x509 != NULL)  X509_free(x509);
	if (pbio != NULL) BIO_free(pbio);
	if (p7 != NULL) PKCS7_free(p7);
	if (p7bio != NULL) BIO_free(p7bio);
	return(ret);
}

//add end 20130622
/*
int UnionCertStrCRLCheck(char *tCRLFileName,char *tCertDer) 
{ 
	int certlen=strlen(tCertDer);
	int i; 
	int iret=0; 
	BIO *pbio = NULL;
	X509 *x509_cert =NULL; 
	X509_CRL *x509_crl=NULL; 
	STACK_OF(X509_REVOKED) *revoked = NULL;
	int num = 0;
	X509_REVOKED *rc = NULL;
	ASN1_INTEGER *serial = NULL;
	char buffer[4096]={0};

	UnionLoadLoadOpenSSLAlg();
	IS_FILE_EXIST(tCRLFileName);
	//读取CRL文件* 
	pbio = BIO_new_file(tCRLFileName,"rb");
	x509_crl = d2i_X509_CRL_bio(pbio,NULL); //默认先读取der编码的crl文件
  if(x509_crl == NULL)
  {
     BIO_free(pbio);  
     pbio = BIO_new_file(tCRLFileName,"rb");
     x509_crl = PEM_read_bio_X509_CRL(pbio, NULL, NULL, NULL); //BASE64编码的crl文件
	if(x509_crl == NULL)
	{
		iret = errCodeOffsetOfCertMDL_ReadFile;
		goto cleanup;
	}
  }
	BIO_free(pbio);
	pbio = NULL;

	// 读证书
	memset(buffer,0,sizeof(buffer));
	UnionPemStrPreHandle(tCertDer,(char *)buffer,"CERTIFICATE",4096);
	pbio = BIO_new_mem_buf(buffer,strlen((char *)buffer));
	if((x509_cert = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_mem_buf(x509_cert,certlen);
		if ((x509_cert = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
		{
			BIO_free(pbio);
			pbio = BIO_new_mem_buf(x509_cert,certlen);
			x509_cert = d2i_X509_bio(pbio,NULL);
		}
	}

	if (x509_cert == NULL)
	{
		UnionUserErrLog("in UnionVerifyCertificateWithPKHsm::read CertfileName failed!\n");
		return errCodeOffsetOfCertMDL_FileFormat;
	}
	BIO_free(pbio);
	pbio = NULL;
	
	revoked = x509_crl->crl->revoked; 
	 
	serial=X509_get_serialNumber(x509_cert); 
	char serNum[100]={0};
	bcdhex_to_aschex(serial->data,10,serNum);
	UnionUserErrLog("serial num is [%s]\n",serNum);
	num=revoked->num; 
	for(i=0;i<num;i++)
	{ 
		rc=sk_X509_REVOKED_pop(revoked); 
		if(ASN1_INTEGER_cmp(serial,rc->serialNumber)==0)
		{ 
			iret=1; 
			break; 
		} 
	} 
cleanup:
	if (pbio != NULL) BIO_free(pbio);
	if (x509_crl != NULL) X509_CRL_free(x509_crl); 
	if (x509_cert != NULL) X509_free(x509_cert); 
	return(iret); 
 }*/ 
int UnionSignWithHsm37(int vkIndex,char *vk,int hashID,unsigned char *data,int len,char *sign);

int UnionSignWithHsm(int vkindex,char *vk,int hashID,unsigned char *data,int len,char *sign)
{
	int     ret;
	char	tmptmp[100];
        //char    hashIDStr[2+1];
        //char    signStr[1024+1];
        char    tmpBuf[1024+1];
        char    shapadStr[] =  "3021300906052B0E03021A05000414";
        //char    md5padStr[] =  "3020300C06082A864886F70D020505000410";
        int     padLen = 0;
        char    hashedData[512+1];
        memset(tmpBuf, 0, sizeof(tmpBuf));
        memset(tmptmp, 0, sizeof(tmptmp));
        memset(hashedData, 0, sizeof(hashedData));
        UnionSHA1(data,len,tmpBuf);
        padLen = strlen(shapadStr);
        aschex_to_bcdhex(shapadStr,padLen,hashedData);
        memcpy(hashedData+padLen/2,tmpBuf,20);
	hashedData[padLen/2+20]=0;

	bcdhex_to_aschex(tmpBuf,20,tmptmp);
	UnionLog("sign hash [%s][%s]\n",data,tmptmp);

        memset(tmpBuf, 0, sizeof(tmpBuf));
        ret =  UnionSignWithHsm37(vkindex,vk , hashID,(unsigned char*) hashedData, padLen/2+20,tmpBuf);
	//ret = UnionSignWithHsmEW(99,vk,);
        bcdhex_to_aschex(tmpBuf, ret, sign);

        UnionLog("in UnionSignWithHsm::sign=[%d][%s]!\n",ret,sign);
        return ret;
}

int UnionSignWithHsm37(int vkIndex,char *vk,int hashID,unsigned char *data,int len,char *sign)
{
        char    vkidx[4+1];
        int     ret=0;
        char    signature[1024+1];
	int	lenOfVK = 0;
	char	vkValue[1280+1];

        if ( (vkIndex < 0 || vkIndex > 20) && (vkIndex != 99) )
        {
                UnionUserErrLog("in UnionSignWithHsm37:: vkindex[%d] out of range !\n", vkIndex);
                return(ret);
        }

        memset(vkidx,0,sizeof(vkidx));
        memset(signature,0,sizeof(signature));
        sprintf(vkidx,"%02d",vkIndex);

	// begin    2014-01-08  leipp 将私钥转换为bcd码
	memset(vkValue,0,sizeof(vkValue));
	lenOfVK = aschex_to_bcdhex(vk,strlen(vk),vkValue);
	// end	

        if((ret = UnionHsmCmd37('1',vkidx,lenOfVK,vkValue,len,(char*)data,signature,sizeof(signature))) < 0)
        {
                UnionUserErrLog("in UnionSignWithHsm37:: SJL06Cmd37 error !\n");
                return(ret);
        }
        memcpy(sign,signature,ret);
        return ret;
}
int UnionGetDesKeyValue(int VKindex, char flag, char *data, int len, char *deskey, int sizeOfDesKey)
{
        int     ret = 0;
        char    vkIndex[3];
        char    dataBCDBuf[2048+1];

        memset(vkIndex, 0, sizeof(vkIndex));
        sprintf(vkIndex, "%02d", VKindex);

        memset(dataBCDBuf, 0, sizeof(dataBCDBuf));
        aschex_to_bcdhex(data, len, dataBCDBuf);

	UnionHsmCmd33(flag,vkIndex,0, NULL, len/2,dataBCDBuf,deskey,sizeOfDesKey);

        //ret = SJL06Cmd33(flag, vkIndex, len/2, dataBCDBuf, deskey, sizeOfDesKey);
        if (ret < 0)
        {
                UnionUserErrLog("in UnionGetDesKeyValue:: SJL06Cmd33 ret=[%d]!\n", ret);
                return(ret);
        }
        UnionDebugLog("in UnionGetDesKeyValue::ret = [%d] key=[%s] !\n", ret,deskey);
        return(ret);
}
 
