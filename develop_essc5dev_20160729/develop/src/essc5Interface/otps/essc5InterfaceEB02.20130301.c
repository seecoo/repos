/*
 * Copyright (c) 2011 Union Tech.
 * All rights reserved.
 *
 * Created on: 2016-01-27
 *   @Author: chenwd
 *   @Version 1.0
 */

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
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "symmetricKeyDB.h"
#include "asymmetricKeyDB.h"
#include "UnionXOR.h"
#include "unionHsmCmdVersion.h"
#include "hsmDefaultFunction.h"
/*
功能：指令PA装载格式化数据至HSM
输入参数：
	format	1024A	打印格式

输出参数：
	无
*/
int UnionDealServiceCodeEB02(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret = 0;
	char	format[1024];
	char	sql[128];
	char	ipAddr[32];
	
        // 获取密码机组内的打印密码机    
        ret = sprintf(sql,"select ipAddr from hsm where status=1 and enabled=1 and uses=1 and hsmGroupID='%s' order by ipAddr",phsmGroupRec->hsmGroupID);
	sql[ret] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB02:: UnionExecRealDBSql[%s]!\n",sql);
		UnionSetResponseRemark("读取密码机表失败");
		return(ret);
        }
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB02:: UnionExecRealDBSql[%s]!\n",sql);
		UnionSetResponseRemark("密码机组[%s]没有可用的打印用途的密码机",phsmGroupRec->hsmGroupID);
		return(errCodeDatabaseMDL_RecordNotFound);
	}

	UnionLocateXMLPackage("detail", 1);
	memset(ipAddr,0,sizeof(ipAddr));
	if ((ret = UnionReadXMLPackageValue("ipAddr", ipAddr, sizeof(ipAddr))) < 0)
	{
			UnionUserErrLog("in UnionDealServiceCodeEB02:: UnionReadXMLPackageValue[%s]!\n","ipAddr");
			return(ret);
	}
        
        // 使用指定的加密机
        UnionSetUseSpecHsmIPAddrForOneCmd(ipAddr);

	// 打印格式
	if ((ret = UnionReadRequestXMLPackageValue("body/format",format,sizeof(format))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB02:: UnionReadRequestXMLPackageValue[%s]!\n","body/SM4Mode");
		return(ret);
	}
	format[ret] = 0;

	if ((ret = UnionHsmCmdPA(format)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB02:: UnionHsmCmdPA[%s]!\n",format);
		return(ret);
	}

	return(0);
}
