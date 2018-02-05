// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2004/11/20
// Version:	1.0

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

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_			// 选用3.x版本的Task模块
#endif
#include "UnionTask.h"

#include "UnionStr.h"
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
	printf(" -startdes	start a desKeyDB backup server.\n");
	printf(" -startpk	start a pkKeyDB backup server.\n");
	printf(" -active	set keyDB backup server active.\n");
	printf(" -inactive	set keyDB backup server inactive.\n");
	printf(" -reload	reload keyDB backup server configuration.\n");
	printf(" -print		print configuration of keyDB backup server.\n");
	printf(" -synchdes all|app|fullKeyName\n");
	printf("		synchronize the des keys to backup server.\n");
	printf(" -synchpk all|app|fullKeyName\n");
	printf("		synchronize the pk keys to backup server.\n");
	
	return(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	int	i;
	long	synchronizedNum = 0;
	char	tmpBuf[512];
	
	UnionSetApplicationName(argv[0]);
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,(char *)UnionGetApplicationName())) == NULL)
	{
		printf("in %s:: UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());

	/*
	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		printf("in %s:: UnionConnectKeyDBBackupServer error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	*/
	
	if (argc < 2)
		return(StartKeyDBBackupServer());
	if (strcasecmp(argv[1],"-START") == 0)
		return(StartKeyDBBackupServer());
	else if (strcasecmp(argv[1],"-ACTIVE") == 0)
	{
		if ((ret = UnionSetKeyDBBackupServerActive()) < 0)
			printf("UnionSetKeyDBBackupServerActive failure!\n");
		else
			printf("UnionSetKeyDBBackupServerActive OK!\n");
	}
	else if (strcasecmp(argv[1],"-INACTIVE") == 0)
	{
		if ((ret = UnionSetKeyDBBackupServerInactive()) < 0)
			printf("UnionSetKeyDBBackupServerInactive failure!\n");
		else
			printf("UnionSetKeyDBBackupServerInactive OK!\n");
	}
	
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
	else if (strcasecmp(argv[1],"-PRINT") == 0)
	{
		ret = UnionPrintKeyDBBackupServer();
	}
	else if (strcasecmp(argv[1],"-SYNCHDES") == 0)
	{
		if ((ret = UnionConnectDesKeyDB()) < 0)
		{
			printf("UnionConnectDesKeyDB error!\n");
			return(UnionTaskActionBeforeExit());
		}
		if ((argc >= 3) && (strcasecmp(argv[2],"all") == 0))
			synchronizedNum = UnionSynchronizeAllDesKeyToBackuper();
		else if ((argc >= 3) && (strcasecmp(argv[2],"app") == 0))
		{
			if (argc >= 4)
				synchronizedNum = UnionSynchronizeDesKeyOfSpecifiedAppToBackuper(argv[3]);
			else
			{
				printf("请输入应用编号::");
				memset(tmpBuf,0,sizeof(tmpBuf));
				scanf("%s",tmpBuf);
				synchronizedNum = UnionSynchronizeDesKeyOfSpecifiedAppToBackuper(tmpBuf);
			}		
		}
		else
		{
			for (i = 2; i < argc; i++)
			{
				if ((ret = UnionSynchronizeSpecifiedDesKeyToBackuper(argv[i])) < 0)
					printf("synchronize [%s] failure!\n",argv[i]);
				else
				{
					printf("synchronize [%s] success!\n",argv[i]);
					synchronizedNum++;
				}
			}
			if (argc <= 2)
			{
				printf("请输入密钥名称::");
				memset(tmpBuf,0,sizeof(tmpBuf));
				scanf("%s",tmpBuf);
				if ((ret = UnionSynchronizeSpecifiedDesKeyToBackuper(tmpBuf)) < 0)
					printf("synchronize [%s] failure!\n",argv[i]);
				else
				{
					printf("synchronize [%s] success!\n",argv[i]);
					synchronizedNum++;
				}
			}		
		}
		printf("%ld keys synchronized!\n",synchronizedNum);
	}
	else if (strcasecmp(argv[1],"-SYNCHPK") == 0)
	{
		if ((ret = UnionConnectPKDB()) < 0)
		{
			printf("UnionConnectPKDB error!\n");
			return(UnionTaskActionBeforeExit());
		}
		if ((argc >= 3) && (strcasecmp(argv[2],"all") == 0))
			synchronizedNum = UnionSynchronizeAllPKToBackuper();
		else if ((argc >= 3) && (strcasecmp(argv[2],"app") == 0))
		{
			if (argc >= 4)
				synchronizedNum = UnionSynchronizePKOfSpecifiedAppToBackuper(argv[3]);
			else
			{
				printf("请输入应用编号::");
				memset(tmpBuf,0,sizeof(tmpBuf));
				scanf("%s",tmpBuf);
				synchronizedNum = UnionSynchronizePKOfSpecifiedAppToBackuper(tmpBuf);
			}		
		}
		else
		{
			for (i = 2; i < argc; i++)
			{
				if ((ret = UnionSynchronizeSpecifiedPKToBackuper(argv[i])) < 0)
					printf("synchronize [%s] failure!\n",argv[i]);
				else
				{
					printf("synchronize [%s] success!\n",argv[i]);
					synchronizedNum++;
				}
			}
			if (argc <= 2)
			{
				printf("请输入密钥名称::");
				memset(tmpBuf,0,sizeof(tmpBuf));
				scanf("%s",tmpBuf);
				synchronizedNum = UnionSynchronizeSpecifiedPKToBackuper(tmpBuf);
			}		
		}
		printf("%ld keys synchronized!\n",synchronizedNum);
	}
	else
		UnionHelp();
	
	return(UnionTaskActionBeforeExit());
}

int UnionSynchTCPIPTaskServer(int handle,struct sockaddr_in *cli_addr,int port,int (*UnionTaskActionBeforeExit)())
{
	int			lenOfKeyRec;
	int			ret;
	TUnionKeyDBOperation 	operation;
	char 			keyDBType;
	unsigned char 		tmpBuf[2048+200];
		
	UnionLog("in UnionSynchTCPIPTaskServer:: clientIPAddr [%s] Port [%d] Connected!\n",inet_ntoa(cli_addr->sin_addr),port);
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
		return(-1);
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
	usleep(5000);
	return(0);
}

int StartKeyDBBackupServer()
{
	int			ret;
	
	if (UnionCreateProcess() > 0)
	{
		return(UnionTaskActionBeforeExit());
	}
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s %d",UnionGetApplicationName(),UnionGetPortOfMineOfKeyDBBackupServer())) == NULL)
	{
		printf("in %s:: UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}

	UnionSetAsKeyDBBackupServer();
	
	if ((ret = UnionTCPIPServer(UnionGetPortOfMineOfKeyDBBackupServer(),(char *)UnionGetApplicationName(),UnionSynchTCPIPTaskServer,UnionTaskActionBeforeExit)) < 0)
	{
		UnionUserErrLog("in %s:: UnionSynchTCPIPSvr [%s] Error! ret = [%d]\n",UnionGetApplicationName(),UnionGetPortOfMineOfKeyDBBackupServer(),ret);
		//return(UnionTaskActionBeforeExit());
	}

	return(UnionTaskActionBeforeExit());
}

