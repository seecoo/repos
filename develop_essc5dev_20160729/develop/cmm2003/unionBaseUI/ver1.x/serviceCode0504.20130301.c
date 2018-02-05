
#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/dir.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "baseUIService.h"

/*********************************
服务代码:	0504
服务名:		权限管理
功能描述:	权限管理
**********************************/

#define MAX_PRIVILEGE_NUM 512

int UnionDealServiceCode0504(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret, i;
	int			len;
	char			privilegeMaster[64];
	char			privilegeMasterValue[128];
	char			privilegeAccess[64];
	char			privilegeAccessValueList[4096+64];
	int			paNum;
	char			paValueGrp[MAX_PRIVILEGE_NUM][128];
	char			sql[2048];
	char			type[32];

	// 授权者类型
	memset(privilegeMaster, 0, sizeof(privilegeMaster));
	if ((ret = UnionReadRequestXMLPackageValue("body/privilegeMaster", privilegeMaster, sizeof(privilegeMaster))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0504:: UnionReadRequestXMLPackageValue[privilegeMaster] ret = [%d]!\n", ret);
		return(ret);
	}

	// 授权者
	memset(privilegeMasterValue, 0, sizeof(privilegeMasterValue));
	if ((ret = UnionReadRequestXMLPackageValue("body/privilegeMasterValue", privilegeMasterValue, sizeof(privilegeMasterValue))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0504:: UnionReadRequestXMLPackageValue[privilegeMasterValue] ret = [%d]!\n", ret);
		return(ret);
	}

	// 授权对象类型
	memset(privilegeAccess, 0, sizeof(privilegeAccess));
	if ((ret = UnionReadRequestXMLPackageValue("body/privilegeAccess", privilegeAccess, sizeof(privilegeAccess))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0504:: UnionReadRequestXMLPackageValue[privilegeAccess] ret = [%d]!\n", ret);
		return(ret);
	}

	// 授权对象
	memset(privilegeAccessValueList, 0, sizeof(privilegeAccessValueList));
	if ((ret = UnionReadRequestXMLPackageValue("body/privilegeAccessValueList", privilegeAccessValueList, sizeof(privilegeAccessValueList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0504:: UnionReadRequestXMLPackageValue[privilegeAccessValueList] ret = [%d]!\n", ret);
		return(ret);
	}

	// 类型
	memset(type, 0, sizeof(type));
	if ((ret = UnionReadRequestXMLPackageValue("body/type", type, sizeof(type))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0504:: UnionReadRequestXMLPackageValue[type] ret = [%d]!\n", ret);
		return(ret);
	}

	memset(paValueGrp, 0, sizeof(paValueGrp));
	if ((paNum = UnionSeprateVarStrIntoVarGrp(privilegeAccessValueList, strlen(privilegeAccessValueList), ',', paValueGrp, MAX_PRIVILEGE_NUM)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0504:: UnionSeprateVarStrIntoVarGrp[%s] ret = [%d]!\n", privilegeAccessValueList, ret);
		return(paNum);
	}

	// 清除原授权记录
	memset(sql, 0, sizeof(sql));
	len = sprintf(sql,"delete from privilege where privilegeMaster = '%s' and privilegeMasterValue = '%s' and privilegeAccess = '%s'", privilegeMaster, privilegeMasterValue, privilegeAccess);
	if (type[0] == '1')
		len += sprintf(sql +len, " and privilegeAccessValue >= 100");
	else
		len += sprintf(sql +len, " and privilegeAccessValue < 100");
		
	UnionAuditLog("in UnionDealServiceCode0504:: UnionExecRealDBSql[%s]!\n",sql);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0504:: UnionExecRealDBSql[%s] ret = %d!\n",sql, ret);
		return(ret);
	}

	// 增加新授权记录回授权表
	for(i = 0; i < paNum; i++)
	{
		memset(sql, 0, sizeof(sql));
		sprintf(sql,"insert into privilege(privilegeMaster, privilegeMasterValue, privilegeAccess, privilegeAccessValue, privilegeOperation) values('%s', '%s', '%s', %s, %d)",  privilegeMaster, privilegeMasterValue, privilegeAccess, paValueGrp[i], 1);
		UnionAuditLog("in UnionDealServiceCode0504:: UnionSelectRealDBRecord[%s]!\n",sql);
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0504:: UnionExecRealDBSql[%s] ret = [%d]!\n",sql, ret);
			return(ret);
		}
		
	}

	UnionLocateResponseXMLPackage("", 0);

	return 0;
}

