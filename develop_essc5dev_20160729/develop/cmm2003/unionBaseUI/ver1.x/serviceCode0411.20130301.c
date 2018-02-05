
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
服务代码:	0411
服务名:		增加表字段
功能描述:	增加表字段
**********************************/

int UnionDealServiceCode0411(PUnionHsmGroupRec phsmGroupRec)
{
	int	i;
	int	ret;
	int	maxID = 0;
	char	tableName[40+1];
	int	fieldID = 0;
	char	fieldName[40+1];
	char	fieldChnName[40+1];
	char	fieldType[40+1];
	int	fieldSize = 0;
	char	defaultValue[128+1];
	char	isPrimaryKey[2+1];
	char	isNull[2+1];
	char	fieldLimitMethod[2048+1];
	char	fieldControl[2048+1];
	char	fieldValueMethod[2048+1];
	char	varNameOfEnabled[30+1];
	char	varValueOfEnabled[128+1];
	char	remark[128+1];
	char	fileName[1024+1];
	char	tmpBuf[512+1];

	// 读取表名
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(tableName);	
	if (strlen(tableName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: tableName can not be null!\n");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}
	
	// 检查表是否已经存在
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTableDef(tableName,fileName);

	// 文件不存在
	if ((access(fileName,0) < 0))
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: fileName[%s] not found!\n",fileName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// 读取字段ID
	memset(tmpBuf,0,sizeof(tmpBuf));
        if ((ret = UnionReadRequestXMLPackageValue("body/fieldID",tmpBuf,sizeof(tmpBuf))) > 0)
        {
                fieldID = atoi(tmpBuf);
		if (fieldID == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0411:: fieldID[%d] is invalid!\n",fieldID);
			return(errCodeObjectMDL_FieldValueIsInvalid);
		}	
        }
	
	// 读取字段名
	memset(fieldName,0,sizeof(fieldName));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldName",fieldName,sizeof(fieldName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadRequestXMLPackageValue[%s]!\n","body/fieldName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(fieldName);
	if (strlen(fieldName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/fieldName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// 读取字段中文名
	memset(fieldChnName,0,sizeof(fieldChnName));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldChnName",fieldChnName,sizeof(fieldChnName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadRequestXMLPackageValue[%s]!\n","body/fieldChnName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(fieldChnName);
	if (strlen(fieldChnName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/fieldChnName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// 读取字段类型
	memset(fieldType,0,sizeof(fieldType));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldType",fieldType,sizeof(fieldType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadRequestXMLPackageValue[%s]!\n","body/fieldType");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(fieldType);
	if (strlen(fieldType) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/fieldType");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// 读取字段大小
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldSize",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadRequestXMLPackageValue[%s]!\n","body/fieldSize");	
		return(ret);
	}
	UnionFilterHeadAndTailBlank(tmpBuf);
	if (strlen(tmpBuf) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/fieldSize");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}
	else if (atoi(tmpBuf) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: fieldSize[%s] is invalid!\n",tmpBuf);
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}
	else
	{
		fieldSize = atoi(tmpBuf);
	}
	// 读取默认值
	memset(defaultValue,0,sizeof(defaultValue));
	if ((ret = UnionReadRequestXMLPackageValue("body/defaultValue",defaultValue,sizeof(defaultValue))) <= 0)
	{
		strcpy(defaultValue,"");
	}

	// 读取是否关键字
	memset(isPrimaryKey,0,sizeof(isPrimaryKey));
	if ((ret = UnionReadRequestXMLPackageValue("body/isPrimaryKey",isPrimaryKey,sizeof(isPrimaryKey))) <= 0)
	{
		strcpy(isPrimaryKey,"0");
	}

	// 读取是否为空
	memset(isNull,0,sizeof(isNull));
	if ((ret = UnionReadRequestXMLPackageValue("body/isNull",isNull,sizeof(isNull))) <= 0)
	{
		strcpy(isNull,"1");	
	}

	// 字段限制方法
	memset(fieldLimitMethod,0,sizeof(fieldLimitMethod));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldLimitMethod",fieldLimitMethod,sizeof(fieldLimitMethod))) <= 0)
	{
		strcpy(fieldLimitMethod,"");
	}

	// 字段控件定义
	memset(fieldControl,0,sizeof(fieldControl));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldControl",fieldControl,sizeof(fieldControl))) <= 0)
	{
		strcpy(fieldControl,"");
	}

	// 字段限制方法
	memset(fieldValueMethod,0,sizeof(fieldValueMethod));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldValueMethod",fieldValueMethod,sizeof(fieldValueMethod))) <= 0)
	{
		strcpy(fieldValueMethod,"");
	}

	// 启用变量名
	memset(varNameOfEnabled,0,sizeof(varNameOfEnabled));
	if ((ret = UnionReadRequestXMLPackageValue("body/varNameOfEnabled",varNameOfEnabled,sizeof(varNameOfEnabled))) <= 0)
	{
		strcpy(varNameOfEnabled,"");
	}

	// 启用变量名
	memset(varValueOfEnabled,0,sizeof(varValueOfEnabled));
	if ((ret = UnionReadRequestXMLPackageValue("body/varValueOfEnabled",varValueOfEnabled,sizeof(varValueOfEnabled))) <= 0)
	{
		strcpy(varValueOfEnabled,"");
	}

	// 读取备注
	memset(remark,0,sizeof(remark));
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",remark,sizeof(remark))) <= 0)
	{
		strcpy(remark,"");
	}

	// 初始化文件
	if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionInitXMLPackage[%s]!\n",fileName);
		return(ret);
	}

	for (i = 1;;i++)
	{
		if ((ret = UnionLocateXMLPackage("field",i)) < 0)
		{
			maxID = i;
			// 增加新的ID
			if ((ret = UnionLocateNewXMLPackage("field",i)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionLocateNewXMLPackage[%s]!\n","field");
				return(ret);
			}

			// 增加字段名
			if ((ret = UnionSetXMLPackageValue("fieldName",fieldName)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","fieldName");
				return(ret);
			}

			// 增加字段中文名
			if ((ret = UnionSetXMLPackageValue("fieldChnName",fieldChnName)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","fieldChnName");
				return(ret);
			}

			// 增加字段类型
			if ((ret = UnionSetXMLPackageValue("fieldType",fieldType)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","fieldType");
				return(ret);
			}

			// 增加字段大小
			memset(tmpBuf,0,sizeof(tmpBuf));
			sprintf(tmpBuf,"%d",fieldSize);
			if ((ret = UnionSetXMLPackageValue("fieldSize",tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","fieldSize");
				return(ret);
			}

			// 增加默认值
			if ((ret = UnionSetXMLPackageValue("defaultValue",defaultValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","defaultValue");
				return(ret);
			}

			// 增加是否关键字
			if ((ret = UnionSetXMLPackageValue("isPrimaryKey",isPrimaryKey)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","isPrimaryKey");
				return(ret);
			}

			// 增加是否为空
			if ((ret = UnionSetXMLPackageValue("isNull",isNull)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","isNull");
				return(ret);
			}

			// 增加字段限制方法
			if ((ret = UnionSetXMLPackageValue("fieldLimitMethod",fieldLimitMethod)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","fieldLimitMethod");
				return(ret);
			}

			// 增加字段控件定义
			if ((ret = UnionSetXMLPackageValue("fieldControl",fieldControl)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","fieldControl");
				return(ret);
			}

			// 增加字段取值方法
			if ((ret = UnionSetXMLPackageValue("fieldValueMethod",fieldValueMethod)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","fieldValueMethod");
				return(ret);
			}

			// 增加字段取值方法
			if ((ret = UnionSetXMLPackageValue("varNameOfEnabled",varNameOfEnabled)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","varNameOfEnabled");
				return(ret);
			}

			// 增加字段取值方法
			if ((ret = UnionSetXMLPackageValue("varValueOfEnabled",varValueOfEnabled)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","varValueOfEnabled");
				return(ret);
			}

			// 增加备注
			if ((ret = UnionSetXMLPackageValue("remark",remark)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","remark");
				return(ret);
			}
			break;
		}
		else
		{
			// 读出字段名
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((ret = UnionReadXMLPackageValue("fieldName",tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadXMLPackageValue[%s]!\n","body/fieldName");
				return(ret);
			}
			// 对比字段名是否存在 
			if (strcmp(fieldName,tmpBuf) == 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: fieldName[%s] already exist!\n",fieldName);
				return(errCodeObjectMDL_FieldValueIsInvalid);
			}
		}
	}


	// 写回XML文件
	if ((ret = UnionXMLPackageToFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionXMLPackageToFile[%s]!\n",fileName);
		return(ret);
	}

	// 加载到共享内存
	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReloadTableDefTBL!\n");
		return(ret);
	}

	// 初始化文件
	if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionInitXMLPackage[%s]!\n",fileName);
		return(ret);
	}

	// 插入指定位置
	if (fieldID > 0 && fieldID < maxID )
	{
		for (i = maxID; i > fieldID; i--)
		{
			// 交换节点数据
			if ((ret = UnionExchangeIDXMLPackage("field",i-1,i)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionExchangeIDXMLPackage %d failed!\n",i);
				return(ret);
			}
		}
	}

	// 写回XML文件
	if ((ret = UnionXMLPackageToFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionXMLPackageToFile[%s]!\n",fileName);
		return(ret);
	}

	// 加载到共享内存
	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReloadTableDefTBL!\n");
		return(ret);
	}

	return 0;
}
