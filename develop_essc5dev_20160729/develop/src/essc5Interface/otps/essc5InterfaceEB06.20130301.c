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
���ܣ�ָ��F8��������������Ӳ�ʹ���ض���Կ�������
���������
	mk		33A	����Կ		32H/1A+32H/1A+3H
	mkIndex		4A	����Կ����	
	mkType		3H	��Կ����	
	mkDvsNum	2N	��ɢ����	
	mkDvsData1	32H	��ɢ����1	
	mkDvsData2	32H	��������2	
	seedLen		2N	���ӳ���	


���������
	seedKey			32H	������Կ	���32H 
	seedKeyCheckValue	16H	��ԿУ��ֵ	

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

	//����Կ		32H/1A+32H/1A+3H
	memset(mk,0,sizeof(mk));
	if ((ret = UnionReadRequestXMLPackageValue("body/mk",mk,sizeof(mk))) < 0)
	{
		//����Կ����		4A
		if ((ret = UnionReadRequestXMLPackageValue("body/mkIndex",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB06:: UnionReadRequestXMLPackageValue[%s]!\n","body/mk and body/mkIndex");
			return(ret);
		}
		tmpBuf[ret] = 0;
		mkIndex = atoi(tmpBuf);
	}

	//��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/mkType",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB06:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkType");
		return(ret);
	}
	tmpBuf[ret] = 0;

	memset(mkType,0,sizeof(mkType));
	UnionTranslateDesKeyTypeTo3CharFormat(UnionConvertSymmetricKeyKeyType(tmpBuf), mkType);
	
	//��ɢ����
	if ((ret = UnionReadRequestXMLPackageValue("body/mkDvsNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB06:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkDvsNum");
		return(ret);
	}
	tmpBuf[ret] = 0;
	mkDvsNum = atoi(tmpBuf);
	
	//��ɢ����1	
	if ((ret = UnionReadRequestXMLPackageValue("body/mkDvsData1",mkDvsData1,sizeof(mkDvsData1))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB06:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkDvsData1");
		return(ret);
	}
	mkDvsData1[ret] = 0;
	
	//��ɢ����2	
	if ((ret = UnionReadRequestXMLPackageValue("body/mkDvsData2",mkDvsData2,sizeof(mkDvsData2))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB06:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkDvsData2");
		return(ret);
	}
	mkDvsData2[ret] = 0;
	
	//���ӳ���
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

	// ����������Կ
	if ((ret = UnionSetResponseXMLPackageValue("body/seedKey",seedKey)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB06:: UnionSetResponseXMLPackageValue seedKey[%s]!\n",seedKey);
		return(ret);
	}

	// ������ԿУ��ֵ
	if ((ret = UnionSetResponseXMLPackageValue("body/seedKeyCheckValue",seedKeyCheckValue)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB06:: UnionSetResponseXMLPackageValue seedKeyCheckValue[%s]!\n",seedKeyCheckValue);
		return(ret);
	}

	return(0);	
}
