//	zhang yong ding
//	2014/07/06
//	Version 1.0

#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_	
#endif

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_	
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_	
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "unionModule.h"
#include "UnionEnv.h"
#include "UnionStr.h"
#include "unionCommand.h"
#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"
#include "UnionLog.h"

#include "unionErrCode.h"
#include "unionTableDef.h"
#include "unionTableData.h"

//PUnionSharedMemoryModule	pgunionTableDataMDL = NULL;
//PUnionTableDataTBL		pgunionTableDataTBL = NULL;

#define		maxTableDataTBLNum	128

int				gunionCurrentTableDataTBLNum = 0;
char				gunionTableName[maxTableDataTBLNum][32+1];
PUnionSharedMemoryModule	pgunionTableDataMDL[maxTableDataTBLNum];
PUnionTableDataTBL		pgunionTableDataTBL[maxTableDataTBLNum];

int UnionIsTableDataTBLConnected(char *tableName)
{
	int	i;

	for (i = 0; i < gunionCurrentTableDataTBLNum; i++)
	{
		if (strcmp(tableName,gunionTableName[i]) == 0)
			return(i);
	}
	// UnionLog("in UnionIsTableDataTBLConnected:: tableName[%s] is not connect\n",tableName);
	return(-1);
}

int UnionGetMDLNameOfTableDataTBL(char *tableName,char *mdlName)
{
	return(sprintf(mdlName,"TABLE_%s_MDL",tableName));
}

int UnionConnectTableDataTBL(char *tableName)
{
	//int	i;
	int	ret;
	int	index = gunionCurrentTableDataTBLNum;
	char	mdlName[128];
	
	if ((ret = UnionIsTableDataTBLConnected(tableName)) >= 0)	// 已经连接
		return(ret);
	
	/*
	if ((num = UnionSelectRealDBRecordCounts(tableName,NULL,NULL)) < 0)
	{
		UnionUserErrLog("in UnionConnectTableDataTBL:: UnionSelectRealDBRecordCounts [%d]\n",num);
		return(num);
	}
	
	//UnionLog("in UnionConnectTableDataTBL:: num[%d]\n",num);
	for (i = 0; i < gunionCurrentTableDataTBLNum; i++)
	{
		if (strecmp(tableName,gunionTableName[i]) == 0)
		{
			index = i;
			if (pgunionTableDataTBL[i]->num != num)
				UnionRemoveTableDataTBL(tableName);
			break;
		}
	}
	*/
	
	snprintf(gunionTableName[index],sizeof(gunionTableName[index]),"%s",tableName);
	
	if (index >= maxTableDataTBLNum)
	{
		UnionUserErrLog("in UnionConnectTableDataTBL:: maxTableDataTBLNum[%d] is too short\n",maxTableDataTBLNum);
		return(errCodeParameter);
	}
	ret = UnionGetMDLNameOfTableDataTBL(tableName,mdlName);
	mdlName[ret] = 0;
	
	pgunionTableDataTBL[index] = NULL;
	
	if ((pgunionTableDataMDL[index] = UnionConnectSharedMemoryModule(mdlName,sizeof(TUnionTableDataTBL))) == NULL)
	{
		UnionLog("in UnionConnectTableDataTBL:: UnionConnectSharedMemoryModule!\n");
		index = ret;
		goto returnTag;
		//return(errCodeSharedMemoryModule);
	}

	if ((pgunionTableDataTBL[index] = (PUnionTableDataTBL)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionTableDataMDL[index])) == NULL)
	{
		UnionLog("in UnionConnectTableDataTBL:: UnionGetAddrOfSharedMemoryModuleUserSpace!\n");
		index = ret;
		goto returnTag;
		//return(errCodeSharedMemoryModule);
	}

	if (UnionIsNewCreatedSharedMemoryModule(pgunionTableDataMDL[index]))
	{
		UnionReloadTableDataTBL(tableName);
	}
returnTag:
	if (index == gunionCurrentTableDataTBLNum)
		gunionCurrentTableDataTBLNum ++;
	//UnionLog("in UnionConnectTableDataTBL:: gunionCurrentTableDataTBLNum[%d]\n",gunionCurrentTableDataTBLNum);
	return(index);
}

int UnionDisconnectTableDataTBL(char *tableName)
{
	int	i;
	int	ret;
	char	mdlName[128];
	
	ret = UnionGetMDLNameOfTableDataTBL(tableName,mdlName);
	mdlName[ret] = 0;
	
	for (i = 0; i < gunionCurrentTableDataTBLNum; i++)
	{
		if (strcmp(tableName,gunionTableName[i]) == 0)
		{
			pgunionTableDataTBL[i] = NULL;
			return(UnionDisconnectShareModule(pgunionTableDataMDL[i]));
		}
	}
	return(0);
}

int UnionRemoveTableDataTBL(char *tableName)
{
	int	ret;
	char	mdlName[128+1];
	
	ret = UnionGetMDLNameOfTableDataTBL(tableName,mdlName);
	mdlName[ret] = 0;

	UnionDisconnectTableDataTBL(tableName);
	return(UnionRemoveSharedMemoryModule(mdlName));
}

int UnionReloadTableDataTBL(char *tableName)
{
	int	i,j,k;
	int	ret = 0;
	unsigned long	pos;
	int	offset;
	int	len;
	int	num;
	int	index;
	int	fieldNum = 0;
	char	sql[2048];
	char	fieldList[512];
	char	fieldGrp[64][128];
	char	fldValue[4096];
	char	dataStr[8192];
	char	hashData[128];
	int	primaryKeyNum;
	char	primaryKeyGrp[conMaxFieldNumOfKeyGroup][64];

	if ((index = UnionConnectTableDataTBL(tableName)) < 0)
	{
		UnionUserErrLog("in UnionReloadTableDataTBL:: UnionConnectTableDataTBL ret = %d!\n", ret);
		return(index);
	}
	
	if (pgunionTableDataTBL[index] == NULL)
	{
		UnionUserErrLog("in UnionReloadTableDataTBL:: table[%s] is not shared memory!\n",tableName);
		return(errCodeParameter);
	}
	
	if ((primaryKeyNum = UnionReadTablePrimaryKeyGrpFromImage(tableName, primaryKeyGrp)) < 0)
	{
		UnionAuditLog("in UnionReloadTableDataTBL:: UnionReadTablePrimaryKeyGrpFromImage!\n");
		return(primaryKeyNum);
	}

	memset(fieldList,0,sizeof(fieldList));
	if ((ret = UnionReadTableRealFieldListOfTableName(tableName,0,fieldList)) < 0)
	{
		UnionUserErrLog("in UnionReloadTableDataTBL:: UnionReadTableRealFieldListOfTableName!\n");
		return(ret);
	}
	
	if ((fieldNum = UnionSeprateVarStrIntoArray(fieldList,strlen(fieldList),',',(char *)&fieldGrp,64,128)) < 0)
	{
		UnionUserErrLog("in UnionReloadTableDataTBL:: UnionSeprateVarStrIntoArray[%s]\n",fieldList);
		return(fieldNum);
	}
	
	if (strcasecmp(tableName,"oldServiceFiled") == 0)
		len = sprintf(sql,"select %s from %s order by oldServiceCode,version,flag,seqNo",fieldList,tableName);
	else
		len = sprintf(sql,"select %s from %s",fieldList,tableName);
	sql[len] = 0;
	if ((num = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionReloadTableDataTBL:: UnionSelectRealDBRecord\n");
		return (num);
	}

	if ((pgunionTableDataTBL[index]->num = num) > maxNumOfTableData)
	{
		UnionUserErrLog("in UnionReloadTableDataTBL:: data num is too more!\n");
		return(errCodeDatabaseMDL_MoreRecordFound);
	}
	
	if (num > USE_HASH_NUM)
		pgunionTableDataTBL[index]->hashflag = 1;
	else
		pgunionTableDataTBL[index]->hashflag = 0;
	
	for (i = 0; i < maxNumOfTableData; i++)
		pgunionTableDataTBL[index]->tableData[i].id = -1;
	
	for (i = 0; i < num; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionReloadTableDataTBL:::: UnionLocateXMLPackage\n");
			continue;
		}
		
		offset = 0;
		len = 0;
		for (j = 0; j < fieldNum; j++)
		{
			if ((ret = UnionReadXMLPackageValue(fieldGrp[j],fldValue,sizeof(fldValue))) < 0)
			{
				UnionUserErrLog("in UnionReloadTableDataTBL:: UnionPutRecFldIntoRecStr [%s]!\n",fieldGrp[j]);
				return(ret);
			}
			fldValue[ret] = 0;
			if ((ret = UnionPutRecFldIntoRecStr(fieldGrp[j],fldValue,ret,dataStr+offset,sizeof(dataStr) - offset - 1)) < 0)
			{
				UnionUserErrLog("in UnionReloadTableDataTBL:: UnionPutRecFldIntoRecStr [%s]!\n",fieldGrp[j]);
				return(ret);
			}
			offset += ret;
			for (k = 0; k < primaryKeyNum; k++)
			{
				if (strcasecmp(primaryKeyGrp[k],fieldGrp[j]) == 0)
					len += snprintf(hashData + len,sizeof(hashData) - len,"%s",fldValue);
			}
		}
		dataStr[offset] = 0;
		
		if (pgunionTableDataTBL[index]->hashflag)
		{
			pos = UnionHashTableHashString(hashData, 0);
			pos %= maxNumOfTableData;
			
			for (k = 0;; k++)
			{
				if (k == maxNumOfTableData)
				{
					UnionUserErrLog("in UnionReloadTableDataTBL:: data num is too more!\n");
					return(errCodeDatabaseMDL_MoreRecordFound);
				}
				
				if (pgunionTableDataTBL[index]->tableData[pos].id == -1)
					break;
				else
					pos = (pos + 1) % maxNumOfTableData;
			}
	
			pgunionTableDataTBL[index]->tableData[pos].id = UnionHashTableHashString(hashData, 1);
			strcpy(pgunionTableDataTBL[index]->tableData[pos].key,hashData);
			pgunionTableDataTBL[index]->tableData[pos].len = offset;
			memcpy(pgunionTableDataTBL[index]->tableData[pos].value,dataStr,offset + 1);
		}
		else
		{
			pgunionTableDataTBL[index]->tableData[i].id = i + 1;
			strcpy(pgunionTableDataTBL[index]->tableData[i].key,hashData);
			pgunionTableDataTBL[index]->tableData[i].len = offset;
			memcpy(pgunionTableDataTBL[index]->tableData[i].value,dataStr,offset + 1);
		}
		
	}

	return(num);
}

int UnionPrintTableDataTBLToFile(char *tableName,FILE *fp)
{
	int	i;
	int	index;
	int	num = 0;
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintTableDataTBLToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
		
	if ((index = UnionConnectTableDataTBL(tableName)) < 0)
	{
		UnionUserErrLog("in UnionPrintTableDataTBLToFile:: UnionConnectTableDataTBL!\n");
		return(index);
	}
	
	if (pgunionTableDataTBL[index] == NULL)
	{
		UnionUserErrLog("in UnionPrintTableDataTBLToFile:: table[%s] is not shared memory!\n",tableName);
		return(errCodeParameter);
	}

	if (pgunionTableDataTBL[index] != NULL)
	{
		for (i = 0; i < maxNumOfTableData; i++)
		{
			if (pgunionTableDataTBL[index]->tableData[i].id == -1)
				continue;
			
			fprintf(fp,"ID      :[%20lu]\nKEY     :[%s]\nVALUE   :[%4d][%s]\n\n",
				pgunionTableDataTBL[index]->tableData[i].id,
				pgunionTableDataTBL[index]->tableData[i].key,
				pgunionTableDataTBL[index]->tableData[i].len,
				pgunionTableDataTBL[index]->tableData[i].value);
			num++;
#ifndef _WIN32
			if ((num != 0) && (num % 10 == 0) && ((fp == stdout) || (fp == stderr)))
			{
				if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
					break;
			}
		}
#endif
		fprintf(fp,"TableDataNum     = [%d]\n",num);
	}
	else
		fprintf(fp,"[%s]共享内存中不存在\n",tableName);
	return(0);
}

/*
int UnionPrintTableDataToFile(PUnionTableDataTBL ptableDataTBL,FILE *fp)
{
	int	i;
	int	num = 0;
	
	if ((ptableDataTBL == NULL) || (fp == NULL))
	{
		UnionUserErrLog("in UnionPrintTableDataToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}

	for (i = 0; i < maxNumOfTableData; i++)
	{
		if (ptableDataTBL->tableData[i].id == -1)
			continue;
		fprintf(fp,"ID      :[%20lu]\nKEY     :[%s]\nVALUE   :[%4d][%s]\n\n",
			ptableDataTBL->tableData[i].id,
			ptableDataTBL->tableData[i].key,
			ptableDataTBL->tableData[i].len,
			ptableDataTBL->tableData[i].value);
		num++;
#ifndef _WIN32
		if ((num != 0) && (num % 10 == 0) && ((fp == stdout) || (fp == stderr)))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
#endif
	}
	fprintf(fp,"TalbeDataNum     = [%d]\n",num);

	return(0);
}

PUnionTableDataTBL UnionGetTableDataTBL(char *tableName)
{
	int	ret;
	
	if ((ret = UnionConnectTableDataTBL(tableName)) < 0)
	{
		UnionUserErrLog("in UnionGetTableDataTBL:: UnionConnectTableDataTBL!\n");
		return(NULL);
	}
	
	return(pgunionTableDataTBL[ret]);
}

int UnionPrintRecOfTableDataToFile(char *tableName,FILE *fp)
{
	PUnionTableDataTBL	ptableDataTBL = NULL;
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintRecOfTableDataToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	
	if ((ptableDataTBL = UnionGetTableDataTBL(tableName)) == NULL)
	{
		UnionUserErrLog("in UnionPrintRecOfTableDataToFile:: UnionGetTableDataTBL[%s]!\n",tableName);
		return(errCodeParameter);
	}
	
	return(UnionPrintTableDataToFile(ptableDataTBL,fp));
}
*/

char *UnionFindTableValue(char *tableName, char *primaryKeyData)
{
	int		i;
	int		index;
	unsigned long	pos;
	unsigned long	id;
	unsigned long	startPos;
	unsigned long	currPos;

	if ((index = UnionConnectTableDataTBL(tableName)) < 0)
	{
		UnionUserErrLog("in UnionFindTableValue:: UnionConnectTableDataTBL!\n");
		return(NULL);
	}
	
	if (pgunionTableDataTBL[index] == NULL)
	{
		// modified 2015-04-23
		//UnionUserErrLog("in UnionFindTableValue:: table[%s] is not shared memory!\n",tableName);
		UnionLog("in UnionFindTableValue:: table[%s] is not shared memory!\n",tableName);
		return(NULL);
	}


	if (!pgunionTableDataTBL[index]->hashflag)
	{
		for (i = 0; i < pgunionTableDataTBL[index]->num; i++)
		{
			if (strcmp(pgunionTableDataTBL[index]->tableData[i].key,primaryKeyData) == 0)
			{
				return(pgunionTableDataTBL[index]->tableData[i].value);
			}
		}
		return(NULL);
	}
	
	pos = UnionHashTableHashString( primaryKeyData, 0 );
	id = UnionHashTableHashString( primaryKeyData, 1 );
	startPos = pos % maxNumOfTableData;
	currPos = startPos;

	while (pgunionTableDataTBL[index]->tableData[currPos].id != -1)
	{
		/*如果仅仅是判断在该表中时候存在这个字符串，就比较这两个hash值就可以了，不用对
			*结构体中的字符串进行比较。这样会加快运行的速度？减少hash表占用的空间？这种
		*方法一般应用在什么场合？*/
		if (pgunionTableDataTBL[index]->tableData[currPos].id == id)
		{
			return(pgunionTableDataTBL[index]->tableData[currPos].value);
		}
		else
			currPos = (currPos + 1) % maxNumOfTableData;
		if (currPos == startPos)
			break;
	}
	return(NULL);
}

int UnionIsExistTableValue(char *tableName)
{
	int		index;

	if ((index = UnionConnectTableDataTBL(tableName)) < 0)
	{
		UnionLog("in UnionIsExistTableValue:: UnionConnectTableDataTBL!\n");
		return(0);
	}
	
	if (pgunionTableDataTBL[index] == NULL)
	{
		UnionLog("in UnionIsExistTableValue:: table[%s] is not shared memory!\n",tableName);
		return(0);
	}
	return(1);
}

