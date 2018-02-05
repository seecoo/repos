//	Author:		Wolfgang Wang
//	Date:		2003/09/28
//	Version:	3.0

//	History Of Modification
//	2003/09/28，在2.1基础上升级为3.0


#define _UnionSocket_3_x_
#define _UnionTask_3_x_
#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "UnionTask.h"
#include "UnionSocket.h"
#include "unionErrCode.h"
#include "UnionLog.h"

extern PUnionTaskInstance	ptaskInstance;

int UnionTCPIPServer(int port,char *ServerName,int (*UnionTCPIPTaskServer)(),int (*UnionTaskActionBeforeExit)())
{
	struct sockaddr_in	serv_addr;
	int			scksvr;
	struct sockaddr_in	cli_addr;
	int			sckinstance;
	int			clilen;
	int			ret;
	struct linger		Linger;	// Added by Wolfgang Wang, 2004/7/13
	int			reuse;
	
	// Initialize the socket
	memset((char *)(&serv_addr), 0,sizeof(struct sockaddr_in));
	serv_addr.sin_family            = AF_INET;
	serv_addr.sin_addr.s_addr       = htonl(INADDR_ANY);
	serv_addr.sin_port = htons((u_short)port);

	if ((scksvr = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		UnionSystemErrLog("in UnionTCPIPServer:: socket()!\n");
		return(errCodeUseOSErrCode);
	}

	reuse = 1;
	if ((ret = setsockopt(scksvr, SOL_SOCKET, SO_REUSEADDR, (void*)&reuse, sizeof(int))) < 0)
	{
		UnionSystemErrLog("in UnionTCPIPServer:: setsockopt()!\n");
		UnionCloseSocket(scksvr);
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

	UnionAuditLog("in UnionTCPIPServer:: a servier bound to [%d] started OK!\n",port);
			
	for (;;)
	{
		// Accept a Client's Connecting reqeust.
		clilen = sizeof(cli_addr);
		sckinstance = accept(scksvr, (struct sockaddr *)&cli_addr,(unsigned int*)&clilen);
		if ( sckinstance < 0 )
		{
			UnionSystemErrLog("in UnionTCPIPServer:: accept()!\n");
			continue;
		}
		sleep(1);
		// Added by Wolfgang Wang, 2004/07/13
		Linger.l_onoff = 1;
		Linger.l_linger = 0;
		if (setsockopt(sckinstance,SOL_SOCKET,SO_LINGER,(char *)&Linger,sizeof(Linger)) != 0)
		{
			UnionSystemErrLog("in UnionTCPIPServerf:: setsockopt linger!");
			return(errCodeUseOSErrCode);
			//continue;
		}

		// End of Addition of 2004/07/13
		
		// Create a new process or a new thread, one of the two process is listening
		// to other connecting request from clients, whereas the other is to fulfill
		// the task and return a response to the connected client, when necessary.
		// When the task process finish fulfiling the task, it should terminate itself
		// automatically.
	    	switch (UnionCreateProcess())
		{
			case 0:
				UnionCloseSocket(sckinstance);
				if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s %d",ServerName,port)) == NULL)
				{
					UnionUserErrLog("in UnionTCPIPServer::UnionCreateTaskInstance Error!\n");
					return(errCodeCreateTaskInstance);
				}
				continue;
			case -1:
				UnionCloseSocket(sckinstance);
				UnionUserErrLog("in UnionTCPIPServer:: UnionCreateProcess!\n");
				continue;
			default:
				if ( (ret = (*UnionTCPIPTaskServer)(sckinstance,&cli_addr,port,UnionTaskActionBeforeExit)) < 0)
					UnionUserErrLog("in UnionTCPIPServer:: UnionTCPIPTaskServer!\n");
				UnionCloseSocket(sckinstance);
				return(ret);
		}
				
    	}
}

