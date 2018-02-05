/*
 * Copyright (c) 2011 Union Tech.
 * All rights reserved.
 *
 * Created on: 2016-01-27
 *   @Author: chenwd
 *   @Version 1.0
 */

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5Interface.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "symmetricKeyDB.h"
#include "asymmetricKeyDB.h"
#include "UnionXOR.h"
#include "unionHsmCmdVersion.h"
#include "hsmDefaultFunction.h"
/*
���ܣ�ָ��PAװ�ظ�ʽ��������HSM
���������
	format	1024A	��ӡ��ʽ

���������
	��
*/
int UnionDealServiceCodeEB02(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret = 0;
	char	format[1024];
	char	sql[128];
	char	ipAddr[32];
	
        // ��ȡ��������ڵĴ�ӡ�����    
        ret = sprintf(sql,"select ipAddr from hsm where status=1 and enabled=1 and uses=1 and hsmGroupID='%s' order by ipAddr",phsmGroupRec->hsmGroupID);
	sql[ret] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB02:: UnionExecRealDBSql[%s]!\n",sql);
		UnionSetResponseRemark("��ȡ�������ʧ��");
		return(ret);
        }
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB02:: UnionExecRealDBSql[%s]!\n",sql);
		UnionSetResponseRemark("�������[%s]û�п��õĴ�ӡ��;�������",phsmGroupRec->hsmGroupID);
		return(errCodeDatabaseMDL_RecordNotFound);
	}

	UnionLocateXMLPackage("detail", 1);
	memset(ipAddr,0,sizeof(ipAddr));
	if ((ret = UnionReadXMLPackageValue("ipAddr", ipAddr, sizeof(ipAddr))) < 0)
	{
			UnionUserErrLog("in UnionDealServiceCodeEB02:: UnionReadXMLPackageValue[%s]!\n","ipAddr");
			return(ret);
	}
        
        // ʹ��ָ���ļ��ܻ�
        UnionSetUseSpecHsmIPAddrForOneCmd(ipAddr);

	// ��ӡ��ʽ
	if ((ret = UnionReadRequestXMLPackageValue("body/format",format,sizeof(format))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB02:: UnionReadRequestXMLPackageValue[%s]!\n","body/SM4Mode");
		return(ret);
	}
	format[ret] = 0;

	if ((ret = UnionHsmCmdPA(format)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB02:: UnionHsmCmdPA[%s]!\n",format);
		return(ret);
	}

	return(0);
}
