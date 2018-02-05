// Wolfgang Wang
// 2010-8-19

#ifndef _sjl05HsmApiSample_
#define _sjl05HsmApiSample_

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
int ConnectHsmSJL05(char *ipAddr,int port);

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
int CloseHsmSJL05();

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
int ExcuteHsmSJL05Cmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);

#endif

