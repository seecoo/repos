// Author:	Wolfgang Wang
// Date:	2004/11/20


#ifndef _keyDBBackuper_
#define _keyDBBackuper_

#define conMDLNameOfUnionKeyDBBackupServer	"KeyDBBackupServerMDL"

#define conMaxNumOfKeyDBBackuper	20

#define conIsDesKeyDB			'1'
#define conIsPKDB			'2'

#define conTaskNameOfKDBSynchronizerSpier	"kdbSynchronizeSpier"	// 2007/12/25增加

typedef enum
{
	conInsertRecord,
	conDeleteRecord,
	conUpdateRecord,
	conUpdateRecordAllFlds,
	conUpdateRecordValueFlds,
	conUpdateRecordAttrFlds,
	conCompareKeyValue,		// 2007/11/29增加，比较密钥值
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

// 20060825 增加
int UnionUpdateLocalKeyDBBackuperDef(char *ipAddr,int port);

// 20060825 增加
int UnionUpdateRemoteKeyDBBackuperDef(char *ipAddr,int port);

// 20070808增加
#ifdef _keyDBBackuper_2_x_
int UnionUpdateSpecRemoteKeyDBBackuperDef(int indexOfBackuper,char *ipAddr,int port,int active);

int UnionSetDefaultDBBackuperBrother(char *ipAddr);

PUnionKeyDBBackuper UnionFindDBBackuperBrother(char *ipAddr);

int UnionSynchronizeKeyDBOperation(TUnionKeyDBOperation operation,char keyDBType,unsigned char *keyRec,int sizeOfKeyRec);

int UnionSetSpecDBBackuperBrotherStatus(char *ipAddr,int status);
int UnionSetSpecDBBackuperBrotherActive(char *ipAddr);
int UnionSetSpecDBBackuperBrotherInactive(char *ipAddr);

// 2007/11/28增加
// 与指定的备份服务器比较密钥值
int UnionCompareDesKeyValueWithSpecBrother(char *ipAddr,char *fullKeyName,char *keyByLmk);
// 2007/11/28增加
// 与所有的备份服务器比较密钥值
int UnionCompareDesKeyValueWithBrothers(char *fullKeyName,char *keyByLmk);

// 2007/12/11增加
int UnionMaintainStatusOfKeyDBBrothers();

// 2007/12/11增加
// 统计所有密钥值不相同的备份服务器，返回值是不同步的备份服务器的数量
// ipAddr，是不同步的备份服务器的IP地址。
int UnionCountAllUnsameDesKeyBrothers(char *fullKeyName,char *keyByLmk,char ipAddr[][15+1],int maxNumOfIPAddr);

// 2007/12/11增加
// 判断是否发生过与备份服务器建立连接失败的情况
int UnionIsCreateSckToKeyDBBackuperFailure();

// 2007/12/11 增加
// 置与备份服务器建立连接失败次数为0
void UnionResetCreateSckToKeyDBBackuperFailure();

#endif

#endif
