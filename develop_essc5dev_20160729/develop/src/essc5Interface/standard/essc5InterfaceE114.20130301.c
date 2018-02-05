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
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "unionHsmCmdVersion.h"

/***************************************
服务代码:	E114
服务名:		申请对称密钥
功能描述:	申请对称密钥
***************************************/
int UnionDealServiceCodeE114(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				mode = 1;
	char				tmpBuf[512];
	int				isRemoteKeyOperate = 0;
	char				platformProtectKey[64];
	char				keyByRKM[64];
	char				remoteCheckValue[32];
	char				remoteKeyValue[64];
	char				sql[128];
	char				keyName[160];
	int				isRemainOriOldKey = 0;
	int				isUseOldKey = 1;

	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;
	TUnionRemoteKeyPlatform		tkeyApplyPlatform;

	// add by liwj 20150616
        TUnionSymmetricKeyDB            symmetricKeyDBZmk;
        PUnionSymmetricKeyValue         psymmetricKeyValueZmk = NULL;
	memset(&symmetricKeyDBZmk, 0, sizeof(symmetricKeyDBZmk));
	// end 

	// 检测是否远程操作
	isRemoteKeyOperate = UnionIsRemoteKeyOperate();

	// 读取密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE114:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(keyName);

	// 读取对称密钥
	UnionInitSymmetricKeyDB(&symmetricKeyDB);
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
	{
		if (ret == errCodeKeyCacheMDL_WrongKeyName)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: 对称密钥[%s]不存在!\n",keyName);
			UnionSetResponseRemark("对称密钥[%s]不存在",keyName);
			return(errCodeKeyCacheMDL_KeyNonExists);
		}
		UnionUserErrLog("in UnionDealServiceCodeE114:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
		return(ret);
	}

	// 模式
	// 1：申请当前密钥，默认值
	// 2：申请新密钥
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) <= 0)
		mode = 1;
	else
	{
		tmpBuf[ret] = 0;
		mode = atoi(tmpBuf);	

		if ((mode != 1) && (mode != 2))
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: mode[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}

	// 读取密钥值
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		if (mode == 1 && isRemoteKeyOperate == 1)	//modify 20140709
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("读取密钥值失败");
			return(errCodeParameter);
		}
		else
			psymmetricKeyValue = &symmetricKeyDB.keyValue[0];

		strcpy(psymmetricKeyValue->lmkProtectMode,phsmGroupRec->lmkProtectMode);
	}
	
	if (!isRemoteKeyOperate) // 本地操作
	{
		// 当前密钥置为旧密钥
		strcpy(psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue);
		strcpy(symmetricKeyDB.oldCheckValue,symmetricKeyDB.checkValue);

		psymmetricKeyValue->keyValue[0] = 0;
		symmetricKeyDB.checkValue[0] = 0;;

		if (strlen(symmetricKeyDB.keyApplyPlatform) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: keyName[%s] keyApplyPlatform can not be null!\n",symmetricKeyDB.keyName);
			UnionSetResponseRemark("密钥申请平台不能为空");
			return(errCodeParameter);
		}
		else
		{
			// modify by leipp 20151209
			// 读取并检查申请和分发平台
			if ((ret = UnionCheckRemoteKeyPlatform(symmetricKeyDB.keyName, symmetricKeyDB.keyApplyPlatform, NULL, &tkeyApplyPlatform, NULL, &ret, NULL)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE114:: UnionCheckRemoteKeyPlatform!\n");
				return(ret);
			}
			// modify end
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
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		if (tkeyApplyPlatform.packageType != 5)
		{
			if (mode == 1)
			{
				if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","291")) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE114:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
					return(ret);
				}
			}
			else
			{
				if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","292")) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE114:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
					return(ret);
				}
			}
		}

		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	if (isRemoteKeyOperate) // 远程操作
	{
		// 系统ID
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);
		}
		tmpBuf[ret] = 0;

		//获取保护密钥
		snprintf(sql,sizeof(sql),"select protectKey from localKeyPlatform where sysID = '%s' and algorithmID = %d",tmpBuf,symmetricKeyDB.algorithmID);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionSelectRealDBRecord[%s]\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: protectKey not found!\n");
			UnionSetResponseRemark("远程平台保护密钥不存在");
			return(errCodeParameter);
		}

		if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionLocateXMLPackage[%s]!\n","detail");	
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("protectKey",platformProtectKey,sizeof(platformProtectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionReadXMLPackageValue[%s]!\n","protectKey");
			return(ret);
		}

		if (mode == 1) //申请当前密钥
		{
			switch(symmetricKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DES算法
					// 把本地LMK对加密转成远程ZMK加密
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,platformProtectKey,keyByRKM,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE114:: Distribute Key UnionHsmCmdA8!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE114: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
					// 把本地LMK对加密转成远程ZMK加密
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,platformProtectKey,keyByRKM,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE114:: Distribute Key UnionHsmCmdST!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE114: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE114:: Distribute Key symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}			
		}
		else // 申请新密钥
		{
			// 将当前密钥置为旧密钥
			strcpy(psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue);
			strcpy(symmetricKeyDB.oldCheckValue,symmetricKeyDB.checkValue);

			psymmetricKeyValue->keyValue[0] = 0;
			symmetricKeyDB.checkValue[0] = 0;

			switch(symmetricKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DES算法
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret =  UnionHsmCmdA0(0,symmetricKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen),NULL,psymmetricKeyValue->keyValue,NULL,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE114:: UnionHsmCmdA0!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE114: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}

					// 把本地LMK对加密转成远程ZMK加密
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,platformProtectKey,keyByRKM,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE114:: Distribute Key UnionHsmCmdA8!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE114: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					// 检查校验值
					if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE114:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
						return(errCodeEssc_CheckValue);
					}					
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdWI("0",symmetricKeyDB.keyType,NULL,psymmetricKeyValue->keyValue,NULL,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE114:: UnionHsmCmdWI!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE114: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					// 把本地LMK对加密转成远程ZMK加密
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,platformProtectKey,keyByRKM,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE114:: Distribute Key UnionHsmCmdST!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE114: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					// 检查校验值
					if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE114:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
						return(errCodeEssc_CheckValue);
					}					
					break;
				default:
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
		}

		if ((ret = UnionSetResponseXMLPackageValue("body/keyValue",keyByRKM)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionSetResponseXMLPackageValue keyValue[%s]!\n",keyByRKM);
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",symmetricKeyDB.checkValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionSetResponseXMLPackageValue checkValue[%s]!\n",symmetricKeyDB.checkValue);
			return(ret);
		}

		if (mode == 2)
		{
			if ((ret =  UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE114:: UnionUpdateSymmetricKeyDBKeyValue,keyName[%s]!\n",symmetricKeyDB.keyName);
				return(ret);
			}
		}
	}
	else	// 本地操作
	{
		// 读取远程密钥值
		if((ret = UnionReadResponseRemoteXMLPackageValue("body/keyValue",remoteKeyValue,sizeof(remoteKeyValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/keyValue");
			return(ret);
		}

		// 读取远程密钥校验值
		if((ret = UnionReadResponseRemoteXMLPackageValue("body/checkValue",remoteCheckValue,sizeof(remoteCheckValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/checkValue");
			return(ret);
		}

		// add by leipp 20150708
		if (tkeyApplyPlatform.packageType != 5)
			snprintf(platformProtectKey,sizeof(platformProtectKey),"%s",psymmetricKeyValueZmk->keyValue);
		else
			snprintf(platformProtectKey,sizeof(platformProtectKey),"%s",tkeyApplyPlatform.protectKey);
		// modify end
		
useOldKey:
		switch(symmetricKeyDB.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:	// DES算法
				// 密钥转成本地LMK对加密
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,platformProtectKey,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE114:: UnionHsmCmdA6 ret[%d]!\n",ret);
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE114: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}

				break;
			case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
				// 密钥转成本地LMK对加密
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,platformProtectKey,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE114:: UnionHsmCmdSV ret[%d]!\n",ret);
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE114: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE114:: symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}			

		// modify by leipp 20150708
		// 检查校验值
		if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
		{
			if ((tkeyApplyPlatform.packageType != 5) && (isUseOldKey))
			{
				snprintf(platformProtectKey,sizeof(platformProtectKey),"%s",psymmetricKeyValueZmk->oldKeyValue);
				isUseOldKey = 0;
				isRemainOriOldKey = 1;
				UnionLog("in UnionDealServiceCodeE114:: use old protectKey!\n");
				goto useOldKey;
			}
			else if(!isUseOldKey)
			{
				UnionUserErrLog("in UnionDealServiceCodeE114:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s] old key\n",symmetricKeyDB.checkValue,remoteCheckValue);
				return(errCodeEssc_CheckValue);
			}
			else
			{
				UnionUserErrLog("in UnionDealServiceCodeE114:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s] current key\n",symmetricKeyDB.checkValue,remoteCheckValue);
				return(errCodeEssc_CheckValue);

			}
		}
		else
		{
			// 远程密钥校验值可能是6位
			strcpy(symmetricKeyDB.checkValue, remoteCheckValue);
		}
		
		if (strcmp(psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue) == 0)
		{
			UnionLog("in UnionDealServiceCodeE114:: remoteKeyValue[%s] == currentkeyValue[%s]\n",psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue);
			return(0);
		}

		if (isRemainOriOldKey)
		{
			if (symmetricKeyDB.keyType == conZMK)
			{
				snprintf(psymmetricKeyValue->oldKeyValue,sizeof(psymmetricKeyValue->oldKeyValue),"%s",psymmetricKeyValueZmk->oldKeyValue);
				snprintf(symmetricKeyDB.oldCheckValue,sizeof(symmetricKeyDB.oldCheckValue),"%s",symmetricKeyDBZmk.oldCheckValue);
			}
		}
		// modify end	
		
		if ((ret =  UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionUpdateSymmetricKeyDBKeyValue,keyName[%s]!\n",symmetricKeyDB.keyName);
			return(ret);
		}
	}

	return(0);
}
