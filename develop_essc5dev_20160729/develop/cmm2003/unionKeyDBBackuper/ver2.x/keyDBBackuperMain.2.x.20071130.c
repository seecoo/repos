// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2004/11/20
// Version:	1.0

// 2007/11/29 增加了-chkalldeskeys命令
// 2007/11/30，在keyDBBackuperMain.2.x.20070807.c的基础上升级成本程序
// 将原功能分折成了客户端功能和服务端功能。
// 本程序仅保留了服务端功能，将客户端功能，封装在了keyDBSynchronizeMain.2.x.20071130.c中。

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
#include "unionKeyDB.h"
#include "unionDesKeyDB.h"
#include "unionCommand.h"

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_			// 选用3.x版本的Task模块
#endif
#include "UnionTask.h"

#include "UnionStr.h"
#include "unionVersion.h"
#include "UnionLog.h"

int 			gsynchTCPIPSvrSckHDL;
PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	if (gsynchTCPIPSvrSckHDL >= 0)
		UnionCloseSocket(gsynchTCPIPSvrSckHDL);
	UnionDisconnectKeyDBBackupServer();
	UnionDisconnectDesKeyDB();
	UnionDisconnectPKDB();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s [command]\n",UnionGetApplicationName());
	printf(" Where command as follow:\n");
	printf(" -start		start a KDB backup server.\n");
	
	return(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	int	i;
	long	synchronizedNum = 0;
	char	tmpBuf[512];
	int	varNum;
	int	varIndex;
	char	*ptr;
	
	UnionSetApplicationName(argv[0]);
	
	if (argc < 2)
	{
		UnionHelp();
		return(errCodeUserRequestHelpInfo);
	}
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("in StartKeySynchronizingServer:: UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-START") == 0)
		return(StartKeyDBBackupServer());
	else if (strcasecmp(argv[1],"-RELOAD") == 0)
	{
		if (UnionConfirm("Are you sure of reloading keyDBBackuper?"))
		{
			if ((ret = UnionLoadKeyDBBackupServerIntoMemory()) < 0)
				printf("UnionLoadKeyDBBackupServerIntoMemory failure!\n");
			else
				printf("UnionLoadKeyDBBackupServerIntoMemory OK!\n");
		}
	}
	else if (strcasecmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if ((ret = UnionLoadKeyDBBackupServerIntoMemory()) < 0)
			printf("UnionLoadKeyDBBackupServerIntoMemory failure!\n");
		else
			printf("UnionLoadKeyDBBackupServerIntoMemory OK!\n");
	}
	else
		UnionHelp();

finishNow:
	return(UnionTaskActionBeforeExit());
}

int UnionSynchTCPIPTaskServer(int handle,struct sockaddr_in *cli_addr,int port,int (*UnionTaskActionBeforeExit)())
{
	int			lenOfKeyRec;
	int			ret;
	TUnionKeyDBOperation 	operation;
	char 			keyDBType;
	unsigned char 		tmpBuf[2048+200];
		
	UnionProgramerLog("in UnionSynchTCPIPTaskServer:: clientIPAddr [%s] Port [%d] Connected!\n",inet_ntoa(cli_addr->sin_addr),port);
	memset(tmpBuf,0,sizeof(tmpBuf));
	bcdhex_to_aschex(((struct sockaddr *)cli_addr)->sa_data,14,(char *)tmpBuf);
	
	gsynchTCPIPSvrSckHDL = handle;
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s %d",inet_ntoa(cli_addr->sin_addr),port)) == NULL)
	{
		UnionUserErrLog("in UnionSynchTCPIPTaskServer::UnionCreateTaskInstance Error!\n");
		return(-1);
	}

	if ((lenOfKeyRec = UnionReadSynchronizingKeyDBOperation(handle,&operation,&keyDBType,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionSynchTCPIPTaskServer::UnionReadSynchronizingKeyDBOperation Error!\n");
		return(lenOfKeyRec);
	}
	switch (keyDBType)
	{
		case conIsDesKeyDB:
			if ((ret = UnionDesKeyDBOperation(operation,(PUnionDesKey)tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionDesKeyDBOperation!\n");
				UnionAnswerKeyDBSynchronizingRequest(handle,"01");
			}
			else
				UnionAnswerKeyDBSynchronizingRequest(handle,"00");
			break;
		case conIsPKDB:
			if ((ret = UnionPKDBOperation(operation,(PUnionDesKey)tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionPKDBOperation!\n");
				UnionAnswerKeyDBSynchronizingRequest(handle,"01");
			}
			else
				UnionAnswerKeyDBSynchronizingRequest(handle,"00");
			break;
		default:
			UnionAnswerKeyDBSynchronizingRequest(handle,"02");
			break;
	}
	usleep(50000);
	return(0);
}

int StartKeyDBBackupServer()
{
	int			ret;
	char			taskName[128+1];
	
	sprintf(taskName,"%s %d",UnionGetApplicationName(),UnionGetPortOfMineOfKeyDBBackupServer());
	if (UnionExistsAnotherTaskOfName(taskName))
	{
		printf("Another task of name [%s] already exists!\n",taskName);
		return(errCodeTaskMDL_AnotherTaskOfNameExists);
	}
	if (UnionCreateProcess() > 0)
	{
		return(UnionTaskActionBeforeExit());
	}
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,taskName)) == NULL)
	{
		printf("in %s:: UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}

	UnionSetAsKeyDBBackupServer();
	
	if ((ret = UnionTCPIPServer(UnionGetPortOfMineOfKeyDBBackupServer(),(char *)UnionGetApplicationName(),UnionSynchTCPIPTaskServer,UnionTaskActionBeforeExit)) < 0)
	{
		UnionUserErrLog("in %s:: UnionSynchTCPIPSvr [%d] Error! ret = [%d]\n",UnionGetApplicationName(),UnionGetPortOfMineOfKeyDBBackupServer(),ret);
		//return(UnionTaskActionBeforeExit());
	}

	return(UnionTaskActionBeforeExit());
}

