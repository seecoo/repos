//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5Interface.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "commWithHsmSvr.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "unionHsmCmdVersion.h"

/***************************************
服务代码:	E101
服务名:		执行密码机指令
功能描述:	执行密码机指令
***************************************/
int UnionDealServiceCodeE101(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	int	len = 0;
	char	hsmCmdReq[8192];
	char	hsmCmdRes[8192];
	char	ascFlag[32];
	int	lenOfMsgHead = 0;
	char	tmpBuf[8192];
	char	msgHead[128];
	
	// 读取密码机请求数据
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmCmdReq",hsmCmdReq,sizeof(hsmCmdReq))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE101:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmCmdReq");
		return(ret);
	}
	hsmCmdReq[ret] = 0;
	len = ret;

	// 读取密码机编码标识
	if ((ret = UnionReadRequestXMLPackageValue("body/ascFlag",ascFlag,sizeof(ascFlag))) <= 0)
		snprintf(ascFlag,sizeof(ascFlag),"%s","11");

	// 读取密码机消息头长度
	if ((ret = UnionReadRequestXMLPackageValue("body/lenOfMsgHead",tmpBuf,sizeof(tmpBuf))) <= 0)
		lenOfMsgHead = 0;	
	else
	{
		tmpBuf[ret] = 0;
		lenOfMsgHead = atoi(tmpBuf);
	}

	if (ascFlag[0] == '1')
	{
		memcpy(tmpBuf,hsmCmdReq,len);
		if (len <= lenOfMsgHead)
		{
			UnionUserErrLog("in UnionDealServiceCodeE101:: hsmCmdReq[%s] len[%d] <= lenOfMsgHead[%d] error!\n",hsmCmdReq,len,lenOfMsgHead);
			return(errCodeParameter);
		}
		memcpy(msgHead,hsmCmdReq,lenOfMsgHead);
		msgHead[lenOfMsgHead] = 0;
	}
	else
	{
		aschex_to_bcdhex(hsmCmdReq,len,tmpBuf);
		if (strlen(hsmCmdReq) <= lenOfMsgHead*2)
		{
			UnionUserErrLog("in UnionDealServiceCodeE101:: hsmCmdReq[%s] len[%zu] <= lenOfMsgHead[%d] error!\n",hsmCmdReq,strlen(hsmCmdReq),lenOfMsgHead*2);
			return(errCodeParameter);
		}
		aschex_to_bcdhex(hsmCmdReq,lenOfMsgHead * 2,msgHead);
		msgHead[lenOfMsgHead] = 0;
		len = len / 2;
	}
	
	// 设置不检查响应
	UnionSetIsNotCheckHsmResCode();

	switch(phsmGroupRec->hsmCmdVersionID)
        {
                case conHsmCmdVerRacalStandardHsmCmd:
                case conHsmCmdVerSJL06StandardHsmCmd:
			if ((ret =  UnionDirectHsmCmd(tmpBuf + lenOfMsgHead,len - lenOfMsgHead,hsmCmdRes,sizeof(hsmCmdRes))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE101:: UnionDirectHsmCmd,ret = [%d]!\n",ret);
				UnionSetIsCheckHsmResCode();
				return(ret);
			}
			break;
                default:
                        UnionUserErrLog("in UnionDealServiceCodeE101:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetIsCheckHsmResCode();
                        UnionSetResponseRemark("非法的加密机指令类型");
                        return(errCodeParameter);
        }
	UnionSetIsCheckHsmResCode();

	len = ret;

	if (ascFlag[1] == '1')
	{
		snprintf(tmpBuf,sizeof(tmpBuf),"%s%s",msgHead,hsmCmdRes);
	}
	else
	{
		bcdhex_to_aschex(msgHead,lenOfMsgHead,tmpBuf);
		bcdhex_to_aschex(hsmCmdRes,len,tmpBuf + lenOfMsgHead * 2);
		tmpBuf[lenOfMsgHead * 2 + len * 2] = 0;
	}
	
	// 设置密码机响应数据
	if ((ret = UnionSetResponseXMLPackageValue("body/hsmCmdRes",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE101:: UnionSetResponseXMLPackageValue[%s]!\n","body/hsmCmdRes");
		return(ret);
	}
	
	return(0);
}


