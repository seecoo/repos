// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2004/11/20
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#ifndef _UnionSocket_3_x_
#define _UnionSocket_3_x_
#endif
#include "UnionSocket.h"

#include "keyDBBackuper.h"
#include "kdbSvrService.h"
#include "unionKeyDB.h"
#include "unionDesKeyDB.h"
#include "unionPK.h"
#include "unionPKDB.h"
#include "unionVersion.h"

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_			// 选用3.x版本的Task模块
#endif
#include "UnionTask.h"

#include "UnionStr.h"
#include "UnionLog.h"

int 			gsynchTCPIPSvrSckHDL;
PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	if (gsynchTCPIPSvrSckHDL >= 0)
		UnionCloseSocket(gsynchTCPIPSvrSckHDL);
	UnionDisconnectDesKeyDB();
	UnionDisconnectPKDB();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s\n",UnionGetApplicationName());
	return(0);
}

int main(int argc,char *argv[])
{
	int			lenOfKeyRec;
	int			ret;
	TUnionKeyDBOperation 	operation;
	char 			keyDBType;
	unsigned char 		tmpBuf[4096+200];
	int			port;
	
	UnionSetApplicationName(argv[0]);
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);
	
	if (UnionExistsAnotherTaskOfName(UnionGetApplicationName()))
	{
		printf("in %s:: another task of name [%s] already exists!\n",UnionGetApplicationName(),UnionGetApplicationName());
		return(errCodeParameter);
	}
	if ((ret = UnionCreateProcess()) > 0)
		return(0);
	
	UnionSetProgramControlDesKeyDB();
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,UnionGetApplicationName())) == NULL)
	{
		printf("in %s:: UnionCreateTaskInstance [%s] Error!\n",UnionGetApplicationName(),UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	for (;;)
	{
		if ((lenOfKeyRec = UnionReadKDBServiceRequest(&operation,&keyDBType,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionExcuteTask::UnionReadKDBServiceRequest Error!\n");
			ret = lenOfKeyRec;
			break;
		}
		if (lenOfKeyRec == 0)
		{
			ret = errCodeParameter;
			goto answerNow;
		}
		switch (keyDBType)
		{
			case conIsDesKeyDB:
				if ((ret = UnionDesKeyDBOperation(operation,(PUnionDesKey)tmpBuf)) < 0)
					UnionUserErrLog("in UnionExcuteTask:: UnionDesKeyDBOperation!\n");
				break;
			case conIsPKDB:
				if ((ret = UnionPKDBOperation(operation,(PUnionPK)tmpBuf)) < 0)
					UnionUserErrLog("in UnionExcuteTask:: UnionPKDBOperation!\n");
				break;
			default:
				UnionUserErrLog("in UnionExcuteTask:: invalid keyDBType [%c]\n",keyDBType);
				ret = errCodeParameter;
				break;
		}
answerNow:
		if ((ret = UnionAnswerKDBService(ret)) < 0)
		{
			UnionUserErrLog("in UnionExcuteTask:: UnionAnswerKDBService!\n");
			break;
		}
	}
	return(ret);
}
