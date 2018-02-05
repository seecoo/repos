// Author:	Wolfgang Wang
// Date:	2001/8/30

// Modification History
// 2002/3/6, Add a declaration of function UnionReceiveFromSocketUntilLen

// 2002/2/18, 将UnionTCPIPServer升级了。2.x以前版本，带两个参数，2.x版本带三个参数，第三个参数为一个函数指针。
// 要使用2.x以后的版本，包含本头文件程序，必须定义宏：_UnionSocket_2_x_

#ifndef _UnionSocket
#define _UnionSocket
  
#define SOCKET_CONNECT_TIMEOUT          50

int UnionGetLocalIpAddr(char *ip,int port,char *localIP,int sizeOfLocalIP);
int UnionGetLocalIpAddrEx(char *ip,int port,char *localIP,int sizeOfLocalIP);
int UnionGetHostByName(char *domain, char *ip);
int UnionCloseSocket(int SocketID);
int UnionCreateSocketClient(char *ip,int port);
int UnionSendToSocket(int SocketID,unsigned char *Buf,unsigned int SizeOfBuf);
int UnionReceiveFromSocket(int SocketID,unsigned char *Buf,unsigned int LenOfBuf);
int UnionReceiveFromSocketUntilLen(int SocketID,unsigned char *Buf,unsigned int LenOfBuf); // Added By Wolfgang Wang, 2002/3/6

int UnionNewTCPIPConnection(int scksvr);
int UnionInitializeTCPIPServer(int port);

#ifdef _UnionSocket_3_x_
int UnionTCPIPServer(int port,char *ServerName,int (*UnionTCPIPTaskServer)(),int (*UnionTaskActionBeforeExit)());
#else
#ifdef _UnionSocket_2_x_
int UnionTCPIPServer(int port,char *ServerName,int (*UnionTCPIPTaskServer)());
#else
int UnionTCPIPServer(int port,char *ServerName);
#endif
#endif

#define UnionErrOffsetOfSocketMdl			-10500
#define UnionErrNullIPAddressPassed			UnionErrOffsetOfSocketMdl - 1
#define	UnionErrMinusSocketPortPassed			UnionErrOffsetOfSocketMdl - 2
#define UnionErrCallingSocket				UnionErrOffsetOfSocketMdl - 3
#define UnionErrCallingConnect				UnionErrOffsetOfSocketMdl - 4
#define UnionErrCallingSetsockopt			UnionErrOffsetOfSocketMdl - 5
#define UnionErrCallingBind				UnionErrOffsetOfSocketMdl - 6
#define UnionErrCallingListen				UnionErrOffsetOfSocketMdl - 7
#define UnionErrCallingAccept				UnionErrOffsetOfSocketMdl - 8
#define UnionErrCallingMalloc				UnionErrOffsetOfSocketMdl - 9
#define UnionErrCallingSend				UnionErrOffsetOfSocketMdl - 10
#define UnionErrCallingRecv				UnionErrOffsetOfSocketMdl - 11

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
int UnionReceiveDataFromSocketWith2BytesLenWithTimeout(int sckHDL,char *buf,int sizeOfBuf,int timeout);

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
int UnionSendToSocketWithTowBytesLen(int fd,unsigned char *buf, unsigned int len);

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
int UnionReceiveAllDataFromSocketWith2BytesLenWithTimeout(int sckHDL,unsigned char *buf,int sizeOfBuf,int timeout);

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
int UnionCommWithSpecSvrWith2BytsLen(char *ipAddr,int port,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfBuf,int timeout);

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
int UnionCommWithSpecSckHDLWith2BytsLen(int sckHDL,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfBuf,int timeout);

/*
功能
	在指定的连接上交换一个报文
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
int UnionCommWithSpecSckHDLWithoutLen(int sckHDL,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfBuf);

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
int UnionCommWithSpecSckHDLInBitsFormatWithoutLen(int sckHDL,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfBuf);

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
int UnionCommWithSpecSvrWithoutLen(char *ipAddr,int port,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfBuf);

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
int UnionCommWithSpecSvrInBitsFormatWithoutLen(char *ipAddr,int port,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfBuf);

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
*/
int UnionReceiveFromSocketWith2BytesLen(int fd, unsigned char *buf,unsigned int sizeOfBuf,int maxWaitTime);

int UnionSetSockSendTimeout(int sckcli, int timeoutSecond);

#endif
