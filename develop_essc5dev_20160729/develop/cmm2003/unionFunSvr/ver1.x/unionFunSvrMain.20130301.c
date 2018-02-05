// Copyright:	Union Tech.
// Author:	张永定
// Date:	2013/1/8
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif
#define _realBaseDB_2_x_

#include "UnionTask.h"
#include "unionVersion.h"
#include "unionFunSvrName.h"
#include "unionFunSvr.h"
#include "unionMDLID.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "unionRealDBCommon.h"
#include "unionCommand.h"
#include "unionHighCachedAPI.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
        //signal(SIGUSR2,SIG_IGN);
        //alarm(0);

        UnionCloseDatabase();
	UnionCloseHighCachedService();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s 服务类型\n",UnionGetApplicationName());
	return(0);
}

int main(int argc,char *argv[])
{
	int		ret;
	char		sql[256];
	char		tmpBuf[128];
	TUnionModuleID	thisMDLID = 0;

	UnionSetApplicationName(argv[0]);
	
	if (argc > 1)
		if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
			return(0);

	if (argc < 2)
		return(UnionHelp());

	if (UnionCreateProcess() > 0)
		return(0);

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s %s",UnionGetApplicationName(),argv[1])) == NULL)
	{
		UnionPrintf("in %s:: UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}

	sleep(1);
	
	// 读取服务类型
	snprintf(sql,sizeof(sql),"select mdlID,serviceClass from serviceType where serviceType = '%s'",argv[1]);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) <= 0)
	{
		UnionPrintf("in %s:: serviceType[%s] not found! sql=[%s] ret = %d\n",UnionGetApplicationName(),argv[1],sql, ret);
		return(UnionTaskActionBeforeExit());
	}
	
	UnionLocateXMLPackage("detail", 1);

	if ((ret = UnionReadXMLPackageValue("mdlID",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionPrintf("in %s:: UnionReadXMLPackageValue[mdlID]!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}

	if ((thisMDLID = atol(tmpBuf)) < 0)
	{
		UnionPrintf("in %s:: thisMDLID[%d] Error!\n",thisMDLID);
		return(UnionTaskActionBeforeExit());
	}
		
	if (thisMDLID < conMDLTypeOffsetOfTcpAndTask)
		thisMDLID += conMDLTypeOffsetOfTcpAndTask;

	if ((ret = UnionReadXMLPackageValue("serviceClass",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionPrintf("in %s:: UnionReadXMLPackageValue[serviceClass]!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	UnionSetFunSvrName(argv[0]);
	UnionSetFixedMDLIDOfFunSvr(thisMDLID);
	
	UnionSetMyModuleType(conMDLTypeUnionFunSvr);

	UnionSetFunSvrStartVar(tmpBuf);
	
	UnionStartFunSvr();
	
	return(UnionTaskActionBeforeExit());
}

