//	Author:		chenwd
//	Copyright:	Union Tech. Guangzhou
//	Date:		2015-01-15

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "UnionTask.h"
#include "unionVersion.h"
#include "UnionStr.h"
#include "UnionLog.h"
#include "UnionSocket.h"
#include "unionCommand.h"
#include "unionMDLID.h"
#include "unionMsgBufGroup.h"
#include "unionRealDBCommon.h"
#include "unionCommBetweenMDL.h"

#include "unionDataSync.h"

PUnionTaskInstance ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage:: %s \n",UnionGetApplicationName());
	return(0);
}

int UnionTaskActionBeforeExit()
{
	//UnionCloseDatabase();	//modify by leipp 20150923
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
        exit(0);
}

int main(int argc, char *argv[])
{
	int	ret;

	unsigned char		msg[81920];

	UnionSetApplicationName(argv[0]);

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	if (UnionCreateProcess() > 0)
		return(0);

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if ((ret = UnionReloadDataSyncIni()) < 0)
	{
		UnionUserErrLog("in %s:: UnionReloadDataSyncIni ret = [%d]!\n",UnionGetApplicationName(), ret);
		return(UnionTaskActionBeforeExit());
	}

	//建立连接
	while(1)
	{
		//从消息队列读取同步数据
		if ((ret = UnionDataSyncRecvMsg(msg,sizeof(msg))) < 0)
		{
			UnionUserErrLog("in %s:: UnionDataSyncRecvMsg!\n", UnionGetApplicationName());
			break;
		}

		if ((ret = UnionDataSyncSendToSocket(0,msg,ret)) < 0)
		{
			UnionUserErrLog("in %s:: UnionDataSyncSendToSocket!\n", UnionGetApplicationName());
			UnionDataSyncCloseSocket();
			continue;
		}
	}

	UnionDataSyncCloseSocket();

	return(UnionTaskActionBeforeExit());
}

