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

//申请KMC，用与保护密钥数据或计算mac密钥(902)

int UnionDealServiceCodeEA02(PUnionHsmGroupRec phsmGroupRec)
{
	int                             ret;
	char				flag[16];
        TUnionSymmetricKeyDB            symmetricKeyDB;
	PUnionSymmetricKeyValue         symmetricKeyValue = NULL;
	char				keyValue[128];
	char				keyLengthFlag;
	TUnionDesKeyType		masterKeyType;
	TUnionDesKeyType              	keyType;
	char				tmpbuf[16];
	char				encFlags[16];
	char				Data[1024];
	char				IV[64];
	char				derivedKey[128];	

	//读取生成密钥标志
	memset(flag, 0, sizeof(flag));
	if ((ret = UnionReadRequestXMLPackageValue("body/flag", flag, sizeof(flag))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM2:: UnionReadRequestXMLPackageValue[%s]!\n", "body/flag");
		return ret;
	}
	//	UnionLog("in UnionDealServiceCodeEEM2:: flag=%s\n",flag);	
	
	if(flag[0] == '0')
	{
		//读取密钥名称
        	memset(&symmetricKeyDB, 0, sizeof(symmetricKeyDB));
        	if ((ret = UnionReadRequestXMLPackageValue("body/keyName", symmetricKeyDB.keyName, sizeof(symmetricKeyDB.keyName))) < 0)
        	{
        	        UnionUserErrLog("in UnionDealServiceCodeEEM2:: UnionReadRequestXMLPackageValue[%s]!\n", "body/keyName");
        	        return(ret);
        	}
        	if ((ret = UnionReadSymmetricKeyDBRec(symmetricKeyDB.keyName, 1, &symmetricKeyDB)) < 0)
        	{
        	        UnionUserErrLog("in UnionDealServiceCodeEEM2::  UnionReadSymmetricKeyDBRec keyName[%s]!\n", symmetricKeyDB.keyName);
        	        return(ret);
        	}
        	if ((symmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB, phsmGroupRec->lmkProtectMode)) == NULL)
        	{
        	        UnionUserErrLog("in UnionDealServiceCodeEEM2:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n", phsmGroupRec->lmkProtectMode);
        	        UnionSetResponseRemark("读取源密钥值失败");
        	        return(errCodeParameter);
        	}
		
		sprintf(keyValue, "%s", symmetricKeyValue->keyValue);
		//sprintf(masterKeyType, "%s", symmetricKeyDB.keyType);
 		masterKeyType = symmetricKeyDB.keyType;

		if(symmetricKeyDB.keyLen == 64)
			keyLengthFlag = '0';
		if(symmetricKeyDB.keyLen == 128)
			keyLengthFlag = '1';

		//读取离散数据
		memset(Data, 0, sizeof(Data));
		if ((ret = UnionReadRequestXMLPackageValue("body/data", Data, sizeof(Data))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEEM2:: UnionReadRequestXMLPackageValue[%s]!\n", "body/data");
			return ret;
		}
	}
	else if(flag[0] == '1')
	{
		//读取密钥值
		memset(keyValue, 0, sizeof(keyValue));
		if((ret = UnionReadRequestXMLPackageValue("body/keyValue", keyValue, sizeof(keyValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEEM2:: UnionReadRequestXMLPackageValue[%s]!\n", "body/keyValue");
			return ret;
		}
		if(strlen(keyValue) == 16)
			keyLengthFlag = '0';
		if(strlen(keyValue) == 32)
			keyLengthFlag = '1';

		//读取密钥类型
		if((ret = UnionReadRequestXMLPackageValue("body/masterkeyType", tmpbuf, sizeof(tmpbuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEEM2:: UnionReadRequestXMLPackageValue[%s]!\n", "body/masterkeyType");
			return ret;
		}
		tmpbuf[ret] = 0;
		if(strncmp(tmpbuf, "000", 3) == 0)
                	masterKeyType= conZMK;
        	if(strncmp(tmpbuf, "008", 3) == 0)
                	masterKeyType= conZAK;
        	if(strncmp(tmpbuf, "00A", 3) == 0)
                	masterKeyType= conZEK;		
	
		if((ret = UnionReadRequestXMLPackageValue("body/GCdata", Data, sizeof(Data))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEEM2:: UnionReadRequestXMLPackageValue[%s]!\n", "body/GCdata");
			return ret;
		}
		Data[ret] = 0;

	}
	else
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM2:: flag != 0 and flag !=1!\n");
		return(errCodeParameter);
	}

	//读取子密钥模板
	if((ret = UnionReadRequestXMLPackageValue("body/ChkeyType", tmpbuf, sizeof(tmpbuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM2:: UnionReadRequestXMLPackageValue[%s]!\n", "body/ChkeyType");
		return ret;
	}
	tmpbuf[ret] = 0;
	if(strncmp(tmpbuf, "000", 3) == 0)
		keyType	= conZMK;
	if(strncmp(tmpbuf, "008", 3) == 0)
		keyType = conZAK;
	if(strncmp(tmpbuf, "00A", 3) == 0)
		keyType = conZEK;

	//取算法标志
	if((ret = UnionReadRequestXMLPackageValue("body/algorithmID",encFlags, sizeof(encFlags))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM2:: UnionReadRequestXMLPackageValue[%s]!\n", "body/algorithmID");
		return ret;
	}
	encFlags[ret] = 0;
	
	memset(IV, 0, sizeof(IV));	
	if (strncmp(encFlags,"01",2) == 0)
	{
		//读取向量
		if((ret = UnionReadRequestXMLPackageValue("body/IV", IV, sizeof(IV))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEEM2:: UnionReadRequestXMLPackageValue[%s]!\n", "body/IV");
			return ret;
		}
		IV[ret] = 0;
	}
	
	memset(derivedKey, 0, sizeof(derivedKey));
	if((ret = UnionHsmCmdX1(encFlags, masterKeyType, keyValue,keyType,&keyLengthFlag,Data,IV,derivedKey)) < 0)	
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM2:: UnionHsmCmdTU ERROR!\n");
		return ret;	
	}
	
	//设置返回值
	if ((ret = UnionSetResponseXMLPackageValue("body/KeyValue",derivedKey)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM2:: UnionSetResponseXMLPackageValue KeyValue[%s]!\n",derivedKey);
		return ret;
	}
	return 0;	
}


