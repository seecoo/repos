// Author:	Wolfgang Wang
// Date:	2008/5/11

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#define _realBaseDB_2_x_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if ( defined __linux__ )
#include <zlib.h>
#endif

#ifndef _WIN32
#include <signal.h>
#include <setjmp.h>
#include <time.h>
#include "unionREC.h"
#include "UnionTask.h"
#include "unionCommConf.h"
#include "unionREC.h"
#else
#include "unionRECVar.h"
#include <winsock.h>
#endif

#include "UnionLog.h"
#include "UnionStr.h"
#include "errCodeTranslater.h"
#include "unionFunSvrName.h"
#include "unionMsgBufGroup.h"

#include "unionVersion.h"
#include "UnionSocket.h"
#include "synchTCPIPSvr.h"
#include "unionCommBetweenMDL.h"
#include "commWithTransSpier.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "packageConvert.h"
#include "registerRunningTime.h"
#include "unionHighCachedAPI.h"
#include "unionTableData.h"

void SetErrorLogNoAloneFile();

#ifndef _WIN32
int				gsynchTCPIPSvrSckHDL = -1;
extern PUnionTaskInstance 	ptaskInstance;

#ifndef _noRegisterClient_
PUnionCommConf			pgunionCommConf = NULL;
#endif

jmp_buf gunionTCPSvrJmpEnvForEsscClient;
jmp_buf gunionTCPSvrJmpEnvForMaxIdleTime;

void UnionDealTCPSvrJmpEnvForEsscClient();
void UnionDealTCPSvrJmpEnvForMaxIdleTime();

int UnionSetXMLResponsePackageForError(int resCode);
int UnionFunSvrInterprotor(char *cliIPAddr,int port,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr);

int UnionGetActiveTCIPSvrSckHDL()
{
	return(gsynchTCPIPSvrSckHDL);
}
#endif

static int unionIsXMLTail(char *xmlStr)
{
	int		i = 0;

	for(i = strlen(xmlStr) - 1; i > 0; i--)
	{
		if(xmlStr[i] == ' ' || xmlStr[i] == '\t' || xmlStr[i] == '\r' || xmlStr[i] == '\n')
		{
			continue;
		}
		if(xmlStr[i] == '>')
		{
			return(1);
		}
		break;
	}
	return(0);
}

int UnionSynchTCPIPTaskServer(int handle,struct sockaddr_in *cli_addr,int port,int (*UnionTaskActionBeforeExit)())
{
	unsigned char	reqStr[conMaxSizeOfClientMsg*3];
	unsigned char	resStr[conMaxSizeOfClientMsg*3];
	unsigned char	tmpBuf[conMaxSizeOfClientMsg*3];
	unsigned int	lenOfReq = 0;
	unsigned int	lenOfRes = 0;
	int		ret;
	int		isUI = 0;
	unsigned long	len = 0;
	int		timeoutOfClient = 0;
	int		maxIdleTimeOfSckConn = 0;
	char		cliName[128];
	char		taskName[128];
	char		cliIPAddr[64];
	int		lenOfLengthField = 2;
	int		commType;
	int		finishThisTask = 0;
	char		sysID[32];
	char		userID[128];
	char		*ptr1 = NULL;
	char		*ptr2 = NULL;
	char		serviceCode[8];
	char		resID[8];
	char		operator[128];
	int		recvFromClientOK = 0;

	//add begin by lusj 20160406  东莞农商接口判断   审核人： 林晓杰
        char    appTaskName[128];
        int             IsESBTask=0;//0:非东莞农商ESB系统接口           1:东莞农商ESB系统接口
        //add end

	memset(cliIPAddr,0,sizeof(cliIPAddr));
	inet_ntop(AF_INET, (void *)&cli_addr->sin_addr, cliIPAddr, sizeof(cliIPAddr));

	snprintf(cliName,sizeof(cliName),"%s %d",cliIPAddr,port);
	snprintf(taskName,sizeof(taskName),"%s client %d",UnionGetApplicationName(),port);

	//add begin  by lusj  20160406 东莞农商ESB接口判断
        memset(appTaskName,0,sizeof(appTaskName));
        snprintf(appTaskName,sizeof(appTaskName),"%s",UnionGetApplicationName());

        if (memcmp(appTaskName,"appTask-esb",11) == 0)
                IsESBTask=1;
        //add end	

#ifndef _WIN32		
	gsynchTCPIPSvrSckHDL = handle;
	// 创建任务实例
	/*
	if (UnionExistsTaskClassOfName(taskName))
	{
		if ((ptaskInstance = UnionCreateTaskInstanceOfName(UnionTaskActionBeforeExit,cliName)) == NULL)
		{
			ret = errCodeCreateTaskInstance;
			UnionUserErrLog("in %s:: UnionCreateTaskInstanceOfName! ret = [%d]\n",cliName,ret);
			goto errorExit;
		}
		commType = conCommShortConn;
	}
	else
	{
		if ((ptaskInstance = UnionCreateTaskInstanceOfName(UnionTaskActionBeforeExit,"%s %d",UnionGetApplicationName(),port)) == NULL)
		{
			ret = errCodeCreateTaskInstance;
			UnionUserErrLog("in %s:: UnionCreateTaskInstanceOfName! ret = [%d]\n",cliName,ret);
			goto errorExit;
		}
	*/
		// 2015-10-15 增加IP地址注册,zhangyd 
		if ((ptaskInstance = UnionCreateTaskInstanceForClient(UnionTaskActionBeforeExit,"%s busy %d %s",UnionGetApplicationName(),port,cliIPAddr)) == NULL)
		{
			ret = errCodeCreateTaskInstance;
			UnionUserErrLog("in %s:: UnionCreateTaskInstanceForClient! ret = [%d]\n",cliName,ret);
			goto errorExit;
		}
		commType = conCommShortConn;
	//}
#ifndef _noRegisterClient_
	if ((pgunionCommConf = UnionAddServerCommConf(cliIPAddr,port,commType,"通讯服务")) == NULL)
	{
#ifdef _limitClient_
		ret = errCodeYLCommConfMDL_AddCommConf;
		UnionUserErrLog("in %s:: UnionAddServerCommConf [%s] [%d]! ret = [%d]\n",cliName,cliIPAddr,port,ret);
		goto errorExit;
#endif
	}
	UnionSetCommConfOK(pgunionCommConf);
#endif
#endif
	
loop:	// 循环处理客户端请求
	recvFromClientOK = 0;

	// 读取超时值
	if ((timeoutOfClient = UnionReadIntTypeRECVar("timeoutOfRecvClientData")) < 0)
		timeoutOfClient = 3;

	// 设置连接空闲时间控制
	if ((maxIdleTimeOfSckConn = UnionReadIntTypeRECVar("maxIdleTimeOfSckConn")) < 0)
		maxIdleTimeOfSckConn = 1800;
#ifdef _WIN32
	// add codes here!
#else
#if ( defined __linux__ ) || ( defined __hpux )
	if (sigsetjmp(gunionTCPSvrJmpEnvForMaxIdleTime,1) != 0) // 超时退出
#elif ( defined _AIX )
	if (setjmp(gunionTCPSvrJmpEnvForMaxIdleTime) != 0)	// 超时退出
#endif
	{
		ret = errCodeEsscMDL_TCPIPTaskTooIdle;
		UnionUserErrLog("in %s:: this connection too idle! close it now! ret = [%d]\n",cliName,ret);
		goto errorExit;
	}
	alarm(maxIdleTimeOfSckConn);
	signal(SIGALRM,UnionDealTCPSvrJmpEnvForMaxIdleTime);
#endif
		
	// 开始从客户端接收数据
	// 接收数据长度
	lenOfLengthField = UnionGetLenOfLenFieldForApp();
	if (lenOfLengthField == 0)
	{
#ifndef _WIN32
		alarm(0);
#endif

#ifdef _WIN32
		// addcodes here
#else
#if ( defined __linux__ ) || ( defined __hpux )
		if (sigsetjmp(gunionTCPSvrJmpEnvForEsscClient,1) != 0)	// 超时退出
#elif ( defined _AIX )
		if (setjmp(gunionTCPSvrJmpEnvForEsscClient) != 0) // 超时退出
#endif
		{
			ret = errCodeSocketMDL_RecvDataTimeout;
			UnionUserErrLog("in %s:: recv from client time out! ret = [%d]\n",cliName,ret);
			goto errorExit;
		}

		alarm(timeoutOfClient);
		signal(SIGALRM,UnionDealTCPSvrJmpEnvForEsscClient);
#endif
		if ((ret = UnionReceiveFromSocket(handle,reqStr,8192)) < 0)
		{
			UnionUserErrLog("in %s:: UnionReceiveFromSocket! ret = [%d]\n",cliName,ret);
			goto errorExit;
		}
		// 重置当前时间
		UnionReInitUserSetTime();
		if (ret == 0)
		{
			UnionProgramerLog("in %s:: connection closed by peer!\n",cliName);
			UnionSetCommConfAbnormal(pgunionCommConf);
			return(0);
		}
		reqStr[ret] = 0;
		lenOfReq = ret;
#ifndef _WIN32
		alarm(0);
#ifndef _noRegisterClient_
		UnionIncreaseCommConfWorkingTimes(pgunionCommConf);
#endif
#endif
	}
	else
	{
		if ((ret = UnionReceiveFromSocketUntilLen(handle,reqStr,2)) < 0)
		{
			UnionUserErrLog("in %s:: UnionReceiveFromSocketUntilLen %d! ret = [%d]\n",cliName,2,ret);
			goto errorExit;
		}

		// 重置当前时间
		UnionReInitUserSetTime();

		if (memcmp(reqStr,"00",2) == 0)		// 长度域是8个字节
		{
			if ((ret = UnionReceiveFromSocketUntilLen(handle,reqStr + 2,6)) < 0)
			{
				UnionUserErrLog("in %s:: UnionReceiveFromSocketUntilLen %d! ret = [%d]\n",cliName,6,ret);
				goto errorExit;
			}
			lenOfLengthField = 8;
			reqStr[lenOfLengthField] = 0;
		}
		else
			lenOfLengthField = 2;
		
#ifndef _WIN32
		alarm(0);
#endif
		if (ret == 0)
		{
			UnionProgramerLog("in %s:: connection closed by peer!\n",cliName);
			UnionSetCommConfAbnormal(pgunionCommConf);
			return(0);
		}
		if (lenOfLengthField == 2)
			lenOfReq = reqStr[0] * 256 + reqStr[1];
		else
			lenOfReq = atol((char *)reqStr);
		
		if ((lenOfReq  < 0) || (lenOfReq >= (int)sizeof(reqStr)))
		{
			ret = errCodeSocketMDL_DataLen;
			UnionUserErrLog("in %s:: lenOfReq = [%d] error! ret = [%d]\n",cliName,lenOfReq,ret);
			goto errorExit;
		}
		if (lenOfReq == 0)	// 测试报文
		{
			UnionProgramerLog("in %s:: testing pack received!\n",cliName);
			lenOfRes = 0;
			// modify by leipp 20150702
			if (lenOfLengthField == 2)
				goto loop;
			// modify end
			goto sendToClient;
		}
		// 设置接收客户端数据的超时机制
#ifdef _WIN32
		// addcodes here
#else
#if ( defined __linux__ ) || ( defined __hpux )
		if (sigsetjmp(gunionTCPSvrJmpEnvForEsscClient,1) != 0)	// 超时退出
#elif ( defined _AIX )
		if (setjmp(gunionTCPSvrJmpEnvForEsscClient) != 0) // 超时退出
#endif
		{
			ret = errCodeSocketMDL_RecvDataTimeout;
			UnionUserErrLog("in %s:: recv from client time out! ret = [%d]\n",cliName,ret);
			goto errorExit;
		}

		alarm(timeoutOfClient);
		signal(SIGALRM,UnionDealTCPSvrJmpEnvForEsscClient);
#endif
		// 接收客户端数据
		if ((ret = UnionReceiveFromSocketUntilLen(handle,reqStr,lenOfReq)) != lenOfReq)
		{
			UnionUserErrLog("in %s:: UnionReceiveFromSocketUntilLen expected [%d] real = [%d]!\n",cliName,lenOfReq,ret);
			ret = errCodeSocketMDL_DataLen;
			goto errorExit;
		}
		reqStr[ret] = 0;
		recvFromClientOK = 1;
#ifndef _WIN32
	alarm(0);
#ifndef _noRegisterClient_
	UnionIncreaseCommConfWorkingTimes(pgunionCommConf);
#endif
#endif
	}

	if (memcmp((char *)reqStr,PACKAGE_VERSION_001,4) == 0)
	{
		UnionSetPackageType(PACKAGE_TYPE_V001);
		if ((ptr1 = strstr((char *)reqStr,"05sysID")) != NULL)
		{
			ptr1 += 7;
			ret = UnionPackageConvertIntStringToInt((unsigned char *)ptr1,4);
			memcpy(sysID,ptr1+4,ret);
			sysID[ret] = 0;
			UnionSetSuffixOfMyLogFile(sysID);
		}
	}
	else if (memcmp((char *)reqStr,"<?xml",5) == 0)
	{
		if(!unionIsXMLTail((char *)reqStr))
		{
			UnionUserErrLog("in %s:: illegle xml tail missing '>'!, ret = [%d]!\n",cliName, errCodeParameter);
			ret = errCodeSocketMDL_ErrCodeNotSet;
			goto errorExit;
		}
		UnionSetPackageType(PACKAGE_TYPE_XML);
		if ((ptr1 = strstr((char *)reqStr,"<sysID>")) != NULL)
		{
			ptr1 += 7;
			if ((ptr2 = strstr(ptr1,"</sysID>")))
			{
				ret = ptr2 - ptr1;
				memcpy(sysID,ptr1,ret);
				sysID[ret] = 0;
			}
		}
		
		if ((isUI = UnionIsUITrans(sysID)))
		{
			if ((ptr1 = strstr((char *)reqStr,"<userID>")) != NULL)
			{
				ptr1 += 8;
				ptr2 = strstr(ptr1,"</userID>");
				if ((ptr2 = strstr(ptr1,"</userID>")))
				{
					ret = ptr2 - ptr1;
					len = sprintf(userID,"UI-%.*s",ret,ptr1);
					userID[len] = 0;
					UnionSetSuffixOfMyLogFile(userID);
				}
			}
		}
		else
		{
			UnionSetSuffixOfMyLogFile(sysID);
		}
	}
	//add begin  by lusj 20160318 东莞农商ESB报文检测 <field type="string" length="6" scale="0" >#CONSUMER_ID#</field>
	else if ((ptr1 = strstr((char *)reqStr,"<sys-header>")) != NULL)
	{
		ptr1=NULL;
		if ((ptr2 = strstr((char *)reqStr,"CONSUMER_ID")) != NULL)
		{
					ptr1 = strstr(ptr2,"<");//CONSUMER_ID域的域值开头的位置
					
					ptr2=NULL;
					ptr2 = strstr(ptr1,">");
					ptr2 += 1;
					
					ptr1=NULL;
					ptr1 = strstr(ptr2,"<");
					
					ret = ptr1 - ptr2;
					memcpy(userID,ptr2,ret);
					userID[ret] = 0;
					UnionSetSuffixOfMyLogFile(userID);
		}
	}//add end by lusj
	else if ((UnionIsCheckKMSPackage('1',(char *)reqStr,lenOfReq,operator,sysID,resID,serviceCode)) < 0) //modify by linxj 20151027
	{
		memcpy(sysID,reqStr,2);
		sysID[2] = 0;
		UnionSetSuffixOfMyLogFile(sysID);
	}else
	{
		UnionSetSuffixOfMyLogFile(sysID);
	}
	//modify end 20151027
	
	UnionSetMultiLogBegin();
	// add by leipp 20151112
	if (!isUI)
	{
		SetErrorLogAloneFile();
	}
	// add end
	//add by lusj 20160406 东莞农商ESB接口判断
        if(IsESBTask==1)
                lenOfLengthField = 0;//东莞农商接收的返回报文是不带长度头
        //add end
	
	if ((ret = UnionFunSvrInterprotor(cliIPAddr,port,reqStr,lenOfReq,resStr + lenOfLengthField,sizeof(resStr) - lenOfLengthField)) < 0)
	{
		UnionUserErrLog("in %s:: UnionFunSvrInterprotor,ret = [%d]!\n",cliName,ret);
		goto errorExit;
	}
	
	lenOfRes = ret;
	if(lenOfRes > conMaxSizeOfClientMsg*3)
	{
		ret = errCodeSocketMDL_DataLen;
		UnionUserErrLog("in %s:: UnionFunSvrInterprotor,lenOfRes [%d] > [%d] ret = %d !\n", cliName, lenOfRes, conMaxSizeOfClientMsg * 3 , ret);
		goto errorExit;
	}

sendToClient: // 向客户端发送数据

	// 2013-04-26 add
	if (isUI && (lenOfLengthField == 2) && (lenOfRes > 65535))
	{
		// 压缩
		len = sizeof(tmpBuf);
		if ((ret = compress(tmpBuf, &len, resStr + lenOfLengthField, lenOfRes)) != 0)
		{
			UnionUserErrLog("in %s:: compress ret = [%d]!\n",cliName,ret);
			UnionSetCommConfAbnormal(pgunionCommConf);
			UnionSetMultiLogEnd();
			return(0 - abs(ret));
		}
		
		// 添加压缩标志
		resStr[2] = '1'; 
		resStr[3] = lenOfRes / (256 * 256);
		resStr[4] = (lenOfRes % (256 * 256)) / 256;
		resStr[5] = (lenOfRes % (256 * 256)) % 256;
		
		memcpy(resStr + 6,tmpBuf,len);
		lenOfRes = len + 4;
		if (lenOfRes > 65535)
		{
			UnionUserErrLog("in %s:: lenOfRes[%d] > 65535!\n",cliName,lenOfRes);
			UnionSetCommConfAbnormal(pgunionCommConf);
			UnionSetMultiLogEnd();
			return(errCodeEsscMDL_ClientDataLenTooLong);
		}
	}
	
	if (lenOfLengthField == 0)
	{
		// 没有长度域
	}
	else if (lenOfLengthField == 2)
	{
		resStr[0] = lenOfRes / 256;
		resStr[1] = lenOfRes % 256;
	}
	else
	{
		ret = resStr[lenOfLengthField];
		sprintf((char *)resStr,"%0*d",lenOfLengthField,lenOfRes);
		resStr[lenOfLengthField] = ret;
	}
	lenOfRes += lenOfLengthField;
	
	if ((ret = UnionSendToSocket(handle,resStr,lenOfRes)) < 0)
	{
		UnionUserErrLog("in %s:: UnionSendToSocket ret = [%d]!\n",cliName,ret);
		UnionSetCommConfAbnormal(pgunionCommConf);
		// add by leipp 20151112
		if (!isUI)
		{
			SetErrorLogNoAloneFile();
		}
		// add end
		UnionSetMultiLogEnd();
		return(ret);
	}

	// add by leipp 20151112
	if (!isUI)
	{
		SetErrorLogNoAloneFile();
	}
	// add end

	UnionSetMultiLogEnd();
	//add by lusj 20160406 东莞农商ESB接口判断,短链接，主动断开连接
        if(IsESBTask==1)
        {
                //UnionCloseSocket(handle);
                //UnionProgramerLog("in %s:: connection closed by peer!\n",cliName);
                UnionSetCommConfAbnormal(pgunionCommConf);
                return(0);
        }
        //add end
	if (finishThisTask)
	{
		usleep(100);
		UnionSetCommConfAbnormal(pgunionCommConf);
		return(ret);
	}
	else
		goto loop;
	
errorExit:
	// add by leipp 20151112
	if (!isUI)
	{
		SetErrorLogNoAloneFile();
	}
	// add end
	UnionProgramerLog("in %s:: tag errorExit ret = %d recvFromClientOK = %d !\n",cliName, ret, recvFromClientOK);

	if (ret >= 0)
		ret = errCodeSocketMDL_ErrCodeNotSet;
	if (!recvFromClientOK)
	{
		sleep(1);
		return(ret);
	}

	if (ret == errCodeEsscMDLReqStr)
	{
		UnionSetCommConfAbnormal(pgunionCommConf);
		UnionSetMultiLogEnd();
		return(ret);
	}
		
	UnionSetXMLResponsePackageForError(ret);
	lenOfRes = UnionResponseXMLPackageToBuf((char *)resStr + lenOfLengthField,sizeof(resStr) - lenOfLengthField);	

	finishThisTask = 1;
	goto sendToClient;
}

#ifndef _WIN32
void UnionDealTCPSvrJmpEnvForEsscClient()
{
	UnionUserErrLog("gunionTCPSvrJmpEnvForEsscClient:: timeout!\n");
#if ( defined __linux__ ) || ( defined __hpux )
	siglongjmp(gunionTCPSvrJmpEnvForEsscClient,10);
#elif ( defined _AIX )
	longjmp(gunionTCPSvrJmpEnvForEsscClient,10);
#endif
}

void UnionDealTCPSvrJmpEnvForMaxIdleTime()
{
	UnionUserErrLog("gunionTCPSvrJmpEnvForEsscClient:: timeout!\n");
#if ( defined __linux__ ) || ( defined __hpux )
	siglongjmp(gunionTCPSvrJmpEnvForMaxIdleTime,10);
#elif ( defined _AIX )
	longjmp(gunionTCPSvrJmpEnvForMaxIdleTime,10);
#endif
}
#endif

