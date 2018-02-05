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

/***************************************
服务代码:       EA05 (原905服务)
服务名:         对称密钥加密数据
功能描述:       使用指定的密钥加密数据
***************************************/

int UnionDealServiceCodeEA05(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret = 0;
	
	int				lenOfDstData = 0;
	char				keyName[128] = {0};
	char				keyCipherTextByLmk[256] = {0};
	char				keyType[16] = {0};
	char				Flag[16] = {0};
	char				Mode[32] = {0};
	char				IV[64] = {0};
	char				srcData[1024] = {0};
	char				dstData[1024] = {0};
	char				keyLength = 0;

        TUnionSymmetricKeyDB            symmetricKeyDB;
        PUnionSymmetricKeyValue         psymmetricKeyValue = NULL;

	memset(&symmetricKeyDB, 0, sizeof(symmetricKeyDB));
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/keyCipherTextByLmk",keyCipherTextByLmk,sizeof(keyCipherTextByLmk))) < 0)
		{
	                UnionUserErrLog("in UnionDealServiceCodeEEM5:: UnionReadRequestXMLPackageValue[%s] or [%s]!\n","body/keyCipherTextByLmk","body/keyName");
                	return(ret);
		}
		if(strlen(keyCipherTextByLmk) == 16)
		{
			keyLength = '0';
		}
		else if(strlen(keyCipherTextByLmk) == 32)
		{
			keyLength = '1';
		}
		else
		{
			UnionUserErrLog("in UnionDealServiceCodeEEM5:: keyCipherTextByLmk != 16 && keyCipherTextByLmk !=32!\n");	
			return(errCodeParameter);
		}
		//读取密钥类型
                if ((ret = UnionReadRequestXMLPackageValue("body/keyType",keyType,sizeof(keyType))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeEEM5:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyType");
                        return(ret);
                }
	}
	else
	{
		// 读取密钥信息
        	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1,&symmetricKeyDB)) < 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeEEM5:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
                	return(ret);
        	}
        	// 读取密钥值
        	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeEEM5:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
                	UnionSetResponseRemark("读取密钥值失败");
                	return(errCodeParameter);
        	}
		if(symmetricKeyDB.keyLen == 64)
		{
			keyLength = '0';
		}	
		else if(symmetricKeyDB.keyLen == 128)
		{
			keyLength = '1';
		}
		else
		{
			UnionUserErrLog("in UnionDealServiceCodeEEM5:: keyLength is error!\n");
			return(errCodeParameter);
		}
		memcpy(keyCipherTextByLmk,psymmetricKeyValue->keyValue,sizeof(psymmetricKeyValue->keyValue));
		UnionTranslateDesKeyTypeTo3CharFormat(symmetricKeyDB.keyType,keyType);
	}

	
	//读取模式Mode
	if((ret = UnionReadRequestXMLPackageValue("body/Mode",Mode,sizeof(Mode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM5:: UnionReadRequestXMLPackageValue[%s]!\n","body/Mode");
		return(ret);
	}
	Mode[ret] = 0;
	
	//读取加解密标识flag
        if ((ret = UnionReadRequestXMLPackageValue("body/Flag",Flag,sizeof(Flag))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM5:: UnionReadRequestXMLPackageValue[%s]!\n","body/Flag");
                return(ret);
	}
	Flag[ret] = 0;

	//读取初始向量IV
	if(strcmp(Mode,"02") == 0)
	{
        	if ((ret = UnionReadRequestXMLPackageValue("body/IV",IV,sizeof(IV))) < 0)
        	{
                        UnionUserErrLog("in UnionDealServiceCodeEEM5:: UnionReadRequestXMLPackageValue[%s]!\n","body/IV");
                        return(ret);
        	}
		IV[ret] = 0;
	}

	//读取要加密的数据
	if ((ret = UnionReadRequestXMLPackageValue("body/srcData",srcData,sizeof(srcData))) < 0)
        {
		UnionUserErrLog("in UnionDealServiceCodeEEM5:: UnionReadRequestXMLPackageValue[%s]!\n","body/srcData");
                return(ret);
        }
	srcData[ret] = 0;

	memset(dstData, 0, sizeof(dstData));
	if((ret = UnionHsmCmdTG(Flag[0],Mode,keyType,keyLength,keyCipherTextByLmk,IV,strlen(srcData),srcData,&lenOfDstData,dstData)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM5:: UnionHsmCmdTG!\n");
                return(ret);
	}	

	if((ret = UnionSetResponseXMLPackageValue("body/dstData",dstData)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEM5:: UnionSetResponseXMLPackageValue dstData[%s]!\n",dstData);
                return(ret);
        }

	return(0);
}
