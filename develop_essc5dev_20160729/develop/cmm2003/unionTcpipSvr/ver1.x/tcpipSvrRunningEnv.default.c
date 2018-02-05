// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2005/12/30
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include "UnionLog.h"

#include "transSpierBuf.h"
#include "unionREC.h"
#include "unionCommConf.h"
#include "unionMsgBuf.h"

int UnionConnectRunningEnv()
{
	int	ret;
	
#ifndef _noRegisterClient_
	if ((ret = UnionConnectCommConfTBL()) < 0)
	{
		UnionUserErrLog("in UnionConnectRunningEnv:: UnionConnectCommConfTBL! ret = [%d]!\n",ret);
		return(ret);
	}
#endif
	/*
	if ((ret = UnionConnectCommBetweenMDLs()) < 0)
	{
		UnionUserErrLog("in UnionConnectRunningEnv:: UnionConnectCommBetweenMDLs! ret = [%d]!\n",ret);
		return(ret);
	}
	*/
#ifndef _noSpierInfo_
	if ((ret = UnionConnectTransSpierBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionConnectRunningEnv:: UnionConnectTransSpierBufMDL! ret = [%d]!\n",ret);
		return(ret);
	}
#endif
	if ((ret = UnionConnectREC()) < 0)
        {
                UnionUserErrLog("in UnionConnectRunningEnv:: UnionConnectREC! ret = [%d]!\n",ret);
                return(ret);
        }
	
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionConnectRunningEnv:: UnionConnectMsgBufMDL! ret = [%d]!\n",ret);
		return(ret);
	}
	
	return(ret);
}
