// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2005/12/30
// Version:	1.0

#include <stdio.h>
#include <string.h>

#include "transSpierBuf.h"
#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif
#include "UnionTask.h"
#include "UnionLog.h"

extern PUnionTaskInstance	ptaskInstance;

int UnionTaskActionBeforeExit();

int UnionConnectRunningEnv()
{
	char	taskName[100];
	int	port;
	
	// 2010-01-22,Wolfgang Wang
	sprintf(taskName,"isBackupMngSvrPort%d",port = UnionGetShortConnTcipSvrPort());
	//printf("[%s]\n",taskName);
	if (UnionReadIntTypeRECVar(taskName) > 0)
	{
		UnionSetAsBackupMngSvr();
		UnionLog("set mngSvr of %d as backuper!\n",port);		
		sprintf(taskName,"%s %d backup",UnionGetApplicationName(),port);
		UnionCreateTaskInstance(UnionTaskActionBeforeExit,taskName);
	}
	else 
	{
		//printf("[%s]\n",taskName);
		sprintf(taskName,"isBackMngSvrPort%d",port);
		if (UnionReadIntTypeRECVar(taskName) > 0)
		{
			UnionSetAsBackMngSvr();		
			sprintf(taskName,"%s %d back",UnionGetApplicationName(),port);
			UnionCreateTaskInstance(UnionTaskActionBeforeExit,taskName);
		}
	}
	// end of addition of 2010-01-22
	return(0);
}
