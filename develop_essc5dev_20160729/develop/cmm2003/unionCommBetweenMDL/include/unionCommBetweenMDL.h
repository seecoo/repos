// Wolfgang Wang
// 2005/12/29

#ifndef	_commBetweenMDL_
#define _commBetweenMDL_

#include "unionMDLID.h"
#include "unionErrCode.h"
#include "unionMsgBuf6.x.h"

typedef struct
{
	TUnionModuleID	requesterMDLID;	// 提出请求的模块的ID
	TUnionModuleID	dealerMDLID;	// 处理请求的模块的ID
} TUnionHeaderOfMDLComm;
typedef TUnionHeaderOfMDLComm		*PUnionHeaderOfMDLComm;

int UnionConnectCommBetweenMDLs();

int UnionDisconnectCommBetweenMDLs();

// 将请求发往指定的模块
/* 输入参数：
	recvMDLID，接收模块的ID
	msg,lenOfMsg,分别为消息和消息长度
   输出参数：
	pmsgHeader，是接收发送该消息时生成的消息头的缓冲
	为NULL时，不输出该消息头
   返回值：
   	>=0，正确
   	负值，错误码
*/
int UnionSendRequestToSpecifiedModuleWithNewMsgHeader(TUnionModuleID recvMDLID,unsigned char *msg,int lenOfMsg,PUnionMessageHeader pmsgHeader);

// 将请求发往指定的模块
/* 输入参数：
	recvMDLID，接收模块的ID
	msg,lenOfMsg,分别为消息和消息长度
   输出参数：
	无
   返回值：
   	>=0，正确
   	负值，错误码
*/
int UnionSendRequestToSpecifiedModule(TUnionModuleID recvMDLID,unsigned char *msg,int lenOfMsg);

// 将响应发往指定的模块
/* 输入参数：
	recvMDLID，接收模块的ID
	msg,lenOfMsg,分别为消息和消息长度
	poriMsgHeader,是该响应对应的请求消息头，为NULL时，无对应的请求消息头
   输出参数：
   	无
   返回值：
   	>=0，正确
   	负值，错误码
*/
int UnionSendResponseToApplyModuleWithOriMsgHeader(TUnionModuleID recvMDLID,unsigned char *msg,int lenOfMsg,PUnionMessageHeader poriMsgHeader);

// 将响应发往指定的模块
/* 输入参数：
	recvMDLID，接收模块的ID
	msg,lenOfMsg,分别为消息和消息长度
   输出参数：
   	无
   返回值：
   	>=0，正确
   	负值，错误码
*/
int UnionSendResponseToApplyModule(TUnionModuleID recvMDLID,unsigned char *msg,int lenOfMsg);

// 读取发往指定模块的请求
/* 输入参数：
	recvMDLID，接收模块的ID
	sizeOfBuf,是接收缓冲的大小
   输出参数：
	msg,为接收到的消息
	prequesterMDLID,是提交请求的模块ID
	pmsgHeader,是请求消息的消息头，如果初始初为NULL，则不读该消息头
   返回值：
   	>=0，消息的长度
   	负值，错误码
*/
int UnionReadRequestToSpecifiedModuleWithMsgHeader(TUnionModuleID recvMDLID,unsigned char *msg,int sizeOfBuf,PUnionModuleID prequesterMDLID,PUnionMessageHeader pmsgHeader);

// 读取发往指定模块的请求
/* 输入参数：
	recvMDLID，接收模块的ID
	sizeOfBuf,是接收缓冲的大小
   输出参数：
	msg,为接收到的消息
	prequesterMDLID,是提交请求的模块ID
   返回值：
   	>=0，消息的长度
   	负值，错误码
*/
int UnionReadRequestToSpecifiedModule(TUnionModuleID recvMDLID,unsigned char *msg,int sizeOfBuf,PUnionModuleID prequesterMDLID);

// 读取发往指定模块的请求
/* 输入参数：
	无
	sizeOfBuf,是接收缓冲的大小
   输出参数：
	msg,为接收到的消息
	prequesterMDLID,是提交请求的模块ID
   返回值：
   	>=0，消息的长度
   	负值，错误码
*/
int UnionReadFirstRequestSendToThisModule(unsigned char *msg,int sizeOfBuf,PUnionModuleID prequesterMDLID);

// 读取发往本模块的响应
/* 输入参数：
	timeout,是读响应的超时值
	sizeOfBuf,是接收缓冲的大小
   输出参数：
	msg,为接收到的消息
	pmsgHeader,是请求消息的消息头，如果初始初为NULL，则不读该消息头
   返回值：
   	>=0，消息的长度
   	负值，错误码
*/
int UnionReadResponseToThisModuleWithMsgHeader(unsigned char *msg,int sizeOfBuf,int timeout,PUnionMessageHeader pmsgHeader);

// 读取发往本模块的响应
/* 输入参数：
	timeout,是读响应的超时值
	sizeOfBuf,是接收缓冲的大小
   输出参数：
	msg,为接收到的消息
   返回值：
   	>=0，消息的长度
   	负值，错误码
*/
int UnionReadResponseToThisModule(unsigned char *msg,int sizeOfBuf,int timeout);

// 与指定的模块进行交互
/* 输入参数：
	recvMDLID,指定的模块
	reqStr,lenOfReqStr,分别是请求和请求长度
	sizeOfBuf,是接收缓冲的大小
	timeout,是读响应的超时值
   输出参数：
	resStr,为接收到的消息
   返回值：
   	>=0，消息的长度
   	负值，错误码
*/
int UnionExchangeWithSpecModule(TUnionModuleID recvMDLID,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr,int timeout);

// 与指定的模块进行交互
/* 输入参数：
	recvMDLID,指定的模块
	reqStr,lenOfReqStr,分别是请求和请求长度
	sizeOfBuf,是接收缓冲的大小
	timeout,是读响应的超时值
   输出参数：
	resStr,为接收到的消息
   返回值：
   	>=0，消息的长度
   	负值，错误码
*/
int UnionExchangeWithSpecModuleVerifyReqAndRes(TUnionModuleID recvMDLID,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr,int timeout);

int UnionExchangeWithSpecModuleVerifyReqAndResCompress(TUnionModuleID recvMDLID,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr,int timeout);

int UnionSendRequestToSpecifiedModuleWithOriMsgHeaderWithTime(TUnionModuleID recvMDLID,int oriMDLID, char *msg,int lenOfMsg,PUnionMessageHeader poriMsgHeader,int timeout);

//add by hzh in 2011.8.18i
int UnionSendRequestToSpecifiedModuleWithNewMsgHeaderWithTime(TUnionModuleID recvMDLID,unsigned char *msg,int lenOfMsg,PUnionMessageHeader pmsgHeader,int timeout);

#endif
