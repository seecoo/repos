//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2004/07/26

#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "unionSJL06API.h"
#include "unionErrCode.h"
#include "UnionLog.h"

PUnionSJL06Conn UnionCreateSJL06Conn(PUnionSJL06 psjl06)
{
	PUnionSJL06Conn	psjl06Conn;
	
	if (psjl06 == NULL)
	{
		UnionUserErrLog("in UnionCreateSJL06Conn:: psjl06 is null!\n");
		return(NULL);
	}
	if ((psjl06Conn = (PUnionSJL06Conn)malloc(sizeof(*psjl06Conn))) == NULL)
	{
		UnionSystemErrLog("in UnionCreateSJL06Conn:: malloc!\n");
		return(NULL);
	}
	psjl06Conn->psjl06 = psjl06;
	if ((psjl06Conn->sckHDL = UnionCreateSocketClient(psjl06Conn->psjl06->staticAttr.ipAddr,
							psjl06Conn->psjl06->staticAttr.port)) < 0)
	{
		UnionUserErrLog("in UnionCreateSJL06Conn:: UnionCreateSocketClient [%s] [%d]!\n",psjl06Conn->psjl06->staticAttr.ipAddr,
							psjl06Conn->psjl06->staticAttr.port);
		free(psjl06Conn);
		UnionSetWorkingSJL06Abnormal(psjl06);
		return(NULL);
	}
	psjl06Conn->psjl06->dynamicAttr.status = conOnlineSJL06;
	++psjl06Conn->psjl06->dynamicAttr.activeLongConn;
	return(psjl06Conn);
}
	
int UnionCloseSJL06Conn(PUnionSJL06Conn psjl06Conn)
{
	if (psjl06Conn == NULL)
		return(0);
	if (psjl06Conn->sckHDL >= 0)
		UnionCloseSocket(psjl06Conn->sckHDL);
	if (psjl06Conn->psjl06->dynamicAttr.activeLongConn > 0)
		--psjl06Conn->psjl06->dynamicAttr.activeLongConn;
	free(psjl06Conn);
	psjl06Conn = NULL;
	return(0);
}

PUnionSJL06Server UnionConnectSJL06Server(char *hsmGrpID,TUnionSJL06ServerType serverType)
{
	int	ret;
	int	i;
	
	PUnionSJL06Server	psjl06Server;
	
	if ((serverType != conUnionESSCSvr) && (serverType != conUnionEnumHsmSvr))
	{
		UnionUserErrLog("in UnionConnectSJL06Server:: serverType = [%d] error!\n",serverType);
		return(NULL);
	}
	
	if (hsmGrpID == NULL)
	{
		UnionUserErrLog("in UnionConnectSJL06Server:: null hsmGrpID!\n");
		return(NULL);
	}
	if (strlen(hsmGrpID) != 3)
	{
		UnionUserErrLog("in UnionConnectSJL06Server:: hsmGrpID [%s] Error!\n",hsmGrpID);
		return(NULL);
	}

	if ((psjl06Server = (PUnionSJL06Server)malloc(sizeof(*psjl06Server))) == NULL)
	{
		UnionSystemErrLog("in UnionConnectSJL06Server:: malloc!\n");
		return(NULL);
	}
	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		UnionUserErrLog("in UnionConnectSJL06Server:: UnionConnectWorkingSJL06MDL!\n");
		free(psjl06Server);
		psjl06Server = NULL;
		return(NULL);
	}
	
	memset(psjl06Server,0,sizeof(*psjl06Server));
	strcpy(psjl06Server->hsmGrpID,hsmGrpID);
	psjl06Server->currentConnNum = 0;
	psjl06Server->serverType = serverType;
	for (i = 0; i < conMaxNumOfSJL06Conn; i++)
	{
		psjl06Server->phsmConn[i] = NULL;
	}
			
	return(psjl06Server);	
}

int UnionDisconnectSJL06Server(PUnionSJL06Server psjl06Server)
{
	int	ret;
	int	i;
	
	if (psjl06Server == NULL)
		return(0);
	
	for (i = 0; i < conMaxNumOfSJL06Conn; i++)
	{
		UnionCloseSJL06Conn(psjl06Server->phsmConn[i]);
	}					
			
	UnionDisconnectWorkingSJL06MDL();
	
	free(psjl06Server);
	
	psjl06Server = NULL;
	
	return(0);
}

#ifndef _SJL06ServerMainFunExternalDefined_
int UnionSJL06ServerService(PUnionSJL06Server psjl06Server,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	PUnionSJL06	psjl06;
	int		i;
	int		ret;
	int		retryTimes = 0;
		
	if (psjl06Server == NULL)
	{
		UnionUserErrLog("in UnionSJL06ServerService:: psjl06Server is null!\n");
		return(errCodeParameter);
	}
	
	i = 0;
	while (i < psjl06Server->currentConnNum)
	{
		if (psjl06Server->phsmConn[i]->psjl06->dynamicAttr.status != conOnlineSJL06)
		{
			UnionCloseSJL06Conn(psjl06Server->phsmConn[i]);
			psjl06Server->phsmConn[i] = psjl06Server->phsmConn[psjl06Server->currentConnNum - 1];
			psjl06Server->phsmConn[psjl06Server->currentConnNum - 1] = NULL;
			--psjl06Server->currentConnNum;
		}
		else
			++i;
	}
	
retryOnceMore:
	if (retryTimes == 3)
		return(errCodeSJL06MDL_ServiceFailure);
	
	if ((psjl06 = UnionFindIdleWorkingSJL06(psjl06Server->hsmGrpID)) == NULL)
	{
		UnionUserErrLog("in UnionSJL06ServerService:: UnionFindIdleWorkingSJL06!\n");
		return(errCodeSJL06MDL_NoWorkingSJL06);
	}
	
	for (i = 0; i < psjl06Server->currentConnNum; i++)
	{
		if (psjl06 == psjl06Server->phsmConn[i]->psjl06)
			break;
	}
	if ((i >= psjl06Server->currentConnNum) && (psjl06Server->currentConnNum < conMaxNumOfSJL06Conn))
	{
		if ((psjl06Server->phsmConn[i] = UnionCreateSJL06Conn(psjl06)) == NULL)
		{
			UnionUserErrLog("in UnionSJL06ServerService:: UnionCreateSJL06Conn!\n");
			retryTimes++;
			goto retryOnceMore;
		}
		++psjl06Server->currentConnNum;
	}
	if (psjl06Server->serverType == conUnionESSCSvr)
		ret = UnionLongConnSJL06Cmd(psjl06Server->phsmConn[i]->sckHDL,psjl06Server->phsmConn[i]->psjl06,(char *)reqStr,lenOfReqStr,(char *)resStr,sizeOfResStr);
	else
		ret = UnionLongConn2HexLenSJL06Cmd(psjl06Server->phsmConn[i]->sckHDL,psjl06Server->phsmConn[i]->psjl06,(char *)reqStr,lenOfReqStr,(char *)resStr,sizeOfResStr);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionSJL06ServerService:: UnionLongConnSJL06Cmd!\n");
		UnionCloseSJL06Conn(psjl06Server->phsmConn[i]);
		psjl06Server->phsmConn[i] = psjl06Server->phsmConn[psjl06Server->currentConnNum - 1];
		psjl06Server->phsmConn[psjl06Server->currentConnNum - 1] = NULL;
		--psjl06Server->currentConnNum;
		retryTimes++;
		goto retryOnceMore;
	}
	return(ret);
}
#endif
