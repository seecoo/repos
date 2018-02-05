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
功能：指令A4用几把密钥的密文成分合成一把密钥，并生成校验值
输入参数：
	SM4Mode		2A	模式	
	keyType		3A	密钥类型	
	partKeyNum	2A	密钥成分数量	
	partKey1	32H	成分1	
	partKey2	32H	成分2	
	partKeyn	32H	成分n	


输出参数：
	keyByLMK	32H	密钥密文	
	checkValue	32H	密钥校验值	

*/
int UnionDealServiceCodeEB05(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret = 0,SM4Mode = 1,keyType,partKeyNum,len = 0,j = 0;
	char	tmpBuf[64];
	char	partKey[20][50];
	char	keyByLMK[64];
	char	checkValue[64];

	//模式
	if ((ret = UnionReadRequestXMLPackageValue("body/SM4Mode",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB05:: UnionReadRequestXMLPackageValue[%s]!\n","body/SM4Mode");
		return(ret);
	}
	tmpBuf[ret] = 0;
	SM4Mode = atoi(tmpBuf);

	//密钥类型
	if ((ret = UnionReadRequestXMLPackageValue("body/keyType",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB05:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyType");
		return(ret);
	}
	tmpBuf[ret] = 0;
	keyType = UnionConvertSymmetricKeyKeyType(tmpBuf);	
	
	//密钥成分数量
	if ((ret = UnionReadRequestXMLPackageValue("body/partKeyNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB05:: UnionReadRequestXMLPackageValue[%s]!\n","body/partKeyNum");
		return(ret);
	}
	tmpBuf[ret] = 0;
	partKeyNum = atoi(tmpBuf);
	
	if(partKeyNum <= 0 || partKeyNum > 9)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB05:: the partKeyNum is error!\n");
		return errCodePackageDefMDL_InvalidMaxFldNum;
	}

	for(j=0; j<partKeyNum; j++)
	{
		memset(partKey[j], 0, sizeof(partKey[j]));
		
		len = sprintf(tmpBuf,"body/partKey%d",j+1);
		tmpBuf[len] = 0;
		if ((ret = UnionReadRequestXMLPackageValue(tmpBuf,partKey[j],sizeof(partKey[j]))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB05:: UnionReadRequestXMLPackageValue[%s]!\n",tmpBuf);
			return(ret);
		}
	}

	memset(keyByLMK,0,sizeof(keyByLMK));
	memset(checkValue,0,sizeof(checkValue));
	if ((ret = UnionHsmCmdA4(SM4Mode,keyType,con128BitsDesKey, partKeyNum, partKey, keyByLMK, checkValue)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB05:: UnionHsmCmdA4 err!\n");
		return(ret);
	}

	// 返回密钥密文
	if ((ret = UnionSetResponseXMLPackageValue("body/keyByLMK",keyByLMK)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB05:: UnionSetResponseXMLPackageValue keyByLMK[%s]!\n",keyByLMK);
		return(ret);
	}

	// 返回密钥校验值
	if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",checkValue)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB05:: UnionSetResponseXMLPackageValue checkValue[%s]!\n",checkValue);
		return(ret);
	}

	return(0);
}
