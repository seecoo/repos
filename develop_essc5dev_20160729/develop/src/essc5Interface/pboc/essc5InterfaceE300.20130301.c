//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "commWithHsmSvr.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "unionHsmCmdVersion.h"

/***************************************
�������:	E300
������:		���������״̬
��������:	���������״̬
***************************************/
int UnionDealServiceCodeE300(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	int	ipSpecHsm = 0;
	char	hsmIP[32+1];
	char	appID[40+1];
	char	sql[2048+1];
	char	status[1+1];
	char	hsmCmdReq[512+1];
	char	hsmCmdRes[512+1];
	char	resCmdOfTest[512+1];	

	// ��ȡ�����IP
	memset(hsmIP,0,sizeof(hsmIP));
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmIP",hsmIP,sizeof(hsmIP))) <= 0)
	{
		// ��ȡӦ�ñ��
		memset(appID,0,sizeof(appID));
		if ((ret = UnionReadRequestXMLPackageValue("head/appID",appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE300:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");
			return(ret);
		}
		memset(sql,0,sizeof(sql));	
		sprintf(sql,"select hsmGroup.reqCmdOfTest,hsmGroup.resCmdOfTest from hsmGroup left join externalAPP on externalAPP.hsmGroupID = hsmGroup.hsmGroupID where externalAPP.appID = '%s' and externalAPP.enabled = 1",appID);
		// ��ѯ���ݿ�
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE300:: UnionSelectRealDBRecord[%s]!\n",sql);
			return(ret);
		}
		else if(ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE300:: appID[%s]!\n",appID);
			return(errCodeEssc_AppNotDefined);
		}
	}
	else
	{
		// ��֤IP�Ϸ���
		if (!UnionIsValidIPAddrStr(hsmIP))
		{
			UnionUserErrLog("in UnionDealServiceCodeE300:: hsmIP[%s] error!\n",hsmIP);
			return(errCodeInvalidIPAddr);
		}

		ipSpecHsm = 1;
		memset(sql,0,sizeof(sql));	
		sprintf(sql,"select hsmGroup.reqCmdOfTest,hsmGroup.resCmdOfTest,hsm.status from hsmGroup left join hsm on hsm.hsmGroupID = hsmGroup.hsmGroupID where hsm.ipAddr = '%s' and hsm.enabled = 1",hsmIP);
		// ��ѯ���ݿ�
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE300:: UnionSelectRealDBRecord[%s]!\n",sql);
			return(ret);
		}
		else if(ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE300:: hsmIP[%s]!\n",hsmIP);
			return(errCodeSJL06MDL_HsmIPNotExists);
		}
	}

	if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE300:: UnionLocateXMLPackage[%s]!\n","detail");
		return(ret);
	}

	if (ipSpecHsm)
	{
		// ��ȡ�����״̬
		memset(status,0,sizeof(status));
		if ((ret = UnionReadXMLPackageValue("status",status,sizeof(status))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE300:: UnionReadXMLPackageValue[%s]!\n","status");
			return(ret);
		}
	
		// ��������״̬
		if (status[0] != '1')
		{
			UnionUserErrLog("in UnionDealServiceCodeE300:: hsmIP[%s] not online!\n",hsmIP);
			return(errCodeSJL06MDL_SJL06StillNotOnline);	
		}
	}
	
	// ��ȡ̽������ָ�� 
	memset(hsmCmdReq,0,sizeof(hsmCmdReq));
	if ((ret = UnionReadXMLPackageValue("reqCmdOfTest",hsmCmdReq,sizeof(hsmCmdReq))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE300:: UnionReadXMLPackageValue[%s]!\n","reqCmdOfTest");
		return(ret);
	}

	// ��ȡ̽����Ӧָ��
	memset(resCmdOfTest,0,sizeof(resCmdOfTest));
	if ((ret = UnionReadXMLPackageValue("resCmdOfTest",resCmdOfTest,sizeof(resCmdOfTest))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE300:: UnionReadXMLPackageValue[%s]!\n","resCmdOfTest");
		return(ret);
	}

	// �趨ָ���������
	if (ipSpecHsm)
		UnionSetUseSpecHsmIPAddrForOneCmd(hsmIP);

	memset(hsmCmdRes,0,sizeof(hsmCmdRes));
	switch(phsmGroupRec->hsmCmdVersionID)
        {
                case conHsmCmdVerRacalStandardHsmCmd:
                case conHsmCmdVerSJL06StandardHsmCmd:
			if ((ret =  UnionDirectHsmCmd(hsmCmdReq,strlen(hsmCmdReq),hsmCmdRes,sizeof(hsmCmdRes))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE300:: UnionDirectHsmCmd,ret = [%d]!\n",ret);
				return(ret);
			}
		 	break;
                default:
                        UnionUserErrLog("in UnionDealServiceCodeE300:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                        return(errCodeParameter);
        }

	// �����Ӧ��
	if (memcmp(hsmCmdRes,resCmdOfTest,strlen(resCmdOfTest)) != 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE300:: hsmCmdRes[%s] != resCmdOfTest[%s]!\n",hsmCmdRes,resCmdOfTest);
		return(errCodeSJL06MDL_SJL06Abnormal);
	}
	
	return(0);
}



