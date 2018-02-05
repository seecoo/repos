// Copyright:	Union Tech.
// Author:	zhangyd
// Date:	2010-10-23
// Version:	2.0

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

//#include <sys/types.h>
#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <netinet/tcp.h>
//#include <sys/wait.h>

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

int	UnionCreateSocketClientWithTimeout(char *ip, int port, int timeout);
int	UnionTaskActionBeforeExit();

#define	max_hsm_message_len			8192
#define	hsm_message_size			64000
#define	hsm_message_head_buf_index_len_offset	2
#define	hsm_message_head_buf_index_len		4
#define SIG_CMD_TO_MAIN_PROCESSS_SEND_OTHER	SIGRTMIN + 1
#define SIG_CMD_TO_MAIN_PROCESSS_STOP_ALL	SIGRTMIN + 2
#define SIG_CMD_TO_THREAD_CONTINUE		SIGRTMIN + 3
#define HSM_CONNECT_ERR				"30"


PUnionCommConf		pgThisCommConf = NULL;

typedef struct
{
	// hsmGroup
	char		hsmGroupID[32];
	char		hsmGroupName[64];
	unsigned int	hsmCmdVersionID;
	unsigned int	lenOfMsgHead;
	char		reqCmdOfTest[128];
	char		resCmdOfTest[128];
	char		sensitiveCmd[1024];
	int		lenOfCommHead;
	TUnionModuleID	mdlID;
	
	// hsm
	char		hsmID[32];
	char		ipAddr[64];
	//char		cmdPrefix[64];
	unsigned int	port;
	unsigned int	uses;
	unsigned int	status;
	int		sckHDL;
	int		timeout;
} TUnionHsmInfo;
typedef TUnionHsmInfo	*PUnionHsmInfo;

typedef struct {
	int			thread_no;
	pthread_t		thread_id;
	PUnionHsmInfo		phsmInfo;
} TUnionHsmThreads;
typedef TUnionHsmThreads *PUnionHsmThreads;
static	TUnionHsmThreads g_hsm_threads[3];

typedef struct {
	TUnionModuleID		applierMDLID;
	TUnionMessageHeader	msgHeader;
	int			packagePrintType;	// '1':请求BCD格式打印，'2':响应BCD格式打印，'3':请求响应都用BCD格式打印
	char			hsmCmdHead[32];
	int			used;		// 0:未使用,1:已使用
} TUnionHsmMessage;

typedef struct {
	int			thread_no;
	pthread_t		thread_id;
	char			ip[64];
	int			cmdLen;
	int			sepcRet;
	char			cmd[max_hsm_message_len];
	TUnionHsmMessage	*phsmMsg;
	PUnionHsmInfo		phsmInfo;
} TUnionOtherCmdBuf;

static	TUnionHsmMessage	g_hsm_message[hsm_message_size];
static	int			g_hsm_message_no = 0;
pthread_mutex_t			g_mutexLock = PTHREAD_MUTEX_INITIALIZER;
static	int			g_current_buf_num = 0;
static	int			g_other_buf_num = 0;
static	TUnionOtherCmdBuf	g_other_cmd_buf[1];
static	int			g_last_hsm_stat = -1;
static	int			g_last_hsm_stat_time = 0;
static	int			g_last_cmd_time = 0;
static	TUnionHsmInfo		g_hsmInfo;			// other hsm
static	TUnionHsmInfo		hsmInfo;			// current hsm

void UnionSetOtherIndex()
{
	pthread_mutex_lock(&g_mutexLock);
	g_other_buf_num = 1;
        pthread_mutex_unlock(&g_mutexLock);
        return;
}
void UnionResetOtherIndex()
{
	pthread_mutex_lock(&g_mutexLock);
	g_other_buf_num = 0;
        pthread_mutex_unlock(&g_mutexLock);
        return;
}
int UnionGetOtherIndex()
{
	return(g_other_buf_num);
}

void UnionDecreaseIndex()
{
	pthread_mutex_lock(&g_mutexLock);
	g_current_buf_num--;
        pthread_mutex_unlock(&g_mutexLock);
        return;
}
void UnionIncreaseIndex()
{
	pthread_mutex_lock(&g_mutexLock);
	g_current_buf_num++;
        pthread_mutex_unlock(&g_mutexLock);
        return;
}
int UnionGetCurrentIndex()
{
	return(g_current_buf_num);
}
void UnionResetCurrentIndex()
{
	pthread_mutex_lock(&g_mutexLock);
	g_current_buf_num = 0;
        pthread_mutex_unlock(&g_mutexLock);
        return;
}

void UnionLetMainProcessSendOther()
{
	raise(SIG_CMD_TO_MAIN_PROCESSS_SEND_OTHER);
}
void UnionLetMainProcessStopAll()
{
	raise(SIG_CMD_TO_MAIN_PROCESSS_STOP_ALL);
}

void UnionLetThreadContinue(pthread_t thread_id)
{
	if(!pthread_kill(thread_id, 0))
	{
		pthread_kill(thread_id, SIG_CMD_TO_THREAD_CONTINUE);
	}
}

void UnionDoMainProcessStopAll()
{
	exit(UnionTaskActionBeforeExit());
}

int UnionHelp()
{
	printf("Usage:: %s hsmID\n",UnionGetApplicationName());
	return(0);
}

int UnionTaskActionBeforeExit()
{
	if (pgThisCommConf)
		UnionSetCommConfAbnormal(pgThisCommConf);
		
	//UnionCloseDatabase();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}
void UnionPrintHsmReqLog(long mdlid, int provider, char *ip, int port, int printType, int len, char *str)
{
	char		*ptr = NULL;
	char		buf[max_hsm_message_len*2];

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
	char		buf[max_hsm_message_len*2];

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
int UnionReadActiveHsmInfoByCondition(char *condition, PUnionHsmInfo phsmInfo)
{
	int	ret;
	char	sql[1024];
	char	tmpBuf[128];
	
	snprintf(sql, sizeof(sql), "select hsmID,hsm.hsmGroupID,ipAddr,port,uses,status,hsmGroupName,hsmCmdVersionID,lenOfCommHead,lenOfMsgHead,reqCmdOfTest,resCmdOfTest,sensitiveCmd from hsm,hsmGroup where hsm.hsmGroupID = hsmGroup.hsmGroupID and hsmGroup.enabled = 1 and hsm.enabled = 1 and %s", condition);

	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionReadActiveHsmInfoByCondition:: UnionSelectRealDBRecord[%s] ret = %d!\n", sql, ret);
		return(ret);
	}
	else if (ret == 0)
		return(0);
	
	UnionLocateXMLPackage("detail", 1);
	
	// 读取密码机ID
	if ((ret = UnionReadXMLPackageValue("hsmID", phsmInfo->hsmID, sizeof(phsmInfo->hsmID))) < 0)
	{
		UnionUserErrLog("in UnionReadActiveHsmInfoByCondition:: UnionReadXMLPackageValue[%s]!\n","hsmID");
		return(ret);
	}
	// 读取密码机IP地址
	if ((ret = UnionReadXMLPackageValue("ipAddr", phsmInfo->ipAddr, sizeof(phsmInfo->ipAddr))) < 0)
	{
		UnionUserErrLog("in UnionReadActiveHsmInfoByCondition:: UnionReadXMLPackageValue[%s]!\n","ipAddr");
		return(ret);
	}
	// 读取密码机端口
	if ((ret = UnionReadXMLPackageValue("port", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadActiveHsmInfoByCondition:: UnionReadXMLPackageValue[%s]!\n","port");
		return(ret);
	}
	phsmInfo->port = atoi(tmpBuf);
	// 读取密码机组
	if ((ret = UnionReadXMLPackageValue("hsmGroupID", phsmInfo->hsmGroupID, sizeof(phsmInfo->hsmGroupID))) < 0)
	{
		UnionUserErrLog("in UnionReadActiveHsmInfoByCondition:: UnionReadXMLPackageValue[%s]!\n","hsmGroupID");
		return(ret);
	}
	// 读取密码机用途
	if ((ret = UnionReadXMLPackageValue("uses", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadActiveHsmInfoByCondition:: UnionReadXMLPackageValue[%s]!\n","uses");
		return(ret);
	}
	phsmInfo->uses = atoi(tmpBuf);
	// 读取密码机状态
	if ((ret = UnionReadXMLPackageValue("status", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadActiveHsmInfoByCondition:: UnionReadXMLPackageValue[%s]!\n","status");
		return(ret);
	}
	phsmInfo->status = atoi(tmpBuf);
		
	phsmInfo->sckHDL = -1;
		
	// 读取密码机组名
	if ((ret = UnionReadXMLPackageValue("hsmGroupName", phsmInfo->hsmGroupName, sizeof(phsmInfo->hsmGroupName))) < 0)
	{
		UnionUserErrLog("in UnionReadActiveHsmInfoByCondition:: UnionReadXMLPackageValue[%s]!\n","hsmGroupName");
		return(ret);
	}
	phsmInfo->hsmGroupName[ret] = 0;
	// 读取密码机指令类型
	if ((ret = UnionReadXMLPackageValue("hsmCmdVersionID", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadActiveHsmInfoByCondition:: UnionReadXMLPackageValue[%s]!\n","hsmCmdVersionID");
		return(ret);
	}
	phsmInfo->hsmCmdVersionID = atoi(tmpBuf);
	// 读取通讯头长度
	if ((ret = UnionReadXMLPackageValue("lenOfCommHead", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadActiveHsmInfoByCondition:: UnionReadXMLPackageValue[%s]!\n","lenOfCommHead");
		return(ret);
	}
	phsmInfo->lenOfCommHead = atoi(tmpBuf);
	// 读取消息头长度
	if ((ret = UnionReadXMLPackageValue("lenOfMsgHead", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadActiveHsmInfoByCondition:: UnionReadXMLPackageValue[%s]!\n","lenOfMsgHead");
		return(ret);
	}
	phsmInfo->lenOfMsgHead = atoi(tmpBuf);
	if (phsmInfo->lenOfMsgHead < 8)
	{
		UnionUserErrLog("in UnionReadActiveHsmInfoByCondition:: lenOfMsgHead[%d] < 8!\n",phsmInfo->lenOfMsgHead);
		return(errCodeTooShortLength);
	}
	
	// 读取探测请求指令
	if ((ret = UnionReadXMLPackageValue("reqCmdOfTest", phsmInfo->reqCmdOfTest, sizeof(phsmInfo->reqCmdOfTest))) < 0)
	{
		UnionUserErrLog("in UnionReadActiveHsmInfoByCondition:: UnionReadXMLPackageValue[%s]!\n","reqCmdOfTest");
		return(ret);
	}
	// 读取探测响应指令
	if ((ret = UnionReadXMLPackageValue("resCmdOfTest", phsmInfo->resCmdOfTest, sizeof(phsmInfo->resCmdOfTest))) < 0)
	{
		UnionUserErrLog("in UnionReadActiveHsmInfoByCondition:: UnionReadXMLPackageValue[%s]!\n","resCmdOfTest");
		return(ret);
	}
	// 读取敏感指令
	if ((ret = UnionReadXMLPackageValue("sensitiveCmd", phsmInfo->sensitiveCmd, sizeof(phsmInfo->sensitiveCmd))) < 0)
	{
		UnionUserErrLog("in UnionReadActiveHsmInfoByCondition:: UnionReadXMLPackageValue[%s]!\n","sensitiveCmd");
		return(ret);
	}
		
	if ((phsmInfo->mdlID = UnionGetFixedMDLIDOfTaskOfHsmGrp(phsmInfo->hsmGroupID)) <= 0)
	{
		UnionUserErrLog("in UnionReadActiveHsmInfoByCondition:: UnionGetFixedMDLIDOfTaskOfHsmGrp[%s]!\n",phsmInfo->hsmGroupID);
		return(phsmInfo->mdlID);
	}
	phsmInfo->mdlID += conMDLTypeOffsetOfHsmSvr;

	return(1);
}

int UnionReadActiveHsmInfoByHsmID(char *hsmID, PUnionHsmInfo phsmInfo)
{
	char		condition[32];
	sprintf(condition, "hsmID = '%s'", hsmID);
	return(UnionReadActiveHsmInfoByCondition(condition, phsmInfo));
}

int UnionReadActiveHsmInfoByHsmIP(char *ipAddr, PUnionHsmInfo phsmInfo)
{
	char		condition[32];
	sprintf(condition, "ipAddr = '%s'", ipAddr);
	return(UnionReadActiveHsmInfoByCondition(condition, phsmInfo));
}

int UnionUpdateHsmStatus(char *hsmID,int curStatus)
{
	int			ret = 0;
	char			sql[128];
	int			curTime = 0;

	curTime = (int)time(NULL);
	// 和上次状态相同且未超过１０分钟则不更新数据库状态
	if(curStatus == g_last_hsm_stat && curTime - g_last_hsm_stat_time < 600)
	{
		return(0);
	}
	
	snprintf(sql, sizeof(sql), "update hsm set status = %d where  hsmID = '%s' and status != %d", curStatus, hsmID, curStatus);
	if ((ret = UnionExecRealDBSql2(0,sql)) < 0)
	{
		UnionUserErrLog("in UnionUpdateHsmStatus:: UnionExecRealDBSql2[%s]\n", sql);
	}
	g_last_hsm_stat = curStatus;
	g_last_hsm_stat_time = curTime;
	return(ret);
}


int UnionExchangCmdWithHsm(int sockHdl, int headLen,unsigned char *reqPackage, int reqLen, unsigned char *resPackage, int resSize)
{
	int	     ret = -1;
	int	     len = 0;

	if ((ret = UnionSendToSocket(sockHdl, reqPackage, reqLen)) < 0)
	{
		UnionUserErrLog("in UnionExchangCmdWithHsm:: UnionSendToSocket Error req = [%s] ret = [%d]!\n", reqPackage + headLen, ret);
		return(ret);
	}

	if ((ret = UnionReceiveFromSocketUntilLen(sockHdl, resPackage, headLen)) != headLen)
	{
		UnionUserErrLog("in UnionExchangCmdWithHsm:: UnionReceiveFromSocketUntilLen read len = %d Error! ret = [%d]!\n", headLen, ret);
		return(ret);
	}

	if (headLen == 2)
	{
		len = resPackage[0] * 256 + resPackage[1];
	}
	else if (headLen > 0)
	{
		resPackage[headLen] = 0;
		len = atoi((char *)resPackage);
	}
	
	if ((ret = UnionReceiveFromSocketUntilLen(sockHdl, resPackage + headLen, len)) != len)
	{
		UnionUserErrLog("in UnionExchangCmdWithHsm:: UnionReceiveFromSocketUntilLen read len = %d Error! ret = [%d]!\n", len, ret);
		return(ret);
	}

	return(ret + headLen);
}


int UnionExchangWithoutCommHead(int sockHdl, unsigned char *reqPackage, int reqLen, unsigned char *resPackage, int resSize)
{
	int	     ret = -1;

	if ((ret = UnionSendToSocket(sockHdl, reqPackage, reqLen)) < 0)
	{
		UnionUserErrLog("in UnionExchangWithoutCommHead:: UnionSendToSocket Error req = [%s] ret = [%d]!\n", reqPackage, ret);
		return(ret);
	}
	
	if ((ret = UnionReceiveFromSocket(sockHdl, resPackage, resSize)) < 0)
	{
		UnionUserErrLog("in UnionExchangWithoutCommHead:: UnionReceiveFromSocket read  Error! ret = [%d]!\n", ret);
		return(ret);
	}
	return(ret);

}

int Uclose(int hdl)
{
	//UnionLog("in Uclose:: close socket %d \n", hdl);
	return(close(hdl));
}
// 检查并返回密码机连接, isCheck 不等于0则发送测试指令
int UnionCheckAndConnHsm(PUnionHsmInfo phsmInfo, int isCheck)
{
	int		ret = -1, reqLen = 0;
	int		sockHdl = -1;
	int		len;
	int		timeoutOfHsm = 0;

	unsigned char	reqBuf[1024];
	unsigned char	resBuf[1024];
	char		tmpBuf[1024];
	

	// 读取加密机超时时间
	if ((timeoutOfHsm = UnionReadIntTypeRECVar("timeoutOfHsm")) <= 2)
		timeoutOfHsm = 3;

	phsmInfo->timeout = timeoutOfHsm;

	if((sockHdl = UnionCreateSocketClientWithTimeout(phsmInfo->ipAddr, phsmInfo->port, phsmInfo->timeout)) < 0)
	{
		UnionUserErrLog("in UnionCheckAndConnHsm:: UnionCreateSocketClient ret = [%d]!\n", sockHdl);
		return(sockHdl);
	}
	//UnionLog("in UnionCheckAndConnHsm create socket %d \n", sockHdl);

	if(isCheck)
	{
		switch(phsmInfo->hsmCmdVersionID)
		{
			// SJL05机指令类型时，为BCD码
			case conHsmCmdVerSJL05StandardHsmCmd:
				len = aschex_to_bcdhex(phsmInfo->reqCmdOfTest,strlen(phsmInfo->reqCmdOfTest),tmpBuf);
				break;
			default:
				len = strlen(phsmInfo->reqCmdOfTest);
				strncpy(tmpBuf,phsmInfo->reqCmdOfTest,len);
				break;
		}
		
		reqLen = phsmInfo->lenOfCommHead + phsmInfo->lenOfMsgHead + len;
		if (phsmInfo->lenOfCommHead == 2)
		{
			reqBuf[0] = (reqLen - phsmInfo->lenOfCommHead) / 256;
			reqBuf[1] = (reqLen - phsmInfo->lenOfCommHead) % 256;
		}
		else if (phsmInfo->lenOfCommHead > 0)
		{
			snprintf((char *)reqBuf,sizeof(reqBuf),"%0*d", phsmInfo->lenOfCommHead, reqLen - phsmInfo->lenOfCommHead);
		}

		memset(reqBuf + phsmInfo->lenOfCommHead, '0', phsmInfo->lenOfMsgHead);
		memcpy(reqBuf + phsmInfo->lenOfCommHead + phsmInfo->lenOfMsgHead, tmpBuf, len);
		reqBuf[0] = 0x00;
		reqBuf[1] = 0x0A;

		if (phsmInfo->lenOfCommHead != 0)
		{
			if ((ret = UnionExchangCmdWithHsm(sockHdl, phsmInfo->lenOfCommHead, reqBuf, reqLen, resBuf, sizeof(resBuf))) < 0)
			{
				UnionUserErrLog("in UnionCheckAndConnHsm:; UnionExchangCmdWithHsm ret = %d reqLen = %d, req = [%s]\n", ret, reqLen, reqBuf + phsmInfo->lenOfCommHead);
				Uclose(sockHdl);
				return(ret);
			}
		}
		else
		{
			if ((ret = UnionExchangWithoutCommHead(sockHdl, reqBuf, reqLen, resBuf, sizeof(resBuf))) < 0)
			{
				UnionUserErrLog("in UnionCheckAndConnHsm:; UnionExchangWithoutCommHead ret = %d reqLen = %d, req = [%s]\n", ret, reqLen, reqBuf + phsmInfo->lenOfCommHead);
				Uclose(sockHdl);
				return(ret);
			}
		}
		
		len = ret - phsmInfo->lenOfCommHead - phsmInfo->lenOfMsgHead;
		switch(phsmInfo->hsmCmdVersionID)
		{
			// SJL05机指令类型时，为BCD码
			case conHsmCmdVerSJL05StandardHsmCmd:
				len = bcdhex_to_aschex((char *)resBuf + (phsmInfo->lenOfCommHead + phsmInfo->lenOfMsgHead),len,tmpBuf);
				tmpBuf[len] = 0;
				break;
			default:
				memcpy(tmpBuf,resBuf + phsmInfo->lenOfCommHead + phsmInfo->lenOfMsgHead,len);
				tmpBuf[len] = 0;
				break;
		}
		
		if (memcmp(tmpBuf, phsmInfo->resCmdOfTest, strlen(phsmInfo->resCmdOfTest)) != 0)
		{
			UnionUserErrLog("in UnionCheckAndConnHsm:; hsmID[%s] testRes = [%s] != [%s]!\n", phsmInfo->hsmID, tmpBuf, phsmInfo->resCmdOfTest);
		
			Uclose(sockHdl);
			return(errCodeParameter);
		}
	}
	return(sockHdl);
}

int UnionSetCmdToOtherHsmBuf(char *specIPAddr, unsigned char *cmd, int cmdLen, TUnionHsmMessage *phsmMsg)
{
	int		bufIndex = 0;

	if((bufIndex = UnionGetOtherIndex()) != 0 && (int)time(NULL) - g_last_cmd_time < 10)
	{
		
		UnionUserErrLog("in UnionSetCmdToOtherHsmBuf:; bufIndex = %d \n!", bufIndex);
		return(-1);
	}
	strcpy(g_other_cmd_buf[bufIndex].ip, specIPAddr);
	memcpy(g_other_cmd_buf[bufIndex].cmd, cmd, cmdLen);
	g_other_cmd_buf[bufIndex].cmdLen = cmdLen;
	g_other_cmd_buf[bufIndex].phsmMsg = phsmMsg;
	return(0);
	
}

int UnionGetCmdFromOtherHsmBuf(char *specIPAddr, unsigned char *cmd, int cmdLen)
{
	return(0);
}

void *UnionSendMsgToHsm(void *p)
{
	int			ret = 0;
	int			len;
	int			offset = 0;
	int			msgOffset = 0;
	int			index = 0;
	char			buf[128];
	char			specIPAddr[64];
	unsigned char		reqBuf[max_hsm_message_len];
	unsigned char		msg[max_hsm_message_len];	
	PUnionHsmInfo		phsmInfo;
	int			tryTimes = 0;
	int			currentIndex = 0;
	int			lenOfHsmCmdHead = 0;
	
	PUnionHsmThreads      	pthread = (PUnionHsmThreads)p;
	
	phsmInfo = pthread->phsmInfo;

	// reqBuf的格式，通讯长度+消息头+消息
	memset(reqBuf + phsmInfo->lenOfCommHead,'0',phsmInfo->lenOfMsgHead);
	msgOffset = phsmInfo->lenOfCommHead + phsmInfo->lenOfMsgHead;

	while(1)
	{
		index = g_hsm_message_no % hsm_message_size;

		tryTimes = 0;
		while((currentIndex = UnionGetCurrentIndex()) >= hsm_message_size)
		{
			UnionLog("in UnionSendMsgToHsm:: hsm_message_size meet to max %d!!!!!!!\n",  currentIndex);
			//if((int)time(NULL) - g_last_cmd_time > phsmInfo->timeout + 1)
			if((int)time(NULL) - g_last_cmd_time > 180) // 三分钟没有交易重置指令缓存数为0
			{
				UnionResetCurrentIndex();
				UnionLog("in UnionSendMsgToHsm:: hsm[%s] timeout[%d] and UnionResetCurrentIndex!\n",  phsmInfo->ipAddr, phsmInfo->timeout);
				break;
			}
			if(tryTimes++ > phsmInfo->timeout * 1000) // 发送缓冲数已满，每1ms检查一次是否已释放出空间，如检查时间超过timeout值则线程终止
			{
				
				UnionUserErrLog("in UnionSendMsgToHsm:: g_current_buf_num[%d] > hsm_message_size[%d] wait timeout[%ds]!\n", 
					currentIndex, hsm_message_size, phsmInfo->timeout);
				UnionLetMainProcessStopAll();
				return(NULL);
			}
			usleep(1000);
		}
			
		g_last_cmd_time = (int)time(NULL);
		UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfHsmSvr);
		if ((len = UnionReadRequestToSpecifiedModuleWithMsgHeader(phsmInfo->mdlID,msg,sizeof(msg),&g_hsm_message[index].applierMDLID,&g_hsm_message[index].msgHeader)) < 0)
		{
			UnionUserErrLog("in UnionSendMsgToHsm:: UnionReadRequestToSpecifiedModuleWithMsgHeader [%ld]!\n",phsmInfo->mdlID);
			//continue;
			UnionLetMainProcessStopAll();
		}
		//UnionLog("in UnionSendMsgToHsm:: recvMsg::: [%.*s...]!\n", len, msg);

		reqBuf[0] = 0x00;
		reqBuf[1] = 0x0A;
		sprintf((char *)reqBuf + phsmInfo->lenOfCommHead + hsm_message_head_buf_index_len_offset, "%0*d00000000000000000000", hsm_message_head_buf_index_len, index);
	
		//phsmInfo->cmdPrefix[0] = 0;
		// 检查打印格式
		offset = 0;
		if(len > offset + 5 && memcmp(msg + offset, "PRTT", 4) == 0)
		{
			g_hsm_message[index].packagePrintType = msg[4];
			//memcpy(phsmInfo->cmdPrefix, msg + offset, 5);
			//phsmInfo->cmdPrefix[5] = 0;
			offset += 5;
		}
		else
			g_hsm_message[index].packagePrintType = '0';

		// 检查是否指定IP地址
		if (len > offset + 5 + 2 && memcmp(msg + offset,"HSMIP",5) == 0)
		{
			//memcpy(phsmInfo->cmdPrefix + strlen(phsmInfo->cmdPrefix), msg + offset, 5);
			offset += 5;
			memcpy(buf,msg + offset,2);
			buf[2] = 0;
			//memcpy(phsmInfo->cmdPrefix + strlen(phsmInfo->cmdPrefix), msg + offset, 2);
			offset += 2;
			ret = atoi(buf);
			memcpy(specIPAddr,msg + offset,ret);
			specIPAddr[ret] = 0;
			//memcpy(phsmInfo->cmdPrefix + strlen(phsmInfo->cmdPrefix), msg + offset, ret);
			offset += ret;


			// 读取指令头
			memcpy(g_hsm_message[index].hsmCmdHead, msg + offset, 2);
			g_hsm_message[index].hsmCmdHead[phsmInfo->lenOfCommHead] = 0;
						
			// IP地址和现在连接的加密机不同
			if (strcmp(specIPAddr,phsmInfo->ipAddr) != 0)
			{
				tryTimes = 0;
				while(tryTimes++ < phsmInfo->timeout * 2)
				{
					if((ret = UnionSetCmdToOtherHsmBuf(specIPAddr, msg + offset, len - offset, &(g_hsm_message[index]))) >= 0)
					{
						UnionLetMainProcessSendOther();
						break;
					}
					UnionLog("in UnionSendMsgToHsm:: current hsm[%s] ceate temp connect to spec hsm[%s] tryTimes = %d!!\n", phsmInfo->ipAddr, specIPAddr, tryTimes);
					usleep(500000);
				}
				if(ret < 0)
				{
					UnionUserErrLog("in UnionSendMsgToHsm:: UnionSetCmdToOtherHsmBuf [%s] time out ret = %d!!\n", specIPAddr, ret);
				}
				else
				{
					// 成功发送一个其它hsm报文给主进程,缓冲区++
					UnionIncreaseIndex();
					UnionSetOtherIndex();
					//UnionIncreaseCommConfWorkingTimes(pgThisCommConf);

					if (++g_hsm_message_no == hsm_message_size)
					{
						g_hsm_message_no = 0;
					}
				}
				if (UnionIsFldStrInUnionFldListStr(phsmInfo->sensitiveCmd,strlen(phsmInfo->sensitiveCmd),',',g_hsm_message[index].hsmCmdHead) <= 0)	
				{
					UnionPrintHsmReqLog(phsmInfo->mdlID, g_hsm_message[index].msgHeader.provider, specIPAddr, phsmInfo->port, g_hsm_message[index].packagePrintType, len - msgOffset, (char *)reqBuf + msgOffset);
				}
				else
				{
					UnionPrintHsmReqLog(phsmInfo->mdlID, g_hsm_message[index].msgHeader.provider, specIPAddr, phsmInfo->port, '8', len - msgOffset, (char *)reqBuf + msgOffset);
				}
				UnionLog("in UnionSendMsgToHsm:: end of spec hsm[%s]!!\n", specIPAddr);
				continue;
	
			}
		}
		len -= offset;
		if(len < 2)
		{
			UnionUserErrLog("in UnionSendMsgToHsm:: cmdLen[%d] < 2!\n", len);
			continue;
		}
		memcpy(reqBuf + phsmInfo->lenOfCommHead + phsmInfo->lenOfMsgHead,msg + offset,len);

		len += msgOffset;
		reqBuf[len] = 0;
		
		// 读取指令头
		switch(phsmInfo->hsmCmdVersionID)
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

		if (lenOfHsmCmdHead > 0)
		{
			memcpy(g_hsm_message[index].hsmCmdHead, reqBuf + msgOffset, lenOfHsmCmdHead);
			g_hsm_message[index].hsmCmdHead[lenOfHsmCmdHead] = 0;
		}
		else
			g_hsm_message[index].hsmCmdHead[0] = 0;

		if (UnionIsFldStrInUnionFldListStr(phsmInfo->sensitiveCmd,strlen(phsmInfo->sensitiveCmd),',',g_hsm_message[index].hsmCmdHead) <= 0)	
			UnionPrintHsmReqLog(phsmInfo->mdlID, g_hsm_message[index].msgHeader.provider, phsmInfo->ipAddr, phsmInfo->port, g_hsm_message[index].packagePrintType, len - msgOffset, (char *)reqBuf + msgOffset);
		else
			UnionPrintHsmReqLog(phsmInfo->mdlID, g_hsm_message[index].msgHeader.provider, phsmInfo->ipAddr, phsmInfo->port, '8', len - msgOffset, (char *)reqBuf + msgOffset);
		
		if (len <= msgOffset)
			continue;

		if (phsmInfo->lenOfCommHead == 2)
		{
			reqBuf[0] = (len - phsmInfo->lenOfCommHead) / 256;
			reqBuf[1] = (len - phsmInfo->lenOfCommHead) % 256;
		}
		else if (phsmInfo->lenOfCommHead > 0)
		{
			sprintf((char *)msg,"%0*d", phsmInfo->lenOfCommHead, len - phsmInfo->lenOfCommHead);
			memcpy(reqBuf, msg, phsmInfo->lenOfCommHead);
		}

		UnionProgramerLog("in UnionSendMsgToHsm:: sendCmd::: [%.*s...]!\n", phsmInfo->lenOfMsgHead + 2, reqBuf + phsmInfo->lenOfCommHead);
		g_hsm_message[index].used = 0;
		ret = UnionSendToSocket(phsmInfo->sckHDL, reqBuf, len);
		if (ret < 0)
		{
			UnionUserErrLog("in UnionSendMsgToHsm:: UnionSendToSocket sock = %d ret = [%d] reqStr[%s]!\n", phsmInfo->sckHDL, ret, reqBuf + phsmInfo->lenOfCommHead);
			UnionLetMainProcessStopAll();
			return(NULL);
		}

		// 成功发送一个socket报文,缓冲区++
		UnionIncreaseIndex();
		//UnionIncreaseCommConfWorkingTimes(pgThisCommConf);

		if (++g_hsm_message_no == hsm_message_size)
		{
			g_hsm_message_no = 0;
		}

		continue;
	}
	return(NULL);
}

void *UnionRecvMsgFromHsm(void *p)
{
	int			ret;
	int			index;
	int			len = 0;
	int			msgOffset = 0;
	unsigned char		resBuf[max_hsm_message_len];
	unsigned char		tmpBuf[128];
	PUnionHsmInfo		phsmInfo;
	
	PUnionHsmThreads      	pthread = (PUnionHsmThreads)p;
	
	phsmInfo = pthread->phsmInfo;
	
	msgOffset = phsmInfo->lenOfCommHead + phsmInfo->lenOfMsgHead;

	while(1)
	{	
		resBuf[max_hsm_message_len-1] = 0;
		UnionSetSockSendTimeout(phsmInfo->sckHDL, 0);
		if (phsmInfo->lenOfCommHead == 2)
		{
			if ((ret = UnionReceiveFromSocketUntilLen(phsmInfo->sckHDL,resBuf, phsmInfo->lenOfCommHead)) != phsmInfo->lenOfCommHead)
			{
				UnionUserErrLog("in UnionRecvMsgFromHsm:: UnionReceiveFromSocketUntilLen read len = %d!, ret = %d\n", phsmInfo->lenOfCommHead, ret);
				//UnionUserErrLog("in UnionRecvMsgFromHsm:: UnionReceiveFromSocket! ret = %d\n", ret);
				break;
			}
			if (ret < phsmInfo->lenOfCommHead)
			{
				UnionUserErrLog("in UnionRecvMsgFromHsm:: UnionReceiveFromSocket! ret[%d] < lenOfCommHead[%d]!\n", ret, phsmInfo->lenOfCommHead);
				break;	
			}
		}
		else if (phsmInfo->lenOfCommHead == 0)
		{
			if ((ret = UnionReceiveFromSocketUntilLen(phsmInfo->sckHDL,resBuf, 1)) != 1)
                        {
                                UnionUserErrLog("in UnionRecvMsgFromHsm:: UnionReceiveFromSocketUntilLen read len = 1!, ret = %d\n", ret);
                                //UnionUserErrLog("in UnionRecvMsgFromHsm:: UnionReceiveFromSocket! ret = %d\n", ret);
                                //break;
                                continue;
                        }

			if ((ret = UnionReceiveFromSocket(phsmInfo->sckHDL,resBuf+1, sizeof(resBuf) - 1 - 1)) < 0)
			{
				//UnionUserErrLog("in UnionRecvMsgFromHsm:: UnionReceiveFromSocketUntilLen read len = %d!, ret = %d\n", phsmInfo->lenOfCommHead, ret);
				UnionUserErrLog("in UnionRecvMsgFromHsm:: UnionReceiveFromSocket! ret = %d\n", ret);
				break;
			}
			if (ret < phsmInfo->lenOfCommHead)
			{
				UnionUserErrLog("in UnionRecvMsgFromHsm:: UnionReceiveFromSocket! ret[%d] < lenOfCommHead[%d]!\n", ret, phsmInfo->lenOfCommHead);
				break;	
			}
		}

		if (phsmInfo->lenOfCommHead == 2)
			len = resBuf[0] * 256 + resBuf[1];
		else if (phsmInfo->lenOfCommHead <= 0)
			len = ret;
		else
		{
			memcpy(tmpBuf, resBuf, phsmInfo->lenOfCommHead);
			tmpBuf[phsmInfo->lenOfCommHead] = 0;
			len = atoi((char *)tmpBuf);
		}
			
		if (phsmInfo->lenOfCommHead == 2)
		{
			UnionSetSockSendTimeout(phsmInfo->sckHDL, phsmInfo->timeout);
			if ((ret = UnionReceiveFromSocketUntilLen(phsmInfo->sckHDL, resBuf + phsmInfo->lenOfCommHead, len)) != len)
			{
				UnionUserErrLog("in UnionRecvMsgFromHsm:: UnionReceiveFromSocketUntilLen read len = %d, ret = %d!!\n", len, ret);
				break;
			}
		}

		UnionProgramerLog("in UnionRecvMsgFromHsm:: recvCmd::: [%.*s...]!\n", phsmInfo->lenOfMsgHead + 2 + 2, resBuf + phsmInfo->lenOfCommHead);


		resBuf[len + phsmInfo->lenOfCommHead] = 0;

		// 解释读回来的index
		memcpy(tmpBuf, resBuf + phsmInfo->lenOfCommHead + hsm_message_head_buf_index_len_offset, hsm_message_head_buf_index_len);
		tmpBuf[hsm_message_head_buf_index_len] = 0;
		index = atoi((char *)tmpBuf);

		if(index >= hsm_message_size)
		{
			UnionUserErrLog("in UnionRecvMsgFromHsm:: index = [%d] >= m_message_size[%d] illeage!\n", index, hsm_message_size);
			ret = errCodeParameter;
			break;
		}

		if(g_hsm_message[index].used == 1) // 判断指令2次返回 丢弃
		{
			UnionLog("in UnionRecvMsgFromHsm:: warning hsm[%s] res cmd[%.2s] recv second times!!\n", phsmInfo->ipAddr, resBuf + phsmInfo->lenOfCommHead);
			continue;
		}
		g_hsm_message[index].used = 1;

		// 接收到一个socket报文，缓冲区--
		UnionDecreaseIndex();
				
		if (UnionIsFldStrInUnionFldListStr(phsmInfo->sensitiveCmd,strlen(phsmInfo->sensitiveCmd),',',g_hsm_message[index].hsmCmdHead) <= 0)	
		 	UnionPrintHsmResLog(phsmInfo->mdlID, g_hsm_message[index].msgHeader.provider, phsmInfo->ipAddr, phsmInfo->port, g_hsm_message[index].packagePrintType, len - msgOffset, (char *)resBuf + msgOffset);
		else
		 	UnionPrintHsmResLog(phsmInfo->mdlID, g_hsm_message[index].msgHeader.provider, phsmInfo->ipAddr, phsmInfo->port, '8', len - msgOffset, (char *)resBuf + msgOffset);
		
		len -= phsmInfo->lenOfMsgHead;

		//UnionLog("in UnionRecvMsgFromHsm:: sendMsg::: [%.2s...]!\n", resBuf + phsmInfo->lenOfCommHead);
		UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfHsmSvr);
		if ((ret = UnionSendResponseToApplyModuleWithOriMsgHeader(g_hsm_message[index].applierMDLID,resBuf + msgOffset,len,&g_hsm_message[index].msgHeader)) < 0)
		{
			UnionUserErrLog("in UnionRecvMsgFromHsm:: UnionSendResponseToApplyModuleWithOriMsgHeader [%ld]!\n",g_hsm_message[index].applierMDLID);
			continue;
		}
	}
	UnionUserErrLog("in UnionRecvMsgFromHsm:: ret [%d]\n", ret);
	UnionLetMainProcessStopAll();
	return(NULL);
}
void *UnionOtherHsmIPDealer(void *p)
{
	int			ret;
	int			len = 0;
	unsigned char		reqBuf[max_hsm_message_len];
	unsigned char		resBuf[max_hsm_message_len];
	unsigned char		tmpBuf[32];
	PUnionHsmInfo		phsmInfo;
	
	TUnionOtherCmdBuf	*potherCmdBuf = (TUnionOtherCmdBuf *)p;
	
	phsmInfo = potherCmdBuf->phsmInfo;

	if(potherCmdBuf->sepcRet < 0)
	{
		phsmInfo = &hsmInfo;
		goto sendResTag;
	}
	UnionLog("in UnionOtherHsmIPDealer:: son dealer read hsm[%s:%d]sock=%d req = [%d:%.2s...]\n", phsmInfo->ipAddr, phsmInfo->port, phsmInfo->sckHDL, potherCmdBuf->cmdLen, potherCmdBuf->cmd);

	if (phsmInfo->lenOfCommHead == 2)
	{
		reqBuf[0] = (potherCmdBuf->cmdLen + phsmInfo->lenOfMsgHead) / 256;
		reqBuf[1] = (potherCmdBuf->cmdLen + phsmInfo->lenOfMsgHead) % 256;
	}
	else
	{
		snprintf((char *)reqBuf,sizeof(reqBuf),"%0*d", phsmInfo->lenOfCommHead, potherCmdBuf->cmdLen);
	}
	memset(reqBuf + phsmInfo->lenOfCommHead, '0',  phsmInfo->lenOfMsgHead);
	memcpy(reqBuf + phsmInfo->lenOfCommHead + phsmInfo->lenOfMsgHead, potherCmdBuf->cmd, potherCmdBuf->cmdLen);
	
	ret = UnionExchangCmdWithHsm(phsmInfo->sckHDL, phsmInfo->lenOfCommHead, reqBuf, potherCmdBuf->cmdLen + phsmInfo->lenOfCommHead + phsmInfo->lenOfMsgHead, resBuf, sizeof(resBuf));
	Uclose(phsmInfo->sckHDL);
	if(ret < 0)
	{
		UnionUserErrLog("in UnionOtherHsmIPDealer:: UnionExchangCmdWithHsm read sck = %d len = %d!, ret = %d \n", phsmInfo->sckHDL, phsmInfo->lenOfCommHead, ret);
		goto sendResTag;
	}
	UnionLog("in UnionOtherHsmIPDealer:: UnionExchangCmdWithHsm read sck = %d len = %d!, ret = %d \n", phsmInfo->sckHDL, phsmInfo->lenOfCommHead, ret);

	if (phsmInfo->lenOfCommHead == 2)
	{
		len = resBuf[0] * 256 + resBuf[1];
	}
	else
	{
		memcpy(tmpBuf, resBuf, phsmInfo->lenOfCommHead);
		tmpBuf[phsmInfo->lenOfCommHead] = 0;
		len = atoi((char *)tmpBuf);
	}
				
	if (UnionIsFldStrInUnionFldListStr(phsmInfo->sensitiveCmd,strlen(phsmInfo->sensitiveCmd),',', potherCmdBuf->phsmMsg->hsmCmdHead) <= 0)	
	 	UnionPrintHsmResLog(phsmInfo->mdlID, potherCmdBuf->phsmMsg->msgHeader.provider, phsmInfo->ipAddr, phsmInfo->port, potherCmdBuf->phsmMsg->packagePrintType, len - phsmInfo->lenOfMsgHead, (char *)resBuf + phsmInfo->lenOfCommHead + phsmInfo->lenOfMsgHead);
	else
	 	UnionPrintHsmResLog(phsmInfo->mdlID, potherCmdBuf->phsmMsg->msgHeader.provider, phsmInfo->ipAddr, phsmInfo->port, '8', len - phsmInfo->lenOfMsgHead, (char *)resBuf + phsmInfo->lenOfCommHead + phsmInfo->lenOfMsgHead);
	
sendResTag:
	if(potherCmdBuf->sepcRet < 0 || ret < 0)
	{
		len = phsmInfo->lenOfCommHead;
		memset(resBuf + len, '0', phsmInfo->lenOfMsgHead);
		len += phsmInfo->lenOfMsgHead;
		memcpy(resBuf + len, potherCmdBuf->phsmMsg->hsmCmdHead, 2);
		len += 2;
		resBuf[len - 1]++;
		memcpy(resBuf + len, HSM_CONNECT_ERR, 2);
		len += 2;
	}
	
	UnionResetOtherIndex();
	UnionLog("in UnionOtherHsmIPDealer:: sendMsg::: [%.4s...]!\n", resBuf + phsmInfo->lenOfCommHead + phsmInfo->lenOfMsgHead);
	UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfHsmSvr);
	if ((ret = UnionSendResponseToApplyModuleWithOriMsgHeader(potherCmdBuf->phsmMsg->applierMDLID, resBuf + phsmInfo->lenOfCommHead + phsmInfo->lenOfMsgHead, len - phsmInfo->lenOfMsgHead, &(potherCmdBuf->phsmMsg->msgHeader))) < 0)
	{
		UnionUserErrLog("in UnionOtherHsmIPDealer:: hsmip = [%s]UnionSendResponseToApplyModuleWithOriMsgHeader [%ld]!\n", phsmInfo->ipAddr, potherCmdBuf->phsmMsg->applierMDLID);
		return(NULL);
	}
	return(NULL);
}
void UnionDoMainProcessSendOther()
{
	int			ret, index = 0;
	pthread_attr_t		attr;

	// 取当前发送信息
	if ((ret =  UnionReadActiveHsmInfoByHsmIP(g_other_cmd_buf[index].ip, &g_hsmInfo)) <= 0)
	{
		UnionUserErrLog("in UnionDoMainProcessSendOther:: UnionReadActiveHsmInfoByHsmIP[%s]!\n", g_other_cmd_buf[index].ip);
		goto createThreadTag;
	}

	// 连接密码机
	if ((g_hsmInfo.sckHDL = UnionCheckAndConnHsm(&g_hsmInfo, 0)) <= 0)
	{
		UnionUserErrLog("in UnionDoMainProcessSendOther:: UnionCheckAndConnHsm hsm[%s:%d] ret = %d!\n", g_hsmInfo.ipAddr, g_hsmInfo.port, g_hsmInfo.sckHDL);
		goto createThreadTag;
	}

	// 启动发送线程 UnionOtherHsmIPDealer
	g_other_cmd_buf[index].thread_no = 3;
	g_other_cmd_buf[index].phsmInfo = &g_hsmInfo;

	//UnionLog("in UnionDoMainProcessSendOther:: pthread_create UnionOtherHsmIPDealer hsm[%s]!\n", g_hsmInfo.ipAddr);

	g_other_cmd_buf[index].sepcRet = 0;



createThreadTag:
	if(ret <= 0)
	{
		g_other_cmd_buf[index].sepcRet = errCodeHsmCmdMDL_HsmNotAvailable;
	}

	// for thread
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if ((ret = pthread_create( &(g_other_cmd_buf[index].thread_id), &attr, UnionOtherHsmIPDealer , &g_other_cmd_buf[index] )) != 0)
	{
		UnionUserErrLog("in UnionDoMainProcessSendOther:: pthread_create UnionOtherHsmIPDealer[%s] ret = %d!\n",  g_other_cmd_buf[index].ip, ret);
		Uclose(g_hsmInfo.sckHDL);
		UnionDecreaseIndex();
		UnionResetOtherIndex();
		return;
	}
	UnionDecreaseIndex();

	UnionLog("in UnionDoMainProcessSendOther:: pthread_create UnionOtherHsmIPDealer[%s] ret = %d OK!\n",  g_other_cmd_buf[index].ip, ret);
		
	return;
}

// 维护子维护, 程序运行无异常时返回0,
int UnionHsmSvrMaintain(PUnionHsmInfo phsmInfo)
{
	int		sockHdl = 0;
	
	// check normal hsm 
	// 拼装测试的请求和响应报文
	if ((sockHdl = UnionCheckAndConnHsm(phsmInfo, 1)) > 0)
	{
		Uclose(sockHdl);
		UnionUpdateHsmStatus(phsmInfo->hsmID, 1);
	}
	else
	{
		UnionUpdateHsmStatus(phsmInfo->hsmID, 0);
		UnionUserErrLog("in UnionHsmSvrMaintain:: test hsm[%s:%d] err! ret = %d\n", phsmInfo->ipAddr, phsmInfo->port, sockHdl);
		return(-2);
	}
	return(0);
}

int UnionThreadMaintain(char *ipAddr)
{
	// check thread 
	if (pthread_kill(g_hsm_threads[0].thread_id, 0) || pthread_kill(g_hsm_threads[1].thread_id, 0)) // 线程不存在 停止所有
	{
		UnionUserErrLog("in UnionThreadMaintain:: hsm[%s] thread err!\n", ipAddr);
		return(-1);
	}
	return(0);
}

int main(int argc,char *argv[])
{
	int			ret;
	char			taskName[128];
	PUnionTaskInstance	ptaskInstance = NULL;
	TUnionHsmInfo		tmp_hsmInfo;
	int			times = 0;
	
	UnionSetApplicationName(argv[0]);

	if (argc < 2)
		return(UnionHelp());

	strcpy(hsmInfo.hsmID, argv[1]);
	
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	if (UnionCreateProcess() > 0)
		return(0);
	
	snprintf(taskName,sizeof(taskName),"%s %s",UnionGetApplicationName(), hsmInfo.hsmID);
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",taskName)) == NULL)
	{
		printf("UnionCreateTaskInstance[%s] Error!\n",taskName);
		return(0);
	}
	
	UnionSetSuffixOfMyLogFile(hsmInfo.hsmID);

	while(1)
	{
		// 读取密码机信息
		if ((ret =  UnionReadActiveHsmInfoByHsmID(hsmInfo.hsmID, &hsmInfo)) <= 0)
		{
			UnionUserErrLog("in %s:: UnionReadActiveHsmInfoByHsmID[%s]!\n",UnionGetApplicationName(), hsmInfo.hsmID);
			sleep(30);
			continue;
		}
	
		if (hsmInfo.lenOfMsgHead < hsm_message_head_buf_index_len + hsm_message_head_buf_index_len_offset)
		{
			UnionUserErrLog("in %s:: lenOfMsgHead[%d] < hsm_message_head_buf_index_len[%d]!\n", UnionGetApplicationName(), hsmInfo.lenOfMsgHead, hsm_message_head_buf_index_len + hsm_message_head_buf_index_len_offset);
			UnionUpdateHsmStatus(hsmInfo.hsmID, 0);
			sleep(30);
			continue;
		}
	
		// 连接密码机
		if ((ret = UnionCheckAndConnHsm(&hsmInfo, 1)) <= 0)
		{
			UnionUserErrLog("in %s:: UnionCheckAndConnHsm hsm[%s:%d] ret = %d!\n",UnionGetApplicationName(), hsmInfo.ipAddr, hsmInfo.port, ret);
			UnionUpdateHsmStatus(hsmInfo.hsmID, 0);
			sleep(10);
			continue;;
		}
		else
		{
			hsmInfo.sckHDL = ret;
			break;
		}
		
	}
	memcpy(&tmp_hsmInfo, &hsmInfo, sizeof(TUnionHsmInfo));

	// 重置当前socket报文数目
	UnionResetCurrentIndex();
	UnionResetOtherIndex();

	// 处理其它IP的密码机指令
	signal(SIG_CMD_TO_MAIN_PROCESSS_SEND_OTHER, UnionDoMainProcessSendOther);
	// 停止进程的处理
	signal(SIG_CMD_TO_MAIN_PROCESSS_STOP_ALL, UnionDoMainProcessStopAll);	

	// 启动发送线程 UnionSendMsgToHsm
	g_hsm_threads[0].thread_no = 0;
	g_hsm_threads[0].phsmInfo = &hsmInfo;
	UnionLog("in %s:: pthread_create UnionSendMsgToHsm !\n",UnionGetApplicationName());
	if ((ret = pthread_create( &(g_hsm_threads[0].thread_id) , NULL , UnionSendMsgToHsm , & g_hsm_threads[0] )) != 0)
	{
		UnionUserErrLog("in %s:: pthread_create UnionSendMsgToHsm!\n",UnionGetApplicationName());
		Uclose(hsmInfo.sckHDL);
		return(UnionTaskActionBeforeExit());
	}	

	// 启动接收线程 UnionRecvMsgFromHsm
	g_hsm_threads[1].thread_no = 1;
	g_hsm_threads[1].phsmInfo = &hsmInfo;
	UnionLog("in %s:: pthread_create UnionRecvMsgFromHsm!\n",UnionGetApplicationName());
	if ((ret = pthread_create( &(g_hsm_threads[1].thread_id) , NULL , UnionRecvMsgFromHsm , & g_hsm_threads[1] )) != 0)
	{
		UnionUserErrLog("in %s:: pthread_create UnionRecvMsgFromHsm!\n",UnionGetApplicationName());
		Uclose(hsmInfo.sckHDL);
		return(UnionTaskActionBeforeExit());		
	}
	
	// 以下主线程执行
	while(1)
	{
		times = 0;
		while(times++ < 10)
		{
			if ((ret = UnionThreadMaintain(tmp_hsmInfo.ipAddr)) == 0) // 正常
			{
				usleep(100000);  // 10秒 = 10 * 1000000
				continue;
			}
			else
			{
				break;	
			}
		}
		if(ret != 0)
		{
			break;
		}

		if ((ret = UnionHsmSvrMaintain(&tmp_hsmInfo)) == 0) // 正常
		{
			continue;
		}
		else // 异常
		{
			UnionUserErrLog("in %s:: UnionHsmSvrMaintain[%s:%d] err ret = %d!!\n", UnionGetApplicationName(), hsmInfo.ipAddr, hsmInfo.port, ret);
			Uclose(hsmInfo.sckHDL);
			break;
		}
	}
	UnionUserErrLog("in %s:: UnionHsmSvrMaintain[%s:%d] err ret = %d!!\n", UnionGetApplicationName(), hsmInfo.ipAddr, hsmInfo.port, ret);
	return(UnionTaskActionBeforeExit());
}
