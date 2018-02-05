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
���ܣ�ָ��FD��֤��̬����
���������
	mk		33A	����Կ		32H/1A+32H/1A+3H
	mkIndex		4A	����Կ����	
	mkType		3H	��Կ����	
	mkDvsNum	2N	��ɢ����	
	mkDvsData1	32H	��ɢ����1	
	mkDvsData2	32H	��ɢ����2		
	seedKey		32H	������Կ����	
	algorithm	1N	��̬�����㷨	1��SM3 2��SM4
	otpLen		2N	�����	Ĭ��Ϊ6
	challengeValLen	2N	��ս�볤��	
	challengeValue	NA	��ս��	
	timeChangeCycle	3N	�仯����
	timeValue	16N	ʱ��ֵ		
	timeWindow	2N	����	
	otp		nN	����֤����	

���������
	clockSkew	1N	ƫ�Ʒ���	��ǰ+�����- 
	clockSkewValue	2N	ƫ������	


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


	//����Կ
	memset(mk,0,sizeof(mk));
	if ((ret = UnionReadRequestXMLPackageValue("body/mk",mk,sizeof(mk))) < 0)
	{
		//����Կ����		4A
		if ((ret = UnionReadRequestXMLPackageValue("body/mkIndex",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionReadRequestXMLPackageValue[%s]!\n","body/mk and body/mkIndex");
			return(ret);
		}
		tmpBuf[ret] = 0;
		mkIndex = atoi(tmpBuf);
	}

	//��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/mkType",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkType");
		return(ret);
	}
	tmpBuf[ret] = 0;
	
	memset(mkType,0,sizeof(mkType));
	UnionTranslateDesKeyTypeTo3CharFormat(UnionConvertSymmetricKeyKeyType(tmpBuf), mkType);
		
	//��ɢ����
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
		//��ɢ����1	32H		
		if ((ret = UnionReadRequestXMLPackageValue("body/mkDvsData1",mkDvsData1,sizeof(mkDvsData1))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB09:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkDvsData1");
			return(ret);
		}
	}

	if ( mkDvsNum == 2)
	{
		//��ɢ����2	32H		
		if ((ret = UnionReadRequestXMLPackageValue("body/mkDvsData2",mkDvsData2,sizeof(mkDvsData2))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB09:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkDvsData2");
			return(ret);
		}
	}
	
	//������Կ����	32H		
	if ((ret = UnionReadRequestXMLPackageValue("body/seedKey",seedKey,sizeof(seedKey))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0A:: UnionReadRequestXMLPackageValue[%s]!\n","body/seedKey");
		return(ret);
	}
	seedKey[ret] = 0;
	seedLen = ret/2;
	
	//�����㷨
	if ((ret = UnionReadRequestXMLPackageValue("body/algorithm",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionReadRequestXMLPackageValue[%s]!\n","body/algorithm");
		return(ret);
	}
	tmpBuf[ret] = 0;
	algorithm = atoi(tmpBuf);

	//�����
	if ((ret = UnionReadRequestXMLPackageValue("body/otpLen",tmpBuf,sizeof(tmpBuf))) > 0)
	{
		tmpBuf[ret] = 0;
		otpLen = atoi(tmpBuf);
	}

	//��ս�볤��
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
		//��ս��
		if ((ret = UnionReadRequestXMLPackageValue("body/challengeValue",challengeValue,sizeof(challengeValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionReadRequestXMLPackageValue[%s]!\n","body/challengeValue");
			return(ret);
		}
	}

	//�仯����
	if ((ret = UnionReadRequestXMLPackageValue("body/timeChangeCycle",tmpBuf,sizeof(timeChangeCycle))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionReadRequestXMLPackageValue[%s]!\n","body/timeChangeCycle");
		return(ret);
	}
	tmpBuf[ret] = 0;
	timeChangeCycle = atoi(tmpBuf);
	
	if (timeChangeCycle != 0)
	{
		//ʱ��ֵ
		if ((ret = UnionReadRequestXMLPackageValue("body/timeValue",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionReadRequestXMLPackageValue[%s]!\n","body/timeValue");
			return(ret);
		}
		tmpBuf[ret] = 0;
		timeValue = atoi(tmpBuf);	
	}
	
	//����
	if ((ret = UnionReadRequestXMLPackageValue("body/timeWindow",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionReadRequestXMLPackageValue[%s]!\n","body/timeWindow");
		return(ret);
	}
	tmpBuf[ret] = 0;
	timeWindow = atoi(tmpBuf);
	
	//����֤����
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

	// ƫ�Ʒ���
	if ((ret = UnionSetResponseXMLPackageValue("body/clockSkew",clockSkew)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionSetResponseXMLPackageValue seedKey[%s]!\n",clockSkew);
		return(ret);
	}

	len = sprintf(tmpBuf,"%d",clockSkewValue);
	tmpBuf[len] = 0;
	// ƫ������
	if ((ret = UnionSetResponseXMLPackageValue("body/clockSkewValue",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB08:: UnionSetResponseXMLPackageValue seedKeyCheckValue[%s]!\n",tmpBuf);
		return(ret);
	}

	return(0);	
}
