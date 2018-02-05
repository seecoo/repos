// Author:	ChenJiaMei
// Date:	2004-7-15

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <semaphore.h>

#ifndef _UnionSocket_3_x_
#define _UnionSocket_3_x_
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_
#endif

#include "UnionSocket.h"
#include "UnionTask.h"
#include "unionModule.h"
#include "unionErrCode.h"
#include "commCheck.h"
#include "UnionStr.h"
#include "UnionEnv.h"
#include "UnionLog.h"
#include "unionVersion.h"

extern PUnionTaskInstance	ptaskInstance;
int UnionTaskActionBeforeExit();

PUnionSharedMemoryModule	pghostGroupMDL = NULL;
PUnionHostGroup			pghostGroup = NULL;
int pghostGroupIsConnected = 0;

#define conMDLNameOfUnionCommCheck	"UnionCommCheck"

int UnionConnectHostGroup()
{
	if (pghostGroupIsConnected)
		return(0);
	
	if ((pghostGroupMDL = UnionConnectSharedMemoryModule(conMDLNameOfUnionCommCheck,sizeof(TUnionHostGroup))) == NULL)
	{
		UnionUserErrLog("in UnionConnectHostGroup:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}
	if ((pghostGroup = (PUnionHostGroup)UnionGetAddrOfSharedMemoryModuleUserSpace(pghostGroupMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectHostGroup:: TUnionHostGroup!\n");
		return(errCodeSharedMemoryModule);
	}
	
	if (UnionIsNewCreatedSharedMemoryModule(pghostGroupMDL))
	{
		pghostGroup->terminateCheckIfAllHostsDown = 0;
		pghostGroup->numOfChecker = 0;
		return(UnionReloadHostGroup());
	}
	else
		return(0);

}

int UnionDisconnectHostGroup()
{
	//if (pghostGroup == NULL)
	//	return(0);
	pghostGroup = NULL;
	pghostGroupMDL = NULL;
	pghostGroupIsConnected = 0;
	return(0);
}

int UnionRemoveHostGroup(int id)
{
	UnionDisconnectHostGroup();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfUnionCommCheck));
}	

/*
int UnionGetIDOfHostGroup()
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		id = -1;
	
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/unionCommCheck.Def",getenv("UNIONETC"));
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionGetIDOfHostGroup:: UnionInitEnvi [%s]!\n",fileName);
		goto exitNow;
	}
	if ((p = UnionGetEnviVarByName("userIDOfHostGroup")) == NULL)
		goto exitNow;
	id = atoi(p);
exitNow:	
	if (id <= 0)
		return(18888);
	else
		return(id);
}
*/

int UnionGetPortOfNullListener()
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		port = -1;
	
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/unionCommCheck.Def",getenv("UNIONETC"));
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionGetPortOfNullListener:: UnionInitEnvi [%s]!\n",fileName);
		goto exitNow;
	}
	if ((p = UnionGetEnviVarByName("portOfNullListener")) == NULL)
		goto exitNow;
	port = atoi(p);
exitNow:	
	if (port <= 0)
		return(8888);
	else
		return(port);
}

int UnionReloadHostGroup()
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		i,hostNum = 0;
	
	if ((ret = UnionConnectHostGroup()) < 0)
	{
		UnionUserErrLog("in UnionReloadHostGroup:: UnionConnectHostGroup!\n");
		return(ret);
	}
	
	for (i = 0; i < conMaxHosts; i++)
	{
		memset(&(pghostGroup->hosts[i]),0,sizeof(TUnionHost));
	}

	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/unionCommCheck.Def",getenv("UNIONETC"));
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionReloadHostGroup:: UnionInitEnvi [%s]!\n",fileName);
		return(0);
		//return(ret);
	}
	if ((p = UnionGetEnviVarByName("intervalPerChecking")) == NULL)
		pghostGroup->intervalPerChecking = 60;
	else
	{
		if ((pghostGroup->intervalPerChecking = atol(p)) <= 0)
			pghostGroup->intervalPerChecking = 60;
	}
	for (i = 0,hostNum = 0; (i < UnionGetEnviVarNum()) && (hostNum < conMaxHosts); i++)
	{
		// 读取IP地址
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
		{
			UnionUserErrLog("in UnionReloadHostGroup:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,0);
			continue;
		}
		if (!UnionIsValidIPAddrStr(p))
			continue;
		strcpy(pghostGroup->hosts[hostNum].ipAddr,p);
		// 读取PORT
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
		{
			UnionUserErrLog("in UnionReloadHostGroup:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
			continue;
		}
		if ((pghostGroup->hosts[hostNum].port = atoi(p)) < 0)
			continue;
		// 读取check
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,2)) == NULL)
		{
			UnionUserErrLog("in UnionReloadHostGroup:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,2);
			continue;
		}
		pghostGroup->hosts[hostNum].checked = atoi(p);
		hostNum++;
	}
	UnionClearEnvi();
	return(0);
}

int UnionCheckAllHosts()
{
	int	ret;
	int	sckHDL;
	int	activeNum = 0;
	int	i;	
	if ((ret = UnionConnectHostGroup()) < 0)
	{
		UnionUserErrLog("in UnionCheckAllHosts:: UnionConnectHostGroup!\n");
		return(ret);
	}
	
	pghostGroup->numOfChecker += 1;
	for (i = 0; i < conMaxHosts; i++)
	{
		if (!UnionIsValidIPAddrStr(pghostGroup->hosts[i].ipAddr))
			continue;
		if (!pghostGroup->hosts[i].checked)
			continue;
		if ((sckHDL = UnionCreateSocketClient(pghostGroup->hosts[i].ipAddr,pghostGroup->hosts[i].port)) < 0)
		{
			UnionUserErrLog("in UnionCheckAllHosts:: UnionCreateSocketClient [%s] [%d]!\n",
					pghostGroup->hosts[i].ipAddr,pghostGroup->hosts[i].port);
			pghostGroup->hosts[i].active = 0;
			continue;
		}
		pghostGroup->hosts[i].active = 1;
		activeNum++;
		UnionCloseSocket(sckHDL);
	}
	pghostGroup->numOfChecker -= 1;
	return(activeNum);
}

int UnionIsTerminateCheckIfAllHostsDown()
{
	if (UnionConnectHostGroup() < 0)
		return(0);
	else
		return(pghostGroup->terminateCheckIfAllHostsDown);
}

int UnionCheckAllHostsAlways()
{
	int	ret;
	
	if ((ret = UnionConnectHostGroup()) < 0)
	{
		UnionUserErrLog("in UnionCheckAllHostsAlways:: UnionConnectHostGroup!\n");
		return(ret);
	}

	if (UnionCreateProcess() > 0)
		return(0);

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s -check",UnionGetApplicationName())) == NULL)
	{
		UnionPrintf("UnionCreateTaskInstance Error!\n");
		return(errCodeTaskMDL_CreateTaskInstance);
	}

	for (;;)
	{
		UnionCheckAllHosts();
		sleep(pghostGroup->intervalPerChecking);
	}
}

int UnionCheckAllHostsUntilAllHostsDown()
{
	int	ret;
	
	if (UnionCreateProcess() > 0)
		return(0);
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s -checkuntilalldown",UnionGetApplicationName())) == NULL)
	{
		UnionPrintf("UnionCreateTaskInstance Error!\n");
		return(errCodeTaskMDL_CreateTaskInstance);
	}
	if ((ret = UnionConnectHostGroup()) < 0)
	{
		UnionUserErrLog("in UnionCheckAllHostsAlways:: UnionConnectHostGroup!\n");
		return(ret);
	}
	for (;;)
	{
		if (UnionCheckAllHosts() <= 0)
		{
			if (UnionIsTerminateCheckIfAllHostsDown())
				return(1);
		}
		else
			sleep(pghostGroup->intervalPerChecking);
	}
}

int UnionOpenExitWhenAllHostsDown()
{
	int	ret;
	
	if ((ret = UnionConnectHostGroup()) < 0)
	{
		UnionUserErrLog("in UnionOpenExitWhenAllHostsDown:: UnionConnectHostGroup!\n");
		return(ret);
	}
	pghostGroup->terminateCheckIfAllHostsDown = 1;
	return(0);
}

int UnionCloseExitWhenAllHostsDown()
{
	int	ret;
	
	if ((ret = UnionConnectHostGroup()) < 0)
	{
		UnionUserErrLog("in UnionCloseExitWhenAllHostsDown:: UnionConnectHostGroup!\n");
		return(ret);
	}
	pghostGroup->terminateCheckIfAllHostsDown = 0;
	return(0);
}

int UnionPrintHostGroupToFile(FILE *fp)
{
	int	ret;
	int	i;	
	if (fp == NULL)
		return(0);
	
	if ((ret = UnionConnectHostGroup()) < 0)
	{
		UnionUserErrLog("in UnionPrintHostGroupToFile:: UnionConnectHostGroup!\n");
		return(ret);
	}
	if (pghostGroup->terminateCheckIfAllHostsDown)
		fprintf(fp,"当所有主机中止时----中止检查\n");
	else
		fprintf(fp,"当所有主机中止时----继续检查\n");
	for (i = 0; i < conMaxHosts; i++)
	{
		if (!UnionIsValidIPAddrStr(pghostGroup->hosts[i].ipAddr))
			continue;
		fprintf(fp,"%15s %05d",pghostGroup->hosts[i].ipAddr,pghostGroup->hosts[i].port);
		if (!(pghostGroup->hosts[i].checked))
			fprintf(fp," 不检测 ");
		else
		{
			fprintf(fp,"  检测  ");
			if (pghostGroup->hosts[i].active)
				fprintf(fp,"正常");
			else
				fprintf(fp,"异常");
		}
		fprintf(fp,"\n");
	}
	return(0);
}

int UnionPrintHostGroup()
{
	return(UnionPrintHostGroupToFile(stdout));
}

int UnionStartNullHostListener()
{
	struct sockaddr_in	serv_addr;
	int			scksvr;
	struct sockaddr_in	cli_addr;
	int			sckinstance;
	int			clilen;

	if (UnionCreateProcess() > 0)
		return(0);
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s -listen %d",UnionGetApplicationName(),UnionGetPortOfNullListener())) == NULL)
	{
		UnionPrintf("UnionCreateTaskInstance Error!\n");
		return(errCodeTaskMDL_CreateTaskInstance);
	}

	// Initialize the socket
	memset((char *)(&serv_addr), 0,sizeof(struct sockaddr_in));
	serv_addr.sin_family            = AF_INET;
	serv_addr.sin_addr.s_addr       = htonl(INADDR_ANY);
	serv_addr.sin_port = htons((u_short)UnionGetPortOfNullListener());

	if ((scksvr = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		UnionSystemErrLog("in UnionTCPIPServer:: socket()!\n");
		return(errCodeUseOSErrCode);
	}

	if (bind(scksvr,(struct sockaddr *)(&serv_addr),sizeof(struct sockaddr_in)) < 0)
	{
		UnionSystemErrLog("in UnionTCPIPServer:: bind()!\n");
		return(errCodeUseOSErrCode);
	}

	// Listen to new-coming connecting request
	if ( listen(scksvr, 10000) < 0 )
	{
		UnionSystemErrLog("in UnionTCPIPServer:: listen()!\n");
		return(errCodeUseOSErrCode);
	}

	UnionAuditLog("in UnionTCPIPServer:: a servier bound to [%d] started OK!\n",UnionGetPortOfNullListener());
			
	for (;;)
	{
		// Accept a Client's Connecting reqeust.
		clilen = sizeof(cli_addr);
		sckinstance = accept(scksvr, (struct sockaddr *)&cli_addr,&clilen);
		if ( sckinstance < 0 )
		{
			UnionSystemErrLog("in UnionTCPIPServer:: accept()!\n");
			continue;
		}
		UnionCloseSocket(sckinstance);
    	}
}

