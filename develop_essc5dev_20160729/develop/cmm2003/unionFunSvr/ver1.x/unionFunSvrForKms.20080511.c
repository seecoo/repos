// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2005/12/30
// Version:	1.0

// 2008/3/25,屏蔽了sig_pipe信号量

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "unionREC.h"

#ifndef _UnionLogMDL_3_x_ 
#define _UnionLogMDL_3_x_ 
#endif

#include "unionErrCode.h"
#include "UnionSocket.h"

#include "unionCommBetweenMDL.h"
#include "unionFunSvrName.h"
#include "UnionLog.h"

int UnionKmsHsmTaskInterprotor(unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr);

int UnionStartFunSvr()
{
	int			ret;
	unsigned char		tmpBuf[8192+100];
	int			lenOfReq,lenOfRes;
	TUnionModuleID		idOfSenderTask;
	int			waitTime;
	time_t			nowTime;
	TUnionModuleID		funSvrID;
	char			varName[100];
	TUnionMessageHeader	msgHeader;
	
	// 2008/3/24, added by Wolfgang Wang
	signal(SIGPIPE,SIG_IGN);	// 忽略该信号量
	// 2008/3/24, end of addition
	
	UnionLog("in UnionStartFunSvr:: funSvr started OK! funSvrID = [%d]\n",funSvrID = UnionGetFixedMDLIDOfFunSvr());
	sprintf(varName,"timeoutOf%s",UnionGetFunSvrName());

	ret = 0;	// 2010-01-21,Wolfgang Wang added
	for (;;)
	{
		// 2010-01-21,Wolfgang Wang added
		if (ret < 0)
		{
			sprintf((char *)tmpBuf,"funSvrExitCode%d",abs(ret));
			if (UnionReadIntTypeRECVar((char *)tmpBuf) > 0)
				return(ret);
		}
		// end of addition of 2010-01-21,Wolfgang Wang
		
		// 从缓冲区读取信息
		if ((lenOfReq = UnionReadRequestToSpecifiedModuleWithMsgHeader(funSvrID,tmpBuf,sizeof(tmpBuf),&idOfSenderTask,&msgHeader)) <= 0)
		{
			UnionUserErrLog("in UnionStartFunSvr:: UnionReadRequestToSpecifiedModuleWithMsgHeader ret = [%d]!\n",lenOfReq);
			ret = lenOfReq;
			//break;
			continue;
		}

#ifndef _sharedHsmTaskLog_
		char			idOfApp[2+1];
		char                    dynamicLogLevelName[100];
                // 此时为应用设置一个专门的日志
                memset(idOfApp,0,sizeof(idOfApp));
                memcpy(idOfApp,tmpBuf,2);
                memset(dynamicLogLevelName,0,sizeof(dynamicLogLevelName));
                sprintf(dynamicLogLevelName,"logLevelOfApp%s",idOfApp);
                UnionDynamicSetLogLevel(UnionReadIntTypeRECVar(dynamicLogLevelName));
                UnionSetSuffixOfMyLogFile(idOfApp);
#endif

		if ((waitTime = UnionReadIntTypeRECVar(varName)) > 0)	// 只有在定义了本变量时才判断超时
		{
			// 判断是否超时消息
			time(&nowTime);
			if ((nowTime - msgHeader.time > waitTime) && (msgHeader.time > 0))
			{
				tmpBuf[lenOfReq] = 0;
				UnionUserErrLog("in UnionStartFunSvr:: outdate msg [%012ld] [%07d] [%04ld] [%04d] [%s]\n",
							idOfSenderTask,msgHeader.provider,nowTime - msgHeader.time,lenOfReq,tmpBuf);
				ret = 0;	// 2010-01-21,Wolfgang Wang added	
				continue;
			}
		}
		tmpBuf[lenOfReq] = 0;
		UnionDebugNullLog("req::[%04d][%s]\n",lenOfReq,tmpBuf);
		if ((lenOfRes = UnionKmsHsmTaskInterprotor(tmpBuf,lenOfReq,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionStartFunSvr:: Interprotor Error ret = [%d]!\n",lenOfRes);
			ret = lenOfRes;
			continue;
		}
		// 2010-7-13,王纯军结加
		if (lenOfRes == 0)
			continue;
		// 2010-7-13，王纯军增加结束
		tmpBuf[lenOfRes] = 0;
		UnionDebugNullLog("res::[%04d][%s]\n",lenOfRes,tmpBuf);		
		if ((ret = UnionSendResponseToApplyModuleWithOriMsgHeader(idOfSenderTask,tmpBuf,lenOfRes,&msgHeader)) < 0)
		{
			UnionUserErrLog("in UnionStartFunSvr:: UnionSendResponseToApplyModuleWithOriMsgHeader!\n");
			//break;
			continue;
		}
	}
	// return(ret);
}

char *UnionGetHsmIPAddrOfThisHsmTask()
{
	return("null");
}
