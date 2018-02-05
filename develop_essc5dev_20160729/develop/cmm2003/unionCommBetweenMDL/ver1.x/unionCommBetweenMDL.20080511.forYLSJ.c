// Wolfgang Wang
// 2005-08-18

// 在20051214基础上升级，2007/1/26

/*
	在程序中增加了编译开关选项_LINUX_，用于控制在linux下，调用sigsetjmp，siglongjmp，而在其它操作系统下，仍调用setjmp/longjmp
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>

jmp_buf	gunionCommBetweenMDLsTimeoutJmpEnv;
void 	UnionCommTimeoutBetweenMDLs();

#include "UnionLog.h"
#include "unionMDLID.h"
#include "unionCommBetweenMDL.h"
#include "unionREC.h"
#include "unionMsgBuf6.x.h"
#include "UnionLog.h"

int UnionConnectCommBetweenMDLs()
{
	return(UnionConnectMsgBufMDL());
}

int UnionDisconnectCommBetweenMDLs()
{
	return(UnionDisconnectMsgBufMDL());
}

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
int UnionSendRequestToSpecifiedModuleWithNewMsgHeader(TUnionModuleID recvMDLID,unsigned char *msg,int lenOfMsg,PUnionMessageHeader pmsgHeader)
{
	unsigned char		buf[8192+256];
	PUnionHeaderOfMDLComm	pheader;
	int			ret;
		
	if (sizeof(*pheader) + lenOfMsg >= sizeof(buf))
	{
		UnionUserErrLog("in UnionSendRequestToSpecifiedModuleWithNewMsgHeader:: lenOfMsg = [%d] too long!\n",lenOfMsg);
		return(errCodeParameter);
	}
	pheader = (PUnionHeaderOfMDLComm)buf;
	pheader->requesterMDLID = UnionGenerateMyDynamicMDLID();
	pheader->dealerMDLID = recvMDLID;
	memcpy(buf+sizeof(*pheader),msg,lenOfMsg);
#if ( defined __linux__ ) || ( defined __hpux )
	if (sigsetjmp(gunionCommBetweenMDLsTimeoutJmpEnv,1) != 0)
#elif ( defined _AIX )
	if (setjmp(gunionCommBetweenMDLsTimeoutJmpEnv) != 0)
#endif
	{
		msg[lenOfMsg] = 0;
		UnionUserErrLog("in UnionSendRequestToSpecifiedModuleWithNewMsgHeader:: timeout! send to mdl [%ld]\nmessage=[%04d][%s]\n",recvMDLID,lenOfMsg,msg);
		alarm(0);
		return(errCodeEsscMDL_SendRequestTimeout);
	}
	alarm(1);
	signal(SIGALRM,UnionCommTimeoutBetweenMDLs);
	if ((ret = UnionBufferMessageWithNewHeader(buf,lenOfMsg+sizeof(*pheader),recvMDLID,pmsgHeader)) < 0)
	{
		msg[lenOfMsg] = 0;
		UnionUserErrLog("in UnionSendRequestToSpecifiedModuleWithNewMsgHeader:: UnionBufferMessageWithNewHeader!\nmessage=[%04d][%s]\n",lenOfMsg,msg);
		alarm(0);
		return(errCodeParameter);
	}
	alarm(0);
	return(ret);
}

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
int UnionSendRequestToSpecifiedModule(TUnionModuleID recvMDLID,unsigned char *msg,int lenOfMsg)
{
	return(UnionSendRequestToSpecifiedModuleWithNewMsgHeader(recvMDLID,msg,lenOfMsg,NULL));
}

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
int UnionSendResponseToApplyModuleWithOriMsgHeader(TUnionModuleID recvMDLID,unsigned char *msg,int lenOfMsg,PUnionMessageHeader poriMsgHeader)
{
	int			pid;
	unsigned char		buf[8192+256];
	PUnionHeaderOfMDLComm	pheader;
	int			ret;
	
	if ((pid = UnionGetPIDOutOfMDLID(recvMDLID)) <= 0)
	{
		UnionUserErrLog("in UnionSendResponseToApplyModuleWithOriMsgHeader:: [%ld] invalid\n",recvMDLID);
		return(errCodeEsscMDL_InvalidMDLID);
	}
/* 银联数据，消息队列API
	if (kill(pid,0) != 0)
	{
		UnionUserErrLog("in UnionSendResponseToApplyModuleWithOriMsgHeader:: the process [%d] of mdl [%ld] not exists!\n",pid,recvMDLID);
		return(errCodeEsscMDL_ProcNotExistsAnymore);
	}
*/

	pheader = (PUnionHeaderOfMDLComm)buf;
	pheader->requesterMDLID = recvMDLID;
	pheader->dealerMDLID = UnionGetMyFixedMDLID();
	if (sizeof(*pheader) + lenOfMsg >= sizeof(buf))
	{
		UnionUserErrLog("in UnionSendResponseToApplyModuleWithOriMsgHeader:: lenOfMsg = [%d] too long!\n",lenOfMsg);
		return(errCodeParameter);
	}
	memcpy(buf+sizeof(*pheader),msg,lenOfMsg);
#if ( defined __linux__ ) || ( defined __hpux )
	if (sigsetjmp(gunionCommBetweenMDLsTimeoutJmpEnv,1) != 0)
#elif ( defined _AIX )
	if (setjmp(gunionCommBetweenMDLsTimeoutJmpEnv) != 0)
#endif
	{
		msg[lenOfMsg] = 0;
		UnionUserErrLog("in UnionSendResponseToApplyModuleWithOriMsgHeader:: timeout! send to mdl [%ld]\nmessage=[%04d][%s]\n",recvMDLID,lenOfMsg,msg);
		alarm(0);
		return(errCodeEsscMDL_SendResponseTimeout);
	}
	alarm(1);
	signal(SIGALRM,UnionCommTimeoutBetweenMDLs);
	if ((ret = UnionBufferMessageWithOriginHeader(buf,lenOfMsg+sizeof(*pheader),recvMDLID,poriMsgHeader)) < 0)
	{
		msg[lenOfMsg] = 0;
		UnionUserErrLog("in UnionSendResponseToApplyModuleWithOriMsgHeader:: UnionBufferMessageWithOriginHeader\nmessage=[%04d][%s]\n",lenOfMsg,msg);
		alarm(0);
		return(ret);
	}
	alarm(0);
	return(ret);
}

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
int UnionSendResponseToApplyModule(TUnionModuleID recvMDLID,unsigned char *msg,int lenOfMsg)
{
	return(UnionSendResponseToApplyModuleWithOriMsgHeader(recvMDLID,msg,lenOfMsg,NULL));
}

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
int UnionReadRequestToSpecifiedModuleWithMsgHeader(TUnionModuleID recvMDLID,unsigned char *msg,int sizeOfBuf,PUnionModuleID prequesterMDLID,PUnionMessageHeader pmsgHeader)
{
	unsigned char		buf[8192+256];
	PUnionHeaderOfMDLComm	pheader;
	int			ret;
	int			len;

loop:	
	if ((ret = UnionReadFirstMsgOfTypeUntilSuccessWithHeader(buf,sizeof(buf),recvMDLID,pmsgHeader)) < 0)
	{
		UnionUserErrLog("in UnionReadRequestToSpecifiedModuleWithMsgHeader:: UnionReadFirstMsgOfTypeUntilSuccessWithHeader [%ld]\n",recvMDLID);
		return(ret);
	}
	if ((len = ret - sizeof(*pheader)) > sizeOfBuf)
	{
		UnionUserErrLog("in UnionReadRequestToSpecifiedModuleWithMsgHeader:: sizeOfBuf [%d] too small\n",sizeOfBuf);
		//return(errCodeSmallBuffer);
		goto loop;
	}
	if (len < 0)
	{
		UnionUserErrLog("in UnionReadRequestToSpecifiedModuleWithMsgHeader:: msgLen [%d] too short\n",ret);
		//return(errCodeEsscMDL_MsgForThisModuleTooShort);
		goto loop;
	}
	pheader = (PUnionHeaderOfMDLComm)buf;
	*prequesterMDLID = pheader->requesterMDLID;
	memcpy(msg,buf+sizeof(*pheader),len);

	if (pheader->dealerMDLID != recvMDLID)
	{
		msg[len] = 0;
		UnionUserErrLog("in UnionReadRequestToSpecifiedModule:: dealerMDLID [%ld] != expected [%ld]\nmessage=[%04d][%s]\n",
					 pheader->dealerMDLID,recvMDLID,len,msg);
		goto loop;
	}
	
	return(len);
}

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
int UnionReadRequestToSpecifiedModule(TUnionModuleID recvMDLID,unsigned char *msg,int sizeOfBuf,PUnionModuleID prequesterMDLID)
{
	return(UnionReadRequestToSpecifiedModuleWithMsgHeader(recvMDLID,msg,sizeOfBuf,prequesterMDLID,NULL));
}

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
int UnionReadFirstRequestSendToThisModule(unsigned char *msg,int sizeOfBuf,PUnionModuleID prequesterMDLID)
{
	return(UnionReadRequestToSpecifiedModuleWithMsgHeader(UnionGetMyFixedMDLID(),msg,sizeOfBuf,prequesterMDLID,NULL));
}

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
int UnionReadResponseToThisModuleWithMsgHeader(unsigned char *msg,int sizeOfBuf,int timeout,PUnionMessageHeader pmsgHeader)
{
	unsigned char		buf[8192+256];
	int			ret;
	int			len;
	PUnionHeaderOfMDLComm	pheader;
	TUnionModuleID		recvMDLID;

#if ( defined __linux__ ) || ( defined __hpux )
	if (sigsetjmp(gunionCommBetweenMDLsTimeoutJmpEnv,1) != 0)
#elif ( defined _AIX )
	if (setjmp(gunionCommBetweenMDLsTimeoutJmpEnv) != 0)
#endif
	{
		UnionUserErrLog("in UnionReadResponseToThisModuleWithMsgHeader:: timeout! read [%012ld]\n",UnionGetMyCurrentDynamicMDLID());
		alarm(0);
		return(errCodeEsscMDL_WaitResponse);
	}
	alarm(timeout);
	signal(SIGALRM,UnionCommTimeoutBetweenMDLs);
	if ((ret = UnionReadFirstMsgOfTypeUntilSuccessWithHeader(buf,sizeof(buf),recvMDLID = UnionGetMyCurrentDynamicMDLID(),pmsgHeader)) < 0)
	{
		UnionUserErrLog("in UnionReadResponseToThisModuleWithMsgHeader:: UnionReadFirstMsgOfTypeUntilSuccessWithHeader read msg of MDLID [%ld]\n",UnionGetMyCurrentDynamicMDLID());
		alarm(0);
		return(ret);
	}
	alarm(0);
	//UnionAuditLog("in UnionReadResponseToThisModuleWithMsgHeader:: mdlID = [%012ld]\n",UnionGetMyCurrentDynamicMDLID());
	if ((len = ret - sizeof(TUnionHeaderOfMDLComm)) > sizeOfBuf)
	{
		UnionUserErrLog("in UnionReadResponseToThisModuleWithMsgHeader:: sizeOfBuf [%d] too small\n",sizeOfBuf);
		return(errCodeSmallBuffer);
	}
	if (len < 0)
	{
		UnionUserErrLog("in UnionReadResponseToThisModuleWithMsgHeader:: msgLen [%d] too short\n",ret);
		return(errCodeEsscMDL_MsgForThisModuleTooShort);
	}
	memcpy(msg,buf+sizeof(TUnionHeaderOfMDLComm),len);
	pheader = (PUnionHeaderOfMDLComm)buf;
	if (pheader->requesterMDLID != recvMDLID)
	{
		msg[len] = 0;
		UnionUserErrLog("in UnionReadResponseToThisModuleWithMsgHeader:: requesterMDLID [%ld] != expected [%ld]\nmessage=[%04d][%s]\n",
					 pheader->requesterMDLID,recvMDLID,len,msg);
		return(errCodeEsscMDL_ReqAndResNotIsIndentified);
	}
	return(len);
}

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
int UnionReadResponseToThisModule(unsigned char *msg,int sizeOfBuf,int timeout)
{
	return(UnionReadResponseToThisModuleWithMsgHeader(msg,sizeOfBuf,timeout,NULL));
}

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
int UnionExchangeWithSpecModule(TUnionModuleID recvMDLID,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr,int timeout)
{
	int			ret;

	if ((ret = UnionSendRequestToSpecifiedModule(recvMDLID,reqStr,lenOfReqStr)) < 0)
	{
		UnionUserErrLog("in UnionExchangeWithSpecModule:: UnionSendRequestToSpecifiedModule!\n");
		return(ret);
	}
	if ((ret = UnionReadResponseToThisModule(resStr,sizeOfResStr,timeout)) < 0)
	{
		UnionUserErrLog("in UnionExchangeWithSpecModule:: UnionReadResponseToThisModule!\n");
		return(ret);
	}
	return(ret);
}

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
int UnionExchangeWithSpecModuleVerifyReqAndRes(TUnionModuleID recvMDLID,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr,int timeout)
{
	int			ret;
	TUnionMessageHeader	reqMsgHeader;
	TUnionMessageHeader	resMsgHeader;
	time_t			start,finish;

	if ((ret = UnionSendRequestToSpecifiedModuleWithNewMsgHeader(recvMDLID,reqStr,lenOfReqStr,&reqMsgHeader)) < 0)
	{
		UnionUserErrLog("in UnionExchangeWithSpecModuleVerifyReqAndRes:: UnionSendRequestToSpecifiedModuleWithNewMsgHeader!\n");
		return(ret);
	}
	time(&start);
loop:
	if ((ret = UnionReadResponseToThisModuleWithMsgHeader(resStr,sizeOfResStr,timeout,&resMsgHeader)) < 0)
	{
		UnionUserErrLog("in UnionExchangeWithSpecModuleVerifyReqAndRes:: UnionReadResponseToThisModuleWithMsgHeader!\n");
		return(ret);
	}
	if ((reqMsgHeader.time != resMsgHeader.time) || (reqMsgHeader.msgIndex != resMsgHeader.msgIndex) ||
		(reqMsgHeader.provider != resMsgHeader.provider) || (reqMsgHeader.dealer != 0) || 
		(resMsgHeader.dealer == 0))
	{
		UnionUserErrLog("in UnionExchangeWithSpecModuleVerifyReqAndRes::\nReq=[%08ld-%012ld-%012ld-%07d-%07d]\nRes=[%08ld-%012ld-%012ld-%07d-%07d]\n",
						reqMsgHeader.time-start,reqMsgHeader.msgIndex,reqMsgHeader.type,reqMsgHeader.provider,reqMsgHeader.dealer,
						resMsgHeader.time-start,resMsgHeader.msgIndex,resMsgHeader.type,resMsgHeader.provider,resMsgHeader.dealer);
		time(&finish);
		if (finish - start > timeout)
			return(errCodeEsscMDL_ReqAndResNotIsIndentified);
		else
			goto loop;
	}
	return(ret);
}

void UnionCommTimeoutBetweenMDLs()
{
	//signal(SIGALRM,SIG_IGN);
	UnionUserErrLog("in UnionCommTimeoutBetweenMDLs:: time out!\n");
#if ( defined __linux__ ) || ( defined __hpux )
	siglongjmp(gunionCommBetweenMDLsTimeoutJmpEnv,10);
#elif ( defined _AIX )
	longjmp(gunionCommBetweenMDLsTimeoutJmpEnv,10);
#endif
}
