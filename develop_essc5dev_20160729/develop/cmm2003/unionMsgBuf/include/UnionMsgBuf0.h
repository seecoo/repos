//	Author:		Wolfgang Wang
//	Date:		2003/09/26
//	Version:	4.0

#ifndef _UnionMsgBuf4_x_
#define _UnionMsgBuf4_x_

#include "time.h"

#define conMDLNameOfMsgBuf	"UnionMsgBufMDL"

#define conMsgSizeInMsgBuf	(sizeof(TUnionMessageHeader) + sizeof(unsigned char) * pgunionMsgBufHDL->msgBufDef.maxSizeOfMsg)

typedef struct
{
	long	type;		// 消息的标识号,不可以为<=0的数
	int	provider;	// 消息提供者的进程号
	time_t	time;		// 置放消息的时间
	int	len;		// 消息的长度
	long	msgIndex;	// 消息的索引号，由本模块分配
	int	dealer;		// 消息的处理者
	short	locked;		// 锁
} TUnionMessageHeader;
typedef TUnionMessageHeader	*PUnionMessageHeader;

typedef struct
{
	long	maxSizeOfMsg;			// 一条消息的最大长度
	int	maxNumOfMsg;			// 消息的最大数目
	long	msgNumExchangedPerSeconds;	// 每秒交换的信息最大数量
	long	maxReadWaitTime;		// 读取一条消息时等待的最大时间
	long	maxWriteWaitTime;		// 写一条消息时等待的最大时间
	int	maxStayTime;			// 一条消息在缓冲存在的最大时间，秒计
	long	maxMsgIndex;			// 消息的最大索引号
	long	intervalWhenReadingFailure;	// 读失败的等待时间，以微秒计
	long	intervalWhenWritingFailure;	// 写失败的等待时间，以微秒计
} TUnionMsgBufDef;
typedef TUnionMsgBufDef		*PUnionMsgBufDef;
typedef struct
{
	TUnionMsgBufDef		msgBufDef;
	long			currentMsgIndex;	// 当前索引号
	unsigned char		*pmsgBuf;		// 其大小为：
			// (sizeof(TUnionMessageHeaer) + sizeof(unsigned char) * maxSizeOfMsg) * maxNumOfMsg
} TUnionMsgBufHDL;
typedef TUnionMsgBufHDL		*PUnionMsgBufHDL;

int UnionGetFileNameOfMsgBufDef(char *fileName);
int UnionInitMsgBufDef(PUnionMsgBufDef pdef);

int UnionGetTotalNumOfMsgBufMDL();
int UnionGetFreePosNumOfMsgBufMDL();

int UnionConnectMsgBufMDL();
int UnionDisconnectMsgBufMDL();
int UnionRemoveMsgBufMDL();
int UnionReloadMsgBufDef();

int UnionPrintAvailablMsgBufPosToFile(FILE *fp);
int UnionPrintInavailabeMsgBufPosToFile(FILE *fp);
int UnionPrintMsgBufStatusToFile(FILE *fp);
int UnionPrintMsgBufToFile(FILE *fp);

long UnionWriteNewMessage(unsigned char *msg,int lenOfMsg,long typeOfMsg,PUnionMessageHeader pmsgHeader);
long UnionWriteNewMessageUntilSuccess(unsigned char *msg,int lenOfMsg,long typeOfMsg,PUnionMessageHeader pmsgHeader);

int UnionWriteOriginMessage(unsigned char *msg,int lenOfMsg,PUnionMessageHeader pmsgHeader);
int UnionWriteOriginMessageUntilSuccess(unsigned char *msg,int lenOfMsg,PUnionMessageHeader pmsgHeader);

int UnionReadMsgOfSpecifiedType(unsigned char *msg,int sizeOfMsg,long typeOfMsg,PUnionMessageHeader pmsgHeader);
int UnionReadMsgOfSpecifiedTypeUntilSuccess(unsigned char *msg,int sizeOfMsg,long typeOfMsg,PUnionMessageHeader pmsgHeader);
int UnionReadFirstMsg(unsigned char *msg,int sizeOfMsg,long *typeOfMsg,PUnionMessageHeader pmsgHeader);
int UnionReadFirstMsgUntilSuccess(unsigned char *msg,int sizeOfMsg,long *typeOfMsg,PUnionMessageHeader pmsgHeader);
int UnionReadMsgOfSpecifiedMsgIndex(unsigned char *msg,int sizeOfMsg,long msgIndex,PUnionMessageHeader pmsgHeader);
int UnionReadMsgOfSpecifiedMsgIndexUntilSuccess(unsigned char *msg,int sizeOfMsg,long msgIndex,PUnionMessageHeader pmsgHeader);

int UnionReadOriginMessage(unsigned char *msg,int sizeOfMsg,PUnionMessageHeader pmsgHeader);
int UnionReadOriginMessageUntilSuccess(unsigned char *msg,int sizeOfMsg,PUnionMessageHeader pmsgHeader);

int UnionFreeRubbishMsg();

int UnionPrintMessageHeaderToFile(PUnionMessageHeader pmsgHeader,FILE *fp);
int UnionPrintStatusOfMsgPosToFile(long index,FILE *fp);

int UnionLockMessageHeader(PUnionMessageHeader pmsgHeader);
int UnionUnlockMessageHeader(PUnionMessageHeader pmsgHeader);

#endif

