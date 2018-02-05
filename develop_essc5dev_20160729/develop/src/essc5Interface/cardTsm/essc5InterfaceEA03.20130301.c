#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "unionHsmCmd.h"
#include "unionHsmCmdJK.h"
#include "unionXMLPackage.h"
#include "symmetricKeyDB.h"
#include "UnionLog.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "essc5Interface.h"
#include "base64.h"
#include "UnionDes.h"
#include "unionRealBaseDB.h"
#include "unionHsmCmd.h"
#include "UnionXOR.h"
#include "unionHsmCmdVersion.h"
#include "defaultIBMPinOffsetDef.h"

//加密一个密钥(903)

int UnionDealServiceCodeEA03(PUnionHsmGroupRec phsmGroupRec)
{
	int                             ret;
	char				keyACipherTextByLmk[128];
	char                            keyAType[16];
	char				keyAlength[16];
	char				EncMode[16];
	char				keyBCipherTextByLmk[128];
	char				keyBType[16];
	char				keyBlength[16];
	char				dstkey[128];
	char				tmpbuf[256];
	int				len = 0;	
	char				IV[64];

	//读取keyA的密文
	if ((ret = UnionReadRequestXMLPackageValue("body/keyA",keyACipherTextByLmk, sizeof(keyACipherTextByLmk))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM3:: UnionReadRequestXMLPackageValue[%s]!\n", "body/keyA");
		return ret;
	}
	keyACipherTextByLmk[ret] = 0;

	//读取keyA的密钥类型
	if ((ret = UnionReadRequestXMLPackageValue("body/typeA", keyAType, sizeof(keyAType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM3:: UnionReadRequestXMLPackageValue[%s]!\n", "body/typeA");
		return ret;
	}
	keyAType[ret] = 0;

	//读取lenA的长度
	if ((ret = UnionReadRequestXMLPackageValue("body/lenA", keyAlength, sizeof(keyAlength))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM3:: UnionReadRequestXMLPackageValue[%s]!\n", "body/lenA");
		return ret;
	}
	keyAlength[ret] = 0;

	// 读取加密算法标识
	if ((ret = UnionReadRequestXMLPackageValue("body/mode", EncMode, sizeof(EncMode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM3:: UnionReadRequestXMLPackageValue[%s]!\n", "body/mode");
		return ret;
	}	
	EncMode[ret] = 0;

	//读取keyB的密钥密文
	 if ((ret = UnionReadRequestXMLPackageValue("body/keyB", keyBCipherTextByLmk, sizeof(keyBCipherTextByLmk))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM3:: UnionReadRequestXMLPackageValue[%s]!\n", "body/keyB");
		return ret;
	}
	keyBCipherTextByLmk[ret] = 0;

	//读取keyB的密钥类型
	if ((ret = UnionReadRequestXMLPackageValue("body/typeB", keyBType, sizeof(keyBType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM3:: UnionReadRequestXMLPackageValue[%s]!\n", "body/typeB");
		return ret;
	}
	keyBType[ret] = 0;

	//读取keyB密钥长度
	if ((ret = UnionReadRequestXMLPackageValue("body/lenB", keyBlength, sizeof(keyBlength))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM3:: UnionReadRequestXMLPackageValue[%s]!\n", "body/lenB");
		return ret;
	}
	keyBlength[ret] = 0;

	// 读取初始化向量
	if ((memcmp(EncMode,"10",2) == 0) || (memcmp(EncMode,"11",2) == 0) || (memcmp(EncMode,"12",2) == 0))
        {
		if ((ret = UnionReadRequestXMLPackageValue("body/IV", IV, sizeof(IV))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEEM3:: UnionReadRequestXMLPackageValue[%s]!\n", "body/IV");
			return ret;
		}
		IV[ret] = 0;
	}
	
	memset(dstkey, 0, sizeof(dstkey));
	if(( ret = RacalCmdTS(EncMode, keyACipherTextByLmk, keyBCipherTextByLmk, IV, 0, NULL, 0, NULL, dstkey)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM3:: RacalCmdTS ERROR!\n");
		return ret;	
	}


	memset(tmpbuf, 0, sizeof(tmpbuf));
	len = bcdhex_to_aschex(dstkey, ret, tmpbuf);
	snprintf(dstkey, sizeof(dstkey), "%s", tmpbuf);
	snprintf(tmpbuf, sizeof(tmpbuf), "%d", len);
	
	if ((ret = UnionSetResponseXMLPackageValue("body/keyLen", tmpbuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM3:: UnionSetResponseXMLPackageValue keyLen[%s]!\n", tmpbuf);
		return ret;
	}

	if ((ret = UnionSetResponseXMLPackageValue("body/dstkey",dstkey)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM3:: UnionSetResponseXMLPackageValue dstkey[%s]!\n",dstkey);
		return ret;
	}
	return 0;	
}


