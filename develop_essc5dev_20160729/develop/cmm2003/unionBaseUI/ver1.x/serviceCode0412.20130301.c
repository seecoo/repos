
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
�������:	0412
������:		�޸ı��ֶ�
��������:	�޸ı��ֶ�
**********************************/

int UnionDealServiceCode0412(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	tableName[40+1];
	int	fieldID = 0;
	char	fieldName[40+1];
	char	fieldChnName[40+1];
	char	fieldType[40+1];
	char	fieldSize[10+1];
	char	defaultValue[128+1];
	char	isPrimaryKey[1+1];
	char	isNull[1+1];
	char	fieldLimitMethod[2048+1];
	char	fieldControl[2048+1];
	char	fieldValueMethod[2048+1];
	char	varNameOfEnabled[30+1];
	char	varValueOfEnabled[128+1];
	char	remark[128+1];
	char	fileName[1024+1];
	char	tmpBuf[128+1];

	// ��ȡ����
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0412:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(tableName);
	if (strlen(tableName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0412:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/tableName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// �����Ƿ��Ѿ�����
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTableDef(tableName,fileName);

	// �ļ�������
	if (access(fileName,0) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0412:: fileName[%s] not found!\n",fileName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// ��ȡ�ֶ�ID
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldID",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0412:: UnionReadRequestXMLPackageValue[%s]!\n","body/fieldID");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(tmpBuf);
	if (strlen(tmpBuf) == 0 || atoi(tmpBuf) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0412:: fieldID[%s] is invalid!\n",tmpBuf);
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}	
	else
		fieldID = atoi(tmpBuf);	
	
	// ��ȡ�ֶ���
	memset(fieldName,0,sizeof(fieldName));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldName",fieldName,sizeof(fieldName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0412:: UnionReadRequestXMLPackageValue[%s]!\n","body/fieldName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(fieldName);
	if (strlen(fieldName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0412:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/fieldName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}


	// ��ʼ���ļ�
	if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0412:: UnionInitXMLPackage[%s]!\n",fileName);
		return(ret);
	}

	// ָ���ֶ�λ�� 
	if ((ret = UnionLocateXMLPackage("field",fieldID)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0412:: field[%d] not found!\n",fieldID);
		return(errCodeObjectMDL_FieldNotExist);
	}
	
	// �����ֶ���
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("fieldName",tmpBuf,sizeof(tmpBuf))) > 0)
	{
		// �޸��ֶ���
		if ((ret = UnionSetXMLPackageValue("fieldName",fieldName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0412:: UnionSetXMLPackageValue[%s]!\n","fieldName");
			return(ret);
		}
	}
		
	// ��ȡ�ֶ�������
	memset(fieldChnName,0,sizeof(fieldChnName));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldChnName",fieldChnName,sizeof(fieldChnName))) > 0)
	{
		// �޸��ֶ�������
		if ((ret = UnionSetXMLPackageValue("fieldChnName",fieldChnName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0412:: UnionSetXMLPackageValue[%s]!\n","fieldChnName");
			return(ret);
		}
	}
	// ��ȡ�ֶ�����
	memset(fieldType,0,sizeof(fieldType));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldType",fieldType,sizeof(fieldType))) > 0)
	{
		// �޸��ֶ�����
		if ((ret = UnionSetXMLPackageValue("fieldType",fieldType)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0412:: UnionSetXMLPackageValue[%s]!\n","fieldType");
			return(ret);
		}
	}

	// ��ȡ�ֶδ�С
	memset(fieldSize,0,sizeof(fieldSize));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldSize",fieldSize,sizeof(fieldSize))) > 0)
	{
		if (atoi(fieldSize) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0412:: fieldSize[%s] is invalid!\n",fieldSize);
			return(errCodeObjectMDL_FieldValueIsInvalid);
		}
		// �޸��ֶδ�С
		if ((ret = UnionSetXMLPackageValue("fieldSize",fieldSize)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0412:: UnionSetXMLPackageValue[%s]!\n","fieldSize");
			return(ret);
		}
	}

	// ��ȡĬ��ֵ
	memset(defaultValue,0,sizeof(defaultValue));
	if ((ret = UnionReadRequestXMLPackageValue("body/defaultValue",defaultValue,sizeof(defaultValue))) >= 0)
	{
		// �޸�Ĭ��ֵ
		if (ret == 0)
			strcpy(defaultValue,"");
		if ((ret = UnionSetXMLPackageValue("defaultValue",defaultValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0412:: UnionSetXMLPackageValue[%s]!\n","defaultValue");
			return(ret);
		}
	}

	// ��ȡ�Ƿ�ؼ���
	memset(isPrimaryKey,0,sizeof(isPrimaryKey));
	if ((ret = UnionReadRequestXMLPackageValue("body/isPrimaryKey",isPrimaryKey,sizeof(isPrimaryKey))) > 0)
	{
		// �޸��Ƿ�ؼ���
		if ((ret = UnionSetXMLPackageValue("isPrimaryKey",isPrimaryKey)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0412:: UnionSetXMLPackageValue[%s]!\n","isPrimaryKey");
			return(ret);
		}
	}

	// ��ȡ�Ƿ�Ϊ��
	memset(isNull,0,sizeof(isNull));
	if ((ret = UnionReadRequestXMLPackageValue("body/isNull",isNull,sizeof(isNull))) > 0)
	{
		// �޸��Ƿ�Ϊ��
		if ((ret = UnionSetXMLPackageValue("isNull",isNull)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0412:: UnionSetXMLPackageValue[%s]!\n","isNull");
			return(ret);
		}
	}	

	// �ֶ����Ʒ���
	memset(fieldLimitMethod,0,sizeof(fieldLimitMethod));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldLimitMethod",fieldLimitMethod,sizeof(fieldLimitMethod))) >= 0)
	{
		// �޸��ֶ����Ʒ���
		if (ret == 0)
			strcpy(fieldLimitMethod,"");
		if ((ret = UnionSetXMLPackageValue("fieldLimitMethod",fieldLimitMethod)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0412:: UnionSetXMLPackageValue[%s]!\n","fieldLimitMethod");
			return(ret);
		}
	}
	
	// �ֶοؼ�����
	memset(fieldControl,0,sizeof(fieldControl));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldControl",fieldControl,sizeof(fieldControl))) >= 0)
	{
		// �޸��ֶοؼ�����
		if (ret == 0)
			strcpy(fieldControl,"");
		if ((ret = UnionSetXMLPackageValue("fieldControl",fieldControl)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0412:: UnionSetXMLPackageValue[%s]!\n","fieldControl");
			return(ret);
		}
	}
		
	// �ֶ����Ʒ���
	memset(fieldValueMethod,0,sizeof(fieldValueMethod));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldValueMethod",fieldValueMethod,sizeof(fieldValueMethod))) >= 0)
	{
		// �޸��ֶ����Ʒ���
		if (ret == 0)
			strcpy(fieldValueMethod,"");
		if ((ret = UnionSetXMLPackageValue("fieldValueMethod",fieldValueMethod)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0412:: UnionSetXMLPackageValue[%s]!\n","fieldValueMethod");
			return(ret);
		}
	}

	// ���ñ�����
	memset(varNameOfEnabled,0,sizeof(varNameOfEnabled));
	if ((ret = UnionReadRequestXMLPackageValue("body/varNameOfEnabled",varNameOfEnabled,sizeof(varNameOfEnabled))) >= 0)
	{
		// �޸����ñ�����
		if (ret == 0)
			strcpy(varNameOfEnabled,"");
		if ((ret = UnionSetXMLPackageValue("varNameOfEnabled",varNameOfEnabled)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0412:: UnionSetXMLPackageValue[%s]!\n","varNameOfEnabled");
			return(ret);
		}
	}
	
	// ���ñ�����
	memset(varValueOfEnabled,0,sizeof(varValueOfEnabled));
	if ((ret = UnionReadRequestXMLPackageValue("body/varValueOfEnabled",varValueOfEnabled,sizeof(varValueOfEnabled))) >= 0)
	{
		// �޸����ñ�����
		if (ret == 0)
			strcpy(varValueOfEnabled,"");
		if ((ret = UnionSetXMLPackageValue("varValueOfEnabled",varValueOfEnabled)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0412:: UnionSetXMLPackageValue[%s]!\n","varValueOfEnabled");
			return(ret);
		}
	}
	
	// ��ȡ��ע
	memset(remark,0,sizeof(remark));
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",remark,sizeof(remark))) >= 0)
	{
		UnionLog("in UnionDealServiceCode0412:: [%s][%d]\n",remark,ret);
		// �޸ı�ע
		if (ret == 0)
			strcpy(remark,"");

		if ((ret = UnionSetXMLPackageValue("remark",remark)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0412:: UnionSetXMLPackageValue[%s]!\n","remark");
			return(ret);
		}

		UnionLog("in UnionDealServiceCode0412:: [%s][%d][%s]\n",tmpBuf,ret,remark);
	}

	// д��XML�ļ�
	if ((ret = UnionXMLPackageToFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0412:: UnionXMLPackageToFile[%s]!\n",fileName);
		return(ret);
	}

	// ���ص������ڴ�
	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0412:: UnionReloadTableDefTBL!\n");
		return(ret);
	}
	return 0;
}
