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

PUnionSharedMemoryModule	pgunionTableDefMDL = NULL;
PUnionTableDefTBL		pgunionTableDefTBL = NULL;

int UnionIsTableDefTBLConnected()
{
	if ((pgunionTableDefTBL == NULL) || (!UnionIsSharedMemoryInited(conMDLNameOfTableDefTBL)))
		return(0);
	else
		return(1);
}

int UnionGetMaxNumOfTableDef()
{
	return(conMaxNumOfTableDef);	
}

int UnionConnectTableDefTBL()
{
	if (UnionIsTableDefTBLConnected())	// 已经连接
		return(0);
	
	if ((pgunionTableDefMDL = UnionConnectSharedMemoryModule(conMDLNameOfTableDefTBL,
			sizeof(TUnionTableDefTBL))) == NULL)
	{
		UnionUserErrLog("in UnionConnectTableDefTBL:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgunionTableDefTBL = (PUnionTableDefTBL)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionTableDefMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectTableDefTBL:: PUnionTableDefTBL!\n");
		return(errCodeSharedMemoryModule);
	}
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionTableDefMDL))
	{
		return(UnionReloadTableDefTBL());
		return(1);
	}
	else
		return(0);
}


int UnionDisconnectTableDefTBL()
{
	pgunionTableDefTBL = NULL;
	return(UnionDisconnectShareModule(pgunionTableDefMDL));
}

int UnionRemoveTableDefTBL()
{
	UnionDisconnectTableDefTBL();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfTableDefTBL));
}


int UnionPrintTableDefTBLToFile(FILE *fp)
{
	int	i;
	int	ret;
	int	num = 0;
	char	tmpBuf[128];
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintTableDefTBLToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
		
	if ((ret = UnionConnectTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintTableDefTBLToFile:: UnionConnectTableDefTBL!\n");
		return(ret);
	}
	
	fprintf(fp,"%40s %40s %20s %8s %s\n","表名","表中文名","表类型","缓存方法","字段数量");
	fprintf(fp,"%.40s ", "-------------------------------------------------------------------");
	fprintf(fp,"%.40s ", "-------------------------------------------------------------------");
	fprintf(fp,"%.20s ", "-------------------------------------------------------------------");
	fprintf(fp,"%.8s ", "-------------------------------------------------------------------");
	fprintf(fp,"%.8s\n", "-------------------------------------------------------------------");
	for (i = 0; i < pgunionTableDefTBL->tableNum; i++)
	{
		/*
		if (UnionPrintTableDefToFile(&(pgunionTableDefTBL->rec[i]),fp) < 0)
			continue;
		*/
		
		memset(tmpBuf,0,sizeof(tmpBuf));
		if (pgunionTableDefTBL->rec[i].methodOfCached == 1)
			strcpy(tmpBuf,"个性化");
		else
			strcpy(tmpBuf,"标准");
		
		fprintf(fp,"%40s %40s %20s %8s %04d\n",
			pgunionTableDefTBL->rec[i].tableName,
			pgunionTableDefTBL->rec[i].tableChnName,
			pgunionTableDefTBL->rec[i].tableType,
			tmpBuf,
			pgunionTableDefTBL->rec[i].fieldNum);
		num++;
#ifndef _WIN32
		if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
#endif
	}
	fprintf(fp,"TableDefNum     = [%d]\n",num);
	return(0);
}

int UnionPrintTableDefToFile(PUnionTableDef pgunionTableDef,FILE *fp)
{
	int	i;
	int	ret;
	int	num = 0;
	
	if ((pgunionTableDef == NULL) || (fp == NULL))
	{
		UnionUserErrLog("in UnionPrintTableDefToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}

	if ((ret = UnionConnectTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintTableDefToFile:: UnionConnectTableDefTBL!\n");
		return(ret);
	}

	fprintf(fp,"tableName     = [%s]\n",pgunionTableDef->tableName);
	fprintf(fp,"%30s %30s %10s %4s %6s %4s %s\n","字段名","中文名","类型","大小","关键字","为空","默认值");

	fprintf(fp,"%.30s ", "----------------------------------");
	fprintf(fp,"%.30s ", "----------------------------------");
	fprintf(fp,"%.10s ", "----------------------------------");
	fprintf(fp,"%.4s ", "-----------------------------------");
	fprintf(fp,"%.6s ", "-----------------------------------");
	fprintf(fp,"%.4s ", "-----------------------------------");
	fprintf(fp,"%.6s\n", "----------------------------------");

	for (i = 0; i < pgunionTableDef->fieldNum; i++)
	{
		if (UnionPrintTableFieldDefToFile(&(pgunionTableDef->fieldDef[i]),fp) < 0)
			continue;
		num++;
#ifndef _WIN32
		if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
#endif
	}
	fprintf(fp,"FieldNum     = [%d]\n",num);

	return(0);
}

int UnionPrintTableFieldDefToFile(PUnionTableFieldDef pgunionTableFieldDef,FILE *fp)
{
	char	strType[32];
	char	strIsPrimaryKey[32];
	char	strIsNull[32];
	char	strFieldSize[32];
	
	if ((pgunionTableFieldDef == NULL) || (fp == NULL))
	{
		UnionUserErrLog("in UnionPrintTableFieldDefToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
	memset(strType,0,sizeof(strType));
	memset(strFieldSize,0,sizeof(strFieldSize));
	switch(pgunionTableFieldDef->fieldType)
	{
		case 1:
			strcpy(strType,"字符串");
			sprintf(strFieldSize,"%.f",pgunionTableFieldDef->fieldSize);
			break;
		case 2:
			strcpy(strType,"整型");
			sprintf(strFieldSize,"%.f",pgunionTableFieldDef->fieldSize);
			break;
		case 3:
			strcpy(strType,"浮点型");
			sprintf(strFieldSize,"%.1f",pgunionTableFieldDef->fieldSize);
			break;
		case 4:
			strcpy(strType,"二进制");
			sprintf(strFieldSize,"%.f",pgunionTableFieldDef->fieldSize);
			break;
		case 5:
			strcpy(strType,"布尔型");
			sprintf(strFieldSize,"%.f",pgunionTableFieldDef->fieldSize);
			break;
		case 6:
			strcpy(strType,"自增长");
			sprintf(strFieldSize,"%.f",pgunionTableFieldDef->fieldSize);
			break;
		case 7:
			strcpy(strType,"时间型");
			sprintf(strFieldSize,"%.f",pgunionTableFieldDef->fieldSize);
			break;
		case 99:
			strcpy(strType,"虚拟型");
			sprintf(strFieldSize,"%.f",pgunionTableFieldDef->fieldSize);
			break;
		default:
			sprintf(strType,"%d",pgunionTableFieldDef->fieldType);
			sprintf(strFieldSize,"%.f",pgunionTableFieldDef->fieldSize);
			break;
	}
	
	memset(strIsPrimaryKey,0,sizeof(strIsPrimaryKey));
	if (pgunionTableFieldDef->isPrimaryKey == 1)
		strcpy(strIsPrimaryKey,"是");
	else
		strcpy(strIsPrimaryKey,"否");

	memset(strIsNull,0,sizeof(strIsNull));
	if (pgunionTableFieldDef->isNull == 1)
		strcpy(strIsNull,"是");
	else
		strcpy(strIsNull,"否");
		
	fprintf(fp,"%30s %30s %10s %4s %6s %4s %s\n",
		pgunionTableFieldDef->fieldName,
		pgunionTableFieldDef->fieldChnName,
		strType,
		strFieldSize,
		strIsPrimaryKey,
		strIsNull,
		pgunionTableFieldDef->defaultValue);
	return(0);
}

int UnionPrintAllTableDefOfTableNameToFile(char *tableName,FILE *fp)
{
	int	i;
	int	ret;
	int	num = 0;
	
	if ((fp == NULL) || (tableName == NULL))
	{
		UnionUserErrLog("in UnionPrintAllTableDefOfTableNameToFile:: fp/tableName is NULL!\n");
		return(errCodeParameter);
	}
		
	if ((ret = UnionConnectTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAllTableDefOfTableNameToFile:: UnionConnectTableDefTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pgunionTableDefTBL->tableNum; i++)
	{
		if (strcmp(pgunionTableDefTBL->rec[i].tableName,tableName) != 0)
			continue;
		if (UnionPrintTableDefToFile(&(pgunionTableDefTBL->rec[i]),fp) < 0)
			continue;
		num++;
#ifndef _WIN32
		if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
#endif
	}
	fprintf(fp,"num = [%d]\n",num);
	return(num);
}

PUnionTableDef UnionFindTableDef(char *tableName)
{
	int	i;
	int	ret;
	
	if (tableName == NULL)
		return(NULL);
			
	if ((ret = UnionConnectTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionFindTableDef:: UnionConnectTableDefTBL!\n");
		return(NULL);
	}
	
	for (i = 0; i < pgunionTableDefTBL->tableNum; i++)
	{
#ifndef _WIN32
		if (strcasecmp(pgunionTableDefTBL->rec[i].tableName,tableName) == 0)
#else
		if (stricmp(pgunionTableDefTBL->rec[i].tableName,tableName) == 0)

#endif
		{
			return(&(pgunionTableDefTBL->rec[i]));
		}
	}
	UnionUserErrLog("in UnionFindTableDef:: UnionConnectTableDefTBL [%s] not found!\n", tableName);
	return(NULL);
}

PUnionTableDefTBL UnionGetTableDefTBL()
{
	int	ret;
	
	if ((ret = UnionConnectTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionGetTableDefTBL:: UnionConnectTableDefTBL!\n");
		return(NULL);
	}
	
	return(pgunionTableDefTBL);
}

int UnionPrintRecOfTableDefToFile(char *tableName,FILE *fp)
{
	PUnionTableDef	pgunionTableDef = NULL;
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintRecOfTableDefToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
	if ((pgunionTableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionUserErrLog("in UnionPrintRecOfTableDefToFile:: UnionFindTableDef[%s]!\n",tableName);
		return(errCodeDatabaseMDL_TableNotFound);
	}
	
	return(UnionPrintTableDefToFile(pgunionTableDef,fp));
}

int UnionTransferDBFieldNameToRealFieldName(char *tableName,char *dbFieldName,char *realFileName)
{
	int	i;
	PUnionTableDef	pgunionTableDef = NULL;
	
	if ((pgunionTableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionLog("in UnionTransferDBFieldNameToRealFieldName:: UnionFindTableDef[%s]!\n",tableName);
		return(errCodeDatabaseMDL_TableNotFound);
	}
	
	for (i = 0; i < pgunionTableDef->fieldNum; i++)
	{
#ifndef _WIN32
		if (strcasecmp(dbFieldName,pgunionTableDef->fieldDef[i].fieldName) == 0)
#else
		if (stricmp(dbFieldName,pgunionTableDef->fieldDef[i].fieldName) == 0)
#endif
		{
			strcpy(realFileName,pgunionTableDef->fieldDef[i].fieldName);
			return(0);
		}
	}
	UnionLog("in UnionTransferDBFieldNameToRealFieldName:: [%s.%s] not found!\n",tableName,dbFieldName);
	return(errCodeObjectMDL_FieldNotExist);
}

// 获取所有表字段清单
int UnionReadTableFieldListOfTableName(char *tableName,int tableFlag,char *tableFieldList)
{
	int		i;
	int		len = 0;
	PUnionTableDef	pgunionTableDef = NULL;
	
	if ((pgunionTableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionUserErrLog("in UnionReadTableFieldListOfTableName:: UnionFindTableDef[%s]!\n",tableName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	for (i = 0; i < pgunionTableDef->fieldNum; i++)
	{
		if (tableFlag)
			len += sprintf(tableFieldList + len,"%s.%s,",tableName,pgunionTableDef->fieldDef[i].fieldName);
		else	
			len += sprintf(tableFieldList + len,"%s,",pgunionTableDef->fieldDef[i].fieldName);
	}
	tableFieldList[len - 1] = 0;
	return(pgunionTableDef->fieldNum);
}

// 获取所有真实表字段清单
int UnionReadTableRealFieldListOfTableName(char *tableName,int tableFlag,char *tableFieldList)
{
	int		i;
	int		len = 0;
	int		fieldNum = 0;
	PUnionTableDef	pgunionTableDef = NULL;
	
	if ((pgunionTableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionUserErrLog("in UnionReadTableRealFieldListOfTableName:: UnionFindTableDef[%s]!\n",tableName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	for (i = 0; i < pgunionTableDef->fieldNum; i++)
	{
		// 虚拟字段
		if (pgunionTableDef->fieldDef[i].fieldType == 99)
			continue;
			
		if (tableFlag)
			len += sprintf(tableFieldList + len,"%s.%s,",tableName,pgunionTableDef->fieldDef[i].fieldName);
		else	
			len += sprintf(tableFieldList + len,"%s,",pgunionTableDef->fieldDef[i].fieldName);
		
		fieldNum ++;
	}
	tableFieldList[len - 1] = 0;
	return(fieldNum);
}

// 获取所有虚拟表字段清单
int UnionReadTableVirtualFieldListOfTableName(char *tableName,int tableFlag,char *tableFieldList)
{
	int		i;
	int		len = 0;
	int		fieldNum = 0;
	PUnionTableDef	pgunionTableDef = NULL;
	
	if ((pgunionTableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionUserErrLog("in UnionReadTableVirtualFieldListOfTableName:: UnionFindTableDef[%s]!\n",tableName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	for (i = 0; i < pgunionTableDef->fieldNum; i++)
	{
		// 虚拟字段
		if (pgunionTableDef->fieldDef[i].fieldType != 99)
			continue;
		
		if (tableFlag)
			len += sprintf(tableFieldList + len,"%s.%s,",tableName,pgunionTableDef->fieldDef[i].fieldName);
		else	
			len += sprintf(tableFieldList + len,"%s,",pgunionTableDef->fieldDef[i].fieldName);
		
		fieldNum ++;
	}
	tableFieldList[len - 1] = 0;
	return(fieldNum);
}

// 判断是否是虚拟字段
int UnionIsVirtualFieldOfTableName(char *tableName,char *fieldName)
{
	int			i;
	PUnionTableDef		pgunionTableDef = NULL;

	if((pgunionTableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionUserErrLog("in UnionIsVirtualFieldOfTableName:: tableName = [%s] not found in image!\n", tableName);
		return(0);
	}
	
	for (i = 0; i < pgunionTableDef->fieldNum; i++)
	{
#ifndef _WIN32
		if (strcasecmp(fieldName,pgunionTableDef->fieldDef[i].fieldName) == 0)
#else
		if (stricmp(fieldName,pgunionTableDef->fieldDef[i].fieldName) == 0)
#endif
		{
			if (pgunionTableDef->fieldDef[i].fieldType == 99)
				return(1);
			else
				return(0);
		}
	}
	return(0);
}

// 判断是否是主键
int UnionIsPrimaryKeyFieldOfTableName(char *tableName,char *fieldName)
{
	int			i;
	PUnionTableDef		pgunionTableDef = NULL;

	if((pgunionTableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionUserErrLog("in UnionIsPrimaryKeyFieldOfTableName:: tableName = [%s] not found in image!\n", tableName);
		return(0);
	}
	
	for (i = 0; i < pgunionTableDef->fieldNum; i++)
	{
#ifndef _WIN32
		if (strcasecmp(fieldName,pgunionTableDef->fieldDef[i].fieldName) == 0)
#else
		if (stricmp(fieldName,pgunionTableDef->fieldDef[i].fieldName) == 0)
#endif
		{
			if (pgunionTableDef->fieldDef[i].isPrimaryKey)
				return(1);
			else
				return(0);
		}
	}
	return(0);
}

// 判断字段是否存在
int UnionExistFieldOfTableName(char *tableName,char *fieldName)
{
	int			i;
	PUnionTableDef		pgunionTableDef = NULL;

	if((pgunionTableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionUserErrLog("in UnionExistFieldOfTableName:: tableName = [%s] not found in image!\n", tableName);
		return(0);
	}
	
	for (i = 0; i < pgunionTableDef->fieldNum; i++)
	{
#ifndef _WIN32
		if (strcasecmp(fieldName,pgunionTableDef->fieldDef[i].fieldName) == 0)
#else
		if (stricmp(fieldName,pgunionTableDef->fieldDef[i].fieldName) == 0)
#endif
		{
			return(1);
		}
	}
	return(0);
}

// 根据表名读取缓存方法,0：标准，1：个性化
int UnionReadMethodOfCachedOfTableName(char *tableName)
{
	PUnionTableDef		pgunionTableDef = NULL;
	
	if((pgunionTableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionUserErrLog("in UnionReadMethodOfCachedOfTableName:: tableName = [%s] not found in image!\n", tableName);
		return(errCodeDatabaseMDL_TableNotFound);
	}
	return(pgunionTableDef->methodOfCached);
}

int UnionGetTableSelectFieldFromImage(char *tableName, char *tableFileList)
{       
	int				offset, i;
	PUnionTableDef			pgunionTableDef = NULL;
	char				prefixFldName[64];
		
	if((pgunionTableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionAuditLog("in UnionGetTableSelectFieldFromImage:: tableName = [%s] not found in image!\n", tableName);
		return(errCodeDatabaseMDL_TableNotFound);
	}
	offset = 0;
	for(i = 0; i < pgunionTableDef->fieldNum; i++)
	{
		memset(prefixFldName, 0, sizeof(prefixFldName));
		UnionChargeFieldNameToDBFieldName((pgunionTableDef->fieldDef[i]).fieldName, prefixFldName);

		if(i == 0)
		{
			offset = sprintf(tableFileList, "%s", prefixFldName);
		}
		else
		{
			offset += sprintf(tableFileList + offset, ",%s", prefixFldName);
		}
	}
	return(0);
} 


int UnionReadTablePrimaryKeyGrpFromImage(char *tableName, char primaryKeyGrp[][64])
{
	int			i, primaryKeyNum;
	PUnionTableDef		pgunionTableDef = NULL;

	if((pgunionTableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionAuditLog("in UnionReadTablePrimaryKeyGrpFromImage:: tableName = [%s] not found in image!\n", tableName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	primaryKeyNum = 0;
	for(i = 0; i < pgunionTableDef->fieldNum; i++)
	{
		if((pgunionTableDef->fieldDef[i]).isPrimaryKey == 0)
		{
			continue;
		}
		if(primaryKeyNum  + 1 > conMaxFieldNumOfKeyGroup)
		{
			UnionAuditLog("in UnionReadTablePrimaryKeyGrpFromImage:: primaryKeyNum = [%d] is too many!\n", primaryKeyNum);
			return(errCodeParameter);
		}
		strcpy(primaryKeyGrp[primaryKeyNum], (pgunionTableDef->fieldDef[i]).fieldName);
		primaryKeyNum++;
	}

	//UnionProgramerLog("in UnionReadTablePrimaryKeyGrpFromImage:: tableName = [%s] primaryKeyFldNum = %d!\n", tableName, primaryKeyNum);
	return(primaryKeyNum);
}

int UnionFormTablePrimaryKeyList(char *tableName, char *primaryKeyList)
{
	int		i,primaryKeyNum,offset;
	char		prefixFldName[64];
	char		primaryKeyGrp[conMaxFieldNumOfKeyGroup][64];
	
	primaryKeyNum = UnionReadTablePrimaryKeyGrpFromImage(tableName, primaryKeyGrp);
	if (primaryKeyNum < 0)
	{
		UnionAuditLog("in UnionFormTablePrimaryKeyList:: primaryKeyNum = [%d] is too many!\n", primaryKeyNum);
		return(primaryKeyNum);
	}
	
	offset = 0;
	memset(prefixFldName, 0, sizeof(prefixFldName));
	for(i = 0; i < primaryKeyNum; i++)
	{
		UnionChargeFieldNameToDBFieldName(primaryKeyGrp[i], prefixFldName);
		if(i == 0)
		{
			offset = sprintf(primaryKeyList + offset, "%s", prefixFldName);
		}
		else
		{
			offset += sprintf(primaryKeyList + offset, ",%s", prefixFldName);
		}
	}

	//UnionProgramerLog("in UnionFormTablePrimaryKeyList:: tableName = [%s] primaryKeyFldNum = %d key = [%s]!\n", tableName, primaryKeyNum, primaryKeyList);
	return(primaryKeyNum);
}

int UnionReadTableFieldGrpFromImage(char *tableName, char fieldGrp[][64])
{
	int			i, fieldNum;
	PUnionTableDef		pgunionTableDef = NULL;

	if((pgunionTableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionAuditLog("in UnionReadTableFieldGrpFromImage:: tableName = [%s] not found in image!\n", tableName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	fieldNum = 0;
	for(i = 0; i < pgunionTableDef->fieldNum; i++)
	{
		if(fieldNum + 1 > conMaxNumOfTableFieldDef)
		{
			UnionAuditLog("in UnionReadTableFieldGrpFromImage:: fieldNum = [%d] is too many!\n", fieldNum);
			return(errCodeParameter);
		}
		strcpy(fieldGrp[fieldNum], (pgunionTableDef->fieldDef[i]).fieldName);
		fieldNum++;
	}

	//UnionProgramerLog("in UnionReadTableFieldGrpFromImage:: tableName = [%s] fieldNum = %d key = %s!\n", tableName, fieldNum, fieldGrp);
	return(fieldNum);
}

int UnionFormTableFieldList(char *tableName, char *fieldList)
{
	int	     i,fieldNum,offset;
	char	    prefixFldName[64];
	char	    fieldGrp[conMaxNumOfTableFieldDef][64];

	fieldNum = UnionReadTableFieldGrpFromImage(tableName, fieldGrp);
	if(fieldNum < 0)
	{
		UnionAuditLog("in UnionFormTableFieldList:: fieldNum = [%d] is too many!\n", fieldNum);
	       return(fieldNum);
	}

	offset = 0;
	memset(prefixFldName, 0, sizeof(prefixFldName));
	for(i = 0; i < fieldNum; i++)
	{
		UnionChargeFieldNameToDBFieldName(fieldGrp[i], prefixFldName);
		if(i == 0)
		{
			offset = sprintf(fieldList + offset, "%s", prefixFldName);
		}
		else
		{
			offset += sprintf(fieldList + offset, ",%s", prefixFldName);
		}
	}

	//UnionProgramerLog("in UnionFormTableFieldList:: tableName = [%s] fieldNum = %d key = %s!\n", tableName, fieldNum, fieldList);
	return(fieldNum);
}

int UnionReadTablePrimaryKeyFromImage(char *tableName, char *primaryKeyList)
{
	return(UnionFormTablePrimaryKeyList(tableName, primaryKeyList));
}


