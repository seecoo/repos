// Author:	Wolfgang Wang
// Date:	2006/8/9

// 这个文件定义了ESSC报文格式

#ifndef _esscKmsPackage_
#define _esscKmsPackage_

#include "esscFldTagDef.h"
#include "remoteKeyPlatform.h"

// 初始化远程操作请求报文
int UnionInitRemoteRequestPackage(PUnionRemoteKeyPlatform pkeyPlatform);

// 设置一个域值
int UnionSetRequestRemotePackageFldValue(char *fldName,char *value);

// 读取一个域值
int UnionReadRequestRemotePackageFldValue(char *fldName,char *value,int sizeofValue);

// 发送报文到远程平台
int UnionTransferPackageToRemotePlatform(PUnionRemoteKeyPlatform pkeyPlatform);

// 设置一个域值
int UnionSetRequestRemotePackageHead(char *fldName,char *value);

// 解析一个请求包
int UnionUnpackMngSvrRequestPackage(char *reqStr,int lenOfReqStr,char *tellerNo,char *resID,char *resCmd,char *resStr,int sizeofResStr);

// 打包一个请求包
int UnionPackMngSvrRequestPackage(char *tellerNo,int resID,int resCmd,int lenOfData,char *data,char *buf,int sizeofBuf);

// 传送文件到远程平台
int UnionTransferFileDataToRemote(int sckHDL,char *fileName,char *tellerNo,int resID);

// 生成一个临时文件
int UnionGenerateTempFile(char *fileFullName,int sizeofFileFullName,int tmpFileIndex);

// 解析一个响应包
int UnionUnpackMngSvrResponsePackage(char *reqStr,int lenOfReqStr,char *tellerNo,char *resID,char *resCmd,char *resStr,int sizeofResStr);

// 转发报文到远程平台
int UnionSendPackageToRemotePlatform(char *ipAddr,int port,int timeout,unsigned char *buf,int lenOfBuf);

char *UnionGetTellerNo();
int UnionGetResID();
#endif
