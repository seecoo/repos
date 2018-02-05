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
功能：指令BU（SM4算法）生成密钥校验值
输入参数：
	SM4Mode		2A	模式
	keyType		3A	密钥类型
	keyLength	2A	密钥长度
	Key		32H	密钥值

输出参数：
	checkValue	16H	密钥校验值	
*/
int UnionDealServiceCodeEB01(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret = 0,SM4Mode = 1,keyType,keyLength;
	char	tmpBuf[64];
	char	Key[64];
	char	checkValue[32];

	//模式
	if ((ret = UnionReadRequestXMLPackageValue("body/SM4Mode",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB01:: UnionReadRequestXMLPackageValue[%s]!\n","body/SM4Mode");
		return(ret);
	}
	tmpBuf[ret] = 0;
	SM4Mode = atoi(tmpBuf);

	//密钥类型
	if ((ret = UnionReadRequestXMLPackageValue("body/keyType",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB01:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyType");
		return(ret);
	}
	tmpBuf[ret] = 0;
	keyType = UnionConvertSymmetricKeyKeyType(tmpBuf);	
	
	//密钥长度
	if ((ret = UnionReadRequestXMLPackageValue("body/keyLength",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB01:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyLength");
		return(ret);
	}
	tmpBuf[ret] = 0;
	
	if(strcmp(tmpBuf,"16") == 0)
		keyLength = con64BitsDesKey;
	else if(strcmp(tmpBuf,"32") == 0)
		keyLength = con128BitsDesKey;
	else if(strcmp(tmpBuf,"48") == 0)
		keyLength = con192BitsDesKey;
	else
	{
		UnionUserErrLog("in UnionDealServiceCodeEB01:: UnionReadRequestXMLPackageValue keyLength = [%s], Not 16 or 32 or 48!\n",tmpBuf);
		return(ret);
	}
		
	
	//密钥值
	if ((ret = UnionReadRequestXMLPackageValue("body/Key",Key,sizeof(Key))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB01:: UnionReadRequestXMLPackageValue[%s]!\n","body/Key");
		return(ret);
	}
	Key[ret] = 0;

	UnionSetIsUseNormalZmkType();
	memset(checkValue,0,sizeof(checkValue));
	if ((ret = UnionHsmCmdBU(SM4Mode,keyType,keyLength,Key,checkValue)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB01:: UnionHsmCmdBU err!\n");
		return(ret);
	}

	// 返回密钥校验值
	if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",checkValue)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB01:: UnionSetResponseXMLPackageValue checkValue[%s]!\n",checkValue);
		return(ret);
	}

	return(0);
}
