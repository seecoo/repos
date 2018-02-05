// Copyright: Union Tech.
// Author:	Wolfgang Wang
// Date:	2008/5/11

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
#include "errCodeTranslater.h"
#include "unionFunSvrName.h"

#ifdef _client_use_ebcdic_
#include "ebcdicAscii.h"
#endif

#include "UnionSocket.h"
#include "synchTCPIPSvr.h"
#include "unionCommBetweenMDL.h"
#include "commWithTransSpier.h"
#include "unionRealBaseDB.h"
#include "unionRealDBCommon.h"

#ifndef _WIN32
int gsynchTCPIPSvrSckHDL = -1;
extern PUnionTaskInstance 	ptaskInstance;

#ifndef _noRegisterClient_
PUnionCommConf			pgunionCommConf = NULL;
#endif

jmp_buf gunionTCPSvrJmpEnvForEsscClient;
jmp_buf gunionTCPSvrJmpEnvForMaxIdleTime;

void UnionDealTCPSvrJmpEnvForEsscClient();
void UnionDealTCPSvrJmpEnvForMaxIdleTime();

int UnionSetXMLResponsePackageForError(char *reqStr,int resCode,char *resStr,int sizeOfResStr);

int UnionGetActiveTCIPSvrSckHDL()
{
	return(gsynchTCPIPSvrSckHDL);
}
#endif

// 2011-10-31 ����������
// ��ȡ������ĳ���
int UnionGetLenOfLengthField()
{
	if (UnionReadIntTypeRECVar("lenOfLengthFieldOnTCPIPSvr") >= 2)
		return(UnionReadIntTypeRECVar("lenOfLengthFieldOnTCPIPSvr"));
	else
		return(2);
}
// 2011-10-31 END

int UnionSynchTCPIPTaskServer(int handle,struct sockaddr_in *cli_addr,int port,int (*UnionTaskActionBeforeExit)())
{
#ifndef  _isXMLPackage_
	int		remarkLen;
	char		errRemark[256+1];
#endif
	unsigned char	reqStr[conMaxSizeOfClientMsg+1];
	unsigned char	resStr[conMaxSizeOfClientMsg+1];
	unsigned char	tmpBuf[conMaxSizeOfClientMsg+1];
	int		lenOfReq,lenOfRes;
	int		ret;
	int		timeoutOfClient,maxIdleTimeOfSckConn,timeoutOfFunSvr;
	char		cliName[100];
	char		varName[100];
	char		cliIPAddr[40+1];
	int		finishThisTask = 0;
	int		recvFromClientOK = 0;
	// add by xusj begin 20091116
	char		reqHeader[6+1];
	char		specHsmTask[100];
	char		fixVarName[100];
	TUnionModuleID		oriHsmTaskID;  //add by hzh in 2011.10.10

	memset(specHsmTask, 0, sizeof specHsmTask);
	memset(fixVarName, 0, sizeof fixVarName);
	memset(reqHeader, 0, sizeof reqHeader);
	// add by xusj end 20091116

	memset(cliIPAddr,0,sizeof(cliIPAddr));
	inet_ntop(AF_INET, (void *)&cli_addr->sin_addr, cliIPAddr, sizeof(cliIPAddr));
	ret = sprintf(cliName,"%s %d",cliIPAddr,port);
	cliName[ret] = 0;
	
#ifndef _noSpierInfo_
	UnionSetClientAttrToldTransSpier(cliIPAddr,port);
#endif
	
#ifndef _WIN32
	gsynchTCPIPSvrSckHDL = handle;
	// ��������ʵ��
	if (UnionExistsTaskClassOfName(cliName))
	{
		if ((ptaskInstance = UnionCreateTaskInstanceOfName(UnionTaskActionBeforeExit,cliName)) == NULL)
		{
			ret = errCodeCreateTaskInstance;
			UnionUserErrLog("in %s:: UnionCreateTaskInstanceOfName! ret = [%d]\n",cliName,ret);
			goto errorExit;
		}
	}
	else
	{
		if ((ptaskInstance = UnionCreateTaskInstanceOfName(UnionTaskActionBeforeExit,"client %d",port)) == NULL)
		{
			ret = errCodeCreateTaskInstance;
			UnionUserErrLog("in %s:: UnionCreateTaskInstanceOfName! ret = [%d]\n",cliName,ret);
			goto errorExit;
		}
	}
#ifndef _noRegisterClient_
	if ((pgunionCommConf = UnionAddServerCommConf(cliIPAddr,port,conCommShortConn,"ͨѶ����")) == NULL)
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
	
	// ��ȡ��ʱֵ
	if ((timeoutOfClient = UnionReadIntTypeRECVar("timeoutOfRecvClientData")) < 0)
		timeoutOfClient = 3;

	//UnionSetMyTransInfoResID(conEssResIDSecurityService);
	
loop:	// ѭ������ͻ�������
	//add by hzh in 2012.12.13 Ϊ������־����(����unionLog.20121101.c)
	UnionSetFlushLogFileLinesMode(1);
	UnionFlushAllLogFileInGrpNoWait();
	//add end
	recvFromClientOK = 0;
	// �������ӿ���ʱ�����
	if ((maxIdleTimeOfSckConn = UnionReadIntTypeRECVar("maxIdleTimeOfSckConn")) < 0)
		maxIdleTimeOfSckConn = 1800;
#ifdef _WIN32
	// add codes here!
#else
#if ( defined __linux__ ) || ( defined __hpux )
	if (sigsetjmp(gunionTCPSvrJmpEnvForMaxIdleTime,1) != 0) // ��ʱ�˳�
#elif ( defined _AIX ) 
	if (setjmp(gunionTCPSvrJmpEnvForMaxIdleTime) != 0)	// ��ʱ�˳�
#endif
	{
		ret = errCodeEsscMDL_TCPIPTaskTooIdle;
		UnionUserErrLog("in %s:: this connection too idle! close it now! ret = [%d]\n",cliName,ret);
		goto errorExit;
	}
	alarm(maxIdleTimeOfSckConn);
	signal(SIGALRM,UnionDealTCPSvrJmpEnvForMaxIdleTime);
#endif
		
	// ��ʼ�ӿͻ��˽�������
	memset(reqStr,0,sizeof(reqStr));
	// �������ݳ���
	if ((ret = UnionReceiveFromSocketUntilLen(handle,reqStr,UnionGetLenOfLengthField())) < 0)
	{
		UnionUserErrLog("in %s:: UnionReceiveFromSocketUntilLen %d! ret = [%d]\n",cliName,UnionGetLenOfLengthField(),ret);
		goto errorExit;
	}
#ifndef _WIN32
	alarm(0);
#endif
	if (ret == 0)
	{
		UnionLog("in %s:: connection closed by peer!\n",cliName);
		UnionFlushAllLogFileInGrpNoWait();
		return(0);
	}
	if (UnionGetLenOfLengthField() == 2)
		lenOfReq = reqStr[0] * 256 + reqStr[1];
	else
		lenOfReq = atol((char *)reqStr);
	
	if ((lenOfReq  < 0) || (lenOfReq >= (int)sizeof(reqStr)))	
	{
		ret = errCodeAPIClientReqLen;
		UnionUserErrLog("in %s:: lenOfReq = [%d] error! ret = [%d]\n",cliName,lenOfReq,ret);
		goto errorExit;
	}
	if (lenOfReq == 0)	// ���Ա���
	{
		UnionNullLog("in %s:: testing pack received!\n",cliName);
		goto loop;
	}
	// ���ý��տͻ������ݵĳ�ʱ����
#ifdef _WIN32
	// add codes here
#else
#if ( defined __linux__ ) || ( defined __hpux )
	if (sigsetjmp(gunionTCPSvrJmpEnvForEsscClient,1) != 0)	// ��ʱ�˳�
#elif ( defined _AIX )
	if (setjmp(gunionTCPSvrJmpEnvForEsscClient) != 0) // ��ʱ�˳�
#endif
	{
		ret = errCodeAPIRecvClientReqTimeout;
		UnionUserErrLog("in %s:: recv from client time out! ret = [%d]\n",cliName,ret);
		goto errorExit;
	}
	alarm(timeoutOfClient);
	signal(SIGALRM,UnionDealTCPSvrJmpEnvForEsscClient);
#endif
	// ���տͻ�������
	if ((ret = UnionReceiveFromSocketUntilLen(handle,reqStr,lenOfReq)) != lenOfReq)
	{
		UnionUserErrLog("in %s:: UnionReceiveFromSocketUntilLen expected [%d] real = [%d]!\n",cliName,lenOfReq,ret);
		ret = errCodeAPIClientReqLen;
		goto errorExit;
	}
	recvFromClientOK = 1;
#ifndef _WIN32
	alarm(0);
#ifndef _noRegisterClient_
	UnionIncreaseCommConfWorkingTimes(pgunionCommConf);
#endif
#endif
	// �ͻ���ʹ��ebcdic��ͨѶ
#ifdef _client_use_ebcdic_
	UnionEbcdicToAscii(reqStr,reqStr,lenOfReq);
#endif		
#ifndef _noSpierInfo_
	UnionSendRequestInfoToTransSpier(lenOfReq,(char *)reqStr);	// ��������Ϣ
#endif		
	// ִ��ָ��
#ifndef _WIN32
	sprintf(varName,"timeoutOf%s",UnionGetFunSvrName());
#else
	sprintf(varName,"timeoutOfTcpipSvr");
#endif

	oriHsmTaskID = UnionGetFixedMDLIDOfFunSvr();   //add by hzh in 2011.10.11����ԭʼ·��
#ifndef	_isXMLPackage_
	// add by xusj begin 20091116 ����·�ɹ���
	memcpy(reqHeader, reqStr, 6);
	if (UnionReadStringTypeRECVar(reqHeader) != NULL)
	{
		strcpy(specHsmTask, UnionReadStringTypeRECVar(reqHeader));
		sprintf(fixVarName,"fixedMDLIDOf%s",specHsmTask);
		UnionSetFixedMDLIDOfFunSvr(UnionReadLongTypeRECVar(fixVarName));
	}
	// add by xusj end 20091116
#endif

	if ((timeoutOfFunSvr = UnionReadIntTypeRECVar(varName)) <= 0)
		timeoutOfFunSvr = 5;
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((lenOfRes = UnionExchangeWithSpecModuleVerifyReqAndRes(UnionGetFixedMDLIDOfFunSvr(),reqStr,lenOfReq,
			tmpBuf,sizeof(tmpBuf)-1,timeoutOfFunSvr)) < 0)
	{
		UnionSetFixedMDLIDOfFunSvr(oriHsmTaskID);   //add by hzh in 2011.10.10�ָ�ԭʼ·��
		ret = lenOfRes;
		UnionUserErrLog("in %s:: UnionExchangeWithSpecModuleVerifyReqAndRes! ret = [%d]\n",cliName,ret);
		goto errorExit;
	}
	else
	{
		UnionSetFixedMDLIDOfFunSvr(oriHsmTaskID);   //add by hzh in 2011.10.10�ָ�ԭʼ·��
		ret = lenOfRes;
		finishThisTask = 0;
	}
		
sendToClient: // ��ͻ��˷�������
#ifndef _WIN32
	alarm(0);
#endif
#ifndef _noSpierInfo_
	UnionSendResponseInfoToTransSpier(lenOfRes,(char *)tmpBuf); // ��������Ϣ
#endif	
	memset(resStr,0,sizeof(resStr));
	if (UnionGetLenOfLengthField() == 2)
	{
		resStr[0] = lenOfRes / 256;
		resStr[1] = lenOfRes % 256;
		memcpy(resStr+2,tmpBuf,lenOfRes);
	}
	else
	{
		sprintf((char *)resStr,"%0*d",UnionGetLenOfLengthField(),lenOfRes);
		memcpy(resStr + UnionGetLenOfLengthField(),tmpBuf,lenOfRes);
	}
	
#ifdef _client_use_ebcdic_
	UnionAsciiToEbcdic(resStr+UnionGetLenOfLengthField(),resStr + UnionGetLenOfLengthField(),lenOfRes);
#endif	
	if ((ret = UnionSendToSocket(handle,resStr,lenOfRes+UnionGetLenOfLengthField())) < 0)
	{
		UnionUserErrLog("in %s:: UnionSendToSocket ret = [%d]!\n",cliName,ret);
		UnionFlushAllLogFileInGrpNoWait();  //add by hzh in 2012.12.13
		return(ret);
	}
	// ѭ������
	if (finishThisTask)
	{
		UnionFlushAllLogFileInGrpNoWait();  //add by hzh in 2012.12.13
		return(ret);
	}
	else
	{
#ifndef _nonUseDatabase_
		UnionCloseDatabase(); // add by xusj 20091215
#endif
		goto loop;
	}
errorExit:
	if (ret >= 0)
		ret = errCodeAPIErrCodeNotSet;
	if (!recvFromClientOK)
	{
		UnionFlushAllLogFileInGrpNoWait();  //add by hzh in 2012.12.13
		return(ret);
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
#ifdef	_isXMLPackage_
	lenOfRes = UnionSetXMLResponsePackageForError((char *)reqStr,ret,(char *)tmpBuf,sizeof(tmpBuf));
#else
	// 2011-11-04 ����������
	if (strlen((char *)reqStr) >= 5)
	{
		lenOfRes = 0;
		//  2�ֽ�Ӧ�ñ�� + 3�ֽڷ������
		memcpy(tmpBuf,reqStr,5);
		lenOfRes += 5;
		// 1�ֽ�����/��Ӧ��ʶ
		memcpy(tmpBuf + lenOfRes,"0",1);
		lenOfRes += 1;
		// 6�ֽڴ�����
		sprintf((char *)tmpBuf + lenOfRes,"%06d",ret);
		lenOfRes += 6;
		memset(errRemark,0,sizeof(errRemark));
		if ((remarkLen = UnionTranslateErrCodeIntoRemark(ret,errRemark,sizeof(errRemark))) > 0) // ת��������
		{
			memcpy(tmpBuf+lenOfRes,"001",3);
			lenOfRes += 3;
			memcpy(tmpBuf+lenOfRes,"999",3);
			lenOfRes += 3;
			sprintf((char *)tmpBuf+lenOfRes,"%04d",remarkLen);
			lenOfRes += 4;
			memcpy(tmpBuf+lenOfRes,errRemark,remarkLen);
			lenOfRes += remarkLen;
		}
	}
	// 2011-11-04 END
	else
		lenOfRes = UnionTranslateErrCodeIntoRemark(ret,(char *)tmpBuf,sizeof(tmpBuf));
#endif
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

