
#ifndef _UnionSJL05Cmd_
#define	_UnionSJL05Cmd_ 

#include "unionDesKey.h"

int UnionDirectHsmCmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);

int UnionSJL05CmdX0(char mode, char *mediaType, char keyType,int compNum, int keyLen, int derivateNum, char derivateData[][32 + 1], int printNum, char printFld[][32], char *value, char *checkvalue);

/*
输入域	长度	类型	备注
命令码	2	A	“A2”
Key type	3	H	密钥类型
Key scheme （LMK）	1	A	用LMK加密方式标志
Print Field 0	n	A	打印字段0，不包含“;”
Delimiter	1	A	值为‘；’，打印字段结束符
Print Field 1	n	A	打印字段1，不包含“;”
…	…	…	…
Last Print Field 	n	A	最后一个打印字段，不包含“;”
消息尾	Nt	A	
输出域	长度	类型	备注
消息头	Nh	A	与输入相同
响应代码	2	A	“A3”
出错代码	2	N	正常为“00”
			“13”：LMK错
“15”：输入数据错误
“17”：授权认证失败
“18”：格式没有定义或打印机故障
Component	16H/
1A+32H/
1A+48H	H          	变种的LMK加密后的密钥分量
*/

int UnionSJL05CmdA2(TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int fldNum,char fld[][80],char *component);

/*
   函数功能：
   A4指令，将密钥分量密文送入密码机解密后合成一个密钥，并用对应LMK加密后返回。
   输入参数：
   keyType：密钥的类型
   keyLength：密钥的长度
   partKeyNum：密钥成分的数量
   partKey：存放各个密钥成分的数组缓冲，为LMK加密的密钥密文
   输出参数：
   keyByLMK：合成的密钥密文，由LMK加密
   checkValue：合成的密钥的校验值

 */
int UnionSJL05CmdA4(int SM4Mode,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,int partKeyNum,char partKey[][50],char *keyByLMK,char *checkValue);

int UnionSJL05CmdA8(TUnionDesKeyType keyType,char *key,char *zmk,char *keyByZMK,char *checkValue);

int UnionSJL05CmdBU(int SM4Mode,TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,char *key,char *checkValue);

int UnionSJL05CmdE0(int crytoFlag,int blockFlag,int encrypMode,int zekLen,char *Zek,int datatype,char *IV,int MsgLen,unsigned char *Msg,unsigned char *encrypData,int *encryDataLen,int sizeOfEncryptedData);

int UnionSJL05CmdE0_f(int crytoFlag,int blockFlag,int encrypMode,int zekLen,\
		char *Zek,int datatype,char *IV,int MsgLen,unsigned char *Msg,\
		unsigned char *encrypData,int *encryDataLen,int sizeOfEncryptedData);

// 生成一对RSA密钥
/* 输入参数
   mode，模式	“0”－产生密钥并保存在HSM内，只输出公钥
							“1”－产生密钥并保存在HSM内，输出公钥和LMK保护的私钥
							“2”－产生密钥，输出公钥和LMK保护的私钥
							“3”－产生密钥并保存在HSM内，不输出公私钥
							“4”-产生密钥并将密钥存入到IC卡备份
   length，模数	0320/512/1024/2048
   pkEncoding	公钥的编码方式
   index	产生密钥存储在HSM内的索引号。Mode!=0时，该域不存在，如：G000
   lenOfPKExponent	公钥exponent的长度	可选参数
   pkExponent	公钥的pkExponent	可选参数
   sizeOfPK	接收公钥的缓冲大小
   sizeOfVK	接收私钥的缓冲大小
   输出参数
   pk		公钥
   lenOfVK		私钥串的长度
   vk		私钥
 */
int UnionSJL05CmdEI(char mode,int length,char *pkEncoding,char *index,
		int lenOfPKExponent,unsigned char *pkExponent,char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK);

/*
函数功能：
	EW指令，用私钥签名 
输入参数：
	hashID:	HASH标识
	signID:	签名标识
	padModeID: 填充模式标识	
	indexOfVK：私钥索引号
	signDataLength：待签名数据的长度
	signData：待签名的数据
	vkLength：LMK加密的私钥长度
	vk：LMK加密的私钥
输出参数：
	signature：生成的签名
返回值：
	<0，函数执行失败，无signature
	0，加密机执行指令失败
	>0，成功，返回signature的长度
*/

int UnionSJL05CmdEW(char *hashID,char *signID,char *padModeID,char *indexOfVK,int signDataLength,char *signData,int vkLength,char *vk,char *signature);

/*
   函数功能：
   UA指令，请求转换RSA私钥从LMK到KEK,LMK保护的RSA私钥为REF结构,KEK使用Mode定义的算法保护REF结构的RSA私钥的每个元素。
   输入参数：
	keyType：密钥类型
	key: 该密钥可由HSM内安全非易失存储区内密钥以及由LMK加密的密文送入
	vkLength: 私钥数据的数据长度
	vk: nB 由LMK保护的RSA私钥（REF结构）
	输出参数：
	vkByKey: key加密的vk数据串
 */

int UnionSJL05CmdUA(TUnionDesKeyType keyType,char *key, int vkLength, char *vk, char *vkByKey);

/*
   函数功能：
   UC指令，使用指定的应用主密钥进行2次离散得到卡片应用子密钥，
   使用指定控制密钥子密钥进行加密保护输出并进行MAC计算。
   输入参数：
   
	mk: 根密钥
	mkType: 根密钥类型109= MK-AC, 209= MK-SMI, 309= MK-SMC, 509= MK-DN
	mkDvsNum: 根密钥离散次数, 1-3次
	mkDvsFlag: 根密钥离散算法 1、银联标准
	mkDvsData: 根密钥离散数据, n*16H, n代表离散次数

	pkType: 保护密钥类型 0=TK(传输密钥)1=DK-SMC(使用MK-SMC实时离散生成的子密钥)
	pk: 保护密钥
	pkDvsNum: 保护密钥离散次数,仅当“保护密钥类型”为1时有,从MK-SMC离散得到DK-SMC的离散次数,范围为1-3
	pkDvsFlag: 保护密钥分散算法 1、银联标准
	pkDvsData: 保护密钥离散数据,n*16H仅当“保护密钥类型”为1时有,保护密钥的离散数据，其中n为“保护密钥离散次数”

输出参数：
criperDataLen: 密文数据长度 4H 密文数据长度(必须是8的倍数，并等于前缀长度、后缀长度、密钥长度的和)
criperData: nB 输出的密文数据
checkValue: 校验值
 */
int UnionSJL05CmdUC(char algoType, char *mkType, char *mk,
		int mkDvsNum, char *mkDvsFlag, char *mkDvsData, char *pkType, char *pk,
		int pkDvsNum, char *pkDvsFlag, char *pkDvsData, int *criperDataLen, char *criperData, char *checkValue);

// 生成一对SM2密钥
/* 输入参数
   	type，类型
	1：签名；2：加密；3：签名和加密
   	length，长度 固定256
   	index,	密钥索引
   	keyPasswd,	密钥口令
   	sizeOfPK	接收公钥的缓冲大小
   	sizeOfVK	接收私钥的缓冲大小
   	输出参数
	pk		公钥
   	lenOfVK		私钥串的长度
   	vk		私钥
   	vkByLmk		纯私钥密文
 */
int UnionSJL05CmdUO(char type,int length,	char *index, char *keyPasswd, char *pk,int sizeOfPK,int *lenOfVK,unsigned char *vk,int sizeOfVK,unsigned char *vkByLmk);

// 生成签名的指令
int UnionSJL05CmdUQ(char *hashID, int lenOfUsrID, char *usrID, int lenOfData,char *data,int vkIndex,int lenOfVK,char *vk,char *sign,int sizeOfSign);

// 国密算法随机产生密钥
int UnionSJL05CmdWI(char mode, char *mediaType, char keyType,int compNum, int keyLen, int derivateNum, char derivateData[][32 + 1], int printNum,	char printFld[][32], char *value, char *checkvalue);

// 计算密钥校验值
int UnionSJL05CmdBS(int SM4Mode, TUnionDesKeyType keyType,TUnionDesKeyLength keyLength,char *key,char *checkValue);

//生成验签的指令
int UnionSJL05CmdUS(int vkIndex, char *hashID, int lenOfUsrID, char *usrID, int lengRCaPK,char *rCaPK,int lenCaCertDataSign,char *caCertDataSign,int lenHashVal,char *hashVal);

// 国际算法签名指令
int UnionSJL05CmdTI(int pkLength, char *pk, int inputDataLength, unsigned char *inputData, unsigned char *outputData);

//SM2私钥转加密
/*输入参数
  algoType	算法标识
  vkByLmk		私钥密文，LMK(36,37)加密
  keyType		密钥类型
  key		主密钥加密下的传输(SM4)密钥
  encMode	加密算法模式
  iv		CBC模式时存在  DES/3DES ：8字节   SM4：16字节
  
  输出参数
  vk		加密下的SM2私钥密文
 */
int UnionSJL05CmdUY(char algoType, unsigned char *vkByLmk, TUnionDesKeyType keyType,char *key,char encMode,char *iv,char *vk);


// 定义打印格式
int UnionSJL05CmdPA(char *format);

// 产生并打印一个密钥分量 国密算法
int UnionSJL05CmdWM(TUnionDesKeyType keyType,int fldNum,char fld[][80],char *component);

#endif

