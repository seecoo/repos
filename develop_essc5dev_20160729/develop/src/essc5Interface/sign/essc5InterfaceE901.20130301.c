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
#include "unionSRJ1401Cmd.h"

/***************************************
�������:	E901
������:		����ǩ����ǩ�����״̬
��������:	����ǩ����ǩ�����״̬
***************************************/
int UnionDealServiceCodeE901(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	int	ipSpecHsm = 0;
	char	hsmIP[64];
	char	appID[32];
	char	sql[2048];
	char	status[32];
	char	hsmCmdReq[512];
	char	hsmCmdRes[512];
	char	resCmdOfTest[512];	

	// add by liwj 2015-05-26
	char	hsmGroupIDList[512];
	char	hsmGroupID[128];
	char	*p;
	// add end

	// ��ȡ�����IP
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmIP",hsmIP,sizeof(hsmIP))) <= 0)
	{
		// ��ȡӦ�ñ��
		//memset(appID,0,sizeof(appID));
		if ((ret = UnionReadRequestXMLPackageValue("head/appID",appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE901:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");
			return(ret);
		}
		//memset(sql,0,sizeof(sql));	
		// modify by liwj 2015-05-26
		snprintf(sql, sizeof(sql), "select hsmGroupIDList from app where app.appID = '%s' and app.enabled = 1", appID);
		// ��ѯ���ݿ�
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE901:: UnionSelectRealDBRecord[%s]!\n",sql);
			return(ret);
		}
		else if(ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE901:: appID[%s]!\n",appID);
			return(errCodeEssc_AppNotDefined);
		}
		if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE901:: UnionLocateXMLPackage[%s]!\n","detail");
			return(ret);
		}
		if ((ret = UnionReadXMLPackageValue("hsmGroupIDList",hsmGroupIDList,sizeof(hsmGroupIDList))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE901:: UnionReadXMLPackageValue[%s]!\n","hsmGroupIDList");
			return(ret);
		}

		if ((p = strchr(hsmGroupIDList, ',')) != NULL)
		{
			*p = 0;
		}
		snprintf(hsmGroupID, sizeof(hsmGroupID), "%s", hsmGroupIDList);

		snprintf(sql,sizeof(sql),"select reqCmdOfTest, resCmdOfTest from hsmGroup where hsmGroupID = '%s'", hsmGroupID);
		//snprintf(sql,sizeof(sql),"select hsmGroup.reqCmdOfTest,hsmGroup.resCmdOfTest from hsmGroup left join app on app.hsmGroupID = hsmGroup.hsmGroupID where app.appID = '%s' and app.enabled = 1",appID);
		// ��ѯ���ݿ�
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE901:: UnionSelectRealDBRecord[%s]!\n",sql);
			return(ret);
		}
		else if(ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE901:: appID[%s]!\n",appID);
			return(errCodeEsscMDL_EsscHsmGrpIDNotDefined);
		}
		// modify end by liwj
	}
	else
	{
		// ��֤IP�Ϸ���
		/*if (!UnionIsValidIPAddrStr(hsmIP))
		{
			UnionUserErrLog("in UnionDealServiceCodeE901:: hsmIP[%s] error!\n",hsmIP);
			return(errCodeInvalidIPAddr);
		}*/

		ipSpecHsm = 1;
		//memset(sql,0,sizeof(sql));	
		sprintf(sql,"select hsmGroup.reqCmdOfTest,hsmGroup.resCmdOfTest,hsm.status from hsmGroup left join hsm on hsm.hsmGroupID = hsmGroup.hsmGroupID where hsm.ipAddr = '%s' and hsm.enabled = 1",hsmIP);
		// ��ѯ���ݿ�
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE901:: UnionSelectRealDBRecord[%s]!\n",sql);
			return(ret);
		}
		else if(ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE901:: hsmIP[%s]!\n",hsmIP);
			return(errCodeSJL06MDL_HsmIPNotExists);
		}
	}

	if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE901:: UnionLocateXMLPackage[%s]!\n","detail");
		return(ret);
	}

	if (ipSpecHsm)
	{
		// ��ȡ�����״̬
		//memset(status,0,sizeof(status));
		if ((ret = UnionReadXMLPackageValue("status",status,sizeof(status))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE901:: UnionReadXMLPackageValue[%s]!\n","status");
			return(ret);
		}
	
		// ��������״̬
		if (status[0] != '1')
		{
			UnionUserErrLog("in UnionDealServiceCodeE901:: hsmIP[%s] not online!\n",hsmIP);
			return(errCodeSJL06MDL_SJL06StillNotOnline);	
		}
	}
	
	// ��ȡ̽������ָ�� 
	//memset(hsmCmdReq,0,sizeof(hsmCmdReq));
	if ((ret = UnionReadXMLPackageValue("reqCmdOfTest",hsmCmdReq,sizeof(hsmCmdReq))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE901:: UnionReadXMLPackageValue[%s]!\n","reqCmdOfTest");
		return(ret);
	}

	// ��ȡ̽����Ӧָ��
	//memset(resCmdOfTest,0,sizeof(resCmdOfTest));
	if ((ret = UnionReadXMLPackageValue("resCmdOfTest",resCmdOfTest,sizeof(resCmdOfTest))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE901:: UnionReadXMLPackageValue[%s]!\n","resCmdOfTest");
		return(ret);
	}

	// �趨ָ���������
	if (ipSpecHsm)
		UnionSetUseSpecHsmIPAddrForOneCmd(hsmIP);

	//memset(hsmCmdRes,0,sizeof(hsmCmdRes));
	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case conHsmCmdVerRacalStandardHsmCmd:
		case conHsmCmdVerSJL06StandardHsmCmd:
			if ((ret =  UnionDirectHsmCmd(hsmCmdReq,strlen(hsmCmdReq),hsmCmdRes,sizeof(hsmCmdRes))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE901:: UnionDirectHsmCmd,ret = [%d]!\n",ret);
				return(ret);
			}
			break;
		case conHsmCmdVerSRJ1401StandardHsmCmd:
			if ((ret = UnionSRJ1401CmdCM100(hsmCmdReq, strlen(hsmCmdReq), hsmCmdRes,sizeof(hsmCmdRes))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE901:: UnionSRJ1401CmdCM100,ret = [%d]!\n",ret);
				return(ret);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE901:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
			return(errCodeParameter);
	}

	// �����Ӧ��
	if (memcmp(hsmCmdRes,resCmdOfTest,strlen(resCmdOfTest)) != 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE901:: hsmCmdRes[%s] != resCmdOfTest[%s]!\n",hsmCmdRes,resCmdOfTest);
		return(errCodeSJL06MDL_SJL06Abnormal);
	}
	
	return(0);
}



