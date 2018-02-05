//      Author:         张永定
//      Copyright:      Union Tech. Guangzhou
//      Date:           2013-01-10

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
服务代码:       8E07
服务名:         分发对称密钥
功能描述:       分发对称密钥
***************************************/
int UnionDealServiceCode8E07(PUnionHsmGroupRec phsmGroupRec)
{
        int                             ret;
        int                             mode = 1;
        char                            tmpBuf[512];
        char                            remoteCheckValue[32];
        char                            remoteKeyValue[64];
	char				keyByZMK[64];

        TUnionSymmetricKeyDB            symmetricKeyDB;
        PUnionSymmetricKeyValue         psymmetricKeyValue = NULL;
        TUnionRemoteKeyPlatform         tkeyDistributePlatform;

	// add by liwj 20150616
        TUnionSymmetricKeyDB            symmetricKeyDBZmk;
        PUnionSymmetricKeyValue         psymmetricKeyValueZmk = NULL;
	memset(&symmetricKeyDBZmk, 0, sizeof(symmetricKeyDBZmk));
	// end 

        memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));

        // 读取密钥名称
        if ((ret = UnionReadRequestXMLPackageValue("body/keyName",symmetricKeyDB.keyName,sizeof(symmetricKeyDB.keyName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E07:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
                return(ret);
        }

        UnionFilterHeadAndTailBlank(symmetricKeyDB.keyName);

        // 读取对称密钥
        if ((ret =  UnionReadSymmetricKeyDBRec(symmetricKeyDB.keyName,0,&symmetricKeyDB)) < 0)
        {
                if (ret == errCodeKeyCacheMDL_WrongKeyName)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E07:: 对称密钥[%s]不存在!\n",symmetricKeyDB.keyName);
                        UnionSetResponseRemark("对称密钥[%s]不存在",symmetricKeyDB.keyName);
                        return(errCodeKeyCacheMDL_KeyNonExists);
                }
                UnionUserErrLog("in UnionDealServiceCode8E07:: UnionReadSymmetricKeyDBRec[%s]!\n",symmetricKeyDB.keyName);
                return(ret);
        }
	//add by zhouxw
	UnionSetHsmGroupIDForHsmSvr(symmetricKeyDB.keyGroup);
	//add end
	//add by zhouxw 20151013
        if ((ret = UnionGetHsmGroupRecByHsmGroupID(symmetricKeyDB.keyGroup,phsmGroupRec)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E07:: UnionGetHsmGroupRecByHsmGroupID 密码机组[%s]不存在!\n",symmetricKeyDB.keyGroup);
                return(ret);
        }
        //add end
        // 模式
        // 1：分发当前密钥，默认值
        // 2：分发新密钥 
        memset(tmpBuf,0,sizeof(tmpBuf));
        if ((ret = UnionReadRequestXMLPackageValue("body/modeFlag",tmpBuf,sizeof(tmpBuf))) <= 0)
                mode = 1;
        else
        {
                mode = atoi(tmpBuf);
                if ((mode != 1) && (mode != 2))
                {
                        UnionUserErrLog("in UnionDealServiceCode8E07:: mode[%s] error!\n",tmpBuf);
                        UnionSetResponseRemark("非法的模式[%s]",tmpBuf);
                        return(errCodeParameter);
                }
        }

        // 读取密钥值
        if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
        {
		if (mode == 1)
		{
			UnionUserErrLog("in UnionDealServiceCode8E07:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("读取密钥值失败");
			return(errCodeParameter);
		}
		else
			psymmetricKeyValue = &symmetricKeyDB.keyValue[0];
        }

        strcpy(psymmetricKeyValue->lmkProtectMode,phsmGroupRec->lmkProtectMode);

        // 检测分发平台是否存在
        if (strlen(symmetricKeyDB.keyDistributePlatform) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E07:: keyDistributePlatform is null!\n");
                UnionSetResponseRemark("密钥分发平台不能为空");
                return(errCodeParameter);
        }

        // 获取分发平台信息
        memset(&tkeyDistributePlatform,0,sizeof(tkeyDistributePlatform));
        if ((ret =  UnionReadRemoteKeyPlatformRec(symmetricKeyDB.keyDistributePlatform,conRemoteKeyPlatformKeyDirectionOfDistribute,&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E07:: UnionReadRemoteKeyPlatformRec keyDistributePlatform[%s]!\n",symmetricKeyDB.keyDistributePlatform);
                return(ret);
        }

        // 初始化远程密钥操作请求报文
        if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E07:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
                return(ret);
        }

	if (tkeyDistributePlatform.packageType != 5)
	{
		if (mode == 1)
		{
			if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","293")) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E07:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
				return(ret);
			}
		}
		else
		{
			if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","294")) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E07:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
				return(ret);
			}
		}
	}
	else
	{
		// 修改对应服务码 
		if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","E115")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E07:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","E115");
			return(ret);
		}

		// 分发模式
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",mode);
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/mode",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E07:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/mode",tmpBuf);
			return(ret);
		}
	}

	// add by liwj 20150616
	if (tkeyDistributePlatform.packageType != 5)
	{
		// 获取 zmk 密钥名称
		snprintf(symmetricKeyDBZmk.keyName, sizeof(symmetricKeyDBZmk.keyName), "%s", symmetricKeyDB.keyName);
		memcpy(symmetricKeyDBZmk.keyName + strlen(symmetricKeyDBZmk.keyName) - 3, "zmk", 3);

		// 读取对称密钥
		if ((ret =  UnionReadSymmetricKeyDBRec(symmetricKeyDBZmk.keyName,0,&symmetricKeyDBZmk)) < 0)
		{
			if (ret == errCodeKeyCacheMDL_WrongKeyName)
			{
				UnionUserErrLog("in UnionDealServiceCode8E07:: 对称密钥[%s]不存在!\n",symmetricKeyDBZmk.keyName);
				UnionSetResponseRemark("对称密钥[%s]不存在",symmetricKeyDBZmk.keyName);
				return(errCodeKeyCacheMDL_KeyNonExists);
			}
			UnionUserErrLog("in UnionDealServiceCode8E07:: UnionReadSymmetricKeyDBRec[%s]!\n",symmetricKeyDBZmk.keyName);
			return(ret);
		}

		// 读取密钥值
		if ((psymmetricKeyValueZmk = UnionGetSymmetricKeyValue(&symmetricKeyDBZmk,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCode8E07:: UnionGetSymmetricKeyValue [%s] key value is null!\n",symmetricKeyDBZmk.keyName);
			return(errCodeDesKeyDBMDL_KeyNotEffective);
		}
	}
	// end

        memset(remoteKeyValue,0,sizeof(remoteKeyValue));
        memset(remoteCheckValue,0,sizeof(remoteCheckValue));
        if (mode == 1)
        {
                switch(symmetricKeyDB.algorithmID)
                {
                        case    conSymmetricAlgorithmIDOfDES:   // DES算法
                                // 把本地LMK对加密转成远程ZMK加密
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						//modify by liwj 20150616
						if (tkeyDistributePlatform.packageType != 5)
						{
							if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,psymmetricKeyValueZmk->keyValue,remoteKeyValue,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key UnionHsmCmdA8!\n");
								return(ret);
							}
						}
						else
						{
							if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,tkeyDistributePlatform.protectKey,remoteKeyValue,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key UnionHsmCmdA8!\n");
								return(ret);
							}
						}
						// end
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E07:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				strcpy(remoteCheckValue, symmetricKeyDB.checkValue);
                                break;
                        case    conSymmetricAlgorithmIDOfSM4:   // SM4算法
                                // 把本地LMK对加密转成远程ZMK加密
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						//modify by liwj 20150616
						if (tkeyDistributePlatform.packageType != 5)
						{
							if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,psymmetricKeyValueZmk->keyValue,remoteKeyValue,remoteCheckValue)) < 0)
							{
									UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key UnionHsmCmdST!\n");
									return(ret);
							}
						}
						else
						{
							if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,tkeyDistributePlatform.protectKey,remoteKeyValue,remoteCheckValue)) < 0)
							{
									UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key UnionHsmCmdST!\n");
									return(ret);
							}
						}
						// end
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E07:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				strcpy(remoteCheckValue, symmetricKeyDB.checkValue);
                                break;
                        default:
                                UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key symmetricKeyDB.algorithmID[%d] error!\n",symmetricKeyDB.algorithmID);
                                return(errCodeEsscMDL_InvalidAlgorithmID);
                }
        }
        else
        {
                // 将当前密钥置为旧密钥
                memset(psymmetricKeyValue->oldKeyValue,0,sizeof(psymmetricKeyValue->oldKeyValue));
                memset(symmetricKeyDB.oldCheckValue,0,sizeof(symmetricKeyDB.oldCheckValue));
                strcpy(psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue);
                strcpy(symmetricKeyDB.oldCheckValue,symmetricKeyDB.checkValue);

                memset(psymmetricKeyValue->keyValue,0,sizeof(psymmetricKeyValue->keyValue));
                memset(symmetricKeyDB.checkValue,0,sizeof(symmetricKeyDB.checkValue));

                switch(symmetricKeyDB.algorithmID)
                {
                        case    conSymmetricAlgorithmIDOfDES:   // DES算法
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret =  UnionHsmCmdA0(0,symmetricKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen),NULL,psymmetricKeyValue->keyValue,keyByZMK,symmetricKeyDB.checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode8E07:: UnionHsmCmdA0!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E07:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
								
                                // 把本地LMK对加密转成远程ZMK加密
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						//modify by liwj 20150616
						if (tkeyDistributePlatform.packageType != 5)
						{
							if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,psymmetricKeyValueZmk->keyValue,remoteKeyValue,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key UnionHsmCmdA8!\n");
								return(ret);
							}
						}
						else
						{
							if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,tkeyDistributePlatform.protectKey,remoteKeyValue,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key UnionHsmCmdA8!\n");
								return(ret);
							}
						}
						// end
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E07:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
                                // 检查校验值
                                if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,16) != 0)
                                {
                                        UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
                                        return(errCodeEssc_CheckValue);
                                }
                                break;
                        case    conSymmetricAlgorithmIDOfSM4:   // SM4算法
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdWI("0",symmetricKeyDB.keyType,NULL,psymmetricKeyValue->keyValue,keyByZMK,symmetricKeyDB.checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode8E07:: UnionHsmCmdWI!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E07:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
                                // 把本地LMK对加密转成远程ZMK加密
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						//modify by liwj 20150616
						if (tkeyDistributePlatform.packageType != 5)
						{
							if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,psymmetricKeyValueZmk->keyValue,remoteKeyValue,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key UnionHsmCmdST!\n");
								return(ret);
							}
						}
						else
						{
							if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,tkeyDistributePlatform.protectKey,remoteKeyValue,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key UnionHsmCmdST!\n");
								return(ret);
							}
						}
						// end
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E07:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
                                // 检查校验值
                                if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,16) != 0)
                                {
                                        UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
                                        return(errCodeEssc_CheckValue);
                                }
                                break;
                        default:
                                UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
                                return(errCodeEsscMDL_InvalidAlgorithmID);
                }
        }
        if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyValue",remoteKeyValue)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E07:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyValue",remoteKeyValue);
                return(ret);
        }
        if ((ret = UnionSetRequestRemoteXMLPackageValue("body/checkValue",remoteCheckValue)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E07:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/checkValue",remoteCheckValue);
                return(ret);
        }

        // 转发密钥操作到远程平台
        if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E07:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
                return(ret);
        }

        if (mode == 2)
        {
                if ((ret =  UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E07:: UnionUpdateSymmetricKeyDBKeyValue,keyName[%s]!\n",symmetricKeyDB.keyName);
                        return(ret);
                }
        }

        return(0);
}


