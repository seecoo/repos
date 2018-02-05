
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
�������:	0401
������:		���ӱ�
��������:	���ӱ�
**********************************/

int UnionDealServiceCode0401(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	tableName[40+1];
	char	tableChnName[40+1];
	char	tableType[128+1];
	char	remark[128+1];
	char	fileName[128+1];
	char	methodOfCached[1+1];

	// ��ȡ����
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);	
	}
	UnionFilterHeadAndTailBlank(tableName);
	if (strlen(tableName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: tableName can not be null!\n");
		UnionSetResponseRemark("��������Ϊ��");
		return(errCodeParameter);
	}

	// �����Ƿ��Ѿ�����
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTableDef(tableName,fileName);
	
	// �ļ��Ѿ�����
	if (access(fileName,0) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: fileName[%s] already exist!\n",fileName);
		return(errCodeDatabaseMDL_TableAlreadyExist);	
	}
	
	// ��ȡ��������
	memset(tableChnName,0,sizeof(tableChnName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableChnName",tableChnName,sizeof(tableChnName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableChnName");
		return(ret);	
	}
	UnionFilterHeadAndTailBlank(tableChnName);
	if (strlen(tableChnName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: tableChnName can not be null!\n");
		UnionSetResponseRemark("������������Ϊ��");
		return(errCodeParameter);
	}

	// ��ȡ������
	memset(tableType,0,sizeof(tableType));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableType",tableType,sizeof(tableType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableType");
		return(ret);	
	}
	UnionFilterHeadAndTailBlank(tableType);	
	if (strlen(tableType) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: tableType can not be null!\n");
		UnionSetResponseRemark("�����Ͳ���Ϊ��");
		return(errCodeParameter);
	}

	// ��ȡ���淽��
	memset(methodOfCached,0,sizeof(methodOfCached));
	if ((ret = UnionReadRequestXMLPackageValue("body/methodOfCached",methodOfCached,sizeof(methodOfCached))) < 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode0401:: UnionReadRequestXMLPackageValue[%s]!\n","body/methodOfCached");
		strcpy(methodOfCached,"");
	}
	UnionFilterHeadAndTailBlank(methodOfCached);

	// ��ȡ��ע
	memset(remark,0,sizeof(remark));
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",remark,sizeof(remark))) <= 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode0401:: UnionReadRequestXMLPackageValue[%s]!\n","body/remark");
		//return(ret);
		strcpy(remark,"");
	}

	// ��ʼ��XML��
	if ((ret = UnionInitXMLPackage(NULL,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: UnionInitXMLPackage!\n");
		return(ret);	
	}

	// ���ӱ�������
	if ((ret = UnionSetXMLPackageValue("tableChnName",tableChnName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: UnionSetXMLPackageValue[%s]!\n","tableChnName");
		return(ret);	
	}
	// ���ӱ����� 
	if ((ret = UnionSetXMLPackageValue("tableType",tableType)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: UnionSetXMLPackageValue[%s]!\n","tableType");
		return(ret);	
	}
	// ���ӻ��淽�� 
	if (strlen(methodOfCached) > 0)
	{
		if ((ret = UnionSetXMLPackageValue("methodOfCached",methodOfCached)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0401:: UnionSetXMLPackageValue[%s]!\n","methodOfCached");
			return(ret);	
		}
	}
	// ���ӱ�ע 
	if ((ret = UnionSetXMLPackageValue("remark",remark)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: UnionSetXMLPackageValue[%s]!\n","remark");
		return(ret);	
	}

	// д��XML�ļ�
	if ((ret = UnionXMLPackageToFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: UnionXMLPackageToFile[%s]!\n",fileName);
		return(ret);
	}

	// ���ص������ڴ�
	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0401:: UnionReloadTableDefTBL!\n");
		return(ret);
	}
	return 0;
}
