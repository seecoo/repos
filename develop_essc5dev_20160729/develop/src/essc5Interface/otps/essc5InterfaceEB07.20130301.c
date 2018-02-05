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
���ܣ�ָ��R8���������
���������
	len	2N	����		
	type	1A	����	1�������� 2������ĸ 3������+��ĸ

���������
	out	32H	�����	���32H 	
*/
int UnionDealServiceCodeEB07(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret = 0,len = 0;
	char	tmpBuf[64];
	char	lenType[32];
	char	out[64];

	//����
	if ((ret = UnionReadRequestXMLPackageValue("body/len",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB07:: UnionReadRequestXMLPackageValue[%s]!\n","body/len");
		return(ret);
	}
	tmpBuf[ret] = 0;
	len = atoi(tmpBuf);

	//����
	if ((ret = UnionReadRequestXMLPackageValue("body/type",lenType,sizeof(lenType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB07:: UnionReadRequestXMLPackageValue[%s]!\n","body/type");
		return(ret);
	}
	lenType[1] = 0;
	
	memset(out,0,sizeof(out));
	if ((ret = UnionHsmCmdR8(len, lenType[0], out)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB07:: UnionHsmCmdR8 err!\n");
		return(ret);
	}

	// ���������
	if ((ret = UnionSetResponseXMLPackageValue("body/out",out)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB07:: UnionSetResponseXMLPackageValue out[%s]!\n",out);
		return(ret);
	}

	return(0);
}
