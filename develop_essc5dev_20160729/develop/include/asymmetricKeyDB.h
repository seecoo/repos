#ifndef _AsymmetricKeyDB_H_
#define _AsymmetricKeyDB_H_

#define	defTableNameOfAsymmetricKeyDB	"asymmetricKeyDB"

// 定义创建者类型
typedef enum 
{
	conAsymmetricCreatorTypeOfUser,
	conAsymmetricCreatorTypeOfApp,
	conAsymmetricCreatorTypeOfKMS,
	conAsymmetricCreatorTypeOfUnknown = 99
} TUnionAsymmetricCreatorType;

// 定义非对称密钥算法标识
typedef enum 
{
	conAsymmetricAlgorithmIDOfRSA,
	conAsymmetricAlgorithmIDOfSM2,
	conAsymmetricAlgorithmIDOfUnknown = 99
} TUnionAsymmetricAlgorithmID;

// 定义非对称密钥密钥长度
typedef enum
{
	con256BitsAsymmetricKey = 256,
	con512BitsAsymmetricKey = 512,
	con1024BitsAsymmetricKey = 1024,
	con1152BitsAsymmetricKey = 1152,
	con1408BitsAsymmetricKey = 1408,
	con1984BitsAsymmetricKey = 1984,
	con2048BitsAsymmetricKey = 2048,
	conUnknownLengthAsymmetricKey = 0
} TUnionAsymmetricKeyLength;

// 定义非对称密钥状态标识
typedef enum 
{
	conAsymmetricKeyStatusOfInitial,	// 初始状态
	conAsymmetricKeyStatusOfEnabled,	// 启用状态
	conAsymmetricKeyStatusOfSuspend,	// 挂起状态
	conAsymmetricKeyStatusOfCancel,		// 撤销状态
	conAsymmetricKeyStatusOfUnknown = 99	// 未知状态
} TUnionAsymmetricKeyStatus;

// 定义非对称密钥类型标识
typedef enum
{
	conAsymmetricKeyTypeOfSignature = 0,			// 签名
	conAsymmetricKeyTypeOfEncryption = 1,			// 加密
	conAsymmetricKeyTypeOfSignatureAndEncryption = 2,	// 签名和加密
	conAsymmetricKeyTypeOfConsult = 3,			// 密钥协商
	conAsymmetricKeyTypeOfSignatureAndEncryptionAndKeyMng = 4,	// 签名和加密，密钥管理
	conAsymmetricKeyTypeOfUnknownPKType = 99
} TUnionAsymmetricKeyType;

// 定义非对称密钥容器
typedef struct
{
	char				keyName[128+1];			// 密钥名称
	char				keyGroup[16+1];			// 密钥组
	TUnionAsymmetricAlgorithmID	algorithmID;			// 算法标识
	TUnionAsymmetricKeyType		keyType;			// 密钥类型
	TUnionAsymmetricKeyLength	keyLen;				// 密钥长度
	int				pkExponent;			// 公钥指数，当算法标识为RSA时才有效
	int				inputFlag;			// 允许导入
	int				outputFlag;			// 允许导出
	int				effectiveDays;			// 有效天数
	TUnionAsymmetricKeyStatus	status;				// 状态
	int				vkStoreLocation;		// 私钥存储位置
	char				hsmGroupID[8+1];		// 加密机组ID
	char				vkIndex[2+1];			// 私钥索引
	int				oldVersionKeyIsUsed;		// 允许使用旧密钥
	char				pkValue[512+1];			// 公钥值
	char				pkCheckValue[16+1];		// 公钥校验值
	char				vkValue[2560+1];		// 私钥值
	char				vkCheckValue[16+1];		// 私钥校验值
	char				oldPKValue[512+1];		// 旧版本的公钥值
	char				oldPKCheckValue[16+1];		// 旧版本的公钥校验值
	char				oldVKValue[2560+1];		// 旧版本的私钥值
	char				oldVKCheckValue[16+1];		// 旧版本的私钥校验值
	char				activeDate[8+1];		// 生效日期
	char				keyUpdateTime[14+1];		// 最近更新时间
	char				keyApplyPlatform[16+1];		// 密钥申请平台
	char				keyDistributePlatform[16+1];	// 密钥分发平台
	TUnionAsymmetricCreatorType	creatorType;			// 创建者类型		
	char				creator[40+1];			// 创建者
	char				createTime[14+1];		// 创建时间
	char				usingUnit[16+1];		// 使用单位
	char				remark[128+1];			// 备注
} TUnionAsymmetricKeyDB;
typedef TUnionAsymmetricKeyDB		*PUnionAsymmetricKeyDB;

typedef struct
{
	char				keyName[128+1];			// 密钥名称
	char				keyGroup[16+1];			// 密钥组
	char				algorithmID[16];		// 算法标识
	char				keyType[16];			// 密钥类型
	char				keyLen[16];			// 密钥长度
	char				pkExponent[16];			// 公钥指数，当算法标识为RSA时才有效
	char				inputFlag[16];			// 允许导入
	char				outputFlag[16];			// 允许导出
	char				effectiveDays[16];		// 有效天数
	char				status[16];			// 状态
	char				vkStoreLocation[16];		// 私钥存储位置
	char				hsmGroupID[8+1];		// 加密机组ID
	char				vkIndex[2+1];			// 私钥索引
	char				oldVersionKeyIsUsed[16];	// 允许使用旧密钥
	char				pkValue[512+1];			// 公钥值
	char				pkCheckValue[16+1];		// 公钥校验值
	char				vkValue[2560+1];		// 私钥值
	char				vkCheckValue[16+1];		// 私钥校验值
	char				oldPKValue[512+1];		// 旧版本的公钥值
	char				oldPKCheckValue[16+1];		// 旧版本的公钥校验值
	char				oldVKValue[2560+1];		// 旧版本的私钥值
	char				oldVKCheckValue[16+1];		// 旧版本的私钥校验值
	char				activeDate[8+1];		// 生效日期
	char				keyUpdateTime[14+1];		// 最近更新时间
	char				keyApplyPlatform[16+1];		// 密钥申请平台
	char				keyDistributePlatform[16+1];	// 密钥分发平台
	char				creatorType[16];		// 创建者类型		
	char				creator[40+1];			// 创建者
	char				createTime[14+1];		// 创建时间
	char				usingUnit[16+1];		// 使用单位
	char				remark[128+1];			// 备注
} TUnionAsymmetricKeyDBStr;
typedef TUnionAsymmetricKeyDBStr	*PUnionAsymmetricKeyDBStr;

/* 
功能：	转换非对称密钥名称
参数：	keyName[in|out]		密钥名称
	sizeofBuf[in]		缓冲大小
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionTransformAsymmetricKeyName(char *keyName,int sizeofBuf);

/* 
功能：	获取非对称密钥库在高速缓存中键名
参数：	keyName[in]		密钥名称
	highCachedKey[out]	键名
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionGetKeyOfHighCachedForAsymmetricKeyDB(char *keyName,char *highCachedKey);

// 判断旧密钥是否允许使用
int UnionOldVersionAsymmetricKeyIsUsed(PUnionAsymmetricKeyDB pasymmetricKeyDB);

// 判断是否是合法的非对称密钥类型
int UnionIsValidAsymmetricKeyType(TUnionAsymmetricKeyType keyType);

// 判断是否是合法的非对称密钥长度
int UnionIsValidAsymmetricKeyLength(TUnionAsymmetricKeyLength keyLen);

// 把算法标识名称转为内部枚举值
TUnionAsymmetricAlgorithmID UnionConvertAsymmetricKeyAlgorithmID(char *algorithmID);

// 判断是否是合法的非对称密钥算法
int UnionIsValidAsymmetricKeyAlgorithmID(TUnionAsymmetricAlgorithmID algorithmID);

// 判断是否是合法的非对称密钥名称
int UnionIsValidAsymmetricKeyName(char *keyName);

// 从密钥名称中获取属主名称
int UnionAnalysisAsymmetricKeyName(char *keyName,char *appID,char *ownerName,char *keyType);

// 判断是否是合法的非对称密钥
int UnionIsValidAsymmetricKeyDB(PUnionAsymmetricKeyDB pasymmetricKeyDB);

/* 
功能：	创建一个非对称密钥容器
参数：	pasymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionCreateAsymmetricKeyDB(PUnionAsymmetricKeyDB pasymmetricKeyDB);

/* 
功能：	删除一个非对称密钥容器
参数：	pasymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionDropAsymmetricKeyDB(PUnionAsymmetricKeyDB pasymmetricKeyDB);

/* 
功能：	把非对称密钥数据结构转换成字符串
参数：	pasymmetricKeyDB[in]	密钥容器信息
	keyDBStr[out]		密钥字符串
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionAsymmetricKeyDBStructToString(PUnionAsymmetricKeyDB pasymmetricKeyDB,char *keyDBStr);

/* 
功能：	把非对称密钥字符串转换成数据结构
参数：	keyDBStr[in]		密钥字符串
	pasymmetricKeyDB[out]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionAsymmetricKeyDBStringToStruct(char *keyDBStr,PUnionAsymmetricKeyDB pasymmetricKeyDB);

/* 
功能：	读取一个非对称密钥容器
参数：	keyName[in]		密钥名称
	isCheckKey[in]		是否检查密钥的可用性，1检查，0不检查		
	pasymmetricKeyDB[out]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionReadAsymmetricKeyDBRec(char *keyName,int isCheckKey,PUnionAsymmetricKeyDB pasymmetricKeyDB);

/* 
功能：	生成非对称密钥
参数：	pasymmetricKeyDB[in|out]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionGenerateAsymmetricKeyDBRec(PUnionAsymmetricKeyDB pasymmetricKeyDB);

int UnionCreateAsymmetricKeyDBWithoutSqlSyn(PUnionAsymmetricKeyDB pasymmetricKeyDB);

/* 
功能：	更新非对称密钥
参数：	pasymmetricKeyDB[in|out]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionUpdateAsymmetricKeyDBKeyValue(PUnionAsymmetricKeyDB pasymmetricKeyDB);

int UnionUpdateAsymmetricKeyDBKeyValueWithoutSqlSyn(PUnionAsymmetricKeyDB pasymmetricKeyDB);

/* 
功能：	检查非对称密钥状态
参数：	pasymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionCheckAsymmetricKeyStatus(PUnionAsymmetricKeyDB pasymmetricKeyDB);

/*
功能：	检查非对称密钥使用权限
参数：	psymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionCheckAsymmetricKeyPrivilege(PUnionAsymmetricKeyDB pasymmetricKeyDB);

/* 
功能：	拼接非对称密钥的创建者
参数：	sysID[in]	系统ID
	appID[in]	应用ID
	creator[out]	创建者
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionPieceAsymmetricKeyCreator(char *sysID,char *appID,char *creator);

int UnionAsymmetricKeyDBStructToStringStruct(PUnionAsymmetricKeyDBStr pasymmetricKeyDBStr, const PUnionAsymmetricKeyDB pasymmetricKeyDB);
int UnionAsymmetricKeyDBStringSturctToStruct(PUnionAsymmetricKeyDB pasymmetricKeyDB, const  PUnionAsymmetricKeyDBStr pasymmetricKeyDBStr);
int UnionIsAsymmetricKeyDBRecInCachedOnStructFormat();

// 删除证书
int UnionDropCertOfCnaps2(char *keyName);

//初始化结构体
void UnionInitASymmetricKeyDB(PUnionAsymmetricKeyDB pasymmetricKeyDB);

#endif

