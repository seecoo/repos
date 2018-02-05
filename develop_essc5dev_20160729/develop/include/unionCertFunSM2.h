#ifndef _UNION_CERT_FUN_SM2_H
#define _UNION_CERT_FUN_SM2_H

#define SM2DefaultUserID "1234567812345678"

//�ļ���ʽ
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
#define UNION_Digest_END  9   //��Ԥ����ժҪ����

// ֤���û���Ϣ�ṹ
typedef struct {
	char countryName[40];   //������
	char stateOrProvinceName[40];  //ʡ��
	char localityName[40];		//������
	char organizationName[40];  //��λ��
	char organizationName2[40];  //��λ��
	char organizationalUnitName[40]; //����
	char organizationalUnitName2[40]; //����
	char commonName[80];		//�û���
	char email[80];				//EMail��ַ
	char alias[80];			//����
} TUnionX509UserInfoSM2;

typedef TUnionX509UserInfoSM2 *PUnionX509UserInfoSM2;


//֤������ṹ
typedef struct {
	int version;			//�汾��
	TUnionX509UserInfoSM2	reqUserInfo;   //�����û���Ϣ
	int	 hashID;		//ժҪ�㷨��� 0--MD5,1--SHA1
	int  alg;           //��Կ�㷨��ʶ 0--RSA 1---SM2
	char derPK[2048];	//der��ʽ��Կ
	char sign[2048];	//������Ϣ��ǩ��
} TUnionP10CerReqSM2;

typedef TUnionP10CerReqSM2		*PUnionP10CerReqSM2;

//֤��ṹ
typedef struct {
	int version;		 //֤��汾��
	char issuserDN[256];
	char userDN[256];
	TUnionX509UserInfoSM2  issuerInfo;   //����֤���ߵ��û���Ϣ
	TUnionX509UserInfoSM2	userInfo;	//֤��ӵ������Ϣ
	int	 hashID;		//ժҪ�㷨��� 0--MD5,1--SHA1, 2--SM3
	int  algID;           //��Կ�㷨��ʶ 0--RSA 1---SM2
	char derPK[1024];	//֤��ӵ���ߵĹ�Կ(der��ʽ)
	char startDateTime[15];	  //֤����Ч��ʼ����YYYYMMDDhhmmss
	char endDataTime[15];	  //֤����Ч��ֹ����YYYYMMDDhhmmss
	char serialNumberAscBuf[100];	//֤�����кŵ�aschex��ʽ(��IE����������ĸ�ʽ)���Դ������кŵĽ���ʱ��.
	unsigned int  serialNumberHash;
	char sign[1024];	//ǩ��֤���ߵ�ǩ��
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
����: ����sm2��p10֤�������ļ�
����:
	vkbyLmk: �����������˽Կ
	vklen: ˽Կ����
	pk: pkΪ"04|x|y"��Hex
	pReqUserInfo: �����û���Ϣ
	fileFormat: �����ļ��ĸ�ʽ 0--PEM��1--DER
	fileName:���ɵ�p10֤�������ļ���(��·��)
����:
	>=0 �ɹ�
	<0 ʧ��
*/
int UnionGenerateSM2PKCS10Ex(int vkIndex, char *vkbyLmk,int vklen, char *pk, PUnionX509UserInfoSM2 pReqUserInfo,int fileFormat,char *fileName,char *signature);

//����֤��
int UnionGetCertificateInfoFromBufEx(char *cert,int certlen,PUnionX509CerSM2 pCertInfo);


/*
���ܣ���֤���Ƿ�Ϊ(ƽ̨)�Լ��䷢��֤�鴮
���룺
	issuerPK����֤�����Ĺ�Կ.  //SM2Ϊ"04|x|y"��Hex, RSA��ΪDER�����ʽ�Ĺ�Կ(Hex)
	cert:   Ҫ��֤��֤���ַ���
	certlen: cert����
����:
	>= 0 �ɹ�
	< 0 ʧ��
*/
int UnionVerifyCertificateWithPKEx(char *issuerPK,char *cert,int certlen);


/*
��������: ƴװSM2 PKCS#7��ʽ��ǩ��(������ԭʼ����)
���������
	sign:  ǩ��(r+s)
	x509Data: ��Կ֤������
	x509DataLen: ��Կ֤�����ݳ���
	caCertData: CA��֤������,PKCS#7ǩ���ļ�������CA��֤��
	x509DataLen: ��֤�����ݳ���
	formatFlag: 0-base64 1-�����Ƹ�ʽ
���������
	pkcs7Sign: pkcs#7��ʽ��ǩ��
���أ�>0 �ɹ��� ǩ���ĳ���
	  < 0 ʧ��
*/
int UnionFormSM2Pkcs7SignDetach(unsigned char sign[64],char *x509Data,int x509DataLen,char *caCertData,int caCertDataLen, char *pkcs7Sign,int sizeofPkcs7Sign,int formatFlag);



/*
��������: ����KCS#7��ʽ��ǩ��
����:
	pkcs7Sign: KCS#7��ʽ�ַ���
	pkcs7Signlen: pkcs7Sign�ĳ���
���:
	cert: Der��ʽ��ǩ����֤��
	certLen:֤�鳤��
	sign: ǩ��
	signlen: ǩ������
	data: ԭʼ����
	datalen: ԭʼ���ݳ���,ע��һЩpkcs7��ǩ���ļ��ﲻ��ԭʼǩ�����ݣ���ʱ��datalenΪ0
	signType:ǩ������, 0--���ʣ�1--����
	algID:�㷨ID 0--RSA,1--SM2
	hashID: ǩ��ʱ�õ�ժҪ�㷨 0--MD5,1--SHA1,2--SM3
����:
	>= 0 ����KCS#7��ʽ��ǩ���ɹ�
	< 0 ʧ��
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
