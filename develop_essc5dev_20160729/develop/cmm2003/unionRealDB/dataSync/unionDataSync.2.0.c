//	Author:		chenwd
//	Copyright:	Union Tech. Guangzhou
//	Date:		2015-01-15

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>
#include <setjmp.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#ifndef _UnionSocket_3_x_
#define _UnionSocket_3_x_
#endif

#include "UnionStr.h"
#include "unionMDLID.h"
#include "unionCommBetweenMDL.h"
#include "UnionLog.h"
#include "unionMsgBufGroup.h"
#include "unionDataSync.h"
#include "UnionSocket.h"
#include "unionXMLPackage.h"

#include "unionFileTransfer.h"
#include "unionREC.h"

static int			gunionDataSyncIsInit = 0;
static TUnionDataSyncIni 	gunionDataSyncIni = {0};

static int			gunionDataSyncIsWaitResult = 0;

jmp_buf gunionDataSyncJmpEnv;
void    UnionDealDataSyncResponseTimeout();

void UnionSetDataSyncIsWaitResult()
{
	gunionDataSyncIsWaitResult = 1;
}

void UnionSetDataSyncIsNotWaitResult()
{
	gunionDataSyncIsWaitResult = 0;
}

int UnionDataSyncIsWaitResult()
{
	return(gunionDataSyncIsWaitResult);
}

PUnionDataSyncIni getDataSyncIniPtr()
{
	int	ret = 0;
	if ((ret = UnionReloadDataSyncIni()) < 0)
	{
		UnionUserErrLog("in getDataSyncIniPtr:: UnionReloadDataSyncIni ret = [%d]!\n",ret);
		return NULL;
	}

	return(&gunionDataSyncIni);
}

// 读取unionDataSync.ini信息
int UnionReloadDataSyncIni()
{
	int	i;
	int	ret;
	int	count = 0;
	int	syncNum = 0;
	int	enabled = 0;
	char	fileName[128];
	char	tmpBuf[128];

	if (gunionDataSyncIsInit)
		return(0);
	
	gunionDataSyncIni.syncNum = 0;
	
	snprintf(fileName,sizeof(fileName),"%s/unionDataSync.ini",getenv("UNIONETC"));

	if (access(fileName,0) != 0)
	{
		UnionLog("in UnionReloadDataSyncIni:: access[%s]!\n",fileName);
		gunionDataSyncIsInit = 1;
		return(1);
	}
	gunionDataSyncIni.localPort = UnionINIReadInt("dataSync","localPort",0,fileName);
	count = UnionINIReadInt("dataSync","syncNum",1,fileName);

	//modify by linxj 20150430
	for (i = 0; i < count; i ++)
	{
		if (syncNum >= DB_SYNC_NUM)
			break;

		snprintf(tmpBuf,sizeof(tmpBuf),"addr%02d",i + 1);

		if ((enabled = UnionINIReadInt(tmpBuf,"enabled",0,fileName)) != 1)
			continue;

		gunionDataSyncIni.syncAddr[syncNum].localResourcesNeedSync = UnionINIReadInt(tmpBuf,"localResourcesNeedSync",0,fileName);

		gunionDataSyncIni.syncAddr[syncNum].highCachedNeedSync = UnionINIReadInt(tmpBuf,"highCachedNeedSync",0,fileName);

		if ((ret = UnionINIReadString(tmpBuf,"sendIPAddr",NULL,gunionDataSyncIni.syncAddr[syncNum].sendIPAddr,sizeof(gunionDataSyncIni.syncAddr[syncNum].sendIPAddr),fileName)) <= 0)
			continue;

		if ((gunionDataSyncIni.syncAddr[syncNum].sendPort = UnionINIReadInt(tmpBuf,"sendPort",-1,fileName)) <= 0)
			continue;
		/*
		if ((gunionDataSyncIni.syncAddr[syncNum].fileRecvPort = UnionINIReadInt(tmpBuf,"fileRecvPort",-1,fileName)) <= 0)
			continue;
		*/
		gunionDataSyncIni.syncAddr[syncNum].socket_no = -1;
		syncNum ++;
	}
	//modify end 20150430
	gunionDataSyncIni.syncNum = syncNum;
	gunionDataSyncIsInit = 1;

	return(1);
}

/*
功能	发送msg到消息队列
输入参数
	msg		发送信息内容
	lenOfMsg	消息长度
	dataType	数据类型
	resultFlag	结果标识
输出参数
	无
返回值
	>=0		成功
	<0		失败
*/
int UnionDataSyncSendMsg(unsigned char *msg,int lenOfMsg,TUnionDataSyncDataType dataType,TUnionDataSyncResultFlag resultFlag)
{
	int	ret;
	unsigned char	tmpBuf[819200];
	
	if ((ret = UnionReloadDataSyncIni()) < 0)
	{
		UnionUserErrLog("in UnionDataSyncSendMsg:: UnionReloadDataSyncIni!\n");
		return(ret);
	}
	
	tmpBuf[0] = '0' + dataType;
	tmpBuf[1] = '0' + resultFlag;
	if (lenOfMsg > sizeof(tmpBuf) - 2)
	{
		UnionUserErrLog("in UnionDataSyncSendMsg:: lenOfMsg [%d] too big!\n", lenOfMsg);
		return(errCodeSmallBuffer);
	}
	memcpy(tmpBuf+2,msg,lenOfMsg);
	
	UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfDataSync);
	if ((ret = UnionSendRequestToSpecifiedModule(conMDLTypeOffsetOfDataSync + conMDLTypeUnionDataSync,tmpBuf,lenOfMsg+2)) < 0)
	{
		UnionUserErrLog("in UnionDataSyncSendMsg:: UnionSendRequestToSpecifiedModule [%d]!\n", conMDLTypeOffsetOfDataSync + conMDLTypeUnionDataSync);
		return(ret);
	}
	
	return(ret);
}

/*
功能	从消息队列取msg
输入参数
	无
输出参数
	msg		获取到的消息
	sizeOfMsg	获取到的消息长度
返回值
	>=		成功
	<0		失败
*/
int UnionDataSyncRecvMsg(unsigned char *msg,int sizeOfMsg)
{
	int	ret;
	TUnionModuleID		dbSyncMDLID;

	if ((ret = UnionReloadDataSyncIni()) < 0)
	{
		UnionUserErrLog("in UnionDataSyncRecvMsg:: UnionReloadDataSyncIni!\n");
		return(ret);
	}

	UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfDataSync);
	if ((ret = UnionReadRequestToSpecifiedModule(conMDLTypeOffsetOfDataSync + conMDLTypeUnionDataSync,msg,sizeOfMsg,&dbSyncMDLID)) < 0 )
	{
		UnionUserErrLog("in UnionDataSyncRecvMsg:: UnionReadRequestToSpecifiedModule[%d]!\n", conMDLTypeOffsetOfDataSync + conMDLTypeUnionDataSync);
		return(ret);
	}

	return(ret);
}

// 将发送失败记录写入到文件
int UnionDataSyncWriteFile(PUnionDataSyncAddr psyncAddr,unsigned char *msg,int lenOfMsg)
{
	int	ret;
	int	len;
	char	strdate[32];
	char	strtime[32];
	char	path[256];
	char	tmpMsg[819200];
	FILE	*fp;

	if ((ret = UnionReloadDataSyncIni()) < 0)
	{
		UnionUserErrLog("in UnionDataSyncWriteFile:: UnionReloadDataSyncIni!\n");
		return(ret);
	}

	UnionGetFullSystemDate(strdate);
	strdate[8] = 0;
	snprintf(path,sizeof(path),"%s/dataSync/dataSync_fail.%s.txt",getenv("UNIONREC"),strdate);
	
	if ((fp = fopen(path,"a+")) == NULL)
	{
		UnionUserErrLog("in UnionDataSyncWriteFile:: fopen [%s] err!\n",path);
		return(errCodeUseOSErrCode);
	}

	UnionGetCurrentDateTimeInMacroFormat(strtime);
	len = snprintf(tmpMsg, sizeof(tmpMsg),"[%s:%d][%s][",psyncAddr->sendIPAddr,psyncAddr->sendPort,strtime);
	memcpy(tmpMsg + len,msg,lenOfMsg);
	len += lenOfMsg;
	tmpMsg[len] = ']';
	len += 1;
	tmpMsg[len] = '\n';
	len += 1;
	if( (ret = fwrite(tmpMsg,len,1,fp)) !=1 )
	{
		UnionUserErrLog("in UnionDataSyncWriteFile:: fwrite err ret = [%d]!",ret);
		ret = errCodeUseOSErrCode;
		fclose(fp);
		return(ret);
	}

	fclose(fp);
	return 0;
}
/*
函数功能：把数据同步信息发送到Socket
输入参数：
	closeSocket:	是否关闭socket
	msg:		消息
	lenOfMsg:	消息的长度
*/
int UnionDataSyncSendToSocket(int closeSocket,unsigned char *msg,int lenOfMsg)
{
	int	i,j;
	int	ret;
	int	len;
	int	result = 0;
	int	syncFlag = 1;
	unsigned char	reqBuf[81920];
	unsigned char	resBuf[81920];
	char	tableName[128];
	
	if ((ret = UnionReloadDataSyncIni()) < 0)
	{
		UnionUserErrLog("in UnionDataSyncSendToSocket:: UnionReloadDataSyncIni!\n");
		return(ret);
	}

	if (gunionDataSyncIni.syncNum > 0)
	{
		reqBuf[0] = lenOfMsg / 256;
		reqBuf[1] = lenOfMsg % 256;
		memcpy(reqBuf+2,msg,lenOfMsg);	
	}

	tableName[0] = 0;
	for (i = 0; i < gunionDataSyncIni.syncNum; i++)
	{
		alarm(0);
		syncFlag = 1;
		
		if (msg[0] - '0' == conSQL)	// 数据库
		{
			if (!gunionDataSyncIni.syncAddr[i].localResourcesNeedSync)
			{
				if (strlen(tableName) == 0)
				{
					if ((ret = UnionGetTableNameAndFieldNameFromSQL((char *)msg + 2,tableName,NULL)) < 0)
					{
						UnionUserErrLog("in UnionDataSyncSendToSocket:: UnionGetTableNameAndFieldNameFromSQL sql[%s]!\n",msg+2);
						return(ret);
					}
				}
				if ((strcasecmp(tableName,"hsm") == 0) || 
					(strcasecmp(tableName,"hsmGroup") == 0)	||
					(strcasecmp(tableName,"lmkProtectMode") == 0) ||
					(strcasecmp(tableName,"highCached") == 0))
					syncFlag = 0;
			}
		}
		else if (msg[0] - '0' == conCache)	// 缓存
		{
			if (gunionDataSyncIni.syncAddr[i].highCachedNeedSync == 0)
				syncFlag = 0;
		}

		if (!syncFlag)
			continue;
connect_socket:
		if (gunionDataSyncIni.syncAddr[i].socket_no <= 0)
		{
			for (j = 0; j < 3; j++)
			{
				if ((gunionDataSyncIni.syncAddr[i].socket_no = UnionCreateSocketClient(gunionDataSyncIni.syncAddr[i].sendIPAddr, gunionDataSyncIni.syncAddr[i].sendPort)) < 0)
				{
					UnionUserErrLog("in UnionDataSyncSendToSocket:: UnionCreateSocketClient syncIPAddr = [%s] syncPort = [%d] !\n", 
						gunionDataSyncIni.syncAddr[i].sendIPAddr, gunionDataSyncIni.syncAddr[i].sendPort);
					usleep(200);
				}
				else
					break;
			}
		}

		if (gunionDataSyncIni.syncAddr[i].socket_no < 0)
		{
			// 写文件
			if (!UnionDataSyncIsWaitResult() && lenOfMsg > 0)
			{
				if ((ret = UnionDataSyncWriteFile(&gunionDataSyncIni.syncAddr[i],msg,lenOfMsg)) < 0)
				{
					UnionUserErrLog("in UnionDataSyncSendToSocket:: UnionDataSyncWriteFile ret = [%d] msg = [%d][%s]!\n", ret, lenOfMsg,msg);
				}
			}
			if (!result)
			{
				UnionSetResponseRemark("同步数据失败，远程服务器[%s:%d]连接失败",gunionDataSyncIni.syncAddr[i].sendIPAddr, gunionDataSyncIni.syncAddr[i].sendPort);
				result = gunionDataSyncIni.syncAddr[i].socket_no;
			}
			continue;
		}

		alarm(0);
#if ( defined __linux__ ) || ( defined __hpux )
		if (sigsetjmp(gunionDataSyncJmpEnv,1) != 0)
#elif ( defined _AIX )
		if (setjmp(gunionDataSyncJmpEnv) != 0)
#endif
		{
			UnionUserErrLog("in UnionDataSyncSendToSocket:: Timeout!\n");
			alarm(0);
			// 写文件
			if (!UnionDataSyncIsWaitResult() && lenOfMsg > 0)
			{
				if ((ret = UnionDataSyncWriteFile(&gunionDataSyncIni.syncAddr[i],msg,lenOfMsg)) < 0)
				{
					UnionUserErrLog("in UnionDataSyncSendToSocket:: UnionDataSyncWriteFile ret = [%d] msg = [%d][%s]!\n", ret, lenOfMsg,msg);
				}
			}
			if (!result)
			{
				UnionSetResponseRemark("同步数据失败，远程服务器[%s:%d]连接失败",gunionDataSyncIni.syncAddr[i].sendIPAddr, gunionDataSyncIni.syncAddr[i].sendPort);
				result = gunionDataSyncIni.syncAddr[i].socket_no;
			}
			break;
		}
		alarm(3);
		signal(SIGALRM,UnionDealDataSyncResponseTimeout);

		//发送
		if ((ret = UnionSendToSocket(gunionDataSyncIni.syncAddr[i].socket_no, reqBuf, lenOfMsg + 2)) < 0)
		{
			UnionUserErrLog("in UnionDataSyncSendToSocket:: UnionSendToSocket msg = [%d][%s]!\n",lenOfMsg,msg);
			UnionCloseSocket(gunionDataSyncIni.syncAddr[i].socket_no);
			gunionDataSyncIni.syncAddr[i].socket_no = -1;
			goto connect_socket;
		}

		//接收响应长度
		if ((ret = UnionReceiveFromSocketUntilLen(gunionDataSyncIni.syncAddr[i].socket_no,resBuf,2)) != 2)
		{
			UnionUserErrLog("in UnionDataSyncSendToSocket:: UnionReceiveFromSocketUntilLen soket_no = [%d]!\n",
				gunionDataSyncIni.syncAddr[i].socket_no);
			UnionCloseSocket(gunionDataSyncIni.syncAddr[i].socket_no);
			gunionDataSyncIni.syncAddr[i].socket_no = -1;
			goto connect_socket;
		}

		//接收响应内容
		len = resBuf[0] * 256 + resBuf[1];
		if (len > 0)
		{
			if ((ret = UnionReceiveFromSocketUntilLen(gunionDataSyncIni.syncAddr[i].socket_no,resBuf,len)) != len)
			{
				UnionUserErrLog("in UnionDataSyncSendToSocket:: UnionReceiveFromSocketUntilLen socket_no = [%d] resBuf = [%s] !\n",
					gunionDataSyncIni.syncAddr[i].socket_no, resBuf);
				UnionCloseSocket(gunionDataSyncIni.syncAddr[i].socket_no);
				gunionDataSyncIni.syncAddr[i].socket_no = -1;
				goto connect_socket;
			}
			resBuf[len] = 0;
			if (memcmp(resBuf,"00",2) != 0)
			{
				// add by leipp begin 20160516
				// 写文件
				if (!UnionDataSyncIsWaitResult() && lenOfMsg > 0)
				{
					if ((ret = UnionDataSyncWriteFile(&gunionDataSyncIni.syncAddr[i],msg,lenOfMsg)) < 0)
					{
						UnionUserErrLog("in UnionDataSyncSendToSocket:: UnionDataSyncWriteFile ret = [%d] msg = [%d][%s]!\n", ret, lenOfMsg,msg);
					}
				}
				// add by leipp end 20160516

				UnionUserErrLog("in UnionDataSyncSendToSocket:: response[%s] is no success !\n",resBuf);
				if (!result)
				{
					result = errCodeEsscMDL_ResponseCodeNotSuccess;
					UnionSetResponseRemark("同步数据失败，远程响应非成功");
				}
				continue;
			}
		}
	}
	alarm(0);
	if (closeSocket)
		UnionDataSyncCloseSocket();
	
	return(result);
}

int UnionDataSyncCloseSocket()
{
	int	i;
	int	ret;
	
	if ((ret = UnionReloadDataSyncIni()) < 0)
	{
		UnionUserErrLog("in UnionDataSyncCloseSocket:: UnionReloadDataSyncIni!\n");
		return(ret);
	}

	for (i = 0; i < gunionDataSyncIni.syncNum; i++)
	{
		if (gunionDataSyncIni.syncAddr[i].socket_no > 0)
		{
			UnionCloseSocket(gunionDataSyncIni.syncAddr[i].socket_no);
			gunionDataSyncIni.syncAddr[i].socket_no = -1;
		}
	}
	return(0);
}

int UnionGetDataSyncLocalPort()
{
	int	ret;

	if ((ret = UnionReloadDataSyncIni()) < 0)
	{
		UnionUserErrLog("in UnionGetDataSyncLocalPort:: UnionReloadDataSyncIni!\n");
		return(ret);
	}

	return(gunionDataSyncIni.localPort);
}

void UnionDealDataSyncResponseTimeout()
{
        UnionUserErrLog("in UnionDealDataSyncResponseTimeout:: response time out!\n");
#if ( defined __linux__ ) || ( defined __hpux )
        siglongjmp(gunionDataSyncJmpEnv,10);
#elif ( defined _AIX )
        longjmp(gunionDataSyncJmpEnv,10);
#endif
}

