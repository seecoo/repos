//	Wolfgang Wang
//	2003/05/01

/*
	2004/11/25 ����Ϊ 3.0
	�ı���UnionDesKey�Ľṹ
	�Ǻ궨�� _UnionDesKey_3_x_ �����Ľṹ
*/

#ifndef _UnionDesKey_1_x_
#define _UnionDesKey_1_x_

//#include "unionKeyDB.h"
#include "sjl06LMK.h"
#include "unionErrCode.h"


// ������Կǿ�ȱ�ʶ
typedef enum 
{
	con64BitsDesKey,
	con128BitsDesKey,
	con192BitsDesKey,
	conUnknownLengthDesKey
} TUnionDesKeyLength;

typedef enum 
{
	conProtectedByLMK0001,	
	conProtectedByLMK0203,
	conProtectedByLMK0405,
	conProtectedByLMK0607,
	conProtectedByLMK0809,
	conProtectedByLMK1011,
	conProtectedByLMK1213,
	conProtectedByLMK1415,
	conProtectedByLMK1617,
	conProtectedByLMK1819,
	conProtectedByLMK2021,
	conProtectedByLMK2223,
	conProtectedByLMK2425,
	conProtectedByLMK2627,
	conProtectedByLMK2829,
	conProtectedByLMK3031,
	conProtectedByLMK3233,
	conProtectedByLMK3435,
	conProtectedByLMK3637,
	conProtectedByLMK3839,
	conProtectedByZMK,
	conUnknownProtectMethod
} TUnionDesKeyProtectMethod;

typedef enum
{
	conZPK,
	conZAK,
	conZMK,
	conTMK,
	conTPK,
	conTAK,
	conPVK,
	conCVK,
	conZEK,
	conWWK,
	conBDK,
	conSelfDefinedKey,
	conEDK,
	conUnknownDesKeyType,
	conMKAC,
	conMKSMC,
	conMKSMI,
	conKMCSeed = 22,
	conKMUSeed = 23,
	conMDKSeed = 21,
} TUnionDesKeyType;
			
#ifdef _UnionDesKey_3_x_
typedef enum 
{
	conDesKeyFullName,
	conDesKeyAttrValue,
	conDesKeyAttrCheckValue,
	conDesKeyAttrLength,
	//conDesKeyAttrProtectMethod,
	conDesKeyAttrType,
	conDesKeyAttrActiveDate,
	conDesKeyAttrMaxEffectiveDays,
	conDesKeyAttrMaxUseTimes,
	conDesKeyAttrUseTimes,
	conDesKeyAttrLastUpdateTime,
	conDesKeyAttrOldVerEffective,
	conDesKeyAttrOldValue,
	conDesKeyAttrOldCheckValue,
	conDesKeyAttrWindowBetweenKeyVer,
	conDesKeyAttrContainer
} TUnionDesKeyAttribute;
typedef struct
{
	char				fullName[40+1];		// ��Կȫ��������owner.keyApp.keyName
	char				value[48+1];		// ��Կ����
	char				checkValue[16+1];	// ��ԿУ��ֵ
	TUnionDesKeyLength		length;			// ��Կǿ��
	//TUnionDesKeyProtectMethod	protectMethod;		// ��Կ��������
	TUnionDesKeyType		type;			// ��Կ����
	char				activeDate[8+1];	// ��Կ��Ч����
	long				maxEffectiveDays;	// ��Կ�����Ч����
	long				maxUseTimes;		// ��Կ���ʹ�õĴ���
	long				useTimes;		// ʹ�ô���
	long				lastUpdateTime;		// ���һ�θ���ʱ��
	char				container[20+1];	// 

	short				oldVerEffective;	// �ɰ汾�Ƿ���Ч
	char				oldValue[48+1];		// �ɰ汾����Կֵ
	char				oldCheckValue[16+1];	// �ɰ汾����ԿУ��ֵ
	int				windowBetweenKeyVer;	// ������Կ�汾��Ĵ�����
#ifdef _UnionDesKey_4_x_
	short				isWritingLocked;	// �Ƿ�д��ס��
#endif
} TUnionDesKey;	
#else	
typedef enum 
{
	conDesKeyAttrValue,
	conDesKeyAttrCheckValue,
	conDesKeyAttrLength,
	//conDesKeyAttrProtectMethod,
	conDesKeyAttrType,
	conDesKeyAttrActiveDate,
	conDesKeyAttrPassiveDate
} TUnionDesKeyAttribute;

// ������Կ����
typedef struct
{
	char				fullName[40+1];		// ��Կȫ��������owner.keyApp.keyName
	char				value[48+1];		// ��Կ����
	char				checkValue[16+1];	// ��ԿУ��ֵ
	TUnionDesKeyLength		length;			// ��Կǿ��
	//TUnionDesKeyProtectMethod	protectMethod;		// ��Կ��������
	TUnionDesKeyType		type;			// ��Կ����
	char				activeDate[8+1];		// ��Կ��Ч����
	char				passiveDate[8+1];		// ��ԿʧЧ����
} TUnionDesKey;
#endif
typedef TUnionDesKey			*PUnionDesKey;

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
	TUnionDesKeyLength	length;
	TUnionDesKeyType	type;
	char			activeDate[8+1];
} TUnionDesKeyQueryCondition;
typedef TUnionDesKeyQueryCondition	*PUnionDesKeyQueryCondition;

// add by wuhy at 20130820
typedef struct
{
	char                            fullName[40+1];         // ��Կȫ��������owner.keyApp.keyName
	int                             version;                // �汾��
	char                            value[48+1];            // ��Կ����
	char                            checkValue[16+1];       // ��ԿУ��ֵ
	TUnionDesKeyLength              length;                 // ��Կǿ��
	int                             keyTransmitProtectMode; // ��Կ���䱣������
	TUnionDesKeyType                type;                   // ��Կ��;
	int                             AlgorithmType;          // ��Կ�㷨����
	char                            keyDBID[20+1];          // �������ʶ
	char                            activeDate[8+1];        // ��Կ��Ч����
	long                            maxEffectiveDays;       // ��Կ�����Ч����
	long                            maxUseTimes;            // ��Կ���ʹ�õĴ���
	long                            useTimes;               // ʹ�ô���
	long                            lastUpdateTime;         // ���һ�θ���ʱ��
	char                            container[20+1];        //

	short                           oldVerEffective;        // �ɰ汾�Ƿ���Ч
	char                            oldValue[48+1];         // �ɰ汾����Կֵ
	char                            oldCheckValue[16+1];    // �ɰ汾����ԿУ��ֵ
	int                             windowBetweenKeyVer;    // ������Կ�汾��Ĵ�����
	short                           isWritingLocked;        // �Ƿ�д��ס��
}TUnionSymmetricKey;
typedef TUnionSymmetricKey		*PUnionSymmetricKey;
// end add


// 2004/11/25 Wolfgang Wang
int UnionUseDesKey(PUnionDesKey pkey);
int UnionUpdateDesKeyValue(PUnionDesKey pkey,char *value,char *checkValue);
int UnionIsOldVerDesKeyEffective(PUnionDesKey pkey);
// End of addition of 2004/11/25

// ����һ����Կ����
int UnionCreateDesKey(char *owner,char *keyApp,char *keyName,PUnionDesKey pDesKey);
// ��ȡһ����Կ����
int UnionReadDesKey(char *owner,char *keyApp,char *keyName,PUnionDesKey pDesKey);
// ɾ��һ����Կ����
int UnionDeleteDesKey(char *owner,char *keyApp,char *keyName);
// ����һ����Կ���������
int UnionUpdateDesKeyAttr(char *owner,char *keyApp,char *keyName,TUnionDesKeyAttribute attr,char *attrValue);

// �ж��Ƿ��ǺϷ�����Կ����
int UnionIsValidDesKeyLength(TUnionDesKeyLength desKeyLen);
// �ж��Ƿ��ǺϷ�����Կ������ʽ
int UnionIsValidDesKeyProtectMethod(TUnionDesKeyProtectMethod method);
// �ж��Ƿ��ǺϷ�����Կ����
int UnionIsValidDesKeyAttribute(TUnionDesKeyAttribute attr);
// �ж��Ƿ��ǺϷ�����Կ����
int UnionIsValidDesKeyCryptogram(char *keyCryptogram);
// �ж��Ƿ��ǺϷ�����ԿУ��ֵ
int UnionIsValidDesKeyCheckValue(char *checkValue);
// �ж��Ƿ��ǺϷ�����Կȫ��
int UnionIsValidDesKeyFullName(char *fullName);
// �ж��Ƿ��ǺϷ�����Կ����
int UnionIsValidDesKeyType(TUnionDesKeyType type);

int UnionPrintDesKey(PUnionDesKey pDesKey);
int UnionIsValidDesKey(PUnionDesKey pDesKey);

int UnionIsDesKeyStillEffective(PUnionDesKey pDesKey);

int UnionFormDefaultDesKey(PUnionDesKey pkey,char *fullName,char *keyValue,TUnionDesKeyType type,char *checkValue);

int UnionGetNameOfDesKey(TUnionDesKeyType type,char *keyName);

// ����Կ�����л�ȡ��������
int UnionAnalysisDesKeyFullName(char *fullName,char *appID,char *ownerName,char *keyName);
int UnionGetAppIDOutOfDesKeyFullName(char *fullName,char *appID);
int UnionGetKeyNameOutOfDesKeyFullName(char *fullName,char *keyName);
int UnionGetOwnerNameOutOfDesKeyFullName(char *fullName,char *owner);

TUnionDesKeyLength UnionConvertDesKeyLength(int keyLen);
TUnionDesKeyType UnionConvertDesKeyType(char *keyTypeName);

TUnionSJL06LMKPairIndex UnionGetProtectLMKPairOfDesKeyType(TUnionDesKeyType type);

int UnionGetNameOfDesKeyType(TUnionDesKeyType type,char *nameOfType);

int UnionGetDesKeyBitsLength(TUnionDesKeyLength length);

// �ж�ĳһ��Կ�Ƿ����ڶ�̬���µ���Կ
int UnionIsDesKeyValueDynamicUpdated(PUnionDesKey pkey);

// 2008/7/18����
// ��һ��des��Կд�뵽����
int UnionPutDesKeyIntoRecStr(PUnionDesKey pDesKey,char *recStr,int sizeOfRecStr);

// 2008/7/17����
// ��DES��¼д�뵽�ļ���
int UnionPrintDesKeyToRecStrTxtFile(PUnionDesKey pDesKey,FILE *fp);

// 2008/7/18����
// ��һ�����ж���Կ
int UnionReadDesKeyFromRecStr(char *recStr,int lenOfRecStr,PUnionDesKey pDesKey);

#endif
