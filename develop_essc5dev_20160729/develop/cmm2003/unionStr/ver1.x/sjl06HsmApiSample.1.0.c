// Wolfgang Wang
// 2010-8-19

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "UnionLog.h"
#include "UnionSocket.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "sjl05HsmApiSample.h"
#include "unionREC.h"

// 与密码机之间的连接
int gsjl06HsmSckHDL = -1;

/*
功能
	建立与密码机之间的连接
输入参数
	ipAddr		密码机IP地址
	port		密码机端口
输出参数
	无
返回值
	>=0		成功，密码机连接
	<0		出错代码
*/
int ConnectHsmSJL06(char *ipAddr,int port)
{
	if (gsjl06HsmSckHDL >= 0)	// 关闭之间的连接
		UnionCloseSocket(gsjl06HsmSckHDL);
	if ((gsjl06HsmSckHDL = UnionCreateSocketClient(ipAddr,port)) < 0)
	{
		UnionUserErrLog("in ConnectHsmSJL06:: UnionCreateSocketClient [%s][%d]!\n",ipAddr,port);
	}
	return(gsjl06HsmSckHDL);
}

/*
功能
	关闭与密码机之间的连接
输入参数
	无
输出参数
	无
返回值
	>=0		成功，密码机连接
	<0		出错代码
*/
int CloseHsmSJL06()
{
	if (gsjl06HsmSckHDL >= 0)	// 关闭之间的连接
		UnionCloseSocket(gsjl06HsmSckHDL);
	gsjl06HsmSckHDL = -1;
	return(0);
}

/*
功能
	执行密码机指令
输入参数
	reqStr		命令的请求串
	lenOfReqStr	命令的请求串长度
	sizeOfResStr	响应串的大小
输出参数
	resStr		命令的响应串
返回值
	>=0		响应串的长度
	<0		出错代码
*/
int ExcuteHsmSJL06Cmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	if (gsjl06HsmSckHDL >= 0)
		return(UnionCommWithSpecSckHDLWith2BytsLen(gsjl06HsmSckHDL,(unsigned char *)reqStr,lenOfReqStr,
				(unsigned char *)resStr,sizeOfResStr,1));
	else
		return(UnionCommWithSpecSvrWith2BytsLen(UnionReadStringTypeRECVar("ipAddrOfHsmModel"),
				UnionReadIntTypeRECVar("portOfHsmModel"),
				(unsigned char *)reqStr,lenOfReqStr,(unsigned char *)resStr,sizeOfResStr,1));
}
