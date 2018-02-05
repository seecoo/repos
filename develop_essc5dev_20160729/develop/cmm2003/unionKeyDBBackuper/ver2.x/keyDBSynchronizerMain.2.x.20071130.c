// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2004/11/20
// Version:	1.0

// 2007/11/30����keyDBBackuperMain.2.x.20070807.c�Ļ����ϴ����˱�����
// ��ԭ���ܷ��۳��˿ͻ��˹��ܺͷ���˹��ܡ�
// �������װ��ԭ����Ŀͻ��˹��ܡ�

// 2007/12/06���޸�֮ǰ������ֻ������һ��keyDBSynchronizer���޸�֮�󣬶���������������򣬲����п��ơ�
// 2007/12/11��������һ������maintain,����ά�����ݷ�����״̬
// 2007/12/11, ������һ��authcheck��������Զ�ͬ����ͬ������Կ
// 2007/12/29��ɾ������ UnionIsCreateSckToKeyDBBackuperFailure ��صĵ��á�

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
#include "kdbSvrService.h"
#include "unionVersion.h"
#include "unionDesKeyDB.h"

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_			// ѡ��3.x�汾��Taskģ��
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
	printf(" -start		start a KDB synchronizing server.\n");
	printf(" -active	set synchronizing active.\n");
	printf(" -inactive	set synchronizing inactive.\n");
	printf(" -sbactive [brotherIPAddr ...]\n");
	printf("		set synchronizing brother active.\n");
	printf(" -sbinactive	[brotherIPAddr ...]\n");
	printf("		set synchronizing brother inactive.\n");
	printf(" -reload	reload synchronizing configuration.\n");
	printf(" -print		print configuration of synchronizing configuration.\n");
	printf(" -synchdes [brotherIPAddr] all|app|fullKeyName\n");
	printf("		synchronize the des keys to brother.\n");
	printf(" -synchpk [brotherIPAddr] all|app|fullKeyName\n");
	printf("		synchronize the pk keys to brother.\n");
	printf(" -chkalldeskeys	check all des keys.\n");
	printf(" -maintain [interval]	maintaining status of keyDBBrothers.\n");
	printf(" -check [interval] auto checking and synchronizing unsame keys to brothers.\n");
	
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
		return(StartKeySynchronizingServer());
	else if (strcasecmp(argv[1],"-MAINTAIN") == 0)	// 2007/12/11�����������
	{
		if (argc > 2)
			return(StartBrothersMaintainer(atoi(argv[2])));
		else
			return(StartBrothersMaintainer(1));
	}
	else if (strcasecmp(argv[1],"-CHECK") == 0)	// 2007/12/11�����������
	{
		if (argc > 2)
			return(StartAutoCheckKeysWithBrothers(atoi(argv[2])));
		else
			return(StartAutoCheckKeysWithBrothers(300));
	}
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
		if (UnionIsQuit(ptr = UnionInput("�����뱸�ݷ�������IP��ַ::")))
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
		if (UnionIsQuit(ptr = UnionInput("�����뱸�ݷ�������IP��ַ::")))
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
		// ����Ƿ�������ר�ŵı��ݷ�����
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
		// ��鱸������
		if (argc > varIndex)
		{
			strcpy(tmpBuf,argv[varIndex]);
			UnionToUpperCase(tmpBuf);
		}
		else
			goto synchDesKeyByKeyName;
		// ����Ƿ�ͬ��������Կ
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
		// ����Ƿ�ͬ��ָ��Ӧ�õ���Կ
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
			ptr = UnionInput("������Ӧ�ñ��::");
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
		// ͬ��ָ������Կ
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
		ptr = UnionInput("��������Կ����::");
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
		// ����Ƿ�������ר�ŵı��ݷ�����
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
		// ��鱸������
		if (argc > varIndex)
		{
			strcpy(tmpBuf,argv[varIndex]);
			UnionToUpperCase(tmpBuf);
		}
		else
			goto synchPKByKeyName;
		// ����Ƿ�ͬ��������Կ
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
		// ����Ƿ�ͬ��ָ��Ӧ�õ���Կ
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
			ptr = UnionInput("������Ӧ�ñ��::");
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
		// ͬ��ָ������Կ
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
		ptr = UnionInput("��������Կ����::");
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

int StartKeySynchronizingServer()
{
	int			lenOfKeyRec;
	int			ret;
	TUnionKeyDBOperation 	operation;
	char 			keyDBType;
	unsigned char 		tmpBuf[4096+200];
	int			port;
	
	/*	2007/12/06 ��������һ�δ���
	if (UnionExistsAnotherTaskOfName(UnionGetApplicationName()))
	{
		printf("Another task of name [%s] already exists!\n",UnionGetApplicationName());
		return(errCodeTaskMDL_AnotherTaskOfNameExists);
	}
	*/
	
	if (UnionCreateProcess() > 0)
	{
		return(UnionTaskActionBeforeExit());
	}
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("in StartKeySynchronizingServer:: UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	UnionSetProgramControlDesKeyDB();
	
	for (;;)
	{
		if ((lenOfKeyRec = UnionReadKeySynchronizeServiceRequest(&operation,&keyDBType,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in StartKeySynchronizingServer::UnionReadKeySynchronizeServiceRequest Error!\n");
			ret = lenOfKeyRec;
			break;
		}
		if (lenOfKeyRec == 0)
		{
			UnionUserErrLog("in StartKeySynchronizingServer:: lenOfKeyRec = [%d]\n",lenOfKeyRec);
			continue;
		}
		if ((ret = UnionSynchronizeKeyDBOperation(operation,keyDBType,tmpBuf,lenOfKeyRec)) < 0)
			UnionUserErrLog("in StartKeySynchronizingServer:: UnionSynchronizeKeyDBOperation! ret = [%d]\n",ret);
		//if (UnionIsCreateSckToKeyDBBackuperFailure())
		//	break;
	}
	return(UnionTaskActionBeforeExit());	// 2007/12/11�޸�
}

// 2007/12/11����
int StartBrothersMaintainer(int interval)
{
	char 		taskName[200];
	int		ret;

	if (UnionCreateProcess() > 0)
	{
		return(UnionTaskActionBeforeExit());
	}
	
	if (interval <= 0)
		interval = 1;
	sprintf(taskName,"%s -maintain %d",UnionGetApplicationName(),interval);	

	if (UnionExistsAnotherTaskOfName(taskName))
	{
		printf("Another task of name [%s] already exists!\n",taskName);
		return(errCodeTaskMDL_AnotherTaskOfNameExists);
	}
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",taskName)) == NULL)
	{
		printf("in StartBrothersMaintainer:: UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	for (;;)
	{
		//UnionResetCreateSckToKeyDBBackuperFailure();
		UnionMaintainStatusOfKeyDBBrothers();
		//if (UnionIsCreateSckToKeyDBBackuperFailure())
		//	break;
		sleep(interval);
	}
	return(UnionTaskActionBeforeExit());
}

int StartAutoCheckKeysWithBrothers(int interval)
{
	char 		taskName[200];
	int		ret;

	if (UnionCreateProcess() > 0)
	{
		return(UnionTaskActionBeforeExit());
	}
	
	if (interval <= 0)
		interval = 300;
	sprintf(taskName,"%s -check %d",UnionGetApplicationName(),interval);	
	if (UnionExistsAnotherTaskOfName(taskName))
	{
		printf("Another task of name [%s] already exists!\n",taskName);
		return(errCodeTaskMDL_AnotherTaskOfNameExists);
	}
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",taskName)) == NULL)
	{
		printf("in StartBrothersMaintainer:: UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	for (;;)
	{
		if (UnionReadIntTypeRECVar("isMainKeyManager") <= 0)
		{
			sleep(interval);
			continue;
		}
		UnionResetCreateSckToKeyDBBackuperFailure();
		UnionAutoSynchronizeDesKeyToBrothers();
		//if (UnionIsCreateSckToKeyDBBackuperFailure())
		//	break;
		sleep(interval);
	}
	return(UnionTaskActionBeforeExit());
}
