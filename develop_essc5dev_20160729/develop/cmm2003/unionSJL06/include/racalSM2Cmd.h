// Author:	Wolfgang Wang
// Date:	2006/3/2

#ifndef _racalSM2Cmd_
#define _racalSM2Cmd_

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
int RacalCmdK1(int hsmSckHDL,PUnionSJL06 pSJL06,char type,int length,char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK,char *errCode);

// 使用SM3算法进行HASH计算
/* 输入参数
	algorithmID	算法标识，3-SM3
	lenOfData	数据长度
	hashData	做Hash的数据
   	sizeOfBuf	接收Hash结果的缓冲大小
   输出参数
   	hashValue	Hash结果
   	errCode		密码机出错码
*/
int RacalCmdM7(int hsmSckHDL,PUnionSJL06 pSJL06,char *algorithmID,int lenOfData,char *hashData,char *hashValue,int sizeOfBuf,char *errCode);

// 生成签名的指令
int RacalCmdK3(int hsmSckHDL,PUnionSJL06 pSJL06,char *hashID,int lenOfData,char *data,int vkIndex,int lenOfVK,char *vk,char *sign,int sizeOfSign,char *errCode);

//生成验签的指令
int RacalCmdK4(int hsmSckHDL,PUnionSJL06 pSJL06,int vkIndex,char *hashID,int lengRCaPK,char *rCaPK,int lenCaCertDataSign,char *caCertDataSign,int lenHashVal,char *hashVal,char *errCode);

#endif

