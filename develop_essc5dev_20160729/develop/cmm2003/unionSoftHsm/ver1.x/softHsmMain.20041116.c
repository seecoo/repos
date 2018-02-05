// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2004/07/29
// Version:	1.0

#define _UnionSocket_3_x_

#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
//#include <net/if_arp.h>

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_// 选用3.x版本的Task模块
#endif
#ifndef _UnionTask_3_2_
#define _UnionTask_3_2_
#endif
#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_// 选用3.x版本的日志模块
#endif

#include "UnionTask.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "UnionSocket.h"
#include "unionVersion.h"
#include "unionCommand.h"

#include "softHsmKeySpace.h"

int gsynchTCPIPSvrSckHDL;
int gport;
char ghsmGrp[3+1];

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHsmSynchTCPIPTaskServer(int handle,struct sockaddr_in *cli_addr,int port,int (*UnionTaskActionBeforeExit)());

int UnionHelp()
{
	printf("Usage:: %s socketPort [hsmGrpID]\n",UnionGetApplicationName());
	return(0);
}

int UnionTaskActionBeforeExit()
{
	if (gsynchTCPIPSvrSckHDL >= 0)
		UnionCloseSocket(gsynchTCPIPSvrSckHDL);
	UnionDisconnectSoftHsmKeySpace();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	char	mainRegisterName[100];
	
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	//if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,UnionGetApplicationName())) == NULL)
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		UnionPrintf("in %s:: UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	if (UnionCreateProcess() > 0)
	{
		return(UnionTaskActionBeforeExit());
	}

	gport = atoi(argv[1]);
	memset(ghsmGrp,0,sizeof(ghsmGrp));
	if (argc <= 2)
		strcpy(ghsmGrp,"001");
	else
	{
		if (strlen(argv[2]) != 3)
		{
			printf("hsmGrpID must be of 3 chars!\n");
			return(UnionTaskActionBeforeExit());
		}
		strcpy(ghsmGrp,argv[2]);
	}
	
	memset(mainRegisterName,0,sizeof(mainRegisterName));
	sprintf(mainRegisterName,"%s %s",UnionGetApplicationName(),ghsmGrp);
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s %d",mainRegisterName,
				gport)) == NULL)
	{
		UnionPrintf("in %s:: UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}

	if ((ret = UnionTCPIPServer(gport,mainRegisterName,UnionHsmSynchTCPIPTaskServer,UnionTaskActionBeforeExit)) < 0)
	{
		UnionUserErrLog("in %s:: UnionSynchTCPIPSvr [%s] Error! ret = [%d]\n",UnionGetApplicationName(),argv[1],ret);
		//return(UnionTaskActionBeforeExit());
	}

	return(UnionTaskActionBeforeExit());
}

int UnionHsmSynchTCPIPTaskServer(int handle,struct sockaddr_in *cli_addr,int port,int (*UnionTaskActionBeforeExit)())
{
	unsigned char		tmpBuf[4096];
	int			lenOfReq,lenOfRes;
	int			ret;
	
	UnionLog("in UnionHsmSynchTCPIPTaskServer:: clientIPAddr [%s] Port [%d] Connected!\n",inet_ntoa(cli_addr->sin_addr),port);
	memset(tmpBuf,0,sizeof(tmpBuf));
	bcdhex_to_aschex(((struct sockaddr *)cli_addr)->sa_data,14,(char *)tmpBuf);
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"hsmClient %s %d",inet_ntoa(cli_addr->sin_addr),port)) == NULL)
	{
		UnionUserErrLog("in UnionHsmSynchTCPIPTaskServer::UnionCreateTaskInstance Error!\n");
		return(-1);
	}
	
	if ((ret = UnionConnectSoftHsmKeySpace()) < 0)	
	{
		printf("in UnionHsmSynchTCPIPTaskServer:: UnionConnectSoftHsmKeySpace\n");
		return(-1);
	}
	
	for (gsynchTCPIPSvrSckHDL=handle;;)
	{
		if ((ret = UnionReceiveFromSocketUntilLen(handle,tmpBuf,2)) != 2)
		{
			// UnionUserErrLog("in UnionHsmSynchTCPIPTaskServer:: UnionReceiveFromSocketUntilLen 2!\n");
			break;
		}
		lenOfReq = tmpBuf[0] * 256 + tmpBuf[1];
		if ((ret = UnionReceiveFromSocketUntilLen(handle,tmpBuf+2,lenOfReq)) != lenOfReq)
		{
			if (ret != 0)
			{
				UnionUserErrLog("in UnionHsmSynchTCPIPTaskServer:: UnionReceiveFromSocketUntilLen expected [%d] real = [%d]!\n",lenOfReq,ret);
			}
			else
				UnionLog("in UnionHsmSynchTCPIPTaskServer:: Connection Closed by Client!\n");
			break;
		}
		tmpBuf[2+lenOfReq] = 0;
		UnionLog("in UnionHsmSynchTCPIPTaskServer:: len = [%d] ReqBuf = [%s]\n",lenOfReq,tmpBuf+2);
		if ((lenOfRes = UnionSoftHsmCmdInterprotor(ghsmGrp,(char *)tmpBuf+2,lenOfReq,(char *)tmpBuf+2,sizeof(tmpBuf)-2)) < 0)
		{
			UnionUserErrLog("in UnionHsmSynchTCPIPTaskServer::UnionSoftHsmCmdInterprotor Error ret = [%d]!\n",ret);
			lenOfRes = 2;
			memcpy(tmpBuf+2,"SE",2);
		}
		tmpBuf[2+lenOfRes] = 0;
		tmpBuf[0] = lenOfRes / 256;
		tmpBuf[1] = lenOfRes % 256;
		UnionLog("in UnionHsmSynchTCPIPTaskServer:: len = [%d] resBuf = [%s]\n",lenOfRes,tmpBuf+2);
		if ((ret = UnionSendToSocket(handle,tmpBuf,lenOfRes+2)) < 0)
		{
			UnionUserErrLog("in UnionHsmSynchTCPIPTaskServer:: UnionSendToSocket [%d]!\n",ret);
			break;	
		}
	}
	//UnionClearESSCSvrEnv();
	//gsynchTCPIPSvrSckHDL = -1;
	//UnionCloseSocket(handle);
	return(ret);
}
