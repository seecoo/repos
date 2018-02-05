//	Author:		lusj
//	Copyright:	Union Tech. Guangzhou
//	Date:		2015-08-20

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "defaultIBMPinOffsetDef.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmdVersion.h"



/*
功能:将一把密钥加密的数据转换为另一把密钥加密（Y1指令）

*/
int UnionDealServiceCodeE164(PUnionHsmGroupRec phsmGroupRec)
{

	int 					ret;
	char					srcKey[136];
	TUnionSymmetricKeyType	srcKeyType=conZEK;
	char					srcMode[2+1];
	char					dstKey[136];
	TUnionSymmetricKeyType	dstKeyType=conZEK;
	char					dstMode[2+1];
	char					srcData[1024+1];
	char					iv_cbc[32+1];
	char					iv_cbcDst[32+1];
	char					dstData[1024+1];
	char					algType[3];
	int 					srcKeyIdx=0;
	int 					dstKeyIdx=0;
	char					srcKeyTypeFromXML[8+1];
	char					dstKeyTypeFromXML[8+1];


	TUnionSymmetricKeyDB		key1;
	PUnionSymmetricKeyValue		psymmetricKeyValue1 = NULL;
	TUnionSymmetricKeyDB		key2;
	PUnionSymmetricKeyValue		psymmetricKeyValue2 = NULL;

	
	//读取源密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/srcKeyName", srcKey, sizeof(srcKey))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE164:: UnionReadRequestXMLPackageValue[%s]!\n","body/srcKeyName");
		return(ret);
	}
	srcKey[ret] = 0;

	
	if (strstr(srcKey,".") == NULL)//密钥名称是以索引的方式送入，那么就读取送入的密钥类型
	{
	
		srcKeyIdx = atoi(srcKey);
		// 读取源密钥类型
		memset(srcKeyTypeFromXML,0,sizeof(srcKeyTypeFromXML));
		if ((ret = UnionReadRequestXMLPackageValue("body/srcKeyType", srcKeyTypeFromXML, sizeof(srcKeyTypeFromXML))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE164:: UnionReadRequestXMLPackageValue[%s]!\n","body/srcKeyType");
			return(ret);
		}

		srcKeyType = atoi(srcKeyTypeFromXML);

	}
	else
	{
		if ((ret = UnionReadSymmetricKeyDBRec(srcKey, 1, &key1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE164:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", srcKey);
			return(ret);
		}
		// 读取对称密钥值
		if ((psymmetricKeyValue1 = UnionGetSymmetricKeyValue(&key1,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE164:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("读取密钥值失败");
			return(errCodeParameter);
		}
		//获取密钥类型
		srcKeyType=key1.keyType;
	}

	//获取源加密算法
	if ((ret = UnionReadRequestXMLPackageValue("body/srcMode", srcMode, sizeof(srcMode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE164:: UnionReadRequestXMLPackageValue[%s]!\n","body/srcMode");
		return(ret);
	}

	//读取目的密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/dstKeyName", dstKey, sizeof(dstKey))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE164:: UnionReadRequestXMLPackageValue[%s]!\n","body/dstKey");
		return(ret);
	}
	dstKey[ret] = 0;
	
	if (strstr(dstKey,".") == NULL)//密钥名称是以索引的方式送入，那么就读取送入的密钥类型
	{
		dstKeyIdx = atoi(dstKey);
		// 读取源密钥类型
		memset(dstKeyTypeFromXML,0,sizeof(dstKeyTypeFromXML));
		/*
		if ((ret = UnionReadRequestXMLPackageValue("body/dstKeyType", dstKeyTypeFromXML, sizeof(dstKeyTypeFromXML))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE164:: UnionReadRequestXMLPackageValue[%s]!\n","body/dstKeyType");
			return(ret);
		}

		dstKeyType = atoi(dstKeyTypeFromXML);
*/
		dstKeyType=atoi(srcKeyTypeFromXML);
	}
	else
	{
		if ((ret = UnionReadSymmetricKeyDBRec(dstKey, 1, &key2)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE164:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", dstKey);
			return(ret);
		}
		// 读取对称密钥值
		if ((psymmetricKeyValue2 = UnionGetSymmetricKeyValue(&key2,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE164:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("读取密钥值失败");
			return(errCodeParameter);
		}
		//获取密钥类型
		dstKeyType=key2.keyType;
	}


	//获取目的加密算法
	if ((ret = UnionReadRequestXMLPackageValue("body/dstMode", dstMode, sizeof(dstMode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE164:: UnionReadRequestXMLPackageValue[%s]!\n","body/dstMode");
		return(ret);
	}

	//获取源数据
	if ((ret = UnionReadRequestXMLPackageValue("body/srcData", srcData, sizeof(srcData))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE164:: UnionReadRequestXMLPackageValue[%s]!\n","body/srcData");
		return(ret);
	}	


	if ((strncmp(srcMode,"10",2) == 0) || (strncmp(srcMode,"11",2) == 0) || (strncmp(srcMode,"12",2) == 0) || (strncmp(srcMode,"20",2) == 0))
	{
		memset(iv_cbc,0,sizeof(iv_cbc));
		if ((ret = UnionReadRequestXMLPackageValue("body/iv_cbc", iv_cbc, sizeof(iv_cbc))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE164:: UnionReadRequestXMLPackageValue[%s]!\n","body/iv_cbc");
			return(ret);
		}	
	}

	//调用转换指令函数
	memset(dstData, 0, sizeof(dstData));
	if (key1.algorithmID == conSymmetricAlgorithmIDOfSM4 || key2.algorithmID == conSymmetricAlgorithmIDOfSM4)
	{
	
		if ((strncmp(dstMode,"10",2) == 0) || (strncmp(dstMode,"12",2) == 0))
		{
			memset(iv_cbcDst,0,sizeof(iv_cbcDst));
        	        if ((ret = UnionReadRequestXMLPackageValue("body/iv_cbcDst", iv_cbcDst, sizeof(iv_cbcDst))) < 0)
        	        {
        	                UnionUserErrLog("in UnionDealServiceCodeE164:: UnionReadRequestXMLPackageValue[%s]!\n","body/iv_cbcDst");
        	                return(ret);
        	        }
		}

		// 读算法转换类型
		memset(algType,0,sizeof(algType));
		if (key1.algorithmID == conSymmetricAlgorithmIDOfSM4 && key2.algorithmID == conSymmetricAlgorithmIDOfSM4)
			strcpy(algType, "03");
		else if (key1.algorithmID != conSymmetricAlgorithmIDOfSM4 && key2.algorithmID == conSymmetricAlgorithmIDOfSM4)
			strcpy(algType, "01");
		else
			strcpy(algType, "02");

		if ((ret = UnionHsmCmdWF(algType, srcKeyType, psymmetricKeyValue1->keyValue, key1.checkValue, srcKeyIdx, srcMode, dstKeyType, psymmetricKeyValue2->keyValue, key2.checkValue, dstKeyIdx, dstMode, strlen(srcData), srcData, iv_cbc, iv_cbcDst, dstData)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE164:: UnionHsmCmdWF[%d]!\n",ret);
			return(ret);
		}
	}
	else
	{
		if ((ret = UnionHsmCmdY1(srcKeyType,psymmetricKeyValue1->keyValue,srcKeyIdx,srcMode,dstKeyType,psymmetricKeyValue2->keyValue,dstKeyIdx,dstMode,strlen(srcData),srcData,iv_cbc,dstData)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE164:: UnionHsmCmdY1 current key ret = [%d]!\n",ret);
			return(ret);
		}
	}

	// 设置转换数据
	if ((ret = UnionSetResponseXMLPackageValue("body/dstData",dstData)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE164:: UnionSetResponseXMLPackageValue dstData[%s]!\n",dstData);
		return(ret);
	}

	return (0);
}


