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
#include "asymmetricKeyDB.h"
#include "UnionMD5.h"
#include "UnionSHA1.h"
#include "unionHsmCmdVersion.h"

/***************************************
服务代码:	E170
服务名:		私钥签名
功能描述:	私钥签名
***************************************/

int UnionDealServiceCodeE170(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				len = 0;
	int				lenOfVkValue = 0;
	char				keyName[160];
	char				dataFillMode[32];
	char				hashID[32];
	char				data[4096];
	char				tmpData[5120];
	char				sign[1024];
	char				vkIndex[32];
	char				userID[1024];
	char                    	ipAddr[64];
	char                    	sql[1024];
	int				lenOfData = 0;
	char				shapadStr[]="3021300906052B0E03021A05000414";
	char				md5padStr[]="3020300C06082A864886F70D020505000410";

	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	
	// 密钥名称	
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE170:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);
	
	// 读取密钥密钥信息
	if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,1,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE170:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",keyName);
		return(ret);
	}

	// 检测私钥类型
	if (asymmetricKeyDB.keyType != 0 && asymmetricKeyDB.keyType != 2)
	{
		UnionUserErrLog("in UnionDealServiceCodeE170:: keyType[%d] is not 0 or 2!\n",asymmetricKeyDB.keyType);
		UnionSetResponseRemark("私钥类型[%d]不支持签名,必须为[0或2]");
		return(errCodeHsmCmdMDL_InvalidKeyType);
	}

	// 数据补位方式	
	if ((ret = UnionReadRequestXMLPackageValue("body/dataFillMode",dataFillMode,sizeof(dataFillMode))) <= 0)
		snprintf(dataFillMode,sizeof(dataFillMode),"1");
	else
	{
		if ((dataFillMode[0] != '0') && (dataFillMode[0] != '1'))
		{
			UnionUserErrLog("in UnionDealServiceCodeE170:: dataFillMode[%s] error!\n",dataFillMode);
			UnionSetResponseRemark("输入参数错误dataFillMode[%s]",dataFillMode);
			return(errCodeParameter);
		}
	}
	
	// 签名数据
	if ((lenOfData = UnionReadRequestXMLPackageValue("body/data",data,sizeof(data))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE170:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
		return(lenOfData);
	}
	data[lenOfData] = 0;
	
	// HASH算法标识
	if ((ret = UnionReadRequestXMLPackageValue("body/hashID",hashID,sizeof(hashID))) > 0)
	{
		// RSA算法
		if (asymmetricKeyDB.algorithmID == conAsymmetricAlgorithmIDOfRSA)
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
				UnionUserErrLog("in UnionDealServiceCodeE170:: hashID[%s] Invalid!\n",hashID);
				UnionSetResponseRemark("RSA算法不支持次hashID[%s]算法标识",hashID);
				return(errCodeParameter);
			}
		}
		else
		{
			// SM2
			if (strcmp(hashID,"03") == 0)
			{
				// 读取用户标识
				if ((ret = UnionReadRequestXMLPackageValue("body/userID",userID,sizeof(userID))) <= 0)
				{
					snprintf(userID,sizeof(userID),"%s","1234567812345678");
					UnionLog("in UnionDealServiceCodeE170:: set default userID[%s]\n",userID);
				}
				else
					userID[ret] = 0;
			}
		}
	}
	
	lenOfVkValue = aschex_to_bcdhex(asymmetricKeyDB.vkValue,strlen(asymmetricKeyDB.vkValue),tmpData);
	memcpy(asymmetricKeyDB.vkValue,tmpData,lenOfVkValue);
	asymmetricKeyDB.vkValue[lenOfVkValue] = 0;

	len = 0;
	
	//检查密码机	
	if (asymmetricKeyDB.vkStoreLocation == 1)
	{
        	snprintf(sql,sizeof(sql),"select * from hsm where hsmGroupID = '%s' and enabled = 1",asymmetricKeyDB.hsmGroupID);

        	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeE170:: UnionExecRealDBSql[%s]!\n",sql);
                	return(ret);
        	}
        	else if (ret == 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeE170:: hsmGroupID[%s]中没有可用密码机!\n",asymmetricKeyDB.hsmGroupID);
                	return(errCodeParameter);
        	}

        	UnionLocateXMLPackage("detail", 1);

        	if ((ret = UnionReadXMLPackageValue("ipAddr", ipAddr, sizeof(ipAddr))) < 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeE170:: UnionReadXMLPackageValue[%s]!\n","ipAddr");
                	return(ret);
        	}
		ipAddr[ret] = 0;
        	UnionSetUseSpecHsmIPAddrForOneCmd(ipAddr);
	}
	
	switch(asymmetricKeyDB.algorithmID)
	{
		case	conAsymmetricAlgorithmIDOfRSA:	// RSA算法

			// 检测是否存在索引
			if (asymmetricKeyDB.vkStoreLocation == 1)
				snprintf(vkIndex,sizeof(vkIndex),"%s",asymmetricKeyDB.vkIndex);	
			else
				snprintf(vkIndex,sizeof(vkIndex),"%s","99");

			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					if ((lenOfData = UnionHsmCmd37(dataFillMode[0],vkIndex,lenOfVkValue,asymmetricKeyDB.vkValue,lenOfData,data,sign,sizeof(sign))) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE170:: UnionHsmCmd37!\n");
						return(lenOfData);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE110:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}
			bcdhex_to_aschex(sign,lenOfData,tmpData);
			tmpData[lenOfData*2] = 0;
			break;
		case	conAsymmetricAlgorithmIDOfSM2:	// SM2算法

			// 检测数据
			// modify by leipp 20151224
			if (strcmp(hashID,"03") != 0)
			{
				if (lenOfData != 64)
				{
					UnionUserErrLog("in UnionDealServiceCodeE170:: data[%s][%d] != 64!\n",data,lenOfData);
					UnionSetResponseRemark("数据长度[%d] != 64",lenOfData);
					return(errCodeParameter);
				}
			}
			else if ((lenOfData % 2 != 0) || (!UnionIsBCDStr(data)))
			{
				UnionUserErrLog("in UnionDealServiceCodeE170:: data[%s] is invalid!\n",data);
				UnionSetResponseRemark("数据不符合十六进制格式");
				return(errCodeParameter);
			}
			// modify end 20151224

			// 检测是否存在索引
			// modify by lisq 20141210 
			//if (asymmetricKeyDB.vkStoreLocation == 1)
			if (asymmetricKeyDB.vkStoreLocation != 0)
				snprintf(vkIndex,sizeof(vkIndex),"%s",asymmetricKeyDB.vkIndex);	
			else
				snprintf(vkIndex,sizeof(vkIndex),"%s","-1");	

			if (strcmp(hashID,"03") != 0)
				snprintf(hashID,sizeof(hashID),"%s","01");
			else
				snprintf(hashID,sizeof(hashID),"%s","02");

			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					if ((lenOfData = UnionHsmCmdK3(hashID,strlen(userID),userID,lenOfData,data,atoi(vkIndex),lenOfVkValue,asymmetricKeyDB.vkValue,tmpData,sizeof(tmpData))) < 0)	
					{
						UnionUserErrLog("in UnionDealServiceCodeE170:: UnionHsmCmdK3!\n");
						return(lenOfData);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE110:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE170:: asymmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	
	// 设置响应签名
	if ((ret = UnionSetResponseXMLPackageValue("body/sign",tmpData)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE170:: UnionSetResponseXMLPackageValue[%s]!\n","body/sign");
		return(ret);
	}
	
	return(0);
}
