// Wolfgang Wang
// 2010-12-20

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <time.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionMsgBuf6.x.h"
#include "unionErrCode.h"
#include "unionServiceBuffer.h"

#define errCodeServiceBufferTimeout			-11000
#define errCodeServiceBufferNoChanelForSpecService	-12000
#define errCodeServiceBufferRequestTooShort		-13000

// 定义服务缓冲区
TUnionServiceBuffer	gunionServiceBuffer;
PUnionServiceBuffer	pgunionServiceBuffer = &gunionServiceBuffer;
int			gunionServiceBufferConnected = 0;			// 缓冲区是否已连接
int			gunionIsServiceBufferTimeout = 0;			// 标识是否发生了超时事件
time_t			gunionCurrentServiceStartTime = 0;		// 当前服务开始时间
int			gunionServiceBufferTimeout = 3;			// 交换区的超时时间
int			gunionCurrentTokenID = -1;			// 当前的令牌标识
int			gunionServiceReceiverWakeuped = 0;		// 标识是否被唤醒

void UnionServiceBufferDealTimeout();

/*
功能
	设置重连服务区标识
输入参数
	无
输出参数
	无
返回值
	无
*/
void UnionSetReconnectServiceBuffer()
{
	if ((errno == -22) || (errno == -35))
		gunionServiceBufferConnected = 0;
	return;
}		

/*
功能
	将服务缓冲写入到日志文件
输入参数
	服务缓冲指针
输出参数
	无
返回值
	无
*/
void UnionLogServiceBuffer(PUnionServiceBuffer pbuffer)
{
	int			index;
	PUnionServiceChanel	pchanel;
	
	if (pbuffer == NULL)
		return;
	for (index = 0; index < pbuffer->chanelNum; index++)
	{
		pchanel = &(pbuffer->chanelGrp[index]);
#ifdef _useSerivceChanelToken_
		UnionNullLog("serverID=%08ld|tokenQueueID=%08d,%08x|requestQueueID=%08d,%08x|responseQueueID=%08d,%08x|\n",
			pchanel->serviceID,
			pchanel->userTokenQueueID,
			pchanel->sysTokenQueueID,
			pchanel->userRequestQueueID,
			pchanel->sysRequestQueueID,
			pchanel->userResponseQueueID,
			pchanel->sysResponseQueueID);
#else
		UnionNullLog("serverID=%08ld|requestQueueID=%08d,%08x|responseQueueID=%08d,%08x|\n",
			pchanel->serviceID,
			pchanel->userRequestQueueID,
			pchanel->sysRequestQueueID,
			pchanel->userResponseQueueID,
			pchanel->sysResponseQueueID);
#endif
	}
	UnionNullLog("chanelNum=%d\n",pbuffer->chanelNum);
	return;
}

/*
功能
	重置超时
输入参数
	userTimeout	用户指定的超时
输出参数
	无
返回值
	无
*/
void UnionResetServiceBufferLeftTimeout(int userTimeout)
{
	gunionIsServiceBufferTimeout = 0;
	time(&gunionCurrentServiceStartTime);
	if (userTimeout > 0)
		gunionServiceBufferTimeout = userTimeout;
	return;
}

/*
功能
	获取剩余的超时
输入参数
	userTimeout	用户指定的超时
输出参数
	无
返回值
	>=0	成功
	<0	失败
*/
int UnionGetServiceBufferLeftTimeout(int userTimeout)
{
	time_t		now;
	int		timeout;
	
	gunionIsServiceBufferTimeout = 0;
	time(&now);
	if (gunionCurrentServiceStartTime <= 0)
		gunionCurrentServiceStartTime = now;
	if (userTimeout > 0)
		gunionServiceBufferTimeout = userTimeout;
	timeout = gunionServiceBufferTimeout - (now - gunionCurrentServiceStartTime);
	if (timeout <= 0)
	{
		UnionUserErrLog("in UnionGetServiceBufferLeftTimeout:: timeout = [%d]!\n",timeout);
		gunionIsServiceBufferTimeout = 1;
		return(UnionSetUserDefinedErrorCode(errCodeServiceBufferTimeout));
	}
	return(timeout);
}

/*
功能
	设置超时
输入参数
	userTimeout	用户指定的超时
输出参数
	无
返回值
	>=0	成功
	<0	失败
*/
int UnionSetServiceBufferTimeout(int userTimeout)
{
	int		timeout;

	if ((timeout = UnionGetServiceBufferLeftTimeout(userTimeout)) < 0)
	{
		UnionUserErrLog("in UnionSetServiceBufferTimeout:: UnionGetServiceBufferLeftTimeout!\n");
		return(timeout);
	}	
	alarm(timeout);
	signal(SIGALRM,UnionServiceBufferDealTimeout);
	return(timeout);
}

/*
功能
	清除超时设置
输入参数
	无
输出参数
	无
返回值
	>=0	成功
	<0	失败
*/
int UnionClearServiceBufferTimeout()
{
	alarm(0);
	gunionIsServiceBufferTimeout = 0;
	return(0);
}

/*
功能
	连接服务缓冲区,如果已连接,不再重连
输入参数
	无
输出参数
	无
返回值
	>=0	成功,服务通道数量
	<0	出错,错误码
*/
int UnionConnectServiceBuffer()
{
	int	ret;
	
	if (gunionServiceBufferConnected)
		return(pgunionServiceBuffer->chanelNum);
	return(UnionConnectServiceBufferAnyway(pgunionServiceBuffer));
}

/*
功能
	连接服务缓冲区
输入参数
	无
输出参数
	无
返回值
	>=0	成功,服务通道数量
	<0	出错,错误码
*/
int UnionConnectServiceBufferAnyway()
{
	int	ret;
	
	gunionServiceBufferConnected = 0;
	if ((ret = UnionInitServiceBuffer(pgunionServiceBuffer)) < 0)
	{
		UnionUserErrLog("in UnionConnectServiceBufferAnyway:: UnionInitServiceBuffer!\n");
		return(ret);
	}
	gunionServiceBufferConnected = 1;
	UnionLogServiceBuffer(pgunionServiceBuffer);
	return(ret);
}

/*
功能
	断开与服务缓冲区的连接
输入参数
	无
输出参数
	无
返回值
	>=0	成功
	<0	出错,错误码
*/
int UnionDisconnectServiceBuffer()
{
	gunionServiceBufferConnected = 0;
	return(0);
}

/*
功能
	获取指定服务对应的通道
输入参数
	serviceID	服务标识
输出参数
	无
返回值
	成功	服务通道指针
	失败	空指针
*/
PUnionServiceChanel UnionFindChanelOfSpecService(long serviceID)
{
	int			index;
	int			ret;
		
	if ((ret = UnionConnectServiceBuffer()) < 0)
	{
		UnionUserErrLog("in UnionGetTokenOfSpecService:: UnionConnectServiceBuffer!\n");
		return(NULL);
	}
	// 查找指定服务通道
	for (index = 0; index < pgunionServiceBuffer->chanelNum; index++)
	{
		if (serviceID == pgunionServiceBuffer->chanelGrp[index].serviceID)
			return(&(pgunionServiceBuffer->chanelGrp[index]));
	}
	UnionLog("in UnionFindChanelOfSpecService:: no chanel set for service [%ld]!\n",serviceID);
	UnionSetUserDefinedErrorCode(errCodeServiceBufferNoChanelForSpecService);
	return(NULL);
}

#ifdef _useSerivceChanelToken_
/*
功能
	清除发给指定令牌的请求
输入参数
	pchanel		服务通道
	tokenID		服务令牌
输出参数
	无
返回值
	>=0		清除的数量
	<0		失败,返回错误码
*/
int UnionClearRequestToSpecToken(PUnionServiceChanel pchanel,int tokenID)
{
	int			num = 0;
	TUnionServicePack	pack;
	int			ret;
	
	if ((pchanel == NULL) || (tokenID <= 0))
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
		
	pack.receiverID = tokenID;
	for (;;)
	{
		if (msgrcv(pchanel->sysRequestQueueID,&pack,sizeof(pack.text),tokenID,IPC_NOWAIT) < 0)
			return(num);
		num++;
	}	
}
#endif

/*
功能
	清除指定响应标识的响应
输入参数
	pchanel		服务通道
	specID		指定的响应标识
输出参数
	无
返回值
	>=0		清除的数量
	<0		失败,返回错误码
*/
int UnionClearResponseOfSpecID(PUnionServiceChanel pchanel,int specID)
{
	int			num = 0;
	TUnionServicePack	pack;
	int			ret;
	
	if ((pchanel == NULL) || (specID <= 0))
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
		
	pack.receiverID = specID;
	for (;;)
	{
		if (msgrcv(pchanel->sysResponseQueueID,&pack,sizeof(pack.text),specID,IPC_NOWAIT) < 0)
			return(num);
		num++;
	}	
}


#ifdef _useSerivceChanelToken_
/*
功能
	获取提供指定服务的令牌
输入参数
	serviceID	服务标识
输出参数
	pchanel		服务通道
返回值
	>=0	令牌标识
	<0	出错,错误码
*/
int UnionGetTokenOfSpecService(long serviceID,PUnionServiceChanel pchanel)
{
	int			ret;
	TUnionServiceToken	token;
	int			tokenID;
	PUnionServiceChanel	ptmpChanel;
	
	// 查找指定服务通道
	if ((ptmpChanel = UnionFindChanelOfSpecService(serviceID)) == NULL)
	{
		UnionUserErrLog("in UnionGetTokenOfSpecService:: no chanel set for service [%ld]!\n",serviceID);
		return(UnionGetUserDefinedErrorCode());
	}

	memset(&token,0,sizeof(token));
	token.serviceID = serviceID;
	// 获取可用超时
	if ((ret = UnionSetServiceBufferTimeout(-1)) < 0)
	{
		UnionUserErrLog("in UnionGetTokenOfSpecService:: UnionSetServiceBufferTimeout!\n");
		return(ret);
	}
	if ((ret = msgrcv(ptmpChanel->sysTokenQueueID,&token,sizeof(token.tokenID),serviceID,0)) < 0)
	{
		UnionClearServiceBufferTimeout();
		UnionSetReconnectServiceBuffer();
		UnionSystemErrLog("in UnionGetTokenOfSpecService:: msgrcv from [%d][%d]!\n",ptmpChanel->userTokenQueueID,ptmpChanel->sysTokenQueueID);
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	UnionClearServiceBufferTimeout();
	memcpy(&tokenID,token.tokenID,sizeof(tokenID));
	if (pchanel != NULL)
		memcpy(pchanel,ptmpChanel,sizeof(*pchanel));
	return(tokenID);
}
#endif

#ifdef _useSerivceChanelToken_
/*
功能
	置指定服务的令牌可用
输入参数
	serviceID	服务标识
	tokenID		令牌号
输出参数
	无
返回值
	>=0		成功
	<0		失败,错误码
*/
int UnionSetTokenOfSpecServiceAvailable(long serviceID,int tokenID)
{
	return(UnionSetTokenOfSpecChanelAvailable(UnionFindChanelOfSpecService(serviceID),tokenID));
}
#endif

#ifdef _useSerivceChanelToken_
/*
功能
	置指定服务的令牌可用
输入参数
	pchanel		服务通道
	tokenID		令牌号
输出参数
	无
返回值
	>=0		成功
	<0		失败,错误码
*/
int UnionSetTokenOfSpecChanelAvailable(PUnionServiceChanel pchanel,int tokenID)
{
	int			index;
	TUnionServiceToken	token;
	int			ret;
	
	if (pchanel == NULL)
	{
		UnionUserErrLog("in UnionSetTokenOfSpecChanelAvailable:: pchanel is NULL!\n");
		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));
	}
	// 置令牌可用
	token.serviceID = pchanel->serviceID;
	memcpy(token.tokenID,&tokenID,sizeof(tokenID));
	if ((ret = msgsnd(pchanel->sysTokenQueueID,&token,sizeof(int),IPC_NOWAIT)) < 0)
	{
		UnionSystemErrLog("in UnionSetTokenOfSpecChanelAvailable:: msgsnd! [%d][%d]!\n",pchanel->userTokenQueueID,pchanel->sysTokenQueueID);
		UnionSetReconnectServiceBuffer();
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	return(ret);
}
#endif

// 发送一个指定服务的请求
/* 输入参数：
	serviceID	服务标识
	msg		消息
	lenOfMsg	消息长度
   输出参数：
	tokenID		令牌标识
   返回值：
   	>=0，正确
   	负值，错误码
*/
int UnionSendRequestOfSpecService(long serviceID,unsigned char *msg,int lenOfMsg)
{
	TUnionServicePack	pack;
	int			ret;
	PUnionServiceChanel	pchanel;
	int			tokenID;
	
	gunionCurrentTokenID = -1;
	if ((lenOfMsg > sizeof(pack.text)) || (lenOfMsg < 0))
	{
		UnionUserErrLog("in UnionSendRequestOfSpecService:: lenOfMsg [%d] error!\n",lenOfMsg);
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	// 找到服务通道
	if ((pchanel = UnionFindChanelOfSpecService(serviceID)) == NULL)
	{
		UnionUserErrLog("in UnionSendRequestOfSpecService:: UnionFindChanelOfSpecService [%ld]!\n",serviceID);
		return(UnionGetUserDefinedErrorCode());
	}
	// 对报文进行赋值
	memcpy(pack.text,msg,lenOfMsg);
	pack.receiverID = serviceID;
loop:
#ifdef _useSerivceChanelToken_		// 使用令牌	
	// 先获得令牌号
	memset(&chanel,0,sizeof(chanel));
	if ((tokenID = UnionGetTokenOfSpecService(serviceID,&chanel)) < 0)
	{
		UnionUserErrLog("in UnionSendRequestOfSpecService:: UnionGetTokenOfSpecService!\n");
		return(tokenID);
	}
	// 将请求标识置为指定令牌
	pack.receiverID = tokenID;
#endif
	// 将请求置入队列
	if ((ret = msgsnd(pchanel->sysRequestQueueID,&pack,lenOfMsg,IPC_NOWAIT)) < 0)
	{
		UnionSetReconnectServiceBuffer();
		UnionSystemErrLog("in UnionSendRequestOfSpecService:: msgsnd [%d][%d]!\n",pchanel->userRequestQueueID,pchanel->sysRequestQueueID);
#ifdef _useSerivceChanelToken_		// 使用令牌	
		UnionSetTokenOfSpecChanelAvailable(pchanel,tokenID);	// 重置令牌可用
#endif
		return(ret);
	}
#ifdef _useSerivceChanelToken_		// 使用令牌	
	// 通知令牌接收请求
	if ((ret = UnionNoticeToken(tokenID)) < 0)	// 令牌已不存在
	{
		UnionSystemErrLog("in UnionSendRequestOfSpecService:: UnionNoticeToken [%d]!\n",tokenID);
		// 清除刚才的垃圾信息
		UnionClearRequestToSpecToken(pchanel,tokenID);
		goto loop;	// 尝试使用另一个令牌重新发送
	}
	gunionCurrentTokenID = tokenID;
#endif
	return(ret);
}

// 发送一个指定服务的响应
/* 输入参数：
	serviceID	服务标识
	msg		消息
	lenOfMsg	消息长度
	responseID	响应标识
   输出参数：
   	无
   返回值：
   	>=0，正确
   	负值，错误码
*/
int UnionSendResponseOfSpecService(long serviceID,unsigned char *msg,int lenOfMsg,int responseID)
{
	TUnionServicePack	pack;
	int			ret;
	int			tokenID;
	PUnionServiceChanel	pchanel;

	if ((lenOfMsg > sizeof(pack.text)) || (lenOfMsg < 0))
	{
		UnionUserErrLog("in UnionSendResponseOfSpecService:: lenOfMsg [%d] error!\n",lenOfMsg);
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	// 对正文进行赋值
	memcpy(pack.text,msg,lenOfMsg);
	pack.receiverID = responseID;
	// 先找出服务通道
	if ((pchanel = UnionFindChanelOfSpecService(serviceID)) == NULL)
	{
		UnionUserErrLog("in UnionSendResponseOfSpecService:: no chanel set for service [%ld]!\n",serviceID);
		return(UnionGetUserDefinedErrorCode());
	}
loop:	
	// 将响应置入队列
	if ((ret = msgsnd(pchanel->sysResponseQueueID,&pack,lenOfMsg,IPC_NOWAIT)) < 0)
	{
		UnionSetReconnectServiceBuffer();
		UnionSystemErrLog("in UnionSendResponseOfSpecService:: msgsnd [%d][%d]!\n",pchanel->userResponseQueueID,pchanel->sysResponseQueueID);
#ifdef _useSerivceChanelToken_		// 使用令牌	
		UnionSetTokenOfSpecChanelAvailable(pchanel,gunionCurrentTokenID);	// 置当前令牌可用
#endif
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
#ifdef _useSerivceChanelToken_		// 使用令牌	
	// 通知请求进程
	if ((ret = UnionNoticeResponseReceiver(responseID)) < 0)
	{
		UnionSystemErrLog("in UnionSendResponseOfSpecService:: UnionNoticeResponseReceiver [%d]!\n",responseID);
		// 清除响应垃圾信息
		UnionClearResponseOfSpecID(pchanel,responseID);
		UnionSetTokenOfSpecChanelAvailable(pchanel,gunionCurrentTokenID);	// 重置令牌可用
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	// 重置令牌可用
	if ((ret = UnionSetTokenOfSpecChanelAvailable(pchanel,gunionCurrentTokenID)) < 0)
	{
		UnionUserErrLog("in UnionSendResponseOfSpecService:: UnionSetTokenOfSpecChanelAvailable [%ld][%d]!\n",serviceID,gunionCurrentTokenID);
		return(UnionSetUserDefinedErrorCode(ret));
	}
#endif
	return(ret);
}

// 读取指定服务的请求
/* 输入参数：
	serviceID	服务标识
	sizeOfBuf	接收缓冲的大小
   输出参数：
	msg		为接收到的消息
   返回值：
   	>=0，消息的长度
   	负值，错误码
*/
int UnionReadRequestOfSpecService(long serviceID,unsigned char *msg,int sizeOfBuf)
{
	int			ret;
	PUnionServiceChanel	pchanel;
	TUnionServicePack	pack;
	int			len;
	
loop:
	gunionCurrentTokenID = -1;
	// 先查找通道
	if ((pchanel = UnionFindChanelOfSpecService(serviceID)) == NULL)
	{
		UnionUserErrLog("in UnionReadRequestOfSpecService:: no chanel set for service [%ld]!\n",serviceID);
		return(UnionGetUserDefinedErrorCode());
	}

	pack.receiverID = serviceID;
#ifdef _useProcIDAsSerivceChanelToken_		// 使用进程号作为令牌标识	
	// 等待接收到消息通知
	for (;;)
	{
		gunionServiceReceiverWakeuped = 0;
		sleep(60);
		if (gunionServiceReceiverWakeuped)
			break;
	}
	// 读取消息
	pack.receiverID = getpid();
	if ((ret = msgrcv(pchanel->sysRequestQueueID,&pack,sizeof(pack.text),getpid(),IPC_NOWAIT)) < 0)
	{
		UnionSetReconnectServiceBuffer();
		UnionSystemErrLog("in UnionReadRequestOfSpecService:: msgrcv [%d][%0x]!\n",pchanel->userRequestQueueID,pchanel->sysRequestQueueID);
		// 置令牌可用
		UnionSetTokenOfSpecChanelAvailable(pchanel,getpid());
		goto loop;
	}
#else
	if ((ret = msgrcv(pchanel->sysRequestQueueID,&pack,sizeof(pack.text),pack.receiverID,0)) < 0)
	{
		UnionSetReconnectServiceBuffer();
		UnionSystemErrLog("in UnionReadRequestOfSpecService:: msgrcv [%d][%0x]!\n",pchanel->userRequestQueueID,pchanel->sysRequestQueueID);
		goto loop;
	}
#endif
	len = ret;
	// 复制数据
	if (len >= sizeOfBuf)
	{
		UnionUserErrLog("in UnionReadRequestOfSpecService:: sizeOfBuf [%d] too small to receive [%d]!\n",sizeOfBuf,len);
#ifdef _useSerivceChanelToken_		// 使用令牌	
		// 置令牌可用
		UnionSetTokenOfSpecChanelAvailable(pchanel,getpid());
#endif
		goto loop;
	}
	memcpy(msg,pack.text,len);
	return(len);
}

// 读取指定服务的响应
/* 输入参数：
	serviceID	服务标识
	responseID	响应标识
	timeout		是读响应的超时值
	sizeOfBuf	是接收缓冲的大小
   输出参数：
	msg		为接收到的消息
   返回值：
   	>=0，消息的长度
   	负值，错误码
*/
int UnionReadResponseOfSpecService(long serviceID,unsigned char *msg,int sizeOfBuf,long responseID,int timeout)
{
	int			ret;
	PUnionServiceChanel	pchanel;
	TUnionServicePack	pack;
	int			len;
	int			leftTimeout;
	
	// 先查找通道
	if ((pchanel = UnionFindChanelOfSpecService(serviceID)) == NULL)
	{
		UnionUserErrLog("in UnionReadResponseOfSpecService:: no chanel set for service [%ld]!\n",serviceID);
		return(UnionGetUserDefinedErrorCode());
	}
loop:
#ifdef _useProcIDAsSerivceChanelToken_		// 使用进程号作为令牌标识	
	// 等待接收到消息通知
	if ((leftTimeout = UnionGetServiceBufferLeftTimeout(timeout)) < 0)
	{
		UnionUserErrLog("in UnionReadResponseOfSpecService:: UnionGetServiceBufferLeftTimeout!\n");
		return(leftTimeout);
	}
	for (;;)
	{
		gunionServiceReceiverWakeuped = 0;
		sleep(leftTimeout);
		if (gunionServiceReceiverWakeuped)
			break;
		if ((leftTimeout = UnionGetServiceBufferLeftTimeout(-1)) < 0)
		{
			UnionUserErrLog("in UnionReadResponseOfSpecService:: UnionGetServiceBufferLeftTimeout!\n");
			return(leftTimeout);
		}
	}
	// 读取消息
	pack.receiverID = getpid();	
	if ((ret = msgrcv(pchanel->sysResponseQueueID,&pack,sizeof(pack.text),getpid(),IPC_NOWAIT)) < 0)
	{
		UnionSetReconnectServiceBuffer();
		UnionSystemErrLog("in UnionReadResponseOfSpecService:: msgrcv [%d][%0x]!\n",pchanel->userResponseQueueID,pchanel->sysResponseQueueID);
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
#else
	pack.receiverID = responseID;
	// 设置超时
	if ((ret = UnionSetServiceBufferTimeout(timeout)) < 0)
	{
		UnionUserErrLog("in UnionReadResponseOfSpecService:: UnionSetServiceBufferTimeout!\n");
		return(ret);
	}
	if ((ret = msgrcv(pchanel->sysResponseQueueID,&pack,sizeof(pack.text),responseID,0)) < 0)
	{
		UnionClearServiceBufferTimeout();
		UnionSetReconnectServiceBuffer();
		UnionSystemErrLog("in UnionReadResponseOfSpecService:: msgrcv [%d][%0x]!\n",pchanel->userResponseQueueID,pchanel->sysResponseQueueID);
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	UnionClearServiceBufferTimeout();	// 清除超时
#endif
	len = ret;
	// 复制数据
	if (len >= sizeOfBuf)
	{
		UnionUserErrLog("in UnionReadResponseOfSpecService:: sizeOfBuf [%d] too small to receive [%d]!\n",sizeOfBuf,len);
		// 置令牌可用
		return(UnionSetUserDefinedErrorCode(errCodeSmallBuffer));
	}
	memcpy(msg,pack.text,len);
	return(len);
}

// 执行一次服务交换
/* 输入参数：
	serviceID	指定的服务
	responseID	响应标识
	reqStr		请求串
	lenOfReqStr	请求长度
	sizeOfResStr	是接收缓冲的大小
	timeout		是读响应的超时值
   输出参数：
	resStr		为接收到的消息
   返回值：
   	>=0，消息的长度
   	负值，错误码
*/
int UnionExcuteSpecService(long serviceID,long responseID,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr,int timeout)
{
	int			ret;
				
	UnionSetServiceBufferTimeout(timeout);
	
	// 先提交请求
	if ((ret = UnionSendRequestOfSpecService(serviceID,reqStr,lenOfReqStr)) < 0)
	{
		UnionUserErrLog("in UnionExcuteSpecService:: UnionSendRequestOfSpecService serviceID=[%ld]\n",serviceID);
		return(ret);
	}
	// 再提取响应
	if ((ret = UnionReadResponseOfSpecService(serviceID,resStr,sizeOfResStr,responseID,-1)) < 0)
	{
		UnionUserErrLog("in UnionExcuteSpecService:: UnionReadResponseOfSpecService serviceID=[%ld] responseID=[%ld]\n",serviceID,responseID);
		return(ret);
	}
	return(ret);
}

/*
功能
	置超时标识
输入参数
	无
输出参数
	无
返回值
	无
*/
void UnionServiceBufferDealTimeout()
{
	gunionIsServiceBufferTimeout = 1;
	return;
}

/*
功能
	获得指定服务通道的超时
输入参数
	chanelID	通道号
输出参数
	无
返回值
	>=0	指定服务通道的超时
	<0	出错代码
*/
int UnionGetTimeoutOfSpecServiceChanel(long chanelID)
{
	char	varName[128+1];
	int	timeout;
	
	sprintf(varName,"timeoutOfChanel%ld",chanelID);
	if ((timeout = UnionReadIntTypeRECVar(varName)) <= 0)
		timeout = 2;
	return(timeout);
}

/*
功能
	获得指定服务通道可以缓冲的最大消息数
输入参数
	chanelID	通道号
输出参数
	无
返回值
	>=0	指定服务通道可以缓冲的最大消息数
	<0	出错代码
*/
int UnionGetMaxNumMsgOfSpecServiceChanel(long chanelID)
{
	char	varName[128+1];
	int	maxNumMsg;
	
	sprintf(varName,"maxNumMsgOfChanel%ld",chanelID);
	if ((maxNumMsg = UnionReadIntTypeRECVar(varName)) <= 0)
		maxNumMsg = 100;
	return(maxNumMsg);
}

/*
功能
	显示结构的标题
输入参数
	fp	输出数据的文件指针
输出参数
	无
返回值
	无
*/
void UnionDisplayTitleOfServiceChanel(FILE *fp)
{
	FILE	*outFp;

	outFp = UnionGetValidFp(fp);
	// 在此增加客户化源码
	fprintf(outFp,"%10s %4s %17s %6s %6s %8s\n","通道标识","功能","队列标识","消息数","字节数","操作时间");

	return;
}

/*
功能
	显示结构的内容
输入参数
	fp	输出数据的文件指针
	pdef	结构指针
输出参数
	无
返回值
		成功	1
		失败	0
*/
int UnionDisplayContentOfServiceChanel(PUnionServiceChanel pdef,FILE *fp)
{
	FILE			*outFp;
	int			msgNum;
	int			msgSize;
	long			ctime;
	int			ret;

	outFp = UnionGetValidFp(fp);
	// 在此增加客户化源码
	if ((ret = UnionGetStatusOfMsgQueue(pdef->sysRequestQueueID,&msgNum,&msgSize,&ctime)) < 0)
	{
		msgNum = -1;
		msgSize = -1;
		ctime = -1;
	}
	fprintf(outFp,"%10ld 请求 %08x-%08d %6d %6d %8d\n",
		pdef->serviceID,
		pdef->userRequestQueueID,pdef->sysRequestQueueID,
		msgNum,msgSize,ctime);
	if ((ret = UnionGetStatusOfMsgQueue(pdef->sysResponseQueueID,&msgNum,&msgSize,&ctime)) < 0)
	{
		msgNum = -1;
		msgSize = -1;
		ctime = -1;
	}
	fprintf(outFp,"%10ld 响应 %08x-%08d %6d %6d %8d\n",
		pdef->serviceID,
		pdef->userResponseQueueID,pdef->sysResponseQueueID,
		msgNum,msgSize,ctime);
	
	return(1);
}

/*
功能
	显示服务通道的状态
输入参数
	fp	文件指针
输出参数
	无
返回值
	>=0	成功
	<0	失败，错误码
*/
int UnionPrintStatusOfServiceBufferToFp(FILE *fp)
{
	PUnionServiceBuffer	pbus;
	int			index;
	int			realNum = 0;
	int			ret;
	
	if ((ret = UnionConnectServiceBuffer()) < 0)
	{
		UnionUserErrLog("in UnionDisplayStatusOfServiceBuffer:: 调用函数[UnionConnectServiceBuffer]出错!");
		return(ret);
	}
	pbus = pgunionServiceBuffer;
	UnionDisplayTitleOfServiceChanel(fp);
	for (index = 0; index < pbus->chanelNum; index++)
	{
		if ((ret = UnionDisplayContentOfServiceChanel(&(pbus->chanelGrp[index]),fp)) <= 0)
			continue;
		realNum++;
	}
	fprintf(fp,"服务通道数量=%d\n",realNum);
	return(realNum);
}

/*
功能
	显示服务通道的状态
输入参数
	fileName	文件名称
输出参数
	无
返回值
	>=0	成功
	<0	失败，错误码
*/
int UnionPrintStatusOfServiceBufferToSpecFile(char *fileName)
{
	FILE			*fp;
	int			ret;
		
	fp = UnionOpenFile(fileName);
	
	ret = UnionPrintStatusOfServiceBufferToFp(fp);
	
	UnionCloseFp(fp);
	
	return(ret);
}

/*
函数功能
       获取一个消息队列的状态
输入参数
       sysID		队列的标识
输出参数
       num		通道中当前等待处理的消息数目
       size		通道中当前等待处理的消息的总长度
       idleTime		空闲时间
返回值
       >=0		成功
       <0		失败
*/
int UnionGetStatusOfMsgQueue(int sysID,int *num,int *size,long *idleTime)
{
	struct msqid_ds status;
	long		selTime;
	time_t		now;
		
	if (msgctl(sysID,IPC_STAT,&status) != 0)
	{
		UnionSystemErrLog("in UnionGetStatusOfMsgQueue:: 调用[msgctl]获取队列[%d]的状态出错!",sysID);
		return(errCodeUseOSErrCode);
	}

	*num = status.msg_qnum;
	*size = status.msg_cbytes;
	if (status.msg_stime > status.msg_rtime)
		selTime = status.msg_stime;
	else
		selTime = status.msg_rtime;
	if (selTime < status.msg_ctime)
		selTime = status.msg_ctime;
	time(&now);
	*idleTime = now - selTime;
	return(0);
}

/*
功能
	维护服务通道的请求队列
输入参数
	pchanel
输出参数
	无
返回值
	>=0	成功
	<0	失败，错误码
*/
int UnionMaintainRequestOfServiceChanel(PUnionServiceChanel pchanel)
{
	int			num,size;
	long			idleTime;
	int			ret;
	int			realNum = 0;
	int			timeout,maxMsgNum;
	
	// 检查参数		
	if (pchanel == NULL)
		return(errCodeNullPointer);
	
	// 获得请求队列的状态
	if ((ret = UnionGetStatusOfMsgQueue(pchanel->sysRequestQueueID,&num,&size,&idleTime)) < 0)
	{
		UnionUserErrLog("in UnionMaintainRequestOfServiceChanel:: 调用函数[UnionGetStatusOfMsgQueue]获得队列[%d]的状态出错!",pchanel->sysRequestQueueID);
		return(ret);
	}
	
	// 判断空闲时间
	timeout = UnionGetTimeoutOfSpecServiceChanel(pchanel->serviceID);
	maxMsgNum = UnionGetMaxNumMsgOfSpecServiceChanel(pchanel->serviceID);
	if (idleTime > timeout)	// 空闲时间超过了预期时间
	{
		if (num > 0)	// 队列中有信息，此时的信息全是垃圾信息，要清理
		{	
			if ((ret = UnionClearAllTimeoutRequest(pchanel->sysRequestQueueID,timeout,num)) < 0)
			{
				UnionUserErrLog("in UnionMaintainRequestOfServiceChanel:: 调用函数[UnionClearAllTimeoutRequest]从队列[%d-%d]清除垃圾请求失败!",pchanel->userRequestQueueID,pchanel->sysRequestQueueID);
				return(ret);
			}
			if (ret > 0)
				UnionNullLog("共从队列[%d-%d]中清除了[%d]个超时[%d]请求!",pchanel->userRequestQueueID,pchanel->sysRequestQueueID,ret,timeout);
			realNum += ret;
			//return(ret);
			num -= ret;	// 从请求中减掉要垃圾请求
		}
		else
			return(0);	// 此时队列中没有信息，不需要清理
	}
	
	// 空闲时间比预期的超时时间小
	if (num <= maxMsgNum)
		return(realNum);	// 此时队列中缓冲的信息数目比可以等待的最大数量小，不需要清理
		
	// 队列中缓冲的信息数目比可以等待的消息数量大,清除多余的消息
	if ((ret = UnionClearRequestOfAtLeastNum(pchanel->sysRequestQueueID,num-maxMsgNum,timeout)) < 0)
	{
		UnionUserErrLog("in UnionMaintainRequestOfServiceChanel:: 调用函数[UnionClearRequestOfAtLeastNum]从队列[%d-%d]清除多余请求失败!",pchanel->userRequestQueueID,pchanel->sysRequestQueueID);
		return(ret);
	}
	if (ret > 0)
		UnionNullLog("共从队列[%d-%d]中清除了[%d]个多余请求!超时时间=[%d]",pchanel->userRequestQueueID,pchanel->sysRequestQueueID,ret,timeout);
	return(ret+realNum);
}

/*
功能
	维护服务通道的响应队列
输入参数
	pchanel
输出参数
	无
返回值
	>=0	成功
	<0	失败，错误码
*/
int UnionMaintainResponseOfServiceChanel(PUnionServiceChanel pchanel)
{
	int			num,size;
	long			idleTime;
	int			ret;
	int			maxNum;
	int			realNum = 0;
	int			timeout,maxMsgNum;
	
	// 检查参数		
	if (pchanel == NULL)
	{
		UnionUserErrLog("in UnionMaintainResponseOfServiceChanel","pchanel不能是空指针!");
		return(errCodeNullPointer);
	}
		
	// 获得响应队列的状态
	if ((ret = UnionGetStatusOfMsgQueue(pchanel->sysResponseQueueID,&num,&size,&idleTime)) < 0)
	{
		UnionUserErrLog("in UnionMaintainResponseOfServiceChanel:: 调用函数[UnionGetStatusOfMsgQueue]获得队列[%d]的状态出错!",pchanel->sysResponseQueueID);
		return(ret);
	}
	
	// 判断空闲时间
	timeout = UnionGetTimeoutOfSpecServiceChanel(pchanel->serviceID);
	maxMsgNum = UnionGetMaxNumMsgOfSpecServiceChanel(pchanel->serviceID);
	if (idleTime > timeout)	// 空闲时间超过了预期时间
	{
		if (num > 0)	// 队列中有信息，此时的信息全是垃圾信息，要清理
		{
			if ((ret = UnionClearAllTimeoutResponse(pchanel->sysResponseQueueID,timeout,num)) < 0)
			{
				UnionUserErrLog("in UnionMaintainResponseOfServiceChanel:: 调用函数[UnionClearAllTimeoutResponse]从队列[%d-%d]清除垃圾响应失败!",pchanel->userResponseQueueID,pchanel->sysResponseQueueID);
				return(ret);
			}
			if (ret > 0)
				UnionNullLog("共从队列[%d-%d]中清除了[%d]个垃圾响应!",pchanel->userResponseQueueID,pchanel->sysResponseQueueID,ret);
			realNum += ret;
			num -= ret;	// 从响应消息中减掉要垃圾请求
		}
		else
			return(0);	// 此时队列中没有信息，不需要清理
	}
	
	// 空闲时间比预期的超时时间小
	maxNum = maxMsgNum / 10;	// 同时缓冲的响应只允许占请求的10%
	if (maxNum <= 0)
		maxNum = 5;
	if (num <= maxNum)
		return(realNum);	// 此时队列中缓冲的信息数目比可以等待的最大数量小，不需要清理
		
	// 队列中缓冲的信息数目比可以等待的消息数量大,清除多余的消息
	if ((ret = UnionClearResponseOfAtLeastNum(pchanel->sysResponseQueueID,num-maxNum,timeout)) < 0)
	{
		UnionUserErrLog("in UnionMaintainResponseOfServiceChanel:: 调用函数[UnionClearResponseOfAtLeastNum]从队列[%d-%d]清除多余响应失败!",pchanel->userResponseQueueID,pchanel->sysResponseQueueID);
		return(ret);
	}
	if (ret > 0)
		UnionNullLog("共从队列[%d-%d]中清除了[%d]个多余响应!",pchanel->userResponseQueueID,pchanel->sysResponseQueueID,ret);
	return(ret+realNum);
}

/*
功能
	维护服务通道
输入参数
	pchanel
输出参数
	无
返回值
	>=0	成功
	<0	失败，错误码
*/
int UnionMaintainServiceChanel(PUnionServiceChanel pchanel)
{
	int			ret;
	int			num = 0;
	
	if ((ret = UnionMaintainRequestOfServiceChanel(pchanel)) < 0)
	{
		UnionUserErrLog("in UnionDisplayStatusOfServiceBuffer:: 调用函数[UnionMaintainRequestOfServiceChanel]出错!");
		return(ret);
	}
	num += ret;
	if ((ret = UnionMaintainResponseOfServiceChanel(pchanel)) < 0)
	{
		UnionUserErrLog("in UnionDisplayStatusOfServiceBuffer:: 调用函数[UnionMaintainResponseOfServiceChanel]出错!");
		return(ret);
	}
	return(num+ret);
}

/*
功能
	维护服务总线
输入参数
	无
输出参数
	无
返回值
	>=0	成功
	<0	失败，错误码
*/
int UnionMaintainServiceBuffer()
{
	PUnionServiceBuffer		pbus;
	int			index;
	int			realNum = 0;
	int			ret;
	int			msgNum = 0;
		
	if ((ret = UnionConnectServiceBuffer()) < 0)
	{
		UnionUserErrLog("in UnionMaintainServiceBuffer:: 调用函数[UnionConnectServiceBuffer]出错!");
		return(ret);
	}
	pbus = pgunionServiceBuffer;
	for (index = 0; index < pbus->chanelNum; index++)
	{
		if ((ret = UnionMaintainServiceChanel(&(pbus->chanelGrp[index]))) <= 0)
			continue;
		realNum++;
		msgNum += ret;
	}
	if (realNum > 0)
		UnionNullLog("修复的服务通道数量=%d\n",realNum);
	return(msgNum);
}

/*
功能
	清除队列中的所有超时服务信息
输入参数
	queueID		队列号
	timeout		超时
	maxNum		最大数量
	isRequest	1:判断请求，0:判断响应
输出参数
	无
返回值
	>=0	成功,垃圾信息数量
	<0	失败，错误码
*/
int UnionClearAllTimeoutServiceInfo(int queueID,int timeout,int maxNum,int isRequest)
{
	int				num = 0;
	TUnionServicePack		pack;
	int				ret;
	long				serviceID;
	int				len;
	int				isTimeout;
	int				retryTimes;
	
	for (retryTimes = 0; retryTimes < maxNum; retryTimes++)
	{
		// 读取一条信息
		if ((len = msgrcv(queueID,&pack,sizeof(pack.text),0,IPC_NOWAIT)) < 0)
			return(num);
		serviceID = pack.receiverID;
		if (UnionIsDirtyServiceInfo(pack.text,len))	// 是一条脏数据
		{
			pack.text[len] = 0;
			UnionNullLog("清除了一条脏数据[%d][%s]",len,pack.text);
			num++;
			continue;
		}
		// 判断是否是超时
		if (isRequest)
			isTimeout = UnionIsTimeoutServiceRequest(pack.text,len,timeout);
		else
			isTimeout = UnionIsTimeoutServiceResponse(pack.text,len,timeout);
		if (isTimeout)
		{
			// 超时
			if (isRequest)
				UnionLogServiceInfo("清除了以下超时请求::",pack.text,len);
			else
				UnionLogServiceInfo("清除了以下超时响应::",pack.text,len);
			num++;
			continue;
		}
		// 不是超时请求
		// 将请求重新放回队列
		if ((ret = msgsnd(queueID,&pack,len,IPC_NOWAIT)) < 0)
		{
			UnionSystemErrLog("UnionClearAllTimeoutServiceInfo","调用[msgsnd]出错!队列标识=[%d]!",queueID);
			return(errCodeUseOSErrCode);
		}
	}
	return(num);
}

/*
功能
	清除队列中的所有垃圾请求
输入参数
	queueID	队列号
	timeout	超时
	maxNum		最大数量
输出参数
	无
返回值
	>=0	成功,垃圾信息数量
	<0	失败，错误码
*/
int UnionClearAllTimeoutRequest(int queueID,int timeout,int maxNum)
{
	return(UnionClearAllTimeoutServiceInfo(queueID,timeout,maxNum,1));
}

/*
功能
	清除队列中的所有垃圾响应
输入参数
	queueID	队列号
	timeout	超时
	maxNum		最大数量
输出参数
	无
返回值
	>=0	成功,垃圾信息数量
	<0	失败，错误码
*/
int UnionClearAllTimeoutResponse(int queueID,int timeout,int maxNum)
{
	return(UnionClearAllTimeoutServiceInfo(queueID,timeout,maxNum,0));
}

/*
功能
	从队列中清除指定数量的信息
输入参数
	queueID		队列号
	atLeastNum	要清除的信息数量
	timeout		超时
	isRequest	1:判断请求，0:判断响应
输出参数
	无
返回值
	>=0	成功,垃圾信息数量
	<0	失败，错误码
*/
int UnionClearServiceInfoOfAtLeastNum(int queueID,int atLeastNum,int timeout,int isRequest)
{
	int				num = 0;
	TUnionServicePack	pack;
	int				ret;
	long				serviceID;
	int				len;
	
	// 先清除超时信息
	//if ((num = UnionClearAllTimeoutServiceInfo(queueID,timeout,atLeastNum,isRequest)) < 0)
	//	UnionUserErrLog("in UnionClearServiceInfoOfAtLeastNum:: 调用函数[UnionClearAllTimeoutServiceInfo]出错!"));
	
	if (num >= atLeastNum)	// 清除的超时信息数比要清除的数目多
		return(num);
	
	// 还没有清除足够的数目
	for (;;)
	{
		// 读取一条信息
		if ((len = msgrcv(queueID,&pack,sizeof(pack.text),0,IPC_NOWAIT)) < 0)
			return(num);
		if (isRequest)
			UnionLogServiceInfo("清除了以下超量请求::",pack.text,len);
		else
			UnionLogServiceInfo("清除了以下超量响应::",pack.text,len);
		num++;
		if (num >= atLeastNum)	// 清除的超时信息数比要清除的数目多
			return(num);
	}
}

/*
功能
	从请求队列中清除指定数量的信息
输入参数
	queueID		队列号
	atLeastNum	要清除的信息数量
	timeout		超时
输出参数
	无
返回值
	>=0	成功,垃圾信息数量
	<0	失败，错误码
*/
int UnionClearRequestOfAtLeastNum(int queueID,int atLeastNum,int timeout)
{
	return(UnionClearServiceInfoOfAtLeastNum(queueID,atLeastNum,timeout,1));
}

/*
功能
	从响应队列中清除指定数量的信息
输入参数
	queueID		队列号
	atLeastNum	要清除的信息数量
	timeout		超时
输出参数
	无
返回值
	>=0	成功,垃圾信息数量
	<0	失败，错误码
*/
int UnionClearResponseOfAtLeastNum(int queueID,int atLeastNum,int timeout)
{
	return(UnionClearServiceInfoOfAtLeastNum(queueID,atLeastNum,timeout,0));
}

/*
功能
	判断是否是垃圾服务信息
输入参数
	buf		服务请求字符串
	len		服务请求长度
输出参数
	无
返回值
	1		是
	0		不是
*/
int UnionIsDirtyServiceInfo(char *buf,int len)
{
	if (len < sizeof(TUnionMessageHeader))
		return(1);	// 是垃圾
	else
		return(0);
}
	
/*
功能
	判断是否是超时的服务请求
输入参数
	buf		服务请求字符串
	len		服务请求长度
	timeout		超时
输出参数
	无
返回值
	1		是
	0		不是
*/
int UnionIsTimeoutServiceRequest(char *buf,int len,int timeout)
{
	time_t			nowSeconds;
	int			ret;
	long			waitTime;
	PUnionMessageHeader	pheader;
	
	if (UnionIsDirtyServiceInfo(buf,len))
		return(0);

	pheader = (PUnionMessageHeader)buf;
	time(&nowSeconds);
	waitTime = nowSeconds - pheader->time;
	UnionNullLog("waitTime = [%ld] timeout = [%ld] now = [%ld] provider = [%ld]!",waitTime,timeout,nowSeconds,pheader->time);
	if (waitTime > timeout)
		return(1);
	else
		return(0);
}

/*
功能
	判断是否是超时的服务响应
输入参数
	buf		服务响应字符串
	len		服务响应长度
	timeout		超时
输出参数
	无
返回值
	1		是
	0		不是
*/
int UnionIsTimeoutServiceResponse(char *buf,int len,int timeout)
{
	return(UnionIsTimeoutServiceRequest(buf,len,timeout));
}

/*
功能
	将一个服务信息写到日志中
输入参数
	title		标题
	buf		服务响应字符串
	len		服务响应长度
输出参数
	无
返回值
	无
*/
void UnionLogServiceInfo(char *title,char *buf,int len)
{
	PUnionMessageHeader	pmsgHeader;
	
	if ((len < 0) || (buf == NULL))
		return;
	if (UnionIsDirtyServiceInfo(buf,len))
		return;
	if (title != NULL)
		UnionNullLog(title);
	pmsgHeader = (PUnionMessageHeader)buf;
	UnionNullLog("time=%ld,provider=%d,dealer=%d,msgIndex=%ld,type=%ld\n",
		pmsgHeader->time,pmsgHeader->provider,pmsgHeader->dealer,pmsgHeader->msgIndex,pmsgHeader->type);
	buf[len] = 0;
	UnionNullLog("data=[%04d][%s]",len-sizeof(*pmsgHeader),buf+sizeof(*pmsgHeader));
	return;
}
