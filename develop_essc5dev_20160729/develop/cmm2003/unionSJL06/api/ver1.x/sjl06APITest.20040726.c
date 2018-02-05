// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2004/07/26
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "unionVersion.h"
#include "UnionTask.h"
#include "UnionLog.h"
#include "unionSJL06API.h"

PUnionTaskInstance	ptaskInstance = NULL;
PUnionSJL06Server	psjl06Server = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectSJL06Server(psjl06Server);
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s CmdString [loopTimes] [hsmGrpID]\n",UnionGetApplicationName());
	return(0);
}

main(int argc,char **argv)
{
	char		resBuf[1024];
	int		len;
	long		loopTimes = 1;
	long		i;
	time_t		start,finish;
	int		t;
	int		pid;
	int		ret;
	char		hsmGrpID[10];

	UnionSetApplicationName(argv[0]);
	
	if (argc < 2)
		return(UnionHelp());
	
	if ((pid = UnionCreateProcess()) > 0)
		return(UnionTaskActionBeforeExit());

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,UnionGetApplicationName())) == NULL)
	{
		UnionPrintf("in %s::UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	len = strlen(argv[1]);
	if (argc >= 3)
		loopTimes = atol(argv[2]);
	if (argc >= 4)
		strcpy(hsmGrpID,argv[3]);
	else
		strcpy(hsmGrpID,"001");

	if ((psjl06Server = UnionConnectSJL06Server(hsmGrpID,conUnionESSCSvr)) == NULL)	
	{
		printf("in %s:: UnionConnectSJL06Server [%s]\n",UnionGetApplicationName(),hsmGrpID);
		return(UnionTaskActionBeforeExit());
	}
	
	time(&start);
	for (i = 0; i < loopTimes; i++)
	{	
		if ((ret = UnionSJL06ServerService(psjl06Server,argv[1],len,resBuf,sizeof(resBuf))) < 0)
		{
			UnionPrintf("in %s::UnionSJL06ServerService Error ret = [%d]!\n",UnionGetApplicationName(),ret);
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
	
	return(UnionTaskActionBeforeExit());
}
