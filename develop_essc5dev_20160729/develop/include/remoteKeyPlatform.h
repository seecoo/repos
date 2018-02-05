#ifndef _RemoteKeyPlatform_H_
#define _RemoteKeyPlatform_H_

#include "symmetricKeyDB.h"

// 定义远程密钥平台密钥方向
typedef enum 
{
	conRemoteKeyPlatformKeyDirectionOfApply = 1,		// 申请
	conRemoteKeyPlatformKeyDirectionOfDistribute = 2	// 分发
} TUnionRemoteKeyPlatformKeyDirection;

// 定义远程密钥平台类型
typedef enum 
{
	conRemoteKeyPlatformTypeOfESSC = 1,	// ESSC
	conRemoteKeyPlatformTypeOfKMS = 2	// KMS
} TUnionRemoteKeyPlatformType;

// 定义对称密钥容器
typedef struct
{
	char					keyPlatformID[16+1];		// 密钥平台ID
	char					keyPlatformName[40+1];		// 密钥平台名称
	int					status;				// 状态
	TUnionSymmetricAlgorithmID		algorithmID;			// 算法标识
	TUnionRemoteKeyPlatformKeyDirection	keyDirection;			// 密钥方向
	TUnionRemoteKeyPlatformType		type;				// 类型
	char					protectKey[48+1];		// 保护密钥
	char					checkValue[16+1];		// 检验值
	char					ipAddr[40+1];			// IP地址
	int					port;				// 端口
	int					timeout;			// 超时时间
	int					packageType;			// 报文类型
	char					permitSysID[16+1];		// 系统ID许可
	char					permitAppID[16+1];		// 应用ID许可
} TUnionRemoteKeyPlatform;
typedef TUnionRemoteKeyPlatform			*PUnionRemoteKeyPlatform;

/* 
功能：	读取一个远程密钥平台记录
参数：	keyPlatformID[in]	密钥平台ID
	keyDirection[in]	密钥方向
	premoteKeyPlatform[out]	远程密钥平台信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionReadRemoteKeyPlatformRec(char *keyPlatformID,TUnionRemoteKeyPlatformKeyDirection keyDirection,PUnionRemoteKeyPlatform premoteKeyPlatform);

/* 
功能：	初始化远程密钥操作请求报文
参数：	premoteKeyPlatform[in]	远程密钥平台信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionInitRemoteRequestPackageToRemoteKeyPlatform(PUnionRemoteKeyPlatform premoteKeyPlatform);

/* 
功能：	转发密钥操作到远程平台
参数：	premoteKeyPlatform[in]	远程密钥平台信息
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionTransferKeyOperateToRemoteKeyPlatform(PUnionRemoteKeyPlatform premoteKeyPlatform);

// 是远程密钥操作
int UnionIsRemoteKeyOperate();

// 读取远程系统编号和应用编号
int UnionReadRemoteSysIDAndAppID(char *sysID,int sizeofSysID,char *appID,int sizeofAppID);

// 初始化3.x远程请求包
int UnionInitEssc3RemoteRequestPackage(char *appID,char *buf,char *serviceCode);

// 读取3.x远程响应包
int UnionReadEssc3RemoteResponsePackage(char *appID,char *buf,char *serviceCode);

// 读取域值
int UnionReadEssc3PackageValueByField(char *buf,char *value,int sizeofValue);

/* 
功能：	检查远程密钥平台
参数：	keyName[in]			密钥名称
	keyApplyPlatform[in]		密钥申请平台
	keyDistributePlatform[in]	密钥分发平台
	pkeyApplyPlatform[out]		远程密钥申请平台信息
	pkeyDistributePlatform[out]	远程密钥分发平台信息
	isRemoteApplyKey[out]		是远程申请密钥
	isRemoteDistributeKey[out]	是远程分发密钥
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionCheckRemoteKeyPlatform(char *keyName,char *keyApplyPlatform, char *keyDistributePlatform,PUnionRemoteKeyPlatform pkeyApplyPlatform,PUnionRemoteKeyPlatform pkeyDistributePlatform,int *isRemoteApplyKey,int *isRemoteDistributeKey);

#endif

