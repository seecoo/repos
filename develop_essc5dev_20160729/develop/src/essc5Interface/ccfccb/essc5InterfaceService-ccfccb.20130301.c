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

	// E200 测试密码机状态
        if (memcmp(serviceCode,"E501",4) == 0)
		ret = UnionDealServiceCodeE501(phsmGroupRec);
        else if (memcmp(serviceCode,"E502",4) == 0)
		ret = UnionDealServiceCodeE502(phsmGroupRec);
        else if (memcmp(serviceCode,"E503",4) == 0)
		ret = UnionDealServiceCodeE503(phsmGroupRec);
        else if (memcmp(serviceCode,"E504",4) == 0)
		ret = UnionDealServiceCodeE504(phsmGroupRec);
	else
	{
		UnionUserErrLog("in UnionDealServiceTask:: Invalid serviceCode[%s]!\n",serviceCode);
		return(errCodeFileTransSvrInvalidServiceCode);
	}

	return(ret);
}
