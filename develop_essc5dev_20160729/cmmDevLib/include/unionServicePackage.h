// Author:	Wolfgang Wang
// Date:	2008/9/26

// ����ļ�������ESSC���ĸ�ʽ

#ifndef _pkcs11ServicePackage_
#define _pkcs11ServicePackage_

#include "unionCmmPackData.h"

#define conIsRequestServicePackage	1
#define conIsResponseServicePackage	0

#define conServiceFldErrorCodeRemark	999	// ��������
#define conServiceFldTestingData	971	// ��������
#define conServiceFldProcID		970	// ���̺�
#define conServiceFldThreadID		969	// �̺߳�
#define conServiceFldPackSSN            968     // ���ĵ�˳���
#define conServiceFldPackTimeStamp      967     // ʱ���
#define conServiceFldPackCliIPAddr	966	// ip��ַ
#define conServiceFldPackCliPort	965	// �˿�

// ���屨��
typedef struct
{
	char			idOfApp[2+1];				// Ӧ�ñ��
	char			serviceID[3+1];				// �������
	int			direction;				// 1,����0,��Ӧ
	int			resCode;				// ��Ӧ�룬ֻ����Ӧ����
	TUnionCmmPackData	datagram;				// ����
} TUnionServicePackage;
typedef TUnionServicePackage	*PUnionServicePackage;

// ����������
typedef TUnionServicePackage		TUnionServiceRequestPackage;
typedef TUnionServiceRequestPackage	*PUnionServiceRequestPackage;

// ������Ӧ����
typedef TUnionServicePackage		TUnionServiceResponsePackage;
typedef TUnionServiceResponsePackage	*PUnionServiceResponsePackage;

// ��ȡ����˵��
/*
�������
	serviceID	�������
�������
	��
����ֵ
	����˵����ָ��
	������ָ��
*/
char *UnionFindCmmPackServiceRemark(int serviceID);
			
// ��ȡ���ʶ˵��
/*
�������
	tag	���ʶ
�������
	��
����ֵ
	˵����ָ��
	������ָ��
*/
char *UnionFindCmmPackFldTagRemark(int tag);

// �Ƚ��������ı�ʶ
/*
�������
	poriCmmPack	Դ��
	pdesCmmPack	Ŀ���
�������
	��
����ֵ
	>= 0		һ��,����һ�±�ʶ�������
	<0		��һ��
*/
int UnionVerifyCmmPackIdentifiedTag(PUnionCmmPackData poriCmmPack,PUnionCmmPackData pdesCmmPack);

// ����һ�����ı�ʶ
/*
�������
	poriCmmPack	Դ��
�������
	pdesCmmPack	Ŀ���
����ֵ
	>= 0		�ɹ�
	<0		�������
*/
int UnionCopyCmmPackIdentifiedTag(PUnionCmmPackData poriCmmPack,PUnionCmmPackData pdesCmmPack);

// Ϊһ������һ����ʶ
/*
�������
	��
�������
	ppack		���ṹ
����ֵ
	>= 0		�ɹ�
	<0		�������
*/
int UnionPutCmmPackIdentifiedTag(PUnionCmmPackData pcmmPack);

void UnionSetLogPackageAnyway();

void UnionCloseLogPackageAnyway();


// ���ý���ĳ����Ǽ�ؿͻ���
int UnionSetPackageAsSpierClientPackage();

// ��ʼ������
void UnionInitServicePackage(PUnionServicePackage ppack);

// ���ð�ͷ
int UnionSetServicePackageHeader(char *idOfApp,int serviceCode,int direction,int resCode,PUnionServicePackage ppack);

// ���ð�ͷ
int UnionSetServicePackageData(PUnionCmmPackData pdata,PUnionServicePackage ppack);

// ��һ����
/*
�������
	ppack		���ṹ
	sizeOfBuf	buf�Ĵ�С
�������
	buf		��õİ�
����ֵ
	>= 0		��õİ��ĳ���
	<0		�������
*/
int UnionPackPackage(PUnionServicePackage ppack,char *buf,int sizeOfBuf);

// ��һ����
/*
�������
	data		����
	lenOfData	���ݵĳ���
�������
	ppack		����İ�
����ֵ
	>= 0		����İ�����Ч����
	<0		�������
*/
int UnionUnpackPackage(char *data,int lenOfData,PUnionServicePackage ppack);

// ��ʼ��������
int UnionInitRequestPackage(PUnionServiceRequestPackage ppack);

// ��ʼ����Ӧ����
int UnionInitResponsePackage(PUnionServiceResponsePackage ppack);

// �������д����־
void UnionLogRequestPackage(PUnionServiceRequestPackage ppack);

// ����Ӧ��д����־
void UnionLogResponsePackage(PUnionServiceResponsePackage ppack);

// ��һ�������
int UnionPackRequestPackage(char *idOfApp,int serviceCode,PUnionCmmPackData pdatagram,char *buf,int sizeOfBuf);

// ��һ�������
int UnionUnpackRequestPackage(char *data,int lenOfData,char *idOfApp,int *serviceID,PUnionCmmPackData pdatagram);

// ��һ����Ӧ��
int UnionPackResponsePackage(char *idOfApp,int serviceCode,int resCode,PUnionCmmPackData pdatagram,char *buf,int sizeOfBuf);

// ��һ����Ӧ��
int UnionUnpackResponsePackage(char *data,int lenOfData,char *idOfApp,int *serviceID,int *resCode,PUnionCmmPackData pdatagram);

#endif

