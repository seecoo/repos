
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
�������:	0413
������:		ɾ�����ֶ�
��������:	ɾ�����ֶ�
**********************************/

int UnionDealServiceCode0413(PUnionHsmGroupRec phsmGroupRec)
{
	int	i;
	int	position = 0;
	int	maxID = 0;
	int	ret;
	char	tableName[40+1];
	char	fieldName[40+1];
	char	fileName[1024+1];
	char	tmpBuf[128+1];

	// ��ȡ����
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0413:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0413:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/tableName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// �����Ƿ��Ѿ�����
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTableDef(tableName,fileName);

	// �ļ�������
	if (access(fileName,0) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0413:: fileName[%s] not found!\n",fileName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// ��ȡ�ֶ���
	memset(fieldName,0,sizeof(fieldName));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldName",fieldName,sizeof(fieldName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0413:: UnionReadRequestXMLPackageValue[%s]!\n","body/fieldName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0413:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/fieldName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// ��ʼ���ļ�	
	if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0413:: UnionInitXMLPackage[%s]!\n",fileName);
		return(ret);
	}

	// ����Ҫɾ���ı��ֶ�ID 
	for (i = 1;;i++)
	{
		if ((ret = UnionLocateXMLPackage("field",i)) < 0)
		{
			// Ҫɾ�����ļ�������
			if (position == 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0413:: fieldName[%s] not found!\n",fieldName);
				return(errCodeObjectMDL_FieldNotExist);
			}
			// ��¼���һ��IDֵ
			maxID = i - 1;
			break;
		}

		// �����ֶ���
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("fieldName",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0413:: UnionReadXMLPackageValue[%s]!\n","body/fieldName");
			return(ret);
		}

		// �Ա��ֶ����Ƿ����
		if (strcmp(fieldName,tmpBuf) == 0)
		{
			// ��¼Ҫɾ��ֵ��IDֵ
			position = i;
		}
		continue;
	}

	UnionLocateXMLPackage("",0);
	
	// ����ID
	for (i = maxID - 1; i >= position; i--)
	{
		if((ret = UnionExchangeIDXMLPackage("field",i,maxID)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0413:: UnionExchangeIDXMLPackage %d failed!\n",i);
			return(ret);
		}
		
	}

	// ɾ��ָ���ı��ֶ�
	if ((ret = UnionDeleteXMLPackageNode("field",maxID)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0413:: UnionDeleteXMLPackageNode node[field][%d]\n",maxID);
		return(ret);
	}

	// д��XML�ļ�
	if ((ret = UnionXMLPackageToFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0413:: UnionXMLPackageToFile[%s]!\n",fileName);
		return(ret);
	}

	// ���ص������ڴ�
	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0413:: UnionReloadTableDefTBL!\n");
		return(ret);
	}
	return 0;
}
