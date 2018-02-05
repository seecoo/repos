#ifndef _UNION_JKS_FUN_
#define _UNION_JKS_FUN_

#define MAX_CERTCHAIN_NUM  5  //证书链最大的级数

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
int UnionEncrytJKSPrivate(char *VKOfPKcs8,char *passwd,char *encryptVK);

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
int UnionPackPkcs8PrivateKeyInfo(char *vkAschex,char *pkcs8VkAschex);

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
int UnionJksStore(char *alias,char *vk,char *passwd, char *certAscHex, char certChain[MAX_CERTCHAIN_NUM][4096],int certChainNum,char *jksFileName,char *jskPasswd);

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
int UnionPfx2JKS(char *pfxFileName,char *passwd, char *jksFileName,char *jksPasswd);

/*
功能：将证书链里的证书按顺序排列
输入：
	certChain: 证书字符串数组
	certChainNum: 证书个数
	descFlag: 排序方向。0：从上到下顺序（从根CA到下一级CA）, 1: 从下往上逆序（从最后一级CA到根CA）
*/
int UnionSortCert(char certChain[MAX_CERTCHAIN_NUM][4096],int certChainNum,int descFlag,char orderCertChain[MAX_CERTCHAIN_NUM][4096]);

#endif

