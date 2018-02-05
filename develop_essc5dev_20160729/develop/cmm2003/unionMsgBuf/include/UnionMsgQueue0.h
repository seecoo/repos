//	Author:		Wolfgang Wang
//	Created Date:	2002/2/1

#ifndef _UnionMsgQueue_1_x_
#define _UnionMsgQueue_1_x_

#define gMsgQueueMaxQueueNum		20
typedef struct
{
	int QueueID;		// 
	int MsgQueueID;		// Identification of a message queue;
	int CurrentMsgNum;	// Current Message Num in Queue;
	int CurrentCharNum;	// Current Chars in Queue;
} TUnionMsgQueueRec;
typedef TUnionMsgQueueRec		*PUnionMsgQueueRec;

int UnionConnectMsgQueueMDL(char *FileName);
int UnionDisconnectMsgQueueMDL();

int UnionMsgQueueRefreshAllRec();
long UnionMsgQueueOutputAllRec(unsigned char *Buf,int SizeOfBuf);

// Error Codes
#define UnionErrNoOffsetForMsgQueue	-10000
#define gErrMsgQueueSmallBuf		(UnionErrNoOffsetForMsgQueue-1)
#define gErrMsgQueueNullPointer		(UnionErrNoOffsetForMsgQueue-2)

#endif
