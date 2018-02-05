// author	Wolfgang Wang
// date		2010-10-25

#ifndef _unionCommWithClient_
#define _unionCommWithClient_

/*
功能
	从客户端，接收数据，数据以2个字节的二进数为长度
	接收到的数据，不包括长度位
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
int UnionRecvInternalPackFromClient(int sckHDL,char *buf,int sizeOfBuf,int timeout);

/*
功能
	从客户端，接收数据，数据以2个字节的二进数为长度
	接收到的数据，不包括长度位
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
int UnionRecvExternalPackFromClient(int sckHDL,char *buf,int sizeOfBuf,int timeout);

/*
功能
	从客户端，接收数据，数据以2个字节的二进数为长度
	接收到的数据，不包括长度位
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
int UnionRecvPackFromClient(int sckHDL,char *buf,int sizeOfBuf,int timeout);

/*
功能
	向客户端发送数据，数据以2个字节的二进数为长度
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
int UnionSendInternalPackToClient(int sckHDL,unsigned char *buf, unsigned int len);

/*
功能
	向客户端发送数据，数据以2个字节的二进数为长度
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
int UnionSendExternalPackToClient(int sckHDL,unsigned char *buf, unsigned int len);

/*
功能
	向客户端发送数据，数据以2个字节的二进数为长度
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
int UnionSendPackToClient(int sckHDL,unsigned char *buf, unsigned int len);

#endif
