// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2005/12/30
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "unionFunSvr.h"
#include "UnionTask.h"
#include "unionVersion.h"
#include "unionFunSvrName.h"
#include "unionMDLID.h"
#include "unionResID.h"
#include "unionREC.h"
#include "unionCommand.h"
#include "commWithHsmSvr.h"
#include "unionRealDBCommon.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
        // signal(SIGUSR2,SIG_IGN); // add by xusj 20100105,屏蔽SIGUSR2信号
        // alarm(0); // add by xusj 20100105,屏蔽警钟信号
//	UnionDisconnectHsmTask(); // add by xusj 20100305

	// 2010-7-13，王纯军增加
#ifdef _useRealDB_
        UnionCloseDatabase();
#endif
	//UnionDisconnectMsgBufMDL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s [hsmGroupID]\n",UnionGetApplicationName());
	return(0);
}

int main(int argc,char *argv[])
{
	char	varName[100];
	long	mdlID = 0;

	//if (argc < 2)
	//	return(UnionHelp());
		
	UnionSetApplicationName(argv[0]);
	
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);
	
	if (UnionCreateProcess() > 0)
		return(0);

	if (argc <= 1)
	{
		if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
		{
			UnionPrintf("in %s:: UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
			return(UnionTaskActionBeforeExit());
		}
		UnionSetHsmGroupIDForHsmSvr("");
	}
	else 
	{
#ifdef _useArgv1AsTaskName_
		if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",argv[1])) == NULL)
#else
		if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s %s",UnionGetApplicationName(),argv[1])) == NULL)
#endif		
		{
			UnionPrintf("in %s:: UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
			return(UnionTaskActionBeforeExit());
		}
		UnionSetDefaultHsm(argv[1]);
	}

	UnionSetFunSvrName(argv[0]);
	sprintf(varName,"fixedMDLIDOf%s",argv[0]);
	//UnionSetFixedMDLIDOfFunSvr(UnionReadLongTypeRECVar(varName));
	//modify by hzh in 2012.7.13 这里需增加判断。当$varName变量不存在时，UnionReadLongTypeRECVar会返回-20002, 消息类型为负时会把小于其绝对值消息类型的消息读走，造成串包的后果
	if((mdlID = UnionReadLongTypeRECVar(varName)) <= 0)
	{
		UnionPrintf("in %s:: unionREC varName[%s] not exists!\n",UnionGetApplicationName(),varName);
		return(UnionTaskActionBeforeExit());
	}
	UnionSetFixedMDLIDOfFunSvr(mdlID);
	//modify end
	
	UnionSetMyModuleType(conMDLTypeUnionFunSvr);

	UnionStartFunSvr();
	
	return(UnionTaskActionBeforeExit());
}

