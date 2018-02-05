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
服务代码:       8E05
服务名:	 导入对称密钥
功能描述:       导入对称密钥
***************************************/
int UnionDealServiceCode8E05(PUnionHsmGroupRec phsmGroupRec)
{
	int			    ret;
	char			    keyValue[64];
	char			    keyByLMK[64];
	char			    keyByRKM[64];
	char			    zmk[64];
	char			    localCheckValue[32];
	char			    checkValue[32];
	char			    protectKey[64];
	char			    protectFlag[32];
	char			    remoteCheckValue[32];
	int			    isRemoteDistributeKey = 0;
	int			    len = 0;

	TUnionSymmetricKeyDB	    symmetricKeyDB;
	PUnionSymmetricKeyValue	 psymmetricKeyValue = NULL;
	TUnionSymmetricKeyDB	    zmkKeyDB;
	PUnionSymmetricKeyValue	 pzmkKeyValue = NULL;
	TUnionRemoteKeyPlatform	 tkeyDistributePlatform;

	memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));

	// 读取密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",symmetricKeyDB.keyName,sizeof(symmetricKeyDB.keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E05:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(symmetricKeyDB.keyName);

	// 读取对称密钥
	if ((ret = UnionReadSymmetricKeyDBRec(symmetricKeyDB.keyName,0,&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E05:: UnionReadSymmetricKeyDBRec[%s]!\n",symmetricKeyDB.keyName);
		return(ret);
	}
	//add by zhouxw 20150907
	//设置使用密码机组
	UnionSetHsmGroupIDForHsmSvr(symmetricKeyDB.keyGroup);
	//add end
	//add by zhouxw 20151013
        if ((ret = UnionGetHsmGroupRecByHsmGroupID(symmetricKeyDB.keyGroup,phsmGroupRec)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E05:: UnionGetHsmGroupRecByHsmGroupID 密码机组[%s]不存在!\n",symmetricKeyDB.keyGroup);
                return(ret);
        }
        //add end
	// 读取密钥值
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) != NULL)
	{
		memset(psymmetricKeyValue->oldKeyValue,0,sizeof(psymmetricKeyValue->oldKeyValue));
		strcpy(psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue);

		memset(symmetricKeyDB.oldCheckValue,0,sizeof(symmetricKeyDB.oldCheckValue));
		strcpy(symmetricKeyDB.oldCheckValue,symmetricKeyDB.checkValue);
		
	}
	else
		psymmetricKeyValue = &symmetricKeyDB.keyValue[0];

	strcpy(psymmetricKeyValue->lmkProtectMode,phsmGroupRec->lmkProtectMode);
	
	// 检验是否允许导入
	if (symmetricKeyDB.inputFlag != 1)
	{
		UnionUserErrLog("in UnionDealServiceCode8E05:: inputFlag = [%d]!\n",symmetricKeyDB.inputFlag);
		UnionSetResponseRemark("密钥不允许导入");
		return(errCodeEsscMDLKeyOperationNotPermitted);
	}

	// 检测是否远程分发
	if (strlen(symmetricKeyDB.keyDistributePlatform) > 0)
	{
		memset(&tkeyDistributePlatform,0,sizeof(tkeyDistributePlatform));
		if ((ret =  UnionReadRemoteKeyPlatformRec(symmetricKeyDB.keyDistributePlatform,conRemoteKeyPlatformKeyDirectionOfDistribute,&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E05:: UnionReadRemoteKeyPlatformRec keyDistributePlatform[%s]!\n",symmetricKeyDB.keyDistributePlatform);
			return(ret);
		}
		isRemoteDistributeKey = 1;
	}

	// 读取密钥值
	memset(keyValue,0,sizeof(keyValue));
	if ((ret = UnionReadRequestXMLPackageValue("body/keyValue",keyValue,sizeof(keyValue))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E05:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyValue");
		return(ret);
	}
	
	// 检查密钥的长度和定义是否一致
	if ((strlen(keyValue) * 4) != symmetricKeyDB.keyLen)
	{
		UnionUserErrLog("in UnionDealServiceCode8E05:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyValue");
		UnionSetResponseRemark("密钥长度非法，密钥长度必须是[%dbit]",symmetricKeyDB.keyLen);
		return(errCodeParameter);
	}

	// 读取校验值
	memset(checkValue,0,sizeof(checkValue));
	if ((ret = UnionReadRequestXMLPackageValue("body/checkValue",checkValue,sizeof(checkValue))) > 0)
	{
		UnionFilterHeadAndTailBlank(checkValue);
		if ((!UnionIsBCDStr(checkValue)) || (strlen(checkValue) < 4))
		{
			UnionUserErrLog("in UnionDealServiceCode8E05:: checkValue[%s] is error!\n",checkValue);
			UnionSetResponseRemark("读取检验值非法,必须为十六进制数,且长度必须大于4");
			return(errCodeParameter);	
		}
	}

	// 读取保护方式
	memset(protectFlag,0,sizeof(protectFlag));
	if ((ret = UnionReadRequestXMLPackageValue("body/protectFlag",protectFlag,sizeof(protectFlag))) < 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode8E05:: UnionReadRequestXMLPackageValue[%s]!\n","body/protectFlag");
		//return(ret);
	}

	// 读取保护密钥
	memset(protectKey,0,sizeof(protectKey));
	if ((protectFlag[0] == '1') || (protectFlag[0] == '2'))
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/protectKey",protectKey,sizeof(protectKey))) <= 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E05:: UnionReadRequestXMLPackageValue[%s]!\n","body/protectKey");
			return(ret);
		}
	}

	memset(zmk,0,sizeof(zmk));
	if (protectFlag[0] == '1')      // 指定密钥名称
	{
		memset(&zmkKeyDB,0,sizeof(zmkKeyDB));
		if ((ret = UnionReadSymmetricKeyDBRec(protectKey,1,&zmkKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E05:: UnionReadSymmetricKeyDBRec protectKey[%s]!\n",protectKey);
			return(ret);
		}
		if (zmkKeyDB.keyType != conZMK)
		{
			UnionUserErrLog("in UnionDealServiceCode8E05::  protectKey is not zmk [%s]!\n",protectKey);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}
		if ((pzmkKeyValue = UnionGetSymmetricKeyValue(&zmkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCode8E05:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("读取密钥值失败");
			return(errCodeParameter);
		}
		strcpy(zmk,pzmkKeyValue->keyValue);
	}
	else if (protectFlag[0] == '2') // 外带ZMK密钥保护
	{
		len = strlen(protectKey);
		if ((len != 16) && (len != 32) && (len != 48))
		{
			UnionUserErrLog("in UnionDealServiceCode8E05:: protectKey[%s] len != [16,32,48]!\n",protectKey);
			return(errCodeEssc_KeyLength);
		}
		strcpy(zmk,protectKey);
	}

	memset(keyByLMK,0,sizeof(keyByLMK));
	memset(localCheckValue,0,sizeof(localCheckValue));
	if (protectFlag[0] == '1' || protectFlag[0] == '2')
	{
		switch(symmetricKeyDB.algorithmID)
		{
			case    conSymmetricAlgorithmIDOfDES: 
				// DES算法
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,zmk,keyValue,keyByLMK,localCheckValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode8E05:: UnionHsmCmdA6!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E05:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			case    conSymmetricAlgorithmIDOfSM4: 
				// SM4算法
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,zmk,keyValue,keyByLMK,localCheckValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode8E05:: UnionHsmCmdSV!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E05:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCode8E05:: key algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
		strcpy(keyValue,keyByLMK);
	}
	else
	{
		// 计算密钥校验值
		switch(symmetricKeyDB.algorithmID)
		{
			case    conSymmetricAlgorithmIDOfDES: // DES算法
				UnionSetIsUseNormalZmkType();
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdBU(0,symmetricKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen),keyValue,localCheckValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode8E05:: UnionHsmCmdBU!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E05:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			case    conSymmetricAlgorithmIDOfSM4: // SM4算法
				UnionSetIsUseNormalZmkType();
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdBU(1,symmetricKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen),keyValue,localCheckValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode8E05:: UnionHsmCmdBU!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E05:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCode8E05:: key algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
	}

	// 检查校验值
	if (memcmp(localCheckValue,checkValue,strlen(checkValue)) != 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E05:: localCheckValue[%s] != checkValue[%s]!\n",localCheckValue,checkValue);
		return(errCodeEssc_CheckValue);
	}

	// 导入新密钥

	memset(psymmetricKeyValue->keyValue,0,sizeof(psymmetricKeyValue->keyValue));
	strcpy(psymmetricKeyValue->keyValue,keyValue);

	memset(symmetricKeyDB.checkValue,0,sizeof(symmetricKeyDB.checkValue));
	if (strlen(checkValue) != 16)
		strcpy(symmetricKeyDB.checkValue,localCheckValue);
	else
		strcpy(symmetricKeyDB.checkValue,checkValue);

	// 远程分发密钥
	if (isRemoteDistributeKey)
	{
		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E05:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// 修改对应服务码 
		if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","E112")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E07:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","E112");
			return(ret);
		}

		memset(keyByRKM,0,sizeof(keyByRKM));
		memset(remoteCheckValue,0,sizeof(remoteCheckValue));
		switch(symmetricKeyDB.algorithmID)
		{
			case    conSymmetricAlgorithmIDOfDES:
				// 把本地LMK对加密转成远程ZMK加密
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,keyValue,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode8E05:: Distribute Key UnionHsmCmdA8!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E05:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				// 检查校验值
				if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,16) != 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E05:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
					return(errCodeEssc_CheckValue);
				}
				break;
			case    conSymmetricAlgorithmIDOfSM4:   // SM4算法
				// 把本地LMK对加密转成远程ZMK加密
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,keyValue,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode8E05:: Distribute Key UnionHsmCmdST!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E05:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				// 检查校验值
				if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,16) != 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E05:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
					return(errCodeEssc_CheckValue);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCode8E05:: Distribute Key algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}

		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/remoteKeyValue",keyByRKM)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E05:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyValue",keyByRKM);
			return(ret);
		}

		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/remoteCheckValue",remoteCheckValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E05:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/checkValue",remoteCheckValue);
			return(ret);
		}
		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E05:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	// 更新密钥
	if ((ret = UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E05:: UnionUpdateSymmetricKeyDBKeyValue!\n");
		return(ret);
	}
	return(0);
}


