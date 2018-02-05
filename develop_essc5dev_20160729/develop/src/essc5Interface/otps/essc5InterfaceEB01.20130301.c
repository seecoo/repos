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
���ܣ�ָ��BU��SM4�㷨��������ԿУ��ֵ
���������
	SM4Mode		2A	ģʽ
	keyType		3A	��Կ����
	keyLength	2A	��Կ����
	Key		32H	��Կֵ

���������
	checkValue	16H	��ԿУ��ֵ	
*/
int UnionDealServiceCodeEB01(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret = 0,SM4Mode = 1,keyType,keyLength;
	char	tmpBuf[64];
	char	Key[64];
	char	checkValue[32];

	//ģʽ
	if ((ret = UnionReadRequestXMLPackageValue("body/SM4Mode",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB01:: UnionReadRequestXMLPackageValue[%s]!\n","body/SM4Mode");
		return(ret);
	}
	tmpBuf[ret] = 0;
	SM4Mode = atoi(tmpBuf);

	//��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyType",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB01:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyType");
		return(ret);
	}
	tmpBuf[ret] = 0;
	keyType = UnionConvertSymmetricKeyKeyType(tmpBuf);	
	
	//��Կ����
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
		
	
	//��Կֵ
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

	// ������ԿУ��ֵ
	if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",checkValue)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB01:: UnionSetResponseXMLPackageValue checkValue[%s]!\n",checkValue);
		return(ret);
	}

	return(0);
}
