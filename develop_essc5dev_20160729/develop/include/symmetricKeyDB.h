#ifndef _SymmetricKeyDB_H_
#define _SymmetricKeyDB_H_

#include "unionDesKey.h"
#include "commWithHsmSvr.h"

#define	defTableNameOfSymmetricKeyDB		"symmetricKeyDB"
#define	defTableNameOfSymmetricKeyValue		"symmetricKeyValue"
#define maxNumOfSymmetricKeyValue		10

// ���崴��������
typedef enum 
{
	conSymmetricCreatorTypeOfUser,
	conSymmetricCreatorTypeOfApp,
	conSymmetricCreatorTypeOfKMS,
	conSymmetricCreatorTypeOfMove,
	conSymmetricCreatorTypeOfUnknown = 99
} TUnionSymmetricCreatorType;

// ����Գ���Կ�㷨��ʶ
typedef enum 
{
	conSymmetricAlgorithmIDOfDES,
	conSymmetricAlgorithmIDOfSM4,
	conSymmetricAlgorithmIDOfSM1,
	conSymmetricAlgorithmIDOfAES,
	conSymmetricAlgorithmIDOfUnknown = 99
} TUnionSymmetricAlgorithmID;

// ����Գ���Կ���ȱ�ʶ
typedef enum 
{
	con64BitsSymmetricKey = 64,
	con128BitsSymmetricKey = 128,
	con192BitsSymmetricKey = 192,
	con256BitsSymmetricKey = 256,
	conUnknownLengthSymmetricKey = 0
} TUnionSymmetricKeyLength;

// ����Գ���Կ״̬��ʶ
typedef enum 
{
	conSymmetricKeyStatusOfInitial,		// ��ʼ״̬
	conSymmetricKeyStatusOfEnabled,		// ����״̬
	conSymmetricKeyStatusOfSuspend,		// ����״̬
	conSymmetricKeyStatusOfCancel,		// ����״̬
	conSymmetricKeyStatusOfUnknown = 99	// δ֪״̬
} TUnionSymmetricKeyStatus;

// ����Գ���Կ���ͱ�ʶ
typedef TUnionDesKeyType	TUnionSymmetricKeyType;

// ����Գ���Կ����
typedef struct
{
	char				lmkProtectMode[16+1];		// LMK������ʽ
	char				keyValue[48+1];			// ��Կ����
	char				oldKeyValue[48+1];		// �ɰ汾����Կֵ
} TUnionSymmetricKeyValue;
typedef TUnionSymmetricKeyValue		*PUnionSymmetricKeyValue;

// ����Գ���Կ����
typedef struct
{
	char				keyName[128+1];			// ��Կ����
	char				keyGroup[16+1];			// ��Կ��
	TUnionSymmetricAlgorithmID	algorithmID;			// �㷨��ʶ
	TUnionSymmetricKeyType		keyType;			// ��Կ����
	TUnionSymmetricKeyLength	keyLen;				// ��Կ����
	int				inputFlag;			// ������
	int				outputFlag;			// ������
	int				effectiveDays;			// ��Ч����
	TUnionSymmetricKeyStatus	status;				// ״̬
	int				oldVersionKeyIsUsed;		// ����ʹ�þ���Կ
	char				activeDate[8+1];		// ��Ч����
	TUnionSymmetricKeyValue		keyValue[maxNumOfSymmetricKeyValue];	// ��Կֵ
	char				checkValue[16+1];		// ��ԿУ��ֵ
	char				oldCheckValue[16+1];		// �ɰ汾����ԿУ��ֵ
	char				keyUpdateTime[14+1];		// �������ʱ��
	char				keyApplyPlatform[16+1];		// ��Կ����ƽ̨
	char				keyDistributePlatform[16+1];	// ��Կ�ַ�ƽ̨
	TUnionSymmetricCreatorType	creatorType;			// ����������			
	char				creator[40+1];			// ������
	char				createTime[14+1];		// ����ʱ��
	char				usingUnit[16+1];		// ʹ�õ�λ
	char				remark[128+1];			// ��ע
} TUnionSymmetricKeyDB;
typedef TUnionSymmetricKeyDB		*PUnionSymmetricKeyDB;

typedef struct
{
	char				keyName[128+1];			// ��Կ����
	char				keyGroup[16+1];			// ��Կ��
	char				algorithmID[16];		// �㷨��ʶ
	char				keyType[16];			// ��Կ����
	char				keyLen[16];			// ��Կ����
	char				inputFlag[16];			// ������
	char				outputFlag[16];			// ������
	char				effectiveDays[16];		// ��Ч����
	char				status[16];			// ״̬
	char				oldVersionKeyIsUsed[16];	// ����ʹ�þ���Կ
	char				activeDate[8+1];		// ��Ч����
	TUnionSymmetricKeyValue		keyValue[maxNumOfSymmetricKeyValue];	// ��Կֵ
	char				checkValue[16+1];		// ��ԿУ��ֵ
	char				oldCheckValue[16+1];		// �ɰ汾����ԿУ��ֵ
	char				keyUpdateTime[14+1];		// �������ʱ��
	char				keyApplyPlatform[16+1];		// ��Կ����ƽ̨
	char				keyDistributePlatform[16+1];	// ��Կ�ַ�ƽ̨
	char				creatorType[16];		// ����������			
	char				creator[40+1];			// ������
	char				createTime[14+1];		// ����ʱ��
	char				usingUnit[16+1];		// ʹ�õ�λ
	char				remark[128+1];			// ��ע
} TUnionSymmetricKeyDBStr;
typedef TUnionSymmetricKeyDBStr		*PUnionSymmetricKeyDBStr;

/* 
���ܣ�	ת���Գ���Կ����
������	keyName[in|out]		��Կ����
	sizeofBuf[in]		�����С
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionTransformSymmetricKeyName(char *keyName,int sizeofBuf);

/* 
���ܣ�	��ȡ�Գ���Կ���ڸ��ٻ����м���
������	keyName[in]		��Կ����
	highCachedKey[out]	����
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionGetKeyOfHighCachedForSymmetricKeyDB(char *keyName,char *highCachedKey);

// ��ȡLMK������ʽ��ı�����Կ
int UnionReadProtectKeyFormLmkProtectMode(TUnionSymmetricAlgorithmID algorithmID,char *lmkProtectMode,char *keyValue,int sizeOfKey,char *checkValue,int sizeOfCV);

// ��ȡ�Գ���Կֵ
PUnionSymmetricKeyValue UnionGetSymmetricKeyValue(PUnionSymmetricKeyDB psymmetricKeyDB,char *lmkProtectMode);

// �жϾ���Կ�Ƿ�����ʹ��
int UnionOldVersionSymmetricKeyIsUsed(PUnionSymmetricKeyDB psymmetricKeyDB);

// ����Կ��������תΪ�ڲ�ö��ֵ
TUnionSymmetricKeyType UnionConvertSymmetricKeyKeyType(char *keyTypeName);

// ����Կ�����ڲ�ö��ת��Ϊ�ⲿ��ʶ��
char *UnionConvertSymmetricKeyKeyTypeToExternal(TUnionSymmetricKeyType keyType);

// �ж��Ƿ��ǺϷ��ĶԳ���Կ����
int UnionIsValidSymmetricKeyType(TUnionSymmetricKeyType keyType);

// �ж��Ƿ��ǺϷ��ĶԳ���Կ����
int UnionIsValidSymmetricKeyLength(TUnionSymmetricKeyLength keyLen);

// �ѶԳ���Կ����תΪDES��Կ����
TUnionDesKeyLength UnionConvertSymmetricKeyKeyLen(TUnionSymmetricKeyLength keyLen);

// ���㷨��ʶ����תΪ�ڲ�ö��ֵ
TUnionSymmetricAlgorithmID UnionConvertSymmetricKeyAlgorithmID(char *algorithmID);

// ���㷨�ڲ�ö��ֵ��תΪ��ʶ��
int UnionConvertSymmetricKeyStringAlgorithmID(TUnionSymmetricAlgorithmID algorithmID,char *strAlgorithmID);

// �ж��Ƿ��ǺϷ��ĶԳ���Կ�㷨
int UnionIsValidSymmetricKeyAlgorithmID(TUnionSymmetricAlgorithmID algorithmID);

// �ж��Ƿ��ǺϷ��ĶԳ���Կ����
int UnionIsValidSymmetricKeyName(char *keyName);

// ����Կ�����л�ȡ�ڵ�����
int UnionAnalysisSymmetricKeyName(char *keyName,char *appID,char *keyNode,char *keyType);

// �ж��Ƿ��ǺϷ��ĶԳ���Կ
int UnionIsValidSymmetricKeyDB(PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
���ܣ�	����һ���Գ���Կ����
������	psymmetricKeyDB[in]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionCreateSymmetricKeyDB(PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
���ܣ�	����һ���Գ���Կ���� sql��ͬ��
������	psymmetricKeyDB[in]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionCreateSymmetricKeyDBWithoutSqlSyn(PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
���ܣ�	ɾ��һ���Գ���Կ����
������	psymmetricKeyDB[in]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionDropSymmetricKeyDB(PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
���ܣ�	�ѶԳ���Կ���ݽṹת�����ַ���
������	psymmetricKeyDB[in]	��Կ������Ϣ
	keyDBStr[out]		��Կ�ַ���
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionSymmetricKeyDBStructToString(PUnionSymmetricKeyDB psymmetricKeyDB,char *keyDBStr);

/* 
���ܣ�	�ѶԳ���Կ�ַ���ת�������ݽṹ
������	keyDBStr[in]		��Կ�ַ���
	psymmetricKeyDB[out]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionSymmetricKeyDBStringToStruct(char *keyDBStr,PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
���ܣ�	��ȡһ���Գ���Կ����
������	keyName[in]		��Կ����
	isCheckKey[in]		�Ƿ�����Կ�Ŀ����ԣ�1��飬0�����		
	psymmetricKeyDB[out]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionReadSymmetricKeyDBRec(char *keyName,int isCheckKey,PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
���ܣ�	���ɶԳ���Կ
������	psymmetricKeyDB[in|out]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionGenerateSymmetricKeyDBRec(PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
���ܣ�	���¶Գ���Կ
������	psymmetricKeyDB[in|out]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionUpdateSymmetricKeyDBKeyValue(PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
���ܣ�	���¶Գ���Կ	sql��ͬ��
������	psymmetricKeyDB[in|out]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionUpdateSymmetricKeyDBKeyValueWithoutSqlSyn(PUnionSymmetricKeyDB psymmetricKeyDB);


int UnionAnalysisPBOCSymmetricKeyName(char *keyName, int version, char *appID, char *keyNode, char *keyType);

/* 
���ܣ�	���Գ���Կ״̬
������	psymmetricKeyDB[in]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionCheckSymmetricKeyStatus(PUnionSymmetricKeyDB psymmetricKeyDB);

/*
���ܣ�	���Գ���Կʹ��Ȩ��
������	psymmetricKeyDB[in]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionCheckSymmetricKeyPrivilege(PUnionSymmetricKeyDB psymmetricKeyDB);

/* 
���ܣ�	ƴ�ӶԳ���Կ�Ĵ�����
������	sysID[in]	ϵͳID
	appID[in]	Ӧ��ID
	creator[out]	������
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionPieceSymmetricKeyCreator(char *sysID,char *appID,char *creator);

/* 
���ܣ�	����һ���Գ���Կ����״̬
������	psymmetricKeyDB[in]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionUpdateSymmetricKeyDBStatus(PUnionSymmetricKeyDB psymmetricKeyDB);

/*
���ܣ�	��ӡ��Կ
������	phsmGroupRec:		�Գ���Կ����
	psymmetricKeyDB��	��Կ��Ϣ
	keyValuePrintFormat�� 	��ӡ��ʽ
	isCheckAlonePrint��	�Ƿ������ӡУ��ֵ 
	numOfComponent�� 	��������
	hsmIP�� 		���ܻ�IP
���أ�
	psymmetricKeyDB:	��Կֵ��У��ֵ
*/
int UnionGenerateAndPrintSymmetricKey(PUnionHsmGroupRec phsmGroupRec,PUnionSymmetricKeyDB psymmetricKeyDB,char *keyValuePrintFormat,int isCheckAlonePrint,int numOfComponent,char *hsmIP,int appendPrintNum,char appendPrintParam[][80]);

int UnionSymmetricKeyDBStructToStringStruct(PUnionSymmetricKeyDBStr psymmetricKeyDBStr, PUnionSymmetricKeyDB psymmetricKeyDB);
int UnionSymmetricKeyDBStringSturctToStruct(PUnionSymmetricKeyDB psymmetricKeyDB, PUnionSymmetricKeyDBStr psymmetricKeyDBStr);
int UnionIsSymmetricKeyDBRecInCachedOnStructFormat();

// ����Կ����ת��Ϊ�ڲ�ö��ֵ
TUnionSymmetricKeyLength UnionConvertSymmetricKeyDBKeyLen(int keyLen);

// ��ʼ����Կ�ṹ��
void UnionInitSymmetricKeyDB(PUnionSymmetricKeyDB psymmetricKeyDB);

// �����Կ�Ƿ����ʹ��
/*
����:	�����Կ�Ƿ����ʹ�ã���Ҫ���ڸ�����Կ�͵�����Կ���
����:	status:		��Կ״̬
	flag:		��ʶ,
			flag = 0,��ʾ�ⲿappӦ��
			flag = 1,��ʾ�ڲ�appӦ�ã��ڲ�ʹ����Կ������״̬
����:	
	>=0		�ɹ�
	<0		ʧ��,���ش�����
*/
int UnionIsUseCheckSymmetricKeyDB(TUnionSymmetricKeyStatus status,int flag);
#endif

