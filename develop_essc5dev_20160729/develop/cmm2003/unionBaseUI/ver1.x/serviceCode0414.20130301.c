
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
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "baseUIService.h"

/*********************************
服务代码:	0414
服务名:		查询表字段
功能描述:	查询表字段
**********************************/

int UnionDealServiceCode0414(PUnionHsmGroupRec phsmGroupRec)
{
	int	i;
	int	ret;
	int	writeFlag = 0;
	int	totalNum = 0;
	char	tableName[40+1];
	char 	fieldName[40+1];
	char	fieldChnName[40+1];
	char	fieldType[40+1];
	char	defaultValue[128+1];
	char	fieldLimitMethod[2048+1];
	char	fieldControl[2048+1];
	char	fieldValueMethod[2048+1];
	char	varNameOfEnabled[30+1];
	char	varValueOfEnabled[128+1];
	char	remark[128+1];
	char	fileName[128+1];
	char	tmpBuf[128+1];

	 // 读取表名
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0414:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0414:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/tableName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// 检查表是否已经存在
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTableDef(tableName,fileName);

	// 文件不存在
	if (access(fileName,0) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0414:: fileName[%s] not found!\n",fileName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// 初始化文件		
	if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0414:: UnionInitXMLPackage[%s]!\n",fileName);
		return(ret);
	}

	totalNum = 0;
	UnionSetResponseXMLPackageValue("body/totalNum","0");
	
	for (i = 1;; i++)
	{
		if ((ret = UnionLocateXMLPackage("field",i)) < 0)
		{
			UnionLog("in UnionDealService0414:: UnionLocateXMLPackage[%d] not found!\n",i);	
			break;
		}
	
		if ((ret = UnionLocateResponseNewXMLPackage("body/detail", totalNum+1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0414:: UnionLocateResponseNewXMLPackage[%d] not found!\n",i);
			return(ret);
		}

		// 字段名 
		memset(fieldName,0,sizeof(fieldName));
		if ((ret = UnionReadXMLPackageValue("fieldName",fieldName,sizeof(fieldName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0414:: UnionReadXMLPackageValue[%d][%s]!\n",i,"fieldName");
			return(ret);
		}
	
		if ((ret = UnionSetResponseXMLPackageValue("fieldName",fieldName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0414:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"fieldName",fieldName);
			return(ret);
		}

		// 字段中文名
		memset(fieldChnName,0,sizeof(fieldChnName));
		if ((ret = UnionReadXMLPackageValue("fieldChnName",fieldChnName,sizeof(fieldChnName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0414:: UnionReadXMLPackageValue[%d][%s]!\n",i,"fieldChnName");
			return(ret);
		}
	
		if ((ret = UnionSetResponseXMLPackageValue("fieldChnName",fieldChnName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0414:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"fieldChnName",fieldChnName);
			return(ret);
		}
		
		// 字段类型
		memset(fieldType,0,sizeof(fieldType));
		if ((ret = UnionReadXMLPackageValue("fieldType",fieldType,sizeof(fieldType))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0414:: UnionReadXMLPackageValue[%d][%s]!\n",i,"fieldType");
			return(ret);
		}
	
		if ((ret = UnionSetResponseXMLPackageValue("fieldType",fieldType)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0414:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"fieldType",fieldType);
			return(ret);
		}

		// 字段大小
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("fieldSize",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			//UnionUserErrLog("in UnionDealServiceCode0414:: UnionReadXMLPackageValue[%d][%s]!\n",i,"fieldSize");
			//return(ret);
			UnionSetXMLPackageValue("fieldSize","0");
			strcpy(tmpBuf,"0");
			writeFlag = 1;
		}
	
		if ((ret = UnionSetResponseXMLPackageValue("fieldSize",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0414:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"fieldSize",tmpBuf);
			return(ret);
		}

		// 默认值
		memset(defaultValue,0,sizeof(defaultValue));
		if ((ret = UnionReadXMLPackageValue("defaultValue",defaultValue,sizeof(defaultValue))) < 0)
		{
			//UnionUserErrLog("in UnionDealServiceCode0414:: UnionReadXMLPackageValue[%d][%s]!\n",i,"defaultValue");
			//return(ret);
			UnionSetXMLPackageValue("defaultValue","");
			writeFlag = 1;
		}
	
		if ((ret = UnionSetResponseXMLPackageValue("defaultValue",defaultValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0414:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"defaultValue",defaultValue);
			return(ret);
		}

		// 是否关键字
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("isPrimaryKey",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			//UnionUserErrLog("in UnionDealServiceCode0414:: UnionReadXMLPackageValue[%d][%s]!\n",i,"isPrimaryKey");
			//return(ret);
			UnionSetXMLPackageValue("isPrimaryKey","0");
			strcpy(tmpBuf,"0");
			writeFlag = 1;
		}
	
		if ((ret = UnionSetResponseXMLPackageValue("isPrimaryKey",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0414:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"isPrimaryKey",tmpBuf);
			return(ret);
		}

		// 是否为空
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("isNull",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			//UnionUserErrLog("in UnionDealServiceCode0414:: UnionReadXMLPackageValue[%d][%s]!\n",i,"isNull");
			//return(ret);
			UnionSetXMLPackageValue("isNull","1");
			strcpy(tmpBuf,"1");
			writeFlag = 1;
		}
	
		if ((ret = UnionSetResponseXMLPackageValue("isNull",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0414:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"isNull",tmpBuf);
			return(ret);
		}

		// 字段限制方法
		memset(fieldLimitMethod,0,sizeof(fieldLimitMethod));
		if ((ret = UnionReadXMLPackageValue("fieldLimitMethod",fieldLimitMethod,sizeof(fieldLimitMethod))) < 0)
		{
			//UnionUserErrLog("in UnionDealServiceCode0414:: UnionReadXMLPackageValue[%d][%s]!\n",i,"fieldLimitMethod");
			//return(ret);
			UnionSetXMLPackageValue("fieldLimitMethod","");
			writeFlag = 1;
		}
	
		if ((ret = UnionSetResponseXMLPackageValue("fieldLimitMethod",fieldLimitMethod)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0414:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"fieldLimitMethod",fieldLimitMethod);
			return(ret);
		}

		// 字段控件定义
		memset(fieldControl,0,sizeof(fieldControl));
		if ((ret = UnionReadXMLPackageValue("fieldControl",fieldControl,sizeof(fieldControl))) < 0)
		{
			//UnionUserErrLog("in UnionDealServiceCode0414:: UnionReadXMLPackageValue[%d][%s]!\n",i,"fieldControl");
			//return(ret);
			UnionSetXMLPackageValue("fieldControl","");
			writeFlag = 1;
		}
	
		if ((ret = UnionSetResponseXMLPackageValue("fieldControl",fieldControl)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0414:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"fieldControl",fieldControl);
			return(ret);
		}
	
		// 字段取值方法	
		memset(fieldValueMethod,0,sizeof(fieldValueMethod));
		if ((ret = UnionReadXMLPackageValue("fieldValueMethod",fieldValueMethod,sizeof(fieldValueMethod))) < 0)
		{
			//UnionUserErrLog("in UnionDealServiceCode0414:: UnionReadXMLPackageValue[%d][%s]!\n",i,"fieldValueMethod");
			//return(ret);
			UnionSetXMLPackageValue("fieldValueMethod","");
			writeFlag = 1;
		}
	
		if ((ret = UnionSetResponseXMLPackageValue("fieldValueMethod",fieldValueMethod)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0414:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"fieldValueMethod",fieldValueMethod);
			return(ret);
		}

		// 启用变量名	
		memset(varNameOfEnabled,0,sizeof(varNameOfEnabled));
		if ((ret = UnionReadXMLPackageValue("varNameOfEnabled",varNameOfEnabled,sizeof(varNameOfEnabled))) < 0)
		{
			UnionSetXMLPackageValue("varNameOfEnabled","");
			writeFlag = 1;
		}
	
		if ((ret = UnionSetResponseXMLPackageValue("varNameOfEnabled",varNameOfEnabled)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0414:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"varNameOfEnabled",varNameOfEnabled);
			return(ret);
		}
	
		// 启用变量值	
		memset(varValueOfEnabled,0,sizeof(varValueOfEnabled));
		if ((ret = UnionReadXMLPackageValue("varValueOfEnabled",varValueOfEnabled,sizeof(varValueOfEnabled))) < 0)
		{
			UnionSetXMLPackageValue("varValueOfEnabled","");
			writeFlag = 1;
		}
	
		if ((ret = UnionSetResponseXMLPackageValue("varValueOfEnabled",varValueOfEnabled)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0414:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"varValueOfEnabled",varValueOfEnabled);
			return(ret);
		}
	
		// 备注
		memset(remark,0,sizeof(remark));
		if ((ret = UnionReadXMLPackageValue("remark",remark,sizeof(remark))) < 0)
		{
			UnionSetXMLPackageValue("remark","");
			writeFlag = 1;
		}
	
		if ((ret = UnionSetResponseXMLPackageValue("remark",remark)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0414:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"remark",remark);
			return(ret);
		}

		totalNum ++;
	}

	if (writeFlag)
		UnionXMLPackageToFile(fileName);
	
	UnionLocateResponseXMLPackage("",0);
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",totalNum);
	UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf);

	return 0;
}
