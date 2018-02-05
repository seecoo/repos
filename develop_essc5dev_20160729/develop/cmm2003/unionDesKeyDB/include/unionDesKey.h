//	Wolfgang Wang
//	2003/05/01

/*
	2004/11/25 升级为 3.0
	改变了UnionDesKey的结构
	是宏定义 _UnionDesKey_3_x_ 引导的结构
*/

#ifndef _UnionDesKey_1_x_
#define _UnionDesKey_1_x_

//#include "unionKeyDB.h"
#include "sjl06LMK.h"
#include "unionErrCode.h"


// 定义密钥强度标识
typedef enum 
{
	con64BitsDesKey,
	con128BitsDesKey,
	con192BitsDesKey,
	conUnknownLengthDesKey
} TUnionDesKeyLength;

typedef enum 
{
	conProtectedByLMK0001,	
	conProtectedByLMK0203,
	conProtectedByLMK0405,
	conProtectedByLMK0607,
	conProtectedByLMK0809,
	conProtectedByLMK1011,
	conProtectedByLMK1213,
	conProtectedByLMK1415,
	conProtectedByLMK1617,
	conProtectedByLMK1819,
	conProtectedByLMK2021,
	conProtectedByLMK2223,
	conProtectedByLMK2425,
	conProtectedByLMK2627,
	conProtectedByLMK2829,
	conProtectedByLMK3031,
	conProtectedByLMK3233,
	conProtectedByLMK3435,
	conProtectedByLMK3637,
	conProtectedByLMK3839,
	conProtectedByZMK,
	conUnknownProtectMethod
} TUnionDesKeyProtectMethod;

typedef enum
{
	conZPK,
	conZAK,
	conZMK,
	conTMK,
	conTPK,
	conTAK,
	conPVK,
	conCVK,
	conZEK,
	conWWK,
	conBDK,
	conSelfDefinedKey,
	conEDK,
	conUnknownDesKeyType,
	conMKAC,
	conMKSMC,
	conMKSMI,
	conKMCSeed = 22,
	conKMUSeed = 23,
	conMDKSeed = 21,
} TUnionDesKeyType;
			
#ifdef _UnionDesKey_3_x_
typedef enum 
{
	conDesKeyFullName,
	conDesKeyAttrValue,
	conDesKeyAttrCheckValue,
	conDesKeyAttrLength,
	//conDesKeyAttrProtectMethod,
	conDesKeyAttrType,
	conDesKeyAttrActiveDate,
	conDesKeyAttrMaxEffectiveDays,
	conDesKeyAttrMaxUseTimes,
	conDesKeyAttrUseTimes,
	conDesKeyAttrLastUpdateTime,
	conDesKeyAttrOldVerEffective,
	conDesKeyAttrOldValue,
	conDesKeyAttrOldCheckValue,
	conDesKeyAttrWindowBetweenKeyVer,
	conDesKeyAttrContainer
} TUnionDesKeyAttribute;
typedef struct
{
	char				fullName[40+1];		// 密钥全名，采用owner.keyApp.keyName
	char				value[48+1];		// 密钥密文
	char				checkValue[16+1];	// 密钥校验值
	TUnionDesKeyLength		length;			// 密钥强度
	//TUnionDesKeyProtectMethod	protectMethod;		// 密钥保护方法
	TUnionDesKeyType		type;			// 密钥类型
	char				activeDate[8+1];	// 密钥有效日期
	long				maxEffectiveDays;	// 密钥最大有效日期
	long				maxUseTimes;		// 密钥最多使用的次数
	long				useTimes;		// 使用次数
	long				lastUpdateTime;		// 最近一次更新时间
	char				container[20+1];	// 

	short				oldVerEffective;	// 旧版本是否生效
	char				oldValue[48+1];		// 旧版本的密钥值
	char				oldCheckValue[16+1];	// 旧版本的密钥校验值
	int				windowBetweenKeyVer;	// 两个密钥版本间的窗口期
#ifdef _UnionDesKey_4_x_
	short				isWritingLocked;	// 是否被写锁住了
#endif
} TUnionDesKey;	
#else	
typedef enum 
{
	conDesKeyAttrValue,
	conDesKeyAttrCheckValue,
	conDesKeyAttrLength,
	//conDesKeyAttrProtectMethod,
	conDesKeyAttrType,
	conDesKeyAttrActiveDate,
	conDesKeyAttrPassiveDate
} TUnionDesKeyAttribute;

// 定义密钥类型
typedef struct
{
	char				fullName[40+1];		// 密钥全名，采用owner.keyApp.keyName
	char				value[48+1];		// 密钥密文
	char				checkValue[16+1];	// 密钥校验值
	TUnionDesKeyLength		length;			// 密钥强度
	//TUnionDesKeyProtectMethod	protectMethod;		// 密钥保护方法
	TUnionDesKeyType		type;			// 密钥类型
	char				activeDate[8+1];		// 密钥有效日期
	char				passiveDate[8+1];		// 密钥失效日期
} TUnionDesKey;
#endif
typedef TUnionDesKey			*PUnionDesKey;

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
	TUnionDesKeyLength	length;
	TUnionDesKeyType	type;
	char			activeDate[8+1];
} TUnionDesKeyQueryCondition;
typedef TUnionDesKeyQueryCondition	*PUnionDesKeyQueryCondition;

// add by wuhy at 20130820
typedef struct
{
	char                            fullName[40+1];         // 密钥全名，采用owner.keyApp.keyName
	int                             version;                // 版本号
	char                            value[48+1];            // 密钥密文
	char                            checkValue[16+1];       // 密钥校验值
	TUnionDesKeyLength              length;                 // 密钥强度
	int                             keyTransmitProtectMode; // 密钥传输保护方法
	TUnionDesKeyType                type;                   // 密钥用途
	int                             AlgorithmType;          // 密钥算法类型
	char                            keyDBID[20+1];          // 档案库标识
	char                            activeDate[8+1];        // 密钥有效日期
	long                            maxEffectiveDays;       // 密钥最大有效日期
	long                            maxUseTimes;            // 密钥最多使用的次数
	long                            useTimes;               // 使用次数
	long                            lastUpdateTime;         // 最近一次更新时间
	char                            container[20+1];        //

	short                           oldVerEffective;        // 旧版本是否生效
	char                            oldValue[48+1];         // 旧版本的密钥值
	char                            oldCheckValue[16+1];    // 旧版本的密钥校验值
	int                             windowBetweenKeyVer;    // 两个密钥版本间的窗口期
	short                           isWritingLocked;        // 是否被写锁住了
}TUnionSymmetricKey;
typedef TUnionSymmetricKey		*PUnionSymmetricKey;
// end add


// 2004/11/25 Wolfgang Wang
int UnionUseDesKey(PUnionDesKey pkey);
int UnionUpdateDesKeyValue(PUnionDesKey pkey,char *value,char *checkValue);
int UnionIsOldVerDesKeyEffective(PUnionDesKey pkey);
// End of addition of 2004/11/25

// 创建一个密钥对象
int UnionCreateDesKey(char *owner,char *keyApp,char *keyName,PUnionDesKey pDesKey);
// 读取一个密钥对象
int UnionReadDesKey(char *owner,char *keyApp,char *keyName,PUnionDesKey pDesKey);
// 删除一个密钥对象
int UnionDeleteDesKey(char *owner,char *keyApp,char *keyName);
// 更改一个密钥对象的属性
int UnionUpdateDesKeyAttr(char *owner,char *keyApp,char *keyName,TUnionDesKeyAttribute attr,char *attrValue);

// 判断是否是合法的密钥长度
int UnionIsValidDesKeyLength(TUnionDesKeyLength desKeyLen);
// 判断是否是合法的密钥保护方式
int UnionIsValidDesKeyProtectMethod(TUnionDesKeyProtectMethod method);
// 判断是否是合法的密钥属性
int UnionIsValidDesKeyAttribute(TUnionDesKeyAttribute attr);
// 判断是否是合法的密钥密文
int UnionIsValidDesKeyCryptogram(char *keyCryptogram);
// 判断是否是合法的密钥校验值
int UnionIsValidDesKeyCheckValue(char *checkValue);
// 判断是否是合法的密钥全称
int UnionIsValidDesKeyFullName(char *fullName);
// 判断是否是合法的密钥类型
int UnionIsValidDesKeyType(TUnionDesKeyType type);

int UnionPrintDesKey(PUnionDesKey pDesKey);
int UnionIsValidDesKey(PUnionDesKey pDesKey);

int UnionIsDesKeyStillEffective(PUnionDesKey pDesKey);

int UnionFormDefaultDesKey(PUnionDesKey pkey,char *fullName,char *keyValue,TUnionDesKeyType type,char *checkValue);

int UnionGetNameOfDesKey(TUnionDesKeyType type,char *keyName);

// 从密钥名称中获取属主名称
int UnionAnalysisDesKeyFullName(char *fullName,char *appID,char *ownerName,char *keyName);
int UnionGetAppIDOutOfDesKeyFullName(char *fullName,char *appID);
int UnionGetKeyNameOutOfDesKeyFullName(char *fullName,char *keyName);
int UnionGetOwnerNameOutOfDesKeyFullName(char *fullName,char *owner);

TUnionDesKeyLength UnionConvertDesKeyLength(int keyLen);
TUnionDesKeyType UnionConvertDesKeyType(char *keyTypeName);

TUnionSJL06LMKPairIndex UnionGetProtectLMKPairOfDesKeyType(TUnionDesKeyType type);

int UnionGetNameOfDesKeyType(TUnionDesKeyType type,char *nameOfType);

int UnionGetDesKeyBitsLength(TUnionDesKeyLength length);

// 判断某一密钥是否属于动态更新的密钥
int UnionIsDesKeyValueDynamicUpdated(PUnionDesKey pkey);

// 2008/7/18增加
// 将一个des密钥写入到串中
int UnionPutDesKeyIntoRecStr(PUnionDesKey pDesKey,char *recStr,int sizeOfRecStr);

// 2008/7/17增加
// 将DES记录写入到文件中
int UnionPrintDesKeyToRecStrTxtFile(PUnionDesKey pDesKey,FILE *fp);

// 2008/7/18增加
// 从一个串中读密钥
int UnionReadDesKeyFromRecStr(char *recStr,int lenOfRecStr,PUnionDesKey pDesKey);

#endif
