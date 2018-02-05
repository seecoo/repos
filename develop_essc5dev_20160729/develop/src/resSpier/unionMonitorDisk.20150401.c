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
#include "unionMonitorDisk.h"

extern int gunionIntervalTimeOfMon;

int UnionDiskSpier()
{
        int             ret = 0;
        int             offset;
	char		systemDateTime[20];
        char            tmpBuf[1024] ={0};
	TUnionDiskInfo	diskInfo[10];
	int		count;
	int		i;

	memset(systemDateTime, 0, sizeof(systemDateTime));
	UnionGetFullSystemDateTime(systemDateTime);

	if ((count = UnionGetDiskInfo(diskInfo, sizeof(diskInfo)/sizeof(diskInfo[0]))) < 0)
	{
		UnionUserErrLog("in UnionDiskSpier:: UnionGetDiskInfo!\n");
		return(count);
	}

	for (i = 0; i < count; i++)
	{
		memset(tmpBuf, 0, sizeof(tmpBuf));
		offset = snprintf(tmpBuf, sizeof(tmpBuf), "systemTime=%s|fileSystem=%s|total=%ld|used=%ld|free=%ld|usedPercent=%d|", systemDateTime, diskInfo[i].fileSystem, diskInfo[i].total, diskInfo[i].used, diskInfo[i].free, diskInfo[i].usedPercent);
		UnionLog("in UnionDiskSpier:: send resID[%d], buf[%s]\n", conResIDDiskInfo, tmpBuf);
		ret =   UnionSendResouceInfoToMonitor(conResIDDiskInfo, tmpBuf, offset, 1);// 向监控服务器发送
		if (ret < 0)
		{
			UnionUserErrLog("in UnionDiskSpier:: UnionSendResouceInfoToMonitor!\n");
			//return ret;
		}
	}
	return(ret);
}

int UnionStartDiskSpier()
{
	int		ret;

	if (gunionIntervalTimeOfMon <= 0)
		gunionIntervalTimeOfMon = 300;
	while (1)
	{
		ret = UnionDiskSpier();
		if (ret < 0)
		{
			UnionUserErrLog("in UnionStartDiskSpier:: UnionDiskSpier error!\n");
			break;
		}
		sleep(gunionIntervalTimeOfMon);
	}
	return ret;
}
