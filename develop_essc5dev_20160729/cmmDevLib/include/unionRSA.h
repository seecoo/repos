#ifndef UNION_RSA_H
#define UNION_RSA_H

int UnionVKEnc(unsigned char *plaintxt,int plaintxtLen,char *vk,int fillFlag,unsigned char *encTxt);
int UnionPKDec(unsigned char *crypk,int crypklen,char *pk,int fillFlag,unsigned char *plaintxt);
int UnionPKEnc(unsigned char *plaintxt,int plaintxtlen,char *pk,int fillFlag,unsigned char *crypktxt);
int UnionVKDec(unsigned char *encTxt,int encTxtLen,char *vk,int fillFlag,unsigned char *plaintxt);

//ASN.1 DER解码，将各种定长型结构、标签分割开来——〉outbuf！
int ASN1DER_Decode(char *encode,char *outbuf);

// 产生RSA密钥对
/*
输入参数：
        bits： 密钥强度，512/1024/1152/1408/1984/2048等
	pkExponent: 公钥指数,  65537,3
        sizeofPK：公钥的缓存大小
        sizeofVK: 私钥的缓存大小
输出参数：
        pk:     公钥(可见字符串，扩张的DER格式BCD码)
        vk：    私钥(可见字符串，扩张的DER格式BCD码)
返回：
        >=0 成功
        <0 失败 
*/
int UnionGenRSAByBitsAndPKExponent(int bits,int pkExponent,char *pk,int sizeofPK,char *vk,int sizeofVK);

// 产生RSA密钥对,指数为65537
/*
输入参数：
	bits： 密钥强度，512/1024/2048等
	sizeofPK：公钥的缓存大小
	sizeofVK: 私钥的缓存大小
输出参数：
	pk:	公钥(可见字符串，扩张的DER格式BCD码)
	vk：    私钥(可见字符串，扩张的DER格式BCD码)
返回：
	>=0 成功
	<0 失败
*/
int UnionGenRSABySS(int bits,char *pk,int sizeofPK,char *vk,int sizeofVK);


// 按PKCS1的填充方式，进行私钥加密
/*
输入参数：
	plaintxt： 二进制明文
	plaintxtLen：明文长度
	vk：       私钥(可见字符串，扩张的DER格式BCD码)
输出参数：
	encTxt：加密的密文数据
返回：
	加密的密文数据长度
*/
int UnionVKEncByPKCS1(unsigned char *plaintxt,int plaintxtLen,char *vk,unsigned char *encTxt);


// 用私钥加密，无填充方式
/*
输入参数：
	plaintxt： 二进制明文
	plaintxtLen：明文长度
	vk：       私钥(可见字符串，扩张的DER格式BCD码)
输出参数：
	encTxt：加密的密文数据
返回：
	加密的密文数据长度
*/
int UnionVKEncNoPADDING(unsigned char *plaintxt,int plaintxtLen,char *vk,unsigned char *encTxt);


// 按PKCS1的填充方式，进行公钥解密
/*
输入参数：
	crypk： 二进制密文数据
	crypklen：密文数据长度
	pk：       公钥(可见字符串，扩张的DER格式BCD码)
输出参数：
	plaintxt：明文数据
返回：
	解密后的明文数据长度
*/
int UnionPKDecByPKCS1(unsigned char *crypk,int crypklen,char *pk,unsigned char *plaintxt);


// 进行公钥解密,无填充方式
/*
输入参数：
	crypk： 二进制密文数据
	crypklen：密文数据长度
	pk：       公钥(可见字符串，扩张的DER格式BCD码)
输出参数：
	plaintxt：明文数据
返回：
	解密后的明文数据长度
*/
int UnionPKDecNoPADDING(unsigned char *crypk,int crypklen,char *pk,unsigned char *plaintxt);


// 按PKCS1的填充方式，进行公钥加密
/*
输入参数：
	plaintxt： 明文二进制数据
	plaintxtlen：明文数据长度
	pk：       公钥(可见字符串，扩张的DER格式BCD码)
输出参数：
	crypktxt：密文数据
返回：
	加密后的密文数据长度
*/
int UnionPKEncByPKCS1(unsigned char *plaintxt,int plaintxtlen,char *pk,unsigned char *crypktxt);

// 进行公钥加密,无填充方式
/*
输入参数：
	plaintxt： 明文二进制数据
	plaintxtlen：明文数据长度
	pk：       公钥(可见字符串，扩张的DER格式BCD码)
输出参数：
	crypktxt：密文数据
返回：
	加密后的密文数据长度
*/
int UnionPKEncNoPADDING(unsigned char *plaintxt,int plaintxtlen,char *pk,unsigned char *crypktxt);

// 按PKCS1的填充方式，进行私钥解密
/*
输入参数：
	encTxt： 二进制密文
	encTxtLen：  密文长度
	vk：       私钥(可见字符串，扩张的DER格式BCD码)
输出参数：
	plaintxt：解密后的明文数据
返回：
	解密的明文数据长度
*/
int UnionVKDecByPKCS1(unsigned char *encTxt,int encTxtLen,char *vk,unsigned char *plaintxt);

// 用私钥解密，无填充方式
/*
输入参数：
	encTxt： 二进制密文
	encTxtLen：  密文长度
	vk：       私钥(可见字符串，扩张的DER格式BCD码)
输出参数：
	plaintxt：解密后的明文数据
返回：
	解密的明文数据长度
*/
int UnionVKDecNoPADDING(unsigned char *encTxt,int encTxtLen,char *vk,unsigned char *plaintxt);


// 公钥 PKCS#1 ASN.1 DER 解码；
int UnionPKCSASN1DER_Decode(char *encode,char *pubkey,char *Evalue);

// 将裸公钥(可见字符串，扩张的BCD码)，编码成DER格式. pubkey--模，Evalue--指数
int UnionPKCSASN1DER_Encode(char *pubkey,char *Evalue,char *encode);

int UnionPKCSCerASN1DER_Decode(char *encode,char *outpubkey,char *Evalue);

#endif


