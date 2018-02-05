//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#ifndef _UnionHsmCmd_
#define _UnionHsmCmd_

#include "unionDesKey.h"

int UnionSetIsUseNormalZmkType();
int UnionSetUseSpecPVKI(int pvki);
int UnionGetUseSpecPVKI();

int UnionTranslateDesKeyTypeTo3CharFormat(TUnionDesKeyType desKeyType, char *keyType);

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
 */
int UnionHsmCmdEI(char type,int length,char *pkEncoding,
		int lenOfPKExponent,unsigned char *pkExponent,int exportNullPK,
		char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK);

// 存储私钥
/* 输入参数
   vkIndex		私钥的索引号
   lenOfVK		私钥长度
   vk		私钥
   输出参数
 */
int UnionHsmCmdEK(int vkIndex,int lenOfVK,unsigned char *vk);

// 生成公钥的MAC值
int UnionHsmCmdEO(char *pkEncoding,int isNullPK,int lenOfPK,char *pk,char *authData,char *mac);

// 验证公钥的MAC值
int UnionHsmCmdEQ(char *pk,char *authData,char *mac);

// 将一个LMK加密的密钥转换为公钥加密
int UnionHsmCmdGK(char *encyFlag,char *padMode,char *lmkType,char *desKeyByLMK,
		char *checkValue,char *pkMac,char *pk,char *authData,
		char *initValue,char *desKeyByPK,int sizeOfBuf);

/* 
   十一、	产生摘要

 */
int UnionHsmCmdGM(char *method,int lenOfData,char *data,
		char *hashResult,int sizeOfBuf);

// 生成签名的指令
int UnionHsmCmdEW(char *hashID,char *signID,char *padID,int lenOfData,char *data,
		int vkIndex,int lenOfVK,char *vk,char *sign,int sizeOfSign);

// 验证签名的指令
int UnionHsmCmdEY(char *hashID,char *signID,char *padID,
		int lenOfSign,char *sign,int lenOfData,char *data,
		char *mac,int isNullPK,int lenOfPK,char *PK,int lenOfAuthData,char *authData);

/*
   将一个公钥加密的密钥转换为LMK加密
 */
int UnionHsmCmdGI(char *encyFlag, char *padMode, 
		char *lmkType, int keyLength, int lenOfDesKeyByPK,
		char *desKeyByPK, int vkIndex, int lenOfVK, unsigned char *vk,
		char *initValue, char *desKeyByLMK, char *checkValue);

// 生成一对SM2密钥
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
 */
int UnionHsmCmdK1(char type,int length,	char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK);

// 生成签名的指令
int UnionHsmCmdK3(char *hashID,int lenOfUsrID, char *usrID,int lenOfData,char *data,int vkIndex,int lenOfVK,char *vk,char *sign,int sizeOfSign);

//生成验签的指令
int UnionHsmCmdK4(int vkIndex,char *hashID,int lenOfUsrID, char *usrID,int lengRCaPK,char *rCaPK,int lenCaCertDataSign,char *caCertDataSign,int lenHashVal,char *hashVal);

// 使用SM3算法进行HASH计算
/* 输入参数
   algorithmID	算法标识，3-SM3
   lenOfData	数据长度
   hashData	做Hash的数据
   sizeOfBuf	接收Hash结果的缓冲大小
   输出参数
   hashValue	Hash结果
 */
int UnionHsmCmdM7(char *algorithmID,int lenOfData,char *hashData,char *hashValue,int sizeOfBuf);

int UnionHsmCmd38(char flag,char *vkIndex,int signLen,char *signature,int dataLen,char *data,char *pk);

int UnionHsmCmd30(char flag,char *pkIndex,char *pk,int dataLen,char *data,char *encData);

int UnionHsmCmdNC(char *version);

int UnionHsmCmd31(char *bmkIndex,char *bmkValue);

int UnionHsmCmd11(TUnionDesKeyLength keyLen,char *bmkIndex,char *bmkValue);

int UnionHsmCmd2A(TUnionDesKeyLength keyLen,char *bmkIndex,char *bmkValue);


int UnionHsmCmd34(char *vkIndex,int lenOfVK,char *pk,int sizeOfPK, char *vk, int sizeOfVk);

int UnionHsmCmd35(char *vkIndex,char *vkByMK,int lenOfVKByMK);

int UnionHsmCmd36(char *vkIndex,char *vkByMK,int sizeOfVKByMK);

/*
   转换DES密钥：从主密钥加密到公钥加密
   用于分发密钥。
   输入参数
   pk 公钥
   keyByMK 密钥密文
   输出参数
   checkValue 检查值 
   sizeOfBuf 密钥长度
   keyByPK 密钥密文（用公钥加密的DES密钥）
 */
int UnionHsmCmd3B(char *pk,char *keyByMK,char *keyByPK,int sizeOfBuf,char *checkValue);

/*
   转换DES密钥：从公钥加密到主密钥加密
   用于接收密钥。
   输出参数
   vkIndex 私钥索引，"00"－"20"：用密码机内的私钥
   lenOfKeyByPK 密钥长度，DES密钥密文的字节数
   keyByPK 用公钥加密的DES密钥
   输出参数
   keyByMK 用主密钥加密的DES密钥
   checkValue 检查值
 */
int UnionHsmCmd3A(int vkIndex,int lenOfKeyByPK,char *keyByPK,char *keyByMK,char *checkValue);

int UnionHsmCmd3E(char *bmk,char *pk,char *keyByBMK,char *checkValue,char *keyByPK);

int UnionHsmCmd3C(char flag,int lenOfData,char *data,char *hash);
int UnionHsmCmd3CForEEMG(char flag,int lenOfData,char *data,char *hash);

int UnionHsmCmd37(char flag,char *vkIndex,int lenOfVK,char *vk,int dataLen,char *data,char *signature,int sizeOfSignature);


int UnionHsmCmd40(char *vkIndex,char *bmk,char *pvk,unsigned char *pinByPK,int lenOfPinByPK,char *pinByPVK);

int UnionHsmCmd41(char *vkIndex,char *bmk,char *zpk,char *pan,unsigned char *pinByPK,int lenOfPinByPK,char *pinByZPK);

int UnionHsmCmd42(char *vkIndex,unsigned char *encryptedData,int lenOfEncryptedData,char *plainText,int sizeOfPlainText);

/*
   功能：将由公钥加密的PIN转换成ANSI9.8标准
   输入参数
   vkIndex 私钥索引，指定的私钥，用于解密PIN数据密文
   type 密钥类型，1：TPK  2：ZPK
   zpk 用于加密PIN的密钥
   pinType PIN类型，1：ANSI9.8
   pan 用户主帐号
   pinByPK 经公钥加密的PIN数据密文
   lenOfPinByPK 密文长度
   输出参数
   pinByZPK 返回的PIN密文--ANSI9.8
 */
int UnionHsmCmd43(char *vkIndex,int lenOfVK,char *vk,char type,char *zpk,char pinType,char *pan,unsigned char *pinByPK,int lenOfPinByPK,char *pinByZPK);

/*
   功能：将由公钥加密的PIN转换成旧网银加密算法加密
   输入参数
   vkIndex 私钥索引，指定的私钥，用于解密PIN数据密文
   pinByPK 经公钥加密的PIN数据密文
   lenOfPinByPK 密文长度
   输出参数
   hash 返回的PIN密文--ANSI9.8
 */
int UnionHsmCmd44(char *vkIndex,char *vkValue, int lenOfVKValue, unsigned char *pinByPK,int lenOfPinByPK,char *hash);

/*
   功能：用EDK密钥加解密数据，如果是解密状态，则必须在授权下才能处理，否则报错。
   输入参数
   flag 0：加密
   edk LMK24-25加密
   lenOfData 输入数据字节数（8的倍数）范围：0008-4096
   indata 待加密或解密的数据（以BCD码表示）
   输出参数
   outdata 输出结果
 */
int UnionHsmCmd50(char flag,char *edk,int lenOfData,char *indata,char *outdata,int sizeOfOutData);


/*
   功能：用RC4算法进行数据加/解密。由于RC4算法的特性，加密和解密都是调用同一指令。
   参数：
   desKeyType      密钥类型，支持edk和zek
   inData          输入数据，binary
   inLen           输入数据长度
   outData         输出数据
   sizeOfOutData   outData指向的缓冲区的长度
   返回：
   <0 出错
   >0 返回数据长度
   add by zhangsb 20140627
 */
int UnionHsmCmdRD(TUnionDesKeyType desKeyType, char *keyValue, char *inData, int inLen, char *outData, int sizeOfOutData);



// 将ZMK加密的密钥转换为LMK加密的ZAK/ZEK
int UnionHsmCmdFK(char type,char *zmk,char *keyByZMK,char *keyByLMK,char *checkValue);

/*
   函数功能:
   TI指令，公钥解密
   输入参数:
encryMode: 1H 算法标识 1－解密数据 2－加密数据 3－验证签名 4－加密密钥 5－解密密钥
pkLength: 送入公钥长度
pk: nB/1A+3H DER编码的公钥或公钥在HSM安全存储区内的位置
signLength: 待验证签名值长度，encryMode = 3时该域存在
sign: nB 待验证签名值，encryMode = 3时该域存在
keyLength: 1H 待加/解密密钥长度，0 = 单长度密钥，1 = 双长度密钥；encryMode = 4 或 encryMode = 5时该域存在
inputDataLength: 加解密数据长度
inputData: nB/1A+3H 待加解密数据或待加解密密钥在HSM安全存储区内的索引
输出参数:
outputData: nB 加解密/加解密密钥数据
errCode: 错误码
返回值:
>0	ouputData的长度
<0	失败
 */

int UnionHsmCmdTI(char encryMode, int pkLength, char *pk, int signLength,
		char *sign, char keyLength, int inputDataLength, char *inputData, char *outputData);

// 将一个ZMK加密的密钥转换为LMK加密,支持国产算法SM1和SFF33
int UnionHsmCmdA6(char *algFlag, TUnionDesKeyType keyType,char *zmk,char *keyByZmk, char *keyByLmk,char *checkValue);

// 生成银联MAC的指令
int UnionHsmCmdMU(char msgNo,int keyType,TUnionDesKeyLength keyLength,char *key,char msgType,char *iv,int msgLen,char *msg,char *mac);

// 将ZMK加密的ZPK转换为LMK加密
int UnionHsmCmdFA(char *zmk,char *zpkByZmk,int variant,char keyLenFlag,char *zpkByLmk,char *checkValue);

// 生成ZAK/ZEK
int UnionHsmCmdFI(char keyFlag,char *zmk,int variant,char keyLenFlag,char *keyByZmk,char *keyByLmk,char *checkValue);

int UnionHsmCmdA0(int outputByZMK,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,char *zmk,char *keyByLMK,char *keyByZMK,char *checkValue);

int UnionHsmCmd0A();

int UnionHsmCmdRA();

int UnionHsmCmdPA(char *format);

int UnionHsmCmdA2(TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int fldNum,char fld[][80],char *component);

int UnionHsmCmdCC(int zpk1Length,char *zpk1,int zpk2Length,char * zpk2,int maxPinLen,char *pinFormat1,char *pinBlockByZPK1, char *accNo,int lenOfAccNo,char *pinFormat2,char *pinBlockByZPK2, int *pinLen);

int UnionHsmCmdMS(char msgNo,int keyType,TUnionDesKeyLength keyLength,char *key,char msgType,char *iv,int msgLen,char *msg,char *mac);

/*
   函数功能：
   DG指令，用PVK生成PIN的PVV(PIN Verification Value)，
   采用的加密标准为Visa Method
   输入参数：
   pinLength：LMK加密的PIN密文长度
   pin：LMK加密的PIN密文
   pvkLength：LMK加密的PVK对长度
   pvk：LMK加密的PVK对
   lenOfAccNo：客户帐号长度
   accNo：客户帐号
   输出参数：
   pvv：产生的PVV

 */
int UnionHsmCmdDG(int pinLength,char *pin,int pvkLength,char *pvk,int lenOfAccNo,char *accNo,char *pvv);

/*
   函数功能：
   CA指令，将一个TPK加密的PIN转换为由ZPK加密
   输入参数：
   tpkLength：LMK加密的TPK长度
   tpk：LMK加密的TPK
   zpkLength：LMK加密的ZPK长度
   zpk：LMK加密的ZPK
   pinFormat1：转换前的PIN格式
   pinBlockByTPK：转换前由TPK加密的PIN密文
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   pinFormat2：转换后的PIN格式
   输出参数：
   pinBlockByZPK：转换后由ZPK加密的PIN密文
 */
int UnionHsmCmdCA(int tpkLength,char *tpk,int zpkLength,char *zpk,char *pinFormat1,char *pinBlockByTPK,char *accNo,int lenOfAccNo,char *pinFormat2,char *pinBlockByZPK);

/*
   函数功能：
   JE指令，将一个ZPK加密的PIN转换为由LMK加密
   输入参数：
   zpkLength：LMK加密的ZPK长度
   zpk：LMK加密的ZPK
   pinFormat：PIN格式
   pinBlockByZPK：转换前由ZPK加密的PIN密文
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   pinBlockByLMK：转换后由LMK加密的PIN密文

 */
int UnionHsmCmdJE(int zpkLength,char *zpk,char *pinFormat,char *pinBlockByZPK,char *accNo,int lenOfAccNo,char *pinBlockByLMK);

/*
   函数功能：
   JC指令，将一个TPK加密的PIN转换为由LMK加密
   输入参数：
   tpkLength：LMK加密的TPK长度
   tpk：LMK加密的TPK
   pinFormat：PIN格式
   pinBlockByTPK：转换前由TPK加密的PIN密文
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   pinBlockByLMK：转换后由LMK加密的PIN密文

 */
int UnionHsmCmdJC(int tpkLength,char *tpk,char *pinFormat,char *pinBlockByTPK,char *accNo,int lenOfAccNo,char *pinBlockByLMK);

/*
   函数功能：
   JG指令，将一个LMK加密的PIN转换为由ZPK加密
   输入参数：
   zpkLength：LMK加密的ZPK长度
   zpk：LMK加密的ZPK
   pinFormat：PIN格式
   pinLength：LMK加密的PIN密文长度
   pinBlockByZPK：转换前由ZPK加密的PIN密文
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   pinBlockByZPK：转换后由LMK加密的PIN密文

 */
int UnionHsmCmdJG(int zpkLength,char *zpk,char *pinFormat,int pinLength,char *pinBlockByLMK,char *accNo,int lenOfAccNo,char *pinBlockByZPK);

/*
   函数功能：
   JA指令，随机产生一个PIN
   输入参数：
   pinLength：要求随机生成的PIN明文的长度
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   pinBlockByLMK：随机产生的PIN的密文，由LMK加密

 */
int UnionHsmCmdJA(int pinLength,char *accNo,int lenOfAccNo,char *pinBlockByLMK);

/*
   函数功能：
   EE指令，用IBM方式产生一个PIN
   输入参数：
   minPINLength：最小PIN长度
   pinValidData：用户自定义数据
   decimalizationTable：十六进制数到十进制数的转换表
   pinOffset：PIN Offset，左对齐，右补'F'
   pvkLength：LMK加密的PVK长度
   pvk：LMK加密的PVK
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   pinBlockByLMK：随机产生的PIN的密文，由LMK加密

 */
int UnionHsmCmdEE(int minPINLength,char *pinValidData,char *decimalizationTable,char *pinOffset,int pvkLength,char *pvk,char *accNo,int lenOfAccNo,char *pinBlockByLMK);

/*
   函数功能：
   DE指令，用IBM方式产生一个PIN的Offset
   输入参数：
   minPINLength：最小PIN长度
   pinValidData：用户自定义数据
   decimalizationTable：十六进制数到十进制数的转换表
   pinLength：LMK加密的PIN密文长度
   pinBlockByLMK：由LMK加密的PIN密文
   pvkLength：LMK加密的PVK长度
   pvk：LMK加密的PVK
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   pinOffset：PIN Offset，左对齐，右补'F'

 */
int UnionHsmCmdDE(int minPINLength,char *pinValidData,char *decimalizationTable,int pinLength,char *pinBlockByLMK,int pvkLength,char *pvk,char *accNo,int lenOfAccNo,char *pinOffset);

/*
   函数功能：
   BA指令，使用本地主密钥加密一个PIN明文
   输入参数：
   pinCryptogramLen：加密后的PIN密文的长度
   pinTextLength：PIN明文的长度
   pinText：PIN明文
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   pinBlockByLMK：LMK加密的PIN密文

 */
int UnionHsmCmdBA(int pinCryptogramLen,int pinTextLength,char *pinText,char *accNo,int lenOfAccNo,char *pinBlockByLMK);

/*
   函数功能：
   NG指令，使用本地主密钥解密一个PIN密文
   输入参数：
   pinCryptogramLen：LMK加密的PIN密文的长度
   pinCryptogram：LMK加密的PIN密文
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   referenceNumber：用LMK18-19加密帐号得到的偏移值
   pinText：PIN明文

 */
int UnionHsmCmdNG(int pinCryptogramLen,char *pinCryptogram,char *accNo,int lenOfAccNo,char *referenceNumber,char *pinText);

/*
   函数功能：
   DA指令，用IBM方式验证终端的PIN
   输入参数：
   pinBlockByTPK：TPK加密的PIN密文
   minPINLength：最小PIN长度
   pinValidData：用户自定义数据
   decimalizationTable：十六进制数到十进制数的转换表
   pinOffset：IBM Offset，左对齐，右补'F'
   tpkLength：LMK加密的TPK长度
   tpk：LMK加密的TPK
   pvkLength：LMK加密的PVK长度
   pvk：LMK加密的PVK
   pinFormat：PIN格式
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：

 */
int UnionHsmCmdDA(char *pinBlockByTPK,int minPINLength,char *pinValidData,char *decimalizationTable,char *pinOffset,int tpkLength,char *tpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo);

/*
   函数功能：
   EA指令，用IBM方式验证交换中心的PIN
   输入参数：
   pinBlockByZPK：ZPK加密的PIN密文
   minPINLength：最小PIN长度
   pinValidData：用户自定义数据
   decimalizationTable：十六进制数到十进制数的转换表
   pinOffset：IBM Offset，左对齐，右补'F'
   zpkLength：LMK加密的ZPK长度
   zpk：LMK加密的ZPK
   pvkLength：LMK加密的TPK长度
   pvk：LMK加密的PVK
   pinFormat：PIN格式
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：

 */
int UnionHsmCmdEA(char *pinBlockByZPK,int minPINLength,char *pinValidData,char *decimalizationTable,char *pinOffset,int zpkLength,char *zpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo);

/*
   函数功能：
   DC指令，用VISA方式验证终端的PIN
   输入参数：
   pinBlockByTPK：TPK加密的PIN密文
   pvv：终端PIN的4位VISA PVV
   tpkLength：LMK加密的TPK长度
   tpk：LMK加密的TPK
   pvkLength：LMK加密的PVK长度
   pvk：LMK加密的PVK
   pinFormat：PIN格式
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：

 */
int UnionHsmCmdDC(char *pinBlockByTPK,char *pvv,int tpkLength,char *tpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo);

/*
   函数功能：
   EC指令，用VISA方式验证交换中心的PIN
   输入参数：
   pinBlockByZPK：ZPK加密的PIN密文
   pvv：终端PIN的4位VISA PVV
   zpkLength：LMK加密的ZPK
   zpk：LMK加密的ZPK
   pvkLength：LMK加密的PVK长度
   pvk：LMK加密的PVK
   pinFormat：PIN格式
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：

 */
int UnionHsmCmdEC(char *pinBlockByZPK,char *pvv,int zpkLength,char *zpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo);

/*
   函数功能：
   BC指令，用比较方式验证终端的PIN
   输入参数：
   pinBlockByTPK：TPK加密的PIN密文
   tpkLength：LMK加密的TPK
   tpk：LMK加密的TPK
   pinByLMKLength：主机PIN密文长度
   pinByLMK：主机PIN密文，由LMK02-03加密
   pinFormat：PIN格式
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
 */
int UnionHsmCmdBC(char *pinBlockByTPK,int tpkLength,char *tpk,int pinByLMKLength,char *pinByLMK,char *pinFormat,char *accNo,int lenOfAccNo);

/*
   函数功能：
   BE指令，用比较方式验证交换中心的PIN
   输入参数：
   pinBlockByZPK：ZPK加密的PIN密文
   zpkLength：LMK加密的ZPK长度
   zpk：LMK加密的ZPK
   pinByLMKLength：主机PIN密文长度
   pinByLMK：主机PIN密文，由LMK02-03加密
   pinFormat：PIN格式
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
 */
int UnionHsmCmdBE(char *pinBlockByZPK,int zpkLength,char *zpk,int pinByLMKLength,char *pinByLMK,char *pinFormat,char *accNo,int lenOfAccNo);

/*
   函数功能：
   CW指令，产生VISA卡校验值CVV
   输入参数：
   cardValidDate：VISA卡的有效期
   cvkLength：CVK的长度
   cvk：CVK
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   cvv：生成的VISA卡的CVV
 */
int UnionHsmCmdCW(char *cardValidDate,int cvkLength,char *cvk,char *accNo,int lenOfAccNo,char *serviceCode,char *cvv);

/*
   函数功能：
   CY指令，验证VISA卡的CVV
   输入参数：
   cvv：待验证的VISA卡的CVV
   cardValidDate：VISA卡的有效期
   cvkLength：CVK的长度
   cvk：CVK
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
 */
int UnionHsmCmdCY(char *cvv,char *cardValidDate,int cvkLength,char *cvk,char *accNo,int lenOfAccNo,char *serviceCode);

int UnionHsmCmdBU(int SM4Mode,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,char *key,char *checkValue);

/*
   函数功能：
   A4指令，用几把密钥的密文成分合成一把密钥，并生成校验值
   输入参数：
   keyType：密钥的类型
   keyLength：密钥的长度
   partKeyNum：密钥成分的数量
   partKey：存放各个密钥成分的数组缓冲，为LMK加密的密钥密文
   输出参数：
   keyByLMK：合成的密钥密文，由LMK加密
   checkValue：合成的密钥的校验值
 */
int UnionHsmCmdA4(int SM4Mode,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int partKeyNum,char partKey[][50],char *keyByLMK,char *checkValue);

/*
   功能    由源帐号生成的PINBLOCK转换为由目的帐号生成的PINBLOCK，并受指定的密钥加密保护
   说明    根据密钥长度确定算法
   输入参数
   zpk 被 LMK 06-07 加密的ZPK
   pvk 被 LMK 14-15加密的ZPK
   maxPinLen 最大PIN长度
   decimalizationTable 十进制数表
   pinValidationData 
   pinBlock1 被ZPK采用ANSI X9.8标准加密
   accNo1 源账号
   accNo2 目标账号
 */
int UnionHsmCmdDF(char *zpk,char *pvk,int maxPinLen,char *decimalizationTable,char *pinValidationData,char *pinBlock1,char *accNo1,char *accNo2,char *pinBlock2,char *pinOffset);

int UnionHsmCmdA8(TUnionDesKeyType keyType,char *key,char *zmk,char *keyByZMK,char *checkValue);

/*
   函数功能：
   33指令，私钥解密
   输入参数：
   flag：用公钥加密时所采用的填充方式，
   '0'：如果数据长度小于密钥长度，加密时先在数据前面补0x00，以使数据长度等于密钥的长度，然后再进行加密；
   '1'：PKCS填充方式（一般情况下使用此方式）
   vkIndex：私钥索引，"00"－"20"
   lenOfVK：私钥长度
   vk：私钥密文
   cipherDataLen：密文数据的字节数
   cipherData：用于解密的密文数据
   sizeOfPlainData：plainData所在的存储空间大小
   输出参数：
   plainData：解密得到的明文数据
 */
int UnionHsmCmd33(char flag,char *vkIndex,int lenOfVK,char *vk,int cipherDataLen,char *cipherData,char *plainData,int sizeOfPlainData);

/*
   功能
   将指定应用的PK加密的PIN转换为ZPK加密。
   输入参数
   pvkIndex：      指定的私钥，用于解密PIN数据密文
   fillMode：      “0”：如果数据长度小于密钥长度，加密时先在数据前面补0x00，以使数据长度等于密钥的长度，然后再进行加密；
   “1”：PKCS填充方式（一般情况下使用此方式）
   ZPK：	   用于加密PIN的密钥。
   accNo：	 用户主帐号
lenOfPinBlock:  公钥加密的PIN密文的长度
pinBlock：      经公钥加密的PIN数据密文
输出参数
lenOfPin：      密钥长度
pinBlock1：     返回ZPK加密下的PIN密文
pinBlock2：     返回ZPK加密的PIN密文（可用ZPK解密还原真实PIN）
lenOfUniqueID： 01-20
UniqueID：      返回的经ASCII扩展后ID码明文
 */
int UnionHsmCmdH2 (int pvkIndex, char fillMode, char *ZPK,char *accNo,int lenOfPinBlock, char *pinBlock, char *lenOfPin,char *pinBlock1, char *pinBlock2, char *lenOfUniqueID, char *UniqueID);

/*
功能
        将指定应用的PK加密的PIN转换为ZPK加密。
输入参数
   srcAlgorithmID  非对称密钥算法标识，0:国际算法,  1:国密算法
   pvkIndex：   指定的私钥，用于解密PIN数据密文
   lenOfVK      私钥长度
   vkValue      私钥值
   fillMode：   “0”：如果数据长度小于密钥长度，加密时先在数据前面补0x00，以使数据长度等于密钥的长度，然后再进行加密；
                “1”：PKCS填充方式（一般情况下使用此方式）
   dstAlgorithmID  ZPK密钥算法标识,0:国际算法,  1:国密算法
   ZPK：        用于加密PIN的密钥。
   accNo：      用户主帐号
   lenOfPinBlock:  公钥加密的PIN密文的长度
   pinBlock：      经公钥加密的PIN数据密文
输出参数
   lenOfPin：      密钥长度
   pinBlock1：     返回ZPK加密下的PIN密文
   pinBlock2：     返回ZPK加密的PIN密文（可用ZPK解密还原真实PIN）
   lenOfUniqueID： 01-20
   UniqueID：      返回的经ASCII扩展后ID码明文

 */
int UnionHsmCmdN6(int srcAlgorithmID,int pvkIndex, int lenOfVK,char *vkValue,char fillMode, int dstAlgorithmID,char *ZPK,char *accNo,int lenOfPinBlock, char *pinBlock, char *lenOfPin,char *pinBlock1, char *pinBlock2, char *lenOfUniqueID, char *UniqueID);

/*
   功能
   将指定应用的PK加密的PIN转换为ZPK加密。
   输入参数
   OriKeyType：    源KEY类型，’０’ - ZPK ；‘１’ - PVK（目标KEY类型不能为0）
   OriKey：	源KEY，用于解密PIN的密钥。
   lenOfAcc：      帐号长度，4~20。
   accNo：	 用户主帐号。
   DesKeyType：    ’０’ - ZPK（只当源KEY类型为0时，目标KEY类型可为0）；’１’ - PVK
   DesKey：	目标KEY，用于加密PIN的密钥。
   pinBlock：      经公钥加密的PIN数据密文。
   输出参数
   lenOfPinBlock1：密码长度。
   pinBlock1：     返回ZPK2/PVK加密下的PIN密文。
 */
int UnionHsmCmdH5(char *OriKey, int lenOfAcc, char *AccNo, char DesKeyType, char *DesKey, char *pinBlock, char *lenOfPinBlock1,char *pinBlock1);

/*
   功能
   将指定应用的PK加密的PIN转换为ZPK加密。
   输入参数
   OriKeyType：    源KEY类型，’０’ - ZPK ；‘１’ - PVK（目标KEY类型不能为0）
   OriKey：	源KEY，用于解密PIN的密钥。
   lenOfAcc：      帐号长度，4~20。
   accNo：	 用户主帐号。
   DesKeyType：    ’０’ - ZPK（只当源KEY类型为0时，目标KEY类型可为0）；’１’ - PVK
   DesKey：	目标KEY，用于加密PIN的密钥。
   pinBlock：      经公钥加密的PIN数据密文。
   输出参数
   lenOfPinBlock1：密码长度。
   pinBlock1：     返回ZPK2/PVK加密下的PIN密文。
 */
int UnionHsmCmdH7(char *OriKey, int lenOfAcc,char *AccNo, char *DesKey, char *pinBlock, char *lenOfPinBlock1,char *pinBlock1);

/*
   功能
   将指定应用的PK加密的PIN转换为ZPK加密。
   输入参数
   ZPK：	   用于解密PIN的密钥。
   lenOfAcc：      帐号长度，4~20。
   accNo：	 用户主帐号。
   pinBlockByZPK： 经ZPK加密的PIN数据密文。
   PVK：	   PVK。
   pinBlockByPVK： 经PVK加密的PIN数据密文。
   输出参数
 */
int UnionHsmCmdH9(char *ZPK, int lenOfAcc, char *AccNo, char *pinBlockByZPK, char *PVK, char *pinBlockByPVK);

/*
   功能
   将指定应用的PK加密的PIN转换为ZPK加密。
   输入参数
   ZPK：	   用于加密PIN的密钥。
   accNo：	 用户主帐号。
   pinBlockByZPK： 经ZPK加密的PIN数据密文。
   dataOfZAK：     产生ZAK密钥的因素之一。
   输出参数
   lenOfPin：      密码长度
   ZAK：	   返回LMK26-27对对加密下的ZAK密钥。
 */
int UnionHsmCmdH4 (char *ZPK, char *AccNo, char *pinBlockByZPK, char *dataOfZAK, char *lenOfPin, char *ZAK);

/*
   功能：把PinOffset转换为专用算法（FINSE算法）加密的密文
   输入参数 
   PVK LMK对（14-15）下加密的PVK；用于解密offset
   pinOffSet Offset的值；采用左对齐的方式在右边填充字符“F”
   minPinLen 最小的PIN长度
   accNo 账号
   decimalizationTable 十进制转换表
   pinValidationData PIN校验数据
   输出参数
   pinBlock PIN密文
 */
int UnionHsmCmdS1(char *PVK, char *pinOffSet,int minPinLen,char *accNo,char *decimalizationTable,char *pinValidationData, char *pinBlock);

/*
   功能：把PinBlock转换为专用算法（FINSE算法）加密的密文
   输入参数
   ZPK LMK对（14-15）下加密的PVK；用于解密offset
   minPinLen 最小的PIN长度
   accNo 账号
   OriPinBlock 源PIN块
   输出参数
   pinBlock PIN密文 
 */
int UnionHsmCmdS2(char *ZPK, int minPinLen,char *accNo,char *OriPinBlock, char *pinBlock);

/*
   功能：采用离散MAK计算MAC
   输入参数
   ZAK 被LMK26-27对加密
   scatData 用于计算离散密钥的数据
   lenOfmac MAC数据长度
   macData MAC数据
   输出参数
   mac 返回的MAC值
 */
int UnionHsmCmdG1(char *ZAK, char *scatData,char *lenOfmac,char *macData, char *mac);

/*
   产生随机数指令
 */
int UnionHsmCmdY3(int min,int max,int Cnt,int isRepeat,char *RandNo);

/*一次性发送加密机数据块最大长度*/
int UnionHsmCmdE0(int crytoFlag,int blockFlag,int encrypMode,int zekLen,char *Zek,int datatype,char *IV,int MsgLen,unsigned char *Msg,unsigned char *encrypData,int *encryDataLen,int sizeOfEncryptedData);

int UnionHsmCmdE0_f(int crytoFlag,int blockFlag,int encrypMode,int zekLen,char *Zek,int datatype,char *IV,int MsgLen,unsigned char *Msg,unsigned char *encrypData,int *encryDataLen,int sizeOfEncryptedData);

/* Y4指令,功能:产生密钥.使用根密钥对离散数据1进行离散,得到离散密钥;使用保护密钥(ZMK)加密输出和LMK06-07对加密输出.
   输入参数:
   kek：保护密钥KEK
   rootKey：应用主密钥
   keyType：密钥类型；001-ZPK，008-ZAK。
   discreteNum：离散次数
   discreteData1：离散数据1
   discreteData2：离散数据2
   discreteData3：离散数据3
   输出参数:
   keyByKek：使用KEK加密的key
checkValue:校验值
 */
int UnionHsmCmdY4 (char *kek, char *rootKey, char *keyType, int discreteNum, char *discreteData1, char *discreteData2, char *discreteData3, char *keyByKek, char *keyByLmk, char *checkValue);

/*
   函数功能：
   A5指令，输入密钥明文的几个成份合成最终的密钥，同时用相应的LMK密钥对加密
   输入参数：
   keyType：密钥的类型
   keyLength：密钥的长度
   partKeyNum：密钥成分的数量
   partKey：存放各个密钥成分的数组缓冲
   输出参数：
   keyByLMK：合成的密钥密文，由LMK加密
   checkValue：合成的密钥的校验值
 */
int UnionHsmCmdA5(int SM4Mode,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int partKeyNum,char partKey[][50],char *keyByLMK,char *checkValue);

/*
   函数功能：
   U2指令，使用指定的应用主密钥进行2次离散得到卡片应用子密钥，
   使用指定控制密钥子密钥进行加密保护输出并进行MAC计算。
   输入参数：
securityMech: 安全机制(S：单DES加密和MAC, T：三DES加密和MAC)
mode: 模式标志, 0-仅加密 1-加密并计算MAC
id: 方案ID, 0=M/Chip4(CBC模式，强制填充X80) 1=VISA/PBOC(带长度指引的ECB) 2=PBOC1.0模式(ECB模式，外带填充数据)

mk: 根密钥
mkType: 根密钥类型109= MK-AC, 209= MK-SMI, 309= MK-SMC, 509= MK-DN
当使用加密机内密钥时该域不起作用
mkIndex: 根密钥索引, K+3位索引方式读取加密机内保存密钥
mkDvsNum: 根密钥离散次数, 1-3次
mkDvsData: 根密钥离散数据, n*16H, n代表离散次数

pkType: 保护密钥类型 0=TK(传输密钥)1=DK-SMC(使用MK-SMC实时离散生成的子密钥)
pk: 保护密钥
pkIndex: 保护密钥索引
pkDvsNum: 保护密钥离散次数,仅当“保护密钥类型”为1时有,从MK-SMC离散得到DK-SMC的离散次数,范围为1-3
pkDvsData: 保护密钥离散数据,n*16H仅当“保护密钥类型”为1时有,保护密钥的离散数据，其中n为“保护密钥离散次数”

proKeyFlag:过程密钥标识，Y:计算过程密钥　N:不计算过程密钥　可选项:当没有该域时缺省为N
proFactor: 过程因子(16H),可选项:仅当过程密钥标志为Y时有

ivCbc: IV-CBC,8H 仅当“方案ID”为0时有

encryptFillDataLen: 加密填充数据长度(4H),仅当“方案ID”为2时有（不大于1024）
和密钥明文一起进行加密的数据长度
encryptFillData: 加密填充数据 nB 仅当“方案ID”为2时有,和密钥明文一起进行加密
encryptFillOffset: 加密填充数据偏移量 4H 仅当“方案ID”为2时有
将密钥明文插入到加密填充数据的位置, 数值必须在0到加密填充数据长度之间

ivMac: IV-MAC,16H 仅当“模式标志”为1时有
macDataLen: MAC填充数据长度 4H 仅当“模式标志”为1时有,和密钥密文一起进行MAC计算的数据长度
macData: MAC填充数据 nB 仅当“模式标志”为1时有,和密钥密文一起进行MAC计算的数据
macOffset: 偏移量 4H 仅当“模式标志”为1时有,将密钥密文插入到MAC填充数据的位置
数值必须在0到MAC填充数据长度之间
输出参数：
mac: MAC值 8B 仅当“模式标志”为1时有
criperDataLen: 密文数据长度 4H 密文数据长度(必须是8的倍数，并等于前缀长度、后缀长度、密钥长度的和)
criperData: nB 输出的密文数据
checkValue: 校验值

 */
int UnionHsmCmdU2(char *securityMech, char *mode, char *id, char *mkType, char *mk, int mkIndex, int mkDvsNum, char *mkDvsData, char *pkType, char *pk, int pkIndex, int pkDvsNum, char *pkDvsData, char *proKeyFlag, char *proFactor, char *ivCbc, int encryptFillDataLen, char *encryptFillData, int encryptFillOffset, char *ivMac, int macDataLen, char *macData, int macOffset, char *mac, int *criperDataLen, char *criperData, char *checkValue);

int UnionHsmCmdKW(char *mode, char *id, int mkIndex, char *mk, char *iv,int divNum, char *divData, int lenOfPan, char *pan, char *bh, char *atc, int lenOfData,char *data, char *ARQC, char *ARC, char *csu, int lenOfData1,char *data1,char *ARPC);

// added 2011-11-08 ０６机用，与09KW指令相同
int UnionHsmCmdKX(char *mode, char *id, int mkIndex, char *mk, char *iv,int lenOfPan, char *pan, char *bh, char *atc, int lenOfData,char *data, char *ARQC, char *ARC, char *csu, int lenOfData1,char *data1,char *ARPC);

int UnionHsmCmdU0(char *mode, char *id, int mkIndex, char *mk, char *iv,char *pan, char *bh, char *atc, int lenOfPlainData,char *plainData, int *lenOfCiperData, char *ciperData);

/*
   函数功能：
   UG指令，请求转换RSA私钥从LMK到KEK,LMK保护的RSA私钥为REF结构,KEK使用Mode定义的算法保护REF结构的RSA私钥的每个元素。
   输入参数：
mode: 加密私钥每个元素的加密算法 "00"-DES_ECB, "01"-DES_ECB_LP, "02"-DES_ECB_P,
"10"-DES_CBC, "11"-DES_CBC_LP, "12"-DES_CBC_P
keyType：密钥类型
key: 该密钥可由HSM内安全非易失存储区内密钥以及由LMK加密的密文送入
iv: 初始向量, mode为CBC编码时，该域存在。
vkLength: 私钥数据的数据长度
vk: nB 由LMK保护的RSA私钥（REF结构）
输出参数：
vkByKey: key加密的vk数据串
 */
int UnionHsmCmdUG(char *mode, TUnionDesKeyType keyType,char *key, char *iv,int vkLength, char *vk, char *vkByKey);

/*
   函数功能:
   TK指令，私钥加密
   输入参数:
encryMode: 1H 算法标识 1－解密数据 2－加密数据 3－签名 4－加密密钥 5－解密密钥
vkLength: 送入私钥长度
vk: nB/1A+3H DER编码的私钥或私钥在HSM安全存储区内的索引
keyLength: 1H 待加/解密密钥长度，0 = 单长度密钥，1 = 双长度密钥；encryMode = 4 或 encryMode = 5时该域存在
inputDataLength: 加解密数据长度
inputData: nB/1A+3H 待加解密数据或待加解密密钥在HSM安全存储区内的索引
输出参数:
outputData: nB 加解密/加解密密钥数据
 */
int UnionHsmCmdTK(char encryMode, int vkLength, char *vk, char keyLength,int inputDataLength, char *inputData, char *outputData);

/*
   函数功能:
   产生随机数
   输入参数:
   lenOfRandomData 随机数长度
   输出参数:
   randomData 随机数
 */
int UnionHsmCmdTE(int lenOfRandomData, char *randomData);

/*
   函数功能:
   产生分散/过程密钥
   输入参数:
   mode 算法标志:
   "00" - EMV2000
   "01" - DES_CBC
   "02" - DES_ECB
   "10" - VISA
   "11" - EMV2000
   masterKeyType	种子密钥类型
   masterKey	种子密钥
   desKeyType	子密钥类型
   keyLengthFlag	待分散主密钥长度	
   0 = 单长度密钥
   1 = 双长度密钥
   data		离散数据
   masterKey	种子密钥
   iv		向量
   输出参数:
   key	子密钥密文

 */

int UnionHsmCmdX1(char *mode, TUnionDesKeyType masterKeyType, char *masterKey,
		TUnionDesKeyType desKeyType, char *keyLengthFlag,char *data, char *iv, char *key);

/*
   函数功能:
   将一把密钥加密的数据转换为另一把密钥加密
   输入参数:
   srcKeyType			      源密钥的类型
   srcKey	  1A+3H/16H/32H/48H       源密钥
   srcModel				源加密算法
   "00"=DES_ECB
   "01"=DES_ECB_LP
   "02"=DES_ECB_P
   "10"=DES_CBC
   "11"=DES_CBC_LP
   "12"=DES_CBC_P
   "20"=M/Chip4（CBC模式，强制填充X80）
   "21"=VISA/PBOC（带长度指引的ECB）
   dstKeyType			      目的密钥类型
   dstKey	  1A+3H/16H/32H/48H       目的密钥
   dstModel				目的加密算法
   "00"=DES_ECB
   "02"=DES_ECB_P
   lenOfSrcCiphertext		      源密钥加密的密文数据长度
   srcCiphertext   nB		      源密钥加密的密文数据
   iv_cbc	  16H		     初始向量。源加密算法为CBC编码时，该域存在
   输出参数:
   dstCiphertext   nB		      目的密钥加密的密文数据
 */
int UnionHsmCmdY1(TUnionDesKeyType srcKeyType,char *srcKey,int srcKeyIdx,char *srcModel,TUnionDesKeyType dstKeyType,char *dstKey,int dstKeyIdx,char *dstModel,int lenOfSrcCiphertext,char *srcCiphertext,char *iv_cbc,char *dstCiphertext);

/*
   函数功能：
   UD指令，增加终端控制命令，管理员在授权状态下操作，实现对密钥的恢复或备份功能。
   输入参数：
   algorithmFlag：	[1A]	算法标识，‘D’-des算法，‘S’-SM4算法，默认为'D’
modeFlag:       [1N]		    模式标识：
1-恢复密钥 2-备份密钥
mkIndex:	[1A+3H] 密钥位置：      用来产生卡片密钥的发卡行主密钥。
使用LMK对28-29相应变种加密。
1A+3H表示使用K+3位索引方式读取加密机内保存密钥。
（暂时先实现1A+3H取密钥）
keyLocale       [32H/1A+32H]
key:	    [32H]		   密钥
仅当模式标志为1时有，LMK04_05加密


输出参数：
encryKey:       [N*8B/N*16B/N*24B]      LMK对加密的密钥密文(lmk04_05加密)
encryKeyLen:			    密钥长度
checkValue:     [16H]		   用密钥加密0的结果
 */
int UnionHsmCmdUD(char *algorithmFlag,char *modeFlag,char *mkIndex,char *keyLocale,char *key,char *encryKey,int *pencryKeyLen,char *checkValue);

/*
   输入参数
   mode 加密模式标识 0=离散密钥3DES加密
   1=离散密钥3DES解密
   2=做1DES_MAC（密钥的左8字节）(ANSI X9.19 MAC)
   3=做3DES_MAC(ANSI X9.19 MAC)
   4=离散密钥DES加密
   5=离散密钥DES解密
   6=过程密钥DES加密
   7=过程密钥DES解密
   id 方案ID 加解密算法模式：
   01=ECB
   02=CBC
   03=CFB
   04=OFB
   mkType 根密钥类型，产生卡片密钥的发卡行密钥类型：
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk 根密钥
   mkIndex 根密钥索引
   mkDvsNum 离散次数
   mkDvsData 离散数据
   proFactor 过程数据
   lenOfData 数据长度
   data 数据
   输出参数
   criperDataLen 数据长度  
   criperData 数据   
 */
int UnionHsmCmdU1(char *mode, char *id, char *mkType, char *mk, int mkIndex,int mkDvsNum, char *mkDvsData, char *proFactor, int lenOfData,char *data, int *criperDataLen, char *criperData);

/*
   输入参数
   mode 模式标识 1=计算MAC
   2=校验MAC
   1=计算C-MAC
   id 方案ID
   0=3DESMAC（使用子密钥进行3DESMAC）
   1=DESMAC（使用过程密钥进行DESMAC）
   2=TAC(使用子密钥进行DESMAC)
   3=3DESMAC（使用2倍长过程密钥进行3DESMAC）
   mkType 根密钥类型 
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk 根密钥
   mkIndex 根密钥索引
   mkDvsNum 离散次数
   mkDvsData 离散数据
   proFactor 过程数据
   macFillFlagMAC 数据填充标识
   1=强制填充0X80
   0=不强制填充0X80
   ivMac IV-MAC 
   lenOfData MAC计算数据长度
   data MAC计算数据
   macFlagMAC 长度标识   
   1=4 BYTE
   2=8 BYTE
   输出参数
   checkMac
   mac MAC值
 */
int UnionHsmCmdUB(char *mode, char *id, char *mkType,char *mk, int mkIndex,int mkDvsNum,char *mkDvsData, char *proFactor, char *macFillFlag, char *ivMac,int lenOfData,char *data, char *macFlag, char *checkMac, char *mac);

/*
   输入参数
   mode 模式标识 
   2=校验MAC
   1=计算C-MAC
   id 方案ID
   0=3DESMAC（使用子密钥进行3DESMAC）
   1=DESMAC（使用过程密钥进行DESMAC）
   2=TAC(使用子密钥进行DESMAC)
   mk 根密钥
   mkDvsNum 离散次数
   mkDvsData 离散数据
   proFactor 过程数据 
   macFillFlag MAC数据填充标识
   1=强制填充0X80
   0=不强制填充0X80
   ivMac IV-MAC 
   lenOfData MAC计算数据长度
   data MAC计算数据
   macFlag MAC长度标识
   1=4 BYTE
   2=8 BYTE
   输出参数
   checkMac
   mac MAC值
 */
int UnionHsmCmdU3(char *mode, char *id, char *mk, int mkIndex,int mkDvsNum,char *mkDvsData, char *proFactor, char *macFillFlag, char *ivMac,int lenOfData,char *data, char *macFlag, char *checkMac, char *mac);

/*
   输入参数
   mode 模式标志
   1-加密并计算密钥校验值(checkvalue)
   id 方案ID
   0=DES_ECB(直接加密，不需填充)
   1=DES_CBC(直接加密，不需填充)
   mkType 根密钥类型
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk 根密钥
   mkDvsNum 离散次数
   mkDvsData 离散数据
   pkType 保护密钥类型
   0=TK(ZEK)（传输密钥）
   1=ZMK
   pk 保护密钥 
   ivCbc IV-CBC，仅当“方案ID”为1时有
   输出参数
   keyCheckValue 密钥校验值
   keyValue 密文数据
 */
int UnionHsmCmdUK(char *securityMech, char *mode, char *id, char *mkType, char *mk, int mkIndex,int mkDvsNum, char *mkDvsData, char *pkType, char *pk, int pkIndex,char *ivCbc,char *keyValue, char *keyCheckValue);

/*
   功能：将安全报文保护的密钥导入到加密机中（EMV 4.1/PBOC）
   输入参数
   mode 模式标志 
   1-验证MAC并解密
   id 方案ID 
   0=M/Chip4（CBC模式，强制填充X80）
   1=VISA/PBOC（带长度指引的ECB）
   mkFlag 存储标志 
   1-将密钥存储到加密机中
   mkIndex 存储索引
   mkLengthFlag 密钥方案，指定导入密钥类型，以确定加密的LMK：
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN

   pkType 保护密钥类型
   0=TK（传输密钥）
   1=DK-SMC（使用MK-SMC实时离散生成的子密钥）
   pk 保护密钥 
   pkDvsNum 保护密钥离散次数
   pkDvsData 保护密钥离散数据 
   ivCbc IV-CBC
   criperDataLen 密文长度 
   criperData 密文数据
   mac MAC
   ivMac IV-MAC 
   macDataLen MAC填充数据长度
   macData MAC填充数据 
   macOffset 偏移量
   输出参数
   keyValue LMK保护的密钥密钥校验值
   checkValue 密钥校验值
 */
int UnionHsmCmdU4(char *mode,char *id,char *mkFlag,int mkIndex,char *mkType,char *mkLengthFlag,char *pkType,char *pk,int pkIndex,int pkDvsNum,char *pkDvsData,char *proKeyFlag,char *ivCbc,int criperDataLen,char *criperData,char *mac,char *ivMac,int macDataLen,char *macData, int macOffset,char *keyValue,char *checkValue);

/*
   功能
   加解密数据
   输入参数：
   encryFlag：1－解密；2－加密
   mode：01－DES_ECB；02－DES_CBC
   keyType：密钥类型，000：ZMK；00A：ZAK
   keyLen：密钥长度，0 = 单长度密钥；1 = 双长度密钥
   key：加解密密钥
   iv：CBC模式时使用的初始化向量
   lenOfData：需要做摘要的数据长度
   pData：需要做摘要的数据
   输出参数
   pCipherDataLen：密文数据长度
   pCipherData：密文数据
 */
int UnionHsmCmdTG(char encryFlag,char *mode,char *keyType,char keyLen,char *key,char *iv,int lenOfData,char *pData,int *pCipherDataLen,char *pCipherData);

/*
   函数功能:
   将数据有keyA加密转加密为keyB加密
   输入参数:
   srcKeyType			      源密钥的类型
   srcKey	  1A+3H/16H/32H/48H       源密钥
   srcModel				源加密算法
   "00"=DES_ECB
   "01"=DES_ECB_LP
   "02"=DES_ECB_P
   "10"=DES_CBC
   "11"=DES_CBC_LP
   "12"=DES_CBC_P
   "20"=M/Chip4（CBC模式，强制填充X80）
   "21"=VISA/PBOC（带长度指引的ECB）
   srcIV	   16H		     初始向量。源加密算法为CBC编码时，该域存在
   dstKeyType			      目的密钥类型
   dstKey	  1A+3H/16H/32H/48H       目的密钥
   dstModel				目的加密算法
   "00"=DES_ECB
   "02"=DES_ECB_P
   dstIV	   16H		     初始向量。目的加密算法为CBC编码时，该域存在
   lenOfSrcCiphertext		      源密钥加密的密文数据长度
   srcCiphertext   nH		      源密钥加密的密文数据

   输出参数:
   dstCiphertext   nH		      目的密钥加密的密文数据
 */
int UnionHsmCmdUE(char *srcKeyType,char *srcKey,int srcKeyIdx,char *srcModel,char *srcIV,char *dstKeyType,char *dstKey,int dstKeyIdx,char *dstModel,char *dstIV,int lenOfSrcCiphertext,char *srcCiphertext,char *dstCiphertext);

/*
   功能
   将密钥由KEK加密转为LMK加密
   输入参数：
   mode：算法标识
   00－DES_ECB
   01－DES_ECB_LP
   02－DES_ECB_P
   10－DES_CBC
   11－DES_CBC_LP
   12－DES_CBC_P
   kekType：密钥类型，000：ZMK；
kekLen:密钥长度，0 = 单长度密钥，1 = 双长度密钥
kek：密钥加密密钥
iv：CBC模式时使用的初始化向量
keyType：密钥类型，000：ZMK；
lenOfKeyCipherTextByKek：需要做摘要的数据长度
keyCipherTextByKek：需要做摘要的数据
输出参数
keyCipherTextByLmk：密文数据
 */
int UnionHsmCmdTU(char *mode,char *kekType,char kekLen,char *kek,char *iv,char *keyType,char keyLen,int lenOfKeyCipherTextByKek,char *keyCipherTextByKek,int *lenOfKeyCipherTextByLmk,char *keyCipherTextByLmk);

int UnionHsmCmdVY(char *k1, int lenOfK1, char *k2, int lenOfK2,int isDis, char *disFac, int lenOfDisFac, int pinMaxLen, char *pinByZPK1, int lenOfPinByZPK1, char *pinFormatZPK1, int lenOfPinFormatZPK1, char *pinFormatMDKENC, int lenOfPinFormatMDKENC, char *accNo1, int lenOfAccNo1, char *accNo2, int lenOfAccNo2,char *pinSec, char *pinFormat, char *isWeaKey);

int UnionHsmCmdPE(char *pinByLmk,char *accNo,int fldNum,char fldGrp[][128+1],char *retValue,int sizeOfBuf);

/* 输入参数
   algorithmID	算法标识，0:ECB	1:CBC
   keyType		密钥类型
   zek		保护密钥
   iv		iv
   keyByZek	密钥密文
   keyFlag		密钥方案X/U
   sizeOfBuf	接收keyByLmk结果的缓冲大小
   输出参数
   keyByLmk	密钥密文
   checkValue	检验值
 */
int UnionHsmCmdM8(char *algorithmID,TUnionDesKeyType keyType,char *zek,char *iv,char *keyByZek,char *keyFlag,char *keyByLmk,char *checkValue,int sizeOfBuf);

// 生成密钥SM4密钥
/* 输入参数
mode:
0－产生密钥
1－产生密钥并在ZMK下加密
keyType		密钥类型
zmk			仅当模式为1时才显示该ZMK域
输出参数
keyByLMK		LMK保护的密钥密文值
keyByZMK		ZMK保护的密钥密文值
checkValue		密钥校验值
 */
int UnionHsmCmdWI(char *mode, TUnionDesKeyType keyType, char *zmk, char *keyByLMK,char *keyByZMK, char *checkValue);

//生成SM4密钥并打印一个成份 
/* 输入参数
   keyType         密钥类型
   fldNumi         打印域个数
   fld             打印域
   输出参数
   component       成分
 */
int UnionHsmCmdWJ(TUnionDesKeyType keyType,int fldNum,char fld[][80],char *component);


/*
   函数功能:
   WF::  将一把密钥加密的数据转换为另一把密钥加密
   输入参数:
   algType      算法转换类型
01:3DES算法转SM4
02:SM4算法转3DES
03:SM4算法转SM4
srcKeyType             源密钥的类型
srcKey                 源密钥
srcKeyCheckValue       源秘钥校验值
srcModel               源加密算法
"00"=ECB
"01"=ECB_LP
"02"=ECB_P
"10"=CBC
"11"=CBC_LP
"12"=CBC_P
dstKeyType                            目的密钥类型
dstKey    1A+3H/16H/32H/48H             目的密钥
dstKeyCheckValue                        目的秘钥校验值 
dstModel                                目的加密算法
"00"=ECB
"02"=ECB_P
"10"=CBC
"12"=ECB_P
lenOfSrcCiphertext                    源密钥加密的密文数据长度
srcCiphertext   nB                    源密钥加密的密文数据
iv_cbc    16H                初始向量。源加密算法为CBC编码时，该域存在
iv_cbcDstKey 16H                     目的加密算法位cbc北嗦胧保糜虼嬖?

输出参数:
dstCiphertext   nB                    目的密钥加密的密文数据
返回值：
成功：>=0  

 */
int UnionHsmCmdWF(char *algType, TUnionDesKeyType srcKeyType,char *srcKey, char* srcKeyCheckValue, int srcKeyIdx,char *srcModel, TUnionDesKeyType dstKeyType,char *dstKey, char* dstKeyCheckValue, int dstKeyIdx,char *dstModel,int lenOfSrcCiphertext,char *srcCiphertext, char *iv_cbc, char *iv_cbcDstKey, char *dstCiphertext);

/*
   RQC校验
   1=执行ARQC校验和ARPC产生
   2=仅仅执行ARPC产生
   id        方案ID   0=PBOC3.0(采用SM4算法计算)
   mkIndex           mk索引，如果mk值为空则使用索引方式
   mk                加密计算主密钥
   mkCheckValue      加密计算主密钥校验值
   pan               离散卡片密钥使用的帐号或者帐号序列号
   atc               使用ATC来计算过程密钥
   lenOfData         加密数据长度
   data             要加密的数据
   ARQC             ARQC
   ARC              ARC
   输出参数:
   ARPC             ARPC
   返回值：
   成功：>=0
 */
int UnionHsmCmdWG(char *mode, char *id, int mkIndex, char *mk, char* mkCheckValue, int lenOfPan, char *pan, char *atc, int lenOfData, char *data, char *ARQC, char *ARC, char *ARPC);

/*
   功能： 
   RSA私钥解密数据转ZPK加密数据
   参数:   
   vkeyLen	私钥长度 私钥存储在加密机内部时，当私钥长度取0
   vkey	私钥只能为外部送入或者为HSM内安全存储区内密钥。
   dataPaddingMode 	?00=非填充 01=PKCS#1 v1.5 02=OAEP
   dataLength  加解密数据长度	
   data 加解密数据
   keyType	ZMKTYPE=0;ZPKTYPE=1;TPKTYPE=2 
   Key  用对应的LMK密钥对解密
   Specialalgorithm	专用算法标识
   输出:
   pin 密文
 */
int UnionHsmCmd70(int vkeyLen, char *vkey, int vkIndex, int dataPaddingMode, int dataLength, char* data,  int keyType, char* key,char* Specialalgorithm, char *pin);

/*
   功能：
   把zpk加密的字符密码密文转换为另一zpk加密的字符密码密文
   参数:
   srcAlgorithmID 源密钥算法标识，0代表国际,1代表国密
   srcZPK 当前加密PIN块的ZPK；LMK对（06-07）下加密
   dstAlgorithmID 目的密钥算法标识，0代表国际,1代表国密
   dstZPK 将要加密PIN块的ZPK；LMK对（06-07）下加密。
   srcPin 源ZPK加密的字符密码密文
   srcFormat 源PIN格式  ,01-ANSI9.8
                         09-农商行格式
   dstFormat 目的PIN格式,01-ANSI9.8
                         09-农商行格式
   srcPan 源12位有效主账号
   dstPan 目的12位 有效主账号
   输出:
   pin 返回PIN密文 
 */
int UnionHsmCmdH8(int srcAlgorithmID,char* srcZPK,int dstAlgorithmID, char* dstZPK, char *srcPin,char *srcFormat,char *dstFormat, char* srcPan, char* dstPan, int* lenOfPin, char* pin);

/*
   功能：
   SM1数据加解密
   参数:
   algorithmID 运算方式 0：SM1 ECB加密,1：SM1 ECB解密, 2：SM1 CBC加密, 3：SM1 CBC解密.
   keyType 密钥类型
   keyValue 加密密钥
   iv 仅当algorithmID等于2、3时有此域
   inData 待处理数据 (algorithmID='1', '3' bcdhex)
   dataLen 待处理数据长度，必须为16的倍数
   sizefEncryptDaga 存放已处理据缓冲区大小 
   输出:
   encryptData 已处理数据�(algorithmID='0','2' bcdhex)
   回值：
   成功：>=0 为已加密数据长度
   失败: < 0
 */
int UnionHsmCmdM1(char *algorithmID,TUnionDesKeyType keyType, char *zek, char *iv, char *inData, int dataLen, char * encryptData, int sizefEncryptDaga);

/*
   功能：8/R9生成随机数

   输入参数: 
   genKeyLen 	指定产生口令的长度
   genKeyType 口令的复杂度		1：纯数字 2、纯字母 3、数字+字母

   输出参数：
   outPutKey  产生的口令 

   返回值：
   产生的口令的长度
 */

int UnionHsmCmdR8(int genKeyLen,  char genKeyType, char* outPutkey);

// 将一个ZMK加密的密钥转换为LMK加密,SM4算法
int UnionHsmCmdSV(TUnionDesKeyType keyType, char *zmk, char *keyByZmk, char *keyByLmk, char *checkValue);
// 将一个LMK加密的密钥转换为ZMK加密,SM4算法
int UnionHsmCmdST(TUnionDesKeyType keyType, char *key, char *zmk, char *keyByZMK, char *checkValue);

int UnionHsmCmdTA(int mode ,int keyLen,char *keyType,char *keyPlain,char *keybyLmk);
int UnionHsmCmdUI(char *mode, char *iv,int kekLen, char *Kek,char *keyType,int n ,
		int mLen, char *m,int eLen, char *e,int dLen, char *d,int pLen, char *p,int qLen, char *q,
		int dmp1Len, char *dmp1,int dmq1Len, char *dmq1,int coefLen, char *coef, unsigned char *keyByLmk,
		int *keyByLmkLen);
//	vkByKek与UG指令结果对应
//	vkByKey: key加密的vk数据串
int UnionHsmCmdUI2(char *mode,char *keyType,int keyLen,char *keyValue,char *iv,char *vkByKek,int vkByKeyLen,unsigned char *keyByLmk);
int UnionHsmCmdTY(int vkIdx,int keyLen,unsigned char *vkByLmk );

/*
   功能：用WWK密钥加解密数据
   输入参数
cryptoFlag: 	0:解密 1:加密
wwk: 		LMK22-23加密
encrypFlag:  	0: ECB模式 1: CBC模式
iv:  		初始向量(仅当CBC模式)
lenOfData: 	输入数据字节数（8的倍数）
data:		待加密或解密的数据
输出参数
outdata: 	输出结果
 */
int UnionHsmCmdB8(char *wwk,int cryptoFlag,int encrypFlag,char *iv,int lenOfData,char *data,char *outdata,int sizeOfOutData);

// SM4算法加解密数据
/* 输入参数
   encryptFlag          加解密标识
   1——解密    
   2——加密
mode:   
01 –SM4-ECB
02 --SM4-CBC
keyType                 支持的密钥类型为ZEK/ZMK
lenOfKey                        1 = 双长度密钥
Key             ‘S’表示SM4密钥,该密钥由LMK加密的密文送入
checkValue                      Key校验值
iv                      初始向量。 Mode=02和，该域存在。
lenOfDate                       加解密数据长度，长度必须是16的倍数
data                    待加解密数据
输出参数
value                   加解密后的数据
 */     
int UnionHsmCmdWA(int encryptFlag, char *mode, char *keyType, int lenOfKey, char *Key, char *checkValue, char *iv, int lenOfDate, unsigned char *data, char *value); 

//用SM2私钥做SM2解密
/*输4入参数
  vkIndex                       密钥索引
  lenOfVK                       外带密钥长度
  VK                    外带密钥
  lenOfCiphertext               密文长度
  ciphertext            密文
  输出参数
  lenOfData             数据长度
  data                  数据
 */
int UnionHsmCmdK6(int vkIndex,int lenOfVK,char *VK,int lenOfCiphertext,unsigned char *ciphertext,int *lenOfData,char *data);

//用SM2公钥做SCE加密
/*输入参数
  pkIndex                       公钥索引
  lenOfPK                       公钥明文长度
  PK                    公钥明文
  lenOfData             数据长度
  data                  数据
  输出参数
  lenOfCiphertext               密文长度
  ciphertext            密文
 */
int UnionHsmCmdK5(int pkIndex,int lenOfPK,char *PK,int lenOfData,char *data,int *lenOfCiphertext,unsigned char *ciphertext);

// 导入SM2密钥对
/* 输入参数
   index                密钥索引
   lenOfVK      密钥密文字节数
   VK           密钥密文
   输出参数
   无
 */
int UnionHsmCmdK2(int index, int lenOfVK, unsigned char *VK);

/*
   函数功能：
   WH离散卡片密钥并以安全报文方式导出（PBOC3.0）
   输入参数：
securityMech: 安全机制(R：SM4加密和MAC)
mode: 模式标志, 0-仅加密 1-加密并计算MAC
id: 方案ID, 0=PBOC3.0模式（CBC模式，强制填充X80） 1= PBOC3.0模式（带长度指引的ECB） 2=PBOC3.0模式(ECB模式，外带填充数据)

mk: 根密钥
mkType: 根密钥类型109= MK-AC, 209= MK-SMI, 309= MK-SMC, 509= MK-DN
当使用加密机内密钥时该域不起作用
mkIndex: 根密钥索引, K+3位索引方式读取加密机内保存密钥
mkCheckValue: 根密钥校验值
mkDvsNum: 根密钥离散次数, 1-3次
mkDvsData: 根密钥离散数据, n*16H, n代表离散次数

pkType: 保护密钥类型 0=TK(传输密钥)1=DK-SMC(使用MK-SMC实时离散生成的子密钥)
pk: 保护密钥
pkIndex: 保护密钥索引
pkDvsNum: 保护密钥离散次数,仅当“保护密钥类型”为1时有,从MK-SMC离散得到DK-SMC的离散次数,范围为1-3
pkDvsData: 保护密钥离散数据,n*16H仅当“保护密钥类型”为1时有,保护密钥的离散数据，其中n为“保护密钥离散次数”
pkCheckValue: 保护密钥校验值

proKeyFlag:过程密钥标识，Y:计算过程密钥　N:不计算过程密钥　可选项:当没有该域时缺省为N
proFactor: 过程因子(32H),可选项:仅当过程密钥标志为Y时有

ivCbc: IV-CBC,32H 仅当“方案ID”为0时有

encryptFillDataLen: 加密填充数据长度(4H),仅当“方案ID”为2时有（不大于1024）
和密钥明文一起进行加密的数据长度
encryptFillData: 加密填充数据 nB 仅当“方案ID”为2时有,和密钥明文一起进行加密
encryptFillOffset: 加密填充数据偏移量 4H 仅当“方案ID”为2时有
将密钥明文插入到加密填充数据的位置, 数值必须在0到加密填充数据长度之间

ivMac: IV-MAC,32H 仅当“模式标志”为1时有
macDataLen: MAC填充数据长度 4H 仅当“模式标志”为1时有,和密钥密文一起进行MAC计算的数据长度
macData: MAC填充数据 nB 仅当“模式标志”为1时有,和密钥密文一起进行MAC计算的数据
macOffset: 偏移量 4H 仅当“模式标志”为1时有,将密钥密文插入到MAC填充数据的位置
数值必须在0到MAC填充数据长度之间
输出参数：
mac: MAC值 16H 仅当“模式标志”为1时有
criperDataLen: 密文数据长度 4H 密文数据长度(必须是8的倍数，并等于前缀长度、后缀长度、密钥长度的和)
criperData: nB 输出的密文数据
checkValue: 16H 校验值

 */
int UnionHsmCmdWH(char *securityMech, char *mode, char *id, char *mkType, char *mk, int mkIndex, char *mkCheckValue, int mkDvsNum, char *mkDvsData, char *pkType, char *pk, int pkIndex, int pkDvsNum, char *pkDvsData, char *pkCheckValue, char *proKeyFlag, char *proFactor, char *ivCbc, int encryptFillDataLen, char *encryptFillData, int encryptFillOffset, char *ivMac, int macDataLen, char *macData, int macOffset, char *mac, int *criperDataLen, char *criperData, char *checkValue);

/*
   功能:	FB:FC)生成动态码

   输入
   mk	密钥	32H/1A+32H/1A+3H	用于离散得到输出保护密钥的根密钥（外带密文为由密码机主密钥保护）
   mkType 密钥类型	3H	当根密不使用索引时存在此域。至少支持00A，109，309中的一种
   pkDvsNum 保护密钥离散次数	1N	0~2次
   pkDvsData1 离散数据1	32H	仅当“离散次数”大于0时存在
   pkDvsData2 离散数据2	32H	仅当“离散次数”大于1时存在

   seedLen    种子密钥密文长度	4N	16的整数倍
   seedKeyCiper 种子密钥密文	32H	由根密钥离散后的密钥保护
   algorithm 1：SM3算法
   otpKeyLen 动态密码长度	2N	默认为06
   challengeValLen	挑战值长度	2N	00~~99
   challengeValue 挑战值 nA
   timeChangeCycle 时钟变化周期 
   输出
   otpKey	OTP动态密码	6N	6位OTP动态密码
 */

int UnionHsmCmdFB(char *mk, char* mkType, int mkIndex, int pkDvsNum, char* pkDvsData1, char* pkDvsData2, int seedLen, char* seedKeyCiper, int otpKeyLen,int algorithm,  int challengeValLen, char* challengeValue, int timeChangeCycle, char* otpKey);

//命令代码	2A	值“FF”
//保护密钥根密钥	32H/1A+32H/1A+3H	用于离散得到输出保护密钥的根密钥，K+索引号（外带密文为由密码机主密钥保护）
//根密钥类型	3H	当根密不使用索引时存在此域。至少支持00A，309中的一种
//变形前保护密钥离散次数	1N	1~2次
//变形前保护离散数据	n*32H	当保护密钥离散次数为1~2时有
//其中n代表保护密钥离散次数
//变形后保护密钥离散次数	1N	1~2次
//变形后保护离散数据	n*32H	当保护密钥离散次数为1~2时有
//其中n代表保护密钥离散次数
//变形前种子密文长度	4N	16的整数倍
//变形前种子密文	n*2H	由根密钥离散后的密钥保护（PKCS#5填充）
//变形算法	1N	1： SM3
//变形前种子校验值	16H	变形算法为1时，与检查sm3(种子)前8字节是否一致。
//变形后的种子长度	4N	从左到右截取多少个字节
//变形要素长度	4N	
//变形要素	n*2H	
//种子插入到变形要素中位置	4N	如0,则变形数据为：种子+变形要素

//输出域	长度	说　明
//响应代码	2A	值“FG”
//错误代码	2N	
//变形后的种子密文长度	4N	
//变形后的种子密文	n*2H	由变形后的保护密钥保护（PKCS#5填充）
//变形后密种子校验值	16H	

int UnionHsmCmdFF(char *mk, char* mkType, int mkIndex, int pkDvsNum, char* pkDvsData1, char* pkDvsData2, int newPkDvsNum, char* newPkDvsData1, char* newPkDvsData2,  int seedLen, char* seedKeyCiper, int algorithm, char *seedCheckValue,  int newSeedLen, int tranElementLen, char *tranElement, int tranOffset, char* seed, char* newSeedCheckValue);

/*
   功能 F8:?/F9)随机生成令牌种子并使用特定密钥保护输

   输入	：
   mk		根密钥	32H/1A+32H/1A+3H	
   mkIndex 	根秘钥索引
   mkType	?密钥类型	3H	当根密不使用索引时存在此域输入。至少支持00A, 309中的一种
   mkDvsNum	保护密钥离散次数	1N	0~2次
   mkDvsData1	离散数据1	32H	仅当“离散次数”大于0时存在
   mkDvsData2	离散数据2	32H	仅当“离散次数”大于1时存在
   seedLen		种子长度	2N	默认为16，小于或等于16

   输出	:
   seedKey	种子密文	32H	由根密钥离散后的保护密钥保护输出
   seedKeyCheckValue 种子校验值	16H	密钥校验值（国密SM4算法）

 */

int UnionHsmCmdF8(char *mk, int mkIndex, char*mkType, int mkDvsNum, char* mkDvsData1, char* mkDvsData2 ,int seedLen,int seedAlgorithm, char *seedKey, char *seedKeyCheckValue);

/*
   命令代码	2	值“D”
   根密钥	32H/1A+32H/1A+3H	用于离散得到输出保护密钥的根密钥，K+索引号（外带密文为由密码机主密钥保护）
   根密钥类型	3H	当根密不使用索引时存在此域。至少支持00A，309中的一种
   保护密钥离散次数	1N	1~2次
   离散数据1	32H	
   离散数据2	32H	仅当“离散次数”大于1时存在
   种子密钥密文	32H	由根密钥离散后的密钥保护
   生成动态口令算法	1N	1：SM3算法，2：SM4算法
   动态口令长度	2N	默认为06
   挑战值长度	2N	00~~99
   挑战值	nA	
   时钟值T0	16N	
   时钟变化周期Tc	3N	秒为单位，1分钟为：060，1秒：001
   时钟窗口	2N	在n多少个时钟周期内验证口令
   待验证动态口令	nN	

   响应代码	2	值“E”
   错误代码	2	
   时钟偏移方向	1A	向前偏移：+
   向后偏移：-
   时钟偏移值	2N	偏移两个周期：02
 */

int UnionHsmCmdFD(char *mk, char* mkType, int mkIndex, int pkDvsNum, char* pkDvsData1, char* pkDvsData2, int seedLen, char* seedKeyCiper, int algorithm, int otpKeyLen, int challengeValLen, char* challengeValue,int timeChangeCycle, int timeValue,int timeWindow, char* otpKey, char* clockSkew,  int* clockSkewValue);

/*
   命令代码	"AE"
keyValue1:	LMK对(14-15)下加密的当前TMK、TPK或PVK
keyValue2:	LMK对(14-15)下加密的存储TMK、TPK或PVK	

返回：
keyByLMK:	当前密钥下加密的存储密钥	
checkValue:	校验值
 */
int UnionHsmCmdAE(char *keyValue1,char *keyValue2,char *keyByLMK,char *checkValue);

/*
   命令		"AG"	
tmk:		LMK对(14-15)下加密的TMK
tak:		LMK对(16-17)下加密的TAK

输出值:
keyByTMK:	TMK下加密的TAK
checkValue:	校验值
 */
int UnionHsmCmdAG(char *tmk,char *tak,char *keyByTMK,char *checkValue);
int UnionHsmCmd4H(char *vkIndex,int lenOfRandom,char *random,char *zpk,char *pan,char *pinByPK,char *pinByZPK);
/*
   功能:将对有公钥加密的一段数据产生MAC
   输入参数
vkIndex      :私钥索引，指定的私钥，用于解密数据密文
lenOfRandom  :随机数长度
random       :随机数
zak          :用于计算MAC的密钥
dataByPK     :经过公钥加密的数据
输出参数
macByZAK
 */
int UnionHsmCmd4G(char *vkIndex,int lenOfRandom,char *rando,char *zak,char *dataByPK,char *macByZAK);
// 导出密钥
/*
   命令		"WP"
   输入值：
   encryFlag       1N  	KEK加密密钥方案 1：3DES加密  2: SM4加密
   keyType1	3H	密钥类型
   keyValue1	1A+32H  密钥密文，LMK下加密密钥，‘S’+32H为SM4密钥，‘X’+32H为DES�
   checkValue1     16H     校验值
   keyType2	3H	KEK密钥类型	
   keyValue2	1A+32H  LMK下加密密钥，‘S’+32H为SM4密钥，‘X’+32H为DES,
   若KEK为SM4密钥，则使用SM4算法加密输出待转加密的密钥；
   若KEK为3DES密钥，则使用3DES算法加密输出待转加密的密钥。
   checkValue2     16H	KEK密钥校验值;		
   输出值:
   keyValue	32H	KEK加密下的密钥，32H的密文。	
 */
int UnionHsmCmdWP(int encryFlag,TUnionDesKeyType keyType1,char *keyValue1,char *checkValue1,TUnionDesKeyType keyType2,char *keyValue2
		,char *checkValue2,char *keyValue);

//将SM4密钥从SM2公钥下加密转换为LMK下加密(SM4、SM2)
// 东莞农商行
/*
   命令		"WO"
   输入值：
   vkIndex		2N	私钥索引“00”－“,"99":使用外带密钥
   lenOfVK		4N	仅当密钥索引为99时有此域，私钥的长度
   vkValue		nB	仅当密钥索引为99时有此域，SM2密钥密文(直接输入SM2的16进制密钥值)
   keyType		3H	密钥类型，输入密钥类型的枚举值
   lenOfSM4	4N	密钥密文长度	
   sm4BySM2	nB	密钥密文(SM2公钥加密),(直接输入SM2的16进制密钥值)
   输出值：
   keyValue	1A+32H	LMK加密的密钥		
   checkValue	16H	密钥的校验值。
 */
int UnionHsmCmdWO(int vkIndex,int lenOfVK,char *vkValue,TUnionDesKeyType keyType,int lenOfSM4,char *sm4BySM2,char *keyValue,char *checkValue);

/*
   函数功能:
   WE::  产生分散/过程密钥
   输入参数:
   mode 算法标志:
   "01" - SM4_CBC
   "02" - SM4_ECB
   masterKeyType   种子密钥类型
   masterKey       种子密钥
   desKeyType      子密钥类型
   keyLengthFlag   待分散主密钥长度   1 = 双长度密钥
   data            离散数据
   masterKey      	种子密钥 
   masterKeyIndex  K+3位索引方式读取加密机内保存密钥
   masterKeyCheckValue 种子密钥校验值
   iv              向量
   输出参数:
   key     子密钥密文
   返回值：
   成功：>=0

 */
int UnionHsmCmdWE(char *mode, TUnionDesKeyType masterKeyType, char *masterKey, char* masterKeyCheckValue, int masterKeyIndex, TUnionDesKeyType desKeyType, char *keyLengthFlag,char *data, char *iv, char *key);

/*
   输入参数
   mode 加密模式标识
   G=离散密钥SM4加密
   H=离散密钥SM4解密
   I=过程密钥SM4加密
   J=过程密钥SM4解密
   id 方案ID
   01=ECB
   02=CBC
   03=CFB
   mkType 根密钥类型
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk 根密钥
   mkCheckValue 根密钥校验值
   mkIndex 根密钥索引
   mkDvsNum 离散次数
   mkDvsData 离散数据
   proFactor 过程数据
   lenOfData 数据长度
   data 数据
   输出参数
   criperDataLen 数据长度  
   criperData 数据   
 */
int UnionHsmCmdWC(char *mode, char *id, char *mkType, char *mk, char *mkCheckValue, int mkIndex, int mkDvsNum, char *mkDvsData, char *proFactor, int lenOfData, char *data, int *criperDataLen, char *criperData);

/*
   函数功能：
   WD:: 计算及校验MAC/TAC（原UB）
   输入参数
   mode 模式标识 1=计算MAC 2=校验MAC
   id 方案ID
   4=SM4MAC（使用2倍长过程密钥进行SM4）
   mkType 根密钥类型 
   109= MK-AC
   209= MK-SMI
   309= MK-SMC
   509= MK-DN
   mk 根密钥
   mkCheckValue 根密钥校验值 
   mkIndex 根密钥索引
   mkDvsNum 离散次数
   mkDvsData 离散数据
   proFactor 过程数据
   macFillFlagMAC 数据填充标识
   1=强制填充0X80
   0=不强制填充0X80
   ivMac IV-MAC 
   lenOfData MAC计算数据长度
   data MAC计算数据
   macFlagMAC 长度标识   
   1=4 BYTE
   2=8 BYTE
   输出参数
   checkMac
   mac MAC值
   返回值：
   成功：>=0

 */

int UnionHsmCmdWD(char *mode, char *id, char *mkType,char *mk, char* mkCheckValue, int mkIndex,int mkDvsNum,
		char *mkDvsData, char *proFactor, char *macFillFlag, char *ivMac,int lenOfData,
		char *data, char *macFlag, char *checkMac, char *mac);

// 生成密钥SM1密钥
/* 
   输入参数
   keyType         密钥类型
   输出参数
   keyByLMK        LMK保护的密钥密文值
   checkValue      密钥校验值
 */
int UnionHsmCmdM0(TUnionDesKeyType keyType, char *keyByLMK, char *checkValue);

/*
   函数功能：
   SM1算法离散子密钥或产生过程密钥，用过程密钥或者子密钥计算并验证MAC。(M9)
   输入参数：
mode: 模式标志
0-计算过程密钥或离散子密钥 
1-计算MAC
2-校验MAC
mkType: 根据密钥类型选择指定的LMK加密输出子密钥和过程密钥 
001 – ZPK （LMK0607加密）
008 – ZAK (LMK2627加密)
00A – ZEK(LMK3031加密)
mk: 密钥密文
mkIndex: 密钥索引, K+3位索引方式读取加密机内保存密钥
mkDvsNum: 指定对密钥离散的次数（0-3次）
mkDvsData: n*8B 离散卡片密钥的数据，其中n代表离散次数. 用指定的分散因子拼接分散因子求反值作为输入数据，做加密计算，产生的16 字节的结果作为子密钥
proKeyFlag: 过程密钥标识,是否对保护密钥进行过程计算,当需要计算过程密钥时,使用过程密钥保护数据.
Y:计算过程密钥
N:不计算过程密钥
可选项:当没有该域时缺省为N
proFactor: 8B 过程因子,在计算过程密钥时，输入数据是8 字节随机数,计算时加密机内拼接8 字节全 “00”。
ivMac: 32H MAC计算的IV
checkMac: 8H 待校验的MAC值 当模式标志为2时，有该域
macDataLen: 4N MAC数据长度
macData: 8H MAC数据
macPrefixDataLen: 4N MAC前缀填充数据长度
macPrefixData: nB MAC前缀填充数据
输出参数：
criperData: 32H 子密钥密文,指定密钥类型下加密，模式标志为0有该域
checkValue: 16H	子密钥校验值,离散子密钥校验值，模式标志为0有该域
proCriperData: 32H 过程密钥密文,过程密钥标识为‘Y’时，且模式标志为0有该域  指定密钥类型下加密
proCheckValue: 16H 过程密钥校验值,过程密钥标识为‘Y’时，且模式标志为0有该域
SM1，LMK0405下加密
3DES，指定密钥类型下加密
mac:MAC值 8H	当模式标志为1，有该域
 */

int UnionHsmCmdM9(char *mode, char *mkType, char *mk, int mkIndex, int mkDvsNum, char *mkDvsData,
		char *proKeyFlag, char *proFactor,char *ivMac, char *checkMac, int macDataLen, char *macData,
		int macPrefixDataLen, char *macPrefixData, char *criperData, char *checkValue, char *proCriperData,
		char *proCheckValue, char *mac);

//导出SM2私钥（TK保护）
/*输入参数
  SM2Index      密钥索引
  lenOfSM2      外带密钥密文长度
  SM2           外带密钥密文
  keyType               密钥类型
  Key           主密钥加密下的传输(SM4)密钥
  checkValue    传输密钥校验值
  输出参数
  lenOfVK               密钥密文字节数
  VK            TK加密下的SM2私钥密文

 */     
int UnionHsmCmdK9(int SM2Index,int lenOfSM2,char *SM2,TUnionDesKeyType keyType,char *Key,char *checkValue,int *lenOfVK,unsigned char *VK);
int UnionHsmCmdK9ForInput(char mode,int SM2Index,int lenOfSM2,char *SM2,int lenOfPK,char *pk,TUnionDesKeyType keyType,char *Key,char *checkValue,int *lenOfVK,unsigned char *VK);

/*
   功能:产生一个PIN校验值PVV
   指令:PV
   输入参数：
pvk:	1A+32H   用LMK1415 加密，‘S’+3
pvkIndex: 1N	 取值为1-F
tpk:	1A+32H	 用LMK1415 加密，‘S’+32
pinBlock:  32H   用TPK加密的ANSI9.8PIN块
PAN:	   16H   16位完整卡号
输出参数:
pvv:	4N	生成的PVV
 */
int UnionHsmCmdPV(char *pvk,char pvkIndex,char *tpk,char *pinBlock,char *pan,char *pvv);

/*
   功能: 计算/验证mac
   指令:W9
   输入参数:
mode:	模式，1:产生MAC，2:校验MAC
zakIndex: zak索引号
zakValue: zak密钥
macCalcFlag: MAC计算方式,1:联机报文的MAC计算方法	
2:顺序文件MAC的计算方法
msgType: 消息类型,0:消息数据为二进制
1:消息数据为扩展十六进制
lenOfMacData: macData数据长度，不大于8192
macData: 用来计算MAC的数据
oriMac:	待校验的MAC值,仅当模式为2时，此域存在
sizeofDesMac:desMac的最大长度
输出参数:
desMac: MAC值

 */
int UnionHsmCmdW9(int mode,char *zakIndex,char *zakValue,int macCalcFlag,int msgType,int lenOfMacData,char *macData,char *oriMac,char *desMac,int sizeofDesMac);

/*
   功能: PIN块的转加密
   指令: W8
   输入参数:
oriAlgorith:	1N	 源PIN块算法，1-DES/3DES, 2-SM4
desAlgorith:	1N	 目的PIN块算法，1-DES/3DES, 2-SM4
oriKeyLenFlag:	1N	 源密钥长度标识,1-单倍，2-双倍，3-三倍
oriKeyIndex/oriZpkValue: 1A+3H/16/32/48H	源zpk索引号或密文
desKeyLenFlag: 	1N	源密钥长度标识,1-单倍，2-双倍，3-三倍
desKeyIndex/desZpkValue: 1A+3H/16/32/48H	目的zpk索引号或密文
oriFormat:	2N	源PIN块格式,‘01’-‘06’输出的PIN块格式
当源PIN块算法为SM4，该域指示的格式只能为“01” 按照9.8格式组成pin块后补F至16字节
desFormat:	2N	目的PIN块格式,‘01’-‘06’输出的PIN块
当源PIN块算法为SM4，该域指示的格式只能为“01” 按照9.8格式组成pin块后补F至16字?		pinBlock:	16H/32H		密钥加密的PIN
oriAccNo:	12/18N		12：‘01’（客户原始账号右12位除去校验位）
18：格式04,其他格式无此域。
desAccNo:	12/18N		12：‘01’（客户原始账号右12位除去校验位）
18：格式04,其他格式无此域。

sizeofPinByZpk:	pinByZpk的最大长度.
输出参数:
pinByZpk:	16H/32H		转加密后的PIN密文
 */
int UnionHsmCmdW8(int oriAlgorith,int desAlgorith,int oriKeyLenFlag,char *oriKeyIndex,char *oriZpkValue,int desKeyLenFlag,char *desKeyIndex,char *desZpkValue,char *oriFormat,char *desFormat,char *pinBlock,char *oriAccNo,char *desAccNo,char *pinByZpk,int sizeofPinByZpk);


//桂林银行专用W8,add by huangh 2016.04.15
/*
   功能: PIN块的转加密
   指令: W8(桂林银行专用W8)
   输入参数:
oriAlgorith:	1N	 源PIN块算法，1-DES/3DES, 2-SM4
desAlgorith:	1N	 目的PIN块算法，1-DES/3DES, 2-SM4
oriKeyLenFlag:	1N	 源密钥长度标识,1-单倍，2-双倍，3-三倍
oriKeyIndex/oriZpkValue: 1A+3H/16/32/48H	源zpk索引号或密文
desKeyLenFlag: 	1N	源密钥长度标识,1-单倍，2-双倍，3-三倍
deskeytype	3N	目的密钥类型:目的密钥类型标识为Y时该域存在；支持ZPK(001) / ZEK(00A)

desKeyIndex/desZpkValue: 1A+3H/16/32/48H	目的zpk索引号或密文
oriFormat:	2N	源PIN块格式,‘01’-‘06’输出的PIN块格式
当源PIN块算法为SM4，该域指示的格式只能为“01” 按照9.8格式组成pin块后补F至16字节
desFormat:	2N	目的PIN块格式,‘01’-‘06’输出的PIN块
当源PIN块算法为SM4，该域指示的格式只能为“01” 按照9.8格式组成pin块后补F至16字�
pinBlock:	16H/32H		密钥加密的PIN
oriAccNo:	12/18N		12：‘01’（客户原始账号右12位除去校验位）
18：格式04,其他格式无此域。
desAccNo:	12/18N		12：‘01’（客户原始账号右12位除去校验位）
18：格式04,其他格式无此域。

sizeofPinByZpk:	pinByZpk的最大长度.
输出参数:
pinByZpk:	16H/32H		转加密后的PIN密文
 */

int UnionHsmCmdW8_Base(int oriAlgorith,int desAlgorith,int oriKeyLenFlag,char *oriKeyIndex,char *oriZpkValue,int desKeyLenFlag,char *deskeytype,char *desKeyIndex,char *desZpkValue,char *oriFormat,char *desFormat,char *pinBlock,char *oriAccNo,char *desAccNo,char *pinByZpk,int sizeofPinByZpk);

/*
   功能：将PIN块从ZPK下SM4加密翻译到LMK下加密。
   指令：	WX
   输入参数：
mode: 模式?:PIN从LMK转到ZPK
1:PIN从ZPK转到LMK
zpkValue: zpk密钥值
oriPinBlock: PIN块	
format:   pin块格式
accNo:    账号,去除校验位的最右12位
sizeofPinDesPinBlock: 目的pin块的最大长度
输出参数:
desPinBlock: LMK对（02-03）下加密的PIN/ZPK加密的PIN
 */
int UnionHsmCmdWX(int mode,char *zpkValue,char *pinBlock,char *format,char *accNo,int lenOfAccNo,char *desPinBlock,int sizeofPinDesPinBlock);

/*
   函数功能：(国密)
   WW指令，加密一个明文PIN/解密一个已加密的PIN
   输入参数：
   mode：0:加密    1:解密
   oriPIN：源pin值
accNo: 账号
lenOfAccNo：客户帐号长度
sizeofDesPIN:desPIN的最大长度
sizeofReferenceNum:referenceNum的最大长度,当加密时才有效
输出参数：
desPIN：目的pin值
referenceNum: 当解密时，才有效 
 */
int UnionHsmCmdWW(int mode,char *oriPIN,char *accNo,int lenOfAccNo,char *desPIN,int sizeofDesPIN,char *referenceNum,int sizeofReferenceNum);

/*
   函数功能：(国密)
   CL指令，产生VISA卡校验值CVV
   输入参数：
   cardValidDate：VISA卡的有效期
   cvk：CVK
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   cvv：生成的VISA卡的CVV
 */
int UnionHsmCmdCL(char *cardValidDate,char *cvk,char *accNo,int lenOfAccNo,char *serviceCode,char *cvv,int sizeofCVV);

/*
   函数功能：(国密)
   WV指令，随机产生一个PIN
   输入参数：
   pinLength：要求随机生成的PIN明文的长度
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   pinBlockByLMK：随机产生的PIN的密文，由LMK加密
sizeofPinBlock: pinBlockByLMK的长度
 */
int UnionHsmCmdWV(int pinLength,char *accNo,int lenOfAccNo,char *pinBlockByLMK,int sizeofPinBlock);

//导出SM2私钥
/*输入参数
  index         密钥索引
  isOutPutPK    是否需要导出公钥, 0或者无该域：表示不需要导出公钥, 1：表示需要导出公钥
  输出参数
  lenOfVK               密钥密文字节数
  VK            密钥密文
  PK            公钥值
 */
int UnionHsmCmdK8(int index,char *isOutPutPK,int *lenOfVK,unsigned char *VK,unsigned char *PK);

// 取出RSA公私密钥<TW>
int UnionHsmCmdTW(char *mode,char *vkIndex,char *pkEncoding,char *vkByMK,int *vkLen,char *pk,int *pkLen);

// 脱机PIN加密
int UnionHsmCmdVA(char *mode, char *id, int mkIndex, char *mk, char *iv, char *pan, char *bh, char *atc, char *plainData, char *P2, int *lenOfCiperData, char *ciperData);

/*
   功能：使用二次生成的密钥生成AnsiX9.9MAC指令
   输入参数：
   keyValue	32H	基础密钥
   random1		16H	随机数A
   random2		16H	随机数B
   lenOfData	3N	MAC数据长度
   data		 	扩展的MAC数据
   输出参数：
   mac		16H	MAC
   返回值：
   <0：函数执行失败，值为失败的错误码
   >=0：函数执行成功
 */
int UnionHsmCmdG3(char *keyValue,char *random1,char *random2,int lenOfData,char *data,char *mac);

/*
   功能:将PIN密文转换为MD5加密
   输入参数：
zpk:              ZPK
srcPinBlock:      PIN密文
accNo:            针对AnxiX9.8格式的帐号
lenOfAccNo:       账号长度
fillMode	  填充模式，0：不填充，1：PIN前填充，2：PIN后填充
fillLen		  填充长度
fillData	  填充数据
输出参数：
dstPinBlock: 	  PIN密文
返回值:
<0：函数执行失败，值为失败的错误码
>=0：函数执行成功
计算方法
1.      将源ZPK解密为明文；
2.      根据ZPK明文、pinblock、账号解密得到PIN明文；
3.      对PIN明文进行MD5；
4.      输出PIN的MD5结果；
 */
int UnionHsmCmdS4(char *zpk,char *srcPinBlock,char *accNo,int lenOfAccNo,int fillMode,int fillLen,char *fillData,int tailFillLen, char *tailFill, char *dstPinBlock);

/*
   功能:将PIN密文在特殊算法和Anxi X9.8算法之间进行转换
   输入参数：
zpk1:              ZPK1
zpk2:              ZPk2
changeFlag:        转换标志(00：特殊算法转AnsiX9.8　01：AnsiX9.8转特殊算法)
srcPinBlock:           16位被密钥1加密的PIN密文
accNo:                 针对AnxiX9.8格式的帐号
lenOfAccNo:            账号长度
输出参数：
dstPinBlock: 16位被密钥2加密的PIN密文
返回值:
<0：函数执行失败，值为失败的错误码
>=0：函数执行成功
 */
int UnionHsmCmdG2(char *zpk1,char *zpk2,int changeFlag,char *srcPinBlock,char *accNo,int lenOfAccNo,char *dstPinBlock);

/*
   函数功能:
   PINBLOCK与ANSI9.8格式相互转换
   输入参数:
zpk1: 源zpk
zpk2: 目的zpk
srcPinBlock: 源pinBlock
srcFormat: 源pinBlock格式    1: ANSI9.8格式; 2: 江西农信PIN专用格式
dstFormat: 目的pinBlock格式  1: ANSI9.8格式; 2: 江西农信PIN专用格式
accNo: 账号
输出参数:
dstPinBlock: 目的pinBlock
返回值:
成功>=0, 失败<0
 */
int UnionHsmCmdCD(char *zpk1,char *zpk2,char *srcPinBlock,int srcFormat,int dstFormat,char *accNo,char *dstPinBlock);

/*
   功能：
   将PIN由X9.8转为特殊算法加密
   参数:
   srcZPK 当前加密PIN块的ZPK；LMK对（06-07）下加密
   dstZPK 将要加密PIN块的ZPK；LMK对（06-07）下加密。
   srcPin 源ZPK加密的字符密码密文
   srcPan 用户有效主帐号
   dstPan 用户有效主帐号
   输出:
   lenOfPin pin明文长度
   dstPinBlock 返回PIN密文 
 */
int UnionHsmCmdN7(char* srcZPK, char* dstZPK, char *srcPinBlock, char* srcPan, char* dstPan, int* lenOfPin, char* dstPinBlock);

/*
   功能
   生成一个密钥，以LMK加密输出，同时可选使用ZMK加密输出或保存在指定索引下
   输入参数
   mode    模式，0：产生密钥；1：产生密钥并在ZMK下加密；2：产生密钥并保存在指定索引下
   keyType 密钥类型
   keyModel        密钥方案表。0：单倍长DES密钥；1：双倍长DES密钥；2：三倍长DES密钥；3：SM4密钥；4：SM1密钥；5：AES密钥
   zmk     LMK下加密的ZMK或密钥索引。仅当模式为1时有此域
   keyIndex        密钥索引。产生密钥存放位置，仅当模式为2时有此域
   输出参数
   keyByLMK        LMK加密下的密钥
   keyByZMK        ZMK加密下的密钥。仅当模式为1时有此域
   checkValue      密钥校验值
   返回值
   >0：成功
   <0：失败
 */
int UnionJTBHsmCmdA0(int mode,TUnionDesKeyType keyType,int keyModel,char *zmk,int keyIndex,char *keyByLMK,char *keyByZMK,char *checkValue);

/*
   功能: 将私钥解密后的登录口令转换为网银的EDK加密
   输入参数：
   fillMode	1N	填充方式
   vkIndex		2N	私钥索引,“00”－“20” ， “99”为从外部输入的私钥
   lenOfPinByPK	4N	公钥加密的登录口令密文数据长度
   pinByPK		nB	公钥加密的登录口令密文数据
   keyLen		4N	edk密钥长度
   vk		nB	用主密钥加密的私钥	
   edk		16H/32H/48H	edk密钥
   输出参数：
   pinByEDK	16H	edk加密的登录口令数据
   返回值：
   <0：函数执行失败，值为失败的错误码
   >=0：函数执行成功
 */
int UnionHsmCmd4A(int fillMode,int vkIndex,int lenOfPinByPK,char *pinByPK,int keyLen,char *keyValue,char *edk,char *pinByEDK);


/*
   功能:将解密后的登录口令(AN9.8)转换为网银的pinoffset
   输入参数：
   zpk		16H/32H/48H	zpk密钥值
   accNo		12N		账号
   pinBlock	16H		zpk加密的pin密文数据
   edk		16H/32H/48H	edk密钥
   输出参数：
   pinOffset	16H	edk加密的登录口令数据
   返回值：
   <0：函数执行失败，值为失败的错误码
   >=0：函数执行成功
 */

int UnionHsmCmd4B(char *zpk,char *accNo,char *pinBlock,char *edk,char *pinOffset);

/*
   功能: 将由公钥加密的PIN转换成ANSI9.8标准
   输入参数：
   vkIndex		2N		索引号
   keyType		1N		密钥类型, 1：TPK  2：ZPK
   keyValue	16H/32H/48H	密钥值
   pinType		1N		pin类型,0：原始PIN ,1：ANSI9.8
   fillMode	1N		数据填充方式,"0"：如果数据长度小于密钥长度，加密时先在数据前面补0x00，以使数据长度等于密钥的长度，然后再进行加密, "1"：PKCS填充方式
   accNo		16N		账号
   pinLen		4N		公钥加密的pin长度	
   pinByPK		B		公钥加密的pin数据
   输出参数：
   pinBlock	16H		keyValue加密的pin
   返回值：
   <0：函数执行失败，值为失败的错误码
   >=0：函数执行成功
 */

int UnionHsmCmd4C(int vkIndex,int keyType,char *keyValue,int pinType,int fillMode,char *accNo,int pinLen,char *pinByPK,char *pinBlock);

int UnionHsmCmdE0_GDSNX(int dataBlock, int CrypToFlag,int modeOfAlgorithm,
		char *ZEK, int inDataType,int outDataType,int padMode,char *padChar,int padCountFlag,char *IV,
		char *lenOfMSG,char *MSGBlock,char *datafmt,char *lenOfMSG1,char *MSGBlock1,char *IV1);

// add by lisq 20141021 交通部专用指令集
/*
   功能
   生成一个密钥，以LMK加密输出，同时可选使用ZMK加密输出或保存在指定索引下
   输入参数
   mode    模式，0：产生密钥；1：产生密钥并在ZMK下加密；2：产生密钥并保存在指定索引下
   keyType 密钥类型
   keyModel        密钥方案表。0：单倍长DES密钥；1：双倍长DES密钥；2：三倍长DES密钥；3：SM4密钥；4：SM1密钥；5：AES密钥
   zmk     LMK下加密的ZMK或密钥索引。仅当模式为1时有此域
   keyIndex        密钥索引。产生密钥存放位置，仅当模式为2时有此域
   输出参数
   keyByLMK        LMK加密下的密钥
   keyByZMK        ZMK加密下的密钥。仅当模式为1时有此域
   checkValue      密钥校验值
   返回值
   >0：成功
   <0：失败
 */
int UnionJTBHsmCmdA0(int mode,TUnionDesKeyType keyType,int keyModel,char *zmk,int keyIndex,char *keyByLMK,char *keyByZMK,char *checkValue);

// 输入一个DES/SM4密钥 A6
/*
   功能
   将ZMK下加密的密钥转换为LMK下加密，可选保存到指定索引下
   输入参数
   algFlag 模式标识。默认为T。T：ZMK加密下密钥导入；R：保存LMK下加密密钥到指定索引。此域为可选项
   keyType 密钥类型
   zmk     LMK下加密的ZMK。仅当模式为T时有此域
   keyByZmk        密钥。当模式为T时，为ZMK加密的密钥；当模式为R时，为LMK加密的密钥。
   keyModel        密钥类型。0：DES密钥；非0：SM4密钥。
   keyIndex        密钥索引。当模式为R时有此域，当模式为T时此域可选。
   输出参数
   keyByLmk        密钥值。LMK加密的密钥，仅当模式为T时有。
   checkValue      密钥校验值
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdA6(char *algFlag, TUnionDesKeyType keyType,char *zmk,char *keyByZmk,int keyModel,int keyIndex,char *keyByLmk,char *checkValue);

// 输出一个密钥DES/SM4 A8
/*
   功能
   LMK加密或索引下密钥使用ZMK加密输出
   输入参数
   mode    模式。0：备份索引下密钥到LMK加密；1：LMK加密或索引密钥导出为ZMK加密。
   keyType 密钥类型
   zmk     LMK下加密的ZMK或密钥索引。仅当模式为1时有。
   key     需要输出的密钥。仅当模式为1时，为ZMK加密的密钥；当模式为0时，为密钥索引，K+3H。
   keyMode 密钥模式。0：DES密钥；1：SM4密钥。
   输出参数
   keyByZMK        密钥值。当模式为0时，为LMK加密；当模式为1时，为ZMK加密。
   checkValue      密钥校验值
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdA8(int mode,TUnionDesKeyType keyType,char *key,int keyMode,char *zmk,char *keyByZMK,char *checkValue);

// 装载一个私钥 EK
/*
   功能
   将LMK加密的RSA/SM2私钥保存到指定索引下
   输入参数
   algFlag 算法标识。1：SM2；其它:RSA。
   vkIndex 私钥索引。范围01到20。
   lenOfVK 私钥长度
   vk      私钥
   输出参数
   无
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdEK(int algFlag,int vkIndex,int lenOfVK,unsigned char *vk);


// 产生非对称密钥对 EI
/*
   功能
   产生非对称密钥
   输入参数
   algFlag 算法标识。0：RSA；1：SM2。
   type    密钥类型。0：签名和加密；1：密钥管理；2：签名和加密，密钥管理；3：密钥协商，当且仅当算法标识为1时有。
   length  密钥长度
   pkEncoding      公钥编码。当算法标识为0时有。01：DER。
   lenOfPKExponent 公钥指数的长度。当算法标识为0时有。
   pkExponent      公钥指数。当算法标识为0时有。
   exportNullPK    裸公钥标识
   index   索引。01-20：密码机内保存新生成的密钥；99：不保存新生成的密钥。
   输出参数
   pk      公钥
   lenOfVK 私钥长度
   vk      私钥
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdEI(int algFlag,char type,int length,char *pkEncoding,
		int lenOfPKExponent,unsigned char *pkExponent,int exportNullPK,int index,
		char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK);

// 从私钥获取公钥 EJ
/*
   功能
   从私钥获取公钥
   输入参数
   algFlag 算法标识。0：RSA；1：SM2。
   vkIndex 私钥索引。01-20。99：外带私钥。
   vkDataLen       私钥数据块长度。仅当私钥索引为99时有。
   vkData  私钥数据块。仅当私钥索引为99时有。
   pkEncode        公钥编码。当算法标识为0时有。01：DER。
   输出参数
   pk      公钥。
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdEJ(int algFlag,int vkIndex,int vkDataLen,char *vkData,char *pkEncode,char *pk,int sizeOfPK);

// 导出非对称私钥 EH
/*
   功能
   导出非对称私钥
   输入参数
   algFlag 算法标识。0：ECB；1：CBC。
   proKeyType      保护密钥类型。0：KEK；1：MDK。
   proKey  保护密钥。用于保护RSA/SM2的密钥或密钥索引。
   proKeyModel     保护密钥方案。0：DES双倍长加密；1：SM4加密。
   speNum  分散次数。范围0-5.
   speData 分散数据
   expAlgFlag      导出算法标识。0：RSA；1：SM2。
   vkIndex 私钥索引。01-20：加密机内私钥；99：外带私钥。
   vkLen   私钥长度
   vkData  私钥
   padFlag 私钥分量填充标识。0：不强制填充；1：强制填充80。
   iv      初始向量。仅算法标识为1时有。
   输出参数
   module  模长
   pkM     公钥模M
   pkE     公钥指数E
   vkD     私钥指数D
   vkP     私钥分量P
   vkQ     私钥分量Q
   vkDP    私钥分量DP
   vkDQ    私钥分量DQ
   vkQINV  私钥分量QINV
   sm2X    SM2公钥X
   sm2Y    SM2公钥Y
   sm2D    SM2私钥分量D
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdEH(int algFlag,TUnionDesKeyType proKeyType,char *proKey,int proKeyModel,int speNum,char *speData,int expAlgFlag,int vkIndex,int vkLen,char *vkData,int padFlag,char *iv,int *module,char *pkM,char *pkE,char *vkE,char *vkD,char *vkP,char *vkQ,char *vkDP,char *vkDQ,char *vkQINV,char *sm2X,char *sm2Y,char *sm2D);

// 私钥解密运算 EP
/*
   功能
   使用RSA/SM2私钥解密数据
   输入参数
   algFlag 算法标识。0：RSA；1：SM2。
   padFlag 填充标识。0：不填充；1：填充，PKCS#1 1.5。当算法标识为0时有。
   dataLen 数据块密文长度。
   cipherData      数据块密文。
   vkIndex 私钥索引。01-20：加密机内私钥；99：外带私钥。
   vkDataLen       私钥长度。
   vkData  私钥。
   输出参数
   plainData       明文数据。
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdEP(int algFlag,int padFlag,int dataLen,char *cipherData,int vkIndex,int vkDataLen,char *vkData,char *plainData,int sizeOfPlainData);

// 公钥加密运算 ER
/*
   功能
   使用RSA/SM2公钥加密数据
   输入参数
   algFlag 算法标识。0：RSA；1：SM2。
   padFlag 填充标识。0：不填充；1：填充，PKCS#1 1.5。当算法标识为0时有。
   dataLen 数据长度。
   plainData       数据。
   pkIndex 公钥索引。01-20：加密机内公钥；99：外带公钥。
   pkDataLen       公钥长度。
   pkData  公钥。
   输出参数
   cipherData      密文数据。
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdER(int algFlag,int padFlag,int dataLen,char *plainData,int pkIndex,int pkDataLen,char *pkData,char *cipherData,int sizeOfcipherData);

// 私钥签名运算 EW
/*
   功能
   使用RSA/SM2私钥计算数据的签名
   输入参数
   hashFlag        HASH算法标识。0：SHA-1；1：MD5；2：ISO 10118-2；3：NO HASH；4：SM3。
   signFlag        签名算法标识。0：RSA；1：SM2。
   padFlag         填充标识。0：不填充；1：填充，PKCS#1 1.5。当签名算法标识为0时有。
   userIDLen       用户标识长度。范围0000-0032。当签名算法标识为1时有。
   userID          用户标识。当签名算法标识为1时有。
   dataLen         签名数据长度。
   data            签名数据。
   vkIndex         私钥索引。01-20：加密机内私钥；99：外带私钥。
   vkDataLen       私钥长度。
   vkData          私钥。
   输出参数
   sign            签名。
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdEW(int hashFlag,int signFlag,int padFlag,int userIDLen,char *userID,int dataLen,char *data,int vkIndex,int vkDataLen,char *vkData,char *sign,int sizeOfSing);


// 公钥验签运算 EY
/* 
   功能
   使用RSA/SM2公钥验证签名
   输入参数
   hashFlag                HASH算法标识。0：SHA-1；1：MD5；2：ISO 10118-2；3：NO HASH；4：SM3。
   signFlag                签名算法标识。0：RSA；1：SM2。
   padFlag                 填充标识。0：不填充；1：填充，PKCS#1 1.5。当签名算法标识为0时有。
   userIDLen               用户标识长度。范围0000-0032。当签名算法标识为1时有。
   userID                  用户标识。当签名算法标识为1时有。
   signLen                 签名长度。
   sign                    签名。
   dataLen                 签名数据长度。
   data                    签名数据。
   macLen                  MAC长度。
   mac                     MAC。
   pkLen                   公钥长度。
   pk                      公钥。
   macDataLen              签别数据长度。
   macData                 签别数据。
   输出参数
   无
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdEY(int hashFlag,int signFlag,int padFlag,int userIDLen,char *userID,int signLen, char *sign,int dataLen,char *data,int macLen, char *mac,int pkLen,char *pk,int macDataLen,char *macData);

// 计算数据摘要 GM
/*
   功能
   计算数据摘要
   输入参数
   hashFlag                HASH算法标识。1：SHA-1；2：MD5；3：ISO 10118-2；4：SM3。
   dataLen                 数据长度。
   data                    数据。
   userIDLen               用户标识长度。范围0000-0032。当签名HSAH算法标识为3时有。
   userID                  用户标识。当签名HSAH算法标识为3时有。
   pkLen                   公钥长度。
   pk                      公钥。
   输出参数
   hash                    HASH值。
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdGM(int hashFlag,int dataLen,char *data,int userIDLen,char *userID,int pkLen,char *pk,int sizeofHash,char *hash);

// 计算及校验MAC/TAC UB
/*
   功能
   对指定密钥进行指定次数的离散得到子密钥或过程密钥作为MAC/TC计算密钥对输入数据计算MAC/TC。
   输入参数
   algFlag         算法标识。0：DES；1：SM4。
   mode            模式标识。0：计算MAC；1：校验MAC。
   id              方案ID。0：使用子密钥进行3DES/SM4 计算MAC；1：使用过程密钥进行DES MAC；2：TAC；3：使用过程密钥进行3DES/SM4 计算MAC。
   mkType          主密钥类型。0：MK-AC；1：MK-SMI；2：MK-SMC；3：MK-DN；4：TAK；5：ZAK。
   mk              主密钥。
   mkIndex         主密钥索引。
   mkDvsNum        主密钥离散次数。范围0-5.
   mkDvsData       主密钥离散数据。
   proFactor       过程数据。仅当方案ID为1或3时有。
   macFillFlag     MAC数据填充标识。0：强制填充0X80；1：不强制填充0X80。
   ivMac           IV-MAC。
   lenOfData       MAC计算数据长度。
   data            MAC计算数据。
   macFlag         MAC长度。
   checkMac        待校验的MAC。仅当模式标识为1时有
   输出参数
   mac             MAC。仅当模式标识为0时有。
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdUB(int algFlag, char *mode, char *id, char *mkType,char *mk, int mkIndex,int mkDvsNum,
		char *mkDvsData, char *proFactor, char *macFillFlag, char *ivMac,int lenOfData,
		char *data, char *macFlag, int macLen, char *checkMac, char *mac);

// 计算及校验MAC/TAC UB
/*
   功能
   对指定密钥进行指定次数的离散得到子密钥或过程密钥作为MAC/TC计算密钥对输入数据计算MAC/TC。
   输入参数
   algFlag         算法标识。0：DES；1：SM4。
   mode            模式标识。0：计算MAC；1：校验MAC。
   id              方案ID。0：使用子密钥进行3DES/SM4 计算MAC；1：使用过程密钥进行DES MAC；2：TAC；3：使用过程密钥进行3DES/SM4 计算MAC。
   mkType          主密钥类型。0：MK-AC；1：MK-SMI；2：MK-SMC；3：MK-DN；4：TAK；5：ZAK。
   mk              主密钥。
   mkIndex         主密钥索引。
   mkDvsNum        主密钥离散次数。范围0-5.
   mkDvsData       主密钥离散数据。
   proFactor       过程数据。仅当方案ID为1或3时有。
   macFillFlag     MAC数据填充标识。0：强制填充0X80；1：不强制填充0X80。
   ivMac           IV-MAC。
   lenOfData       MAC计算数据长度。
   data            MAC计算数据。
   macFlag         MAC长度。
   checkMac        待校验的MAC。仅当模式标识为1时有
   输出参数
   mac             MAC。仅当模式标识为0时有。
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdUB(int algFlag, char *mode, char *id, char *mkType,char *mk, int mkIndex,int mkDvsNum,
		char *mkDvsData, char *proFactor, char *macFillFlag, char *ivMac,int lenOfData,
		char *data, char *macFlag, int macLen, char *checkMac, char *mac);


// ARQC/TC/ACC校验，ARPC产生 KW
/*
   功能
   ARQC/TC/ACC的校验，ARPC的产生或同时验证ARQC并产生ARPC
   输入参数
   mode    模式标识。0：仅验证ARQC；1：验证ARQC并EMV4.1方式一产生ARPC；2：仅EMV4.1方式一产生ARPC；3：验证ARQC并EMV4.1方式二产生ARPC；4：仅EMV4.1方式二产生ARPC；5：仅验证ARQC（VISA）；6：验证ARQC并EMV4.1方式一产生ARPC（VISA）。
   id      密钥离散方案。
   mkIndex 主密钥索引。
   mk      主密钥。
   iv      初始向量。
   dvsNum  离散次数。
   dvsData 离散数据。
   lenOfPan        账号长度。
   pan     账号
   bh      B/H参数
   atc     ATC
   lenOfData       交易数据长度
   data            交易数据
   ARQC            待验证的ARQC
   ARC     ARC
   csu     CSU
   lenOfData1      鉴定数据长度
   data1   鉴定数据
   输出数据
   ARPC    ARPC
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdKW(char *mode, char *id, int mkIndex, char *mk, char *iv,int dvsNum,char *dvsData,
		int lenOfPan, char *pan, char *bh, char *atc, int lenOfData,
		char *data, char *ARQC, char *ARC, char *csu, int lenOfData1,
		char *data1,char *ARPC);

// 产生安全通道会话密钥 G1
/*
   功能
   产生安全通道会话密钥
   输入参数
   algFlag         分散标识。0：先分散子密钥后分散过程密钥；1：只分散过程密钥。
   keyType         主密钥类型。
   keyIndex        主密钥索引。
   key             主密钥值。
   keyData         分散密钥数据。仅当分散标识为0时有。
   SN              SN。
   输出参数
   sencKey         安全通道认证密钥
   cmacKey         CMAC计算密钥
   rmacKey         RMAC计算密钥
   sdekKey         数据加密密钥
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdG1(int algFlag,TUnionDesKeyType keyType,int keyIndex,char *key,char *keyData,char *SN,char *sencKey,char *cmacKey,char *rmacKey,char *sdekKey);

// 对称转加密数据 UY
/*
   功能
   对称转加密数据，钱包/联机业务。
   输入参数
   tranFlag        算法转换标识。0：DES转SM4；1：SM4转DES；2：SM4转SM4；3：DES转DES。
   srcKeyType      源密钥类型
   srcKeyIndex     源密钥索引
   srcKey          源密钥
   srcAlgFlag      源算法标识。0：ECB；1：ECB_LP；2：ECP_P；3：CBC；4：CBC_LP：5：CBC_P。
   destKeyType     目的密钥类型
   destKeyIndex    目的密钥索引
   destKey         目的密钥
   destAlgFlag     目的算法标识。0：ECB；1：ECB_LP；2：ECP_P；3：CBC；4：CBC_LP：5：CBC_P。
   srcIv           源初始向量。当源算法标识为CBC时有。
   destIv          目的初始向量。当目的算法标识为CBC时有。
   dataLen         数据长度
   data            数据
   输出参数
   destData        目的数据
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdUY(int tranFlag,TUnionDesKeyType srcKeyType,int srcKeyIndex,char *srcKey,int srcAlgFlag,TUnionDesKeyType destKeyType,int destKeyIndex,char *destKey,int destAlgFlag,char *srcIv,char *destIv,int dataLen,char *data,int sizeofDestData,char *destData);


// 分散密钥数据加解密计算 U1
/*
   功能
   对指定密钥进行指定次数的离散得到子密钥或过程密钥，使用该密钥对输入数据进行加密或解密。
   输入参数
   algFlag         算法标识。0：DES；1：SM4。
   mode            加密模式标识
   id              方案ID
   mkType          主密钥类型
   mk              主密钥
   mkIndex         主密钥索引
   mkDvsNum        离散次数
   mkDvsData       离散数据
   proFactor       过程数据
   iv              初始向量
   lenOfData       数据长度
   data            数据
   输出参数
   criperData      密文或明文
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdU1(int algFlag, int mode, char *id, char *mkType, char *mk, int mkIndex,
		int mkDvsNum, char *mkDvsData, char *proFactor, char *iv, int lenOfData,
		char *data, int *criperDataLen, char *criperData);

// 安全报文方式导出密钥 U3
/*
   功能
   离散卡片密钥并以安全报文方式导出
   输入参数
   algFlag         算法标识。0：DES；1：SM4。
   mode            模式标识。0：仅加密；1：加密并计算MAC。
   id              方案ID
   mkType          主密钥类型
   mk              主密钥
   mkIndex         主密钥索引
   mkDvsNum        离散次数
   mkDvsData       离散数据
   proKeyType      保护密钥类型
   proKey          保护密钥
   proKeyIndex     保护密钥索引
   proDvsNum       保护密钥离散次数
   proDvsData      过程密钥离散数据
   proFlag         过程密钥标识
   proFactor       过程数据
   iv              初始向量
   encPadDataLen   加密数据长度
   encPadData      加密数据
   ivMac           IV-MAC
   macPadDataLen   MAC填充数据长度
   macPadData      MAC填充数据
   dataOffset      偏移量
   输出参数
   criperDataLen   密文数据长度
   criperData      密文数据
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdU3(int algFlag, char *mode, char *id, char *mkType, char *mk, int mkIndex,
		int mkDvsNum, char *mkDvsData, int proKeyType, char *proKey, int proKeyIndex,
		int proDvsNum, char *proDvsData,int proFlag, char *proFactor, char *iv, int encPadDataLen,
		char *encPadData, char *ivMac, int macPadDataLen, char *macPadData, char *dataOffset, char *mac, int *criperDataLen, char *criperData);

// 生成一个对称密钥并以分量形式打印 NE
/*
   功能
   生成一个对称密钥并以分量形式打印
   输入参数
   keyType         密钥类型
   keyLength       密钥强度
   sm4Flag         SM4密钥标识。非0：SM4密钥；0：DES密钥。
   fldNum          分量个数
   fld             分量值
   输出参数
   component       密钥值
   返回值
   >=0：成功
   <0：失败
 */
int UnionJTBHsmCmdNE(TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int sm4Flag,int fldNum,char fld[][80],char *component);

// add by lisq 20141021 end

/*      
	功能:   用C-mac算法计算MAC 
	输入参数：
	zakKey          32H             lmk(008)加密的ZAK
	IV              16H             初始向量
	macData         NH              运算MAC数据
	输出参数：
	CMAC           16H             C-MAC       
	ICV             16H             ZAK左半部份（16位）对C-MAC DES加密运算
	返回值：        
	<0：函数执行失败，值为失败的错误码
	>=0：函数执行成功
 */  
int UnionHsmCmdCF(char *zakKey, char *IV, char *macData, char *CMAC, char *ICV);
//add by zhouxw 20141115 end

// add by lisq 20141117 攀枝花商行
/*
   功能
   SM2公钥加密的PIN明文用SM2私钥做SCE解密后，再用SM4算法加密输出
   输入参数
   mode    算法标识。0：SM2加密转为SM4加密；1：SM2加密转为攀枝花专用算法加密。
   vkIndex 私钥索引。01-20；99：外带密钥
   vkLen   外带私钥长度。仅当vkIndex为99时有
   vk      外带私钥。仅当vkIndex为99时有
   cipherTextLen   密文长度
   cipherText      密文
   zpkValue        ZPK密钥值
   accNo   账号
   输出参数
   pinBlkByZpk     ZPK加密的PIN密文

   返回值
   >=0：成功；<0：失败
 */

int UnionHsmCmdKH(int mode, int vkIndex, int vkLen, char *vk, int cipherTextLen, char *cipherText, char *zpkValue, char *accNo, char *pinBlkByZpk, int sizeofPinBlkByZpk);

/*
   功能
   私有加密算法加密pin
   输入参数
   dataLen 明文数据长度，仅支持6位
   plainData       明文数据，仅支持6位
   sizeofCipherData        密文数据输出参数大小
   输出参数
   cipherData      密文数据
   返回值  
   <0：失败
   >=0：成功
 */
int UnionHsmCmdWY(int dataLen, char *plainData, char *cipherData, int sizeofCipherData);

// add by lisq 20141117 end 攀枝花商行

int RacalCmdHN( char *key1,char *key2,char *date, char *flag, int len ,char *cdate ,int *dstlen,char *dstdate);


int RacalCmdHM(char *key1, char *flag, char *date, int len ,char *macdate ,char *mac);

int RacalCmd50ForHR(char flag,char *edk,int lenOfData,char *indata,char *outdata,int *sizeOfOutData);

int RacalCmdTQ(int modeFlag, char *algFlag, char *keyType, int keyLen, char *keyValue, char *iv, int macDataLen, char *macData, int macLen, char *mac);

int RacalCmdHJ(char *vkIndex, int lenOfVK, char *valueOfVK, char *flag ,char *keyOfZPK ,char *lenOfPin,char *pinBlock,char *clientNo,char *pinOfHR,char *replayFactor);

int RacalCmdHK (char *index,char * keylen, char *key,char *flag,  char * datalen, char *data, char *num,char *genelen,char *gene,char *pin);

// modify by zhouxw 20160105 begin
//int RacalCmdTS( char *key1,char *key2,int blen,char *bdate,int alen,char *adate, char *dstkey);
int RacalCmdTS(char *mode, char *key1,char *key2,char *IV,int blen,char *bdate,int alen,char *adate, char *dstkey);
// modify by zhouxw 10160105 end

int RacalCmdHI(char *factorOfKey, char *keyOfZPK, char *pan, char *pin ,char *pinBlock);

int RacalCmdHB( char *cpOriZPK, char *cpDestZPK, char *AccNo, char *cpOriPIN,char *cpOutPIN);

int RacalCmdHE( char *zpkvalue, char* srcpinblock, int flag,char *account,char *dstpinbolck);

int RacalCmdHF( char *zpkvalue, int flag,char *account,char *gene,char *encrypteddata,char *dstpinbolck,char *nodeid);


/*
   功能：公钥加密的PIN密文转为DES密钥加密（带因子加密）。
   输入参数:
   srcAlgorithmID  非对称密钥算法标识,0:国际算法,  1:国密算法
   vkIndex         指定的私钥，用于解密PIN数据密文
   lenOfVK      私钥长度
   vkValue      私钥值
   flag         “0”：如果数据长度小于密钥长度，加密时先在数据前面补0x00，以使数据长度等于密钥的长度，然后再进行加密；
                “1”：PKCS填充方式（一般情况下使用此方式）
   dstAlgorithmID ZPK密钥算法标识,0:国际算法,  1:国密算法
keyValue:       用于加密PIN的密钥
pan             用户有效主帐号长度
lenOfPinByPK    经公钥加密的PIN数据密文长度
pinByPk         经公钥加密的PIN数据密文
输出参数:
lenOfPinBlock   des密钥加密的pin长度
pinBlock        des密钥加密的pin
idCodeLen       01-20
idCode          ID码明文
返回值:
<0：函数执行失败，值为失败的错误码
>=0：函数执行成功
 */
int RacalCmdN8(int srcAlgorithmID,int vkIndex,int lenOfVK,char *vkValue, char flag, int dstAlgorithmID,char *keyValue, char *pan, int lenOfPinByPK,char *pinByPk,int *lenOfPinBlock, char *pinBlock, int *idCodeLen, char *idCode);

// add by lisq 20150120 微众银行

int UnionHsmCmdKJ(int mode, int flag, char *mkType, char *zek, char *mk, int divNum, char *divData, char *pk, int *keyByPkLen, char *keyByPk);
// add by lisq 20150120 end 微众银行

int UnionHsmCmd8A(int mode, int vkIndex,int vkLen,char *vkValue,int  fillMode,int lenOfPinByPK, char *pinByPK, char *factorData,int keyLen, char *keyValue, char *randomData, int lenOfAccNo,char *accNo, char *dataByZEK,char *digest, char *pinBlock);

/*
   函数功能：
   8D指令，用IBM方式产生一个PIN的Offset
   输入参数：
   minPINLength：最小PIN长度
   pinValidData：用户自定义数据
   decimalizationTable：十六进制数到十进制数的转换表
   pinLength：LMK加密的PIN密文长度
   pinBlockByLMK：由LMK加密的PIN密文
   pvkLength：LMK加密的PVK长度
   pvk：LMK加密的PVK
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   pinOffset：PIN Offset，左对齐，右补'F'

 */
int UnionHsmCmd8D(int minPINLength,char *pinValidData,char *decimalizationTable,int pinLength,char *pinBlockByLMK,int pvkLength,char *pvk,char *accNo,int lenOfAccNo,char *pinOffset);

/*
   函数功能:
   KK指令,SM2公钥加密的pin转换成zpk（DES和SM4）加密
   输入参数：
   keyIndex        SM2密钥索引
   vkLen           SM2外带密钥长度
   vkValue         SM2外带密钥
   lenOfPinByPK    SM2公钥加密的密文长度
   pinByPK         密文
   algorithmID     算法标识,1：SM4密钥, 2: 3DES密钥
   keyType         密钥类型,1：ZPK, 2：TPK 
   pinFormat       PIN格式
   lenOfAccNo      账号长度        
   accNo           账号
   keyLen          密钥长度
   keyValue        ZPK或TPK密钥
   sizeofPinByKey  pinByKey的最大长度
   输出参数：
   pinByKey        DES或SM4下加密的PIN

 */

int unionHsmCmdKK(char *keyIndex, int vkLen, char *vkValue, int lenOfPinByPK, char *pinByPK, int algorithmID, int keyType, char *pinFormat, int lenOfAccNo,char *accNo,
		char *keyValue,char *specialAlg, char *pinByKey,int sizeofPinByKey);
/*
   函数功能:
   KL指令,SM2公钥加密的渠道密码转换成zpk（DES和SM4）加密
   输入参数：
   keyIndex        SM2密钥索引
   vkLen           SM2外带密钥长度
   vkValue 	SM2外带密钥
   lenOfPinByPK    SM2公钥加密的密文长度
   pinByPK  	密文
   algorithmID     算法标识,1：SM4密钥, 2: 3DES密钥
   keyType         密钥类型, 0：ZMK, 1：ZPK, 2：ZEK 
   keyValue        密钥值
   sizeofPinByKey	pinByKey最大长度
   输出参数：
   pinByKey        DES或SM4下加密的PIN

 */

int unionHsmCmdKL(char *keyIndex, int vkLen, char *vkValue, int lenOfPinByPK, char *pinByPK, int algorithmID, int keyType, char *keyValue, char *pinByKey,int sizeofPinByKey);

int UnionHsmCmdKL(char *keyIndex, int vkLen, char *vkValue, int lenOfPinByPK, char *pinByPK, int algorithmID, int keyType, char *keyValue, int saltedOffset, int saltedLen, char *saltedData, char *pinByKey, int sizeofPinByKey, char *hash, int sizeOfHash);

/*(国密)
  函数功能：
  8E指令，用IBM方式产生一个PIN的Offset
  输入参数：
  minPINLength：最小PIN长度
  pinValidData：用户自定义数据
  decimalizationTable：十六进制数到十进制数的转换表
  pinLength：LMK加密的PIN密文长度
  pinBlockByLMK：由LMK加密的PIN密文
  pvk：LMK加密的PVK
  accNo：客户帐号
  lenOfAccNo：客户帐号长度
  输出参数：
  pinOffset：PIN Offset，左对齐，右补'F'

 */
int UnionHsmCmd8E(int minPINLength,char *pinValidData,char *decimalizationTable,int pinLength,char *pinBlockByLMK,char *pvk,char *accNo,int lenOfAccNo,char *pinOffset);

/*
   函数功能: 9B指令        将ZPK加密的PIN密文转为行内软算法加密
   输入参数:
   algorithmID                     密钥算法标识    03DES
   1SM4
   mode                            加密模式  0：核心存折算法
   1：核心卡算法
   2：村镇银行算法
   accNo1                          卡号序号  账号中去除校验位的最右7位
   zpk                             在LMK对加密下的zpk
   pinBlock                        ZPK加密的PIN密文
   format                          Pin格式
   accNo2                          账号  账号中去除校验位的最右12位
   输出参数:
   Pin                             私有软算法加密的PIN密文
 */
int UnionHsmCmd9B(int algorithmID, char *mode, char *accNo1, char *zpk, char *pinBlock, char *format, char *accNo2, char *Pin);

/*
   函数功能：9C指令        将村镇银行软算法加密的PIN密文转为ZPK加密
   输入参数：
   algorithmID                     密钥算法标识    03DES
1 :SM4
zpk                             在LMK对加密下的zpk
pinBlock                        ZPK加密的PIN密文
accNo                           账号
输出参数：
pinByZPK                        ZPK加密的PIN密文
 */
int UnionHsmCmd9C(int algorithmID, char *zpk, char *pinBlock, char *accNo, char *pinByZPK);

/*
   函数功能:9D指令         生成&计算村镇银行MAC
   输入参数：
   mode            模式    1：产生MAC
   2：校验MAC
   macDataLen      用来计算MAC的数据的长度
   macData         用来计算MAC的数据
   mac1Len         待校验的MAC值的长度
   mac1            待校验的MAC值
   输出参数
   mac2            生成的MAC
 */
int UnionHsmCmd9D(char *mode, char *macDataLen, char *macData, char *mac1, char *mac2);
/*
   功能: 将扩展后的ZPK加密的密文用另一把密钥加密(仍为扩展后形式)
   指令: KM
   输入参数:
pinFlag:	1A		Y:源PIN块为扩展形式，
N:未扩展源PIN块
不填则默认为Y
oriAlgorith:	1N		源PIN块算法，1-DES/3DES, 2-SM4
desAlgorith:	1N		目的PIN块算法，1-DES/3DES, 2-SM4
oriKeyLenFlag:	1N		源密钥长度标识,1-单倍，2-双倍，3-三倍
oriKeyIndex/oriZpkValue:1A+3H/16/32/48H	源zpk索引号或密文
desKeyLenFlag: 	1N		源密钥长度标识,1-单倍，2-双倍，3-三倍
desKeyIndex/desZpkValue:1A+3H/16/32/48H	目的zpk索引号或密文
oriFormat:	2N		源PIN块格式,‘01’-‘06’输出的PIN块格式
当源PIN块算法为SM4，该域指示的格式只能为“01” 按照9.8格式组成pin块后补F至16字节
desFormat:	2N		目的PIN块格式,‘01’-‘06’输出的PIN块
当源PIN块算法为SM4，该域指示的格式只能为“01” 按照9.8格式组成pin块后补F至16节
pinBlock:	32H/48H/64H	密钥加密的PIN
oriAccNo:	12/18N		12：‘01’（客户原始账号右12位除去校验位）
18：格式04,其他格式无此域。
desAccNo:	12/18N		12：‘01’（客户原始账号右12位除去校验位）
18：格式04,其他格式无此域。

sizeofPinByZpk:	pinByZpk的最大长度.
输出参数:
pinByZpk:	32H/48H/64H		转加密后的PIN密文
 */
int UnionHsmCmdKM(char pinFlag,int oriAlgorith,int desAlgorith,int oriKeyLenFlag,char *oriKeyIndex,char *oriZpkValue,int desKeyLenFlag,char *desKeyIndex,char *desZpkValue,char *oriFormat,char *desFormat,char *pinBlock,char *oriAccNo,char *desAccNo,char *pinByZpk,int sizeofPinByZpk);

int UnionHsmCmdSD (int srcAlgorithmID,int pkIndex,int lenOfVK,char *vkValue,char *fillMode,char *dataFlag,int lenOfData,char *data,int lenOfID, char *ID,int dstAlgorithmID,int lenofhashFlag,char *hashFlag,char *EDK,char *pinBlockByPK,char *lenOfPIN,char *pinBlockByEDK);

/*
   功能:转化PIN。
   输入参数:
   rootKey：应用主密钥
   keyType：密钥类型；001-ZPK，008-ZAK。
   discreteNum：离散次数
   discreteData1：离散数据1
   discreteData2：离散数据2
   discreteData3：离散数据3
   resPinBlock：源pinblock
   AccNo：帐号
   pinFmt：pin格式
   zpk：目的zpk
   输出参数:
   disPinBlock：目的pinblock。
   errCode：密码机返回码。
   返回值:
   <0: 函数执行失败，值为失败的错误码
   >=0: 函数执行成功
 */
int UnionHsmCmdY5 (char *rootKey, char *keyType,int discreteNum, char *discreteData1, char *discreteData2, char *discreteData3, char *resPinBlock, char *AccNo, char *pinFmt, char * zpk, char *disPinBlock);

/*
   功能:Mac计算，返回MAC值。
   输入参数:
   rootKey：应用主密钥
   keyType：密钥类型；001-ZPK，008-ZAK。
   discreteNum：离散次数
   discreteData1：离散数据1
   discreteData2：离散数据2
   discreteData3：离散数据3
   msgBlockNum：消息块号 (0:仅一块,1:第一块,2:中间块,3:最后块)
   msgType：消息类型(0－消息数据为二进制,1－消息数据为扩展十六进制)
   iv：IV,默认初始值16H的0
   msgLength：消息长度
msg: 消息块
输出参数:
Mac：MAC
errCode：密码机返回码。
返回值:
<0：函数执行失败，值为失败的错误码
>=0：函数执行成功
 */
int UnionHsmCmdY7( char *rootKey, char * keyType,int discreteNum, char *discreteData1, char *discreteData2, char *discreteData3, char msgBlockNum , char msgType, int  msgLength, char *msg, char *mac);

//用SM2公钥做SCE加密
/*输入参数
  pkIndex                       公钥索引
  lenOfPK                       公钥明文长度
  PK                    公钥明文
  lenOfData             数据长度
  data                  数据
  输出参数
  lenOfCiphertext               密文长度
  ciphertext            密文
 */
int UnionHsmCmdKE(int pkIndex,int lenOfPK,char *PK,int lenOfData,char *data,int *lenOfCiphertext,unsigned char *ciphertext);

//用SM2私钥做SM2解密
/*输入参数
  vkIndex                       密钥索引
  lenOfVK                       外带密钥长度
  VK                    外带密钥
  lenOfCiphertext               密文长度
  ciphertext            密文
  输出参数
  lenOfData             数据长度
  data                  数据
 */     
int UnionHsmCmdKF(int vkIndex,int lenOfVK,char *VK,int lenOfCiphertext,unsigned char *ciphertext,int *lenOfData,char *data);

// 导入SM2公钥
/* 输入参数
   index                密钥索引
   lenOfPK      密钥密文字节数
   PK           密钥密文
   输出参数
   无
 */
int UnionHsmCmdE8(int index, int lenOfPK, char *PK);

//将ZPK（DES和SM4）加密的pin转成SM2公钥加密
/* 输入参数
   algorithmID  1A      算法标识,1:SM4密钥, 2:3DES密钥
   SM2Index     2N      SM2索引,"00"---"20"
   lenOfSM2	4N	SM2私钥长度
   SM2Value	nB	SM2私钥值
   keyType      1A      密钥类型,1:ZPK, 2:TPK
   keyIndex     1A+3H   密钥索引 
   keyValue     1A+32H  密钥值,3DES 1A为X，SM4 1A 为S
   format       2H      参见pin格式说明
   lenOfAccNo   nN      账号长度 
   accNo        nN      账号
   pinBlock     16/32H  如果算法标识是3des则16H
   如果算法标识是SM4则32H
   sizeofPinByPK 4N     返回值最大值

   输出参数
   pinByPK      nB      公钥加密的密文  
   无
 */
int UnionHsmCmdKN(int algorithmID, int SM2Index,int lenOfSM2,char *SM2Value,int keyType,char *keyIndex,char *keyValue,char *format,int lenOfAccNo,char *accNo,char *pinBlock,char *pinByPK,int sizeofPinByPK);

int UnionHsmCmdVB(char* modeOfAlgorithm,char* szMethodID, char* szMKSMC, char* szPanNum, char* szAtc, char* szZPK, char* szPinCryptograph , char* szAccount, char* szLinkOffPinMod, char* szEncryptedData);


/*
   功能: 翻译字符PIN，SM4加密的ANSI9.8格式的pinblock转换为SM4算法网银格式的PIN
   指令: LD
   输入参数:
oriZpkValue: 1A+32H     源zpk密文
oriZpkCheckValue:       16H     外带密钥需要进行密钥校验
desZpkValue: 1A+32H     目的zpk密文
dstZpkCheckValue:       16H     外带密钥需要进行密钥校验
pinBlock:       32H     AnsiX9.8格式的PIN密文
lenOfOriAccNo:  N       源账号长度
oriAccNo:       12N     用户有效主帐号
lenOfDstAccNo:  N       目的账号长度
desAccNo:       12N     用户有效主帐号
sizeofPinByZpk: pinByZpk的最大长度.
输出参数:
pinLen          N               pin明文长度
pinByZpk:       64H             转加密后的PIN密文
 */
int UnionHsmCmdLD(char *oriZpkValue,char *oriZpkCheckValue,char *dstZpkValue,char *dstZpkCheckValue,char *pinBlock,int lenOfOriAccNo,char *oriAccNo,int lenOfDstAccNo,char *dstAccNo,int *pinLen,char *pinByZpk,int sizeofPinByZpk);

/*
   功能: PIN密文转换为核心特殊算法加密互转
   指令: LC
   输入参数:
   mode            2N      00：PIN由DES/3DES/SM4加密转为核心特殊加密
   01：PIN由核心特殊加密转为SM4/DES/3DES加密
   algorithmID     2N      1: des/3des
2: SM4
zpkIndex:       1A+3H   zpk密钥索引号
zpk:            16H/1A+32H/1A+48H       zpk密文
checkValue:     16H     当ZPK为‘S’+32H的方式时该域存在
format:         2N      ‘01’-‘06’输出的PIN块格式
当PIN块算法为SM4，该域指示的格式只能为“01” 按照9.8格式组成pin块后补F至16字节
pinBlock:       16H/32H/24H     PIN密钥加密的PIN块密文,长度由加密源PIN块的算法决定。
当模式为01核心算法加密的PIN密文
lenOfAccNo:     N       源账号长度
accNo:          12/18N          12：‘01’（客户原始账号右12位除去校验位）
18：格式04,其他格式无此域。
sizeofPinByZpk: pinByZpk的最大长度.
输出参数:
pinByZpk:       16H/32H         PIN密钥加密的PIN块密文,长度由加密PIN块的算法决定。模式为01该域存在。
 */
int UnionHsmCmdLC(int mode,int algorithmID,char *zpkIndex,char *zpk,char *checkValue,int format,char *pinBlock,int lenOfAccNo,char *accNo,char *pinByZpk,int sizeofPinByZpk);

/*
   功能: 私钥解密对称密钥，再将对称密钥加密的登录密码转为md5或SHA1结果输出
   指令: HR
   输入参数:
   fillMode        1N      0: 如果数据长度小于密钥长度，加密时先在数据前面补
1: PKCS填充方式（一般情况下使用此方式）
vkIndex         2N      索引值"00-20"
lenOfVKValue    4N      私钥长度
vkValue         nB      私钥值
lenOfKeyByPK    4N      公钥加密的密钥值长度
keyByPK         nB      公钥加密的密钥值
pinBlock        16H/32H 登录密码密文
random		32H	随机数
algorithmID     1N      1：md5, 2：SHA1
sizeofOutPinBlock       nN      pinByMD5的最大长度
输出参数:
outPinBlock:    16H/32H
 */
int UnionHsmCmdHR(int fillMode,int vkIndex,int lenOfVKValue,char *vkValue,int lenOfKeyByPK,char *keyByPK,char *pinBlock,char *random,int algorithmID,char *outPinBlock,int sizeofOutPinBlock);
/*
   功能: 计算离散、过程密钥
   指令: US
   输入参数:
   mode		2N		离散模式	01: PBOC离散
02: PBOC离散后字密钥与数据异或
algorithmID	1A		离散算法方案 	X：3DES算法	S: SM4算法
rootKeyType	3H		根密钥类型 	008,00A,000,001
rootKeyValue	1A+32H/3H	根密钥		算法与上述离散算法方案保持一致
discreteNum	1N		离散次数	1-3
discreteData	N*16H		离散数据
processFlag	1A		过程密钥标识	Y:有过程密钥
N:不生成过程密钥
processData	32H		生成过程密钥数据
lenOfData	2N		离散模式为02时存在
data		32H		由外部用0补齐,离散模式为02时存在
discreteKeyType	3H		生成离散子密钥类型	008,00A,000,001

输出参数:
keyValue	32H		密钥密文
checkValue	16H		校验值
lenOfOutData	2N		数据长度
outData				密钥明文与数据运算值
 */
int UnionHsmCmdUS(int mode,char algorithmID,TUnionDesKeyType rootKeyType,char *rootKeyValue,int discreteNum,char *discreteData,char *processFlag,char *processData,int lenOfData,char *data,TUnionDesKeyType discreteKeyType,char *keyValue,char *checkValue,int *lenOfOutData,char *outData);

/*
   功能:将PIN由X9.8转为特殊算法加密
   输入参数：
   zpk		16H/32H/48H	zpk密钥值
   accNo		12N		账号
   pinBlock	16H		zpk加密的pin密文数据
   algFlag  1A 目的edk标志
   edk		16H/32H/48H	edk密钥
   输出参数：
   pin	16H	返回的pin块
   返回值：
   <0：函数执行失败，值为失败的错误码
   >=0：函数执行成功
 */

int UnionHsmCmdNI(char *zpk,int algFlag, char *edk,char *pinBlock,char *accNo,char *pin);
/*
   功能:将ZPK加密的PIN进行弱密码检测
   输入参数：
   algorithmID	1N		算法标识,1:des/3des, 2:SM4
   keyIndex	1A+3H		密钥索引号
   zpk		16H/1A+32H/1A+48H	zpk密钥值
   checkValue	16H		当ZPK为国密时，此域存在
   format	2N		PIN块格式,当PIN块算法为SM4，该域指示的格式只能为"01"
   pinBlock	16H		zpk加密的pin密文数据
   accNo	12N/18N		账号
   rule		1N		0:6位密码为相同的数字
1:使用逐项递增/递减1的顺序数字组合
2:规则包含0和1两种弱密码
输出参数：
返回值：
<0：函数执行失败，值为失败的错误码
=0:	0:表示非弱密码
=1:	1:表示弱密码

 */

int UnionHsmCmdZY(int algorithmID,char *keyIndex,char *zpk,char *checkValue,char *format,char *pinBlock,char *accNo,int rule);
/*
   功能：将由公钥加密的PIN转换成ANSI9.8标准

   输入域  长度    类型    说　明
   命令代码        2       A       值"HD"
   算法标识        1       N       0
   私钥索引        2       N       指定的私钥，用于解密PIN数据密文
   填充方式        1       N       0       1
   源PIN格式       2       N       07深圳农商行专用
   目的PIN格式     2       N       01 ANSI9.8
   密钥类型        3       N       001 ZPK
   ZPK值           16/32+A H       用于加密PIN
   原账号          12      N       当源PIN格式为01时存在
   目的账号        12      N       当目的PIN格式为01时存在
   PIN密文长度     4       N
   PIN密文         n       B       经公钥加密的PIN数据密文


   输出域  长度    类型    说　明
   响应代码        2       A       值"HE"
   错误代码        2       H
   PIN密文         16      H       返回的PIN密文--ANSI9.8
 */
int UnionHsmCmdHD(char *AlgorithmMode,char *vkIndex,char *DataPadFlag,char *FirstPinCiperFormat,char *SecondPinCiperFormat,
		char *zpk, char *conEsscFldAccNo1, char *conEsscFldAccNo2, int lenOfPinByPK,char *pinByPK,int lenOfVK,char *vk, char *pinByZPK);

/**
  add by leipp 20151111

  功能：加密明文pin(四川农信)

  输入域   	长度类型    说　明
  命令代码        2A      	值"SP"
  mode		1N      	模式标志,1:加密存折PIN
5:加密卡PIN
pin		6H		pin明文,6位数字
sizeOfEncPin	nN		返回密文长度

输出域  	长度类型	说明
响应代码        2A       	值"SQ"
encPin		16H

 **/
int UnionHsmCmdSP(int mode, char *pin, char *encPin, int sizeOfEncPin);

/**
  add by leipp 20151111

  功能:将标准PinBlock转为定制算法加密的Pin

  输入域   	长度类型    说　明
  命令代码        2A      	值"SN"
  mode		1N      	模式标志,1:加密存折PIN
5:加密卡PIN
keyValue	16H/32H/	ZPK密钥值
1A+32H/1A+48H
pinType		2A		01：ANSIx9.8格式
accout		12H		PIN账号
pinBlock	16H		PIN密文,ZPK下加密的PIN密文，明文pin格式是6位数字
sizeOfEncPin	nN		返回密文长度

输出域  	长度类型	说明
响应代码        2A       	值"SO"
encPin		16H

 **/

int UnionHsmCmdSN(int mode, char *keyValue, char *pinType, char *accout, char *pinBlock, char *encPin, int sizeOfEncPin);

/*
   add by leipp 20151111
   功能:	导入四川农信私有算法密钥
   描述：	密钥由3个成份组成，每个成份不足8个字节时，后补0.
   在加密机内开辟专门的存储区存储，不再带索引。

   输入域   	长度类型    说　明
   命令代码        2A      	值"SJ"
   key1		16H		LMK0607下加密
   key2		16H		LMK0607下加密
   key3		16H		LMK0607下加密

   输出域  	长度类型	说明
   响应代码        2A       	值"SK"

 */
int UnionHsmCmdSJ(char *key1, char *key2, char *key3);

/*
   函数功能：
   XS指令，验证VISA卡的CVV
   输入参数：
   cvv：待验证的VISA卡的CVV
   cardValidDate：VISA卡的有效期
   cvkLength：CVK的长度
   cvk：CVK
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   errCode：错误代码
   返回值：
   <0，函数执行失败
   0，验证成功
 */

int UnionHsmCmdXS(char *cvv,char *cardValidDate, int cvkLength,char *cvk,char *accNo,int lenOfAccNo,char *serviceCode);

/*
   函数功能：
   SH指令，产生VISA卡校验值CVV
   输入参数：
   cardValidDate：VISA卡的有效期
   cvkLength：CVK的长度
   cvk：CVK
   accNo：客户帐号
   lenOfAccNo：客户帐号长度
   输出参数：
   cvv：生成的VISA卡的CVV
   返回值：
   <0，函数执行失败，无cvv
   >0，成功，返回cvv的长度
 */

int UnionHsmCmdSH(char *cardValidDate,int cvkLength,char *cvk, char *accNo,int lenOfAccNo,char *serviceCode,char *cvv);

/*
   柳州银行定制指令

   函数功能：
   公钥加密PIN转成DES加密（支持字符，国密算法）

   输入参数：

   algorithmID	  		算法标示		1N	  	1：RSA	   2：SM2
   vkIndex			私钥索引    		2N   		"00"－"20"  	"99"：使用外带密钥
   lenOfVK   			外带私钥长度		4N		仅当密钥索引为99时有此域，下一个域长度		
   vk				外带私钥
   fillMode			用公钥加密时所		1N 	  	当算法标示为：RSA时有此域		
   采用的填充方式					
   ZEKValue			zek值		
   format			补位方式		1N   		1：填充0x00：    2：填充0x80：
   encrypMode			加密模式		1N   		1:ECB	 2:CBC
   dataPrefixLen		填充数据前缀长度	2N		如果是：00，侧没有前缀填充数据
   dataPrefix			填充数据前缀		3H		当填充数据长度前缀缀不是00时有此域
   dataSuffixLen		填充数据后缀长度	2N		如果是：00，侧没有后缀填充数据
   dataSuffix			填充数据后缀		3H		当填充数据后缀长度不是00时有此域
   lenOfPinByPK		密文长度			4N
   PinByPK			公钥加密密文   		nB   		经公钥加密的PIN数据密文SM2算法为c1+c3+c2

   输出参数：
   PinByZek			PIN密文			nH		zek加密的数据长度


 */
int UnionHsmCmd6A(int algorithmID,int algorithmID_ZEK,int vkIndex,int lenOfVK,char *vk,int fillMode,char *specialAlg,char *ZEKValue,int format,int encrypMode,int lenOfIV,char *IV,int dataPrefixLen,char *dataPrefix,int dataSuffixLen,char *dataSuffix,int lenOfPinByPK,char *PinByPK,char *PinByZek,char *complexityOfPin);

/*
   青岛银行定制指令

   功能: 
   数据由SM2算法公钥加密转加密为SM4算法加密。

   输入参数：
   vkIndex 私钥索引        2N
   lenOfvk 私钥长度        4N
   vk              私钥密文        nB
   keyType 对称密钥类型    3H
   keyValue        对称密钥        1A+32H
   lenOfdata       数据长度        4N
   data            数　据  nB

   输出参数：
   cipherData      对称密钥加密后的密文    n*2

   返回值：
   <0：函数执行失败，值为失败的错误码
   >=0：函数执行成功
 */     

int UnionHsmCmdPR(int vkIndex, int lenOfvk, char *vk,TUnionDesKeyType keyType,char *keyValue,int lenOfdata,char *data,char *cipherData);
/*将lmk加密密钥转换为pk加密
  输入参数：	keyType 密钥类型
  keyValue LMK密钥加密密文
  checkValue 校验值
  pkIndex  公钥索引
  lenOfPK  公钥长度
  pk 公钥值
  输出参数
  lenOfKeyByPK PK加密的密钥值长度
  keyByPK  PK加密的密钥值	
  pkHash   密钥明文HASH
  返回值：        PK加密的密钥值长度       */
int UnionHsmCmdWN(char *keyType, char* keyValue, char* checkValue,int pkIndex,int lenOfPK,char *pk,int* lenOfKeyByPK,char* keyByPK,char* pkHash);
/*
   将ZPK加密机的PIN转为银行专用算法得到PIN密文输出
   输入参数：
   zpkKeyValue	16H或1A+32H或1A+48H	源ZPK	 	当前加密PIN块的ZPK；LMK对（06-07）下加密
   pinBlock	16H			源PIN块		源ZPK下加密的源PIN块
   pinFormat	2N			源PIN块格式
   accout		12N			账号		账号中去除校验位的最右12位
   resPinFormat	2N			目的PIN块算法	01：无锡农商行专用算法	02：太仓农商行专用算法	03：南京银行专用算法
   parameter	8A/12A	�		银行传入参数	8A太仓银行 12A南京银行
   输出参数：
   resPinBlock	16H	目的PIN块
 */
int UnionHsmCmdS5(char* zpkKeyValue, char* pinBlock, char* pinFormat,char* accout,char* resPinFormat,char* parameter,char* resPinBlock);
/*
   将南京银行专用算法的PIN密文转为X9.8格式的PIN密文输出
   输入参数：
   parameter       12A                     银行传入参数
   resPinFormat    2N                      目的PIN块算法     01：ANSI9.8
   zpkKeyValue     16H或1A+32H或1A+48H     目的ZPK           LMK对（06-07）下加密
   accout          12N                     账号              账号中去除校验位的最右12位
   pinBlock        16H                     Pin块密文         南京银行专用算法加密
   输出参数：
   resPinBlock     16H                     目的PIN块
 */
int UnionHsmCmdSR(char* parameter, char* resPinFormat, char* zpkKeyValue, char* accout, char* pinBlock, char* resPinBlock);

// add by leipp 20151228
/*
   登录密码初始化，把SM2公钥加密的密文，转换成SM3/ZPK国密算法加密输出
   输入参数：
   mode		1A			1-	产生PIN随机因子，SM3摘要输出
   2-	外带PIN随机因子，SM3摘要输出
   3-	SM2公钥加密转为ZPK（SM4算法）加密输出
   vkIndex		2N			00-20，99表示SM2外带
   vkLen		4N			仅当密钥索引为99时有此域
   vkValue     	nB			仅当密钥索引为99时有此域，SM2密钥密文
   lenOfPinByPK	4N			密文长度
   pinByPK		nB			SM2公钥加密的密文。（C1+C3+C2）
   factorData	8H			防重放因子，由外部传入，私钥解密密文后，获得因子明文，由传入因子比对
   keyValue	1A+32H			PIN随机因子密钥/ZPK, PIN随机因子与PIN明文结合组合成PINBLOCK，随机因子由加密机随机生成32位16进制数。
   当模式为1、2密钥类型为ZEK（SM4密钥），
   当模式为3为ZPK(SM4密钥)。
   randomData	32H			PIN随机因子密文
   lenOfAccNo	2N			账号长度
   accNo		nN			账号

   输出参数：
   pinByZEK        32H                     当模式为1，由ZEK加密的pin随机因子密文。
   digest		64H			SM3算法摘要。当模式标志为1、2有此域
   pinBlock	32H			当模式为3，Zpk加密的PINBLOCK密文（ANSI 9.8格式）。
 */

int UnionHsmCmd8B(int mode, int vkIndex,int vkLen,char *vkValue,int lenOfPinByPK, char *pinByPK, char *factorData, char *keyValue, char *randomData, int lenOfAccNo,char *accNo, char *pinByZEK,char *digest, char *pinBlock);

/*
   平安银行	PH指令

   函数功能：
   把SM2公钥加密机的密码转换为专用算法加密

   输入参数：
   ID			算法ID		1A			   1-  把SM2公钥加密的密码转为企业网银登录密码专用算法加密
   2-  把SM2公钥加密的密码转为个人网银登录密码专用算法加密
   3-  把SM2公钥加密的密码转为MD5算法加密
   4-  把SM2公钥加密的密码转为ZPK加密
   vkIndex		私钥索引	 	2N			   00-20，99表示SM2外带
   lenOfVK		私钥长的		4N			   仅当密钥索引为99时有此域，下一个域长度
   vk			私钥密文		nB			   仅当密钥索引为99时有此域，SM2密钥密文
   lenOfPinByPK	密文长度		4N			
   PinByPK		密文			nB			   SM2公钥加密的密文。（C1+C3+C2）
   ZPKValue		zpk			1A+16 H /32 H /48 H		当ID=1，4时有该域，DES算法ZPK
   lenOfaccNO	帐号长度或者附件信息长度	2N			当ID=1时有该域，4~20
   accNO		帐号（PAN）或者附件信息		N			当ID=1时有该域，用户主帐号或者附件信息，若为全"0"，则该域不参与加密PIN的运算
   当ID=4时有该域，用户有效账号12位

   输出参数：
   referFactor	重放因子	
   PinBlock		pin密文					当ID=1时，企业网银登录密码专用算法加密密文 32H
   当ID=2时，个人网银登录密码专用算法加密密文 28B
   当ID=3时，MD5算法加密密文 				 32H
   当ID=4时，ZPK加密的密文，ZPK加密（DES算法）的密文，ANSI 9.8格式 16H
 */

int UnionHsmCmdPH(int ID,int vkIndex,int lenOfVK,char *vk,int lenOfPinByPK,char *PinByPK,char *ZPKValue,int lenOfaccNO,char *accNO,char *referFactor,char *PinBlock);	
	
int UnionHsmCmdWL(char *keytype,char *zmkvalue,char *zmkcheckvalue,char *zmkData,char *outkey,char *outcheckvalue);

int UnionHsmCmd98(char *zmkKey, int keyFlag, char *bankID, char *PSAMID, char *random, char *keyValue);

/*
   将ZPK加密的pinBlock转换成MD5，SHA-1和SM3
   输入参数：
   algorithmID          1N                      PIN块算法
                                                1： DES/3DES
                                                2： SM4
   keyType              3H                      密钥类型
                                                001： ZPK
                                                002： TPK
   keyValue             1A+3H/16H/32H/48H       密钥值
   checkValue           16H                     校验值，当为SM4算法时有该域
   format               2N                      PIN块格式
   pinBlock             16H/32H                 密钥加密的PIN块
   accNo                N                       账号
   hashID               2N                      哈希算法标识
   dataOffset           4N                      加盐偏移
   length               4N                      加盐长度
   data                 nH                      加盐数据
   输出参数：
   hash                 nH                      哈希
 */
int UnionHsmCmdZW(int algorithmID, char *keyType, char *keyValue, char *checkValue, char *format, char *pinBlock, char *accNo, int lenOfAccNo, char *hashID, int dataOffset, int length, char *data, char *hash, int sizeOfHash);

// add begin by zhouxw 20160419
int UnionHsmCmdW1(char *algoriAthmID, char *vkIndex, TUnionDesKeyType dstKeyType, char *keyValue, int dataFillMode, int cipherDataLen, char *cipherData, int lenOfUserName, char *userName, char *pinBlock, int sizeOfPinBlock);

int UnionHsmCmdW3(char *srcZPK, TUnionDesKeyType dstKeyType, char *dstKeyValue, int maxPinLen, char *srcPinBlock, char *format, char *accNo, int lenOfUserName, char *userName, char *dstPinBlock);

int UnionHsmCmdH6(char *srcZPK, int lenOfPin, char *pin, int lenOfPan, char *pan, int lenOfPinBlock, char *pinBlock);

int UnionHsmCmdY8(char *srcZPK, int lenOfPin, char *pin, int lenOfPan, char *pan, int lenOfPlainPin, char *plainPin);
// add end by zhouxw 20160419
int UnionHsmCmdS9(char *keyIndex,int vkLen ,char *vkValue,int lenOfPinByPK,char *pinByPK,int dataFillMode,int DataFillHeadLen,char *DataFillHead,int DataFillTailLen,char *DataFillTail,char *hash, int sizeofHash);

//脚本数据加密命令（EMV 4.1/PBOC）
/*输入参数      
  mode                  模式
  id                    方案ID
  mkIndex                       MK-SMC索引
  mk                    MK-SMC
  checkMk                       MK-SMC校验值
  pan                   PAN/PAN序列号
  atc                   ATC
  iv                    模式标志位0时有此域
  lenOfPlainData                明文数据长度
  plainData             铭文数据
  输出参数
  lenOfCipherData               密文数据长度
  cipherData            密文数据
 */     
int UnionHsmCmdWB(char *mode,char *id,int mkIndex,char *mk,char *checkMk,char *pan,char *atc,char *iv,int lenOfPlainData,char *plainData,int *lenOfCipherData,char *cipherData);
#endif
