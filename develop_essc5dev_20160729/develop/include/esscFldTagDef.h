// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/8/10
// Version:	1.0

#include <stdio.h>

#ifndef _esscFldTagDef_
#define _esscFldTagDef_

#define conEsscFldKeyName			1			// 密钥的名称
#define conEsscFldFirstWKName			conEsscFldKeyName	// 第一个工作密钥的名称
#define conEsscFldSecondWKName			(conEsscFldKeyName+1)	// 第二个工作密钥的名称
#define conEsscFldZMKName			11			// ZMK密钥的名称
#define conEsscFldFirstZMKName			conEsscFldZMKName	// 第一个ZMK密钥的名称
#define conEsscFldSecondZMKName			(conEsscFldZMKName+1)	// 第二个ZMK密钥的名称

#define conEsscFldMacData			21			// MAC数据
#define conEsscFldMac				22			// MAC
#define conEsscFldZakByZmk                      23                      // ZAK BY ZMK
#define conEsscFldPlainPin			31			// PIN明文
#define conEsscFldPinOffset			32			// PINOffset
#define conEsscFldEncryptedPinByZPK		33			// ZPK加密的PIN密文
#define conEsscFldEncryptedPinByZPK1		conEsscFldEncryptedPinByZPK			// ZPK1加密的PIN密文
#define conEsscFldEncryptedPinByZPK2		(conEsscFldEncryptedPinByZPK+1)			// ZPK2加密的PIN密文
#define conEsscFldEncryptedPinByLMK0203		35			// LMK0203加密的PIN密文
#define conEsscFldVisaPVV			36			// Visa PVV
#define conEsscFldIBMPinOffset			37			// IBM PinOffset
#define conEsscFldPinByRsaPK			38			// RSAPK加密的PIN

#define conEsscFldAccNo				41			// 账号
#define conEsscFldAccNo1			conEsscFldAccNo			// 账号1
#define conEsscFldAccNo2			(conEsscFldAccNo+1)		// 账号2

#define conEsscFldKeyCheckValue			51				// 密钥校验值
#define conEsscFldFirstKeyCheckValue		conEsscFldKeyCheckValue		// 第一个密钥校验值
#define conEsscFldSecondKeyCheckValue		(conEsscFldKeyCheckValue+1)	// 第二个密钥校验值

#define conEsscFldKeyValue			61			// 密钥值
#define conEsscFldFirstKeyValue			conEsscFldKeyValue	// 第一个密钥值
#define conEsscFldSecondKeyValue		(conEsscFldKeyValue+1)	// 第二个密钥值

#define conEsscFldVisaCVV			71			// VisaCVV
#define conEsscFldCardPeriod			72			// 卡有效期
#define conEsscFldServiceID			73			// 服务代码

#define conEsscFldData				81			// 数据域
#define conEsscFldPlainData			conEsscFldData		// 明文数据
#define conEsscFldCiperData			82			// 密文数据
#define conEsscFldHashData			83			// HASH数据，2008/4/10，added
#define conEsscFldHashDegist			84			// Hash摘要，2008/4/10，added

#define conEsscFldSignData			91			// 签名数据
#define conEsscFldSign				92			// 签名
#define conEsscFldSignDataPadFlag		93			// 签名数据的填充方式

#define conEsscFldDirectHsmCmdReq		100		// 直接的加密机指令
#define conEsscFldDirectHsmCmdRes		101		// 直接的加密机指令响应

#define conEsscFldHsmGrpID			201		// 密码机号
#define conEsscFldKeyContainer			202		// 密钥属主
#define conEsscFldKeyLenFlag			203		// 密钥长度标识
#define conEsscFldForRemoteKMSvrFlag		204		// 为远程密钥服务器的标志
#define conEsscFldForPinLength			205		// PIN的长度
#define conEsscFldRefrenceNumber		206		// 参考数据
#define conEsscFldIDOfApp			207		// 应用编号
#define conEsscFldLengthOfKey			208		// 密钥长度
#define conEsscFldSuccessHsmNum			209		// 执行指令成功的HSM的数量
#define conEsscFldKeyIndex			210		// 密钥索引
#define conEsscFldHsmIPAddrList			211		// 密码机IP地址列表
#define conEsscFldPKCodingMethod		212		// PK的编码方式
#define conEsscFldIV				213		// 初始向量
#define conEsscFldAlgorithmMode			214		// 算法标志
#define conEsscFldAlgorithm01Mode		conEsscFldAlgorithmMode		// 第一种算法标志
#define conEsscFldAlgorithm02Mode		(conEsscFldAlgorithmMode+1)	// 第二种算法标志
#define conEsscFldFileName			220		// 文件名，含全路径
#define conEsscFldKeyTypeFlag			221		// 密钥类型标识
#define conEsscFldIDOfAppManaged		222		// 当前管理的应用编号

#define conEsscFldAutoSignature			300		// 自动赋值的签名

#define conEsscFldAutoSign			400		// 自动签名

// 2009/4/28,王纯军,增加
#define conEsscFldNodeID			401		// 节点标识
#define conEsscFldFirstNodeID			conEsscFldNodeID		// 第一个节点标识
#define conEsscFldSecondNodeID			(conEsscFldNodeID+1)		// 第二个节点标识
#define conEsscFldKeyVersion			410		// 密钥版本号
#define conEsscFldSecondKeyVersion              409             // 密钥版本号 
#define conEsscFldKeyActiveDate			411		// 密钥生效日期
#define conEsscFldKeyInactiveDate		412		// 密钥失效日期
#define conEsscFldKeyReleaseDate		413		// 密钥发布日期
#define conEsscFldKeyPlainValue			414		// 密钥明文值
#define conEsscFldKeyModuleID			415		// 模板标识
#define conEsscFldRecordNumber			416		// 记录数量
#define conEsscFldRecordList			417		// 批量记录串
#define conEsscFldRecordStr			418		// 唯一记录串
#define conEsscFldKeyValueFormat		419		// 密钥值的表示方法
#define conEsscCertificateFormat		420		// 证书格式
#define conEsscFirstCertificateFormat		conEsscCertificateFormat	// 第一个证书格式
#define conEsscSecondCertificateFormat		(conEsscCertificateFormat+1)	// 第二个证书格式
#define conEsscDisperseTimes			425		// 离散次数
#define conEsscDisperseData			426		// 离散数据
#define conEsscARQC				427		// ARQC
#define conEsscARPC				428		// ARPC
#define conEsscARC				429		// ARC
// 2009/48,增加结束

#define conEsscFldPinCiperFormat		500					// PIN密文的格式
#define conEsscFldFirstPinCiperFormat		conEsscFldPinCiperFormat		// 第一个PIN密文的格式
#define conEsscFldSecondPinCiperFormat		(conEsscFldPinCiperFormat+1)		// 第二个PIN密文的格式

// Mary add, 2008-9-16
#define conEsscFldOffsetOfSpecPINCipher		600		// 特殊方式加密的PIN密文，此处仅定义偏移量

#define conEsscFldErrorCodeRemark		999		// 错误码描述

// 2007/10/26增加
#define conEsscFldRemark			998		// 备注信息
#define conEsscFldResID				997		// 资源标识
// 2007/10/26增加结束

// 2009/01/13增加
#define conEsscFldUniqueID			301		// 唯一ID
#define conEsscFldLenOfUniqueID			302		// 唯一ID的长度
// 2009/01/13增加结束

// 2009/04/09增加
#define conEsscFldRandNum			303				// 随机数
#define conEsscFldRandNum1			conEsscFldRandNum		// 随机数1
#define conEsscFldRandNum2			(conEsscFldRandNum+1)		// 随机数2
#define conEsscFldRandNum3			(conEsscFldRandNum+2)		// 随机数3
// 2009/04/09增加结束

// 2009/06/04增加
#define conEsscFldARQC				313			// ARQC
#define conEsscFldARPC				314			// ARPC
#define conEsscFldARC				315			// ARC
// 2009/06/04增加结束

// Mary add begin, 20090708
#define conEsscFldMessageFlag			900			// 消息标识，用户自定义，用于在异步通讯时进行消息匹配
// Mary add end, 20090708

typedef struct
{
	int	tag;		// 域标识
	char	remark[40+1];	// 操作说明
} TUnionEsscFldTag;
typedef TUnionEsscFldTag	*PUnionEsscFldTag;

#define conMaxNumOfEsscFldTag	256
typedef struct
{
	int			realNum;
	TUnionEsscFldTag	tagList[conMaxNumOfEsscFldTag];
} TUnionEsscFldTagGroup;
typedef TUnionEsscFldTagGroup	*PUnionEsscFldTagGroup;
	
int UnionGetDefFileNameOfEsscFldTag(char *fileName);

// 从字符串中读出域标识定义
int UnionReadEsscFldTagFromDefStr(char *str,PUnionEsscFldTag prec);

int UnionPrintEsscFldTagToFile(PUnionEsscFldTag prec,FILE *fp);

int UnionConnectEsscFldTagGroup();

int UnionDisconnectEsscFldTagGroup();

int UnionPrintEsscFldTagGroupToFile(FILE *fp);

int UnionPrintEsscFldTagGroupToSpecFile(char *fileName);

PUnionEsscFldTag UnionFindEsscFldTag(int tag);

char *UnionFindEsscFldTagRemark(int tag);

#endif
