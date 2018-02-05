
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
服务代码:	0503
服务名:		下载视图
功能描述:	下载视图
**********************************/

int UnionDealServiceCode0503(PUnionHsmGroupRec phsmGroupRec)
{
	int	i,j,k;
	int	ret;
	//int	isExistField;
	int	maxTableNum = 8;
	int	maxFieldNum = 64;
	int	fieldNum = 0;
	char	tableName[128];
	char	tableField[2048];
	char	viewName[128];
	char	listDisplayField[2048];
	char	editDisplayField[2048];
	char	viewCondition[2048];
	char	joinCondition[2048];
	char	tmpBuf[128];
	char	sql[1024];
	char	tableNameGrp[maxTableNum][128];
	char	fieldGrp[maxFieldNum][128];
	char	*ptr = NULL;
	int	useFlag = 2;
	int	writeFlag = 0;
	char	fileName[128];

	int	tableNum;
	char	fieldName[64];
	char	fieldChnName[64];
	char	fieldType[32];
	char	fieldLimitMethod[2048];
	char	fieldControl[2048];
	char	fieldValueMethod[2048];
	char	remark[128];	

	// 读取视图名
	memset(viewName,0,sizeof(viewName));
	if ((ret = UnionReadRequestXMLPackageValue("body/viewName",viewName,sizeof(viewName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionReadRequestXMLPackageValue[viewName]!\n");
		return(ret);
	}

	// 读取用途
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/useFlag",tmpBuf,sizeof(tmpBuf))) < 0)
		useFlag = 2;
	else
		useFlag = atoi(tmpBuf);

	// 读取数据库中的记录
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select * from sysView where viewName = %s",viewName);

	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: viewName[%s] not find!\n",viewName);
		return(errCodeDatabaseMDL_RecordNotFound);
	}

	if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
	{
		UnionUserErrLog("in UnionDealService0503:: UnionLocateXMLPackage[%d]\n",1);
		return(ret);
	}

	// 读取表名
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadXMLPackageValue("tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionReadXMLPackageValue[tableName]!\n");
		return(ret);
	}

	// 拼分表名定义串
	memset(tableNameGrp,0,sizeof(tableNameGrp));
	if ((tableNum = UnionSeprateVarStrIntoVarGrp(tableName,strlen(tableName),',',tableNameGrp,maxTableNum)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionSeprateVarStrIntoVarGrp[%s]!\n",tableName);
		return(tableNum);
	}
	
	// 读取表字段
	memset(tableField,0,sizeof(tableField));
	if ((ret = UnionReadXMLPackageValue("tableField",tableField,sizeof(tableField))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionReadXMLPackageValue[tableField]!\n");
		return(ret);
	}

	// 读取List显示字段
	memset(listDisplayField,0,sizeof(listDisplayField));
	if ((ret = UnionReadXMLPackageValue("listDisplayField",listDisplayField,sizeof(listDisplayField))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionReadXMLPackageValue[listDisplayField]!\n");
		return(ret);
	}

	// 读取编辑显示字段
	memset(editDisplayField,0,sizeof(editDisplayField));
	if ((ret = UnionReadXMLPackageValue("editDisplayField",editDisplayField,sizeof(editDisplayField))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionReadXMLPackageValue[editDisplayField]!\n");
		return(ret);
	}

	// 读取视图条件
	memset(viewCondition,0,sizeof(viewCondition));
	if ((ret = UnionReadXMLPackageValue("viewCondition",viewCondition,sizeof(viewCondition))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionReadXMLPackageValue[viewCondition]!\n");
		return(ret);
	}
	
	// 读取多表JOIN条件
	memset(joinCondition,0,sizeof(joinCondition));
	if ((ret = UnionReadXMLPackageValue("joinCondition",joinCondition,sizeof(joinCondition))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionReadXMLPackageValue[joinCondition]!\n");
		return(ret);
	}

	// 读取备注
	memset(remark,0,sizeof(remark));
	if ((ret = UnionReadXMLPackageValue("remark",remark,sizeof(remark))) < 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode0503:: UnionReadXMLPackageValue[remark]!\n");
		//return(ret);
		strcpy(remark,"");
	}

	if ((ret = UnionTransformTableFileList(useFlag,tableName,tableField)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionTransformTableFileList[%d][%s][%s]!\n",useFlag,tableName,tableField);
		return(ret);
	}
	tableField[ret] = 0;

	memset(fieldGrp,0,sizeof(fieldGrp));
	if ((fieldNum = UnionSeprateVarStrIntoVarGrp(tableField,strlen(tableField),',',fieldGrp,maxFieldNum)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionSeprateVarStrIntoVarGrp[%s]!\n",tableField);
		return(fieldNum);
	}

	if ((ret = UnionTransformTableFileList(useFlag,tableName,listDisplayField)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionTransformTableFileList[%d][%s][%s]!\n",useFlag,tableName,listDisplayField);
		return(ret);
	}
	listDisplayField[ret] = 0;

	if ((ret = UnionTransformTableFileList(useFlag,tableName,editDisplayField)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionTransformTableFileList[%d][%s][%s]!\n",useFlag,tableName,editDisplayField);
		return(ret);
	}
	editDisplayField[ret] = 0;

	/*
	if ((ret = UnionLocateResponseNewXMLPackage("body/detail", i)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionLocateResponseNewXMLPackage!\n");
		return(ret);
	}
	*/

	if ((ret = UnionSetResponseXMLPackageValue("body/viewName",viewName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionSetResponseXMLPackageValue[%s][%s]!\n","body/viewName",viewName);
		return(ret);
	}
	
	if ((ret = UnionSetResponseXMLPackageValue("body/tableName",tableName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionSetResponseXMLPackageValue[%s][%s]!\n","body/tableName",tableName);
		return(ret);
	}

	if ((ret = UnionSetResponseXMLPackageValue("body/tableField",tableField)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionSetResponseXMLPackageValue[%s][%s]!\n","body/tableField",tableField);
		return(ret);
	}
	
	if ((ret = UnionSetResponseXMLPackageValue("body/listDisplayField",listDisplayField)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionSetResponseXMLPackageValue[%s][%s]!\n","body/listDisplayField",listDisplayField);
		return(ret);
	}

	if ((ret = UnionSetResponseXMLPackageValue("body/editDisplayField",editDisplayField)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionSetResponseXMLPackageValue[%s][%s]!\n","body/editDisplayField",editDisplayField);
		return(ret);
	}

	if ((ret = UnionSetResponseXMLPackageValue("body/viewCondition",viewCondition)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionSetResponseXMLPackageValue[%s][%s]!\n","body/viewCondition",viewCondition);
		return(ret);
	}

	if ((ret = UnionSetResponseXMLPackageValue("body/joinCondition",joinCondition)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionSetResponseXMLPackageValue[%s][%s]!\n","body/joinCondition",joinCondition);
		return(ret);
	}

	if ((ret = UnionSetResponseXMLPackageValue("body/remark",remark)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0503:: UnionSetResponseXMLPackageValue[%s][%s]!\n","body/remark",remark);
		return(ret);
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",fieldNum);
	UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf);
	for (i = 0; i < fieldNum; i++)
	{	
		//isExistField = 0;
		for (j = 0; j < tableNum; j++)
		{
			memset(tableName,0,sizeof(tableName));
			memset(fieldName,0,sizeof(fieldName));
			if ((ptr = strchr(fieldGrp[i],'.')) == NULL)	// 不带表名
			{
				strcpy(tableName,tableNameGrp[j]);
				strcpy(fieldName,fieldGrp[i]);
				if (UnionExistFieldOfTableName(tableName,fieldName))
				{
					//isExistField = 1;
					break;
				}
			}
			else						// 带表名
			{
				memcpy(tableName,fieldGrp[i],ptr - fieldGrp[i]);
				strcpy(fieldName,ptr + 1);
				if (UnionExistFieldOfTableName(tableName,fieldName))
				{
					//isExistField = 1;
					break;
				}
				else
				{
					UnionUserErrLog("in UnionDealServiceCode0503:: UnionExistFieldOfTableName[%s]!\n",fieldGrp[i]);
					return(errCodeObjectMDL_FieldNotExist);
				}
			}
		}
		
		/*
		if (!isExistField)
		{
			UnionUserErrLog("in UnionDealServiceCode0503:: UnionExistFieldOfTableName[%s]!\n",fieldGrp[i]);
			return(errCodeObjectMDL_FieldNotExist);
		}
		*/

		// 检查表是否已经存在
		memset(fileName,0,sizeof(fileName));
		UnionGetFileNameOfTableDef(tableName,fileName);
	
		// 文件不存在
		if (access(fileName,0) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0503:: fileName[%s] not found!\n",fileName);
			return(errCodeDatabaseMDL_TableNotFound);
		}
	
		// 初始化文件
		if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0503:: UnionInitXMLPackage[%s]!\n",fileName);
			return(ret);
		}
	
		for (k = 1;;k++)
		{
			// 读取定位
			if ((ret = UnionLocateXMLPackage("field",k)) < 0)
			{
				
				UnionAuditLog("in UnionDealServiceCode0503:: UnionLocateXMLPackage k = [%d]\n", k);
				break;
			}

			// 字段名 
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((ret = UnionReadXMLPackageValue("fieldName",tmpBuf,sizeof(tmpBuf))) < 0)
	 		{
		 		UnionUserErrLog("in UnionDealServiceCode0503:: UnionReadXMLPackageValue[%d][fieldName][%s]!\n",k,fieldName);
		 		return(ret);
	 		}
	 		if (strcmp(fieldName,tmpBuf) != 0)
	 			continue;
	 		else
	 			break;
	 	}

		// 响应定位	
		if ((ret = UnionLocateResponseNewXMLPackage("body/detail", i + 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0503:: UnionLocateResponseNewXMLPackage[%d]!\n",i + 1);
			return(ret);
		}
		
		if ((ret = UnionSetResponseXMLPackageValue("fieldName",fieldGrp[i])) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0503:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"fieldName",fieldGrp[i]);
			return(ret);
		}

		// 字段中文名
		memset(fieldChnName,0,sizeof(fieldChnName));
		if ((ret = UnionReadXMLPackageValue("fieldChnName",fieldChnName,sizeof(fieldChnName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0503:: UnionReadXMLPackageValue[%d][fieldChnName][%s]!\n",i,fieldChnName);
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("fieldChnName",fieldChnName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0503:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"fieldChnName",fieldChnName);
			return(ret);
		}

		// 字段类型
		memset(fieldType,0,sizeof(fieldType));
		if ((ret = UnionReadXMLPackageValue("fieldType",fieldType,sizeof(fieldType))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0503:: UnionReadXMLPackageValue[%d][fieldType][%s]!\n",i,fieldType);
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("fieldType",fieldType)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0503:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"fieldType",fieldType);
			return(ret);
		}

		// 字段大小
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("fieldSize",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			//UnionUserErrLog("in UnionDealServiceCode0503:: UnionReadXMLPackageValue[%d][fieldSize][%s]!\n",i,tmpBuf);
			//return(ret);
			UnionSetXMLPackageValue("fieldSize","0");
			strcpy(tmpBuf,"0");
			writeFlag = 1;
		}

		if ((ret = UnionSetResponseXMLPackageValue("fieldSize",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0503:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"fieldSize",tmpBuf);
			return(ret);
		}
		
		// 是否关键字
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("isPrimaryKey",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			//UnionUserErrLog("in UnionDealServiceCode0503:: UnionReadXMLPackageValue[%d][isPrimaryKey][%s]!\n",i,tmpBuf);
			//return(ret);
			UnionSetXMLPackageValue("isPrimaryKey","0");
			strcpy(tmpBuf,"0");
			writeFlag = 1;
		}

		if ((ret = UnionSetResponseXMLPackageValue("isPrimaryKey",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0503:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"isPrimaryKey",tmpBuf);
			return(ret);
		}

		// 是否为空
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("isNull",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			//UnionUserErrLog("in UnionDealServiceCode0503:: UnionReadXMLPackageValue[%d][isNull][%s]!\n",i,tmpBuf);
			//return(ret);
			UnionSetXMLPackageValue("isNull","1");
			strcpy(tmpBuf,"1");
			writeFlag = 1;
		}

		if ((ret = UnionSetResponseXMLPackageValue("isNull",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0503:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"isNull",tmpBuf);
			return(ret);
		}

		// 字段限制方法 
		memset(fieldLimitMethod,0,sizeof(fieldLimitMethod));
		if ((ret = UnionReadXMLPackageValue("fieldLimitMethod",fieldLimitMethod,sizeof(fieldLimitMethod))) < 0)
		{
			//UnionUserErrLog("in UnionDealServiceCode0503:: UnionReadXMLPackageValue[%d][fieldLimitMethod][%s]!\n",i,fieldLimitMethod);
			//return(ret);
			UnionSetXMLPackageValue("fieldLimitMethod","");
			writeFlag = 1;
		}

		if ((ret = UnionSetResponseXMLPackageValue("fieldLimitMethod",fieldLimitMethod)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0503:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"fieldLimitMethod",fieldLimitMethod);
			return(ret);
		}

		// 字段控件定义
		memset(fieldControl,0,sizeof(fieldControl));
		if ((ret = UnionReadXMLPackageValue("fieldControl",fieldControl,sizeof(fieldControl))) < 0)
		{
			//UnionUserErrLog("in UnionDealServiceCode0503:: UnionReadXMLPackageValue[%d][fieldControl][%s]!\n",i,fieldControl);
			//return(ret);
			UnionSetXMLPackageValue("fieldControl","");
			writeFlag = 1;
		}

		if ((ret = UnionSetResponseXMLPackageValue("fieldControl",fieldControl)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0503:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"fieldControl",fieldControl);
			return(ret);
		}

		// 字段取值方法 
		memset(fieldValueMethod,0,sizeof(fieldValueMethod));
		if ((ret = UnionReadXMLPackageValue("fieldValueMethod",fieldValueMethod,sizeof(fieldValueMethod))) < 0)
		{
			//UnionUserErrLog("in UnionDealServiceCode0503:: UnionReadXMLPackageValue[%d][fieldValueMethod][%s]!\n",i,fieldValueMethod);
			//return(ret);
			UnionSetXMLPackageValue("fieldValueMethod","");
			writeFlag = 1;
		}
		
		if ((ret = UnionSetResponseXMLPackageValue("fieldValueMethod",fieldValueMethod)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0503:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"fieldValueMethod",fieldValueMethod);
			return(ret);
		}
			
		// 备注
		memset(remark,0,sizeof(remark));
		if ((ret = UnionReadXMLPackageValue("remark",remark,sizeof(remark))) < 0)
		{
			//UnionUserErrLog("in UnionDealServiceCode0503:: UnionReadXMLPackageValue[%d][fieldValueMethod][%s]!\n",i,fieldValueMethod);
			//return(ret);
			UnionSetXMLPackageValue("remark","");
			writeFlag = 1;
		}
		
		if ((ret = UnionSetResponseXMLPackageValue("remark",remark)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0503:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"remark",remark);
			return(ret);
		}
		
		if(writeFlag)
			UnionXMLPackageToFile(fileName);
		
	}

	return 0;
}
