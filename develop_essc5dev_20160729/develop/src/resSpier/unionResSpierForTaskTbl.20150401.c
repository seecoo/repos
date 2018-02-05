#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include "unionErrCode.h"
#include "unionResID.h"
#include "mngSvrServicePackage.h"

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "UnionTask.h"
#include "unionREC.h"

extern int	gunionIntervalTimeOfMon;
extern int	gunionResId;
extern int	gunionSendTestResDataFlag;

extern int	UnionSendTransInfoToMonitor(int resID,char *data,int len);

int UnionResSpierInit()
{
	gunionResId = conResIDTaskTBL;

	gunionSendTestResDataFlag = UnionReadIntTypeRECVar("resSpierSendTestDataOfTaskTbl");

	gunionIntervalTimeOfMon = UnionReadIntTypeRECVar("intervalTimeOfTaskTbl");
	if (gunionIntervalTimeOfMon <= 0)
		gunionIntervalTimeOfMon = 60;

	return 0;
}

int UnionSendResSpierTestData()
{
	char 		tmpBuf[1024] ={0};
	sprintf(tmpBuf,"taskName=%s|instanceNum=%d|minInstanceNum=%d|logFile=%s|startCmd=%s|",
			"test", 0, 0, "test", "test");
	// 向监控服务器发送
	UnionSendTransInfoToMonitor(gunionResId, tmpBuf, strlen(tmpBuf));
	
	return(0);
}

int UnionSendResSpierData()
{
	int		index = 0;
	//int		clientNum = 0;
	int		ret = 0;
	//char		rec[512+1];
	//unsigned char	pack[512+64+1];
	int		offset;
	//int		i;
	char 		tmpBuf[1024] ={0};

	PUnionTaskClass ptaskClassGrp,ptaskClass;
	int		taskClassNum = 0;

	if ((ptaskClassGrp = UnionGetCurrentTaskClassGrp()) == NULL)
	{
		UnionUserErrLog("in UnionSendTcpSpierData:: UnionGetCurrentTaskClassGrp!\n");
		return(errCodeTaskMDL_Connect);
	}

	taskClassNum = UnionGetCurrentTaskClassNum();

	for (index = 0; index < taskClassNum; index++)
	{
		ptaskClass = ptaskClassGrp + index;
		if (strlen(ptaskClass->name) == 0)
			continue;
		/*
		   if ((ptaskClass->minNum == 0) || (ptaskClass->currentNum == 0))
		   continue;
		 */
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"taskName=%s|instanceNum=%d|minInstanceNum=%d|logFile=%s|startCmd=%s|",\
				ptaskClass->name,ptaskClass->currentNum,\
				ptaskClass->minNum,ptaskClass->logFileName,ptaskClass->startCmd);

		offset = strlen(tmpBuf);

		UnionLog("in UnionSendResSpierData::send, resID[%d], buf[%s]\n", gunionResId, tmpBuf);
		ret = 	UnionSendTransInfoToMonitor(gunionResId, tmpBuf, offset);// 向监控服务器发送
		if (ret < 0)
		{
			UnionUserErrLog("in UnionSendResSpierData::UnionSendTransInfoToMonitor!\n");
			break;
		}
	}

	return ret;
}

