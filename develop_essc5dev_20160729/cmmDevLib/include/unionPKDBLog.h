// Author:	Wolfgang Wang
// Date:	2003/09/24

// Version 2.x 2003/09/24,Wolfgang Wang,ÓÉunionKeyDBInMemory.h¸Ä³É

#ifndef _UnionPKDBLog_
#define _UnionPKDBLog_

#include "unionPK.h"

typedef enum
{
	conRecordInsert,
	conRecordDelete,
	conRecordUpdate
} TUnionPKDBLogOperation;

typedef struct
{
	TUnionPKDBLogOperation	operation;
	char				time[14+1];
	TUnionPK			key;
} TUnionPKDBLogRecord;
typedef TUnionPKDBLogRecord		*PUnionPKDBLogRecord;

int UnionLogPKDBOperation(TUnionPKDBLogOperation operation,PUnionPK pkey);
int UnionPrintPKDBLog(char *startTime,char *endTime);
int UnionRestorePKDBFromLog(char *startTime,char *endTime);

int UnionOpenRestoringPKDBFromLog();
int UnionCloseRestoringPKDBFromLog();

int UnionSynchronizePKDBOperation(TUnionPKDBLogOperation operation,PUnionPK ppk);

#endif
