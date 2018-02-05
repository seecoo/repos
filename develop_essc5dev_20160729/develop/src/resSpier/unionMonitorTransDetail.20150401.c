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
#include "unionMonitorTransDetail.h"


int UnionTransDetailSpier()
{
	int			ret;
	int			len;
	unsigned char		buf[1024];
	char			sendBuf[1024];
	char			systemTime[20];
	char			serviceCode[8];
	char			sysID[24];
	char			appID[24];
	char			clientIPAddr[16];
	char			useTime[8];
	char			responseCode[16];
	char			responseRemark[512];

	TUnionMessageHeader     msgHeader;
	TUnionModuleID          applierMDLID;

	UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfMon);
	if ((len = UnionReadRequestToSpecifiedModuleWithMsgHeader(conMDLTypeOffsetOfMonSvr + conMDLTypeUnionMonSvr,buf,sizeof(buf),&applierMDLID,&msgHeader)) <= 0)
	{
		UnionUserErrLog("in UnionTransDetailSpier:: UnionReadRequestToSpecifiedModuleWithMsgHeader [%d]!\n",conMDLTypeOffsetOfMonSvr + conMDLTypeUnionMonSvr);
		return(len);
	}               

	memset(systemTime, 0, sizeof(systemTime));
	UnionGetFullSystemDateTime(systemTime);

	buf[len] = 0;
	UnionLog("in UnionTransDetailSpier:: TransDetail [%s]\n", buf);

	if (memcmp((char *)buf,PACKAGE_VERSION_001,4) == 0)
        {
                UnionSetPackageType(PACKAGE_TYPE_V001);
        }
        else if (memcmp((char *)buf,"<?xml",5) == 0)
        {
                UnionSetPackageType(PACKAGE_TYPE_XML);
        }
	else
	{
		UnionUserErrLog("UnionTransDetailSpier:: package type error!\n");
		return(errCodePackageDefMDL_InvalidPackageType);
	}

	if ((ret = UnionInitResponseXMLPackage(NULL,(char *)buf,len)) < 0)
	{
		UnionUserErrLog("UnionTransDetailSpier:: UnionInitResponseXMLPackage !\n");
		return(ret);
	}
	memset(serviceCode, 0, sizeof(serviceCode));
	if ((ret = UnionReadResponseXMLPackageValue("head/serviceCode", serviceCode, sizeof(serviceCode))) < 0)
	{
		UnionUserErrLog("in UnionTransDetailSpier:: UnionReadResponseXMLPackageValue serviceCode!\n");
		return(ret);
	}
	memset(sysID, 0, sizeof(sysID));
	if ((ret = UnionReadResponseXMLPackageValue("head/sysID", sysID, sizeof(sysID))) < 0)
	{
		UnionUserErrLog("in UnionTransDetailSpier:: UnionReadResponseXMLPackageValue sysID!\n");
		return(ret);
	}
	memset(appID, 0, sizeof(appID));
	if ((ret = UnionReadResponseXMLPackageValue("head/appID", appID, sizeof(appID))) < 0)
	{
		UnionUserErrLog("in UnionTransDetailSpier:: UnionReadResponseXMLPackageValue appID!\n");
		return(ret);
	}
	memset(clientIPAddr, 0, sizeof(clientIPAddr));
	if ((ret = UnionReadResponseXMLPackageValue("head/clientIPAddr", clientIPAddr, sizeof(clientIPAddr))) < 0)
	{
		UnionUserErrLog("in UnionTransDetailSpier:: UnionReadResponseXMLPackageValue clientIPAddr!\n");
		return(ret);
	}
	memset(useTime, 0, sizeof(useTime));
	if ((ret = UnionReadResponseXMLPackageValue("head/useTime", useTime, sizeof(useTime))) < 0)
	{
		UnionUserErrLog("in UnionTransDetailSpier:: UnionReadResponseXMLPackageValue useTime!\n");
		return(ret);
	}
	memset(responseCode, 0, sizeof(responseCode));
	if ((ret = UnionReadResponseXMLPackageValue("head/responseCode", responseCode, sizeof(responseCode))) < 0)
	{
		UnionUserErrLog("in UnionTransDetailSpier:: UnionReadResponseXMLPackageValue responseCode!\n");
		return(ret);
	}
	memset(responseRemark, 0, sizeof(responseRemark));
	if ((ret = UnionReadResponseXMLPackageValue("head/responseRemark", responseRemark, sizeof(responseRemark))) < 0)
	{
		UnionUserErrLog("in UnionTransDetailSpier:: UnionReadResponseXMLPackageValue responseRemark!\n");
		return(ret);
	}

/*
	if ((ret = UnionXMLInitFromBuf(&xmlc, (char *)buf)) < 0)
	{
		UnionUserErrLog("in UnionTransDetailSpier:: UnionXMLInitFromBuf!\n");
		return(ret);
	}

	memset(serviceCode, 0, sizeof(serviceCode));
	if ((ret = UnionXMLGet(&xmlc, "/union/head/serviceCode", 0, serviceCode, sizeof(serviceCode))) < 0)
	{
		UnionUserErrLog("in UnionTransDetailSpier:: UnionXMLGet serviceCode!\n");
		return(ret);
	}
	memset(sysID, 0, sizeof(sysID));
	if ((ret = UnionXMLGet(&xmlc, "/union/head/sysID", 0, sysID, sizeof(sysID))) < 0)
	{
		UnionUserErrLog("in UnionTransDetailSpier:: UnionXMLGet sysID!\n");
		return(ret);
	}
	memset(appID, 0, sizeof(appID));
	if ((ret = UnionXMLGet(&xmlc, "/union/head/appID", 0, appID, sizeof(appID))) < 0)
	{
		UnionUserErrLog("in UnionTransDetailSpier:: UnionXMLGet appID!\n");
		return(ret);
	}
	memset(clientIPAddr, 0, sizeof(clientIPAddr));
	if ((ret = UnionXMLGet(&xmlc, "/union/head/clientIPAddr", 0, clientIPAddr, sizeof(clientIPAddr))) < 0)
	{
		UnionUserErrLog("in UnionTransDetailSpier:: UnionXMLGet clientIPAddr!\n");
		return(ret);
	}
	memset(useTime, 0, sizeof(useTime));
	if ((ret = UnionXMLGet(&xmlc, "/union/head/useTime", 0, useTime, sizeof(useTime))) < 0)
	{
		UnionUserErrLog("in UnionTransDetailSpier:: UnionXMLGet useTime!\n");
		return(ret);
	}
	memset(responseCode, 0, sizeof(responseCode));
	if ((ret = UnionXMLGet(&xmlc, "/union/head/responseCode", 0, responseCode, sizeof(responseCode))) < 0)
	{
		UnionUserErrLog("in UnionTransDetailSpier:: UnionXMLGet responseCode!\n");
		return(ret);
	}
	memset(responseRemark, 0, sizeof(responseRemark));
	if ((ret = UnionXMLGet(&xmlc, "/union/head/responseRemark", 0, responseRemark, sizeof(responseRemark))) < 0)
	{
		UnionUserErrLog("in UnionTransDetailSpier:: UnionXMLGet responseRemark!\n");
		return(ret);
	}
*/

	len = snprintf(sendBuf, sizeof(sendBuf), "systemTime=%s|serviceCode=%s|sysID=%s|appID=%s|clientIPAddr=%s|useTime=%s|responseCode=%s|responseRemark=%s|", systemTime, serviceCode, sysID, appID, clientIPAddr, useTime, responseCode, responseRemark);
	UnionLog("in UnionTransDetailSpier:: sendBuf [%s]\n", sendBuf);

	if ((ret = UnionSendResouceInfoToMonitor(conResIDTransDetail,(char *)sendBuf,len, 0)) < 0)
	{
		UnionUserErrLog("in UnionTransDetailSpier:: UnionSendResouceInfoToMonitor\n");
		return(ret);
	}
	return ret;
}

int UnionStartTransDetailSpier()
{
	int		ret;
	while (1)
	{
		if ((ret = UnionTransDetailSpier()) < 0)
		{
			UnionUserErrLog("in UnionStartTransDetailSpier:: UnionTransDetailSpier!\n");
			continue;
		}
	}
	return 0;
}
