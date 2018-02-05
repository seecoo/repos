#ifndef _unionMsg_h_
#define _unionMsg_h_

#define MAX_MSG_DATA 4096
#define UNION_QUEUE_PERM 0600

typedef struct
{
	long 	type;
	char	data[MAX_MSG_DATA];
} TUnionQueueData;
typedef TUnionQueueData *PUnionQueueData;

int UnionInitQueue(int resID);
int UnionConnectQueue(int userID);
int UnionRemoveQueue(int userID);
int UnionClearQueue(int resID);
int UnionRcvQueueData(int resID, PUnionQueueData pqueue, int queueSize, long type, int flag);
int UnionSndQueueData(int resID, PUnionQueueData pqueue, int dataLen, int flag);
int UnionPrintQueueInfo(int resID);
int UnionMaintainQueueData(int resID, int timeout);
int UnionGetQueueCurrentNum(int resID);
int UnionRcvQueueDataWithTimeout(int resID, PUnionQueueData pqueue, int queueSize, long type, int timeout);

#endif
