
#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "UnionStr.h"
#include "baseUIService.h"

/*********************************
服务代码:	0401
服务名:		增加表
功能描述:	增加表
**********************************/

int UnionDealServiceCode0401(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	tableName[40+1];
	char	tableChnName[40+1];
	char	tableType[128+1];
	char	remark[128+1];
	char	fileName[128+1];
	char	methodOfCached[1+1];

	// 读取表名
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);	
	}
	UnionFilterHeadAndTailBlank(tableName);
	if (strlen(tableName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: tableName can not be null!\n");
		UnionSetResponseRemark("表名不能为空");
		return(errCodeParameter);
	}

	// 检查表是否已经存在
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTableDef(tableName,fileName);
	
	// 文件已经存在
	if (access(fileName,0) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: fileName[%s] already exist!\n",fileName);
		return(errCodeDatabaseMDL_TableAlreadyExist);	
	}
	
	// 读取表中文名
	memset(tableChnName,0,sizeof(tableChnName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableChnName",tableChnName,sizeof(tableChnName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableChnName");
		return(ret);	
	}
	UnionFilterHeadAndTailBlank(tableChnName);
	if (strlen(tableChnName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: tableChnName can not be null!\n");
		UnionSetResponseRemark("表中文名不能为空");
		return(errCodeParameter);
	}

	// 读取表类型
	memset(tableType,0,sizeof(tableType));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableType",tableType,sizeof(tableType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableType");
		return(ret);	
	}
	UnionFilterHeadAndTailBlank(tableType);	
	if (strlen(tableType) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: tableType can not be null!\n");
		UnionSetResponseRemark("表类型不能为空");
		return(errCodeParameter);
	}

	// 读取缓存方法
	memset(methodOfCached,0,sizeof(methodOfCached));
	if ((ret = UnionReadRequestXMLPackageValue("body/methodOfCached",methodOfCached,sizeof(methodOfCached))) < 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode0401:: UnionReadRequestXMLPackageValue[%s]!\n","body/methodOfCached");
		strcpy(methodOfCached,"");
	}
	UnionFilterHeadAndTailBlank(methodOfCached);

	// 读取备注
	memset(remark,0,sizeof(remark));
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",remark,sizeof(remark))) <= 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode0401:: UnionReadRequestXMLPackageValue[%s]!\n","body/remark");
		//return(ret);
		strcpy(remark,"");
	}

	// 初始化XML包
	if ((ret = UnionInitXMLPackage(NULL,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: UnionInitXMLPackage!\n");
		return(ret);	
	}

	// 增加表中文名
	if ((ret = UnionSetXMLPackageValue("tableChnName",tableChnName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: UnionSetXMLPackageValue[%s]!\n","tableChnName");
		return(ret);	
	}
	// 增加表类型 
	if ((ret = UnionSetXMLPackageValue("tableType",tableType)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: UnionSetXMLPackageValue[%s]!\n","tableType");
		return(ret);	
	}
	// 增加缓存方法 
	if (strlen(methodOfCached) > 0)
	{
		if ((ret = UnionSetXMLPackageValue("methodOfCached",methodOfCached)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0401:: UnionSetXMLPackageValue[%s]!\n","methodOfCached");
			return(ret);	
		}
	}
	// 增加备注 
	if ((ret = UnionSetXMLPackageValue("remark",remark)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: UnionSetXMLPackageValue[%s]!\n","remark");
		return(ret);	
	}

	// 写入XML文件
	if ((ret = UnionXMLPackageToFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: UnionXMLPackageToFile[%s]!\n",fileName);
		return(ret);
	}

	// 加载到共享内存
	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: UnionReloadTableDefTBL!\n");
		return(ret);
	}
	return 0;
}
