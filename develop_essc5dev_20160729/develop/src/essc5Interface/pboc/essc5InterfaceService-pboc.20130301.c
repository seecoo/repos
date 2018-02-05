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

	// E301校验ARQC(525)
        if (memcmp(serviceCode,"E301",4) == 0)
		ret = UnionDealServiceCodeE301(phsmGroupRec);
	// E302 生成ARPC(526/529)
        else if (memcmp(serviceCode,"E302",4) == 0)
		ret = UnionDealServiceCodeE302(phsmGroupRec);
	// E303 患用苁軦RPC(527)
        else if (memcmp(serviceCode,"E303",4) == 0)
		ret = UnionDealServiceCodeE303(phsmGroupRec);
	//E304 脚本数据计算mac(528)
        else if (memcmp(serviceCode,"E304",4) == 0)
		ret = UnionDealServiceCodeE304(phsmGroupRec);
	// E300 测试密码机状态
        else if (memcmp(serviceCode,"E300",4) == 0)
		ret = UnionDealServiceCodeE300(phsmGroupRec);
	else
	{
		UnionUserErrLog("in UnionDealServiceTask:: Invalid serviceCode[%s]!\n",serviceCode);
		return(errCodeFileTransSvrInvalidServiceCode);
	}

	return(ret);
	
}
