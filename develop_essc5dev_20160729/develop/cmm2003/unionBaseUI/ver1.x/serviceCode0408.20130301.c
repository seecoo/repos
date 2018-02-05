//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-02-22

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
#include "unionVersion.h"
#include "inputAndOutputDataForDB.h"
#include "baseUIService.h"

/*********************************
�������:	0408
������:		�����ݿ���ɾ����
��������:	�����ݿ���ɾ����
**********************************/

int UnionDealServiceCode0408(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	int			fldNum = 0;
	char			tableName[64];
	char			*database;
	char			sql[128];
	PUnionTableDef		ptableDef = NULL;
	PUnionTableFieldDef	ptableFieldDef = NULL;
	
	// ����
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0408:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);
	}

	// ���ݿ�
	database = UnionGetDataBaseType();

	if ((ret = UnionOutputOneTableDataFromDB(tableName)) < 0)
		UnionLog("in UnionDealServiceCode0408:: UnionOutputOneTableDataFromDB error! ret = [%d]!\n",ret);
	else
		UnionLog("in UnionDealServiceCode0408:: UnionOutputOneTableDataFromDB success! �ܼ�¼��[%d]!\n",ret);

	snprintf(sql,sizeof(sql),"drop table %s",tableName);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0408:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
	
	// ɾ������
	if ((strcasecmp(database,"ORACLE") == 0) ||
		(strcasecmp(database,"DB2") == 0) ||
		(strcasecmp(database,"INFORMIX") == 0) ||
		(strcasecmp(database,"MYSQL") == 0))
	{
		if ((ptableDef = UnionFindTableDef(tableName)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCode0408:: UnionFindTableDef[%s]!\n",tableName);
			return(errCodeDatabaseMDL_TableNotFound);
		}
		for (fldNum = 0; fldNum < ptableDef->fieldNum; fldNum++)
		{
			ptableFieldDef = &ptableDef->fieldDef[fldNum];
			
			if (ptableFieldDef->fieldType == 6)	// ������
			{
				if (strcasecmp(database,"MYSQL") == 0)
					snprintf(sql,sizeof(sql),"delete from sequence where seqName = '%s_id_seq'",tableName);
				else
					snprintf(sql,sizeof(sql),"drop sequence %s_id_seq",tableName);
				if ((ret = UnionExecRealDBSql(sql)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode0408:: UnionExecRealDBSql[%s]!\n",sql);
					return(ret);
				}
				return(0);
			}
		}
	}
	return(0);		
}
