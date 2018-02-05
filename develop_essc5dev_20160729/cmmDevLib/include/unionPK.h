//	Wolfgang Wang
//	2004/11/11

#ifndef _UnionPK_1_x_
#define _UnionPK_1_x_

// ������Կǿ�ȱ�ʶ
#ifdef _UnionPK_4_x_
typedef enum 
{
	con512RSAPair = 512,
	con1024RSAPair = 1024,
	con2048RSAPair = 2048,
	con4096RSAPair = 4096,
	con1152RSAPair = 1152,
	con1408RSAPair = 1408,
	conUnknownLengthPK = 0
} TUnionPKLength;
#else
typedef enum 
{
	con512RSAPair,
	con1024RSAPair,
	con2048RSAPair,
	con4096RSAPair,
	conUnknownLengthPK
} TUnionPKLength;
#endif

/*
typedef enum
{
	conSignature,
	conEncryption,
	conSelfDefinedPK,
	conUnknownPKType
} TUnionPKType;
*/
//update hzh in 2012.11.8
typedef enum
{
	conSignature = 0,   //ָ��Ϊ65537��ǩ������Կ
	conEncryption = 1,  //ָ��Ϊ65537����Կ��������Կ
	conSelfDefinedPK = 2,  //ָ��Ϊ65537���Զ�����Կ
	conSignatureExponent3 = 10,   //ָ��Ϊ3��ǩ������Կ
	conEncryptionExponent3 = 11,   //ָ��Ϊ3����Կ��������Կ
	conSelfDefinedPKExponent3 = 12,  //ָ��Ϊ3����Կ��������Կ
	conUnknownPKType = 99
} TUnionPKType;

#ifdef _UnionPK_3_x_
typedef enum 
{
	conPKAttrFullName,
	conPKAttrValue,
	conPKAttrCheckValue,
	conPKAttrLength,
	conPKAttrType,
	conPKAttrActiveDate,
	conPKAttrMaxEffectiveDays,
	conPKAttrMaxUseTimes,
	conPKAttrUseTimes,
	conPKAttrLastUpdateTime,
	conPKAttrOldVerEffective,
	conPKAttrOldValue,
	conPKAttrOldCheckValue,
	conPKAttrWindowBetweenKeyVer
} TUnionPKAttribute;
	
// ������Կ����
typedef struct
{
	char				fullName[40+1];		// ��Կȫ��������owner.keyApp.keyName
	char				value[1024+1];		// ��Կ����
	char				checkValue[16+1];	// ��ԿУ��ֵ
	TUnionPKLength			length;			// ��Կǿ��
	TUnionPKType			type;			// ��Կ����
	char				activeDate[8+1];	// ��Կ��Ч����
	long				maxEffectiveDays;	// ��Կ�����Ч����
	long				maxUseTimes;		// ��Կ���ʹ�õĴ���
	long				useTimes;		// ʹ�ô���
	long				lastUpdateTime;		// ���һ�θ���ʱ��
	char				container[20+1];	
	
	short				oldVerEffective;	// �ɰ汾�Ƿ���Ч
	char				oldValue[1024+1];	// �ɰ汾����Կֵ
	char				oldCheckValue[16+1];	// �ɰ汾����ԿУ��ֵ
	int				windowBetweenKeyVer;	// ������Կ�汾��Ĵ�����
#ifdef _UnionPK_4_x_
	short				isWritingLocked;	// �Ƿ�д��ס��
#endif
} TUnionPK;
#else			
typedef enum 
{
	conPKAttrValue,
	conPKAttrCheckValue,
	conPKAttrLength,
	conPKAttrType,
	conPKAttrActiveDate,
	conPKAttrPassiveDate
} TUnionPKAttribute;
	
// ������Կ����
typedef struct
{
	char				fullName[40+1];		// ��Կȫ��������owner.keyApp.keyName
	char				value[1024+1];		// ��Կ����
	char				checkValue[16+1];	// ��ԿУ��ֵ
	TUnionPKLength			length;			// ��Կǿ��
	TUnionPKType			type;			// ��Կ����
	char				activeDate[8+1];	// ��Կ��Ч����
	char				passiveDate[8+1];	// ��ԿʧЧ����
} TUnionPK;
#endif
typedef TUnionPK			*PUnionPK;

typedef struct
{
	char			app[10+1];
	char			owner[40+1];
	char			name[40+1];
	char			container[20+1];
	long			usedTimes;
	long			usedDays;
	int			checkWritingLocked;
	short			checkOutdate;
	TUnionPKLength		length;
	TUnionPKType		type;
	char			activeDate[8+1];
} TUnionPKQueryCondition;
typedef TUnionPKQueryCondition	*PUnionPKQueryCondition;


// 2004/11/25 Wolfgang Wang
int UnionUsePK(PUnionPK pkey);
int UnionIsOldVerPKEffective(PUnionPK pkey);
int UnionUpdatePKValue(PUnionPK pkey,char *value,char *checkValue);
// End of addition of 2004/11/25

// ����һ����Կ����
int UnionCreatePK(char *owner,char *keyApp,char *keyName,PUnionPK pPK);
// ��ȡһ����Կ����
int UnionReadPK(char *owner,char *keyApp,char *keyName,PUnionPK pPK);
// ɾ��һ����Կ����
int UnionDeletePK(char *owner,char *keyApp,char *keyName);
// ����һ����Կ���������
int UnionUpdatePK(char *owner,char *keyApp,char *keyName,TUnionPKAttribute attr,char *attrValue);

// �ж��Ƿ��ǺϷ�����Կ����
int UnionIsValidPKLength(TUnionPKLength desKeyLen);
// �ж��Ƿ��ǺϷ�����Կ����
int UnionIsValidPKAttribute(TUnionPKAttribute attr);
// �ж��Ƿ��ǺϷ�����Կ����
int UnionIsValidPKValue(char *keyCryptogram);
// �ж��Ƿ��ǺϷ�����ԿУ��ֵ
int UnionIsValidPKCheckValue(char *checkValue);
// �ж��Ƿ��ǺϷ�����Կȫ��
int UnionIsValidPKFullName(char *fullName);
// �ж��Ƿ��ǺϷ�����Կ����
int UnionIsValidPKType(TUnionPKType type);

int UnionPrintPK(PUnionPK pPK);
int UnionIsValidPK(PUnionPK pPK);

int UnionIsPKStillEffective(PUnionPK pPK);

int UnionFormDefaultPK(PUnionPK pkey,char *fullName,char *keyValue,TUnionPKType type,char *checkValue);

int UnionGetNameOfPK(TUnionPKType type,char *keyName);

// ����Կ�����л�ȡ��������
int UnionAnalysisPKFullName(char *fullName,char *appID,char *ownerName,char *keyName);
int UnionGetAppIDOutOfPKFullName(char *fullName,char *appID);
int UnionGetKeyNameOutOfPKFullName(char *fullName,char *keyName);
int UnionGetOwnerNameOutOfPKFullName(char *fullName,char *owner);

TUnionPKLength UnionConvertPKLength(int keyLen);
TUnionPKType UnionConvertPKType(char *keyTypeName);

int UnionGetNameOfPKType(TUnionPKType type,char *nameOfType);
int UnionGetAscCharLengthOfPK(TUnionPKLength length);
int UnionGetBitsLengthOfRSAPair(TUnionPKLength length);

// ��PK��¼д�뵽�ļ���
int UnionPrintPKToRecStrTxtFile(PUnionPK pPK,FILE *fp);

// ��һ��PK��Կд�뵽����
int UnionPutPKIntoRecStr(PUnionPK pPK,char *recStr,int sizeOfRecStr);

// ��һ�����ж� PK ��Կ
int UnionReadPKFromRecStr(char *recStr,int lenOfRecStr,PUnionPK pPK);

#endif
