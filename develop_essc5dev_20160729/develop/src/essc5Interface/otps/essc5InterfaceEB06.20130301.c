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
/*
功能：指令F8随机生成令牌种子并使用特定密钥保护输出
输入参数：
	mk		33A	根密钥		32H/1A+32H/1A+3H
	mkIndex		4A	根密钥索引	
	mkType		3H	密钥类型	
	mkDvsNum	2N	离散次数	
	mkDvsData1	32H	离散数据1	
	mkDvsData2	32H	理算数据2	
	seedLen		2N	种子长度	


输出参数：
	seedKey			32H	种子密钥	最大32H 
	seedKeyCheckValue	16H	密钥校验值	

*/
int UnionDealServiceCodeEB06(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret = 0,mkIndex,mkDvsNum = 0,seedLen = 0;
	char	mk[64];
	char	mkType[32];
	char	mkDvsData1[64];
	char	mkDvsData2[64];
	char	tmpBuf[64];
	
	char	seedKey[128];
	char	seedKeyCheckValue[32];

	//根密钥		32H/1A+32H/1A+3H
	memset(mk,0,sizeof(mk));
	if ((ret = UnionReadRequestXMLPackageValue("body/mk",mk,sizeof(mk))) < 0)
	{
		//根密钥索引		4A
		if ((ret = UnionReadRequestXMLPackageValue("body/mkIndex",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB06:: UnionReadRequestXMLPackageValue[%s]!\n","body/mk and body/mkIndex");
			return(ret);
		}
		tmpBuf[ret] = 0;
		mkIndex = atoi(tmpBuf);
	}

	//密钥类型
	if ((ret = UnionReadRequestXMLPackageValue("body/mkType",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB06:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkType");
		return(ret);
	}
	tmpBuf[ret] = 0;

	memset(mkType,0,sizeof(mkType));
	UnionTranslateDesKeyTypeTo3CharFormat(UnionConvertSymmetricKeyKeyType(tmpBuf), mkType);
	
	//离散次数
	if ((ret = UnionReadRequestXMLPackageValue("body/mkDvsNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB06:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkDvsNum");
		return(ret);
	}
	tmpBuf[ret] = 0;
	mkDvsNum = atoi(tmpBuf);
	
	//离散数据1	
	if ((ret = UnionReadRequestXMLPackageValue("body/mkDvsData1",mkDvsData1,sizeof(mkDvsData1))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB06:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkDvsData1");
		return(ret);
	}
	mkDvsData1[ret] = 0;
	
	//离散数据2	
	if ((ret = UnionReadRequestXMLPackageValue("body/mkDvsData2",mkDvsData2,sizeof(mkDvsData2))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB06:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkDvsData2");
		return(ret);
	}
	mkDvsData2[ret] = 0;
	
	//种子长度
	if ((ret = UnionReadRequestXMLPackageValue("body/seedLen",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB06:: UnionReadRequestXMLPackageValue[%s]!\n","body/seedLen");
		return(ret);
	}
	tmpBuf[ret] = 0;
	seedLen = atoi(tmpBuf);
	
	memset(seedKey,0,sizeof(seedKey));
	memset(seedKeyCheckValue,0,sizeof(seedKeyCheckValue));
	if ((ret = UnionHsmCmdF8(mk, mkIndex, mkType, mkDvsNum, mkDvsData1, mkDvsData2 , seedLen, 1, seedKey, seedKeyCheckValue)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB06:: UnionHsmCmdF8 err!\n");
		return(ret);
	}

	// 返回种子密钥
	if ((ret = UnionSetResponseXMLPackageValue("body/seedKey",seedKey)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB06:: UnionSetResponseXMLPackageValue seedKey[%s]!\n",seedKey);
		return(ret);
	}

	// 返回密钥校验值
	if ((ret = UnionSetResponseXMLPackageValue("body/seedKeyCheckValue",seedKeyCheckValue)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB06:: UnionSetResponseXMLPackageValue seedKeyCheckValue[%s]!\n",seedKeyCheckValue);
		return(ret);
	}

	return(0);	
}
