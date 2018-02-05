// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2005/12/30
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "UnionTask.h"
#include "unionVersion.h"
#include "unionFunSvrName.h"
#include "unionMDLID.h"
#include "unionResID.h"
#include "unionFunSvr.h"
#include "unionREC.h"
#include "unionCommand.h"
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


int main(int argc,char *argv[])
{
	char	varName[128];

	long	hsmTaskID=0;
	char	tmp[128];
	char	*ptr = tmp;
	char	**ptr1 = &ptr;
	
	UnionSetApplicationName(argv[0]);
	
	if (argc > 1)
	{
		snprintf(tmp,sizeof(tmp),"%s",argv[1]);
		if (UnionExcutePreDefinedCommand(argc-1,ptr1))
		{
			return(0);
		}
	}
	
	if ( (UnionCreateProcess()) > 0)
		return(0);
	
	if (argc <= 1)
	{
		if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
		{
			UnionPrintf("in %s:: UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
			return(UnionTaskActionBeforeExit());
		}
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
	}

	UnionSetFunSvrName(argv[0]);
	snprintf(varName,sizeof(varName),"fixedMDLIDOf%s",argv[0]);
	//UnionSetFixedMDLIDOfFunSvr(UnionReadLongTypeRECVar(varName));
	//modify by hzh in 2012.7.13 这里需增加判断。当$varName变量不存在时，UnionReadLongTypeRECVar会返回-20002, 消息类型为负时会把小于其绝对值消息类型的消息读走，造成串包的后果
	if((hsmTaskID =UnionReadLongTypeRECVar(varName)) <= 0)
	{
		UnionPrintf("unionREC varName[%s] not exists!\n",varName);
		return(UnionTaskActionBeforeExit());
	}
	UnionSetFixedMDLIDOfFunSvr(hsmTaskID);
	//modify end
	
	UnionSetMyModuleType(conMDLTypeUnionFunSvr);
	if (argc > 1)
		UnionSetFunSvrStartVar(argv[1]);
	else
		UnionSetFunSvrStartVar("");
	
	UnionStartFunSvr();
	
	return(UnionTaskActionBeforeExit());
}

int UnionHelp()
{
	printf("Usage:: %s [var]\n",UnionGetApplicationName());
	return(0);
}
