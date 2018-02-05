
#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <math.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "unionVersion.h"
#include "baseUIService.h"

/*********************************
服务代码:	0208
服务名:		删除组织
功能描述:	删除组织
**********************************/

int UnionDealServiceCode0208(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	organizationName[128];

	// 表名
	if ((ret = UnionReadRequestXMLPackageValue("body/organizationName",organizationName,sizeof(organizationName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0208:: UnionReadRequestXMLPackageValue[%s]!\n","body/organizationName");
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0208:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/organizationName");
		return(errCodeParameter);
	}

	if ((ret =  UnionDeleteRealDBTree("organization","organizationName","higherOrganization",1,organizationName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0208:: UnionDeleteRealDBTree!\n");
		return(ret);
	}
	
	return(0);
}
