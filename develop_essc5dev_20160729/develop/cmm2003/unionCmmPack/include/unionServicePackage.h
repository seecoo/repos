// Author:	Wolfgang Wang
// Date:	2008/9/26

// 这个文件定义了ESSC报文格式

#ifndef _pkcs11ServicePackage_
#define _pkcs11ServicePackage_

#include "unionCmmPackData.h"

#define conIsRequestServicePackage	1
#define conIsResponseServicePackage	0

#define conServiceFldErrorCodeRemark	999	// 错误描述
#define conServiceFldTestingData	971	// 测试数据
#define conServiceFldProcID		970	// 进程号
#define conServiceFldThreadID		969	// 线程号
#define conServiceFldPackSSN            968     // 报文的顺序号
#define conServiceFldPackTimeStamp      967     // 时间戳
#define conServiceFldPackCliIPAddr	966	// ip地址
#define conServiceFldPackCliPort	965	// 端口

// 定义报文
typedef struct
{
	char			idOfApp[2+1];				// 应用编号
	char			serviceID[3+1];				// 服务代码
	int			direction;				// 1,请求，0,响应
	int			resCode;				// 响应码，只有响应才有
	TUnionCmmPackData	datagram;				// 正文
} TUnionServicePackage;
typedef TUnionServicePackage	*PUnionServicePackage;

// 定义请求报文
typedef TUnionServicePackage		TUnionServiceRequestPackage;
typedef TUnionServiceRequestPackage	*PUnionServiceRequestPackage;

// 定义响应报文
typedef TUnionServicePackage		TUnionServiceResponsePackage;
typedef TUnionServiceResponsePackage	*PUnionServiceResponsePackage;

// 获取服务说明
/*
输入参数
	serviceID	服务代码
输出参数
	无
返回值
	服务说明的指针
	出错，空指针
*/
char *UnionFindCmmPackServiceRemark(int serviceID);
			
// 获取域标识说明
/*
输入参数
	tag	域标识
输出参数
	无
返回值
	说明的指针
	出错，空指针
*/
char *UnionFindCmmPackFldTagRemark(int tag);

// 比较两个包的标识
/*
输入参数
	poriCmmPack	源包
	pdesCmmPack	目标包
输出参数
	无
返回值
	>= 0		一致,返回一致标识域的数量
	<0		不一致
*/
int UnionVerifyCmmPackIdentifiedTag(PUnionCmmPackData poriCmmPack,PUnionCmmPackData pdesCmmPack);

// 复制一个包的标识
/*
输入参数
	poriCmmPack	源包
输出参数
	pdesCmmPack	目标包
返回值
	>= 0		成功
	<0		出错代码
*/
int UnionCopyCmmPackIdentifiedTag(PUnionCmmPackData poriCmmPack,PUnionCmmPackData pdesCmmPack);

// 为一个包打一个标识
/*
输入参数
	无
输出参数
	ppack		包结构
返回值
	>= 0		成功
	<0		出错代码
*/
int UnionPutCmmPackIdentifiedTag(PUnionCmmPackData pcmmPack);

void UnionSetLogPackageAnyway();

void UnionCloseLogPackageAnyway();


// 设置解包的程序是监控客户端
int UnionSetPackageAsSpierClientPackage();

// 初始化报文
void UnionInitServicePackage(PUnionServicePackage ppack);

// 设置包头
int UnionSetServicePackageHeader(char *idOfApp,int serviceCode,int direction,int resCode,PUnionServicePackage ppack);

// 设置包头
int UnionSetServicePackageData(PUnionCmmPackData pdata,PUnionServicePackage ppack);

// 打一个包
/*
输入参数
	ppack		包结构
	sizeOfBuf	buf的大小
输出参数
	buf		打好的包
返回值
	>= 0		打好的包的长度
	<0		出错代码
*/
int UnionPackPackage(PUnionServicePackage ppack,char *buf,int sizeOfBuf);

// 解一个包
/*
输入参数
	data		数据
	lenOfData	数据的长度
输出参数
	ppack		解出的包
返回值
	>= 0		解出的包的有效长度
	<0		出错代码
*/
int UnionUnpackPackage(char *data,int lenOfData,PUnionServicePackage ppack);

// 初始化请求报文
int UnionInitRequestPackage(PUnionServiceRequestPackage ppack);

// 初始化响应报文
int UnionInitResponsePackage(PUnionServiceResponsePackage ppack);

// 将请求包写入日志
void UnionLogRequestPackage(PUnionServiceRequestPackage ppack);

// 将响应包写入日志
void UnionLogResponsePackage(PUnionServiceResponsePackage ppack);

// 打一个请求包
int UnionPackRequestPackage(char *idOfApp,int serviceCode,PUnionCmmPackData pdatagram,char *buf,int sizeOfBuf);

// 解一个请求包
int UnionUnpackRequestPackage(char *data,int lenOfData,char *idOfApp,int *serviceID,PUnionCmmPackData pdatagram);

// 打一个响应包
int UnionPackResponsePackage(char *idOfApp,int serviceCode,int resCode,PUnionCmmPackData pdatagram,char *buf,int sizeOfBuf);

// 解一个响应包
int UnionUnpackResponsePackage(char *data,int lenOfData,char *idOfApp,int *serviceID,int *resCode,PUnionCmmPackData pdatagram);

#endif

