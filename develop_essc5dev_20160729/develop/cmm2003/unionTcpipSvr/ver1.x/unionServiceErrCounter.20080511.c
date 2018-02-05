//	Author:		Wolfgang Wang
//	Date:		2009/9/30
//	Version:	1.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionREC.h"
#include "unionServiceErrCounter.h"
#include "errCodeTranslater.h"
#include "transSpierBuf.h"

TUnionServiceErrCounterGrp	gunionServiceErrCounterGrp;
PUnionServiceErrCounterGrp	pgunionServiceErrCounterGrp = NULL;

TUnionSecuSvrStatusCounter	gunionSecuSvrStatusCounter;
PUnionSecuSvrStatusCounter	pgunionSecuSvrStatusCounter = NULL;

int UnionConnectServiceErrCounter()
{
	if (pgunionServiceErrCounterGrp != NULL)
		return(0);
	return(UnionResetServiceErrCounter());
}

int UnionResetServiceErrCounter()
{
	if (pgunionServiceErrCounterGrp == NULL)
		pgunionServiceErrCounterGrp = &gunionServiceErrCounterGrp;
	memset(pgunionServiceErrCounterGrp,0,sizeof(*pgunionServiceErrCounterGrp));
	time(&(pgunionServiceErrCounterGrp->startTime));
	UnionGetFullSystemDateTime(pgunionServiceErrCounterGrp->startTimeStr);
	pgunionServiceErrCounterGrp->nowTime = pgunionServiceErrCounterGrp->startTime;
	return(0);
}

int UnionIncreaseServiceErrCode(char *idOfApp __attribute__((unused)),char *serviceCode,int errCode)
{
	int				ret;
	int				index;
	PUnionServiceErrCounter		pcounter;
	
	if ((ret = UnionConnectServiceErrCounter()) < 0)
		return(ret);
	if (errCode > 0)
		errCode = 0;
	for (index = 0; index < pgunionServiceErrCounterGrp->counterNum; index++)
	{
		pcounter = &(pgunionServiceErrCounterGrp->errGrp[index]);
		if ((strcmp(idOfApp,pcounter->idOfApp) != 0) || (strcmp(serviceCode,pcounter->serviceCode) != 0) || (errCode != pcounter->errCode))
			continue;
		time(&(pgunionServiceErrCounterGrp->nowTime));
		pcounter->times += 1;
		return(pcounter->times);
	}
	if (pgunionServiceErrCounterGrp->counterNum >= conMaxNumOfServiceErrCounter)
	{
		UnionAuditLog("in UnionIncreaseServiceErrCode:: errGrp is full!\n");
		return(errCodeErrCounterGrpIsFull);
	}
	pcounter = &(pgunionServiceErrCounterGrp->errGrp[pgunionServiceErrCounterGrp->counterNum]);
	memcpy(pcounter->idOfApp,idOfApp,sizeof(pcounter->idOfApp)-1);
	memcpy(pcounter->serviceCode,serviceCode,sizeof(pcounter->serviceCode)-1);
	pcounter->errCode = errCode;
	pcounter->times = 1;
	pgunionServiceErrCounterGrp->counterNum += 1;
	time(&(pgunionServiceErrCounterGrp->nowTime));
	return(pcounter->times);
}

int UnionSendAllSecuSvrServiceCountToMonitor(int resID)
{
	int				timeSnap;
	int				index;
	char				tmpBuf[1024+256+1];
	int				len;
	PUnionServiceErrCounter		pcounter;
	char				nowStr[14+1];
	char				serviceRemark[128+1], nameBuf[40+1];
	char				*p;
	
	if (pgunionServiceErrCounterGrp == NULL)
		return(errCodeErrCounterGrpNotConnected);
		
	if ((timeSnap = UnionReadIntTypeRECVar("secuServiceSpiedInterval")) < 0)
		timeSnap = 60;
	if ((pgunionServiceErrCounterGrp->nowTime - pgunionServiceErrCounterGrp->startTime < timeSnap) 
		&& (pgunionServiceErrCounterGrp->counterNum < conMaxNumOfServiceErrCounter))
	{
		// 2010-10-21, huangbx, 修改, 定义了spierEverySecuService参数则发送每笔交易监控
		if ((timeSnap = UnionReadIntTypeRECVar("spierEverySecuService")) < 0)
			return(0);	// 不符合发送条件
	}
	
	memset(nowStr,0,sizeof(nowStr));
	UnionGetFullSystemDateTime(nowStr);
	for (index = 0; index < pgunionServiceErrCounterGrp->counterNum; index++)
	{
		pcounter = &(pgunionServiceErrCounterGrp->errGrp[index]);

	// 2010-11-18 HuangBaoxin, 增加服务代码说明
	//	sprintf(tmpBuf,"startTime=%s|finishTime=%s|idOfApp=%s|serviceCode=%s|errCode=%d|times=%ld|",
	//		pgunionServiceErrCounterGrp->startTimeStr,nowStr,
	//		pcounter->idOfApp,pcounter->serviceCode,pcounter->errCode,pcounter->times);
		memset(serviceRemark, 0, sizeof(serviceRemark));
		memset(nameBuf, 0, sizeof(nameBuf));
		sprintf(nameBuf, "serviceRemark%s", pcounter->serviceCode);
		if ((p = UnionReadStringTypeRECVar(nameBuf)) != NULL)
		{
			strcpy(serviceRemark, p);
		}	
		else {
			sprintf(serviceRemark, "can not find remark for service code %s", pcounter->serviceCode);
		}

		//sprintf(tmpBuf,"startTime=%s|finishTime=%s|idOfApp=%s|serviceCode=%s|errCode=%d|times=%ld|serviceRemark=%s|",
		//	pgunionServiceErrCounterGrp->startTimeStr, nowStr, pcounter->idOfApp,
		//	pcounter->serviceCode,pcounter->errCode,pcounter->times, serviceRemark);
		
		// 2011-02-12 LiangJie, 把服务代码说明改为错误码说明
		sprintf(tmpBuf,"startTime=%s|finishTime=%s|idOfApp=%s|serviceCode=%s|errCode=%d|times=%ld|serviceRemark=%s|",
			pgunionServiceErrCounterGrp->startTimeStr, nowStr, pcounter->idOfApp,
			pcounter->serviceCode,pcounter->errCode,pcounter->times, UnionFindRemarkOfErrCode(pcounter->errCode));	

		len = strlen(tmpBuf);
		// huangbx，修改，dealOK置为0
		/*
		if (pcounter->errCode < 0)
			sprintf(tmpBuf+len,"dealOK=0|");
		else
			sprintf(tmpBuf+len,"dealOK=1|");
		*/
		sprintf(tmpBuf+len,"dealOK=0|");
		// huangbx
		len += 9;
		UnionSendTransInfoToMonitor(resID,tmpBuf,len);
		UnionNullLog("[%04d][%s]\n",len,tmpBuf);
	}
	UnionResetServiceErrCounter();
	return(pgunionServiceErrCounterGrp->counterNum);
}

// Add By HuangBaoxin, 2010-12-02
int UnionConnectToSecuSvrStatusCounter()
{
	if (pgunionSecuSvrStatusCounter != NULL)
		return 0;
	
	pgunionSecuSvrStatusCounter = &gunionSecuSvrStatusCounter;
	memset(pgunionSecuSvrStatusCounter, 0, sizeof(TUnionSecuSvrStatusCounter));
	
	return 0;
}

int UnionResetSecuSvrStatusCounter()
{
	int	ret;
	if ((ret = UnionConnectToSecuSvrStatusCounter()) < 0)
	{
		UnionUserErrLog("in UnionIncreaseSecuSvrStatusCounterByErrCode::UnionConnectToSecuSvrStatusCounter!");
		return ret;	
	}
	memset(pgunionSecuSvrStatusCounter, 0, sizeof(TUnionSecuSvrStatusCounter));
	return 0;
}

int UnionIncreaseSecuSvrStatusCounterByErrCode(char *idOfApp __attribute__((unused)),char *serviceCode __attribute__((unused)),int errCode)
{
	int	ret;
	int	timeSnap = 0;		
	char	tmpBuf[1024+1];
	char	nowTimeStr[14+1];
	int	len;
	int	resID = 0;
	
	if ((resID = UnionReadIntTypeRECVar("conResIDSecuSvrStatusTbl")) <= 0)
			resID = 97;
				
	if ((timeSnap = UnionReadIntTypeRECVar("intervalTimeForSecuSvrStatusCounter")) < 0)
			timeSnap = 10;
	
	if ((ret = UnionConnectToSecuSvrStatusCounter()) < 0)
	{
		UnionUserErrLog("in UnionIncreaseSecuSvrStatusCounterByErrCode::UnionConnectToSecuSvrStatusCounter!");
		return ret;	
	}
	
	if (pgunionSecuSvrStatusCounter->startTime == 0) {
		time(&(pgunionSecuSvrStatusCounter->startTime));
		UnionGetFullSystemDateTime(pgunionSecuSvrStatusCounter->startTimeStr);
	}

	time(&(pgunionSecuSvrStatusCounter->finishTime));
		
	if (errCode < 0)
		pgunionSecuSvrStatusCounter->failNum++;
	
	pgunionSecuSvrStatusCounter->totalNum++;
	
	if (pgunionSecuSvrStatusCounter->totalNum > 0)
		pgunionSecuSvrStatusCounter->failRate = (double)pgunionSecuSvrStatusCounter->failNum/(double)pgunionSecuSvrStatusCounter->totalNum * 100.00;
	
	UnionLog("in UnionIncreaseSecuSvrStatusCounterByErrCode::errCode[%d], startTime[%ld], finishTime[%ld]\n",
		errCode, pgunionSecuSvrStatusCounter->startTime, pgunionSecuSvrStatusCounter->finishTime);
		
	if ((pgunionSecuSvrStatusCounter->finishTime - pgunionSecuSvrStatusCounter->startTime) >= timeSnap)
	{
		memset(tmpBuf, 0, sizeof(tmpBuf));
		memset(nowTimeStr, 0, sizeof(nowTimeStr));
		UnionGetFullSystemDateTime(nowTimeStr);
		sprintf(tmpBuf, "startTime=%s|finishTime=%s|totalNum=%ld|failNum=%ld|failRate=%f|dealOK=0|",
				pgunionSecuSvrStatusCounter->startTimeStr, nowTimeStr,
				pgunionSecuSvrStatusCounter->totalNum,
				pgunionSecuSvrStatusCounter->failNum,
				pgunionSecuSvrStatusCounter->failRate);
				
		len = strlen(tmpBuf);
		UnionSendTransInfoToMonitor(resID,tmpBuf,len);
		UnionNullLog("[%04d][%s]\n",len,tmpBuf);
		
		UnionResetSecuSvrStatusCounter();
	}
	
	return 0;
}

// End Add By HuangBaoxin, 2010-12-02

