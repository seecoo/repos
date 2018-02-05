#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include "unionErrCode.h"
#include "unionMonitorResID.h"
#include "mngSvrServicePackage.h"
#include "unionMsgQueue.h"
#include "UnionLog.h"
#include "unionTransInfoToMonitor.h"

extern int gunionIntervalTimeOfMon;

int UnionMsgQueueSpier()
{
	int		index = 0;
	int		offset = 0,ret = 0;
	char 		tmpBuf[1024] ={0};
	char		systemTime[20];
	TUnionMsgQueueGrp	*pUnionMsgQueueGrp;
	
	char		queueRemark[128];
	
	if ((pUnionMsgQueueGrp = UnionGetAllMsgQueueStatus()) == NULL)
	{
		UnionUserErrLog("in UnionMsgQueueSpier::UnionGetAllMsgQueueStatus!\n");
		return(-1);
	}

	memset(systemTime, 0, sizeof(systemTime));
	UnionGetFullSystemDateTime(systemTime);
	
	for (index = 0; index < pUnionMsgQueueGrp->num; index++)
	{
		// 2010-11-17 HuangBaoxin, 增加消息队列备注信息域
		if (index == 0)
		{
			snprintf(queueRemark,sizeof(queueRemark),"当前空闲统计");
		}
		else
			snprintf(queueRemark,sizeof(queueRemark),"当前占用统计");
		
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"userId=%d|systemId=%d|msgNum=%d|msgLength=%d|queueRemark=%s",\
			pUnionMsgQueueGrp->queue[index].userID,
			pUnionMsgQueueGrp->queue[index].systemID,
			pUnionMsgQueueGrp->queue[index].msgNum,
			pUnionMsgQueueGrp->queue[index].charNum,
			queueRemark);
		
		offset = strlen(tmpBuf);
		
		UnionLog("in UnionMsgQueueSpier::send, resID[%d], buf[%s]\n", conResIDQueue, tmpBuf);
		if ((ret = UnionSendResouceInfoToMonitor(conResIDQueue,tmpBuf,offset, 1)) < 0) // 向监控服务器发送
		{
			UnionUserErrLog("in UnionMsgQueueSpier:: UnionSendResouceInfoToMonitor\n");
			break;
		}
	}

	return ret;
}

int UnionStartMsgQueueSpier()
{
	if (gunionIntervalTimeOfMon <= 0)
		gunionIntervalTimeOfMon = 5;

	while(1)
	{
		UnionMsgQueueSpier();
		sleep(gunionIntervalTimeOfMon);
	}
}
