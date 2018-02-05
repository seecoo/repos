//	Author:		Wolfgang Wang
//	Date:		2003/09/28
//	Version:	3.0

//	History Of Modification
//	2003/09/28，在2.0基础上升级为3.0，未作任何改动

#define _UnionSocket_3_x_
#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <setjmp.h>
#include <signal.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "UnionSocket.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "UnionLog.h"

jmp_buf	UnionCreateSckCliJmpEnv;

void UnionDealCreateSckCliTimeout();
void UnionDealJmpEnvForSocketRec();

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
		return(errCodeParameter);
	}
	
	if (port < 0)
	{

		UnionUserErrLog("in UnionCreateSocketClient:: Minus Socket Port [%d]!\n",port);
		return(errCodeParameter);
	}
		
	memset((char *)(&psckadd),'0',sizeof(struct sockaddr_in));
	psckadd.sin_family            = AF_INET;
	psckadd.sin_addr.s_addr       = inet_addr(ip);
	psckadd.sin_port=htons((u_short)port);

	if ((sckcli = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		UnionSystemErrLog("in UnionCreatSocketClient:: socket! IP = [%s] port = [%d]\n",ip,port);
		return(errCodeUseOSErrCode);
	}

#if ( defined __linux__ ) || ( defined __hpux )
	if (sigsetjmp(UnionCreateSckCliJmpEnv,1) != 0)
#elif ( defined _AIX )
	if (setjmp(UnionCreateSckCliJmpEnv) != 0)
#endif
	{
		UnionUserErrLog("in UnionCreateSocketClient:: Timeout!\n");
		alarm(0);
		close(sckcli);
		return(errCodeSocketMDL_Timeout);
	}
	/**modify by xusj 20100224 begin ***/
	/*alarm(5);*/
	alarm(10);
	/**modify by xusj 20100224 end ***/
	signal(SIGALRM,UnionDealCreateSckCliTimeout);
	
	if (connect(sckcli,(struct sockaddr *)(&psckadd),sizeof(struct sockaddr_in)) < 0)
	{
		UnionSystemErrLog("in UnionCreatSocketClient:: connect! IP = [%s] port = [%d]\n",ip,port);
		//shutdown(sckcli,2);
		close(sckcli);
		alarm(0);
		return(errCodeUseOSErrCode);
	}
#ifdef _sendDataAfterClose_
	Linger.l_onoff = 0;
#else
	Linger.l_onoff = 1;
#endif
	Linger.l_linger = 0;
	if (setsockopt(sckcli,SOL_SOCKET,SO_LINGER,(char *)&Linger,sizeof(Linger)) != 0)
	{
		UnionSystemErrLog("in UnionCreatSocketClient:: setsockopt linger!");
		close(sckcli);
		alarm(0);
		return(errCodeUseOSErrCode);
	}

	if (setsockopt(sckcli, SOL_SOCKET, SO_OOBINLINE, (char *)&on, sizeof(on)))
	{
		UnionSystemErrLog("in UnionCreatSocketClient:: setsockopt SO_OOBINLINE!\n");
		close(sckcli);
		alarm(0);
		return(errCodeUseOSErrCode);
	}
 
	on = 1;
	if (setsockopt(sckcli, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on)))
	{
		UnionSystemErrLog("in UnionCreatSocketClient:: setsockopt: TCP_NODELAY");
		close(sckcli);
		alarm(0);
		return(errCodeUseOSErrCode);
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
		return(errCodeParameter);
	}
	if (buf == NULL)
	{
		UnionUserErrLog("in UnionSendToSocket:: Null Str to Send!\n");
		return(errCodeParameter);
	}
	
	for (;;)
	{
		if ((rc = send(fd,&buf[RealSend],len - RealSend,0)) != (int)len - RealSend)
		{
			UnionSystemErrLog("in UnionSendToSocket:: send! Real Send [%d] Expected Send = [%d]\n",rc,len - RealSend);
			if (rc < 0)
			{
				return(errCodeUseOSErrCode);
			}
			else
				RealSend += rc;
			if (RealSend > (int)len)
				return(errCodeUseOSErrCode);
			usleep(5);
		}
		else
			return(len);
	}
}

int UnionSendToSocketWithTowBytesLen(int fd,unsigned char *buf, unsigned int len)
{
	int		ret;
	unsigned char	lenBuf[2+1];
	
	lenBuf[0] = len / 256;
	lenBuf[1] = len % 256;
	
	if ((ret = send(fd,lenBuf,2,0)) != 2)
	{
		UnionSystemErrLog("in UnionSendToSocketWithTowBytesLen:: send len buf!\n");
		return(errCodeUseOSErrCode);
	}
	if ((ret = send(fd,buf,len,0)) != (int)len)
	{
		UnionSystemErrLog("in UnionSendToSocketWithTowBytesLen:: real send len = [%d] != expected [%d]!\n",ret,len);
		return(errCodeUseOSErrCode);
	}
	return(ret+2);
}		

int UnionReceiveFromSocket(int fd, unsigned char *buf,unsigned int len)
{
	int rc;
	
	if (fd < 0)
	{
		UnionUserErrLog("in UnionReceiveFromSocket:: SocketID = [%d] !\n",fd);
		return(errCodeParameter);
	}
	if (buf == NULL)
	{
		UnionUserErrLog("in UnionReceiveFromSocket:: Null buffer for receive!\n");
		return(errCodeParameter);
	}
	
	if ((rc = recv(fd,buf,len,0)) <= 0)
	{
		if (rc == 0)
		{
			UnionDebugLog("in UnionReceiveFromSocketUntilLen:: Connection is Closed!\n");
			return(rc);
		}
		if ((errno != 108) && (errno != 104) && (errno != 73) && (errno != 232) && (errno != 0) && (errno != 35) && (errno != 2))
		{
			UnionSystemErrLog("in UnionReceiveFromSocket:: recv! rc = [%d]\n",rc);
			return(errCodeUseOSErrCode);
		}
		else	
		{
			UnionDebugLog("in UnionReceiveFromSocket:: Connection is Closed! rc = [%d]\n",rc);
			return(0);
		}
	}
	else
	{
		return(rc);
	}
}

// 20101025, huangbx, 为星展银行修改特殊版本, recv收到关闭信号时, 继续等待接收
int gunionSIGPIPEflag = 0;
jmp_buf	gunionSocketJmpEnvForAutoSendTestBuf;
sigset_t		gunionNewset, gunionOldset;

void union_handle_pipe(int sig __attribute__((unused)))
{
	UnionAuditLog("warning:: handle_pipe!\n");
#if ( defined __linux__ ) || ( defined __hpux )
	siglongjmp(gunionSocketJmpEnvForAutoSendTestBuf,10);
#elif ( defined _AIX )
	longjmp(gunionSocketJmpEnvForAutoSendTestBuf,10);
#endif
}
void UnionSignalSet()
{
	sigemptyset(&gunionNewset);
	sigaddset(&gunionNewset, SIGALRM);
	if (sigprocmask(SIG_UNBLOCK, &gunionNewset, &gunionOldset) < 0)
		UnionSystemErrLog("in UnionSignalSet::sigprocmask\n");
}

void UnionSignalReset()
{
	if (sigprocmask(SIG_SETMASK, &gunionOldset, NULL) < 0)
		UnionSystemErrLog("in UnionSignalReset::sigprocmask\n");	
}
int UnionReceiveFromSocketUntilLen(int fd, unsigned char *buf,unsigned int len)
{
	int	rc;
	unsigned int	RecvLen;
	int	alarmTime = 30, leftAlarm = 0, lastAlarmFlag = 0;
	int	ret;
	char *testbuf="0000";

	if (fd < 0)
	{
		UnionUserErrLog("in UnionReceiveFromSocketUntilLen:: SocketID = [%d] !\n",fd);
		return(errCodeParameter);
	}
	if (buf == NULL)
	{
		UnionUserErrLog("in UnionReceiveFromSocketUntilLen:: Null buffer for receive!\n");
		return(errCodeParameter);
	}
	
	if (!gunionSIGPIPEflag) {
		signal(SIGPIPE,SIG_IGN);	// 忽略该信号量
		gunionSIGPIPEflag = 1;
	}
	
	// 保存信号量设置
	UnionSignalSet();
	
	leftAlarm = alarm(alarmTime);
	if (leftAlarm > 0)
		lastAlarmFlag = 1;
	UnionLog("in UnionReceiveFromSocketUntilLen::leftAlarm=[%d]\n", leftAlarm);
	
#if ( defined __linux__ ) || ( defined __hpux )
	if (sigsetjmp(gunionSocketJmpEnvForAutoSendTestBuf,1) != 0)
#elif ( defined _AIX )
	if (setjmp(gunionSocketJmpEnvForAutoSendTestBuf) != 0)
#endif
	{
		if ((ret = UnionSendToSocket(fd, (unsigned char *)testbuf, 4) < 0))
			UnionUserErrLog("in UnionReceiveFromSocketUntilLen:: UnionSendToSocket, send test buf[%d]!\n", ret);
		else
			UnionAuditLog("in UnionReceiveFromSocketUntilLen:: auto send tcpip test buf 0000!\n");
	}
	
	signal(SIGALRM, union_handle_pipe);
	
	if (!lastAlarmFlag)
		alarm(alarmTime);
	else if ((lastAlarmFlag) && (leftAlarm > alarmTime))
	{
		leftAlarm -= alarmTime;
		alarm(alarmTime);
	}
	else
	{
		UnionSignalReset();
		alarm(leftAlarm);
	}
	
	for (RecvLen = 0;;)
	{
		if ((rc = recv(fd,&buf[RecvLen],len - RecvLen,0)) <= 0)
		{
			if (rc == 0)
			{
				UnionDebugLog("in UnionReceiveFromSocketUntilLen:: Connection is Closed, recv 0, RecvLen=[%d], retry but not close!\n", RecvLen);
				if (lastAlarmFlag) {
					UnionSignalReset();
					alarm(leftAlarm);
				} else
					alarm(0);
					
				return(RecvLen);
			}
			
			if ((errno != 108) && (errno != 104) && (errno != 73) && (errno != 232) && (errno != 0) && (errno != 35) && (errno != 2))
			{
				UnionSystemErrLog("in UnionReceiveFromSocketUntilLen:: recv=[%d]!\n", rc);
				if (lastAlarmFlag) {
					UnionSignalReset();
					alarm(leftAlarm);
				} else
					alarm(0);
				return(errCodeUseOSErrCode);
			}
			else	
			{
				UnionDebugLog("in UnionReceiveFromSocketUntilLen:: Connection is Closed, rc=[%d], socket was closeed by peer!\n", rc);
				if (lastAlarmFlag) {
					UnionSignalReset();
					alarm(leftAlarm);
				} else
					alarm(0);
				return(RecvLen);
			}
		}
		RecvLen += rc;
		if (RecvLen == len)
		{
			break;
		}
	}
	
	if (lastAlarmFlag) {
		UnionSignalReset();
		alarm(leftAlarm);
	} else
		alarm(0);
	
	return(len);
}

void UnionDealJmpEnvForSocketRec()
{
	UnionUserErrLog("in UnionDealJmpEnvForSocketRec:: Timeout!\n");
#if ( defined __linux__ ) || ( defined __hpux )
	siglongjmp(UnionCreateSckCliJmpEnv,10);
#elif ( defined _AIX )
	longjmp(UnionCreateSckCliJmpEnv,10);
#endif
}

void UnionDealCreateSckCliTimeout()
{
	UnionUserErrLog("in UnionDealCreateSckCliTimeout:: Timeout!\n");
#if ( defined __linux__ ) || ( defined __hpux )
	siglongjmp(UnionCreateSckCliJmpEnv,10);
#elif ( defined _AIX )
	longjmp(UnionCreateSckCliJmpEnv,10);
#endif
}

int UnionInitializeTCPIPServer(int port)
{
	struct sockaddr_in	serv_addr;
	int	scksvr;
	int			ret;
	int			reuse;
	
	// Initialize the socket
	memset((char *)(&serv_addr), 0,sizeof(struct sockaddr_in));
	serv_addr.sin_family            = AF_INET;
	serv_addr.sin_addr.s_addr       = htonl(INADDR_ANY);
	serv_addr.sin_port = htons((u_short)port);

	if ((scksvr = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		UnionSystemErrLog("in UnionInitializeTCPIPServer:: socket()!\n");
		return(errCodeUseOSErrCode);
	}

	reuse = 1;
	if ((ret = setsockopt(scksvr, SOL_SOCKET, SO_REUSEADDR, (void*)&reuse, sizeof(int))) < 0)
	{
		UnionSystemErrLog("in UnionInitializeTCPIPServer:: setsockopt()!\n");
		UnionCloseSocket(scksvr);
		return(errCodeUseOSErrCode);
	}

	if (bind(scksvr,(struct sockaddr *)(&serv_addr),sizeof(struct sockaddr_in)) < 0)
	{
		UnionSystemErrLog("in UnionInitializeTCPIPServer:: bind()!\n");
		return(errCodeUseOSErrCode);
	}

	// Listen to new-coming connecting request
	if ( listen(scksvr, 10000) < 0 )
	{
		UnionSystemErrLog("in UnionInitializeTCPIPServer:: listen()!\n");
		return(errCodeUseOSErrCode);
	}

	return(scksvr);
}

int UnionNewTCPIPConnection(int scksvr)
{
	struct sockaddr_in	cli_addr;
	int	sckinstance;
	int			clilen;
	
	// Accept a Client's Connecting reqeust.
	clilen = sizeof(cli_addr);
	sckinstance = accept(scksvr, (struct sockaddr *)&cli_addr,(unsigned int*)&clilen);
	if ( sckinstance < 0 )
	{
		UnionSystemErrLog("in UnionTCPIPServer:: accept()!\n");
		return(errCodeUseOSErrCode);
	}
		
    	return(sckinstance);
}

/*
功能
	接收数据，数据以2个字节的二进数为长度
输入参数
	sckHDL		socket句柄
	sizeOfBuf	接收数据的缓冲区的大小
	timeout		超时
输出参数
	buf		接收数据的缓冲区
返回值
	>=0		接收的数据的长度
	<0		错误代码
*/
int UnionReceiveDataFromSocketWith2BytesLenWithTimeout(int sckHDL,char *buf,int sizeOfBuf,int timeout)
{
	unsigned char	lenBuf[2+1];
	int		ret;
	int		dataLen;
	
	if (buf == NULL)
		return(errCodeParameter);
		
	if (timeout < 0)
		timeout = 1;
	if (timeout == 0)
		goto recvLenNow;
#if ( defined __linux__ ) || ( defined __hpux )
	if (sigsetjmp(UnionCreateSckCliJmpEnv,1) != 0)	// 超时退出
#elif ( defined _AIX )
	if (setjmp(UnionCreateSckCliJmpEnv) != 0) // 超时退出
	{
		ret = errCodeAPIRecvClientReqTimeout;
		UnionUserErrLog("in UnionReceiveDataFromSocketWith2BytesLenWithTimeout:: recv from client time out! ret = [%d]\n",ret);
		goto errorExit;
	}
#endif
	alarm(timeout);
	signal(SIGALRM,UnionDealJmpEnvForSocketRec);

recvLenNow:
	// 开始接收数据长度
	memset(lenBuf,0,2);
	if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,lenBuf,2)) < 0)
	{
		UnionUserErrLog("in UnionReceiveDataFromSocketWith2BytesLenWithTimeout:: UnionReceiveFromSocketUntilLen 2! ret = [%d]\n",ret);
		goto errorExit;
	}
	if (ret == 0)
	{
		UnionLog("in UnionReceiveDataFromSocketWith2BytesLenWithTimeout:: connection closed by peer!\n");
		return(errCodePeerCloseSckConn);
	}
	if (((dataLen = lenBuf[0] * 256 + lenBuf[1]) < 0) || (dataLen >= sizeOfBuf))
	{
		ret = errCodeAPIClientReqLen;
		UnionUserErrLog("in UnionReceiveDataFromSocketWith2BytesLenWithTimeout:: dataLen = [%d] error! ret = [%d]\n",dataLen,ret);
		goto errorExit;
	}
	if (dataLen == 0)	// 测试报文
	{
		UnionLog("in UnionReceiveDataFromSocketWith2BytesLenWithTimeout:: testing pack received!\n");
		alarm(0);
		return(0);
	}
	if (timeout == 0)	// 接收长度时没有限超时，但接收数据要限超时
	{
		timeout = 1;
#if ( defined __linux__ ) || ( defined __hpux )
		if (sigsetjmp(UnionCreateSckCliJmpEnv,1) != 0)	// 超时退出
#elif ( defined _AIX )
		if (setjmp(UnionCreateSckCliJmpEnv) != 0) // 超时退出
		{
			ret = errCodeAPIRecvClientReqTimeout;
			UnionUserErrLog("in UnionReceiveDataFromSocketWith2BytesLenWithTimeout:: recv from client time out! ret = [%d]\n",ret);
			goto errorExit;
		}
#endif
		alarm(timeout);
		signal(SIGALRM,UnionDealJmpEnvForSocketRec);
	}
	// 接收数据
	if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,(unsigned char *)buf,dataLen)) != dataLen)
	{
		UnionUserErrLog("in UnionReceiveDataFromSocketWith2BytesLenWithTimeout:: UnionReceiveFromSocketUntilLen expected [%d] != real = [%d]!\n",dataLen,ret);
		ret = errCodeAPIClientReqLen;
		goto errorExit;
	}
	alarm(0);
	return(dataLen);
	
errorExit:
	alarm(0);
	UnionCloseSocket(sckHDL);
	return(ret);
}

/*
功能
	接收数据，数据以2个字节的二进数为长度，二进制长度也包括在接收的数据中
输入参数
	sckHDL		socket句柄
	sizeOfBuf	接收数据的缓冲区的大小
	timeout		超时
输出参数
	buf		接收数据的缓冲区
返回值
	>=0		接收的数据的长度，包括二进制的长度域在内
	<0		错误代码
*/
int UnionReceiveAllDataFromSocketWith2BytesLenWithTimeout(int sckHDL,unsigned char *buf,int sizeOfBuf,int timeout)
{
	int		ret;
	int		dataLen;
	
	if (buf == NULL)
		return(errCodeParameter);
		
	if (timeout < 0)
		timeout = 1;
	if (timeout == 0)
		goto recvLenNow;
#if ( defined __linux__ ) || ( defined __hpux )
	if (sigsetjmp(UnionCreateSckCliJmpEnv,1) != 0)	// 超时退出
#elif ( defined _AIX )
	if (setjmp(UnionCreateSckCliJmpEnv) != 0) // 超时退出
	{
		ret = errCodeAPIRecvClientReqTimeout;
		UnionUserErrLog("in UnionReceiveAllDataFromSocketWith2BytesLenWithTimeout:: recv from client time out! ret = [%d]\n",ret);
		goto errorExit;
	}
#endif
	alarm(timeout);
	signal(SIGALRM,UnionDealJmpEnvForSocketRec);

recvLenNow:
	// 开始接收数据长度
	memset(buf,0,2);
	if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,buf,2)) < 0)
	{
		UnionUserErrLog("in UnionReceiveAllDataFromSocketWith2BytesLenWithTimeout:: UnionReceiveFromSocketUntilLen 2! ret = [%d]\n",ret);
		goto errorExit;
	}
	if (ret == 0)
	{
		UnionLog("in UnionReceiveAllDataFromSocketWith2BytesLenWithTimeout:: connection closed by peer!\n");
		return(errCodePeerCloseSckConn);
	}
	if (((dataLen = buf[0] * 256 + buf[1]) < 0) || (dataLen >= sizeOfBuf))
	{
		ret = errCodeAPIClientReqLen;
		UnionUserErrLog("in UnionReceiveAllDataFromSocketWith2BytesLenWithTimeout:: dataLen = [%d] error! ret = [%d]\n",dataLen,ret);
		goto errorExit;
	}
	if (dataLen == 0)	// 测试报文
	{
		UnionLog("in UnionReceiveAllDataFromSocketWith2BytesLenWithTimeout:: testing pack received!\n");
		alarm(0);
		return(0);
	}
	if (timeout == 0)	// 接收长度时没有限超时，但接收数据要限超时
	{
		timeout = 1;
#if ( defined __linux__ ) || ( defined __hpux )
		if (sigsetjmp(UnionCreateSckCliJmpEnv,1) != 0)	// 超时退出
#elif ( defined _AIX )
		if (setjmp(UnionCreateSckCliJmpEnv) != 0) // 超时退出
		{
			ret = errCodeAPIRecvClientReqTimeout;
			UnionUserErrLog("in UnionReceiveAllDataFromSocketWith2BytesLenWithTimeout:: recv from client time out! ret = [%d]\n",ret);
			goto errorExit;
		}
#endif
		alarm(timeout);
		signal(SIGALRM,UnionDealJmpEnvForSocketRec);
	}
	// 接收数据
	if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,buf+2,dataLen)) != dataLen)
	{
		UnionUserErrLog("in UnionReceiveAllDataFromSocketWith2BytesLenWithTimeout:: UnionReceiveFromSocketUntilLen expected [%d] != real = [%d]!\n",dataLen,ret);
		ret = errCodeAPIClientReqLen;
		goto errorExit;
	}
	alarm(0);
	return(dataLen+2);
	
errorExit:
	alarm(0);
	UnionCloseSocket(sckHDL);
	return(ret);
}

/*
功能
	在指定的连接上交换一个报文
输入参数
	sckHDL		连接
	reqStr		请求数据串
	lenOfReqStr	请求数据串长度
	sizeOfBuf	接收数据的缓冲区的大小
	timeout		超时
输出参数
	resStr		接收数据的缓冲区
返回值
	>=0		接收的数据的长度，不包括二进制的长度域在内
	<0		错误代码
*/
int UnionCommWithSpecSckHDLWith2BytsLen(int sckHDL,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfBuf,int timeout)
{
	int	ret;
	
	if ((ret = UnionSendToSocketWithTowBytesLen(sckHDL,reqStr,lenOfReqStr)) < 0)
	{
		UnionUserErrLog("in UnionCommWithSpecSckHDLWith2BytsLen:: UnionSendToSocketWithTowBytesLen!\n");
		return(ret);
	}

	if ((ret = UnionReceiveDataFromSocketWith2BytesLenWithTimeout(sckHDL,(char *)resStr,sizeOfBuf,timeout)) < 0)
	{
		if (ret != errCodePeerCloseSckConn)
			UnionUserErrLog("in UnionCommWithSpecSckHDLWith2BytsLen:: UnionReceiveDataFromSocketWith2BytesLenWithTimeout!\n");
	}
 
 	return(ret);
}

/*
功能
	与指定的服务器交换一个报文
输入参数
	ipAddr		服务器的IP地址，不指定，则采用127.0.0.1
	port		服务器的端口
	reqStr		请求数据串
	lenOfReqStr	请求数据串长度
	sizeOfBuf	接收数据的缓冲区的大小
	timeout		超时
输出参数
	resStr		接收数据的缓冲区
返回值
	>=0		接收的数据的长度，不包括二进制的长度域在内
	<0		错误代码
*/
int UnionCommWithSpecSvrWith2BytsLen(char *ipAddr,int port,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfBuf,int timeout)
{
	char	localIPAddr[] = "127.0.0.1";
	char	*thisIPAddr;
	int	sckHDL;
	int	ret;
	
	if (ipAddr == NULL)
		thisIPAddr = localIPAddr;
	else
		thisIPAddr = ipAddr;

	if ((sckHDL = UnionCreateSocketClient(thisIPAddr,port)) < 0)
	{
		UnionUserErrLog("in UnionCommWithSpecSvrWith2BytsLen:: UnionCreateSocketClient! [%s][%d]\n",thisIPAddr,port);
		return(sckHDL);
	}
	
	if ((ret = UnionSendToSocketWithTowBytesLen(sckHDL,reqStr,lenOfReqStr)) < 0)
	{
		UnionUserErrLog("in UnionCommWithSpecSvrWith2BytsLen:: UnionSendToSocketWithTowBytesLen! [%s][%d]\n",thisIPAddr,port);
		UnionCloseSocket(sckHDL);
		return(ret);
	}

	if ((ret = UnionReceiveDataFromSocketWith2BytesLenWithTimeout(sckHDL,(char *)resStr,sizeOfBuf,timeout)) < 0)
	{
		if (ret != errCodePeerCloseSckConn)
			UnionUserErrLog("in UnionCommWithSpecSvrWith2BytsLen:: UnionReceiveDataFromSocketWith2BytesLenWithTimeout! [%s][%d]\n",thisIPAddr,port);
	}
 
 	UnionCloseSocket(sckHDL);
 	return(ret);
}


/*
功能
	在指定的连接上交换一个报文
输入参数
	sckHDL		连接
	reqStr		请求数据串
	lenOfReqStr	请求数据串长度
	sizeOfBuf	接收数据的缓冲区的大小
	timeout		超时
输出参数
	resStr		接收数据的缓冲区
返回值
	>=0		接收的数据的长度，不包括二进制的长度域在内
	<0		错误代码
*/
int UnionCommWithSpecSckHDLWithoutLen(int sckHDL,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfBuf)
{
	int	ret;
	
	if ((ret = UnionSendToSocket(sckHDL,reqStr,lenOfReqStr)) < 0)
	{
		UnionUserErrLog("in UnionCommWithSpecSckHDLWithoutLen:: UnionSendToSocketWithTowBytesLen!\n");
		return(ret);
	}

	if ((ret = UnionReceiveFromSocket(sckHDL,resStr,sizeOfBuf)) < 0)
	{
		if (ret != errCodePeerCloseSckConn)
			UnionUserErrLog("in UnionCommWithSpecSckHDLWithoutLen:: UnionReceiveFromSocket!\n");
	}
 
 	return(ret);
}

/*
功能
	在指定的连接上交换一个报文,报文采用2进制
输入参数
	sckHDL		连接
	reqStr		请求数据串
	lenOfReqStr	请求数据串长度
	sizeOfBuf	接收数据的缓冲区的大小
输出参数
	resStr		接收数据的缓冲区
返回值
	>=0		接收的数据的长度，不包括二进制的长度域在内
	<0		错误代码
*/
int UnionCommWithSpecSckHDLInBitsFormatWithoutLen(int sckHDL,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfBuf)
{
	int	ret;
	unsigned char	tmpBuf[4096+1];
	
	if (lenOfReqStr >= (int)sizeof(tmpBuf))
	{
		UnionUserErrLog("in UnionCommWithSpecSckHDLInBitsFormatWithoutLen:: lenOfReqStr [%d] larger than expected [%zu]!\n",lenOfReqStr,sizeof(tmpBuf));
		return(errCodeSmallBuffer);
	}
	aschex_to_bcdhex((char *)reqStr,lenOfReqStr,(char *)tmpBuf);
	if ((ret = UnionCommWithSpecSckHDLWithoutLen(sckHDL,tmpBuf,lenOfReqStr/2,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionCommWithSpecSckHDLInBitsFormatWithoutLen:: UnionCommWithSpecSckHDLWithoutLen!\n");
		return(ret);
	}
	if (ret * 2 >= sizeOfBuf)
	{
		UnionUserErrLog("in UnionCommWithSpecSckHDLInBitsFormatWithoutLen:: sizeOfBuf [%d] smaller than expected [%d]!\n",sizeOfBuf,ret*2);
		return(errCodeSmallBuffer);
	}
	bcdhex_to_aschex((char *)tmpBuf,ret,(char *)resStr);
	return(ret*2);
}

/*
功能
	与指定的服务器交换一个报文
输入参数
	ipAddr		服务器的IP地址，不指定，则采用127.0.0.1
	port		服务器的端口
	reqStr		请求数据串
	lenOfReqStr	请求数据串长度
	sizeOfBuf	接收数据的缓冲区的大小
输出参数
	resStr		接收数据的缓冲区
返回值
	>=0		接收的数据的长度，不包括二进制的长度域在内
	<0		错误代码
*/
int UnionCommWithSpecSvrWithoutLen(char *ipAddr,int port,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfBuf)
{
	char	localIPAddr[] = "127.0.0.1";
	char	*thisIPAddr;
	int	sckHDL;
	int	ret;
	
	if (ipAddr == NULL)
		thisIPAddr = localIPAddr;
	else
		thisIPAddr = ipAddr;

	if ((sckHDL = UnionCreateSocketClient(thisIPAddr,port)) < 0)
	{
		UnionUserErrLog("in UnionCommWithSpecSvrWithoutLen:: UnionCreateSocketClient! [%s][%d]\n",thisIPAddr,port);
		return(sckHDL);
	}
	
	if ((ret = UnionCommWithSpecSckHDLWithoutLen(sckHDL,reqStr,lenOfReqStr,resStr,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionCommWithSpecSvrWithoutLen:: UnionCommWithSpecSckHDLWithoutLen! [%s][%d]\n",thisIPAddr,port);
	}
 
 	UnionCloseSocket(sckHDL);
 	return(ret);
}

/*
功能
	与指定的服务器交换一个报文,报文采用二进制数
输入参数
	ipAddr		服务器的IP地址，不指定，则采用127.0.0.1
	port		服务器的端口
	reqStr		请求数据串
	lenOfReqStr	请求数据串长度
	sizeOfBuf	接收数据的缓冲区的大小
输出参数
	resStr		接收数据的缓冲区
返回值
	>=0		接收的数据的长度，不包括二进制的长度域在内
	<0		错误代码
*/
int UnionCommWithSpecSvrInBitsFormatWithoutLen(char *ipAddr,int port,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfBuf)
{
	char	localIPAddr[] = "127.0.0.1";
	char	*thisIPAddr;
	int	sckHDL;
	int	ret;
	
	if (ipAddr == NULL)
		thisIPAddr = localIPAddr;
	else
		thisIPAddr = ipAddr;

	if ((sckHDL = UnionCreateSocketClient(thisIPAddr,port)) < 0)
	{
		UnionUserErrLog("in UnionCommWithSpecSvrInBitsFormatWithoutLen:: UnionCreateSocketClient! [%s][%d]\n",thisIPAddr,port);
		return(sckHDL);
	}
	
	if ((ret = UnionCommWithSpecSckHDLInBitsFormatWithoutLen(sckHDL,reqStr,lenOfReqStr,resStr,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionCommWithSpecSvrInBitsFormatWithoutLen:: UnionCommWithSpecSckHDLInBitsFormatWithoutLen! [%s][%d]\n",thisIPAddr,port);
	}
 
 	UnionCloseSocket(sckHDL);
 	return(ret);
}
