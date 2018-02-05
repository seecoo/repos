// Author:	Wolfgang Wang
// Date:	2006/8/9

// ����ļ�������ESSC���ĸ�ʽ

#ifndef _esscKmsPackage_
#define _esscKmsPackage_

#include "esscFldTagDef.h"
#include "remoteKeyPlatform.h"

// ��ʼ��Զ�̲���������
int UnionInitRemoteRequestPackage(PUnionRemoteKeyPlatform pkeyPlatform);

// ����һ����ֵ
int UnionSetRequestRemotePackageFldValue(char *fldName,char *value);

// ��ȡһ����ֵ
int UnionReadRequestRemotePackageFldValue(char *fldName,char *value,int sizeofValue);

// ���ͱ��ĵ�Զ��ƽ̨
int UnionTransferPackageToRemotePlatform(PUnionRemoteKeyPlatform pkeyPlatform);

// ����һ����ֵ
int UnionSetRequestRemotePackageHead(char *fldName,char *value);

// ����һ�������
int UnionUnpackMngSvrRequestPackage(char *reqStr,int lenOfReqStr,char *tellerNo,char *resID,char *resCmd,char *resStr,int sizeofResStr);

// ���һ�������
int UnionPackMngSvrRequestPackage(char *tellerNo,int resID,int resCmd,int lenOfData,char *data,char *buf,int sizeofBuf);

// �����ļ���Զ��ƽ̨
int UnionTransferFileDataToRemote(int sckHDL,char *fileName,char *tellerNo,int resID);

// ����һ����ʱ�ļ�
int UnionGenerateTempFile(char *fileFullName,int sizeofFileFullName,int tmpFileIndex);

// ����һ����Ӧ��
int UnionUnpackMngSvrResponsePackage(char *reqStr,int lenOfReqStr,char *tellerNo,char *resID,char *resCmd,char *resStr,int sizeofResStr);

// ת�����ĵ�Զ��ƽ̨
int UnionSendPackageToRemotePlatform(char *ipAddr,int port,int timeout,unsigned char *buf,int lenOfBuf);

char *UnionGetTellerNo();
int UnionGetResID();
#endif
