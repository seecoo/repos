
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
�������:	0417
������:		ɾ��������
��������:	ɾ��������
**********************************/

int UnionDealServiceCode0417(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret =0;
	int	i = 0;
	int	flag = 0;
	int	maxID = 0;
	int	typeID = 0;
	char	tableName[40+1];
	char	typeName[40+1];
	char	fileName[1024+1];
	char	tmpBuf[128+1];

	// ��ȡ����
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0417:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0417:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/tableName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// �����Ƿ��Ѿ�����
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTableDef(tableName,fileName);

	// �ļ�������
	if (access(fileName,0) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0417:: fileName[%s] not found!\n",fileName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// ��ȡflag
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/flag",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0417:: UnionReadRequestXMLPackageValue[%s]!\n","body/tmpBuf");
		return(ret);
	}
	else if (ret == 0 || atoi(tmpBuf) <= 0 || atoi(tmpBuf) >3)
	{
		UnionUserErrLog("in UnionDealServiceCode0417:: flag[%d] error!\n",atoi(tmpBuf));
		return(errCodeParameter);
	}
	else
		flag = atoi(tmpBuf);

	memset(typeName,0,sizeof(typeName));
	switch(flag)
	{
		case	1:
			strcpy(typeName,"index");
			break;
		case	2:
			strcpy(typeName,"unique");
			break;
		case	3:
			strcpy(typeName,"foreignkey");
			break;
		default	:
			UnionUserErrLog("in UnionDealServiceCode0417:: flag[%d] error!\n",flag);
			return(errCodeParameter);
	}

	// ��ȡtypeID
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/typeID",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0417:: UnionReadRequestXMLPackageValue[%s]!\n","body/tmpBuf");
		return(ret);
	}
	else if (ret == 0 || atoi(tmpBuf) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0417:: typeID[%d]!\n",atoi(tmpBuf));
		return(errCodeParameter);
	}
	else
		typeID = atoi(tmpBuf);

	// ��ʼ���ļ�	
	if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0417:: UnionInitXMLPackage[%s]!\n",fileName);
		return(ret);
	}

	// ����Ҫɾ���ı��ֶ�ID 
	for (i = 1; i <= 20; i++)
	{
		if ((ret = UnionLocateXMLPackage(typeName,i)) < 0)
		{
			if (i == 1)
			{
				UnionUserErrLog("in UnionDealServiceCode0417:: typeName[%s][1] not found!\n",typeName);
				return(errCodeParameter);
			}
			if (typeID >= i)
			{
				UnionUserErrLog("in UnionDealServiceCode0417:: [%s][%d] not found!\n",typeName,typeID);
				return(errCodeParameter);
			}
			// ��¼���һ��IDֵ
			maxID = i - 1;
			break;
		}
	}

	UnionLocateXMLPackage("",0);
	
	// ����ID
	for (i = maxID - 1; i >= typeID; i--)
	{
		if((ret = UnionExchangeIDXMLPackage(typeName,i,maxID)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0417:: UnionExchangeIDXMLPackage [%s][%d] failed!\n",typeName,i);
			return(ret);
		}
		
	}

	// ɾ��ָ���ı��ֶ�
	if ((ret = UnionDeleteXMLPackageNode(typeName,maxID)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0417:: UnionDeleteXMLPackageNode node[%s][%d]\n",typeName,maxID);
		return(ret);
	}

	// д��XML�ļ�
	if ((ret = UnionXMLPackageToFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0417:: UnionXMLPackageToFile[%s]!\n",fileName);
		return(ret);
	}

	// ���ص������ڴ�
	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0417:: UnionReloadTableDefTBL!\n");
		return(ret);
	}
	return 0;
}
