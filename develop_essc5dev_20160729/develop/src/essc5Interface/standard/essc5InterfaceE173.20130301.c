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
服务代码:	E173
服务名:		私钥解密
功能描述:	私钥解密

***************************************/
int UnionDealServiceCodeE173(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret = 0;
	char				dataFillMode[32];
	char				vkIndex[32];
	char				algorithmID[32];
	int				cipherDataLen = 0;
	char				cipherData[4096];
	char				plainData[4096];
	char				tmpData[4096];
	int				lenOfVK = 0;
	int				lenOfData = 0;
	char				keyName[160];
	int				mode = 0;
	char				*pos = NULL;
	int				lenOfField = 0;

	TUnionAsymmetricKeyDB		asymmetricKeyDB;

	// 私钥密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) <= 0)
	{
		// 私钥索引
		if ((ret = UnionReadRequestXMLPackageValue("body/vkIndex",asymmetricKeyDB.vkIndex,sizeof(asymmetricKeyDB.vkIndex))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE173:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkIndex");
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE173:: vkIndex can not be null!\n");
			UnionSetResponseRemark("索引不能为空");
			return(errCodeParameter);
		}
		asymmetricKeyDB.vkIndex[ret] = 0;
		UnionFilterHeadAndTailBlank(asymmetricKeyDB.vkIndex);
		snprintf(vkIndex,sizeof(vkIndex),"%02d",atoi(asymmetricKeyDB.vkIndex));
		
		//算法标识  
		if ((ret = UnionReadRequestXMLPackageValue("body/algorithmID",algorithmID,sizeof(algorithmID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE172:: UnionReadRequestXMLPackageValue[%s]!\n","body/algorithmID");
			return(ret);
		}
		algorithmID[ret] = 0;

		if ((asymmetricKeyDB.algorithmID = UnionConvertAsymmetricKeyAlgorithmID(algorithmID)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE172:: UnionConvertAsymmetricKeyAlgorithmID algorithmID[%s] is invalid!\n",algorithmID);
			return(asymmetricKeyDB.algorithmID);
		}
	}
	else
	{
		mode = 1;
		// 读取密钥密钥信息     
		keyName[ret] = 0;

		// add by zhouxw 20160108
		// 华兴银行密钥名称限制
		if (strcasecmp(UnionGetIDOfCustomization(),"GHB") == 0)
		{
			if((pos = strchr(keyName, '.')) == NULL)
			{
				UnionUserErrLog("in UnionDealServiceCodeE173:: wrong keyName[%s] format\n", keyName);
				UnionSetResponseRemark("密钥名格式错");
				return(errCodeParameter);
			}
			lenOfField = pos - keyName + 1;
			if(strncmp(keyName, "GHB_SPE", 7) != 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE173:: wrong keyName[%s] format\n", keyName);
                                UnionSetResponseRemark("密钥名格式错");
                                return(errCodeParameter);
			}
			if((pos = strchr(keyName + lenOfField, '.')) == NULL)
			{
				UnionUserErrLog("in UnionDealServiceCodeE173:: wrong keyName[%s] format\n", keyName);
                                UnionSetResponseRemark("密钥名格式错");
                                return(errCodeParameter);
			}
			lenOfField = pos - keyName + 1;
			if(strncmp(keyName + lenOfField, "RSA", 3) != 0 && strncmp(keyName + lenOfField, "SM2", 3) != 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE173:: wrong keyName[%s] format\n", keyName);
                                UnionSetResponseRemark("密钥名格式错");
                                return(errCodeParameter);
			}
			if(lenOfField + 3 != ret)
			{
				UnionUserErrLog("in UnionDealServiceCodeE173:: wrong keyName[%s] format\n", keyName);
                                UnionSetResponseRemark("密钥名格式错");
                                return(errCodeParameter);
			}
		}
		// add by zhouxw 20160108

		if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,1,&asymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE173:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",asymmetricKeyDB.keyName);
			return(ret);
		}

		// 检测私钥类型
		if (asymmetricKeyDB.keyType != 1 && asymmetricKeyDB.keyType != 2)
		{
			UnionUserErrLog("in UnionDealServiceCodeE173:: keyType[%d] is not 1 or 2!\n",asymmetricKeyDB.keyType);
			UnionSetResponseRemark("私钥类型[%d]不支持加密,必须为[1或2]",asymmetricKeyDB.keyType);
			return(errCodeHsmCmdMDL_InvalidKeyType);
		}
		
		ret = sprintf(vkIndex,"%s",asymmetricKeyDB.vkIndex);
		vkIndex[ret] = 0;

		 // 检测是否存在索引
		if (asymmetricKeyDB.vkStoreLocation == 0)
		{
			snprintf(vkIndex,sizeof(vkIndex),"99");//modify by huangh 20160708
			lenOfVK = aschex_to_bcdhex(asymmetricKeyDB.vkValue,strlen(asymmetricKeyDB.vkValue),tmpData);
			memcpy(asymmetricKeyDB.vkValue,tmpData,lenOfVK);
			asymmetricKeyDB.vkValue[lenOfVK] = 0;
		}
	}
	
	if (asymmetricKeyDB.algorithmID == conAsymmetricAlgorithmIDOfRSA)
	{
		// 公钥加密时采用的填充方式
		if ((ret = UnionReadRequestXMLPackageValue("body/dataFillMode",dataFillMode,sizeof(dataFillMode))) <= 0)
			strcpy(dataFillMode,"1");
	}

	// 密文数据
	if ((cipherDataLen = UnionReadRequestXMLPackageValue("body/cipherData",cipherData,sizeof(cipherData))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE173:: UnionReadRequestXMLPackageValue[%s]!\n","body/cipherData");
		return(cipherDataLen);
	}
	cipherData[cipherDataLen] = 0;
	
	lenOfData = aschex_to_bcdhex(cipherData,cipherDataLen,tmpData);
	
	switch(asymmetricKeyDB.algorithmID)
	{
		case	conAsymmetricAlgorithmIDOfRSA:	// RSA算法
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					// modify by leipp 20160707 增加外带私钥支持
					if ((ret = UnionHsmCmd33(dataFillMode[0],vkIndex,lenOfVK,asymmetricKeyDB.vkValue,lenOfData,tmpData,tmpData,lenOfData)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE173:: UnionHsmCmd33!\n");
						return(ret);
					}
					bcdhex_to_aschex(tmpData,ret,plainData);
					plainData[ret*2] = 0;
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE173:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}

			break;
		case	conAsymmetricAlgorithmIDOfSM2:	// SM2算法
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					// modify by leipp 20150730 从K6改为KF
					if ((ret = UnionHsmCmdKF(atoi(vkIndex),lenOfVK,asymmetricKeyDB.vkValue,lenOfData,(unsigned char *)tmpData,&lenOfData,tmpData)) < 0)
					{
						if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && (atoi(vkIndex) < 0) && (mode == 1) && UnionOldVersionAsymmetricKeyIsUsed(&asymmetricKeyDB))
						{
							lenOfVK = aschex_to_bcdhex(asymmetricKeyDB.oldVKValue,strlen(asymmetricKeyDB.oldVKValue),plainData);
							plainData[lenOfVK] = 0;
							if ((ret = UnionHsmCmdKF(atoi(vkIndex),lenOfVK,plainData,lenOfData,(unsigned char *)tmpData,&lenOfData,tmpData)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE173:: UnionHsmCmdK6 old version vkValue!\n");
								return(ret);
							}
						}
						else
						{
							UnionUserErrLog("in UnionDealServiceCodeE173:: UnionHsmCmdK6 !\n");
							return(ret);
						}
					}
					
					bcdhex_to_aschex(tmpData,ret,plainData);
					plainData[ret*2] = 0;
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE173:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE173:: asymmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}
		
	// 设置响应数据
	if ((ret = UnionSetResponseXMLPackageValue("body/plainData",plainData)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE173:: UnionSetResponseXMLPackageValue[%s]!\n","body/plainData");
		return(ret);
	}
	
	return(0);
}
