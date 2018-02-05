// Author:	Wolfgang Wang
// Date:	2006/3/2

#ifndef _racalRsaCmd_
#define _racalRsaCmd_

#include "sjl06.h"

// 生成一对RSA密钥
/* 输入参数
	type，类型	0，只用于签名
			1，只用于密钥管理
			2，签名和管理
	length，模数	0320/512/1024/2048
	pkEncoding	公钥的编码方式
	lenOfPKExponent	公钥exponent的长度	可选参数
	pkExponent	公钥的pkExponent	可选参数
	exportNullPK	1，输出裸PK，0，编码方式的PK
   	sizeOfPK	接收公钥的缓冲大小
   	sizeOfVK	接收私钥的缓冲大小
   输出参数
   	pk		公钥
   	lenOfVK		私钥串的长度
   	vk		私钥
   	errCode		密码机出错码
*/
int RacalCmdEI(int hsmSckHDL,PUnionSJL06 pSJL06,char type,int length,char *pkEncoding,
		int lenOfPKExponent,unsigned char *pkExponent,int exportNullPK,
		char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK,char *errCode);

// 存储私钥
/* 输入参数
	vkIndex		私钥的索引号
	lenOfVK		私钥长度
	vk		私钥
   输出参数
   	errCode		密码机出错码
*/
int RacalCmdEK(int hsmSckHDL,PUnionSJL06 pSJL06,int vkIndex,int lenOfVK,unsigned char *vk,char *errCode);

// 生成公钥的MAC值
int RacalCmdEO(int hsmSckHDL,PUnionSJL06 pSJL06,char *pkEncoding,int isNullPK,int lenOfPK,char *pk,char *authData,char *mac,char *errCode);

// 将一个LMK加密的密钥转换为公钥加密
int RacalCmdGK(int hsmSckHDL,PUnionSJL06 pSJL06,char *encyFlag,char *padMode,char *lmkType,char *desKeyByLMK,
		char *checkValue,char *pkMac,char *pk,char *authData,
		char *initValue,char *desKeyByPK,int sizeOfBuf,char *errCode);

/* 
十一、	产生摘要

*/
int RacalCmdGM(int hsmSckHDL,PUnionSJL06 pSJL06,char *method,int lenOfData,char *data,
		char *hashResult,int sizeOfBuf,char *errCode);

// 生成签名的指令
int RacalCmdEW(int hsmSckHDL,PUnionSJL06 pSJL06,char *hashID,char *signID,char *padID,int lenOfData,char *data,
		int vkIndex,int lenOfVK,char *vk,char *sign,int sizeOfSign,char *errCode);

// 验证签名的指令
int RacalCmdEY(int hsmSckHDL,PUnionSJL06 pSJL06,char *hashID,char *signID,char *padID,
		int lenOfSign,char *sign,int lenOfData,char *data,
		char *mac,int isNullPK,int lenOfPK,char *PK,int lenOfAuthData,char *authData,char *errCode);

// 生成公钥的MAC值
// 针对30所加密机，公钥不是按标准ASN.1格式编码的DER补位的
int RacalCmdEO_For30Hsm(int hsmSckHDL,PUnionSJL06 pSJL06,char *pkEncoding,int isNullPK,
	int lenOfPK,char *pk,char *authData,char *mac,char *errCode);
		
// 验证签名的指令
// 针对30所加密机，公钥不是按标准ASN.1格式编码的DER补位的
int RacalCmdEY_For30Hsm(int hsmSckHDL,PUnionSJL06 pSJL06,char *hashID,char *signID,char *padID,
		int lenOfSign,char *sign,int lenOfData,char *data,
		char *mac,int isNullPK,int lenOfPK,char *PK,int lenOfAuthData,char *authData,char *errCode);

/*
	将一个公钥加密的密钥转换为LMK加密
*/
int RacalCmdGI(int hsmSckHDL,PUnionSJL06 pSJL06, char *encyFlag, char *padMode, 
	char *lmkType, int keyLength, int lenOfDesKeyByPK,
	char *desKeyByPK, int vkIndex, int lenOfVK, unsigned char *vk,
	char *initValue, char *desKeyByLMK, char *checkValue, char *errCode);
		
#endif

