//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2004/11/23


#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif
#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"
#include "unionSckCommCli.h"

PUnionTaskInstance	ptaskInstance = NULL;
PUnionCommSvrConn	pgcommSvrConn = NULL;

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 	reload the commSvrDef\n");
	printf("  -print 	print the commSvrDef\n");
	printf("\n");
	printf("  -normal ipAddr|both\n");
	printf("  -abnormal ipAddr|both\n");
	printf("  -coldbackup ipAddr|both\n");
	printf("  -maintain ipAddr|both\n");
	printf("  -reverse 	reverse the roles of the primary and secondary server\n");
	printf("  -zero ipAddr|both\n");
	printf("	zerolize the commTimes of the server\n");
	printf("\n");
	printf("  -hotwm	set hot backup working mode\n");
	printf("  -balwm	set balance working mode\n");
	printf("  -maiwm	set maintaining working mode\n");
	printf("  -timeout value set timeout\n");
	printf("  -check timeInterval\n");
	printf("\n");
	printf("  -test\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectCommSvr(pgcommSvrConn);
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int Test()
{
	char		reqBuf[1024],resBuf[1024];
	int		len;
	long		loopTimes = 1;
	long		i;
	time_t		start,finish;
	int		tid,terminals;
	int		pid;
	int		ret;

	//if ((pid = UnionCreateProcess()) > 0)
	//	return(UnionTaskActionBeforeExit());

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s test",UnionGetApplicationName())) == NULL)
	{
		printf("in %s::UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}

	if ((pgcommSvrConn = UnionConnectCommSvr()) == NULL)	
	{
		printf("in %s:: UnionConnectCommSvr!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	//printf("%x\n",pgcommSvrConn);
	
//loopTesting:
	memset(reqBuf,0,sizeof(reqBuf));
	printf("Input request string::\n");
	scanf("%s",reqBuf);
	len = strlen(reqBuf);
	printf("Input loop times::");
	scanf("%ld",&loopTimes);
	printf("Input terminals::");
	scanf("%d",&terminals);
	for (tid = 0; tid < terminals; tid++)
	{
		if ((pid = UnionCreateProcess()) > 0)
			continue;
		if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
		{
			printf("UnionCreateTaskInstance Error!\n");
			return(UnionTaskActionBeforeExit());
		}
		time(&start);
		for (i = 0; i < loopTimes; i++)
		{	
			if ((ret = UnionCommSvrConnService(pgcommSvrConn,(unsigned char *)reqBuf,len,(unsigned char *)resBuf,sizeof(resBuf))) < 0)
			{
				printf("in %s::UnionCommSvrConnService Error ret = [%d]!\n",UnionGetApplicationName(),ret);
				continue;
				//break;
			}
			resBuf[ret] = 0;
			if ((loopTimes == 1) || (i % 1000 == 0))
			{
				printf("pid = [%d]\n",getpid());
				printf("Len = [%d]\n",ret);
				printf("Buf = [%s]\n",resBuf);
				time(&finish);
				printf("i = [%12ld] Time Used = [%12ld]\n",i,finish - start);
				if (finish - start > 0)
					printf("tps = [%ld]\n",i / (finish-start));
			}
		}
		if (loopTimes != 1)
		{
			time(&finish);
			printf("finished Moment = [%ld] started Moment = [%ld] Time Used = [%ld]\n",finish,start,finish - start);
			printf("loopTimes = [%ld] TimeUsed = [%ld]\n",i,finish - start);
			UnionAuditLog("loopTimes = [%ld] TimeUsed = [%ld]\n",i,finish - start);
			if (finish - start > 0)
			{
				printf("tps = [%ld]\n",loopTimes / (finish-start));
				UnionAuditLog("tps = [%ld]\n",loopTimes / (finish-start));
			}
		}
	}
	//printf("\n\nContinue to testing(Y/N)?");
	//scanf("%s",resBuf);
	//if (toupper(resBuf[0]) == 'Y')
	//	goto loopTesting;
	
	return(UnionTaskActionBeforeExit());
}

int CheckCommSvr(int interval)
{
	int	ret;
	int	pid;
	
	if ((pid = UnionCreateProcess()) < 0)
		return(-1);
	if (pid > 0)
		return(0);
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s check %d",UnionGetApplicationName(),interval)) == NULL)
	{
		printf("in CheckCommSvr:: UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if ((ret = UnionConnectCommSvrDefMDL()) < 0)
	{
		printf("in CheckCommSvr:: UnionConnectCommSvrDefMDL!\n");
		return(ret);
	}
	
	sleep(120);
	for (;;)
	{
		UnionCheckCommSvr();
		sleep(interval);
	}
}

int main(int argc,char *argv[])
{
	int	ret;

	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	if (strcasecmp(argv[1],"-RELOAD") == 0)
	{
		if (!UnionConfirm("Are you sure of reloading commSvrDef?"))
			return(-1);
		if ((ret = UnionReloadCommSvrDef()) < 0)
			printf("UnionReloadCommSvrDef Error! ret = [%d]\n",ret);
		else
			printf("UnionReloadCommSvrDef OK!\n");
	}
	else if (strcasecmp(argv[1],"-PRINT") == 0)
	{
		if ((ret = UnionPrintCommSvrDef()) < 0)
			printf("UnionPrintCommSvrDef Error! ret = [%d]\n",ret);
	}
	else if (strcasecmp(argv[1],"-TEST") == 0)
		return(Test());
	else if (strcasecmp(argv[1],"-ABNORMAL") == 0)
	{
		if (argc < 3)
			UnionHelp();
		else
		{
			if ((ret = UnionSetSckSvrStatus(argv[2],conSckSvrAbnormal)) < 0)
				printf("UnionSetSckSvrStatus [%s] Abnormal Error!\n",argv[2]);
			else
				printf("UnionSetSckSvrStatus [%s] Abnormal OK!\n",argv[2]);
		}
	}
	else if (strcasecmp(argv[1],"-NORMAL") == 0)
	{
		if (argc < 3)
			UnionHelp();
		else
		{
			if ((ret = UnionSetSckSvrStatus(argv[2],conSckSvrNormal)) < 0)
				printf("UnionSetSckSvrStatus [%s] Normal Error!\n",argv[2]);
			else
				printf("UnionSetSckSvrStatus [%s] Normal OK!\n",argv[2]);
		}
	}
	else if (strcasecmp(argv[1],"-MAINTAIN") == 0)
	{
		if (argc < 3)
			UnionHelp();
		else
		{
			if ((ret = UnionSetSckSvrStatus(argv[2],conSckSvrMaintaining)) < 0)
				printf("UnionSetSckSvrStatus [%s] Maintain Error!\n",argv[2]);
			else
				printf("UnionSetSckSvrStatus [%s] Maintain OK!\n",argv[2]);
		}
	}
	else if (strcasecmp(argv[1],"-TIMEOUT") == 0)
	{
		if (argc < 3)
			UnionHelp();
		else
		{
			if ((ret = UnionSetTimeoutOfCommSvr(atoi(argv[2]))) < 0)
				printf("UnionSetTimeoutOfCommSvr [%s] Error!\n",argv[2]);
			else
				printf("UnionSetTimeoutOfCommSvr [%s] OK!\n",argv[2]);
		}
	}
	else if (strcasecmp(argv[1],"-COLDBACKUP") == 0)
	{
		if (argc < 3)
			UnionHelp();
		else
		{
			if ((ret = UnionSetSckSvrStatus(argv[2],conSckSvrColdBackup)) < 0)
				printf("UnionSetSckSvrStatus [%s] ColdBackup Error!\n",argv[2]);
			else
				printf("UnionSetSckSvrStatus [%s] ColdBackup OK!\n",argv[2]);
		}
	}
	else if (strcasecmp(argv[1],"-ZERO") == 0)
	{
		if (argc < 3)
			UnionHelp();
		else
		{
			if ((ret = UnionZerolizeSckSvrCommTimes(argv[2])) < 0)
				printf("UnionZerolizeSckSvrCommTimes [%s] Error!\n",argv[2]);
			else
				printf("UnionZerolizeSckSvrCommTimes [%s] OK!\n",argv[2]);
		}
	}
	else if (strcasecmp(argv[1],"-REVERSE") == 0)
	{
		if ((ret = UnionReversePrimaryAndSecondarySckSvr()) < 0)
			printf("UnionReversePrimaryAndSecondarySckSvr Error!\n");
		else
			printf("UnionReversePrimaryAndSecondarySckSvr OK!\n");
	}
	else if (strcasecmp(argv[1],"-HOTWM") == 0)
	{
		if ((ret = UnionSetCommSvrStatus(conCommSvrHotBackupWorkingMode)) < 0)
			printf("UnionSetCommSvrStatus HotBackup Error!\n");
		else
			printf("UnionSetCommSvrStatus HotBackup OK!\n");
	}
	else if (strcasecmp(argv[1],"-BALWM") == 0)
	{
		if ((ret = UnionSetCommSvrStatus(conCommSvrBalanceWorkingMode)) < 0)
			printf("UnionSetCommSvrStatus Balance Error!\n");
		else
			printf("UnionSetCommSvrStatus Balance OK!\n");
	}
	else if (strcasecmp(argv[1],"-MAIWM") == 0)
	{
		if ((ret = UnionSetCommSvrStatus(conCommSvrMaintainWorkingMode)) < 0)
			printf("UnionSetCommSvrStatus Maintaining Error!\n");
		else
			printf("UnionSetCommSvrStatus Maintaining OK!\n");
	}
	else if (strcasecmp(argv[1],"-CHECK") == 0)
	{
		if (argc >= 3)
			ret = CheckCommSvr(atoi(argv[2]));
		else
			ret = CheckCommSvr(60);	
	}
	else
		UnionHelp();
		
	return(UnionTaskActionBeforeExit());
}

