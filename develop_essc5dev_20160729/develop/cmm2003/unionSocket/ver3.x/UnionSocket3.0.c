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
#include <netdb.h>

#include "UnionSocket.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "UnionLog.h"
#include "unionREC.h"

jmp_buf	UnionCreateSckCliJmpEnv;

void UnionDealCreateSckCliTimeout();
void UnionDealJmpEnvForSocketRec();

int UnionGetHostByName(char *domain, char *ip)
{
        int                             index;
        struct hostent                  *pHostent;
        char                            ipAddr[128+8+1];
        char                            **ipAddrList;

        pHostent = gethostbyname(domain);
        if (pHostent == NULL)
        {
                UnionSystemErrLog("in UnionGetHostByName:: gethostbyname! domain = [%s] !\n", domain);
                return(errCodeUseOSErrCode);
        }

        memset(ipAddr, 0, sizeof(ipAddr));
        switch(pHostent->h_addrtype)
        {
                case AF_INET:
                //case AF_INET6:
                        ipAddrList = pHostent->h_addr_list;
                        for(index = 0; *ipAddrList != NULL; ipAddrList++, index++)
                        {
                                memset(ipAddr, 0, sizeof(ipAddr));
                                inet_ntop(pHostent->h_addrtype, *ipAddrList, ipAddr,sizeof(ipAddr));
                                //UnionProgramerLog("in UnionGetHostByName:: ip[%d] = [%s]!\n", index, ipAddr);
                        }

                        memset(ipAddr, 0, sizeof(ipAddr));
                        inet_ntop(pHostent->h_addrtype, pHostent->h_addr, ipAddr, sizeof(ipAddr));
                        //UnionAuditLog("in UnionGetHostByName:: selected first ipAddr [%s]!\n", ipAddr);
                        strncpy(ip, ipAddr, 12 + 3);
                        break;

                default:
                        UnionUserErrLog("in UnionGetHostByName:: not support addrType = [%d](only support AF_INET)!\n", pHostent->h_addrtype);
                        return(errCodeParameter);
                        break;
        }

        return(0);
}

int UnionCloseSocket(int handle)
{
	return(close(handle));
}

int UnionCreateSocketClient(char *ip,int port)
{
	struct				sockaddr_in psckadd;
	int				sckcli;
	int				ret = 0;
	struct linger			Linger;
	int				on=1;
	char				ipAddr[16];

	if (ip== NULL)
	{
		UnionUserErrLog("in UnionCreateSocketClient:: NULL IPAddress!\n");
		return(errCodeParameter);
	}

	// 支持域名
	// add by leipp 20160114
	memset(ipAddr,0,sizeof(ipAddr));
	// end

	if (UnionIsValidIPAddrStr(ip))
	{
		strcpy(ipAddr, ip);
	}
	else
	{
		if ((ret = UnionGetHostByName(ip,ipAddr)) < 0)
		{
                        UnionUserErrLog("in UnionCreateSocketClient:: UnionGetHostByName[%s]!\n", ip);
                        return(ret);
		}
	}
	
	
	if (port < 0)
	{

		UnionUserErrLog("in UnionCreateSocketClient:: Minus Socket Port [%d]!\n",port);
		return(errCodeParameter);
	}


	memset((char *)(&psckadd),'0',sizeof(struct sockaddr_in));
	psckadd.sin_family            = AF_INET;
	psckadd.sin_addr.s_addr       = inet_addr(ipAddr);
	psckadd.sin_port=htons((u_short)port);

	if ((sckcli = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		UnionSystemErrLog("in UnionCreatSocketClient:: socket! IP = [%s] port = [%d]\n",ipAddr,port);
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
		UnionSystemErrLog("in UnionCreatSocketClient:: connect! IP = [%s] port = [%d]\n",ipAddr,port);
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

// added 2013-06-21
int UnionSetSockSendTimeout(int sckcli, int timeoutSecond)
{
	struct timeval		timeout = {10, 0};

	timeout.tv_sec = timeoutSecond;

	if(setsockopt(sckcli, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval)))
	{
		UnionSystemErrLog("in UnionSetSockSendTimeout:: setsockopt: SO_RCVTIMEO timeout[%d]!\n!", timeoutSecond);
		//close(sckcli);
		return(UnionErrCallingSetsockopt);
        }

	return(0);
}

int UnionSendToSocket(int fd,unsigned char *buf, unsigned int len)
{
	int	rc;
	
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

	/*	
	for (;;)
	{
		if ((rc = send(fd,&buf[RealSend],len - RealSend,0)) != len - RealSend)
		{
			UnionSystemErrLog("in UnionSendToSocket:: send! Real Send [%d] Expected Send = [%d]\n",rc,len - RealSend);
			if (rc < 0)
			{
				return(errCodeUseOSErrCode);
			}
			else
				RealSend += rc;
			if (RealSend > len)
				return(errCodeUseOSErrCode);
			usleep(5);
		}
		else
			return(len);
	}
	*/
	//UnionLog("fd = [%d] buf = [%0x] len = [%d]\n",fd,buf,len);
	if ((rc = send(fd,buf,len,0)) != (int)len)
	{
		if (rc < 0)
		{
			UnionSystemErrLog("in UnionSendToSocket:: send! ret = [%d] errno = [%d]\n",rc,errno);
			return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
		}
		UnionUserErrLog("in UnionSendToSocket:: send = [%d]!\n",rc);
		return(errCodeParameter);
	}
	return(len);
}

/*
功能
	发送数据，数据以2个字节的二进数为长度
输入参数
	sckHDL		socket句柄
	buf		要发送的数据，不包括长度位
	len		数据长度
输出参数
	无
返回值
	>=0		实际发送的数据的长度
	<0		错误代码
*/
int UnionSendToSocketWithTowBytesLen(int fd,unsigned char *buf, unsigned int len)
{
	int		ret;
	unsigned char	tmpBuf[8192+2];
	
	tmpBuf[0] = len / 256;
	tmpBuf[1] = len % 256;
	
	if (len + 2 >= sizeof(tmpBuf))
	{
		UnionUserErrLog("in UnionSendToSocketWithTowBytesLen:: too small buffer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeSmallBuffer));
	}
	memcpy(tmpBuf+2,buf,len);
	if ((ret = send(fd,tmpBuf,len+2,0)) < 0)
	{
		UnionSystemErrLog("in UnionSendToSocketWithTowBytesLen:: send len buf!\n");
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	return(ret);
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
			UnionDebugLog("in UnionReceiveFromSocket:: Connection is Closed!\n");
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

int UnionReceiveFromSocketUntilLen(int fd, unsigned char *buf,unsigned int len)
{
	int	rc;
	unsigned int	RecvLen;
	
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
	
	for (RecvLen = 0;;)
	{
		if ((rc = recv(fd,&buf[RecvLen],len - RecvLen,0)) <= 0)
		{
			if (rc == 0)
			{
				//UnionDebugLog("in UnionReceiveFromSocketUntilLen:: Connection is Closed!\n");
				return(RecvLen);
			}
			if ((errno != 108) && (errno != 104) && (errno != 73) && (errno != 232) && (errno != 0) && (errno != 35) && (errno != 2))
			{
				UnionSystemErrLog("in UnionReceiveFromSocketUntilLen:: recv!\n");
				return(errCodeUseOSErrCode);
			}
			else	
			{
				//UnionDebugLog("in UnionReceiveFromSocketUntilLen:: Connection is Closed!\n");
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

int UnionGetTimeoutOfMngSvr()
{
	int ret;

	if ((ret = UnionReadIntTypeRECVar("timeoutOfRev")) < 3)
		ret = 3;
	return(ret);
}


int UnionReceiveFromSocketUntilLenForHsmSvr(int fd, unsigned char *buf,unsigned int len)
{
	int     rc;
	int     RecvLen;
	struct timeval timeout;

	//hzh add in 2011.9.15 增加超时控制
	timeout.tv_sec = UnionGetTimeoutOfMngSvr();
	timeout.tv_usec = 0;
	//add end

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

	// hzh add in 2011.9.15 增加超时控制
	setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,(const char  * )&timeout,sizeof(timeout));

	for (RecvLen = 0;;)
	{
		if ((rc = recv(fd,&buf[RecvLen],len - RecvLen,0)) <= 0)
		{
			if ((errno != 108) && (errno != 104))
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
		UnionSystemErrLog("in UnionInitializeTCPIPServer:: bind() errno = [%d]!\n", errno);
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
#endif
	{
		ret = errCodeAPIRecvClientReqTimeout;
		UnionUserErrLog("in UnionReceiveDataFromSocketWith2BytesLenWithTimeout:: recv from client time out! ret = [%d]\n",ret);
		goto errorExit;
	}
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
		alarm(0);
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
#endif
		{
			ret = errCodeAPIRecvClientReqTimeout;
			UnionUserErrLog("in UnionReceiveDataFromSocketWith2BytesLenWithTimeout:: recv from client time out! ret = [%d]\n",ret);
			goto errorExit;
		}
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
#endif
	{
		ret = errCodeAPIRecvClientReqTimeout;
		UnionUserErrLog("in UnionReceiveAllDataFromSocketWith2BytesLenWithTimeout:: recv from client time out! ret = [%d]\n",ret);
		goto errorExit;
	}
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
#endif
		{
			ret = errCodeAPIRecvClientReqTimeout;
			UnionUserErrLog("in UnionReceiveAllDataFromSocketWith2BytesLenWithTimeout:: recv from client time out! ret = [%d]\n",ret);
			goto errorExit;
		}
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
		UnionUserErrLog("in UnionCommWithSpecSckHDLInBitsFormatWithoutLen:: lenOfReqStr [%d] larger than expected [%d]!\n",lenOfReqStr,(int)sizeof(tmpBuf));
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

/*
// 2011-1-23,王纯军增加
功能
	从一个连接上接收报文,报文前面采用2个字节的长度，2字节长度是2进制数
输入参数
	fd		连接
	sizeOfBuf	接收数据的缓冲区的大小
	maxWaitTime	最大等待时间
输出参数
	buf		接收数据的缓冲区
返回值
	>=0		接收的数据的长度，不包括二进制的长度域在内
	<0		错误代码
*/int UnionReceiveFromSocketWith2BytesLen(int fd, unsigned char *buf,unsigned int sizeOfBuf,int maxWaitTime)
{
	int	rc;
	unsigned int	cliSetLen;
	unsigned char	recvBuf[8192+2+1];
	int	ret;
	unsigned int	realRecvedNum;
		
	if ((fd < 0) || (buf == NULL))
	{
		UnionUserErrLog("in UnionReceiveFromSocket:: SocketID = [%d] !\n",fd);
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}

	// 如果设置了等待时间
	if (maxWaitTime > 0)
		alarm(maxWaitTime);
		
	if ((rc = recv(fd,recvBuf,sizeof(recvBuf)-1,0)) < 0)
	{
		if ((errno != 108) && (errno != 104) && (errno != 73) && (errno != 232) && (errno != 0) && (errno != 35) && (errno != 2))
		{
			UnionSystemErrLog("in UnionReceiveFromSocket:: recv! rc = [%d]\n",rc);
			ret = UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
			goto exitNow;
		}
		UnionDebugLog("in UnionReceiveFromSocketWith2BytesLen:: Connection is Closed!\n");
		ret = 0;
		goto exitNow;
	}
	if (rc == 0)
	{
		UnionDebugLog("in UnionReceiveFromSocketWith2BytesLen:: Connection is Closed rc = 0!\n");
		ret = rc;
		goto exitNow;
	}
	if (rc < 2)
	{
		UnionUserErrLog("in UnionReceiveFromSocketWith2BytesLen:: rc [%d] = 2!\n",rc);
		ret = UnionSetUserDefinedErrorCode(errCodeAPIClientReqLen);
		goto exitNow;
	}
	cliSetLen = recvBuf[0] * 256 + recvBuf[1];
	realRecvedNum = rc - 2;
	UnionLog("in UnionReceiveFromSocketWith2BytesLen:: clientSetLen [%d] != realRecvLen [%d]!\n",cliSetLen,realRecvedNum);
	
	if (realRecvedNum > cliSetLen)	// 接收到的数据比长度指示位的数据长
	{
		UnionUserErrLog("in UnionReceiveFromSocketWith2BytesLen:: clientSetLen [%d] != realRecvLen [%d]!\n",cliSetLen,realRecvedNum);
		ret = UnionSetUserDefinedErrorCode(errCodeAPIClientReqLen);
		goto exitNow;
	}
	if (cliSetLen > realRecvedNum)	// 还要继续接收数据
	{
		for (;;)
		{
			if ((rc = recv(fd,recvBuf+realRecvedNum+2,sizeof(recvBuf)-1,0)) < 0)
			{
				UnionSystemErrLog("in UnionReceiveFromSocket:: recv! rc = [%d]\n",rc);
				ret = UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
				goto exitNow;
			}
			realRecvedNum += rc;
			if (realRecvedNum < cliSetLen)	// 还没有接收完数据
				continue;
			if (realRecvedNum > cliSetLen)	// 接收到的数据比实际数据长
			{
				UnionUserErrLog("in UnionReceiveFromSocketWith2BytesLen:: clientSetLen [%d] < realRecvLen [%d]!\n",cliSetLen,realRecvedNum);
				ret = UnionSetUserDefinedErrorCode(errCodeAPIClientReqLen);
				goto exitNow;
			}
			break;
		}
	}
	if (cliSetLen >= sizeOfBuf)
	{
		UnionUserErrLog("in UnionReceiveFromSocketWith2BytesLen:: sizeOfBuf [%d] < expected [%d]!\n",sizeOfBuf,cliSetLen);
		ret = UnionSetUserDefinedErrorCode(errCodeSmallBuffer);
		goto exitNow;
	}
	memcpy(buf,recvBuf+2,cliSetLen);
	ret = cliSetLen;
exitNow:
	if (maxWaitTime > 0)
		alarm(0);
	return(ret);
}

//add by yangw 20160622
/***********************************************************
功能:           
        socket设置为非阻塞
参数:           
        fd[in]                  socket句柄
返回值:                 
        无                      
***********************************************************/
void UnionSetSocketNonBlock(int fd)
{                               
        int flags;                      
                                        
        flags = fcntl(fd, F_GETFL);     
        flags |= O_NONBLOCK;    
        fcntl(fd, F_SETFL, flags);
        return;
}               
                
/***********************************************************
功能:                   
        socket设置阻塞  
参数:                   
        fd[in]                  socket句柄
返回值:  
        无
***********************************************************/
void UnionSetSocketBlock(int fd)
{       
        int flags;
                
        flags = fcntl(fd, F_GETFL);
        flags &= ~O_NONBLOCK;
        fcntl(fd, F_SETFL, flags);
        return;
}       

/* 获取本机IP
参数:
        ip[in]                  远程服务器IP
        port[in]                远程服务器端口
        localIP[out]            本地IP
        sizeOfLocalIP[in]
返回值:
        >=0                     报文的实际长度
        <0                      失败，返回错误码
***********************************************************/
int UnionGetLocalIpAddrEx(char *ip,int port,char *localIP,int sizeOfLocalIP)
{
        struct sockaddr_in      psckadd;
        struct sockaddr_in      localadd;
        struct timeval          timeo;
        int                     error;
        fd_set                  writeset;
#if ( defined __hpux )
        int                     len;
#else
        socklen_t               len;
#endif
        int                     sckcli;
        int                     ret = 0;
        char                    ipAddr[64];

#ifdef _WIN32
        WORD wVersionRequested = MAKEWORD(2, 1);
        WSADATA wsaData;
        WSAStartup(wVersionRequested, &wsaData);
#endif

        // 支持域名
	if (!UnionIsValidIPAddrStr(ip))
        {
                if ((ret = UnionGetHostByName(ip,ipAddr)) < 0)
                {
                        UnionUserErrLog("in UnionGetLocalIpAddrEx:: UnionGetHostByName[%s]!\n", ip);
                        return(ret);
                }
                strcpy(ip,ipAddr);
        }

        memset((char *)(&psckadd),'0',sizeof(struct sockaddr_in));
        psckadd.sin_family          = AF_INET;
        psckadd.sin_addr.s_addr       = inet_addr(ip);
        psckadd.sin_port=htons((u_short)port);

        if ((sckcli = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
		return(errCodeSocketMDL_Error);
        }
        // 设置为非阻塞方式
        UnionSetSocketNonBlock(sckcli);

        if (connect(sckcli,(struct sockaddr *)(&psckadd),sizeof(struct sockaddr_in)) != 0)
        {
                if (errno != EINPROGRESS)
                {
                        UnionLog("in UnionGetLocalIpAddrEx:: connect! IP = [%s] port = [%d] errno = [%d]\n",ip,port,errno);
                        close(sckcli);
			return(errCodeSocketMDL_Error);
                }

                // 连接超时
                timeo.tv_sec  = SOCKET_CONNECT_TIMEOUT / 1000;
                timeo.tv_usec = SOCKET_CONNECT_TIMEOUT % 1000 * 1000;
                FD_ZERO(&writeset);
                FD_SET(sckcli, &writeset);

                ret = select(sckcli+1, NULL, &writeset, NULL, &timeo);
                if (ret == 0) //返回0，代表在描述词状态改变已超过timeout时间
                {
                        UnionLog("in UnionGetLocalIpAddrEx:: connect! IP = [%s] port = [%d] timeout\n",ip,port);
                        close(sckcli);
			return(errCodeSocketMDL_Error);
                }
                else if (ret > 0)
                {
                        if (FD_ISSET(sckcli, &writeset))
                        {
                                len = sizeof(error);
                                getsockopt(sckcli, SOL_SOCKET, SO_ERROR, &error, &len);
                                if (error != 0)
                                {
                                        UnionLog("in UnionGetLocalIpAddrEx:: connect! IP = [%s] port = [%d] errno = [%d]\n",ip,port,errno);
                                        close(sckcli);
					return(errCodeSocketMDL_Error);
                                }
                        }

                }
                else            // 返回-1， 有错误发生，错误原因存在于errno
                {
                        UnionLog("in UnionGetLocalIpAddrEx:: connect! IP = [%s] port = [%d] errno = [%d]\n",ip,port,errno);
                        close(sckcli);
			return(errCodeSocketMDL_Error);
                }
        }

        len = sizeof(localadd);
        if (getsockname(sckcli,(struct sockaddr *)&localadd,&len))
        {
                close(sckcli);
		return(errCodeSocketMDL_Error);
        }
        inet_ntop(AF_INET, &localadd.sin_addr, localIP, sizeOfLocalIP);

        close(sckcli);
        return(0);
}

int UnionGetLocalIpAddr(char *ip,int port,char *localIP,int sizeOfLocalIP)
{
	struct sockaddr_in	psckadd;
	struct sockaddr_in	localadd;
	socklen_t     		len;
	int			sckcli;
	int			ret = 0;
	char			ipAddr[16];

#ifdef _WIN32
	WORD wVersionRequested = MAKEWORD(2, 1);  
	WSADATA wsaData;  
	WSAStartup(wVersionRequested, &wsaData);  
#endif

	// 支持域名
	if (!UnionIsValidIPAddrStr(ip))
	{
		if ((ret = UnionGetHostByName(ip,ipAddr)) < 0)
		{
			UnionUserErrLog("in UnionGetLocalIpAddr:: UnionGetHostByName[%s]!\n", ip);
			return(ret);
		}
		strcpy(ip,ipAddr);
	}

	memset((char *)(&psckadd),'0',sizeof(struct sockaddr_in));
	psckadd.sin_family	    = AF_INET;
	psckadd.sin_addr.s_addr       = inet_addr(ip);
	psckadd.sin_port=htons((u_short)port);

	if ((sckcli = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		return(errCodeSocketMDL_Error);
	}
	
	if (connect(sckcli,(struct sockaddr *)(&psckadd),sizeof(struct sockaddr_in)) < 0)
	{
		close(sckcli);
		return(errCodeSocketMDL_Error);
	}

	len = sizeof(localadd);	
	if (getsockname(sckcli,(struct sockaddr *)&localadd,&len))
	{
		close(sckcli);
		return(errCodeSocketMDL_Error);
	}
	inet_ntop(AF_INET, &localadd.sin_addr, localIP, sizeOfLocalIP);

	close(sckcli);
	return(0);
}

// with timeout add 20150306 
int UnionCreateSocketClientWithTimeout(char *initIp,int port,int timeout)
{
	struct				sockaddr_in psckadd;
	int				sckcli;
	int				ret;
	struct linger			Linger;
	int				on=1;
	struct timeval			timeo;
	char				ipAddr[32+1];
	char				*ip;	
	
	//ipAddr[0] = 0;
	ip = initIp;	

	// 支持域名
        if (!UnionIsValidIPAddrStr(initIp))
        {
		ip = ipAddr; 
                if ((ret = UnionGetHostByName(initIp,ip)) < 0)
                {
                        UnionUserErrLog("in UnionCreateSocketClient:: UnionGetHostByName[%s]!\n", ip);
                        return(ret);
                }
		UnionLog("int UnionCreateSocketClient:: ip = %s\n", ip);
        }

	if (ip == NULL)
	{
		UnionUserErrLog("in UnionCreateSocketClientWithTimeout:: NULL IPAddress!\n");
		return(errCodeParameter);
	}
	
	if (port < 0)
	{

		UnionUserErrLog("in UnionCreateSocketClientWithTimeout:: Minus Socket Port [%d]!\n",port);
		return(errCodeParameter);
	}
		
	memset((char *)(&psckadd),'0',sizeof(struct sockaddr_in));
	psckadd.sin_family	    = AF_INET;
	psckadd.sin_addr.s_addr       = inet_addr(ip);
	psckadd.sin_port=htons((u_short)port);

	if ((sckcli = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		UnionSystemErrLog("in UnionCreateSocketClientWithTimeout:: socket! IP = [%s] port = [%d]\n",ip,port);
		return(errCodeUseOSErrCode);
	}
	
	// 设置发送超时
	if (timeout > 0)
	{
		timeo.tv_sec = timeout;
		timeo.tv_usec = 0;
		if (setsockopt(sckcli, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(struct timeval)) < 0)
		{
			UnionSystemErrLog("in UnionCreateSocketClientWithTimeout:: setsockopt SO_SNDTIMEO errno=[%d]!",errno);
			close(sckcli);
			return(errCodeUseOSErrCode);
		}
	}
	
	if (connect(sckcli,(struct sockaddr *)(&psckadd),sizeof(struct sockaddr_in)) < 0)
	{
		UnionSystemErrLog("in UnionCreateSocketClientWithTimeout:: connect! IP = [%s] port = [%d]\n",ip,port);
		close(sckcli);
		return(errCodeUseOSErrCode);
	}

	// 设置接收超时
	if (timeout > 0)
	{
		timeo.tv_sec = timeout;
		timeo.tv_usec = 0;
		if (setsockopt(sckcli, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(struct timeval)) < 0)
		{
			UnionSystemErrLog("in UnionCreateSocketClientWithTimeout:: setsockopt: SO_RCVTIMEO errno[%d]!\n!", errno);
			return(errCodeUseOSErrCode);
		}
	}

	Linger.l_onoff = 0;
	Linger.l_linger = 0;
	if (setsockopt(sckcli,SOL_SOCKET,SO_LINGER,(char *)&Linger,sizeof(Linger)) != 0)
	{
		UnionSystemErrLog("in UnionCreateSocketClientWithTimeout:: setsockopt linger!");
		close(sckcli);
		return(errCodeUseOSErrCode);
	}

	if (setsockopt(sckcli, SOL_SOCKET, SO_OOBINLINE, (char *)&on, sizeof(on)))
	{
		UnionSystemErrLog("in UnionCreateSocketClientWithTimeout:: setsockopt SO_OOBINLINE!\n");
		close(sckcli);
		return(errCodeUseOSErrCode);
	}
 
	on = 1;
	if (setsockopt(sckcli, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on)))
	{
		UnionSystemErrLog("in UnionCreateSocketClientWithTimeout:: setsockopt: TCP_NODELAY");
		close(sckcli);
		return(errCodeUseOSErrCode);
	}

	return(sckcli);
}
