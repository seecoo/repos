
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
�������:	0402
������:		�޸ı�	
��������:	�޸ı�	
**********************************/

int UnionDealServiceCode0402(PUnionHsmGroupRec phsmGroupRec)
{
	int	i;
	int	ret;
	int	flag = 0;
	int	isUpdateCascade = 0;
	int	isDeleteCascade = 0;
	char	tableName[40+1];
	char	newTableName[40+1];
	char	tableChnName[40+1];
	char	tableType[128+1];
	char	methodOfCached[1+1];
	char	remark[128+1];
	char	fileName[128+1];
	char	newFileName[128+1];
	char	tmpBuf[512+1];

	// ��ȡ����
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);	
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/tableName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}
	

	// �����Ƿ��Ѿ�����
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTableDef(tableName,fileName);

	// �ļ�������
	if (access(fileName,0) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: fileName[%s] not found!\n",fileName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// ��ȡ�±���
	memset(newTableName,0,sizeof(newTableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/newTableName",newTableName,sizeof(newTableName))) > 0)
	{
		memset(newFileName,0,sizeof(newFileName));
		UnionGetFileNameOfTableDef(newTableName,newFileName);
		
		if (strcmp(tableName,newTableName) != 0)
		{	
			if (access(newFileName,0) == 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0402:: newFileName[%s] already exist!\n",newFileName);
				return(errCodeDatabaseMDL_TableAlreadyExist);
			}	
			if ((ret = rename(fileName,newFileName)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0402:: rename [%s] to [%s] is failed!\n",fileName,newFileName);
				return(ret);
			}
			flag = 1;
		}
	}

	// ��ȡ��������
	memset(tableChnName,0,sizeof(tableChnName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableChnName",tableChnName,sizeof(tableChnName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableChnName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/tableChnName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}


	// ��ȡ������
	memset(tableType,0,sizeof(tableType));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableType",tableType,sizeof(tableType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableType");
		return(ret);	
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/tableType");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// ��ȡ���ٻ���ʱ��
	memset(methodOfCached,0,sizeof(methodOfCached));
	if ((ret = UnionReadRequestXMLPackageValue("body/methodOfCached",methodOfCached,sizeof(methodOfCached))) < 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%s]!\n","body/methodOfCached");
		strcpy(methodOfCached,"0");	
	}

	// ��ȡ��ע
	memset(remark,0,sizeof(remark));
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",remark,sizeof(remark))) < 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%s]!\n","body/remark");
		//return(ret);
		strcpy(remark,"");
	}

	// ��ʼ���ļ�	
	if (flag == 1)
	{
		memset(fileName,0,sizeof(fileName));
		strcpy(fileName,newFileName);
	}
	if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionInitXMLPackage[%s]!\n",fileName);
		return(ret);
	}

	// �޸ı�������
	if ((ret = UnionSetXMLPackageValue("tableChnName",tableChnName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","tableChnName");
		return(ret);
	}

	// �޸ı�����
	if ((ret = UnionSetXMLPackageValue("tableType",tableType)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","tableType");
		return(ret);
	}

	// �޸Ļ��淽��
	if ((ret = UnionSetXMLPackageValue("methodOfCached",methodOfCached)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","methodOfCached");
		return(ret);
	}

	// �޸ı�ע
	if ((ret = UnionSetXMLPackageValue("remark",remark)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","remark");
		return(ret);
	}

	// ��ȡΨһֵ�ֶ�
	for (i = 0;;i++)
	{
		// ��ȡ��Ψһֵ�ֶδ���Ԥ�Ƶ�
		if (i >= 16)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: uniqueNum [%d]  > 16!\n",i+1);
			return(errCodeObjectMDL_FieldNumberTooMuch);
		}

		if ((ret = UnionLocateRequestXMLPackage("body/unique", i+1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionLocateRequestXMLPackage unique[%d] not found!\n",i+1);
			break;
		}

		UnionLocateNewXMLPackage("unique",i+1);

		// ��ȡΨһֵ����
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("name",tmpBuf,sizeof(tmpBuf))) <= 0)
		{
			strcpy(tmpBuf,"");
		}
		// �޸�Ψһֵ����
		if ((ret = UnionSetXMLPackageValue("name",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","unique/name");
			return(ret);	
		}
		
		// ��ȡfields
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("fields",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402::  UnionReadRequestXMLPackageValue[%d][%s]\n",i+1,"unique/fields");
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%d] [%s] can not be null!\n",i+1,"unique/fields");
			return(errCodeObjectMDL_FieldValueIsInvalid);
		}
		
		// �޸�Ψһֵ
		if ((ret = UnionSetXMLPackageValue("fields",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","fields");
			return(ret);	
		}
	}

	// ��ȡ����
	for (i = 0;;i++)
	{
		// ��ȡ������������Ԥ�Ƶ�
		if (i >= 16)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: indexNum [%d]  > 16!\n",i+1);
			return(errCodeObjectMDL_FieldNumberTooMuch);
		}

		if ((ret = UnionLocateRequestXMLPackage("body/index", i+1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionLocateRequestXMLPackage index[%d] not found!\n",i+1);
			break;
		}

		UnionLocateNewXMLPackage("index",i+1);

		// ��ȡ��������
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("name",tmpBuf,sizeof(tmpBuf))) <= 0)
		{
			strcpy(tmpBuf,"");
		}

		// �޸���������
		if ((ret = UnionSetXMLPackageValue("name",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","index/name");
			return(ret);	
		}

		// ��ȡfields
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("fields",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402::  UnionReadRequestXMLPackageValue[%d][%s] index\n",i+1,"fields");
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%d] [%s] can not be null!\n",i+1,"index/fields");
			return(errCodeObjectMDL_FieldValueIsInvalid);
		}
		// �޸�����
		if ((ret = UnionSetXMLPackageValue("fields",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","fields");
			return(ret);	
		}
	}
	
	// ��ȡ���
	for (i = 0;;i++)
	{
		// �趨�����������Ԥ�Ƶ�
		if (i >= 16)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: foreignkeyNum [%d]  > 16!\n",i+1);
			return(errCodeObjectMDL_FieldNumberTooMuch);
		}
		
		if ((ret = UnionLocateRequestXMLPackage("body/foreignkey", i+1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionLocateRequestXMLPackage foreign[%d] not found!\n",i+1);
			break;
		}

		UnionLocateNewXMLPackage("foreignkey",i+1);

		// ��ȡ�������
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("name",tmpBuf,sizeof(tmpBuf))) <= 0)
		{
			strcpy(tmpBuf,"");
		}
		// �޸��������
		if ((ret = UnionSetXMLPackageValue("name",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","foreignkey/name");
			return(ret);	
		}
		// ��ȡ����еĲ��ձ���
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("references",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402::  UnionReadRequestXMLPackageValue[%d][%s]\n",i+1,"references");
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%d] [%s] can not be null!\n",i+1,"references");
			return(errCodeObjectMDL_FieldValueIsInvalid);
		}

		// �޸�����еĲ��ձ���
		if ((ret = UnionSetXMLPackageValue("references",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","references");
			return(ret);	
		}

		// ��ȡ����б��ر���ֶ���
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("localfields",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%d][%s]\n",i+1,"localfields");
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%d] [%s] can not be null!\n",i+1,"localfields");
			return(errCodeObjectMDL_FieldValueIsInvalid);
		}

		// �޸�����еĲ��ձ���
		if ((ret = UnionSetXMLPackageValue("localfields",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","localfields");
			return(ret);	
		}

		// ��ȡ����в��ձ���ֶ���
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("foreignfields",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%d][%s]\n",i+1,"foreignfields");
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionReadRequestXMLPackageValue[%d] [%s] can not be null!\n",i+1,"foreignfields");
			return(errCodeObjectMDL_FieldValueIsInvalid);
		}

		// �޸�����еĲ��ձ���
		if ((ret = UnionSetXMLPackageValue("foreignfields",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","foreignfields");
			return(ret);	
		}

		// ��ȡ����еļ�������
		// ��ȡ�����������
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("isUpdateCascade",tmpBuf,sizeof(tmpBuf))) <= 0)
		{
			strcpy(tmpBuf,"");
		}
		else if (atoi(tmpBuf) == 1 || atoi(tmpBuf) == 0)
			isUpdateCascade = atoi(tmpBuf);
		else
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: isUpdateCascade[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}

		// ��ȡ�������ɾ��
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("isDeleteCascade",tmpBuf,sizeof(tmpBuf))) <= 0)
		{
			strcpy(tmpBuf,"");
		}
		else if (atoi(tmpBuf) == 1 || atoi(tmpBuf) == 0)
			isDeleteCascade = atoi(tmpBuf);
		else
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: isDeleteCascade[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}

		// �޸�����еļ�������
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",isUpdateCascade);
		if ((ret = UnionSetXMLPackageValue("isUpdateCascade",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","isUpdateCascade");
			return(ret);	
		}

		// �޸�����еļ���ɾ��
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",isDeleteCascade);
		if ((ret = UnionSetXMLPackageValue("isDeleteCascade",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0402:: UnionSetXMLPackageValue[%s]!\n","isDeleteCascade");
			return(ret);	
		}

	}

	// д��XML�ļ�
	if ((ret = UnionXMLPackageToFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionXMLPackageToFile[%s]!\n",fileName);
		return(ret);
	}

	// ���ص������ڴ�
	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0402:: UnionReloadTableDefTBL!\n");
		return(ret);
	}
	return 0;
}
