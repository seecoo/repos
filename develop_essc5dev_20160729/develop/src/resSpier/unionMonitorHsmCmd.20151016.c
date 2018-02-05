#include <stdio.h>
#include <string.h>
#include <syslog.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "unionREC.h"
#include "UnionTask.h"
#include "unionVersion.h"
#include "unionCommBetweenMDL.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "unionXMLPackage.h"
#include "unionMXML.h"
#include "unionCommand.h"
#include "unionRealDBCommon.h"
#include "unionMsgBufGroup.h"
#include "unionMDLID.h"
#include "UnionStr.h"
#include "unionMonitorResID.h"
#include "unionTransInfoToMonitor.h"
#include "unionMonitorHsmCmd.h"


int UnionHsmCmdSpier()
{
	int			ret;
	int			len;
	unsigned char		buf[512];
	char			sendBuf[544];
	char			systemDateTime[20];

	TUnionMessageHeader     msgHeader;
	TUnionModuleID          applierMDLID;

	UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfMon);
	if ((len = UnionReadRequestToSpecifiedModuleWithMsgHeader(conMDLTypeOffsetOfMonSvr+ conMDLTypeUnionMon_HsmCmd,buf,sizeof(buf),&applierMDLID,&msgHeader)) <= 0)
	{
		UnionUserErrLog("in UnionHsmCmdSpier:: UnionReadRequestToSpecifiedModuleWithMsgHeader [%d]!\n",conMDLTypeOffsetOfMonSvr + conMDLTypeUnionMon_HsmCmd);
		return(len);
	}               
	buf[len] = 0;
	UnionDebugLog("in UnionHsmCmdSpier:: send buf [%s]\n", buf);

	memset(systemDateTime, 0, sizeof(systemDateTime));
	UnionGetFullSystemDateTime(systemDateTime);
	len = snprintf(sendBuf,sizeof(sendBuf),"%s|systemTime=%s|",buf,systemDateTime);

	if ((ret = UnionSendResouceInfoToMonitor(conResIDMonHsmCmd,sendBuf,len, 1)) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdSpier:: UnionSendResouceInfoToMonitor\n");
		return(ret);
	}
	return ret;
}

int UnionStartHsmCmdSpier()
{
	int		ret;
	while (1)
	{
		if ((ret = UnionHsmCmdSpier()) < 0)
		{
			UnionUserErrLog("in UnionStartHsmCmdSpier:: UnionHsmCmdSpier!\n");
			continue;
		}
	}
	return 0;
}
