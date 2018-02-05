//	Author:		Wolfgang Wang
//	Date:		2002/01/22

//	History Of Modification
//	2002/3/6, Wolfang Wang, Add a funcation UnionReceiveFromSocketUntilLen

// 与1.2版相比，增加了函数：
/*			int UnionNewTCPIPConnection(int scksvr)
			int UnionInitializeTCPIPServer(int port)
*/

/* 与以前的版本相比，将创建Socket的超时由30秒改为5秒 */

/* 在1.4版本基础上改写 */
/* 与1.4代码完全相同，仅是为了与UnionTCPIPSvr保持一致版本而设计 */

#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>
#include <errno.h>


#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "UnionSocket.h"
#include "UnionLog.h"

jmp_buf	UnionCreateSckCliJmpEnv;

void UnionDealCreateSckCliTimeout();

int UnionCloseSocket(int handle)
{
	return(close(handle));
}

int UnionCreateSocketClient(char *ip,int port)
{
	struct				sockaddr_in psckadd;
	int				sckcli;
	struct linger			Linger;
	int				on=1;

	if (ip == NULL)
	{
		UnionUserErrLog("in UnionCreateSocketClient:: NULL IPAddress!\n");
		return(UnionErrNullIPAddressPassed);
	}
	
	if (port < 0)
	{

		UnionUserErrLog("in UnionCreateSocketClient:: Minus Socket Port [%d]!\n",port);
		return(UnionErrMinusSocketPortPassed);
	}
		
	memset((char *)(&psckadd),'0',sizeof(struct sockaddr_in));
	psckadd.sin_family            = AF_INET;
	psckadd.sin_addr.s_addr       = inet_addr(ip);
	psckadd.sin_port=htons((u_short)port);

	if ((sckcli = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		UnionSystemErrLog("in UnionCreatSocketClient:: socket! IP = [%s] port = [%d]\n",ip,port);
		return(UnionErrCallingSocket);
	}

	if (setjmp(UnionCreateSckCliJmpEnv) != 0)
	{
		UnionUserErrLog("in UnionCreateSocketClient:: Timeout!\n");
		alarm(0);
		close(sckcli);
		return(-1);
	}
	alarm(5);
	signal(SIGALRM,UnionDealCreateSckCliTimeout);
	
	if (connect(sckcli,(struct sockaddr *)(&psckadd),sizeof(struct sockaddr_in)) < 0)
	{
		UnionSystemErrLog("in UnionCreatSocketClient:: connect! IP = [%s] port = [%d]\n",ip,port);
		//shutdown(sckcli,2);
		close(sckcli);
		alarm(0);
		return(UnionErrCallingConnect);
	}

	Linger.l_onoff = 1;
	Linger.l_linger = 0;
	if (setsockopt(sckcli,SOL_SOCKET,SO_LINGER,(char *)&Linger,sizeof(Linger)) != 0)
	{
		UnionSystemErrLog("in UnionCreatSocketClient:: setsockopt linger!");
		close(sckcli);
		alarm(0);
		return(UnionErrCallingSetsockopt);
	}

	if (setsockopt(sckcli, SOL_SOCKET, SO_OOBINLINE, (char *)&on, sizeof(on)))
	{
		UnionSystemErrLog("in UnionCreatSocketClient:: setsockopt SO_OOBINLINE!\n");
		close(sckcli);
		alarm(0);
		return(UnionErrCallingSetsockopt);
	}
 
	on = 1;
	if (setsockopt(sckcli, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on)))
	{
		UnionSystemErrLog("in UnionCreatSocketClient:: setsockopt: TCP_NODELAY");
		close(sckcli);
		alarm(0);
		return(UnionErrCallingSetsockopt);
	}

	alarm(0);
	return(sckcli);
}

int UnionSendToSocket(int fd,unsigned char *buf, unsigned int len)
{
	int	rc;
	int	RealSend = 0;
	
	if (fd < 0)
	{
		UnionUserErrLog("in UnionSendToSocket:: SocketID = [%d] !\n",fd);
		return(-1);
	}
	if (buf == NULL)
	{
		UnionUserErrLog("in UnionSendToSocket:: Null Str to Send!\n");
		return(-1);
	}
	
	for (;;)
	{
		if ((rc = send(fd,&buf[RealSend],len - RealSend,0)) != len - RealSend)
		{
			UnionSystemErrLog("in UnionSendToSocket:: send! Real Send [%d] Expected Send = [%d]\n",rc,len - RealSend);
			if (rc < 0)
			{
				return(UnionErrCallingSend);
			}
			else
				RealSend += rc;
			if (RealSend > len)
				return(UnionErrCallingSend);
			usleep(5);
		}
		else
			return(len);
	}
}

int UnionReceiveFromSocket(int fd, unsigned char *buf,unsigned int len)
{
	int rc;
	
	if (fd < 0)
	{
		UnionUserErrLog("in UnionReceiveFromSocket:: SocketID = [%d] !\n",fd);
		return(-1);
	}
	if (buf == NULL)
	{
		UnionUserErrLog("in UnionReceiveFromSocket:: Null buffer for receive!\n");
		return(-1);
	}
	
	if ((rc = recv(fd,buf,len,0)) <= 0)
	{
		if (errno != 108)
		{
			UnionSystemErrLog("in UnionReceiveFromSocket:: recv! rc = [%d]\n",rc);
			return(UnionErrCallingRecv);
		}
		else	
		{
			UnionLog("in UnionReceiveFromSocket:: Connection is Closed! rc = [%d]\n",rc);
			return(0);
		}
	}
	else
	{
		return(rc);
	}
}

int UnionReceiveFromSocketUntilLen(int fd, unsigned char *buf,unsigned int len)
{
	int	rc;
	int	RecvLen;
	
	if (fd < 0)
	{
		UnionUserErrLog("in UnionReceiveFromSocketUntilLen:: SocketID = [%d] !\n",fd);
		return(-1);
	}
	if (buf == NULL)
	{
		UnionUserErrLog("in UnionReceiveFromSocketUntilLen:: Null buffer for receive!\n");
		return(-1);
	}
	
	for (RecvLen = 0;;)
	{
		if ((rc = recv(fd,&buf[RecvLen],len - RecvLen,0)) <= 0)
		{
			if (errno != 108)
			{
				UnionSystemErrLog("in UnionReceiveFromSocketUntilLen:: recv!\n");
				return(UnionErrCallingRecv);
			}
			else	
			{
				UnionLog("in UnionReceiveFromSocketUntilLen:: Connection is Closed!\n");
				return(RecvLen);
			}
		}
		RecvLen += rc;
		if (RecvLen == len)
		{
			break;
		}
	}
	return(len);
}

void UnionDealCreateSckCliTimeout()
{
	UnionUserErrLog("in UnionDealCreateSckCliTimeout:: Timeout!\n");
	longjmp(UnionCreateSckCliJmpEnv,10);
}

int UnionInitializeTCPIPServer(int port)
{
	struct sockaddr_in	serv_addr;
	int	scksvr;
	int			ret;

	// Initialize the socket
	memset((char *)(&serv_addr), 0,sizeof(struct sockaddr_in));
	serv_addr.sin_family            = AF_INET;
	serv_addr.sin_addr.s_addr       = htonl(INADDR_ANY);
	serv_addr.sin_port = htons((u_short)port);

	if ((scksvr = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		UnionSystemErrLog("in UnionInitializeTCPIPServer:: socket()!\n");
		return(UnionErrCallingSocket);
	}

	if (bind(scksvr,(struct sockaddr *)(&serv_addr),sizeof(struct sockaddr_in)) < 0)
	{
		UnionSystemErrLog("in UnionInitializeTCPIPServer:: bind()!\n");
		return(UnionErrCallingBind);
	}

	// Listen to new-coming connecting request
	if ( listen(scksvr, 10000) < 0 )
	{
		UnionSystemErrLog("in UnionInitializeTCPIPServer:: listen()!\n");
		return(UnionErrCallingListen);
	}

	return(scksvr);
}

int UnionNewTCPIPConnection(int scksvr)
{
	struct sockaddr_in	cli_addr;
	int	sckinstance;
	int			clilen;
	char			cliip[40];
	
	// Accept a Client's Connecting reqeust.
	clilen = sizeof(cli_addr);
	sckinstance = accept(scksvr, (struct sockaddr *)&cli_addr,&clilen);
	if ( sckinstance < 0 )
	{
		UnionSystemErrLog("in UnionTCPIPServer:: accept()!\n");
		return(UnionErrCallingAccept);
	}
		
    	return(sckinstance);
}
