#ifndef _UNION_VK_FORM_
#define _UNION_VK_FORM_

#define DerByteType  2      //二进制字符串
#define	DerOIDType  6		//OID对象
#define	DerOctStringType  4    //字符串
#define	DerNullType  5			//空值
#define algIdString  "2A864886F70D010101"

/*
功能：取DER格式串指示的长度,及之后属性所在地址的位置
输入：
	derByte：der格式的字符串
	derByteLen：derByte的长度
输出：
	该字串的第一个属性值所在地址的位置
返回：
	>= 成功。表示DER格式串指示的长度
	< 0. 失败
*/
int UnionFetchLenOfDerVal(unsigned char *derByte,int derByteLen,int *locationOfVal);

/*
功能：取DER格式串指示的属性长度,及属性的值
输入：
	derByte：der格式的字符串
	derByteLen：derByte的长度
	derType：	属性的类型
输出：
	pval：指向属性值地址的指针
	valLen：属性值的长度
返回：
	>=0 成功，表示下一个属性的位置
	<0 失败
*/
int UnionGetNextValOfDer(unsigned char *derByte,int derByteLen,int derType,unsigned char **pval,int *valLen);

/*
功能：根据DER格式的私钥(扩展的ascHex)，取出DER格式的公钥(扩展的ascHex)
输入：
	derVkAscHex：DER格式的私钥(扩展的ascHex)
输出：
	derPkAscHex：DER格式的公钥(扩展的ascHex)
返回：
	>=0 成功，
	<0 失败
*/
int UnionGetPKFromVK(char *derVkAscHex,char *derPkAscHex);


/*
功能：根据DER格式的私钥(扩展的ascHex)，取出各分量(扩展的ascHex)
输入：
	derVkAscHex：DER格式的私钥(扩展的ascHex)
输出：
	moduluesAscHex：公钥模(扩展的ascHex)
	publicExponentAscHex: 公钥指数(扩展的ascHex)
	privateExponentAscHex: 私钥的指数(扩展的ascHex)
	primePAscHex:	私钥的P(扩展的ascHex)
	primeQAscHex: 私钥的Q(扩展的ascHex)
	dmp1AscHex: 私钥的dmp1(扩展的ascHex)
	dmq1AscHex: 私钥的dmq1(扩展的ascHex)
	coefAscHex: 私钥的coef(扩展的ascHex)
返回：
	>=0 成功，
	<0 失败
*/
int UnionUnPackFromVK(char *derVkAscHex,char *modulues, char *publicExponent, char *privateExponent,char *primeP,char *primeQ,char *dmp1,char *dmq1,char *coef);


#endif
