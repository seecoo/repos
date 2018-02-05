// Wolfgang Wang
// 2010-8-19

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionExcuteExternalCmdFile.h"

#include "sjl05HsmApiSample.h"

/*
功能
	执行一个外部命令-建立与密码机之间的连接
输入参数
	reqStr		外部命令的请求串
	lenOfReqStr	外部命令的请求串长度
	sizeOfResStr	响应串的大小
输出参数
	resStr		命令的响应串
返回值
	>=0		响应串的长度
	<0		出错代码
*/
int UnionExcuteFun_ConnectHsmSJL05(char *reqStr,int lenOfReqStr)
{
	char	ipAddr[40+1];
	int	port = -1;
	int	ret;
	
	// 读取加密机IP地址
	memset(ipAddr,0,sizeof(ipAddr));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"hsmIPAddr",ipAddr,sizeof(ipAddr))) < 0)
	{
		UnionUserErrLog("in UnionExcuteFun_ConnectHsmSJL05:: UnionReadRecFldFromRecStr [%s] from [%04d][%s]\n","hsmIPAddr",lenOfReqStr,reqStr);
		return(ret);
	}
	// 读取加密机端口
	if ((ret = UnionReadIntTypeRecFldFromRecStr(reqStr,lenOfReqStr,"hsmPort",&port)) < 0)
	{
		UnionUserErrLog("in UnionExcuteFun_ConnectHsmSJL05:: UnionReadRecFldFromRecStr [%s] from [%04d][%s]\n","hsmPort",lenOfReqStr,reqStr);
		return(ret);
	}
	// 调用与加密机建连的函数
	if ((ret = ConnectHsmSJL05(ipAddr,port)) < 0)
	{
		UnionUserErrLog("in UnionExcuteFun_ConnectHsmSJL05:: ConnectHsmSJL05 [%s] [%d]!\n",ipAddr,port);
		return(ret);
	}
	return(0);
}


/*
功能
	执行一个外部命令-断开与密码机之间的连接
输入参数
	reqStr		外部命令的请求串
	lenOfReqStr	外部命令的请求串长度
	sizeOfResStr	响应串的大小
输出参数
	resStr		命令的响应串
返回值
	>=0		响应串的长度
	<0		出错代码
*/
int UnionExcuteFun_CloseHsmSJL05()
{
	int	ret;
	
	// 调用与加密机断连的函数
	if ((ret = CloseHsmSJL05()) < 0)
	{
		UnionUserErrLog("in UnionExcuteFun_CloseHsmSJL05:: CloseHsmSJL05!\n");
		return(ret);
	}
	return(0);
}

/*
功能
	执行一个外部命令-执行密码机指令
输入参数
	reqStr		外部命令的请求串
	lenOfReqStr	外部命令的请求串长度
	sizeOfResStr	响应串的大小
输出参数
	resStr		命令的响应串
返回值
	>=0		响应串的长度
	<0		出错代码
*/
int UnionExcuteFun_ExcuteHsmSJL05Cmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	char	hsmCmd[4096+1];
	int	ret;
	int	lenOfHsmCmdReqStr;
	
	// 读取加密机指令请求
	memset(hsmCmd,0,sizeof(hsmCmd));
	if ((lenOfHsmCmdReqStr = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"hsmCmdReqStr",hsmCmd,sizeof(hsmCmd))) < 0)
	{
		UnionUserErrLog("in UnionExcuteFun_ExcuteHsmSJL05Cmd:: UnionReadRecFldFromRecStr [%s] from [%04d][%s]\n","hsmCmdReqStr",lenOfReqStr,reqStr);
		return(lenOfHsmCmdReqStr);
	}
	// 调用执行加密机指令的函数
	if ((ret = ExcuteHsmSJL05Cmd(hsmCmd,lenOfHsmCmdReqStr,hsmCmd,sizeof(hsmCmd))) < 0)
	{
		UnionUserErrLog("in UnionExcuteFun_ExcuteHsmSJL05Cmd:: ExcuteHsmSJL05Cmd [%04d][%s]!\n",lenOfHsmCmdReqStr,hsmCmd);
		return(ret);
	}
	hsmCmd[ret] = 0;
	if ((ret = UnionPutRecFldIntoRecStr("hsmCmdResStr",hsmCmd,ret,resStr,sizeOfResStr)) < 0)
	{
		UnionUserErrLog("in UnionExcuteFun_ExcuteHsmSJL05Cmd:: UnionPutRecFldIntoRecStr [hsmCmdResStr]!\n");
		return(ret);
	}		
	return(ret);
}

/*
功能
	执行一个外部命令
输入参数
	reqStr		外部命令的请求串
	lenOfReqStr	外部命令的请求串长度
	sizeOfResStr	响应串的大小
输出参数
	resStr		命令的响应串
返回值
	>=0		响应串的长度
	<0		出错代码
*/
int UnionExcuteExternalCmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	char	funName[128+1];
	int	ret;
	
	// 读取函数名称
	memset(funName,0,sizeof(funName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"funName",funName,sizeof(funName))) < 0)
	{
		UnionUserErrLog("in UnionExcuteExternalCmd:: UnionReadRecFldFromRecStr [%s] from [%04d][%s]\n","funName",lenOfReqStr,reqStr);
		return(ret);
	}
	
	if (strcmp(funName,"ConnectHsmSJL05") == 0)		// 建立与加密机的连接
		return(UnionExcuteFun_ConnectHsmSJL05(reqStr,lenOfReqStr));
	else if (strcmp(funName,"CloseHsmSJL05") == 0)	// 关闭与加密机的连接
		return(UnionExcuteFun_CloseHsmSJL05());
	else if (strcmp(funName,"ExcuteHsmSJL05Cmd") == 0)	// 执行加密机指令
		return(UnionExcuteFun_ExcuteHsmSJL05Cmd(reqStr,lenOfReqStr,resStr,sizeOfResStr));
	/*
	else if (strcmp(funName,"UserInput") == 0)	// 用户输入
		return(UnionExcuteFun_ExcuteUserInput(reqStr,lenOfReqStr,resStr,sizeOfResStr));
	*/
	else
	{
		UnionUserErrLog("in UnionExcuteExternalCmd:: funName = [%s] not supported!\n",funName);
		return(errCodeFunctionNotSupported);
	}
}
