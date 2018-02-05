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
//#include "unionHsmCmd.h"
#include "unionHsmCmdVersion.h"

/***************************************
服务代码:	8E0A
服务名:		检查密钥有效时间
功能描述:	检查密钥有效时间
***************************************/
/*int UnionDealServiceCode8E0A(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret = 0;
	char				curDateTime[16+1];
	char				keyUpdateTime[16+1];
	int				residueDays = 0;
	char				sql[128+1];
	int				len = 0;

	TUnionSymmetricKeyDB		symmetricKeyDB;

	len = sprintf(sql,"select * from symmetricKeyDB");
	sql[len] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0A:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	
	// 读取对称密钥
	if ((ret = UnionReadSymmetricKeyDBRec(symmetricKeyDB.keyName,&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0A:: UnionReadSymmetricKeyDBRec[%s]!\n",symmetricKeyDB.keyName);
		return(ret);
	}
	

	return(0);
}
*/

