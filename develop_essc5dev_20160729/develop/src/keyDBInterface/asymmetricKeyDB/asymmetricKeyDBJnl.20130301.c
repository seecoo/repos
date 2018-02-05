//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-07-24

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include "UnionLog.h"
#include "unionErrCode.h"

#include "asymmetricKeyDB.h"
#include "asymmetricKeyDBJnl.h"
#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"

/* 
���ܣ�	�ǼǷǶԳ���Կ������־
������	operation[in]		������ʶ
	pasymmetricKeyDB[in]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionInsertAsymmetricKeyDBJnl(TUnionAsymmetricKeyDBOperation operation,PUnionAsymmetricKeyDB pasymmetricKeyDB)
{
	int	ret;
	int	operatorType;
	char	sql[8192];
	char	operTime[32];
	char	sysID[32];
	char	appID[32];
	char	operator[64];
	
	// ��ȡsysID
	if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
	{
		UnionUserErrLog("in UnionInsertAsymmetricKeyDBJnl:: UnionReadRequestXMLPackageValue[head/sysID]! ret = [%d]\n",ret);
		return(ret);
	}
	
	if ((memcmp(sysID,"CUI",3) == 0) || (memcmp(sysID,"BUI",3) == 0))
	{
		operatorType = 0;
		if ((ret = UnionReadRequestXMLPackageValue("head/userID",operator,sizeof(operator))) < 0)
		{
			UnionUserErrLog("in UnionInsertAsymmetricKeyDBJnl:: UnionReadRequestXMLPackageValue[head/userID]! ret = [%d]\n",ret);
			return(ret);
		}
	}
	else
	{
		operatorType = 1;
		if ((ret = UnionReadRequestXMLPackageValue("head/appID",appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionInsertAsymmetricKeyDBJnl:: UnionReadRequestXMLPackageValue[head/appID]! ret = [%d]\n",ret);
			return(ret);
		}
		UnionPieceAsymmetricKeyCreator(sysID,appID,operator);
	}
	
	switch(operation)
	{
		case	conAsymmetricKeyDBOperationInsert:
			strcpy(operTime,pasymmetricKeyDB->createTime);
			break;
		case	conAsymmetricKeyDBOperationUpdate:
			strcpy(operTime,pasymmetricKeyDB->keyUpdateTime);
			break;
		case	conAsymmetricKeyDBOperationDelete:
		default:
			UnionGetFullSystemDateTime(operTime);
			break;
	}
	
	snprintf(sql,sizeof(sql),"insert into asymmetricKeyDBJnl(operTime,keyName,operation,pkValue,pkCheckValue,vkValue,vkCheckValue,oldPKValue,oldPKCheckValue,oldVKValue,oldVKCheckValue,operatorType,operator)values('%s','%s',%d,'%s','%s','%s','%s','%s','%s','%s','%s',%d,'%s')",
		operTime,
		pasymmetricKeyDB->keyName,
		operation,
		pasymmetricKeyDB->pkValue,
		pasymmetricKeyDB->pkCheckValue,
		pasymmetricKeyDB->vkValue,
		pasymmetricKeyDB->vkCheckValue,
		pasymmetricKeyDB->oldPKValue,
		pasymmetricKeyDB->oldPKCheckValue,
		pasymmetricKeyDB->oldVKValue,
		pasymmetricKeyDB->oldVKCheckValue,
		operatorType,
		operator);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionInsertAsymmetricKeyDBJnl:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
		return(ret);
	}
		
	return(ret);
}

/* 
���ܣ�	��ȡ�ǼǷǶԳ���Կ������־��SQL���
������	operation[in]		������ʶ
	pasymmetricKeyDB[in]	��Կ������Ϣ
	sql[out]		sql������
	sizeOfSql		sql��������С
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionGetSqlForAsymmetricKeyDBJnl(TUnionAsymmetricKeyDBOperation operation,PUnionAsymmetricKeyDB pasymmetricKeyDB,char *sql,int sizeOfSql)
{
	int	len = 0;
	int	operatorType;
	char	*poperTime = NULL;
	char	operator[64];
	char	*poperator = NULL;
	PUnionXMLPackageHead	pxmlPackageHead = UnionGetXMLPackageHead();
	
	if (pxmlPackageHead->isUI)
	{
		operatorType = 0;
		poperator = pxmlPackageHead->userID;
	}
	else
	{
		operatorType = 1;
		UnionPieceAsymmetricKeyCreator(pxmlPackageHead->sysID,pxmlPackageHead->appID,operator);
		poperator = operator;
	}

	switch(operation)
	{
		case	conAsymmetricKeyDBOperationInsert:
			poperTime = pasymmetricKeyDB->createTime;
			break;
		case	conAsymmetricKeyDBOperationUpdate:
			poperTime = pasymmetricKeyDB->keyUpdateTime;
			break;
		case	conAsymmetricKeyDBOperationDelete:
		default:
			poperTime = UnionGetCurrentFullSystemDateTime();
			break;
	}
	
	len = snprintf(sql,sizeOfSql,"insert into asymmetricKeyDBJnl(operTime,keyName,operation,pkValue,pkCheckValue,vkValue,vkCheckValue,oldPKValue,oldPKCheckValue,oldVKValue,oldVKCheckValue,operatorType,operator)values('%s','%s',%d,'%s','%s','%s','%s','%s','%s','%s','%s',%d,'%s');",
		poperTime,
		pasymmetricKeyDB->keyName,
		operation,
		pasymmetricKeyDB->pkValue,
		pasymmetricKeyDB->pkCheckValue,
		pasymmetricKeyDB->vkValue,
		pasymmetricKeyDB->vkCheckValue,
		pasymmetricKeyDB->oldPKValue,
		pasymmetricKeyDB->oldPKCheckValue,
		pasymmetricKeyDB->oldVKValue,
		pasymmetricKeyDB->oldVKCheckValue,
		operatorType,
		poperator);
		
	return(len);	
}
