// Copyright:	Union Tech.
// Author:	zhangyd
// Date:	2010-10-23
// Version:	2.0
// 2016-05 modified version 3.0 支持信号中断中转回原点

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <signal.h>
#include <setjmp.h>

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#ifndef _UnionSocket_3_x_
#define _UnionSocket_3_x_
#endif

#include "UnionSocket.h"
#include "unionMDLID.h"
#include "unionCommBetweenMDL.h"
#include "UnionTask.h"
#include "unionCommConf.h"
#include "unionREC.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionVersion.h"
#include "unionRealBaseDB.h"
#include "unionCommand.h"
#include "unionXMLPackage.h"
#include "unionRealDBCommon.h"
#include "unionHsmCmdVersion.h"
#include "unionMsgBufGroup.h"
#include "clusterSyn.h"

static char		gunionHsmID[32];
jmp_buf	gunionHsmJmpEnv;
jmp_buf	gunionHsmTestJmpEnv;
int	UnionDealHsmTestingPackage();
int	UnionDealHsmTestingPackage2();
void	UnionDealHsmResponseTimeout();

PUnionCommConf		pgThisCommConf = NULL;
int			gunionHsmSckHDL = -1;
int			gunionHsmLastNormalSckHDL = -1;


typedef struct
{
	char		hsmGroupID[32];
	char		hsmGroupName[64];
	unsigned int	hsmCmdVersionID;
	unsigned int	lenOfCommHead;
	unsigned int	lenOfMsgHead;
	char		reqCmdOfTest[160];
	char		resCmdOfTest[160];
	char		sensitiveCmd[1024];
} TUnionHsmGroup;
typedef TUnionHsmGroup	*PUnionHsmGroup;

typedef struct
{
	char		ipAddr[64];
	unsigned int	port;
	char		hsmGroupID[32];
	unsigned int	uses;
	unsigned int	status;
} TUnionHsm;
typedef TUnionHsm	*PUnionHsm;

int UnionHelp()
{
	printf("Usage:: %s hsmID\n",UnionGetApplicationName());
	return(0);
}

int UnionTaskActionBeforeExit()
{
	// modify by chenqy 20151124
	if (pgThisCommConf && strlen(pgThisCommConf->ipAddr) != 0)
		//UnionSetCommConfAbnormal(pgThisCommConf);
		memset(pgThisCommConf, 0, sizeof(TUnionCommConf));
	// modify end
	
	if (gunionHsmSckHDL >= 0)
		UnionCloseSocket(gunionHsmSckHDL);

	//UnionCloseDatabase();
	UnionDisconnectCommConfTBL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}
void UnionPrintHsmReqLog(long mdlid, int provider, char *ip, int port, int printType, int len, char *str)
{
	char		*ptr = NULL;
	char		buf[819200*2];

	ptr = str;

	if((printType - '0') & 0x4)
	{
		memset(buf, '*', len);
		buf[len] = 0;
		ptr = buf;
		UnionLog("req[%d][%ld][%s:%d]:: [%2.2s][%04d][%s]\n", provider, mdlid, ip, port, str, len, ptr);
		return;
	}

	if((printType - '0') & 0x1)
	{
		bcdhex_to_aschex(str, len, buf);
		buf[len*2] = 0;
		ptr = buf;
		len *= 2;
	}

	UnionLog("req[%d][%ld][%s:%d]:: [%2.2s][%04d][%s]\n", provider, mdlid, ip, port, str, len, ptr);

	return;

}

void UnionPrintHsmResLog(long mdlid, int provider, char *ip, int port, int printType, int len, char *str)
{
	char		*ptr = NULL;
	char		buf[819200];

	ptr = str;

	if((printType - '0') & 0x8)
	{
		memset(buf, '*', len);
		buf[len] = 0;
		ptr = buf;
		UnionLog("res[%d][%ld][%s:%d]:: [%2.2s][%04d][%s]\n", provider, mdlid, ip, port, str, len, ptr);
		return;
	}

	if((printType - '0') & 0x2)
	{
		bcdhex_to_aschex(str, len, buf);
		ptr = buf;
		len *= 2;
		buf[len] = 0;
	}

	UnionLog("res[%d][%ld][%s:%d]:: [%2.2s][%04d][%s]\n", provider, mdlid, ip, port, str, len, ptr);

	return;

}

int UnionReadHsmRecFromHsm(char *hsmID,PUnionHsm pHsmRec,PUnionHsmGroup pHsmGroup)
{
	int	ret;
	char	sql[1024];
	char	tmpBuf[256];
	snprintf(sql,sizeof(sql) - 1,"select hsmName,hsm.hsmGroupID,ipAddr,port,uses,status,hsmGroupName,hsmCmdVersionID,lenOfCommHead,lenOfMsgHead,reqCmdOfTest,resCmdOfTest,sensitiveCmd from hsm,hsmGroup where hsm.hsmGroupID = hsmGroup.hsmGroupID and hsmGroup.enabled = 1 and hsm.enabled = 1 and hsmID='%s'",hsmID);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) <= 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	UnionLocateXMLPackage("detail", 1);
	
	// 读取密码机IP地址
	if ((ret = UnionReadXMLPackageValue("ipAddr", pHsmRec->ipAddr, sizeof(pHsmRec->ipAddr))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadXMLPackageValue[%s]!\n","ipAddr");
		return(UnionTaskActionBeforeExit());
	}
	// 读取密码机端口
	if ((ret = UnionReadXMLPackageValue("port", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadXMLPackageValue[%s]!\n","port");
		return(ret);
	}
	pHsmRec->port = atoi(tmpBuf);
	// 读取密码机组
	if ((ret = UnionReadXMLPackageValue("hsmGroupID", pHsmRec->hsmGroupID, sizeof(pHsmRec->hsmGroupID))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadXMLPackageValue[%s]!\n","hsmGroupID");
		return(ret);
	}
	strcpy(pHsmGroup->hsmGroupID,pHsmRec->hsmGroupID);
	// 读取密码机用途
	if ((ret = UnionReadXMLPackageValue("uses", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadXMLPackageValue[%s]!\n","uses");
		return(ret);
	}
	pHsmRec->uses = atoi(tmpBuf);
	// 读取密码机状态
	if ((ret = UnionReadXMLPackageValue("status", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadXMLPackageValue[%s]!\n","status");
		return(ret);
	}
	pHsmRec->status = atoi(tmpBuf);
	// 读取密码机组名
	if ((ret = UnionReadXMLPackageValue("hsmGroupName", pHsmGroup->hsmGroupName, sizeof(pHsmGroup->hsmGroupName))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadXMLPackageValue[%s]!\n","hsmGroupName");
		return(ret);
	}
	pHsmGroup->hsmGroupName[ret] = 0;
	// 读取密码机指令类型
	if ((ret = UnionReadXMLPackageValue("hsmCmdVersionID", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadXMLPackageValue[%s]!\n","hsmCmdVersionID");
		return(ret);
	}
	pHsmGroup->hsmCmdVersionID = atoi(tmpBuf);
	// 读取通讯头长度
	if ((ret = UnionReadXMLPackageValue("lenOfCommHead", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadXMLPackageValue[%s]!\n","lenOfCommHead");
		return(ret);
	}
	pHsmGroup->lenOfCommHead = atoi(tmpBuf);
	// 读取消息头长度
	if ((ret = UnionReadXMLPackageValue("lenOfMsgHead", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadXMLPackageValue[%s]!\n","lenOfMsgHead");
		return(ret);
	}
	pHsmGroup->lenOfMsgHead = atoi(tmpBuf);
	// 读取探测请求指令
	if ((ret = UnionReadXMLPackageValue("reqCmdOfTest", pHsmGroup->reqCmdOfTest, sizeof(pHsmGroup->reqCmdOfTest))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadXMLPackageValue[%s]!\n","reqCmdOfTest");
		return(ret);
	}
	pHsmGroup->reqCmdOfTest[ret] = 0;
	// 读取探测响应指令
	if ((ret = UnionReadXMLPackageValue("resCmdOfTest", pHsmGroup->resCmdOfTest, sizeof(pHsmGroup->resCmdOfTest))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadXMLPackageValue[%s]!\n","resCmdOfTest");
		return(ret);
	}
	pHsmGroup->resCmdOfTest[ret] = 0;
	// 读取敏感指令
	if ((ret = UnionReadXMLPackageValue("sensitiveCmd", pHsmGroup->sensitiveCmd, sizeof(pHsmGroup->sensitiveCmd))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadXMLPackageValue[%s]!\n","sensitiveCmd");
		return(ret);
	}
	pHsmGroup->sensitiveCmd[ret] = 0;
	return(1);
}

int UnionUpdateHsmStatus(char *hsmID,int status)
{
	int	ret;
	int	oldStatus;
	char	sql[256];
	
	if (status)
		oldStatus = 0;
	else
		oldStatus = 1;
	
	snprintf(sql,sizeof(sql),"update hsm set status = %d where hsmID = '%s' and status = %d",status,hsmID,oldStatus);
	if ((ret = UnionExecRealDBSql2(0,sql)) < 0)
	{
		UnionUserErrLog("in UnionUpdateHsmStatus:: UnionExecRealDBSql2[%s]\n",sql);
		return(ret);
	}
	return(ret);
}

int UnionSendHsmCmdRunningMsg(char *hsmGroupID,char *hsmIP,char *hsmCmd,char *responseCode)
{
	int			ret = 0;
	int			lenOfMsgBuf = 0;
	char			msgBuf[128];
	static int		hsmCmdSucceedNum = 0;
	static int		hsmCmdFailedNum = 0;
	TUnionMessageHeader     reqMsgHeader;

        if (UnionExistsTaskOfName("transMonSvr") <= 0 && UnionExistsTaskOfName("resSpier -HSMCMD") <= 0)
        {
                UnionLog("in UnionSendHsmCmdRunningMsg:: task transMonSvr or resSpierForTaskTbl not started!\n");
                return(0);
        }

	// add by leipp 20151125
	if (memcmp(hsmCmd,"NC",2) != 0)
	{
		if (((atoi(responseCode) == 2) && ((memcmp(hsmCmd,"8D",2) == 0) || (memcmp(hsmCmd,"8E",2) == 0) || (memcmp(hsmCmd,"SD",2) == 0)	\
			|| (memcmp(hsmCmd,"DE",2) == 0) || (memcmp(hsmCmd,"EE",2) == 0) || (memcmp(hsmCmd,"EA",2) == 0) || (memcmp(hsmCmd,"DF",2) == 0))) 	\
			|| (atoi(responseCode) == 0))
		{
			hsmCmdSucceedNum++;
		}
		else
		{
			hsmCmdFailedNum++;
		}

		if ((hsmCmdFailedNum + hsmCmdSucceedNum) < 1000)
			return 0;
	}

	if ((hsmCmdFailedNum + hsmCmdSucceedNum) == 0)
		return 0;
	// add by leipp end

        memset(&reqMsgHeader,0,sizeof(reqMsgHeader));
	
	UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfMon);
	//lenOfMsgBuf = snprintf(msgBuf,sizeof(msgBuf),"hsmGrpID=%s|hsmIpAddr=%s|cmdCode=%s|responseCode=%s",hsmGroupID,hsmIP,hsmCmd,responseCode);
	lenOfMsgBuf = snprintf(msgBuf,sizeof(msgBuf),"hsmGrpID=%s|hsmIpAddr=%s|totalNum=%d|succeedNum=%d|failedNum=%d",hsmGroupID,hsmIP,hsmCmdFailedNum+hsmCmdSucceedNum,hsmCmdSucceedNum,hsmCmdFailedNum);	
        if ((ret = UnionSendRequestToSpecifiedModuleWithNewMsgHeaderWithTime(conMDLTypeOffsetOfMonSvr + conMDLTypeUnionMon_HsmCmd,(unsigned char *)msgBuf,lenOfMsgBuf,&reqMsgHeader,1)) < 0)
        {
                UnionLog("in UnionSendHsmCmdRunningMsg:: UnionSendRequestToSpecifiedModuleWithNewMsgHeader!\n");
		hsmCmdFailedNum = 0;
		hsmCmdSucceedNum = 0;
                return(ret);
        }
	UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfHsmSvr);

	hsmCmdFailedNum = 0;
	hsmCmdSucceedNum = 0;
	return 0;
}
int main(int argc,char *argv[])
{
	int			ret;
	int			failConnect;
	int			maxFreeTime = 0;
	int			len = 0;
	int			len2 = 0;
	int			firstFlag = 1;
	int			offset = 0;
	int			msgOffset = 0;
	int			timeoutOfHsm;
	int			lenOfHsmCmdHead = 2;
	int			trySendFlag = 0;
	int			isConnectSpecHsm = 0;
	char			hsmID[32];
	char			hsmCmdHead[32];
	char			taskName[128];
	char			buf[1024];
	char			specIPAddr[64];
	char			hsmCmd[8];
	char			hsmCmdResCode[8];
	unsigned char		msg[819200];
	unsigned char		reqBuf[81920];
	unsigned char		resBuf[81920];
	unsigned char		lenBuf[32];
	TUnionModuleID		mdlidOfHsm = -1;
	TUnionModuleID		applierMDLID;
	TUnionMessageHeader	msgHeader;
	TUnionHsm		hsmRec;
	TUnionHsmGroup		hsmGroup;
	int                     packagePrintType = '0'; // '1':请求BCD格式打印，'2':响应BCD格式打印，'3':请求响应都用BCD格式打印
	int			hsmSckHDL = -1;
	PUnionTaskInstance	ptaskInstance = NULL;
	int			readHsmTryTimes	= 0;
	
	UnionSetApplicationName(argv[0]);

	if (argc < 2)
		return(UnionHelp());

	strcpy(hsmID,argv[1]);
	snprintf(gunionHsmID,sizeof(gunionHsmID),"%s",hsmID);
	
	//add by zhouxw 20150906
	if(UnionGetStatusOfHsmID(hsmID) != 1)
		return(0);
	//add end
	
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	if (UnionCreateProcess() > 0)
		return(0);

	ret = snprintf(taskName,sizeof(taskName)-1,"%s %s",UnionGetApplicationName(),hsmID);
	taskName[ret] = 0;
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",taskName)) == NULL)
	{
		printf("UnionCreateTaskInstance[%s] Error!\n",taskName);
		return(0);
	}
	
	UnionSetSuffixOfMyLogFile(hsmID);
	
againConnectHsm:
	readHsmTryTimes	= 10; // 读数据库的密码机如10分钟都不成功则程序退出(重启)
	do
	{
		if ((ret =  UnionReadHsmRecFromHsm(hsmID,&hsmRec,&hsmGroup)) > 0)
		{
			break;
		}
		else
		{
			UnionUserErrLog("in %s:: UnionReadHsmRecFromHsm[%s]!\n",UnionGetApplicationName(),hsmID);
			sleep(60);
			//return(UnionTaskActionBeforeExit());
		}
	}while(readHsmTryTimes-- > 0);
	if(ret <= 0)
	{
		UnionUserErrLog("in %s:: UnionReadHsmRecFromHsm[%s] exit!\n",UnionGetApplicationName(),hsmID);
		return(UnionTaskActionBeforeExit());
	}
	// 支持域名
        if (!UnionIsValidIPAddrStr(hsmRec.ipAddr))
        {
                if ((ret = UnionGetHostByName(hsmRec.ipAddr,specIPAddr)) < 0)
                {
                        UnionUserErrLog("in %s:: UnionGetHostByName[%s]!\n",UnionGetApplicationName(),hsmRec.ipAddr);
                        return(ret);
                }
                snprintf(hsmRec.ipAddr,sizeof(hsmRec.ipAddr),"%s",specIPAddr);
        }
	
	if ((pgThisCommConf = UnionAddClientCommConf(hsmRec.ipAddr,hsmRec.port,conCommLongConn,"密码机")) == NULL)
	{
		UnionUserErrLog("in %s:: UnionAddClientCommConf for [%s] [%d]\n",UnionGetApplicationName(),hsmRec.ipAddr,hsmRec.port);
		return(UnionTaskActionBeforeExit());
	}

	switch(hsmGroup.hsmCmdVersionID)
	{
		case conHsmCmdVerSJJ11270StandardHsmCmd:
		case conHsmCmdVerSJJ1127SStandardHsmCmd:
			lenOfHsmCmdHead = 4;
			break;
		case conHsmCmdVerSJL05StandardHsmCmd:
			lenOfHsmCmdHead = 0;
			break;
		default:
			lenOfHsmCmdHead = 2;
			break;
	}

	if ((mdlidOfHsm = UnionGetFixedMDLIDOfTaskOfHsmGrp(hsmRec.hsmGroupID)) <= 0)
	{
		UnionUserErrLog("in %s:: UnionGetFixedMDLIDOfTaskOfHsmGrp[%s]!\n",UnionGetApplicationName(),hsmRec.hsmGroupID);
		return(UnionTaskActionBeforeExit());
	}

	mdlidOfHsm += conMDLTypeOffsetOfHsmSvr;
	UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfHsmSvr); // added 2014-06-12
	
	UnionLog("in %s:: try to connect [%ld] [%s] [%d]!\n",UnionGetApplicationName(),mdlidOfHsm,hsmRec.ipAddr,hsmRec.port);
	
	for (failConnect = 0;;)
	{
		if (failConnect > 10)
		{
			//UnionPrintf("in %s:: UnionCreateSocketClient[%s][%d] error!\n",UnionGetApplicationName(),hsmRec.ipAddr,hsmRec.port);
			UnionUserErrLog("in %s:: UnionCreateSocketClient[%s][%d] error!\n",UnionGetApplicationName(),hsmRec.ipAddr,hsmRec.port);
			return(UnionTaskActionBeforeExit());
		}
		
		if ((hsmSckHDL = UnionCreateSocketClient(hsmRec.ipAddr,hsmRec.port)) < 0)
		{
			UnionUserErrLog("in %s:: UnionCreateSocketClient[%s][%d]!\n",UnionGetApplicationName(),hsmRec.ipAddr,hsmRec.port);
			UnionUpdateHsmStatus(hsmID,0);
			sleep(1);
			failConnect ++;
		}
		else
		{
			gunionHsmSckHDL = hsmSckHDL;
			break;
		}
	}
		
	UnionSetCommConfOK(pgThisCommConf);
	UnionSuccessLog("in %s:: connect [%ld] [%s] [%d] OK!\n",UnionGetApplicationName(),mdlidOfHsm,hsmRec.ipAddr,hsmRec.port);

	// 读取加密机超时时间
	if ((timeoutOfHsm = UnionReadIntTypeRECVar("timeoutOfHsm")) <= 2)
		timeoutOfHsm = 3;

	// reqBuf的格式，通讯长度+消息头+消息
	// modify by liwj 20150701
	if (hsmGroup.hsmCmdVersionID == conHsmCmdVerSRJ1401StandardHsmCmd)
	{
		// 8字节报文长度 + 2字节应用编号(CM) + 8字节的消息头+3字节服务代码 + 1字节请求/响应标识 + 报文正文
		memset(reqBuf + hsmGroup.lenOfCommHead + 2, '0', hsmGroup.lenOfMsgHead);
		msgOffset = hsmGroup.lenOfCommHead;
	}
	else
	{
		memset(reqBuf + hsmGroup.lenOfCommHead,'0',hsmGroup.lenOfMsgHead);
		msgOffset = hsmGroup.lenOfCommHead + hsmGroup.lenOfMsgHead;
	}
	// end

	while(1)
	{
		//UnionSetMultiLogBegin();
		
		if (isConnectSpecHsm)
		{
			if(hsmSckHDL >= 0)
			{
				UnionLog("in %s:: close spec hsmSckHDL!\n", UnionGetApplicationName());
				UnionCloseSocket(hsmSckHDL);
			}
			//gunionHsmSckHDL = -1;
			gunionHsmSckHDL = gunionHsmLastNormalSckHDL;
			hsmSckHDL = gunionHsmLastNormalSckHDL;
			//UnionSetCommConfAbnormal(pgThisCommConf);
			//isConnectSpecHsm = 0;
			//UnionSetMultiLogEnd();
			//goto againConnectHsm;
		}
		
		if (firstFlag)
		{
			if (UnionDealHsmTestingPackage() < 0)
			{
				break;
			}
			firstFlag = 0;
			alarm(0);
		}

		hsmSckHDL = gunionHsmSckHDL;

		while(1)
		{
			if ((maxFreeTime = UnionReadIntTypeRECVar("maxFreeTimeOfHsm")) < 0)
				maxFreeTime = 10;
			alarm(maxFreeTime);
			signal(SIGALRM,(void *)UnionDealHsmTestingPackage2);

			UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfHsmSvr);
			memset(&msgHeader,0,sizeof(msgHeader));
			if ((len = UnionReadRequestToSpecifiedModuleWithMsgHeader(mdlidOfHsm,msg,sizeof(msg),&applierMDLID,&msgHeader)) < 0)
			{
				alarm(0);
				if(len == errCodeOffsetOfMsgBufMDL) // 由探测指令中断
					continue;
			}
			alarm(0);
			break;
		}
		if (len < 0)
		{
			UnionUserErrLog("in %s:: UnionReadRequestToSpecifiedModuleWithMsgHeader [%ld]!\n",UnionGetApplicationName(),mdlidOfHsm);
			break;
		}
		msg[len] = 0;
		UnionProgramerLog("in %s:: unionmsg receive [%.2s] from [%ld]!\n", UnionGetApplicationName(), msg, applierMDLID);
		
		
		// 检查打印格式
		offset = 0;
		if(memcmp(msg + offset, "PRTT", 4) == 0)
		{
			packagePrintType = msg[4];
			offset += 5;
                }

		// 检查是否指定IP地址
		if (memcmp(msg + offset,"HSMIP",5) == 0)
		{
			offset += 5;
			memcpy(buf,msg + offset,2);
			buf[2] = 0;
			offset += 2;
			ret = atoi(buf);
			memcpy(specIPAddr,msg + offset,ret);
			specIPAddr[ret] = 0;
			offset += ret;
						
			// IP地址和现在连接的加密机不同
			if (strcmp(specIPAddr,hsmRec.ipAddr) != 0)
			{
				//UnionCloseSocket(hsmSckHDL);
				//gunionHsmSckHDL = -1;
				gunionHsmLastNormalSckHDL = gunionHsmSckHDL;
				if ((hsmSckHDL = UnionCreateSocketClient(specIPAddr,hsmRec.port)) < 0)
				{
					UnionUserErrLog("in %s:: UnionCreateSocketClient spec HSM [%s] [%d]!\n",UnionGetApplicationName(),specIPAddr,hsmRec.port);
					// add by leipp 20151016
					if (hsmGroup.hsmCmdVersionID == conHsmCmdVerSRJ1401StandardHsmCmd)
					{
						memcpy(hsmCmd,msg+offset+2,2);
						hsmCmd[2] = 0;
					}
					else
					{
						memcpy(hsmCmd,msg+offset,2);
						hsmCmd[2] = 0;
					}
					UnionSendHsmCmdRunningMsg(hsmRec.hsmGroupID,specIPAddr,hsmCmd,"-1");
					// end by leipp 20151016
					hsmSckHDL = gunionHsmLastNormalSckHDL;
					//break;
					continue;
				}
				gunionHsmSckHDL = hsmSckHDL;
				isConnectSpecHsm = 1;
			}
		}
		len -= offset;
		// modify by liwj 20150701
		if (hsmGroup.hsmCmdVersionID == conHsmCmdVerSRJ1401StandardHsmCmd)
		{
			memcpy(reqBuf + hsmGroup.lenOfCommHead, msg + offset, 2);
			memcpy(reqBuf + hsmGroup.lenOfCommHead + 2 + hsmGroup.lenOfMsgHead, msg + offset + 2, len - 2);
			len -= 2;
			// add by leipp 20151016
			memcpy(hsmCmd,msg+offset+2,2);
			hsmCmd[2] = 0;
			// end by leipp 20151016	
		}
		else
		{
			memcpy(reqBuf + hsmGroup.lenOfCommHead + hsmGroup.lenOfMsgHead,msg + offset,len);
			// add by leipp 20151016
			memcpy(hsmCmd,msg+offset,2);
			hsmCmd[2] = 0;
			// end by leipp 20151016
		}
		// end
		len += msgOffset;
		reqBuf[len] = 0;
		
		// 读取指令头
		if (lenOfHsmCmdHead > 0)
		{
			memcpy(hsmCmdHead,reqBuf + msgOffset,lenOfHsmCmdHead);
			hsmCmdHead[lenOfHsmCmdHead] = 0;
		}
		else
			hsmCmdHead[0] = 0;

		if (UnionIsFldStrInUnionFldListStr(hsmGroup.sensitiveCmd,strlen(hsmGroup.sensitiveCmd),',',hsmCmdHead) <= 0)	
			UnionPrintHsmReqLog(mdlidOfHsm, msgHeader.provider, hsmRec.ipAddr, hsmRec.port, packagePrintType, len - msgOffset, (char *)reqBuf + msgOffset);
		else
			UnionPrintHsmReqLog(mdlidOfHsm, msgHeader.provider, hsmRec.ipAddr, hsmRec.port, '8', len - msgOffset, (char *)reqBuf + msgOffset);
			
		if (len > msgOffset)
		{
			if (hsmGroup.lenOfCommHead == 2)
			{
				reqBuf[0] = (len - hsmGroup.lenOfCommHead) / 256;
				reqBuf[1] = (len - hsmGroup.lenOfCommHead) % 256;
			}
			else if (hsmGroup.lenOfCommHead > 0 ) // modify by liwj 20150701
			{
				snprintf(buf, sizeof(buf), "%0*d", hsmGroup.lenOfCommHead,len - hsmGroup.lenOfCommHead);
				memcpy(reqBuf, buf, hsmGroup.lenOfCommHead);
			}
			// end

			alarm(0);
#if ( defined __linux__ ) || ( defined __hpux )
			if (sigsetjmp(gunionHsmJmpEnv,1) != 0)
#elif ( defined _AIX )
			if (setjmp(gunionHsmJmpEnv) != 0)
#endif
			{
				UnionUserErrLog("in %s:: Timeout!\n",UnionGetApplicationName());
				// 2014-4-16 add by zhangyd
				UnionUpdateHsmStatus(hsmID,0);
				// add by leipp 20151016
				UnionSendHsmCmdRunningMsg(hsmRec.hsmGroupID,hsmRec.ipAddr,hsmCmd,"-1");
				// end by leipp 20151016
				break;
			}
			alarm(timeoutOfHsm);
			signal(SIGALRM,UnionDealHsmResponseTimeout);
trySend:
			//UnionLog("[debug]:: UnionSendToSocket reqBuf[%s] len[%d]\n", reqBuf, len);
			if ((ret = UnionSendToSocket(hsmSckHDL,reqBuf,len)) < 0)
			{
				if (!trySendFlag && isConnectSpecHsm == 0)
				{
					UnionLog("[debug]:: UnionSendToSocket reTry \n");
					trySendFlag = 1;
					UnionCloseSocket(hsmSckHDL);
					gunionHsmSckHDL = -1;
					if ((hsmSckHDL = UnionCreateSocketClient(hsmRec.ipAddr,hsmRec.port)) < 0)
					{
						// 2014-4-16 add by zhangyd
						UnionUpdateHsmStatus(hsmID,0);
						// 2014-4-16 end by zhangyd
						UnionUserErrLog("in %s:: UnionCreateSocketClient [%s] [%d]!\n",UnionGetApplicationName(),hsmRec.ipAddr,hsmRec.port);

						// add by leipp 20151016
						UnionSendHsmCmdRunningMsg(hsmRec.hsmGroupID,hsmRec.ipAddr,hsmCmd,"-1");
						// end by leipp 20151016
						break;
					}
					gunionHsmSckHDL = hsmSckHDL;
					alarm(timeoutOfHsm);
					signal(SIGALRM,UnionDealHsmResponseTimeout);
					goto trySend;
				}
				else
				{
					UnionUserErrLog("in %s:: UnionSendToSocket!\n",UnionGetApplicationName());
					UnionUpdateHsmStatus(hsmID,0);
					// add by leipp 20151016
					UnionSendHsmCmdRunningMsg(hsmRec.hsmGroupID,hsmRec.ipAddr,hsmCmd,"-1");
					// end by leipp 20151016
					break;
				}
			}
			
			if ((ret = UnionReceiveFromSocketUntilLen(hsmSckHDL,resBuf,hsmGroup.lenOfCommHead)) != hsmGroup.lenOfCommHead)
			{
				UnionUserErrLog("in %s:: UnionReceiveFromSocketUntilLen!\n",UnionGetApplicationName());
				// add by leipp 20151016
				UnionSendHsmCmdRunningMsg(hsmRec.hsmGroupID,hsmRec.ipAddr,hsmCmd,"-1");
				// end by leipp 20151016
				break;
			}
			//modify end 20150929


			if (ret < hsmGroup.lenOfCommHead)
			{
				UnionUserErrLog("in %s:: UnionReceiveFromSocket ret[%d] < lenOfCommHead[%d]!\n",UnionGetApplicationName(),ret,hsmGroup.lenOfCommHead);
				UnionUpdateHsmStatus(hsmID,0);
				// add by leipp 20151016
				UnionSendHsmCmdRunningMsg(hsmRec.hsmGroupID,hsmRec.ipAddr,hsmCmd,"-1");
				// end by leipp 20151016
				break;
			}
			if (hsmGroup.lenOfCommHead == 2)
				len = resBuf[0] * 256 + resBuf[1];
			else if (hsmGroup.lenOfCommHead <= 0)
				len = ret;
			else
			{
				memcpy(lenBuf,resBuf,hsmGroup.lenOfCommHead);
				lenBuf[hsmGroup.lenOfCommHead] = 0;
				len = atoi((char *)lenBuf);
			}
			
			UnionProgramerLog("in %s:: ret[%d] len[%d] resBuf[%.50s]\n",UnionGetApplicationName(), ret, len, resBuf);
			if ((ret - hsmGroup.lenOfCommHead) < len)
			{
				len2 = len - (ret - hsmGroup.lenOfCommHead);
				if ((ret = UnionReceiveFromSocketUntilLen(hsmSckHDL,resBuf + ret,len2)) != len2)
				{
					UnionUserErrLog("in %s:: UnionReceiveFromSocketUntilLen ret[%d] len2[%d]!\n",UnionGetApplicationName(),ret,len2);
					// add by leipp 20151016
					UnionSendHsmCmdRunningMsg(hsmRec.hsmGroupID,hsmRec.ipAddr,hsmCmd,"-1");
					// end by leipp 20151016
					break;
				}

				// modify by leipp 20151020
				if (hsmGroup.hsmCmdVersionID == conHsmCmdVerSRJ1401StandardHsmCmd)
				{
					if (len2 < (hsmGroup.lenOfCommHead + hsmGroup.lenOfMsgHead + 2 + 2))
					{
						UnionUserErrLog("in %s:: len[%d] < hsmGroup.lenOfCommHead[%d] + hsmGroup.lenOfMsgHead[%d] + 2 + 2!\n",UnionGetApplicationName(),len2,hsmGroup.lenOfCommHead,hsmGroup.lenOfMsgHead);
						// add by leipp 20151016
						UnionSendHsmCmdRunningMsg(hsmRec.hsmGroupID,hsmRec.ipAddr,hsmCmd,"-1");
						// end by leipp 20151016
						break;
					}

					memcpy(hsmCmdResCode,resBuf + hsmGroup.lenOfCommHead +hsmGroup.lenOfMsgHead + 2 + 2,2);
					hsmCmdResCode[2] = 0;
				}
				else
				{
					if (len2 < (hsmGroup.lenOfCommHead + hsmGroup.lenOfMsgHead + 2))
					{
						UnionUserErrLog("in %s:: len[%d] < hsmGroup.lenOfCommHead[%d] + hsmGroup.lenOfMsgHead[%d] + 2!\n",UnionGetApplicationName(),len2,hsmGroup.lenOfCommHead,hsmGroup.lenOfMsgHead);
						// add by leipp 20151016
						UnionSendHsmCmdRunningMsg(hsmRec.hsmGroupID,hsmRec.ipAddr,hsmCmd,"-1");
						// end by leipp 20151016
						break;
					}
					memcpy(hsmCmdResCode,resBuf + hsmGroup.lenOfCommHead + hsmGroup.lenOfMsgHead + 2,2);
					hsmCmdResCode[2] = 0;
				}
				// modify by leipp 20151020
			}
			resBuf[len + hsmGroup.lenOfCommHead] = 0;
			UnionIncreaseCommConfWorkingTimes(pgThisCommConf);
		}
		else
			continue;

		//if (strstr(hsmGroup.sensitiveCmd,hsmCmdHead) == NULL)
		if (UnionIsFldStrInUnionFldListStr(hsmGroup.sensitiveCmd,strlen(hsmGroup.sensitiveCmd),',',hsmCmdHead) <= 0)	
		 	UnionPrintHsmResLog(mdlidOfHsm, msgHeader.provider, hsmRec.ipAddr, hsmRec.port, packagePrintType, len - msgOffset, (char *)resBuf + msgOffset);
		else
		 	UnionPrintHsmResLog(mdlidOfHsm, msgHeader.provider, hsmRec.ipAddr, hsmRec.port, '8', len - msgOffset, (char *)resBuf + msgOffset);
			

		len -= hsmGroup.lenOfMsgHead;
		if (len <= 0)
		{
			UnionUserErrLog("in %s:: unionmsg sned len [%d] <= 0!\n",UnionGetApplicationName(),len);
			// add by leipp 20151016
			UnionSendHsmCmdRunningMsg(hsmRec.hsmGroupID,hsmRec.ipAddr,hsmCmd,"-1");
			// end by leipp 20151016
			break;
		}
		UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfHsmSvr);
		UnionProgramerLog("in %s:: unionmsg send [%.2s] to [%ld]!\n", UnionGetApplicationName(), resBuf + msgOffset, applierMDLID);
		if ((ret = UnionSendResponseToApplyModuleWithOriMsgHeader(applierMDLID,resBuf + msgOffset,len,&msgHeader)) < 0)
		{
			UnionUserErrLog("in %s:: UnionSendResponseToApplyModuleWithOriMsgHeader [%ld]!\n",UnionGetApplicationName(),applierMDLID);
			// add by leipp 20151016
			UnionSendHsmCmdRunningMsg(hsmRec.hsmGroupID,hsmRec.ipAddr,hsmCmd,"-1");
			// end by leipp 20151016
			break;
		}

		// add by leipp 20151016
		UnionSendHsmCmdRunningMsg(hsmRec.hsmGroupID,hsmRec.ipAddr,hsmCmd,hsmCmdResCode);
		// end by leipp 20151016

		// 判断哪些指令有2次返回
		if ((memcmp(hsmCmdHead,"A2",2) == 0) ||
				(memcmp(hsmCmdHead,"NE",2) == 0) ||
				(memcmp(hsmCmdHead,"OC",2) == 0) ||
				(memcmp(hsmCmdHead,"OE",2) == 0) ||
				(memcmp(hsmCmdHead,"PE",2) == 0) ||
				(memcmp(hsmCmdHead,"OA",2) == 0))
		{
			if (hsmSckHDL > 0)
			{
				UnionCloseSocket(hsmSckHDL);
				gunionHsmSckHDL = -1;
			}

			UnionSetCommConfAbnormal(pgThisCommConf);
			//UnionSetMultiLogEnd();
			goto againConnectHsm;
		}
		//UnionSetMultiLogEnd();
	}
	//UnionSetMultiLogEnd();
	return(UnionTaskActionBeforeExit());
}	

//modify by linxj 20160219
int UnionDealHsmTestingPackage()
{
	int			ret;
	int			len = 0;
	int			len2 = 0;
	int			msgOffset = 0;
	int			timeoutOfHsm;
	int			lenOfHsmCmdHead = 2;
	int			trySendFlag = 0;
	int			isConnectSpecHsm = 0;
	char			hsmID[32];
	char			hsmCmdHead[32];
	char			buf[1024];
	char			specIPAddr[64];
	char			hsmCmd[8];
	char			hsmCmdResCode[8];
	unsigned char		reqBuf[81920];
	unsigned char		resBuf[81920];
	unsigned char		lenBuf[32];
	TUnionHsm		hsmRec;
	TUnionHsmGroup		hsmGroup;
	int			hsmSckHDL = -1;

	snprintf(hsmID,sizeof(hsmID),"%s",gunionHsmID);

	if ((timeoutOfHsm = UnionReadIntTypeRECVar("timeoutOfHsm")) <= 2)
		timeoutOfHsm = 3;

	if ((ret =  UnionReadHsmRecFromHsm(hsmID,&hsmRec,&hsmGroup)) <= 0)
	{
		UnionUserErrLog("in UnionDealHsmTestingPackage:: UnionReadHsmRecFromHsm[%s] ret = %d !\n",hsmID, ret);
		return(errCodeParameter);
	}
	// 支持域名
        if (!UnionIsValidIPAddrStr(hsmRec.ipAddr))
        {
                if ((ret = UnionGetHostByName(hsmRec.ipAddr,specIPAddr)) < 0)
                {
                        UnionUserErrLog("in %s:: UnionGetHostByName[%s]!\n",UnionGetApplicationName(),hsmRec.ipAddr);
                        return(ret);
                }
                snprintf(hsmRec.ipAddr,sizeof(hsmRec.ipAddr),"%s",specIPAddr);
        }

	// reqBuf的格式，通讯长度+消息头+消息
	// modify by liwj 20150701
	if (hsmGroup.hsmCmdVersionID == conHsmCmdVerSRJ1401StandardHsmCmd)
	{
		// 8字节报文长度 + 2字节应用编号(CM) + 8字节的消息头+3字节服务代码 + 1字节请求/响应标识 + 报文正文
		memset(reqBuf + hsmGroup.lenOfCommHead + 2, '0', hsmGroup.lenOfMsgHead);
		msgOffset = hsmGroup.lenOfCommHead;
	}
	else
	{
		memset(reqBuf + hsmGroup.lenOfCommHead,'0',hsmGroup.lenOfMsgHead);
		msgOffset = hsmGroup.lenOfCommHead + hsmGroup.lenOfMsgHead;
	}
	// end

	snprintf(hsmCmd,sizeof(hsmCmd),"NC");
	// 发送测试指令
	switch(hsmGroup.hsmCmdVersionID)
	{
		// SJL05机指令类型时，为BCD码
		case conHsmCmdVerSJL05StandardHsmCmd:
			len = aschex_to_bcdhex(hsmGroup.reqCmdOfTest,strlen(hsmGroup.reqCmdOfTest),(char *)reqBuf + msgOffset);
			break;
		// add by liwj 20150701
		case conHsmCmdVerSRJ1401StandardHsmCmd:
			memcpy(reqBuf + msgOffset, hsmGroup.reqCmdOfTest, 2);// 拷贝 CM
			msgOffset += 2 + hsmGroup.lenOfMsgHead;
			len = snprintf((char *)reqBuf + msgOffset, sizeof(reqBuf) - msgOffset, "%s", hsmGroup.reqCmdOfTest + 2);
			break;
		// end
		default:
			len = strlen(hsmGroup.reqCmdOfTest);
			strncpy((char *)reqBuf + msgOffset,hsmGroup.reqCmdOfTest,len);
			break;
	}

	len += msgOffset;
	reqBuf[len] = 0;

	// 读取指令头
	if (lenOfHsmCmdHead > 0)
	{
		memcpy(hsmCmdHead,reqBuf + msgOffset,lenOfHsmCmdHead);
		hsmCmdHead[lenOfHsmCmdHead] = 0;
	}
	else
		hsmCmdHead[0] = 0;

	if (len > msgOffset)
	{
		if (hsmGroup.lenOfCommHead == 2)
		{
			reqBuf[0] = (len - hsmGroup.lenOfCommHead) / 256;
			reqBuf[1] = (len - hsmGroup.lenOfCommHead) % 256;
		}
		else if (hsmGroup.lenOfCommHead > 0 ) // modify by liwj 20150701
		{
			//sprintf((char *)reqBuf,"%0*d",hsmGroup.lenOfCommHead,len - hsmGroup.lenOfCommHead);
			snprintf(buf, sizeof(buf), "%0*d", hsmGroup.lenOfCommHead,len - hsmGroup.lenOfCommHead);
			memcpy(reqBuf, buf, hsmGroup.lenOfCommHead);
		}
		// end

#if ( defined __linux__ ) || ( defined __hpux )
		if (sigsetjmp(gunionHsmJmpEnv,1) != 0)
#elif ( defined _AIX )
			if (setjmp(gunionHsmJmpEnv) != 0)
#endif
			{
				UnionUserErrLog("in %s:: Timeout!\n",UnionGetApplicationName());
				// 2014-4-16 add by zhangyd
				UnionUpdateHsmStatus(hsmID,0);
				// add by leipp 20151016
				UnionSendHsmCmdRunningMsg(hsmRec.hsmGroupID,hsmRec.ipAddr,hsmCmd,"-1");
				// end by leipp 20151016
				return errCodeParameter;
			}
		alarm(timeoutOfHsm);
		signal(SIGALRM,UnionDealHsmResponseTimeout);
trySend:
		UnionLog("in UnionDealHsmTestingPackage:: reqBuf[%s] len[%d]\n", reqBuf+msgOffset, len-msgOffset);
		if ((ret = UnionSendToSocket(gunionHsmSckHDL,reqBuf,len)) < 0)
		{
			if (!trySendFlag && isConnectSpecHsm == 0)
			{
				UnionLog("[debug]:: UnionSendToSocket reTry \n");
				trySendFlag = 1;
				UnionCloseSocket(gunionHsmSckHDL);
				gunionHsmSckHDL = -1;
				if ((hsmSckHDL = UnionCreateSocketClient(hsmRec.ipAddr,hsmRec.port)) < 0)
				{
					// 2014-4-16 add by zhangyd
					UnionUpdateHsmStatus(hsmID,0);
					// 2014-4-16 end by zhangyd
					UnionUserErrLog("in UnionDealHsmTestingPackage:: UnionCreateSocketClient [%s] [%d]!\n",hsmRec.ipAddr,hsmRec.port);

					// add by leipp 20151016
					UnionSendHsmCmdRunningMsg(hsmRec.hsmGroupID,hsmRec.ipAddr,hsmCmd,"-1");
					// end by leipp 20151016
					return(hsmSckHDL);
				}
				gunionHsmSckHDL = hsmSckHDL;
				alarm(timeoutOfHsm);
				signal(SIGALRM,UnionDealHsmResponseTimeout);
				goto trySend;
			}
		}
		if ((ret = UnionReceiveFromSocketUntilLen(gunionHsmSckHDL,resBuf,hsmGroup.lenOfCommHead)) != hsmGroup.lenOfCommHead)
		{
			UnionUserErrLog("in UnionDealHsmTestingPackage:: UnionReceiveFromSocketUntilLen %d != %d!\n", ret, hsmGroup.lenOfCommHead);
			// add by leipp 20151016
			UnionSendHsmCmdRunningMsg(hsmRec.hsmGroupID,hsmRec.ipAddr,hsmCmd,"-1");
			// end by leipp 20151016
			return(errCodeParameter);	
		}
		//modify end 20150929


		if (ret < hsmGroup.lenOfCommHead)
		{
			UnionUserErrLog("in UnionDealHsmTestingPackage:: UnionReceiveFromSocket ret[%d] < lenOfCommHead[%d]!\n",ret,hsmGroup.lenOfCommHead);
			UnionUpdateHsmStatus(hsmID,0);
			// add by leipp 20151016
			UnionSendHsmCmdRunningMsg(hsmRec.hsmGroupID,hsmRec.ipAddr,hsmCmd,"-1");
			// end by leipp 20151016
			return(errCodeParameter);
		}
		if (hsmGroup.lenOfCommHead == 2)
			len = resBuf[0] * 256 + resBuf[1];
		else if (hsmGroup.lenOfCommHead <= 0)
			len = ret;
		else
		{
			memcpy(lenBuf,resBuf,hsmGroup.lenOfCommHead);
			lenBuf[hsmGroup.lenOfCommHead] = 0;
			len = atoi((char *)lenBuf);
		}

		UnionProgramerLog("in UnionDealHsmTestingPackage:: ret[%d] len[%d]!\n", ret, len);
		if ((ret - hsmGroup.lenOfCommHead) < len)
		{
			len2 = len - (ret - hsmGroup.lenOfCommHead);
			if ((ret = UnionReceiveFromSocketUntilLen(gunionHsmSckHDL,resBuf + ret,len2)) != len2)
			{
				UnionUserErrLog("in UnionDealHsmTestingPackage:: UnionReceiveFromSocketUntilLen ret[%d] len2[%d]!\n",ret,len2);
				// add by leipp 20151016
				UnionSendHsmCmdRunningMsg(hsmRec.hsmGroupID,hsmRec.ipAddr,hsmCmd,"-1");
				// end by leipp 20151016
				return(errCodeParameter);
			}
			UnionProgramerLog("in UnionDealHsmTestingPackage:: len2[%d] resBuf[%s]!\n", len2, resBuf+msgOffset);

			// modify by leipp 20151020
			if (hsmGroup.hsmCmdVersionID == conHsmCmdVerSRJ1401StandardHsmCmd)
			{
				if (len2 < (hsmGroup.lenOfCommHead + hsmGroup.lenOfMsgHead + 2 + 2))
				{
					UnionUserErrLog("in UnionDealHsmTestingPackage:: len[%d] < hsmGroup.lenOfCommHead[%d] + hsmGroup.lenOfMsgHead[%d] + 2 + 2!\n", len2,hsmGroup.lenOfCommHead,hsmGroup.lenOfMsgHead);
					// add by leipp 20151016
					UnionSendHsmCmdRunningMsg(hsmRec.hsmGroupID,hsmRec.ipAddr,hsmCmd,"-1");
					// end by leipp 20151016
					return(errCodeParameter);
				}

				memcpy(hsmCmdResCode,resBuf + hsmGroup.lenOfCommHead +hsmGroup.lenOfMsgHead + 2 + 2,2);
				hsmCmdResCode[2] = 0;
			}
			else
			{
				if (len2 < (hsmGroup.lenOfCommHead + hsmGroup.lenOfMsgHead + 2))
				{
					UnionUserErrLog("in %s:: len[%d] < hsmGroup.lenOfCommHead[%d] + hsmGroup.lenOfMsgHead[%d] + 2!\n",UnionGetApplicationName(),len2,hsmGroup.lenOfCommHead,hsmGroup.lenOfMsgHead);
					// add by leipp 20151016
					UnionSendHsmCmdRunningMsg(hsmRec.hsmGroupID,hsmRec.ipAddr,hsmCmd,"-1");
					// end by leipp 20151016
					return(errCodeParameter);
				}
				memcpy(hsmCmdResCode,resBuf + hsmGroup.lenOfCommHead + hsmGroup.lenOfMsgHead + 2,2);
				hsmCmdResCode[2] = 0;
			}
			// modify by leipp 20151020
		}
		resBuf[len + hsmGroup.lenOfCommHead] = 0;
		//UnionLog("in UnionDealHsmTestingPackage:: resBuf[%s] len[%d]\n", resBuf+msgOffset, len-msgOffset);
		UnionIncreaseCommConfWorkingTimes(pgThisCommConf);
	}

	alarm(0);

	// 发送测试指令
	len = ret - msgOffset;
	if (len <= 0)
	{
		UnionUserErrLog("in UnionDealHsmTestingPackage:: len[%d] <= 0\n", len);
		return(errCodeParameter);
	}
	switch(hsmGroup.hsmCmdVersionID)
	{
		// SJL05机指令类型时，为BCD码
		case conHsmCmdVerSJL05StandardHsmCmd:
			len = bcdhex_to_aschex((char *)resBuf + msgOffset,len,buf);
			buf[len] = 0;
			break;
		default:
			memcpy(buf,resBuf + msgOffset,len);
			buf[len] = 0;
			break;
	}

	if (memcmp(buf,hsmGroup.resCmdOfTest,strlen(hsmGroup.resCmdOfTest)) != 0)
	{
		UnionUserErrLog("in UnionDealHsmTestingPackage:: hsmRes[%s] != resCmdOfTest[%s]\n",buf,hsmGroup.resCmdOfTest);
		UnionUpdateHsmStatus(hsmID,0);
		return(errCodeParameter);
	}
	UnionLog("in UnionDealHsmTestingPackage:: reqBuf = [%s] resBuf = [%s] OK!\n", reqBuf+msgOffset, resBuf+msgOffset);
	UnionUpdateHsmStatus(hsmID,1);
	return 0;
}

int UnionDealHsmTestingPackage2()
{
	
	return(UnionDealHsmTestingPackage());
	/*
	int	ret = 0;
	while(1)
	{
		if((ret = UnionDealHsmTestingPackage()) == 0)
		{
			break;
		}
		alarm(0);
		sleep(10);
	}

	return(ret);
	*/
}

void UnionDealHsmResponseTimeout()
{
	UnionUserErrLog("in UnionDealHsmResponseTimeout:: hsm response time out!\n");
#if ( defined __linux__ ) || ( defined __hpux )
	siglongjmp(gunionHsmJmpEnv,10);
#elif ( defined _AIX )
	longjmp(gunionHsmJmpEnv,10);
#endif
}

