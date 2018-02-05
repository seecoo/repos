
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
#include "baseUIService.h"

/*********************************
服务代码:	0403
服务名:		删除表
功能描述:	删除表
**********************************/

int UnionDealServiceCode0403(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	tableName[40+1];
	char	fileName[1024+1];

	// 读取表名
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0403:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0403:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/tableName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
		
	}

	// 检查表是否已经存在
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTableDef(tableName,fileName);

	// 文件已经存在
	if ((access(fileName,0) == 0))
	{
		// 删除表
		if ((ret = unlink(fileName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0403:: delete fileName[%s] is failed!\n",fileName);
			return(ret);
		}
	}
	else
	{
		UnionUserErrLog("in UnionDealServiceCode0403:: fileName[%s] not found!\n",fileName);
		return(errCodeDatabaseMDL_TableNotFound);
	}
	// 加载到共享内存
	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0403:: UnionReloadTableDefTBL!\n");
		return(ret);
	}

	return 0;
}
