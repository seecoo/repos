// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2004/11/20
// Version:	1.0

// 2007/11/29 增加了-chkalldeskeys命令

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
	printf(" -startdes	start a desKeyDB backup server.\n");
	printf(" -startpk	start a pkKeyDB backup server.\n");
	printf(" -active	set keyDB backup server active.\n");
	printf(" -inactive	set keyDB backup server inactive.\n");
	printf(" -sbactive [brotherIPAddr ...]\n");
	printf("		set keyDB backup brother active.\n");
	printf(" -sbinactive	[brotherIPAddr ...]\n");
	printf("		set keyDB backup brother inactive.\n");
	printf(" -reload	reload keyDB backup server configuration.\n");
	printf(" -print		print configuration of keyDB backup server.\n");
	printf(" -synchdes [brotherIPAddr] all|app|fullKeyName\n");
	printf("		synchronize the des keys to brother.\n");
	printf(" -synchpk [brotherIPAddr] all|app|fullKeyName\n");
	printf("		synchronize the pk keys to brother.\n");
	printf(" -chkalldeskeys	check all des keys.\n");
	
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
	else if (strcasecmp(argv[1],"-SBACTIVE") == 0)
	{
		for (varIndex = 2; varIndex < argc; varIndex++)
		{
			ptr = argv[varIndex];
			if (!UnionConfirm("Are you sure of setting DBBackuper [%s] active?",ptr))
				continue;
			if ((ret = UnionSetSpecDBBackuperBrotherActive(ptr)) < 0)
				printf("UnionSetSpecDBBackuperBrotherActive [%s] failure! ret = [%d]\n",ptr,ret);
			else
				printf("UnionSetSpecDBBackuperBrotherActive [%s] OK!\n",ptr);
		}
		if (argc - 2 > 0)
			goto finishNow;
loopInput1:
		if (UnionIsQuit(ptr = UnionInput("请输入备份服务器的IP地址::")))
			goto finishNow;
		if (!UnionConfirm("Are you sure of setting DBBackuper [%s] active?",ptr))
			goto loopInput1;
		if ((ret = UnionSetSpecDBBackuperBrotherActive(ptr)) < 0)
			printf("UnionSetSpecDBBackuperBrotherActive [%s] failure! ret = [%d]\n",ptr,ret);
		else
			printf("UnionSetSpecDBBackuperBrotherActive [%s] OK!\n",ptr);
		goto loopInput1;			
	}
	else if (strcasecmp(argv[1],"-SBINACTIVE") == 0)
	{
		for (varIndex = 2; varIndex < argc; varIndex++)
		{
			ptr = argv[varIndex];
			if (!UnionConfirm("Are you sure of setting DBBackuper [%s] inactive?",ptr))
				continue;
			if ((ret = UnionSetSpecDBBackuperBrotherInactive(ptr)) < 0)
				printf("UnionSetSpecDBBackuperBrotherInactive [%s] failure! ret = [%d]\n",ptr,ret);
			else
				printf("UnionSetSpecDBBackuperBrotherInactive [%s] OK!\n",ptr);
		}
		if (argc > 0)
			goto finishNow;
loopInput2:
		if (UnionIsQuit(ptr = UnionInput("请输入备份服务器的IP地址::")))
			goto finishNow;
		if (!UnionConfirm("Are you sure of setting DBBackuper [%s] active?",ptr))
			goto loopInput2;
		if ((ret = UnionSetSpecDBBackuperBrotherInactive(ptr)) < 0)
			printf("UnionSetSpecDBBackuperBrotherInactive [%s] failure! ret = [%d]\n",ptr,ret);
		else
			printf("UnionSetSpecDBBackuperBrotherInactive [%s] OK!\n",ptr);
		goto loopInput2;			
	}
	else if (strcasecmp(argv[1],"-SYNCHDES") == 0)
	{
		if ((ret = UnionConnectDesKeyDB()) < 0)
		{
			printf("UnionConnectDesKeyDB error!\n");
			return(UnionTaskActionBeforeExit());
		}
		varIndex = 2;
		varNum = argc - 2;
		if (argc <= varIndex)
			goto synchDesKeyByKeyName;
		// 检查是否设置了专门的备份服务器
		if (UnionIsValidIPAddrStr(argv[varIndex]))
		{
			if ((ret = UnionSetDefaultDBBackuperBrother(argv[varIndex])) < 0)
			{
				printf("UnionSetDefaultDBBackuperBrother [%s] failure! ret = [%d]\n",argv[varIndex],ret);
				goto finishNow;
			}
			varIndex++;
			varNum--;
		}
		// 检查备份属性
		if (argc > varIndex)
		{
			strcpy(tmpBuf,argv[varIndex]);
			UnionToUpperCase(tmpBuf);
		}
		else
			goto synchDesKeyByKeyName;
		// 检查是否同步所有密钥
		if (strcasecmp(tmpBuf,"ALL") == 0)
		{
			if (!UnionConfirm("Are you sure of backupping all deskeys to the DBBackuper?"))
				goto finishNow;
			if ((synchronizedNum = UnionSynchronizeAllDesKeyToBackuper()) < 0)
				printf("UnionSynchronizeAllDesKeyToBackuper failure! ret = [%d]\n",synchronizedNum);
			else
				printf("%ld desKeys synchronized!\n",synchronizedNum);
			goto finishNow;
		}
		// 检查是否同步指定应用的密钥
		if (strcasecmp(tmpBuf,"APP") == 0)
		{
			varNum--;
			for (++varIndex; varIndex < argc; varIndex++)
			{
				ptr = argv[varIndex];
				if (!UnionConfirm("Are you sure of backupping all deskeys of app [%s] to the DBBackuper?",ptr))
					continue;
				if ((synchronizedNum = UnionSynchronizeDesKeyOfSpecifiedAppToBackuper(ptr)) < 0)
					printf("UnionSynchronizeDesKeyOfSpecifiedAppToBackuper failure! ret = [%d]\n",synchronizedNum);
				else
					printf("%ld desKeys synchronized!\n",synchronizedNum);
			}
			if (varNum > 0)
				goto finishNow;
loopInput3:
			ptr = UnionInput("请输入应用编号::");
			strcpy(tmpBuf,ptr);
			if (UnionIsQuit(ptr))
				goto finishNow;
			if (!UnionConfirm("Are you sure of backupping all deskeys of app [%s] to the DBBackuper?",ptr))
				goto loopInput3;
			ptr = tmpBuf;
			if ((synchronizedNum = UnionSynchronizeDesKeyOfSpecifiedAppToBackuper(ptr)) < 0)
				printf("UnionSynchronizeDesKeyOfSpecifiedAppToBackuper failure! ret = [%d]\n",synchronizedNum);
			else
				printf("%ld desKeys synchronized!\n",synchronizedNum);
			goto loopInput3;
		}
		// 同步指定的密钥
synchDesKeyByKeyName:
		for (varIndex; varIndex < argc; varIndex++)
		{
			ptr = argv[varIndex];
			if (!UnionConfirm("Are you sure of backupping deskey [%s] to the DBBackuper?",ptr))
				continue;
			if ((ret = UnionSynchronizeSpecifiedDesKeyToBackuper(ptr)) < 0)
				printf("synchronize [%s] failure! ret = [%d]\n",ptr,ret);
			else
				printf("synchronize [%s] success!\n",ptr);
		}
		if (varNum > 0)
			goto finishNow;
loopInput4:
		ptr = UnionInput("请输入密钥名称::");
		strcpy(tmpBuf,ptr);
		if (UnionIsQuit(ptr))
			goto finishNow;
		if (!UnionConfirm("Are you sure of backupping deskey [%s] to the DBBackuper?",ptr))
			goto loopInput4;
		ptr = tmpBuf;
		if ((ret = UnionSynchronizeSpecifiedDesKeyToBackuper(ptr)) < 0)
			printf("synchronize [%s] failure! ret = [%d]\n",ptr,ret);
		else
			printf("synchronize [%s] success!\n",ptr);
		goto loopInput4;
	}
	else if (strcasecmp(argv[1],"-SYNCHPK") == 0)
	{
		if ((ret = UnionConnectPKDB()) < 0)
		{
			printf("UnionConnectPKDB error!\n");
			return(UnionTaskActionBeforeExit());
		}
		varIndex = 2;
		varNum = argc - 2;
		if (argc <= varIndex)
			goto synchPKByKeyName;
		// 检查是否设置了专门的备份服务器
		if (UnionIsValidIPAddrStr(argv[varIndex]))
		{
			if ((ret = UnionSetDefaultDBBackuperBrother(argv[varIndex])) < 0)
			{
				printf("UnionSetDefaultDBBackuperBrother [%s] failure! ret = [%d]\n",argv[varIndex],ret);
				goto finishNow;
			}
			varIndex++;
			varNum--;
		}
		// 检查备份属性
		if (argc > varIndex)
		{
			strcpy(tmpBuf,argv[varIndex]);
			UnionToUpperCase(tmpBuf);
		}
		else
			goto synchPKByKeyName;
		// 检查是否同步所有密钥
		if (strcasecmp(tmpBuf,"ALL") == 0)
		{
			if (!UnionConfirm("Are you sure of backupping all pks to the DBBackuper?"))
				goto finishNow;
			if ((synchronizedNum = UnionSynchronizeAllPKToBackuper()) < 0)
				printf("UnionSynchronizeAllPKToBackuper failure! ret = [%d]\n",synchronizedNum);
			else
				printf("%ld desKeys synchronized!\n",synchronizedNum);
			goto finishNow;
		}
		// 检查是否同步指定应用的密钥
		if (strcasecmp(tmpBuf,"APP") == 0)
		{
			varNum--;
			for (++varIndex; varIndex < argc; varIndex++)
			{
				ptr = argv[varIndex];
				if (!UnionConfirm("Are you sure of backupping all pks of app [%s] to the DBBackuper?",ptr))
					continue;
				if ((synchronizedNum = UnionSynchronizePKOfSpecifiedAppToBackuper(ptr)) < 0)
					printf("UnionSynchronizePKOfSpecifiedAppToBackuper failure! ret = [%d]\n",synchronizedNum);
				else
					printf("%ld desKeys synchronized!\n",synchronizedNum);
			}
			if (varNum > 0)
				goto finishNow;
loopInput5:
			ptr = UnionInput("请输入应用编号::");
			strcpy(tmpBuf,ptr);
			if (UnionIsQuit(ptr))
				goto finishNow;
			if (!UnionConfirm("Are you sure of backupping all pks of app [%s] to the DBBackuper?",ptr))
				goto loopInput3;
			ptr = tmpBuf;
			if ((synchronizedNum = UnionSynchronizePKOfSpecifiedAppToBackuper(ptr)) < 0)
				printf("UnionSynchronizePKOfSpecifiedAppToBackuper failure! ret = [%d]\n",synchronizedNum);
			else
				printf("%ld desKeys synchronized!\n",synchronizedNum);
			goto loopInput5;
		}
		// 同步指定的密钥
synchPKByKeyName:
		for (varIndex; varIndex < argc; varIndex++)
		{
			ptr = argv[varIndex];
			if (!UnionConfirm("Are you sure of backupping pk [%s] to the DBBackuper?",ptr))
				continue;
			if ((ret = UnionSynchronizeSpecifiedPKToBackuper(ptr)) < 0)
				printf("synchronize [%s] failure! ret = [%d]\n",ptr,ret);
			else
				printf("synchronize [%s] success!\n",ptr);
		}
		if (varNum > 0)
			goto finishNow;
loopInput6:
		ptr = UnionInput("请输入密钥名称::");
		strcpy(tmpBuf,ptr);
		if (UnionIsQuit(ptr))
			goto finishNow;
		if (!UnionConfirm("Are you sure of backupping pk [%s] to the DBBackuper?",ptr))
			goto loopInput4;
		ptr = tmpBuf;
		if ((ret = UnionSynchronizeSpecifiedPKToBackuper(ptr)) < 0)
			printf("synchronize [%s] failure! ret = [%d]\n",ptr,ret);
		else
			printf("synchronize [%s] success!\n",ptr);
		goto loopInput6;
	}
	else if (strcasecmp(argv[1],"-CHKALLDESKEYS") == 0)
	{
		if ((ret = UnionCompareAllDesKeyValueInKeyDBWithBrothers()) < 0)
			printf("UnionCompareAllDesKeyValueInKeyDBWithBrothers failure! ret = [%d]\n",ret);
		else
			printf("%04d not indentified!\n",ret);
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
		UnionUserErrLog("in %s:: UnionSynchTCPIPSvr [%d] Error! ret = [%d]\n",UnionGetApplicationName(),UnionGetPortOfMineOfKeyDBBackupServer(),ret);
		//return(UnionTaskActionBeforeExit());
	}

	return(UnionTaskActionBeforeExit());
}

