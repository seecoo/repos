// Author:	Wolfgang Wang
// Date:	2004/11/20


#ifndef _keyDBBackuper_
#define _keyDBBackuper_

#define conMDLNameOfUnionKeyDBBackupServer	"KeyDBBackupServerMDL"

#define conMaxNumOfKeyDBBackuper	20

#define conIsDesKeyDB			'1'
#define conIsPKDB			'2'

#define conTaskNameOfKDBSynchronizerSpier	"kdbSynchronizeSpier"	// 2007/12/25����

typedef enum
{
	conInsertRecord,
	conDeleteRecord,
	conUpdateRecord,
	conUpdateRecordAllFlds,
	conUpdateRecordValueFlds,
	conUpdateRecordAttrFlds,
	conCompareKeyValue,		// 2007/11/29���ӣ��Ƚ���Կֵ
} TUnionKeyDBOperation;
typedef TUnionKeyDBOperation		*PUnionKeyDBOperation;

typedef struct
{
	char			keyDBType;
	TUnionKeyDBOperation	operation;
	int			lenOfKeyRec;
} TUnionKeyDBBackuperRecordHeader;
typedef TUnionKeyDBBackuperRecordHeader	*PUnionKeyDBBackuperRecordHeader;

typedef struct
{
	char		ipAddr[15+1];
	int		port;
#ifdef _keyDBBackuper_2_x_
	int		active;
#endif
} TUnionKeyDBBackuper;
typedef TUnionKeyDBBackuper		*PUnionKeyDBBackuper;

typedef struct
{
	int			active;
	TUnionKeyDBBackuper	myself;
#ifdef _keyDBBackuper_2_x_
	int			realNum;
	TUnionKeyDBBackuper	brother[conMaxNumOfKeyDBBackuper];
#else
	TUnionKeyDBBackuper	brother;
#endif
} TUnionKeyDBBackupServer;
typedef TUnionKeyDBBackupServer	*PUnionKeyDBBackupServer;

int UnionGetNameOfKeyDBBackupServer(char *fileName);

int UnionConnectKeyDBBackupServer();
int UnionDisconnectKeyDBBackupServer();
int UnionRemoveKeyDBBackupServerInMemory();

int UnionLoadKeyDBBackupServerIntoMemory();
int UnionPrintKeyDBBackupServer();
int UnionPrintKeyDBBackupServerToFile(FILE *fp);
int UnionPrintStatusOfKeyDBBackupServerToFile(FILE *fp);

int UnionSetKeyDBBackupServerActive();
int UnionSetKeyDBBackupServerInactive();

int UnionIsKeyDBBackupServer();
int UnionSetAsKeyDBBackupServer();

int UnionGetPortOfMineOfKeyDBBackupServer();

int UnionSynchronizeKeyDBOperation(TUnionKeyDBOperation operation,char keyDBType,unsigned char *keyRec,int sizeOfKeyRec);

int UnionReadSynchronizingKeyDBOperation(int handle,TUnionKeyDBOperation *operation,char *keyDBType,unsigned char *keyRec,int sizeOfKeyRecBufBuf);
int UnionAnswerKeyDBSynchronizingRequest(int handle,char *errCode);


int UnionPrintKeyDBBackupServerToSpecFile(char *fileName);

// 20060825 ����
int UnionUpdateLocalKeyDBBackuperDef(char *ipAddr,int port);

// 20060825 ����
int UnionUpdateRemoteKeyDBBackuperDef(char *ipAddr,int port);

// 20070808����
#ifdef _keyDBBackuper_2_x_
int UnionUpdateSpecRemoteKeyDBBackuperDef(int indexOfBackuper,char *ipAddr,int port,int active);

int UnionSetDefaultDBBackuperBrother(char *ipAddr);

PUnionKeyDBBackuper UnionFindDBBackuperBrother(char *ipAddr);

int UnionSynchronizeKeyDBOperation(TUnionKeyDBOperation operation,char keyDBType,unsigned char *keyRec,int sizeOfKeyRec);

int UnionSetSpecDBBackuperBrotherStatus(char *ipAddr,int status);
int UnionSetSpecDBBackuperBrotherActive(char *ipAddr);
int UnionSetSpecDBBackuperBrotherInactive(char *ipAddr);

// 2007/11/28����
// ��ָ���ı��ݷ������Ƚ���Կֵ
int UnionCompareDesKeyValueWithSpecBrother(char *ipAddr,char *fullKeyName,char *keyByLmk);
// 2007/11/28����
// �����еı��ݷ������Ƚ���Կֵ
int UnionCompareDesKeyValueWithBrothers(char *fullKeyName,char *keyByLmk);

// 2007/12/11����
int UnionMaintainStatusOfKeyDBBrothers();

// 2007/12/11����
// ͳ��������Կֵ����ͬ�ı��ݷ�����������ֵ�ǲ�ͬ���ı��ݷ�����������
// ipAddr���ǲ�ͬ���ı��ݷ�������IP��ַ��
int UnionCountAllUnsameDesKeyBrothers(char *fullKeyName,char *keyByLmk,char ipAddr[][15+1],int maxNumOfIPAddr);

// 2007/12/11����
// �ж��Ƿ������뱸�ݷ�������������ʧ�ܵ����
int UnionIsCreateSckToKeyDBBackuperFailure();

// 2007/12/11 ����
// ���뱸�ݷ�������������ʧ�ܴ���Ϊ0
void UnionResetCreateSckToKeyDBBackuperFailure();

#endif

#endif
