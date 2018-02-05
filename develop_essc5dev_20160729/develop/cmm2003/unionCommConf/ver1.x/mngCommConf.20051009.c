//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	2.0

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif
#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"
#include "UnionStr.h"

#include "unionCommConf.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 	reload the unionCommConfTBL\n");
	printf("  -all		print all unionCommConf\n");
	printf("  -ipAddr	print all unionCommConf of ipAddr\n");
	printf("  -port		print all unionCommConf of port\n");
	printf("  -remark	print all unionCommConf of remark\n");
	printf("  -server	print all server CommConf\n");
	printf("  -client	print all client CommConf\n");
	printf("  -edit\n");
	printf("  ipAddr/port\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectCommConfTBL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int PrintCommConfOfPort(int num,char *name[])
{
	int	i;
	int	ret;
	char	*p;
		
	for (i = 0; i < num; i++)
	{
		if ((ret = UnionPrintCommConfOfPortToFile(atoi(name[i]),stdout)) < 0)
			printf("UnionPrintCommConfOfPortToFile [%s] Error! ret = [%d]\n",name[i],ret);
	}
	
	if (num > 0)
		return(0);
	
loopInput:
	p = UnionInput("Input port::");
	if (UnionIsQuit(p))
		return(0);
	if ((ret = UnionPrintCommConfOfPortToFile(atoi(p),stdout)) < 0)
		printf("UnionPrintCommConfOfPortToFile [%s] Error! ret = [%d]\n",p,ret);
	goto loopInput;
}

int PrintCommConfOfIPAddr(int num,char *ipAddr[])
{
	int	i;
	int	ret;
	char	*p;
		
	for (i = 0; i < num; i++)
	{
		if ((ret = UnionPrintCommConfOfIPAddrToFile(ipAddr[i],stdout)) < 0)
			printf("UnionPrintCommConfOfIPAddrToFile [%s] Error! ret = [%d]\n",ipAddr[i],ret);
	}
	
	if (num > 0)
		return(0);
	
loopInput:
	p = UnionInput("Input ipAddr::");
	if (UnionIsQuit(p))
		return(0);
	if ((ret = UnionPrintCommConfOfIPAddrToFile(p,stdout)) < 0)
		printf("UnionPrintCommConfOfIPAddrToFile [%s] Error! ret = [%d]\n",p,ret);
	goto loopInput;
}

int PrintCommConf(int num,char *name[])
{
	int	i;
	int	ret;
		
	for (i = 0; i < num; i++)
	{
		if (UnionIsValidIPAddrStr(name[i]))
		{
			if ((ret = UnionPrintCommConfOfIPAddrToFile(name[i],stdout)) < 0)
				printf("UnionPrintCommConfOfIPAddrToFile [%s] Error! ret = [%d]\n",name[i],ret);
		}
		else
		{
			if ((ret = UnionPrintCommConfOfPortToFile(atoi(name[i]),stdout)) < 0)
				printf("UnionPrintCommConfOfPortToFile [%s] Error! ret = [%d]\n",name[i],ret);
		}
	}
	return(0);	
}

int EditHelp()
{
	printf("Help::\n");
	printf("ipAddr|server|client|port|long|short|remark|delete|add|deletebyremark|help\n");
	return(0);
}

int EditCommConf()
{
	char		*command;
	TUnionCommConf	commConf;
	PUnionCommConf	pcommConf;
	
	memset(&commConf,0,sizeof(TUnionCommConf));
	commConf.procType = conCommClient;
	commConf.connType = conCommLongConn;
loop:	
	if (UnionIsQuit(command = UnionInput("\nCommand>")))
		return(0);
	
	if (UnionIsValidIPAddrStr(command))
	{
		strcpy(commConf.ipAddr,command);
	}
	else if (UnionIsDigitStr(command))
	{
		if (atoi(command) > 0)
			commConf.port = atoi(command);
		else
			printf("Wrong Port!!\n");
	}
	else if (strcasecmp(command,"SERVER") == 0)
		commConf.procType = conCommServer;
	else if (strcasecmp(command,"CLIENT") == 0)
		commConf.procType = conCommClient;
	else if  (strcasecmp(command,"LONG") == 0)
		commConf.connType = conCommLongConn;
	else if  (strcasecmp(command,"SHORT") == 0)
		commConf.connType = conCommShortConn;
	else if  (strcasecmp(command,"REMARK") == 0)
	{
		if (UnionIsQuit(command = UnionInput("Input Remark::")))
			goto loop;
		if (strlen(command) >= sizeof(commConf.remark))
		{
			memset(commConf.remark,0,sizeof(commConf.remark));
			memcpy(commConf.remark,command,sizeof(commConf.remark)-1);
		}
		else
			strcpy(commConf.remark,command);
		UnionPrintCommConfOfRemarkToFile(commConf.remark,stdout);
	}
	else if (strcasecmp(command,"ALL") == 0)
		UnionPrintCommConfTBLToFile(stdout);
	else if (strcasecmp(command,"ADD") == 0)
	{
		UnionPrintCommConfToFile(&commConf,stdout);
		if (UnionConfirm("Are you sure of add this commConf?"))
		{
			if ((pcommConf = UnionAddCommConf(commConf.ipAddr,commConf.port,commConf.procType,commConf.connType,commConf.remark)) == NULL)
				printf("Add failure!\n");		
			else
			{
				UnionPrintCommConfToFile(pcommConf,stdout);
				printf("Add OK!\n");
			}
		}
	}
	else if (strcasecmp(command,"DELETE") == 0)
	{
		UnionPrintCommConfToFile(&commConf,stdout);
		if (UnionConfirm("Are you sure of delete this commConf?"))
		{
			printf("Delete %d commConfs!\n",
				UnionDeleteSpecifiedCommConf(commConf.ipAddr,commConf.port,commConf.procType));
		}
	}
	else if (strcasecmp(command,"DELETEBYREMARK") == 0)
	{
		UnionPrintCommConfToFile(&commConf,stdout);
		if (UnionConfirm("Are you sure of delete commConf of remark %s?",commConf.remark))
		{
			printf("Delete %d commConfs!\n",UnionDeleteSpecifiedCommConfByRemark(commConf.remark));
		}
	}
	else if (strcasecmp(command,"HELP") == 0)
		EditHelp();
	else
		EditHelp();
	goto loop;
}

int main(int argc,char *argv[])
{
	int	ret;
		
	UnionSetApplicationName(argv[0]);

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	if (argc < 2)
		return(UnionHelp());
		
	/*
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	*/
	
	if (strcasecmp(argv[1],"-RELOAD") == 0)
	{
		if (!UnionConfirm("确定加载共享内存[监控表]吗?"))
			return(-1);
		if ((ret = UnionReloadCommConfTBL()) < 0)
			printf("***** %-30s Error!\n","加载共享内存[监控表]");
		else
			printf("***** %-30s OK!\n","加载共享内存[监控表]");
	}
	else if (strcasecmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if ((ret = UnionReloadCommConfTBL()) < 0)
			printf("***** %-30s Error!\n","加载共享内存[监控表]");
		else
			printf("***** %-30s OK!\n","加载共享内存[监控表]");
	}
	else if (strcasecmp(argv[1],"-ALL") == 0)
		UnionPrintCommConfTBLToFile(stdout);
	else if (strcasecmp(argv[1],"-PORT") == 0)
		PrintCommConfOfPort(argc-2,&argv[2]);			
	else if (strcasecmp(argv[1],"-REMARK") == 0)
		UnionPrintCommConfOfRemarkToFile(argv[2],stdout);			
	else if (strcasecmp(argv[1],"-IPADDR") == 0)
		PrintCommConfOfIPAddr(argc-2,&argv[2]);			
	else if (strcasecmp(argv[1],"-SERVER") == 0)
		UnionPrintAllServerCommConfToFile(stdout);			
	else if (strcasecmp(argv[1],"-CLIENT") == 0)
		UnionPrintAllClientCommConfToFile(stdout);
	else if ((strcasecmp(argv[1],"-EDIT") == 0) || (strcasecmp(argv[1],"EDIT") == 0))
		EditCommConf();	
	else
		PrintCommConf(argc-1,&argv[1]);	
	return(UnionTaskActionBeforeExit());
}

