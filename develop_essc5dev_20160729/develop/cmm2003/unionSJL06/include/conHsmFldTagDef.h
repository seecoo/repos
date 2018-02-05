// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/8/10
// Version:	1.0

#ifndef _esscFldTagDef_
#define _esscFldTagDef_


#define certInfoSubjectDNTag     0   //证书唯一标示名subject
#define certInfoIssuerDNTag      1   //颁发者唯一标示名issuer
#define certInfoSnTag            2   //证书序列号SN(H型字符串)
#define certInfoBeforeTimeTag    3   //生效时间(YYYYMMDDHHMMSS)
#define certInfoAfterTimeTag     4   //过期时间(YYYYMMDDHHMMSS)
#define certInfoAlgIDTag         5   //证书公钥算法标识(“RSA”或“SM2”)
#define certInfoPKTag            6   //证书公钥数据(RSA公钥格式为DER Hex格式,SM2公钥 为”04||xHex||yHex”)


#define conHsmFldDeviceHandle  100  //设备句柄
#define conHsmFldSessionHandle 101  //会话句柄
#define conHsmFldDeviceInfo	   105  //设备信息
#define conHsmFldRandom        106  //随机数
#define conHsmFldRandomLength  107  //随机数长度
#define conHsmFldKeyIndex      108  //密码设备存储私钥的索引值
#define conHsmFlducPassword    110  //使用私钥权限的标识码
#define conHsmFldPwdLength     111  //私钥访问控制码长度

#define conHsmFlducPublicKey   115     //公钥结构
#define conHsmFlducTmpPublicKey   116  //临时公钥结构
#define conHsmFlducPrivateKey  117     //RSA私钥结构

#define  conHsmFlduiIPKIndex   118   //密码设备内部存储公钥的索引值
#define  conHsmFlduiISKIndex   119   //密码设备内部存储私钥的索引值

#define  conHsmFlduiKeyBits        120   //会话密钥长度
#define  conHsmFldKeyValue         122   //密钥密文
#define  conHsmFldKeyValueLength   123   //密钥长度
#define  conHsmFldKeyHandle        124   //密钥句柄



#define  conHsmFlducSponsorID      130  //密钥协商ID
#define  conHsmFlduiSponsorIDLength      131  //密钥协商ID长度

#define  conHsmFldphAgreementHandle   133 //协商密钥句柄

#define  conHsmFlducResponseID  	    140  //响应方ID值
#define  conHsmFlduiResponseIDLength   141 //响应方ID长度

#define  conHsmFldAlgID   145 //公钥的算法标识
#define  conHsmFldDataIV   146 //IV数据

#define  conHsmFldDataInput 	 150 //缓冲区指针，用于存放输入的数据
#define  conHsmFldInputLength	 151//输入的数据长度
#define  conHsmFldDataOutput	 160//缓冲区指针，用于存放输出的数据
#define  conHsmFldOutputLength	 161//输出的数据长度

#define  conHsmFldSignature     170 //签名数据结构
	
#define  conHsmFlducData 	       31       //输出的数据明文
#define  conHsmFlduiDataLength    181	    //输出的数据明文长度	

#define  conHsmFlducMAC           183
#define  conHsmFlduiMACLength     184

#define  conHsmFlducID          186
#define  conHsmFlduiIDLength     187

#define  conHsmFlducHash         188
#define  conHsmFlduiHashLength   189

		
#define  conHsmFldFileName          190   //缓冲区指针，用于存放输入的文件名
#define  conHsmFldFileNameLen       191   //文件名长度
#define  conHsmFldFileuiFileSize    196   // file size
#define  conHsmFldFileOffset        192   //指定写入文件时的偏移值
#define  conHsmFldFileLength        193   //指定写入文件内容的长度
#define  conHsmFldFileBuffer        194   //存放输入的写文件数据	
	
#define conBankName  134	   //机构名
#define conCertData  136	   //证书数据
#define conPageNo    137       //数据记录的页码
#define conMaxNumPerPage  138  //每页的最大记录数
#define conTotalNum  139       //总的记录数
#define conCurrentNum 140      //当页记录数
#define conCertDN     141      //证书DN	
#define conCERTINFOissuer        142   //颁发者DN
#define conCERTINFOserialNumber  143   //证书序列号
#define conCERTINFOsubject       144   //证书主题
#define conCERTINFOnotBefore     145   //证书有效期的起始时间
#define conCERTINFOnotAfter      146   //证书有效期的终止时间
#define conCERTINFOsignresult    147   //证书签名结果
#define conCERTINFOcert          conCertData   //证书Der编码
#define conHsmFldAlgID2          155   //公钥的算法标识
#define conFlag                  148    //标识
#define conFlag1                 conFlag    //标识1
#define conFlag2                 (conFlag+1)    //标识2
#define conAPPID                 207     //应用标识
#define conBankCode              301	   //机构号
#define conVersion               410     //版本号
#define conSignData              91     //签名要素
#define conSignature             92     //数字签名
#define conOperatorID            214     //操作动作标识


#define conCountryName 331  //国家名
#define conStateOrProvinceName 332 //省名
#define conLocalityName   333		//城市名
#define conOrganizationName  334  //单位名
#define conOrganizationName2 335  //单位名
#define conOrganizationalUnitName 336 //部门
#define conOrganizationalUnitName2 337 //部门
#define conCommonName  338		//用户名
#define conEmail   339			//EMail地址

//国库电子支付
#define congkAlias             1     //别名
#define congkInData           801   //输入数据
#define congkOutData          802   //输出数据
	
#define  conHsmECCPublicKey_bits     201   
#define  conHsmECCPublicKey_x		 202
#define  conHsmECCPublicKey_y        203

#define  conHsmECCPrivateKey_bits    206
#define  conHsmECCPrivateKey_D       207


#define  conHsmFldSignature_r       210
#define  conHsmFldSignature_s       211
	


#define  conHsmRSAPublicKey_BITS     	231   
#define  conHsmRSAPublicKey_M		232
#define  conHsmRSAPublicKey_E    		233


#define  conHsmRSAPrivateKey_BITS          241
#define  conHsmRSAPrivateKey_M             242
#define  conHsmRSAPrivateKey_E		 	   243
#define  conHsmRSAPrivateKey_D			   244
#define  conHsmRSAPrivateKey_PRIME         245
#define  conHsmRSAPrivateKey_PEXP         246
#define  conHsmRSAPrivateKey_COEF         247

#define  conHsmFldECCCipher_X			250
#define  conHsmFldECCCipher_Y			251
#define  conHsmFldECCCipher_C			252
#define  conHsmFldECCCipher_M			253
#define  conHsmFldECCCipher_L			254
	
#define  conHsmECCPublicKey_bits     201   
#define  conHsmECCPublicKey_x		 202   
#define  conHsmECCPublicKey_y        203    

#define  	conHsmECCPublicKeyTmp_bits    261
#define  	conHsmECCPublicKeyTmp_x	    262
#define  	conHsmECCPublicKeyTmp_y     263
		
#define  	conHsmFldContextHex			288
#define  	conHsmFldAgreementZA		289	
	
	
	
	
	

	
	
#define conEsscFldKeyName			1			// 密钥的名称
#define conEsscFldFirstWKName			conEsscFldKeyName	// 第一个工作密钥的名称
#define conEsscFldSecondWKName			(conEsscFldKeyName+1)	// 第二个工作密钥的名称
#define conEsscFldZMKName			11			// ZMK密钥的名称
#define conEsscFldFirstZMKName			conEsscFldZMKName	// 第一个ZMK密钥的名称
#define conEsscFldSecondZMKName			(conEsscFldZMKName+1)	// 第二个ZMK密钥的名称

#define conEsscFldMacData			21			// MAC数据
#define conEsscFldMac				22			// MAC

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

#define conEsscFldPlainData			81			// 明文数据
#define conEsscFldCiperData			82			// 密文数据

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
#define conEsscFldKeyTypeFlag				221		// 密钥类型标示
#define conEsscFldAlgorithm01Mode		conEsscFldAlgorithmMode		// 第一种算法标志
#define conEsscFldAlgorithm02Mode		(conEsscFldAlgorithmMode+1)	// 第二种算法标志

#define conEsscFldAutoSignature			300		// 自动赋值的签名

// 2009/04/09增加
#define conEsscFldRandNum			303				// 随机数
#define conEsscFldRandNum1			conEsscFldRandNum		// 随机数1
#define conEsscFldRandNum2			(conEsscFldRandNum+1)		// 随机数2
#define conEsscFldRandNum3			(conEsscFldRandNum+2)		// 随机数3
// 2009/04/09增加结束

// 2009/06/04增加
#define conEsscFldARQC                          313                     // ARQC                 
#define conEsscFldARPC                          314                     // ARPC                 
#define conEsscFldARC                           315                     // ARC

#define conEsscFldAutoSign			400		// 自动签名

// 2009/4/28,王纯军,增加
#define conEsscFldNodeID			401		// 节点标识
#define conEsscFldFirstNodeID			conEsscFldNodeID		// 第一个节点标识
#define conEsscFldSecondNodeID			(conEsscFldNodeID+1)		// 第二个节点标识
#define conEsscFldKeyVersion			410		// 密钥版本号
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

#define conEsscFldErrorCodeRemark		999		// 错误码描述

// 2007/10/26增加
#define conEsscFldRemark			998		// 备注信息
#define conEsscFldResID				997		// 资源标识
// 2007/10/26增加结束

#define conMngBankCode               110 //机构号
#define conMngBankName               111  //机构名
#define conMngCertData              112  //证书数据
#define conMngIsAddtoCRL             113  //是否加入黑名单   1-加入,0-不加入
#define conMngcertFlag                114  //证书是否存在,1 -存在   
#define conMngCertDN                   115     //证书DN	
#define conMngCERTINFOissuer        116  //颁发者DN
#define conMngCERTINFOserialNumber  117   //证书序列号
#define conMngCERTINFOsubject       118  //证书主题
#define conMngCERTINFOnotBefore     119   //证书有效期的起始时间
#define conMngCERTINFOnotAfter      120   //证书有效期的终止时间
#define conMngCERTINFOsignresult    121   //证书签名结果
#define conMngCERTINFOcert          122   //证书Der编码
#define conMngTotalNum              123   //证书总的记录数
#define conMngPageNo    124       //数据记录的页码
#define conMngMaxNumPerPage  125  //每页的最大记录数
#define conMngCurrentNum 126      //当页记录数
#define conMngCertinfo   127     //证书信息
#define  conMngHsmFldFileName          128   //缓冲区指针，用于存放输入的文件名
#define  conMngCRLFlag     129  //CRL是否存在,1 -存在   
#define conMngKeyType  130  //密钥类型 1--SM2,0--RSA
#define conMngFlducPassword    131  //使用私钥权限的标识码

#define conFlag                  148    //标识
#define conCountryName 331  //国家名
#define conStateOrProvinceName 332 //省名
#define conLocalityName   333		//城市名
#define conOrganizationName  334  //单位名
#define conOrganizationName2 335  //单位名
#define conOrganizationalUnitName 336 //部门
#define conOrganizationalUnitName2 337 //部门
#define conCommonName  338		//用户名
#define conEmail   339			//EMail地址
#define conMngPKCS10Buf  340   //证书请求文件内容
#define conMngCertSynDevinfo	136   //证书同步的设备信息
#define conMngCertSynTargetHsmIp	137 //证书同步设备IP
#define conMngCertSynTargetHsmport	138	//证书同步设备端口
#define conMngCertSynDevIsExist		139	//证书同步设备是否存在 0-不存在 1-已存在

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

