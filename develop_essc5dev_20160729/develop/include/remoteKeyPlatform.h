#ifndef _RemoteKeyPlatform_H_
#define _RemoteKeyPlatform_H_

#include "symmetricKeyDB.h"

// ����Զ����Կƽ̨��Կ����
typedef enum 
{
	conRemoteKeyPlatformKeyDirectionOfApply = 1,		// ����
	conRemoteKeyPlatformKeyDirectionOfDistribute = 2	// �ַ�
} TUnionRemoteKeyPlatformKeyDirection;

// ����Զ����Կƽ̨����
typedef enum 
{
	conRemoteKeyPlatformTypeOfESSC = 1,	// ESSC
	conRemoteKeyPlatformTypeOfKMS = 2	// KMS
} TUnionRemoteKeyPlatformType;

// ����Գ���Կ����
typedef struct
{
	char					keyPlatformID[16+1];		// ��Կƽ̨ID
	char					keyPlatformName[40+1];		// ��Կƽ̨����
	int					status;				// ״̬
	TUnionSymmetricAlgorithmID		algorithmID;			// �㷨��ʶ
	TUnionRemoteKeyPlatformKeyDirection	keyDirection;			// ��Կ����
	TUnionRemoteKeyPlatformType		type;				// ����
	char					protectKey[48+1];		// ������Կ
	char					checkValue[16+1];		// ����ֵ
	char					ipAddr[40+1];			// IP��ַ
	int					port;				// �˿�
	int					timeout;			// ��ʱʱ��
	int					packageType;			// ��������
	char					permitSysID[16+1];		// ϵͳID���
	char					permitAppID[16+1];		// Ӧ��ID���
} TUnionRemoteKeyPlatform;
typedef TUnionRemoteKeyPlatform			*PUnionRemoteKeyPlatform;

/* 
���ܣ�	��ȡһ��Զ����Կƽ̨��¼
������	keyPlatformID[in]	��Կƽ̨ID
	keyDirection[in]	��Կ����
	premoteKeyPlatform[out]	Զ����Կƽ̨��Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionReadRemoteKeyPlatformRec(char *keyPlatformID,TUnionRemoteKeyPlatformKeyDirection keyDirection,PUnionRemoteKeyPlatform premoteKeyPlatform);

/* 
���ܣ�	��ʼ��Զ����Կ����������
������	premoteKeyPlatform[in]	Զ����Կƽ̨��Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionInitRemoteRequestPackageToRemoteKeyPlatform(PUnionRemoteKeyPlatform premoteKeyPlatform);

/* 
���ܣ�	ת����Կ������Զ��ƽ̨
������	premoteKeyPlatform[in]	Զ����Կƽ̨��Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionTransferKeyOperateToRemoteKeyPlatform(PUnionRemoteKeyPlatform premoteKeyPlatform);

// ��Զ����Կ����
int UnionIsRemoteKeyOperate();

// ��ȡԶ��ϵͳ��ź�Ӧ�ñ��
int UnionReadRemoteSysIDAndAppID(char *sysID,int sizeofSysID,char *appID,int sizeofAppID);

// ��ʼ��3.xԶ�������
int UnionInitEssc3RemoteRequestPackage(char *appID,char *buf,char *serviceCode);

// ��ȡ3.xԶ����Ӧ��
int UnionReadEssc3RemoteResponsePackage(char *appID,char *buf,char *serviceCode);

// ��ȡ��ֵ
int UnionReadEssc3PackageValueByField(char *buf,char *value,int sizeofValue);

/* 
���ܣ�	���Զ����Կƽ̨
������	keyName[in]			��Կ����
	keyApplyPlatform[in]		��Կ����ƽ̨
	keyDistributePlatform[in]	��Կ�ַ�ƽ̨
	pkeyApplyPlatform[out]		Զ����Կ����ƽ̨��Ϣ
	pkeyDistributePlatform[out]	Զ����Կ�ַ�ƽ̨��Ϣ
	isRemoteApplyKey[out]		��Զ��������Կ
	isRemoteDistributeKey[out]	��Զ�̷ַ���Կ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionCheckRemoteKeyPlatform(char *keyName,char *keyApplyPlatform, char *keyDistributePlatform,PUnionRemoteKeyPlatform pkeyApplyPlatform,PUnionRemoteKeyPlatform pkeyDistributePlatform,int *isRemoteApplyKey,int *isRemoteDistributeKey);

#endif

