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

#include "symmetricKeyDB.h"
#include "symmetricKeyDBJnl.h"
#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"

/* 
���ܣ�	�ǼǶԳ���Կ������־
������	operation[in]		������ʶ
	psymmetricKeyDB[in]	��Կ������Ϣ
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionInsertSymmetricKeyDBJnl(TUnionSymmetricKeyDBOperation operation,PUnionSymmetricKeyDB psymmetricKeyDB)
{
	int	i;
	//int	len;
	int	ret;
	int	operatorType;
	char	sql[2048];
	char	operTime[32];
	char	sysID[32];
	char	appID[32];
	char	operator[64];
	
	// ��ȡsysID
	//memset(sysID,0,sizeof(sysID));
	if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
	{
		UnionUserErrLog("in UnionInsertSymmetricKeyDBJnl:: UnionReadRequestXMLPackageValue[head/sysID]! ret = [%d]\n",ret);
		return(ret);
	}
	
	//memset(operator,0,sizeof(operator));
	if ((memcmp(sysID,"CUI",3) == 0) || (memcmp(sysID,"BUI",3) == 0))
	{
		operatorType = 0;
		if ((ret = UnionReadRequestXMLPackageValue("head/userID",operator,sizeof(operator))) < 0)
		{
			UnionUserErrLog("in UnionInsertSymmetricKeyDBJnl:: UnionReadRequestXMLPackageValue[head/userID]! ret = [%d]\n",ret);
			return(ret);
		}
	}
	else
	{
		operatorType = 1;
		//memset(appID,0,sizeof(appID));
		if ((ret = UnionReadRequestXMLPackageValue("head/appID",appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionInsertSymmetricKeyDBJnl:: UnionReadRequestXMLPackageValue[head/appID]! ret = [%d]\n",ret);
			return(ret);
		}
		UnionPieceSymmetricKeyCreator(sysID,appID,operator);
	}
		
	switch(operation)
	{
		case	conSymmetricKeyDBOperationInsert:
			snprintf(operTime,sizeof(operTime),"%s",psymmetricKeyDB->createTime);
			break;
		case	conSymmetricKeyDBOperationUpdate:
			snprintf(operTime,sizeof(operTime),"%s",psymmetricKeyDB->keyUpdateTime);
			break;
		case	conSymmetricKeyDBOperationDelete:
		default:
			UnionGetFullSystemDateTime(operTime);
			operTime[14] = 0;
			break;
	}

	for (i = 0; i < maxNumOfSymmetricKeyValue; i++)
	{
		if (strlen(psymmetricKeyDB->keyValue[i].lmkProtectMode) == 0)
			break;

		snprintf(sql,sizeof(sql),"insert into symmetricKeyDBJnl(operTime,keyName,operation,lmkProtectMode,keyValue,checkValue,oldKeyValue,oldCheckValue,operatorType,operator)values('%s','%s',%d,'%s','%s','%s','%s','%s',%d,'%s')",
			operTime,
			psymmetricKeyDB->keyName,
			operation,
			psymmetricKeyDB->keyValue[i].lmkProtectMode,
			psymmetricKeyDB->keyValue[i].keyValue,
			psymmetricKeyDB->checkValue,
			psymmetricKeyDB->keyValue[i].oldKeyValue,
			psymmetricKeyDB->oldCheckValue,
			operatorType,
			operator);
		
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionInsertSymmetricKeyDBJnl:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
			return(ret);
		}
	}
	return(ret);
}

/* 
���ܣ�	��ȡ�ǼǶԳ���Կ������־��SQL���
������	operation[in]		������ʶ
	psymmetricKeyDB[in]	��Կ������Ϣ
	sql[out]		sql������
	sizeOfSql		sql��������С
����ֵ��>=0			�ɹ�
	<0			ʧ�ܣ����ش�����
*/
int UnionGetSqlForSymmetricKeyDBJnl(TUnionSymmetricKeyDBOperation operation,PUnionSymmetricKeyDB psymmetricKeyDB,char *sql,int sizeOfSql)
{
	int	i;
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
		UnionPieceSymmetricKeyCreator(pxmlPackageHead->sysID,pxmlPackageHead->appID,operator);
		poperator = operator;
	}
		
	switch(operation)
	{
		case	conSymmetricKeyDBOperationInsert:
			poperTime = psymmetricKeyDB->createTime;
			break;
		case	conSymmetricKeyDBOperationUpdate:
			poperTime = psymmetricKeyDB->keyUpdateTime;
			break;
		case	conSymmetricKeyDBOperationDelete:
		default:
			poperTime = UnionGetCurrentFullSystemDateTime();
			break;
	}

	for (i = 0; i < maxNumOfSymmetricKeyValue; i++)
	{
		if (strlen(psymmetricKeyDB->keyValue[i].lmkProtectMode) == 0)
			break;

		len += snprintf(sql + len,sizeOfSql - len,"insert into symmetricKeyDBJnl(operTime,keyName,operation,lmkProtectMode,keyValue,checkValue,oldKeyValue,oldCheckValue,operatorType,operator)values('%s','%s',%d,'%s','%s','%s','%s','%s',%d,'%s');",
			poperTime,
			psymmetricKeyDB->keyName,
			operation,
			psymmetricKeyDB->keyValue[i].lmkProtectMode,
			psymmetricKeyDB->keyValue[i].keyValue,
			psymmetricKeyDB->checkValue,
			psymmetricKeyDB->keyValue[i].oldKeyValue,
			psymmetricKeyDB->oldCheckValue,
			operatorType,
			poperator);		
	}
	return(len);
}

