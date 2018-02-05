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
#include <dirent.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <math.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "unionVersion.h"
#include "baseUIService.h"

/*********************************
�������:	0406
������:		�ȶ������ֵ�
��������:	�ȶ������ֵ�
**********************************/

int UnionDealServiceCode0406(PUnionHsmGroupRec phsmGroupRec)
{
	int	i = 0;
	int	j = 0;
	int	ret = 0;
	int	len = 0;
	int	dbTableNum = 0;
	int	dbFieldNum = 0;
	int	tableNum = 0;
	int	maxTableNum = 128;
	char	tableName[2048];
	char	tableGrp[maxTableNum][128];
	char	dbTableName[128][64];
	char	dbFieldName[512];	
	char	tmpBuf[128];
	char	sql[1024];
	char	content[40960];
	char	*database = NULL;
	
	PUnionTableDefTBL	ptableDefTBL = NULL;
	PUnionTableDef		ptableDef = NULL;
	PUnionTableFieldDef	ptableFieldDef = NULL;
	
	// ����
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionLog("in UnionDealServiceCode0406:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		//return(ret);
	}
	UnionToUpperCase(tableName);

	// ƴ�ֱ��崮
	if (strlen(tableName) > 0)
	{
		if ((tableNum = UnionSeprateVarStrIntoVarGrp(tableName,strlen(tableName),',',tableGrp,maxTableNum)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0406:: UnionSeprateVarStrIntoVarGrp[%s]!\n",tableName);
			return(tableNum);
		}
	}

	// ���ݿ�
	database = UnionGetDataBaseType();
	
	// �������ݿ��еı�
	memset(sql,0,sizeof(sql));
	if (strcasecmp(database,"ORACLE") == 0)
		sprintf(sql,"select table_name from user_tables");
	else if (strcasecmp(database,"DB2") == 0)
		sprintf(sql,"select tabname from syscat.tables");
	else if (strcasecmp(database,"MYSQL") == 0)
		sprintf(sql,"select table_name from information_schema.tables where table_schema='USER'");	

	if ((ret = UnionSelectRealDBRecord(sql,0,0)) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0406:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// ��ȡ������
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0406:: UnionReadXMLPackageValue[%s]!\n","totalNum");
		return(ret);
	}
	dbTableNum = atoi(tmpBuf);

	// ��ȡ���ݿ��еı�
	for (i = 0; i < dbTableNum; i++)
	{	
		if ((ret = UnionLocateXMLPackage("detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0406:: UnionLocateXMLPackage!\n");
			return(ret);
		}

		memset(dbTableName[i],0,sizeof(dbTableName[i]));
		if ((ret = UnionReadXMLPackageValue("table_name", dbTableName[i], sizeof(dbTableName[i]))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0406:: UnionReadXMLPackageValue[%d][%s]!\n",i+1,"table_name");
			return(ret);
		}
	}

	if (strlen(tableName) != 0)
	{
		memset(content,0,sizeof(content));
		len += sprintf(content + len,"01 �����Ƕ����ж�ı�\n");

		// ��鶨�����Ƿ����
		for (i = 0; i < tableNum; i++)
		{
			if ((ptableDef = UnionFindTableDef(tableGrp[i])) == NULL)
			{
				UnionUserErrLog("in UnionDealServiceCode0406:: UnionFindTableDef[%s] not found!\n",tableGrp[i]);
				return(errCodeDatabaseMDL_TableNotFound);
			}
		
			// �Աȶ����еı��Ƿ������ݿ��д���	
			for (j = 0; j < dbTableNum; j++)
			{
				if (strcasecmp(tableGrp[i],dbTableName[j]) == 0)
					break;
			}
			if (j == dbTableNum)
			{
				len += sprintf(content + len,"\t%s\t\tδ����\n",tableGrp[i]);
			}
		}
		len += sprintf(content + len,"02 ���������ݿ��붨���б��ֶζԱ�\n");
	}
	else
	{
		// ������ݿ��еı��Ƿ��ж���
		memset(content,0,sizeof(content));
		len += sprintf(content + len,"01 ���������ݿ��ж�ı�\n");
		for (i = 0; i < dbTableNum; i++)
		{	
			// ����Ƿ��ڶ�����
			if ((ptableDef = UnionFindTableDef(dbTableName[i])) == NULL)
			{
				len += sprintf(content + len,"\t%s\t\tδ����\n",dbTableName[i]);
			}
		}

		if ((ptableDefTBL = UnionGetTableDefTBL()) == NULL)
		{
			UnionLog("in UnionDealServiceCode0406:: UnionGetTableDefTBL!\n");
			return(errCodeParameter);
		}

		// ��鶨��ı������ݿ����Ƿ����
		len += sprintf(content + len,"02 �����Ƕ����ж�ı�\n");
		for (i =0; i < ptableDefTBL->tableNum; i++)
		{
			ptableDef = &ptableDefTBL->rec[i];

			for (j = 0; j < dbTableNum; j++)
			{
				if (strcasecmp(ptableDefTBL->rec[i].tableName,dbTableName[j]) == 0)
					break;
			}

			if (j == dbTableNum)
			{
				len += sprintf(content + len,"\t%s\t\tδ����\n",ptableDefTBL->rec[i].tableName);
			}
		}
		len += sprintf(content + len,"03 ���������ݿ��붨���б��ֶζԱ�\n");
	}

	// �����ֶ�
	for (i = 0; i < dbTableNum; i++)
	{
		if (strlen(tableName) > 0)
		{
			for (j = 0; j < tableNum; j++)
			{
				if (strcasecmp(dbTableName[i],tableGrp[j]) == 0)
				{
					ptableDef = UnionFindTableDef(dbTableName[i]);
					break;
				}
			}
			if (j == tableNum)
				continue;
		}
		else
		{
			if ((ptableDef = UnionFindTableDef(dbTableName[i])) == NULL)
				continue;
		}

		// �������ݿ��еı��ֶ�
		memset(sql,0,sizeof(sql));
		sprintf(sql,"select * from user_tab_columns where table_name='%s'",dbTableName[i]);

		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0406:: UnionSelectRealDBRecord[%s]!\n",sql);
			return(ret);
		}

		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0406:: UnionReadXMLPackageValue[%s]!\n","totalNum");
			return(ret);
		}
		dbFieldNum = atoi(tmpBuf);

		for (j = 0; j < dbFieldNum; j++)
		{
			if ((ret = UnionLocateXMLPackage("detail", j+1)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0406:: UnionLocateXMLPackage!\n");
				return(ret);
			}

			//column_name,data_type,data_length,nullable,data_default
			// �ֶ���	
			memset(dbFieldName,0,sizeof(dbFieldName));
			if ((ret = UnionReadXMLPackageValue("column_name", dbFieldName, sizeof(dbFieldName))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0406:: UnionReadXMLPackageValue[%d][%s]!\n",j+1,"column_name");
				return(ret);
			}

			/*for (k = 0; k < ptableDef->fieldNum; k++)
			{
				if (strcasecmp(dbFieldName,ptableDef->fieldDef[k].fieldName) != 0)
					continue;
				else
				{
					break;
				}
			}*/
			ptableFieldDef = &ptableDef->fieldDef[j];
			
			if (strcasecmp(ptableFieldDef->fieldName,dbFieldName) != 0)
			{
				if (strstr(content,dbTableName[i]) == NULL)
				{
					len += sprintf(content + len,"-- ���ݿ����: [%s]\t\t\n",dbTableName[i]);	
				}
				len += sprintf(content + len,"\t-- �ֶ���: [%s]\t\t������δ����\n",dbFieldName);
				if (strstr(content,ptableDef->tableName) == NULL)
				{
					len += sprintf(content + len,"-- �����б���: %s\t\t\n",ptableDef->tableName);
				}
				len += sprintf(content + len,"\t-- �ֶ���: [%s]\t\t���ݿ���δ����\n",ptableFieldDef->fieldName);
				continue;
			}

			// �ֶ�����	
			memset(dbFieldName,0,sizeof(dbFieldName));
			if ((ret = UnionReadXMLPackageValue("data_type", dbFieldName, sizeof(dbFieldName))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0406:: UnionReadXMLPackageValue[%d][%s]!\n",j+1,"data_type");
				return(ret);
			}

			if (strlen(dbFieldName) == 0)
			{
				// �����ж�����ֶ�
				memset(tmpBuf,0,sizeof(tmpBuf));
				sprintf(tmpBuf,"%d",ptableFieldDef->fieldType);
				if (strlen(tmpBuf) != 0)
				{
					if (strstr(content,ptableDef->tableName) == NULL)	
						len += sprintf(content + len,"-- �����б���: %s\t\t\n",ptableDef->tableName);	
					len += sprintf(content + len,"\t-- �ֶ���: %s\t\t\n",ptableFieldDef->fieldName);
					len += sprintf(content + len,"\t\t[%s]\t\t���ݿ���δ����\n","�ֶ�����");
				}
				else
				{
					len += sprintf(content + len,"-- [%s] ���ݿ�Ͷ����ж�û�н����ֶ�����!",ptableDef->tableName);
				}
			}
			else
			{
				// ���ݿ��ж�����ֶ�
				memset(tmpBuf,0,sizeof(tmpBuf));
				sprintf(tmpBuf,"%d",ptableFieldDef->fieldType);
				if (strlen(tmpBuf) == 0)
				{
					if (strstr(content,dbTableName[i]) == NULL)
						len += sprintf(content + len,"-- ���ݿ����: %s\t\t\n",dbTableName[i]);
					len += sprintf(content + len,"\t-- �ֶ���: %s\t\t\n",ptableFieldDef->fieldName);
					len += sprintf(content + len,"\t\t[%s]\t\t������δ����\n","�ֶ�����");
				}
				else
				{
					switch(ptableFieldDef->fieldType)
					{
						case	1:	// �ַ���
						case	4:	// ������
						case	7:	// ʱ����
							if (strcmp(database,"INFORMIX") == 0)
								strcpy(tmpBuf,"lvarchar");
							else
								strcpy(tmpBuf,"varchar2");
							break;
						case	2:	// ����
						case	3:	// ������
						case	6:	// ������
							if (strcmp(database,"INFORMIX") == 0)
								strcpy(tmpBuf,"decimal");
							else
								strcpy(tmpBuf,"number");
							break;
						case	5:	// ������
							if (strcmp(database,"INFORMIX") == 0)
								strcpy(tmpBuf,"int");
							else
								strcpy(tmpBuf,"number");
							break;
						case	99:	// ������
							break;
						default :
							if (strcmp(database,"INFORMIX") == 0)
								strcpy(tmpBuf,"lvarchar");
							else
								strcpy(tmpBuf,"varchar2");
							break;
					}
					if (strcasecmp(dbFieldName,tmpBuf) != 0)
					{
						if (strstr(content,dbTableName[i]) == NULL)
							len += sprintf(content + len,"-- ���ݿ����: %s\t\t\n",dbTableName[i]);
							len += sprintf(content + len,"\t-- �ֶ���: %s\t\t\n",ptableFieldDef->fieldName);
						len += sprintf(content + len,"\t\t%s db = %s,def = %s\t\t�����\n","fieldType",dbFieldName,tmpBuf);
					}
				}
			}

			// �ֶδ�С
			memset(dbFieldName,0,sizeof(dbFieldName));
			if ((ret = UnionReadXMLPackageValue("data_length", dbFieldName, sizeof(dbFieldName))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0406:: UnionReadXMLPackageValue[%d][%s]!\n",j+1,"data_length");
				return(ret);
			}

			if ((ptableFieldDef->fieldType != 2) && 
				(ptableFieldDef->fieldType != 3) && 
				(ptableFieldDef->fieldType != 5) &&
				(ptableFieldDef->fieldType != 6) &&
				(ptableFieldDef->fieldType != 99))
			{
				if (strlen(dbFieldName) == 0)
				{
					// �����ж�����ֶ�
					memset(tmpBuf,0,sizeof(tmpBuf));
					sprintf(tmpBuf,"%.f",ptableFieldDef->fieldSize);
					if (strlen(tmpBuf) != 0)
					{
						if (strstr(content,ptableDef->tableName) == NULL)	
							len += sprintf(content + len,"-- �����б���:%s\t\t\n",ptableDef->tableName);
						len += sprintf(content + len,"\t-- �ֶ���:%s\t\t\n",ptableFieldDef->fieldName);
						len += sprintf(content + len,"\t\t[%s]\t\t���ݿ���δ����\n","�ֶδ�С");
					}
					else
					{
						len += sprintf(content + len,"-- [%s] ���ݿ�Ͷ����ж�û�н����ֶδ�С(fieldSize)!",ptableDef->tableName);
					}
				}
				else
				{
					// ���ݿ��ж�����ֶ�
					memset(tmpBuf,0,sizeof(tmpBuf));
					sprintf(tmpBuf,"%.f",ptableFieldDef->fieldSize);
					if (strlen(tmpBuf) == 0)
					{
						if (strstr(content,dbTableName[i]) == NULL)
						len += sprintf(content + len,"-- ���ݿ����: %s\t\t\n",dbTableName[i]);
						len += sprintf(content + len,"\t-- �ֶ���: %s\t\t\n",ptableFieldDef->fieldName);
						len += sprintf(content + len,"\t\t[%s]\t\t������δ����\n","�ֶδ�С");
					}
				
					// �ֶδ�С�Ա� 
					if (strlen(tmpBuf) != 0)
					{
						if (strcmp(dbFieldName,tmpBuf) != 0)	
						{
							if (strstr(content,dbTableName[i]) == NULL)
								len += sprintf(content + len,"-- ���ݿ����: %s\t\t\n",dbTableName[i]);
							len += sprintf(content + len,"\t-- �ֶ���: %s\t\t\n",ptableFieldDef->fieldName);	
							len += sprintf(content + len,"\t\t%s db = %s,def = %s\t\t�����\n","fieldSize",dbFieldName,tmpBuf);
						}
					}
				}
			}

			// Ĭ��ֵ
			memset(dbFieldName,0,sizeof(dbFieldName));
			if ((ret = UnionReadXMLPackageValue("data_default", dbFieldName, sizeof(dbFieldName))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0406:: UnionReadXMLPackageValue[%d][%s]!\n",j+1,"data_default");
				return(ret);
			}

			if (strlen(dbFieldName) == 0)
			{
				// �����ж�����ֶ�
				if(strlen(ptableFieldDef->defaultValue) != 0)
				{
					if (strstr(content,ptableDef->tableName) == NULL)
						len += sprintf(content + len,"-- �����б���: %s\t\t\n",ptableDef->tableName);
					len += sprintf(content + len,"\t-- �ֶ���: %s\t\t\n",ptableFieldDef->fieldName);
					len += sprintf(content + len,"\t\t[%s]\t\t���ݿ���δ����\n","Ĭ��ֵ");
				}
			}
			else
			{
				// ���ݿ��ж�����ֶ�
				if(strlen(ptableFieldDef->defaultValue) == 0)
				{
					if (strstr(content,dbTableName[i]) == NULL)
						len += sprintf(content + len,"-- ���ݿ����: %s\t\t\n",dbTableName[i]);
					len += sprintf(content + len,"\t-- �ֶ���: %s\t\t\n",ptableFieldDef->fieldName);
					len += sprintf(content + len,"\t\t[%s]\t\t������δ����\n","Ĭ��ֵ");
				}

				// Ĭ��ֵ�Ա�
				if(strlen(ptableFieldDef->defaultValue) != 0)
				{
					memset(tmpBuf,0,sizeof(tmpBuf));
					if ((ptableFieldDef->fieldType == 2) || (ptableFieldDef->fieldType == 5) || (ptableFieldDef->fieldType == 3))
					{
						sprintf(tmpBuf,"%s",ptableFieldDef->defaultValue);
					}
					else
						sprintf(tmpBuf,"'%s'",ptableFieldDef->defaultValue);
					if (strcasecmp(dbFieldName,tmpBuf) != 0)
					{
						if (strstr(content,dbTableName[i]) == NULL)
							len += sprintf(content + len,"-- ���ݿ����: %s\t\t\n",dbTableName[i]);
						len += sprintf(content + len,"\t-- �ֶ���: %s\t\t\n",ptableFieldDef->fieldName);
						len += sprintf(content + len,"\t\t%s db = %s,def = %s\t\t�����\n","defaultValue",dbFieldName,tmpBuf);
					}
				}
			}

			// �Ƿ�Ϊ��
			memset(dbFieldName,0,sizeof(dbFieldName));
			if ((ret = UnionReadXMLPackageValue("nullable", dbFieldName, sizeof(dbFieldName))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0406:: UnionReadXMLPackageValue[%d][%s]!\n",j+1,"nullable");
				return(ret);
			}

			if (strlen(dbFieldName) == 0)
			{
				// �����ж�����ֶ�
				memset(tmpBuf,0,sizeof(tmpBuf));
				sprintf(tmpBuf,"%d",ptableFieldDef->isNull);
				if(strlen(tmpBuf) != 0)
				{
					if (strstr(content,ptableDef->tableName) == NULL)
						len += sprintf(content + len,"-- �����б���: %s\t\t\n",ptableDef->tableName);
					len += sprintf(content + len,"\t-- �ֶ���: %s\t\t\n",ptableFieldDef->fieldName);
					len += sprintf(content + len,"\t\t[%s]\t\t���ݿ���δ����\n","Ϊ��");
				}
			}
			else
			{
				// ���ݿ��ж�����ֶ�
				memset(tmpBuf,0,sizeof(tmpBuf));
				sprintf(tmpBuf,"%d",ptableFieldDef->isNull);
				if(strlen(tmpBuf) == 0)
				{
					if (strstr(content,dbTableName[i]) == NULL)
						len += sprintf(content + len,"-- ���ݿ����: %s\t\t\n",dbTableName[i]);
					len += sprintf(content + len,"\t-- �ֶ���: %s\t\t\n",ptableFieldDef->fieldName);
					len += sprintf(content + len,"\t\t[%s]\t\t������δ����\n","Ϊ��");
				}

			 	// �Ƿ�Ϊ�նԱ�
				if (strlen(tmpBuf) != 0)
				{
					if (strcasecmp(dbFieldName,ptableFieldDef->isNull == 1 ? "Y":"N") != 0)
					{
						if (strstr(content,dbTableName[i]) == NULL)
							len += sprintf(content + len,"-- ���ݿ����: %s\t\t\n",dbTableName[i]);
						len += sprintf(content + len,"\t-- �ֶ���: %s\t\t\n",ptableFieldDef->fieldName);
						len += sprintf(content + len,"\t\t%s db = %s,def = %s\t\t�����\n","isNull",dbFieldName,tmpBuf);
					}
				}
			}
		}
		for (;j < ptableDef->fieldNum; j++)
		{
			if (ptableDef->fieldDef[j].fieldType == 99)
				continue;
			len += sprintf(content + len,"-- �����б���: [%s]\t\t\n",ptableDef->tableName);
			len += sprintf(content + len,"\t-- �ֶ���: [%s]\t\t���ݿ���δ����\n",ptableDef->fieldDef[j].fieldName);
		}
	}

	if ((ret = UnionSetResponseXMLPackageValue("body/content",content)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0406:: UnionSetResponseXMLPackageValue[%s]!\n","body/content");
		return(ret);
	}
	return(0);		
}
