//	Author:		Wolfgang Wang
//	Date:		2001/7/25

// 与1.2版相比，将函数：
/*			int UnionNewTCPIPConnection(int scksvr)
			int UnionInitializeTCPIPServer(int port)
   移到UnionSocket1.3.c中。
*/

// 与1.3版相比，在UnionTCPIPServer中，创建了一个子进程 */

#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "UnionTask.h"
#include "UnionSocket.h"
#include "UnionLog.h"

int UnionTCPIPServer(int port,char *ServerName)
{
	struct sockaddr_in	serv_addr;
	int			scksvr;
	struct sockaddr_in	cli_addr;
	int			sckinstance;
	int			clilen;
	char			cliip[40];
	int			ret;

	// Initialize the socket
	memset((char *)(&serv_addr), 0,sizeof(struct sockaddr_in));
	serv_addr.sin_family            = AF_INET;
	serv_addr.sin_addr.s_addr       = htonl(INADDR_ANY);
	serv_addr.sin_port = htons((u_short)port);

	if ((scksvr = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		UnionSystemErrLog("in UnionTCPIPServer:: socket()!\n");
		return(UnionErrCallingSocket);
	}

	if (bind(scksvr,(struct sockaddr *)(&serv_addr),sizeof(struct sockaddr_in)) < 0)
	{
		UnionSystemErrLog("in UnionTCPIPServer:: bind()!\n");
		return(UnionErrCallingBind);
	}

	// Listen to new-coming connecting request
	if ( listen(scksvr, 10000) < 0 )
	{
		UnionSystemErrLog("in UnionTCPIPServer:: listen()!\n");
		return(UnionErrCallingListen);
	}

	if (UnionCreateProcess() > 0)
		return(0);
	
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
		
		// Create a new process or a new thread, one of the two process is listening
		// to other connecting request from clients, whereas the other is to fulfill
		// the task and return a response to the connected client, when necessary.
		// When the task process finish fulfiling the task, it should terminate itself
		// automatically.
	    	switch (UnionCreateProcess())
		{
			case 0:
				UnionRegisterTask(ServerName);
				UnionCloseSocket(sckinstance);
				continue;
			case -1:
				UnionUserErrLog("in UnionTCPIPServer:: UnionCreateProcess!\n");
				UnionCloseSocket(sckinstance);
				continue;
			default:
				UnionUnregisterTask();
				if ( (ret = UnionTCPIPTaskServer(sckinstance,inet_ntoa(cli_addr.sin_addr),port)) != 0)
					UnionUserErrLog("in UnionTCPIPServer:: UnionTCPIPTaskServer!\n");
				UnionCloseSocket(sckinstance);
				return(ret);
		}
				
    	}
}

