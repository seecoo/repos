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
功能：指令FD验证动态口令
输入参数：
	mk		33A	根密钥		32H/1A+32H/1A+3H
	mkIndex		4A	根密钥索引	
	mkType		3H	密钥类型	
	mkDvsNum	2N	离散次数	
	mkDvsData1	32H	离散数据1	
	mkDvsData2	32H	离散数据2		
	seedKey		32H	种子密钥密文	
	algorithm	1N	动态口令算法	1：SM3 2：SM4
	otpLen		2N	口令长度	默认为6
	challengeValLen	2N	挑战码长度	
	challengeValue	NA	挑战码	
	timeChangeCycle	3N	变化周期
	timeValue	16N	时钟值		
	timeWindow	2N	窗口	
	otp		nN	待验证口令	

输出参数：
	clockSkew	1N	偏移方向	向前+，向后- 
	clockSkewValue	2N	偏移周期	


*/
int UnionDealServiceCodeEB08(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret = 0,mkIndex,mkDvsNum = 0,seedLen = 0,algorithm,otpLen = 6,challengeValLen = 0,timeChangeCycle = 0,timeValue = 0,timeWindow = 0,clockSkewValue = 0,len = 0;
	char	mk[64];
	char	mkType[32];
	char	mkDvsData1[64];
	char	mkDvsData2[64];
	char	seedKey[128];
	char	challengeValue[64];
	char	otp[64];
	char	tmpBuf[64];
		
	char	clockSkew[32];


	//根密钥
	memset(mk,0,sizeof(mk));
	if ((ret = UnionReadRequestXMLPackageValue("body/mk",mk,sizeof(mk))) < 0)
	{
		//根密钥索引		4A
		if ((ret = UnionReadRequestXMLPackageValue("body/mkIndex",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionReadRequestXMLPackageValue[%s]!\n","body/mk and body/mkIndex");
			return(ret);
		}
		tmpBuf[ret] = 0;
		mkIndex = atoi(tmpBuf);
	}

	//密钥类型
	if ((ret = UnionReadRequestXMLPackageValue("body/mkType",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkType");
		return(ret);
	}
	tmpBuf[ret] = 0;
	
	memset(mkType,0,sizeof(mkType));
	UnionTranslateDesKeyTypeTo3CharFormat(UnionConvertSymmetricKeyKeyType(tmpBuf), mkType);
		
	//离散次数
	if ((ret = UnionReadRequestXMLPackageValue("body/mkDvsNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkDvsNum");
		return(ret);
	}
	tmpBuf[ret] = 0;
	mkDvsNum = atoi(tmpBuf);
	
	if(mkDvsNum != 1 && mkDvsNum != 2)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB09:: the mkDvsNum not 1 or 2!\n");
		return errCodePackageDefMDL_InvalidMaxFldNum;
	}
	
	memset(mkDvsData1,0,sizeof(mkDvsData1));
	memset(mkDvsData2,0,sizeof(mkDvsData2));
	if ( mkDvsNum >= 1)
	{
		//离散数据1	32H		
		if ((ret = UnionReadRequestXMLPackageValue("body/mkDvsData1",mkDvsData1,sizeof(mkDvsData1))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB09:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkDvsData1");
			return(ret);
		}
	}

	if ( mkDvsNum == 2)
	{
		//离散数据2	32H		
		if ((ret = UnionReadRequestXMLPackageValue("body/mkDvsData2",mkDvsData2,sizeof(mkDvsData2))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB09:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkDvsData2");
			return(ret);
		}
	}
	
	//种子密钥密文	32H		
	if ((ret = UnionReadRequestXMLPackageValue("body/seedKey",seedKey,sizeof(seedKey))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0A:: UnionReadRequestXMLPackageValue[%s]!\n","body/seedKey");
		return(ret);
	}
	seedKey[ret] = 0;
	seedLen = ret/2;
	
	//变形算法
	if ((ret = UnionReadRequestXMLPackageValue("body/algorithm",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionReadRequestXMLPackageValue[%s]!\n","body/algorithm");
		return(ret);
	}
	tmpBuf[ret] = 0;
	algorithm = atoi(tmpBuf);

	//口令长度
	if ((ret = UnionReadRequestXMLPackageValue("body/otpLen",tmpBuf,sizeof(tmpBuf))) > 0)
	{
		tmpBuf[ret] = 0;
		otpLen = atoi(tmpBuf);
	}

	//挑战码长度
	if ((ret = UnionReadRequestXMLPackageValue("body/challengeValLen",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionReadRequestXMLPackageValue[%s]!\n","body/challengeValLen");
		return(ret);
	}
	tmpBuf[ret] = 0;
	challengeValLen = atoi(tmpBuf);

	memset(challengeValue,0,sizeof(challengeValue));
	if (challengeValLen != 0)
	{
		//挑战码
		if ((ret = UnionReadRequestXMLPackageValue("body/challengeValue",challengeValue,sizeof(challengeValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionReadRequestXMLPackageValue[%s]!\n","body/challengeValue");
			return(ret);
		}
	}

	//变化周期
	if ((ret = UnionReadRequestXMLPackageValue("body/timeChangeCycle",tmpBuf,sizeof(timeChangeCycle))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionReadRequestXMLPackageValue[%s]!\n","body/timeChangeCycle");
		return(ret);
	}
	tmpBuf[ret] = 0;
	timeChangeCycle = atoi(tmpBuf);
	
	if (timeChangeCycle != 0)
	{
		//时钟值
		if ((ret = UnionReadRequestXMLPackageValue("body/timeValue",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionReadRequestXMLPackageValue[%s]!\n","body/timeValue");
			return(ret);
		}
		tmpBuf[ret] = 0;
		timeValue = atoi(tmpBuf);	
	}
	
	//窗口
	if ((ret = UnionReadRequestXMLPackageValue("body/timeWindow",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionReadRequestXMLPackageValue[%s]!\n","body/timeWindow");
		return(ret);
	}
	tmpBuf[ret] = 0;
	timeWindow = atoi(tmpBuf);
	
	//待验证口令
	if ((ret = UnionReadRequestXMLPackageValue("body/otp",otp,sizeof(otp))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionReadRequestXMLPackageValue[%s]!\n","body/otp");
		return(ret);
	}
	otp[ret] = 0;
	
	memset(clockSkew,0,sizeof(clockSkew));
	if ((ret = UnionHsmCmdFD(mk, mkType, mkIndex, mkDvsNum, mkDvsData1, mkDvsData2, seedLen, seedKey, algorithm, otpLen,challengeValLen, challengeValue, timeChangeCycle, timeValue, timeWindow, otp,  clockSkew, &clockSkewValue)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionHsmCmdFD err!\n");
		return(ret);
	}

	// 偏移方向
	if ((ret = UnionSetResponseXMLPackageValue("body/clockSkew",clockSkew)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionSetResponseXMLPackageValue seedKey[%s]!\n",clockSkew);
		return(ret);
	}

	len = sprintf(tmpBuf,"%d",clockSkewValue);
	tmpBuf[len] = 0;
	// 偏移周期
	if ((ret = UnionSetResponseXMLPackageValue("body/clockSkewValue",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionSetResponseXMLPackageValue seedKeyCheckValue[%s]!\n",tmpBuf);
		return(ret);
	}

	return(0);	
}
