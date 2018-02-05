//	Author:		张永定	
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

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
#include "essc5Interface.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "defaultIBMPinOffsetDef.h"
#include "asymmetricKeyDB.h"
#include "UnionMD5.h"
#include "UnionSHA1.h"
#include "unionHsmCmdVersion.h"

/***************************************
服务代码:	E172
服务名:		公钥加密
功能描述:	公钥加密
***************************************/
int UnionDealServiceCodeE172(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				mode;
	int				maxData = 4096;
	char				keyName[128];
	char				pkValue[1024];
	int				pkExponent = 0;
	char				algorithmID[32];
	char				dataFillMode[32];
	//char				fillData[1024];
	char				plainData[1132];
	int				lenOfData = 0;
	char				cipherData[4096];
	char				tmpBuf[32];
	char				tmpData[2048];
	int				lenOfCipherData = 0;
	//int				addLen = 0;
	int				keyLen = 0;
	int				keyType = 0;
	int				pkExp = 0;
	char				derPK[1024];

	TUnionAsymmetricKeyDB		asymmetricKeyDB;

	// 模式
	// 1：密钥名称
	// 2：外带公钥	
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE172:: UnionReadRequestXMLPackageValue[%s]!\n","body/mode");
		return(ret);
	}
	else
	{
		tmpBuf[ret] = 0;
		mode = atoi(tmpBuf);
	}

	if ((mode != 1) && (mode != 2))
	{
		UnionUserErrLog("in UnionDealServiceCodeE172:: mode[%d] Invalid\n",mode);
		UnionSetResponseRemark("模式[%d]无效",mode);
		return(errCodeParameter);
	}
	
	if ((lenOfData = UnionReadRequestXMLPackageValue("body/plainData",plainData,sizeof(plainData))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE172:: UnionReadRequestXMLPackageValue[%s]!\n","body/plainData");
		return(lenOfData);
	}
	else if (lenOfData == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE172:: plainData can not be null!\n");
		UnionSetResponseRemark("明文数据不能为空");
		return(errCodeParameter);
	}
	plainData[lenOfData] = 0;

	// 读取数据补位方式
	if ((ret = UnionReadRequestXMLPackageValue("body/dataFillMode",dataFillMode,sizeof(dataFillMode))) <= 0)
		strcpy(dataFillMode,"1");	
	
	// 模式
	if (mode == 1)	// 密钥名称
	{
		// 读取密钥名称
		if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE172:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
			return(ret);
		}
		keyName[ret] = 0;
		UnionFilterHeadAndTailBlank(keyName);
		
		if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,1,&asymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE172:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",keyName);
			return(ret);
		}

		// 检测私钥类型
		if (asymmetricKeyDB.keyType != 1 && asymmetricKeyDB.keyType != 2)
		{
			UnionUserErrLog("in UnionDealServiceCodeE172:: keyType[%d] is not 1 or 2!\n",asymmetricKeyDB.keyType);
			UnionSetResponseRemark("私钥类型[%d]不支持加密,必须为[1或2]",asymmetricKeyDB.keyType);
			return(errCodeHsmCmdMDL_InvalidKeyType);
		}
		
		if (asymmetricKeyDB.algorithmID == conAsymmetricAlgorithmIDOfRSA)	// RSA算法
		{
			keyType = conAsymmetricAlgorithmIDOfRSA;
			pkExp = asymmetricKeyDB.pkExponent;
			keyLen = sprintf(pkValue,"%s",asymmetricKeyDB.pkValue);
			pkValue[keyLen] = 0;
		}
		else // SM2
		{
			keyType = conAsymmetricAlgorithmIDOfSM2;
			keyLen = sprintf(pkValue,"%s",asymmetricKeyDB.pkValue);
			pkValue[keyLen] = 0;
		}
	}
	else   // 外带公钥
	{
		//公钥	mode为2时存在
		if ((keyLen = UnionReadRequestXMLPackageValue("body/pkValue",pkValue,sizeof(pkValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE172:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkValue");
			return(keyLen);
		}
		if (keyLen == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE172:: pkValue can not be null!\n");
			UnionSetResponseRemark("公钥值不能为空");
			return(errCodeParameter);
		}

		keyLen = UnionFilterHeadAndTailBlank(pkValue);
		pkValue[keyLen] = 0;

		//算法标识	mode为2时存在，RSA、SM2
		if ((ret = UnionReadRequestXMLPackageValue("body/algorithmID",algorithmID,sizeof(algorithmID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE172:: UnionReadRequestXMLPackageValue[%s]!\n","body/algorithmID");
			return(ret);
		}
		UnionFilterHeadAndTailBlank(algorithmID);
		
		if (strcasecmp(algorithmID,"RSA") == 0)
		{
			keyType = conAsymmetricAlgorithmIDOfRSA;

			// 获取公钥指数
			if ((ret = UnionReadRequestXMLPackageValue("body/pkExponent",tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE172:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkExponent");
				return(ret);
			}
			tmpBuf[ret] = 0;
			UnionFilterHeadAndTailBlank(tmpBuf);
			if (atoi(tmpBuf) != 3 && atoi(tmpBuf) != 65537)
			{
				UnionUserErrLog("in UnionDealServiceCodeE172:: pkExponent[%d] error!\n",atoi(tmpBuf));
				UnionSetResponseRemark("公钥指数[%s]错误",tmpBuf);
				return(errCodeParameter);
			}
			pkExp = atoi(tmpBuf);
		}
		else if (strcasecmp(algorithmID,"SM2") == 0)
			keyType = conAsymmetricAlgorithmIDOfSM2;
		else
		{
			UnionUserErrLog("in UnionDealServiceCodeE172:: algorithmID[%s]!\n",algorithmID);
			UnionSetResponseRemark("算法标识[%s]不正确",algorithmID);
			return(errCodeParameter);
		}
	}
	
	// 填充数据
	//comment out by zhouxw
	/*
        if ((lenOfData % keyLen != 0) && dataFillMode[0] == '0' && keyType == conAsymmetricAlgorithmIDOfRSA)
        {
                addLen = keyLen - lenOfData % keyLen;
		memset(fillData,'0',addLen);
		memcpy(fillData+addLen,plainData,lenOfData);
		lenOfData = keyLen;
		memcpy(plainData,fillData,lenOfData);
		plainData[lenOfData] = 0;	
        }
	*/
	if(dataFillMode[0] == '0' && keyType == conAsymmetricAlgorithmIDOfRSA && lenOfData > keyLen)
	{
		lenOfData = keyLen;
		plainData[lenOfData] = 0;
	}

	switch(keyType)
	{
		case	conAsymmetricAlgorithmIDOfRSA:	// RSA算法
			// 获取公钥指数
                        pkExponent = UnionGetCurrentRsaExponent();

                        // 设置公钥指数
                        if (pkExponent != pkExp)
                                UnionSetCurrentRsaExponent(pkExp);

                        // 将裸公钥, 编码成DER格式
                        if ((ret = UnionFormANSIDERRSAPK(pkValue,keyLen,derPK,sizeof(derPK))) < 0)
                        {
                                UnionUserErrLog("in UnionDealServiceCodeE172:: UnionPKCSASN1DEREncodeExt!\n");
                                UnionSetResponseRemark("裸公钥转化DER失败");
                                return(ret);
                        }
                        bcdhex_to_aschex(derPK,ret,pkValue);
                        pkValue[ret*2] = 0;

                        // 还原公钥指
                        if (pkExponent != pkExp)
                                UnionSetCurrentRsaExponent(pkExponent);

			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmd30(dataFillMode[0],NULL,pkValue,lenOfData,plainData,cipherData)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE172:: UnionHsmCmd30!\n");
						return(ret);
					}
					cipherData[ret] = 0;
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE172:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}
			break;
		case	conAsymmetricAlgorithmIDOfSM2:	// SM2算法
			if (lenOfData + 192 > maxData)
			{
				UnionUserErrLog("in UnionDealServiceCodeE172:: data len[%d] > [%d]!\n",lenOfData,(maxData - 192)/2);
				UnionSetResponseRemark("数据长度len[%d] > [%d]",lenOfData,(maxData - 192)/2);
				return(errCodeParameter);
			}

			aschex_to_bcdhex(plainData,lenOfData,tmpData);
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					// modify by leipp 20150730 从K5改为KE
					if ((ret = UnionHsmCmdKE(-1,strlen(pkValue),pkValue,lenOfData/2,tmpData,&lenOfCipherData,(unsigned char *)(tmpData))) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE172:: UnionHsmCmdKE!\n");
						return(ret);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE172:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}

			// 设置密文数据
			bcdhex_to_aschex(tmpData,lenOfCipherData,cipherData);
			cipherData[lenOfCipherData*2] = 0;
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE172: asymmetricKeyDB.algorithmID[%d] error!\n",keyType);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	// 设置响应密文数据
	if ((ret = UnionSetResponseXMLPackageValue("body/cipherData",cipherData)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE172:: UnionSetResponseXMLPackageValue[%s]!\n","body/cipherData");
		return(ret);
	}
	
	return(0);
}
