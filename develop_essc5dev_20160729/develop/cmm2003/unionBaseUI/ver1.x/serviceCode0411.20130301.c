
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
#include "UnionStr.h"
#include "baseUIService.h"


/*********************************
�������:	0411
������:		���ӱ��ֶ�
��������:	���ӱ��ֶ�
**********************************/

int UnionDealServiceCode0411(PUnionHsmGroupRec phsmGroupRec)
{
	int	i;
	int	ret;
	int	maxID = 0;
	char	tableName[40+1];
	int	fieldID = 0;
	char	fieldName[40+1];
	char	fieldChnName[40+1];
	char	fieldType[40+1];
	int	fieldSize = 0;
	char	defaultValue[128+1];
	char	isPrimaryKey[2+1];
	char	isNull[2+1];
	char	fieldLimitMethod[2048+1];
	char	fieldControl[2048+1];
	char	fieldValueMethod[2048+1];
	char	varNameOfEnabled[30+1];
	char	varValueOfEnabled[128+1];
	char	remark[128+1];
	char	fileName[1024+1];
	char	tmpBuf[512+1];

	// ��ȡ����
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(tableName);	
	if (strlen(tableName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: tableName can not be null!\n");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}
	
	// �����Ƿ��Ѿ�����
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTableDef(tableName,fileName);

	// �ļ�������
	if ((access(fileName,0) < 0))
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: fileName[%s] not found!\n",fileName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// ��ȡ�ֶ�ID
	memset(tmpBuf,0,sizeof(tmpBuf));
        if ((ret = UnionReadRequestXMLPackageValue("body/fieldID",tmpBuf,sizeof(tmpBuf))) > 0)
        {
                fieldID = atoi(tmpBuf);
		if (fieldID == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0411:: fieldID[%d] is invalid!\n",fieldID);
			return(errCodeObjectMDL_FieldValueIsInvalid);
		}	
        }
	
	// ��ȡ�ֶ���
	memset(fieldName,0,sizeof(fieldName));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldName",fieldName,sizeof(fieldName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadRequestXMLPackageValue[%s]!\n","body/fieldName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(fieldName);
	if (strlen(fieldName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/fieldName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// ��ȡ�ֶ�������
	memset(fieldChnName,0,sizeof(fieldChnName));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldChnName",fieldChnName,sizeof(fieldChnName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadRequestXMLPackageValue[%s]!\n","body/fieldChnName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(fieldChnName);
	if (strlen(fieldChnName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/fieldChnName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// ��ȡ�ֶ�����
	memset(fieldType,0,sizeof(fieldType));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldType",fieldType,sizeof(fieldType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadRequestXMLPackageValue[%s]!\n","body/fieldType");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(fieldType);
	if (strlen(fieldType) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/fieldType");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// ��ȡ�ֶδ�С
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldSize",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadRequestXMLPackageValue[%s]!\n","body/fieldSize");	
		return(ret);
	}
	UnionFilterHeadAndTailBlank(tmpBuf);
	if (strlen(tmpBuf) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/fieldSize");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}
	else if (atoi(tmpBuf) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: fieldSize[%s] is invalid!\n",tmpBuf);
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}
	else
	{
		fieldSize = atoi(tmpBuf);
	}
	// ��ȡĬ��ֵ
	memset(defaultValue,0,sizeof(defaultValue));
	if ((ret = UnionReadRequestXMLPackageValue("body/defaultValue",defaultValue,sizeof(defaultValue))) <= 0)
	{
		strcpy(defaultValue,"");
	}

	// ��ȡ�Ƿ�ؼ���
	memset(isPrimaryKey,0,sizeof(isPrimaryKey));
	if ((ret = UnionReadRequestXMLPackageValue("body/isPrimaryKey",isPrimaryKey,sizeof(isPrimaryKey))) <= 0)
	{
		strcpy(isPrimaryKey,"0");
	}

	// ��ȡ�Ƿ�Ϊ��
	memset(isNull,0,sizeof(isNull));
	if ((ret = UnionReadRequestXMLPackageValue("body/isNull",isNull,sizeof(isNull))) <= 0)
	{
		strcpy(isNull,"1");	
	}

	// �ֶ����Ʒ���
	memset(fieldLimitMethod,0,sizeof(fieldLimitMethod));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldLimitMethod",fieldLimitMethod,sizeof(fieldLimitMethod))) <= 0)
	{
		strcpy(fieldLimitMethod,"");
	}

	// �ֶοؼ�����
	memset(fieldControl,0,sizeof(fieldControl));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldControl",fieldControl,sizeof(fieldControl))) <= 0)
	{
		strcpy(fieldControl,"");
	}

	// �ֶ����Ʒ���
	memset(fieldValueMethod,0,sizeof(fieldValueMethod));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldValueMethod",fieldValueMethod,sizeof(fieldValueMethod))) <= 0)
	{
		strcpy(fieldValueMethod,"");
	}

	// ���ñ�����
	memset(varNameOfEnabled,0,sizeof(varNameOfEnabled));
	if ((ret = UnionReadRequestXMLPackageValue("body/varNameOfEnabled",varNameOfEnabled,sizeof(varNameOfEnabled))) <= 0)
	{
		strcpy(varNameOfEnabled,"");
	}

	// ���ñ�����
	memset(varValueOfEnabled,0,sizeof(varValueOfEnabled));
	if ((ret = UnionReadRequestXMLPackageValue("body/varValueOfEnabled",varValueOfEnabled,sizeof(varValueOfEnabled))) <= 0)
	{
		strcpy(varValueOfEnabled,"");
	}

	// ��ȡ��ע
	memset(remark,0,sizeof(remark));
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",remark,sizeof(remark))) <= 0)
	{
		strcpy(remark,"");
	}

	// ��ʼ���ļ�
	if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionInitXMLPackage[%s]!\n",fileName);
		return(ret);
	}

	for (i = 1;;i++)
	{
		if ((ret = UnionLocateXMLPackage("field",i)) < 0)
		{
			maxID = i;
			// �����µ�ID
			if ((ret = UnionLocateNewXMLPackage("field",i)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionLocateNewXMLPackage[%s]!\n","field");
				return(ret);
			}

			// �����ֶ���
			if ((ret = UnionSetXMLPackageValue("fieldName",fieldName)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","fieldName");
				return(ret);
			}

			// �����ֶ�������
			if ((ret = UnionSetXMLPackageValue("fieldChnName",fieldChnName)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","fieldChnName");
				return(ret);
			}

			// �����ֶ�����
			if ((ret = UnionSetXMLPackageValue("fieldType",fieldType)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","fieldType");
				return(ret);
			}

			// �����ֶδ�С
			memset(tmpBuf,0,sizeof(tmpBuf));
			sprintf(tmpBuf,"%d",fieldSize);
			if ((ret = UnionSetXMLPackageValue("fieldSize",tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","fieldSize");
				return(ret);
			}

			// ����Ĭ��ֵ
			if ((ret = UnionSetXMLPackageValue("defaultValue",defaultValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","defaultValue");
				return(ret);
			}

			// �����Ƿ�ؼ���
			if ((ret = UnionSetXMLPackageValue("isPrimaryKey",isPrimaryKey)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","isPrimaryKey");
				return(ret);
			}

			// �����Ƿ�Ϊ��
			if ((ret = UnionSetXMLPackageValue("isNull",isNull)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","isNull");
				return(ret);
			}

			// �����ֶ����Ʒ���
			if ((ret = UnionSetXMLPackageValue("fieldLimitMethod",fieldLimitMethod)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","fieldLimitMethod");
				return(ret);
			}

			// �����ֶοؼ�����
			if ((ret = UnionSetXMLPackageValue("fieldControl",fieldControl)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","fieldControl");
				return(ret);
			}

			// �����ֶ�ȡֵ����
			if ((ret = UnionSetXMLPackageValue("fieldValueMethod",fieldValueMethod)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","fieldValueMethod");
				return(ret);
			}

			// �����ֶ�ȡֵ����
			if ((ret = UnionSetXMLPackageValue("varNameOfEnabled",varNameOfEnabled)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","varNameOfEnabled");
				return(ret);
			}

			// �����ֶ�ȡֵ����
			if ((ret = UnionSetXMLPackageValue("varValueOfEnabled",varValueOfEnabled)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","varValueOfEnabled");
				return(ret);
			}

			// ���ӱ�ע
			if ((ret = UnionSetXMLPackageValue("remark",remark)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionSetXMLPackageValue[%s]!\n","remark");
				return(ret);
			}
			break;
		}
		else
		{
			// �����ֶ���
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((ret = UnionReadXMLPackageValue("fieldName",tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionReadXMLPackageValue[%s]!\n","body/fieldName");
				return(ret);
			}
			// �Ա��ֶ����Ƿ���� 
			if (strcmp(fieldName,tmpBuf) == 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: fieldName[%s] already exist!\n",fieldName);
				return(errCodeObjectMDL_FieldValueIsInvalid);
			}
		}
	}


	// д��XML�ļ�
	if ((ret = UnionXMLPackageToFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionXMLPackageToFile[%s]!\n",fileName);
		return(ret);
	}

	// ���ص������ڴ�
	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReloadTableDefTBL!\n");
		return(ret);
	}

	// ��ʼ���ļ�
	if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionInitXMLPackage[%s]!\n",fileName);
		return(ret);
	}

	// ����ָ��λ��
	if (fieldID > 0 && fieldID < maxID )
	{
		for (i = maxID; i > fieldID; i--)
		{
			// �����ڵ�����
			if ((ret = UnionExchangeIDXMLPackage("field",i-1,i)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0411:: UnionExchangeIDXMLPackage %d failed!\n",i);
				return(ret);
			}
		}
	}

	// д��XML�ļ�
	if ((ret = UnionXMLPackageToFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionXMLPackageToFile[%s]!\n",fileName);
		return(ret);
	}

	// ���ص������ڴ�
	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0411:: UnionReloadTableDefTBL!\n");
		return(ret);
	}

	return 0;
}
