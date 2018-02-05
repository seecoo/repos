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

int UnionCpuSpier()
{
        int             ret = 0;
        int             offset;
	char		systemDateTime[20];
        char            tmpBuf[1024] ={0};
	int		cpuUserUsed;
	int		cpuSystemUsed;
	int		cpuIdle;


	memset(systemDateTime, 0, sizeof(systemDateTime));
	UnionGetFullSystemDateTime(systemDateTime);

	if ((ret = UnionGetCpuInfo(&cpuUserUsed, &cpuSystemUsed, &cpuIdle)) < 0)
	{
		UnionUserErrLog("in UnionCpuSpier:: UnionGetCpuInfo!\n");
		return(ret);
	}

	offset = snprintf(tmpBuf, sizeof(tmpBuf), "systemTime=%s|cpuUserUsed=%d|cpuSystemUsed=%d|cpuIdle=%d|", systemDateTime, cpuUserUsed, cpuSystemUsed, cpuIdle);
	UnionLog("in UnionCpuSpier:: send resID[%d], buf[%s]\n", conResIDCpuInfo, tmpBuf);
	ret =   UnionSendResouceInfoToMonitor(conResIDCpuInfo, tmpBuf, offset, 1);// 向监控服务器发送
	if (ret < 0)
	{
		UnionUserErrLog("in UnionCpuSpier:: UnionSendResouceInfoToMonitor!\n");
		return ret;
	}
	return(ret);
}

int UnionStartCpuSpier()
{
	int		ret;

	if (gunionIntervalTimeOfMon <= 0)
		gunionIntervalTimeOfMon = 30;
	while (1)
	{
		ret = UnionCpuSpier();
		if (ret < 0)
		{
			UnionUserErrLog("in UnionStartCpuSpier:: UnionCpuSpier error!\n");
			break;
		}
		sleep(gunionIntervalTimeOfMon);
	}
	return ret;
}
