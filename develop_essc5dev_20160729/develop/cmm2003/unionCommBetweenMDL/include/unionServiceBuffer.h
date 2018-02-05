// Wolfgang Wang
// 2005/12/29

#ifndef	_unionServiceBuffer_
#define _unionServiceBuffer_

#include "unionMsgBuf6.x.h"

// 2010-12-20,增加，王纯军
// 定义一个服务进程令牌
typedef struct
{
	long	serviceID;		// 服务标识
	char	tokenID[8];		// 服务进程
} TUnionServiceToken;
typedef TUnionServiceToken		*PUnionServiceToken;

// 定义一个服务消息
typedef struct
{
	long	receiverID;		// 接受者标识
	char	text[8192+1];		// 消息正文
} TUnionServicePack;
typedef TUnionServicePack		*PUnionServicePack;

// 定义一个服务通道
typedef struct
{
	long	serviceID;		// 服务标识号
#ifdef _useSerivceChanelToken_
	int	userTokenQueueID;	// 用户指定的令牌进程队列标识
	int	sysTokenQueueID;	// 可用的令牌队列标识
#endif
	int	userRequestQueueID;	// 用户指定的请求缓冲队列
	int	sysRequestQueueID;	// 请求缓冲队列的OS标识号
	int	userResponseQueueID;	// 用户指定的响应缓冲队列
	int	sysResponseQueueID;	// 响应缓冲队列的OS标识号
} TUnionServiceChanel;
typedef TUnionServiceChanel		*PUnionServiceChanel;

// 定义服务组
#define conMaxNumOfServiceChanel	8
typedef struct
{
	int				chanelNum;				// 通道数量
	TUnionServiceChanel		chanelGrp[conMaxNumOfServiceChanel];	// 通道组
} TUnionServiceBuffer;
typedef TUnionServiceBuffer		*PUnionServiceBuffer;

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
void UnionResetServiceBufferLeftTimeout(int userTimeout);

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
void UnionSetReconnectServiceBuffer();

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
void UnionLogServiceBuffer(PUnionServiceBuffer pbuffer);

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
int UnionGetServiceBufferLeftTimeout(int userTimeout);

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
int UnionSetServiceBufferTimeout(int userTimeout);

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
int UnionClearServiceBufferTimeout();

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
int UnionConnectServiceBuffer();

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
int UnionConnectServiceBufferAnyway();

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
int UnionDisconnectServiceBuffer();

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
PUnionServiceChanel UnionFindChanelOfSpecService(long serviceID);

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
int UnionClearRequestToSpecToken(PUnionServiceChanel pchanel,int tokenID);
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
int UnionClearResponseOfSpecID(PUnionServiceChanel pchanel,int specID);

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
int UnionGetTokenOfSpecService(long serviceID,PUnionServiceChanel pchanel);
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
int UnionSetTokenOfSpecServiceAvailable(long serviceID,int tokenID);
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
int UnionSetTokenOfSpecChanelAvailable(PUnionServiceChanel pchanel,int tokenID);
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
int UnionSendRequestOfSpecService(long serviceID,unsigned char *msg,int lenOfMsg);

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
int UnionSendResponseOfSpecService(long serviceID,unsigned char *msg,int lenOfMsg,int responseID);

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
int UnionReadRequestOfSpecService(long serviceID,unsigned char *msg,int sizeOfBuf);

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
int UnionReadResponseOfSpecService(long serviceID,unsigned char *msg,int sizeOfBuf,long responseID,int timeout);

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
int UnionExcuteSpecService(long serviceID,long responseID,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr,int timeout);

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
void UnionServiceBufferDealTimeout();

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
int UnionGetTimeoutOfSpecServiceChanel(long chanelID);

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
int UnionGetMaxNumMsgOfSpecServiceChanel(long chanelID);

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
void UnionDisplayTitleOfServiceChanel(FILE *fp);

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
int UnionDisplayContentOfServiceChanel(PUnionServiceChanel pdef,FILE *fp);

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
int UnionPrintStatusOfServiceBufferToFp(FILE *fp);

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
int UnionPrintStatusOfServiceBufferToSpecFile(char *fileName);

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
int UnionGetStatusOfMsgQueue(int sysID,int *num,int *size,long *idleTime);

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
int UnionMaintainRequestOfServiceChanel(PUnionServiceChanel pchanel);

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
int UnionMaintainResponseOfServiceChanel(PUnionServiceChanel pchanel);

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
int UnionMaintainServiceChanel(PUnionServiceChanel pchanel);

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
int UnionMaintainServiceBuffer();

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
int UnionClearAllTimeoutServiceInfo(int queueID,int timeout,int maxNum,int isRequest);

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
int UnionClearAllTimeoutRequest(int queueID,int timeout,int maxNum);

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
int UnionClearAllTimeoutResponse(int queueID,int timeout,int maxNum);

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
int UnionClearServiceInfoOfAtLeastNum(int queueID,int atLeastNum,int timeout,int isRequest);

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
int UnionClearRequestOfAtLeastNum(int queueID,int atLeastNum,int timeout);

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
int UnionClearResponseOfAtLeastNum(int queueID,int atLeastNum,int timeout);

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
int UnionIsDirtyServiceInfo(char *buf,int len);
	
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
int UnionIsTimeoutServiceRequest(char *buf,int len,int timeout);

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
int UnionIsTimeoutServiceResponse(char *buf,int len,int timeout);

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
void UnionLogServiceInfo(char *title,char *buf,int len);

#endif
