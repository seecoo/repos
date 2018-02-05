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

//功能：
//        将数据由KEK加密，转加密为B密钥加密  (HSM:UE)

int UnionDealServiceCodeEA06(PUnionHsmGroupRec phsmGroupRec)
{
	int                             ret;
	TUnionSymmetricKeyDB            symmetricKeyDB;
        PUnionSymmetricKeyValue         symmetricKeyValue = NULL;
	char				DisperseData[256];
	char				DisperseTimes[16];
	char				srcEncMode[16];
	char				srcIV[64];
	TUnionSymmetricKeyDB            symmetricKeyDB1;
	PUnionSymmetricKeyValue         symmetricKeyValue1 = NULL;
	char				DisperseData2[256];
	char				DisperseTimes2[16];
	char				dstEncMode[16];
	char				dstIV[64];
	char				ciperData[256];
	int				i = 0;
	char				X1Data[256];
	char				keyLengthFlag;
	char				tmpbuf[256];
	char				tmpbufRes[256];
	char				keyNamevalue[128];
	char				keyName2value[128];	
	char				keyType[16];
	char				keyType2[16];
	char				dstCiphertext[256];
	

	//读取密钥源加密密钥在平台的存储名称
        memset(&symmetricKeyDB, 0, sizeof(symmetricKeyDB));
        if ((ret = UnionReadRequestXMLPackageValue("body/keyName", symmetricKeyDB.keyName, sizeof(symmetricKeyDB.keyName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEM1:: UnionReadRequestXMLPackageValue[%s]!\n", "body/keyName");
                return(ret);
        }
        if ((ret = UnionReadSymmetricKeyDBRec(symmetricKeyDB.keyName, 1, &symmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEM1::  UnionReadSymmetricKeyDBRec keyName[%s]!\n", symmetricKeyDB.keyName);
                return(ret);
        }
        if ((symmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB, phsmGroupRec->lmkProtectMode)) == NULL)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEM1:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n", phsmGroupRec->lmkProtectMode);
                UnionSetResponseRemark("读取源密钥值失败");
                return(errCodeParameter);
        }
	
	//源密钥的离散次数
	if ((ret = UnionReadRequestXMLPackageValue("body/DisperseTimes", DisperseTimes, sizeof(DisperseTimes))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM6:: UnionReadRequestXMLPackageValue[%s]!\n", "body/DisperseTimes");
		return ret;
	}
	DisperseTimes[ret] = 0;

	//源密钥离散数据
	if ((ret = UnionReadRequestXMLPackageValue("body/DisperseData", DisperseData, sizeof(DisperseData))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM6:: UnionReadRequestXMLPackageValue[%s]!\n", "body/DisperseData");
		return ret;
	}
	DisperseData[ret] = 0;

	if(atoi(DisperseTimes) * 16 != strlen(DisperseData))
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM6::DisperseTimes) * 16 != strlen(DisperseData)!\n");
		return (errCodeParameter);
	}

	//目的密钥名称
	memset(&symmetricKeyDB1, 0, sizeof(symmetricKeyDB1));
        if ((ret = UnionReadRequestXMLPackageValue("body/keyName2", symmetricKeyDB1.keyName, sizeof(symmetricKeyDB1.keyName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEM6:: UnionReadRequestXMLPackageValue[%s]!\n", "body/keyName2");
                return(ret);
        }
        if ((ret = UnionReadSymmetricKeyDBRec(symmetricKeyDB1.keyName, 1, &symmetricKeyDB1)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEM6::  UnionReadSymmetricKeyDBRec keyName[%s]!\n", symmetricKeyDB1.keyName);
                return(ret);
        }
        if ((symmetricKeyValue1 = UnionGetSymmetricKeyValue(&symmetricKeyDB1, phsmGroupRec->lmkProtectMode)) == NULL)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEM6:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n", phsmGroupRec->lmkProtectMode);
                UnionSetResponseRemark("读取源密钥值失败");
                return(errCodeParameter);
        }
	
	//目的密钥离散次数
	if ((ret = UnionReadRequestXMLPackageValue("body/DisperseTimes2", DisperseTimes2, sizeof(DisperseTimes2))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM6:: UnionReadRequestXMLPackageValue[%s]!\n", "body/DisperseTimes2");
		return ret; 
	}
	DisperseTimes2[ret] = 0;
	
	//目的密钥离散数据
	if ((ret = UnionReadRequestXMLPackageValue("body/DisperseData2", DisperseData2, sizeof(DisperseData2))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM6:: UnionReadRequestXMLPackageValue[%s]!\n", "body/DisperseData2");
		return ret;
	}
	DisperseData2[ret] = 0;

	//数据密文
	if ((ret = UnionReadRequestXMLPackageValue("body/ciperData", ciperData, sizeof(ciperData))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM6:: UnionReadRequestXMLPackageValue[%s]!\n", "body/ciperData");
		return ret;
	}
	ciperData[ret] = 0;

	//读取数据源加密算法
	if ((ret = UnionReadRequestXMLPackageValue("body/srcEncMode", srcEncMode, sizeof(srcEncMode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM6:: UnionReadRequestXMLPackageValue[%s]!\n", "body/srcEncMode");
		return ret;
	}
	srcEncMode[ret] = 0;

	//读取源加密算法使用的初始向量
	srcIV[0] = 0;
	if((memcmp(srcEncMode,"10",2) == 0) || (memcmp(srcEncMode,"11",2) == 0) || (memcmp(srcEncMode,"12",2) == 0))
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/srcIV", srcIV, sizeof(srcIV))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEEM6:: UnionReadRequestXMLPackageValue[%s]!\n", "body/srcIV");
			return ret;
		}
		srcIV[ret] = 0;
	}	

	//读取目的加密算法
	if ((ret = UnionReadRequestXMLPackageValue("body/dstEncMode", dstEncMode, sizeof(dstEncMode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM6:: UnionReadRequestXMLPackageValue[%s]!\n", "body/dstEncMode");
		return ret;
	}
	dstEncMode[ret] = 0;
	
	//读取目的加密法使用的初始向量
	dstIV[0] = 0;
	if ((memcmp(dstEncMode,"10",2) == 0) || (memcmp(dstEncMode,"11",2) == 0) || (memcmp(dstEncMode,"12",2) == 0))
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/dstIV", dstIV, sizeof(dstIV))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEEM6:: UnionReadRequestXMLPackageValue[%s]!\n", "body/dstIV");
			return ret;
		}	
		dstIV[ret] = 0;
	}

	memset(tmpbuf, 0, sizeof(tmpbuf));
        memset(tmpbufRes, 0, sizeof(tmpbufRes));
        strcpy(tmpbuf, symmetricKeyValue->keyValue);

	if(strlen(symmetricKeyValue->keyValue) == 16)
		keyLengthFlag = '0';
	else if(strlen(symmetricKeyValue->keyValue) == 32)
		keyLengthFlag = '1';
	else 
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM6:: symmetricKeyValue->keyValue ERROR!\n");
		return (errCodeParameter);
	}

	for(i = 0; i < atoi(DisperseTimes); i++)
	{
		//对离散数据取反合并
                memset(X1Data, 0, sizeof(X1Data));
                strncpy(X1Data,DisperseData+i*16,16);
                if( (ret = UnionCPL(X1Data, 16, X1Data+16)) < 0)
                {
			UnionUserErrLog("in UnionDealServiceCodeEEM6::UnionCPL ERROR!\n");
			return ret;
		}
		//UnionLog("symmetricKeyValue->keyValue = %s\n", tmpbuf);		
		if((ret = UnionHsmCmdX1("02",symmetricKeyDB.keyType,tmpbuf,symmetricKeyDB.keyType,&keyLengthFlag, X1Data,srcIV, tmpbufRes)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEEM6:: UnionHsmCmdX1 ERROR!\n");
			return ret;
		}
		memset(tmpbuf, 0, sizeof(tmpbuf));
                strcpy(tmpbuf,tmpbufRes);
                memset(tmpbufRes, 0, sizeof(tmpbufRes));

	}
	
	snprintf(keyNamevalue, sizeof(keyNamevalue), "%s", tmpbuf);
	//UnionLog("keyNamevalue=%s\n", keyNamevalue);	

	memset(tmpbuf, 0, sizeof(tmpbuf));
        memset(tmpbufRes, 0, sizeof(tmpbufRes));
        strcpy(tmpbuf, symmetricKeyValue1->keyValue);

        if(strlen(symmetricKeyValue1->keyValue) == 16)
                keyLengthFlag = '0';
        else if(strlen(symmetricKeyValue1->keyValue) == 32)
                keyLengthFlag = '1';
        else
        {
                UnionUserErrLog("in UnionDealServiceCodeEEM6:: symmetricKeyValue1->keyValue ERROR!\n");
                return (errCodeParameter);
        }

        for(i = 0; i < atoi(DisperseTimes2); i++)
        {
                //对离散数据取反合并
                memset(X1Data, 0, sizeof(X1Data));
                strncpy(X1Data,DisperseData2+i*16,16);
                if( (ret = UnionCPL(X1Data, 16, X1Data+16)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeEEM6::UnionCPL ERROR!\n");
                        return ret;
                }
                //UnionLog("symmetricKeyValue1->keyValue = %s\n", tmpbuf);
                if((ret = UnionHsmCmdX1("02",symmetricKeyDB1.keyType,tmpbuf,symmetricKeyDB1.keyType,&keyLengthFlag, X1Data, dstIV, tmpbufRes)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeEEM6:: UnionHsmCmdX1 ERROR!\n");
                        return ret;
                }
                memset(tmpbuf, 0, sizeof(tmpbuf));
                strcpy(tmpbuf,tmpbufRes);
                memset(tmpbufRes, 0, sizeof(tmpbufRes));

        }
	snprintf(keyName2value,sizeof(keyName2value), "%s", tmpbuf);
	//UnionLog("keyName2value=%s\n", keyName2value);

	memset(keyType, 0, sizeof(keyType));
	UnionTranslateDesKeyTypeTo3CharFormat(symmetricKeyDB.keyType,keyType);
	memset(keyType2, 0, sizeof(keyType2));
	UnionTranslateDesKeyTypeTo3CharFormat(symmetricKeyDB1.keyType,keyType2);
	memset(dstCiphertext, 0, sizeof(dstCiphertext));
	if((ret = UnionHsmCmdUE(keyType,keyNamevalue, 0, srcEncMode, srcIV, keyType2, keyName2value, 0, dstEncMode,dstIV, strlen(ciperData), ciperData, dstCiphertext)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM6:: UnionHsmCmdUE ERROR!\n");
		return ret;	
	}

	if ((ret = UnionSetResponseXMLPackageValue("body/dstCipher", dstCiphertext)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEM6:: UnionSetResponseXMLPackageValue dstCipher[%s]!\n",dstCiphertext);
		return ret;
	}
	return 0;	
}


