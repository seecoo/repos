// Author:	Wolfgang Wang
// Date:	2003/10/09

#include "sjl06.h"
#include "unionDesKey.h"

#ifndef _SJL06Cmd_
#define _SJL06Cmd_

#ifndef _Use_SJL06Server_
#ifdef _SJL06CmdForJK_IC_RSA_

/*
2007/4/11,王纯军，增加
50指令
功能：用EDK密钥加解密数据，如果是解密状态，则必须在授权下才能处理，否则报错。

输  入  消  息  格  式

输入域		长度	类型	说　明
消息头		m	A	
命令代码	2	A	值"50"
Flag		1	N	0：加密
			1：解密
EDK		16 or 32
		1A+32 or 1A+48	
			H	LMK24-25加密
DATA_length	4	N	输入数据字节数（8的倍数）范围：0008-4096
DATA		n*2	H	待加密或解密的数据（以BCD码表示）

输  出  消  息  格  式

输出域	长度	类型	说　明
消息头		M	A	
响应代码	2	A	值"51"
错误代码	2	N	
DATA		n*2	H	输出结果

*/
int RacalCmd50(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,char *edk,int lenOfData,char *indata,char *outdata,int sizeOfOutData,char *errCode);

// 20051206，王纯军增加
// 将ZMK加密的密钥转换为LMK加密的ZAK/ZEK
int RacalCmdFK(int hsmSckHDL,PUnionSJL06 pSJL06,char type,char *zmk,char *keyByZMK,char *keyByLMK,char *checkValue,char *errCode);
/*
输  出  消  息  格  式
输入域	长度	类型	说　明
命令代码		2A	值为"TI"。
源KEY标志	1	N	1：TPK           2：ZPK
源KEY	16
1A+32
1A+48	H	当KEY标志为1时为TPK，LMK对（14-15）下加密；
当KEY标志为2时为ZPK，LMK对（06-07）下加密。
目的KEY标志	1	N	1：TPK           2：ZPK
目的KEY	16
1A+32
1A+48	H	当KEY标志为1时为TPK，LMK对（14-15）下加密；
当KEY标志为2时为ZPK，LMK对（06-07）下加密。
源PIN块	16	H	源ZPK下加密的源PIN块。
源PIN格式	1	N	1：ANSI9.8格式    2：IBM格式
源PAN	16	H	用户主帐号，仅用到最右12位; 当源PIN格式为1时有此域
目的PIN格式	1	N	1：ANSI9.8格式    2：IBM格式
目的PAN	16	H	用户主帐号，仅用到最右12位; 当源PIN格式为1时有此域

输  出  消  息  格  式
输出域	长度	类型	说　明
响应代码	2	A	值"TJ"
错误代码	2	H	
PIN密文	16	H	TPK或ZPK下加密
*/
int RacalCmdTI(int hsmSckHDL,PUnionSJL06 pSJL06,char zpk1type,int zpk1Length,char *zpk1,char zpk2type,int zpk2Length,char * zpk2,
		int maxPinLen,char *pinFormat1,char *pinBlockByZPK1,
		char *accNo1,int lenOfAccNo1,char *accNo2,int lenOfAccNo2,
		char *pinFormat2,char *pinBlockByZPK2,char *errCode);

// 将ZMK加密的ZPK转换为LMK加密
int RacalCmdFA(int hsmSckHDL,PUnionSJL06 pSJL06,char *zmk,char *zpkByZmk,int variant,char keyLenFlag,char *zpkByLmk,char *checkValue,char *errCode);
// RSA指令
int SJL06Cmd3E(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmk,char *PK,char *keyByBMK,char *checkValue,char *keyByPK,char *errCode);

int SJL06Cmd34(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,int lenOfVK,char *pk,int sizeOfPK,char *errCode);
int SJL06Cmd35(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,char *vkByMK,int lenOfVKByMK,char *errCode);
int SJL06Cmd36(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,char *vkByMK,int sizeOfVKByMK,char *errCode);
int SJL06Cmd37(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,char *vkIndex,int dataLen,char *data,char *signature,int sizeOfSignature,char *errCode);
int SJL06Cmd38(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,char *vkIndex,int signLen,char *signature,int dataLen,char *data,char *pk,char *errCode);
int SJL06Cmd36(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,char *vkByMK,int sizeOfVKByMK,char *errCode);
int SJL06Cmd40(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,char *bmkForPVK,char *pvk,unsigned char *pinByPK,int lenOfPinByPK,char *pinByPVK,char *errCode);
int SJL06Cmd41(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,char *bmk,char *zpk,char *pan,unsigned char *pinByPK,int lenOfPinByPK,char *pinByZPK,char *errCode);
int SJL06Cmd42(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,unsigned char *encryptedData,int lenOfEncryptedData,char *plainText,int sizeOfPlainText,char *errCode);
int SJL06Cmd3C(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,int lenOfData,char *data,char *hash,char *errCode);

// Mary add begin, 2008-9-23
/*
函数功能：
	33指令，私钥解密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	flag：用公钥加密时所采用的填充方式，
		'0'：如果数据长度小于密钥长度，加密时先在数据前面补0x00，以使数据长度等于密钥的长度，然后再进行加密；
		'1'：PKCS填充方式（一般情况下使用此方式）
	vkIndex：私钥索引，"00"－"20"
	cipherDataLen：密文数据的字节数
	cipherData：用于解密的密文数据
	sizeOfPlainData：plainData所在的存储空间大小
输出参数：
	plainData：解密得到的明文数据
	errCode：错误代码
返回值：
	<0，函数执行失败，无plainData
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回plainData的长度
*/
int SJL06Cmd33(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,char *vkIndex,int cipherDataLen,char *cipherData,char *plainData,int sizeOfPlainData,char *errCode);
// Mary add end, 2008-9-23

// 广发MP专用指令
int SJL06Cmd71(int hsmSckHDL,PUnionSJL06 pSJL06,char *version1,char *group1,char *index1,char *lsdata,
		char *version2,char *group2,char *index2,char *cardNo,
		char *key,char *random,char *errCode);
// 返回解密后的数据的长度
int SJL06Cmd72(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,
		char *lsdata,char *gcdata,char *bmk,char *pik,char *accNo,
		int pinOffset1,int pinLen1,int pinOffset2,int pinLen2,
		int dataLen,char *data,char *plainData,int sizeOfPlainDataBuf,char *errCode);
// 返回加密后数据的长度
int SJL06Cmd74(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,
		char *lsdata,char *gcdata,
		int dataLen,char *data,char *encryptedData,int sizeOfEncryptedDataBuf,char *errCode);
int SJL06Cmd75(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,
		char *lsdata,char *gcdata,
		int dataLen,char *data,char *mac,char *errCode);
int SJL06Cmd76(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,
		char *lsdata,char *gcdata,
		int macDataLen,char *macData,
		int dataLen,char *data,char *errCode);
int SJL06Cmd77(int hsmSckHDL,PUnionSJL06 pSJL06,char *version1,char *group1,char *index1,char *lsdata,
		char *version2,char *group2,char *index2,char *cardNo,
		char *key,char *random,char *errCode);
// 返回加密后数据的长度
int SJL06Cmd7C(int hsmSckHDL,PUnionSJL06 pSJL06,char *version1,char *group1,char *index1,
		char *version2,char *group2,char *index2,
		char *lsdata,char *gcdata,
		int macDataHeadLen,char *macDataHead,
		int dataLen,char *data,
		int offset,
		char *encryptedData,int sizeOfEncryptedDataBuf,char *errCode);
int SJL06Cmd7D(int hsmSckHDL,PUnionSJL06 pSJL06,char *version1,char *group1,char *index1,char *lsdata,
		char *version2,char *group2,char *index2,char *lsData,
		char *errCode);
		
// 返回解密后的数据的长度
int SJL06Cmd73(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,
		char *lsdata,char *gcdata,char *bmk,char *pik,char *accNo,
		char seprator,int pinFld1,int pinFld2,
		int dataLen,char *data,char *plainData,int sizeOfPlainDataBuf,char *errCode);

// 通用IC卡指令
int SJL06CmdE2(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,char *key,char *keyCheckValue,char *errCode);

int SJL06CmdE0(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,char *key,char *keyCheckValue,char *errCode);
int SJL06CmdE4(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,char *errCode);
int SJL06CmdE6(int hsmSckHDL,PUnionSJL06 pSJL06,char *version,char *group,char *index,char *key,char *keyCheckValue,char *errCode);
int SJL06CmdD0(int hsmSckHDL,PUnionSJL06 pSJL06,char *tmkIndex,char *key,char *keyCheckValue,char *errCode);
int SJL06CmdD2(int hsmSckHDL,PUnionSJL06 pSJL06,char *tmkIndex,char *key,char *keyCheckValue,char *errCode);
int SJL06CmdD4(int hsmSckHDL,PUnionSJL06 pSJL06,char *tmkIndex,char *errCode);
int SJL06CmdD6(int hsmSckHDL,PUnionSJL06 pSJL06,char *tmkIndex,char *key,char *keyCheckValue,char *errCode);
int SJL06Cmd10(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *version ,char *group,char *index,char *lsData,char *errCode);
int SJL06Cmd12(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *gcData,char *errCode);
int SJL06Cmd14(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *tmkIndex,char *txKey,char *errCode);
int SJL06Cmd20(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06Cmd22(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06Cmd24(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06Cmd26(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *keyType,char *macMode,char *tmkIndex,char *txKey,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06CmdC0(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *tmkIndex,char *inputData,char *outputData,char *errCode);
int SJL06CmdC2(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *inputData,char *outputData,char *errCode);
int SJL06Cmd50(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *version,char *group,char *index,
	       char *keyFlag,char *lsData,char *gcData,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06Cmd52(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *version,char *group,char *index,
	       char *keyFlag,char *lsData,char *gcData,int dataLength,char *macData,char *mac,char *errCode);
int SJL06Cmd54(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *version,char *group,char *index,
	       char *keyFlag,char *lsData,char *gcData,int dataLength1,char *macData1,char *mac1,
	       int dataLength2,char *macData2,char *mac2,char *errCode);
int SJL06Cmd56(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *version,char *group,char *index,
	       char *lsData,int dataLength,char *macData,char *mac,char *errCode);
int SJL06Cmd58(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *version,char *group,char *index,
	       char *lsData,int dataLength,char *macData,char *mac,char *errCode);
int SJL06Cmd70(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *enFlag,char *version,char *group,char *index,
	       char *lsData,int dataLength,char *inputData,char *mac,char *outputData,char *errCode);
int SJL06CmdC4(int hsmSckHDL,PUnionSJL06 pSJL06,char *desType,char *enFlag,char *tmkIndex,char *txKey,
	       int dataLength,char *inputData,char *outputData,char *errCode);


// 通用磁条卡指令

#ifdef _Suport3Des_
int SJL06Cmd1A(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,char *errCode);
int SJL06Cmd11(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd13(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,char *checkValue,char *errCode);
int SJL06Cmd16(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,char *checkValue,char *errCode);
int SJL06Cmd17(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmk1Index,char *bmk2Index,char *keyValue1,char *keyValue2,char *errCode);
int SJL06Cmd2A(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd31(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd60(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char type,char *bmk,char *pik,char *pin,char *pan,char *pinByPIK,char *errCode);
int SJL06Cmd61(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen1,TUnionDesKeyLength keyLen2,char *bmk1Index,char *bmk2Index,char *keyValue1,char *keyValue2,char *pin1,char *pan,char *pin2,char *errCode);
int SJL06Cmd68(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *pik,char *enPin,char *pan,char *clearPin,char *errCode);
int SJL06Cmd80(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,int macDataLen,char *macData,char *mac,char *errCode);
int SJL06Cmd81(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyLength keyLen,char *bmkIndex,char *mak,char *mac,int macDataLength,char *macData,char *errCode);
#else
int SJL06Cmd1A(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue,char *errCode);
int SJL06Cmd11(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd13(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue,char *checkValue,char *errCode);
int SJL06Cmd16(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue,char *checkValue,char *errCode);
int SJL06Cmd17(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmk1Index,char *bmk2Index,char *keyValue1,char *keyValue2,char *errCode);
int SJL06Cmd2A(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd31(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd60(int hsmSckHDL,PUnionSJL06 pSJL06,char type,char *bmk,char *pik,char *pin,char *pan,char *pinByPIK,char *errCode);
int SJL06Cmd61(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmk1Index,char *bmk2Index,char *keyValue1,char *keyValue2,char *pin1,char *pan,char *pin2,char *errCode);
int SJL06Cmd68(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *pik,char *enPin,char *pan,char *clearPin,char *errCode);
int SJL06Cmd80(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue,int macDataLen,char *macData,char *mac,char *errCode);
int SJL06Cmd81(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *mak,char *mac,int macDataLength,char *macData,char *errCode);
#endif
int SJL06Cmd15(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue1,char *keyValue2,char *errCode);


int SJL06Cmd1B(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue,char *errCode);
int SJL06Cmd1C(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue1,char *variant,char *keyValue2,char *errCode);
int SJL06Cmd21(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *tmkIndex,char *tmkValue,char *errCode);
int SJL06Cmd32(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *tmkIndex,char *tmkValue,char *errCode);
int SJL06Cmd62(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinType,char *tmkIndex,char *clearPin,char *pan,char *enPin,char *errCode);
int SJL06Cmd63(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinType,char *tmkIndex,char *bmkIndex,char *pik,char *enPin1,char *account,char *enPin2,char *errCode);
int SJL06Cmd64(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinType,char *tmkIndex,char *bmkIndex,char *pik,char *enPin1,char *account,char *enPin2,char *errCode);
int SJL06Cmd65(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *pik,char *enPin1,char *pan,char *tmkIndex,char *enPin2,char *errCode);
int SJL06Cmd67(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex1,char *pik1,char *bmkIndex2,char *pik2,char *enPin1,char *pan,char *enPin2,char *errCode);
int SJL06Cmd69(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex1,char *pik1,char *bmkIndex2,char *pik2,char *enPin1,char *pan1,char *pan2,char *enPin2,char *errCode);
int SJL06Cmd84(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinType,char *tmkIndex,char *bmkIndex,char *pik,char *mak,char *enPin1,char *pan,char *addInfo,int macDataLength,char *macData,char *enPin2,char *mac,char *errCode);
int SJL06Cmd85(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex1,char *mak1,char *bmkIndex2,char *mak2,char *mac1,int macDataLength1,char *macData1,int macDataLength2,char *macData2,char *mac2,char *errCode);
int SJL06Cmd86(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex1,char *pik1,char *mak1,char *bmkIndex2,char *pik2,char *mak2,char *enPin1,char *pan,char *mac1,int macDataLength1,char *macData1,int macDataLength2,char *macData2,char *enPin2,char *mac2,char *errCode);
int SJL06Cmd01(int hsmSckHDL,PUnionSJL06 pSJL06,char *errCode);
int SJL06Cmd03(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *errCode);
int SJL06Cmd04(int hsmSckHDL,PUnionSJL06 pSJL06,char *tmkIndex,char *errCode);
int SJL06Cmd90(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *cvka,char *cvkb,char *data,char *cvv,char *errCode);
int SJL06Cmd91(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *cvka,char *cvkb,char *data,char *cvv,char *errCode);
int SJL06Cmd92(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *pvkIndex,char *pvka,char *pvkb,char *pik,char *pinBlock,char *pan,char *pvv,char *errCode);
int SJL06Cmd93(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *pvkIndex,char *pvka,char *pvkb,char *pik,char *pinBlock,char *pan,char *pvv,char *errCode);
int SJL06Cmd7A(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmkIndex,char *keyValue,int dataLength,char *flag,char *data,char *enData,char *errCode);

#endif // for _SJL06CmdForJK_IC_RSA_

#ifdef _RacalCmdForNewRacal_
#include "3DesRacalCmd.h"
#include "3DesRacalSyntaxRules.h"

/*
七、	转换DES密钥：从主密钥加密到公钥加密
用于分发密钥。

输入域	长度	类型	说　明
命令代码	2	A	值"3B"
密钥密文	32	H	用主密钥加密的DES密钥
公　钥	n	B	


输出域	长度	类型	说　明
响应代码	2	A	"3C"
错误代码	2	H	
检查值	16	H	DES密钥加密64bits的0
密钥长度	4	N	DES密钥密文的字节数
密钥密文	n	B	用公钥加密的DES密钥
*/
int SJL06Cmd3B(int hsmSckHDL,PUnionSJL06 pSJL06,char *pk,char *keyByMK,char *keyByPK,int sizeOfBuf,char *checkValue,char *errCode);

/*
六、	转换DES密钥：从公钥加密到主密钥加密
用于接收密钥。

输入域	长度	类型	说　明
命令代码	2	A	值"3A"
私钥索引	2	N	"00"－"20"：用密码机内的私钥
密钥长度	4	N	DES密钥密文的字节数
密钥密文	n	B	用公钥加密的DES密钥


输出域	长度	类型	说　明
响应代码	2	A	"3B"
错误代码	2	H	
DES密钥	32	H	用主密钥加密的DES密钥
检查值	16	H	DES密钥加密64bits的0
*/
int SJL06Cmd3A(int hsmSckHDL,PUnionSJL06 pSJL06,int vkIndex,int lenOfKeyByPK,char *keyByPK,char *keyByMK,char *checkValue,char *errCode);

int SJL06Cmd3E(int hsmSckHDL,PUnionSJL06 pSJL06,char *bmk,char *pk,char *keyByBMK,char *checkValue,char *keyByPK,char *errCode);
int SJL06Cmd3C(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,int lenOfData,char *data,char *hash,char *errCode);
int SJL06Cmd38(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,char *vkIndex,int signLen,char *signature,int dataLen,char *data,char *pk,char *errCode);

int SJL06Cmd37(int hsmSckHDL,PUnionSJL06 pSJL06,char flag,char *vkIndex,int dataLen,char *data,char *signature,int sizeOfSignature,char *errCode);
int SJL06Cmd34(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,int lenOfVK,char *pk,int sizeOfPK,char *errCode);
int SJL06Cmd40(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,char *bmk,char *pvk,unsigned char *pinByPK,int lenOfPinByPK,char *pinByPVK,char *errCode);
int SJL06Cmd41(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,char *bmk,char *zpk,char *pan,unsigned char *pinByPK,int lenOfPinByPK,char *pinByZPK,char *errCode);
int SJL06Cmd42(int hsmSckHDL,PUnionSJL06 pSJL06,char *vkIndex,unsigned char *encryptedData,int lenOfEncryptedData,char *plainText,int sizeOfPlainText,char *errCode);

// 生成银联MAC
int RacalCmdMU(int hsmSckHDL,PUnionSJL06 pSJL06,
		char keyType,char keyLength,char *key,
		char msgType,int msgLen,char *msg,
		char *mac,char *errCode);
int RacalCmdA0(int hsmSckHDL,PUnionSJL06 pSJL06,
	int outputByZMK,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,
	char *zmk,char *keyByLMK,char *keyByZMK,char *checkValue,char *errCode);

// 将一个ZMK加密的密钥转换为LMK加密
int RacalCmdA6(int hsmSckHDL,PUnionSJL06 pSJL06,
	TUnionDesKeyType keyType,char *zmk,char *keyByZmk,
	char *keyByLmk,char *checkValue,char *errCode);

int RacalCmd0A(int hsmSckHDL,PUnionSJL06 pSJL06,char *errCode);
int RacalCmdRA(int hsmSckHDL,PUnionSJL06 pSJL06,char *errCode);
int RacalCmdPA(int hsmSckHDL,PUnionSJL06 pSJL06,char *format,char *errCode);
int RacalCmdA2(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int fldNum,char fld[][80],char *component,char *errCode);

// 转换PIN
int RacalCmdCC(int hsmSckHDL,PUnionSJL06 pSJL06,int zpk1Length,char *zpk1,int zpk2Length,char * zpk2,
		int maxPinLen,char *pinFormat1,char *pinBlockByZPK1,
		char *accNo,int lenOfAccNo,char *pinFormat2,char *pinBlockByZPK2,char *errCode);

// 生成MAC
int RacalCmdMS(int hsmSckHDL,PUnionSJL06 pSJL06,
		char keyType,char keyLength,char *key,
		char msgType,int msgLen,char *msg,
		char *mac,char *errCode);

// Mary add begin, 2004-3-11
#define gPVKI				"0"
//#define gServiceCode			"000"
//#define gIBMDecimalizationTable	"0123456789012345"
//#define gIBMDefaultPinOffset		"FFFFFFFFFFFFFFFF"


/*
函数功能：
	DG指令，用PVK生成PIN的PVV(PIN Verification Value)，
	采用的加密标准为Visa Method
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinLength：LMK加密的PIN密文长度
	pin：LMK加密的PIN密文
	pvkLength：LMK加密的PVK对长度
	pvk：LMK加密的PVK对
	lenOfAccNo：客户帐号长度
	accNo：客户帐号
输出参数：
	pvv：产生的PVV
	errCode：错误代码
返回值：
	<0，函数执行失败，无PVV
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回PVV的长度
*/
int RacalCmdDG(int hsmSckHDL,PUnionSJL06 pSJL06,int pinLength,char *pin,int pvkLength,char *pvk,\
		int lenOfAccNo,char *accNo,char *pvv,char *errCode);

/*
函数功能：
	CA指令，将一个TPK加密的PIN转换为由ZPK加密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByZPK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByZPK的长度
*/
int RacalCmdCA(int hsmSckHDL,PUnionSJL06 pSJL06,int tpkLength,char *tpk,int zpkLength,\
		char *zpk,char *pinFormat1,char *pinBlockByTPK,char *accNo,\
		int lenOfAccNo,char *pinFormat2,char *pinBlockByZPK,char *errCode);

/*
函数功能：
	JE指令，将一个ZPK加密的PIN转换为由LMK加密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	zpkLength：LMK加密的ZPK长度
	zpk：LMK加密的ZPK
	pinFormat：PIN格式
	pinBlockByZPK：转换前由ZPK加密的PIN密文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByLMK：转换后由LMK加密的PIN密文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdJE(int hsmSckHDL,PUnionSJL06 pSJL06,int zpkLength,char *zpk,\
		char *pinFormat,char *pinBlockByZPK,char *accNo,\
		int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
函数功能：
	JC指令，将一个TPK加密的PIN转换为由LMK加密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	tpkLength：LMK加密的TPK长度
	tpk：LMK加密的TPK
	pinFormat：PIN格式
	pinBlockByTPK：转换前由TPK加密的PIN密文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByLMK：转换后由LMK加密的PIN密文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdJC(int hsmSckHDL,PUnionSJL06 pSJL06,int tpkLength,char *tpk,\
		char *pinFormat,char *pinBlockByTPK,char *accNo,\
		int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
函数功能：
	JG指令，将一个LMK加密的PIN转换为由ZPK加密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	zpkLength：LMK加密的ZPK长度
	zpk：LMK加密的ZPK
	pinFormat：PIN格式
	pinLength：LMK加密的PIN密文长度
	pinBlockByZPK：转换前由ZPK加密的PIN密文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByZPK：转换后由LMK加密的PIN密文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByZPK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByZPK的长度
*/
int RacalCmdJG(int hsmSckHDL,PUnionSJL06 pSJL06,int zpkLength,char *zpk,\
		char *pinFormat,int pinLength,char *pinBlockByLMK,char *accNo,\
		int lenOfAccNo,char *pinBlockByZPK,char *errCode);

/*
函数功能：
	JA指令，随机产生一个PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinLength：要求随机生成的PIN明文的长度
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByLMK：随机产生的PIN的密文，由LMK加密
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdJA(int hsmSckHDL,PUnionSJL06 pSJL06,int pinLength,char *accNo,\
		int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
函数功能：
	EE指令，用IBM方式产生一个PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdEE(int hsmSckHDL,PUnionSJL06 pSJL06,int minPINLength,char *pinValidData,\
		char *decimalizationTable,char *pinOffset,int pvkLength,char *pvk,\
		char *accNo,int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
函数功能：
	DE指令，用IBM方式产生一个PIN的Offset
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinOffset
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinOffset的长度
*/
int RacalCmdDE(int hsmSckHDL,PUnionSJL06 pSJL06,int minPINLength,char *pinValidData,\
		char *decimalizationTable,int pinLength,char *pinBlockByLMK,int pvkLength,\
		char *pvk,char *accNo,int lenOfAccNo,char *pinOffset,char *errCode);

/*
函数功能：
	BA指令，使用本地主密钥加密一个PIN明文
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinCryptogramLen：加密后的PIN密文的长度
	pinTextLength：PIN明文的长度
	pinText：PIN明文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByLMK：LMK加密的PIN密文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdBA(int hsmSckHDL,PUnionSJL06 pSJL06,int pinCryptogramLen,int pinTextLength,\
		char *pinText,char *accNo,int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
函数功能：
	NG指令，使用本地主密钥解密一个PIN密文
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinCryptogramLen：PIN密文的长度
	pinCryptogram：PIN密文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	referenceNumber：用LMK18-19加密帐号得到的偏移值
	pinText：PIN明文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinText
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinText的长度
*/
int RacalCmdNG(int hsmSckHDL,PUnionSJL06 pSJL06,int pinCryptogramLen,char *pinCryptogram,\
		char *accNo,int lenOfAccNo,char *referenceNumber,char *pinText,char *errCode);

/*
函数功能：
	DA指令，用IBM方式验证终端的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdDA(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinBlockByTPK,int minPINLength,\
		char *pinValidData,char *decimalizationTable,char *pinOffset,int tpkLength,\
		char *tpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
函数功能：
	EA指令，用IBM方式验证交换中心的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdEA(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinBlockByZPK,int minPINLength,\
		char *pinValidData,char *decimalizationTable,char *pinOffset,int zpkLength,\
		char *zpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
函数功能：
	DC指令，用VISA方式验证终端的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdDC(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinBlockByTPK,\
		char *pvv,int tpkLength,char *tpk,int pvkLength,char *pvk,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
函数功能：
	EC指令，用VISA方式验证交换中心的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdEC(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinBlockByZPK,\
		char *pvv,int zpkLength,char *zpk,int pvkLength,char *pvk,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
函数功能：
	BC指令，用比较方式验证终端的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinBlockByTPK：TPK加密的PIN密文
	tpkLength：LMK加密的TPK
	tpk：LMK加密的TPK
	pinByLMKLength：主机PIN密文长度
	pinByLMK：主机PIN密文，由LMK02-03加密
	pinFormat：PIN格式
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdBC(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinBlockByTPK,\
		int tpkLength,char *tpk,int pinByLMKLength,char *pinByLMK,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
函数功能：
	BE指令，用比较方式验证交换中心的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinBlockByZPK：ZPK加密的PIN密文
	zpkLength：LMK加密的ZPK长度
	zpk：LMK加密的ZPK
	pinByLMKLength：主机PIN密文长度
	pinByLMK：主机PIN密文，由LMK02-03加密
	pinFormat：PIN格式
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdBE(int hsmSckHDL,PUnionSJL06 pSJL06,char *pinBlockByZPK,\
		int zpkLength,char *zpk,int pinByLMKLength,char *pinByLMK,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
函数功能：
	CW指令，产生VISA卡校验值CVV 
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	cardValidDate：VISA卡的有效期
	cvkLength：CVK的长度
	cvk：CVK
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	cvv：生成的VISA卡的CVV
	errCode：错误代码
返回值：
	<0，函数执行失败，无cvv
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回cvv的长度
*/
int RacalCmdCW(int hsmSckHDL,PUnionSJL06 pSJL06,char *cardValidDate,int cvkLength,char *cvk,\
		char *accNo,int lenOfAccNo,char *serviceCode,char *cvv,char *errCode);

/*
函数功能：
	CY指令，验证VISA卡的CVV 
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdCY(int hsmSckHDL,PUnionSJL06 pSJL06,char *cvv,char *cardValidDate,\
		int cvkLength,char *cvk,char *accNo,int lenOfAccNo,char *serviceCode,char *errCode);

/*
函数功能：
	EW指令，用私钥签名 
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	indexOfVK：私钥索引号
	signDataLength：待签名数据的长度
	signData：待签名的数据
	vkLength：LMK加密的私钥长度
	vk：LMK加密的私钥
输出参数：
	signature：生成的签名
	errCode：错误代码
返回值：
	<0，函数执行失败，无signature
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回signature的长度
*/
//int RacalCmdEW(int hsmSckHDL,PUnionSJL06 pSJL06,char *indexOfVK,int signDataLength,
//		char *signData,int vkLength,char *vk,char *signature,char *errCode);

/*
函数功能：
	EY指令，用公钥验证签名
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	macOfPK：公钥的MAC值
	signatureLength：待验证的签名的长度
	signature：待验证的签名
	signDataLength：待签名数据的长度
	signData：待签名的数据
	publicKeyLength：公钥的长度
	publicKey：公钥
输出参数：
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
//int RacalCmdEY(int hsmSckHDL,PUnionSJL06 pSJL06,char *macOfPK,int signatureLength,
//		char *signature,int signDataLength,char *signData,int publicKeyLength,
//		char *publicKey,char *errCode);
// Mary add end, 2004-3-11

// Mary add begin, 2004-3-24
/*
函数功能：
	BU指令，产生一把密钥的校验值
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	keyType：密钥的类型
	keyLength：密钥的长度
	key：LMK加密的密钥密文
输出参数：
	checkValue：生成的密钥校验值
	errCode：错误代码
返回值：
	<0，函数执行失败，无checkValue
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回checkValue的长度
*/
int RacalCmdBU(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyType keyType,\
		TUnionDesKeyLength keyLength,char *key,char *checkValue,char *errCode);

/*
函数功能：
	A4指令，用几把密钥的密文成分合成一把密钥，并生成校验值
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	keyType：密钥的类型
	keyLength：密钥的长度
	partKeyNum：密钥成分的数量
	partKey：存放各个密钥成分的数组缓冲，为LMK加密的密钥密文
输出参数：
	keyByLMK：合成的密钥密文，由LMK加密
	checkValue：合成的密钥的校验值
	errCode：错误代码
返回值：
	<0，函数执行失败，无keyByLMK和checkValue
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回keyByLMK和checkValue的总长度
*/
int RacalCmdA4(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyType keyType,\
		TUnionDesKeyLength keyLength,int partKeyNum,char partKey[][50],\
		char *keyByLMK,char *checkValue,char *errCode);
// Mary add end, 2004-3-24

/*
函数功能：
	A5指令，输入密钥明文的几个成份合成最终的密钥，同时用相应的LMK密钥对加密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	keyType：密钥的类型
	keyLength：密钥的长度
	partKeyNum：密钥成分的数量
	partKey：存放各个密钥成分的数组缓冲
输出参数：
	keyByLMK：合成的密钥密文，由LMK加密
	checkValue：合成的密钥的校验值
	errCode：错误代码
返回值：
	<0，函数执行失败，无keyByLMK和checkValue
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回keyByLMK和checkValue的总长度
*/
int RacalCmdA5(int hsmSckHDL,PUnionSJL06 pSJL06,TUnionDesKeyType keyType,\
		TUnionDesKeyLength keyLength,int partKeyNum,char partKey[][50],\
		char *keyByLMK,char *checkValue,char *errCode);

int RacalCmdA8(int hsmSckHDL,PUnionSJL06 pSJL06,
	TUnionDesKeyType keyType,char *key,char *zmk,
	char *keyByZMK,char *checkValue,char *errCode);
#endif // for _RacalCmdForNewRacal_

#endif // ifndef _Use_SJL06Server_


#ifdef _Use_SJL06Server_

#include "unionSJL06API.h"

#ifdef _SJL06CmdForJK_IC_RSA_

// 将ZMK加密的ZPK转换为LMK加密
int RacalCmdFA(PUnionSJL06Server psjl06Server,char *zmk,char *zpkByZmk,int variant,char keyLenFlag,char *zpkByLmk,char *checkValue,char *errCode);
// RSA指令
int SJL06Cmd3E(PUnionSJL06Server psjl06Server,char *bmk,char *PK,char *keyByBMK,char *checkValue,char *keyByPK,char *errCode);
int SJL06Cmd34(PUnionSJL06Server psjl06Server,char *vkIndex,int lenOfVK,char *pk,int sizeOfPK,char *errCode);
int SJL06Cmd35(PUnionSJL06Server psjl06Server,char *vkIndex,char *vkByMK,int lenOfVKByMK,char *errCode);
int SJL06Cmd36(PUnionSJL06Server psjl06Server,char *vkIndex,char *vkByMK,int sizeOfVKByMK,char *errCode);
int SJL06Cmd37(PUnionSJL06Server psjl06Server,char flag,char *vkIndex,int dataLen,char *data,char *signature,int sizeOfSignature,char *errCode);
int SJL06Cmd38(PUnionSJL06Server psjl06Server,char flag,char *vkIndex,int signLen,char *signature,int dataLen,char *data,char *pk,char *errCode);
int SJL06Cmd36(PUnionSJL06Server psjl06Server,char *vkIndex,char *vkByMK,int sizeOfVKByMK,char *errCode);
int SJL06Cmd40(PUnionSJL06Server psjl06Server,char *vkIndex,char *bmkForPVK,char *pvk,unsigned char *pinByPK,int lenOfPinByPK,char *pinByPVK,char *errCode);
int SJL06Cmd41(PUnionSJL06Server psjl06Server,char *vkIndex,char *bmk,char *zpk,char *pan,unsigned char *pinByPK,int lenOfPinByPK,char *pinByZPK,char *errCode);
int SJL06Cmd42(PUnionSJL06Server psjl06Server,char *vkIndex,unsigned char *encryptedData,int lenOfEncryptedData,char *plainText,int sizeOfPlainText,char *errCode);
int SJL06Cmd3C(PUnionSJL06Server psjl06Server,char flag,int lenOfData,char *data,char *hash,char *errCode);

// 广发MP专用指令
int SJL06Cmd71(PUnionSJL06Server psjl06Server,char *version1,char *group1,char *index1,char *lsdata,
		char *version2,char *group2,char *index2,char *cardNo,
		char *key,char *random,char *errCode);
// 返回解密后的数据的长度
int SJL06Cmd72(PUnionSJL06Server psjl06Server,char *version,char *group,char *index,
		char *lsdata,char *gcdata,char *bmk,char *pik,char *accNo,
		int pinOffset1,int pinLen1,int pinOffset2,int pinLen2,
		int dataLen,char *data,char *plainData,int sizeOfPlainDataBuf,char *errCode);
// 返回加密后数据的长度
int SJL06Cmd74(PUnionSJL06Server psjl06Server,char *version,char *group,char *index,
		char *lsdata,char *gcdata,
		int dataLen,char *data,char *encryptedData,int sizeOfEncryptedDataBuf,char *errCode);
int SJL06Cmd75(PUnionSJL06Server psjl06Server,char *version,char *group,char *index,
		char *lsdata,char *gcdata,
		int dataLen,char *data,char *mac,char *errCode);
int SJL06Cmd76(PUnionSJL06Server psjl06Server,char *version,char *group,char *index,
		char *lsdata,char *gcdata,
		int macDataLen,char *macData,
		int dataLen,char *data,char *errCode);
int SJL06Cmd77(PUnionSJL06Server psjl06Server,char *version1,char *group1,char *index1,char *lsdata,
		char *version2,char *group2,char *index2,char *cardNo,
		char *key,char *random,char *errCode);
// 返回加密后数据的长度
int SJL06Cmd7C(PUnionSJL06Server psjl06Server,char *version1,char *group1,char *index1,
		char *version2,char *group2,char *index2,
		char *lsdata,char *gcdata,
		int macDataHeadLen,char *macDataHead,
		int dataLen,char *data,
		int offset,
		char *encryptedData,int sizeOfEncryptedDataBuf,char *errCode);
int SJL06Cmd7D(PUnionSJL06Server psjl06Server,char *version1,char *group1,char *index1,char *lsdata,
		char *version2,char *group2,char *index2,char *lsData,
		char *errCode);
		
// 返回解密后的数据的长度
int SJL06Cmd73(PUnionSJL06Server psjl06Server,char *version,char *group,char *index,
		char *lsdata,char *gcdata,char *bmk,char *pik,char *accNo,
		char seprator,int pinFld1,int pinFld2,
		int dataLen,char *data,char *plainData,int sizeOfPlainDataBuf,char *errCode);

// 通用IC卡指令
int SJL06CmdE2(PUnionSJL06Server psjl06Server,char *version,char *group,char *index,char *key,char *keyCheckValue,char *errCode);

int SJL06CmdE0(PUnionSJL06Server psjl06Server,char *version,char *group,char *index,char *key,char *keyCheckValue,char *errCode);
int SJL06CmdE4(PUnionSJL06Server psjl06Server,char *version,char *group,char *index,char *errCode);
int SJL06CmdE6(PUnionSJL06Server psjl06Server,char *version,char *group,char *index,char *key,char *keyCheckValue,char *errCode);
int SJL06CmdD0(PUnionSJL06Server psjl06Server,char *tmkIndex,char *key,char *keyCheckValue,char *errCode);
int SJL06CmdD2(PUnionSJL06Server psjl06Server,char *tmkIndex,char *key,char *keyCheckValue,char *errCode);
int SJL06CmdD4(PUnionSJL06Server psjl06Server,char *tmkIndex,char *errCode);
int SJL06CmdD6(PUnionSJL06Server psjl06Server,char *tmkIndex,char *key,char *keyCheckValue,char *errCode);
int SJL06Cmd10(PUnionSJL06Server psjl06Server,char *desType,char *version ,char *group,char *index,char *lsData,char *errCode);
int SJL06Cmd12(PUnionSJL06Server psjl06Server,char *desType,char *gcData,char *errCode);
int SJL06Cmd14(PUnionSJL06Server psjl06Server,char *desType,char *tmkIndex,char *txKey,char *errCode);
int SJL06Cmd20(PUnionSJL06Server psjl06Server,char *desType,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06Cmd22(PUnionSJL06Server psjl06Server,char *desType,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06Cmd24(PUnionSJL06Server psjl06Server,char *desType,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06Cmd26(PUnionSJL06Server psjl06Server,char *desType,char *keyType,char *macMode,char *tmkIndex,char *txKey,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06CmdC0(PUnionSJL06Server psjl06Server,char *desType,char *tmkIndex,char *inputData,char *outputData,char *errCode);
int SJL06CmdC2(PUnionSJL06Server psjl06Server,char *desType,char *inputData,char *outputData,char *errCode);
int SJL06Cmd50(PUnionSJL06Server psjl06Server,char *desType,char *version,char *group,char *index,
	       char *keyFlag,char *lsData,char *gcData,int dataLength,char *inputData,char *mac,char *errCode);
int SJL06Cmd52(PUnionSJL06Server psjl06Server,char *desType,char *version,char *group,char *index,
	       char *keyFlag,char *lsData,char *gcData,int dataLength,char *macData,char *mac,char *errCode);
int SJL06Cmd54(PUnionSJL06Server psjl06Server,char *desType,char *version,char *group,char *index,
	       char *keyFlag,char *lsData,char *gcData,int dataLength1,char *macData1,char *mac1,
	       int dataLength2,char *macData2,char *mac2,char *errCode);
int SJL06Cmd56(PUnionSJL06Server psjl06Server,char *desType,char *version,char *group,char *index,
	       char *lsData,int dataLength,char *macData,char *mac,char *errCode);
int SJL06Cmd58(PUnionSJL06Server psjl06Server,char *desType,char *version,char *group,char *index,
	       char *lsData,int dataLength,char *macData,char *mac,char *errCode);
int SJL06Cmd70(PUnionSJL06Server psjl06Server,char *desType,char *enFlag,char *version,char *group,char *index,
	       char *lsData,int dataLength,char *inputData,char *mac,char *outputData,char *errCode);
int SJL06CmdC4(PUnionSJL06Server psjl06Server,char *desType,char *enFlag,char *tmkIndex,char *txKey,
	       int dataLength,char *inputData,char *outputData,char *errCode);


// 通用磁条卡指令
int SJL06Cmd31(PUnionSJL06Server psjl06Server,char *bmkIndex,char *bmkValue,char *errCode);

#ifdef _Suport3Des_
int SJL06Cmd1A(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,char *errCode);
int SJL06Cmd11(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd1B(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,char *errCode);
int SJL06Cmd13(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,char *checkValue,char *errCode);
int SJL06Cmd16(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,char *checkValue,char *errCode);
int SJL06Cmd17(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmk1Index,char *bmk2Index,char *keyValue1,char *keyValue2,char *errCode);
int SJL06Cmd2A(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd60(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char type,char *bmk,char *pik,char *pin,char *pan,char *pinByPIK,char *errCode);
int SJL06Cmd61(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen1,TUnionDesKeyLength keyLen2,char *bmk1Index,char *bmk2Index,char *keyValue1,char *keyValue2,char *pin1,char *pan,char *pin2,char *errCode);
int SJL06Cmd68(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmkIndex,char *pik,char *enPin,char *pan,char *clearPin,char *errCode);
int SJL06Cmd80(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmkIndex,char *keyValue,int macDataLen,char *macData,char *mac,char *errCode);
int SJL06Cmd81(PUnionSJL06Server psjl06Server,TUnionDesKeyLength keyLen,char *bmkIndex,char *mak,char *mac,int macDataLength,char *macData,char *errCode);
#else
int SJL06Cmd1A(PUnionSJL06Server psjl06Server,char *bmkIndex,char *keyValue,char *errCode);
int SJL06Cmd11(PUnionSJL06Server psjl06Server,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd1B(PUnionSJL06Server psjl06Server,char *bmkIndex,char *keyValue,char *errCode);
int SJL06Cmd13(PUnionSJL06Server psjl06Server,char *bmkIndex,char *keyValue,char *checkValue,char *errCode);
int SJL06Cmd16(PUnionSJL06Server psjl06Server,char *bmkIndex,char *keyValue,char *checkValue,char *errCode);
int SJL06Cmd17(PUnionSJL06Server psjl06Server,char *bmk1Index,char *bmk2Index,char *keyValue1,char *keyValue2,char *errCode);
int SJL06Cmd2A(PUnionSJL06Server psjl06Server,char *bmkIndex,char *bmkValue,char *errCode);
int SJL06Cmd60(PUnionSJL06Server psjl06Server,char type,char *bmk,char *pik,char *pin,char *pan,char *pinByPIK,char *errCode);
int SJL06Cmd61(PUnionSJL06Server psjl06Server,char *bmk1Index,char *bmk2Index,char *keyValue1,char *keyValue2,char *pin1,char *pan,char *pin2,char *errCode);
int SJL06Cmd68(PUnionSJL06Server psjl06Server,char *bmkIndex,char *pik,char *enPin,char *pan,char *clearPin,char *errCode);
int SJL06Cmd80(PUnionSJL06Server psjl06Server,char *bmkIndex,char *keyValue,int macDataLen,char *macData,char *mac,char *errCode);
int SJL06Cmd81(PUnionSJL06Server psjl06Server,char *bmkIndex,char *mak,char *mac,int macDataLength,char *macData,char *errCode);
#endif

int SJL06Cmd15(PUnionSJL06Server psjl06Server,char *bmkIndex,char *keyValue1,char *keyValue2,char *errCode);


int SJL06Cmd1C(PUnionSJL06Server psjl06Server,char *bmkIndex,char *keyValue1,char *variant,char *keyValue2,char *errCode);
int SJL06Cmd21(PUnionSJL06Server psjl06Server,char *bmkIndex,char *tmkIndex,char *tmkValue,char *errCode);
int SJL06Cmd32(PUnionSJL06Server psjl06Server,char *bmkIndex,char *tmkIndex,char *tmkValue,char *errCode);
int SJL06Cmd62(PUnionSJL06Server psjl06Server,char *pinType,char *tmkIndex,char *clearPin,char *pan,char *enPin,char *errCode);
int SJL06Cmd63(PUnionSJL06Server psjl06Server,char *pinType,char *tmkIndex,char *bmkIndex,char *pik,char *enPin1,char *account,char *enPin2,char *errCode);
int SJL06Cmd64(PUnionSJL06Server psjl06Server,char *pinType,char *tmkIndex,char *bmkIndex,char *pik,char *enPin1,char *account,char *enPin2,char *errCode);
int SJL06Cmd65(PUnionSJL06Server psjl06Server,char *bmkIndex,char *pik,char *enPin1,char *pan,char *tmkIndex,char *enPin2,char *errCode);
int SJL06Cmd67(PUnionSJL06Server psjl06Server,char *bmkIndex1,char *pik1,char *bmkIndex2,char *pik2,char *enPin1,char *pan,char *enPin2,char *errCode);
int SJL06Cmd69(PUnionSJL06Server psjl06Server,char *bmkIndex1,char *pik1,char *bmkIndex2,char *pik2,char *enPin1,char *pan1,char *pan2,char *enPin2,char *errCode);
int SJL06Cmd84(PUnionSJL06Server psjl06Server,char *pinType,char *tmkIndex,char *bmkIndex,char *pik,char *mak,char *enPin1,char *pan,char *addInfo,int macDataLength,char *macData,char *enPin2,char *mac,char *errCode);
int SJL06Cmd85(PUnionSJL06Server psjl06Server,char *bmkIndex1,char *mak1,char *bmkIndex2,char *mak2,char *mac1,int macDataLength1,char *macData1,int macDataLength2,char *macData2,char *mac2,char *errCode);
int SJL06Cmd86(PUnionSJL06Server psjl06Server,char *bmkIndex1,char *pik1,char *mak1,char *bmkIndex2,char *pik2,char *mak2,char *enPin1,char *pan,char *mac1,int macDataLength1,char *macData1,int macDataLength2,char *macData2,char *enPin2,char *mac2,char *errCode);
int SJL06Cmd01(PUnionSJL06Server psjl06Server,char *errCode);
int SJL06Cmd03(PUnionSJL06Server psjl06Server,char *bmkIndex,char *errCode);
int SJL06Cmd04(PUnionSJL06Server psjl06Server,char *tmkIndex,char *errCode);
int SJL06Cmd90(PUnionSJL06Server psjl06Server,char *bmkIndex,char *cvka,char *cvkb,char *data,char *cvv,char *errCode);
int SJL06Cmd91(PUnionSJL06Server psjl06Server,char *bmkIndex,char *cvka,char *cvkb,char *data,char *cvv,char *errCode);
int SJL06Cmd92(PUnionSJL06Server psjl06Server,char *bmkIndex,char *pvkIndex,char *pvka,char *pvkb,char *pik,char *pinBlock,char *pan,char *pvv,char *errCode);
int SJL06Cmd93(PUnionSJL06Server psjl06Server,char *bmkIndex,char *pvkIndex,char *pvka,char *pvkb,char *pik,char *pinBlock,char *pan,char *pvv,char *errCode);
int SJL06Cmd7A(PUnionSJL06Server psjl06Server,char *bmkIndex,char *keyValue,int dataLength,char *flag,char *data,char *enData,char *errCode);

#endif // for _SJL06CmdForJK_IC_RSA_

#ifdef _RacalCmdForNewRacal_
#include "3DesRacalCmd.h"
#include "3DesRacalSyntaxRules.h"

// 将一个ZMK加密的密钥转换为LMK加密
int RacalCmdA6(PUnionSJL06Server psjl06Server,
	TUnionDesKeyType keyType,char *zmk,char *keyByZmk,
	char *keyByLmk,char *checkValue,char *errCode);

int RacalCmdA0(PUnionSJL06Server psjl06Server,
	int outputByZMK,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,
	char *zmk,char *keyByLMK,char *keyByZMK,char *checkValue,char *errCode);

int RacalCmd0A(PUnionSJL06Server psjl06Server,char *errCode);
int RacalCmdRA(PUnionSJL06Server psjl06Server,char *errCode);
int RacalCmdPA(PUnionSJL06Server psjl06Server,char *format,char *errCode);
int RacalCmdA2(PUnionSJL06Server psjl06Server,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int fldNum,char fld[][80],char *component,char *errCode);

// 转换PIN
int RacalCmdCC(PUnionSJL06Server psjl06Server,int zpk1Length,char *zpk1,int zpk2Length,char * zpk2,
		int maxPinLen,char *pinFormat1,char *pinBlockByZPK1,
		char *accNo,int lenOfAccNo,char *pinFormat2,char *pinBlockByZPK2,char *errCode);

int RacalCmdTI(PUnionSJL06Server psjl06Server,char zpk1type,int zpk1Length,char *zpk1,char zpk2type,int zpk2Length,char * zpk2,
		int maxPinLen,char *pinFormat1,char *pinBlockByZPK1,
		char *accNo1,int lenOfAccNo1,char *accNo2,int lenOfAccNo2,char *pinFormat2,char *pinBlockByZPK2,char *errCode);

// 生成MAC
int RacalCmdMS(PUnionSJL06Server psjl06Server,
		char keyType,char keyLength,char *key,
		char msgType,int msgLen,char *msg,
		char *mac,char *errCode);

// 生成MAC
/*int RacalCmdMU(PUnionSJL06Server psjl06Server,
		char keyType,char keyLength,char *key,
		char msgType,int msgLen,char *msg,
		char *mac,char *errCode);
*/

// Mary add begin, 2004-3-11
#define gPVKI				"0"
#define gServiceCode			"000"
//#define gIBMDecimalizationTable	"0123456789012345"
//#define gIBMDefaultPinOffset		"FFFFFFFFFFFFFFFF"


/*
函数功能：
	DG指令，用PVK生成PIN的PVV(PIN Verification Value)，
	采用的加密标准为Visa Method
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinLength：LMK加密的PIN密文长度
	pin：LMK加密的PIN密文
	pvkLength：LMK加密的PVK对长度
	pvk：LMK加密的PVK对
	lenOfAccNo：客户帐号长度
	accNo：客户帐号
输出参数：
	pvv：产生的PVV
	errCode：错误代码
返回值：
	<0，函数执行失败，无PVV
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回PVV的长度
*/
int RacalCmdDG(PUnionSJL06Server psjl06Server,int pinLength,char *pin,int pvkLength,char *pvk,\
		int lenOfAccNo,char *accNo,char *pvv,char *errCode);

/*
函数功能：
	CA指令，将一个TPK加密的PIN转换为由ZPK加密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByZPK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByZPK的长度
*/
int RacalCmdCA(PUnionSJL06Server psjl06Server,int tpkLength,char *tpk,int zpkLength,\
		char *zpk,char *pinFormat1,char *pinBlockByTPK,char *accNo,\
		int lenOfAccNo,char *pinFormat2,char *pinBlockByZPK,char *errCode);

/*
函数功能：
	JE指令，将一个ZPK加密的PIN转换为由LMK加密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	zpkLength：LMK加密的ZPK长度
	zpk：LMK加密的ZPK
	pinFormat：PIN格式
	pinBlockByZPK：转换前由ZPK加密的PIN密文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByLMK：转换后由LMK加密的PIN密文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdJE(PUnionSJL06Server psjl06Server,int zpkLength,char *zpk,\
		char *pinFormat,char *pinBlockByZPK,char *accNo,\
		int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
函数功能：
	JC指令，将一个TPK加密的PIN转换为由LMK加密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	tpkLength：LMK加密的TPK长度
	tpk：LMK加密的TPK
	pinFormat：PIN格式
	pinBlockByTPK：转换前由TPK加密的PIN密文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByLMK：转换后由LMK加密的PIN密文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdJC(PUnionSJL06Server psjl06Server,int tpkLength,char *tpk,\
		char *pinFormat,char *pinBlockByTPK,char *accNo,\
		int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
函数功能：
	JG指令，将一个LMK加密的PIN转换为由ZPK加密
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	zpkLength：LMK加密的ZPK长度
	zpk：LMK加密的ZPK
	pinFormat：PIN格式
	pinLength：LMK加密的PIN密文长度
	pinBlockByZPK：转换前由ZPK加密的PIN密文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByZPK：转换后由LMK加密的PIN密文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByZPK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByZPK的长度
*/
int RacalCmdJG(PUnionSJL06Server psjl06Server,int zpkLength,char *zpk,\
		char *pinFormat,int pinLength,char *pinBlockByLMK,char *accNo,\
		int lenOfAccNo,char *pinBlockByZPK,char *errCode);

/*
函数功能：
	JA指令，随机产生一个PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinLength：要求随机生成的PIN明文的长度
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByLMK：随机产生的PIN的密文，由LMK加密
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdJA(PUnionSJL06Server psjl06Server,int pinLength,char *accNo,\
		int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
函数功能：
	EE指令，用IBM方式产生一个PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdEE(PUnionSJL06Server psjl06Server,int minPINLength,char *pinValidData,\
		char *decimalizationTable,char *pinOffset,int pvkLength,char *pvk,\
		char *accNo,int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
函数功能：
	DE指令，用IBM方式产生一个PIN的Offset
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinOffset
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinOffset的长度
*/
int RacalCmdDE(PUnionSJL06Server psjl06Server,int minPINLength,char *pinValidData,\
		char *decimalizationTable,int pinLength,char *pinBlockByLMK,int pvkLength,\
		char *pvk,char *accNo,int lenOfAccNo,char *pinOffset,char *errCode);

/*
函数功能：
	BA指令，使用本地主密钥加密一个PIN明文
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinCryptogramLen：加密后的PIN密文的长度
	pinTextLength：PIN明文的长度
	pinText：PIN明文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	pinBlockByLMK：LMK加密的PIN密文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinBlockByLMK
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinBlockByLMK的长度
*/
int RacalCmdBA(PUnionSJL06Server psjl06Server,int pinCryptogramLen,int pinTextLength,\
		char *pinText,char *accNo,int lenOfAccNo,char *pinBlockByLMK,char *errCode);

/*
函数功能：
	NG指令，使用本地主密钥解密一个PIN密文
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinCryptogramLen：PIN密文的长度
	pinCryptogram：PIN密文
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	referenceNumber：用LMK18-19加密帐号得到的偏移值
	pinText：PIN明文
	errCode：错误代码
返回值：
	<0，函数执行失败，无pinText
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回pinText的长度
*/
int RacalCmdNG(PUnionSJL06Server psjl06Server,int pinCryptogramLen,char *pinCryptogram,\
		char *accNo,int lenOfAccNo,char *referenceNumber,char *pinText,char *errCode);

/*
函数功能：
	DA指令，用IBM方式验证终端的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdDA(PUnionSJL06Server psjl06Server,char *pinBlockByTPK,int minPINLength,\
		char *pinValidData,char *decimalizationTable,char *pinOffset,int tpkLength,\
		char *tpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
函数功能：
	EA指令，用IBM方式验证交换中心的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdEA(PUnionSJL06Server psjl06Server,char *pinBlockByZPK,int minPINLength,\
		char *pinValidData,char *decimalizationTable,char *pinOffset,int zpkLength,\
		char *zpk,int pvkLength,char *pvk,char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
函数功能：
	DC指令，用VISA方式验证终端的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdDC(PUnionSJL06Server psjl06Server,char *pinBlockByTPK,\
		char *pvv,int tpkLength,char *tpk,int pvkLength,char *pvk,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
函数功能：
	EC指令，用VISA方式验证交换中心的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdEC(PUnionSJL06Server psjl06Server,char *pinBlockByZPK,\
		char *pvv,int zpkLength,char *zpk,int pvkLength,char *pvk,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
函数功能：
	BC指令，用比较方式验证终端的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinBlockByTPK：TPK加密的PIN密文
	tpkLength：LMK加密的TPK
	tpk：LMK加密的TPK
	pinByLMKLength：主机PIN密文长度
	pinByLMK：主机PIN密文，由LMK02-03加密
	pinFormat：PIN格式
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdBC(PUnionSJL06Server psjl06Server,char *pinBlockByTPK,\
		int tpkLength,char *tpk,int pinByLMKLength,char *pinByLMK,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
函数功能：
	BE指令，用比较方式验证交换中心的PIN
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	pinBlockByZPK：ZPK加密的PIN密文
	zpkLength：LMK加密的ZPK长度
	zpk：LMK加密的ZPK
	pinByLMKLength：主机PIN密文长度
	pinByLMK：主机PIN密文，由LMK02-03加密
	pinFormat：PIN格式
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdBE(PUnionSJL06Server psjl06Server,char *pinBlockByZPK,\
		int zpkLength,char *zpk,int pinByLMKLength,char *pinByLMK,\
		char *pinFormat,char *accNo,int lenOfAccNo,char *errCode);

/*
函数功能：
	CW指令，产生VISA卡校验值CVV 
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	cardValidDate：VISA卡的有效期
	cvkLength：CVK的长度
	cvk：CVK
	accNo：客户帐号
	lenOfAccNo：客户帐号长度
输出参数：
	cvv：生成的VISA卡的CVV
	errCode：错误代码
返回值：
	<0，函数执行失败，无cvv
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回cvv的长度
*/
int RacalCmdCW(PUnionSJL06Server psjl06Server,char *cardValidDate,int cvkLength,char *cvk,\
		char *accNo,int lenOfAccNo,char *serviceCode,char *cvv,char *errCode);

/*
函数功能：
	CY指令，验证VISA卡的CVV 
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
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
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
int RacalCmdCY(PUnionSJL06Server psjl06Server,char *cvv,char *cardValidDate,\
		int cvkLength,char *cvk,char *accNo,int lenOfAccNo,char *serviceCode,char *errCode);

/*
函数功能：
	EW指令，用私钥签名 
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	indexOfVK：私钥索引号
	signDataLength：待签名数据的长度
	signData：待签名的数据
	vkLength：LMK加密的私钥长度
	vk：LMK加密的私钥
输出参数：
	signature：生成的签名
	errCode：错误代码
返回值：
	<0，函数执行失败，无signature
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回signature的长度
*/
//int RacalCmdEW(PUnionSJL06Server psjl06Server,char *indexOfVK,int signDataLength,
//		char *signData,int vkLength,char *vk,char *signature,char *errCode);

/*
函数功能：
	EY指令，用公钥验证签名
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	macOfPK：公钥的MAC值
	signatureLength：待验证的签名的长度
	signature：待验证的签名
	signDataLength：待签名数据的长度
	signData：待签名的数据
	publicKeyLength：公钥的长度
	publicKey：公钥
输出参数：
	errCode：错误代码
返回值：
	<0，函数执行失败
	0，若errCode为'00'，验证成功，否则加密机执行指令失败，errCode中包含加密机的错误代码
*/
//int RacalCmdEY(PUnionSJL06Server psjl06Server,char *macOfPK,int signatureLength,
//		char *signature,int signDataLength,char *signData,int publicKeyLength,
//		char *publicKey,char *errCode);
// Mary add end, 2004-3-11

// Mary add begin, 2004-3-24
/*
函数功能：
	BU指令，产生一把密钥的校验值
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	keyType：密钥的类型
	keyLength：密钥的长度
	key：LMK加密的密钥密文
输出参数：
	checkValue：生成的密钥校验值
	errCode：错误代码
返回值：
	<0，函数执行失败，无checkValue
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回checkValue的长度
*/
int RacalCmdBU(PUnionSJL06Server psjl06Server,TUnionDesKeyType keyType,\
		TUnionDesKeyLength keyLength,char *key,char *checkValue,char *errCode);

/*
函数功能：
	A4指令，用几把密钥的密文成分合成一把密钥，并生成校验值
输入参数：
	hsmSckHDL：与加密机建立的SOCKET长连接
	pSJL06：加密机属性，包括IP地址等
	keyType：密钥的类型
	keyLength：密钥的长度
	partKeyNum：密钥成分的数量
	partKey：存放各个密钥成分的数组缓冲，为LMK加密的密钥密文
输出参数：
	keyByLMK：合成的密钥密文，由LMK加密
	checkValue：合成的密钥的校验值
	errCode：错误代码
返回值：
	<0，函数执行失败，无keyByLMK和checkValue
	0，加密机执行指令失败，errCode中包含加密机的错误代码
	>0，成功，返回keyByLMK和checkValue的总长度
*/
int RacalCmdA4(PUnionSJL06Server psjl06Server,TUnionDesKeyType keyType,\
		TUnionDesKeyLength keyLength,int partKeyNum,char partKey[][50],\
		char *keyByLMK,char *checkValue,char *errCode);
// Mary add end, 2004-3-24

int RacalCmdDF(PUnionSJL06Server psjl06Server,char *zpk,char *pvk,int maxPinLen,char *decimalizationTable,
		char *pinValidationData,char *pinBlock1,char *accNo1,char *accNo2,
		char *pinBlock2,char *pinOffset,char *errCode);

int RacalCmdA8(PUnionSJL06Server psjl06Server,
	TUnionDesKeyType keyType,char *key,char *zmk,
	char *keyByZMK,char *checkValue,char *errCode);

// 20051206，王纯军增加
int RacalCmdFK(PUnionSJL06Server psjl06Server,char type,char *zmk,char *keyByZMK,char *keyByLMK,char *checkValue,char *errCode);

/*
六、	转换DES密钥：从公钥加密到主密钥加密
用于接收密钥。

输入域	长度	类型	说　明
命令代码	2	A	值"3A"
私钥索引	2	N	"00"－"20"：用密码机内的私钥
密钥长度	4	N	DES密钥密文的字节数
密钥密文	n	B	用公钥加密的DES密钥


输出域	长度	类型	说　明
响应代码	2	A	"3B"
错误代码	2	H	
DES密钥	32	H	用主密钥加密的DES密钥
检查值	16	H	DES密钥加密64bits的0
*/
int SJL06Cmd3A(PUnionSJL06Server psjl06Server,int vkIndex,int lenOfKeyByPK,char *keyByPK,char *keyByMK,char *checkValue,char *errCode);

/*
七、	转换DES密钥：从主密钥加密到公钥加密
用于分发密钥。

输入域	长度	类型	说　明
命令代码	2	A	值"3B"
密钥密文	32	H	用主密钥加密的DES密钥
公　钥	n	B	


输出域	长度	类型	说　明
响应代码	2	A	"3C"
错误代码	2	H	
检查值	16	H	DES密钥加密64bits的0
密钥长度	4	N	DES密钥密文的字节数
密钥密文	n	B	用公钥加密的DES密钥
*/
int SJL06Cmd3B(PUnionSJL06Server psjl06Server,char *pk,char *keyByMK,char *keyByPK,int sizeOfBuf,char *checkValue,char *errCode);

// 生成ZAK/ZEK
int RacalCmdFI(PUnionSJL06Server psjl06Server,char keyFlag,char *zmk,int variant,char keyLenFlag,
	char *keyByZmk,char *keyByLmk,char *checkValue,char *errCode);

// 生成银联MAC
/*int RacalCmdMU(PUnionSJL06Server psjl06Server,
		char keyType,char keyLength,char *key,
		char msgType,int msgLen,char *msg,
		char *mac,char *errCode);
*/


//lixiang add begin,2009-01-19
/*
功能	
	将指定应用的PK加密的PIN转换为ZPK加密。
输入参数
	hsmSckHDL：	与加密机建立的SOCKET长连接
	pSJL06：	加密机属性，包括IP地址等
	pvkIndex：	指定的私钥，用于解密PIN数据密文
	fillMode：	“0”：如果数据长度小于密钥长度，加密时先在数据前面补0x00，以使数据长度等于密钥的长度，然后再进行加密；
			“1”：PKCS填充方式（一般情况下使用此方式）
	ZPK：		用于加密PIN的密钥。
	accNo：		用户主帐号
	lenOfPinBlock:	公钥加密的PIN密文的长度
	pinBlock：	经公钥加密的PIN数据密文
输出参数	
	lenOfPin：	密钥长度
	pinBlock1：	返回ZPK加密下的PIN密文
	pinBlock2：	返回ZPK加密的PIN密文（可用ZPK解密还原真实PIN）
	lenOfUniqueID：	01-20
	UniqueID：	返回的经ASCII扩展后ID码明文
	errCode：	密码机返回码
返回值	
	<0：		函数执行失败，值为失败的错误码
	>=0：		函数执行成功
*/
int RacalCmdH2 (PUnionSJL06Server psjl06Server, int pvkIndex, char fillMode, char *ZPK, char *accNo,int lenOfPinBlock, char *pinBlock, char *lenOfPin,  \
		char *pinBlock1, char *pinBlock2, char *lenOfUniqueID, char *UniqueID, char *errCode);

/*
功能	
	将指定应用的PK加密的PIN转换为ZPK加密。
输入参数
	hsmSckHDL：	与加密机建立的SOCKET长连接
	pSJL06：	加密机属性，包括IP地址等
	OriKeyType：	源KEY类型，’０’ - ZPK ；‘１’ - PVK（目标KEY类型不能为0）
	OriKey：	源KEY，用于解密PIN的密钥。
	lenOfAcc：	帐号长度，4~20。
	accNo：		用户主帐号。
	DesKeyType：	’０’ - ZPK（只当源KEY类型为0时，目标KEY类型可为0）；’１’ - PVK
	DesKey：	目标KEY，用于加密PIN的密钥。
	pinBlock：	经公钥加密的PIN数据密文。
输出参数
	lenOfPinBlock1：密码长度。
	pinBlock1：	返回ZPK2/PVK加密下的PIN密文。
	errCode：	密码机返回码。
返回值	
	<0：		函数执行失败，值为失败的错误码
	>=0：		函数执行成功
*/
int RacalCmdH5 (int hsmSckHDL,PUnionSJL06 pSJL06,char *OriKey, char *lenOfAcc, 	char *AccNo,char DesKeyType, \
			char *DesKey, char *pinBlock, char *lenOfPinBlock1, char *pinBlock1, char *errCode);
		 
/*
功能	
	将指定应用的PK加密的PIN转换为ZPK加密。
输入参数
	hsmSckHDL：	与加密机建立的SOCKET长连接
	pSJL06：	加密机属性，包括IP地址等
	ZPK：		用于解密PIN的密钥。
	lenOfAcc：	帐号长度，4~20。
	accNo：		用户主帐号。
	pinBlockByZPK：	经ZPK加密的PIN数据密文。
	PVK：		PVK。
	pinBlockByPVK：	经PVK加密的PIN数据密文。
输出参数
	errCode：	密码机返回码。
返回值	
	<0：		函数执行失败，值为失败的错误码
	>=0：		函数执行成功
*/

/*
功能	
	将指定应用的PK加密的PIN转换为ZPK加密。
输入参数
	hsmSckHDL：	与加密机建立的SOCKET长连接
	pSJL06：	加密机属性，包括IP地址等
	OriKeyType：	源KEY类型，’０’ - ZPK ；‘１’ - PVK（目标KEY类型不能为0）
	OriKey：	源KEY，用于解密PIN的密钥。
	lenOfAcc：	帐号长度，4~20。
	accNo：		用户主帐号。
	DesKeyType：	’０’ - ZPK（只当源KEY类型为0时，目标KEY类型可为0）；’１’ - PVK
	DesKey：	目标KEY，用于加密PIN的密钥。
	pinBlock：	经公钥加密的PIN数据密文。
输出参数
	lenOfPinBlock1：密码长度。
	pinBlock1：	返回ZPK2/PVK加密下的PIN密文。
	errCode：	密码机返回码。
返回值	
	<0：		函数执行失败，值为失败的错误码
	>=0：		函数执行成功
*/
int RacalCmdH7 (int hsmSckHDL,PUnionSJL06 pSJL06, char *OriKey, char *lenOfAcc,char *AccNo, char *DesKey, \
		char *pinBlock, char *lenOfPinBlock1,char *pinBlock1, char *errCode);


int RacalCmdH9 (int hsmSckHDL,PUnionSJL06 pSJL06, char *ZPK, char *lenOfAcc, char *AccNo, \
		char *pinBlockByZPK, char *PVK, char *pinBlockByPVK, char *errCode);
		
/*
功能	
	将指定应用的PK加密的PIN转换为ZPK加密。
输入参数
	hsmSckHDL：	与加密机建立的SOCKET长连接
	pSJL06：	加密机属性，包括IP地址等
	ZPK：		用于加密PIN的密钥。
	accNo：		用户主帐号。
	pinBlockByZPK：	经ZPK加密的PIN数据密文。
	dataOfZAK：	产生ZAK密钥的因素之一。
输出参数	
	lenOfPin：	密码长度
	ZAK：		返回LMK26-27对对加密下的ZAK密钥。
	errCode：	密码机返回码。
返回值	
	<0：		函数执行失败，值为失败的错误码
	>=0：		函数执行成功
*/
int RacalCmdH4 (int hsmSckHDL, PUnionSJL06 pSJL06, char *ZPK, char *AccNo, char *pinBlockByZPK, \
	char *dataOfZAK, char *lenOfPin, char *ZAK, char *errCode);

/*
功能
	把PinOffset转换为专用算法（FINSE算法）加密的密文
输  入  消  息  格  式

输  入  域	长  度	类  型	内      容
命   令	2	A	S1
PVK	16H或1A+32H或1A+48H	H	LMK对（14-15）下加密的PVK；用于解密offset。
PinOffset	12	N	Offset的值；采用左对齐的方式在右边填充字符“F”。
检查长度	2	N	最小的PIN长度。
账号	12	N	账号中去除校验位的最右12位。
十进制转换表	16	N	将十六进制转换为十进制的转换表。
PIN校验数据	12	A	用户定义的、包含十六进制字符和字符“N”的数据，用来指示HSM插入账号最后5位的位置。


输  出  消  息  格  式
输  出  域	长  度	类  型	内      容
应答码	2	A	S2
错误代码	2	H	00： 正确      其他：失败
结果	6	H	PIN密文
*/
int RacalCmdS1(int hsmSckHDL, PUnionSJL06 pSJL06, char *PVK, char *pinOffSet,int minPinLen, \
	char *accNO,char *decimalizationTable,char *pinValidationData, char *pinBlock, char *errCode);

/*
功能：
	把PinBlock转换为专用算法（FINSE算法）加密的密文
输  入  消  息  格  式

输  入  域	长  度	类  型	内      容
命   令	2	A	S2
ZPK	16H或1A+32H或1A+48H	H	LMK对（14-15）下加密的PVK；用于解密offset。
检查长度	2	N	最小的PIN长度。
账号	12	N	账号中去除校验位的最右12位。
源PIN块	16H	H	源PIN块


输  出  消  息  格  式
输  出  域	长  度	类  型	内      容
应答码	2	A	S 3
错误代码	2	H	00： 正确      其他：失败
结果	6	H	PIN密文
*/
int RacalCmdS2(int hsmSckHDL, PUnionSJL06 pSJL06, char *ZPk, int minPinLen,char *accNo, \
	char *OriPinBlock, char *pinBlock,char *errCode);
	
/*
Field	Length&Type	Details
COMMAND MESSAGE
Message header	mA	(Subsequently returned to the Host unchanged).
Command code	2A	Value E0.
Message block number	1N	0: The only block.
1: The first block.
2: A middle block.
3: The last block.
Crypto flag	1N	0: DES encryption, 1: DES decryption
Algorithm:
Mode of operation	1N	1: ECB – Electronic Code Book
2: CBC – Cipher Block Chaining
3: CFB – Cipher Feed Back
4: OFB – Output Feed Back
5: PCBC
Key flag	1N	0: ZEK – Zone Encryption Key
ZEK	16H/1A+32H/1A+48H	ZEK encrypted under LMK pair 30-31.

Input data format	1N	0: Binary, 1: expanded Hex
Onput data format	1N	0: Binary, 1: expanded Hex
Pad  mode	1N	Present only when message block number is 0, or 3.
0: If data lengths are exact multiples of eight bytes, not padding; otherwise padding characters defined by the following field (Pad character)until data lengths are exact multiples of eight bytes.
1: If  data lengths are exact multiples of eight bytes ,Padding another eight bytes defined  by the following field( (Pad character)； otherwise  padding characters defined by the following field(Pad character),until data lengths are exact multiples of eight bytes.
Pad character	4H
	Present only when message block number is 0, or 3。 .e.g.
          Pad mode|Pad character|Pad count flag
ANSI X9.19  :  0         0000           0
ANSI X9.23  :  1         0000           1
PBOC MAC  :  1         8000           0 
etc.
Pad count flag	1N	Present only when message block number is 0, or 3. 
0：Last byte is not padding count 
1：Last byte is padding count,and the count is within the range of X’01’ to X’08’
IV	16H	Initialization value, present only when Algorithm:Mode of operation is 2, 3,4or 5.
Message length	3H	Actual Message length in bytes.
Binary: nB, 
Expanded Hex : n/2 n must be even.
Message block	nB	The clear/cipher text message block.
RESPONSE MESSAGE

Message header	mA  	Returned to the Host unchanged.
Response code	2A	Value E1.
Error code   	2N	00 : No Errors.
05: Invalid message block number
10 : ZEK/TEK parity error
12 : No keys loaded in user storage.
13 : LMK error. Report to supervisor.
15 : Error in input data.
21 : Invalid user storage index.
26: Invalid key scheme
80: Output error
Onput data format	1N	0: Binary, 1: expanded Hex
Message length	3H	The encrypted/decrypted message length in bytes.
Binary: nB, expanded Hex: n/2 n must be even.
Message block	nB	The encrypted/decrypted text message block.
Next Initial Vector	16H	The next initial vector. Present only in mode of operation: CBC, OFB, CFB, PCBC and Message block number is 1 or 2.
*/
int RacalCmdE0(int hsmSckHDL, PUnionSJL06 pSJL06, char *dataBlock, char *CrypToFlag,char *modeOfAlgorithm, \
	char *ZEK, char *inDataType,char *outDataType,char *padMode,char *padChar,char *padCountFlag,char *IV, \
	char *lenOfMSG,char *MSGBlock,char *datafmt,char *lenOfMSG1,char *MSGBlock1,char *IV1,char *errCode);
	
/*
采用离散MAK计算MAC

输  入  消  息  格  式

输入域	长度	类型	说　明
命令代码	2	A	值“G1”
ZAK	16H/1A+32H/1A+48H	N	被LMK26-27对加密
离散数据	16	H	用于计算离散密钥的数据
MAC数据长度	3H	H	用于计算MAC的数据
MAC数据	nB	B	MAC数据
							输    出    消    息
输出域	长度	类型	说　明
响应代码	2	A	值“G2”
错误代码	2	H	
MAC	16	H	返回的MAC值
计算流程：
1.	用ZAK密钥对离散数据采用IC卡标准离散算法进行离散，如果ZAK为64bit长，则直接对离散数据进行DES加密即可。
2.	用离散密钥对MAC数据采用Ansi9.19（ZAK双倍长以上）或9.9（ZAK单倍长）算法计算MAC；
*/
int RacalCmdG1(int hsmSckHDL, PUnionSJL06 pSJL06, char *ZAK, char *scatData,char *lenOfmac,char *macData, char *mac,char *errCode);


//lixiang add end,2009-01-19

//add by changzx 2009-3-3
/*产生随机数指令*/
int RacalCmdY3(int hsmSckHDL,PUnionSJL06 pSJL06,int min,int max,int Cnt,int isRepeat,char *RandNo,char *errCode);

/*
指令功能：邮政PINBLOCK与ANSI9.8格式相互转换
输  入  消  息  格  式
输入域	长度	与类型		说　明
消息头	mA	
命令代码	2A	Value “ Y1 ”
原ZPK	16H or
1A+32H or 1A+48H
	在LMK对（06-07）下加密
目的ZPK	16H or
1A+32H or 1A+48H
	在LMK对（06-07）下加密
源PIN块	16H or 32H	源ZPK下加密的源PIN块。
源PINBLOCK格式	1N	1：ANSI9.8格式
2：邮政PIN专用格式和专用算法
目的PINBLOCK格式	1N	1：ANSI9.8格式
2：邮政PIN专用格式和专用算法
账号	12N	用户主帐号，仅用到最右12位，不含较验位;
输  出  消  息  格  式
消息头	mA	
响应代码	2A	Value “ Y2 ”
错误代码	2N	
PINBLOCK	16H或32H	当转化成ANSI9.8格式后，PINBLOCK是16H，当转化成邮政PIN格式后是32H。
注：1、邮政PINBLOCK为不带帐号加密。
2、当把ANSI9.8格式转换邮政专用格式时必须为不带帐号加密。
*/
int RacalCmdY1(int hsmSckHDL,PUnionSJL06 pSJL06,char *OriZPK,char *DesZPK,char *OriPinBlock,char *accNo,char *pinBlock,char *errCode);




#endif // for _RacalCmdForNewRacal_

#endif // ifdef _Use_SJL06Server_

#endif

