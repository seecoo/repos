#ifndef _UNION_CERT_FUN_
#define _UNION_CERT_FUN_

//�ļ���ʽ
#define PEM		0     
#define DER		1

// ֤���û���Ϣ�ṹ
typedef struct {
	char countryName[40];   //������
	char stateOrProvinceName[40];  //ʡ��
	char localityName[40];		//������
	char organizationName[40];  //��λ��
	char organizationalUnitName[40]; //����
	char commonName[80];		//�û���
	char email[80];				//EMail��ַ
	char alias[80];			//����
} TUnionX509UserInfo;

typedef TUnionX509UserInfo *PUnionX509UserInfo;


//֤������ṹ
typedef struct {
	int version;			//�汾��
	TUnionX509UserInfo	reqUserInfo;   //�����û���Ϣ
	int	 hashID;		//ժҪ�㷨��� 0--MD5,1--SHA1
	char derPK[2048];	//der��ʽ��Կ
	char sign[2048];	//������Ϣ��ǩ��
} TUnionP10CerReq;

typedef TUnionP10CerReq		*PUnionP10CerReq;

//֤��ṹ
typedef struct {
	int version;		//�汾��
	TUnionX509UserInfo  issuerInfo;   //����֤���ߵ��û���Ϣ
	TUnionX509UserInfo	userInfo;	//֤��ӵ������Ϣ
	int	 hashID;		//ժҪ�㷨��� 0--MD5,1--SHA1
	char derPK[2048];	//֤��ӵ���ߵĹ�Կ(der��ʽ)
	char startDateTime[15];	  //֤����Ч��ʼ����YYYYMMDDhhmmss
	char endDataTime[15];	  //֤����Ч��ֹ����YYYYMMDDhhmmss
	long serialNumber;			//֤�����к�
	char serialNumberAscBuf[100];	//֤�����кŵ�aschex��ʽ(��IE����������ĸ�ʽ)���Դ������кŵĽ���ʱ��. add in 2010.6.2
	char sign[2048];	//ǩ��֤���ߵ�ǩ��
} TUnionX509Cer;

typedef TUnionX509Cer		*PUnionX509Cer;


/*
����:�����ÿ������(des_ede3_cbc)��˽Կ�ļ�
����:
	keybits:RSA��Կǿ��512/1024/2048��
	passwd:˽Կ��������
	keyFileName:Ҫ���ɵ�˽Կ�ļ���(��·��)
����:
	>=0 �ɹ�
	<0 ʧ��
*/
int UnionGenerateKeyPemWith3DesCBC(char *vk,char *passwd,char *keyFileName);

/*
����:��˽Կ�ļ�����p10֤�������ļ�
����:
	keyFileName: ˽Կ�ļ�
	passwd: ˽Կ��������
	hashID: ժҪ��־. 0--MD5,1--SHA1
	pReqUserInfo: �����û���Ϣ
	fileFormat: �����ļ��ĸ�ʽ 0--PEM��1--DER
	fileName:���ɵ�p10֤�������ļ���(��·��)
����:
	>=0 �ɹ�
	<0 ʧ��
*/
int UnionGeneratePKCS10(char *vkeyFileName,char *passwd,int hashID,PUnionX509UserInfo pReqUserInfo,int fileFormat,char *fileName);


/*
 ���ܣ���֤֤�������ļ���ʽ��ǩ���Ƿ���ȷ
���룺
	CerReqFileName, ֤�������ļ���
���أ�
	>=0����֤�ɹ�
	<0��֤�������ļ���֤ʧ��
 */
 int UnionVerifyCertificateREQ(char *CerReqFileName);

 /*
 ���ܣ���֤�������ļ�ȡ������Ϣ����ֵ
���룺
	CerReqFileName, ֤�������ļ���
���:
	pCerReqInfo: ֤��������Ϣ�ṹָ��
���أ�
	>=0���ɹ�
	<0��ʧ��
 */
int UnionGetCertificateREQInfo(char *CerReqFileName,PUnionP10CerReq pCerReqInfo);

/*
���ܣ����Լ�(��CA�Ľ�ɫ)ǩ��֤��
����:
	vkeyFileName: ˽Կ�ļ�
	passwd: ˽Կ��������
	hashID: ժҪ��־. 0--MD5,1--SHA1
	pUserInfo: �û���Ϣ�ṹָ��
	sn: ֤�����к�(Ψһֵ)
	days: ֤����Ч����
	fileFormat: �����ļ��ĸ�ʽ 0--PEM��1--DER
	fileName:���ɵ�p10֤�������ļ���(��·��)
����:
	>=0 �ɹ�
	<0 ʧ��
*/
int UnionMakeSelfCert(char *vkeyFileName,char *passwd,int hashID,PUnionX509UserInfo pUserInfo,long sn,int days,int fileFormat,char *selfCertfileName);


/*
���ܣ�����֤�������ļ�����������ǩ��֤��
����:
	CACerFileName: CA֤���ļ�
	vkeyFileName: CA֤���Ӧ��˽Կ�ļ������ڸ�֤��ǩ��
	passwd: ˽Կ�ļ��ı�������
	hashID: ǩ��ʱ�õ�ժҪ��־ 0--MD5,1--SHA1
	CerReqFileName: ֤�������ļ���
	sn: ����֤������к�
	days��֤�����Ч����
	fileFormat: �����ļ��ĸ�ʽ 0--PEM��1--DER
	CertfileName: Ҫ����֤����ļ���(��·��)
����:
	>= 0 �ɹ�
	< 0 ʧ��
*/
int UnionMakeCert(char *CACerFileName,char *vkeyFileName,char *passwd,int hashID,char *CerReqFileName,long sn,int days,int fileFormat,char *CertfileName);

/*
���ܣ�����һ��֤����֤���Ƿ�Ϊ�Լ��䷢��֤�飬ע��Ҫ��֤CertfileName֤�飬����Ҫ����CACerFileName֤��
���룺
	CACerFileName��CA֤���ļ�
	CertfileName: Ҫ��֤��֤���ļ�
����:
	>= 0 �ɹ�
	< 0 ʧ��
*/
int UnionVerifyCertificate(char *CACerFileName,char *CertfileName);


/*
���ܣ���֤���ļ���ȡ��Կ
���룺
	CerFileName��֤���ļ�
	sizeofderPK: ��Կ����󳤶�
�����
	derPK��der��ʽ�Ĺ�Կ(��չ��aschex�ɼ��ַ���)
����:
	>= 0 �ɹ�
	< 0 ʧ��
*/
int UnionGetPKOfCertificate(char *CerFileName,char *derPK,int sizeofderPK);


/*
���ܣ���֤���ļ���ȡ֤����Ϣ����ֵ
���룺
	CerFileName��֤���ļ�
	pCertInfo��֤����Ϣ�ṹָ��
����:
	>= 0 �ɹ�
	< 0 ʧ��
*/
int UnionGetCertificateInfo(char *CerFileName,PUnionX509Cer pCertInfo);

/*
���ܣ���˽Կ�ļ���X509֤��ϲ���pfx�ļ�
���룺
	keyFilename��˽Կ�ļ�
	passwd��˽Կ�ļ���������
	crtFileName: x509֤���ļ�
	caCrtFileName :ca֤���ļ�
	alias:	����
	pfxFileName: Ҫ���ɵ�pfx�ļ�
���أ�
	>= 0 �ɹ�
	< 0 ʧ��
*/
int UnionCombinationPfx(char *keyFilename,char *passwd,char *crtFileName,char *caCrtFileName,char *alias,char *pfxFileName);

/*
���ܣ������֤��(�����CA֤��)��pfx�ļ�
���룺
	pfxFileName: pfx֤���ļ�
	passwd: pfx֤�����
	caCrtFileName: CA֤��
���أ�
	>= 0 �ɹ�
	< 0 ʧ��
*/
int UnionImportCAToPfx(char *pfxFileName,char *passwd,char *caCrtFileName);

/*
	�������ܣ���pfx֤���ȡ����˽Կ��
	���������
	pfxFilname��pfx֤���ļ���
	pfxPassword: ˽Կ����

	���������
	VK: pfx֤���˽Կ(DER����Ŀɼ��ַ���)
	PK:  pfx֤��Ĺ�Կ(DER����Ŀɼ��ַ���)

	���أ�>=0 �ɹ��� < 0 ʧ��
*/
int UnionGetRSAPairFromPfx(char *pfxFilname,char *pfxPassword, char *VK, char *PK);


/*
����: ���֤���Ƿ��ڳ����б���
���룺
	tCRLFileName�� ֤�鳷���б��ļ���
	tCertFileName: ������֤����
����:
	<0 ���� 
	=0,���ں����� 
	=1,�ں����� 
*/
int UnionCertCRLCheck(char *tCRLFileName,char *tCertFileName);


/*
	�������ܣ���pfx֤���ȡ˽Կ,��Կ֤�飬CA֤��
	���������
	pfxFilname��pfx֤���ļ���
	pfxPassword: ˽Կ����
���������
	VK: pfx֤���˽Կ(DER����Ŀɼ��ַ���)
	x509FileName:  ���湫Կ֤���ļ���
	caCertFileName������CA��֤���ļ���

	���أ�>=0 �ɹ��� < 0 ʧ��
*/
int UnionGetVKAndCertFromPfx(char *pfxFilname,char *pfxPassword, char *VK, char *x509FileName,char *caCertFileName,char* cerBuf,char* caCerBuf);


/*
��������: ����PKCS#7��ʽ��ǩ��
���������
	vkindex: 0-20��ʾ˽Կ������, 99��ʾ���LMK���ܵ�VK, -1��ʾ������vk����˽Կ
	vk: ˽Կ�������ַ���. ��vkindexΪ0-20ʱ���ò��������塣��vkindex==99ʱ��ΪLMK���ܵ����ģ���vkindex==-1ʱ��Ϊ����˽Կ
	vklen: vk�ĳ���
	data: ��ǩ������
	dataLen: ���ݳ���
	x509FileName: ��Կ֤���ļ�
	caCertFileName: CA��֤��
���������
	pkcs7Sign: pkcs#7��ʽ��ǩ��
���أ�>0 �ɹ��� ǩ���ĳ���
	  < 0 ʧ��
*/
int UnionSignWithPkcs7(int vkindex,unsigned char *vk,int vklen,unsigned char *data,int dataLen,char *x509FileName,char *caCertFileName,unsigned char *pkcs7Sign,int sizeofPkcs7Sign);


/*
��������: ����PKCS#7��ʽ��ǩ��
���������
	vkindex: 0-20��ʾ˽Կ������, 99��ʾ���LMK���ܵ�VK, -1��ʾ������vk����˽Կ
	vk: ˽Կ�ַ���,��չ��aschex. ��vkindexΪ0-20ʱ���ò��������塣��vkindex==99ʱ��ΪLMK���ܵ����ģ���vkindex==-1ʱ��Ϊ����˽ԿDER��ʽ
	data: ��ǩ������
	dataLen: ���ݳ���
	x509FileName: ��Կ֤���ļ�
	caCertFileName: CA��֤��
���������
	pkcs7Sign: pkcs#7��ʽ��ǩ��
���أ�>0 �ɹ��� ǩ���ĳ���
	  < 0 ʧ��
*/
int UnionSignWithPkcs7Hsm(int vkindex,char *vk,unsigned char *data,int dataLen,char *x509FileName,char *caCertFileName,unsigned char *pkcs7Sign,int sizeofPkcs7Sign);


/*
���ܣ������㷨��������ǩ��
���룺
	vk��˽Կ,DER��ʽ����չ�ַ���
	hashID��ժҪ�㷨0-md5,1-sha1
	data: ��ǩ��������
	len: ���ݳ���
�����
	sign: ǩ��
���أ�
	>= 0 �ɹ�
	< 0 ʧ��
*/
int UnionSignWithSoft(char *vk,int hashID,unsigned char *data,int len,char *sign);

/*
����:�����㷨��֤ǩ��
���룺
vk����Կ,DER��ʽ����չ�ַ��� 
	hashID��ժҪ�㷨0-md5,1-sha1
	data: ��ǩ��������
	len: ���ݳ���
	sign: ǩ��

���أ�
	=1 �ɹ�
	=0 ��֤ʧ��
	<0 ����
*/
int UnionVerifySignWithSoft(char *pk,int hashID,unsigned char *data,int len,char *sign);

/*
���ܣ�����֤�����󣬸�������ǩ��֤��,����X509֤��PEM��ʽ��
����:
	PUnionX509UserInfo: ƽ̨�û���Ϣ
	vkindex: 0-20��ʾ˽Կ������, 99��ʾ���LMK���ܵ�VK, -1��ʾ������vk����˽Կ
	vk: ˽Կ�ַ���,��չ��ASCHEX. ��vkindexΪ0-20ʱ���ò��������塣��vkindex==99ʱ��ΪLMK���ܵ����ģ���vkindex==-1ʱ��ΪDER��ʽ������˽Կ
	hashID: ǩ��ʱ�õ�ժҪ��־ 0--MD5,1--SHA1
	CerReq: ֤�������ִ�
	CerReqlen: CerReq����
	sn: ����֤������к�
	days��֤�����Ч����
	Cert: ����PEM��ʽ֤���ִ�
����:
	>= 0 �ɹ�
	< 0 ʧ��
*/
int UnionMakeCertWithHsmSign(PUnionX509UserInfo pEsscInfo,int vkindex,char *vk,int hashID,char *CerReq,int CerReqlen,long sn,int days,char *Cert,int sizeofCert);

/*
���ܣ���֤���Ƿ�Ϊ(ƽ̨)�Լ��䷢��֤�鴮
���룺
	esscPK����֤����(��ƽ̨)�Ĺ�Կ
	cert:   Ҫ��֤��֤���ַ���
	certlen: cert����
����:
	>= 0 �ɹ�
	< 0 ʧ��
*/
int UnionVerifyCertificateWithPK(char *esscPK,char *cert,int certlen);

/*
���ܣ���֤���Ƿ�Ϊ(ƽ̨)�Լ��䷢��֤�鴮
���룺
	hsmFlag: ʹ�ü��ܻ���ǩ��־�� 1-ʹ�ü��ܻ���֤֤�����ǩ����0-�����㷨��֤֤�����ǩ��
	esscPK����֤����(��ƽ̨)�Ĺ�Կ
	cert:   Ҫ��֤��֤���ַ���
	certlen: cert����
����:
	>= 0 �ɹ�
	< 0 ʧ��
*/
int UnionVerifyCertificateWithPKHsm(int hsmFlag,char *esscPK,char *cert,int certlen);

/*
���ܣ���֤���ַ�����ȡ֤����Ϣ����ֵ
���룺
	cert��֤���ַ���
	certlen: ֤���ַ�������
���:
	PK��֤�鹫Կder��ʽ����չ�ַ���
����:
	>= 0 �ɹ�
	< 0 ʧ��
*/
int UnionGetCertificatePKFromBuf(char *cert,int certlen,char *PK,int sizeofPK);

/*
���ܣ���֤���ַ�����ȡ֤����Ϣ����ֵ
���룺
	cert��֤���ַ���
	certlen: ֤���ַ�������
���:
	pCertInfo��֤����Ϣ�ṹָ��
����:
	>= 0 �ɹ�
	< 0 ʧ��
*/
int UnionGetCertificateInfoFromBuf(char *cert,int certlen,PUnionX509Cer pCertInfo);

/*
��������: ����KCS#7��ʽ��ǩ��
����: 
	pkcs7Sign: pkcs7��ʽǩ���ִ�
	pkcs7Signlen: pkcs7Sign����
���:
	cert: ǩ��֤��(x509)pem��ʽ�ַ���
	certlen: ֤�鳤��
	data: ԭʼ����
	datalen: ���ݳ���
	sign: ǩ���ִ�
	signlen: sign����
���أ�
	>=0  ����KCS#7��ʽ��ǩ���ɹ�
	<0  ʧ��
*/
int UnionParseSignWithPkcs7(unsigned char *pkcs7Sign,int pkcs7Signlen,unsigned char *cert,int *certlen,unsigned char *data,int *datalen,unsigned char *sign,int *signlen);

/*
��������: ����KCS#7��ʽ��ǩ��
����:
	pkcs7Sign: KCS#7��ʽ�ַ���
	pkcs7Signlen: pkcs7Sign�ĳ���
���:
	cert: pem��ʽ��ǩ����֤��
	sign: ǩ��
	data: ԭʼ����
	datalen: ԭʼ���ݳ���,ע��һЩpkcs7��ǩ���ļ��ﲻ��ԭʼǩ�����ݣ���ʱ��datalenΪ0
	hashID: ǩ��ʱ�õ�ժҪ�㷨
����:
	>= 0 ����KCS#7��ʽ��ǩ���ɹ�
	< 0 ʧ��
*/
int UnionParaseJHSignWithPkcs7(unsigned char *pkcs7Sign,int pkcs7Signlen,char *cert,char *sign,unsigned char *data,int *datalen,int *hashID);

/*
��������: ����PKCS#7��ʽ��ǩ��
���������
	vkindex: 0-20��ʾ˽Կ������, 99��ʾ���LMK���ܵ�VK, -1��ʾ������vk����˽Կ
	vk: ˽Կ�ַ���,��չ��aschex. ��vkindexΪ0-20ʱ���ò��������塣��vkindex==99ʱ��ΪLMK���ܵ����ģ���vkindex==-1ʱ��Ϊ����˽ԿDER��ʽ
	data: ��ǩ������
	dataLen: ���ݳ���
	NIDDataFlag: PKCS#7���Ƿ�Ҫ��ǩ�����ݵı�־��0���������� 1--��ǩ������
	x509FileName: ��Կ֤���ļ�
	caCertFileName: CA��֤��,PKCS#7ǩ���ļ�������CA��֤��
���������
	pkcs7Sign: pkcs#7��ʽ��ǩ��
���أ�>0 �ɹ��� ǩ���ĳ���
	  < 0 ʧ��
*/
int UnionJHSignWithPkcs7Hsm(int vkIndex, char *vk,unsigned char *data,int dataLen,int NIDDataFlag,char *bankCertBuf,char *caCertBuf, char *pkcs7Sign,int sizeofPkcs7Sign);
//int UnionJHSignWithPkcs7Hsm(int vkindex,char *vk,unsigned char *data,int dataLen,int NIDDataFlag,char *x509FileName,char *caCertFileName,char *pkcs7Sign,int sizeofPkcs7Sign);

/*
��������: ��ʼ��pb7��ʽ��֤�����ļ�
����: 
	pb7FileName: pb7��ʽ֤�����ļ���
���أ�
	>=0  �ɹ�
	<0  ʧ��
*/
int UnionInitPb7File(char *pb7FileName);

/*
��������: ��X509֤�鵼�뵽pb7��ʽ��֤�����ļ�
����: 
	caCertFileName��X509֤���ļ���
	pb7FileName: pb7��ʽ֤�����ļ���
���أ�
	>=0  �ɹ�
	<0  ʧ��
*/
int UnionImportCACert2Pb7File(char *caCertFileName,char *pb7FileName);

/*
��������: ��pb7֤�����ļ�У��X509֤��ĺϷ���
����: 
	CertfileName: X509֤���ļ�
	pb7FileName: pb7��ʽ֤�����ļ���
���أ�
	>=0  �ɹ�
	<0  ʧ��
*/
int UnionVerifyCertByP7bCAlinks(char *CertfileName,char *p7bFileName);

/*
����:��֤����У��֤�鳷���б�ĺϷ���
	���룺
		p7bFileName:֤�����ļ���
		tCRLFileName:֤�鳷���б���ļ���
	���أ�
		>=0 �ɹ�
		<0  ʧ��
*/
int UnionVerifyCRL(char *p7bFileName,char *tCRLFileName);

int UnionCertCRLCheckUseID(char *serialNumber,char *tCRLFileName);
int UnionCertStrCRLCheck(char *tCRLFileName,char *tCertDer) ;
int UnionSignWithHsm(int vkindex,char *vk,int hashID,unsigned char *data,int len,char *sign);
int UnionPemStrPreHandle(char *inbuf,char *outbuf,char *flagID,int buflen);
int UnionGetDesKeyValue(int VKindex, char flag, char *data, int len, char *deskey, int sizeOfDesKey);

#endif

