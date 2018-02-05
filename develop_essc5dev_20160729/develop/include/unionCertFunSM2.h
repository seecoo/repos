#ifndef _UNION_CERT_FUN_SM2_H
#define _UNION_CERT_FUN_SM2_H

#define SM2DefaultUserID "1234567812345678"

//文件格式
#define PEM		0     
#define DER		1
#define UNION_RSA_ALG 0
#define UNION_SM2_ALG 1
#define UNION_MD5_Digest_ALG 0
#define UNION_SHA1_Digest_ALG 1

#define UNION_SHA224_Digest_ALG 224
#define UNION_SHA256_Digest_ALG 256
#define UNION_SHA384_Digest_ALG 384
#define UNION_SHA512_Digest_ALG 512

#define UNION_SM3_Digest_ALG 2
#define UNION_SIGN_DATA_TYPE 0
#define UNION_SM2_SIGN_DATA_TYPE 1
#define UNION_Digest_END  9   //已预做了摘要处理

// 证书用户信息结构
typedef struct {
	char countryName[40];   //国家名
	char stateOrProvinceName[40];  //省名
	char localityName[40];		//城市名
	char organizationName[40];  //单位名
	char organizationName2[40];  //单位名
	char organizationalUnitName[40]; //部门
	char organizationalUnitName2[40]; //部门
	char commonName[80];		//用户名
	char email[80];				//EMail地址
	char alias[80];			//别名
} TUnionX509UserInfoSM2;

typedef TUnionX509UserInfoSM2 *PUnionX509UserInfoSM2;


//证书请求结构
typedef struct {
	int version;			//版本号
	TUnionX509UserInfoSM2	reqUserInfo;   //请求用户信息
	int	 hashID;		//摘要算法标记 0--MD5,1--SHA1
	int  alg;           //公钥算法标识 0--RSA 1---SM2
	char derPK[2048];	//der格式公钥
	char sign[2048];	//请求信息的签名
} TUnionP10CerReqSM2;

typedef TUnionP10CerReqSM2		*PUnionP10CerReqSM2;

//证书结构
typedef struct {
	int version;		 //证书版本号
	char issuserDN[256];
	char userDN[256];
	TUnionX509UserInfoSM2  issuerInfo;   //发布证书者的用户信息
	TUnionX509UserInfoSM2	userInfo;	//证书拥有者信息
	int	 hashID;		//摘要算法标记 0--MD5,1--SHA1, 2--SM3
	int  algID;           //公钥算法标识 0--RSA 1---SM2
	char derPK[1024];	//证书拥有者的公钥(der格式)
	char startDateTime[15];	  //证书有效起始日期YYYYMMDDhhmmss
	char endDataTime[15];	  //证书有效终止日期YYYYMMDDhhmmss
	char serialNumberAscBuf[100];	//证书序列号的aschex形式(如IE浏览器看到的格式)，对大数序列号的解析时用.
	unsigned int  serialNumberHash;
	char sign[1024];	//签发证书者的签名
	unsigned char tbsCertificateBuf[2048];
	int tbsCertificateBufLen;
	unsigned char certDerBuf[2048];
	int certDerBufLen;
} TUnionX509CerSM2;

typedef TUnionX509CerSM2		*PUnionX509CerSM2;

typedef struct {
	unsigned int	key;
	int		status;		//0: not found, 1: found
	char		serialNumber[64];
	char		revokeDate[16];
} TUnionCrl;

typedef TUnionCrl		*PUnionCrl;


#ifdef __cplusplus
extern "C" {
#endif


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
int UnionGenerateSM2PKCS10Ex(int vkIndex, char *vkbyLmk,int vklen, char *pk, PUnionX509UserInfoSM2 pReqUserInfo,int fileFormat,char *fileName,char *signature);

//解析证书
int UnionGetCertificateInfoFromBufEx(char *cert,int certlen,PUnionX509CerSM2 pCertInfo);


/*
功能：验证看是否为(平台)自己颁发的证书串
输入：
	issuerPK：发证机构的公钥.  //SM2为"04|x|y"的Hex, RSA的为DER编码格式的公钥(Hex)
	cert:   要认证的证书字符串
	certlen: cert长度
返回:
	>= 0 成功
	< 0 失败
*/
int UnionVerifyCertificateWithPKEx(char *issuerPK,char *cert,int certlen);


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
int UnionFormSM2Pkcs7SignDetach(unsigned char sign[64],char *x509Data,int x509DataLen,char *caCertData,int caCertDataLen, char *pkcs7Sign,int sizeofPkcs7Sign,int formatFlag);



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
int UnionParasePKcs7SignEx(unsigned char *pkcs7Sign,int pkcs7Signlen,unsigned char *cert,int *certLen,unsigned char *sign,int *signlen,unsigned char *data,int *datalen,int *signType,int *algID,int *hashID);

int UnionRSToSM2SigDer(unsigned char rs[64],unsigned char *signatureDer,int *signatureDerLen);
int UnionGetRSFromSM2SigDer(unsigned char *signatureDer,int signatureDerLen,unsigned char rs[64]);

int UnionTrimPemHeadTail(char *inbuf,char *outbuf,char *flagID);
int UnionPemStrPreHandle(char *inbuf,char *outbuf,char *flagID,int buflen);
void UnionLoadLoadOpenSSLAlg();

#ifdef __cplusplus
}
#endif

#endif
