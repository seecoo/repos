// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2008/5/11
// Version:	1.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#ifndef _UnionSocket_3_x_
#define _UnionSocket_3_x_
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "UnionLog.h"
#include "UnionSocket.h"
#include "synchTCPIPSvr.h"
//#include "esscMDLID.h"
#include "unionMDLID.h"
#include "UnionTask.h"
#include "unionCommConf.h"
#include "unionFunSvrName.h"
#include "unionREC.h"
#include "unionVersion.h"
#include "unionCommand.h"
#include "commWithHsmSvr.h"

#ifndef _noRegisterClient_
extern PUnionCommConf	pgunionCommConf;
#endif

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	int	sckHDL;
	
	if ((sckHDL = UnionGetActiveTCIPSvrSckHDL()) >= 0)
		UnionCloseSocket(sckHDL);
#ifndef _noRegisterClient_
	UnionSetCommConfAbnormal(pgunionCommConf);
#endif	
	//UnionDisconnectMsgBufMDL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s port hsmGroupID\n",UnionGetApplicationName());
	return(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	char	taskName[40];

	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);
	
	sprintf(taskName,"%s %d",UnionGetApplicationName(),atoi(argv[1]));
	
	if (UnionExistsAnotherTaskOfName(taskName) > 0)
	{
		printf("Another task of name [%s] already exists!\n",taskName);
		return(UnionTaskActionBeforeExit());
	}

	if (UnionCreateProcess() > 0)
		return(0);
	
	UnionSetFunSvrName(argv[2]);
	UnionSetHsmGroupIDForHsmSvr(argv[2]);
	UnionSetMyModuleType(conMDLTypeUnionLongConnTcpipSvr);
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,taskName)) == NULL)
	{
		UnionPrintf("in %s:: UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}

	if ((ret = UnionConnectRunningEnv()) < 0)
	{
		UnionPrintf("in %s:: UnionConnectRunningEnv!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	if ((ret = UnionTCPIPServer(atoi(argv[1]),argv[0],UnionSynchTCPIPTaskServer,UnionTaskActionBeforeExit)) < 0)
	{
		UnionUserErrLog("in %s:: UnionSynchTCPIPSvr [%s] Error! ret = [%d]\n",UnionGetApplicationName(),argv[1],ret);
	}

	return(UnionTaskActionBeforeExit());
}

