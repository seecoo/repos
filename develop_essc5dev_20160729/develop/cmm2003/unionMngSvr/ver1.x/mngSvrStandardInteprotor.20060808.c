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
#include "simuMngSvrLocally.h"
#include "mngSvrOnlineSSN.h"
#include "unionCommConf.h"
#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif
#include "UnionTask.h"
#include "commWithTransSpier.h"
#include "UnionLog.h"

jmp_buf	gunionTCPSvrJmpEnvForMngClient;
jmp_buf	gunionTCPSvrJmpEnvForMaxIdleTime;
void UnionDealTCPSvrJmpEnvForMngClient();
void UnionDealTCPSvrJmpEnvForMaxIdleTime();

extern PUnionCommConf UnionFindClientCommConfWithType(char *ipAddr,int port,TUnionCommConnType connType);
extern PUnionTaskInstance               pgthisInstance;

PUnionCommConf	pgunionCommConf = NULL;

extern PUnionTaskInstance	ptaskInstance;
int	gsynchTCPIPSvrSckHDL = -1;

int UnionFreeUserSpecResource()
{
	UnionSetCommConfAbnormal(pgunionCommConf);	
	if (gsynchTCPIPSvrSckHDL >= 0)
		UnionCloseSocket(gsynchTCPIPSvrSckHDL);
	UnionCloseDatabase();
	return(0);
}


int UnionManagementTaskInterprotor(int handle)
{
	char				resID[3+1];
	char				resCmd[3+1];
	char				tellerNo[40+1];
	int				ret;
	char				data[4096*4+1];
	int				fileRecved = 0;
	int				lenOfData;
	char				*tmpFileName;
	char				cliIPAddr[15+1];
	char				tmpBuf[256];
	char				dataFileName[256];
	char				cmdBuf[300];

	int				batchNo;
	char				batchNoStr[128];

	
	// add by xusj begin 20091214
	UnionSetMyTransInfoResID(conResIDMngService);
	// add by xusj end

	// 读资源号
	memset(resID,0,sizeof(resID));
	if ((ret = UnionReadResMngRequestPackageFld(conMngSvrPackFldNameResID,resID,sizeof(resID))) < 0)
	{
		UnionUserErrLog("in UnionManagementTaskInterprotor:: UnionReadResMngRequestPackageFld [%s]\n",conMngSvrPackFldNameResID);
		return(ret);
	}
	UnionSetCurrentResID(atoi(resID));
	
	// 读资源命令
	memset(resCmd,0,sizeof(resCmd));
	if ((ret = UnionReadResMngRequestPackageFld(conMngSvrPackFldNameResCmd,resCmd,sizeof(resCmd))) < 0)
	{
		UnionUserErrLog("in UnionManagementTaskInterprotor:: UnionReadResMngRequestPackageFld [%s]\n",conMngSvrPackFldNameResCmd);
		return(ret);
	}
	UnionSetCurrentServiceID(atoi(resCmd));

	// 读柜员号
	memset(tellerNo,0,sizeof(tellerNo));
	if ((ret = UnionReadResMngRequestPackageFld(conMngSvrPackFldNameTellerNo,tellerNo,sizeof(tellerNo))) < 0)
	{
		UnionUserErrLog("in UnionManagementTaskInterprotor:: UnionReadResMngRequestPackageFld [%s]\n",conMngSvrPackFldNameTellerNo);
		return(ret);
	}
	// 读数据
	memset(data,0,sizeof(data));
	if ((lenOfData = UnionReadResMngRequestPackageFld(conMngSvrPackFldNameData,data,sizeof(data))) < 0)
	{
		UnionUserErrLog("in UnionManagementTaskInterprotor:: UnionReadResMngRequestPackageFld [%s]\n",conMngSvrPackFldNameData);
		return(lenOfData);
	}

	// 取客户端IP
	if ( UnionReadRecFldFromRecStr(data,lenOfData,"cliIpAddr",cliIPAddr,sizeof(cliIPAddr)) > 0)
		UnionSetResMngClientIPAddr(cliIPAddr);

	// add by xusj 20100607 begin 
	UnionSetCurrentMngSvrClientReqStr(data,lenOfData);
	// add by xusj 20100607 end 

	// 登记请求流水
	UnionRegisterMngSvrRequestService(atoi(resID),atoi(resCmd),tellerNo,lenOfData,data);
	
	if (!UnionIsBackuperMngSvr())
	{
		// add by xusj begin 20090824, 判断合法客户端
		if (!UnionIsNonAuthorizationMngSvrService(atoi(resID),atoi(resCmd)))
		{
			ret = UnionVerifyLegitimateClient(tellerNo);
			if (ret < 0)
			{
				UnionUserErrLog("in UnionManagementTaskInterprotor:: UnionVerifyLegitimateClient [%s]\n",tellerNo);
				return(ret);
			}
		}
		UnionDeleteRecFldFromRecStr(data, "cliIpAddr");
		lenOfData = strlen(data);
		// add by xusj end 20090824, 判断合法客户端
	}

	// 判断是否登录
#ifndef _runningAsBackSvr_	// 2009/5/30，王纯军增加该开关，以控制作为后台进程时，不进行操作员登录检查
	if (!((atoi(resCmd) == conResSpecCmdOfOperatorTBL_logon) && (atoi(resID) == conResIDOperatorTBL)) && !UnionIsNonAuthorizationMngSvrService(atoi(resID),atoi(resCmd)))
	{
		if ((ret = UnionIsTellerStillLogon(tellerNo)) < 0)
		{
			UnionUserErrLog("in UnionManagementTaskInterprotor:: UnionIsTellerStillLogon [%s]\n",tellerNo);
			if (ret == 0)
				ret = errCodeOperatorMDL_NotLogon;
			lenOfData = 0;
			goto exitNow;
		}
	}	
#endif	

	// 执行请求
	//UnionLog("in UnionManagementTaskInterprotor:: excuting this request...\n");
	//data[lenOfData] = 0;
	//UnionLog("in UnionManagementTaskInterprotor:: resID = [%d] cmdID = [%d] lenOfData = [%04d] data = [%s]\n",atoi(resID),atoi(resCmd),lenOfData,data);
	// 2010-6-9，王纯军增加
	if (UnionIsNonMngSvr())	
	{
		UnionSetCurrentMngSvrClientReqStr(data,lenOfData);
	}
	// 2010-6-9，王纯军增加结束
	// modified by 2012-10-18
	/*
	if ((lenOfData = UnionReadMngSvrDataLocally(handle,tellerNo,atoi(resID),atoi(resCmd),data,lenOfData,data,sizeof(data),&fileRecved)) < 0)
	{
		UnionUserErrLog("in UnionManagementTaskInterprotor:: UnionReadMngSvrDataLocally!\n");
		ret = lenOfData;
		goto exitNow;
	}
	*/

	// added 2012-09-12
	// 查询RSA动态批次号的记录 批次号:1NNNNYYDDD, NNNN为批次号,YY为年份,DDD为一年中的第几天
	if(atoi(resID) == 919 && atoi(resCmd) == 51)
	{
		memset(batchNoStr, 0, sizeof(batchNoStr));
		if ((ret = UnionReadRecFldFromRecStr(data, lenOfData, "batchNo", batchNoStr, sizeof(batchNoStr))) >= 0)
        	{
			batchNo = atoi(batchNoStr);
			memset(batchNoStr, 0, sizeof(batchNoStr));
			if(UnionFormRsaBatchNoCondition(batchNo, batchNoStr))
			{
				UnionDeleteRecFldFromRecStr(data, "batchNo");
				sprintf(data + strlen(data), "SQL::%s", batchNoStr);
				lenOfData = strlen(data);
			}
        	}
	}
	// end of addition 2012-09-12


	if((ret = UnionReadRecFldFromRecStr(data, lenOfData, "isUnionBatchService", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		if ((lenOfData = UnionReadMngSvrDataLocally(handle,tellerNo,atoi(resID),atoi(resCmd),data,lenOfData,data,sizeof(data),&fileRecved)) < 0)
		{
			UnionUserErrLog("in UnionManagementTaskInterprotor:: UnionReadMngSvrDataLocally! ret = %d\n", lenOfData);
			ret = lenOfData;
			goto exitNow;
		}
	}
	else
	{
		sprintf(tmpBuf, "%s/batchService.%s-%s.%d.tmp", getenv("UNIONTEMP"), resID, resCmd, getpid());
		// 接收上传文件	
		if ((ret = UnionMngSvrRecvFileFromClient(handle, tmpBuf)) < 0)
		{		
			UnionUserErrLog("in UnionManagementTaskInterprotor:: UnionMngSvrRecvFileFromClient [%s] ret = %d!\n", tmpBuf, ret);
			return(ret);
		}
		
		if ((ret = UnionReadMngSvrDataLocallyByBatchFile(handle,tellerNo,atoi(resID),atoi(resCmd), tmpBuf,data,sizeof(data),&fileRecved)) < 0)
		{
			UnionUserErrLog("in UnionManagementTaskInterprotor:: UnionReadMngSvrDataLocallyByBatchFile ret = %d!\n", lenOfData);
			goto exitNow;
		}
		sprintf(cmdBuf, "rm -f %s", tmpBuf);
		system(cmdBuf);
	}
	// end of modification 2012-10-18

	//UnionLog("in UnionManagementTaskInterprotor:: fileRecved = [%d]\n",fileRecved);
	// 有文件接收到，下载文件到客户端
	if (fileRecved)
	{
		UnionLog("in UnionManagementTaskInterprotor:: begin  tempFile Name = [%s]\n",UnionGetCurrentMngSvrTempFileName());
		if ((ret = UnionMngSvrTransferFile(handle,tmpFileName = UnionGetCurrentMngSvrTempFileName())) < 0)
		{
			UnionUserErrLog("in UnionManagementTaskInterprotor:: UnionMngSvrTransferFile [%s]!!\n",tmpFileName);
			UnionDeleteMngSvrTempFile();
			goto exitNow;
		}
		UnionLog("in UnionManagementTaskInterprotor:: end  tempFile Name = [%s]\n",UnionGetCurrentMngSvrTempFileName());
		UnionDeleteMngSvrTempFile();
	}
	//UnionNullLog("in UnionManagementTaskInterprotor:: res::[%04d][%s]\n",lenOfData,data);
	// 2012-10-24 张永定修改
	//if ((ret = UnionSetResMngResponsePackageFld(conMngSvrPackFldNameData,data,lenOfData)) < 0)
	if ((ret = UnionSetResMngResponsePackageFld(conMngSvrPackFldNameData,data,strlen(data))) < 0)
	{
		UnionUserErrLog("in UnionManagementTaskInterprotor:: UnionSetResMngResponsePackageFld [%s]\n",conMngSvrPackFldNameData);
		goto exitNow;
	}
	ret = lenOfData;
exitNow:
	// 登记响应流水
	//UnionRegisterMngSvrResponseService(atoi(resID), atoi(resCmd), ret,lenOfData,data,fileRecved);	
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
	int 		maxIdleTimeOfSckConn = 0;   //超过这个时间无客户端数据接收断开当前连接 
	PUnionLogFile           poldLogFile = NULL;
	
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
	// added 2014-09-06
	// 创建子任务后 强制加载一次任务日志
	UnionForceInitZlog();
	// end of addition 2014-09-06

	if ((pgunionCommConf = UnionAddServerCommConf(cliIPAddr,port,conCommShortConn,"配置管理")) == NULL)
	{
#ifdef _limitClient_
		UnionUserErrLog("in %s:: UnionAddServerCommConf [%s] [%d]\n",taskName,cliIPAddr,port);
		return(errCodeYLCommConfMDL_AddCommConf);
#endif
	}
	UnionSetCommConfOK(pgunionCommConf);
	UnionSetClientAttrToldTransSpier(cliIPAddr,port);
	
	gsynchTCPIPSvrSckHDL=handle;
	for (;;)
	{
		// 从客户端接收数据
		UnionDebugLog("in UnionSynchTCPIPTaskServer:: waiting for request ...\n");
		
		//add by hzh 2012.10.24 
		if ((maxIdleTimeOfSckConn = UnionReadIntTypeRECVar("maxIdleTimeOfSckConn")) < 0)
			maxIdleTimeOfSckConn = 10;
#ifdef _LINUX_
		if (sigsetjmp(gunionTCPSvrJmpEnvForMaxIdleTime,1) != 0)	// 超时退出
#else
		if (setjmp(gunionTCPSvrJmpEnvForMaxIdleTime) != 0)	// 超时退出
#endif
		{
			alarm(0);
			UnionAuditLog("in UnionSynchTCPIPTaskServer:: warning:: this connection too idle! close it now!\n");
			ret = errCodeEsscMDL_TCPIPTaskTooIdle;
			return ret;
		}
		alarm(maxIdleTimeOfSckConn);
		signal(SIGALRM,UnionDealTCPSvrJmpEnvForMaxIdleTime);
		//add end
		
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
		alarm(0);
		
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
		//UnionSendInfoToTransSpier(len,tmpBuf);
		//modify by hzh in 2012.2.22, 防止释放共享内存连接后，pgunionCommConf地址未置NULL的情况
		if ( (pgunionCommConf = UnionFindClientCommConfWithType(cliIPAddr,port,conCommShortConn)) != NULL)
			UnionIncreaseCommConfWorkingTimes(pgunionCommConf);
		//modify end 
		
		UnionAuditNullLog("\n**** Transcation Begin****\n");
		UnionSendRequestInfoToTransSpierAlways(len,tmpBuf);
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
		//sprintf(responseCode,"%06ld",ret % 1000000);
		sprintf(responseCode,"%06d",ret % 1000000);
		UnionSetResMngResponsePackageFld(conMngSvrPackFldNameResponseCode,responseCode,6);
		goto pack;
setResponse:
		//sprintf(responseCode,"%06ld",ret % 1000000);
		sprintf(responseCode,"%06d",ret % 1000000);
		UnionLog("in UnionSynchTCPIPTaskServer:: ret[%d] responseCode[%s]!\n",ret,responseCode);
		UnionSetResMngResponsePackageFld(conMngSvrPackFldNameResponseCode,responseCode,6);
		// 转换错误码为说明
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UnionTranslateErrCodeIntoRemark(ret,(char *)tmpBuf,sizeof(tmpBuf))) > 0)
		{
			UnionSetResMngResponsePackageFld(conMngSvrPackFldNameData,(char *)tmpBuf,len);
			//UnionSendInfoToErrorSpier(len,(char *)tmpBuf);
		}
		goto pack;
pack:
		// 打包响应
		if ((ret = UnionPackResMngResponsePackage((char *)tmpBuf+2,sizeof(tmpBuf)-1-2)) < 0)
		{
			UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionPackResMngResponsePackage!");
			break;
		}
		//UnionSendInfoToTransSpier(ret,tmpBuf+2);
		//UnionIncreaseCommConfWorkingTimes(pgunionCommConf);
		//modify by hzh in 2012.2.22, 防止释放共享内存连接后，pgunionCommConf地址未置NULL的情况
		if ( (pgunionCommConf = UnionFindClientCommConfWithType(cliIPAddr,port,conCommShortConn)) != NULL)
			UnionIncreaseCommConfWorkingTimes(pgunionCommConf);
		//modify end 
		
		tmpBuf[0] = ret / 256;
		tmpBuf[1] = ret % 256;
		if ((ret = UnionSendToSocket(handle,tmpBuf,ret+2)) < 0)
		{
			UnionUserErrLog("in UnionSynchTCPIPTaskServer:: UnionSendToSocket!");
			break;
		}
		tmpBuf[ret+2] = 0;
		UnionSendResponseInfoToTransSpier(ret,tmpBuf+2);
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

void UnionDealTCPSvrJmpEnvForMaxIdleTime()
{
	UnionAuditLog("warning:: gunionTCPSvrJmpEnvForEsscClient:: timeout!\n");
#ifdef _LINUX_
	siglongjmp(gunionTCPSvrJmpEnvForMaxIdleTime,10);
#else
	longjmp(gunionTCPSvrJmpEnvForMaxIdleTime,10);
#endif
}
int UnionReadMngSvrDataLocallyByBatchFile(int handle,char *tellerNo,int resID,int serviceID, char *dataFileName, char *resStr,int sizeOfResStr,int *fileRecved)
{
	int				ret;
	int				lenOfRecStr;
	int				num;
	char				*pFileName = NULL;
	char				recStr[1024];
	FILE				*dataFp = NULL, *resultFp = NULL;

	pFileName = UnionGenerateMngSvrTempFile();
	if((dataFp = fopen(dataFileName, "r")) == NULL)	
	{
		UnionUserErrLog("in UnionReadMngSvrDataLocallyByBatchFile, open file: [%s] failed.\n", dataFileName);
		return errCodeUseOSErrCode;
	}
	if((resultFp = fopen(pFileName, "w+")) == NULL)	
	{
		fclose(dataFp);
		UnionUserErrLog("in UnionReadMngSvrDataLocallyByBatchFile open file: [%s] failed.\n", pFileName);
		return errCodeUseOSErrCode;
	}
	num = 0;
	while (!feof(dataFp))
	{
		memset(recStr, 0, sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneLineFromTxtStr(dataFp, recStr, sizeof(recStr))) < 0)
		{
			continue;
		}
		num++;
		if(num == 1)
		{
			if(strstr(recStr, "[UnionBatchFile]") != recStr)
			{
				ret = errCodeEnviMDL_NotRecFormatDefStr;
				UnionUserErrLog("in UnionReadMngSvrDataLocallyByBatchFile:: upload file format[UnionBatchFile] Err ret = [%d]\n", ret);
				fclose(dataFp);
				fclose(resultFp);
				return(ret);
			}
			continue;
		}
		if ((ret = UnionReadMngSvrDataLocally(handle, tellerNo, resID, serviceID, recStr, lenOfRecStr, resStr,sizeOfResStr, fileRecved)) < 0)
		{
			UnionUserErrLog("in UnionReadMngSvrDataLocallyByBatchFile:: UnionReadMngSvrDataLocally reqStr = [%s] ret = %d\n", recStr, ret);
			sprintf(recStr + lenOfRecStr, "|remark=error: errCode=%d|\n", ret);
		}
		else
		{
			sprintf(recStr + lenOfRecStr, "|remark=ok!|\n");
		}
		fprintf(resultFp, recStr);
	}
	if(*fileRecved == 1)
	{
		UnionLog("in UnionReadMngSvrDataLocallyByBatchFile:: WARNING skiping single service[%d-%d] recv file!!\n", resID, serviceID);
	}

	fclose(dataFp);
	fclose(resultFp);
	*fileRecved = 1;
	return(num);
}

