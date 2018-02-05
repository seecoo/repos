// Author:	Wolfgang Wang
// Date:	2006/8/9

// 这个文件定义了ESSC报文格式

#ifndef _esscEsscPackage_
#define _esscEsscPackage_

#include "esscFldTagDef.h"

// 新旧报文转换方法
#define convertMethodOfReadFixedValue			1
#define convertMethodOfReadPackageValue			2
#define convertMethodOfReadPackageValueAndExpand	3
#define convertMethodOfReadPackageValueAndCompress	4
#define convertMethodOfReadZMKNameBySameAppNoAndNode	5
#define convertMethodOfCheckFieldIsExist		6
#define convertMethodOfReadKeyNameByHsmGroupIDAndIndex	7
#define convertMethodOfReadKeyNameByAppNoAndHsmGroupID	8
#define convertMethodOfReadKeyNameByAppNo		9
#define convertMethodOfReadPackageValueByRequest	10
#define convertMethodOfReadPackageResponseDataLen	11
#define convertMethodOfReadPackageValueByAlgorithmID	12
#define convertMethodOfReadFourBytesOfPackageValue	13

// 以下定义ESSC请求报文头
typedef struct
{
	char	appID[32];		// 应用编号
	char	serviceID[32];		// 服务编号
	char	flag[32];		// 标志
	char	numOfFld[32];		// 域的个数
} TUnionEsscRequestPackageHeader;
typedef TUnionEsscRequestPackageHeader	*PUnionEsscRequestPackageHeader;

// 以下定义ESSC响应报文头
typedef struct
{
	char	appID[32];		// 应用编号
	char	serviceID[32];		// 服务编号
	char	flag[32];		// 标志
	char	responseCode[32];	// 响应码
	char	numOfFld[32];		// 域的个数
} TUnionEsscResponsePackageHeader;
typedef TUnionEsscResponsePackageHeader	*PUnionEsscResponsePackageHeader;

// 以下定义一个域的结构
typedef struct
{
	int		tag;		// 域的标识号
	int		len;		// 域的长度
	unsigned char	*value;		// 域的值
} TUnionEsscPackageFld;
typedef TUnionEsscPackageFld		*PUnionEsscPackageFld;

#define conMaxNumOfEsscPackageFld	24
typedef struct
{
	TUnionEsscPackageFld	fldGrp[conMaxNumOfEsscPackageFld];	// 域组
	int			fldNum;					// 域数量
	unsigned char		dataBuf[4096+128];			// 数据缓冲,2007/11/19,大小由修改前的2048+1，改为现在的值
	int			offset;					// 数据缓冲当前可用区域的起始位置
} TUnionEsscPackage;
typedef TUnionEsscPackage	*PUnionEsscPackage;

int UnionGetVersionOfEsscPackage();

// 获取请求包的域数目
int UnionGetMaxFldNumOfEsscRequestPackage();

// 将一个ESSC报文域打入到包中,返回打入到包中的数据的长度
int UnionPutEsscPackageFldIntoStr(char *serviceID,int index,PUnionEsscPackageFld pfld,char *buf,int sizeOfBuf);

// 从一个ESSC报文中读取一个域，返回域在包中占的长度
int UnionReadEsscPackageFldFromStr(char *serviceID,int index,char *data,int len,PUnionEsscPackageFld pfld,int sizeOfFldValue);

// 初始化请求报文
int UnionInitEsscRequestPackage();

// 设置请求报文域
int UnionSetEsscRequestPackageFld(int fldTag,int len,char *value);

// 根据域标识号，读取请求报文域
int UnionReadEsscRequestPackageFld(int fldTag,char *value,int sizeOfBuf);

// 根据域索引号，读取请求报文域
int UnionReadEsscRequestPackageFldByIndex(int index,int *fldTag,char *value,int sizeOfBuf);

// 将请求包写入日志
void UnionLogEsscRequestPackage();

// 打一个请求包
int UnionPackEsscRequestPackage(char *idOfApp,char *serviceID,char *buf,int sizeOfBuf);

// 解一个请求包
int UnionUnpackEsscRequestPackage(char *data,int lenOfData,char *idOfApp,char *serviceID);

// 获取响应包的域数目
int UnionGetMaxFldNumOfEsscResponsePackage();

// 初始化响应报文
int UnionInitEsscResponsePackage();

// 设置响应报文域
int UnionSetEsscResponsePackageFld(int fldTag,int len,char *value);

// 读取响应报文域
int UnionReadEsscResponsePackageFld(int fldTag,char *value,int sizeOfBuf);

// 根据域索引号，读取响应报文域
int UnionReadEsscResponsePackageFldByIndex(int index,int *fldTag,char *value,int sizeOfBuf);

// 将响应包写入日志
void UnionLogEsscResponsePackage();

// 打一个响应包
int UnionPackEsscResponsePackage(char *idOfApp,char *serviceID,int responseCode,char *buf,int sizeOfBuf);

// 解一个响应包
int UnionUnpackEsscResponsePackage(char *data,int lenOfData,char *idOfApp,char *serviceID);

// 设置解包的程序是监控客户端
int UnionSetEsscPackageAsSpierClientPackage();

// 打一个数据域包
int UnionPackEsscDataFldPackage(char *buf,int sizeOfBuf);

// 解一个请求包
int UnionUnpackEsscDataFldPackage(char *data,int lenOfData);

void UnionSetLogEsscPackageAnyway();

void UnionCloseLogEsscPackageAnyway();

// 将响应包写入日志
void UnionLogEsscResponsePackageAnyway(char *idOfApp,char *serviceID,int responseCode);

// 将请求包写入日志
void UnionLogEsscRequestPackageAnyway(char *idOfApp,char *serviceID);

// 根据请求标识检测报文值是否需要打印
int UnionIsPrintByRequestFlag(char *oldServiceCode,int version,int parameter);

// 根据服务码和应用编号确定密钥名称
int UnionSetConvertPackageKeyName(char *serviceID,char *appNo);
int UnionIsCheckUnionPayPackage();

//根据服务码和应用编号设置平台3KMSvr端口的报文对应的服务码和报文体
int UnionSetKMSvrServiceCode(char *appID, char *serviceCode, char *currServiceCode);
int UnionSetKMSvrRequestPackage(char *serviceCode);
#endif
