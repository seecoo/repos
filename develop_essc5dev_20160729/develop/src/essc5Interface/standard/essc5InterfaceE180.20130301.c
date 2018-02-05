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
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "UnionMD5.h"
#include "UnionSHA1.h"
#include "UnionSHA256.h"
#include "unionHsmCmdVersion.h"
#include "asymmetricKeyDB.h"

/***************************************
服务代码:	E180
服务名:		计算数据摘要
功能描述:	计算数据摘要	
***************************************/
int UnionDealServiceCodeE180(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret = 0;
	char				hashID[32];
	char				digest[1024];
	char				data[4096];
	char				dataBcd[4096];
	char				tmpData[512];
	char				dataType[32];
	int				lenOfData = 0;

	//  读取摘要数据
	if ((lenOfData = UnionReadRequestXMLPackageValue("body/data",data,sizeof(data))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE180:: UnionReadRequestXMLPackageValue[body/data]!\n");
		return(lenOfData);
	}
	else if (lenOfData == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE180:: data is null!\n");
		return(errCodeParameter);
	}
	data[lenOfData] = 0;

	// 数据类型
	if ((ret = UnionReadRequestXMLPackageValue("body/dataType",dataType,sizeof(dataType))) < 0)
	{
		strcpy(dataType,"1");
	}
	else
		dataType[ret] = 0;

	if (dataType[0] != '1' && dataType[0] != '2')
	{
		UnionUserErrLog("in UnionDealServiceCodeE180:: dataType[%s] not in [1,2]!\n",dataType);
		return(errCodeParameter);
	}

	if (dataType[0] == '2')
	{
		aschex_to_bcdhex(data,lenOfData,dataBcd);	
		lenOfData = lenOfData / 2;
		memcpy(data,dataBcd,lenOfData);
		data[lenOfData] = 0;
	}

	// 算法标识
	if ((ret = UnionReadRequestXMLPackageValue("body/hashID",hashID,sizeof(hashID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE180:: UnionReadRequestXMLPackageValue[body/hashID]!\n");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE180:: hashID is null!\n");
		return(errCodeParameter);
	}
	else
		hashID[ret] = 0;

	if ((hashID[0] != '1') && (hashID[0] != '2') && (hashID[0] != '3') && (hashID[0] != '4') && (hashID[0] != '5'))
	{
		UnionUserErrLog("in UnionDealServiceCodeE180:: hashID[%s] not in [1,2,3,4,5]!\n",hashID);
		return(errCodeParameter);
	}

	switch(atoi(hashID))
	{
		case	1: // SHA-1
			UnionSHA1((unsigned char *)data,lenOfData,(unsigned char *)tmpData);
			bcdhex_to_aschex(tmpData,20,digest);
			digest[40] = 0;
			break;
		case	2: // MD5
			UnionMD5((unsigned char *)data,lenOfData,(unsigned char *)digest);
			digest[32] = 0;
			break;
		case	5: // SHA256
			UnionSHA256(data, lenOfData, (unsigned char *)digest);
			break;
		case	3: // ISO 10118-2
		//	break;
		case	4: // SM3
			sm3(data,lenOfData,(unsigned char *)tmpData);
			bcdhex_to_aschex(tmpData,32,digest);
			digest[64] = 0;
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE180:: hashID[%s] not in [1,2,3,4,5]!\n",hashID);
			return(errCodeParameter);
	}

	// 设置响应数据
	if ((ret = UnionSetResponseXMLPackageValue("body/digest",digest)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE180:: UnionSetResponseXMLPackageValue[%s]!\n","body/digest");
		return(ret);
	}
	
	return(0);
}
