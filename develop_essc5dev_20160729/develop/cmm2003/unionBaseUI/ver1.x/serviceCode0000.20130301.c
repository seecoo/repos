//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "baseUIService.h"

/***************************************
服务代码:	0001
服务名:		获取认证码
功能描述:	获取认证码
***************************************/
int UnionDealServiceCode0000(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	tmpBuf[1024+1];
	char	lenBuf[16+1];
	
	// 读取测试数据
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/test",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0000:: UnionReadRequestXMLPackageValue[%s]!\n","body/test");
		return(ret);
	}

	UnionLog("in UnionDealServiceCode0000:: tmpBuf[%s]\n",tmpBuf);
	// 设置响应数据
	memset(lenBuf,0,sizeof(lenBuf));
	sprintf(lenBuf,"%d",(int)strlen(tmpBuf));
	if ((ret = UnionSetResponseXMLPackageValue("body/test",lenBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0000:: UnionSetResponseXMLPackageValue[%s]!\n","body/test");
		return(ret);
	}
	
	return(0);
}

