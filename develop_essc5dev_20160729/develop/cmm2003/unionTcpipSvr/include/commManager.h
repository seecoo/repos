// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2011-1-23

#ifndef _commManager_
#define _commManager_

/*
功能
	获取当前通讯句柄
输入参数
	无
输出参数
	无
返回值
	>=0		当前通讯句柄
	<0		失败，错误码
*/
int UnionGetActiveTCIPSvrSckHDL();

/*
功能
	设置当前通讯句柄
输入参数
	sckHDL		当前通讯句柄
输出参数
	无
返回值
	无
*/
void UnionSetActiveTCPIPSvrSckHDL(int sckHDL);

/*
功能
	将包数目加1
输入参数
	无
输出参数
	无
返回值
	无
*/
void UnionIncreaseClientPackNum();

/*
功能
	登记任务
输入参数
	cli_addr	客户端信息
	port		客户端端口
输出参数
	无
返回值
	>=0		成功
	<0		失败，错误码
*/
int UnionRegisterTcpipConnTask(struct sockaddr_in *cli_addr,int port,int (*funName)());

#endif
