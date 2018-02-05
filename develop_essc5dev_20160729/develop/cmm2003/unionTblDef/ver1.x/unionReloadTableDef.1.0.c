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

// �ж��Ƿ��������
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
	
	// ��Ŀ¼
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
		
		// �����ļ�
		memset(tableFileName,0,sizeof(tableFileName));
		sprintf(tableFileName,"%s/%s",fileDir,fileName);
		
		if ((ret = UnionInitXMLPackage(tableFileName,NULL,0)) < 0)
		{
			UnionUserErrLog("in UnionReloadTableDefTBL:: UnionInitXMLPackage[%s]!\n",tableFileName);
			continue;
		}
		
		memset(&pgunionTableDefTBL->rec[tableNum],0,sizeof(pgunionTableDefTBL->rec[tableNum]));
	
		// ����
		UnionGetTableNameFromFileName(fileName,pgunionTableDefTBL->rec[tableNum].tableName);
		
		// ��������
		if ((ret = UnionReadXMLPackageValue("tableChnName", pgunionTableDefTBL->rec[tableNum].tableChnName, sizeof(pgunionTableDefTBL->rec[tableNum].tableChnName))) < 0)
		{
			UnionUserErrLog("in UnionReloadTableDefTBL:: UnionReadXMLPackageValue[tableChnName]!\n");
			continue;
		}
		
		// ������
		if ((ret = UnionReadXMLPackageValue("tableType", pgunionTableDefTBL->rec[tableNum].tableType, sizeof(pgunionTableDefTBL->rec[tableNum].tableType))) < 0)
		{
			UnionUserErrLog("in UnionReloadTableDefTBL:: UnionReadXMLPackageValue[tableType]!\n");
			continue;
		}

		// ���淽��,0����׼��1�����Ի�
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
			// �ֶ���
			if ((ret = UnionReadXMLPackageValue("fieldName", pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldName, sizeof(pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldName))) < 0)
			{
				UnionUserErrLog("in UnionReloadTableDefTBL:: UnionReadXMLPackageValue[%s]!\n","fieldName");
				continue;
			}

			// �ֶ�������
			if ((ret = UnionReadXMLPackageValue("fieldChnName", pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldChnName, sizeof(pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldChnName))) < 0)
			{
				UnionUserErrLog("in UnionReloadTableDefTBL:: UnionReadXMLPackageValue[%s]!\n","fieldChnName");
				continue;
			}

			// �ֶ�����
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((ret = UnionReadXMLPackageValue("fieldType", tmpBuf, sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionReloadTableDefTBL:: UnionReadXMLPackageValue[%s]!\n","fieldType");
				continue;
			}
			pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldType = atoi(tmpBuf);
			
			// �ֶδ�С
			if ((pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldType == 1) ||		// �ַ���
				(pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldType == 3) ||	// ������
				(pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldType == 4) ||	// ������
				(pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldType == 7) ||	// ����ʱ��
				(pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldType == 99))		// ������
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
			else if (pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldType == 5)	// ������
				pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldSize = 1;
			else
				pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].fieldSize = 0;
				
			// Ĭ��ֵ
			if ((ret = UnionReadXMLPackageValue("defaultValue", pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].defaultValue, sizeof(pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].defaultValue))) < 0)
			{
				UnionLog("in UnionReloadTableDefTBL:: UnionReadXMLPackageValue[%s]!\n","defaultValue");
				strcpy(pgunionTableDefTBL->rec[tableNum].fieldDef[fieldNum].defaultValue,"");
				//continue;
			}
			
			// �Ƿ�ؼ���
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
				// �Ƿ�Ϊ��
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

	���ܣ����������������Ψһֵ�Ƚ������
	������
		tableName: ����
		sql: �������ɵ�sql���
	����ֵ��
		<0: ����
		>=0:��ȷ

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
	
	// ��ʼ���ļ�
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTableDef(tableName,fileName);

	if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionInitXMLPackage[%s]!\n",fileName);
		return(ret);
	}

	// ��ȡ���
	for (i = 0; ; i++)
	{
		// �趨�����������Ԥ�Ƶ�
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

		// ��ȡ����еĲ��ձ���
		memset(foreignTableName[i],0,sizeof(foreignTableName[i]));
		if ((ret = UnionReadXMLPackageValue("references",foreignTableName[i],sizeof(foreignTableName[i]))) < 0)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName::  UnionReadXMLPackageValue[%d][%s]\n",i+1,"references");
			return(ret);
		}

		// ��������ɾ����ʶ
		strcpy(isDeleteCascade[i], "1");
		if ((ret = UnionReadXMLPackageValue("isDeleteCascade",isDeleteCascade[i],sizeof(isDeleteCascade[i]))) < 0)
		{
			//UnionLog("in UnionGetExpandSqlByTableName::  UnionReadXMLPackageValue[%d][%s]\n",i+1,"isDeleteCascade");
			//return(ret);
		}
		
		// �����������±�ʶ
		strcpy(isUpdateCascade[i], "1");
		if ((ret = UnionReadXMLPackageValue("isUpdateCascade",isUpdateCascade[i],sizeof(isUpdateCascade[i]))) < 0)
		{
			//UnionLog("in UnionGetExpandSqlByTableName::  UnionReadXMLPackageValue[%d][%s]\n",i+1,"isUpdateCascade");
			//return(ret);
		}

		// ��ȡ����б��ر���ֶ���
		memset(localFields[i],0,sizeof(localFields[i]));
		if ((ret = UnionReadXMLPackageValue("localfields",localFields[i],sizeof(localFields[i]))) < 0)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionReadXMLPackageValue[%d][%s]\n",i+1,"localfields");
			return(ret);
		}

		// ��ȡ����в��ձ���ֶ���
		memset(foreignFields[i],0,sizeof(foreignFields[i]));
		if ((ret = UnionReadXMLPackageValue("foreignfields",foreignFields[i],sizeof(foreignFields[i]))) < 0)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionReadXMLPackageValue[%d][%s]\n",i+1,"foreignfields");
			return(ret);
		}
	}

	// ��ȡΨһֵ�ֶ�
	for (i = 0; ; i++)
	{
		// ��ȡ��Ψһֵ�ֶδ���Ԥ�Ƶ�
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

		// ��ȡfields
		memset(uniqueFields[i],0,sizeof(uniqueFields[i]));
		if ((ret = UnionReadXMLPackageValue("fields",uniqueFields[i],sizeof(uniqueFields[i]))) < 0)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName::  UnionReadXMLPackageValue[%d][%s] unique\n",i+1,"fields");
			return(ret);
		}
	}

	// ��ȡ����
	for (i = 0; ; i++)
	{
		// ��ȡ������������Ԥ�Ƶ�
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

		// ��ȡfields
		memset(indexFields[i],0,sizeof(indexFields[i]));
		if ((ret = UnionReadXMLPackageValue("fields",indexFields[i],sizeof(indexFields[i]))) < 0)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName::  UnionReadXMLPackageValue[%d][%s] index\n",i+1,"fields");
			return(ret);
		}
	}

	 // ��ȡ�ļ�
	if ((localPTableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionFindTableDef[%s] not found!\n",tableName);
		return(errCodeDatabaseMDL_TableNotFound);
	}
	
	// �������Ϸ���
	for (i = 0; i < 16; i++)
	{
		// ��ȡ����в��ձ�
		if (strlen(foreignTableName[i]) == 0)
			break;

		memset(fileName,0,sizeof(fileName));
		UnionGetFileNameOfTableDef(foreignTableName[i],fileName);

		// ��ʼ���ļ�
		if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionInitXMLPackage[%s]!\n",fileName);
			return(ret);
		}
		
		// ��ȡ���ձ��е�Ψһֵ�ֶ�
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
			// ��ȡΨһֵ
			memset(foreignUniqueFields[j],0,sizeof(foreignUniqueFields[j]));
			if ((ret = UnionReadXMLPackageValue("fields",foreignUniqueFields[j],sizeof(foreignUniqueFields[j]))) < 0)
			{
				UnionUserErrLog("in UnionGetExpandSqlByTableName::  UnionReadXMLPackageValue[%d][%s]  foreign unique\n",j+1,"fields");
				return(ret);
			}
		}
		
		// ��ȡ�ļ�
		if ((foreignPTableDef = UnionFindTableDef(foreignTableName[i])) == NULL)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionFindTableDef[%s] not found!\n",foreignTableName[i]);
			return(errCodeDatabaseMDL_TableNotFound);
		}

		// �������в��ձ��е��ֶ��Ƿ�Ψһ
		//for (j = 0; j < 16; j++)
		{
			// ƴ�����崮
			memset(foreignFieldName,0,sizeof(foreignFieldName));
			memset(tmpBuf,0,sizeof(tmpBuf));
			strcpy(tmpBuf,foreignFields[i]);
			if ((foreignFieldNum = UnionSeprateVarStrIntoArray(foreignFields[i],strlen(foreignFields[i]),',',(char *)&foreignFieldName,32,128)) < 0)
			{
				UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionSeprateVarStrIntoArray [%04zu][%s]!\n",strlen(foreignFields[i]),foreignFields[i]);
				return(foreignFieldNum);
			}
			strcpy(foreignFields[i],tmpBuf);
			
			// ƴ�����崮
			memset(localFieldName,0,sizeof(localFieldName));
			memset(tmpBuf,0,sizeof(tmpBuf));
			strcpy(tmpBuf,localFields[i]);
			if ((localFieldNum = UnionSeprateVarStrIntoArray(localFields[i],strlen(localFields[i]),',',(char *)&localFieldName,32,128)) < 0)
			{
				UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionSeprateVarStrIntoArray [%04zu][%s]!\n",strlen(localFields[i]),localFields[i]);
				return(localFieldNum);
			}
			strcpy(localFields[i],tmpBuf);
			
			// ����б��ر�����ձ��ֶ������Ա�
			if (foreignFieldNum != localFieldNum)
			{
				UnionUserErrLog("in UnionGetExpandSqlByTableName:: foreignFieldNum[%d] != localFieldNum[%d]!\n",foreignFieldNum,localFieldNum);
				return(errCodeObjectMDL_FieldNumberError);
			}
		}
			
			// �������б��е��ֶ����Ƿ����
			for (index = 0; index < localFieldNum; index++)
			{
				// �������б��ر���ֶ����Ƿ����
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

				// �������в��ձ��ֶ����Ƿ����
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
				
				// �Ա��ֶ�����
				// 6 �� 2 ��ͬһ����
				if (localPTableDef->fieldDef[t].fieldType != foreignPTableDef->fieldDef[h].fieldType &&
					abs(localPTableDef->fieldDef[t].fieldType - foreignPTableDef->fieldDef[h].fieldType) != 4)
				{
					UnionUserErrLog("in UnionGetExpandSqlByTableName:: foreignFieldName[%s].fieldType[%d] != localFieldName[%s].fieldType[%d] !\n",foreignFieldName[index],foreignPTableDef->fieldDef[h].fieldType, localFieldName[index], localPTableDef->fieldDef[t].fieldType);
					return(errCodeObjectMDL_FieldTypeInvalid);
				}

				// �Ա��ֶδ�С
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

	// ��ȡΨһֵ���ֶ���,�����Ψһֵ�е��ֶ����Ƿ����
	for (i = 0; i < 16; i++)
	{
		memset(uniqueFieldName,0,sizeof(uniqueFieldName));
		memset(tmpBuf,0,sizeof(tmpBuf));
		strcpy(tmpBuf,uniqueFields[i]);
	
		// ƴ�����崮
		if ((uniqueFieldNum = UnionSeprateVarStrIntoArray(uniqueFields[i],strlen(uniqueFields[i]),',',(char *)&uniqueFieldName,32,128)) < 0)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionSeprateVarStrIntoArray [%04zu][%s]!\n",strlen(uniqueFields[i]),uniqueFields[i]);
			return(uniqueFieldNum);
		}
		strcpy(uniqueFields[i],tmpBuf);

		// ����ֶ����Ƿ����
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

	// ��ȡ�������ֶ���,����������е��ֶ����Ƿ����
	for (i = 0; i < 16; i++)
	{
		memset(indexFieldName,0,sizeof(indexFieldName));
		memset(tmpBuf,0,sizeof(tmpBuf));
		strcpy(tmpBuf,indexFields[i]);

		// ƴ�����崮
		if ((indexFieldNum = UnionSeprateVarStrIntoArray(indexFields[i],strlen(indexFields[i]),',',(char *)&indexFieldName,32,128)) < 0)
		{
			UnionUserErrLog("in UnionGetExpandSqlByTableName:: UnionSeprateVarStrIntoArray [%04zu][%s]!\n",strlen(indexFields[i]),indexFields[i]);
			return(indexFieldNum);
		}
		strcpy(indexFields[i],tmpBuf);

		// ����ֶ����Ƿ����
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

	// ����Ψһֵ���
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
			len += sprintf(sql + len,",\n\t-- ����Ψһֵ");
			len += sprintf(sql + len,"\n\tunique (%s)",uniqueFields[i]);
		}
		else
		{
			len += sprintf(sql + len,",\n\tunique (%s)",uniqueFields[i]);
		}
	}
	// ����������
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
		
		// ORACLE��DB2��֧�ּ�������
		/* 
		if(strcmp(isUpdateCascade[i], "1") == 0)
			sprintf(cascadeStr + offset,"on update cascade ");
		*/
		
		if (0 == i)
		{
			if (sql[len-1] == ',')
			{
				len += sprintf(sql + len,"\n\t-- �������");
			}
			else
			{
				//strcpy(&sql[len - 4],",");
				//len = strlen(sql);
				len += sprintf(sql + len,",\n\t-- �������");
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

	// ��������
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


// ���ɽ������
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
	
	// д�������
	len += sprintf(sql + len,"-- %s %s\n",ptableDef->tableName,ptableDef->tableChnName);
	len += sprintf(sql + len,"create table %s\n",ptableDef->tableName);
	len += sprintf(sql + len,"(\n");
		
	// �����ֶ�
	memset(strpPrimaryKey,0,sizeof(strpPrimaryKey));
	for (fldNum = 0; fldNum < ptableDef->fieldNum; fldNum++)
	{
		ptableFieldDef = &ptableDef->fieldDef[fldNum];
		
		// �����ֶ�
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
			case 1:		// �ַ���
			case 7:		// ʱ����
				if (strcmp(database,"INFORMIX") == 0)
					len += sprintf(sql + len,"\t%s%slvarchar",ptableFieldDef->fieldName,tabBuf);
				else
					len += sprintf(sql + len,"\t%s%svarchar",ptableFieldDef->fieldName,tabBuf);
				len += sprintf(sql + len,"(%.f)",ptableFieldDef->fieldSize);
				
				if (strcmp(database,"MYSQL") == 0)
					len += sprintf(sql + len," binary");
				break;
			case 2:		// ����
				len += sprintf(sql + len,"\t%s%sinteger",ptableFieldDef->fieldName,tabBuf);
				break;
			case 3:		// ������
				memset(tmpBuf,0,sizeof(tmpBuf));
				memset(strFieldSize,0,sizeof(strFieldSize));
				sprintf(tmpBuf,"%.1f",ptableFieldDef->fieldSize);
				UnionConvertOneFldSeperatorInRecStrIntoAnother(tmpBuf,strlen(tmpBuf),'.',',',strFieldSize,sizeof(strFieldSize));
				if ((strcmp(database,"INFORMIX") == 0) || (strcmp(database,"MYSQL") == 0))
					len += sprintf(sql + len,"\t%s%sdecimal(%s)",ptableFieldDef->fieldName,tabBuf,strFieldSize);
				else
					len += sprintf(sql + len,"\t%s%snumber(%s)",ptableFieldDef->fieldName,tabBuf,strFieldSize);
				break;
			case 4:		// ������
				if (strcmp(database,"INFORMIX") == 0)
					len += sprintf(sql + len,"\t%s%slvarchar",ptableFieldDef->fieldName,tabBuf);
				else
					len += sprintf(sql + len,"\t%s%svarchar",ptableFieldDef->fieldName,tabBuf);
				len += sprintf(sql + len,"(%.f)",ptableFieldDef->fieldSize);
				if (strcmp(database,"MYSQL") == 0)
					len += sprintf(sql + len," binary");
				break;
			case 5:		// ������
				len += sprintf(sql + len,"\t%s%sinteger",ptableFieldDef->fieldName,tabBuf);
				break;
			case 6:		// ������
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
		
		// ȱʡֵ
		if (strlen(ptableFieldDef->defaultValue) > 0)
		{
			switch (ptableFieldDef->fieldType)
			{
				case 2:		// ����
				case 3:		// ������
				case 5:		// ������
				case 6:		// ������
					len += sprintf(sql + len," default %s",ptableFieldDef->defaultValue);
					break;
				default:
					len += sprintf(sql + len," default '%s'",ptableFieldDef->defaultValue);
					break;							
			}
		}

		switch (ptableFieldDef->fieldType)
		{
			case 6:		// ������
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
		// �����ؼ���
		ret = strlen(strpPrimaryKey);
		if (ret > 0)
			strpPrimaryKey[ret - 1] = 0;
		if (strlen(strpPrimaryKey) > 0)
		{
			len += sprintf(sql + len,",\n\t-- ����ؼ���\n");
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

	// ��������
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
	if(strlen(tableName) == 0) // �������б��SQL���
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
	else	// ֻ����tableName��SQL���
	{
		// ƴ�ֱ��崮
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
				// �������
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
						UnionSetResponseRemark("��[%s]: �����[%s]���ܺͱ�����ͬ",ptableDefTBL->rec[i].tableName,foreignTableName);
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
���ܣ�	����ֶ��Ƿ�Ϊ��
������	tableName[in]		����
	fieldName[in]		�ֶ���
����ֵ��1			��
	0			��
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
	
	// ��ʼ���ļ�
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
		
		// �ֶ���
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("fieldName", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckFieldNameIsTrue:: UnionReadXMLPackageValue[%s]!\n","fieldName");
			continue;
		}
		
		if (strcmp(fieldName,tmpBuf) != 0)
			continue;
		
		// ���õı�����
		memset(varNameOfEnabled,0,sizeof(varNameOfEnabled));
		if ((ret = UnionReadXMLPackageValue("varNameOfEnabled", varNameOfEnabled, sizeof(varNameOfEnabled))) <= 0)
			return(1);
		
		// ������û�ж���
		if (UnionReadStringTypeRECVar(varNameOfEnabled) == NULL)
			return(1);
			
		// ���õı���ֵ
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
���ܣ�	ת�����ֶΣ����˲���ʾ���ֶ�
������	useFlag[in]		��;��1����ʾ��2���˵�����
	tableList[in]		����
	fieldList[in|out]	�ֶ���
����ֵ��1			��
	0			��
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

	// ƴ�ֱ������崮
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
			if ((ptr = strchr(fieldGrp[i],'.')) == NULL)	// ��������
			{
				strcpy(tableName,tableNameGrp[j]);
				strcpy(fieldName,fieldGrp[i]);

				if (useFlag == 1)	// ��ʾ
					ret = UnionCheckFieldNameIsTrue(tableName,fieldName);
				else			// �˵�����
					ret = UnionExistFieldOfTableName(tableName,fieldName);
				
				if (ret)
				{
					len += sprintf(tmpBuf+len,"%s,",fieldGrp[i]);
					break;
				}
				else
					continue;
			}
			else						// ������
			{
				memcpy(tableName,fieldGrp[i],ptr - fieldGrp[i]);
				strcpy(fieldName,ptr + 1);

				if (useFlag == 1)	// ��ʾ
					ret = UnionCheckFieldNameIsTrue(tableName,fieldName);
				else			// �˵�����
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

// ���ݱ�����ȡ���ݿ�����λ��
int UnionGetDBIndexByTableName(char *tableName)
{
	int	i;
	int	ret;
	char	fileName[256];
	char	dbIndex[32];
	PUnionTableDefTBL	ptableDefTBL = NULL;
	
	// ��SQLITE��������
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
		// ��ʼ���ļ�
		ret = UnionGetFileNameOfTableDef(ptableDefTBL->rec[i].tableName,fileName);
		fileName[ret] = 0;
	
		if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
		{
			UnionUserErrLog("in UnionGetDBIndexByTableName:: UnionInitXMLPackage[%s]!\n",fileName);
			return(ret);
		}
		
		strcpy(gunionDBIndexTBL.rec[i].tableName,ptableDefTBL->rec[i].tableName);
		
		// ���ݿ�����
		if ((ret = UnionReadXMLPackageValue("dbIndex", dbIndex, sizeof(dbIndex))) <= 0)
		{
			if (strcmp(ptableDefTBL->rec[i].tableType,"ϵͳ��") == 0)
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
