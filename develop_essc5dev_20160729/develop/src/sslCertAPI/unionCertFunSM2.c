/*
create by hzh in 2015.5
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <openssl/asn1.h>
#include <openssl/asn1t.h>
#include <openssl/objects.h>
//#include <ec_lcl.h>
#include <openssl/asn1.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/pkcs12.h>
#include <openssl/des.h>  
#include <openssl/ec.h>
#include "unionCertFunSM2.h"
#include "unionGmP7.h"
#include "unionCharCode.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "base64.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
//add by zhouxw 201512004
#include "unionRSA.h"
//add end


#define CertRSAEncryptionAlg "300D06092A864886F70D0101010500"
#define CertSM2EncryptionAlg "301306072A8648CE3D020106082A811CCF5501822D"
#define CertSM2SignAlg "300B06092A811CCF5501822D01"    //1.2.156.10197.1.301.1
#define CertSM2SignAlgAndNull "300D06092A811CCF5501822D010500" //1.2.156.10197.1.301.1 + null
#define CertSM2WithSM3Alg "300A06082A811CCF55018375"    //1.2.156.10197.1.501
#define CertSM2WithSM3AlgAndNull "300C06082A811CCF550183750500" //1.2.156.10197.1.501 + null

#define sha1WithRsaEncryptOBJ  "06092A864886F70D010105"
#define md5WithRsaEncryptOBJ   "06092A864886F70D010104"
#define sm3WithSM2SignatureOBJ  "06082A811CCF55018375"
//add by zhouxw 20151202
#define sha256WithRsaEncryptOBJ "06092A864886F70D01010B"
//add end

#define PKCS7_SIGNED_DATA_TYPE_DER  "06092A864886F70D010702"   // 1.2.840.113549.1.7.2  pkcs7国际签名数据类型
#define SM2_PKCS7_SIGNED_DATA_TYPE_DER "060A2A811CCF550601040202"  //1.2.156.10197.6.1.4.2.2 pkcs7国密签名数据类型
#define SM2_PKCS7_DATA_TYPE_DER "060A2A811CCF550601040201"   //1.2.156.10197.6.1.4.2.1 sm2 pkcs7数据
#define SM3_HASH_ALG_WITH_KEY_DER "06092A811CCF5501831102"   //1.2.156.10197.1.401.2 SM3哈希算法，带密钥运算
#define SM3_HASH_ALG_DER "06082A811CCF55018311"   //1.2.156.10197.1.401 SM3哈希算法 
#define SHA1_ALG_OBJ_DER "06052B0E03021A"    //SHA1 obj der 1.3.14.3.2.26
#define MD5_ALG_OBJ_DER "06082A864886F70D0205"    //MD5 obj der  1.2.840.113549.2.5
#define SHA256_ALG_OBJ_DER "6086480165030402010500"	//SHA256


//SM2私钥签名, 这个函数需自己实现调用密码机指令实现
/*
输入:
	DbyLMK:私钥D密文
	dlen: D密文长度
	userID:用户ID
	userIDlen:用户ID长度
	data:待签名数据
	datalen:数据长度
输出:
	rout:签名r
	sout:签名s
*/
//extern int UnionSM2Sign(int vkIndex, unsigned char *DbyLMK,int dlen,unsigned char *userID,int userIDlen,unsigned char *data,int datalen,unsigned char rout[32],unsigned char sout[32]);
 
//extern int UnionSM2SignEx(unsigned char d[32],unsigned char x[32],unsigned char y[32],unsigned char *userID,int userIDlen,unsigned char *data,int datalen,unsigned char rout[32],unsigned char sout[32]);
 

//SM2公钥验签 
//extern int UnionSM2Verify(unsigned char pkx[32],unsigned char pky[32],unsigned char *userID,int userIDlen,unsigned char *data,int datalen,unsigned char rin[32],unsigned char sin[32]);

//comment by zhouxw 20151204
//int UnionPKDecByPKCS1(unsigned char *crypk,int crypklen,char *pk,unsigned char *plaintxt);
//comment end

static int gLoadOpenSSLAlgFlag = 0;
static int UnionUni2ascEx(unsigned char *uni, int unilen,char *asc,int sizeasc);
static int X509_NAME_get_text_by_NID_Ex(X509_NAME *name, int nid, unsigned char buf[][41], int len);
static int X509_NAME_get_text_by_OBJ_Ex(X509_NAME *name, ASN1_OBJECT *obj, unsigned char buf[][41],
	     int num);
 
int i2d_PKCS7_Ex(PKCS7 *a, unsigned char **out);
PKCS7 *d2i_PKCS7_Ex(PKCS7 **a, const unsigned char **in, long len);
int ASN1_item_ex_d2i_m(ASN1_VALUE **pval, const unsigned char **in, long len,
			const ASN1_ITEM *it,
			int tag, int aclass, char opt, ASN1_TLC *ctx);
ASN1_VALUE *ASN1_item_d2i_m(ASN1_VALUE **pval,
		const unsigned char **in, long len, const ASN1_ITEM *it);

int ASN1_item_ex_i2d_m(ASN1_VALUE **pval, unsigned char **out,
			const ASN1_ITEM *it, int tag, int aclass);


X509_ALGOR  *SM2_X509_ALGOR_new();
X509_ALGOR *SM3_X509_Hash_ALGOR_new();
X509_ALGOR *SM3_P7_Hash_ALGOR_new();
X509_ALGOR  *SM2_P7_ALGOR_new();

static int X509_NAME_get_text_by_NID_Ex(X509_NAME *name, int nid, unsigned char buf[][41], int len)
	{
	ASN1_OBJECT *obj;

	obj=OBJ_nid2obj(nid);
	if (obj == NULL) return(-1);
	return(X509_NAME_get_text_by_OBJ_Ex(name,obj,buf,len));
	}

static int X509_NAME_get_text_by_OBJ_Ex(X509_NAME *name, ASN1_OBJECT *obj, unsigned char buf[][41],
	     int num)
{
	int i;
	int n = 0;
	X509_NAME_ENTRY *ne;
	STACK_OF(X509_NAME_ENTRY) *sk;
	ASN1_STRING *data;
	char tbuf[1024];
	int tlen = 0;
	int k = 0;
	if (name == NULL) return(-1);
	 
	sk=name->entries;
	n=sk_X509_NAME_ENTRY_num(sk);
	for (i=0; i < n ; i++)
	{
		ne=sk_X509_NAME_ENTRY_value(sk,i);
		if (OBJ_cmp(ne->object,obj) == 0)
		{
			data = ne->value;
			tlen = data->length;
			if(tlen > sizeof(tbuf)-1) tlen = sizeof(tbuf)-1;
			memcpy(tbuf,data->data,tlen);
			tbuf[tlen]='\0';
			 
			//V_ASN1_BMPSTRING
			if(data->type == V_ASN1_BMPSTRING)
			{
			  tlen = UnionUni2ascEx((unsigned char *)tbuf,tlen,(char *)buf[k],41);
			}
			else if(data->type == V_ASN1_UTF8STRING)  //V_ASN1_UTF8STRING
			{
				tlen = UnionUtf8ToAnsi((unsigned char *)tbuf,tlen,(unsigned char *)buf[k],41);
			}
			else {
				if(tlen > 40)
					tlen = 40;
				memcpy(buf[k],tbuf,tlen);
			}
			k++;
			if(k >= num)
				break;
		}
	}
	return k;
	 
}

static void UnionTrimCtrlMN(char *inbuf,char *outbuf)
{
	//char input[19200];
	char *input = NULL;
	char *p = NULL;

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

int UnionTrimPemHeadTail(char *inbuf,char *outbuf,char *flagID)
{
        //char input[19200];
        char *p = NULL;
        int offset = 0;
        char header[100];
        char ender[100];
        char *p1 = NULL;
	int isNeedTrim = 0;


        //memset(input,0,sizeof(input));
        //strcpy(input,inbuf);
        //p1 = input;
        p1 = inbuf;
        sprintf(header,"-----BEGIN %s-----",flagID);
        sprintf(ender,"-----END %s-----",flagID);
        if ((p = strstr(inbuf,header)) != NULL)
        {
                offset = (p - p1) + strlen(header);
		isNeedTrim = 1;
        }
        if ((p = strstr(inbuf+offset,ender)) != NULL)
                *p = '\0';
	if(isNeedTrim)
        	UnionTrimCtrlMN(inbuf+offset,outbuf);
	return(isNeedTrim);
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
	int ret = 0;

	 
	UnionLog("UnionPemStrPreHandle buflen %d\n",buflen);
	UnionLog("UnionPemStrPreHandle malloc %d\n",(int)sizeof(char)*(buflen+100));

	input = malloc(sizeof(char)*(buflen+100));
	buf= malloc(sizeof(char)*(buflen+100));
	if(!input||!buf)
	{
		UnionUserErrLog("UnionPemStrPreHandle::malloc failed!\n");
		ret = -1;
		goto endfree;
	}
	strcpy(input,inbuf);

	memset(buf,0,sizeof(char)*(buflen+1));
	UnionTrimCtrlMN(input,buf);
	strcpy(input,buf);
	 

	sprintf(header,"-----BEGIN %s-----",flagID);
	sprintf(ender,"-----END %s-----",flagID);
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
		ret = strlen(outbuf);
		goto endfree;
	}

	memset(buf,0,sizeof(char)*(buflen+1));
	offset = 0;
	if (neendHeadFlag)
	{
	    if((strlen(header) + strlen(ender)) >= buflen)
	    {
	      ret = -1;
		  goto endfree;
	    }
		strcpy(buf,header);
	}
	else {
		strncpy(buf,input,strlen(header));
		offset += strlen(header);
	}

	if (neendHeadCtrlFlag) 
	{
	    if(strlen(buf) >= buflen)
	    {
	       ret = -1;
		   goto endfree;
	    }
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
    ret = strlen(outbuf);

endfree:
	if(input) free(input);
	if(buf) free(buf);
	input = NULL;
	buf = NULL;
	return ret;
}


static int UnionUni2ascEx(unsigned char *uni, int unilen,char *asc,int sizeasc)
{
	int asclen, i;
	 
	asclen = unilen / 2;
	/* If no terminating zero allow for one */
	if (!unilen || uni[unilen - 1]) asclen++;
	uni++;
	
	if(asclen > sizeasc)
		return -1;
	for (i = 0; i < unilen; i+=2) asc[i>>1] = uni[i];
	asc[asclen - 1] = 0;
	return asclen;
}


const char *UnionGetPrefixOfMD5OID()
{
    	const static char md5pre[] = "3020300C06082A864886F70D020505000410";
	return md5pre;
}

const char *UnionGetPrefixOfSHA1OID()
{
    	const static char sha1pre[] = "3021300906052B0E03021A05000414";
	return sha1pre;
}

const char *UnionGetPrefixOfSHA256OID()
{
	const static char sha256pre[] = "3031300d060960864801650304020105000420";
	return sha256pre;
}


int Union_SM2_PKCS7_SET_SIGNED_Type(PKCS7* p7)
{
	ASN1_OBJECT *obj;
	char buf[1024] = {0};
	unsigned char derBuf[512] = {0};
	unsigned char *p = NULL;
	int len = 0;
	obj = ASN1_OBJECT_new();

	strcpy(buf,SM2_PKCS7_SIGNED_DATA_TYPE_DER);  //1.2.156.10197.6.1.4.2.2
	
	len = aschex_to_bcdhex(buf,strlen(buf),(char *)derBuf);
	p = derBuf;
	d2i_ASN1_OBJECT(&obj,(const unsigned char **)&p,len);
	if(p7->type != NULL)
	{
		ASN1_OBJECT_free(p7->type);
	}
	p7->type=obj;
	if(p7->d.sign != NULL)
	{
		PKCS7_SIGNED_free(p7->d.sign);
	}
    p7->d.sign=PKCS7_SIGNED_new();
	ASN1_INTEGER_set(p7->d.sign->version,1);
	 
	return 1;
}


int Union_SM2_PKCS7_SET_Data_Type(PKCS7* p7)
{
	ASN1_OBJECT *obj;
	char buf[1024] = {0};
	unsigned char derBuf[512] = {0};
	unsigned char *p = NULL;
	int len = 0;
	obj = ASN1_OBJECT_new();
	strcpy(buf,SM2_PKCS7_DATA_TYPE_DER);  //1.2.156.10197.6.1.4.2.1
	len = aschex_to_bcdhex(buf,strlen(buf),(char *)derBuf);
	p = derBuf;
	d2i_ASN1_OBJECT(&obj,(const unsigned char **)&p,len);
	if(p7->type) {
		ASN1_OBJECT_free(p7->type);
	}
	p7->type=obj;
	if(p7->d.data) {
		M_ASN1_OCTET_STRING_free(p7->d.data);
	}
    p7->d.data=M_ASN1_OCTET_STRING_new();
	return 1;
}

int Union_SM2_PKCS7_content_new_data(PKCS7* p7)
{
	PKCS7 *ret=NULL;

	if ((ret=PKCS7_new()) == NULL) goto err;
	if (!Union_SM2_PKCS7_SET_Data_Type(ret)) goto err;
	if (p7->d.sign->contents != NULL) {
			PKCS7_free(p7->d.sign->contents);
	}
	p7->d.sign->contents=ret;

	return(1);
err:
	if (ret != NULL) PKCS7_free(ret);
	return(0);
}


int Union_SM2_PKCS7_signed_add_signer(PKCS7 *p7, PKCS7_SIGNER_INFO *psi)
{
	int i,j;
	X509_ALGOR *alg;
	STACK_OF(PKCS7_SIGNER_INFO) *signer_sk;
	STACK_OF(X509_ALGOR) *md_sk;
	ASN1_OBJECT *signerInfoAlg = NULL;  
	signer_sk=	p7->d.sign->signer_info;
	md_sk=		p7->d.sign->md_algs;
		 
	signerInfoAlg = psi->digest_alg->algorithm;
	/* If the digest is not currently listed, add it */
	j=0;
	for (i=0; i<sk_X509_ALGOR_num(md_sk); i++)
	{
	alg=sk_X509_ALGOR_value(md_sk,i);
	if(alg->algorithm->length > 0 && 
		alg->algorithm->length == signerInfoAlg->length &&
		memcmp(alg->algorithm->data,signerInfoAlg->data,alg->algorithm->length) == 0)
		{
			j=1;
			break;
		}
	}
	if (!j) /* we need to add another algorithm */
	{
		alg = SM3_P7_Hash_ALGOR_new();
		sk_X509_ALGOR_push(md_sk,alg);
	}
	 
	sk_PKCS7_SIGNER_INFO_push(signer_sk,psi);
	return(1);
}


int UnionGetCertInfoAlgAndPK(int reqFlag,void *pcer,int *algFlag,unsigned char *PKDer)
{
	X509 *cer = NULL;
	X509_REQ *req = NULL;
	X509_ALGOR *algor = NULL;
	ASN1_BIT_STRING *public_key = NULL;
	unsigned char derBuf[4096] = {0};
	int len = 0;
	unsigned char *p = NULL;
	char tbuf[8192] = {0};

	if(reqFlag == 1)
	{
		req = (X509_REQ *)pcer;
		if(req == NULL || req->req_info == NULL || req->req_info->pubkey == NULL || req->req_info->pubkey->algor == NULL ||
		req->req_info->pubkey->public_key == NULL)
		{
			UnionUserErrLog("in UnionGetReqCertInfoAlgAndPK::req para err.\n");
			return -1;
		}
		algor = req->req_info->pubkey->algor;
		public_key = req->req_info->pubkey->public_key;
	}
	else {
		cer = (X509 *)pcer;
		if(cer == NULL || cer->cert_info == NULL || cer->cert_info->key == NULL || cer->cert_info->key->algor == NULL ||
			cer->cert_info->key->public_key == NULL)
		{
			UnionUserErrLog("in UnionGetCertInfoAlgAndPK::cer para err.\n");
			return -1;
		}
		algor = cer->cert_info->key->algor;
		public_key = cer->cert_info->key->public_key;
	}
	
	p = derBuf;
	if((len = i2d_X509_ALGOR(algor,&p)) <= 0)
	{
		UnionUserErrLog("in UnionGetCertInfoAlgAndPK::req algor para err.\n");
		return -2;
	}
	 
	bcdhex_to_aschex((char *)derBuf,len,tbuf);
	tbuf[len*2] = 0;
	//printf("in UnionGetCertInfoAlgAndPK::req algor derBuf Hex=[%s].\n",tbuf);
	if(strcmp(CertRSAEncryptionAlg,tbuf) == 0)
	{
		*algFlag = UNION_RSA_ALG;
	}
	else if (strcmp(CertSM2EncryptionAlg,tbuf) == 0)
	{
		*algFlag = UNION_SM2_ALG;
	}
	else {
		*algFlag = -1;
		//printf("in UnionGetCertInfoAlgAndPK::not support req algor derBuf Hex=[%s].\n",tbuf);
		return -3;
	}
	
	memcpy(PKDer,public_key->data,public_key->length);
	len = public_key->length;
	 
	return len;
}


int Union_SM2_PKCS7_set_detached(PKCS7* p7,int larg)
{
	ASN1_OBJECT *obj;
	unsigned char *p = NULL;
	unsigned char derBuf[100] = {0};
	char buf[1024] = {0};
	int len = 0;
	p7->detached = larg;
	obj = p7->d.sign->contents->type;
	p= derBuf;
	len = i2d_ASN1_OBJECT(obj,&p);
	if(len <= 0)
		return -1;
	bcdhex_to_aschex((char *)derBuf,len,buf);

	if (larg && strcmp(buf,SM2_PKCS7_DATA_TYPE_DER) == 0)
	{
		ASN1_OCTET_STRING *os;
		os=p7->d.sign->contents->d.data;
		ASN1_OCTET_STRING_free(os);
		p7->d.sign->contents->d.data = NULL;
	}
	return 0;		
}

X509_ALGOR *SM3_X509_Hash_ALGOR_new()
{
	X509_ALGOR  *alg = NULL;
	const char buf[100] = "300D06092A811CCF55018311020500";  //1.2.156.10197.1.401.2 + null  SM3算法,带公钥 
	unsigned char derBuf[100] = {0};
	const unsigned char *p = NULL;
	int len = 0;
	alg = X509_ALGOR_new();
	len = aschex_to_bcdhex((char *)buf,strlen(buf),(char *)derBuf);
	p = derBuf;
	if(d2i_X509_ALGOR(&alg,&p,len) == NULL)
	{
		UnionUserErrLog("SM3_X509_Hash_ALGOR_new failed!\n");
		X509_ALGOR_free(alg);
		return NULL;
	}
	//printf("-----------SM3_X509_Hash_ALGOR_new OK!\n");
	return alg;
}


X509_ALGOR *SM3_P7_Hash_ALGOR_new()
{
	X509_ALGOR  *alg = NULL;
	const char buf[100] = "300C06082A811CCF550183110500";  //1.2.156.10197.1.401 SM3哈希算法 
	unsigned char derBuf[100] = {0};
	const unsigned char *p = NULL;
	int len = 0;
	alg = X509_ALGOR_new();
	len = aschex_to_bcdhex((char *)buf,strlen(buf),(char *)derBuf);
	p = derBuf;
	if(d2i_X509_ALGOR(&alg,&p,len) == NULL)
	{
		UnionUserErrLog("SM3_P7_Hash_ALGOR_new failed!\n");
		X509_ALGOR_free(alg);
		return NULL;
	}
	return alg;
}


int Union_SM2_PKCS7_SIGNER_INFO_set(PKCS7_SIGNER_INFO *p7i, X509 *x509)
{
	if (!ASN1_INTEGER_set(p7i->version,1))
		goto err;
	if (!X509_NAME_set(&p7i->issuer_and_serial->issuer,
			X509_get_issuer_name(x509)))
		goto err;
 
	M_ASN1_INTEGER_free(p7i->issuer_and_serial->serial);
	if (!(p7i->issuer_and_serial->serial=
			M_ASN1_INTEGER_dup(X509_get_serialNumber(x509))))
		goto err;

	  
	X509_ALGOR_free(p7i->digest_alg);
	 p7i->digest_alg = SM3_P7_Hash_ALGOR_new();
	 
	 X509_ALGOR_free(p7i->digest_enc_alg);
	 p7i->digest_enc_alg = SM2_P7_ALGOR_new();
	return(1);
err:
	return(0);
}

PKCS7_SIGNER_INFO *Union_SM2_PKCS7_add_signature(PKCS7 *p7, X509 *x509)
{
	PKCS7_SIGNER_INFO *si;

	if ((si=PKCS7_SIGNER_INFO_new()) == NULL) goto err;
	if (!Union_SM2_PKCS7_SIGNER_INFO_set(si,x509)) goto err;
	if (!Union_SM2_PKCS7_signed_add_signer(p7,si)) goto err;
	return(si);
err:
	return(NULL);
}

int Union_SM2_PKCS7_add_sign_certificate(PKCS7 *p7, X509 *x509)
{
	STACK_OF(X509) **sk;
	 
	sk= &(p7->d.sign->cert);

	if (*sk == NULL)
		*sk=sk_X509_new_null();
	CRYPTO_add(&x509->references,1,CRYPTO_LOCK_X509);
	sk_X509_push(*sk,x509);
	return(1);
}

ASN1_OCTET_STRING *ECDSA_R_S_SIG_OCTET_new(unsigned char r[32],unsigned char s[32])
{
	ECDSA_SIG *sig = NULL;
	BIGNUM *tr,*ts;
	char rHex[65] = {0};
	char sHex[65] = {0};
	unsigned char *p = NULL;
	unsigned char derBuf[256] = {0};
	int len = 0;
	ASN1_OCTET_STRING  *octet_string = NULL;

	sig = ECDSA_SIG_new();
	
	bcdhex_to_aschex((char *)r,32,rHex);
	bcdhex_to_aschex((char *)s,32,sHex);
	tr = BN_new();
	ts = BN_new();
	if (!BN_hex2bn(&tr, rHex))
	{
		BN_free(tr);
		BN_free(ts);
		ECDSA_SIG_free(sig);
		return NULL;
	}
	if (!BN_hex2bn(&ts, sHex))
	{
		BN_free(tr);
		BN_free(ts);
		ECDSA_SIG_free(sig);
		return NULL;
	}
	sig->r = tr;
	sig->s = ts;
	p = derBuf;
	if((len = i2d_ECDSA_SIG(sig,&p)) < 0)
	{
		BN_free(tr);
		BN_free(ts);
		ECDSA_SIG_free(sig);
		return NULL;
	}
	ECDSA_SIG_free(sig); 
	octet_string = ASN1_OCTET_STRING_new();
	octet_string->type = V_ASN1_OCTET_STRING;
	octet_string->length = len;
	octet_string->data = (unsigned char *)OPENSSL_malloc(len);
	memcpy(octet_string->data,derBuf,len);
	 
	return octet_string;
}

/*
功能:加载OpenSSL的算法
*/
void UnionLoadLoadOpenSSLAlg()
{
	if (gLoadOpenSSLAlgFlag)
		return;
	OpenSSL_add_all_algorithms();
	ERR_load_crypto_strings();
	gLoadOpenSSLAlgFlag = 1;
}



X509_ALGOR  *SM2_X509_ALGOR_new()
{
	X509_ALGOR  *alg = NULL;
	const char buf[100] = CertSM2EncryptionAlg;  //ecPublicKey(1.2.840.10045.2.1) + SM2椭圆曲线公钥算法(1.2.156.10197.1.301)
	unsigned char derBuf[100] = {0};
	const unsigned char *p = NULL;
	int len = 0;
	alg = X509_ALGOR_new();
	len = aschex_to_bcdhex((char *)buf,strlen(buf),(char *)derBuf);
	p = derBuf;
	if(d2i_X509_ALGOR(&alg,&p,len) == NULL)
	{
		X509_ALGOR_free(alg);
		return NULL;
	}
	return alg;
}


X509_ALGOR  *SM2_P7_ALGOR_new()
{
	X509_ALGOR  *alg = NULL;
	const char buf[100] = CertSM2SignAlgAndNull;   
	unsigned char derBuf[100] = {0};
	const unsigned char *p = NULL;
	int len = 0;
	alg = X509_ALGOR_new();
	len = aschex_to_bcdhex((char *)buf,strlen(buf),(char *)derBuf);
	p = derBuf;
	if(d2i_X509_ALGOR(&alg,&p,len) == NULL)
	{
		X509_ALGOR_free(alg);
		return NULL;
	}
	return alg;
} 

ASN1_BIT_STRING *X509_PUBLIC_BIT_STRING_new(unsigned char pkx[32],unsigned char pky[32])
{
 
	ASN1_BIT_STRING  *bit_string = NULL;
	unsigned char pkOct[1024];
	unsigned char *p = NULL;
	int len = 0;
	char buf[1024] = {0};
	bit_string  =  ASN1_BIT_STRING_new();
	 
	pkOct[0] = 0x04;
	memcpy(pkOct+1,pkx,32);
	memcpy(pkOct+33,pky,32);
	 
	bit_string->type = V_ASN1_BIT_STRING;
	bit_string->length = 65;
	bit_string->data = (unsigned char *)OPENSSL_malloc(65);
	memcpy(bit_string->data,pkOct,65);

	
	bit_string->flags &= ~(ASN1_STRING_FLAG_BITS_LEFT|0x07);
	bit_string->flags |=  ASN1_STRING_FLAG_BITS_LEFT;
	p = pkOct;
	memset(pkOct,0,sizeof(pkOct));
	len = i2d_ASN1_BIT_STRING(bit_string,&p);
	memset(buf,0,sizeof(buf));
    bcdhex_to_aschex((char *)pkOct,len,buf);
	//printf("X509_PUBLIC_BIT_STRING buf=[%s].\n",buf);
	 
	return bit_string;
}


X509_ALGOR *SM2_SM3_X509_SIG_ALGOR_new()
{
	X509_ALGOR  *alg = NULL;
	const char buf[100] = "300C06082A811CCF550183750500"; 
	unsigned char derBuf[100] = {0};
	const unsigned char *p = NULL;
	int len = 0;
	alg = X509_ALGOR_new();
	len = aschex_to_bcdhex((char *)buf,strlen(buf),(char *)derBuf);
	p = derBuf;
	if(d2i_X509_ALGOR(&alg,&p,len) == NULL)
	{
		//printf("SM2_SM3_X509_SIG_ALGOR_new failed!\n");
		X509_ALGOR_free(alg);
		return NULL;
	}
	//printf("-----------SM2_SM3_X509_SIG_ALGOR_new OK!\n");
	return alg;
}


ASN1_BIT_STRING *ECDSA_R_S_SIG_Bit_new(unsigned char r[32],unsigned char s[32])
{
	ECDSA_SIG *sig = NULL;
	BIGNUM *tr,*ts;
	char rHex[65] = {0};
	char sHex[65] = {0};
	unsigned char *p = NULL;
	unsigned char derBuf[256] = {0};
	int len = 0;
	ASN1_BIT_STRING  *bit_string = NULL;

	sig = ECDSA_SIG_new();
	
	bcdhex_to_aschex((char *)r,32,rHex);
	bcdhex_to_aschex((char *)s,32,sHex);
	tr = BN_new();
	ts = BN_new();
	if (!BN_hex2bn(&tr, rHex))
	{
		BN_free(tr);
		BN_free(ts);
		ECDSA_SIG_free(sig);
		return NULL;
	}
	if (!BN_hex2bn(&ts, sHex))
	{
		BN_free(tr);
		BN_free(ts);
		ECDSA_SIG_free(sig);
		return NULL;
	}
	sig->r = tr;
	sig->s = ts;
	p = derBuf;
	if((len = i2d_ECDSA_SIG(sig,&p)) < 0)
	{
		BN_free(tr);
		BN_free(ts);
		ECDSA_SIG_free(sig);
		return NULL;
	}
	 
	bit_string = ASN1_BIT_STRING_new();
	bit_string->type = V_ASN1_BIT_STRING;
	bit_string->length = len;
	bit_string->data = (unsigned char *)OPENSSL_malloc(len);
	memcpy(bit_string->data,derBuf,len);
	bit_string->flags &= ~(ASN1_STRING_FLAG_BITS_LEFT|0x07);
	bit_string->flags |=  ASN1_STRING_FLAG_BITS_LEFT;
	return bit_string;
}


/*
功能: 生成sm2的p10证书请求文件
输入:
	vkbyLmk: 密码机保护的私钥
	vklen: 私钥长度
	pk: pk为"04|x|y"的Hex
	pReqUserInfo: 请求用户信息
	fileFormat: 生成文件的格式 0--PEM或1--DER
	fileName:生成的p10证书请求文件名(带路径)
返回:
	>=0 成功
	<0 失败
*/
int UnionGenerateSM2PKCS10Ex(int vkIndex, char *vkbyLmk, int vklen, char *pk, PUnionX509UserInfoSM2 pReqUserInfo,int fileFormat,char *fileName, char *signature)
{
	X509_REQ            *req = NULL;
	int                  ret;
	long               version;
	X509_NAME         *name;
	X509_PUBKEY          *pkey = NULL;
	X509_NAME_ENTRY   *entry=NULL;

	unsigned char      bytes[100];
	int                len;
	   
	BIO                      *b;
	unsigned char		buf[8192+1];
	 
	unsigned char *buf_in=NULL; 
	    
	unsigned char pkx[32] = {0};
	unsigned char pky[32] = {0};
	char tbuf[8192] = {0};
	  
	unsigned char r[32] = {0},s[32] = {0};
	int  reqinfolen = 0;
	 
	UnionLoadLoadOpenSSLAlg();

	if (pReqUserInfo == NULL || fileName == NULL)
	{
		UnionUserErrLog("in UnionGenerateSM2PKCS10Ex::para err!\n");
		return errCodeOffsetOfCertMDL_CodeParameter;
	}
	if(strlen(pk) != 130 || strncmp(pk,"04",2) != 0)
 	{
		 UnionUserErrLog("in UnionGenerateSM2PKCS10Ex::para err2!\n");
		 return errCodeOffsetOfCertMDL_CodeParameter;
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
	 
	 if(pReqUserInfo->organizationalUnitName2 != NULL && strlen(pReqUserInfo->organizationalUnitName2) != 0)
	 {
		 memset(bytes,0,sizeof(bytes));
		 len = UnionAnsiToUtf8((unsigned char *)pReqUserInfo->organizationalUnitName2,strlen(pReqUserInfo->organizationalUnitName2),bytes,sizeof(bytes));
		 entry=X509_NAME_ENTRY_create_by_txt(&entry,"organizationalUnitName",V_ASN1_UTF8STRING,bytes,len);
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
	 
	/* subject name */
	ret=X509_REQ_set_subject_name(req,name);
	 
	X509_NAME_free(name);
	/* pub key */
	  
	pkey = X509_PUBKEY_new();
	X509_ALGOR_free(pkey->algor);
	pkey->algor = SM2_X509_ALGOR_new();
	aschex_to_bcdhex(pk+2,64,(char *)pkx);
	aschex_to_bcdhex(pk+2+64,64,(char *)pky);
	ASN1_BIT_STRING_free(pkey->public_key);
	pkey->public_key = X509_PUBLIC_BIT_STRING_new(pkx,pky); 
	X509_PUBKEY_free(req->req_info->pubkey);
	req->req_info->pubkey = pkey;
	buf_in = buf;
	memset(buf,0,sizeof(buf));
	reqinfolen = i2d_X509_REQ_INFO(req->req_info,&buf_in);
	memset(tbuf,0,sizeof(tbuf));
	bcdhex_to_aschex((char *)buf,reqinfolen,tbuf);
	UnionLog("X509_REQ_INFO=[%s].\n",tbuf);
	
	//调密码机
	if((ret = UnionHsmCmdK3("02", strlen(SM2DefaultUserID), SM2DefaultUserID, reqinfolen*2, tbuf, vkIndex, vklen, vkbyLmk, signature, 128)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSM2PKCS10Ex:: UnionHsmCmdK3 return[%d]\n", ret);
		X509_REQ_free(req);
                return (ret);
	}
	aschex_to_bcdhex(signature, 64, (char *)r);
	aschex_to_bcdhex(signature+64, 64, (char *)s);
	
	//ret = UnionSM2Sign(vkIndex, vkbyLmk,vklen,(unsigned char *)SM2DefaultUserID,strlen(SM2DefaultUserID),buf,reqinfolen,r,s); 
	//软算法
	//ret = UnionSM2SignEx(vkbyLmk,pkx,pky,(unsigned char *)SM2DefaultUserID,strlen(SM2DefaultUserID),buf,reqinfolen,r,s);
	/*
	if(ret != 0)
	{
		UnionUserErrLog("UnionSM2Sign wrong\n");
		X509_REQ_free(req);
		return -1;
	}else{
		UnionLog("UnionSM2Sign done\n");
	}
	*/
	X509_ALGOR_free(req->sig_alg);
	req->sig_alg = SM2_SM3_X509_SIG_ALGOR_new();
	ASN1_BIT_STRING_free(req->signature);
	req->signature = ECDSA_R_S_SIG_Bit_new(r,s);
		
	UnionLog("fileName:%s\n",fileName);
	// 写入文件PEM格式 
	b=BIO_new_file(fileName,"wb");
	if (fileFormat == PEM) {
		PEM_write_bio_X509_REQ(b,req);
	}
	 else {
		UnionLog("cannot write PEM file %s\n",fileName);
		i2d_X509_REQ_bio(b,req);
	}
	BIO_free(b);
	if((b=BIO_new_file(fileName,"wb")) == NULL)
	{
		UnionUserErrLog("cannot write file %s\n",fileName);
		X509_REQ_free(req);
		return -1;
	}
	 
	if (fileFormat == PEM) {
		PEM_write_bio_X509_REQ(b,req);
	}
	else {
		i2d_X509_REQ_bio(b,req);
	}
	BIO_free(b);
	 
	X509_REQ_free(req);
	   
	return 0;
}

 
// 转换证书时间为北京时间,返回time_t
static int UnionConvertCrtTimeZtoBeiJin(char *timeZ,char *bjTime)
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


int UnionGetCertSignAlgAndSignture(int reqFlag,void *pcert,int *signAlgFlag,unsigned char *signature)
{
	X509 *cer = NULL;
	X509_REQ *req = NULL;
	ASN1_OBJECT *algorObj = NULL;
	ASN1_BIT_STRING *signAsnStr = NULL;
	unsigned char derBuf[4096] = {0};
	int len = 0;
	unsigned char *p = NULL;
	char tbuf[8192] = {0};

	if(reqFlag == 1)
	{
		req = (X509_REQ *)pcert;
		if(req == NULL || req->sig_alg == NULL || req->sig_alg == NULL || req->signature == NULL )
		{
			UnionUserErrLog("in UnionGetCertSignAlgAndSignture::req para err.\n");
			return -1;
		}
		algorObj = req->sig_alg->algorithm;
		signAsnStr = req->signature;
	}
	else {
		cer = (X509 *)pcert;
		if(cer == NULL || cer->sig_alg == NULL || cer->sig_alg == NULL || cer->signature == NULL )
		{
			UnionUserErrLog("in UnionGetCertSignAlgAndSignture::cer para err.\n");
			return -1;
		}
		algorObj = cer->sig_alg->algorithm;
		signAsnStr = cer->signature;
	}
	p = derBuf;
	if((len = i2d_ASN1_OBJECT(algorObj,&p)) <= 0)
	{
		UnionUserErrLog("in UnionGetCertSignAlgAndSignture::req algor para err.\n");
		return -2;
	}
	bcdhex_to_aschex((char *)derBuf,len,tbuf);
	tbuf[len*2] = 0;
	if(strcmp(sha1WithRsaEncryptOBJ,tbuf) == 0)
	{
		*signAlgFlag = UNION_SHA1_Digest_ALG;
	}
	else if(strcmp(md5WithRsaEncryptOBJ,tbuf) == 0)
	{
		*signAlgFlag = UNION_MD5_Digest_ALG;
	}
	else if(strcmp(sm3WithSM2SignatureOBJ,tbuf) == 0)
	{
		*signAlgFlag = UNION_SM3_Digest_ALG;
	}
	//add by zhouxw 20151202
	else if(strcmp(sha256WithRsaEncryptOBJ, tbuf) == 0)
	{
		*signAlgFlag = UNION_SHA256_Digest_ALG;
	}
	//add end
	else {
		*signAlgFlag = -1;
		UnionUserErrLog("in UnionGetCertSignAlgAndSignture::req signAlgFlag [%s] not supoort.\n",tbuf);
		return -2;
	}
	 
	memcpy(signature,signAsnStr->data,signAsnStr->length);
	len = signAsnStr->length;
	 
	return len;
}


//解析证书
int UnionGetCertificateInfoFromBufEx(char *cert,int certlen,PUnionX509CerSM2 pCertInfo)
{
	X509 *cer = NULL;
	char buffer[8192];
	int len = 0;
	unsigned char pkDer[1024] = {0};
	int pkDerLen = 0;
	int ret = 0;
	BIO *pbio = NULL;
	unsigned char *pbuf = NULL;
	ASN1_BIT_STRING *signature;
	X509_NAME *pName = NULL;
	PUnionX509UserInfoSM2 pUserInfo = NULL;
	int i = 0;
	unsigned char tbuf[1024];
	BIGNUM bnum;
	int k = 0;
	unsigned char buffn[2][41];
	unsigned char signatureBuf[1024] = {0};
	int signatureBufLen = 0;

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

	BIO_free(pbio);
	pbio = NULL;

	if (cer == NULL)
	{
		UnionUserErrLog("in UnionGetCertificateInfoFromBuf::read cert[%s] certlen=[%d] failed!\n",cert,certlen);
		return errCodeOffsetOfCertMDL_FileFormat;
	}
	
	pbuf = (unsigned char *)pCertInfo->certDerBuf;
	pCertInfo->certDerBufLen = i2d_X509(cer,&pbuf);
	if(pCertInfo->certDerBufLen <= 0)
	{
		UnionUserErrLog("in UnionParasePKcs7SignEx::Get X509 failed.\n");
		return errCodeOffsetOfCertMDL_FileFormat;
	}

	pbuf = pCertInfo->tbsCertificateBuf;
	if((pCertInfo->tbsCertificateBufLen = i2d_X509_CINF(cer->cert_info,&pbuf)) <= 0)
	{
		X509_free(cer);
		UnionUserErrLog("in UnionGetCertificateInfoFromBuf::i2d_X509_CINF failed!\n");
		return errCodeOffsetOfCertMDL_FileFormat;
	}
	 
	//add end
	//公钥
	if((pkDerLen = UnionGetCertInfoAlgAndPK(0,cer,&pCertInfo->algID,pkDer)) <= 0)
	{
		X509_free(cer);
		UnionUserErrLog("in UnionGetCertificateInfo::UnionGetCertInfoAlgAndPK failed!\n");
		return errCodeOffsetOfCertMDL_FileFormat;
	}
	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)pkDer,pkDerLen,(char *)buffer);
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

		k = 0;
		memset(buffer,0,sizeof(buffer));
		ret = X509_NAME_get_text_by_NID(pName, NID_commonName, buffer, sizeof(buffer));
		if (ret >= 1)  {
				UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)pUserInfo->commonName,sizeof(pUserInfo->commonName));
				sprintf(buffer,"CN=%s",pUserInfo->commonName);
				if(i==0) {
					strcat(pCertInfo->issuserDN,buffer);
					
				}
				else {
					strcat(pCertInfo->userDN,buffer);
					
				}
				k++;
		}

		memset(buffer,0,sizeof(buffer));
		ret = X509_NAME_get_text_by_NID(pName, NID_pkcs9_emailAddress, buffer, sizeof(buffer));
		if (ret >= 1)  {
				UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)pUserInfo->email,sizeof(pUserInfo->email));
				if(k > 0)
					sprintf(buffer,",E=%s",pUserInfo->email);
				else
					sprintf(buffer,"E=%s",pUserInfo->email);
				if(i==0) {
					strcat(pCertInfo->issuserDN,buffer);
				}
				else {
					strcat(pCertInfo->userDN,buffer);
				}
				k++;
		}

		memset(buffn,0,sizeof(buffn));
		ret = X509_NAME_get_text_by_NID_Ex(pName, NID_organizationalUnitName, buffn, 2);
		if (ret >= 2) {
				len = strlen((char *)buffn[1]);
				//UnionUtf8ToAnsi((unsigned char *)&buffn[1][1],len,(unsigned char *)pUserInfo->organizationalUnitName2,sizeof(pUserInfo->organizationalUnitName2));
				UnionUtf8ToAnsi((unsigned char *)&buffn[1][0],len,(unsigned char *)pUserInfo->organizationalUnitName2,sizeof(pUserInfo->organizationalUnitName2));
				if(k > 0)
				{   
					sprintf(buffer,",OU=%s",pUserInfo->organizationalUnitName2);
					}
				else
				{ 
					sprintf(buffer,"OU=%s",pUserInfo->organizationalUnitName2);
					}
				if(i==0) {
					strcat(pCertInfo->issuserDN,buffer);
				}
				else {
					strcat(pCertInfo->userDN,buffer);
				}
				k++;
		}
		if (ret >= 1) {
				len = strlen((char *)buffn[0]);
				//UnionUtf8ToAnsi((unsigned char *)&buffn[0][1],len,(unsigned char *)pUserInfo->organizationalUnitName,sizeof(pUserInfo->organizationalUnitName));
                UnionUtf8ToAnsi((unsigned char *)&buffn[0][0],len,(unsigned char *)pUserInfo->organizationalUnitName,sizeof(pUserInfo->organizationalUnitName));

				if(k > 0)
				{ 
					sprintf(buffer,",OU=%s",pUserInfo->organizationalUnitName);
					}
				else
				{
					sprintf(buffer,"OU=%s",pUserInfo->organizationalUnitName);
					}
				if(i==0) {
					strcat(pCertInfo->issuserDN,buffer);
				}
				else {
					strcat(pCertInfo->userDN,buffer);
				}
				k++;
		}
		memset(buffn,0,sizeof(buffn));
		ret = X509_NAME_get_text_by_NID_Ex(pName, NID_organizationName, buffn, 2);
		if (ret >= 2)
		{
				len = strlen((char *)buffn[1]);
				//UnionUtf8ToAnsi((unsigned char *)&buffn[1][1],len,(unsigned char *)pUserInfo->organizationName2,sizeof(pUserInfo->organizationName2));
				UnionUtf8ToAnsi((unsigned char *)&buffn[1][0],len,(unsigned char *)pUserInfo->organizationName2,sizeof(pUserInfo->organizationName2));
				if(k > 0)
				{ 
					sprintf(buffer,",O=%s",pUserInfo->organizationName2);
					}
				else
				{  
					sprintf(buffer,"O=%s",pUserInfo->organizationName2);
					}
				if(i==0) {
					strcat(pCertInfo->issuserDN,buffer);
				}
				else {
					strcat(pCertInfo->userDN,buffer);
				}
				k++;
		}
		 
		if (ret >= 1)  {
				len = strlen((char *)buffn[0]);
				//UnionUtf8ToAnsi((unsigned char *)&buffn[0][1],len,(unsigned char *)pUserInfo->organizationName,sizeof(pUserInfo->organizationName));
                UnionUtf8ToAnsi((unsigned char *)&buffn[0][0],len,(unsigned char *)pUserInfo->organizationName,sizeof(pUserInfo->organizationName));
				if(k > 0)
				{ 
					sprintf(buffer,",O=%s",pUserInfo->organizationName);
					}
				else
				{ 	sprintf(buffer,"O=%s",pUserInfo->organizationName);
					}
				if(i==0) {
					strcat(pCertInfo->issuserDN,buffer);
				}
				else {
					strcat(pCertInfo->userDN,buffer);
				}
				k++;
		}
		
		memset(buffer,0,sizeof(buffer));
		ret = X509_NAME_get_text_by_NID(pName, NID_localityName, buffer, sizeof(buffer));
		if (ret >= 1)  {
				UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)pUserInfo->localityName,sizeof(pUserInfo->localityName));
				if(k > 0)
					sprintf(buffer,",L=%s",pUserInfo->localityName);
				else
					sprintf(buffer,"L=%s",pUserInfo->localityName);
				if(i==0) {
					strcat(pCertInfo->issuserDN,buffer);
				}
				else {
					strcat(pCertInfo->userDN,buffer);
				}
				k++;
		}

		memset(buffer,0,sizeof(buffer));
		ret = X509_NAME_get_text_by_NID(pName, NID_stateOrProvinceName, buffer, sizeof(buffer));
		if (ret >= 1)  {
				UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)pUserInfo->stateOrProvinceName,sizeof(pUserInfo->stateOrProvinceName));
				if(k > 0)
					sprintf(buffer,",S=%s",pUserInfo->stateOrProvinceName);
				else
					sprintf(buffer,"S=%s",pUserInfo->stateOrProvinceName);
				if(i==0) {
					strcat(pCertInfo->issuserDN,buffer);
				}
				else {
					strcat(pCertInfo->userDN,buffer);
				}
				k++;
		}

		memset(buffer,0,sizeof(buffer));
		ret = X509_NAME_get_text_by_NID(pName, NID_countryName, buffer, sizeof(buffer));
		if (ret >= 1)  {
				UnionUtf8ToAnsi((unsigned char *)buffer,ret,(unsigned char *)pUserInfo->countryName,sizeof(pUserInfo->countryName));
				if(k > 0)
					sprintf(buffer,",C=%s",pUserInfo->countryName);
				else
					sprintf(buffer,"C=%s",pUserInfo->countryName);
				
				if(i==0) {
					strcat(pCertInfo->issuserDN,buffer);
				}
				else {
					strcat(pCertInfo->userDN,buffer);
				}
				k++;
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
	//pCertInfo->serialNumber = ASN1_INTEGER_get(cer->cert_info->serialNumber);
	 
	BN_init(&bnum);
	ASN1_INTEGER_to_BN(cer->cert_info->serialNumber,&bnum);
	memset(tbuf,0,sizeof(tbuf));
	len = BN_bn2bin(&bnum,tbuf);
	BN_clear_free(&bnum);
	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)tbuf,len,buffer);
	strcpy(pCertInfo->serialNumberAscBuf,buffer);
  
	//摘要算法标记
	if((signatureBufLen = UnionGetCertSignAlgAndSignture(1,cer,&pCertInfo->hashID,signatureBuf)) < 0)
	{
		X509_free(cer);
		UnionUserErrLog("UnionGetCertSignAlgAndSignture failed!\n");
		return signatureBufLen;
	}
	 
	// 签名
	signature = cer->signature;
	pbuf = signature->data;
	len = signature->length;
	memset(buffer,0,sizeof(buffer));
	bcdhex_to_aschex((char *)pbuf,len,(char *)buffer);
	strcpy(pCertInfo->sign,(char *)buffer);
	X509_free(cer);
	 
	return 1;
}


int UnionGetRSOfSM2Signature(unsigned char *derSignature,int len,unsigned char r[32],unsigned char s[32])
{
	ECDSA_SIG *sig = NULL;
	char *pBuf = NULL;
	const unsigned char *p = derSignature;
	sig = ECDSA_SIG_new();
	if(d2i_ECDSA_SIG(&sig,&p,len) == NULL)
	{
		UnionUserErrLog("in UnionGetRSOfSM2Signature :: d2i_ECDSA_SIG failed!\n");
		return -1;
	}
	pBuf = BN_bn2hex(sig->r);
	aschex_to_bcdhex(pBuf,64,(char *)r);
	OPENSSL_free(pBuf);
	pBuf = BN_bn2hex(sig->s);
	aschex_to_bcdhex(pBuf,64,(char *)s);
	OPENSSL_free(pBuf);
	return 0;
}


/*
功能：验证看是否为(平台)自己颁发的证书串
输入：
	issuerPK：发证机构的公钥
	cert:   要认证的证书字符串
	certlen: cert长度
返回:
	>= 0 成功
	< 0 失败
*/
int UnionVerifyCertificateWithPKEx(char *issuerPK,char *cert,int certlen)
{
	//char pkX[65] = {0};
	//char pkY[65] = {0};
	unsigned char r[32];
	unsigned char s[32];
	unsigned char plaintxt[1024] = {0};
	unsigned char hashVal[100] = {0};
	int len = 0;
	int hashValLen = 0;
	int plaintxtLen = 0;
	TUnionX509CerSM2  tx509Cert = {0};
	int ret = 0;
	unsigned char signature[1024] = {0};
	int signatureLen = 0;
	char	pk[128];
	char	ascsign[128];
	char	ascTbsCertificateBuf[4096];
	if((ret = UnionGetCertificateInfoFromBufEx(cert,certlen,&tx509Cert)) < 0)
	{
		UnionUserErrLog("in UnionVerifyCertificateWithPKEx::UnionGetCertificateInfoFromBuf failed!\n");
		return ret;
	}
//	UnionLog("in UnionVerifyCertificateWithPKEx![1] \n");
	if(tx509Cert.algID == UNION_SM2_ALG) 
	{
		if(tx509Cert.hashID != UNION_SM3_Digest_ALG || strncmp(tx509Cert.derPK,"04",2) != 0 || strlen(tx509Cert.derPK) != 130)
		{
			UnionUserErrLog("in UnionVerifyCertificateWithPKEx:: para err!\n");
			return -1;
		}
		 
		signatureLen = aschex_to_bcdhex(tx509Cert.sign,strlen(tx509Cert.sign),(char *)signature);
		if(UnionGetRSOfSM2Signature(signature,signatureLen,r,s) != 0)
		{
			UnionUserErrLog("in UnionVerifyCertificateWithPKEx::UnionGetRSOfSM2Signature err!\n");
			return -2;
		}
		//aschex_to_bcdhex(issuerPK+2,64,pkX);
		//aschex_to_bcdhex(issuerPK+2+64,64,pkY);

		//直接调用K4指令
		memcpy(pk, issuerPK+2, 128);
		bcdhex_to_aschex((char *)r, 32, ascsign);
		bcdhex_to_aschex((char *)s, 32, ascsign+64);
		bcdhex_to_aschex((char *)tx509Cert.tbsCertificateBuf, tx509Cert.tbsCertificateBufLen, ascTbsCertificateBuf);
		if((ret = UnionHsmCmdK4(-1, "02", strlen(SM2DefaultUserID), SM2DefaultUserID, 128, pk, 128, ascsign, tx509Cert.tbsCertificateBufLen*2, ascTbsCertificateBuf)) < 0)
		{
			UnionUserErrLog("in UnionVerifyCertificateWithPKEx:: UnionHsmCmdK4 return[%d]\n", ret);
			return(ret);
		}
		return(0);

		//return UnionSM2Verify((unsigned char *)pkX,(unsigned char *)pkY,(unsigned char *)SM2DefaultUserID,strlen(SM2DefaultUserID),tx509Cert.tbsCertificateBuf,tx509Cert.tbsCertificateBufLen,r,s);
	}
	else if(tx509Cert.algID  == UNION_RSA_ALG) 
	{
		const EVP_MD   *md = NULL;
		char *str = NULL;
		signatureLen = aschex_to_bcdhex(tx509Cert.sign,strlen(tx509Cert.sign),(char *)signature);
		if((plaintxtLen = UnionPKDecByPKCS1(signature,signatureLen,issuerPK,plaintxt)) < 0)
		{
			UnionUserErrLog("in UnionVerifyCertificateWithPKEx::UnionGetRSOfSM2Signature err!\n");
			return -2;
		}
		if(tx509Cert.hashID == UNION_MD5_Digest_ALG)
		{
			md = EVP_md5();
			str = (char *)UnionGetPrefixOfMD5OID();
			len = aschex_to_bcdhex(str,strlen(str),(char *)hashVal);
		}
		else if(tx509Cert.hashID  == UNION_SHA1_Digest_ALG)
		{
			md = EVP_sha1();
			str = (char *)UnionGetPrefixOfSHA1OID();
			len = aschex_to_bcdhex(str,strlen(str),(char *)hashVal);
		}
		// add by zhouxw 20151204
		else if(tx509Cert.hashID == UNION_SHA256_Digest_ALG)
		{
			md = EVP_sha256();
			str = (char *)UnionGetPrefixOfSHA256OID();
			len = aschex_to_bcdhex(str,strlen(str),(char *)hashVal);
		}
		// add end
		else {
			UnionUserErrLog("not support this signAlgFlag[%d]!\n",tx509Cert.hashID );
			return -1;
		}
		EVP_Digest(tx509Cert.tbsCertificateBuf,tx509Cert.tbsCertificateBufLen, hashVal+len,(unsigned int *)&hashValLen,md, NULL);
		hashValLen += len;
		if(hashValLen == plaintxtLen && memcmp(plaintxt,hashVal,hashValLen) == 0)
		{
			UnionLog("UnionVerifyCertificateWithPKEx OK!\n");
			return 0;
		}
		else {
			UnionUserErrLog("UnionVerifyCertificateWithPKEx failed!\n");
			return -2;
		}
	}
	else {
		UnionUserErrLog("in UnionVerifyCertificateWithPKEx::not support this algFlag[%d]!\n",tx509Cert.algID);
		return -1;
	}
}



/*
函数功能: 拼装SM2 PKCS#7格式的签名(不包含原始数据)
输入参数：
	sign:  签名(r+s)
	x509Data: 公钥证书数据
	x509DataLen: 公钥证书数据长度
	caCertData: CA根证书数据,PKCS#7签名文件不包含CA根证书
	x509DataLen: 根证书数据长度
	formatFlag: 0-base64 1-二进制格式
输出参数：
	pkcs7Sign: pkcs#7格式的签名
返回：>0 成功， 签名的长度
	  < 0 失败
*/
int UnionFormSM2Pkcs7SignDetach(unsigned char sign[64],char *x509Data,int x509DataLen,char *caCertData,int caCertDataLen, char *pkcs7Sign,int sizeofPkcs7Sign,int formatFlag)
{
	BIO *pbio = NULL;
	PKCS7* p7 = NULL;
	PKCS7_SIGNER_INFO* info = NULL;
	int i = 0;
	BIO *p7bio = NULL;
	STACK_OF(X509) *ca = NULL; 
	X509 *x509=NULL;
	X509 *caCert = NULL;
	int ret = 0;
	//char tbuf[CERT_MAX_LEN+1]; 
	char buffer[8192];
	char *tbuf = NULL; 
	const int tbufLen = 1024*1024*4+1;
	int algFlag = 0;
	unsigned char pk[1025] = {0};
	unsigned char r[32],s[32];
	BUF_MEM *bptr = NULL;
	unsigned char *pBuf = NULL;
	unsigned char derBuf[8192] = {0};
	int len = 0;
	STACK_OF(PKCS7_SIGNER_INFO) *si_sk=NULL;


	if ( x509Data == NULL ||  pkcs7Sign == NULL )
	{
		UnionUserErrLog("in UnionFormSM2Pkcs7SignDetach::params err!\n");
		return errCodeOffsetOfCertMDL_CodeParameter;
	}
	UnionLoadLoadOpenSSLAlg();  
	 

	memset(buffer,0,sizeof(buffer));
	UnionPemStrPreHandle(x509Data,buffer,"CERTIFICATE",8192);
	pbio = BIO_new_mem_buf(buffer,strlen(buffer));
	if((x509 = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_mem_buf(x509Data,x509DataLen);
		if((x509 = PEM_read_bio_X509(pbio,NULL,NULL,NULL)) == NULL)
		{
		 	pbio = BIO_new_mem_buf(x509Data,x509DataLen);
			x509 = d2i_X509_bio(pbio,NULL);
		}
	}

	BIO_free(pbio);
	pbio = NULL;
	 
	if (x509 == NULL)
	{
		UnionUserErrLog("in UnionFormSM2Pkcs7SignDetach::x509 format err!\n");
		return errCodeOffsetOfCertMDL_FileFormat;
	}

	if(UnionGetCertInfoAlgAndPK(0,x509,&algFlag,pk) < 0)
	{
		UnionUserErrLog("in UnionFormSM2Pkcs7SignDetach::open sk_X509_new_null failure\n");
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}
	
	if ((ca=sk_X509_new_null()) == NULL)
	{
			UnionUserErrLog("in UnionFormSM2Pkcs7SignDetach::open sk_X509_new_null failure\n");
			ret = errCodeOffsetOfCertMDL_OpenSSl;
			goto cleanup;
	}

	tbuf = (char *)malloc(sizeof(char)*tbufLen);
	if(!tbuf)
	{
			UnionUserErrLog("in UnionFormSM2Pkcs7SignDetach::malloc failed\n");
			ret = -1;
			goto cleanup;
	}
	if (caCertData != NULL && caCertDataLen > 0)
	{
		pbio = BIO_new_mem_buf(caCertData,caCertDataLen);
		caCert = d2i_X509_bio(pbio,NULL);
			
		if (caCert == NULL)
		{
			UnionUserErrLog("in UnionFormSM2Pkcs7SignDetach::get caCert failed!\n");
			ret = errCodeOffsetOfCertMDL_FileFormat;
			goto cleanup;
		}
		if (!sk_X509_push(ca,caCert))
		{
			UnionUserErrLog("in UnionFormSM2Pkcs7SignDetach::sk_X509_push failed!\n");
			ret = errCodeOffsetOfCertMDL_OpenSSl;
			goto cleanup;
		}
		// add
		BIO_free(pbio);
		pbio = NULL;
		caCert = NULL;
		// end add 
	}

	p7 = PKCS7_new();
	 
	Union_SM2_PKCS7_SET_SIGNED_Type(p7);
	Union_SM2_PKCS7_content_new_data(p7);

	Union_SM2_PKCS7_set_detached(p7,1);

	//添加签名者信息，
	//x509：签名证书
	info = Union_SM2_PKCS7_add_signature(p7, x509);
	 
	//添加签名者证书
	Union_SM2_PKCS7_add_sign_certificate(p7,x509);

	//添加签名者的CA证书链
	for (i=0; i<sk_X509_num(ca); i++)
	{
		Union_SM2_PKCS7_add_sign_certificate(p7, sk_X509_value(ca, i));
	}

	memcpy(r,sign,32);
	memcpy(s,sign+32,32);
	  
	si_sk = p7->d.sign->signer_info;
	info=sk_PKCS7_SIGNER_INFO_value(si_sk,0);
	ASN1_OCTET_STRING_free(info->enc_digest);
	info->enc_digest = ECDSA_R_S_SIG_OCTET_new(r,s);
	  
	pBuf = derBuf;
	len = i2d_PKCS7_Ex(p7,&pBuf);
	if(formatFlag == 1)
	{
		if (len > sizeofPkcs7Sign)
		{
			UnionUserErrLog("in UnionFormSM2Pkcs7SignDetach::sizeofPkcs7Sign[%d] too small!\n",sizeofPkcs7Sign);
			ret = errCodeOffsetOfCertMDL_SmallBuffer;
			goto cleanup;
		}
		memcpy(pkcs7Sign,derBuf,len);
		ret = len;
	}
	else {
		int outlen = 0;
		outlen = (len+2)/3*4;
		
		if (outlen  > sizeofPkcs7Sign)
		{
			UnionUserErrLog("in UnionFormSM2Pkcs7SignDetach::sizeofPkcs7Sign[%d] too small!\n",sizeofPkcs7Sign);
			ret = errCodeOffsetOfCertMDL_SmallBuffer;
			goto cleanup;
		}
		to64frombits((unsigned char *)pkcs7Sign,derBuf,len); 
	 
		ret = strlen(pkcs7Sign);
	}
	
cleanup:
	if(tbuf != NULL)
	{
		free(tbuf);
		tbuf=NULL;
	}
	if(bptr != NULL) 
	{
		BUF_MEM_free(bptr);
		bptr = NULL;
	}
	if (pbio != NULL) BIO_free(pbio);
	if (x509 != NULL)  X509_free(x509);
	if (caCert != NULL) X509_free(caCert);   
	if (p7 != NULL) PKCS7_free(p7);
	if (p7bio != NULL) BIO_free(p7bio);
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


int UnionGetSignType(ASN1_OBJECT *type)
{
	unsigned char derBuf[512] = {0};
	char buf[1024] = {0};
	unsigned char *p = NULL;
	int len = 0;

	p = derBuf;
	len = i2d_ASN1_OBJECT(type,&p);
	if(len < 0)
		return -1;
	bcdhex_to_aschex((char *)derBuf,len,buf);
	buf[len*2] = 0;
	if(strcmp(buf,PKCS7_SIGNED_DATA_TYPE_DER) == 0)
	{
		return UNION_SIGN_DATA_TYPE;
	}
	else if(strcmp(buf,SM2_PKCS7_SIGNED_DATA_TYPE_DER) == 0)
	{
		return UNION_SM2_SIGN_DATA_TYPE;
	}
	else
	    return -2;
}


 
int UnionGetDigestAlgType(X509_ALGOR *xa)
{
	unsigned char derBuf[512] = {0};
	char buf[1024] = {0};
	unsigned char *p = NULL;
	int len = 0;

	p = derBuf;
	len = i2d_ASN1_OBJECT(xa->algorithm,&p);
	if(len < 0) {
		UnionUserErrLog("in UnionGetDigestAlgType::i2d_ASN1_OBJECT failed.\n");
		return -1;
	}
	bcdhex_to_aschex((char *)derBuf,len,buf);
	buf[len*2] = 0;
	if(strcmp(buf,MD5_ALG_OBJ_DER) == 0)
	{
		return UNION_MD5_Digest_ALG;
	}
	else if(strcmp(buf,SHA1_ALG_OBJ_DER) == 0)
	{
		return UNION_SHA1_Digest_ALG;
	}
	else if(strcmp(buf,SM3_HASH_ALG_WITH_KEY_DER) == 0)
	{
		return UNION_SM3_Digest_ALG;
	}
	else if(strcmp(buf,SM3_HASH_ALG_DER) == 0)  
	{
		return UNION_SM3_Digest_ALG;
	}
	else {
		UnionUserErrLog("in UnionGetDigestAlgType::not support digest alg [%s].\n",buf);
	    return -3;
	}
}

int UnionGetEncDigestAlgType(X509_ALGOR *xa)
{
	unsigned char derBuf[512] = {0};
	char buf[1024] = {0};
	unsigned char *p = NULL;
	int len = 0;
	p = derBuf;
	len = i2d_X509_ALGOR(xa,&p);
	if(len < 0) {
		UnionUserErrLog("in UnionGetEncDigestAlgType::i2d_ASN1_OBJECT failed.\n");
		return -1;
	}
	bcdhex_to_aschex((char *)derBuf,len,buf);
	buf[len*2] = 0;
	if(strcmp(buf,CertRSAEncryptionAlg) == 0)
	{
		return UNION_RSA_ALG;
	}
	else if(strcmp(buf,CertSM2SignAlg) == 0 || strcmp(buf,CertSM2SignAlgAndNull) == 0 
		|| strcmp(buf,CertSM2WithSM3Alg) == 0 || strcmp(buf,CertSM2WithSM3AlgAndNull) == 0   //add in 2015.9.10
		)
	{
		return UNION_SM2_ALG;
	}
	else {
		UnionUserErrLog("in UnionGetEncDigestAlgType::not support encdigest alg [%s].\n",buf);
	    return -3;
	}
}


int UnionGetRSFromSM2SigDer(unsigned char *signatureDer,int signatureDerLen,unsigned char rs[64])
{
	ECDSA_SIG *sig = NULL;
	unsigned char *p = NULL;
	char buf[256] = {0};
	sig = ECDSA_SIG_new();
	p = signatureDer;
	if(d2i_ECDSA_SIG(&sig,(const unsigned char **)&p,signatureDerLen) == NULL)
	{
		ECDSA_SIG_free(sig);
		return -1;
	}
	memset(buf,0,sizeof(buf));
	p = (unsigned char *)BN_bn2hex(sig->r);
	strcpy(buf,(char *)p);
	aschex_to_bcdhex(buf,64,(char *)rs);
	OPENSSL_free(p);
	memset(buf,0,sizeof(buf));
	p = (unsigned char *)BN_bn2hex(sig->s);
	strcpy(buf,(char *)p);
	aschex_to_bcdhex(buf,64,(char *)rs+32);
	OPENSSL_free(p);
	ECDSA_SIG_free(sig);
	return 64;
}


int UnionRSToSM2SigDer(unsigned char rs[64],unsigned char *signatureDer,int *signatureDerLen)
{
	ECDSA_SIG *sig = NULL;
	BIGNUM *tr,*ts;
	char rHex[65] = {0};
	char sHex[65] = {0};
	unsigned char *p = NULL;
	unsigned char derBuf[256] = {0};
	int len = 0;
	sig = ECDSA_SIG_new();
	 
	bcdhex_to_aschex((char *)rs,32,rHex);
	bcdhex_to_aschex((char *)rs+32,32,sHex);
	tr = BN_new();
	ts = BN_new();
	if (!BN_hex2bn(&tr, rHex))
	{
		BN_free(tr);
		BN_free(ts);
		ECDSA_SIG_free(sig);
		return 0;
	}
	if (!BN_hex2bn(&ts, sHex))
	{
		BN_free(tr);
		BN_free(ts);
		ECDSA_SIG_free(sig);
		return 0;
	}
	sig->r = tr;
	sig->s = ts;
	p = derBuf;
	if((len = i2d_ECDSA_SIG(sig,&p)) < 0)
	{
		BN_free(tr);
		BN_free(ts);
		ECDSA_SIG_free(sig);
		return len;
	}
	memcpy(signatureDer, derBuf, len);
	*signatureDerLen = len;
	ECDSA_SIG_free(sig); 
	return len;
}

/*
函数功能: 解析KCS#7格式的签名
输入:
	pkcs7Sign: KCS#7格式字符串
	pkcs7Signlen: pkcs7Sign的长度
输出:
	cert: Der格式的签名者证书
	certLen:证书长度
	sign: 签名
	signlen: 签名长度
	data: 原始数据
	datalen: 原始数据长度,注意一些pkcs7的签名文件里不含原始签名数据，此时的datalen为0
	signType:签名类型, 0--国际，1--国密
	algID:算法ID 0--RSA,1--SM2
	hashID: 签名时用的摘要算法 0--MD5,1--SHA1,2--SM3
返回:
	>= 0 解析KCS#7格式的签名成功
	< 0 失败
*/
int UnionParasePKcs7SignEx(unsigned char *pkcs7Sign,int pkcs7Signlen,unsigned char *cert,int *certLen,unsigned char *sign,int *signlen,unsigned char *data,int *datalen,int *signType,int *algID,int *hashID)
{
	BIO *pbio = NULL;
	PKCS7* p7 = NULL;
	BIO *p7bio = NULL;
	X509 *x509=NULL;
	int ret = 0;
	char tbuf[8192+1] = {0}; 
	unsigned char *pBuf = NULL;
	 
	//char buffer[4096];
	char *buffer = NULL;
	STACK_OF(PKCS7_SIGNER_INFO) *sk = NULL;
	int signCount = 0;
	PKCS7_SIGNER_INFO *signInfo = NULL;
	ASN1_STRING	*asndata = NULL;
	ASN1_OCTET_STRING	*encDigest = NULL;
	X509_ALGOR *xa = NULL;
	STACK_OF(X509_ALGOR) *md_sk=NULL;
	int buflen=0;
	int myret = 0;

	if (pkcs7Sign == NULL || cert == NULL || sign == NULL || hashID == NULL )
	{
		UnionUserErrLog("in UnionParasePKcs7SignEx::params err!\n");
		return errCodeOffsetOfCertMDL_CodeParameter;
	}
	UnionLoadLoadOpenSSLAlg();  
	
	// 读pkcs7
	buflen=pkcs7Signlen*2+1;
	buffer =(char *)malloc(sizeof(char)*buflen);
	if(!buffer)
	{
		UnionUserErrLog("in UnionParasePKcs7SignEx::malloc failed!\n");
		return -1;
	}
	memset(buffer,0,buflen);
	 
	myret=UnionPemStrPreHandle((char *)pkcs7Sign,buffer,"PKCS7",buflen);
	UnionLog("in UnionParasePKcs7SignEx::UnionPemStrPreHandle ret=%d\n",myret);
	pbio = BIO_new_mem_buf(buffer,strlen(buffer));
	 
	if((p7 = (PKCS7 *)PEM_ASN1_read_bio((d2i_of_void *)d2i_PKCS7_Ex,"PKCS7",pbio,NULL,NULL,NULL)) == NULL)
	{
		BIO_free(pbio);
		pbio = NULL;
		pbio = BIO_new_mem_buf(pkcs7Sign,pkcs7Signlen);
		if((p7 = (PKCS7 *)PEM_ASN1_read_bio((d2i_of_void *)d2i_PKCS7_Ex,"PKCS7",pbio,NULL,NULL,NULL)) == NULL)
		{
			BIO_free(pbio);
			pbio = NULL;
			pBuf = pkcs7Sign;
			p7 = d2i_PKCS7_Ex(NULL,(const unsigned char **)&pBuf,pkcs7Signlen);
		}
	}
	 
	if (p7 == NULL || p7->type == NULL || p7->d.sign->version == NULL)
	{
		UnionUserErrLog("in UnionParasePKcs7SignEx::get p7 failed!\n");
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}
	 
	ret = UnionGetSignType(p7->type);
	if(ret != UNION_SIGN_DATA_TYPE && ret != UNION_SM2_SIGN_DATA_TYPE)
	{
		UnionUserErrLog("in UnionParasePKcs7SignEx::p7->type is err!\n");
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}
	*signType =  ret; 
	if(pbio) {
		BIO_free(pbio);
		pbio = NULL;
	}
	 
	//读取原始数据
	asndata = p7->d.sign->contents->d.data;
	if(datalen)
		*datalen = 0;
	if (asndata != 0)
	{
		if (data != NULL)
		{
			memcpy(data,asndata->data,asndata->length);
			*datalen = asndata->length;
		}
	}
 
	md_sk=p7->d.sign->md_algs;
	xa=sk_X509_ALGOR_value(md_sk,0);
	if(xa == NULL)
	{
		UnionUserErrLog("in UnionParasePKcs7SignEx::digest alg is null!\n");
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}
 
	//获得签名者信息stack
	sk = p7->d.sign->signer_info;
	//获得签名者个数(本例只有1个)
	signCount = sk_PKCS7_SIGNER_INFO_num(sk);
	if (signCount != 1) {
		UnionUserErrLog("in UnionParasePKcs7SignEx::signCount=[%d]\n",signCount);
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}
	
	//获得签名者信息
	signInfo = sk_PKCS7_SIGNER_INFO_value(sk,0);
	encDigest = signInfo->enc_digest;
	
	ret = UnionGetDigestAlgType(signInfo->digest_alg);
	if(ret != UNION_MD5_Digest_ALG && ret != UNION_SHA1_Digest_ALG && ret != UNION_SM3_Digest_ALG
		&& ret != UNION_SHA224_Digest_ALG && ret != UNION_SHA256_Digest_ALG
		&& ret != UNION_SHA384_Digest_ALG && ret != UNION_SHA512_Digest_ALG
		)
	{
		UnionUserErrLog("in UnionParasePKcs7SignEx::digest alg para err!\n");
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}
	*hashID = ret; 
	
	ret = UnionGetEncDigestAlgType(signInfo->digest_enc_alg);
	if(ret != UNION_RSA_ALG && ret != UNION_SM2_ALG)
	{
		UnionUserErrLog("in UnionParasePKcs7SignEx::encdigest err!\n");
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}
	*algID = ret;
	UnionLog("in UnionParasePKcs7SignEx::algID=[%d].\n",*algID);

	if(*algID == UNION_RSA_ALG)
	{
		*signlen = encDigest->length;
		memcpy(sign,encDigest->data,*signlen);
	}
	else {
		if((ret = UnionGetRSFromSM2SigDer(encDigest->data,encDigest->length,sign)) < 0)
		{
			UnionUserErrLog("in UnionParasePKcs7SignEx::UnionGetRSFromSM2SigDer failed.\n");
			ret = errCodeOffsetOfCertMDL_FileFormat;
			goto cleanup;
		}
		*signlen = ret;
	}
	 
	//获得签名者证书
	x509 = X509_find_by_issuer_and_serial(p7->d.sign->cert,
			signInfo->issuer_and_serial->issuer,
			signInfo->issuer_and_serial->serial);
 
	// 将X509证书，保存到缓冲
	/*
	pbio = BIO_new( BIO_s_mem() ); 
	i2d_X509_bio(pbio,x509);
	BIO_get_mem_ptr(pbio, &bptr); 
	
	memcpy(cert, bptr->data, bptr->length); 
	*certLen = bptr->length;
	BUF_MEM_free(bptr);
	bptr = NULL;

	pbio->ptr = NULL;
	BIO_free(pbio);
	pbio = NULL;
	*/
	pBuf = (unsigned char *)tbuf;
	*certLen = i2d_X509(x509,&pBuf);
	if(*certLen <= 0)
	{
		UnionUserErrLog("in UnionParasePKcs7SignEx::Get X509 failed.\n");
		ret = errCodeOffsetOfCertMDL_FileFormat;
		goto cleanup;
	}
  memcpy(cert,tbuf, *certLen); 
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

int UnionGetSerialNumberAscBufFromCRLFile(char *tCRLFileName)
{
	int i;
	int iret=0;
	BIO *pbio = NULL;
	X509_CRL *x509_crl=NULL;
	STACK_OF(X509_REVOKED) *revoked = NULL;
	int num = 0;
	X509_REVOKED *rc = NULL;
	//ASN1_INTEGER *serial = NULL;
	
	char            buffer[8192];
	unsigned char   tbuf[1024];
	BIGNUM          bnum;
	int             len = 0;
	char            serialNumberAscBuf[128];
	
	UnionLoadLoadOpenSSLAlg();
	/*读取CRL文件*/
	pbio = BIO_new_file(tCRLFileName,"rb");
	x509_crl = d2i_X509_CRL_bio(pbio,NULL);
	if(x509_crl == NULL)
	{
		BIO_free(pbio);
		pbio = BIO_new_file(tCRLFileName,"rb");
		x509_crl = PEM_read_bio_X509_CRL(pbio, NULL, NULL, NULL);
		if(x509_crl == NULL)
		{
			goto cleanup;
		}
	}

	BIO_free(pbio);
	pbio = NULL;
	
	revoked = x509_crl->crl->revoked;
	
	//num=revoked->num;
	num = sk_X509_REVOKED_num(revoked);
	for(i=0;i<num;i++)
	{
		rc=sk_X509_REVOKED_pop(revoked);
		BN_init(&bnum);
		ASN1_INTEGER_to_BN(rc->serialNumber, &bnum);
		memset(tbuf,0,sizeof(tbuf));
		len = BN_bn2bin(&bnum,tbuf);
		BN_clear_free(&bnum);
		memset(buffer,0,sizeof(buffer));
		bcdhex_to_aschex((char *)tbuf,len,buffer);
		strcpy(serialNumberAscBuf,buffer);
	}

cleanup:
	if (pbio != NULL) BIO_free(pbio);
	if (x509_crl != NULL) X509_CRL_free(x509_crl);
	return(iret);
}
