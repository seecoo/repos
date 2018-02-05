// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/08/08
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "unionResID.h"
#include "unionOperationAuthorization.h"
#include "unionMngSvrList.h"
#include "mngSvrCommProtocol.h"
#include "mngSvrTeller.h"
#include "simuMngSvrLocally.h"
#include "UnionLog.h"

int UnionReadConfOfOtherMngSvr(int resID,int serviceID,char *ipAddr,int *port)
{
	char	mngSvrID[40+1];
	int	ret;
	
	memset(mngSvrID,0,sizeof(mngSvrID));
	if ((ret = UnionReadOperationAuthorizationRecFld(resID,serviceID,conOperationAuthorizationFldNameIdOfMngSvr,mngSvrID,sizeof(mngSvrID))) < 0)
	{
		UnionUserErrLog("in UnionReadConfOfOtherMngSvr:: UnionReadOperationAuthorizationRecFld resID [%03d] serviceID [%03d] not valid!\n",resID,serviceID);
		return(ret);
	}
	if ((ret = UnionReadMngSvrListRecFld(mngSvrID,conMngSvrListFldNameIpAddr,ipAddr,16)) < 0)
	{
		UnionUserErrLog("in UnionReadConfOfOtherMngSvr:: UnionReadMngSvrListRecFld resID [%03d] serviceID [%03d] mngSvrID = [%s]\n",resID,serviceID,mngSvrID);
		return(ret);
	}
	if ((ret = UnionReadMngSvrListRecIntTypeFld(mngSvrID,conMngSvrListFldNamePort,port)) < 0)
	{
		UnionUserErrLog("in UnionReadConfOfOtherMngSvr:: UnionReadMngSvrListRecFld resID [%03d] serviceID [%03d] mngSvrID = [%s]\n",resID,serviceID,mngSvrID);
		return(ret);
	}
	return(ret);
}
			
int UnionExcuteOtherSvrService(int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	char		ipAddr[15+1];
	char		tmpStr[10+1];
	int		isUseCliMngSvr = 0;
	int		port;
	int		ret;
	
	memset(ipAddr,0,sizeof(ipAddr));
	ret = UnionReadFldFromCurrentMngSvrClientReqStr("ipAddrOfMngSvr", ipAddr, sizeof(ipAddr));
	if (ret > 0)
	{
		memset(tmpStr, 0, sizeof tmpStr);
		ret = UnionReadFldFromCurrentMngSvrClientReqStr("portOfMngSvr", tmpStr, sizeof(tmpStr));
		if (ret > 0)
		{
			port = atoi(tmpStr);
			isUseCliMngSvr = 1;
		}
	}
	if (!isUseCliMngSvr)
	{
		if ((ret = UnionReadConfOfOtherMngSvr(resID,serviceID,ipAddr,&port)) < 0)
		{
			UnionUserErrLog("in UnionExcuteOtherSvrService:: UnionReadConfOfOtherMngSvr resID [%03d] serviceID [%03d] not valid!\n",resID,serviceID);
			return(ret);
		}
	}
	UnionLog("in UnionExcuteOtherSvrService:: begin comm with [%s] [%d] by teller [%s]\n",ipAddr,port,UnionGetCurrentOperationTellerNo());
	if ((ret = UnionCommunicationWithSpecMngSvr(ipAddr,port,UnionGetCurrentOperationTellerNo(),resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved)) < 0)
	{
		UnionUserErrLog("in UnionCommunicationWithSpecMngSvr:: UnionExchangeInfoWithSpecMngSvr resID [%03d] serviceID [%03d] with mngSvr [%s] [%d]\n",
				resID,serviceID,ipAddr,port);
		return(ret);
	}
	UnionLog("in UnionExcuteOtherSvrService:: comm with [%s] [%d] by teller [%s] OK\n",ipAddr,port,UnionGetCurrentOperationTellerNo());
	return(ret);
}

