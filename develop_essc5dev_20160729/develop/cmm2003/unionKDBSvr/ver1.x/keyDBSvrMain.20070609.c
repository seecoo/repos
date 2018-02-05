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
#include "keyDBSvr.h"
#include "unionKeyDB.h"
#include "unionDesKeyDB.h"
#include "unionPK.h"
#include "unionPKDB.h"

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
	int			sckHDL;
	int			sckConn;
	
	UnionSetApplicationName(argv[0]);
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);
	
	sprintf(tmpBuf,"%s %d",UnionGetApplicationName(),port = UnionGetPortOfKDBSvr());
	if (UnionExistsAnotherTaskOfName(tmpBuf))
	{
		printf("in %s:: another task of name [%s] already exists!\n",UnionGetApplicationName(),tmpBuf);
		return(errCodeParameter);
	}
	if ((ret = UnionCreateProcess()) > 0)
		return(0);
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,tmpBuf)) == NULL)
	{
		printf("in %s:: UnionCreateTaskInstance [%s] Error!\n",UnionGetApplicationName(),tmpBuf);
		return(UnionTaskActionBeforeExit());
	}
	
	// 初始化连接
	if ((sckHDL = UnionInitializeTCPIPServer(port)) < 0)
	{
		printf("in %s:: UnionInitializeTCPIPServer [%d] Error!\n",UnionGetApplicationName(),port);
		return(UnionTaskActionBeforeExit());
	}
	
loop:
	if ((sckConn = UnionNewTCPIPConnection(sckHDL)) < 0)
	{
		UnionUserErrLog("in %s:: UnionNewTCPIPConnection!\n",UnionGetApplicationName());
		UnionCloseSocket(sckHDL);
		return(sckConn);
	}
	UnionLog("one client connected!\n");
	for (;;)
	{
		if ((lenOfKeyRec = UnionReadKDBServiceRequest(sckConn,&operation,&keyDBType,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionExcuteTask::UnionReadKDBServiceRequest Error!\n");
			break;
		}
		UnionLog("req = [%c][%02d][%04ld]\n",keyDBType,operation,lenOfKeyRec);
		if (lenOfKeyRec == 0)
			break;
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
				ret = errCodeParameter;
				break;
		}
		if ((ret = UnionAnswerKDBService(sckConn,ret)) < 0)
		{
			UnionUserErrLog("in UnionExcuteTask:: UnionAnswerKDBService!\n");
			break;
		}
		UnionLog("answer ok!\n");
	}
	UnionCloseSocket(sckConn);
	goto loop;
}
