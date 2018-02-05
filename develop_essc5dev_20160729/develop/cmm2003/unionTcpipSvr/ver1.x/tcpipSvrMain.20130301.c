// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2008/5/11
// Version:	1.0

#include <stdio.h>
#include <string.h>

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#ifndef _UnionSocket_3_x_
#define _UnionSocket_3_x_
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_   
#endif

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_   
#endif

#include "UnionLog.h"
#include "UnionSocket.h"
#include "synchTCPIPSvr.h"
#include "unionMDLID.h"
#include "UnionTask.h"
#include "unionCommConf.h"
#include "unionFunSvrName.h"
#include "unionREC.h"
#include "unionVersion.h"
#include "unionCommand.h"
#include "unionMsgBuf.h"
#include "unionHighCachedAPI.h"
#include "unionRealBaseDB.h"
#include "unionRealDBCommon.h"

#ifndef _noRegisterClient_
extern PUnionCommConf	pgunionCommConf;
#endif

int UnionConnectRunningEnv();

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage:: %s port [funSvrName]\n",UnionGetApplicationName());
	return(0);
}

int UnionTaskActionBeforeExit()
{
	int	sckHDL;
	
	if ((sckHDL = UnionGetActiveTCIPSvrSckHDL()) >= 0)
		UnionCloseSocket(sckHDL);
#ifndef _noRegisterClient_
	UnionSetCommConfAbnormal(pgunionCommConf);
#endif	
	UnionDisconnectMsgBufMDL();
	UnionCloseDatabase();
	UnionCloseHighCachedService();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	char	taskName[40];
	char	varName[100];

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
	
	if (argc > 2)
	{
		UnionSetFunSvrName(argv[2]);
		sprintf(varName,"fixedMDLIDOf%s",argv[2]);
		//UnionProgramerLog("in %s:: value of [%s] = [%ld]\n",UnionGetApplicationName(),varName,UnionReadLongTypeRECVar(varName));
		UnionSetFixedMDLIDOfFunSvr(UnionReadLongTypeRECVar(varName));
		UnionSetMyModuleType(conMDLTypeUnionLongConnTcpipSvr);
	}
	
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

