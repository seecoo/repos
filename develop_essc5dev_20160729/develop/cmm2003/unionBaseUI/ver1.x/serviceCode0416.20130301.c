
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
�������:	0416
������:		�ƶ����ֶ�
��������:	�ƶ����ֶ�
**********************************/

int UnionDealServiceCode0416(PUnionHsmGroupRec phsmGroupRec)
{
	int	i;
	int	ret;
	int	curFieldID = 0;
	int	newFieldID = 0;
	char	tableName[40+1];
	char	fileName[1024+1];
	int	maxID = 0;
	char	tmpBuf[128+1];

	// ��ȡ����
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0416:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0416:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/tableName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}	

	// �����Ƿ��Ѿ�����
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTableDef(tableName,fileName);

	// �ļ�������
	if (access(fileName,0) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0416:: fileName[%s] not found!\n",fileName);
		return(errCodeDatabaseMDL_TableNotFound);
	}
	
	// ��ȡ�ֶε�ǰID
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/curFieldID",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0416:: UnionReadRequestXMLPackageValue[%s]!\n","body/curFieldID");
		return(ret);
	}
	else
		curFieldID = atoi(tmpBuf);

	if (ret == 0 || curFieldID == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0416:: UnionReadRequestXMLPackageValue[%s] can not be null or is invalid!\n","body/curFieldID");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// ��ȡ�ֶ���ID
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/newFieldID",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0416:: UnionReadRequestXMLPackageValue[%s]!\n","body/newFieldID");
		return(ret);
	}
	else
		newFieldID = atoi(tmpBuf);

	if (ret == 0 || newFieldID == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0416:: UnionReadRequestXMLPackageValue[%s] can not be null or is invalid!\n","body/newFieldID");
		return(errCodeObjectMDL_FieldValueIsInvalid);
	}

	// ��ʼ���ļ�
	if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0416:: UnionInitXMLPackage[%s]!\n",fileName);
		return(ret);
	}
	
	for (i = 1; ;i++)
	{
		if ((ret = UnionLocateXMLPackage("field",i)) < 0)
		{
			maxID = i - 1;
			if (maxID < curFieldID || maxID < newFieldID)
			{
				if (curFieldID < newFieldID)
					UnionUserErrLog("in UnionDealServiceCode0416:: maxID[%d] < newFieldID[%d]!\n",maxID,newFieldID);
				else
					UnionUserErrLog("in UnionDealServiceCode0416:: maxID[%d] < curFieldID[%d]!\n",maxID,curFieldID);
				return(errCodeObjectMDL_FieldValueIsInvalid);
			}
			break;
		}
	}
	
	UnionLocateXMLPackage("",0);

	// ����ָ��λ��
	if (curFieldID < newFieldID)
	{
		for (i = curFieldID; i < newFieldID; i++)
		{
			// ����ID
			if((ret = UnionExchangeIDXMLPackage("field",i,i+1)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0416:: UnionExchangeIDXMLPackage %d failed!\n",i);
				return(ret);
			}
		}
	}
	else if (curFieldID > newFieldID)
	{
		for (i = curFieldID; i > newFieldID; i--)
		{
			// ����ID
			if((ret = UnionExchangeIDXMLPackage("field",i-1,i)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0416:: UnionExchangeIDXMLPackage %d failed!\n",i);
				return(ret);
			}
		}
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
