//	Author:		Wolfgang Wang
//	Date:		2007/8/3

#ifndef _UnionKDBSvrService_
#define _UnionKDBSvrService_

#include "time.h"
#include "keyDBBackuper.h"

#define conKDBSvrRequest				1000000000
#define conKDBSvrKDBImageRequest			1110000000
#define conKDBSvrKeySynchronizeRequest			1100000000	// 2007/11/30增加该变量
#define conKDBSvrKeySynchronizeResultSpierRequest 	1200000000	// 2007/12/25增加该变量

#define conMDLNameOfKDBSvrBuf	"UnionKDBSvrMDL"

#define conSizeOfEachKDBSvrRec	(sizeof(TUnionKDBSvrServiceHeader) + sizeof(unsigned char) * pgunionKDBSvrBufHDL->maxSizeOfRec)

// 2007/12/25 增加
typedef struct
{
	char			time[10+1];
	TUnionKeyDBOperation	operation;
	char			ipAddr[15+1];	// 被同步到的服务器地址
	int			port;		// 被同步到的服务器端口
	char			fullKeyName[40+1];	// 被同步的密钥名称
	int			resCode;	// 同步动作的执行结果
} TUnionKeySynchronizeResult;
typedef TUnionKeySynchronizeResult	*PUnionKeySynchronizeResult;

typedef struct
{
	long			type;
	char			keyDBType;
	int			applierIsKeyDBBackuper;
	TUnionKeyDBOperation	operation;
	time_t			time;
	int			provider;
	int			dealer;
	long			index;
	int			lenOfRec;	// 记录的长度，对于响应是错误代码,0为请求被正确执行
} TUnionKDBSvrServiceHeader;
typedef TUnionKDBSvrServiceHeader	*PUnionKDBSvrServiceHeader;

typedef struct
{
	int			maxSizeOfRec;
	int			maxNumOfRec;
	int			timeout;
} TUnionKDBSvrBufHDL;
typedef TUnionKDBSvrBufHDL	*PUnionKDBSvrBufHDL;

int UnionApplyKDBImageService(TUnionKeyDBOperation operation,char keyDBType,unsigned char *keyRec,int sizeOfKeyRec);

int UnionReadKDBImageServiceRequest(TUnionKeyDBOperation *operation,char *keyDBType,unsigned char *keyRec,int sizeOfRecBuf);

long UnionFormKDBSvrServiceUniqueKey(PUnionKDBSvrServiceHeader pheader);

PUnionKDBSvrServiceHeader UnionGetKDBSvrServiceHeaderOfIndex(int index);

int UnionIsKDBSvrBufMDLConnected();

int UnionGetFileNameOfKDBSvrBufDef(char *fileName);

int UnionGetTimeoutOfKDBSvrBufMDL();

int UnionGetTotalNumOfKDBSvrBufMDL();

int UnionInitKDBSvrBufDef(PUnionKDBSvrBufHDL pdef);

int UnionReconnectKDBSvrBufMDLAnyway();

int UnionConnectKDBSvrBufMDL();
	
int UnionDisconnectKDBSvrBufMDL();

int UnionRemoveKDBSvrBufMDL();

int UnionReloadKDBSvrBufDef();

int UnionApplyKDBService(TUnionKeyDBOperation operation,char keyDBType,unsigned char *keyRec,int sizeOfKeyRec);

int UnionReadKDBServiceRequest(TUnionKeyDBOperation *operation,char *keyDBType,unsigned char *keyRec,int sizeOfRecBuf);

int UnionAnswerKDBService(int errCode);

int UnionPrintAvailablKDBSvrBufPosToFile(FILE *fp);

int UnionPrintInavailabeKDBSvrBufPosToFile(FILE *fp);

int UnionPrintKDBSvrBufStatusToFile(FILE *fp);

int UnionPrintKDBSvrBufToFile(FILE *fp);

int UnionResetAllKDBRecPos();

int UnionClearKDBSvrBufRubbish();

int UnionResetKDBServiceTimeout(int timeout);

// 2007/11/30，增加
// 申请一个异步的密钥同步服务
int UnionApplyKeySynchronizeServiceNowait(TUnionKeyDBOperation operation,char keyDBType,unsigned char *keyRec,int sizeOfKeyRec);

// 2007/11/30，王纯军增加
// 读取一个密钥同步请求
int UnionReadKeySynchronizeServiceRequest(TUnionKeyDBOperation *operation,char *keyDBType,unsigned char *keyRec,int sizeOfRecBuf);

// 2007/12/25，王纯军增加
// 读取一个密钥同步监控请求
int UnionReadKeySynchronizeResultSpierRequest(PUnionKeySynchronizeResult presult);

// 2009/9/19，王纯军增加
void UnionSetCurrentKeyName(char *keyName);

// 2009/9/19，王纯军增加
int UnionSpierKeyDBService(int operation,int resCode);

// 2009/9/19，王纯军增加
int UnionSpierKeyDBServiceWithFullInfo(char *ipAddr,int port,char *fullName,int operation,int resCode);

#endif

