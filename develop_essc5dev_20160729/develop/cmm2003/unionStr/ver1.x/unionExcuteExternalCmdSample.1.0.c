// Wolfgang Wang
// 2010-8-19

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionExcuteExternalCmdFile.h"

#include "sjl05HsmApiSample.h"

/*
����
	ִ��һ���ⲿ����-�����������֮�������
�������
	reqStr		�ⲿ���������
	lenOfReqStr	�ⲿ��������󴮳���
	sizeOfResStr	��Ӧ���Ĵ�С
�������
	resStr		�������Ӧ��
����ֵ
	>=0		��Ӧ���ĳ���
	<0		�������
*/
int UnionExcuteFun_ConnectHsmSJL05(char *reqStr,int lenOfReqStr)
{
	char	ipAddr[40+1];
	int	port = -1;
	int	ret;
	
	// ��ȡ���ܻ�IP��ַ
	memset(ipAddr,0,sizeof(ipAddr));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"hsmIPAddr",ipAddr,sizeof(ipAddr))) < 0)
	{
		UnionUserErrLog("in UnionExcuteFun_ConnectHsmSJL05:: UnionReadRecFldFromRecStr [%s] from [%04d][%s]\n","hsmIPAddr",lenOfReqStr,reqStr);
		return(ret);
	}
	// ��ȡ���ܻ��˿�
	if ((ret = UnionReadIntTypeRecFldFromRecStr(reqStr,lenOfReqStr,"hsmPort",&port)) < 0)
	{
		UnionUserErrLog("in UnionExcuteFun_ConnectHsmSJL05:: UnionReadRecFldFromRecStr [%s] from [%04d][%s]\n","hsmPort",lenOfReqStr,reqStr);
		return(ret);
	}
	// ��������ܻ������ĺ���
	if ((ret = ConnectHsmSJL05(ipAddr,port)) < 0)
	{
		UnionUserErrLog("in UnionExcuteFun_ConnectHsmSJL05:: ConnectHsmSJL05 [%s] [%d]!\n",ipAddr,port);
		return(ret);
	}
	return(0);
}


/*
����
	ִ��һ���ⲿ����-�Ͽ��������֮�������
�������
	reqStr		�ⲿ���������
	lenOfReqStr	�ⲿ��������󴮳���
	sizeOfResStr	��Ӧ���Ĵ�С
�������
	resStr		�������Ӧ��
����ֵ
	>=0		��Ӧ���ĳ���
	<0		�������
*/
int UnionExcuteFun_CloseHsmSJL05()
{
	int	ret;
	
	// ��������ܻ������ĺ���
	if ((ret = CloseHsmSJL05()) < 0)
	{
		UnionUserErrLog("in UnionExcuteFun_CloseHsmSJL05:: CloseHsmSJL05!\n");
		return(ret);
	}
	return(0);
}

/*
����
	ִ��һ���ⲿ����-ִ�������ָ��
�������
	reqStr		�ⲿ���������
	lenOfReqStr	�ⲿ��������󴮳���
	sizeOfResStr	��Ӧ���Ĵ�С
�������
	resStr		�������Ӧ��
����ֵ
	>=0		��Ӧ���ĳ���
	<0		�������
*/
int UnionExcuteFun_ExcuteHsmSJL05Cmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	char	hsmCmd[4096+1];
	int	ret;
	int	lenOfHsmCmdReqStr;
	
	// ��ȡ���ܻ�ָ������
	memset(hsmCmd,0,sizeof(hsmCmd));
	if ((lenOfHsmCmdReqStr = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"hsmCmdReqStr",hsmCmd,sizeof(hsmCmd))) < 0)
	{
		UnionUserErrLog("in UnionExcuteFun_ExcuteHsmSJL05Cmd:: UnionReadRecFldFromRecStr [%s] from [%04d][%s]\n","hsmCmdReqStr",lenOfReqStr,reqStr);
		return(lenOfHsmCmdReqStr);
	}
	// ����ִ�м��ܻ�ָ��ĺ���
	if ((ret = ExcuteHsmSJL05Cmd(hsmCmd,lenOfHsmCmdReqStr,hsmCmd,sizeof(hsmCmd))) < 0)
	{
		UnionUserErrLog("in UnionExcuteFun_ExcuteHsmSJL05Cmd:: ExcuteHsmSJL05Cmd [%04d][%s]!\n",lenOfHsmCmdReqStr,hsmCmd);
		return(ret);
	}
	hsmCmd[ret] = 0;
	if ((ret = UnionPutRecFldIntoRecStr("hsmCmdResStr",hsmCmd,ret,resStr,sizeOfResStr)) < 0)
	{
		UnionUserErrLog("in UnionExcuteFun_ExcuteHsmSJL05Cmd:: UnionPutRecFldIntoRecStr [hsmCmdResStr]!\n");
		return(ret);
	}		
	return(ret);
}

/*
����
	ִ��һ���ⲿ����
�������
	reqStr		�ⲿ���������
	lenOfReqStr	�ⲿ��������󴮳���
	sizeOfResStr	��Ӧ���Ĵ�С
�������
	resStr		�������Ӧ��
����ֵ
	>=0		��Ӧ���ĳ���
	<0		�������
*/
int UnionExcuteExternalCmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	char	funName[128+1];
	int	ret;
	
	// ��ȡ��������
	memset(funName,0,sizeof(funName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"funName",funName,sizeof(funName))) < 0)
	{
		UnionUserErrLog("in UnionExcuteExternalCmd:: UnionReadRecFldFromRecStr [%s] from [%04d][%s]\n","funName",lenOfReqStr,reqStr);
		return(ret);
	}
	
	if (strcmp(funName,"ConnectHsmSJL05") == 0)		// ��������ܻ�������
		return(UnionExcuteFun_ConnectHsmSJL05(reqStr,lenOfReqStr));
	else if (strcmp(funName,"CloseHsmSJL05") == 0)	// �ر�����ܻ�������
		return(UnionExcuteFun_CloseHsmSJL05());
	else if (strcmp(funName,"ExcuteHsmSJL05Cmd") == 0)	// ִ�м��ܻ�ָ��
		return(UnionExcuteFun_ExcuteHsmSJL05Cmd(reqStr,lenOfReqStr,resStr,sizeOfResStr));
	/*
	else if (strcmp(funName,"UserInput") == 0)	// �û�����
		return(UnionExcuteFun_ExcuteUserInput(reqStr,lenOfReqStr,resStr,sizeOfResStr));
	*/
	else
	{
		UnionUserErrLog("in UnionExcuteExternalCmd:: funName = [%s] not supported!\n",funName);
		return(errCodeFunctionNotSupported);
	}
}
