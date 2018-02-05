// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2003/09/28
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "unionErrCode.h"
#include "ebcdicAscii.h"
#include "UnionLog.h"

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "UnionTask.h"

int	gsckHDL = -1;

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	if (gsckHDL >= 0)
		UnionCloseSocket(gsckHDL);
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int GetHsmSvrAddr(char *IPAddr,int *Port)
{
	char	*Variable;
	char	FileName[128];
	
	memset(FileName,0,sizeof(FileName));
	sprintf(FileName,"%s/HsmSvr.CFG",getenv("UNIONETC"));
		
	if (UnionInitEnvi(FileName) < 0)
	{	
		printf("Fail to UnionInitEnvi [%s]\n",FileName);
		return(-1);
	}
	
	if ((Variable = UnionGetEnviVarByName("HsmSvrIPAddr")) == NULL)
	{
		printf("Fail to get IPAddr!\n");
		return(-1);
	}
	else
	{
		memcpy(IPAddr,Variable,strlen(Variable));
		printf("HsmSvr IP = [%s]\n",Variable);
	}

	if ((Variable = UnionGetEnviVarByName("HsmSvrPort")) == NULL)
	{
		printf("Fail to get Port!\n");
		return(-1);
	}
	else
	{
		*Port = atoi(Variable);
		printf("Port = [%d]\n",*Port);
	}
	
	UnionClearEnvi();
	
	return(0);
}	

int UnionHelp()
{
	printf("Usage:: %s CmdString [loopTimes] [terminals]\n",UnionGetApplicationName());
	return(0);
}

main(int argc,char **argv)
{
	unsigned char	cmdBuf[1024];
	unsigned char	resBuf[1024];
	int		len;
	int		port;
	char		hsmSvrIPAddr[20];
	long		loopTimes = 1;
	long		i;
	time_t		start,finish;
	int		terminals = 1;
	int		t;
	int		pid;
	int		ret;

	if (argc < 2)
		return(UnionHelp());
	
	UnionSetApplicationName(argv[0]);
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		UnionPrintf("in %s::UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}

	memset(cmdBuf,0,sizeof(cmdBuf));
	len = strlen(argv[1]);
	memcpy(cmdBuf + 2,argv[1],len);
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	memset(hsmSvrIPAddr,0,sizeof(hsmSvrIPAddr));
	if (GetHsmSvrAddr(hsmSvrIPAddr,&port) < 0)
	{
		UnionPrintf("in %s::Fail to get address for HsmSvr!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	cmdBuf[0] = len / 256;
	cmdBuf[1] = len % 256;
	printf("cmdBuf = [%s] len = [%d]\n",cmdBuf+2,len);
	UnionAsciiToEbcdic(cmdBuf+2,cmdBuf+2,len);

	len += 2;
	
	if (argc >= 3)
		loopTimes = atol(argv[2]);
	if (argc >= 4)
		terminals = atoi(argv[3]);

	for (t = 0; t < terminals; t++)
	{
		if ((pid = UnionCreateProcess()) > 0)
			continue;
		if ((gsckHDL = UnionCreateSocketClient(hsmSvrIPAddr,port)) < 0)
		{
			UnionPrintf("in %s::UnionCreateSocketClient Error!\n",UnionGetApplicationName());
			return(UnionTaskActionBeforeExit());
		}
		if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
		{
			UnionPrintf("in %s::UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
			return(UnionTaskActionBeforeExit());
		}
		time(&start);
		for (i = 0; i < loopTimes; i++)
		{	
			//printf("cmdBuf = [%s]\n",cmdBuf+2);
			if ((ret = UnionSendToSocket(gsckHDL,cmdBuf,len)) < 0)
			{
				UnionPrintf("in %s::UnionSendToSocket Error ret = [%d]!\n",ret,UnionGetApplicationName());
				break;
			}
			memset(resBuf,0,sizeof(resBuf));	
			if ((ret = UnionReceiveFromSocket(gsckHDL,resBuf,sizeof(resBuf))) < 0)
			{
				UnionPrintf("in %s::UnionReceiveFromSocket Error! I = [%ld]\n",UnionGetApplicationName(),i);
				break;
			}
			else
			{
				if ((loopTimes == 1) || (i % 1000 == 0))
				{
					UnionEbcdicToAscii(resBuf+2,resBuf+2,ret-2);
					printf("pid = [%d]\n",getpid());
					printf("Len = [%d]\n",ret);
					printf("Buf = [%s]\n",resBuf+2);
					time(&finish);
					printf("I = [%12ld] Time Used = [%12ld]\n",i,finish - start);
					if (finish - start > 0)
						printf("tps = [%ld]\n",i / (finish-start));
				}
			}

		}
		if (loopTimes != 1)
		{
			time(&finish);
			UnionEbcdicToAscii(resBuf+2,resBuf+2,ret-2);
			printf("finished Moment = [%ld] started Moment = [%ld] Time Used = [%ld]\n",finish,start,finish - start);
			printf("loopTimes = [%ld] TimeUsed = [%ld]\n",i,finish - start);
			UnionAuditLog("loopTimes = [%ld] TimeUsed = [%ld]\n",i,finish - start);
			if (finish - start > 0)
			{
				printf("tps = [%ld]\n",loopTimes / (finish-start));
				UnionAuditLog("tps = [%ld]\n",loopTimes / (finish-start));
			}
		}
		UnionCloseSocket(gsckHDL);
		return(UnionTaskActionBeforeExit());
	}
	
	return(UnionTaskActionBeforeExit());
}
