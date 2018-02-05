
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
�������:	0203
������:		��������
��������:	��������
***************************************/
int UnionDealServiceCode0203(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	authUserID[60+1];
	char	userID[60+1];
	char	newPassword[128+1];
	char	passwdUpdateTime[14+1];
	char	sql[8192+1];
	
	// ����ԱID
	memset(authUserID,0,sizeof(authUserID));
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",authUserID,sizeof(authUserID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0203:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);
	}
	
	// ��ȡ�޸��û�ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("body/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0203:: UnionReadRequestXMLPackageValue[%s]!\n","body/userID");
		return(ret);
	}
	
	// ����Ƿ�������������
	if (strcmp(authUserID,userID) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0203:: authUserID[%s] == userID[%s]!\n",authUserID,userID);
		return(errCodeOperatorMDL_CannotDeleteSelf);
	}

	// ��ȡ����������
	memset(newPassword,0,sizeof(newPassword));
	if ((ret = UnionReadRequestXMLPackageValue("body/newPassword",newPassword,sizeof(newPassword))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0203:: UnionReadRequestXMLPackageValue[%s]!\n","body/newPassword");
		return(ret);
	}

	memset(passwdUpdateTime,0,sizeof(passwdUpdateTime));
	UnionGetFullSystemDateTime(passwdUpdateTime);
	// ��������
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysUser set userPassword = '%s',passwdUpdateTime = '%s' where userID = '%s'",newPassword,passwdUpdateTime,userID);

	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0203:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0203:: UnionExecRealDBSql[%s]!\n",sql);
		return(errCodeDatabaseMDL_RecordUpdateFailure);
	}

	return(0);
}

