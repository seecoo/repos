//	Author:		zhang yong ding
//	Date:		2012/12/11
//	Version:	1.0

#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_	
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/dir.h>

#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "UnionProc.h"
#include "unionRecFile.h"
#include "UnionStr.h"
#include "unionWorkingDir.h"
#include "unionREC.h"
#ifndef _WIN32
#include "unionCommand.h"
#endif

#include "unionVersion.h"
#include "unionErrCode.h"
#include "unionTableDef.h"
#include "unionMXML.h"
#include "unionXMLPackage.h"

extern PUnionTableDefTBL		pgunionTableDefTBL;
int			gunionDBIndexIsLoad = 0;
TUnionDBIndexTBL	gunionDBIndexTBL;

#define maxTableNum	128
#define maxFieldNum	64

int UnionGetFileDirOfTableDef(char *fileName)
{
	sprintf(fileName,"%s/tableDef",getenv("UNIONETC"));
	return(0);
}

int UnionGetFileNameOfTableDef(char *tableName,char *fileName)
{
	return(sprintf(fileName,"%s/tableDef/%s.xml",getenv("UNIONETC"),tableName));
}

// 判断是否是虚拟表
int UnionIsVirtualTable(char *tableName)
{
	if (strncasecmp(tableName,"Virtual_",8) == 0)
		return(1);
	else
		return(0);
}

int UnionGetTableNameFromFileName(char *fileName,char *tableName)
{
	memcpy(tableName,fileName,strlen(fileName) - 4);
	return(0);
}

int UnionReloadTableDefTBL()
{
	int	i,j;
	int	ret;
	int	tableNum = 0;
	int	fieldNum = 0;
	int	fileNum = 0;
	char	tmpBuf[128];
	char	tableFileName[128];
	char	fileDir[128];
	char	fileName[128];

	struct dirent	**nameList;
	
	if ((ret = UnionConnectTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionReloadTableDefTBL:: UnionConnectTableDefTBL ret = %d!\n", ret);
		return(ret);
	}
	
	memset(fileDir,0,sizeof(fileDir));
	UnionGetFileDirOfTableDef(fileDir);
	
	// 打开目录
	if ((fileNum = scandir(fileDir,&nameList ,0,alphasort)) < 0)
	{
		UnionUserErrLog("in UnionReloadTableDefTBL:: scandir[%s]!\n",fileDir);
		return(fileNum);
	}
	
	tableNum = 0;
	for (i = 0; i < fileNum; i++)
	{
		memset(fileName,0,sizeof(fileName));
		memcpy(fileName,nameList[i]->d_name,nameList[i]->d_reclen);
		free(nameList[i]);
	
		if (fileName[0] == '.')
			continue;
		
		if (memcmp(fileName + strlen(fileName) - 4, ".xml",4) != 0)
			continue;
		
		// 加载文件
		memset(tableFileName,0,sizeof(tableFileName));
		sprintf(tableFileName,"%s/%s",fileDir,fileName);
		
		if ((ret = UnionInitXMLPackage(tableFileName,NULL,0)) < 0)
		{
			UnionUserErrLog("in UnionReloadTableDefTBL:: UnionInitXMLPackage[%s]!\n",tableFileName);
			continue;
		}
		
		memset(&pgunionTableDefTBL->rec[tableNum],0,sizeof(pgunionTableDefTBL->rec[tableNum]));
	
		// 表名
		UnionGetTableNameFromFileName(fileName,pgunionTableDefTBL->rec[tableNum].tableName);
		
		// 表中文名
		if ((ret = UnionReadXMLPackageValue("tableChnName", pgunionTableDefTBL->rec[tableNum].tableChnName, sizeof(pgunionTableDefTBL->rec[tableNum].tableChnName))) < 0)
		{
			UnionUserErrLog("in UnionReloadTableDefTBL:: UnionReadXMLPackageValue[tableChnName]!\n");
			continue;
		}
		
		// 表类型
		if ((ret = UnionReadXMLPackageValue("tableType", pgunionTableDefTBL->rec[tableNum].tableType, sizeof(pgunionTableDefTBL->rec[tableNum].tableType))) < 0)
		{
			UnionUserErrLog("in UnionReloadTableDefTBL:: UnionReadXMLPackageValue[tableType]!\n");
			continue;
		}

		// 缓存方法,0：标准，1：个性化
		if ((ret = UnionReadXMLPackageValue("methodOfCached", tmpBuf, sizeof(tmpBuf))) < 0)
			pgunionTableDefTBL->rec[tableNum].methodOfCached = 0;
		else
			pgunionTableDefTBL->rec[tableNum].methodOfCached = atoi(tmpBuf);

		fieldNum = 0;
		for (j = 0; ; j++)
		{
			if ((ret = UnionLocateXMLPackage("field",j+1)) < 0)
			{
				UnionLog("in UnionReloadTableDefTBL:: UnionLocateXMLPackage[%s %d]!\n","field",j+1);
				break;
			}
			// 字段名
			if ((ret = UnionReadXMLPackageValue("fieldName", pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldName, sizeof(pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldName))) < 0)
			{
				UnionUserErrLog("in UnionReloadTableDefTBL:: UnionReadXMLPackageValue[%s]!\n","fieldName");
				continue;
			}

			// 字段中文名
			if ((ret = UnionReadXMLPackageValue("fieldChnName", pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldChnName, sizeof(pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldChnName))) < 0)
			{
				UnionUserErrLog("in UnionReloadTableDefTBL:: UnionReadXMLPackageValue[%s]!\n","fieldChnName");
				continue;
			}

			// 字段类型
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((ret = UnionReadXMLPackageValue("fieldType", tmpBuf, sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionReloadTableDefTBL:: UnionReadXMLPackageValue[%s]!\n","fieldType");
				continue;
			}
			pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldType = atoi(tmpBuf);
			
			// 字段大小
			if ((pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldType == 1) ||		// 字符串
				(pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldType == 3) ||	// 浮点型
				(pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldType == 4) ||	// 二进制
				(pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldType == 7) ||	// 日期时间
				(pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldType == 99))		// 虚拟型
			{
				memset(tmpBuf,0,sizeof(tmpBuf));
				if ((ret = UnionReadXMLPackageValue("fieldSize", tmpBuf, sizeof(tmpBuf))) < 0)
				{
					UnionUserErrLog("in UnionReloadTableDefTBL:: UnionReadXMLPackageValue[%s]!\n","fieldSize");
					continue;
				}
				if ((pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldSize = atof(tmpBuf)) > 4000.00)
					pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldSize = 4000.00;
			}
			else if (pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldType == 5)	// 布尔型
				pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldSize = 1;
			else
				pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldSize = 0;
				
			// 默认值
			if ((ret = UnionReadXMLPackageValue("defaultValue", pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].defaultValue, sizeof(pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].defaultValue))) < 0)
			{
				UnionLog("in UnionReloadTableDefTBL:: UnionReadXMLPackageValue[%s]!\n","defaultValue");
				strcpy(pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].defaultValue,"");
				//continue;
			}
			
			// 是否关键字
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((ret = UnionReadXMLPackageValue("isPrimaryKey", tmpBuf, sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionReloadTableDefTBL:: UnionReadXMLPackageValue[%s]!\n","isPrimaryKey");
				continue;
			}
			pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].isPrimaryKey = atoi(tmpBuf);
			
			if (pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].isPrimaryKey == 1)
			{
				pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].isNull = 0;
			}
			else
			{
				// 是否为空
				memset(tmpBuf,0,sizeof(tmpBuf));
				if ((ret = UnionReadXMLPackageValue("isNull", tmpBuf, sizeof(tmpBuf))) < 0)
				{
					UnionLog("in UnionReloadTableDefTBL:: UnionReadXMLPackageValue[%s]!\n","isNull");
					pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].isNull = 1;
				}
				else
					pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].isNull = atoi(tmpBuf);
			}
			fieldNum ++;
		}
		UnionLog("in UnionReloadTableDefTBL:: tableNum = [%d]\n",pgunionTableDefTBL->tableNum);
		pgunionTableDefTBL->rec[tableNum].fieldNum = fieldNum;
		tableNum ++;
	}

	free(nameList);
	pgunionTableDefTBL->tableNum = tableNum;
	UnionLog("in UnionReloadTableDefTBL:: tableNum = [%d]\n",pgunionTableDefTBL->tableNum);

	return(pgunionTableDefTBL->tableNum);
}

/******************************

	功能：创建外键、索引、唯一值等建表语句
	参数：
		tableName: 表名
		sql: 返回生成的sql语句
	返回值：
		<0: 错误
		>=0:正确

*******************************/
int UnionGetExpandSqlByTableName(char *database,char *tableName,char *sql,int auto_increment_falg,int startNum)
{
	int	ret = 0;
	int	j = 0;
	int	t = 0;
	int	i = 0;
	int	h = 0;
	int	index = 0;
	int	len = 0;
	//int	offset = 0;
	char	tmpBuf[1024];
	char	foreignTableName[16][64];
	char	isDeleteCascade[16][32];
	char	isUpdateCascade[16][32];
	char	cascadeStr[128];
	char	foreignFields[16][1024];
	char	localFields[16][1024];
	char	uniqueFields[16][1024];
	char	indexFields[16][1024];
	char	foreignUniqueFields[16][1024];

	char	foreignFieldName[32][128];
	char	localFieldName[32][128];
	char	uniqueFieldName[32][128];
	char	indexFieldName[32][128];

	int	foreignFieldNum = 0;
	int	localFieldNum = 0;
	int	uniqueFieldNum = 0;
	int	indexFieldNum = 0;
	char	fileName[1024];
	
	PUnionTableDef localPTableDef = NULL,foreignPTableDef = NULL;

	memset(foreignTableName,0,sizeof(foreignTableName));
	memset(foreignFields,0,sizeof(foreignFields));
	memset(localFields,0,sizeof(localFields));
	memset(uniqueFields,0,sizeof(uniqueFields));
	memset(indexFields,0,sizeof(indexFields));
	memset(foreignUniqueFields,0,sizeof(foreignUniqueFields));
	memset(foreignFieldName,0,sizeof(foreignFieldName));
	memset(localFieldName,0,sizeof(localFieldName));
	memset(uniqueFieldName,0,sizeof(uniqueFieldName));
	memset(indexFieldName,0,sizeof(indexFieldName));
	
	// 初始化文件
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTableDef(tableName,fileName);

	if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionInitXMLPackage[%s]!\n",fileName);
		return(ret);
	}

	// 读取外键
	for (i = 0; ; i++)
	{
		// 设定的外键数大于预计的
		if (i >= 16)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName:: foreignkeyNum [%d]  > 16!\n",i+1);
			return(errCodeObjectMDL_FieldNumberTooMuch);
		}
		
		if ((ret = UnionLocateXMLPackage("foreignkey", i+1)) < 0)
		{
			UnionLog("in UnionGetExpandSqlByTableName:: UnionLocateXMLPackage foreign[%d] not found!\n",i+1);
			break;
		}

		// 读取外键中的参照表名
		memset(foreignTableName[i],0,sizeof(foreignTableName[i]));
		if ((ret = UnionReadXMLPackageValue("references",foreignTableName[i],sizeof(foreignTableName[i]))) < 0)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName::  UnionReadXMLPackageValue[%d][%s]\n",i+1,"references");
			return(ret);
		}

		// 读外表关联删除标识
		strcpy(isDeleteCascade[i], "1");
		if ((ret = UnionReadXMLPackageValue("isDeleteCascade",isDeleteCascade[i],sizeof(isDeleteCascade[i]))) < 0)
		{
			//UnionLog("in UnionGetExpandSqlByTableName::  UnionReadXMLPackageValue[%d][%s]\n",i+1,"isDeleteCascade");
			//return(ret);
		}
		
		// 读外表关联更新标识
		strcpy(isUpdateCascade[i], "1");
		if ((ret = UnionReadXMLPackageValue("isUpdateCascade",isUpdateCascade[i],sizeof(isUpdateCascade[i]))) < 0)
		{
			//UnionLog("in UnionGetExpandSqlByTableName::  UnionReadXMLPackageValue[%d][%s]\n",i+1,"isUpdateCascade");
			//return(ret);
		}

		// 读取外键中本地表的字段名
		memset(localFields[i],0,sizeof(localFields[i]));
		if ((ret = UnionReadXMLPackageValue("localfields",localFields[i],sizeof(localFields[i]))) < 0)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionReadXMLPackageValue[%d][%s]\n",i+1,"localfields");
			return(ret);
		}

		// 读取外键中参照表的字段名
		memset(foreignFields[i],0,sizeof(foreignFields[i]));
		if ((ret = UnionReadXMLPackageValue("foreignfields",foreignFields[i],sizeof(foreignFields[i]))) < 0)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionReadXMLPackageValue[%d][%s]\n",i+1,"foreignfields");
			return(ret);
		}
	}

	// 读取唯一值字段
	for (i = 0; ; i++)
	{
		// 读取的唯一值字段大于预计的
		if (i >= 16)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName:: uniqueNum [%d]  > 16!\n",i+1);
			return(errCodeObjectMDL_FieldNumberTooMuch);
		}

		if ((ret = UnionLocateXMLPackage("unique", i+1)) < 0)
		{
			UnionLog("in UnionGetExpandSqlByTableName:: UnionLocateXMLPackage unique[%d] not found!\n",i+1);
			break;
		}

		// 读取fields
		memset(uniqueFields[i],0,sizeof(uniqueFields[i]));
		if ((ret = UnionReadXMLPackageValue("fields",uniqueFields[i],sizeof(uniqueFields[i]))) < 0)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName::  UnionReadXMLPackageValue[%d][%s] unique\n",i+1,"fields");
			return(ret);
		}
	}

	// 读取索引
	for (i = 0; ; i++)
	{
		// 读取的索引数大于预计的
		if (i >= 16)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName:: indexNum [%d] > 16!\n",i+1);
			return(errCodeObjectMDL_FieldNumberTooMuch);
		}

		if ((ret = UnionLocateXMLPackage("index", i+1)) < 0)
		{
			UnionLog("in UnionGetExpandSqlByTableName:: UnionLocateXMLPackage index[%d] not found!\n",i+1);
			break;
		}

		// 读取fields
		memset(indexFields[i],0,sizeof(indexFields[i]));
		if ((ret = UnionReadXMLPackageValue("fields",indexFields[i],sizeof(indexFields[i]))) < 0)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName::  UnionReadXMLPackageValue[%d][%s] index\n",i+1,"fields");
			return(ret);
		}
	}

	 // 读取文件
	if ((localPTableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionFindTableDef[%s] not found!\n",tableName);
		return(errCodeDatabaseMDL_TableNotFound);
	}
	
	// 检查外键合法性
	for (i = 0; i < 16; i++)
	{
		// 读取外键中参照表
		if (strlen(foreignTableName[i]) == 0)
			break;

		memset(fileName,0,sizeof(fileName));
		UnionGetFileNameOfTableDef(foreignTableName[i],fileName);

		// 初始化文件
		if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionInitXMLPackage[%s]!\n",fileName);
			return(ret);
		}
		
		// 读取参照表中的唯一值字段
		for (j = 0; ; j++)
		{
			if (j >= 16)
			{
				UnionUserErrLog("in UnionGetExpandSqlByTableName:: uniqueNum [%d]  > 16!\n",j+1);
				return(errCodeObjectMDL_FieldNumberTooMuch);
			}

			if ((ret = UnionLocateXMLPackage("unique", j+1)) < 0)
			{
				UnionLog("in UnionGetExpandSqlByTableName:: UnionLocateXMLPackage unique[%d] not found!\n",j+1);
				break;
			}
			// 读取唯一值
			memset(foreignUniqueFields[j],0,sizeof(foreignUniqueFields[j]));
			if ((ret = UnionReadXMLPackageValue("fields",foreignUniqueFields[j],sizeof(foreignUniqueFields[j]))) < 0)
			{
				UnionUserErrLog("in UnionGetExpandSqlByTableName::  UnionReadXMLPackageValue[%d][%s]  foreign unique\n",j+1,"fields");
				return(ret);
			}
		}
		
		// 读取文件
		if ((foreignPTableDef = UnionFindTableDef(foreignTableName[i])) == NULL)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionFindTableDef[%s] not found!\n",foreignTableName[i]);
			return(errCodeDatabaseMDL_TableNotFound);
		}

		// 检查外键中参照表中的字段是否唯一
		//for (j = 0; j < 16; j++)
		{
			// 拼分域定义串
			memset(foreignFieldName,0,sizeof(foreignFieldName));
			memset(tmpBuf,0,sizeof(tmpBuf));
			strcpy(tmpBuf,foreignFields[i]);
			if ((foreignFieldNum = UnionSeprateVarStrIntoArray(foreignFields[i],strlen(foreignFields[i]),',',(char *)&foreignFieldName,32,128)) < 0)
			{
				UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionSeprateVarStrIntoArray [%04zu][%s]!\n",strlen(foreignFields[i]),foreignFields[i]);
				return(foreignFieldNum);
			}
			strcpy(foreignFields[i],tmpBuf);
			
			// 拼分域定义串
			memset(localFieldName,0,sizeof(localFieldName));
			memset(tmpBuf,0,sizeof(tmpBuf));
			strcpy(tmpBuf,localFields[i]);
			if ((localFieldNum = UnionSeprateVarStrIntoArray(localFields[i],strlen(localFields[i]),',',(char *)&localFieldName,32,128)) < 0)
			{
				UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionSeprateVarStrIntoArray [%04zu][%s]!\n",strlen(localFields[i]),localFields[i]);
				return(localFieldNum);
			}
			strcpy(localFields[i],tmpBuf);
			
			// 外键中本地表与参照表字段数量对比
			if (foreignFieldNum != localFieldNum)
			{
				UnionUserErrLog("in UnionGetExpandSqlByTableName:: foreignFieldNum[%d] != localFieldNum[%d]!\n",foreignFieldNum,localFieldNum);
				return(errCodeObjectMDL_FieldNumberError);
			}
		}
			
			// 检查外键中表中的字段名是否存在
			for (index = 0; index < localFieldNum; index++)
			{
				// 检查外键中本地表的字段名是否存在
				for (t = 0; t < localPTableDef->fieldNum; t++)
				{
					if (strcmp(localFieldName[index],localPTableDef->fieldDef[t].fieldName) == 0)
						break;
				}
				if (t == localPTableDef->fieldNum)
				{
					UnionUserErrLog("in UnionGetExpandSqlByTableName:: localFieldName [%s] not found!\n",localFieldName[index]);
					return(errCodeObjectMDL_FieldNotExist);
				}

				// 检查外键中参照表字段名是否存在
				for (h = 0; h < foreignPTableDef->fieldNum; h++)
				{
					if (strcmp(foreignFieldName[index],foreignPTableDef->fieldDef[h].fieldName) == 0)
						break;
				}
				if (h == foreignPTableDef->fieldNum)
				{
					UnionUserErrLog("in UnionGetExpandSqlByTableName:: foreignTableName[%s] foreignFieldName [%s] not found!\n",foreignTableName[i],foreignFieldName[index]);
					return(errCodeObjectMDL_FieldNotExist);
				}
				
				// 对比字段类型
				// 6 和 2 是同一类型
				if (localPTableDef->fieldDef[t].fieldType != foreignPTableDef->fieldDef[h].fieldType &&
					abs(localPTableDef->fieldDef[t].fieldType - foreignPTableDef->fieldDef[h].fieldType) != 4)
				{
					UnionUserErrLog("in UnionGetExpandSqlByTableName:: foreignFieldName[%s].fieldType[%d] != localFieldName[%s].fieldType[%d] !\n",foreignFieldName[index],foreignPTableDef->fieldDef[h].fieldType, localFieldName[index], localPTableDef->fieldDef[t].fieldType);
					return(errCodeObjectMDL_FieldTypeInvalid);
				}

				// 对比字段大小
				if ((localPTableDef->fieldDef[t].fieldType != 2) && (localPTableDef->fieldDef[t].fieldType != 3) && (localPTableDef->fieldDef[t].fieldType != 5))
				{
					if (localPTableDef->fieldDef[t].fieldSize != foreignPTableDef->fieldDef[h].fieldSize)
					{
						UnionUserErrLog("in UnionGetExpandSqlByTableName:: foreignFieldName[%s].fieldSize != localFieldName[%s].fieldSize!\n",foreignFieldName[index],localFieldName[index]);
						return(errCodeObjectMDL_ObjectIsInvalid);
					}
				}
				
			}
	}

	// 读取唯一值的字段名,并检查唯一值中的字段名是否存在
	for (i = 0; i < 16; i++)
	{
		memset(uniqueFieldName,0,sizeof(uniqueFieldName));
		memset(tmpBuf,0,sizeof(tmpBuf));
		strcpy(tmpBuf,uniqueFields[i]);
	
		// 拼分域定义串
		if ((uniqueFieldNum = UnionSeprateVarStrIntoArray(uniqueFields[i],strlen(uniqueFields[i]),',',(char *)&uniqueFieldName,32,128)) < 0)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionSeprateVarStrIntoArray [%04zu][%s]!\n",strlen(uniqueFields[i]),uniqueFields[i]);
			return(uniqueFieldNum);
		}
		strcpy(uniqueFields[i],tmpBuf);

		// 检查字段名是否存在
		for (t = 0; t < uniqueFieldNum; t++)
		{
			if (strlen(uniqueFieldName[t]) == 0)
				continue;
			for (j = 0; j < localPTableDef->fieldNum; j++)
			{
				if (strcmp(uniqueFieldName[t],localPTableDef->fieldDef[j].fieldName) == 0)
					break;
			}
			if (j == localPTableDef->fieldNum)
			{
				UnionUserErrLog("in UnionGetExpandSqlByTableName:: uniqueFieldName[%s] not found!\n",uniqueFieldName[t]);
				return(errCodeObjectMDL_FieldNotExist);
			}
		}
	
	}

	// 读取索引的字段名,并检查索引中的字段名是否存在
	for (i = 0; i < 16; i++)
	{
		memset(indexFieldName,0,sizeof(indexFieldName));
		memset(tmpBuf,0,sizeof(tmpBuf));
		strcpy(tmpBuf,indexFields[i]);

		// 拼分域定义串
		if ((indexFieldNum = UnionSeprateVarStrIntoArray(indexFields[i],strlen(indexFields[i]),',',(char *)&indexFieldName,32,128)) < 0)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionSeprateVarStrIntoArray [%04zu][%s]!\n",strlen(indexFields[i]),indexFields[i]);
			return(indexFieldNum);
		}
		strcpy(indexFields[i],tmpBuf);

		// 检查字段名是否存在
		for (t = 0; t < indexFieldNum; t++)
		{
			if (strlen(indexFieldName[t]) == 0)
				continue;
			for (j = 0; j < localPTableDef->fieldNum; j++)
			{
				if (strcmp(indexFieldName[t],localPTableDef->fieldDef[j].fieldName) == 0)
					break;
			}
			if (j == localPTableDef->fieldNum)
			{
				UnionUserErrLog("in UnionGetExpandSqlByTableName:: indexFieldName[%s] not found!\n",indexFieldName[i]);
				return(errCodeObjectMDL_FieldNotExist);
			}
		}
	}

	//len = strlen(sql);
	len = 0;

	// 创建唯一值语句
	for (i = 0; i < 16; i++)
	{
		if (strlen(uniqueFields[i]) == 0)
		{
			break;
		}
		if (i == 0)
		{
			//strcpy(&sql[len - 4],",");
			//len = strlen(sql);
			len += sprintf(sql + len,",\n\t-- 定义唯一值");
			len += sprintf(sql + len,"\n\tunique (%s)",uniqueFields[i]);
		}
		else
		{
			len += sprintf(sql + len,",\n\tunique (%s)",uniqueFields[i]);
		}
	}
	// 创建外键语句
	for (i = 0; i < 16; i++)
	{
		if (strlen(foreignTableName[i]) == 0)
		{
			break;
		}

		memset(cascadeStr,0,sizeof(cascadeStr));
		// offset = 0;
		if(strcmp(isDeleteCascade[i], "1") == 0)
			sprintf(cascadeStr,"on delete cascade ");
			//offset = sprintf(cascadeStr,"on delete cascade ");
		
		// ORACLE和DB2不支持级联更新
		/* 
		if(strcmp(isUpdateCascade[i], "1") == 0)
			sprintf(cascadeStr + offset,"on update cascade ");
		*/
		
		if (0 == i)
		{
			if (sql[len-1] == ',')
			{
				len += sprintf(sql + len,"\n\t-- 定义外键");
			}
			else
			{
				//strcpy(&sql[len - 4],",");
				//len = strlen(sql);
				len += sprintf(sql + len,",\n\t-- 定义外键");
			}
			len += sprintf(sql + len,"\n\tforeign key (%s) references  %s (%s) %s",localFields[i],foreignTableName[i],foreignFields[i], cascadeStr);
		}
		else
		{
			len += sprintf(sql + len,",\n\tforeign key (%s) references  %s (%s) %s",localFields[i],foreignTableName[i],foreignFields[i], cascadeStr);
		}
	}

	if (strcmp(database,"MYSQL") == 0)
		len += sprintf(sql + len, "\n)engine=innodb ");
	else
		len += sprintf(sql + len, "\n)");
		
	if (auto_increment_falg == 1)
	{
		len += sprintf(sql + len, "auto_increment=%d;\n",startNum);
	}
	else
		len += sprintf(sql + len, ";\n");

	// 创建索引
	for (i = 0; i < 16; i++)
	{
		if (strlen(indexFields[i]) == 0)
		{
			break;
		}
		len += sprintf(sql + len,"create index %s_index_%d on %s (%s);\n",tableName,i+1,tableName,indexFields[i]);
	}

	return(len);
}


// 生成建表语句
int UnionGetCreateSQLByTableName(char *tableName,char *dbType,char *buf,int sizeOfBuf)
{
	int	ret;
	int	sequence_falg = 0;
	int	auto_increment_falg = 0;
	int	len = 0;
	int	fldNum = 0;
	int	startNum = 1;
	char	sql[2048];
	char	strpPrimaryKey[512];
	char	tmpBuf[128];
	char	strFieldSize[32];
	char	tabBuf[128];
	char	database[128];
	PUnionTableDef		ptableDef = NULL;
	PUnionTableFieldDef	ptableFieldDef = NULL;
	
	if ((strcasecmp(tableName,"sysMenu") == 0) || (strcasecmp(tableName,"sysView") == 0) || (strcasecmp(tableName,"sysButton") == 0))
	{
		startNum = 1000;
	}
	
	if (UnionIsVirtualTable(tableName))
		return(0);
	
	if ((ptableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionUserErrLog("in UnionGetCreateSQLByTableName:: UnionFindTableDef[%s]!\n",tableName);
		return(errCodeDatabaseMDL_TableNotFound);
	}
	
	if (dbType == NULL)
		strcpy(database,"ORACLE");
	else
		strcpy(database,dbType);
	
	// 写创建语句
	len += sprintf(sql + len,"-- %s %s\n",ptableDef->tableName,ptableDef->tableChnName);
	len += sprintf(sql + len,"create table %s\n",ptableDef->tableName);
	len += sprintf(sql + len,"(\n");
		
	// 创建字段
	memset(strpPrimaryKey,0,sizeof(strpPrimaryKey));
	for (fldNum = 0; fldNum < ptableDef->fieldNum; fldNum++)
	{
		ptableFieldDef = &ptableDef->fieldDef[fldNum];
		
		// 虚拟字段
		if (ptableFieldDef->fieldType == 99)
			continue;
			
		if(fldNum == 0)
		{
			len += sprintf(sql + len,"\t-- %s\n",ptableFieldDef->fieldChnName);
		}
		else
		{
			len += sprintf(sql + len,",\n\t-- %s\n",ptableFieldDef->fieldChnName);
		}
		/*
		if (strlen(ptableFieldDef->remark) > 0)
			len += sprintf(sql + len,"\t-- %s\n",ptableFieldDef->remark);
		*/
		
		if (ptableFieldDef->isPrimaryKey)
		{
			strcat(strpPrimaryKey,ptableFieldDef->fieldName);
			strcat(strpPrimaryKey,",");
		}
		
		memset(tabBuf,0,sizeof(tabBuf));
		if (strlen(ptableFieldDef->fieldName) >= 16)
			strcpy(tabBuf,"\t");
		else if (strlen(ptableFieldDef->fieldName) >= 8) 
			strcpy(tabBuf,"\t\t");
		else
			strcpy(tabBuf,"\t\t\t");
			
		switch (ptableFieldDef->fieldType)
		{
			case 1:		// 字符串
			case 7:		// 时间型
				if (strcmp(database,"INFORMIX") == 0)
					len += sprintf(sql + len,"\t%s%slvarchar",ptableFieldDef->fieldName,tabBuf);
				else
					len += sprintf(sql + len,"\t%s%svarchar",ptableFieldDef->fieldName,tabBuf);
				len += sprintf(sql + len,"(%.f)",ptableFieldDef->fieldSize);
				
				if (strcmp(database,"MYSQL") == 0)
					len += sprintf(sql + len," binary");
				break;
			case 2:		// 整型
				len += sprintf(sql + len,"\t%s%sinteger",ptableFieldDef->fieldName,tabBuf);
				break;
			case 3:		// 浮点型
				memset(tmpBuf,0,sizeof(tmpBuf));
				memset(strFieldSize,0,sizeof(strFieldSize));
				sprintf(tmpBuf,"%.1f",ptableFieldDef->fieldSize);
				UnionConvertOneFldSeperatorInRecStrIntoAnother(tmpBuf,strlen(tmpBuf),'.',',',strFieldSize,sizeof(strFieldSize));
				if ((strcmp(database,"INFORMIX") == 0) || (strcmp(database,"MYSQL") == 0))
					len += sprintf(sql + len,"\t%s%sdecimal(%s)",ptableFieldDef->fieldName,tabBuf,strFieldSize);
				else
					len += sprintf(sql + len,"\t%s%snumber(%s)",ptableFieldDef->fieldName,tabBuf,strFieldSize);
				break;
			case 4:		// 二进制
				if (strcmp(database,"INFORMIX") == 0)
					len += sprintf(sql + len,"\t%s%slvarchar",ptableFieldDef->fieldName,tabBuf);
				else
					len += sprintf(sql + len,"\t%s%svarchar",ptableFieldDef->fieldName,tabBuf);
				len += sprintf(sql + len,"(%.f)",ptableFieldDef->fieldSize);
				if (strcmp(database,"MYSQL") == 0)
					len += sprintf(sql + len," binary");
				break;
			case 5:		// 布尔型
				len += sprintf(sql + len,"\t%s%sinteger",ptableFieldDef->fieldName,tabBuf);
				break;
			case 6:		// 自增长
				//if (strcmp(database,"INFORMIX") == 0)
					//len += sprintf(sql + len,"\t%s%sserial(%d)",ptableFieldDef->fieldName,tabBuf,startNum);
				//else
					len += sprintf(sql + len,"\t%s%sinteger",ptableFieldDef->fieldName,tabBuf);
				break;
			default:
				if (strcmp(database,"INFORMIX") == 0)
					len += sprintf(sql + len,"\t%s%slvarchar",ptableFieldDef->fieldName,tabBuf);
				else
					len += sprintf(sql + len,"\t%s%svarchar",ptableFieldDef->fieldName,tabBuf);
				len += sprintf(sql + len,"(%.f)",ptableFieldDef->fieldSize);
				if (strcmp(database,"MYSQL") == 0)
					len += sprintf(sql + len," binary");
				break;
		}
		
		// 缺省值
		if (strlen(ptableFieldDef->defaultValue) > 0)
		{
			switch (ptableFieldDef->fieldType)
			{
				case 2:		// 整型
				case 3:		// 浮点型
				case 5:		// 布尔型
				case 6:		// 自增长
					len += sprintf(sql + len," default %s",ptableFieldDef->defaultValue);
					break;
				default:
					len += sprintf(sql + len," default '%s'",ptableFieldDef->defaultValue);
					break;							
			}
		}

		switch (ptableFieldDef->fieldType)
		{
			case 6:		// 自增长
				if (strcmp(database,"MYSQL") == 0)
				{
					if (ptableFieldDef->isNull == 0)
						len += sprintf(sql + len," not null");
					auto_increment_falg = 1;
					len += sprintf(sql + len," auto_increment");
				}
				else if (strcmp(database,"SQLITE") == 0)
				{
					auto_increment_falg = 2;
					len += sprintf(sql + len," primary key autoincrement");
				}
				else
				{
					if (ptableFieldDef->isNull == 0)
						len += sprintf(sql + len," not null");
					sequence_falg = 1;
				}
				break;
			default:
				if (ptableFieldDef->isNull == 0)
					len += sprintf(sql + len," not null");
				break;
		}
	}
	
	if (auto_increment_falg != 2)
	{
		// 创建关键字
		ret = strlen(strpPrimaryKey);
		if (ret > 0)
			strpPrimaryKey[ret - 1] = 0;
		if (strlen(strpPrimaryKey) > 0)
		{
			len += sprintf(sql + len,",\n\t-- 定义关键字\n");
			len += sprintf(sql + len,"\tprimary key (%s)",strpPrimaryKey);
		}
	}
	
	if ((ret = UnionGetExpandSqlByTableName(database,ptableDef->tableName,sql+len,auto_increment_falg,startNum)) < 0)
	{
		UnionUserErrLog("in UnionGetCreateSQLByTableName:: UnionGetExpandSqlByTableName!\n");
		return(ret);
	}

	//len += sprintf(sql + len,"\n);\n");
	
	len += ret;
	//len = strlen(sql);

	// 创建序列
	if (sequence_falg)
	{
		// FFFFFF
		len += sprintf(sql + len,"create sequence %s_id_seq increment by 1 start with %d maxvalue 16777215 cycle nocache;\n",tableName,startNum);
	}

		
	if (len < sizeOfBuf)
	{
		memcpy(buf,sql,len);
	}
	else
	{
		UnionUserErrLog("in UnionGetCreateSQLByTableName:: len[%d] >= sizeOfBuf[%d]!\n",len,sizeOfBuf);
		return(errCodeSmallBuffer);
	}
	return(len);
}

int UnionGetSQLScript(char *tableName,char *database, char *flag, char *buf,int sizeOfBuf)
{
	int	i, j, k;
	int	ret;
	int	len = 0;
	int	tmpNum = 0;
	int	totalNum = 0;
	int	tableNum = 0;
	char	sql[40960];
	char	tableGrp[maxTableNum][128];
	char	*ptr = NULL;
	char	tableList[128][64], tableXmlFile[256], fileDir[256];
	int	isTurnToGen,genNum,creatTableInexList[128], haveGen;
	char	foreignTableName[64];	
	char	sequenceList[64][64];
	int	sequenceListIndex = 0;

	
	PUnionTableDef		ptableDef = NULL;
	PUnionTableDefTBL	ptableDefTBL = NULL;
	
	if ((ptableDefTBL = UnionGetTableDefTBL()) == NULL)
	{
		UnionLog("in UnionGetSQLScript:: UnionGetTableDefTBL!\n");
		return(errCodeParameter);
	}

	totalNum = 0;
	memset(sql,0,sizeof(sql));
	memset(tableList, 0, sizeof(tableList));
	if(strlen(tableName) == 0) // 生成所有表的SQL语句
	{
		genNum = ptableDefTBL->tableNum;
		tmpNum = ptableDefTBL->tableNum;
		for (i =0; i < ptableDefTBL->tableNum; i++)
		{
			creatTableInexList[i] = 0;
			strcpy(tableList[i], ptableDefTBL->rec[i].tableName);
		}
	
		memset(fileDir,0,sizeof(fileDir));
		UnionGetFileDirOfTableDef(fileDir);
	}
	else	// 只生成tableName的SQL语句
	{
		// 拼分表定义串
		if ((tableNum = UnionSeprateVarStrIntoArray(tableName,strlen(tableName),',',(char *)&tableGrp,maxTableNum,128)) < 0)
		{
			UnionUserErrLog("in UnionGetSQLScript:: UnionSeprateVarStrIntoArray[%s]!\n",tableName);
			return(tableNum);
		}
		for (i = 0; i < tableNum; i++)
		{
			if ((ptableDef = UnionFindTableDef(tableGrp[i])) == NULL)
			{
				UnionUserErrLog("in UnionGetSQLScript:: UnionFindTableDef[%s]!\n",tableName);
				return(errCodeDatabaseMDL_TableNotFound);
			}
			creatTableInexList[i] = 0;
			strcpy(tableList[i],tableGrp[i]);
		}
		genNum = tableNum;
		tmpNum = tableNum;
		memset(fileDir,0,sizeof(fileDir));
		UnionGetFileDirOfTableDef(fileDir);
	}
	while(totalNum < genNum)
	{
		for (i =0; i < ptableDefTBL->tableNum; i++)
		{
			haveGen = 0;
			isTurnToGen = 1;

			if (UnionIsVirtualTable(ptableDefTBL->rec[i].tableName))
			{
				totalNum ++;
				continue;
			}

			if (strlen(tableName) > 0)
			{
				for(k = 0; k < tmpNum; k++)
				{
					if(strcmp(ptableDefTBL->rec[i].tableName, tableList[k]) != 0)
						continue;
					else
						break;
				}
				if (k == tmpNum)
					continue;
			}

			for(k = 0; k < tmpNum; k++)
			{
				if(strcmp(ptableDefTBL->rec[i].tableName, tableList[k]) == 0 && creatTableInexList[k] > 0)
				{
					haveGen = 1;
					break;
				}
			}
			if(haveGen)
			{
				continue;
			}

			memset(tableXmlFile, 0, sizeof(tableXmlFile));
			sprintf(tableXmlFile, "%s/%s.xml", fileDir, ptableDefTBL->rec[i].tableName);
			if((ret = UnionInitXMLPackage(tableXmlFile,NULL,0)) < 0)
			{
				UnionUserErrLog("in UnionGetSQLScript:: UnionInitXMLPackage[%s]!\n", tableXmlFile);
				return(ret);
			}
			for (j = 0; ; j++)
			{
				if ((ret = UnionLocateXMLPackage("foreignkey", j + 1)) < 0)
				{
					UnionLog("in UnionGetSQLScript:: UnionLocateXMLPackage[%s %s %d]!\n",ptableDefTBL->rec[i].tableName, "foreignkey", j + 1);
					UnionLog("in UnionGetSQLScript:: UnionLocateXMLPackage[%s %d]!\n","foreignkey", j + 1);
					break;
				}
				// 读外表名
				memset(foreignTableName, 0, sizeof(foreignTableName));
				if ((ret = UnionReadXMLPackageValue("references", foreignTableName, sizeof(foreignTableName))) < 0)
				{
					UnionUserErrLog("in UnionGetSQLScript:: UnionReadXMLPackageValue[%s]!\n","fieldName");
					continue;
				}
				isTurnToGen = 1; // added 2014-05-15
				for(k = 0; k < tmpNum; k++)
				{
					if (strcmp(foreignTableName,ptableDefTBL->rec[i].tableName) == 0)
					{
						UnionUserErrLog("in UnionGetSQLScript:: foreignTableName[%s] == tableName[%s] invalied!\n",foreignTableName,ptableDefTBL->rec[i].tableName);
						UnionSetResponseRemark("表[%s]: 外键表[%s]不能和本表相同",ptableDefTBL->rec[i].tableName,foreignTableName);
						return (errCodeParameter);
					}

					if(strcmp(foreignTableName, tableList[k]) == 0 && creatTableInexList[k] == 0)
					{
						isTurnToGen = 0;
					}
				}
				UnionLog("in UnionGetSQLScript:: isTurnToGen = %d [%s %d]!\n", isTurnToGen, ptableDefTBL->rec[i].tableName, j + 1);
			}
			if(!isTurnToGen)
			{
				continue;
			}
		
			if ((ret =  UnionGetCreateSQLByTableName(ptableDefTBL->rec[i].tableName, database, sql + len, sizeof(sql) - len)) < 0)
			{
				UnionUserErrLog("in UnionGetSQLScript:: UnionGetCreateSQLByTableName[%s]!\n",ptableDefTBL->rec[i].tableName);
				return(ret);
			}
			
			if((ptr = strstr(sql + len, "create sequence ")) != NULL)
			{
				// add sequenceList
				UnionProgramerLog("in UnionGetSQLScript:: get 'create sequence ' [%s]!\n", sql + len);
				ptr = ptr + strlen("create sequence ");
				sscanf(ptr, "%63s", sequenceList[sequenceListIndex++]);
				UnionProgramerLog("in UnionGetSQLScript:: sequenceList[%d] = [%s]!\n", sequenceListIndex - 1, sequenceList[sequenceListIndex - 1]);
			}

			len += ret;
			totalNum ++;

			for(k = 0; k < tmpNum; k++)
			{
				if(strcmp(ptableDefTBL->rec[i].tableName, tableList[k]) == 0)
				{
					creatTableInexList[k] = totalNum;
				}
			}
		}
	}
	
	if(strcmp("DROP", flag) == 0)
	{
		memset(sql, 0, sizeof(sql));
		len = 0;
		for(i = 0; i < totalNum; i++)
		{
			for(j = 0; j < tmpNum; j++)
			{
				if(creatTableInexList[j] == totalNum - i)
				{
					UnionProgramerLog("in UnionGetSQLScript:: gen drop SQL: table create index = %d table = [%s]!\n", creatTableInexList[j], tableList[j]);
					len += sprintf(sql + len, "drop table %s;\n", tableList[j]);
					break;
				}
			}
		}
		// drop sequence
		for(i = 0; i < sequenceListIndex; i++)
		{
			len += sprintf(sql + len, "drop sequence %s;\n", sequenceList[i]);
		}
	}

	strncpy(buf, sql, sizeOfBuf);
	return(totalNum);		
}

/* 
功能：	辨别字段是否为真
参数：	tableName[in]		表名
	fieldName[in]		字段名
返回值：1			是
	0			否
*/
int UnionCheckFieldNameIsTrue(char *tableName,char *fieldName)
{
	int	i = 0;
	int	ret = 0;
	char	fileName[1024];
	char	tmpBuf[128];
	char	varNameOfEnabled[64];
	char	varValueOfEnabled[128];
	
	if (!UnionExistFieldOfTableName(tableName,fieldName))
		return(0);
	
	// 初始化文件
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTableDef(tableName,fileName);

	if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionCheckFieldNameIsTrue:: UnionInitXMLPackage[%s]!\n",fileName);
		return(1);
	}

	for (i = 0; ; i++)
	{
		if ((ret = UnionLocateXMLPackage("field",i+1)) < 0)
		{
			UnionLog("in UnionCheckFieldNameIsTrue:: UnionLocateXMLPackage[%s %d]!\n","field",i+1);
			break;
		}
		
		// 字段名
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("fieldName", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckFieldNameIsTrue:: UnionReadXMLPackageValue[%s]!\n","fieldName");
			continue;
		}
		
		if (strcmp(fieldName,tmpBuf) != 0)
			continue;
		
		// 启用的变量名
		memset(varNameOfEnabled,0,sizeof(varNameOfEnabled));
		if ((ret = UnionReadXMLPackageValue("varNameOfEnabled", varNameOfEnabled, sizeof(varNameOfEnabled))) <= 0)
			return(1);
		
		// 变量名没有定义
		if (UnionReadStringTypeRECVar(varNameOfEnabled) == NULL)
			return(1);
			
		// 启用的变量值
		memset(varValueOfEnabled,0,sizeof(varValueOfEnabled));
		if ((ret = UnionReadXMLPackageValue("varValueOfEnabled", varValueOfEnabled, sizeof(varValueOfEnabled))) <= 0)
			return(1);

		if (strcmp(UnionReadStringTypeRECVar(varNameOfEnabled),varValueOfEnabled) == 0)
			return(1);
		else
			return(0);
	}

	return(1);
}

/* 
功能：	转换表字段，过滤不显示的字段
参数：	useFlag[in]		用途，1：显示，2：菜单管理
	tableList[in]		表名
	fieldList[in|out]	字段名
返回值：1			是
	0			否
*/
int UnionTransformTableFileList(int useFlag,char *tableList,char *fieldList)
{
	int	i,j;
	int	len = 0;;
	int	ret;
	int	fieldNum;
	int	tableNum;
	char	tableName[128];
	char	fieldName[128];
	char	tmpBuf[2048];
	char	tableNameGrp[maxTableNum][128];
	char	fieldGrp[maxFieldNum][128];
	char	*ptr = NULL;

	// 拼分表名定义串
	memset(tableNameGrp,0,sizeof(tableNameGrp));
	if ((tableNum = UnionSeprateVarStrIntoArray(tableList,strlen(tableList),',',(char *)&tableNameGrp,maxTableNum,128)) < 0)
	{
		UnionUserErrLog("in UnionTransformTableFileList:: UnionSeprateVarStrIntoArray[%s]!\n",tableList);
		return(tableNum);
	}

	memset(fieldGrp,0,sizeof(fieldGrp));
	if ((fieldNum = UnionSeprateVarStrIntoArray(fieldList,strlen(fieldList),',',(char *)&fieldGrp,maxFieldNum,128)) < 0)
	{
		UnionUserErrLog("in UnionTransformTableFileList:: UnionSeprateVarStrIntoArray[%s]!\n",fieldList);
		return(fieldNum);
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	for (i = 0; i < fieldNum; i++)
	{
		for (j = 0; j < tableNum; j++)
		{
			memset(tableName,0,sizeof(tableName));
			memset(fieldName,0,sizeof(fieldName));
			if ((ptr = strchr(fieldGrp[i],'.')) == NULL)	// 不带表名
			{
				strcpy(tableName,tableNameGrp[j]);
				strcpy(fieldName,fieldGrp[i]);

				if (useFlag == 1)	// 显示
					ret = UnionCheckFieldNameIsTrue(tableName,fieldName);
				else			// 菜单管理
					ret = UnionExistFieldOfTableName(tableName,fieldName);
				
				if (ret)
				{
					len += sprintf(tmpBuf+len,"%s,",fieldGrp[i]);
					break;
				}
				else
					continue;
			}
			else						// 带表名
			{
				memcpy(tableName,fieldGrp[i],ptr - fieldGrp[i]);
				strcpy(fieldName,ptr + 1);

				if (useFlag == 1)	// 显示
					ret = UnionCheckFieldNameIsTrue(tableName,fieldName);
				else			// 菜单管理
					ret = UnionExistFieldOfTableName(tableName,fieldName);
				
				if (ret)
					len += sprintf(tmpBuf+len,"%s,",fieldGrp[i]);

				break;
			}
		}
	}
	if (len > 0)
	{
		tmpBuf[len] = 0;
		memcpy(fieldList,tmpBuf,len);
		return(len - 1);
	}
	else
	{
		fieldList[0] = 0;
		return(0);
	}
}

// 根据表名获取数据库索引位置
int UnionGetDBIndexByTableName(char *tableName)
{
	int	i;
	int	ret;
	char	fileName[256];
	char	dbIndex[32];
	PUnionTableDefTBL	ptableDefTBL = NULL;
	
	// 非SQLITE，不区分
	if (strcasecmp(UnionGetDataBaseType(),"SQLITE") != 0)
		return(0);

loop:	
	if (gunionDBIndexIsLoad)
	{
		for (i = 0; i < gunionDBIndexTBL.tableNum; i ++)
		{
			if (strcasecmp(tableName,gunionDBIndexTBL.rec[i].tableName) == 0)
				return(gunionDBIndexTBL.rec[i].dbIndex);
		}
		return(0);
	}
	
	if ((ptableDefTBL = UnionGetTableDefTBL()) == NULL)
	{
		UnionLog("in UnionGetDBIndexByTableName:: UnionGetTableDefTBL!\n");
		return(errCodeParameter);
	}

	for (i = 0; i < ptableDefTBL->tableNum; i ++)
	{
		// 初始化文件
		ret = UnionGetFileNameOfTableDef(ptableDefTBL->rec[i].tableName,fileName);
		fileName[ret] = 0;
	
		if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
		{
			UnionUserErrLog("in UnionGetDBIndexByTableName:: UnionInitXMLPackage[%s]!\n",fileName);
			return(ret);
		}
		
		strcpy(gunionDBIndexTBL.rec[i].tableName,ptableDefTBL->rec[i].tableName);
		
		// 数据库索引
		if ((ret = UnionReadXMLPackageValue("dbIndex", dbIndex, sizeof(dbIndex))) <= 0)
		{
			if (strcmp(ptableDefTBL->rec[i].tableType,"系统表") == 0)
				gunionDBIndexTBL.rec[i].dbIndex = 0;
			else
				gunionDBIndexTBL.rec[i].dbIndex = 1;
		}
		else
			gunionDBIndexTBL.rec[i].dbIndex = atoi(dbIndex);
	}
	gunionDBIndexTBL.tableNum = ptableDefTBL->tableNum;
	gunionDBIndexIsLoad = 1;
	goto loop;
}
