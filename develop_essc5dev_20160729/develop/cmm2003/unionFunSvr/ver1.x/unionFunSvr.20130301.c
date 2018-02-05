#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "UnionLog.h"
#include "unionErrCode.h"
#include "UnionSocket.h"
#include "unionREC.h"
#include "unionMsgBufGroup.h"

#include "unionCommBetweenMDL.h"
#include "unionFunSvrName.h"
#include "unionFunSvr.h"
#include "UnionStr.h"

int UnionStartFunSvr()
{
	int			ret;
	int			isAPP;
	char			tmpBuf[1024];
	unsigned char		reqBuf[204800];
	unsigned char		resBuf[204800];
	long			lenOfReq,lenOfRes;
	TUnionModuleID		idOfSenderTask;
	int			waitTime;
	time_t			nowTime;
	TUnionModuleID		funSvrID;
	char			varName[512];
	TUnionMessageHeader	msgHeader;
	char			sysID[32];
	char			userID[64];
	char			*ptr1 = NULL;
	char			*ptr2 = NULL;
	
	signal(SIGPIPE,SIG_IGN);	// 忽略该信号量

	if ((ret = UnionUserSpecFunForFunSvr()) < 0)
	{
		UnionUserErrLog("in UnionStartFunSvr:: UnionUserSpecFunForFunSvr ret = [%d]!\n",ret);
		return(ret);
	}
	
	funSvrID = UnionGetFixedMDLIDOfFunSvr();
	UnionLog("in UnionStartFunSvr:: funSvr started OK! funSvrID = [%ld]\n",funSvrID);

	if ((ptr1 = UnionGetFunSvrStartVar()) != NULL)
	{
		isAPP = atoi(ptr1);
	}
	else
		isAPP = 0;
	
	if (((ptr1 = UnionGetFunSvrName()) != NULL) && (strlen(ptr1) > 0))
	{
		ret = sprintf(varName,"%s%s","timeoutOf",ptr1);
		varName[ret] = 0;
	}
	else
		varName[0] = 0;

	ret = 0;
	for (;;)
	{
		if (ret < 0)
		{
			sprintf(tmpBuf,"funSvrExitCode%d",abs(ret));
			if (UnionReadIntTypeRECVar(tmpBuf) > 0)
			{
				return(ret);
			}
		}

		if (isAPP == 1)
			UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfAPP);
		else
			UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfUI);
		
		// 从缓冲区读取信息
		if ((lenOfReq = UnionReadRequestToSpecifiedModuleWithMsgHeader(funSvrID,reqBuf,sizeof(reqBuf),&idOfSenderTask,&msgHeader)) <= 0)
		{
			UnionUserErrLog("in UnionStartFunSvr:: UnionReadRequestToSpecifiedModuleWithMsgHeader funSvrID[%ld] ret = [%d]!\n", funSvrID, ret);
			ret = lenOfReq;
			//continue;
			break;
		}
		reqBuf[lenOfReq] = 0;
		
		// 读取系统ID
		if ((ptr1 = strstr((char *)reqBuf,"<sysID>")) != NULL)
		{
			ptr1 += 7;
			if ((ptr2 = strstr(ptr1,"</sysID>")))
			{
				ret = ptr2 - ptr1;
				memcpy(sysID,ptr1,ret);
				sysID[ret] = 0;
			}
		}
		
		if (UnionIsUITrans(sysID))
		{
			if ((ptr1 = strstr((char *)reqBuf,"<userID>")) != NULL)
			{
				ptr1 += 8;
				ptr2 = strstr(ptr1,"</userID>");
				if ((ptr2 = strstr(ptr1,"</userID>")))
				{
					ret = ptr2 - ptr1;
					memcpy(userID,ptr1,ret);
					userID[ret] = 0;
					UnionSetSuffixOfMyLogFile(userID);
				}
			}
		}
		else
		{
			UnionSetSuffixOfMyLogFile(sysID);
		}

		if ((waitTime = UnionReadIntTypeRECVar(varName)) > 0)	// 只有在定义了本变量时才判断超时
		{
			// 判断是否超时消息
			time(&nowTime);
			if ((nowTime - msgHeader.time > waitTime) && (msgHeader.time > 0))
			{
				reqBuf[lenOfReq] = 0;
				UnionUserErrLog("in UnionStartFunSvr:: outdate msg [%012ld] [%07d] [%04ld] [%08ld] [%s]\n",
							idOfSenderTask,msgHeader.provider,nowTime - msgHeader.time,lenOfReq,reqBuf);
				ret = 0;	// 2010-01-21,Wolfgang Wang added	
				continue;
			}
		}
		UnionSetMultiLogBegin();
		if ((ret = UnionFunSvrInterprotor((char *)reqBuf,lenOfReq,(char *)resBuf,sizeof(resBuf))) < 0)
		{
			UnionUserErrLog("in UnionStartFunSvr:: UnionFunSvrInterprotor Error ret = [%d]!\n",ret);
			UnionSetMultiLogEnd();
			continue;
		}
		UnionSetMultiLogEnd();

		if (ret == 0)
			continue;

		resBuf[ret] = 0;
		lenOfRes = ret;
		
		if (isAPP == 1)
			UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfAPP);
		else
			UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfUI);
		if ((ret = UnionSendResponseToApplyModuleWithOriMsgHeader(idOfSenderTask,resBuf,lenOfRes,&msgHeader)) < 0)
		{
			UnionUserErrLog("in UnionStartFunSvr:: UnionSendResponseToApplyModuleWithOriMsgHeader!\n");
			continue;
		}
	}
	return(ret);
}
