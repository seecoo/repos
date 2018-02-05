// Author:	Wolfgang Wang
// Date:	2006/8/9

// ����ļ�������ESSC���ĸ�ʽ

#ifndef _esscEsscPackage_
#define _esscEsscPackage_

#include "esscFldTagDef.h"

// �¾ɱ���ת������
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

// ���¶���ESSC������ͷ
typedef struct
{
	char	appID[32];		// Ӧ�ñ��
	char	serviceID[32];		// ������
	char	flag[32];		// ��־
	char	numOfFld[32];		// ��ĸ���
} TUnionEsscRequestPackageHeader;
typedef TUnionEsscRequestPackageHeader	*PUnionEsscRequestPackageHeader;

// ���¶���ESSC��Ӧ����ͷ
typedef struct
{
	char	appID[32];		// Ӧ�ñ��
	char	serviceID[32];		// ������
	char	flag[32];		// ��־
	char	responseCode[32];	// ��Ӧ��
	char	numOfFld[32];		// ��ĸ���
} TUnionEsscResponsePackageHeader;
typedef TUnionEsscResponsePackageHeader	*PUnionEsscResponsePackageHeader;

// ���¶���һ����Ľṹ
typedef struct
{
	int		tag;		// ��ı�ʶ��
	int		len;		// ��ĳ���
	unsigned char	*value;		// ���ֵ
} TUnionEsscPackageFld;
typedef TUnionEsscPackageFld		*PUnionEsscPackageFld;

#define conMaxNumOfEsscPackageFld	24
typedef struct
{
	TUnionEsscPackageFld	fldGrp[conMaxNumOfEsscPackageFld];	// ����
	int			fldNum;					// ������
	unsigned char		dataBuf[4096+128];			// ���ݻ���,2007/11/19,��С���޸�ǰ��2048+1����Ϊ���ڵ�ֵ
	int			offset;					// ���ݻ��嵱ǰ�����������ʼλ��
} TUnionEsscPackage;
typedef TUnionEsscPackage	*PUnionEsscPackage;

int UnionGetVersionOfEsscPackage();

// ��ȡ�����������Ŀ
int UnionGetMaxFldNumOfEsscRequestPackage();

// ��һ��ESSC��������뵽����,���ش��뵽���е����ݵĳ���
int UnionPutEsscPackageFldIntoStr(char *serviceID,int index,PUnionEsscPackageFld pfld,char *buf,int sizeOfBuf);

// ��һ��ESSC�����ж�ȡһ���򣬷������ڰ���ռ�ĳ���
int UnionReadEsscPackageFldFromStr(char *serviceID,int index,char *data,int len,PUnionEsscPackageFld pfld,int sizeOfFldValue);

// ��ʼ��������
int UnionInitEsscRequestPackage();

// ������������
int UnionSetEsscRequestPackageFld(int fldTag,int len,char *value);

// �������ʶ�ţ���ȡ��������
int UnionReadEsscRequestPackageFld(int fldTag,char *value,int sizeOfBuf);

// �����������ţ���ȡ��������
int UnionReadEsscRequestPackageFldByIndex(int index,int *fldTag,char *value,int sizeOfBuf);

// �������д����־
void UnionLogEsscRequestPackage();

// ��һ�������
int UnionPackEsscRequestPackage(char *idOfApp,char *serviceID,char *buf,int sizeOfBuf);

// ��һ�������
int UnionUnpackEsscRequestPackage(char *data,int lenOfData,char *idOfApp,char *serviceID);

// ��ȡ��Ӧ��������Ŀ
int UnionGetMaxFldNumOfEsscResponsePackage();

// ��ʼ����Ӧ����
int UnionInitEsscResponsePackage();

// ������Ӧ������
int UnionSetEsscResponsePackageFld(int fldTag,int len,char *value);

// ��ȡ��Ӧ������
int UnionReadEsscResponsePackageFld(int fldTag,char *value,int sizeOfBuf);

// �����������ţ���ȡ��Ӧ������
int UnionReadEsscResponsePackageFldByIndex(int index,int *fldTag,char *value,int sizeOfBuf);

// ����Ӧ��д����־
void UnionLogEsscResponsePackage();

// ��һ����Ӧ��
int UnionPackEsscResponsePackage(char *idOfApp,char *serviceID,int responseCode,char *buf,int sizeOfBuf);

// ��һ����Ӧ��
int UnionUnpackEsscResponsePackage(char *data,int lenOfData,char *idOfApp,char *serviceID);

// ���ý���ĳ����Ǽ�ؿͻ���
int UnionSetEsscPackageAsSpierClientPackage();

// ��һ���������
int UnionPackEsscDataFldPackage(char *buf,int sizeOfBuf);

// ��һ�������
int UnionUnpackEsscDataFldPackage(char *data,int lenOfData);

void UnionSetLogEsscPackageAnyway();

void UnionCloseLogEsscPackageAnyway();

// ����Ӧ��д����־
void UnionLogEsscResponsePackageAnyway(char *idOfApp,char *serviceID,int responseCode);

// �������д����־
void UnionLogEsscRequestPackageAnyway(char *idOfApp,char *serviceID);

// ���������ʶ��ⱨ��ֵ�Ƿ���Ҫ��ӡ
int UnionIsPrintByRequestFlag(char *oldServiceCode,int version,int parameter);

// ���ݷ������Ӧ�ñ��ȷ����Կ����
int UnionSetConvertPackageKeyName(char *serviceID,char *appNo);
int UnionIsCheckUnionPayPackage();

//���ݷ������Ӧ�ñ������ƽ̨3KMSvr�˿ڵı��Ķ�Ӧ�ķ�����ͱ�����
int UnionSetKMSvrServiceCode(char *appID, char *serviceCode, char *currServiceCode);
int UnionSetKMSvrRequestPackage(char *serviceCode);
#endif
