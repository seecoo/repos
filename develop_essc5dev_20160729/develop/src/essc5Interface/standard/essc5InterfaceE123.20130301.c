//	Author:		张永定
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
服务代码:	E123
服务名:		导出公钥
功能描述:	导出公钥
***************************************/
int UnionDealServiceCodeE123(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	char			keyName[160];

	TUnionAsymmetricKeyDB	asymmetricKeyDB;
	
	// 密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE123:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);

	// 读取非对称密钥
	if ((ret = UnionReadAsymmetricKeyDBRec(keyName,1,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE123:: UnionReadAsymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}	

	if (asymmetricKeyDB.outputFlag)	
	{
		// 设置响应公钥
		if ((ret = UnionSetResponseXMLPackageValue("body/pkValue",asymmetricKeyDB.pkValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE123:: UnionSetResponseXMLPackageValue[%s]!\n","body/pkValue");
			return(ret);
		}	
	}
	else
	{
		UnionUserErrLog("in UnionDealServiceCodeE123:: outputFlag[%d]!\n",asymmetricKeyDB.outputFlag);
		UnionSetResponseRemark("密钥[%s]不允许导出!",asymmetricKeyDB.keyName);
		return(errCodeEsscMDLKeyOperationNotPermitted);	
	}
	
	return(0);
}
