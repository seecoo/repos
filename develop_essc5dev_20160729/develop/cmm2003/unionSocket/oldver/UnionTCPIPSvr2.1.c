//	Author:		Wolfgang Wang
//	Date:		2001/7/25

// 与1.2版相比，将函数：
/*			int UnionNewTCPIPConnection(int scksvr)
			int UnionInitializeTCPIPServer(int port)
   移到UnionSocket1.3.c中。
*/

// 与1.3版相比，在UnionTCPIPServer中，创建了一个子进程 */

/* 本版本在1.4版本基础上改写 */
// 2002/2/18, 将UnionTCPIPServer升级了。2.x以前版本，带两个参数，2.x版本带三个参数，第三个参数为一个函数指针。


// 2003/02/26，王纯军，侦听进程的注册名有问题，对之进行了改进。
/*
	socket通讯中，若客户端和服务器建立长链接，服务器程序关闭链接，终止进程后，
	若客户端没有关闭该链接，则不能重新启动服务器程序，bind函数调用失败。
	在服务器程序的accept函数调用之后增加语句，即使客户端没有关闭链接，
	服务器程序也可以主动关闭socket链接，重新启动。
*/

#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#ifndef _UnionSocket_2_x_
#define _UnionSocket_2_x_
#endif

#define _UnionTask_2_x_
#include "UnionTask.h"
#include "UnionSocket.h"
#include "UnionLog.h"

int UnionTCPIPServer(int port,char *ServerName,int (*UnionTCPIPTaskServer)())
{
	struct sockaddr_in	serv_addr;
	int			scksvr;
	struct sockaddr_in	cli_addr;
	int			sckinstance;
	int			clilen;
	char			cliip[40];
	int			ret;
	struct linger		Linger;	// Added by Wolfgang Wang, 2003/02/26
	
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

	UnionRegisterTask("%s %d",ServerName,port);
	UnionSuccessLog("in UnionTCPIPServer:: %s %d is started OK!\n",ServerName,port);
			
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
		
		// Added by Wolfgang Wang, 2003/02/26
		Linger.l_onoff = 1;
		Linger.l_linger = 0;
		if (setsockopt(sckinstance,SOL_SOCKET,SO_LINGER,(char *)&Linger,sizeof(Linger)) != 0)
		{
			UnionSystemErrLog("in UnionCreatSocketClient:: setsockopt linger!");
			close(sckinstance);
			return(UnionErrCallingSetsockopt);
		}
		// End of Addition of 2003/02/26
		
		// Create a new process or a new thread, one of the two process is listening
		// to other connecting request from clients, whereas the other is to fulfill
		// the task and return a response to the connected client, when necessary.
		// When the task process finish fulfiling the task, it should terminate itself
		// automatically.
	    	switch (UnionCreateProcess())
		{
			case 0:
				UnionRegisterTask("%s %d",ServerName,port);
				UnionCloseSocket(sckinstance);
				continue;
			case -1:
				UnionUserErrLog("in UnionTCPIPServer:: UnionCreateProcess!\n");
				UnionCloseSocket(sckinstance);
				continue;
			default:
				UnionUnregisterTask();
				if ( (ret = (*UnionTCPIPTaskServer)(sckinstance,inet_ntoa(cli_addr.sin_addr),port)) != 0)
					UnionUserErrLog("in UnionTCPIPServer:: UnionTCPIPTaskServer!\n");
				UnionCloseSocket(sckinstance);
				return(ret);
		}
				
    	}
}

