// Author:	Wolfgang Wang
// Date:	2003/09/24

// Version 2.x 2003/09/24,Wolfgang Wang,ÓÉunionKeyDBInMemory.h¸Ä³É

#ifndef _UnionDesKeyDBLog_
#define _UnionDesKeyDBLog_

#include "unionDesKey.h"
#include "unionErrCode.h"

typedef enum
{
	conRecordInsert,
	conRecordDelete,
	conRecordUpdate
} TUnionDesKeyDBLogOperation;

typedef struct
{
	TUnionDesKeyDBLogOperation	operation;
	char				time[14+1];
	TUnionDesKey			desKey;
} TUnionDesKeyDBLogRecord;
typedef TUnionDesKeyDBLogRecord		*PUnionDesKeyDBLogRecord;

int UnionLogDesKeyDBOperation(TUnionDesKeyDBLogOperation operation,PUnionDesKey pdesKey);
int UnionPrintDesKeyDBLog(char *startTime,char *endTime);
int UnionRestoreDesKeyDBFromLog(char *startTime,char *endTime);

int UnionOpenRestoringDesKeyDBFromLog();
int UnionCloseRestoringDesKeyDBFromLog();

int UnionSynchronizeDesKeyDBOperation(TUnionDesKeyDBLogOperation operation,PUnionDesKey pdesKey);

#endif
