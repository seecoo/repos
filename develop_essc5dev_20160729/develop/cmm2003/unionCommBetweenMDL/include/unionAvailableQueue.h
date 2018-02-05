// Wolfgang Wang
// 2010-12-22

#ifndef	_unionAvailableQueue_
#define _unionAvailableQueue_

#include "unionErrCode.h"

// 定义一个服务队列
typedef struct
{
	int	userRequestQueueID;		// 用户指定的请求缓冲队列
	int	sysRequestQueueID;		// 请求缓冲队列的OS标识号
	int	userResponseQueueID;		// 用户指定的响应缓冲队列
	int	sysResponseQueueID;		// 响应缓冲队列的OS标识号
} TUnionServiceQueue;
typedef TUnionServiceQueue	*PUnionServiceQueue;

// 定义一个服务队列池
// 一个队列池中可以包括的队列的最大数量
#define conMaxNumOfQueuePerPool			64
typedef struct
{
	char			poolID[40+1];			// 队列池标识
	int			queueNum;			// 队列池中包括的队列数量
	PUnionServiceQueue	queueGrp[conMaxNumOfQueuePerPool];	// 队列
} TUnionQueuePool;
typedef TUnionQueuePool		*PUnionQueuePool;

#endif
