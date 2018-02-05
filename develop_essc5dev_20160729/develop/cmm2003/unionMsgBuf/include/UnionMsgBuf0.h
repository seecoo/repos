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
	long	type;		// ��Ϣ�ı�ʶ��,������Ϊ<=0����
	int	provider;	// ��Ϣ�ṩ�ߵĽ��̺�
	time_t	time;		// �÷���Ϣ��ʱ��
	int	len;		// ��Ϣ�ĳ���
	long	msgIndex;	// ��Ϣ�������ţ��ɱ�ģ�����
	int	dealer;		// ��Ϣ�Ĵ�����
	short	locked;		// ��
} TUnionMessageHeader;
typedef TUnionMessageHeader	*PUnionMessageHeader;

typedef struct
{
	long	maxSizeOfMsg;			// һ����Ϣ����󳤶�
	int	maxNumOfMsg;			// ��Ϣ�������Ŀ
	long	msgNumExchangedPerSeconds;	// ÿ�뽻������Ϣ�������
	long	maxReadWaitTime;		// ��ȡһ����Ϣʱ�ȴ������ʱ��
	long	maxWriteWaitTime;		// дһ����Ϣʱ�ȴ������ʱ��
	int	maxStayTime;			// һ����Ϣ�ڻ�����ڵ����ʱ�䣬���
	long	maxMsgIndex;			// ��Ϣ�����������
	long	intervalWhenReadingFailure;	// ��ʧ�ܵĵȴ�ʱ�䣬��΢���
	long	intervalWhenWritingFailure;	// дʧ�ܵĵȴ�ʱ�䣬��΢���
} TUnionMsgBufDef;
typedef TUnionMsgBufDef		*PUnionMsgBufDef;
typedef struct
{
	TUnionMsgBufDef		msgBufDef;
	long			currentMsgIndex;	// ��ǰ������
	unsigned char		*pmsgBuf;		// ���СΪ��
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

