//	Author:		张永定
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
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"

/***************************************
服务代码:	8E02
服务名:		删除对称密钥
功能描述:	删除对称密钥
***************************************/
int UnionDealServiceCode8E02(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	TUnionSymmetricKeyDB		symmetricKeyDB;
	
	memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));
	
	// 密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",symmetricKeyDB.keyName,sizeof(symmetricKeyDB.keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E02:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(symmetricKeyDB.keyName);
	
	// 删除对称密钥
	if ((ret = UnionDropSymmetricKeyDB(&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E02:: UnionDropSymmetricKeyDB keyName[%s]!\n",symmetricKeyDB.keyName);
		return(ret);
	}

	return(0);
}


