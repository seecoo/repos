#include <stdio.h>
#include <string.h>
#include <syslog.h>

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "unionVersion.h"
#include "unionMonitorResID.h"
#include "unionTransInfoToMonitor.h"

#include "unionErrCode.h"
#include "unionResID.h"
#include "mngSvrServicePackage.h"

#include "UnionTask.h"
#include "unionREC.h"
#include "unionHardwareInfo.h"

extern int gunionIntervalTimeOfMon;

int UnionMemSpier()
{
        int             ret = 0;
        int             offset;
	char		systemDateTime[20];
        char            tmpBuf[1024] ={0};
	long		memoryTotal;
	long		memoryUesed;
	long		memoryFree;


	memset(systemDateTime, 0, sizeof(systemDateTime));
	UnionGetFullSystemDateTime(systemDateTime);

	if ((ret = UnionGetMemInfo(&memoryTotal, &memoryUesed, &memoryFree)) < 0)
	{
		UnionUserErrLog("in UnionMemSpier:: UnionGetMemInfo!\n");
		return(ret);
	}

	offset = snprintf(tmpBuf, sizeof(tmpBuf), "systemTime=%s|memoryTotal=%ld|memoryUesed=%ld|memoryFree=%ld|", systemDateTime, memoryTotal, memoryUesed, memoryFree);
	UnionLog("in UnionMemSpier:: send resID[%d], buf[%s]\n", conResIDMemInfo, tmpBuf);
	ret =   UnionSendResouceInfoToMonitor(conResIDMemInfo, tmpBuf, offset, 1);// 向监控服务器发送
	if (ret < 0)
	{
		UnionUserErrLog("in UnionMemSpier:: UnionSendResouceInfoToMonitor!\n");
		return ret;
	}
	return(ret);
}

int UnionStartMemSpier()
{
	int		ret;

	if (gunionIntervalTimeOfMon <= 0)
		gunionIntervalTimeOfMon = 30;
	while (1)
	{
		ret = UnionMemSpier();
		if (ret < 0)
		{
			UnionUserErrLog("in UnionStartMemSpier:: UnionMemSpier error!\n");
			break;
		}
		sleep(gunionIntervalTimeOfMon);
	}
	return ret;
}
