
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
#include "unionVersion.h"
#include "baseUIService.h"

/*********************************
服务代码:	0506
服务名:		修改菜单
功能描述:	修改菜单
**********************************/

int UnionDealServiceCode0506(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	int	i = 0;
	int	newMenuName = 0;
	int	menuName = 0;
	char	tmpBuf[128+1];
	int	idValue[2] = {99999998,99999999};
	int	errFlag = 0;
	int	retErrNo = 0;
	char	sql[1024+1];

	// 读取新菜单名
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/newMenuName",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionReadRequestXMLPackageValue[body/newMenuName]!\n");
		return(ret);
	}
	else
		newMenuName = atoi(tmpBuf);
	
	// 读取菜单名
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/menuName",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionReadRequestXMLPackageValue[body/menuName]!\n");
		return(ret);
	}
	else
		menuName = atoi(tmpBuf);

	if (newMenuName == menuName)
		return 0;

	// 检查序号是否已经存在
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select menuName from sysMenu where menuName = %d",newMenuName);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret > 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: menuName[%d] is already exist!\n",newMenuName);
		UnionSetResponseRemark("新菜单名已经存在");
		return(errCodeParameter);
	}

	// 增加一条菜单
	for (i = 0; i < 2; i++)
	{
		memset(sql,0,sizeof(sql));
		sprintf(sql,"insert into sysMenu(menuName,menuType,menuLevel,menuDisplayName,isVisible,seqNo,isLeaf) values(%d,1,1,'test',1,0,0)",idValue[i]);
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
			return(ret);
		}
	}

	// 增加一条视图
	memset(sql,0,sizeof(sql));		
	sprintf(sql,"insert into sysView(viewName) values(%d)",idValue[0]);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	// 修改按钮
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysButton set viewName = %d where viewName = %d",idValue[0],menuName);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		errFlag = 1;
		UnionSetResponseRemark("更新按钮失败");
		retErrNo = ret;
		goto delete;
	}

	// 修改视图
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysView set viewName = %d where viewName = %d",idValue[1],menuName);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		errFlag = 1;
		UnionSetResponseRemark("更新视图失败");
		retErrNo = ret;
		goto  delete;
	}

	// 更新菜单
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysMenu set menuName = %d where menuName = %d",newMenuName,menuName);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		errFlag = 1;
		UnionSetResponseRemark("更新菜单失败");
		retErrNo = ret;
		goto  delete;
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		errFlag = 1;
		UnionSetResponseRemark("更新菜单失败");
		retErrNo = ret;
		goto  delete;
	}

	// 更新子菜单
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysMenu set menuParentName = %d where menuParentName = %d",newMenuName,menuName);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		errFlag = 1;
		UnionSetResponseRemark("更新子菜单失败");
		retErrNo = ret;
		goto  delete;
	}

	// 修改视图
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysView set viewName = %d where viewName = %d",newMenuName,idValue[1]);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		errFlag = 1;
		UnionSetResponseRemark("更新视图失败");
		retErrNo = ret;
		goto  delete;
	}

	// 修改操作视图
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysButton set operViewName = %d where operViewName = %d",newMenuName,menuName);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		errFlag = 1;
		UnionSetResponseRemark("更新操作视图失败");
		retErrNo = ret;
		goto  delete;
	}

	// 修改按钮
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysButton set viewName = %d where viewName = %d",newMenuName,idValue[0]);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		errFlag = 1;
		UnionSetResponseRemark("更新按钮失败");
		retErrNo = ret;
		goto  delete;
	}
	
	// 更新授权
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update privilege set privilegeAccessValue = %d where privilegeAccess = 'MENU' and privilegeAccessValue = %d",newMenuName,menuName); 
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		errFlag = 1;
		UnionSetResponseRemark("更新授权失败");
		retErrNo = ret;
		goto  delete;
	}
delete:
	// 删除增加的菜单
	for (i = 0; i < 2; i++)
	{
		memset(sql,0,sizeof(sql));
		sprintf(sql,"delete from sysMenu where menuName = %d",idValue[i]); 
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
			return(ret);
		}
	}

	// 删除增加的视图
	memset(sql,0,sizeof(sql));
	sprintf(sql,"delete from sysView where viewName = %d",idValue[0]); 
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	if (errFlag == 1)
		return(retErrNo);
	
	return 0;
}
