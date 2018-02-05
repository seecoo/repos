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
�������:	E912
������:		�ӷ���������֤��(873)
��������:	�ӷ���������֤��(873)
***************************************/
int UnionDealServiceCodeE912(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	appName[128];
	char	certData[8192];
	int	certDataLen;

	// ֤��Ӧ����
	if ((ret = UnionReadRequestXMLPackageValue("body/appName",appName,sizeof(appName) - 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE912:: UnionReadRequestXMLPackageValue[%s]!\n","head/appName");
		return(ret);
	}
	appName[ret] = 0;

	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case conHsmCmdVerSRJ1401StandardHsmCmd:
			if (( ret = UnionSRJ1401CmdCM873(appName, (unsigned char *)certData, &certDataLen)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE912:: UnionSRJ1401CmdCM873,ret = [%d]!\n",ret);
				return(ret);
			}
			certData[certDataLen] = 0;
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE912:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
			return(errCodeParameter);
	}

	// ����֤����ڱ�ʶ
	if((ret = UnionSetResponseXMLPackageValue("body/certData",certData)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE912:: UnionSetResponseXMLPackageValue[%s]!\n","body/certData");
                return(ret);
        }

	return(0);
}

