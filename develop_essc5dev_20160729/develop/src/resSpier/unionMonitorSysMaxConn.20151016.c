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
#include "unionMonitorSysMaxConn.h"


int UnionSysMaxConnSpier()
{
	int			ret;
	int			len;
	unsigned char		buf[512];
	char			sendBuf[544];
	char			systemDateTime[20];

	TUnionMessageHeader     msgHeader;
	TUnionModuleID          applierMDLID;

	UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfMon);
	UnionDebugLog("in UnionSysMaxConnSpier:: mdlType[%d]\n",conMDLTypeOffsetOfMonSvr + conMDLTypeUnionMon_MaxConn);
	if ((len = UnionReadRequestToSpecifiedModuleWithMsgHeader(conMDLTypeOffsetOfMonSvr + conMDLTypeUnionMon_MaxConn,buf,sizeof(buf),&applierMDLID,&msgHeader)) <= 0)
	{
		UnionUserErrLog("in UnionSysMaxConnSpier:: UnionReadRequestToSpecifiedModuleWithMsgHeader [%d]!\n",conMDLTypeOffsetOfMonSvr + conMDLTypeUnionMon_MaxConn);
		return(len);
	}               
	buf[len] = 0;
	UnionDebugLog("in UnionSysMaxConnSpier:: send buf [%s]\n", buf);

	memset(systemDateTime, 0, sizeof(systemDateTime));
	UnionGetFullSystemDateTime(systemDateTime);
	len = snprintf(sendBuf,sizeof(sendBuf),"%s|systemTime=%s|",buf,systemDateTime);

	if ((ret = UnionSendResouceInfoToMonitor(conResIDHsmThrough,sendBuf,len, 1)) < 0)
	{
		UnionUserErrLog("in UnionSysMaxConnSpier:: UnionSendResouceInfoToMonitor\n");
		return(ret);
	}
	return ret;
}

int UnionStartSysMaxConnSpier()
{
	int		ret;
	while (1)
	{
		if ((ret = UnionSysMaxConnSpier()) < 0)
		{
			UnionUserErrLog("in UnionStartSysMaxConnSpier:: UnionSysMaxConnSpier!\n");
			continue;
		}
	}
	return 0;
}
