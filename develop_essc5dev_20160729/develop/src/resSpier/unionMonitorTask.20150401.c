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

#define RESSPIERNAME	"resSpierForTaskTbl"


extern int gunionIntervalTimeOfMon;

int UnionTaskSpier()
{
	int             index = 0;
        int             ret = 0;
        int             offset;
	char		systemDateTime[20];
        char            tmpBuf[1024] ={0};

        PUnionTaskClass ptaskClassGrp, ptaskClass;
        int             taskClassNum = 0;

        if ((ptaskClassGrp = UnionGetCurrentTaskClassGrp()) == NULL)
        {
                UnionUserErrLog("in UnionTaskSpier:: UnionGetCurrentTaskClassGrp!\n");
                return(errCodeTaskMDL_Connect);
        }

        taskClassNum = UnionGetCurrentTaskClassNum();

	memset(systemDateTime, 0, sizeof(systemDateTime));
	UnionGetFullSystemDateTime(systemDateTime);

        for (index = 0; index < taskClassNum; index++)
        {
                ptaskClass = ptaskClassGrp + index;
                if (strlen(ptaskClass->name) == 0)
                        continue;
		if (strncmp(ptaskClass->name, UnionGetApplicationName(), strlen(UnionGetApplicationName())) == 0)
			continue;

                memset(tmpBuf,0,sizeof(tmpBuf));
                snprintf(tmpBuf,sizeof(tmpBuf), "taskName=%s|instanceNum=%d|minInstanceNum=%d|logFile=%s|systemTime=%s|",\
                                ptaskClass->name,ptaskClass->currentNum,\
                                ptaskClass->minNum,ptaskClass->logFileName,systemDateTime);

                offset = strlen(tmpBuf);

                UnionLog("in UnionTaskSpier:: send resID[%d], buf[%s]\n", conResIDTask, tmpBuf);
                ret =   UnionSendResouceInfoToMonitor(conResIDTask, tmpBuf, offset, 1);// 向监控服务器发送
                if (ret < 0)
                {
                        UnionUserErrLog("in UnionTaskSpier:: UnionSendResouceInfoToMonitor!\n");
                        break;
                }
	}
	return(ret);
}

int UnionStartTaskSpier()
{
	int	ret;

	if (gunionIntervalTimeOfMon <= 0)
		gunionIntervalTimeOfMon = 30;
	while (1)
	{
		ret = UnionTaskSpier();
		if (ret < 0)
		{
			UnionUserErrLog("in UnionStartTaskSpier:: UnionTaskSpier error!\n");
			break;
		}
		sleep(gunionIntervalTimeOfMon);
	}
	return ret;
}
