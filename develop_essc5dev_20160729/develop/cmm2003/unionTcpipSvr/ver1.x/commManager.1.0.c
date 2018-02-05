// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2011-1-23

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
#include "unionVersion.h"

#ifdef _client_use_ebcdic_
#include "ebcdicAscii.h"
#endif

#include "UnionSocket.h"
#include "commManager.h"

#ifndef _WIN32
int gsynchTCPIPSvrSckHDL = -1;
extern PUnionTaskInstance 	ptaskInstance;

#ifndef _noRegisterClient_
PUnionCommConf			pgunionCommConf = NULL;
#endif

/*
����
	��ȡ��ǰͨѶ���
�������
	��
�������
	��
����ֵ
	>=0		��ǰͨѶ���
	<0		ʧ�ܣ�������
*/
int UnionGetActiveTCIPSvrSckHDL()
{
	return(gsynchTCPIPSvrSckHDL);
}

/*
����
	���õ�ǰͨѶ���
�������
	sckHDL		��ǰͨѶ���
�������
	��
����ֵ
	��
*/
void UnionSetActiveTCPIPSvrSckHDL(int sckHDL)
{
	sckHDL = gsynchTCPIPSvrSckHDL;
	return;
}

/*
����
	������Ŀ��1
�������
	��
�������
	��
����ֵ
	��
*/
void UnionIncreaseClientPackNum()
{
#ifndef _noRegisterClient_	// ���ͻ��˰���Ŀ��1
	UnionIncreaseCommConfWorkingTimes(pgunionCommConf);
#endif
	return;
}
#endif

/*
����
	�Ǽ�����
�������
	cli_addr	�ͻ�����Ϣ
	port		�ͻ��˶˿�
�������
	��
����ֵ
	>=0		�ɹ�
	<0		ʧ�ܣ�������
*/
int UnionRegisterTcpipConnTask(struct sockaddr_in *cli_addr,int port,int (*UnionTaskActionBeforeExit)())
{
	int		ret;
	char		cliName[100];
	char		cliIPAddr[40+1];
	char		varName[256+1];
	int		connType;
	char		serviceName[128+1];
	char		*ptr;
	int		resID;
	
	memset(cliIPAddr,0,sizeof(cliIPAddr));
	inet_ntop(AF_INET, (void *)&cli_addr->sin_addr, cliIPAddr, sizeof(cliIPAddr));
	ret = sprintf(cliName,"%s %d",cliIPAddr,port);
	cliName[ret] = 0;

#ifndef _noSpierInfo_
	UnionSetClientAttrToldTransSpier(cliIPAddr,port);
	sprintf(varName,"resIDOf%s",UnionGetApplicationName());
	UnionSetMyTransInfoResID(UnionReadIntTypeRECVar(varName));
#endif

	// ��������ʵ��
	if (UnionExistsTaskClassOfName(cliName))
	{
		if ((ptaskInstance = UnionCreateTaskInstanceOfName(UnionTaskActionBeforeExit,cliName)) == NULL)
		{
			UnionUserErrLog("in %s:: UnionCreateTaskInstanceOfName! ret = [%d]\n",cliName,ret);
			return(UnionSetUserDefinedErrorCode(errCodeCreateTaskInstance));
		}
	}
	else
	{
		if ((ptaskInstance = UnionCreateTaskInstanceOfName(UnionTaskActionBeforeExit,"client %d",port)) == NULL)
		{
			ret = errCodeCreateTaskInstance;
			UnionUserErrLog("in %s:: UnionCreateTaskInstanceOfName! ret = [%d]\n",cliName,ret);
			return(UnionSetUserDefinedErrorCode(errCodeCreateTaskInstance));
		}
	}
#ifndef _noRegisterClient_
	sprintf(varName,"isShortConnOf%s",UnionGetApplicationName());
	if (UnionReadIntTypeRECVar(varName) > 0)
		connType = conCommShortConn;
	else
		connType = conCommLongConn;
	sprintf(varName,"serviceNameOf%s",UnionGetApplicationName());
	if ((ptr = UnionReadStringTypeRECVar(varName)) == NULL)
		strcpy(serviceName,"��������");
	else
		strcpy(serviceName,"ptr");
	if ((pgunionCommConf = UnionAddServerCommConf(cliIPAddr,port,connType,serviceName)) == NULL)
	{
#ifdef _limitClient_
		UnionUserErrLog("in %s:: UnionAddServerCommConf [%s] [%d]! ret = [%d]\n",cliName,cliIPAddr,port,ret);
		return(UnionSetUserDefinedErrorCode(errCodeYLCommConfMDL_AddCommConf));
#endif
	}
	UnionSetCommConfOK(pgunionCommConf);
#endif

	return(0);
}
