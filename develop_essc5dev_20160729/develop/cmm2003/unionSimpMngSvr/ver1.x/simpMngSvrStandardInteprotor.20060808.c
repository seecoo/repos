// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/08/08
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "unionResID.h"
#include "mngSvrServicePackage.h"
#include "mngSvrCommProtocol.h"
#include "unionErrCode.h"
#include "mngSvrOnlineSSN.h"
#include "unionCommConf.h"
#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif
#include "UnionTask.h"
#include "UnionLog.h"

jmp_buf	gunionTCPSvrJmpEnvForMngClient;

void UnionDealTCPSvrJmpEnvForMngClient();

PUnionCommConf	pgunionCommConf = NULL;

extern PUnionTaskInstance	ptaskInstance;
int	gsynchTCPIPSvrSckHDL = -1;

char *UnionGetHsmIPAddrOfThisHsmTask()
{
	return (NULL);
}

int UnionReadMngSvrDataLocally(int handle, int resID, int serviceID, char *reqStr, int lenOfReqStr, char *resStr, int sizeOfResStr, int *fileRecved)
{
	int		ret;

	// 判断是否是文件处理命令
        ret = UnionExcuteMngSvrFileSvrService(handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved);
        if (ret >= 0)
                return(ret);
        if (ret != errCodeFileSvrMDL_InvalidCmd)
        {
                UnionUserErrLog("in UnionManagementTaskInterprotor:: UnionExcuteMngSvrFileSvrService ret = [%d]\n",ret);
                return(ret);
        }

	// 执行客户化命令
	if ((ret = UnionExcuteUserSpecResSvrService(handle,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved)) < 0)
	{
		UnionUserErrLog("in UnionReadMngSvrDataLocally:: UnionExcuteUserSpecResSvrService!\n");
		return(ret);
	}

	return ret;
}

int UnionManagementTaskInterprotor(int handle)
{
	char				resID[3+1];
	char				resCmd[3+1];
	int				ret;
	char				data[4096*4+1];
	int				lenOfData;
	char				*tmpFileName;
	int				fileRecved=0;

	// 读资源号
        memset(resID,0,sizeof(resID));
        if ((ret = UnionReadResMngRequestPackageFld(conMngSvrPackFldNameResID,resID,sizeof(resID))) < 0)
        {
                UnionUserErrLog("in UnionManagementTaskInterprotor:: UnionReadResMngRequestPackageFld [%s]\n",conMngSvrPackFldNameResID);
                return(ret);
        }

	// 读资源命令
	memset(resCmd,0,sizeof(resCmd));
	if ((ret = UnionReadResMngRequestPackageFld(conMngSvrPackFldNameResCmd,resCmd,sizeof(resCmd))) < 0)
	{
		UnionUserErrLog("in UnionManagementTaskInterprotor:: UnionReadResMngRequestPackageFld [%s]\n",conMngSvrPackFldNameResCmd);
		return(ret);
	}

	// 读数据
	memset(data,0,sizeof(data));
	if ((lenOfData = UnionReadResMngRequestPackageFld(conMngSvrPackFldNameData,data,sizeof(data))) < 0)
	{
		UnionUserErrLog("in UnionManagementTaskInterprotor:: UnionReadResMngRequestPackageFld [%s]\n",conMngSvrPackFldNameData);
		return(lenOfData);
	}

	// 执行请求
	if ((lenOfData = UnionReadMngSvrDataLocally(handle, atoi(resID), atoi(resCmd), data, lenOfData, data, sizeof(data), &fileRecved)) < 0)
	{
		UnionUserErrLog("in UnionManagementTaskInterprotor:: UnionReadMngSvrDataLocally!\n");
		ret = lenOfData;
		goto exitNow;
	}
	
      // 有文件接收到，下载文件到客户端
        if (fileRecved)
        {
                if ((ret = UnionMngSvrTransferFile(handle,tmpFileName = UnionGetCurrentMngSvrTempFileName())) < 0)
                {
                        UnionUserErrLog("in UnionManagementTaskInterprotor:: UnionMngSvrTransferFile [%s]!!\n",tmpFileName);
                        UnionDeleteMngSvrTempFile();
                        goto exitNow;
                }
                UnionDeleteMngSvrTempFile();
        }

	if ((ret = UnionSetResMngResponsePackageFld(conMngSvrPackFldNameData,data,lenOfData)) < 0)
	{
		UnionUserErrLog("in UnionManagementTaskInterprotor:: UnionSetResMngResponsePackageFld [%s]\n",conMngSvrPackFldNameData);
		goto exitNow;
	}
	ret = lenOfData;
exitNow:
	return(ret);
}

int UnionSynchTCPIPTaskServer(int handle,struct sockaddr_in *cli_addr,int port,int (*UnionTaskActionBeforeExit)())
{
	unsigned char		tmpBuf[8192*2+1];
	int			len;
	int			ret;
	char			cliIPAddr[40];
	char			taskName[100];
	char			responseCode[6+1];
	
	memset(cliIPAddr,0,sizeof(cliIPAddr));
	inet_ntop(AF_INET, (void *)&cli_addr->sin_addr, cliIPAddr, sizeof(cliIPAddr));
	len = sprintf(taskName,"%s %d",cliIPAddr,port);
	taskName[len] = 0;
	UnionDebugLog("in UnionSynchTCPIPTaskServer:: clientIPAddr [%s] Port [%d] Connected!\n",cliIPAddr,port);
	
	// 设置客户端的IP地址和端口号
	UnionSetResMngClientIPAddr(cliIPAddr);
	UnionSetResMngClientPort(port);
	
	if (UnionExistsTaskClassOfName(taskName))
	{
		if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,taskName)) == NULL)
		{
			UnionUserErrLog("in %s:: UnionCreateTaskInstance!\n",taskName);
			return(errCodeCreateTaskInstance);
		}
	}
	else
	{
		if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"mngClient")) == NULL)
		{
			UnionUserErrLog("in %s:: UnionCreateTaskInstance!\n",taskName);
			return(errCodeCreateTaskInstance);
		}
	}
	if ((pgunionCommConf = UnionAddServerCommConf(cliIPAddr,port,conCommShortConn,"配置管理")) == NULL)
	{
#ifdef _limitClient_
		UnionUserErrLog("in %s:: UnionAddServerCommConf [%s] [%d]\n",taskName,cliIPAddr,port);
		return(errCodeYLCommConfMDL_AddCommConf);
#endif
	}
	UnionSetCommConfOK(pgunionCommConf);
	
	gsynchTCPIPSvrSckHDL=handle;
	for (;;)
	{
		// 从客户端接收数据
		UnionDebugLog("in UnionSynchTCPIPTaskServer:: waiting for request ...\n");
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReceiveFromSocketUntilLen(handle,tmpBuf,2)) < 0)
		{
			UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionReceiveFromSocketUntilLen!\n");
			break;
		}
		if (ret == 0)	
			break;
		if ((len = tmpBuf[0] * 256 + tmpBuf[1]) == 0)
		{
			UnionUserErrLog("in UnionSynchTCPIPTaskServer:: client data len = [%d]\n",len);
			ret = errCodeEsscMDL_ClientDataLenIsZero;
			break;
		}
		if (len >= sizeof(tmpBuf) - 1)
		{
			UnionUserErrLog("in UnionSynchTCPIPTaskServer:: len = [%d] too long!\n",len);
			ret = errCodeEsscMDL_ClientDataLenTooLong;
			break;
		}
		// 设置接收客户端数据的超时机制
#ifdef _LINUX_
		if (sigsetjmp(gunionTCPSvrJmpEnvForMngClient,1) != 0)	// 超时退出
#else
		if (setjmp(gunionTCPSvrJmpEnvForMngClient) != 0)	// 超时退出
#endif
		{
			ret = errCodeEsscMDL_TimeoutForReceivingData;
			break;
		}
		alarm(5);
		signal(SIGALRM,UnionDealTCPSvrJmpEnvForMngClient);
		if ((ret = UnionReceiveFromSocketUntilLen(handle,tmpBuf,len)) < 0)
		{
			UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionReceiveFromSocketUntilLen expected [%d] real = [%d]!\n",len,ret);
			break;
		}
		alarm(0);
		if (ret == 0)
		{
			UnionDebugLog("in UnionSynchTCPIPTaskServer:: Connection Closed by Client!\n");
			break;
		}
		//tmpBuf[len] = 0;
		//UnionDebugLog("in UnionSynchTCPIPTaskServer:: len = [%d] [%s]\n",len,tmpBuf);
		UnionIncreaseCommConfWorkingTimes(pgunionCommConf);
		UnionAuditNullLog("\n**** Transcation Begin****\n");
		// 解包请求
		if ((ret = UnionUnpackResMngRequestPackage((char *)tmpBuf,len)) < 0)
		{
			UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionUnpackResMngRequestPackage\n");
			goto setResponse;
		}
		// 执行请求
		if ((ret = UnionManagementTaskInterprotor(handle)) < 0)
		{
			UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionManagementTaskInterprotor!\n");
			goto setResponse;
		}
		// modified by 2012-04-28 ld => d
		//sprintf(responseCode,"%06ld",ret % 1000000);
		sprintf(responseCode,"%06d",ret % 1000000);
		UnionSetResMngResponsePackageFld(conMngSvrPackFldNameResponseCode,responseCode,6);
		goto pack;
setResponse:
		//sprintf(responseCode,"%06ld",ret % 1000000); modified by 2012-04-28 ld => d
		sprintf(responseCode,"%06d",ret % 1000000);
		UnionSetResMngResponsePackageFld(conMngSvrPackFldNameResponseCode,responseCode,6);
		// 转换错误码为说明
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UnionTranslateErrCodeIntoRemark(ret,(char *)tmpBuf,sizeof(tmpBuf))) > 0)
		{
			UnionSetResMngResponsePackageFld(conMngSvrPackFldNameData,(char *)tmpBuf,len);
		}
		goto pack;
pack:
		// 打包响应
		if ((ret = UnionPackResMngResponsePackage((char *)tmpBuf+2,sizeof(tmpBuf)-1-2)) < 0)
		{
			UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionPackResMngResponsePackage!");
			break;
		}
		UnionIncreaseCommConfWorkingTimes(pgunionCommConf);
		tmpBuf[0] = ret / 256;
		tmpBuf[1] = ret % 256;
		if ((ret = UnionSendToSocket(handle,tmpBuf,ret+2)) < 0)
		{
			UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionSendToSocket!");
			break;
		}
		tmpBuf[ret+2] = 0;
		//UnionNullLog("res::[%04d][%s]\n",ret,tmpBuf+2);
		UnionAuditNullLog("**** Transcation End ****\n\n");
	}
	alarm(0);
	return(ret);
}

void UnionDealTCPSvrJmpEnvForMngClient()
{
	UnionUserErrLog("gunionTCPSvrJmpEnvForMngClient:: timeout!\n");
#ifdef _LINUX_
	siglongjmp(gunionTCPSvrJmpEnvForMngClient,10);
#else
	longjmp(gunionTCPSvrJmpEnvForMngClient,10);
#endif
}

int UnionFreeUserSpecResource()
{
	return 0;
}

int UnionExcuteDBSvrOperation(char *resName,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	return 0;
}

