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

// ��ʼ��XML��
int UnionInitXMLPackage(char *fileName,char *xmlBuf,int lenOfBuf);

// ��ʼ������XML��
int UnionInitRequestXMLPackage(char *fileName,char *xmlBuf,int lenOfBuf);

// ��ʼ����ӦXML��
int UnionInitResponseXMLPackage(char *fileName,char *xmlBuf,int lenOfBuf);

// ��ʼ��Զ������XML��
int UnionInitRequestRemoteXMLPackage(char *fileName,char *xmlBuf,int lenOfBuf);

// ��ʼ��Զ����ӦXML��
int UnionInitResponseRemoteXMLPackage(char *fileName,char *xmlBuf,int lenOfBuf);

// XML��д���ļ�
int UnionXMLPackageToFile(char *fileName);

// ����XML��д���ļ�
int UnionRequestXMLPackageToFile(char *fileName);

// ��ӦXML��д���ļ�
int UnionResponseXMLPackageToFile(char *fileName);

// XML��д��BUF
int UnionXMLPackageToBuf(char *buf,int sizeOfBuf);

// ����XML��д��BUF
int UnionRequestXMLPackageToBuf(char *buf,int sizeOfBuf);

// ��ӦXML��д��BUF
int UnionResponseXMLPackageToBuf(char *buf,int sizeOfBuf);

// Զ������XML��д��BUF
int UnionRequestRemoteXMLPackageToBuf(char *buf,int sizeOfBuf);

// Զ����ӦXML��д��BUF
int UnionResponseRemoteXMLPackageToBuf(char *buf,int sizeOfBuf);

// �ͷ�XML��
void UnionFreeXMLPackage();

// �ͷ�����XML��
void UnionFreeRequestXMLPackage();

// �ͷ���ӦXML��
void UnionFreeResponseXMLPackage();

// �ͷ�Զ������XML��
void UnionFreeRequestRemoteXMLPackage();

// �ͷ�Զ����ӦXML��
void UnionFreeResponseRemoteXMLPackage();

// ����XML��
void UnionResetXMLPackage();

// ��������XML��
void UnionResetRequestXMLPackage();

// ������ӦXML��
void UnionResetResponseXMLPackage();

// ����Զ������XML��
void UnionResetRequestRemoteXMLPackage();

// ����Զ����ӦXML��
void UnionResetResponseRemoteXMLPackage();

// ��ӡ��־
int UnionLogXMLPackage();

// ��ӡ������־
int UnionLogRequestXMLPackage();

// ��ӡ��Ӧ��־
int UnionLogResponseXMLPackage();

// ��ӡԶ��������־
int UnionLogRequestRemoteXMLPackage();

// ��ӡԶ����Ӧ��־
int UnionLogResponseRemoteXMLPackage();

// ��λ�ڵ�
int UnionLocateXMLPackage(char *nodeName,int id);

// ��λ�����
int UnionLocateRequestXMLPackage(char *nodeName,int id);

// ��λ��Ӧ��
int UnionLocateResponseXMLPackage(char *nodeName,int id);

// ��λԶ�������
int UnionLocateRequestRemoteXMLPackage(char *nodeName,int id);

// ��λԶ��Ӧ��
int UnionLocateResponseRemoteXMLPackage(char *nodeName,int id);

// �����½ڵ㲢��λ
int UnionLocateNewXMLPackage(char *nodeName,int id);

// ����������½ڵ㲢��λ
int UnionLocateRequsetNewXMLPackage(char *nodeName,int id);

// ��Ӧ�������½ڵ㲢��λ
int UnionLocateResponseNewXMLPackage(char *nodeName,int id);

// ��ȡ�ֶ�ֵ
int UnionReadXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// ��ȡ�������ֶ�ֵ
int UnionReadRequestXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// ��ȡ��Ӧ�����ֶ�ֵ
int UnionReadResponseXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// ��ȡԶ���������ֶ�ֵ
int UnionReadRequestRemoteXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// ��ȡԶ����Ӧ�����ֶ�ֵ
int UnionReadResponseRemoteXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// ��ȡ�ڵ��������ֶ�ֵ
int UnionSelectXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// ��ȡ�����Ľڵ��������ֶ�ֵ
int UnionSelectRequestXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// ��ȡ��Ӧ���Ľڵ��������ֶ�ֵ
int UnionSelectResponseXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// ��ȡԶ�������Ľڵ��������ֶ�ֵ
int UnionSelectRequestRemoteXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// ��ȡԶ����Ӧ���Ľڵ��������ֶ�ֵ
int UnionSelectResponseRemoteXMLPackageValue(char *nodeName,char *value,int sizeOfBuf);

// �����ֶ�ֵ
int UnionSetXMLPackageValue(char *nodeName,char *value);

// �����������ֶ�ֵ
int UnionSetRequestXMLPackageValue(char *nodeName,char *value);

// �����������ֶ�ֵ
int UnionSetRequestXMLPackageBitValue(char *nodeName,int lenOfValue,char *value);

// ������Ӧ�����ֶ�ֵ
int UnionSetResponseXMLPackageValue(char *nodeName,char *value);

// ����Զ���������ֶ�ֵ
int UnionSetRequestRemoteXMLPackageValue(char *nodeName,char *value);

// ����Զ����Ӧ�����ֶ�ֵ
int UnionSetResponseRemoteXMLPackageValue(char *nodeName,char *value);

// ɾ���ڵ�
int UnionDeleteXMLPackageNode(char *nodeName,int id);

// ɾ������ڵ�
int UnionDeleteRequestXMLPackageNode(char *nodeName,int id);

// ɾ����Ӧ�ڵ�
int UnionDeleteResponseXMLPackageNode(char *nodeName,int id);

// ɾ��Զ������ڵ�
int UnionDeleteRequestRemoteXMLPackageNode(char *nodeName,int id);

// ɾ��Զ����Ӧ�ڵ�
int UnionDeleteResponseRemoteXMLPackageNode(char *nodeName,int id);

// ��������ͬ·���ڵ��IDֵ
int UnionExchangeIDXMLPackage(char *nodeName,int id1,int id2);

// ��������ͬ·������ڵ��IDֵ
int UnionExchangeIDRequestXMLPackage(char *nodeName,int id1,int id2);

// ��������ͬ·����Ӧ�ڵ��IDֵ
int UnionExchangeIDResponseXMLPackage(char *nodeName,int id1,int id2);

// ������Ӧ����
void UnionSetResponseRemark(char *fmt,...);

// ��ȡ��Ӧ����
int UnionGetResponseRemark(char *errRemark,int sizeofBuf);

// ��ʼ����Ӧ����ͷ
int UnionInitHeadOfResponseXMLPackage();

PUnionXMLPackageHead UnionGetXMLPackageHead();

#endif
