// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/8/10
// Version:	1.0

#include <stdio.h>

#ifndef _esscFldTagDef_
#define _esscFldTagDef_

#define conEsscFldKeyName			1			// ��Կ������
#define conEsscFldFirstWKName			conEsscFldKeyName	// ��һ��������Կ������
#define conEsscFldSecondWKName			(conEsscFldKeyName+1)	// �ڶ���������Կ������
#define conEsscFldZMKName			11			// ZMK��Կ������
#define conEsscFldFirstZMKName			conEsscFldZMKName	// ��һ��ZMK��Կ������
#define conEsscFldSecondZMKName			(conEsscFldZMKName+1)	// �ڶ���ZMK��Կ������

#define conEsscFldMacData			21			// MAC����
#define conEsscFldMac				22			// MAC
#define conEsscFldZakByZmk                      23                      // ZAK BY ZMK
#define conEsscFldPlainPin			31			// PIN����
#define conEsscFldPinOffset			32			// PINOffset
#define conEsscFldEncryptedPinByZPK		33			// ZPK���ܵ�PIN����
#define conEsscFldEncryptedPinByZPK1		conEsscFldEncryptedPinByZPK			// ZPK1���ܵ�PIN����
#define conEsscFldEncryptedPinByZPK2		(conEsscFldEncryptedPinByZPK+1)			// ZPK2���ܵ�PIN����
#define conEsscFldEncryptedPinByLMK0203		35			// LMK0203���ܵ�PIN����
#define conEsscFldVisaPVV			36			// Visa PVV
#define conEsscFldIBMPinOffset			37			// IBM PinOffset
#define conEsscFldPinByRsaPK			38			// RSAPK���ܵ�PIN

#define conEsscFldAccNo				41			// �˺�
#define conEsscFldAccNo1			conEsscFldAccNo			// �˺�1
#define conEsscFldAccNo2			(conEsscFldAccNo+1)		// �˺�2

#define conEsscFldKeyCheckValue			51				// ��ԿУ��ֵ
#define conEsscFldFirstKeyCheckValue		conEsscFldKeyCheckValue		// ��һ����ԿУ��ֵ
#define conEsscFldSecondKeyCheckValue		(conEsscFldKeyCheckValue+1)	// �ڶ�����ԿУ��ֵ

#define conEsscFldKeyValue			61			// ��Կֵ
#define conEsscFldFirstKeyValue			conEsscFldKeyValue	// ��һ����Կֵ
#define conEsscFldSecondKeyValue		(conEsscFldKeyValue+1)	// �ڶ�����Կֵ

#define conEsscFldVisaCVV			71			// VisaCVV
#define conEsscFldCardPeriod			72			// ����Ч��
#define conEsscFldServiceID			73			// �������

#define conEsscFldData				81			// ������
#define conEsscFldPlainData			conEsscFldData		// ��������
#define conEsscFldCiperData			82			// ��������
#define conEsscFldHashData			83			// HASH���ݣ�2008/4/10��added
#define conEsscFldHashDegist			84			// HashժҪ��2008/4/10��added

#define conEsscFldSignData			91			// ǩ������
#define conEsscFldSign				92			// ǩ��
#define conEsscFldSignDataPadFlag		93			// ǩ�����ݵ���䷽ʽ

#define conEsscFldDirectHsmCmdReq		100		// ֱ�ӵļ��ܻ�ָ��
#define conEsscFldDirectHsmCmdRes		101		// ֱ�ӵļ��ܻ�ָ����Ӧ

#define conEsscFldHsmGrpID			201		// �������
#define conEsscFldKeyContainer			202		// ��Կ����
#define conEsscFldKeyLenFlag			203		// ��Կ���ȱ�ʶ
#define conEsscFldForRemoteKMSvrFlag		204		// ΪԶ����Կ�������ı�־
#define conEsscFldForPinLength			205		// PIN�ĳ���
#define conEsscFldRefrenceNumber		206		// �ο�����
#define conEsscFldIDOfApp			207		// Ӧ�ñ��
#define conEsscFldLengthOfKey			208		// ��Կ����
#define conEsscFldSuccessHsmNum			209		// ִ��ָ��ɹ���HSM������
#define conEsscFldKeyIndex			210		// ��Կ����
#define conEsscFldHsmIPAddrList			211		// �����IP��ַ�б�
#define conEsscFldPKCodingMethod		212		// PK�ı��뷽ʽ
#define conEsscFldIV				213		// ��ʼ����
#define conEsscFldAlgorithmMode			214		// �㷨��־
#define conEsscFldAlgorithm01Mode		conEsscFldAlgorithmMode		// ��һ���㷨��־
#define conEsscFldAlgorithm02Mode		(conEsscFldAlgorithmMode+1)	// �ڶ����㷨��־
#define conEsscFldFileName			220		// �ļ�������ȫ·��
#define conEsscFldKeyTypeFlag			221		// ��Կ���ͱ�ʶ
#define conEsscFldIDOfAppManaged		222		// ��ǰ�����Ӧ�ñ��

#define conEsscFldAutoSignature			300		// �Զ���ֵ��ǩ��

#define conEsscFldAutoSign			400		// �Զ�ǩ��

// 2009/4/28,������,����
#define conEsscFldNodeID			401		// �ڵ��ʶ
#define conEsscFldFirstNodeID			conEsscFldNodeID		// ��һ���ڵ��ʶ
#define conEsscFldSecondNodeID			(conEsscFldNodeID+1)		// �ڶ����ڵ��ʶ
#define conEsscFldKeyVersion			410		// ��Կ�汾��
#define conEsscFldSecondKeyVersion              409             // ��Կ�汾�� 
#define conEsscFldKeyActiveDate			411		// ��Կ��Ч����
#define conEsscFldKeyInactiveDate		412		// ��ԿʧЧ����
#define conEsscFldKeyReleaseDate		413		// ��Կ��������
#define conEsscFldKeyPlainValue			414		// ��Կ����ֵ
#define conEsscFldKeyModuleID			415		// ģ���ʶ
#define conEsscFldRecordNumber			416		// ��¼����
#define conEsscFldRecordList			417		// ������¼��
#define conEsscFldRecordStr			418		// Ψһ��¼��
#define conEsscFldKeyValueFormat		419		// ��Կֵ�ı�ʾ����
#define conEsscCertificateFormat		420		// ֤���ʽ
#define conEsscFirstCertificateFormat		conEsscCertificateFormat	// ��һ��֤���ʽ
#define conEsscSecondCertificateFormat		(conEsscCertificateFormat+1)	// �ڶ���֤���ʽ
#define conEsscDisperseTimes			425		// ��ɢ����
#define conEsscDisperseData			426		// ��ɢ����
#define conEsscARQC				427		// ARQC
#define conEsscARPC				428		// ARPC
#define conEsscARC				429		// ARC
// 2009/48,���ӽ���

#define conEsscFldPinCiperFormat		500					// PIN���ĵĸ�ʽ
#define conEsscFldFirstPinCiperFormat		conEsscFldPinCiperFormat		// ��һ��PIN���ĵĸ�ʽ
#define conEsscFldSecondPinCiperFormat		(conEsscFldPinCiperFormat+1)		// �ڶ���PIN���ĵĸ�ʽ

// Mary add, 2008-9-16
#define conEsscFldOffsetOfSpecPINCipher		600		// ���ⷽʽ���ܵ�PIN���ģ��˴�������ƫ����

#define conEsscFldErrorCodeRemark		999		// ����������

// 2007/10/26����
#define conEsscFldRemark			998		// ��ע��Ϣ
#define conEsscFldResID				997		// ��Դ��ʶ
// 2007/10/26���ӽ���

// 2009/01/13����
#define conEsscFldUniqueID			301		// ΨһID
#define conEsscFldLenOfUniqueID			302		// ΨһID�ĳ���
// 2009/01/13���ӽ���

// 2009/04/09����
#define conEsscFldRandNum			303				// �����
#define conEsscFldRandNum1			conEsscFldRandNum		// �����1
#define conEsscFldRandNum2			(conEsscFldRandNum+1)		// �����2
#define conEsscFldRandNum3			(conEsscFldRandNum+2)		// �����3
// 2009/04/09���ӽ���

// 2009/06/04����
#define conEsscFldARQC				313			// ARQC
#define conEsscFldARPC				314			// ARPC
#define conEsscFldARC				315			// ARC
// 2009/06/04���ӽ���

// Mary add begin, 20090708
#define conEsscFldMessageFlag			900			// ��Ϣ��ʶ���û��Զ��壬�������첽ͨѶʱ������Ϣƥ��
// Mary add end, 20090708

typedef struct
{
	int	tag;		// ���ʶ
	char	remark[40+1];	// ����˵��
} TUnionEsscFldTag;
typedef TUnionEsscFldTag	*PUnionEsscFldTag;

#define conMaxNumOfEsscFldTag	256
typedef struct
{
	int			realNum;
	TUnionEsscFldTag	tagList[conMaxNumOfEsscFldTag];
} TUnionEsscFldTagGroup;
typedef TUnionEsscFldTagGroup	*PUnionEsscFldTagGroup;
	
int UnionGetDefFileNameOfEsscFldTag(char *fileName);

// ���ַ����ж������ʶ����
int UnionReadEsscFldTagFromDefStr(char *str,PUnionEsscFldTag prec);

int UnionPrintEsscFldTagToFile(PUnionEsscFldTag prec,FILE *fp);

int UnionConnectEsscFldTagGroup();

int UnionDisconnectEsscFldTagGroup();

int UnionPrintEsscFldTagGroupToFile(FILE *fp);

int UnionPrintEsscFldTagGroupToSpecFile(char *fileName);

PUnionEsscFldTag UnionFindEsscFldTag(int tag);

char *UnionFindEsscFldTagRemark(int tag);

#endif
