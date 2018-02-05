
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
#include "baseUIService.h"

/*********************************
�������:	0415
������:		�޸Ľ����ֶζ���
��������:	�޸Ľ����ֶζ���
**********************************/

int UnionDealServiceCode0415(PUnionHsmGroupRec phsmGroupRec)
{
	int	i;
	int	ret;
	char	tableName[40+1];
	char	fieldName[40+1];
	char	fieldLimitMethod[2048+1];
	char	fieldControl[2048+1];
	char	fieldValueMethod[2048+1];
	char	fileName[1024+1];
	char	tmpBuf[128+1];

	// ��ȡ����
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0415:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);	
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0415:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/tableName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}
	
	// �����Ƿ��Ѿ�����
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTableDef(tableName,fileName);
	
	// �ļ�������
	if (access(fileName,0) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0415:: fileName[%s] not found!\n",fileName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// ��ȡ�ֶ���
	memset(fieldName,0,sizeof(fieldName));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldName",fieldName,sizeof(fieldName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0415:: UnionReadRequestXMLPackageValue[%s]!\n","body/fieldName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0415:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/fieldName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}


	// ��ʼ���ļ�
	if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0415:: UnionInitXMLPackage[%s]!\n",fileName);
		return(ret);	
	}

	// ���Ҳ��޸Ľ����ֶζ���
	for (i = 1;;i++)
	{
		if ((ret = UnionLocateXMLPackage("field",i)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0415:: field[%d][%s] not found!\n",i,fieldName);
			return(errCodeObjectMDL_FieldNotExist);
		}

		// �����ֶ���
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("fieldName",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0415:: UnionReadXMLPackageValue[%s]!\n","body/fieldName");
			return(ret);
		}

		// �Ա��ֶ����Ƿ���� 
		if (strcmp(fieldName,tmpBuf) == 0)
		{
			// ��ȡ�ֶ����Ʒ���
			memset(fieldLimitMethod,0,sizeof(fieldLimitMethod));
			if ((ret = UnionReadRequestXMLPackageValue("body/fieldLimitMethod",fieldLimitMethod,sizeof(fieldLimitMethod))) >= 0)
			{
				if (ret == 0)
					strcpy(fieldLimitMethod,"");
				// �޸��ֶ����Ʒ���
				if ((ret = UnionSetXMLPackageValue("fieldLimitMethod",fieldLimitMethod)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode0415:: UnionSetXMLPackageValue[%s]!\n","fieldLimitMethod");
					return(ret);
				}
			}
	
			// ��ȡ�ֶοؼ�����
			memset(fieldControl,0,sizeof(fieldControl));
			if ((ret = UnionReadRequestXMLPackageValue("body/fieldControl",fieldControl,sizeof(fieldControl))) >=  0)
			{
				if (ret == 0)
					strcpy(fieldControl,"");
				// �޸��ֶοؼ�����
				if ((ret = UnionSetXMLPackageValue("fieldControl",fieldControl)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode0415:: UnionSetXMLPackageValue[%s]!\n","fieldControl");
					return(ret);
				}
			}
		
			// ��ȡ�ֶ�ȡֵ����
			memset(fieldValueMethod,0,sizeof(fieldValueMethod));
			if ((ret = UnionReadRequestXMLPackageValue("body/fieldValueMethod",fieldValueMethod,sizeof(fieldValueMethod))) >= 0)
			{
				if (ret == 0)
					strcpy(fieldValueMethod,"");
				// �޸��ֶ�ȡֵ����
				if ((ret = UnionSetXMLPackageValue("fieldValueMethod",fieldValueMethod)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode0415:: UnionSetXMLPackageValue[%s]!\n","fieldValueMethod");
					return(ret);
				}
			}

			break;
		}
		continue;
	}

	// д��XML�ļ�
	if ((ret = UnionXMLPackageToFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0415:: UnionXMLPackageToFile[%s]!\n",fileName);
		return(ret);
	}

	// ���ص������ڴ�
	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0415:: UnionReloadTableDefTBL!\n");
		return(ret);
	}
	return 0;
}
