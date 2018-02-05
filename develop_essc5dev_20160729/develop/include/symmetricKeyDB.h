#ifndef _SymmetricKeyDB_H_
#define _SymmetricKeyDB_H_

#include "unionDesKey.h"
#include "commWithHsmSvr.h"

#define	defTableNameOfSymmetricKeyDB		"symmetricKeyDB"
#define	defTableNameOfSymmetricKeyValue		"symmetricKeyValue"
#define maxNumOfSymmetricKeyValue		10

// 定义创建者类型
typedef enum 
{
	conSymmetricCreatorTypeOfUser,
	conSymmetricCreatorTypeOfApp,
	conSymmetricCreatorTypeOfKMS,
	conSymmetricCreatorTypeOfMove,
	conSymmetricCreatorTypeOfUnknown = 99
} TUnionSymmetricCreatorType;

// 定义对称密钥算法标识
typedef enum 
{
	conSymmetricAlgorithmIDOfDES,
	conSymmetricAlgorithmIDOfSM4,
	conSymmetricAlgorithmIDOfSM1,
	conSymmetricAlgorithmIDOfAES,
	conSymmetricAlgorithmIDOfUnknown = 99
} TUnionSymmetricAlgorithmID;

// 定义对称密钥长度标识
typedef enum 
{
	con64BitsSymmetricKey = 64,
	con128BitsSymmetricKey = 128,
	con192BitsSymmetricKey = 192,
	con256BitsSymmetricKey = 256,
	conUnknownLengthSymmetricKey = 0
} TUnionSymmetricKeyLength;

// 定义对称密钥状态标识
typedef enum 
{
	conSymmetricKeyStatusOfInitial,		// 初始状态
	conSymmetricKeyStatusOfEnabled,		// 启用状态
	conSymmetricKeyStatusOfSuspend,		// 挂起状态
	conSymmetricKeyStatusOfCancel,		// 撤销状态
	conSymmetricKeyStatusOfUnknown = 99	// 未知状态
} TUnionSymmetricKeyStatus;

// 定义对称密钥类型标识
typedef TUnionDesKeyType	TUnionSymmetricKeyType;

// 定义对称密钥容器
typedef struct
{
	char				lmkProtectMode[16+1];		// LMK保护方式
	char				keyValue[48+1];			// 密钥密文
	char				oldKeyValue[48+1];		// 旧版本的密钥值
} TUnionSymmetricKeyValue;
typedef TUnionSymmetricKeyValue		*PUnionSymmetricKeyValue;

// 定义对称密钥容器
typedef struct
{
	char				keyName[128+1];			// 密钥名称
	char				keyGroup[16+1];			// 密钥组
	TUnionSymmetricAlgorithmID	algorithmID;			// 算法标识
	TUnionSymmetricKeyType		keyType;			// 密钥类型
	TUnionSymmetricKeyLength	keyLen;				// 密钥长度
	int				inputFlag;			// 允许导入
	int				outputFlag;			// 允许导出
	int				effectiveDays;			// 有效天数
	TUnionSymmetricKeyStatus	status;				// 状态
	int				oldVersionKeyIsUsed;		// 允许使用旧密钥
	char				activeDate[8+1];		// 生效日期
	TUnionSymmetricKeyValue		keyValue[maxNumOfSymmetricKeyValue];	// 密钥值
	char				checkValue[16+1];		// 密钥校验值
	char				oldCheckValue[16+1];		// 旧版本的密钥校验值
	char				keyUpdateTime[14+1];		// 最近更新时间
	char				keyApplyPlatform[16+1];		// 密钥申请平台
	char				keyDistributePlatform[16+1];	// 密钥分发平台
	TUnionSymmetricCreatorType	creatorType;			// 创建者类型			
	char				creator[40+1];			// 创建者
	char				createTime[14+1];		// 创建时间
	char				usingUnit[16+1];		// 使用单位
	char				remark[128+1];			// 备注
} TUnionSymmetricKeyDB;
typedef TUnionSymmetricKeyDB		*PUnionSymmetricKeyDB;

typedef struct
{
	char				keyName[128+1];			// 密钥名称
	char				keyGroup[16+1];			// 密钥组
	char				algorithmID[16];		// 算法标识
	char				keyType[16];			// 密钥类型
	char				keyLen[16];			// 密钥长度
	char				inputFlag[16];			// 允许导入
	char				outputFlag[16];			// 允许导出
	char				effectiveDays[16];		// 有效天数
	char				status[16];			// 状态
	char				oldVersionKeyIsUsed[16];	// 允许使用旧密钥
	char				activeDate[8+1];		// 生效日期
	TUnionSymmetricKeyValue		keyValue[maxNumOfSymmetricKeyValue];	// 密钥值
	char				checkValue[16+1];		// 密钥校验值
	char				oldCheckValue[16+1];		// 旧版本的密钥校验值
	char				keyUpdateTime[14+1];		// 最近更新时间
	char				keyApplyPlatform[16+1];		// 密钥申请平台
	char				keyDistributePlatform[16+1];	// 密钥分发平台
	char				creatorType[16];		// 创建者类型			
	char				creator[40+1];			// 创建者
	char				createTime[14+1];		// 创建时间
	char				usingUnit[16+1];		// 使用单位
	char				remark[128+1];			// 备注
} TUnionSymmetricKeyDBStr;
typedef TUnionSymmetricKeyDBStr		*PUnionSymmetricKeyDBStr;

/* 
功能：	转换对称密钥名称
参数：	keyName[in|out]		密钥名称
	sizeofBuf[in]		缓冲大小
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionTransformSymmetricKeyName(char *keyName,int sizeofBuf);

/* 
功能：	获取对称密钥库在高速缓存中键名
参数：	keyName[in]		密钥名称
	highCachedKey[out]	键名
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionGetKeyOfHighCachedForSymmetricKeyDB(char *keyName,char *highCachedKey);

// 读取LMK保护方式表的保护密钥
int UnionReadProtectKeyFormLmkProtectMode(TUnionSymmetricAlgorithmID algorithmID,char *lmkProtectMode,char *keyValue,int sizeOfKey,char *checkValue,int sizeOfCV);

// 获取对称密钥值
PUnionSymmetricKeyValue UnionGetSymmetricKeyValue(PUnionSymmetricKeyDB psymmetricKeyDB,char *lmkProtectMode);

// 判断旧密钥是否允许使用
int UnionOldVersionSymmetricKeyIsUsed(PUnionSymmetricKeyDB psymmetricKeyDB);

// 把密钥类型名称转为内部枚举值
TUnionSymmetricKeyType UnionConvertSymmetricKeyKeyType(char *keyTypeName);

// 把密钥类型内部枚举转换为外部可识别
char *UnionConvertSymmetricKeyKeyTypeToExternal(TUnionSymmetricKeyType keyType);

// 判断是否是合法的对称密钥类型
int UnionIsValidSymmetricKeyType(TUnionSymmetricKeyType keyType);

// 判断是否是合法的对称密钥长度
int UnionIsValidSymmetricKeyLength(TUnionSymmetricKeyLength keyLen);

// 把对称密钥长度转为DES密钥长度
TUnionDesKeyLength UnionConvertSymmetricKeyKeyLen(TUnionSymmetricKeyLength keyLen);

// 把算法标识名称转为内部枚举值
TUnionSymmetricAlgorithmID UnionConvertSymmetricKeyAlgorithmID(char *algorithmID);

// 把算法内部枚举值称转为标识名
int UnionConvertSymmetricKeyStringAlgorithmID(TUnionSymmetricAlgorithmID algorithmID,char *strAlgorithmID);

// 判断是否是合法的对称密钥算法
int UnionIsValidSymmetricKeyAlgorithmID(TUnionSymmetricAlgorithmID algorithmID);

// 判断是否是合法的对称密钥名称
int UnionIsValidSymmetricKeyName(char *keyName);

// 从密钥名称中获取节点名称
int UnionAnalysisSymmetricKeyName(char *keyName,char *appID,char *keyNode,char *keyType);

// 判断是否是合法的对称密钥
int UnionIsValidSymmetricKeyDB(PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
功能：	创建一个对称密钥容器
参数：	psymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionCreateSymmetricKeyDB(PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
功能：	创建一个对称密钥容器 sql不同步
参数：	psymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionCreateSymmetricKeyDBWithoutSqlSyn(PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
功能：	删除一个对称密钥容器
参数：	psymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionDropSymmetricKeyDB(PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
功能：	把对称密钥数据结构转换成字符串
参数：	psymmetricKeyDB[in]	密钥容器信息
	keyDBStr[out]		密钥字符串
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionSymmetricKeyDBStructToString(PUnionSymmetricKeyDB psymmetricKeyDB,char *keyDBStr);

/* 
功能：	把对称密钥字符串转换成数据结构
参数：	keyDBStr[in]		密钥字符串
	psymmetricKeyDB[out]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionSymmetricKeyDBStringToStruct(char *keyDBStr,PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
功能：	读取一个对称密钥容器
参数：	keyName[in]		密钥名称
	isCheckKey[in]		是否检查密钥的可用性，1检查，0不检查		
	psymmetricKeyDB[out]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionReadSymmetricKeyDBRec(char *keyName,int isCheckKey,PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
功能：	生成对称密钥
参数：	psymmetricKeyDB[in|out]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionGenerateSymmetricKeyDBRec(PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
功能：	更新对称密钥
参数：	psymmetricKeyDB[in|out]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionUpdateSymmetricKeyDBKeyValue(PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
功能：	更新对称密钥	sql不同步
参数：	psymmetricKeyDB[in|out]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionUpdateSymmetricKeyDBKeyValueWithoutSqlSyn(PUnionSymmetricKeyDB psymmetricKeyDB);


int UnionAnalysisPBOCSymmetricKeyName(char *keyName, int version, char *appID, char *keyNode, char *keyType);

/* 
功能：	检查对称密钥状态
参数：	psymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionCheckSymmetricKeyStatus(PUnionSymmetricKeyDB psymmetricKeyDB);

/*
功能：	检查对称密钥使用权限
参数：	psymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionCheckSymmetricKeyPrivilege(PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
功能：	拼接对称密钥的创建者
参数：	sysID[in]	系统ID
	appID[in]	应用ID
	creator[out]	创建者
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionPieceSymmetricKeyCreator(char *sysID,char *appID,char *creator);

/* 
功能：	更新一个对称密钥容器状态
参数：	psymmetricKeyDB[in]	密钥容器信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionUpdateSymmetricKeyDBStatus(PUnionSymmetricKeyDB psymmetricKeyDB);

/*
功能：	打印密钥
参数：	phsmGroupRec:		对称密钥容器
	psymmetricKeyDB：	密钥信息
	keyValuePrintFormat： 	打印格式
	isCheckAlonePrint：	是否独立打印校验值 
	numOfComponent： 	分量数量
	hsmIP： 		加密机IP
返回：
	psymmetricKeyDB:	密钥值和校验值
*/
int UnionGenerateAndPrintSymmetricKey(PUnionHsmGroupRec phsmGroupRec,PUnionSymmetricKeyDB psymmetricKeyDB,char *keyValuePrintFormat,int isCheckAlonePrint,int numOfComponent,char *hsmIP,int appendPrintNum,char appendPrintParam[][80]);

int UnionSymmetricKeyDBStructToStringStruct(PUnionSymmetricKeyDBStr psymmetricKeyDBStr, PUnionSymmetricKeyDB psymmetricKeyDB);
int UnionSymmetricKeyDBStringSturctToStruct(PUnionSymmetricKeyDB psymmetricKeyDB, PUnionSymmetricKeyDBStr psymmetricKeyDBStr);
int UnionIsSymmetricKeyDBRecInCachedOnStructFormat();

// 把密钥长度转换为内部枚举值
TUnionSymmetricKeyLength UnionConvertSymmetricKeyDBKeyLen(int keyLen);

// 初始化密钥结构体
void UnionInitSymmetricKeyDB(PUnionSymmetricKeyDB psymmetricKeyDB);

// 检查密钥是否可以使用
/*
功能:	检查密钥是否可以使用，主要用于更新密钥和导入密钥检查
参数:	status:		密钥状态
	flag:		标识,
			flag = 0,表示外部app应用
			flag = 1,表示内部app应用，内部使用密钥无需检查状态
返回:	
	>=0		成功
	<0		失败,返回错误码
*/
int UnionIsUseCheckSymmetricKeyDB(TUnionSymmetricKeyStatus status,int flag);
#endif

