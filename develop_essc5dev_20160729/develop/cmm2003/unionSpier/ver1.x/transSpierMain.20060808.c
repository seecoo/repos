// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2005/12/30
// Version:	1.0

#include <stdio.h>
#include <string.h>

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "unionMDLID.h"
#include "UnionTask.h"
#include "unionVersion.h"
#include "unionResID.h"
#include "transSpierBuf.h"
#include "unionServiceErrCounter.h"
#include "errCodeTranslater.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "mngSvrServicePackage.h"
#include "unionCommand.h"
#include "unionREC.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTransSpierBufMDL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}


// 安全服务
int UnionDealSecuSvrServiceMonitorPack(int resID,char *header,int headerLen,char *data,int dataLen)
{
	char				idOfApp[2+1],serviceCode[3+1],errCodeStr[6+1];
	int				errCode;
	int				ret;
	int				flag;
	
	if (dataLen < 2 + 3 + 1)
	{
		UnionUserErrLog("in UnionDealSecuSvrServiceMonitorPack:: invalid data pack!\n");
		return(errCodeParameter);
	}
	if (data[2+3] != '0')	// 不是响应
	{
		data[dataLen] = 0;
		UnionAuditLog("in UnionDealSecuSvrServiceMonitorPack:: data pack not response pack! [%04d][%s][%04d][%s]\n",headerLen,header,dataLen,data);
		return(0);
	}
	UnionLog("in UnionDealSecuSvrServiceMonitorPack::[%04d][%s]\n", dataLen, data);
	
	memcpy(idOfApp,data,2);
	idOfApp[2] = 0;
	memcpy(serviceCode,data+2,3);
	serviceCode[3] = 0;
	memcpy(errCodeStr,data+2+3+1,6);
	errCodeStr[6] = 0;
	errCode = atoi(errCodeStr);

	// Add By HuangBaoxin, 2010-12-02
	if ((ret = UnionIncreaseSecuSvrStatusCounterByErrCode(idOfApp,serviceCode,errCode)) < 0)
	{
		UnionUserErrLog("in UnionDealSecuSvrServiceMonitorPack:: UnionIncreaseSecuSvrStatusCounterByErrCode!\n");	
	}
		
	//	Add By HuangBaoxin, 2009/11/09
	flag = UnionReadIntTypeRECVar("spierNormalFlagOfSecuSvr");
	if ((flag == 0) && (errCode == 0))
		return 0;
	else if ( (flag > 0) && (errCode >= 0) )
		return 0;
	//	End Add
	
	/*Add By LiangJie, 20110328*/
	char specServiceVar[32];
	sprintf(specServiceVar, "notSpierService%03d", atoi(serviceCode));
	if (UnionReadIntTypeRECVar(specServiceVar) == 1)
		return 0; 
	/*End Add*/
	
	if (((ret = UnionIncreaseServiceErrCode(idOfApp,serviceCode,errCode)) < 0) && (ret != errCodeErrCounterGrpIsFull))
	{
		UnionUserErrLog("in UnionDealSecuSvrServiceMonitorPack:: UnionIncreaseServiceErrCode!\n");
		return(ret);
	}
	UnionSendAllSecuSvrServiceCountToMonitor(resID);	// 发送错误码
	if (ret == errCodeErrCounterGrpIsFull) // 错误码表满了
	{
		if (((ret = UnionIncreaseServiceErrCode(idOfApp,serviceCode,errCode)) < 0) && (ret != errCodeErrCounterGrpIsFull))
		{
			UnionUserErrLog("in UnionDealSecuSvrServiceMonitorPack:: UnionIncreaseServiceErrCode!\n");
			return(ret);
		}
	}
	return(ret);
}		

int UnionDealKDBServiceMonitorPack(int resID,char *header,int headerLen,char *data,int dataLen)
{
	char	tmpBuf[1024*2+256+1];
	int	len;
	char	resCodeStr[256+1], *remark;
	int	responseCode = 0;
	int	ret;
	int	flag;
	
	if (headerLen >= sizeof(tmpBuf))
		return(errCodeParameter);

// Add By Huangbaoxin, 20091105	
	if ( (ret = UnionReadRecFldFromRecStr(data,dataLen,"remark",resCodeStr,sizeof(resCodeStr))) < 0 )
	{
		memset(resCodeStr, 0, sizeof(resCodeStr));
		if ( (ret = UnionReadRecFldFromRecStr(data,dataLen,"responseCode",resCodeStr,sizeof(resCodeStr))) < 0 )
		{
			UnionUserErrLog("in UnionDealKDBServiceMonitorPack::UnionReadRecFldFromRecStr[responseCode]!");
			UnionNullLog("[%04d][%s]\n",dataLen,data);
			return ret;
		}
		responseCode = atoi(resCodeStr);
		remark = UnionFindRemarkOfErrCode(atoi(resCodeStr));
		sprintf(data+dataLen, "remark=%s|", remark);
		dataLen += (8 + strlen(remark));
	}
// End Add
	
	// Add By Huangbaoxin, 2009/11/26, 置为未处理
	UnionUpdateSpecFldValueOnUnionRec(data, dataLen+1, "dealOK", "0");
	
	//	Add By HuangBaoxin, 2009/11/09
	flag = UnionReadIntTypeRECVar("spierNormalFlagOfKDBSvr");
	if ((flag == 0) && (responseCode == 0))
		return 0;
	else if ( (flag > 0) && (responseCode >= 0) )
		return 0;
	//	End Add
	
	memcpy(tmpBuf,header,headerLen);
	if (headerLen + dataLen >= sizeof(tmpBuf))
	{
		memcpy(tmpBuf+headerLen,data,sizeof(tmpBuf)-headerLen-1);
		memset(tmpBuf+sizeof(tmpBuf)-4,'.',3);
		len = sizeof(tmpBuf)-1;
	}
	else
	{
		memcpy(tmpBuf+headerLen,data,dataLen);
		len = headerLen + dataLen;
	}
	
	UnionNullLog("[%04d][%s]\n",len,tmpBuf);
	// 向监控服务器发送
	return(UnionSendTransInfoToMonitor(resID,tmpBuf,len));
}
	 
int UnionDealMngSvrServiceMonitorPack(int resID,char *header,int headerLen,char *data,int dataLen)
{
	char	tmpBuf[1024+256+1];
	int	len;
	int	isRequest;
	char	tellerNo[40+1];
	int	operResID,resCmd,resCode=0;
	int	lenOfData,offsetOfData;
	int	dealOK = 1;
	int	flag;
	
	if (headerLen >= sizeof(tmpBuf))
		return(errCodeParameter);
	memcpy(tmpBuf,header,headerLen);
	// 解包头
	if ((offsetOfData = UnionUnpackMngSvrPackageHeader(data,dataLen,&isRequest,tellerNo,&operResID,&resCmd,&resCode,&lenOfData)) < 0)
	{
		UnionUserErrLog("in UnionDealMngSvrServiceMonitorPack:: UnionUnpackMngSvrPackageHeader!\n");
		return(offsetOfData);
	}

	//	Add By HuangBaoxin, 2009/11/09
	flag = UnionReadIntTypeRECVar("spierNormalFlagOfMngSvr");
	if ((flag == 0) && (resCode == 0))
		return 0;
	else if ( (flag > 0) && (resCode >= 0) )
		return 0;
	//	End Add
	
	if (resCode < 0)
		dealOK = 0;

	// Add By Huangbaoxin,20091127
	dealOK=0;
		
	sprintf(tmpBuf+headerLen,"dealOK=%d|isRequest=%d|tellerNo=%s|operResID=%d|resCmd=%d|resCode=%d|lenOfData=%d|parameter=",
		dealOK, isRequest,tellerNo,operResID,resCmd,resCode,lenOfData);
	len = headerLen + strlen(tmpBuf+headerLen);
	// 转换配置报文正文
	if (lenOfData > dataLen - offsetOfData)
		lenOfData = dataLen - offsetOfData;
	UnionConvertOneFldSeperatorInRecStrIntoAnother(data+offsetOfData,lenOfData,'|','^',data+offsetOfData,lenOfData+1);
	if (len + lenOfData >= sizeof(tmpBuf)-1)
	{
		memcpy(tmpBuf+len,data+offsetOfData,sizeof(tmpBuf)-len-2);
		memset(tmpBuf+sizeof(tmpBuf)-5,'.',3);
		len = sizeof(tmpBuf)-2;
	}
	else
	{
		memcpy(tmpBuf+len,data+offsetOfData,lenOfData);
		len = len + lenOfData;
	}
	tmpBuf[len] = '|';
	len++;
	tmpBuf[len] = 0;
	UnionNullLog("[%04d][%s]\n",len,tmpBuf);
	// 向监控服务器发送
	return(UnionSendTransInfoToMonitor(resID,tmpBuf,len));
}

int UnionDealKmSvrServiceMonitorPack(int resID,char *header,int headerLen,char *data,int dataLen)
{
	char				idOfApp[2+1],serviceCode[3+1],errCodeStr[6+1];
	int				errCode;
	int				isRequest;
	char				tmpBuf[1024+1];
	int				dealOK = 1;
	char				*remark;
	int				len;
	int				flag;
	
	if (dataLen < 2 + 3 + 1)
	{
		UnionUserErrLog("in UnionDealKmSvrServiceMonitorPack:: invalid data pack!\n");
		return(errCodeParameter);
	}
	isRequest = data[2+3] - '0';
	memcpy(idOfApp,data,2);
	idOfApp[2] = 0;
	memcpy(serviceCode,data+2,3);
	serviceCode[3] = 0;
	if (!isRequest)
	{
		memcpy(errCodeStr,data+2+3+1,6);
		errCodeStr[6] = 0;
		errCode = atoi(errCodeStr);
		remark = UnionFindRemarkOfErrCode(errCode);
	}
	else
	{
		errCode = 0;
		remark = data;
	}

	//	Add By HuangBaoxin, 2009/11/09
	flag = UnionReadIntTypeRECVar("spierNormalFlagOfKDBSvr");
	if ((flag == 0) && (errCode == 0))
		return 0;
	else if ( (flag > 0) && (errCode >= 0) )
		return 0;
	//	End Add
	
	if (errCode < 0)
		dealOK = 0;
	
	// Add By Huangbaoxin,20091127
	dealOK=0;
		
	memcpy(tmpBuf,header,headerLen);
	sprintf(tmpBuf+headerLen,"dealOK=%d|responseCode=%d|operation=%d|fullKeyName=|remark=%s|",
			dealOK, errCode,atoi(serviceCode),remark);
	len = headerLen + strlen(tmpBuf+headerLen);
	tmpBuf[len] = 0;
	UnionNullLog("[%04d][%s]\n",len,tmpBuf);
	// 向监控服务器发送
	return(UnionSendTransInfoToMonitor(resID,tmpBuf,len));
}		

int UnionStartTransSpier(char *ipAddr)
{
	unsigned char			data[1024+1];
	int				dataLen;
	int				headerLen;
	int				resID;
	TUnionTransSpierMsgHeader	msgHeader;
	char				header[256+1];
	int				intervalTimeOfLastConnFail;
	
	intervalTimeOfLastConnFail = UnionReadIntTypeRECVar("intervalTimeOfLastConnFail");
	if (intervalTimeOfLastConnFail > 0)
		UnionSetIntervalTimeOfLastConnFail(intervalTimeOfLastConnFail);	

loop:
	// 获取要监控的信息
	memset(data,0,sizeof(data));
	memset(&msgHeader, 0, sizeof(msgHeader));
	
#ifdef _transSpier_2_x_
	if ((dataLen = UnionReadFirstTransSpierMsgWithMsgHeaderUntilSuccess(data,sizeof(data)-1,&resID,&msgHeader)) < 0)
	{
		UnionUserErrLog("in UnionStartTransSpier:: UnionReadMsgOfSpecifiedTypeUntilSuccess!\n");
		return(dataLen);
	}
#else
	if ((dataLen = UnionReadFirstTransSpierMsgUntilSuccess(data,sizeof(data)-1,&resID)) < 0)
	{
		UnionUserErrLog("in UnionStartTransSpier:: UnionReadFirstTransSpierMsgUntilSuccess!\n");
		return(dataLen);
	}
	msgHeader.len = dataLen;
	msgHeader.resID = resID;	
#endif
	
	// 拼装报文头
	memset(header,0,sizeof(header));
	if ((headerLen = UnionPutTransSpierMsgHeaderIntoStr(&msgHeader,header,sizeof(header))) < 0)
	{
		UnionUserErrLog("in UnionStartTransSpier:: UnionPutTransSpierMsgHeaderIntoStr!\n");
		goto loop;
	}
	switch (resID)
	{
		case	conResIDSecurityService:		//安全服务
			UnionDealSecuSvrServiceMonitorPack(resID,header,headerLen,(char *)data,dataLen);
			goto loop;
		case	conResIDKDBService:			//密钥服务(密钥客户端操作)
		case	conResIDKDBSynchronizerService:		//密钥同步服务
		case	conResIDKDBBackuper:			//密钥备份服务
		case	conResIDKDBSvr:				//密钥映像
			UnionDealKDBServiceMonitorPack(resID,header,headerLen,(char *)data,dataLen);
			goto loop;
		case	conResIDKeyMngService:			//密钥管理服务
#ifdef _transSpier_2_x_
			UnionDealKDBServiceMonitorPack(resID,header,headerLen,(char *)data,dataLen);
#else
			UnionDealKmSvrServiceMonitorPack(resID,header,headerLen,(char *)data,dataLen);
#endif
			goto loop;
		case	conResIDMngService:			//系统管理服务
			UnionDealMngSvrServiceMonitorPack(resID,header,headerLen,(char *)data,dataLen);
			goto loop;					
		default:
			// 不监控资源
			break;
	}
	// 不监控资源
	UnionNullLog("[%s][%s]\n",header,data);
	goto loop;
}

int UnionHelp()
{
	printf("Usage:: %s\n",UnionGetApplicationName());
	return(0);
}

int main(int argc,char *argv[])
{
	UnionSetApplicationName(argv[0]);
		
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);
	
	if (UnionCreateProcess() > 0)
		return(0);

	//UnionSetMyModuleType(conEsscMDLTypeTransSpier);		

	UnionSetLongConnwithMonitor();
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		UnionPrintf("in %s:: UnionCreateTaskInstance '%s' Error!\n",UnionGetApplicationName(),argv[1]);
		return(UnionTaskActionBeforeExit());
	}

	UnionStartTransSpier(argv[1]);
	
	return(UnionTaskActionBeforeExit());
}


