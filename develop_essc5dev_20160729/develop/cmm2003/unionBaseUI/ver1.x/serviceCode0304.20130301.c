//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "baseUIService.h"
#include "unionVersion.h"

/***************************************
服务代码:	0304
服务名:		查询记录
功能描述:	查询记录
***************************************/
int UnionRecurisiveQueryChildOrganization(char *condition,int sizeofCondition);

int UnionDealServiceCode0304(PUnionHsmGroupRec phsmGroupRec)
{
	int	i,j,k;
	int	len = 0;
	int	ret;
	int	isFound = 0;
	int	tableFlag = 0;
	int	tableNum = 0;
	int	fieldNum = 0;
	int	allTotalNum = 0;
	int	totalNum = 0;
	int	distinctFlag = 0;
	int	maxTableNum = 8;
	int	maxFieldNum = 64;
	char	tableName[128];
	char	tableGrp[maxTableNum][128];
	char	fieldValue[8192];
	char	fieldListChnName[4096];
	char	fieldList[1024];
	char	dbFieldList[maxTableNum][2048];
	char	joinCondition[1024];
	char	condition[204800];
	char	order_by[128];
	int	currentPage = 0;
	int	numOfPerPage = 0;
	char	tmpNum[32];
	char	sql[204800];
	char	fieldGrp[maxFieldNum][128];
	char	*ptr = NULL;
	
	PUnionTableDef          ptableDef = NULL;	

	// 读取表名
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0304:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(tableName);
	if (strlen(tableName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0304:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/tableName");
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// 拼分表定义串
	if ((tableNum = UnionSeprateVarStrIntoVarGrp(tableName,strlen(tableName),',',tableGrp,maxTableNum)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0304:: UnionSeprateVarStrIntoVarGrp[%s]!\n",tableName);
		return(tableNum);
	}
	/*
	if (tableNum > 2)
	{
		return(errCodeDatabaseMDL_MoreTable);
	}
	else 
	*/
	if (tableNum >= 2)
		tableFlag = 1;
	else
		tableFlag = 0;

	// 读取域清单
	memset(fieldList,0,sizeof(fieldList));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldList",fieldList,sizeof(fieldList))) < 0)
	{
		//UnionLog("in UnionDealServiceCode0304:: UnionReadRequestXMLPackageValue[%s]!\n","body/fieldList");
		//return(ret);
		strcpy(fieldList,"*");
	}
	
	UnionFilterHeadAndTailBlank(fieldList);
	if (strlen(fieldList) == 0)
		strcpy(fieldList,"*");

	// 检查并去掉distinct
	if (memcmp(fieldList,"distinct ",9) == 0)
	{
		distinctFlag = 1;
		memmove(fieldList,fieldList + 9,ret - 9);
		fieldList[ret - 9] = 0;
	}
	// 读取条件
	memset(condition,0,sizeof(condition));
	if ((ret = UnionReadRequestXMLPackageValue("body/condition",condition,sizeof(condition))) < 0)
	{
		//UnionLog("in UnionDealServiceCode0304:: UnionReadRequestXMLPackageValue[%s]!\n","body/condition");
		//return(ret);
	}
	UnionFilterHeadAndTailBlank(condition);
	
	// 读取多表JOIN条件
	memset(joinCondition,0,sizeof(joinCondition));
	if (tableNum > 1)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/joinCondition",joinCondition,sizeof(joinCondition))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0304:: UnionReadRequestXMLPackageValue[%s]!\n","body/condition");
			return(ret);
		}
	}
	// 读取order_by
	memset(order_by,0,sizeof(order_by));
	if ((ret = UnionReadRequestXMLPackageValue("body/order_by",order_by,sizeof(order_by))) < 0)
	{
		//UnionLog("in UnionDealServiceCode0304:: UnionReadRequestXMLPackageValue[%s]!\n","body/order_by");
		//return(ret);
	}
	// 读取当前页码
	if ((ret = UnionReadRequestXMLPackageValue("body/currentPage",tmpNum,sizeof(tmpNum))) < 0)
	{
		//UnionLog("in UnionDealServiceCode0304:: UnionReadRequestXMLPackageValue[%s]!\n","body/currentPage");
		//return(ret);
	}
	else
	{
		tmpNum[ret] = 0;
		currentPage = atoi(tmpNum);
	}
	
	// 读取当前页码
	if ((ret = UnionReadRequestXMLPackageValue("body/numOfPerPage",tmpNum,sizeof(tmpNum))) < 0)
	{
		//UnionLog("in UnionDealServiceCode0304:: UnionReadRequestXMLPackageValue[%s]!\n","body/numOfPerPage");
		//return(ret);
	}
	else
	{
		tmpNum[ret] = 0;
		numOfPerPage = atoi(tmpNum);
	}

	for (i = 0; i < tableNum; i++)
	{
		memset(dbFieldList[i],0,sizeof(dbFieldList[i]));
		if ((ret = UnionReadTableRealFieldListOfTableName(tableGrp[i],tableFlag,dbFieldList[i])) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0304:: UnionReadTableRealFieldListOfTableName[%s]!\n",tableGrp[i]);
			return(ret);
		}
	}
	// 检查字段清单是否为*
	len = 0;
	len += snprintf(sql + len,sizeof(sql) - len,"select ");
	
	if (memcmp(condition,"distinct",8) == 0)
	{	
		len += snprintf(sql + len,sizeof(sql) - len,"distinct ");	
		memset(condition,0,sizeof(condition));
		UnionLog("in UnionDealServiceCode0304:: sql = %s\n",sql);
	}

	if (fieldList[0] != '*')
	{
		// 拼分域定义串
		memset(fieldGrp,0,sizeof(fieldGrp));
		if ((fieldNum = UnionSeprateVarStrIntoVarGrp(fieldList,strlen(fieldList),',',fieldGrp,maxFieldNum)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0304:: UnionSeprateVarStrIntoVarGrp[%s]!\n",fieldList);
			return(fieldNum);
		}

		if (distinctFlag)
			len += snprintf(sql + len,sizeof(sql) - len,"distinct ");

		for (i = 0; i < fieldNum; i++)
		{
			for (j = 0; j < tableNum; j++)
			{
				if ((ptr = strchr(fieldGrp[i],'.')) == NULL)
					ptr = fieldGrp[i];
				else
				{
					if (memcmp(fieldGrp[i],tableGrp[j],strlen(tableGrp[j])) != 0)
						continue;
					else
						ptr += 1;
				}
				
				// 检查字段是否存在
				if (!UnionExistFieldOfTableName(tableGrp[j],ptr))
					continue;
				
				// 检查是否是虚拟字段
				if (UnionIsVirtualFieldOfTableName(tableGrp[j],ptr))
					continue;
				
				len += snprintf(sql + len,sizeof(sql) - len,"%s,",fieldGrp[i]);
				break;
			}
		}
		len -= 1;
	}
	else
	{
		for (i = 0; i < tableNum; i++)
			len += snprintf(sql + len,sizeof(sql) - len,"%s,",dbFieldList[i]);
		len -= 1;
	}
	
	len += snprintf(sql + len,sizeof(sql) - len," from %s",tableGrp[0]);
	
	// 拼装多表JOIN条件
	if (tableNum > 1)
		len += snprintf(sql + len,sizeof(sql) - len," %s",joinCondition);	
	
	// 拼装条件
	if (strlen(condition) > 0)
	{
		//if ((memcmp(tableGrp[0],"operationAudit",strlen(tableGrp[0])) == 0) &&
		//	((ptr = strstr(condition,"organization in")) != NULL))
		if ((ptr = strstr(condition,"{CurrentUserAndLowerOrganization_long}")) != NULL)
		{
			if ((ret = UnionRecurisiveQueryChildOrganization(condition,sizeof(condition))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0304:: UnionRecurisiveQueryChildOrganization!\n");	
				return(ret);
			}
		}
		len += snprintf(sql + len,sizeof(sql) - len," where %s",condition);
	}

	// 拼装order by
	if (strlen(order_by) > 0)
		len += snprintf(sql + len,sizeof(sql) - len," order by %s",order_by);

	// 查询总记录数
	if (currentPage >= 0)
	{
		if ((allTotalNum = UnionSelectRealDBRecordCounts(tableGrp[0],joinCondition,condition)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0304:: UnionSelectRealDBRecordCounts[%s][%s][%s]!\n",tableGrp[0],joinCondition,condition);
			return(allTotalNum);
		}
	}

	// 查询数据库
	if (allTotalNum > 0)
	{
		if ((ret = UnionSelectRealDBRecord(sql,currentPage,numOfPerPage)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0304:: UnionSelectRealDBRecord[%s]!\n",sql);
			return(ret);
		}
	}

	if (fieldList[0] == '*')
	{
		len = 0;
		for (i = 0; i < tableNum; i++)
			len += snprintf(fieldList + len,sizeof(fieldList) - len,"%s,",dbFieldList[i]);
		
		fieldList[len - 1] = 0;
	}

	// 赋值字段清单
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0304:: UnionSetResponseXMLPackageValue[%s]!\n","body/fieldList");
		return(ret);
	}
	
	// 拼分域定义串
	memset(fieldGrp,0,sizeof(fieldGrp));
	if ((fieldNum = UnionSeprateVarStrIntoVarGrp(fieldList,strlen(fieldList),',',fieldGrp,maxFieldNum)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0304:: UnionSeprateVarStrIntoVarGrp[%s]!\n",fieldList);
		return(fieldNum);
	}

	len = 0;
	// 获取字段中文名	
	memset(fieldListChnName,0,sizeof(fieldListChnName));
	for (i = 0; i < fieldNum; i++)
	{
		for (k = 0; k < tableNum; k++)
		{
			if ((ptableDef = UnionFindTableDef(tableGrp[k])) == NULL)
			{
				UnionUserErrLog("in UnionDealServiceCode0304::  UnionFindTableDef[%d][%s] not found!\n",k,tableGrp[k]);
				return(errCodeParameter);
			}
		
			isFound = 0;
			for (j = 0; j < ptableDef->fieldNum; j++)
			{
				UnionFilterHeadAndTailBlank(fieldGrp[i]);
				if ((ptr = strchr(fieldGrp[i],'.')) == NULL)
					ptr = fieldGrp[i];
				else
				{
					if (memcmp(fieldGrp[i],tableGrp[k],strlen(tableGrp[k])) != 0)
						break;
					else
						ptr += 1;
				}
					
				if (strcmp(ptr,ptableDef->fieldDef[j].fieldName) == 0)
				{
					len += snprintf(fieldListChnName + len,sizeof(fieldListChnName) - len,"%s,",ptableDef->fieldDef[j].fieldChnName);
					isFound = 1;
					break;
				}
			}
			
			if (isFound)
				break;
		}
	}
	
	fieldListChnName[len - 1] = '\0';

	// 赋值字段中文名
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0304:: UnionSetResponseXMLPackageValue[%s]!\n","body/fieldListChnName");
		return(ret);
	}

	// 赋值总记录数
	snprintf(tmpNum,sizeof(tmpNum),"%d",allTotalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/allTotalNum",tmpNum)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0304:: UnionSetResponseXMLPackageValue[%s]!\n","body/allTotalNum");
		return(ret);
	}

	memset(tmpNum,0,sizeof(tmpNum));
	if (allTotalNum > 0)
	{	
		// 读取记录数量
		if ((ret = UnionReadXMLPackageValue("totalNum", tmpNum, sizeof(tmpNum))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0304:: UnionReadXMLPackageValue[%s]!\n","totalNum");
			return(ret);
		}
		totalNum = atoi(tmpNum);
	}
	else
	{
		totalNum = 0;
		strcpy(tmpNum,"0");
	}
	
	// 赋值记录数量
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpNum)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0304:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
		return(ret);
	}

	UnionLog("in UnionDealServiceCode0304:: allTotalNum[%d] totalNum[%d]  sql[%s]!\n",allTotalNum,totalNum,sql);	

	for (i = 0; i < totalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0304:: UnionLocateXMLPackage!\n");
			return(ret);
		}
		
		if ((ret = UnionLocateResponseNewXMLPackage("body/detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0304:: UnionLocateResponseNewXMLPackage!\n");
			return(ret);
		}
		
		for (j = 0; j < fieldNum; j++)
		{
			UnionFilterHeadAndTailBlank(fieldGrp[j]);
			for (k = 0; k < tableNum; k++)
			{
				if ((ptr = strchr(fieldGrp[j],'.')) == NULL)
					ptr = fieldGrp[j];
				else
				{
					if (memcmp(fieldGrp[j],tableGrp[k],strlen(tableGrp[k])) != 0)
						continue;
					else
						ptr += 1;
				}
	
				// 检查字段是否存在
				if (!UnionExistFieldOfTableName(tableGrp[k],ptr))
				{
					//UnionSetResponseXMLPackageValue(fieldGrp[j],"");
					continue;
				}
				
				// 检查是否是虚拟字段
				if (UnionIsVirtualFieldOfTableName(tableGrp[k],ptr))
				{
					UnionSetResponseXMLPackageValue(fieldGrp[j],"");
					break;
				}

				memset(fieldValue,0,sizeof(fieldValue));
				if ((ret = UnionReadXMLPackageValue(ptr, fieldValue, sizeof(fieldValue))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode0304:: UnionReadXMLPackageValue[%d][%d][%s]!\n",i,j,ptr);
					UnionLogXMLPackage();
					return(ret);
				}
				if ((ret = UnionSetResponseXMLPackageValue(fieldGrp[j],fieldValue)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode0304:: UnionSetResponseXMLPackageValue[%d][%d][%s][%s]!\n",i,j,fieldGrp[j],fieldValue);
					UnionLogXMLPackage();
					return(ret);
				}
				break;
			}
		}
	}

	return(0);
}

/*
	功能： 当组织过多时，根据所给组织查询子组织
	返回值
		> 0	返回，带有递归查询的sql条件
		= 0	返回，不需要做递归查询
		< 0	返回，出错
*/
int UnionRecurisiveQueryChildOrganization(char *condition,int sizeofCondition)
{
	char	tmpBuf[2048000];
	char	sql[204800];
	int	ret = 0;
	int	len = 0;
	char	organizationName[128];
	char	*ptr = NULL;
	char	tmpCondition[128];
	int	allTotalNum = 0;
	char	userID[128];

	int	returnSql = 0;
	char	*database = NULL;

	database = UnionGetDataBaseType();

	ptr = strstr(condition,"{CurrentUserAndLowerOrganization_long}");

	if ((ret = UnionReadRequestXMLPackageValue("body/organizationName",organizationName,sizeof(organizationName))) < 0)
	{
		UnionLog("in UnionRecurisiveQueryChildOrganization:: UnionReadRequestXMLPackageValue[%s]!\n","body/organizationName");
		//读取用户
		if ((ret = UnionReadRequestXMLPackageValue("head/userID",userID,sizeof(userID))) < 0)
		{
			UnionUserErrLog("in UnionRecurisiveQueryChildOrganization:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
			return(ret);
		}

		// 查询用户的组织
		snprintf(sql,sizeof(sql),"select * from sysUser where userID = '%s'",userID);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionRecurisiveQueryChildOrganization:: UnionSelectRealDBRecord sql[%s]!\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionRecurisiveQueryChildOrganization:: count[0] sql[%s]!\n",sql);
			return(errCodeDatabaseMDL_RecordNotFound);
		}

		if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
		{
			UnionUserErrLog("in UnionRecurisiveQueryChildOrganization:: UnionLocateXMLPackage[%s]!\n","detail");
			return(ret);
		}
		
		// 获取组织
		if ((ret = UnionReadXMLPackageValue("organization",organizationName,sizeof(organizationName))) < 0)
		{
			UnionUserErrLog("in UnionRecurisiveQueryChildOrganization:: UnionReadXMLPackageValue[%s]!\n","organization");
			return(ret);
		}
		organizationName[ret] = 0;
	}
	organizationName[ret] = 0;

	// 查询是否有子组织,没有则无需递归查询
	snprintf(tmpCondition,sizeof(tmpCondition),"higherOrganization = '%s'",organizationName);
	if ((allTotalNum = UnionSelectRealDBRecordCounts("organization",NULL,tmpCondition)) < 0)
	{
		UnionUserErrLog("in UnionRecurisiveQueryChildOrganization:: UnionSelectRealDBRecordCounts tableName[%s] condition[%s]!\n","organization",tmpCondition);
		return(allTotalNum);
	}
	if (allTotalNum == 0)
	{
		snprintf(sql,sizeof(sql),"'%s'",organizationName);
	}
	else if (allTotalNum > 0)
	{
		if (strcasecmp(database,"ORACLE") == 0)
			returnSql = 1;
		else
			returnSql = 0;
	
		if ((ret = UnionSelectRealDBTree(returnSql,"organization","organizationName","higherOrganization",1,organizationName,sql,sizeof(sql))) < 0)
		{
			UnionUserErrLog("in UnionRecurisiveQueryChildOrganization:: UnionSelectRealDBTree!\n");
			return(ret);
		}
	}

	// 拼装条件
	len = ptr-condition;
	memcpy(tmpBuf,condition,len);
	len += snprintf(tmpBuf+len,sizeof(tmpBuf) - len," (%s) %s",sql,ptr + strlen("{CurrentUserAndLowerOrganization_long}"));
	
	if (len >= sizeofCondition)
	{
		UnionUserErrLog("in UnionRecurisiveQueryChildOrganization:: len[%d] > sizeofCondition[%d]!\n",len,sizeofCondition);
		return(errCodeParameter);
	}

	strcpy(condition,tmpBuf);
	condition[len] = 0;
	return 1;
}
