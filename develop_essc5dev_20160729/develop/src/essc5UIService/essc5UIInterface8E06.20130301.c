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
服务代码:       8E06
服务名:         申请对称密钥
功能描述:       申请对称密钥
***************************************/
int UnionDealServiceCode8E06(PUnionHsmGroupRec phsmGroupRec)
{
        int                             ret;
        char                            keyName[128];
        int                             mode = 1;
        char                            tmpBuf[512];
	int				isUseOldKey = 1;
	int				isRemainOriOldKey = 0;
        char                            remoteCheckValue[32];
        char                            remoteKeyValue[64];
	char				protectKey[64];

        TUnionSymmetricKeyDB            symmetricKeyDB;
        PUnionSymmetricKeyValue         psymmetricKeyValue = NULL;
        TUnionRemoteKeyPlatform         tkeyApplyPlatform;

	// add by liwj 20150616
        TUnionSymmetricKeyDB            symmetricKeyDBZmk;
        PUnionSymmetricKeyValue         psymmetricKeyValueZmk = NULL;
	memset(&symmetricKeyDBZmk, 0, sizeof(symmetricKeyDBZmk));
	// end 

        memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));

        // 读取密钥名称
        memset(keyName,0,sizeof(keyName));
        if ((ret = UnionReadRequestXMLPackageValue("body/keyName",symmetricKeyDB.keyName,sizeof(symmetricKeyDB.keyName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E06:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
                return(ret);
        }

        UnionFilterHeadAndTailBlank(symmetricKeyDB.keyName);

        // 读取对称密钥
        if ((ret =  UnionReadSymmetricKeyDBRec(symmetricKeyDB.keyName,0,&symmetricKeyDB)) < 0)
        {
                if (ret == errCodeKeyCacheMDL_WrongKeyName)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E06:: 对称密钥[%s]不存在!\n",symmetricKeyDB.keyName);
                        UnionSetResponseRemark("对称密钥[%s]不存在",symmetricKeyDB.keyName);
                        return(errCodeKeyCacheMDL_KeyNonExists);
                }
                UnionUserErrLog("in UnionDealServiceCode8E06:: UnionReadSymmetricKeyDBRec[%s]!\n",symmetricKeyDB.keyName);
                return(ret);
        }
	//add by zhouxw 20150907
	UnionSetHsmGroupIDForHsmSvr(symmetricKeyDB.keyGroup);
	//add end
	//add by zhouxw 20151013
        if ((ret = UnionGetHsmGroupRecByHsmGroupID(symmetricKeyDB.keyGroup,phsmGroupRec)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E06:: UnionGetHsmGroupRecByHsmGroupID 密码机组[%s]不存在!\n",symmetricKeyDB.keyGroup);
                return(ret);
        }
        //add end
	// 读取密钥值
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) != NULL)
	{
		// 当前密钥置为旧密钥
		strcpy(psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue);
		strcpy(symmetricKeyDB.oldCheckValue,symmetricKeyDB.checkValue);
		memset(psymmetricKeyValue->keyValue,0,sizeof(psymmetricKeyValue->keyValue));
		memset(symmetricKeyDB.checkValue,0,sizeof(symmetricKeyDB.checkValue));
	}
	else
		psymmetricKeyValue = &symmetricKeyDB.keyValue[0];

        strcpy(psymmetricKeyValue->lmkProtectMode,phsmGroupRec->lmkProtectMode);


        // 模式
        // 1：申请当前密钥，默认值
        // 2：申请新密钥
        memset(tmpBuf,0,sizeof(tmpBuf));
        if ((ret = UnionReadRequestXMLPackageValue("body/modeFlag",tmpBuf,sizeof(tmpBuf))) <= 0)
                mode = 1;
        else
        {
                mode = atoi(tmpBuf);
                if ((mode != 1) && (mode != 2))
                {
                        UnionUserErrLog("in UnionDealServiceCode8E06:: mode[%s] error!\n",tmpBuf);
                        UnionSetResponseRemark("非法的模式[%s]",tmpBuf);
                        return(errCodeParameter);
                }
        }

        // 检测申请平台
        if (strlen(symmetricKeyDB.keyApplyPlatform) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E06:: keyName[%s] keyApplyPlatform can not be null!\n",symmetricKeyDB.keyName);
                UnionSetResponseRemark("密钥申请平台不能为空");
                return(errCodeParameter);
        }
        else
        {
                memset(&tkeyApplyPlatform,0,sizeof(tkeyApplyPlatform));
                if ((ret = UnionReadRemoteKeyPlatformRec(symmetricKeyDB.keyApplyPlatform,conRemoteKeyPlatformKeyDirectionOfApply,&tkeyApplyPlatform)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E06:: UnionReadRemoteKeyPlatformRec keyApplyPlatform[%s]!\n",symmetricKeyDB.keyApplyPlatform);
                        return(ret);
                }
        }

        // 初始化远程密钥操作请求报文
        if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E06:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
                return(ret);
        }

	if (tkeyApplyPlatform.packageType != 5)
	{
		if (mode == 1)
		{
			if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","291")) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E06:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
				return(ret);
			}
		}
		else
		{
			if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","292")) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E06:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
				return(ret);
			}
		}
	}
	else
	{
		// 修改对应服务码 
		if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","E114")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E06:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","E114");
			return(ret);
		}

		// 申请模式
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",mode);
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/mode",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E06:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/mode",tmpBuf);
			return(ret);
		}
	}

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
				UnionUserErrLog("in UnionDealServiceCode8E06:: 对称密钥[%s]不存在!\n",symmetricKeyDBZmk.keyName);
				UnionSetResponseRemark("对称密钥[%s]不存在",symmetricKeyDBZmk.keyName);
				return(errCodeKeyCacheMDL_KeyNonExists);
			}
			UnionUserErrLog("in UnionDealServiceCode8E06:: UnionReadSymmetricKeyDBRec[%s]!\n",symmetricKeyDBZmk.keyName);
			return(ret);
		}

		// 读取密钥值
		if ((psymmetricKeyValueZmk = UnionGetSymmetricKeyValue(&symmetricKeyDBZmk,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCode8E06:: UnionGetSymmetricKeyValue [%s] key value is null!\n",symmetricKeyDBZmk.keyName);
			return(errCodeDesKeyDBMDL_KeyNotEffective);
		}
	}
	// end

        // 转发密钥操作到远程平台
        if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E06:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
                return(ret);
        }

        // 读取远程密钥值
        memset(remoteKeyValue,0,sizeof(remoteKeyValue));
        if((ret = UnionReadResponseRemoteXMLPackageValue("body/keyValue",remoteKeyValue,sizeof(remoteKeyValue))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E06:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/keyValue");
                return(ret);
        }

        // 读取远程密钥校验值
        memset(remoteCheckValue,0,sizeof(remoteCheckValue));
        if((ret = UnionReadResponseRemoteXMLPackageValue("body/checkValue",remoteCheckValue,sizeof(remoteCheckValue))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E06:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/checkValue");
                return(ret);
        }

	if (tkeyApplyPlatform.packageType != 5)
		snprintf(protectKey,sizeof(protectKey),"%s",psymmetricKeyValueZmk->keyValue);
	else
		snprintf(protectKey,sizeof(protectKey),"%s",tkeyApplyPlatform.protectKey);

useOldKey:
        switch(symmetricKeyDB.algorithmID)
        {
                case    conSymmetricAlgorithmIDOfDES:   // DES算法
                        // 密钥转成本地LMK对加密
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,protectKey,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode8E06:: UnionHsmCmdA6!\n");
						return(ret);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCode8E06:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}
                        break;
                case    conSymmetricAlgorithmIDOfSM4:   // SM4算法
                        // 密钥转成本地LMK对加密
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,protectKey,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode8E06:: UnionHsmCmdSV!\n");
						return(ret);
					}
					// end
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCode8E06:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}
                        break;
                default:
                        UnionUserErrLog("in UnionDealServiceCode8E06:: symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
                        return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	// modify by leipp 2015_07_08
	// 检查校验值
	if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
	{
		if (tkeyApplyPlatform.packageType != 5 && isUseOldKey)
		{
			snprintf(protectKey,sizeof(protectKey),"%s",psymmetricKeyValueZmk->oldKeyValue);
			UnionLog("in UnionDealServiceCode8E06:: use old protectkey\n");
			isUseOldKey = 0;
			isRemainOriOldKey = 1;
			goto useOldKey;
		}
		else if (!isUseOldKey)
		{
			UnionUserErrLog("in UnionDealServiceCode8E06:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s] old key\n",symmetricKeyDB.checkValue,remoteCheckValue);
			return(errCodeEssc_CheckValue);
		}
		else
		{
			UnionUserErrLog("in UnionDealServiceCode8E06:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s] current key\n",symmetricKeyDB.checkValue,remoteCheckValue);
			return(errCodeEssc_CheckValue);
		}
	}
	else
	{	
		strcpy(symmetricKeyDB.checkValue, remoteCheckValue);
	}
	// modify by leipp end

        if (strcmp(psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue) == 0)
        {
		UnionLog("in UnionDealServiceCode8E06:: remote KeyValue[%s] == current keyValue[%s]\n",psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue);
		return(0);
        }

	// modify by leipp 2015_07_08
	if (isRemainOriOldKey)
	{
		if (symmetricKeyDB.keyType == conZMK)
		{
			snprintf(psymmetricKeyValue->oldKeyValue,sizeof(psymmetricKeyValue->oldKeyValue),"%s",psymmetricKeyValueZmk->oldKeyValue);
			snprintf(symmetricKeyDB.oldCheckValue,sizeof(symmetricKeyDB.oldCheckValue),"%s",symmetricKeyDBZmk.oldCheckValue);
		}
	}
	//modify by leipp end

	if ((ret =  UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E06:: UnionUpdateSymmetricKeyDBKeyValue,keyName[%s]!\n",symmetricKeyDB.keyName);
		return(ret);
	}

        return(0);
}
