//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	2.0

#define _UnionLogMDL_3_x_
#define _UnionTask_3_x_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"

#include "unionMsgBuf.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 	reload the msgBufDef\n");
	printf("  -available 	print the available msgBuf pos\n");
	printf("  -occupied 	print the occupied msgBuf pos\n");
	printf("  -status	print the status of msgBuf\n");
	printf("  -printall	print the whole status of msgBuf\n");
	printf("  -clear	clear rubbish message\n");
	printf("  -nowaitbuffer msg type	buffer message\n");
	printf("  -nowaittype type	read specified type message\n");
	printf("  -waittype type	read specified type message\n");
	printf("  -origin	testing functions on origin message\n");
	printf("  -posstatus pos	read status of pos\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectMsgBufMDL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	char	*p;

	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"mngMsgBuf")) == NULL)
	{
		UnionPrintf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionPrintf("UnionConnectMsgBufMDL Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"-RELOAD") == 0)
	{
		if (!UnionConfirm("Are you sure of reloading msgBufDefinition?"))
			return(-1);
		if ((ret = UnionReloadMsgBufDef()) < 0)
			UnionPrintf("in mngMsgBuf:: UnionReloadMsgBufDef Error! ret = [%d]\n",ret);
		else
			UnionPrintf("in mngMsgBuf:: UnionReloadMsgBufDef OK!\n");
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if ((ret = UnionReloadMsgBufDef()) < 0)
			UnionPrintf("in mngMsgBuf:: UnionReloadMsgBufDef Error! ret = [%d]\n",ret);
		else
			UnionPrintf("in mngMsgBuf:: UnionReloadMsgBufDef OK!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-AVAILABLE") == 0)
	{
		if ((ret = UnionPrintAvailablMsgBufPosToFile(stdout)) < 0)
			UnionPrintf("in mngMsgBuf:: UnionPrintAvailablMsgBufPosToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"-OCCUPIED") == 0)
	{
		if ((ret = UnionPrintInavailabeMsgBufPosToFile(stdout)) < 0)
			UnionPrintf("in mngMsgBuf:: UnionPrintInavailabeMsgBufPosToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-STATUS") == 0)
	{
		if ((ret = UnionPrintMsgBufStatusToFile(stdout)) < 0)
			UnionPrintf("in mngMsgBuf:: UnionPrintMsgBufStatusToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-PRINTALL") == 0)
	{
		if ((ret = UnionPrintMsgBufToFile(stdout)) < 0)
			UnionPrintf("in mngMsgBuf:: UnionPrintMsgBufToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-CLEAR") == 0)
		return(StartClearTask(argc-2,&argv[2]));

	if (strcasecmp(argv[1],"-NOWAITBUFFER") == 0)
		return(NowaitBufferMessage(argc-2,&argv[2]));

	if (strcasecmp(argv[1],"-NOWAITTYPE") == 0)
		return(NowaitReadSpecifiedTypeMessage(argc-2,&argv[2]));

	if (strcasecmp(argv[1],"-WAITTYPE") == 0)
		return(WaitReadSpecifiedTypeMessage(argc-2,&argv[2]));
	
	if (strcasecmp(argv[1],"-ORIGIN") == 0)
		return(TestOriginFunctions());

	if (strcasecmp(argv[1],"-POSSTATUS") == 0)
		return(PosStatus(argc-2,&argv[2]));

	printf("wrong command = [%s]\n",argv[1]);
	UnionHelp();
	return(UnionTaskActionBeforeExit());
}

int StartClearTask(int argc,char *argv[])
{
	int	ret;
	int	interval;
	
	if (UnionCreateProcess() > 0)
		return(UnionTaskActionBeforeExit());
	
	if (argc <= 0)
		interval = 1;
	else
	{
		if ((interval = atoi(argv[0])) <= 0)
			interval = 1;
	}
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"mngMsgBuf -clear %d",interval)) == NULL)
	{
		UnionPrintf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	for (;;)
	{
		
		if ((ret = UnionFreeRubbishMsg()) < 0)
		{
			UnionUserErrLog("in StartClearTask:: UnionFreeRubbishMsg!\n");
				return(UnionTaskActionBeforeExit());
		}
		sleep(interval);
	}
}

int NowaitBufferMessage(int argc,char *argv[])
{
	long	msgIndex;
	char	*p;
	long	type = -1;
	char	tmpBuf[512];
	TUnionMessageHeader msgHeader;
	long	loopTimes = 1,index;
	time_t	start,finish;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if (argc > 0)
	{
		if (strlen(argv[0]) > sizeof(tmpBuf) - 1)
			return(UnionTaskActionBeforeExit());
		strcpy(tmpBuf,argv[0]);
	}
	if (argc > 1)
		type = atol(argv[1]);
	else
	{
		if (UnionIsQuit(p = UnionInput("Input msgType or quit/exit to exit::")))
			return(UnionTaskActionBeforeExit());
		type = atol(p);
	}
	
	if (argc > 0)
	{
		if ((msgIndex = UnionWriteNewMessage((unsigned char *)tmpBuf,strlen(tmpBuf),type,&msgHeader)) < 0)
			printf("UnionWriteNewMessage Error! ret = [%d]\n",msgIndex);
		else
		{
			UnionPrintMessageHeaderToFile(&msgHeader,stdout);
			printf("UnionWriteNewMessage OK! msgIndex = [%ld]\n",msgIndex);
		}
		return(UnionTaskActionBeforeExit());
	}
	
loop:
	p = UnionInput("Buffer(type/looptimes/quit/exit/message directly)>");
	memset(tmpBuf,0,sizeof(tmpBuf));
	if (strlen(p) > sizeof(tmpBuf) - 1)
		goto loop;
	strcpy(tmpBuf,p);
	if (UnionIsQuit(p))
		return(UnionTaskActionBeforeExit());
	if (strcasecmp(p,"TYPE") == 0)
	{
		type = atol(UnionInput("Input Message Type::"));
		goto loop;
	}
	if (strcasecmp(p,"LOOPTIMES") == 0)
	{
		loopTimes = atol(UnionInput("Input LoopTimes::"));
		goto loop;
	}
	if (!UnionConfirm("Are you sure of buffer this message [%s] of type [%ld]\n",tmpBuf,type))
		goto loop;
	time(&start);
	for (index = 0; index < loopTimes; index++)
	{
		if ((msgIndex = UnionWriteNewMessage((unsigned char *)tmpBuf,strlen(tmpBuf),type,&msgHeader)) < 0)
			printf("UnionWriteNewMessage Error! ret = [%d]\n",msgIndex);
		else
		{
			if (index % 1000 == 0)
			{
				UnionPrintMessageHeaderToFile(&msgHeader,stdout);
				printf("UnionWriteNewMessage OK! msgIndex = [%ld]\n",msgIndex);
			}
		}
	}
	time(&finish);
	printf("time = [%ld] loopTimes = [%ld]\n",finish-start,loopTimes);
	goto loop;
}

int NowaitReadSpecifiedTypeMessage(int argc,char *argv[])
{
	int	ret;
	long	type = -1;
	char	tmpBuf[512];
	int	i;
	char	*p;
	TUnionMessageHeader msgHeader;
	long	loopTimes = 1,index;
	time_t	start,finish;
	
	for (i = 0; i < argc; i++)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadMsgOfSpecifiedType((unsigned char *)tmpBuf,sizeof(tmpBuf),type = atol(argv[i]),&msgHeader)) < 0)
			printf("UnionReadMsgOfSpecifiedType Error! ret = [%d]\n",ret);
		else
			printf("type = [%ld] msgIndex = [%ld]\nMessage = [%s]\n",type,msgHeader.msgIndex,tmpBuf);
	}
	if (argc > 0)
		return(UnionTaskActionBeforeExit());

loop:
	if (UnionIsQuit(p = UnionInput("Read(message type/looptimes/quit/exit)>")))
		return(UnionTaskActionBeforeExit());
	if (strcasecmp(p,"LOOPTIMES") == 0)
	{
		loopTimes = atol(UnionInput("Input LoopTimes::"));
		goto loop;
	}
	time(&start);
	for (index = 0; index < loopTimes; index++)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadMsgOfSpecifiedType((unsigned char *)tmpBuf,sizeof(tmpBuf),type = atol(p),&msgHeader)) < 0)
			printf("UnionReadMsgOfSpecifiedType Error! ret = [%d]\n",ret);
		else
		{
			if (index % 1000 == 0)
				printf("type = [%ld] msgIndex = [%ld]\nMessage = [%s]\n",type,msgHeader.msgIndex,tmpBuf);
		}
	}
	time(&finish);
	printf("time = [%ld] loopTimes = [%ld]\n",finish-start,loopTimes);
	goto loop;
}

int WaitReadSpecifiedTypeMessage(int argc,char *argv[])
{
	int	ret;
	long	type = -1;
	char	tmpBuf[512];
	int	i;
	char	*p;
	long	loopTimes = 1,index;
	time_t	start,finish;
	TUnionMessageHeader msgHeader;
	
	for (i = 0; i < argc; i++)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadMsgOfSpecifiedTypeUntilSuccess((unsigned char *)tmpBuf,sizeof(tmpBuf),type = atol(argv[i]),&msgHeader)) < 0)
			printf("UnionReadMsgOfSpecifiedTypeUntilSuccess Error! ret = [%d]\n",ret);
		else
			printf("type = [%ld] msgIndex = [%ld]\nMessage = [%s]\n",type,msgHeader.msgIndex,tmpBuf);
	}
	if (argc > 0)
		return(UnionTaskActionBeforeExit());

loop:
	if (UnionIsQuit(p = UnionInput("Read(message type/looptimes/quit/exit)>")))
		return(UnionTaskActionBeforeExit());
	if (strcasecmp(p,"LOOPTIMES") == 0)
	{
		loopTimes = atol(UnionInput("Input LoopTimes::"));
		goto loop;
	}
	time(&start);
	for (index = 0; index < loopTimes; index++)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadMsgOfSpecifiedTypeUntilSuccess((unsigned char *)tmpBuf,sizeof(tmpBuf),type = atol(p),&msgHeader)) < 0)
			printf("UnionReadMsgOfSpecifiedTypeUntilSuccess Error! ret = [%d]\n",ret);
		else
		{
			if (index % 1000 == 0)
			{
				time(&finish);
				if (finish-start != 0)
					printf("tps = [%ld] index = [%ld] msgIndex = [%ld] time = [%ld] type = [%ld]\nMessage = [%s]\n",index / (finish-start),index,msgHeader.msgIndex,finish-start,type,tmpBuf);
				else
					printf("index = [%ld] time = [%ld] msgIndex = [%ld] type = [%ld]\nMessage = [%s]\n",index,finish-start,msgHeader.msgIndex,type,tmpBuf);
			}
		}
	}
	time(&finish);
	printf("time = [%ld] loopTimes = [%ld]\n",finish-start,loopTimes);
	goto loop;
}

int TestOriginFunctions()
{
	int	ret;
	char	tmpBuf[512];
	TUnionMessageHeader msgHeader;
	char	*p;

	memset(tmpBuf,0,sizeof(tmpBuf));
	
	if (UnionIsQuit(p = UnionInput("message type::")))
		return(UnionTaskActionBeforeExit());
	
	if ((ret = UnionReadMsgOfSpecifiedTypeUntilSuccess((unsigned char *)tmpBuf,sizeof(tmpBuf),atol(p),&msgHeader)) < 0)
	{
		printf("UnionReadMsgOfSpecifiedTypeUntilSuccess Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}		
	else
		UnionPrintMessageHeaderToFile(&msgHeader,stdout);
	
	if (!UnionConfirm("Write Origin Message Back?"))
		return(UnionTaskActionBeforeExit());
	
	if ((ret = UnionWriteOriginMessage((unsigned char *)tmpBuf,ret,&msgHeader)) < 0)
	{
		printf("UnionWriteOriginMessage Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}		
	else
		printf("UnionWriteOriginMessage OK!\n");

	memset(tmpBuf,0,sizeof(tmpBuf));	
	if ((ret = UnionReadOriginMessage((unsigned char *)tmpBuf,sizeof(tmpBuf),&msgHeader)) < 0)
		printf("UnionReadOriginMessage Error! ret = [%d]\n",ret);
	else
	{
		UnionPrintMessageHeaderToFile(&msgHeader,stdout);
		printf("len = [%d] tmpBuf = [%s]\n",ret,tmpBuf);
	}
	return(UnionTaskActionBeforeExit());
}

int PosStatus(int argc,char *argv[])
{
	int	ret;
	int	i;
	char	*p;
	
	for (i = 0; i < argc; i++)
	{
		if ((ret = UnionPrintStatusOfMsgPosToFile(atol(argv[i]),stdout)) < 0)
			printf("UnionPrintStatusOfMsgPosToFile Error! ret = [%d]\n",ret);
	}
	if (argc > 0)
		return(UnionTaskActionBeforeExit());

loop:
	if (UnionIsQuit(p = UnionInput("Read(pos/quit/exit)>")))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionPrintStatusOfMsgPosToFile(atol(p),stdout)) < 0)
		printf("UnionPrintStatusOfMsgPosToFile Error! ret = [%d]\n",ret);
	goto loop;
}	
