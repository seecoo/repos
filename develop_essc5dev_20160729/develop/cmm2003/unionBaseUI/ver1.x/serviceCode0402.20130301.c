
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
服务代码:	0402
服务名:		修改表	
功能描述:	修改表	
**********************************/

int UnionDealServiceCode0402(PUnionHsmGroupRec phsmGroupRec)
{
	int	i;
	int	ret;
	int	flag = 0;
	int	isUpdateCascade = 0;
	int	isDeleteCascade = 0;
	char	tableName[40+1];
	char	newTableName[40+1];
	char	tableChnName[40+1];
	char	tableType[128+1];
	char	methodOfCached[1+1];
	char	remark[128+1];
	char	fileName[128+1];
	char	newFileName[128+1];
	char	tmpBuf[512+1];

	// 读取表名
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);	
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/tableName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}
	

	// 检查表是否已经存在
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTableDef(tableName,fileName);

	// 文件不存在
	if (access(fileName,0) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: fileName[%s] not found!\n",fileName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// 读取新表名
	memset(newTableName,0,sizeof(newTableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/newTableName",newTableName,sizeof(newTableName))) > 0)
	{
		memset(newFileName,0,sizeof(newFileName));
		UnionGetFileNameOfTableDef(newTableName,newFileName);
		
		if (strcmp(tableName,newTableName) != 0)
		{	
			if (access(newFileName,0) == 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0402:: newFileName[%s] already exist!\n",newFileName);
				return(errCodeDatabaseMDL_TableAlreadyExist);
			}	
			if ((ret = rename(fileName,newFileName)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0402:: rename [%s] to [%s] is failed!\n",fileName,newFileName);
				return(ret);
			}
			flag = 1;
		}
	}

	// 读取表中文名
	memset(tableChnName,0,sizeof(tableChnName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableChnName",tableChnName,sizeof(tableChnName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableChnName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/tableChnName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}


	// 读取表类型
	memset(tableType,0,sizeof(tableType));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableType",tableType,sizeof(tableType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableType");
		return(ret);	
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/tableType");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// 读取高速缓存时间
	memset(methodOfCached,0,sizeof(methodOfCached));
	if ((ret = UnionReadRequestXMLPackageValue("body/methodOfCached",methodOfCached,sizeof(methodOfCached))) < 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%s]!\n","body/methodOfCached");
		strcpy(methodOfCached,"0");	
	}

	// 读取备注
	memset(remark,0,sizeof(remark));
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",remark,sizeof(remark))) < 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%s]!\n","body/remark");
		//return(ret);
		strcpy(remark,"");
	}

	// 初始化文件	
	if (flag == 1)
	{
		memset(fileName,0,sizeof(fileName));
		strcpy(fileName,newFileName);
	}
	if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionInitXMLPackage[%s]!\n",fileName);
		return(ret);
	}

	// 修改表中文名
	if ((ret = UnionSetXMLPackageValue("tableChnName",tableChnName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","tableChnName");
		return(ret);
	}

	// 修改表类型
	if ((ret = UnionSetXMLPackageValue("tableType",tableType)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","tableType");
		return(ret);
	}

	// 修改缓存方法
	if ((ret = UnionSetXMLPackageValue("methodOfCached",methodOfCached)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","methodOfCached");
		return(ret);
	}

	// 修改备注
	if ((ret = UnionSetXMLPackageValue("remark",remark)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","remark");
		return(ret);
	}

	// 读取唯一值字段
	for (i = 0;;i++)
	{
		// 读取的唯一值字段大于预计的
		if (i >= 16)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: uniqueNum [%d]  > 16!\n",i+1);
			return(errCodeObjectMDL_FieldNumberTooMuch);
		}

		if ((ret = UnionLocateRequestXMLPackage("body/unique", i+1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionLocateRequestXMLPackage unique[%d] not found!\n",i+1);
			break;
		}

		UnionLocateNewXMLPackage("unique",i+1);

		// 读取唯一值名称
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("name",tmpBuf,sizeof(tmpBuf))) <= 0)
		{
			strcpy(tmpBuf,"");
		}
		// 修改唯一值名称
		if ((ret = UnionSetXMLPackageValue("name",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","unique/name");
			return(ret);	
		}
		
		// 读取fields
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("fields",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402::  UnionReadRequestXMLPackageValue[%d][%s]\n",i+1,"unique/fields");
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%d] [%s] can not be null!\n",i+1,"unique/fields");
			return(errCodeObjectMDL_FieldValueIsInvalid);
		}
		
		// 修改唯一值
		if ((ret = UnionSetXMLPackageValue("fields",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","fields");
			return(ret);	
		}
	}

	// 读取索引
	for (i = 0;;i++)
	{
		// 读取的索引数大于预计的
		if (i >= 16)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: indexNum [%d]  > 16!\n",i+1);
			return(errCodeObjectMDL_FieldNumberTooMuch);
		}

		if ((ret = UnionLocateRequestXMLPackage("body/index", i+1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionLocateRequestXMLPackage index[%d] not found!\n",i+1);
			break;
		}

		UnionLocateNewXMLPackage("index",i+1);

		// 读取索引名称
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("name",tmpBuf,sizeof(tmpBuf))) <= 0)
		{
			strcpy(tmpBuf,"");
		}

		// 修改索引名称
		if ((ret = UnionSetXMLPackageValue("name",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","index/name");
			return(ret);	
		}

		// 读取fields
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("fields",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402::  UnionReadRequestXMLPackageValue[%d][%s] index\n",i+1,"fields");
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%d] [%s] can not be null!\n",i+1,"index/fields");
			return(errCodeObjectMDL_FieldValueIsInvalid);
		}
		// 修改索引
		if ((ret = UnionSetXMLPackageValue("fields",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","fields");
			return(ret);	
		}
	}
	
	// 读取外键
	for (i = 0;;i++)
	{
		// 设定的外键数大于预计的
		if (i >= 16)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: foreignkeyNum [%d]  > 16!\n",i+1);
			return(errCodeObjectMDL_FieldNumberTooMuch);
		}
		
		if ((ret = UnionLocateRequestXMLPackage("body/foreignkey", i+1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionLocateRequestXMLPackage foreign[%d] not found!\n",i+1);
			break;
		}

		UnionLocateNewXMLPackage("foreignkey",i+1);

		// 读取外键名称
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("name",tmpBuf,sizeof(tmpBuf))) <= 0)
		{
			strcpy(tmpBuf,"");
		}
		// 修改外键名称
		if ((ret = UnionSetXMLPackageValue("name",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","foreignkey/name");
			return(ret);	
		}
		// 读取外键中的参照表名
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("references",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402::  UnionReadRequestXMLPackageValue[%d][%s]\n",i+1,"references");
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%d] [%s] can not be null!\n",i+1,"references");
			return(errCodeObjectMDL_FieldValueIsInvalid);
		}

		// 修改外键中的参照表名
		if ((ret = UnionSetXMLPackageValue("references",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","references");
			return(ret);	
		}

		// 读取外键中本地表的字段名
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("localfields",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%d][%s]\n",i+1,"localfields");
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%d] [%s] can not be null!\n",i+1,"localfields");
			return(errCodeObjectMDL_FieldValueIsInvalid);
		}

		// 修改外键中的参照表名
		if ((ret = UnionSetXMLPackageValue("localfields",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","localfields");
			return(ret);	
		}

		// 读取外键中参照表的字段名
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("foreignfields",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%d][%s]\n",i+1,"foreignfields");
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%d] [%s] can not be null!\n",i+1,"foreignfields");
			return(errCodeObjectMDL_FieldValueIsInvalid);
		}

		// 修改外键中的参照表名
		if ((ret = UnionSetXMLPackageValue("foreignfields",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","foreignfields");
			return(ret);	
		}

		// 读取外键中的级联更新
		// 读取外键级联更新
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("isUpdateCascade",tmpBuf,sizeof(tmpBuf))) <= 0)
		{
			strcpy(tmpBuf,"");
		}
		else if (atoi(tmpBuf) == 1 || atoi(tmpBuf) == 0)
			isUpdateCascade = atoi(tmpBuf);
		else
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: isUpdateCascade[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}

		// 读取外键级联删除
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("isDeleteCascade",tmpBuf,sizeof(tmpBuf))) <= 0)
		{
			strcpy(tmpBuf,"");
		}
		else if (atoi(tmpBuf) == 1 || atoi(tmpBuf) == 0)
			isDeleteCascade = atoi(tmpBuf);
		else
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: isDeleteCascade[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}

		// 修改外键中的级联更新
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",isUpdateCascade);
		if ((ret = UnionSetXMLPackageValue("isUpdateCascade",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","isUpdateCascade");
			return(ret);	
		}

		// 修改外键中的级联删除
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",isDeleteCascade);
		if ((ret = UnionSetXMLPackageValue("isDeleteCascade",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","isDeleteCascade");
			return(ret);	
		}

	}

	// 写回XML文件
	if ((ret = UnionXMLPackageToFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionXMLPackageToFile[%s]!\n",fileName);
		return(ret);
	}

	// 加载到共享内存
	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionReloadTableDefTBL!\n");
		return(ret);
	}
	return 0;
}
