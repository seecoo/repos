//	Author:		������
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
�������:	E101
������:		ִ�������ָ��
��������:	ִ�������ָ��
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
	
	// ��ȡ�������������
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmCmdReq",hsmCmdReq,sizeof(hsmCmdReq))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE101:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmCmdReq");
		return(ret);
	}
	hsmCmdReq[ret] = 0;
	len = ret;

	// ��ȡ����������ʶ
	if ((ret = UnionReadRequestXMLPackageValue("body/ascFlag",ascFlag,sizeof(ascFlag))) <= 0)
		snprintf(ascFlag,sizeof(ascFlag),"%s","11");

	// ��ȡ�������Ϣͷ����
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
	
	// ���ò������Ӧ
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
                        UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
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
	
	// �����������Ӧ����
	if ((ret = UnionSetResponseXMLPackageValue("body/hsmCmdRes",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE101:: UnionSetResponseXMLPackageValue[%s]!\n","body/hsmCmdRes");
		return(ret);
	}
	
	return(0);
}


