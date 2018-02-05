//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "essc5Interface.h"

// 执行密码机指令
int UnionDealServiceTask(char *serviceCode,PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;

        if (memcmp(serviceCode,"E601",4) == 0)
		ret = UnionDealServiceCodeE601(phsmGroupRec);
        else if (memcmp(serviceCode,"E602",4) == 0)
		ret = UnionDealServiceCodeE602(phsmGroupRec);
        else if (memcmp(serviceCode,"E603",4) == 0)
		ret = UnionDealServiceCodeE603(phsmGroupRec);
	else
	{
		UnionUserErrLog("in UnionDealServiceTask:: Invalid serviceCode[%s]!\n",serviceCode);
		return(errCodeFileTransSvrInvalidServiceCode);
	}

	return(ret);
}
