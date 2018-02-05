//      Author:		zhouxw
//      Copyright:	Union Tech. Guangzhou
//      Date:		2016-07-21

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5UIService.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "asymmetricKeyDB.h"
#include "unionHsmCmd.h"

/***************************************
  �������:       8EE3
  ������:         ɾ��֤��
  ��������:       ɾ��֤��
 ***************************************/
int UnionDealServiceCode8EE3(PUnionHsmGroupRec phsmGroupRec)
{
	int                             ret;
	char                            sql[128];
	//int                             vkIdx = 0;
	TUnionAsymmetricKeyDB           asymmetricKeyDB;

	memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));

	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",asymmetricKeyDB.keyName,sizeof(asymmetricKeyDB.keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE3:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(asymmetricKeyDB.keyName);
	if (strlen(asymmetricKeyDB.keyName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE3:: keyName can not be null!\n");
		UnionSetResponseRemark("��Կ���Ʋ���Ϊ��!");
		return(errCodeParameter);
	}

	// ��ȡ�ǶԳ���Կ
	if ((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName,0,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE3:: UnionReadAsymmetricKeyDBRec[%s]!\n",asymmetricKeyDB.keyName);
		return(ret);
	}


	// ����ɾ������Կ����״̬Ϊδ��
	// modify by lisq 20141210 �������޸�Ϊ��˽Կ�洢λ��Ϊ1��2ʱ��
	//if (asymmetricKeyDB.vkStoreLocation == 1)
	if (asymmetricKeyDB.vkStoreLocation != 0)
	{
		snprintf(sql,sizeof(sql),"update vkKeyIndex set status = 0 where hsmGroupID = '%s' and vkIndex = '%s' and algorithmID = %d",asymmetricKeyDB.hsmGroupID,asymmetricKeyDB.vkIndex,asymmetricKeyDB.algorithmID);
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8EE3:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
			return(ret);
		}
	}

	// ɾ���ǶԳ���Կ
	if ((ret = UnionDropAsymmetricKeyDB(&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE3:: UnionDropAsymmetricKeyDB keyName[%s]!\n",asymmetricKeyDB.keyName);
		return(ret);
	}

	memset(sql,0,sizeof(sql));
	sprintf(sql,"delete from cert where keyName= '%s'",asymmetricKeyDB.keyName);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EE3:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
		return(ret);
	}
	return(0);
}

