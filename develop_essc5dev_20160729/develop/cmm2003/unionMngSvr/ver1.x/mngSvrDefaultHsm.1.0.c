// Wolfgang Wang,2009/5/14
// KMC��ȱʡ���ܻ�����

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionREC.h"
#include "UnionLog.h"
#include "unionErrCode.h"
#include "sjl06.h"

TUnionSJL06  gunionDefaultMngSvrSJL06;
int	gunionIsDefaultMngSvrSJL06Inited = 0;

/*
����
	��ȡKMC��ȱʡ�����
�������
	��
�������
	��
����ֵ
	�ɹ���ȱʡ������ĵ�ַָ��
	ʧ�ܣ���ָ��
*/
PUnionSJL06 UnionFindDefaultKMCSJL06()
{
	int		ret;
	char    	*ipAddr;
	int		port;
	int		lenOfMsgHeader;
	PUnionSJL06	psjl06;
		
	if (gunionIsDefaultMngSvrSJL06Inited)
		return(&gunionDefaultMngSvrSJL06);

	// ��ȡIP��ַ
	if ((ipAddr = UnionReadStringTypeRECVar("defaultHsmIPAddrOfMngSvr")) == NULL)
	{
		UnionUserErrLog("in UnionFindDefaultKMCSJL06:: UnionReadStringTypeRECVar [defaultHsmIPAddrOfMngSvr]!\n");
		return(NULL);
	}
	// ��ȡ�˿�
	if ((port = UnionReadIntTypeRECVar("defaultHsmPortOfMngSvr")) <= 0)
	{
		UnionLog("in UnionFindDefaultKMCSJL06:: UnionReadStringTypeRECVar [defaultHsmPortOfMngSvr] failure!\n");
		port = 8;
	}
	// ��ȡ��Ϣͷ
	if ((lenOfMsgHeader = UnionReadIntTypeRECVar("defaultHsmMsgHeaderLenOfMngSvr")) < 0)
	{
		UnionLog("in UnionFindDefaultKMCSJL06:: UnionReadStringTypeRECVar [defaultHsmMsgHeaderLenOfMngSvr] failure!\n");
		lenOfMsgHeader = 8;
	}
	
	psjl06 = &gunionDefaultMngSvrSJL06;
	strcpy(psjl06->staticAttr.hsmGrpID,"102");
	memset(&gunionDefaultMngSvrSJL06, 0,sizeof(gunionDefaultMngSvrSJL06));
	strcpy(psjl06->staticAttr.hsmCmdVersion,conHsmCmdVersionSJL06);
	strcpy(psjl06->staticAttr.ipAddr,ipAddr);
	psjl06->staticAttr.port = port;
	psjl06->staticAttr.lenOfLenFld = 2;
	psjl06->staticAttr.lenOfMsgHeader = lenOfMsgHeader;
	memset(psjl06->staticAttr.msgHeader,'0',lenOfMsgHeader);
	psjl06->staticAttr.maxConLongConn = 128;
	// ��̬����
	psjl06->dynamicAttr.status = conOnlineSJL06;

	gunionIsDefaultMngSvrSJL06Inited = 1;
	return(&gunionDefaultMngSvrSJL06);
}
