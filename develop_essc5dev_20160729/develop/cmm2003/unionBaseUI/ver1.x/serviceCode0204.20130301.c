
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
�������:	0204
������:		ɾ���û�
��������:	ɾ���û�
***************************************/
int UnionDealServiceCode0204(PUnionHsmGroupRec phsmGroupRec)
{
	int	i;
	int	ret;
	char	authUserID[60+1];
	char	userID[60+1];
	char	sql[2][512+1];

	// ����ԱID 
	memset(authUserID,0,sizeof(authUserID));
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",authUserID,sizeof(authUserID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0204:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);
	}

	// ��ȡ�޸��û�ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("body/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0204:: UnionReadRequestXMLPackageValue[%s]!\n","body/userID");
		return(ret);
	}

	// userID����Ϊ�Լ���admin
	if (strcmp(authUserID,userID) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0204:: �û�[%s]������ɾ��!\n",userID);
		return(errCodeOperatorMDL_CannotDeleteSelf);
	}
	if (strcmp("admin",userID) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0204:: �û�[%s]������ɾ��!\n",userID);
		return(0);
	}
	// ɾ�����ݿ��е�ָ���û�
	memset(sql[0],0,sizeof(sql[0]));
	sprintf(sql[0],"delete from  sysUser where userID = '%s'",userID);

	// ɾ����Ȩ�����û���Ϣ
	memset(sql[1],0,sizeof(sql[1]));
	sprintf(sql[1],"delete from  privilege where privilegeMaster = 'USER' and  privilegeMasterValue = '%s'",userID);

	for (i = 0; i < 2; i++)
	{
		if ((ret = UnionExecRealDBSql(sql[i])) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0204:: UnionExecRealDBSql[%s]!\n",sql[i]);
			return(ret);
		}
	}

	return(0);
}

