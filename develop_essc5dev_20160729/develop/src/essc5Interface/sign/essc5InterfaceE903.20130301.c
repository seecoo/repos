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
#include "base64.h"

/***************************************
�������:	E903
������:		��ͨ��ǩ(881)
��������:	��ͨ��ǩ(881)
***************************************/
int UnionDealServiceCodeE903(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	orgData[8192];
	//char	bcdOrgData[8192];
	char	*bcdOrgData = NULL;
	char	appName[128];
	char	tmpBuf[128];
	char	signData[8192];
	char	certData[8192];
	char	sCert[8192];
	unsigned char	baseSignData[8192*2];
	int	algFlag;
	int	orgDataLen;
	int	signDataLen;
	int	isUseAppName;
	int	certExportFlag;
	int	iCertLen;
	int	signFlag;

	//CERTINFO	certInfo;

	// ��ȡԭʼ����
	if ((orgDataLen = UnionReadRequestXMLPackageValue("body/orgData",orgData,sizeof(orgData) - 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE903:: UnionReadRequestXMLPackageValue[%s]!\n","head/orgData");
		return(orgDataLen);
	}
	orgData[orgDataLen] = 0;
	/*
	if (orgDataLen % 2)
	{
		UnionUserErrLog("in UnionDealServiceCodeEES4:: orgDataLen[%d]!\n",orgDataLen);
		return(errCodeParameter);
	}
	aschex_to_bcdhex(orgData, orgDataLen, bcdOrgData);
	bcdOrgData[orgDataLen / 2] = 0;
	orgDataLen /= 2;
	*/
	bcdOrgData = orgData;

	// ֤��Ӧ����
	if ((ret = UnionReadRequestXMLPackageValue("body/appName",appName,sizeof(appName) - 1)) < 0)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/certData",certData,sizeof(certData) - 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE903:: UnionReadRequestXMLPackageValue[%s] or UnionReadRequestXMLPackageValue[%s]!\n","head/appName", "body/certData");
			return(ret);
		}
		else
		{
			certData[ret] = 0;
			isUseAppName = 0;
		}
	}
	else
	{
		appName[ret] = 0;
		isUseAppName = 1;
	}

	// �㷨��ʶ
	if ((ret = UnionReadRequestXMLPackageValue("body/algFlag",tmpBuf,sizeof(tmpBuf) - 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE903:: UnionReadRequestXMLPackageValue[%s]!\n","head/algFlag");
		return(ret);
	}
	tmpBuf[ret] = 0;
	algFlag = atoi(tmpBuf);

	// ǩ����������
	if ((ret = UnionReadRequestXMLPackageValue("body/signFlag",tmpBuf,sizeof(tmpBuf) - 1)) < 0)
	{
		signFlag = 0;
	}
	else
	{
		tmpBuf[ret] = 0;
		signFlag = atoi(tmpBuf);
	}
	if (signFlag != 0 && signFlag != 1)
	{
		UnionUserErrLog("in UnionDealServiceCodeE903:: signFlag[%d] error!\n",signFlag);
		return(errCodeParameter);
	}

	// ǩ��
	if ((ret = UnionReadRequestXMLPackageValue("body/signData",(char *)baseSignData,sizeof(baseSignData) - 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE903:: UnionReadRequestXMLPackageValue[%s]!\n","head/signData");
		return(ret);
	}
	baseSignData[ret] = 0;
	if (signFlag == 0)
	{
		aschex_to_bcdhex((char *)baseSignData, ret, signData);
		signData[ret / 2] = 0;
		signDataLen = ret / 2;
	}
	else
	{
		signDataLen = from64tobits(signData, (char *)baseSignData);
		signData[signDataLen] = 0;
	}

	// ���֤���ʶ
	if (isUseAppName)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/certExportFlag",tmpBuf,sizeof(tmpBuf) - 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE903:: UnionReadRequestXMLPackageValue[%s]!\n","head/certExportFlag");
			return(ret);
		}
		tmpBuf[ret] = 0;
		certExportFlag = atoi(tmpBuf);
	}

	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case conHsmCmdVerSRJ1401StandardHsmCmd:
			if (isUseAppName)
			{
				if ((ret = UnionSRJ1401CmdCM881((unsigned char *)bcdOrgData, orgDataLen, appName,NULL, signData,signDataLen, algFlag, certExportFlag, sCert, &iCertLen)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE903:: UnionSRJ1401CmdCM881 use appName,ret = [%d]!\n",ret);
					return(ret);
				}
			}
			else
			{
				if ((ret = UnionSRJ1401CmdCM881((unsigned char *)bcdOrgData, orgDataLen, NULL,certData, signData,signDataLen, algFlag, certExportFlag, sCert, &iCertLen)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE903:: UnionSRJ1401CmdCM881 use certData,ret = [%d]!\n",ret);
					return(ret);
				}
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE903:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
			return(errCodeParameter);
	}

	// ����֤������
	if (isUseAppName && certExportFlag)
	{
		if((ret = UnionSetResponseXMLPackageValue("body/certData",sCert)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE903:: UnionSetResponseXMLPackageValue[%s] certData[%s]!\n","body/certData", sCert);
			return(ret);
		}
	}

	return(0);
}

