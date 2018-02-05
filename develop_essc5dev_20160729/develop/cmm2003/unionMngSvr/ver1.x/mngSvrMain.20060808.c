// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/3/19
// Version:	1.0

// 2007/12/27 修改
/*
	在程序中增加了编译开关选项_LINUX_，用于控制在linux下，调用sigsetjmp，siglongjmp，而在其它操作系统下，仍调用setjmp/longjmp
*/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <setjmp.h>
#include <signal.h>

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif
#include "UnionTask.h"
#ifndef _UnionSocket_3_x_
#define _UnionSocket_3_x_
#endif

#include "UnionSocket.h"
#include "unionCommConf.h"
#include "commWithTransSpier.h"
#include "mngSvrServicePackage.h"
#include "unionResID.h"
#include "unionMDLID.h"
#include "errCodeTranslater.h"
#include "UnionLog.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionSynchTCPIPTaskServer(int handle,struct sockaddr_in *cli_addr,int port,int (*UnionTaskActionBeforeExit)());

int UnionTaskActionBeforeExit()
{
	UnionFreeUserSpecResource();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	char	taskName[100];
	int	port;
	
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	
	// 2010-01-22,Wolfgang Wang
	port = atoi(argv[1]);
	memset(taskName,0,sizeof(taskName));
	UnionFormTaskNameOfMngSvr(port,taskName);
	// end of addition of 2010-01-22

	if (UnionExistsAnotherTaskOfName(taskName) > 0)
	{
		printf("Another task of name [%s] already exists!\n",taskName);
		return(UnionTaskActionBeforeExit());
	}

	if (UnionCreateProcess() > 0)
		return(0);

	UnionSetMyResIDToTransSpier(conResIDMngService);
	UnionSetMyModuleType(conMDLTypeUnionMngSvr);

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,taskName)) == NULL)
	{
		UnionPrintf("in argv[0]::UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	
	if ((ret = UnionTCPIPServer(port,argv[0],UnionSynchTCPIPTaskServer,UnionTaskActionBeforeExit)) < 0)
	{
		UnionUserErrLog("in argv[0]:: UnionSynchTCPIPSvr [%d] Error! ret = [%d]\n",port,ret);
		return(UnionTaskActionBeforeExit());
	}

	return(UnionTaskActionBeforeExit());
}

int UnionHelp()
{
	printf("Usage:: %s port\n",UnionGetApplicationName());
	return(0);
}

