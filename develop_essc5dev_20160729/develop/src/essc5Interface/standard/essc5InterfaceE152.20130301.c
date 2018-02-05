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
#include "unionHsmCmd.h"
#include "symmetricKeyDB.h"
#include "UnionXOR.h"

/***************************************
服务代码:	E152
服务名:		验证并生成MAC
功能描述:	验证并生成MAC
***************************************/
int UnionDealServiceCodeE152(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	int	mode = 0;
	char	tmpBuf[32];
	char	keyName1[160];
	char	keyName2[160];
	char	keyValue1[64];
	char	keyValue2[64];
	int	algorithmID = 1;
	
	// 密钥名称1
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName1",keyName1,sizeof(keyName1))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE152:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName1");
		return(ret);
	}
	keyName1[ret] = 0;

	// 密钥名称2
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName2",keyName2,sizeof(keyName2))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE152:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName2");
		return(ret);
	}
	keyName2[ret] = 0;
	
	// 模式
	// 1：指定密钥名称
	// 2：指定密钥密文
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) <= 0)
		mode = 1;
	else
	{
		tmpBuf[ret] = 0;
		mode = atoi(tmpBuf);
	}

	// 算法标识
	// 可选,默认为1
	// 1：ANSIX9.19
	// 2：中国银联标准 
	// 3：银联POS标准
	if ((ret = UnionReadRequestXMLPackageValue("body/algorithmID",tmpBuf,sizeof(tmpBuf))) <= 0)
		algorithmID = 1;
	else
	{
		tmpBuf[ret] = 0;
		algorithmID = atoi(tmpBuf);
	}
	
	if (mode == 2)	// 指定密钥密文
	{
		// 密钥密文1
		if ((ret = UnionReadRequestXMLPackageValue("body/keyValue1",keyValue1,sizeof(keyValue1))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE152:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyValue1");
			return(ret);
		}
		keyValue1[ret] = 0;

		// 密钥密文2
		if ((ret = UnionReadRequestXMLPackageValue("body/keyValue2",keyValue2,sizeof(keyValue2))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE152:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyValue2");
			return(ret);
		}
		keyValue2[ret] = 0;
	}
	
	// 验证MAC
	if ((ret = UnionSetRequestXMLPackageValue("body/keyName",keyName1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE152:: UnionSetRequestXMLPackageValue[%s][%s]!\n","body/keyName",keyName1);
		return(ret);
	}
	//UnionLogRequestXMLPackage();
	
	if (mode == 2)
	{
		if ((ret = UnionSetRequestXMLPackageValue("body/keyValue",keyValue1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE152:: UnionSetRequestXMLPackageValue[%s][%s]!\n","body/keyValue",keyValue1);
			return(ret);
		}
	}

	if ((ret = UnionDealServiceCodeE151(phsmGroupRec)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE152:: UnionDealServiceCodeE151!\n");
		return(ret);
	}

	// 生成MAC
	if ((ret = UnionSetRequestXMLPackageValue("body/keyName",keyName2)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE152:: UnionSetRequestXMLPackageValue[%s][%s]!\n","body/keyName",keyName2);
		return(ret);
	}
	
	if (mode == 2)
	{
		if ((ret = UnionSetRequestXMLPackageValue("body/keyValue",keyValue2)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE152:: UnionSetRequestXMLPackageValue[%s][%s]!\n","body/keyValue",keyValue2);
			return(ret);
		}
	}

	if ((ret = UnionDealServiceCodeE150(phsmGroupRec)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE152:: UnionDealServiceCodeE150!\n");
		return(ret);
	}
	
	// 删除临时节点
	UnionDeleteRequestXMLPackageNode("body/keyName",0);
	if (mode == 2)
		UnionDeleteRequestXMLPackageNode("body/keyValue",0);
		
	return(0);
}
