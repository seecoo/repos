//	Wolfgang Wang, 2004/7/23

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "UnionStr.h"
#include "UnionLog.h"		// 使用3.x版本
#include "unionFileTransfer.h"
#include "UnionTask.h"
#include "unionVersion.h"
#include "unionREC.h"
#include "unionCommand.h"
#include "UnionSocket.h"
PUnionTaskInstance		ptaskInstance = NULL;

PUnionFileTransferHandle	pgfileTransferHandle = NULL;
PUnionFileReceiveServer		pfileReceiveServer = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s [port]\n",UnionGetApplicationName());
	return(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	int	port = 0;
	char	taskName[128+1];
	int			socket = 0;
	struct sockaddr_in	cli_addr;
	int			clilen;
	char	ipAddr[40+1];
	
	UnionSetApplicationName(argv[0]);
	
	if (argc < 2)
	{
		if ((port = UnionReadIntTypeRECVar("portOfFileReceiver")) <= 0)
		{
			printf("UnionReadIntTypeRECVar[portOfFileReceiver] Error!\n");
			return(0);
		}
		//return(UnionHelp());
	}
	else
		port = atoi(argv[1]);
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",argv[0])) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
		
	if (UnionCreateProcess() > 0)
		return(UnionTaskActionBeforeExit());
	
	memset(taskName,0,sizeof(taskName));
	if (argc < 2)
		strcpy(taskName,argv[0]);
	else
		sprintf(taskName,"%s %d",argv[0],port);
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",taskName)) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	
	if ((pfileReceiveServer = UnionInitFileReceiveServer(port)) == NULL)
	{
		printf("in %s:: UnionInitFileReceiveServer[%d] Error!\n",taskName,port);
		return(UnionTaskActionBeforeExit());
	}

	for (;;)
	{
		// Accept a Client's Connecting reqeust.
		clilen = sizeof(cli_addr);
		socket = accept(pfileReceiveServer->socket, (struct sockaddr *)&cli_addr,(socklen_t *)&clilen);
		if (socket < 0 )
		{
			printf("in %s:: accept() error!\n",taskName);
			break;
		}
		memset(ipAddr,0,sizeof(ipAddr));
		inet_ntop(AF_INET, (void *)&cli_addr.sin_addr, ipAddr, sizeof(ipAddr));

		
		// fork独立进程
		switch (UnionCreateProcess())
		{
			// 子进程
			case 0:
				UnionCloseSocket(pfileReceiveServer->socket);
				pfileReceiveServer->socket = -1;//close listen socket fd
				if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",taskName)) == NULL)
                                {
                                        UnionUserErrLog("in %s:: UnionCreateTaskInstance Error!\n",taskName);
                                        return(errCodeCreateTaskInstance);
                                }
				if ((pgfileTransferHandle = UnionAcceptFileTransferCall(socket,ipAddr,pfileReceiveServer)) == NULL)
				{
					UnionUserErrLog("in %s:: UnionAcceptFileTransferCall Error!\n",taskName);
					goto child_exit;
				}
				if (UnionIsFileReceiver() == 1)
				{
					if ((ret = UnionReceiveFile(pgfileTransferHandle)) < 0)
						UnionUserErrLog("in %s::UnionReceiveFile failure!\n",taskName);
					else
						UnionLog("in %s::UnionReceiveFile OK!\n",taskName);
				}
				else if (UnionIsFileReceiver() == 0)
				{
					if ((ret = UnionTransferFile(pgfileTransferHandle)) < 0)
						UnionUserErrLog("in %s::UnionTransferFile failure!\n",taskName);
					else
						UnionLog("in %s::UnionTransferFile OK!\n",taskName);
				}
				else 
					UnionLog("in::cmd OK!\n");
child_exit:
				UnionCloseSocket(socket);
				UnionReleaseFileTransferHandle(pgfileTransferHandle);
				UnionReleaseFileReceiveServer(pfileReceiveServer);	
				return(UnionTaskActionBeforeExit());
			case -1:
				UnionReleaseFileTransferHandle(pgfileTransferHandle);
				UnionUserErrLog("in %s:: UnionCreateProcess!\n",taskName);
				continue;
			// 父进程
			default:
				UnionCloseSocket(socket);
				continue;
		}
	}

	UnionReleaseFileReceiveServer(pfileReceiveServer);	
	return(UnionTaskActionBeforeExit());
}
