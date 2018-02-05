#ifndef _AsymmetricKeyDB_H_
#define _AsymmetricKeyDB_H_

#define	defTableNameOfAsymmetricKeyDB	"asymmetricKeyDB"

// ���崴��������
typedef enum 
{
	conAsymmetricCreatorTypeOfUser,
	conAsymmetricCreatorTypeOfApp,
	conAsymmetricCreatorTypeOfKMS,
	conAsymmetricCreatorTypeOfUnknown = 99
} TUnionAsymmetricCreatorType;

// ����ǶԳ���Կ�㷨��ʶ
typedef enum 
{
	conAsymmetricAlgorithmIDOfRSA,
	conAsymmetricAlgorithmIDOfSM2,
	conAsymmetricAlgorithmIDOfUnknown = 99
} TUnionAsymmetricAlgorithmID;

// ����ǶԳ���Կ��Կ����
typedef enum
{
	con256BitsAsymmetricKey = 256,
	con512BitsAsymmetricKey = 512,
	con1024BitsAsymmetricKey = 1024,
	con1152BitsAsymmetricKey = 1152,
	con1408BitsAsymmetricKey = 1408,
	con1984BitsAsymmetricKey = 1984,
	con2048BitsAsymmetricKey = 2048,
	conUnknownLengthAsymmetricKey = 0
} TUnionAsymmetricKeyLength;

// ����ǶԳ���Կ״̬��ʶ
typedef enum 
{
	conAsymmetricKeyStatusOfInitial,	// ��ʼ״̬
	conAsymmetricKeyStatusOfEnabled,	// ����״̬
	conAsymmetricKeyStatusOfSuspend,	// ����״̬
	conAsymmetricKeyStatusOfCancel,		// ����״̬
	conAsymmetricKeyStatusOfUnknown = 99	// δ֪״̬
} TUnionAsymmetricKeyStatus;

// ����ǶԳ���Կ���ͱ�ʶ
typedef enum
{
	conAsymmetricKeyTypeOfSignature = 0,			// ǩ��
	conAsymmetricKeyTypeOfEncryption = 1,			// ����
	conAsymmetricKeyTypeOfSignatureAndEncryption = 2,	// ǩ���ͼ���
	conAsymmetricKeyTypeOfConsult = 3,			// ��ԿЭ��
	conAsymmetricKeyTypeOfSignatureAndEncryptionAndKeyMng = 4,	// ǩ���ͼ��ܣ���Կ����
	conAsymmetricKeyTypeOfUnknownPKType = 99
} TUnionAsymmetricKeyType;

// ����ǶԳ���Կ����
typedef struct
{
	char				keyName[128+1];			// ��Կ����
	char				keyGroup[16+1];			// ��Կ��
	TUnionAsymmetricAlgorithmID	algorithmID;			// �㷨��ʶ
	TUnionAsymmetricKeyType		keyType;			// ��Կ����
	TUnionAsymmetricKeyLength	keyLen;				// ��Կ����
	int				pkExponent;			// ��Կָ�������㷨��ʶΪRSAʱ����Ч
	int				inputFlag;			// ������
	int				outputFlag;			// ������
	int				effectiveDays;			// ��Ч����
	TUnionAsymmetricKeyStatus	status;				// ״̬
	int				vkStoreLocation;		// ˽Կ�洢λ��
	char				hsmGroupID[8+1];		// ���ܻ���ID
	char				vkIndex[2+1];			// ˽Կ����
	int				oldVersionKeyIsUsed;		// ����ʹ�þ���Կ
	char				pkValue[512+1];			// ��Կֵ
	char				pkCheckValue[16+1];		// ��ԿУ��ֵ
	char				vkValue[2560+1];		// ˽Կֵ
	char				vkCheckValue[16+1];		// ˽ԿУ��ֵ
	char				oldPKValue[512+1];		// �ɰ汾�Ĺ�Կֵ
	char				oldPKCheckValue[16+1];		// �ɰ汾�Ĺ�ԿУ��ֵ
	char				oldVKValue[2560+1];		// �ɰ汾��˽Կֵ
	char				oldVKCheckValue[16+1];		// �ɰ汾��˽ԿУ��ֵ
	char				activeDate[8+1];		// ��Ч����
	char				keyUpdateTime[14+1];		// �������ʱ��
	char				keyApplyPlatform[16+1];		// ��Կ����ƽ̨
	char				keyDistributePlatform[16+1];	// ��Կ�ַ�ƽ̨
	TUnionAsymmetricCreatorType	creatorType;			// ����������		
	char				creator[40+1];			// ������
	char				createTime[14+1];		// ����ʱ��
	char				usingUnit[16+1];		// ʹ�õ�λ
	char				remark[128+1];			// ��ע
} TUnionAsymmetricKeyDB;
typedef TUnionAsymmetricKeyDB		*PUnionAsymmetricKeyDB;

typedef struct
{
	char				keyName[128+1];			// ��Կ����
	char				keyGroup[16+1];			// ��Կ��
	char				algorithmID[16];		// �㷨��ʶ
	char				keyType[16];			// ��Կ����
	char				keyLen[16];			// ��Կ����
	char				pkExponent[16];			// ��Կָ�������㷨��ʶΪRSAʱ����Ч
	char				inputFlag[16];			// ������
	char				outputFlag[16];			// ������
	char				effectiveDays[16];		// ��Ч����
	char				status[16];			// ״̬
	char				vkStoreLocation[16];		// ˽Կ�洢λ��
	char				hsmGroupID[8+1];		// ���ܻ���ID
	char				vkIndex[2+1];			// ˽Կ����
	char				oldVersionKeyIsUsed[16];	// ����ʹ�þ���Կ
	char				pkValue[512+1];			// ��Կֵ
	char				pkCheckValue[16+1];		// ��ԿУ��ֵ
	char				vkValue[2560+1];		// ˽Կֵ
	char				vkCheckValue[16+1];		// ˽ԿУ��ֵ
	char				oldPKValue[512+1];		// �ɰ汾�Ĺ�Կֵ
	char				oldPKCheckValue[16+1];		// �ɰ汾�Ĺ�ԿУ��ֵ
	char				oldVKValue[2560+1];		// �ɰ汾��˽Կֵ
	char				oldVKCheckValue[16+1];		// �ɰ汾��˽ԿУ��ֵ
	char				activeDate[8+1];		// ��Ч����
	char				keyUpdateTime[14+1];		// �������ʱ��
	char				keyApplyPlatform[16+1];		// ��Կ����ƽ̨
	char				keyDistributePlatform[16+1];	// ��Կ�ַ�ƽ̨
	char				creatorType[16];		// ����������		
	char				creator[40+1];			// ������
	char				createTime[14+1];		// ����ʱ��
	char				usingUnit[16+1];		// ʹ�õ�λ
	char				remark[128+1];			// ��ע
} TUnionAsymmetricKeyDBStr;
typedef TUnionAsymmetricKeyDBStr	*PUnionAsymmetricKeyDBStr;

/* 
���ܣ�	ת���ǶԳ���Կ����
������	keyName[in|out]		��Կ����
	sizeofBuf[in]		�����С
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionTransformAsymmetricKeyName(char *keyName,int sizeofBuf);

/* 
���ܣ�	��ȡ�ǶԳ���Կ���ڸ��ٻ����м���
������	keyName[in]		��Կ����
	highCachedKey[out]	����
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionGetKeyOfHighCachedForAsymmetricKeyDB(char *keyName,char *highCachedKey);

// �жϾ���Կ�Ƿ�����ʹ��
int UnionOldVersionAsymmetricKeyIsUsed(PUnionAsymmetricKeyDB pasymmetricKeyDB);

// �ж��Ƿ��ǺϷ��ķǶԳ���Կ����
int UnionIsValidAsymmetricKeyType(TUnionAsymmetricKeyType keyType);

// �ж��Ƿ��ǺϷ��ķǶԳ���Կ����
int UnionIsValidAsymmetricKeyLength(TUnionAsymmetricKeyLength keyLen);

// ���㷨��ʶ����תΪ�ڲ�ö��ֵ
TUnionAsymmetricAlgorithmID UnionConvertAsymmetricKeyAlgorithmID(char *algorithmID);

// �ж��Ƿ��ǺϷ��ķǶԳ���Կ�㷨
int UnionIsValidAsymmetricKeyAlgorithmID(TUnionAsymmetricAlgorithmID algorithmID);

// �ж��Ƿ��ǺϷ��ķǶԳ���Կ����
int UnionIsValidAsymmetricKeyName(char *keyName);

// ����Կ�����л�ȡ��������
int UnionAnalysisAsymmetricKeyName(char *keyName,char *appID,char *ownerName,char *keyType);

// �ж��Ƿ��ǺϷ��ķǶԳ���Կ
int UnionIsValidAsymmetricKeyDB(PUnionAsymmetricKeyDB pasymmetricKeyDB);

/* 
���ܣ�	����һ���ǶԳ���Կ����
������	pasymmetricKeyDB[in]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionCreateAsymmetricKeyDB(PUnionAsymmetricKeyDB pasymmetricKeyDB);

/* 
���ܣ�	ɾ��һ���ǶԳ���Կ����
������	pasymmetricKeyDB[in]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionDropAsymmetricKeyDB(PUnionAsymmetricKeyDB pasymmetricKeyDB);

/* 
���ܣ�	�ѷǶԳ���Կ���ݽṹת�����ַ���
������	pasymmetricKeyDB[in]	��Կ������Ϣ
	keyDBStr[out]		��Կ�ַ���
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionAsymmetricKeyDBStructToString(PUnionAsymmetricKeyDB pasymmetricKeyDB,char *keyDBStr);

/* 
���ܣ�	�ѷǶԳ���Կ�ַ���ת�������ݽṹ
������	keyDBStr[in]		��Կ�ַ���
	pasymmetricKeyDB[out]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionAsymmetricKeyDBStringToStruct(char *keyDBStr,PUnionAsymmetricKeyDB pasymmetricKeyDB);

/* 
���ܣ�	��ȡһ���ǶԳ���Կ����
������	keyName[in]		��Կ����
	isCheckKey[in]		�Ƿ�����Կ�Ŀ����ԣ�1��飬0�����		
	pasymmetricKeyDB[out]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionReadAsymmetricKeyDBRec(char *keyName,int isCheckKey,PUnionAsymmetricKeyDB pasymmetricKeyDB);

/* 
���ܣ�	���ɷǶԳ���Կ
������	pasymmetricKeyDB[in|out]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionGenerateAsymmetricKeyDBRec(PUnionAsymmetricKeyDB pasymmetricKeyDB);

int UnionCreateAsymmetricKeyDBWithoutSqlSyn(PUnionAsymmetricKeyDB pasymmetricKeyDB);

/* 
���ܣ�	���·ǶԳ���Կ
������	pasymmetricKeyDB[in|out]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionUpdateAsymmetricKeyDBKeyValue(PUnionAsymmetricKeyDB pasymmetricKeyDB);

int UnionUpdateAsymmetricKeyDBKeyValueWithoutSqlSyn(PUnionAsymmetricKeyDB pasymmetricKeyDB);

/* 
���ܣ�	���ǶԳ���Կ״̬
������	pasymmetricKeyDB[in]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionCheckAsymmetricKeyStatus(PUnionAsymmetricKeyDB pasymmetricKeyDB);

/*
���ܣ�	���ǶԳ���Կʹ��Ȩ��
������	psymmetricKeyDB[in]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionCheckAsymmetricKeyPrivilege(PUnionAsymmetricKeyDB pasymmetricKeyDB);

/* 
���ܣ�	ƴ�ӷǶԳ���Կ�Ĵ�����
������	sysID[in]	ϵͳID
	appID[in]	Ӧ��ID
	creator[out]	������
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionPieceAsymmetricKeyCreator(char *sysID,char *appID,char *creator);

int UnionAsymmetricKeyDBStructToStringStruct(PUnionAsymmetricKeyDBStr pasymmetricKeyDBStr, const PUnionAsymmetricKeyDB pasymmetricKeyDB);
int UnionAsymmetricKeyDBStringSturctToStruct(PUnionAsymmetricKeyDB pasymmetricKeyDB, const  PUnionAsymmetricKeyDBStr pasymmetricKeyDBStr);
int UnionIsAsymmetricKeyDBRecInCachedOnStructFormat();

// ɾ��֤��
int UnionDropCertOfCnaps2(char *keyName);

//��ʼ���ṹ��
void UnionInitASymmetricKeyDB(PUnionAsymmetricKeyDB pasymmetricKeyDB);

#endif

