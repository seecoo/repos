//      Author:	 张永定
//      Copyright:      Union Tech. Guangzhou
//      Date:	   2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5UIService.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "unionHsmCmdVersion.h"

/***************************************
服务代码:       8E03
服务名:	 更新对称密钥
功能描述:       更新对称密钥
***************************************/
int UnionDealServiceCode8E03(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	char				keyByRKM[64];
	int				isRemoteApplyKey = 0;
	int				isRemoteDistributeKey = 0;
	char				remoteKeyValue[64];
	char				remoteCheckValue[32];
	char				keyByZMK[64];

	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;
	TUnionRemoteKeyPlatform		tkeyApplyPlatform;
	TUnionRemoteKeyPlatform		tkeyDistributePlatform;

	// add by liwj 20150616
        TUnionSymmetricKeyDB            symmetricKeyDBZmk;
        PUnionSymmetricKeyValue         psymmetricKeyValueZmk = NULL;
	memset(&symmetricKeyDBZmk, 0, sizeof(symmetricKeyDBZmk));
	// end 

	memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));

	// 密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",symmetricKeyDB.keyName,sizeof(symmetricKeyDB.keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E03:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(symmetricKeyDB.keyName);
	if (strlen(symmetricKeyDB.keyName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E03:: keyName can not be null!\n");
		UnionSetResponseRemark("密钥名称不能为空!");
		return(errCodeParameter);
	}

	// 读取对称密钥
	if ((ret = UnionReadSymmetricKeyDBRec(symmetricKeyDB.keyName,0,&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E03:: UnionReadSymmetricKeyDBRec[%s]!\n",symmetricKeyDB.keyName);
		return(ret);
	}
	
	//add by zhouxw 20150827
	UnionSetHsmGroupIDForHsmSvr(symmetricKeyDB.keyGroup);
	//add end
	//add by zhouxw 20151013
	if ((ret = UnionGetHsmGroupRecByHsmGroupID(symmetricKeyDB.keyGroup,phsmGroupRec)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E03:: UnionGetHsmGroupRecByHsmGroupID 密码机组[%s]不存在!\n",symmetricKeyDB.keyGroup);
		return(ret);	
        }
	//add end

	// 读取密钥值
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) != NULL)
	{
		memset(psymmetricKeyValue->oldKeyValue,0,sizeof(psymmetricKeyValue->oldKeyValue));
		memset(symmetricKeyDB.oldCheckValue,0,sizeof(symmetricKeyDB.oldCheckValue));

		strcpy(psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue);
		strcpy(symmetricKeyDB.oldCheckValue,symmetricKeyDB.checkValue);

		memset(psymmetricKeyValue->keyValue,0,sizeof(psymmetricKeyValue->keyValue));
		memset(symmetricKeyDB.checkValue,0,sizeof(symmetricKeyDB.checkValue));

	}
	else
		psymmetricKeyValue = &symmetricKeyDB.keyValue[0];

	strcpy(psymmetricKeyValue->lmkProtectMode,phsmGroupRec->lmkProtectMode);


	// 远程申请
	if (strlen(symmetricKeyDB.keyApplyPlatform) > 0)
	{
		memset(&tkeyApplyPlatform,0,sizeof(tkeyApplyPlatform));
		if ((ret =  UnionReadRemoteKeyPlatformRec(symmetricKeyDB.keyApplyPlatform,conRemoteKeyPlatformKeyDirectionOfApply,&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E03:: UnionReadRemoteKeyPlatformRec keyApplyPlatform[%s]!\n",symmetricKeyDB.keyApplyPlatform);
			return(ret);
		}
		isRemoteApplyKey = 1;
	}

	// 远程分发
	if (strlen(symmetricKeyDB.keyDistributePlatform) > 0)
	{
		memset(&tkeyDistributePlatform,0,sizeof(tkeyDistributePlatform));
		if ((ret =  UnionReadRemoteKeyPlatformRec(symmetricKeyDB.keyDistributePlatform,conRemoteKeyPlatformKeyDirectionOfDistribute,&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E03:: UnionReadRemoteKeyPlatformRec keyDistributePlatform[%s]!\n",symmetricKeyDB.keyDistributePlatform);
			return(ret);
		}
		isRemoteDistributeKey = 1;

		// 检测是否允许导出
		/*
		if (symmetricKeyDB.outputFlag == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E03:: keyName is not permit output!\n");
			UnionSetResponseRemark("密钥不允许导出!");
			return(errCodeParameter);
		}
		*/
	}

	// 检测分发和申请是否同一平台
	if (isRemoteApplyKey && isRemoteDistributeKey)
	{
		if (strcmp(tkeyApplyPlatform.ipAddr,tkeyDistributePlatform.ipAddr) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E03:: tkeyApplyPlatform.ipAddr[%s] == tkeyDistributePlatform.ipAddr[%s] is error!\n",tkeyApplyPlatform.ipAddr,tkeyDistributePlatform.ipAddr);
			UnionSetResponseRemark("申请和分发平台IP地址不能相同");
			return(errCodeParameter);
		}
	}

	// 申请远程密钥
	if (isRemoteApplyKey)
	{
		// add by liwj 20150616
		if (tkeyApplyPlatform.packageType != 5)
		{
			// 获取 zmk 密钥名称
			snprintf(symmetricKeyDBZmk.keyName, sizeof(symmetricKeyDBZmk.keyName), "%s", symmetricKeyDB.keyName);
			memcpy(symmetricKeyDBZmk.keyName + strlen(symmetricKeyDBZmk.keyName) - 3, "zmk", 3);

			// 读取对称密钥
			if ((ret =  UnionReadSymmetricKeyDBRec(symmetricKeyDBZmk.keyName,0,&symmetricKeyDBZmk)) < 0)
			{
				if (ret == errCodeKeyCacheMDL_WrongKeyName)
				{
					UnionUserErrLog("in UnionDealServiceCode8E03:: 对称密钥[%s]不存在!\n",symmetricKeyDBZmk.keyName);
					UnionSetResponseRemark("对称密钥[%s]不存在",symmetricKeyDBZmk.keyName);
					return(errCodeKeyCacheMDL_KeyNonExists);
				}
				UnionUserErrLog("in UnionDealServiceCode8E03:: UnionReadSymmetricKeyDBRec[%s]!\n",symmetricKeyDBZmk.keyName);
				return(ret);
			}

			// 读取密钥值
			if ((psymmetricKeyValueZmk = UnionGetSymmetricKeyValue(&symmetricKeyDBZmk,phsmGroupRec->lmkProtectMode)) == NULL)
			{
				UnionUserErrLog("in UnionDealServiceCode8E03:: UnionGetSymmetricKeyValue [%s] key value is null!\n",symmetricKeyDBZmk.keyName);
				return(errCodeDesKeyDBMDL_KeyNotEffective);
			}
		}
		// end

		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E03:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// 修改对应服务码 
		// modify by lisq 20150104
		//if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","E111")) < 0)
		if (tkeyApplyPlatform.packageType == 3)
		{
			if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","292")) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E03:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","292");
				return(ret);
			}
		}
		else if (tkeyApplyPlatform.packageType == 5)
		{
			if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","E111")) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E03:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","E111");
				return(ret);
			}
		}
		else
		{
			UnionUserErrLog("in UnionDealServiceCode8E03::packageType[%d] is invalid!\n",tkeyDistributePlatform.packageType);
			return(errCodeParameter);
		}
		//modify by lisq 20150104 end

		// 设置申请标志 1:申请
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/remoteOperateFlag","1")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E03:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/remoteOperateFlag","1");
			return(ret);
		}

		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E03:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	if (isRemoteApplyKey)
	{
		// 读取远程密钥值
		memset(remoteKeyValue,0,sizeof(remoteKeyValue));
		if((ret = UnionReadResponseRemoteXMLPackageValue("body/keyValue",remoteKeyValue,sizeof(remoteKeyValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E03:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/keyValue");
			return(ret);
		}

		// 读取远程密钥校验值
		memset(remoteCheckValue,0,sizeof(remoteCheckValue));
		if((ret = UnionReadResponseRemoteXMLPackageValue("body/checkValue",remoteCheckValue,sizeof(remoteCheckValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E03:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/checkValue");
			return(ret);
		}

		switch(symmetricKeyDB.algorithmID)
		{
			case    conSymmetricAlgorithmIDOfDES:   // DES算法
				// 密钥转成本地LMK对加密
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						//modify by liwj 20150616
						if (tkeyApplyPlatform.packageType != 5)
						{
							if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,psymmetricKeyValueZmk->keyValue,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E03:: UnionHsmCmdA6!\n");
								return(ret);
							}
						}
						else
						{
							if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,tkeyApplyPlatform.protectKey,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E03:: UnionHsmCmdA6!\n");
								return(ret);
							}
						}
						// end
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E03:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}

				// 检查校验值
				if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,16) != 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E03:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
					return(errCodeEssc_CheckValue);
				}
				break;
			case    conSymmetricAlgorithmIDOfSM4:   // SM4算法
				// 密钥转成本地LMK对加密
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						//modify by liwj 20150616
						if (tkeyApplyPlatform.packageType != 5)
						{
							if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,psymmetricKeyValueZmk->keyValue,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E03:: UnionHsmCmdSV!\n");
								return(ret);
							}
						}
						else
						{
							if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,tkeyApplyPlatform.protectKey,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E03:: UnionHsmCmdSV!\n");
								return(ret);
							}
						}
						// end
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E03:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
								
				// 检查校验值
				if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,16) != 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E03:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
					return(errCodeEssc_CheckValue);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCode8E03:: symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
	}
	else
	{
		switch(symmetricKeyDB.algorithmID)
		{
			case    conSymmetricAlgorithmIDOfDES:   // DES算法
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdA0(0,symmetricKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen),NULL,psymmetricKeyValue->keyValue,keyByZMK,symmetricKeyDB.checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode8E03:: UnionHsmCmdA0!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E03:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			case    conSymmetricAlgorithmIDOfSM4:   // SM4算法
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdWI("0",symmetricKeyDB.keyType,NULL,psymmetricKeyValue->keyValue,keyByZMK,symmetricKeyDB.checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode8E03:: UnionHsmCmdWI!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E03:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCode8E03:: symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
	}

	// 分发远程密钥
	if (isRemoteDistributeKey)
	{
		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E03:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// 修改对应服务码 
		// modify by lisq 20150104
		//if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","E111")) < 0)
		if (tkeyDistributePlatform.packageType == 3)
		{
			if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","294")) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E03:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","294");
				return(ret);
			}
		}
		else if (tkeyDistributePlatform.packageType == 5)
		{
			if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","E111")) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E03:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","E111");
				return(ret);
			}
		}
		else
		{
			UnionUserErrLog("in UnionDealServiceCode8E03::packageType[%d] is invalid!\n",tkeyDistributePlatform.packageType);
			return(errCodeParameter);
		}
		// modify by lisq 20150104 end

		// 设置申请标志 2:分发
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/remoteOperateFlag","2")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E03:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/remoteOperateFlag","2");
			return(ret);
		}

		// 增加密钥和校验值
		memset(keyByRKM,0,sizeof(keyByRKM));
		memset(remoteCheckValue,0,sizeof(remoteCheckValue));
		switch(symmetricKeyDB.algorithmID)
		{
			case    conSymmetricAlgorithmIDOfDES:   // DES算法
				// 把本地LMK对加密转成远程ZMK加密
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode8E03:: Distribute Key UnionHsmCmdA8!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E03:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				// 检查校验值
				if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,16) != 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E03:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
					return(errCodeEssc_CheckValue);
				}
				break;
			case    conSymmetricAlgorithmIDOfSM4:   // SM4算法
				// 把本地LMK对加密转成远程ZMK加密
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode8E03:: Distribute Key UnionHsmCmdST!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E03:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				// 检查校验值
				if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,16) != 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E03:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
					return(errCodeEssc_CheckValue);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCode8E03:: Distribute Key symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyValue",keyByRKM)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E03:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyValue",keyByRKM);
			return(ret);
		}
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/checkValue",remoteCheckValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E03:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/checkValue",remoteCheckValue);
			return(ret);
		}

		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E03:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	// 更新对称密钥
	if ((ret = UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E03:: UnionUpdateSymmetricKeyDBKeyValue keyName[%s]!\n",symmetricKeyDB.keyName);
		return(ret);
	}

	return(0);
}


