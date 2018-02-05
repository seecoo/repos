
#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <math.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "unionVersion.h"
#include "baseUIService.h"

/*********************************
服务代码:	0507
服务名:		删除菜单
功能描述:	删除菜单
**********************************/

int UnionDealServiceCode0507(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	menuName[32];
	char	sql[8192];
	char	menuList[4096];
	
	// 表名
	if ((ret = UnionReadRequestXMLPackageValue("body/menuName",menuName,sizeof(menuName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0507:: UnionReadRequestXMLPackageValue[%s]!\n","body/menuName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0507:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/menuName");
		return(errCodeParameter);
	}

	if ((ret = UnionSelectRealDBTree(0,"sysMenu","menuName","menuParentName",2,menuName,menuList,sizeof(menuList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0507:: UnionSelectRealDBTree!\n");
		return(ret);
	}
	// 删除对应的按钮
	snprintf(sql,sizeof(sql),"delete from sysButton where viewName in (%s)",menuList);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0507:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	// 删除对应的视图
	snprintf(sql,sizeof(sql),"delete from sysView where viewName in (%s)",menuList);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0507:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	// 删除菜单
	snprintf(sql,sizeof(sql),"delete from sysMenu where menuName in (%s)",menuList);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0507:: UnionExecRealDBSql[%s] error!\n",sql);
		return(ret);
	}

	// 删除授权中的记录
	snprintf(sql,sizeof(sql),"delete from privilege where (privilegeAccess = 'MENU' or privilegeAccess = 'BUTTON') and privilegeAccessValue in (%s)",menuList); 
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0507:: UnionExecRealDBSql[%s] error!\n",sql);
		return(ret);
	}

	return(0);
}
