// Wolfgang Wang
// 2010-8-19

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "UnionLog.h"
#include "UnionSocket.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "sjl05HsmApiSample.h"
#include "unionREC.h"

// �������֮�������
int gsjl06HsmSckHDL = -1;

/*
����
	�����������֮�������
�������
	ipAddr		�����IP��ַ
	port		������˿�
�������
	��
����ֵ
	>=0		�ɹ������������
	<0		�������
*/
int ConnectHsmSJL06(char *ipAddr,int port)
{
	if (gsjl06HsmSckHDL >= 0)	// �ر�֮�������
		UnionCloseSocket(gsjl06HsmSckHDL);
	if ((gsjl06HsmSckHDL = UnionCreateSocketClient(ipAddr,port)) < 0)
	{
		UnionUserErrLog("in ConnectHsmSJL06:: UnionCreateSocketClient [%s][%d]!\n",ipAddr,port);
	}
	return(gsjl06HsmSckHDL);
}

/*
����
	�ر��������֮�������
�������
	��
�������
	��
����ֵ
	>=0		�ɹ������������
	<0		�������
*/
int CloseHsmSJL06()
{
	if (gsjl06HsmSckHDL >= 0)	// �ر�֮�������
		UnionCloseSocket(gsjl06HsmSckHDL);
	gsjl06HsmSckHDL = -1;
	return(0);
}

/*
����
	ִ�������ָ��
�������
	reqStr		���������
	lenOfReqStr	��������󴮳���
	sizeOfResStr	��Ӧ���Ĵ�С
�������
	resStr		�������Ӧ��
����ֵ
	>=0		��Ӧ���ĳ���
	<0		�������
*/
int ExcuteHsmSJL06Cmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	if (gsjl06HsmSckHDL >= 0)
		return(UnionCommWithSpecSckHDLWith2BytsLen(gsjl06HsmSckHDL,(unsigned char *)reqStr,lenOfReqStr,
				(unsigned char *)resStr,sizeOfResStr,1));
	else
		return(UnionCommWithSpecSvrWith2BytsLen(UnionReadStringTypeRECVar("ipAddrOfHsmModel"),
				UnionReadIntTypeRECVar("portOfHsmModel"),
				(unsigned char *)reqStr,lenOfReqStr,(unsigned char *)resStr,sizeOfResStr,1));
}
