// Author:	Wolfgang Wang
// Date:	2011/1/23

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "UnionStr.h"
#include "unionREC.h"

#include "unionCommWithClient.h"
#include "commManager.h"
#include "UnionLog.h"

#ifdef _client_use_ebcdic_
#include "ebcdicAscii.h"
#endif

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
int UnionRecvInternalPackFromClient(int sckHDL,char *buf,int sizeOfBuf,int timeout)
{
	return(UnionRecvPackFromClient(sckHDL,buf,sizeOfBuf,timeout));
}

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
int UnionRecvExternalPackFromClient(int sckHDL,char *buf,int sizeOfBuf,int timeout)
{
	return(UnionRecvPackFromClient(sckHDL,buf,sizeOfBuf,timeout));
}

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
int UnionRecvPackFromClient(int sckHDL,char *buf,int sizeOfBuf,int timeout)
{
	int	ret;
	
	if ((ret = UnionReceiveDataFromSocketWith2BytesLenWithTimeout(sckHDL,buf,sizeOfBuf,timeout)) < 0)
	{
		UnionUserErrLog("in UnionRecvPackFromClient:: UnionReceiveDataFromSocketWith2BytesLenWithTimeout!\n");
		return(ret);
	}
	UnionIncreaseClientPackNum();	// 将客户端包数目加1
#ifdef _client_use_ebcdic_
	// 客户端使用ebcdic码通讯
	UnionEbcdicToAscii(buf,buf,ret);
#endif
#ifndef _noSpierInfo_
	UnionSendRequestInfoToTransSpier(ret,buf);	// 监控这个消息
#else
	UnionNullLog("[%04d][%s]\n",ret,buf);
#endif		
	return(ret);
}

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
int UnionSendInternalPackToClient(int sckHDL,unsigned char *buf, unsigned int len)
{
	return(UnionSendPackToClient(sckHDL,buf,len));
}

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
int UnionSendExternalPackToClient(int sckHDL,unsigned char *buf, unsigned int len)
{
	return(UnionSendPackToClient(sckHDL,buf,len));
}

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
int UnionSendPackToClient(int sckHDL,unsigned char *buf, unsigned int len)
{
	int	ret;
	
#ifndef _noSpierInfo_
	UnionSendResponseInfoToTransSpier(len,buf); // 监控这个消息
#else
	UnionNullLog("[%04d][%s]\n",len,buf);
#endif	
#ifdef _client_use_ebcdic_	// 进行码制转换
	UnionAsciiToEbcdic(buf,buf,len);
#endif
	if ((ret = UnionSendToSocketWithTowBytesLen(sckHDL,buf,len)) < 0)
	{
		UnionUserErrLog("in UnionSendPackToClient:: UnionSendToSocketWithTowBytesLen!\n");
		return(ret);
	}
	UnionIncreaseClientPackNum();	// 将客户端包数目加1
	return(ret);
}
