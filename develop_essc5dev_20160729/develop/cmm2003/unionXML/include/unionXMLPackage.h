#ifndef __UnionXMLPackage_H__
#define __UnionXMLPackage_H__

#include "unionPackage.h"

#define defUnionXMLRootName	"union"
#define defUnionXMLEncoding	"GBK"

#define XML_PACK_SIZE		32

typedef enum
{
	PACKAGE_TYPE_XML,
	PACKAGE_TYPE_V001
}TUnionPackageType;

typedef struct
{
	char	serviceCode[XML_PACK_SIZE];
	int	isUI;
	char	sysID[XML_PACK_SIZE];
	char	appID[XML_PACK_SIZE];
	char	userID[XML_PACK_SIZE];
	char	clientIPAddr[XML_PACK_SIZE*2];
	char	transTime[XML_PACK_SIZE];
	char	userInfo[XML_PACK_SIZE*4];
}TUnionXMLPackageHead;
typedef TUnionXMLPackageHead		*PUnionXMLPackageHead;

void UnionSetPackageType(TUnionPackageType packageType);

TUnionPackageType UnionGetPackageType();

// 初始化XML包
int UnionInitXMLPackage(char *fileName,char *xmlBuf,int lenOfBuf);

// 初始化请求XML包
int UnionInitRequestXMLPackage(char *fileName,char *xmlBuf,int lenOfBuf);

// 初始化响应XML包
int UnionInitResponseXMLPackage(char *fileName,char *xmlBuf,int lenOfBuf);

// 初始化远程请求XML包
int UnionInitRequestRemoteXMLPackage(char *fileName,char *xmlBuf,int lenOfBuf);

// 初始化远程响应XML包
int UnionInitResponseRemoteXMLPackage(char *fileName,char *xmlBuf,int lenOfBuf);

// XML包写入文件
int UnionXMLPackageToFile(char *fileName);

// 请求XML包写入文件
int UnionRequestXMLPackageToFile(char *fileName);

// 响应XML包写入文件
int UnionResponseXMLPackageToFile(char *fileName);

// XML包写入BUF
int UnionXMLPackageToBuf(char *buf,int sizeOfBuf);

// 请求XML包写入BUF
int UnionRequestXMLPackageToBuf(char *buf,int sizeOfBuf);

// 响应XML包写入BUF
int UnionResponseXMLPackageToBuf(char *buf,int sizeOfBuf);

// 远程请求XML包写入BUF
int UnionRequestRemoteXMLPackageToBuf(char *buf,int sizeOfBuf);

// 远程响应XML包写入BUF
int UnionResponseRemoteXMLPackageToBuf(char *buf,int sizeOfBuf);

// 释放XML包
void UnionFreeXMLPackage();

// 释放请求XML包
void UnionFreeRequestXMLPackage();

// 释放响应XML包
void UnionFreeResponseXMLPackage();

// 释放远程请求XML包
void UnionFreeRequestRemoteXMLPackage();

// 释放远程响应XML包
void UnionFreeResponseRemoteXMLPackage();

// 重置XML包
void UnionResetXMLPackage();

// 重置请求XML包
void UnionResetRequestXMLPackage();

// 重置响应XML包
void UnionResetResponseXMLPackage();

// 重置远程请求XML包
void UnionResetRequestRemoteXMLPackage();

// 重置远程响应XML包
void UnionResetResponseRemoteXMLPackage();

// 打印日志
int UnionLogXMLPackage();

// 打印请求日志
int UnionLogRequestXMLPackage();

// 打印响应日志
int UnionLogResponseXMLPackage();

// 打印远程请求日志
int UnionLogRequestRemoteXMLPackage();

// 打印远程响应日志
int UnionLogResponseRemoteXMLPackage();

// 定位节点
int UnionLocateXMLPackage(char *nodeName,int id);

// 定位请求包
int UnionLocateRequestXMLPackage(char *nodeName,int id);

// 定位响应包
int UnionLocateResponseXMLPackage(char *nodeName,int id);

// 定位远程请求包
int UnionLocateRequestRemoteXMLPackage(char *nodeName,int id);

// 定位远程应包
int UnionLocateResponseRemoteXMLPackage(char *nodeName,int id);

// 增加新节点并定位
int UnionLocateNewXMLPackage(char *nodeName,int id);

// 请求包增加新节点并定位
int UnionLocateRequsetNewXMLPackage(char *nodeName,int id);

// 响应包增加新节点并定位
int UnionLocateResponseNewXMLPackage(char *nodeName,int id);

// 读取字段值
int UnionReadXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// 读取请求报文字段值
int UnionReadRequestXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// 读取响应报文字段值
int UnionReadResponseXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// 读取远程请求报文字段值
int UnionReadRequestRemoteXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// 读取远程响应报文字段值
int UnionReadResponseRemoteXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// 读取节点下所有字段值
int UnionSelectXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// 读取请求报文节点下所有字段值
int UnionSelectRequestXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// 读取响应报文节点下所有字段值
int UnionSelectResponseXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// 读取远程请求报文节点下所有字段值
int UnionSelectRequestRemoteXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// 读取远程响应报文节点下所有字段值
int UnionSelectResponseRemoteXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// 设置字段值
int UnionSetXMLPackageValue(char *nodeName,char *value);

// 设置请求报文字段值
int UnionSetRequestXMLPackageValue(char *nodeName,char *value);

// 设置请求报文字段值
int UnionSetRequestXMLPackageBitValue(char *nodeName,int lenOfValue,char *value);

// 设置响应报文字段值
int UnionSetResponseXMLPackageValue(char *nodeName,char *value);

// 设置远程请求报文字段值
int UnionSetRequestRemoteXMLPackageValue(char *nodeName,char *value);

// 设置远程响应报文字段值
int UnionSetResponseRemoteXMLPackageValue(char *nodeName,char *value);

// 删除节点
int UnionDeleteXMLPackageNode(char *nodeName,int id);

// 删除请求节点
int UnionDeleteRequestXMLPackageNode(char *nodeName,int id);

// 删除响应节点
int UnionDeleteResponseXMLPackageNode(char *nodeName,int id);

// 删除远程请求节点
int UnionDeleteRequestRemoteXMLPackageNode(char *nodeName,int id);

// 删除远程响应节点
int UnionDeleteResponseRemoteXMLPackageNode(char *nodeName,int id);

// 交换两个同路径节点的ID值
int UnionExchangeIDXMLPackage(char *nodeName,int id1,int id2);

// 交换两个同路径请求节点的ID值
int UnionExchangeIDRequestXMLPackage(char *nodeName,int id1,int id2);

// 交换两个同路径响应节点的ID值
int UnionExchangeIDResponseXMLPackage(char *nodeName,int id1,int id2);

// 设置响应描述
void UnionSetResponseRemark(char *fmt,...);

// 读取响应描述
int UnionGetResponseRemark(char *errRemark,int sizeofBuf);

// 初始化响应报文头
int UnionInitHeadOfResponseXMLPackage();

PUnionXMLPackageHead UnionGetXMLPackageHead();

#endif
