
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
服务代码:	0413
服务名:		删除表字段
功能描述:	删除表字段
**********************************/

int UnionDealServiceCode0413(PUnionHsmGroupRec phsmGroupRec)
{
	int	i;
	int	position = 0;
	int	maxID = 0;
	int	ret;
	char	tableName[40+1];
	char	fieldName[40+1];
	char	fileName[1024+1];
	char	tmpBuf[128+1];

	// 读取表名
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0413:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0413:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/tableName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// 检查表是否已经存在
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTableDef(tableName,fileName);

	// 文件不存在
	if (access(fileName,0) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0413:: fileName[%s] not found!\n",fileName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// 读取字段名
	memset(fieldName,0,sizeof(fieldName));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldName",fieldName,sizeof(fieldName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0413:: UnionReadRequestXMLPackageValue[%s]!\n","body/fieldName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0413:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/fieldName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// 初始化文件	
	if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0413:: UnionInitXMLPackage[%s]!\n",fileName);
		return(ret);
	}

	// 查找要删除的表字段ID 
	for (i = 1;;i++)
	{
		if ((ret = UnionLocateXMLPackage("field",i)) < 0)
		{
			// 要删除的文件不存在
			if (position == 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0413:: fieldName[%s] not found!\n",fieldName);
				return(errCodeObjectMDL_FieldNotExist);
			}
			// 记录最后一个ID值
			maxID = i - 1;
			break;
		}

		// 读出字段名
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("fieldName",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0413:: UnionReadXMLPackageValue[%s]!\n","body/fieldName");
			return(ret);
		}

		// 对比字段名是否存在
		if (strcmp(fieldName,tmpBuf) == 0)
		{
			// 记录要删除值的ID值
			position = i;
		}
		continue;
	}

	UnionLocateXMLPackage("",0);
	
	// 更新ID
	for (i = maxID - 1; i >= position; i--)
	{
		if((ret = UnionExchangeIDXMLPackage("field",i,maxID)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0413:: UnionExchangeIDXMLPackage %d failed!\n",i);
			return(ret);
		}
		
	}

	// 删除指定的表字段
	if ((ret = UnionDeleteXMLPackageNode("field",maxID)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0413:: UnionDeleteXMLPackageNode node[field][%d]\n",maxID);
		return(ret);
	}

	// 写回XML文件
	if ((ret = UnionXMLPackageToFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0413:: UnionXMLPackageToFile[%s]!\n",fileName);
		return(ret);
	}

	// 加载到共享内存
	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0413:: UnionReloadTableDefTBL!\n");
		return(ret);
	}
	return 0;
}
