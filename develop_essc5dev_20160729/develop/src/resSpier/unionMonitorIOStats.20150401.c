#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionHardwareInfo.h"
#include "unionMonitorResID.h"
#include "unionTransInfoToMonitor.h"

#define conDeviceMaxNum	5
#define conNetMaxNum	5
extern int gunionIntervalTimeOfMon;

int UnionIOStatSpier()
{
	int			ret = 0;
	int			count;
	int			i, len;
	char			tmpBuf[1024];
	char			systemDateTime[20];
	TUnionDeviceIOStats	deviceIOStats[conDeviceMaxNum];
	TUnionNetIOStats	netIOStats[conNetMaxNum];

	memset(systemDateTime, 0, sizeof(systemDateTime));
	UnionGetFullSystemDateTime(systemDateTime);

	// diskIO
	memset(deviceIOStats, 0, sizeof(deviceIOStats));
	if ((count = UnionGetDiskIOStats(deviceIOStats, conDeviceMaxNum)) < 0)
	{
		UnionUserErrLog("in UnionIOStatSpier:: UnionGetDiskIOStats!\n");
		return(count);
	}
	for (i = 0; i < count && i < conDeviceMaxNum; i++)
	{
		len = snprintf(tmpBuf, sizeof(tmpBuf), "systemTime=%s|device=disk:%s|inKB=%0.2lf|outKB=%0.2lf|tps=%d|", systemDateTime,deviceIOStats[i].name,deviceIOStats[i].inKB, deviceIOStats[i].outKB, deviceIOStats[i].tps);
		UnionLog("in UnionIOStatSpier:: send resID[%d], buf[%s]\n", conResIDIOInfo, tmpBuf);
		ret =   UnionSendResouceInfoToMonitor(conResIDIOInfo, tmpBuf, len, 1);// 向监控服务器发送
		if (ret < 0)
		{
			UnionUserErrLog("in UnionIOStatSpier:: UnionSendResouceInfoToMonitor!\n");
			return (ret);
		}
	}

	// netIO
	memset(netIOStats, 0, sizeof(netIOStats));
	if ((count = UnionGetNetIOStats(netIOStats, conNetMaxNum)) < 0)
	{
		UnionUserErrLog("in UnionIOStatSpier:: UnionGetNetIOStats!\n");
		return(count);
	}
	for (i = 0; i < count && i < conNetMaxNum; i++)
	{
		len = snprintf(tmpBuf, sizeof(tmpBuf), "systemTime=%s|device=network:%s|inKB=%0.2lf|outKB=%0.2lf|tps=%d|", systemDateTime,netIOStats[i].name, netIOStats[i].inKB, netIOStats[i].outKB, 0);
		UnionLog("in UnionIOStatSpier:: send resID[%d], buf[%s]\n", conResIDIOInfo, tmpBuf);
		ret =   UnionSendResouceInfoToMonitor(conResIDIOInfo, tmpBuf, len, 1);// 向监控服务器发送
		if (ret < 0)
		{
			UnionUserErrLog("in UnionIOStatSpier:: UnionSendResouceInfoToMonitor!\n");
			return (ret);
		}
	}
	
	return(ret);
}

int UnionStartIOStatSpier()
{
	int		ret;

	if (gunionIntervalTimeOfMon <= 0)
		gunionIntervalTimeOfMon = 30;
	while (1)
	{
		ret = UnionIOStatSpier();
		if (ret < 0)
		{
			UnionUserErrLog("in UnionStartIOStatSpier:: UnionIOStatSpier error!\n");
			break;
		}
		sleep(gunionIntervalTimeOfMon);
	}
	return ret;
}
