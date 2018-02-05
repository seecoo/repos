// Wolfgang Wang,2009/5/14
// KMC的缺省加密机程序

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
功能
	获取KMC的缺省密码机
输入参数
	无
输入出数
	无
返回值
	成功，缺省密码机的地址指针
	失败，空指针
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

	// 读取IP地址
	if ((ipAddr = UnionReadStringTypeRECVar("defaultHsmIPAddrOfMngSvr")) == NULL)
	{
		UnionUserErrLog("in UnionFindDefaultKMCSJL06:: UnionReadStringTypeRECVar [defaultHsmIPAddrOfMngSvr]!\n");
		return(NULL);
	}
	// 读取端口
	if ((port = UnionReadIntTypeRECVar("defaultHsmPortOfMngSvr")) <= 0)
	{
		UnionLog("in UnionFindDefaultKMCSJL06:: UnionReadStringTypeRECVar [defaultHsmPortOfMngSvr] failure!\n");
		port = 8;
	}
	// 读取消息头
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
	// 动态属性
	psjl06->dynamicAttr.status = conOnlineSJL06;

	gunionIsDefaultMngSvrSJL06Inited = 1;
	return(&gunionDefaultMngSvrSJL06);
}
