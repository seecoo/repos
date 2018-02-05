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
服务代码:	E11B
服务名:		更新密钥所有信息	
功能描述:	更新密钥所有信息 	
***************************************/
int UnionDealServiceCodeE11B(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				len = 0;
	int				outputByZMK = 0;
	int				mode = 1;
	int				inputFlag = 0;
	int				outputFlag = 0;
	int				exportFlag = 0;
	int				enabled;
	int				isDistributeFlag = 0;
	int				isApplyFlag = 0;
	int				isRemoteApplyKey = 0;
	int				isRemoteDistributeKey = 0;
	int				isRemoteKeyOperate = 0;
	char				protectKey[128];
	char				tmpBuf[256];
	char				zmk[64];
	char				tmk[64];
	char				keyByZMK[64];
	char				keyByTMK[64];
	char				keyByRKM[64];
	char				platformProtectKey[64];	
	char				remoteKeyValue[64];
	char				remoteCheckValue[32];
	char				sysID[32];
	char				appID[32];
	char				sql[256];
	char				checkValue[32];
	char				keyName[128];
	char				*ptr = NULL;

        char                            keyNode[80];
        char                            keyType[16];

	TUnionSymmetricKeyDB		symmetricKeyDB;
	TUnionSymmetricKeyDB		zmkKeyDB;
	PUnionSymmetricKeyValue		pzmkKeyValue = NULL;
	TUnionRemoteKeyPlatform		tkeyApplyPlatform;
	TUnionRemoteKeyPlatform		tkeyDistributePlatform;
	
	// 检测远程标识
	isRemoteKeyOperate = UnionIsRemoteKeyOperate();
	
	memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));

	// 密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
		keyName[0] = 0;
	else
		keyName[ret] = 0;

	UnionFilterHeadAndTailBlank(keyName);
	
	//密钥版本号 目前仅kms2发的旧报文转成XML报文需要带有version
        if((ret = UnionReadRequestXMLPackageValue("body/version",tmpBuf, sizeof(tmpBuf))) > 0)
        {
		if((ret = UnionAnalysisSymmetricKeyName(keyName, appID, keyNode, keyType)) < 0)
               	{
                       	UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionAnalysisSymmetricKeyName keyName[%s]\n", keyName);
                       	return(ret);
               	}
               	strcat(keyNode, "-001");
               	memset(keyName, 0, sizeof(keyName));
               	memcpy(keyName, appID, strlen(appID));
               	strcat(keyName, ".");
               	strcat(keyName, keyNode);
               	strcat(keyName, ".");
               	strcat(keyName, keyType);
	}
		
	snprintf(symmetricKeyDB.keyName,sizeof(symmetricKeyDB.keyName),"%s",keyName);

	// 模式
	// 0：不生成密钥
	// 1：生成密钥，默认值
	/*if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) <= 0)
		mode = 1;
	else
	{
		tmpBuf[ret] = 0;
		mode = atoi(tmpBuf);
		if ((mode != 0) && (mode != 1) && (mode != 2))
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: mode[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}*


	// 读取更新密钥标识
	if ((ret = UnionReadRequestXMLPackageValue("body/updateKeyFlag",updateKeyFlag,sizeof(updateKeyFlag))) <= 0)
		strcpy(updateKeyFlag,"0");*/

	// 检查密钥名称是否存在
	if (strlen(symmetricKeyDB.keyName) > 0)
	{
		if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) != errCodeKeyCacheMDL_WrongKeyName)
		{
		}
	}
	
	// 密钥保护方式
	strcpy(symmetricKeyDB.keyValue[0].lmkProtectMode,phsmGroupRec->lmkProtectMode);

	// 密钥组
	if ((ret = UnionReadRequestXMLPackageValue("body/keyGroup",symmetricKeyDB.keyGroup,sizeof(symmetricKeyDB.keyGroup))) <= 0)
		strcpy(symmetricKeyDB.keyGroup,"default");

	// 密钥类型
	if ((ret = UnionReadRequestXMLPackageValue("body/keyType",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyType");
		return(ret);
	}
	tmpBuf[ret] = 0;
	if (UnionIsDigitString(tmpBuf))
	{
		symmetricKeyDB.keyType = atoi(tmpBuf);
	}
	else	
	{
		if ((symmetricKeyDB.keyType = UnionConvertSymmetricKeyKeyType(tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: keyType[%s] error!\n",tmpBuf);
			return(symmetricKeyDB.keyType);	
		}
	}

	// 算法标识 
	if ((ret = UnionReadRequestXMLPackageValue("body/algorithmID",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","body/algorithmID");
		return(ret);	
	}
	tmpBuf[ret] = 0;
	if ((symmetricKeyDB.algorithmID = UnionConvertSymmetricKeyAlgorithmID(tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE11B:: algorithmID[%s] error!\n",tmpBuf);
		return(errCodeEsscMDL_InvalidAlgorithmID);	
	}

	// 密钥长度
	if ((ret = UnionReadRequestXMLPackageValue("body/keyLen",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyLen");
		return(ret);
	}
	tmpBuf[ret] = 0;
	if (!UnionIsDigitString(tmpBuf))
	{
		UnionUserErrLog("in UnionDealServiceCodeE11B:: keyLen[%s] is invalid!\n",tmpBuf);
		return(errCodeEssc_KeyLength);
	}

	switch (atoi(tmpBuf))
	{
		case	0:
		case	16:
		case	64:
			symmetricKeyDB.keyLen = con64BitsSymmetricKey;
			break;
		case	1:
		case	32:
		case	128:
			symmetricKeyDB.keyLen = con128BitsSymmetricKey;
			break;
		case	2:
		case	48:
		case	192:
			symmetricKeyDB.keyLen = con192BitsSymmetricKey;
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE11B:: keyLen[%s] is error!\n",tmpBuf);
			return(errCodeEssc_KeyLength);
	}

	if (symmetricKeyDB.algorithmID == conSymmetricAlgorithmIDOfSM4 && (symmetricKeyDB.keyLen != con128BitsSymmetricKey))
	{
		UnionUserErrLog("in UnionDealServiceCodeE11B:: SM4 keyLen[%d]!\n",symmetricKeyDB.keyLen);
		UnionSetResponseRemark("SM4密钥长度非法");
		return(errCodeParameter);
	}

	// 允许使用旧密钥
	if ((ret = UnionReadRequestXMLPackageValue("body/oldVersionKeyIsUsed",tmpBuf,sizeof(tmpBuf))) <= 0)
		symmetricKeyDB.oldVersionKeyIsUsed = 1;
	else
	{
		tmpBuf[ret] = 0;
		symmetricKeyDB.oldVersionKeyIsUsed = atoi(tmpBuf);
	}
	if ((symmetricKeyDB.oldVersionKeyIsUsed != 0) && (symmetricKeyDB.oldVersionKeyIsUsed != 1))
	{
		UnionUserErrLog("in UnionDealServiceCodeE11B:: oldVersionKeyIsUsed[%s] error!\n",tmpBuf);
		return(errCodeParameter);
	}

	// 允许导入标识
	// 可选，默认值为0
	// 0：不允许
	// 1：允许
	if ((ret = UnionReadRequestXMLPackageValue("body/inputFlag",tmpBuf,sizeof(tmpBuf))) <= 0)
		symmetricKeyDB.inputFlag = 0;
	else
	{
		tmpBuf[ret] = 0;
		symmetricKeyDB.inputFlag = atoi(tmpBuf);
		if ((symmetricKeyDB.inputFlag != 0) && (symmetricKeyDB.inputFlag != 1) && (symmetricKeyDB.inputFlag != 2))
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: inputFlag[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}

	// 允许导出标识
	// 可选，默认值为0
	// 0：不允许
	// 1：允许
	if ((ret = UnionReadRequestXMLPackageValue("body/outputFlag",tmpBuf,sizeof(tmpBuf))) <= 0)
		symmetricKeyDB.outputFlag = 0;
	else
	{
		tmpBuf[ret] = 0;
		symmetricKeyDB.outputFlag = atoi(tmpBuf);
		if ((symmetricKeyDB.outputFlag != 0) && (symmetricKeyDB.outputFlag != 1))
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: outputFlag[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}

	// 有效天数
	if ((ret = UnionReadRequestXMLPackageValue("body/effectiveDays",tmpBuf,sizeof(tmpBuf))) <= 0)
		symmetricKeyDB.effectiveDays = 0;
	else
	{
		UnionFilterHeadAndTailBlank(tmpBuf);
		if (!UnionIsDigitString(tmpBuf))
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: effectiveDays[%s] error!\n",tmpBuf);
			UnionSetResponseRemark("有效天数[%s]非法,必须为数字",tmpBuf);
                        return(errCodeParameter);
		}
		
		symmetricKeyDB.effectiveDays = atoi(tmpBuf);
	}

	// 启用标识
	// 0：不启用
	// 1：即刻启用，并生效，默认值
	// 2：即刻启用：指定时间生效
	if ((ret = UnionReadRequestXMLPackageValue("body/enabled",tmpBuf,sizeof(tmpBuf))) <= 0)
		enabled = 1;
	else
	{
		tmpBuf[ret] = 0;
		enabled = atoi(tmpBuf);
	}
		
	if (mode == 0 || mode == 2)
	{
		enabled = 0;
	}
	
	UnionLog("in UnionDealServiceCodeE11B:: mode[%d] inputFlag[%d] outputFlag[%d] enabled[%d] exportFlag[%d],isRemoteKeyOperate[%d]\n",mode,inputFlag,outputFlag,enabled,exportFlag,isRemoteKeyOperate);
	
	if (enabled == 0)
		symmetricKeyDB.status = conSymmetricKeyStatusOfInitial;	// 状态设置为初始化状态
	else
		symmetricKeyDB.status = conSymmetricKeyStatusOfEnabled;	// 状态设置为启用状态

	// 保护密钥
	// 当exportFlag为0时，不存在
	// 当exportFlag为1时：为密钥名称
	// 当exportFlag为2,3时，为密钥值
	if (exportFlag != 0)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/protectKey",protectKey,sizeof(protectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","body/protectKey");
			return(ret);
		}
		protectKey[ret] = 0;
	}
	
	// 读取保护密钥
	if (exportFlag == 0)
		outputByZMK = 0;
	else if (exportFlag == 1)
	{
		outputByZMK = 1;
		if ((ret =  UnionReadSymmetricKeyDBRec(protectKey,0,&zmkKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadSymmetricKeyDBRec protectKey[%s]!\n",protectKey);
			return(ret);
		}
		if ((zmkKeyDB.keyType != conZMK) && (zmkKeyDB.keyType != conTMK))
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: is not zmk or tmk [%s]!\n",protectKey);
			UnionSetResponseRemark("非法保护密钥类型");	
			return(errCodeEsscMDL_WrongUsageOfKey);
		}
		if ((pzmkKeyValue = UnionGetSymmetricKeyValue(&zmkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("读取密钥值失败");
			return(errCodeParameter);
		}
		if (zmkKeyDB.keyType == conTMK)
		{
			if (symmetricKeyDB.keyType != conTMK && 
				symmetricKeyDB.keyType != conTPK && 
				symmetricKeyDB.keyType != conPVK && 
			 	symmetricKeyDB.keyType != conTAK)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: keyName[%s] keyType is not tmk,tpk,pvk or tak!\n",symmetricKeyDB.keyName);
				UnionSetResponseRemark("密钥[%s]不支持[TMK]保护方式",symmetricKeyDB.keyName);
				return(errCodeParameter);
			}
			
			strcpy(tmk,pzmkKeyValue->keyValue);
		}
		else
			strcpy(zmk,pzmkKeyValue->keyValue);
	}
	else
	{
		outputByZMK = 1;
		len = strlen(protectKey);
		if ((len != 16) && (len != 32) && (len != 48))
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: protectKey[%s] len != [16,32,48]\n",protectKey);
			return(errCodeEssc_KeyLength);
		}
		if (exportFlag == 3)
		{
			if (symmetricKeyDB.keyType != conTMK && 
				symmetricKeyDB.keyType != conTPK && 
				symmetricKeyDB.keyType != conPVK && 
			 	symmetricKeyDB.keyType != conTAK)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: keyName[%s] keyType is not tmk,tpk,pvk or tak!\n",symmetricKeyDB.keyName);
				UnionSetResponseRemark("密钥[%s]不支持[TMK]保护方式",symmetricKeyDB.keyName);
				return(errCodeParameter);
			}

			strcpy(tmk,protectKey);
		}
		else
			strcpy(zmk,protectKey);
	}
	
	if (!isRemoteKeyOperate)		// 本地密钥操作	
	{
		// modify by leipp 20151209 增加UnionCheckRemoteKeyPlatform函数
		// 密钥申请平台
		UnionReadRequestXMLPackageValue("body/keyApplyPlatform",symmetricKeyDB.keyApplyPlatform,sizeof(symmetricKeyDB.keyApplyPlatform));
	
		// 密钥分发平台
		UnionReadRequestXMLPackageValue("body/keyDistributePlatform",symmetricKeyDB.keyDistributePlatform,sizeof(symmetricKeyDB.keyDistributePlatform));

		// 读取并检查申请和分发平台
		if ((ret = UnionCheckRemoteKeyPlatform(symmetricKeyDB.keyName, symmetricKeyDB.keyApplyPlatform, symmetricKeyDB.keyDistributePlatform, &tkeyApplyPlatform, &tkeyDistributePlatform, &isRemoteApplyKey, &isRemoteDistributeKey)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionCheckRemoteKeyPlatform!\n");
			return(ret);
		}
		// modify by leipp end
	}
	else			// 远程密钥操作
	{
		isRemoteApplyKey = 0;
		isRemoteDistributeKey = 0;
		
		// 系统ID
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);
		}
		sysID[ret] = 0;

		//获取保护密钥
		snprintf(sql,sizeof(sql),"select protectKey from localKeyPlatform where sysID = '%s' and algorithmID = %d",sysID,symmetricKeyDB.algorithmID);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionSelectRealDBRecord[%s]\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: protectKey not found!\n");
			UnionSetResponseRemark("远程保护密钥不存在");
			return(errCodeParameter);
		}

		if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionLocateXMLPackage[%s]!\n","detail");	
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("protectKey",platformProtectKey,sizeof(platformProtectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadXMLPackageValue[%s]!\n","protectKey");
			return(ret);
		}
		platformProtectKey[ret] = 0;

		// 密钥申请
		if ((ret = UnionReadRequestXMLPackageValue("body/keyApplyPlatform",symmetricKeyDB.keyApplyPlatform,sizeof(symmetricKeyDB.keyApplyPlatform))) > 0)
		{
			strcpy(symmetricKeyDB.keyApplyPlatform,"");
			isApplyFlag = 1;
		}

		// 密钥分发
		if ((ret = UnionReadRequestXMLPackageValue("body/keyDistributePlatform",symmetricKeyDB.keyDistributePlatform,sizeof(symmetricKeyDB.keyDistributePlatform))) > 0)
		{
			strcpy(symmetricKeyDB.keyDistributePlatform,"");
			isDistributeFlag = 1;
		}
	}
	
	if (isRemoteApplyKey)		// 远程申请密钥
	{
		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}
		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	if (mode == 1)	// 生成密钥
	{
		if (isRemoteApplyKey)	// 远程申请
		{
			// 读取远程密钥值
			if((ret = UnionReadResponseRemoteXMLPackageValue("body/keyValue",remoteKeyValue,sizeof(remoteKeyValue))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/keyValue");
				return(ret);
			}
			remoteKeyValue[ret] = 0;

			// 读取远程密钥校验值
			if((ret = UnionReadResponseRemoteXMLPackageValue("body/checkValue",remoteCheckValue,sizeof(remoteCheckValue))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/checkValue");
				return(ret);
			}
			remoteCheckValue[ret] = 0;

			switch(symmetricKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DES算法
					// 密钥转成本地LMK对加密
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case	conHsmCmdVerRacalStandardHsmCmd:
						case	conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,tkeyApplyPlatform.protectKey,remoteKeyValue,symmetricKeyDB.keyValue[0].keyValue,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdA6!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}

					// 检查校验值
					if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE11B:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
						return(errCodeEssc_CheckValue);
					}

					if (outputByZMK == 1)
					{
						// 把本地LMK对加密转成ZMK/TMK加密
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case	conHsmCmdVerRacalStandardHsmCmd:
							case	conHsmCmdVerSJL06StandardHsmCmd:
								if (((exportFlag == 1) && (zmkKeyDB.keyType == conTMK)) || (exportFlag == 3))
								{
									// TMK保护输出
									if (symmetricKeyDB.keyType == conTMK || symmetricKeyDB.keyType == conTPK || symmetricKeyDB.keyType == conPVK)
									{
										if ((ret = UnionHsmCmdAE(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
										{
											UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdAE!\n");
											return(ret);
										}
										strcpy(keyByZMK,keyByTMK);
									}
									else if (symmetricKeyDB.keyType == conTAK)
									{
										if ((ret = UnionHsmCmdAG(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
										{
											UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdAG!\n");
											return(ret);
										}
										strcpy(keyByZMK,keyByTMK);
									}
								}
								else	// zmk保护输出
								{
									if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,symmetricKeyDB.keyValue[0].keyValue,zmk,keyByZMK,symmetricKeyDB.checkValue)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdA8!\n");
										return(ret);
									}

									// 检查校验值
									if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE11B:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
										return(errCodeEssc_CheckValue);
									}
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("非法的加密机指令类型");
								return(errCodeParameter);
						}
					}
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
					// 密钥转成本地LMK对加密
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,tkeyApplyPlatform.protectKey,remoteKeyValue,symmetricKeyDB.keyValue[0].keyValue,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdSV!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					// 检查校验值
					if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE11B:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
						return(errCodeEssc_CheckValue);
					}

					if (outputByZMK == 1)
					{
						// 把本地LMK对加密转成ZMK加密
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case conHsmCmdVerRacalStandardHsmCmd:
							case conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,symmetricKeyDB.keyValue[0].keyValue,zmk,keyByZMK,symmetricKeyDB.checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdST!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("非法的加密机指令类型");
								return(errCodeParameter);
						}
						// 检查校验值
						if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE11B:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
							return(errCodeEssc_CheckValue);
						}
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE11B:: symmetricKeyDB.algorithmID[%d] error!\n",symmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}			
		}
		else if (isDistributeFlag)	// 远程分发
		{
			// 读取远程密钥值
			if((ret = UnionReadRequestXMLPackageValue("body/keyValue",remoteKeyValue,sizeof(remoteKeyValue))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyValue");
				return(ret);
			}
			remoteKeyValue[ret] = 0;

			// 读取远程密钥校验值
			if((ret = UnionReadRequestXMLPackageValue("body/checkValue",remoteCheckValue,sizeof(remoteCheckValue))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","body/checkValue");
				return(ret);
			}
			remoteCheckValue[ret] = 0;


			switch(symmetricKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DES算法
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,platformProtectKey,remoteKeyValue,symmetricKeyDB.keyValue[0].keyValue,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdA6!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
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
							if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,platformProtectKey,remoteKeyValue,symmetricKeyDB.keyValue[0].keyValue,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdSV!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key symmetricKeyDB.algorithmID[%d] error!\n",symmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}			

			// 检查校验值
			if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
				return(errCodeEssc_CheckValue);
			}
		}
		else if(isRemoteApplyKey == 0)		// 本地生成
		{
			switch(symmetricKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DES算法
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if (((exportFlag == 1) && (zmkKeyDB.keyType == conTMK)) || (exportFlag == 3))
								strcpy(tmpBuf,"0");
							else
								sprintf(tmpBuf,"%d",outputByZMK);
								
							if ((ret = UnionHsmCmdA0(atoi(tmpBuf),symmetricKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen),zmk,symmetricKeyDB.keyValue[0].keyValue,keyByZMK,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdA0!\n");
								return(ret);
							}

							if (((exportFlag == 1) && (zmkKeyDB.keyType == conTMK)) || (exportFlag == 3))
							{
								// TMK保护输出
								if (symmetricKeyDB.keyType == conTMK || symmetricKeyDB.keyType == conTPK || symmetricKeyDB.keyType == conPVK)
								{
									if ((ret = UnionHsmCmdAE(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdAE!\n");
										return(ret);
									}
									strcpy(keyByZMK,keyByTMK);
								}
								else if (symmetricKeyDB.keyType == conTAK)
								{
									if ((ret = UnionHsmCmdAG(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdAG!\n");
										return(ret);
									}
									strcpy(keyByZMK,keyByTMK);
								}
							}
							
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					if (isApplyFlag) // 远程
					{
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case conHsmCmdVerRacalStandardHsmCmd:
							case conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,symmetricKeyDB.keyValue[0].keyValue,platformProtectKey,keyByZMK,remoteCheckValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key UnionHsmCmdA8!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("非法的加密机指令类型");
								return(errCodeParameter);
						}
						// 检查校验值
						if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
							return(errCodeEssc_CheckValue);
						}					
					}
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
					snprintf(tmpBuf,sizeof(tmpBuf),"%d",outputByZMK);
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdWI(tmpBuf,symmetricKeyDB.keyType,zmk,symmetricKeyDB.keyValue[0].keyValue,keyByZMK,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdWI!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					if (isApplyFlag) // 远程
					{
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case conHsmCmdVerRacalStandardHsmCmd:
							case conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,symmetricKeyDB.keyValue[0].keyValue,platformProtectKey,keyByZMK,remoteCheckValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key UnionHsmCmdST!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("非法的加密机指令类型");
								return(errCodeParameter);
						}
						// 检查校验值
						if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
							return(errCodeEssc_CheckValue);
						}
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE11B:: symmetricKeyDB.algorithmID[%d] error!\n",symmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
		}
	}

	// 生效日期
	// 当enabled为2时，存在
	if (enabled == 1)
	{
		UnionGetFullSystemDate(symmetricKeyDB.activeDate);
	}
	else if (enabled == 2)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/activeDate",symmetricKeyDB.activeDate,sizeof(symmetricKeyDB.activeDate))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","body/activeDate");
			return(ret);	
		}
		
		UnionFilterHeadAndTailBlank(symmetricKeyDB.activeDate);
		if(strlen(symmetricKeyDB.activeDate) >0)
		{
			if (!UnionIsValidFullDateStr(symmetricKeyDB.activeDate))
			{
                	        UnionUserErrLog("in UnionDealServiceCodeE11B:: activeDate[%s] error!\n",symmetricKeyDB.activeDate);
                	        UnionSetResponseRemark("生效日期格式[%s]非法,必须为YYYYMMDD格式",symmetricKeyDB.activeDate);
				return(errCodeParameter);
                	}
		}
		else
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: activeDate can not be null!\n");
			UnionSetResponseRemark("生效日期不能为空");
			return(errCodeParameter);
		}
	}
	
	if ((ret = UnionReadRequestXMLPackageValue("body/creatorType",tmpBuf,sizeof(tmpBuf))) <= 0)
		symmetricKeyDB.creatorType = conSymmetricCreatorTypeOfApp;
	else
	{
		tmpBuf[ret] = 0;
		if (atoi(tmpBuf) == conSymmetricCreatorTypeOfKMS)
			symmetricKeyDB.creatorType = conSymmetricCreatorTypeOfKMS;
		else
			symmetricKeyDB.creatorType = conSymmetricCreatorTypeOfApp;
	}
	
	if (isRemoteKeyOperate)		// 远程密钥操作
	{
		if ((ret = UnionReadRemoteSysIDAndAppID(sysID,sizeof(sysID),appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRemoteSysIDAndAppID!\n");
			return(ret);	
		}
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",symmetricKeyDB.usingUnit,sizeof(symmetricKeyDB.usingUnit))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);
		}
	}
	else				// 本地密钥操作
	{
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);	
		}
		if ((ret = UnionReadRequestXMLPackageValue("head/appID",appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");
			return(ret);	
		}
		if (strcasecmp(UnionGetIDOfCustomization(),"aliyun") == 0)	// 阿里云
		{
			if ((ptr = strchr(sysID,':')) == NULL)
			{
				UnionUserErrLog("in UnionCheckUnitID:: sysID[%s] error\n",sysID);
				return(errCodeParameter);
			}               
			ptr += 1;
			sprintf(symmetricKeyDB.usingUnit,"%s",ptr);
		}
	}
	// 设置创建者
	UnionPieceSymmetricKeyCreator(sysID,appID,symmetricKeyDB.creator);

	if (isRemoteDistributeKey)	// 远程分发密钥
	{
		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}
		if (mode == 1)	// 生成密钥
		{
			// 增加密钥和校验值
			switch(symmetricKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DES算法
					// 把本地LMK对加密转成远程ZMK加密
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,symmetricKeyDB.keyValue[0].keyValue,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key UnionHsmCmdA8!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					// 检查校验值
					if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
						return(errCodeEssc_CheckValue);
					}					
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
					// 把本地LMK对加密转成远程ZMK加密
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,symmetricKeyDB.keyValue[0].keyValue,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key UnionHsmCmdST!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					// 检查校验值
					if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
						return(errCodeEssc_CheckValue);
					}					
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key symmetricKeyDB.algorithmID[%d] error!\n",symmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}			
			if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyValue",keyByRKM)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyValue",keyByRKM);
				return(ret);
			}
			if ((ret = UnionSetRequestRemoteXMLPackageValue("body/checkValue",remoteCheckValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/checkValue",remoteCheckValue);
				return(ret);
			}
		}
		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	if (strlen(symmetricKeyDB.keyName) > 0)
	{
		if (mode == 0 || mode == 2)	// 不生成密钥
		{
			if ((ret =  UnionCreateSymmetricKeyDB(&symmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionCreateSymmetricKeyDB,keyName[%s]!\n",symmetricKeyDB.keyName);
				return(ret);	
			}
		}
		else
		{
			if ((ret =  UnionGenerateSymmetricKeyDBRec(&symmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionGenerateSymmetricKeyDBRec,keyName[%s]!\n",symmetricKeyDB.keyName);
				return(ret);	
			}
		}
	}
	
	// modify by leipp 20160125	修复向远程分发密钥不需要返回密钥与校验值,增加[isRemoteKeyOperate == 0]
	if ((exportFlag != 0 && isRemoteKeyOperate == 0)|| (isRemoteKeyOperate && isApplyFlag == 1))
	{
		if (!isRemoteKeyOperate)
		{
			if ((ret = UnionSetResponseXMLPackageValue("body/keyValue2",symmetricKeyDB.keyValue[0].keyValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyValue2");
				return(ret);
			}
		}

		if ((ret = UnionSetResponseXMLPackageValue("body/keyValue",keyByZMK)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyValue");
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",symmetricKeyDB.checkValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionSetResponseXMLPackageValue[%s]!\n","body/checkValue");
			return(ret);
		}
	}
	// modify end
	return(0);
}
