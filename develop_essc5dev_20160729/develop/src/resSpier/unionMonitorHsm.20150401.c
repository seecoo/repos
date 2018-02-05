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

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif
#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"

#include "UnionTask.h"
#include "unionREC.h"
#include "unionMonitorHsm.h"

extern int gunionIntervalTimeOfMon;

int UnionHsmSpier()
{
	int		ret;
	int		i;
	int		offset;
	int		recordNum;
	char		sql[512];
	char		hsmInfo[1024];
	char		hsmID[32];
	char		hsmGroupID[32];
	char		ipAddr[16];
	int		port;
	int		status;
	int		enabled;
	char		tmpBuf[8];
	char		systemDateTime[20];

	memset(systemDateTime, 0, sizeof(systemDateTime));
	UnionGetFullSystemDateTime(systemDateTime);

	snprintf(sql, sizeof(sql), "select hsmID,hsmGroupId,IpAddr,Port,enabled,status from hsm where enabled=1");
	if ((recordNum = UnionSelectRealDBRecord(sql,0,0)) < 0)
        {
                UnionUserErrLog("in UnionStartHsmSpier:: UnionSelectRealDBRecord[%s]!\n",sql);
                return(recordNum);
        }
        else if (recordNum == 0)
        {
                UnionLog("in UnionHsmSpier:: count[0] sql[%s]!\n",sql);
                //return(errCodeParameter);
        }

	for(i = 0; i < recordNum;i++)
	{
		offset = 0;
		memset(hsmInfo, 0, sizeof(hsmInfo));
		if ((ret = UnionLocateXMLPackage("detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionHsmSpier:: UnionLocateXMLPackage\n");
			continue;
		}
		if ((ret = UnionReadXMLPackageValue("hsmID", hsmID, sizeof(hsmID))) < 0)
		{
			UnionUserErrLog("in UnionHsmSpier:: UnionReadXMLPackageValue[%s]!\n","hsmID");
			continue;
		}
		if ((ret = UnionReadXMLPackageValue("hsmGroupID", hsmGroupID, sizeof(hsmGroupID))) < 0)
		{
			UnionUserErrLog("in UnionHsmSpier:: UnionReadXMLPackageValue[%s]!\n","hsmGroupID");
			continue;
		}
		if ((ret = UnionReadXMLPackageValue("ipAddr", ipAddr, sizeof(ipAddr))) < 0)
		{
			UnionUserErrLog("in UnionHsmSpier:: UnionReadXMLPackageValue[%s]!\n","ipAddr");
			continue;
		}
		memset(tmpBuf, 0, sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("port", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionHsmSpier:: UnionReadXMLPackageValue[%s]!\n","port");
			continue;
		}
		port = atoi(tmpBuf);
		memset(tmpBuf, 0, sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("status", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionHsmSpier:: UnionReadXMLPackageValue[%s]!\n","status");
			continue;
		}
		status = atoi(tmpBuf);
		memset(tmpBuf, 0, sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("enabled", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionHsmSpier:: UnionReadXMLPackageValue[%s]!\n","enabled");
			continue;
		}
		enabled = atoi(tmpBuf);

		//组报文发送给监控服务器
		offset=snprintf(hsmInfo,sizeof(hsmInfo),"hsmID=%s|hsmGroupID=%s|ipAddr=%s|port=%d|status=%d|enabled=%d|systemTime=%s|", hsmID, hsmGroupID, ipAddr, port, status,enabled, systemDateTime);
                UnionLog("in UnionHsmSpier:: send resID[%d], buf[%s]\n", conResIDHsm, hsmInfo);
                ret =   UnionSendResouceInfoToMonitor(conResIDHsm, hsmInfo, offset, 1);// 向监控服务器发送
                if (ret < 0)
                {
                        UnionUserErrLog("in UnionHsmSpier:: UnionSendResouceInfoToMonitor!\n");
			continue;
                }
	}
	return 0;
}

int UnionStartHsmSpier()
{
	int	ret;

	if (gunionIntervalTimeOfMon <= 0)
		gunionIntervalTimeOfMon = 30;
	while (1)
	{
		ret = UnionHsmSpier();
		if (ret < 0)
		{
			UnionUserErrLog("in UnionStartHsmSpier:: UnionHsmSpier error!\n");
			break;
		}
		sleep(gunionIntervalTimeOfMon);
	}
	return ret;
}
