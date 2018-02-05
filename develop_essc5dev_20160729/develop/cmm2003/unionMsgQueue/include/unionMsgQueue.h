//	Author:		Wolfgang Wang
//	Created Date:	2002/2/1

#ifndef _UnionMsgQueue_1_x_
#define _UnionMsgQueue_1_x_

#include <string.h>

#define conMaxNumOfMsgQueue		20
typedef struct
{
	int userID;		// 
	int systemID;		// Identification of a message queue;
	int msgNum;	// Current Message Num in Queue;
	int charNum;	// Current Chars in Queue;
} TUnionMsgQueueRec;
typedef TUnionMsgQueueRec		*PUnionMsgQueueRec;

typedef struct
{
	int			num;
	TUnionMsgQueueRec	queue[conMaxNumOfMsgQueue];
} TUnionMsgQueueGrp;
typedef TUnionMsgQueueGrp	*PUnionMsgQueueGrp;

// 2008/11/25,王纯军增加
PUnionMsgQueueGrp UnionGetAllMsgQueueStatus();

int UnionConnectMsgQueueMDLInEnvFile();

int UnionConnectMsgQueueMDL();
int UnionDisconnectMsgQueueMDL();
int UnionQueryAllMsgQueueStatus();

#endif
