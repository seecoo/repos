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
功能：指令FF令牌种子变形
输入参数：
	mk		33A	根密钥		32H/1A+32H/1A+3H
	mkIndex		4A	根密钥索引	
	mkType		3H	密钥类型	当根密不使用索引时存在此域。至少支持00A，309中的一种
	mkDvsNum	2N	离散次数	变形前
	mkDvsData1	32H	离散数据1	
	mkDvsData2	32H	离散数据2	
	mkNewDvsNum	2N	离散次数	变形后
	mkNewDvsData1	32H	离散数据1	
	mkNewDvsData2	32H	离散数据2	
	seedLen		2N	种子长度	
	seedKey		32H	种子密钥密文	由根密钥离散后的密钥保护（PKCS#5填充）
	algorithm	1N	变形算法	变形算法为1时，与检查sm3(种子)前8字节是否一致。
	seedCheckValue	16H	种子校验值	
	newSeedLen	4N	变形后种子长度	从左到右截取多少个字节
	tranElementLen	4N	变形要素长度	
	tranElement	n*2H	变形要素	
	tranOffset	4N	变形要素位置	如0,则变形数据为：种子+变形要素


输出参数：
	newSeed			N*2H	新种子
	newSeedCheckValue	16H	校验值

*/
int UnionDealServiceCodeEB0A(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret = 0,mkIndex,mkDvsNum = 0,mkNewDvsNum = 0,seedLen = 0,algorithm,newSeedLen = 0,tranElementLen = 0,tranOffset = 0;
	char	mk[64];
	char	mkType[32];
	char	mkDvsData1[64];
	char	mkDvsData2[64];
	char	mkNewDvsData1[64];
	char	mkNewDvsData2[64];
	char	seedKey[128];
	char	seedCheckValue[32];
	char	tranElement[512];
	char	tmpBuf[64];
	char	newSeed[64];
	char	newSeedCheckValue[32];

	//根密钥		32H/1A+32H/1A+3H
	memset(mk,0,sizeof(mk));
	if ((ret = UnionReadRequestXMLPackageValue("body/mk",mk,sizeof(mk))) < 0)
	{
		//根密钥索引		4A
		if ((ret = UnionReadRequestXMLPackageValue("body/mkIndex",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0A:: UnionReadRequestXMLPackageValue[%s]!\n","body/mk and body/mkIndex");
			return(ret);
		}
		tmpBuf[ret] = 0;
		mkIndex = atoi(tmpBuf);
	}

	//密钥类型
	if ((ret = UnionReadRequestXMLPackageValue("body/mkType",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0A:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkType");
		return(ret);
	}
	tmpBuf[ret] = 0;
	
	memset(mkType,0,sizeof(mkType));
	UnionTranslateDesKeyTypeTo3CharFormat(UnionConvertSymmetricKeyKeyType(tmpBuf), mkType);
		
	//离散次数	变形前
	if ((ret = UnionReadRequestXMLPackageValue("body/mkDvsNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0A:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkDvsNum");
		return(ret);
	}
	tmpBuf[ret] = 0;
	mkDvsNum = atoi(tmpBuf);
	
	if(mkDvsNum != 1 && mkDvsNum != 2)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0A:: the mkDvsNum not 1 or 2!\n");
		return errCodePackageDefMDL_InvalidMaxFldNum;
	}
	
	memset(mkDvsData1,0,sizeof(mkDvsData1));
	memset(mkDvsData2,0,sizeof(mkDvsData2));
	if ( mkDvsNum >= 1)
	{
		//离散数据1	32H		
		if ((ret = UnionReadRequestXMLPackageValue("body/mkDvsData1",mkDvsData1,sizeof(mkDvsData1))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0A:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkDvsData1");
			return(ret);
		}
	}

	if ( mkDvsNum == 2)
	{
		//离散数据2	32H		
		if ((ret = UnionReadRequestXMLPackageValue("body/mkDvsData2",mkDvsData2,sizeof(mkDvsData2))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0A:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkDvsData2");
			return(ret);
		}
	}
	
	//离散次数	变形后
	if ((ret = UnionReadRequestXMLPackageValue("body/mkNewDvsNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0A:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkNewDvsNum");
		return(ret);
	}
	tmpBuf[ret] = 0;
	mkNewDvsNum = atoi(tmpBuf);

	if(mkNewDvsNum != 1 && mkNewDvsNum != 2)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0A:: the mkNewDvsNum not 1 or 2!\n");
		return errCodePackageDefMDL_InvalidMaxFldNum;
	}
	
	memset(mkNewDvsData1,0,sizeof(mkNewDvsData1));
	memset(mkNewDvsData2,0,sizeof(mkNewDvsData2));
	if ( mkNewDvsNum >= 1)
	{
		//离散数据1	32H		
		if ((ret = UnionReadRequestXMLPackageValue("body/mkNewDvsData1",mkNewDvsData1,sizeof(mkNewDvsData1))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0A:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkNewDvsData1");
			return(ret);
		}
	}

	if ( mkNewDvsNum == 2)
	{
		//离散数据2	32H		
		if ((ret = UnionReadRequestXMLPackageValue("body/mkNewDvsData2",mkNewDvsData2,sizeof(mkNewDvsData2))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0A:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkNewDvsData2");
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
		UnionUserErrLog("in UnionDealServiceCodeEB0A:: UnionReadRequestXMLPackageValue[%s]!\n","body/algorithm");
		return(ret);
	}
	tmpBuf[ret] = 0;
	algorithm = atoi(tmpBuf);
	
	//种子校验值	32H		
	if ((ret = UnionReadRequestXMLPackageValue("body/seedCheckValue",seedCheckValue,sizeof(seedCheckValue))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0A:: UnionReadRequestXMLPackageValue[%s]!\n","body/seedCheckValue");
		return(ret);
	}
	seedCheckValue[ret] = 0;
	
	//变形后种子长度	从左到右截取多少个字节
	if ((ret = UnionReadRequestXMLPackageValue("body/newSeedLen",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0A:: UnionReadRequestXMLPackageValue[%s]!\n","body/newSeedLen");
		return(ret);
	}
	tmpBuf[ret] = 0;
	newSeedLen = atoi(tmpBuf);

	//变形要素长度
	if ((ret = UnionReadRequestXMLPackageValue("body/tranElementLen",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0A:: UnionReadRequestXMLPackageValue[%s]!\n","body/tranElementLen");
		return(ret);
	}
	tmpBuf[ret] = 0;
	tranElementLen = atoi(tmpBuf);
	
	//变形要素	
	if ((ret = UnionReadRequestXMLPackageValue("body/tranElement",tranElement,sizeof(tranElement))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0A:: UnionReadRequestXMLPackageValue[%s]!\n","body/tranElement");
		return(ret);
	}
	tranElement[ret] = 0;

	//变形要素位置
	if ((ret = UnionReadRequestXMLPackageValue("body/tranOffset",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0A:: UnionReadRequestXMLPackageValue[%s]!\n","body/tranOffset");
		return(ret);
	}
	tmpBuf[ret] = 0;
	tranOffset = atoi(tmpBuf);	
	
	memset(newSeed,0,sizeof(newSeed));
	memset(newSeedCheckValue,0,sizeof(newSeedCheckValue));
	if ((ret = UnionHsmCmdFF(mk, mkType, mkIndex, mkDvsNum, mkDvsData1, mkDvsData2, mkNewDvsNum, mkNewDvsData1, mkNewDvsData2,  seedLen, seedKey, algorithm, seedCheckValue,  newSeedLen, tranElementLen, tranElement, tranOffset, newSeed, newSeedCheckValue)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0A:: UnionHsmCmdFF err!\n");
		return(ret);
	}

	// 返回新种子
	if ((ret = UnionSetResponseXMLPackageValue("body/newSeed",newSeed)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0A:: UnionSetResponseXMLPackageValue newSeed[%s]!\n",newSeed);
		return(ret);
	}

	// 返回校验值
	if ((ret = UnionSetResponseXMLPackageValue("body/newSeedCheckValue",newSeedCheckValue)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0A:: UnionSetResponseXMLPackageValue newSeedCheckValue[%s]!\n",newSeedCheckValue);
		return(ret);
	}

	return(0);
}
