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

//将密钥转加密为密码机主密钥加密（901）

int UnionDealServiceCodeEA01(PUnionHsmGroupRec phsmGroupRec)
{
	int                             ret;
        TUnionSymmetricKeyDB            symmetricKeyDB;
	PUnionSymmetricKeyValue         symmetricKeyValue = NULL;
	char				kekType[16];
	char				srcEncMode[16];
	char                    	keyType[16];
	char				keyCipherTextByKek[256];
	char				IV[64];
	char				keyCipherTextByLmk[256];
	char				keyCipherTextByKekLen ;
	int				keyCipherTextByLmkLen ;

	//读取KEK密钥名称
        memset(&symmetricKeyDB, 0, sizeof(symmetricKeyDB));
        if ((ret = UnionReadRequestXMLPackageValue("body/keyName", symmetricKeyDB.keyName, sizeof(symmetricKeyDB.keyName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEA01:: UnionReadRequestXMLPackageValue[%s]!\n", "body/keyName");
                return(ret);
        }
        if ((ret = UnionReadSymmetricKeyDBRec(symmetricKeyDB.keyName, 1, &symmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEA01::  UnionReadSymmetricKeyDBRec keyName[%s]!\n", symmetricKeyDB.keyName);
                return(ret);
        }
        if ((symmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB, phsmGroupRec->lmkProtectMode)) == NULL)
        {
                UnionUserErrLog("in UnionDealServiceCodeEA01:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n", phsmGroupRec->lmkProtectMode);
                UnionSetResponseRemark("读取源密钥值失败");
                return(errCodeParameter);
        }
	memset(kekType, 0, sizeof(kekType));
	UnionTranslateHsmKeyTypeString(symmetricKeyDB.keyType, kekType);

	//读取加密算法
	memset(srcEncMode, 0, sizeof(srcEncMode));
	if ((ret = UnionReadRequestXMLPackageValue("body/mode", srcEncMode ,sizeof(srcEncMode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEA01:: UnionReadRequestXMLPackageValue[%s]!\n", "body/mode");
		return(ret);
	}

	//读取待加密的密钥类型
	memset(keyType, 0, sizeof(keyType));	
	if ((ret = UnionReadRequestXMLPackageValue("body/type", keyType, sizeof(keyType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEA01:: UnionReadRequestXMLPackageValue[%s]!\n", "body/type");
		return(ret);
	}

	//读取kek加密的密钥密文
	memset(keyCipherTextByKek, 0, sizeof(keyCipherTextByKek));
	if ((ret = UnionReadRequestXMLPackageValue("body/CipherText", keyCipherTextByKek, sizeof(keyCipherTextByKek))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEA01:: UnionReadRequestXMLPackageValue[%s]!\n", "body/CipherText");
		return(ret);
	}

	//获取密钥单双倍长度	
	if (strlen(keyCipherTextByKek) == 16)
                keyCipherTextByKekLen = '0';
        else if (strlen(keyCipherTextByKek) == 32)
                keyCipherTextByKekLen = '1';
	else
	{
		UnionUserErrLog("in UnionDealServiceCodeEA01::keyCipherTextByKek != 16 && keyCipherTextByKek != 32\n");
		return(errCodeParameter);
	}
	
	//读取IV
	if ((memcmp(srcEncMode,"10",2) == 0) || (memcmp(srcEncMode,"11",2) == 0) || (memcmp(srcEncMode,"12",2) == 0))
        {
                //读取IV
                memset(IV,0,sizeof(IV));
		if ((ret = UnionReadRequestXMLPackageValue("body/IV", IV, sizeof(IV))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEA01:: UnionReadRequestXMLPackageValue[%s]!\n", "body/IV");
			return(ret);
		}	
	}

	memset(keyCipherTextByLmk, 0, sizeof(keyCipherTextByLmk));
	keyCipherTextByLmkLen = sizeof(keyCipherTextByLmk);
	if( (ret = UnionHsmCmdTU(srcEncMode,kekType,keyCipherTextByKekLen,symmetricKeyValue->keyValue,IV,keyType,keyCipherTextByKekLen,strlen(keyCipherTextByKek),keyCipherTextByKek,&keyCipherTextByLmkLen,keyCipherTextByLmk)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEA01:: UnionHsmCmdTU ERROR!\n");
		return ret;	
	}

	//设置返回值
	if ((ret = UnionSetResponseXMLPackageValue("body/keyCipherTextByLmk",keyCipherTextByLmk)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEA01:: UnionSetResponseXMLPackageValue keyCipherTextByLmk[%s]!\n",keyCipherTextByLmk);
		return ret;
	}
	return 0;	
}


