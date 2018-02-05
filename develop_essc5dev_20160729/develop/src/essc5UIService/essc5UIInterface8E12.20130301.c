//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

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
�������:	8E12
������:		ɾ���ǶԳ���Կ
��������:	ɾ���ǶԳ���Կ
***************************************/
int UnionDealServiceCode8E12(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	char				sql[128];
	char				keyName[136];
	//int				vkIdx = 0;
	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	
	memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));
	
	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E12:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);
	if (strlen(keyName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E12:: keyName can not be null!\n");
		UnionSetResponseRemark("��Կ���Ʋ���Ϊ��!");
		return(errCodeParameter);
	}

	// ��ȡ�ǶԳ���Կ
	if ((ret = UnionReadAsymmetricKeyDBRec(keyName,0,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E12:: UnionReadAsymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}


	// ����ɾ������Կ����״̬Ϊδ��
	// modify by lisq 20141210 �������޸�Ϊ��˽Կ�洢λ��Ϊ1��2ʱ��
	//if (asymmetricKeyDB.vkStoreLocation == 1)
	if (asymmetricKeyDB.vkStoreLocation != 0)
	{
		//sprintf(sql,"update vkKeyIndex set status = 0 where hsmGroupID = '%s' and vkIndex = '%s'",asymmetricKeyDB.hsmGroupID,asymmetricKeyDB.vkIndex);
		snprintf(sql,sizeof(sql),"update vkKeyIndex set status = 0 where hsmGroupID = '%s' and vkIndex = '%s' and algorithmID = %d",asymmetricKeyDB.hsmGroupID,asymmetricKeyDB.vkIndex,asymmetricKeyDB.algorithmID);
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E12:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
			return(ret);
		}
	}
	
	// ɾ���ǶԳ���Կ
	if ((ret = UnionDropAsymmetricKeyDB(&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E12:: UnionDropAsymmetricKeyDB keyName[%s]!\n",asymmetricKeyDB.keyName);
		return(ret);
	}

	return(0);
}


