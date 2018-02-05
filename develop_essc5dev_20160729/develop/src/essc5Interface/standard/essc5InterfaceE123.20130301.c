//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5Interface.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "asymmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "commWithHsmSvr.h"

/***************************************
�������:	E123
������:		������Կ
��������:	������Կ
***************************************/
int UnionDealServiceCodeE123(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	char			keyName[160];

	TUnionAsymmetricKeyDB	asymmetricKeyDB;
	
	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE123:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);

	// ��ȡ�ǶԳ���Կ
	if ((ret = UnionReadAsymmetricKeyDBRec(keyName,1,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE123:: UnionReadAsymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}	

	if (asymmetricKeyDB.outputFlag)	
	{
		// ������Ӧ��Կ
		if ((ret = UnionSetResponseXMLPackageValue("body/pkValue",asymmetricKeyDB.pkValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE123:: UnionSetResponseXMLPackageValue[%s]!\n","body/pkValue");
			return(ret);
		}	
	}
	else
	{
		UnionUserErrLog("in UnionDealServiceCodeE123:: outputFlag[%d]!\n",asymmetricKeyDB.outputFlag);
		UnionSetResponseRemark("��Կ[%s]��������!",asymmetricKeyDB.keyName);
		return(errCodeEsscMDLKeyOperationNotPermitted);	
	}
	
	return(0);
}
