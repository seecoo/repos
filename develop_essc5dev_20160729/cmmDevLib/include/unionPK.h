//	Wolfgang Wang
//	2004/11/11

#ifndef _UnionPK_1_x_
#define _UnionPK_1_x_

// 定义密钥强度标识
#ifdef _UnionPK_4_x_
typedef enum 
{
	con512RSAPair = 512,
	con1024RSAPair = 1024,
	con2048RSAPair = 2048,
	con4096RSAPair = 4096,
	con1152RSAPair = 1152,
	con1408RSAPair = 1408,
	conUnknownLengthPK = 0
} TUnionPKLength;
#else
typedef enum 
{
	con512RSAPair,
	con1024RSAPair,
	con2048RSAPair,
	con4096RSAPair,
	conUnknownLengthPK
} TUnionPKLength;
#endif

/*
typedef enum
{
	conSignature,
	conEncryption,
	conSelfDefinedPK,
	conUnknownPKType
} TUnionPKType;
*/
//update hzh in 2012.11.8
typedef enum
{
	conSignature = 0,   //指数为65537的签名用密钥
	conEncryption = 1,  //指数为65537的密钥管理用密钥
	conSelfDefinedPK = 2,  //指数为65537的自定义密钥
	conSignatureExponent3 = 10,   //指数为3的签名用密钥
	conEncryptionExponent3 = 11,   //指数为3的密钥管理用密钥
	conSelfDefinedPKExponent3 = 12,  //指数为3的密钥管理用密钥
	conUnknownPKType = 99
} TUnionPKType;

#ifdef _UnionPK_3_x_
typedef enum 
{
	conPKAttrFullName,
	conPKAttrValue,
	conPKAttrCheckValue,
	conPKAttrLength,
	conPKAttrType,
	conPKAttrActiveDate,
	conPKAttrMaxEffectiveDays,
	conPKAttrMaxUseTimes,
	conPKAttrUseTimes,
	conPKAttrLastUpdateTime,
	conPKAttrOldVerEffective,
	conPKAttrOldValue,
	conPKAttrOldCheckValue,
	conPKAttrWindowBetweenKeyVer
} TUnionPKAttribute;
	
// 定义密钥类型
typedef struct
{
	char				fullName[40+1];		// 密钥全名，采用owner.keyApp.keyName
	char				value[1024+1];		// 密钥密文
	char				checkValue[16+1];	// 密钥校验值
	TUnionPKLength			length;			// 密钥强度
	TUnionPKType			type;			// 密钥类型
	char				activeDate[8+1];	// 密钥有效日期
	long				maxEffectiveDays;	// 密钥最大有效日期
	long				maxUseTimes;		// 密钥最多使用的次数
	long				useTimes;		// 使用次数
	long				lastUpdateTime;		// 最近一次更新时间
	char				container[20+1];	
	
	short				oldVerEffective;	// 旧版本是否生效
	char				oldValue[1024+1];	// 旧版本的密钥值
	char				oldCheckValue[16+1];	// 旧版本的密钥校验值
	int				windowBetweenKeyVer;	// 两个密钥版本间的窗口期
#ifdef _UnionPK_4_x_
	short				isWritingLocked;	// 是否被写锁住了
#endif
} TUnionPK;
#else			
typedef enum 
{
	conPKAttrValue,
	conPKAttrCheckValue,
	conPKAttrLength,
	conPKAttrType,
	conPKAttrActiveDate,
	conPKAttrPassiveDate
} TUnionPKAttribute;
	
// 定义密钥类型
typedef struct
{
	char				fullName[40+1];		// 密钥全名，采用owner.keyApp.keyName
	char				value[1024+1];		// 密钥密文
	char				checkValue[16+1];	// 密钥校验值
	TUnionPKLength			length;			// 密钥强度
	TUnionPKType			type;			// 密钥类型
	char				activeDate[8+1];	// 密钥有效日期
	char				passiveDate[8+1];	// 密钥失效日期
} TUnionPK;
#endif
typedef TUnionPK			*PUnionPK;

typedef struct
{
	char			app[10+1];
	char			owner[40+1];
	char			name[40+1];
	char			container[20+1];
	long			usedTimes;
	long			usedDays;
	int			checkWritingLocked;
	short			checkOutdate;
	TUnionPKLength		length;
	TUnionPKType		type;
	char			activeDate[8+1];
} TUnionPKQueryCondition;
typedef TUnionPKQueryCondition	*PUnionPKQueryCondition;


// 2004/11/25 Wolfgang Wang
int UnionUsePK(PUnionPK pkey);
int UnionIsOldVerPKEffective(PUnionPK pkey);
int UnionUpdatePKValue(PUnionPK pkey,char *value,char *checkValue);
// End of addition of 2004/11/25

// 创建一个密钥对象
int UnionCreatePK(char *owner,char *keyApp,char *keyName,PUnionPK pPK);
// 读取一个密钥对象
int UnionReadPK(char *owner,char *keyApp,char *keyName,PUnionPK pPK);
// 删除一个密钥对象
int UnionDeletePK(char *owner,char *keyApp,char *keyName);
// 更改一个密钥对象的属性
int UnionUpdatePK(char *owner,char *keyApp,char *keyName,TUnionPKAttribute attr,char *attrValue);

// 判断是否是合法的密钥长度
int UnionIsValidPKLength(TUnionPKLength desKeyLen);
// 判断是否是合法的密钥属性
int UnionIsValidPKAttribute(TUnionPKAttribute attr);
// 判断是否是合法的密钥密文
int UnionIsValidPKValue(char *keyCryptogram);
// 判断是否是合法的密钥校验值
int UnionIsValidPKCheckValue(char *checkValue);
// 判断是否是合法的密钥全称
int UnionIsValidPKFullName(char *fullName);
// 判断是否是合法的密钥类型
int UnionIsValidPKType(TUnionPKType type);

int UnionPrintPK(PUnionPK pPK);
int UnionIsValidPK(PUnionPK pPK);

int UnionIsPKStillEffective(PUnionPK pPK);

int UnionFormDefaultPK(PUnionPK pkey,char *fullName,char *keyValue,TUnionPKType type,char *checkValue);

int UnionGetNameOfPK(TUnionPKType type,char *keyName);

// 从密钥名称中获取属主名称
int UnionAnalysisPKFullName(char *fullName,char *appID,char *ownerName,char *keyName);
int UnionGetAppIDOutOfPKFullName(char *fullName,char *appID);
int UnionGetKeyNameOutOfPKFullName(char *fullName,char *keyName);
int UnionGetOwnerNameOutOfPKFullName(char *fullName,char *owner);

TUnionPKLength UnionConvertPKLength(int keyLen);
TUnionPKType UnionConvertPKType(char *keyTypeName);

int UnionGetNameOfPKType(TUnionPKType type,char *nameOfType);
int UnionGetAscCharLengthOfPK(TUnionPKLength length);
int UnionGetBitsLengthOfRSAPair(TUnionPKLength length);

// 将PK记录写入到文件中
int UnionPrintPKToRecStrTxtFile(PUnionPK pPK,FILE *fp);

// 将一个PK密钥写入到串中
int UnionPutPKIntoRecStr(PUnionPK pPK,char *recStr,int sizeOfRecStr);

// 从一个串中读 PK 密钥
int UnionReadPKFromRecStr(char *recStr,int lenOfRecStr,PUnionPK pPK);

#endif
