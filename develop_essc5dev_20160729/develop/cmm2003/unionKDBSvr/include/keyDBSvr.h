//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2004/11/20

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "unionModule.h"
//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2007/6/9

#ifndef _keyDBSvr_
#define _keyDBSvr_

char *UnionGetIPAddrOfKDBSvr();

int UnionGetPortOfKDBSvr();

int UnionGetTimeoutOfKDBSvr();

int UnionApplyKDBService(TUnionKeyDBOperation operation,char keyDBType,unsigned char *keyRec,int sizeOfKeyRec);

int UnionReadKDBServiceRequest(int handle,TUnionKeyDBOperation *operation,char *keyDBType,unsigned char *keyRec,int sizeOfKeyRecBuf);

int UnionAnswerKDBService(int handle,int errCode);

#endif
