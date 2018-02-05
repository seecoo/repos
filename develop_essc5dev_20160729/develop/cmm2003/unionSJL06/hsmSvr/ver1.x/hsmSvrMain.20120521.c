// Copyright:	Union Tech.
// Author:	zhangyd
// Date:	2010-10-23
// Version:	2.0

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
#include "UnionStr.h"
#include "UnionLog.h"
#include "unionVersion.h"
#include "unionRealBaseDB.h"
#include "unionCommand.h"
#include "unionRealDBCommon.h"
#include "unionHsmCmdVersion.h"

#include "unionHsm.h"
#include "unionHsmGroup.h"

jmp_buf	gunionHsmJmpEnv;
jmp_buf	gunionHsmTestJmpEnv;
void	UnionDealHsmTestingPackage();
void	UnionDealHsmResponseTimeout();

int UnionReadRecFldFromRecStr(char *record, int rLen, char *fldName, char *buf, int bufSize);
char *UnionGetPrefixOfDBField();
int UnionIsValidIPAddrStr(char *ip);

PUnionCommConf		pgThisCommConf = NULL;

PUnionTaskInstance	ptaskInstance = NULL;
int			gunionHsmSckHDL = -1;

int			gunionIsTestHsmCmd = 0;
int			gunionISConnectHsmOK = 0;

int UnionHelp()
{
	printf("Usage:: %s hsmIPAddr\n",UnionGetApplicationName());
	return(0);
}

int UnionTaskActionBeforeExit()
{
	if (gunionISConnectHsmOK)
		UnionSetCommConfAbnormal(pgThisCommConf);
	if (gunionHsmSckHDL >= 0)
		UnionCloseSocket(gunionHsmSckHDL);
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

void UnionPrintHsmReqLog(long mdlid, int provider, char *ip, int port, int printType, int len, char *str)
{
	char		*ptr = NULL;
	char		buf[8192];

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
	char		buf[8192];

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
		memset(buf, 0, sizeof(buf));
		bcdhex_to_aschex(str, len, buf);
		ptr = buf;
		len *= 2;
	}

	UnionLog("res[%d][%ld][%s:%d]:: [%2.2s][%04d][%s]\n", provider, mdlid, ip, port, str, len, ptr);

	return;

}


int UnionReadHsmRecFromHsm(char *hsmID,PUnionHsm pHsmRec,PUnionHsmGroup pHsmGroup)
{
	int				ret;
	char				sql[512];
	char				record[256];
	char				tmpBuf[32];
	char				*pdbFldPrefix = NULL;
	char				*BLANKVALUE = "";

	if((pdbFldPrefix = UnionGetPrefixOfDBField()) == NULL)
	{
		pdbFldPrefix = BLANKVALUE;
	}

	strcpy(pHsmRec->ipAddr, hsmID);
	memset(sql, 0, sizeof(sql));

	sprintf(sql, "select hsm.%shsmGrpID, %sport, %shsmStatusID, %shsmCmdVersionID, %slenOfLenFld, %slenOfMsgHeader, %smsgHeader, %stestCmdReq, %stestCmdSuccessRes from hsm, hsmGroup where hsm.%shsmGrpID = hsmGroup.%shsmGrpID and hsm.%sactive = 1 and %sipAddr = '%s'", pdbFldPrefix, pdbFldPrefix, pdbFldPrefix, pdbFldPrefix, pdbFldPrefix, pdbFldPrefix, pdbFldPrefix, pdbFldPrefix, pdbFldPrefix, pdbFldPrefix, pdbFldPrefix, pdbFldPrefix, pdbFldPrefix, pHsmRec->ipAddr);

	memset(record, 0, sizeof(record));
	if((ret = UnionOpenRealDBSql(sql, record, sizeof(record) -1 )) < 0)
	{
		UnionAuditLog("in UnionReadHsmRecFromHsm:: UnionOpenRealDBSql [%s] ret = [%d]!\n", sql, ret);
		return(ret);
	}

	//select hsm.unhsmGrpID, unport, unhsmStatusID, unhsmCmdVersionID, unlenOfLenFld, unlenOfMsgHeader, unmsgHeader, untestCmdReq, untestCmdSuccessRes from hsm, hsmGroup where hsm.unhsmGrpID = hsmGroup.unhsmGrpID and hsm.unactive = 1 and unipAddr = '192.1.2.207'
	//[FIELD0=102|FIELD1=8|FIELD2=1|FIELD3=1273|FIELD4=2|FIELD5=8|FIELD6=|FIELD7=|FIELD8=|]
	
	// 读取密码机组号
	if((ret = UnionReadRecFldFromRecStr(record, strlen(record), "FIELD0", pHsmGroup->hsmGrpID, sizeof(pHsmGroup->hsmGrpID))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadRecFldFromRecStr FIELD0 hsm hsmGroup ret = [%d]!\n", ret);
		return(ret);
	}
	strcpy(pHsmRec->hsmGrpID,pHsmGroup->hsmGrpID);

	// 读取密码机端口
	memset(tmpBuf,0,sizeof(tmpBuf));
	if((ret = UnionReadRecFldFromRecStr(record, strlen(record), "FIELD1", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadRecFldFromRecStr FIELD1 hsm port ret = [%d]!\n", ret);
		return(ret);
	}
	pHsmRec->port = atoi(tmpBuf);

	// 读取密码机状态
	memset(tmpBuf,0,sizeof(tmpBuf));
	if((ret = UnionReadRecFldFromRecStr(record, strlen(record), "FIELD2", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadRecFldFromRecStr FIELD2 hsm hsmStatusID ret = [%d]!\n", ret);
		return(ret);
	}
	pHsmRec->hsmStatusID = atoi(tmpBuf);

	// 读取密码机指令类型
	memset(tmpBuf,0,sizeof(tmpBuf));
	if((ret = UnionReadRecFldFromRecStr(record, strlen(record), "FIELD3", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadRecFldFromRecStr FIELD3 hsm hsmCmdVersionID ret = [%d]!\n", ret);
		return(ret);
	}
	pHsmGroup->hsmCmdVersionID = atoi(tmpBuf);

	// 读取通讯头长度
	memset(tmpBuf,0,sizeof(tmpBuf));
	if((ret = UnionReadRecFldFromRecStr(record, strlen(record), "FIELD4", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadRecFldFromRecStr FIELD4 hsm lenOfLenFld ret = [%d]!\n", ret);
		return(ret);
	}
	pHsmGroup->lenOfLenFld = atoi(tmpBuf);

	// 读取消息头长度
	memset(tmpBuf,0,sizeof(tmpBuf));
	if((ret = UnionReadRecFldFromRecStr(record, strlen(record), "FIELD5", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadRecFldFromRecStr FIELD5 hsm lenOfMsgHeader ret = [%d]!\n", ret);
		return(ret);
	}
	pHsmGroup->lenOfMsgHeader = atoi(tmpBuf);

	// 读取消息头
	if((ret = UnionReadRecFldFromRecStr(record, strlen(record), "FIELD6", pHsmGroup->msgHeader, sizeof(pHsmGroup->msgHeader))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadRecFldFromRecStr FIELD6 hsm MsgHeader ret = [%d]!\n", ret);
		return(ret);
	}

	// 读取探测请求指令
	if((ret = UnionReadRecFldFromRecStr(record, strlen(record), "FIELD7", pHsmGroup->testCmdReq, sizeof(pHsmGroup->testCmdReq))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadRecFldFromRecStr FIELD7 hsm testCmdReq ret = [%d]!\n", ret);
		return(ret);
	}

	// 读取探测响应指令
	if((ret = UnionReadRecFldFromRecStr(record, strlen(record), "FIELD8", pHsmGroup->testCmdSuccessRes, sizeof(pHsmGroup->testCmdSuccessRes))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmRecFromHsm:: UnionReadRecFldFromRecStr FIELD8 hsm testCmdSuccessRes ret = [%d]!\n", ret);
		return(ret);
	}

	return(1);
}

int main(int argc,char *argv[])
{
	int			ret;
	int			len = 0;
	int			firstFlag = 1;
	int			offset = 0;
	int			timeoutOfHsm;
	int			maxFreeTime;
	int			lenOfHsmCmdHead = 2;
	int			trySendFlag = 0;
	int			isConnectSpecHsm = 0;
	char			hsmID[16+1];
	char			hsmCmdHead[16+1];
	char			taskName[128+1];
	char			buf[128+1];
	char			specIPAddr[32+1];
	char			msg[8192+1];
	unsigned char		tmpBuf[8192+1];
	unsigned char		lenBuf[8+1];
	TUnionModuleID		mdlidOfHsm = -1;
	TUnionModuleID		applierMDLID;
	TUnionMessageHeader	msgHeader;
	TUnionHsm		hsmRec;
	TUnionHsmGroup		hsmGroup;
	int                     packagePrintType = '0'; // '1':请求BCD格式打印，'2':响应BCD格式打印，'3':请求响应都用BCD格式打印
	char			tmpStr[8192+1];
		
	if (argc < 2)
		return(UnionHelp());
	
	UnionSetApplicationName(argv[0]);

	memset(hsmID,0,sizeof(hsmID));
	strcpy(hsmID,argv[1]);

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	if(!UnionIsValidIPAddrStr(hsmID))
	{
		printf("in [%s]:: arg1[%s] UnionIsInvalidIPAddrStr!\n", UnionGetApplicationName(), hsmID);
		return(-1);
	}

	if (UnionCreateProcess() > 0)
		return(0);

	memset(taskName,0,sizeof(taskName));
	sprintf(taskName,"%s %s",UnionGetApplicationName(),hsmID);
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",taskName)) == NULL)
	{
		printf("UnionCreateTaskInstance[%s] Error!\n",taskName);
		return(0);
	}

	
againConnectHsm:
	memset(&hsmRec,0,sizeof(hsmRec));
	memset(&hsmGroup,0,sizeof(hsmGroup));
	if ((ret =  UnionReadHsmRecFromHsm(hsmID,&hsmRec,&hsmGroup)) <= 0)
	{
		UnionUserErrLog("in %s:: UnionReadHsmRecFromHsm[%s]!\n",UnionGetApplicationName(),hsmID);
		return(UnionTaskActionBeforeExit());
	}
	
	if ((pgThisCommConf = UnionAddClientCommConf(hsmRec.ipAddr,hsmRec.port,conCommLongConn,"密码机")) == NULL)
	{
		printf("UnionAddClientCommConf for [%s] [%d]\n",hsmRec.ipAddr,hsmRec.port);
		return(UnionTaskActionBeforeExit());
	}

	if ((hsmGroup.hsmCmdVersionID == conHsmCmdVerSJJ11270StandardHsmCmd) ||
		(hsmGroup.hsmCmdVersionID == conHsmCmdVerSJJ1127SStandardHsmCmd))
	{
		lenOfHsmCmdHead = 4;
	}
	else
		lenOfHsmCmdHead = 2;

	if ((mdlidOfHsm = UnionGetFixedMDLIDOfTaskOfHsmGrp(hsmRec.hsmGrpID)) <= 0)
	{
		UnionUserErrLog("in %s:: UnionGetFixedMDLIDOfTaskOfHsmGrp[%s]!\n",UnionGetApplicationName(),hsmRec.hsmGrpID);
		return(UnionTaskActionBeforeExit());
	}
	
	UnionLog("in %s:: try to connect [%ld] [%s] [%d]!\n",UnionGetApplicationName(),mdlidOfHsm,hsmRec.ipAddr,hsmRec.port);
	for (;;)
	{
		if ((gunionHsmSckHDL = UnionCreateSocketClient(hsmRec.ipAddr,hsmRec.port)) < 0)
		{
			UnionUserErrLog("in %s:: UnionCreateSocketClient [%s] [%d]!\n",UnionGetApplicationName(),hsmRec.ipAddr,hsmRec.port);
			UnionUpdateIntTypeSpecFldOfHsmRec(hsmRec.ipAddr, conHsmFldNameHsmStatusID, conHsmStatusAbnormalSJL06);
			UnionAuditLog("in %s:: UnionUpdateIntTypeSpecFldOfHsmRec set [%s] conHsmStatusAbnormalSJL06!\n", UnionGetApplicationName(), hsmRec.ipAddr);
			//UnionKillTaskInstanceByName(taskName);
			sleep(2);
			return(UnionTaskActionBeforeExit()); 
		}
		else
			break;
	}
		
	UnionSetCommConfOK(pgThisCommConf);
	gunionISConnectHsmOK = 1;
	UnionSuccessLog("in %s:: connect [%ld] [%s] [%d] OK!\n",UnionGetApplicationName(),mdlidOfHsm,hsmRec.ipAddr,hsmRec.port);

	// 读取加密机超时时间
	if ((timeoutOfHsm = UnionReadIntTypeRECVar("timeoutOfHsm")) <= 2)
		timeoutOfHsm = 3;

	while(1)
	{
		if (isConnectSpecHsm)
		{
			UnionCloseSocket(gunionHsmSckHDL);
			UnionSetCommConfAbnormal(pgThisCommConf);
			isConnectSpecHsm = 0;
			goto againConnectHsm;
		}
			
		memset(tmpBuf,0,sizeof(tmpBuf));
		memset(tmpBuf,'0',hsmGroup.lenOfMsgHeader);
		memset(&msgHeader,0,sizeof(msgHeader));
		
#if ( defined __linux__ ) || ( defined __hpux )
		if (sigsetjmp(gunionHsmTestJmpEnv,1) != 0)
#elif ( defined _AIX )
		if (setjmp(gunionHsmTestJmpEnv) != 0)
#endif
		{
			// 发送测试指令
			strcpy((char *)tmpBuf + hsmGroup.lenOfMsgHeader,hsmGroup.testCmdReq);
			len = strlen(hsmGroup.testCmdReq);
			gunionIsTestHsmCmd = 1;
			firstFlag = 0;
			alarm(0);
			goto sendHsmCmd;
		}
		gunionIsTestHsmCmd = 0;
		if (firstFlag)
		{
			alarm(1);
			signal(SIGALRM,UnionDealHsmTestingPackage);
		}
		else
		{
			if ((maxFreeTime = UnionReadIntTypeRECVar("maxFreeTimeOfHsm")) < 0)
				maxFreeTime = 10;
			alarm(maxFreeTime);
			signal(SIGALRM,UnionDealHsmTestingPackage);
		}

		if ((len = UnionReadRequestToSpecifiedModuleWithMsgHeader(mdlidOfHsm,
				tmpBuf + hsmGroup.lenOfMsgHeader,sizeof(tmpBuf) - hsmGroup.lenOfMsgHeader,&applierMDLID,&msgHeader)) < 0)
		{
			UnionUserErrLog("in %s:: UnionReadRequestToSpecifiedModuleWithMsgHeader [%ld]!\n",UnionGetApplicationName(),mdlidOfHsm);
			break;
		}
		UnionProgramerLog("in %s:: unionmsg receive [%.2s] from [%ld]!\n", UnionGetApplicationName(), tmpBuf + hsmGroup.lenOfMsgHeader, applierMDLID);

		memset(msg, 0, sizeof(msg));
		memcpy(msg, (char *)tmpBuf + hsmGroup.lenOfMsgHeader, len);

		// 检查打印格式
		if(memcmp(tmpBuf + hsmGroup.lenOfMsgHeader, "PRTT", 4) == 0)
		{
			packagePrintType = tmpBuf[hsmGroup.lenOfMsgHeader + 4];
			len -= 5;
			memmove(tmpBuf + hsmGroup.lenOfMsgHeader, tmpBuf + hsmGroup.lenOfMsgHeader + 5, len);
		}

		// 检查是否指定IP地址
		if (memcmp(tmpBuf + hsmGroup.lenOfMsgHeader, "HSMIP", 5) == 0)
		{
			memset(buf,0,sizeof(buf));
			offset = 5;
			memcpy(buf, tmpBuf + hsmGroup.lenOfMsgHeader + offset, 2);
			memset(specIPAddr,0,sizeof(specIPAddr));
			offset += 2;
			memcpy(specIPAddr,tmpBuf + hsmGroup.lenOfMsgHeader + offset,atoi(buf));
			offset += atoi(buf);
			
			len -= offset;
			memmove(tmpBuf + hsmGroup.lenOfMsgHeader, tmpBuf + hsmGroup.lenOfMsgHeader + offset, len);

			// IP地址和现在连接的加密机不同
			if (strcmp(specIPAddr,(char *)hsmRec.ipAddr) != 0)
			{
				UnionCloseSocket(gunionHsmSckHDL);
				if ((gunionHsmSckHDL = UnionCreateSocketClient(specIPAddr,hsmRec.port)) < 0)
				{
					UnionUserErrLog("in %s:: UnionCreateSocketClient spec HSM [%s] [%d]!\n",UnionGetApplicationName(),specIPAddr,hsmRec.port);
					break;
				}
				isConnectSpecHsm = 1;
			}
		}

sendHsmCmd:
		alarm(0);
		len += hsmGroup.lenOfMsgHeader;
		tmpBuf[len] = 0;
	
		// 读取指令头
		memset(hsmCmdHead,0,sizeof(hsmCmdHead));
		memcpy(hsmCmdHead,tmpBuf + hsmGroup.lenOfMsgHeader,lenOfHsmCmdHead);
		
		memset(tmpStr, 0, sizeof(tmpStr));
		bcdhex_to_aschex((char *)tmpBuf, len+hsmGroup.lenOfLenFld, tmpStr);
		UnionAuditLog("+++ req [%s]\n", tmpStr);

		//UnionLog("req[%ld][%s][%d]:: [%04d] [%s]\n",mdlidOfHsm,hsmRec.ipAddr,hsmRec.port,len,tmpBuf);
		UnionPrintHsmReqLog(mdlidOfHsm, msgHeader.provider, hsmRec.ipAddr, hsmRec.port, packagePrintType, len - hsmGroup.lenOfMsgHeader, (char *)(tmpBuf + hsmGroup.lenOfMsgHeader));
		if (len > hsmGroup.lenOfMsgHeader)
		{
			memmove(tmpBuf + hsmGroup.lenOfLenFld,tmpBuf,len);
			if (hsmGroup.lenOfLenFld == 2)
			{
				tmpBuf[0] = len / 256;
				tmpBuf[1] = len % 256;
			}
			else
				sprintf((char *)tmpBuf,"%0*d",hsmGroup.lenOfLenFld,len);

			alarm(0);
#if ( defined __linux__ ) || ( defined __hpux )
			if (sigsetjmp(gunionHsmJmpEnv,1) != 0)
#elif ( defined _AIX )
			if (setjmp(gunionHsmJmpEnv) != 0)
#endif
			{
				alarm(0);
				UnionUserErrLog("in %s:: Timeout!\n",UnionGetApplicationName());
				break;
			}
			alarm(timeoutOfHsm);
			signal(SIGALRM,UnionDealHsmResponseTimeout);
trySend:
			if ((ret = UnionSendToSocket(gunionHsmSckHDL,tmpBuf,len + hsmGroup.lenOfLenFld)) < 0)
			{
				if (!trySendFlag && isConnectSpecHsm == 0)
				{
					trySendFlag = 1;
					UnionCloseSocket(gunionHsmSckHDL);
					gunionHsmSckHDL = -1;
					if ((gunionHsmSckHDL = UnionCreateSocketClient(hsmRec.ipAddr,hsmRec.port)) < 0)
					{
						UnionUserErrLog("in %s :: UnionCreateSocketClient [%s] [%d]!\n",UnionGetApplicationName(),hsmRec.ipAddr,hsmRec.port);
						//add at 20130621
						ret = UnionSendRequestToSpecifiedModuleWithOriMsgHeaderWithTime(mdlidOfHsm, applierMDLID, msg, strlen(msg), &msgHeader, 0); 
						if(ret < 0)
						{
							UnionUserErrLog("in %s :: UnionSendRequestToSpecifiedModuleWithOriMsgHeaderWithTime error!\n",UnionGetApplicationName());
						}
						//end add
						break;
					}
					alarm(timeoutOfHsm);
					signal(SIGALRM,UnionDealHsmResponseTimeout);
					goto trySend;
				}
				else
				{
					alarm(0);
					UnionUserErrLog("in %s:: UnionSendToSocket [%s] ret = [%d]!\n",UnionGetApplicationName(), hsmRec.ipAddr, ret);
					UnionKillTaskInstanceByName(taskName);
					UnionUpdateIntTypeSpecFldOfHsmRec(hsmRec.ipAddr, conHsmFldNameHsmStatusID, conHsmStatusAbnormalSJL06);
					//add at 20130621
					ret = UnionSendRequestToSpecifiedModuleWithOriMsgHeaderWithTime(mdlidOfHsm, applierMDLID, msg, strlen(msg), &msgHeader, 0); 
					if(ret < 0)
					{
						UnionUserErrLog("in %s :: UnionSendRequestToSpecifiedModuleWithOriMsgHeaderWithTime error!\n",UnionGetApplicationName());
					}
					//end add
					break;
				}
			}
			memset(lenBuf,0,sizeof(lenBuf));
			if ((ret = UnionReceiveFromSocketUntilLen(gunionHsmSckHDL,lenBuf,hsmGroup.lenOfLenFld)) != hsmGroup.lenOfLenFld)
			{
				UnionUserErrLog("in %s:: UnionReceiveFromSocketUntilLen!\n",UnionGetApplicationName());
				break;
			}
			if (hsmGroup.lenOfLenFld == 2)
				len = lenBuf[0] * 256 + lenBuf[1];
			else
				len = atoi((char *)lenBuf);
			
			if (len >= sizeof(tmpBuf))
			{
				UnionUserErrLog("in %s:: UnionReceiveFromSocketUntilLen len [%d] larger than expected [%zu]!\n",
						UnionGetApplicationName(),len,sizeof(tmpBuf));
				break;
			}
			if ((ret = UnionReceiveFromSocketUntilLen(gunionHsmSckHDL,tmpBuf,len)) != len)
			{
				UnionUserErrLog("in %s:: UnionReceiveFromSocketUntilLen!\n",UnionGetApplicationName());
				break;
			}
			UnionIncreaseCommConfWorkingTimes(pgThisCommConf);
		}
		else
			continue;

		/*
		if (strstr(hsmGroup.sensitiveCmd,hsmCmdHead) == NULL)
			UnionLog("res[%ld][%s][%d]:: [%04d] [%s]\n",mdlidOfHsm,hsmRec.ipAddr,hsmRec.port,len,tmpBuf);
		else
			UnionLog("res[%ld][%s][%d]:: [%04d] [******]\n",mdlidOfHsm,hsmRec.ipAddr,hsmRec.port,len);
		*/
		//UnionLog("res[%ld][%s][%d]:: [%04d] [%s]\n",mdlidOfHsm,hsmRec.ipAddr,hsmRec.port,len,tmpBuf);
		UnionPrintHsmResLog(mdlidOfHsm, msgHeader.provider, hsmRec.ipAddr, hsmRec.port, packagePrintType, len - hsmGroup.lenOfMsgHeader, (char *)(tmpBuf + hsmGroup.lenOfMsgHeader));
			
		// 测试指令
		alarm(0);
		if (gunionIsTestHsmCmd)
		{
			gunionIsTestHsmCmd = 0;
			if (memcmp(hsmGroup.testCmdSuccessRes,tmpBuf + hsmGroup.lenOfMsgHeader,strlen(hsmGroup.testCmdSuccessRes)) != 0)
			{
				UnionUserErrLog("in UnionHsmSvrTask:: hsmRes[%s] != testCmdSuccessRes[%s]\n",tmpBuf + hsmGroup.lenOfMsgHeader,hsmGroup.testCmdSuccessRes);
				UnionKillTaskInstanceByName(taskName);
				UnionUpdateIntTypeSpecFldOfHsmRec(hsmRec.ipAddr, conHsmFldNameHsmStatusID, conHsmStatusAbnormalSJL06);
				break;
			}
			UnionUpdateIntTypeSpecFldOfHsmRec(hsmRec.ipAddr, conHsmFldNameHsmStatusID, conHsmStatusOnlineSJL06);
		}
		else
		{

			len -= hsmGroup.lenOfMsgHeader;
			if ((ret = UnionSendResponseToApplyModuleWithOriMsgHeader(applierMDLID,tmpBuf + hsmGroup.lenOfMsgHeader,len,&msgHeader)) < 0)
			{
				UnionUserErrLog("in %s:: UnionSendResponseToApplyModuleWithOriMsgHeader [%ld]!\n",UnionGetApplicationName(),applierMDLID);
				break;
			}
			UnionProgramerLog("in %s:: unionmsg send [%.2s] to [%ld]!\n", UnionGetApplicationName(), tmpBuf + hsmGroup.lenOfMsgHeader, applierMDLID);
			// 判断哪些指令有2次返回
			if ((memcmp(hsmCmdHead,"A2",2) == 0) ||
				(memcmp(hsmCmdHead,"NE",2) == 0) ||
				(memcmp(hsmCmdHead,"OC",2) == 0) ||
				(memcmp(hsmCmdHead,"OE",2) == 0) ||
				(memcmp(hsmCmdHead,"PE",2) == 0) ||
				(memcmp(hsmCmdHead,"OA",2) == 0))
			{
				if (gunionHsmSckHDL > 0)
				{
					UnionCloseSocket(gunionHsmSckHDL);
					gunionHsmSckHDL = -1;
				}
				
				UnionSetCommConfAbnormal(pgThisCommConf);
				goto againConnectHsm;
			}
		}
	}
	return(UnionTaskActionBeforeExit());
}	

void UnionDealHsmTestingPackage()
{
	//UnionAuditLog("in UnionDealHsmTestingPackage:: the connection hasn't working for a long time!\n");
#if ( defined __linux__ ) || ( defined __hpux )
	siglongjmp(gunionHsmTestJmpEnv,10);
#elif ( defined _AIX )
	longjmp(gunionHsmTestJmpEnv,10);
#endif
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


