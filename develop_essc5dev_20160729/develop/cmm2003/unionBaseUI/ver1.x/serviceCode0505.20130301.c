
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
服务代码:	0505
服务名:		清除冗余菜单数据
功能描述:	清除冗余菜单数据
**********************************/

int UnionDealServiceCode0505(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	sql[2048+1];
	char	cleanType[1+1];

	// 读取清除类别
	memset(cleanType,0,sizeof(cleanType));
	if ((ret = UnionReadRequestXMLPackageValue("body/cleanType",cleanType,sizeof(cleanType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0505:: UnionReadRequestXMLPackageValue[%s]!\n","body/order_by");
		return(ret);
	}

	if (cleanType[0] == '1')	// 菜单（包括菜单表、视图表、按钮表）
	{
		// 删除不存在父节点的菜单
		memset(sql,0,sizeof(sql));
		sprintf(sql,"delete from sysMenu where menuParentName not in (select menuName from sysMenu)");
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0505:: UnionExecRealDBSql[%s]!\n",sql);
			return(ret);
		}
		
		// 删除不存在菜单的视图
		memset(sql,0,sizeof(sql));
		sprintf(sql,"delete from sysView where viewName not in (select menuName from sysMenu)");
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0505:: UnionExecRealDBSql[%s]!\n",sql);
			return(ret);
		}
	
		// 删除不存在视图的按钮
		memset(sql,0,sizeof(sql));
		sprintf(sql,"delete from sysButton where viewName not in (select viewName from sysView)");
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0505:: UnionExecRealDBSql[%s]!\n",sql);
			return(ret);
		}
	}
	else if (cleanType[0] == '2')	// 权限管理
	{
		// 删除冗余授权
		memset(sql,0,sizeof(sql));
		sprintf(sql,"delete from privilege where "
			"(privilegeMaster = 'ROLE' and privilegeMasterValue not in (select roleID from sysRole)) OR "
			"(privilegeMaster = 'USER' and privilegeMasterValue not in (select userID from sysUser)) OR "
			"(privilegeAccess = 'MENU' and privilegeAccessValue not in (select menuName from sysMenu)) OR "
			"(privilegeAccess = 'BUTTON' and privilegeAccessValue not in (select btnName from sysButton))");
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0505:: UnionExecRealDBSql[%s]!\n",sql);
			return(ret);
		}
	}
	
	return 0;
}

