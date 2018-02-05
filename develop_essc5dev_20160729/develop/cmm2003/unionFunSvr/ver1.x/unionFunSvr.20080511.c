// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2005/12/30
// Version:	1.0

// 2008/3/25,������sig_pipe�ź���

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#ifndef _UnionLogMDL_3_x_   
#define _UnionLogMDL_3_x_   
#endif

#include "unionErrCode.h"
#include "UnionSocket.h"

#include "unionCommBetweenMDL.h"
#include "unionFunSvrName.h"
#include "unionFunSvr.h"
#include "unionREC.h"
#include "UnionLog.h"

int UnionStartFunSvr()
{
	int			ret;
	unsigned char		tmpBuf[81920];
	int			lenOfReq,lenOfRes;
	TUnionModuleID		idOfSenderTask;
	int			waitTime;
	time_t			nowTime;
	TUnionModuleID		funSvrID;
	char			varName[128];
	TUnionMessageHeader	msgHeader;
	
	// 2008/3/24, added by Wolfgang Wang
	signal(SIGPIPE,SIG_IGN);	// ���Ը��ź���
	// 2008/3/24, end of addition

	// 2009-12-5,����������
	if ((ret = UnionUserSpecFunForFunSvr()) < 0)
	{
		UnionUserErrLog("in UnionStartFunSvr:: UnionUserSpecFunForFunSvr ret = [%d]!\n",ret);
		return(ret);
	}
	// 2009-12-5,���������ӽ���
	
	UnionLog("in UnionStartFunSvr:: funSvr started OK! funSvrID = [%ld]\n",funSvrID = UnionGetFixedMDLIDOfFunSvr());
	snprintf(varName,sizeof(varName),"timeoutOf%s",UnionGetFunSvrName());

	ret = 0;	// 2010-01-21,Wolfgang Wang added
	for (;;)
	{
		//add by hzh in 2012.11.1 Ϊ����д��־����
		UnionSetFlushLogFileLinesMode(1);
		UnionFlushAllLogFileInGrpNoWait();
    //add end            
		// 2010-01-21,Wolfgang Wang added
		if (ret < 0)
		{
			sprintf((char *)tmpBuf,"funSvrExitCode%d",abs(ret));
			if (UnionReadIntTypeRECVar((char *)tmpBuf) > 0)
			{
				UnionFlushAllLogFileInGrpNoWait();  //add by hzh in 2012.11.1
				return(ret);
			}
		}
		// end of addition of 2010-01-21,Wolfgang Wang
		
		// �ӻ�������ȡ��Ϣ
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
                // ��ʱΪӦ������һ��ר�ŵ���־
                memcpy(idOfApp,tmpBuf,2);
		idOfApp[2] = 0;

                snprintf(dynamicLogLevelName,sizeof(dynamicLogLevelName),"logLevelOfApp%s",idOfApp);
                UnionDynamicSetLogLevel(UnionReadIntTypeRECVar(dynamicLogLevelName));
                UnionSetSuffixOfMyLogFile(idOfApp);
#endif

		if ((waitTime = UnionReadIntTypeRECVar(varName)) > 0)	// ֻ���ڶ����˱�����ʱ���жϳ�ʱ
		{
			// �ж��Ƿ�ʱ��Ϣ
			time(&nowTime);
			if ((nowTime - msgHeader.time > waitTime) && (msgHeader.time > 0))
			{
				tmpBuf[lenOfReq] = 0;
				UnionUserErrLog("in UnionStartFunSvr:: outdate msg [%012ld] [%07d] [%04ld] [%08d] [%s]\n",
							idOfSenderTask,msgHeader.provider,nowTime - msgHeader.time,lenOfReq,tmpBuf);
				ret = 0;	// 2010-01-21,Wolfgang Wang added	
				continue;
			}
		}
		tmpBuf[lenOfReq] = 0;
		UnionDebugNullLog("req::[%08d][%s]\n",lenOfReq,tmpBuf);
		if ((lenOfRes = UnionFunSvrInterprotor((char *)tmpBuf,lenOfReq,(char *)tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionStartFunSvr:: Interprotor Error ret = [%d]!\n",lenOfRes);
			ret = lenOfRes;
			/** add by xusj 20100226 begin **/
			if (ret == errCodeSJL06MDL_SJL06StillNotOnline)
				break;
			/** add by xusj 20100226 end **/
			continue;
		}
		// 2010-7-13,���������
		if (lenOfRes == 0)
			continue;
		// 2010-7-13�����������ӽ���
		tmpBuf[lenOfRes] = 0;
		UnionDebugNullLog("res::[%08d][%s]\n",lenOfRes,tmpBuf);		
		if ((ret = UnionSendResponseToApplyModuleWithOriMsgHeader(idOfSenderTask,tmpBuf,lenOfRes,&msgHeader)) < 0)
		{
			UnionUserErrLog("in UnionStartFunSvr:: UnionSendResponseToApplyModuleWithOriMsgHeader!\n");
			//break;
			continue;
		}
	}
	return(ret);
}
