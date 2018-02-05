#ifndef _dataSyncIni_
#define _dataSyncIni_

#ifndef _unionFileTransfer_2_x_
#define _unionFileTransfer_2_x_
#endif

#define DB_SYNC_NUM	10

typedef enum
{
	conSQL,			// 仅SQL
	conCache,		// 仅Cache
	conSQLAndCache,		// SQL+Cache
	conSymmetricKeyMsg	// 对称密钥信息		//add by zhouxw 20150910
}TUnionDataSyncDataType;
typedef TUnionDataSyncDataType		*PUnionDataSyncDataType;

typedef enum
{
	conNotWaitResult,	// 不等待执行结果
	conWaitResult		// 等待执行结果
}TUnionDataSyncResultFlag;
typedef TUnionDataSyncResultFlag		*PUnionDataSyncResultFlag;

typedef struct
{
	int	localResourcesNeedSync;	// 本地资源是否同步，比如HSM
	int	highCachedNeedSync;	// 备咚倩捍媸欠裢剑热鏼emcached
	char	sendIPAddr[64];		// 同步平台接收IP地址
	int	sendPort;		// 同步平台接收端口
//	int	fileRecvPort;		// 同步平台文件接收端口
	int	socket_no;
}TUnionDataSyncAddr;
typedef TUnionDataSyncAddr	*PUnionDataSyncAddr;

typedef struct
{
	int	localPort;		// 本地平台接收端口
	int	syncNum;
	TUnionDataSyncAddr	syncAddr[DB_SYNC_NUM];		
}TUnionDataSyncIni;
typedef TUnionDataSyncIni		*PUnionDataSyncIni;

void UnionSetDataSyncIsWaitResult();

void UnionSetDataSyncIsNotWaitResult();

int UnionDataSyncIsWaitResult();

/*
功能	读取unionDBSync.ini信息
输入参数
	无
输出参数
	无
返回值
	=0		正确
	<0		错误
*/
int UnionReloadDataSyncIni();

/*
功能	发送msg到消息队列
输入参数
	msg		发送信息内容
	lenOfMsg	消息长度
	dataType	数据类型
	resultFlag	结果标识
输出参数
	无
返回值
	>=		成功
	<0		失败
*/
int UnionDataSyncSendMsg(unsigned char *msg,int lenOfMsg,TUnionDataSyncDataType dataType,TUnionDataSyncResultFlag resultFlag);

/*
功能	从消息队列取msg
输入参数
	无
输出参数
	msg		获取到的消息
	sizeOfMsg	获取到的消息长度
返回值
	>=		成功
	<0		失败
*/
int UnionDataSyncRecvMsg(unsigned char *msg,int sizeOfMsg);

/*
功能	将发送失败记录写入到文件
输入参数
	psyncAddr	
	msg		失败的记录信息
	sizeOfMsg	信息长度
输出参数
	无
返回值
	>=0		成功
	<0		失败
*/
int UnionDataSyncWriteFile(PUnionDataSyncAddr psyncAddr,unsigned char *msg,int lenOfMsg);

int UnionDataSyncSendToSocket(int closeSocket,unsigned char *msg,int lenOfMsg);

int UnionDataSyncCloseSocket();

int UnionGetDataSyncLocalPort();

PUnionDataSyncIni getDataSyncIniPtr();

#endif

