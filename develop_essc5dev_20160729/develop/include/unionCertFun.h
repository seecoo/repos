#ifndef _UNION_CERT_FUN_
#define _UNION_CERT_FUN_

//文件格式
#define PEM		0     
#define DER		1

// 证书用户信息结构
typedef struct {
	char countryName[40];   //国家名
	char stateOrProvinceName[40];  //省名
	char localityName[40];		//城市名
	char organizationName[40];  //单位名
	char organizationalUnitName[40]; //部门
	char commonName[80];		//用户名
	char email[80];				//EMail地址
	char alias[80];			//别名
} TUnionX509UserInfo;

typedef TUnionX509UserInfo *PUnionX509UserInfo;


//证书请求结构
typedef struct {
	int version;			//版本号
	TUnionX509UserInfo	reqUserInfo;   //请求用户信息
	int	 hashID;		//摘要算法标记 0--MD5,1--SHA1
	char derPK[2048];	//der格式公钥
	char sign[2048];	//请求信息的签名
} TUnionP10CerReq;

typedef TUnionP10CerReq		*PUnionP10CerReq;

//证书结构
typedef struct {
	int version;		//版本号
	TUnionX509UserInfo  issuerInfo;   //发布证书者的用户信息
	TUnionX509UserInfo	userInfo;	//证书拥有者信息
	int	 hashID;		//摘要算法标记 0--MD5,1--SHA1
	char derPK[2048];	//证书拥有者的公钥(der格式)
	char startDateTime[15];	  //证书有效起始日期YYYYMMDDhhmmss
	char endDataTime[15];	  //证书有效终止日期YYYYMMDDhhmmss
	long serialNumber;			//证书序列号
	char serialNumberAscBuf[100];	//证书序列号的aschex形式(如IE浏览器看到的格式)，对大数序列号的解析时用. add in 2010.6.2
	char sign[2048];	//签发证书者的签名
} TUnionX509Cer;

typedef TUnionX509Cer		*PUnionX509Cer;


/*
功能:产生用口令加密(des_ede3_cbc)的私钥文件
输入:
	keybits:RSA密钥强度512/1024/2048等
	passwd:私钥保护口令
	keyFileName:要生成的私钥文件名(带路径)
返回:
	>=0 成功
	<0 失败
*/
int UnionGenerateKeyPemWith3DesCBC(char *vk,char *passwd,char *keyFileName);

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
int UnionGeneratePKCS10(char *vkeyFileName,char *passwd,int hashID,PUnionX509UserInfo pReqUserInfo,int fileFormat,char *fileName);


/*
 功能：验证证书请求文件格式及签名是否正确
输入：
	CerReqFileName, 证书请求文件名
返回：
	>=0：验证成功
	<0：证书请求文件验证失败
 */
 int UnionVerifyCertificateREQ(char *CerReqFileName);

 /*
 功能：从证书请求文件取请求信息属性值
输入：
	CerReqFileName, 证书请求文件名
输出:
	pCerReqInfo: 证书请求信息结构指针
返回：
	>=0：成功
	<0：失败
 */
int UnionGetCertificateREQInfo(char *CerReqFileName,PUnionP10CerReq pCerReqInfo);

/*
功能：给自己(根CA的角色)签发证书
输入:
	vkeyFileName: 私钥文件
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
int UnionMakeSelfCert(char *vkeyFileName,char *passwd,int hashID,PUnionX509UserInfo pUserInfo,long sn,int days,int fileFormat,char *selfCertfileName);


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
int UnionMakeCert(char *CACerFileName,char *vkeyFileName,char *passwd,int hashID,char *CerReqFileName,long sn,int days,int fileFormat,char *CertfileName);

/*
功能：用上一级证书验证看是否为自己颁发的证书，注意要认证CertfileName证书，首先要信用CACerFileName证书
输入：
	CACerFileName：CA证书文件
	CertfileName: 要认证的证书文件
返回:
	>= 0 成功
	< 0 失败
*/
int UnionVerifyCertificate(char *CACerFileName,char *CertfileName);


/*
功能：从证书文件获取公钥
输入：
	CerFileName：证书文件
	sizeofderPK: 公钥的最大长度
输出：
	derPK：der格式的公钥(扩展的aschex可见字符串)
返回:
	>= 0 成功
	< 0 失败
*/
int UnionGetPKOfCertificate(char *CerFileName,char *derPK,int sizeofderPK);


/*
功能：从证书文件获取证书信息属性值
输入：
	CerFileName：证书文件
	pCertInfo：证书信息结构指针
返回:
	>= 0 成功
	< 0 失败
*/
int UnionGetCertificateInfo(char *CerFileName,PUnionX509Cer pCertInfo);

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
int UnionCombinationPfx(char *keyFilename,char *passwd,char *crtFileName,char *caCrtFileName,char *alias,char *pfxFileName);

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
int UnionImportCAToPfx(char *pfxFileName,char *passwd,char *caCrtFileName);

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
int UnionGetRSAPairFromPfx(char *pfxFilname,char *pfxPassword, char *VK, char *PK);


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
int UnionCertCRLCheck(char *tCRLFileName,char *tCertFileName);


/*
	函数功能：从pfx证书获取私钥,公钥证书，CA证书
	输入参数：
	pfxFilname：pfx证书文件名
	pfxPassword: 私钥密码
输出参数：
	VK: pfx证书的私钥(DER编码的可见字符串)
	x509FileName:  保存公钥证书文件名
	caCertFileName：保存CA根证书文件名

	返回：>=0 成功， < 0 失败
*/
int UnionGetVKAndCertFromPfx(char *pfxFilname,char *pfxPassword, char *VK, char *x509FileName,char *caCertFileName,char* cerBuf,char* caCerBuf);


/*
函数功能: 生成PKCS#7格式的签名
输入参数：
	vkindex: 0-20表示私钥索引号, 99表示外带LMK加密的VK, -1表示将采用vk明文私钥
	vk: 私钥二进制字符串. 当vkindex为0-20时，该参数无意义。当vkindex==99时，为LMK加密的密文，当vkindex==-1时，为明文私钥
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
int UnionSignWithPkcs7(int vkindex,unsigned char *vk,int vklen,unsigned char *data,int dataLen,char *x509FileName,char *caCertFileName,unsigned char *pkcs7Sign,int sizeofPkcs7Sign);


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
int UnionSignWithPkcs7Hsm(int vkindex,char *vk,unsigned char *data,int dataLen,char *x509FileName,char *caCertFileName,unsigned char *pkcs7Sign,int sizeofPkcs7Sign);


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
int UnionSignWithSoft(char *vk,int hashID,unsigned char *data,int len,char *sign);

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
int UnionVerifySignWithSoft(char *pk,int hashID,unsigned char *data,int len,char *sign);

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
int UnionMakeCertWithHsmSign(PUnionX509UserInfo pEsscInfo,int vkindex,char *vk,int hashID,char *CerReq,int CerReqlen,long sn,int days,char *Cert,int sizeofCert);

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
int UnionVerifyCertificateWithPK(char *esscPK,char *cert,int certlen);

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
int UnionVerifyCertificateWithPKHsm(int hsmFlag,char *esscPK,char *cert,int certlen);

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
int UnionGetCertificatePKFromBuf(char *cert,int certlen,char *PK,int sizeofPK);

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
int UnionGetCertificateInfoFromBuf(char *cert,int certlen,PUnionX509Cer pCertInfo);

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
int UnionParseSignWithPkcs7(unsigned char *pkcs7Sign,int pkcs7Signlen,unsigned char *cert,int *certlen,unsigned char *data,int *datalen,unsigned char *sign,int *signlen);

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
int UnionParaseJHSignWithPkcs7(unsigned char *pkcs7Sign,int pkcs7Signlen,char *cert,char *sign,unsigned char *data,int *datalen,int *hashID);

/*
函数功能: 生成PKCS#7格式的签名
输入参数：
	vkindex: 0-20表示私钥索引号, 99表示外带LMK加密的VK, -1表示将采用vk明文私钥
	vk: 私钥字符串,扩展的aschex. 当vkindex为0-20时，该参数无意义。当vkindex==99时，为LMK加密的密文，当vkindex==-1时，为明文私钥DER格式
	data: 待签名数据
	dataLen: 数据长度
	NIDDataFlag: PKCS#7中是否要含签名数据的标志。0，不包含， 1--含签名数据
	x509FileName: 公钥证书文件
	caCertFileName: CA根证书,PKCS#7签名文件不包含CA根证书
输出参数：
	pkcs7Sign: pkcs#7格式的签名
返回：>0 成功， 签名的长度
	  < 0 失败
*/
int UnionJHSignWithPkcs7Hsm(int vkIndex, char *vk,unsigned char *data,int dataLen,int NIDDataFlag,char *bankCertBuf,char *caCertBuf, char *pkcs7Sign,int sizeofPkcs7Sign);
//int UnionJHSignWithPkcs7Hsm(int vkindex,char *vk,unsigned char *data,int dataLen,int NIDDataFlag,char *x509FileName,char *caCertFileName,char *pkcs7Sign,int sizeofPkcs7Sign);

/*
函数功能: 初始化pb7格式的证书链文件
输入: 
	pb7FileName: pb7格式证书链文件名
返回：
	>=0  成功
	<0  失败
*/
int UnionInitPb7File(char *pb7FileName);

/*
函数功能: 将X509证书导入到pb7格式的证书链文件
输入: 
	caCertFileName：X509证书文件名
	pb7FileName: pb7格式证书链文件名
返回：
	>=0  成功
	<0  失败
*/
int UnionImportCACert2Pb7File(char *caCertFileName,char *pb7FileName);

/*
函数功能: 用pb7证书链文件校验X509证书的合法性
输入: 
	CertfileName: X509证书文件
	pb7FileName: pb7格式证书链文件名
返回：
	>=0  成功
	<0  失败
*/
int UnionVerifyCertByP7bCAlinks(char *CertfileName,char *p7bFileName);

/*
功能:用证书链校验证书撤销列表的合法性
	输入：
		p7bFileName:证书链文件名
		tCRLFileName:证书撤销列表的文件名
	返回：
		>=0 成功
		<0  失败
*/
int UnionVerifyCRL(char *p7bFileName,char *tCRLFileName);

int UnionCertCRLCheckUseID(char *serialNumber,char *tCRLFileName);
int UnionCertStrCRLCheck(char *tCRLFileName,char *tCertDer) ;
int UnionSignWithHsm(int vkindex,char *vk,int hashID,unsigned char *data,int len,char *sign);
int UnionPemStrPreHandle(char *inbuf,char *outbuf,char *flagID,int buflen);
int UnionGetDesKeyValue(int VKindex, char flag, char *data, int len, char *deskey, int sizeOfDesKey);

#endif

