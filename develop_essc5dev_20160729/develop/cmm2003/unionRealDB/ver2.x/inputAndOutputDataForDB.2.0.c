//	张永定
//	2012-12-27

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "unionRealDBCommon.h"
#include "unionRealBaseDB.h"
#include "unionTableDef.h"
#include "unionXMLPackage.h"
#include "inputAndOutputDataForDB.h"

int UnionInputOneTableDataToDB_oldVersion(char *tableName,char *prefix,int *successNum, int *failNum)
{
	int	i,j;
	int	ret;
	int	iCnt;
	int	lenOfFiled = 0;
	int	lenOfValue = 0;
	int	sNum = 0;
	int	fNum = 0;
	int	fieldNum = 0;
	char	fieldList[2048+1];
	char	valueList[409600+1];
	char	value[8192+1];
	char	tmpBuf[8192+1];
	char	sql[819200+1];
	char	fieldGrp[128][128];
	char	fileName[512+1];
	char	tmpFieldName[128+1];
	PUnionTableDef	ptableDef = NULL;

	memset(tmpBuf,0,sizeof(tmpBuf));
	strcpy(tmpBuf,tableName);
	//UnionToLowerCase(tmpBuf);
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/database/%s.xml",getenv("UNIONREC"),tmpBuf);

	if ((ptableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionUserErrLog("in UnionInputOneTableDataToDB:: UnionFindTableDef[%s] failure!\n",tableName);
		return(-1);
	}
	
	memset(fieldList,0,sizeof(fieldList));
	if ((ret = UnionReadTableFieldListOfTableName(tableName,0,fieldList)) < 0)
	{
		UnionUserErrLog("in UnionInputOneTableDataToDB:: UnionReadTableFieldListOfTableName[%s] failure!\n",tableName);
		return(ret);
	}

	if ((fieldNum = UnionSeprateVarStrIntoVarGrp(fieldList, strlen(fieldList),',',fieldGrp,128)) < 0)
	{
		UnionUserErrLog("in UnionInputOneTableDataToDB:: UnionSeprateVarStrIntoVarGrp[%s] failure!\n",fieldList);
		return(fieldNum);
	}
	
	if (access(fileName,0) != 0)
	{
		UnionUserErrLog("in UnionInputOneTableDataToDB:: fileName[%s] 不存在!\n",fileName);
		return(0);
	}

	// 初始化XML文件
	if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionInputOneTableDataToDB:: UnionInitXMLPackage[%s] failure!\n",fileName);
		return(ret);
	}
	
	// 读取总数量
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionInputOneTableDataToDB:: UnionReadXMLPackageValue[%s]!\n","totalNum");
		return(ret);
	}
	iCnt = atoi(tmpBuf);
	
	sNum = 0;
	fNum = 0;
	for(i = 0; i < iCnt; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionInputOneTableDataToDB:: UnionLocateXMLPackage!\n");
			failNum ++;
			continue;
		}
		
		//memset(fieldList,0,sizeof(fieldList));
		//memset(valueList,0,sizeof(valueList));
		lenOfFiled = 0;
		lenOfValue = 0;
		for (j = 0; j < fieldNum; j++)
		{
			memset(tmpBuf,0,sizeof(tmpBuf));
			memset(tmpFieldName,0,sizeof(tmpFieldName));
			sprintf(tmpFieldName,"%s%s",prefix,fieldGrp[j]);
			if (UnionReadXMLPackageValue(tmpFieldName, tmpBuf, sizeof(tmpBuf)) < 0)
			{
				continue;
			}
			
			memset(value,0,sizeof(value));
			UnionTranslateSpecCharInDBStr(tmpBuf,strlen(tmpBuf),value,sizeof(value));

			//lenOfFiled += sprintf(fieldList + lenOfFiled,"%s,",fieldGrp[j]);
			
			switch(ptableDef->fieldDef[j].fieldType)
			{
				case 1 :	// 字符串
				case 4 :	// 二进制
				case 7 :	// 时间
					lenOfFiled += sprintf(fieldList + lenOfFiled,"%s,",fieldGrp[j]);
					lenOfValue += sprintf(valueList + lenOfValue,"'%s',",value);
					break;
				default:
					if (strlen(value) > 0)
					{
						lenOfFiled += sprintf(fieldList + lenOfFiled,"%s,",fieldGrp[j]);
						lenOfValue += sprintf(valueList + lenOfValue,"%s,",value);
					}
					break;
			}
		}
		
		fieldList[lenOfFiled - 1] = 0;
		valueList[lenOfValue - 1] = 0;
		
		//memset(sql,0,sizeof(sql));
		ret = sprintf(sql,"insert into %s(%s) values(%s)",tableName,fieldList,valueList);
		sql[ret] = 0;
		
		if ((ret = UnionExecRealDBSql2(0,sql)) < 0)
		{
			UnionUserErrLog("in UnionInputOneTableDataToDB:: UnionExecRealDBSql2[%s]!\n",sql);
			fNum ++;
			continue;
		}
		sNum ++;
	}
	*successNum = sNum;
	*failNum = fNum;
	return(iCnt);
}

int UnionInputOneTableDataToDB(char *tableName,int *successNum, int *failNum)
{
	int	i,j;
	int	ret;
	int	iCnt;
	int	lenOfFiled = 0;
	int	lenOfValue = 0;
	int	sNum = 0;
	int	fNum = 0;
	int	fieldNum = 0;
	char	fieldList[2048+1];
	char	valueList[409600+1];
	char	value[8192+1];
	char	tmpBuf[8192+1];
	char	sql[819200+1];
	char	fieldGrp[128][128];
	char	fileName[512+1];
	PUnionTableDef	ptableDef = NULL;

	memset(tmpBuf,0,sizeof(tmpBuf));
	strcpy(tmpBuf,tableName);
	//UnionToLowerCase(tmpBuf);
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/database/%s.xml",getenv("UNIONREC"),tmpBuf);

	if ((ptableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionUserErrLog("in UnionInputOneTableDataToDB:: UnionFindTableDef[%s] failure!\n",tableName);
		return(-1);
	}
	
	UnionSetDatabaseIndex(UnionGetDBIndexByTableName(tableName));
	
	memset(fieldList,0,sizeof(fieldList));
	if ((ret = UnionReadTableRealFieldListOfTableName(tableName,0,fieldList)) < 0)
	{
		UnionUserErrLog("in UnionInputOneTableDataToDB:: UnionReadTableRealFieldListOfTableName[%s] failure!\n",tableName);
		return(ret);
	}

	if ((fieldNum = UnionSeprateVarStrIntoVarGrp(fieldList, strlen(fieldList),',',fieldGrp,128)) < 0)
	{
		UnionUserErrLog("in UnionInputOneTableDataToDB:: UnionSeprateVarStrIntoVarGrp[%s] failure!\n",fieldList);
		return(fieldNum);
	}
	
	if (access(fileName,0) != 0)
	{
		UnionUserErrLog("in UnionInputOneTableDataToDB:: fileName[%s] 不存在!\n",fileName);
		return(0);
	}

	// 初始化XML文件
	if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionInputOneTableDataToDB:: UnionInitXMLPackage[%s] failure!\n",fileName);
		return(ret);
	}
	
	// 读取总数量
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionInputOneTableDataToDB:: UnionReadXMLPackageValue[%s]!\n","totalNum");
		return(ret);
	}
	iCnt = atoi(tmpBuf);
	
	sNum = 0;
	fNum = 0;
	for(i = 0; i < iCnt; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionInputOneTableDataToDB:: UnionLocateXMLPackage!\n");
			failNum ++;
			continue;
		}

		
		//memset(fieldList,0,sizeof(fieldList));
		//memset(valueList,0,sizeof(valueList));
		lenOfFiled = 0;
		lenOfValue = 0;
		for (j = 0; j < fieldNum; j++)
		{

			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((ret = UnionReadXMLPackageValue(fieldGrp[j], tmpBuf, sizeof(tmpBuf))) < 0)
			{
				continue;
			}

			memset(value,0,sizeof(value));
			UnionTranslateSpecCharInDBStr(tmpBuf,strlen(tmpBuf),value,sizeof(value));

			//lenOfFiled += sprintf(fieldList + lenOfFiled,"%s,",fieldGrp[j]);
			
			switch(ptableDef->fieldDef[j].fieldType)
			{
				case 1 :	// 字符串
				case 4 :	// 二进制
				case 7 :	// 时间
					if (strlen(value) > 0)
					{
						lenOfFiled += sprintf(fieldList + lenOfFiled,"%s,",fieldGrp[j]);
						lenOfValue += sprintf(valueList + lenOfValue,"'%s',",value);
					}
					break;
				default:
					if (strlen(value) > 0)
					{
						lenOfFiled += sprintf(fieldList + lenOfFiled,"%s,",fieldGrp[j]);
						lenOfValue += sprintf(valueList + lenOfValue,"%s,",value);
					}
					break;
			}
		}
		
		fieldList[lenOfFiled - 1] = 0;
		valueList[lenOfValue - 1] = 0;
		
		//memset(sql,0,sizeof(sql));
		ret = sprintf(sql,"insert into %s(%s) values(%s)",tableName,fieldList,valueList);
		sql[ret] = 0;
		
		if ((ret = UnionExecRealDBSql2(0,sql)) < 0)
		{
			UnionUserErrLog("in UnionInputOneTableDataToDB:: UnionExecRealDBSql2[%s] ret[%d]!\n",sql,ret);
			fNum ++;
			continue;
		}
		sNum ++;
	}
	*successNum = sNum;
	*failNum = fNum;
	return(iCnt);
}

int UnionOutputOneTableDataFromDB(char *tableName)
{
	int	ret;
	int	iCnt = 0;
	char	fileName[256+1];
	char	tmpBuf[256+1];
	char	sql[1024+1];

	memset(tmpBuf,0,sizeof(tmpBuf));
	strcpy(tmpBuf,tableName);
	//UnionToLowerCase(tmpBuf);
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/database/%s.xml",getenv("UNIONREC"),tmpBuf);
	
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select * from %s",tableName);

	UnionSetDatabaseIndex(UnionGetDBIndexByTableName(tableName));
	
	if ((iCnt =  UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("UnionSelectRealDBRecord failure! ret = [%d]\n",iCnt);
		return(iCnt);
	}

	UnionLog("in UnionOutputOneTableDataFromDB:: output begin iCnt[%d]\n",iCnt);
	if (iCnt > 0)
	{
		if (access(fileName,0) == 0)
		{
			memset(tmpBuf,0,sizeof(tmpBuf));
			sprintf(tmpBuf,"%s.%s",fileName,UnionGetCurrentFullSystemDateTime());
			rename(fileName,tmpBuf);
		}
	
		if ((ret = UnionXMLPackageToFile(fileName)) < 0)
		{
			UnionUserErrLog("UnionXMLPackageToFile failure! fileName = [%s]\n",fileName);
			return(ret);
		}
	}

	UnionLog("in UnionOutputOneTableDataFromDB:: output end iCnt[%d]\n",iCnt);
	return(iCnt);
}
