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
#include "symmetricKeyDB.h"

/***************************************
  服务代码:	E171
  服务名:	公钥验签
  功能描述:	公钥验签
 ***************************************/
int UnionDealServiceCodeE171(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				mode;
	char				keyName[160];
	char				pkValue[1024];
	char				algorithmID[32];
	char				dataFillMode[32];
	char				hashID[32];
	char				data[4096];
	char				tmpBuf[1024];
	char				sign[1024];
	char				userID[1024];
	char				tmpData[5120];
	int				vkIndex = -1;
	int				keyLen = 0;
	int				lenOfSign = 0;
	int				keyType;
	int 				pkExponent = 65537;
	int 				tpkExponent = 0;
	char				shapadStr[]="3021300906052B0E03021A05000414";
	char				md5padStr[]="3020300C06082A864886F70D020505000410";
	int				lenOfData = 0;

	TUnionAsymmetricKeyDB		asymmetricKeyDB;

	// 模式
	// 1：密钥名称
	// 2：外带公钥	
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE171:: UnionReadRequestXMLPackageValue[%s]!\n","body/mode");
		return(ret);
	}
	else
	{
		tmpBuf[ret] = 0;
		mode = atoi(tmpBuf);
	}

	if ((mode != 1) && (mode != 2))
	{
		UnionUserErrLog("in UnionDealServiceCodeE171:: mode[%d] Invalid\n",mode);
		UnionSetResponseRemark("模式mode[%d]无效",mode);
		return(errCodeParameter);
	}

	// 签名数据
	if ((lenOfData = UnionReadRequestXMLPackageValue("body/data",data,sizeof(data))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE171:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
		return(lenOfData);
	}
	data[lenOfData] = 0;

	// 签名
	if ((lenOfSign = UnionReadRequestXMLPackageValue("body/sign",sign,sizeof(sign))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE171:: UnionReadRequestXMLPackageValue[%s]!\n","body/sign");
		return(lenOfSign);
	}
	UnionFilterHeadAndTailBlank(sign);
	if ((lenOfSign = strlen(sign)) > 0)
	{
		if (!UnionIsBCDStr(sign))
		{
			UnionUserErrLog("in UnionDealServiceCodeE171:: sign[%s] is error!\n",sign);
			UnionSetResponseRemark("签名非法,必须为十六进制数");
			return(errCodeParameter);
		}
	}
	sign[lenOfSign] = 0;

	tpkExponent = UnionGetCurrentRsaExponent();	
	// 模式
	if (mode == 1)	// 密钥名称
	{
		// 读取密钥名称
		if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE171:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
			return(ret);
		}
		UnionFilterHeadAndTailBlank(keyName);

		if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,1,&asymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE171:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",keyName);
			return(ret);
		}

		// 检测私钥类型
		if (asymmetricKeyDB.keyType != 0 && asymmetricKeyDB.keyType != 2)
		{
			UnionUserErrLog("in UnionDealServiceCodeE171:: keyType[%d] is not 0 or 2!\n",asymmetricKeyDB.keyType);
			UnionSetResponseRemark("私钥类型[%d]不支持签名,必须为[0或2]");
			return(errCodeHsmCmdMDL_InvalidKeyType);
		}

		snprintf(pkValue,sizeof(pkValue),"%s",asymmetricKeyDB.pkValue);

		if (asymmetricKeyDB.algorithmID == conAsymmetricAlgorithmIDOfRSA)
		{
			keyType = conAsymmetricAlgorithmIDOfRSA;
			keyLen = asymmetricKeyDB.keyLen / 8;
			if(asymmetricKeyDB.pkExponent >0)
			{
				UnionSetCurrentRsaExponent(asymmetricKeyDB.pkExponent);
			}
		}
		else
			keyType = conAsymmetricAlgorithmIDOfSM2;

		// add by leipp 20151225 当公钥存在加密机中时,从加密机中获取
		if (asymmetricKeyDB.vkStoreLocation != 0)
			vkIndex = atoi(asymmetricKeyDB.vkIndex);
	}
	else	// 外带公钥
	{
		//公钥	mode为2时存在
		if ((ret = UnionReadRequestXMLPackageValue("body/pkValue",pkValue,sizeof(pkValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE171:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkValue");
			return(ret);
		}
		pkValue[ret] = 0;
		UnionFilterHeadAndTailBlank(pkValue);

		//算法标识	mode为2时存在，RSA、SM2
		if ((ret = UnionReadRequestXMLPackageValue("body/algorithmID",algorithmID,sizeof(algorithmID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE171:: UnionReadRequestXMLPackageValue[%s]!\n","body/algorithmID");
			return(ret);
		}
		UnionFilterHeadAndTailBlank(algorithmID);

		if (strcasecmp(algorithmID,"RSA") == 0)
		{
			// modify by leipp 20150921
			//keyType = conAsymmetricAlgorithmIDOfSM2;	
			keyType = conAsymmetricAlgorithmIDOfRSA;	
			keyLen = strlen(pkValue) / 2;
			if ((ret = UnionReadRequestXMLPackageValue("body/pkExponent",tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UnionSetCurrentRsaExponent(pkExponent);
			}
			else
			{
				tmpBuf[ret] = 0;
				pkExponent =atoi(tmpBuf);
				if(pkExponent != 3 && pkExponent != 65537)
				{
					UnionUserErrLog("in UnionDealServiceCodeE171:: the pkExponent is not 3 or 65537!\n");
					UnionSetResponseRemark("指数不正确[%d]",pkExponent);
					return(errCodeParameter);
				}
				UnionSetCurrentRsaExponent(pkExponent);
			}
		}
		else if (strcasecmp(algorithmID,"SM2") == 0)
			keyType = conAsymmetricAlgorithmIDOfSM2;
		else
		{
			UnionUserErrLog("in UnionDealServiceCodeE171:: algorithmID[%s]!\n",algorithmID);
			UnionSetResponseRemark("算法标识[%s]不正确",algorithmID);
			return(errCodeParameter);
		}
	}

	// 数据补位方式	
	if ((ret = UnionReadRequestXMLPackageValue("body/dataFillMode",dataFillMode,sizeof(dataFillMode))) <= 0)
		snprintf(dataFillMode,sizeof(dataFillMode),"1");
	else
	{
		if ((dataFillMode[0] != '0') && (dataFillMode[0] != '1'))
		{
			UnionUserErrLog("in UnionDealServiceCodeE171:: dataFillMode[%s] error!\n",dataFillMode);
			UnionSetResponseRemark("输入参数错误dataFillMode[%s]",dataFillMode);
			return(errCodeParameter);
		}
	}

	// HASH算法标识
	if ((ret = UnionReadRequestXMLPackageValue("body/hashID",hashID,sizeof(hashID))) > 0)
	{
		// RSA算法
		if (keyType == conAsymmetricAlgorithmIDOfRSA)
		{	
			if (strcmp(hashID,"01") == 0)
			{
				UnionSHA1((unsigned char *)data,lenOfData,(unsigned char *)tmpData);	
				lenOfData = aschex_to_bcdhex(shapadStr,strlen(shapadStr),data);
				memcpy(data+lenOfData,tmpData,20);
				lenOfData += 20;
				data[lenOfData] = 0;
			}
			else if (strcmp(hashID,"02") == 0)
			{
				ret = sprintf(tmpData,"%s",md5padStr);
				UnionMD5((unsigned char *)data,lenOfData,(unsigned char *)tmpData+ret);
				lenOfData = aschex_to_bcdhex(tmpData,ret+32,data);
				data[lenOfData] = 0;
			} 

			if (strcmp(hashID,"03") == 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE171:: hashID[%s] Invalid!\n",hashID);
				UnionSetResponseRemark("RSA算法不支持次hashID[%s]算法标识",hashID);
				return(errCodeParameter);
			}
		}
		else
		{
			if (strcmp(hashID,"03") == 0)
			{
				// 读取用户标识
				if ((ret = UnionReadRequestXMLPackageValue("body/userID",userID,sizeof(userID))) <= 0)
				{
					snprintf(userID,sizeof(userID),"%s","1234567812345678");
					UnionLog("in UnionDealServiceCodeE171:: set default userID[%s]\n",userID);
				}
				else
					userID[ret] = 0;
			}
		}
	}

	switch(keyType)
	{
		case	conAsymmetricAlgorithmIDOfRSA:	// RSA算法

			lenOfSign = aschex_to_bcdhex(sign,lenOfSign,tmpData);
			memcpy(sign,tmpData,lenOfSign);
			sign[lenOfSign] = 0;

			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmd38(dataFillMode[0],NULL,lenOfSign,sign,lenOfData,data,pkValue)) < 0)
					{
						UnionSetCurrentRsaExponent(tpkExponent);
						//modify by lisq 20150313 当前版本密钥失败后尝试旧版本密钥
						if ((mode == 1) && UnionOldVersionAsymmetricKeyIsUsed(&asymmetricKeyDB))
						{
							snprintf(pkValue,sizeof(pkValue),"%s",asymmetricKeyDB.oldPKValue);
							if ((ret = UnionHsmCmd38(dataFillMode[0],NULL,lenOfSign,sign,lenOfData,data,pkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE171:: UnionHsmCmd38 old version pk!\n");
								UnionSetCurrentRsaExponent(tpkExponent);
								return(ret);
							}
						}
						else
						{	UnionUserErrLog("in UnionDealServiceCodeE171:: [%s] old version key not used!\n", asymmetricKeyDB.keyName);
							UnionSetCurrentRsaExponent(tpkExponent);
							return(ret);
						}
						//modify by lisq 20150313 end
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE171:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					UnionSetCurrentRsaExponent(tpkExponent);
					return(errCodeParameter);
			}
			UnionSetCurrentRsaExponent(tpkExponent);
			break;
		case	conAsymmetricAlgorithmIDOfSM2:	// SM2算法

			// 检测数据长度
			// modify by leipp 20151224
			if (strcmp(hashID,"03") != 0)
			{
				if (lenOfData != 64)
				{
					UnionUserErrLog("in UnionDealServiceCodeE171:: data[%s][%d] != 64!\n",data,lenOfData);
					UnionSetResponseRemark("数据长度[%d] != 64",lenOfData);
					return(errCodeParameter);
				}
			}
			else if ((lenOfData % 2 != 0) || (!UnionIsBCDStr(data)))
			{
				UnionUserErrLog("in UnionDealServiceCodeE171:: data[%s] is invalid!\n",data);
				UnionSetResponseRemark("数据不符合十六进制格式");
				return(errCodeParameter);
			}
			// modify end 20151224

			if (strcmp(hashID,"03") != 0)
				strcpy(hashID,"01");
			else
				strcpy(hashID,"02");

			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmdK4(vkIndex,hashID,strlen(userID),userID,strlen(pkValue),pkValue,lenOfSign,sign,lenOfData,data)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE171:: UnionHsmCmdK4!\n");
						//modify by lisq 20150313 当前版本密钥失败后尝试旧版本密钥
						if ((mode == 1) && UnionOldVersionAsymmetricKeyIsUsed(&asymmetricKeyDB))
						{
							snprintf(pkValue,sizeof(pkValue),"%s",asymmetricKeyDB.oldPKValue);
							if ((ret = UnionHsmCmdK4(vkIndex,hashID,strlen(userID),userID,strlen(pkValue),pkValue,lenOfSign,sign,strlen(data),data)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE171:: UnionHsmCmdK4 old version pk!\n");
								return(ret);
							}
						}
						else
						{
							UnionUserErrLog("in UnionDealServiceCodeE171:: [%s] old version key not used!\n", asymmetricKeyDB.keyName);
							return(ret);
						}
						//modify by lisq 20150313 end
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE171:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE171: algorithmID[%d] error!\n",keyType);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	return(0);
}
