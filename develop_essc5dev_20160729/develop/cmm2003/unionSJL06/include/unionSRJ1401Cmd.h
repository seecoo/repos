#ifndef _unionSRJ1401Cmd_H_
#define _unionSRJ1401Cmd_H_

typedef struct _CERTINFO
{
        char bankCode[20];      /*��������*/
        char issuer[256];                    /* �䷢��DN*/
        char serialNumber[40];  /* ֤�����к�*/
        char subject[256];              /* ֤������*/
        char notBefore[20];             /* ֤����Ч�ڵ���ʼʱ��*/
        char notAfter[20];              /* ֤����Ч�ڵ���ֹʱ��*/
        int  algID;           //��Կ�㷨��ʶ 0--RSA 1---SM2
        char derPK[1024];         //֤��ӵ���ߵĹ�Կ(der��ʽ)
        char signresult[20];            /*֤��ǩ�����*/
        unsigned char cert [2048];      /* ֤��Der����*/
        int  certLen;                   /* ֤��Der���볤��*/
} CERTINFO;

//==================֤��=======================//
//��ʾ��֤��ṹ
typedef struct
{
	char certAppname[256];//֤��Ӧ����
	char bankName[80];   //������
	char bankCode[40];   //��������
	//TUnionX509UserInfo	userInfo;	//֤��ӵ������Ϣ
	char issuserDN[256];
	char userDN[256];
	char startDateTime[15];	  //֤����Ч��ʼ����YYYYMMDDhhmmss
	char endDataTime[15];	  //֤����Ч��ֹ����YYYYMMDDhhmmss
	int IsInCrl;  //1 --��   0---����
	int  algID;           //��Կ�㷨��ʶ 0--RSA 1---SM2
	char derPK[1024];	//֤��ӵ���ߵĹ�Կ(der��ʽ)
	char serialNumberAscBuf[100];	//֤�����кŵ�aschex��ʽ(��IE����������ĸ�ʽ)���Դ������кŵĽ���ʱ��.
	unsigned char certDerBuf[2048];
	int certDerBufLen;
	int vkindex;
}TUnionX509Certinfo;
typedef TUnionX509Certinfo	*PTTUnionX509Certinfo;

//��ʾ��֤��ṹ
typedef struct
{
	char certAppname[256];
	char bankName[80];   //������
	char bankCode[40];   //��������
	//TUnionX509UserInfo	userInfo;	//֤��ӵ������Ϣ
	char issuserDN[256];
	char userDN[256];
	char startDateTime[15];	  //֤����Ч��ʼ����YYYYMMDDhhmmss
	char endDataTime[15];	  //֤����Ч��ֹ����YYYYMMDDhhmmss
	int IsInCrl;  //1 --��   0---����
	int  algID;           //��Կ�㷨��ʶ 0--RSA 1---SM2
	//	int version;
	char serialNumberAscBuf[100];	//֤�����кŵ�aschex��ʽ(��IE����������ĸ�ʽ)���Դ������кŵĽ���ʱ��.
	int vkindex;
}TUnionX509CertinfoESSC;

typedef struct
{
	int count;
	//TUnionX509Certinfo certinfo[MAX_X509_Cert_NUM];
	TUnionX509Certinfo certinfo[60];
}TUnionX509CertinfoWithCount;

typedef TUnionX509CertinfoWithCount		*PTUnionX509CertinfoWithCount;

typedef struct
{
	char ip[16];
	int port;
}TUnionTargetDev;


typedef struct
{
	int count;
	TUnionTargetDev certinfo[5];
}TUnionTargetDevWithCount;


// �������Ƿ�����(100)
int UnionSRJ1401CmdCM100(char *testBuf, int lenOfTestBuf, char *resBuf, int lenOfResBuf);

int UnionSRJ1401CmdCM880(unsigned char* pOrgData, int nOrgDataLen, char* appName, int iAlgorithm, char* pSignData, int * pSignDataLen);

int UnionSRJ1401CmdCM881(unsigned char* pOrgData, int nOrgDataLen, char* appName, char *sCertDN,char* pSignData, int nSignDataLen, int iAlgorithm, int bRetCert, char *sCert, int *pCertLen);

int UnionSRJ1401CmdCM882(unsigned char *pOrgData, int nOrgDataLen, char *appName, int iAlgorithm, char *pSignData, int *pSignDataLen);

int UnionSRJ1401CmdCM883(unsigned char *pOrgData, int nOrgDataLen, char *pSignData,int nSignDataLen, int bRetCert,char *sCert, int *pCertLen);

int UnionSRJ1401CmdCM894(unsigned char* pOrgData, int nOrgDataLen, char* appName, int iAlgorithm,unsigned char *pSignData, int* pSignDataLen);

int UnionSRJ1401CmdCM895(char* pSignData,int nSignDataLen,int bRetCert,char *sCert, int *pCertLen);

int UnionSRJ1401CmdCM896(unsigned char* pOrgData,int nOrgDataLen,char *appName, char* sDN,unsigned char* pMessData, int* pMessDataLen);

int UnionSRJ1401CmdCM897(char *pMessData,char *appName,unsigned char* pOrgData,int *pOrgDataLen);

int UnionSRJ1401CmdCM870(char *appName,unsigned char* sCert,int nCertLen);

int UnionSRJ1401CmdCM873(char *appName, unsigned char *pCertData, int *pCertDataLen);

int UnionSRJ1401CmdCM898(char *appName, char *sCert,int nCertLen,int ntag, char* data, int* dataLen);

int UnionSRJ1401CmdCM876(char *appName, char *sCert,int nCertLen);

int UnionSRJ1401CmdCM877(char *appName);

int UnionSRJ1401CmdCM878(char *appName, char *sCert,int nCertLen);

int UnionSRJ1401CmdCM879(char* appName, char *sCert,int nCertLen);

int UnionSRJ1401CmdES623(int *totalNum);

int UnionSRJ1401CmdES631(char *bankCode,char *bankName,unsigned char* certBuf,int nCertDataSize, int *result);

int UnionSRJ1401CmdES620(char *bankCode,char *bankName,unsigned char *pCertData, int nCertDataSize, int *result);

int UnionSRJ1401CmdES632(char *bankCode,char *bankName,int keyType,int keyIndex,char* certBuf,int nCertDataSize, int *result);

int  UnionSRJ1401CmdES630(char *bankCode,char* bankName,int vkindex,int keyType,char *passwd,unsigned char *certBuf, int nCertDataSize, int *result);

int UnionSRJ1401CmdES621(char *sCertDN);

int UnionSRJ1401CmdES622(char *sCertDN, unsigned char *pCertData, int *pCertDataSize);

int UnionSRJ1401CmdES626(char* fileName);

int UnionSRJ1401CmdES821(int PageNo, int nMaxCntOfPage,TUnionX509CertinfoESSC *certInfo,int *pRetCnt);

int UnionSRJ1401CmdES822(char *sCertDN,PTTUnionX509Certinfo certInfo,int *certExist);

int UnionSRJ1401CmdES824(int keyindex,int algID,char *countryName,char *stateOrProvinceName,char *localityName,char *organizationName, char *organizationalUnitName,char *commonName,char *email,char *p10buf);

int UnionSRJ1401CmdES825(int keyindex,int algID,char *sCertDN,char *p10buf);

int UnionSRJ1401CmdES823(TUnionTargetDevWithCount targetInfo);

int UnionSRJ1401CmdES843(TUnionTargetDevWithCount *targetInfo);

int UnionSRJ1401CmdES844(char *ip, int port);

int UnionSRJ1401CmdES845(char *ip, int port);

#endif // _unionSRJ1401Cmd_H_
