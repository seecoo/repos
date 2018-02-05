//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "baseUIService.h"

/***************************************
�������:	0102
������:		�û��˳�
��������:	�û��˳�
***************************************/
int UnionDealServiceCode0102(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	userID[40+1];
	char	clientIPAddr[20+1];
	char	sql[512+1];
	
	// ��ȡ�û�ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0102:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);
	}

	// ��ȡ�ͻ���IP
	memset(clientIPAddr,0,sizeof(clientIPAddr));
	if ((ret = UnionReadRequestXMLPackageValue("head/clientIPAddr",clientIPAddr,sizeof(clientIPAddr))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0102:: UnionReadRequestXMLPackageValue[%s]!\n","head/clientIPAddr");
		return(ret);
	}

	// �����û���
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysUser set loginFlag = 0 where userID = '%s'",userID);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0102:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0102:: UnionExecRealDBSql[%s]!\n",sql);
		return(errCodeDatabaseMDL_RecordUpdateFailure);
	}
	
	// ������Ӧ����	
	return(0);
}

