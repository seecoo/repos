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
服务代码:	E111
服务名:		更新对称密钥
功能描述:	更新对称密钥
***************************************/
int UnionDealServiceCodeE111(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				len;
	int				outputByZMK = 0;
	int				mode = 1;
	int				exportFlag = 0;
	char				protectKey[128];
	char				tmpBuf[128];
	char				zmk[64];
	char				tmk[64];
	char				keyByZMK[64];
	char				keyByRKM[64];
	char				keyByTMK[64];
	char				platformProtectKey[64];
	int				isRemoteKeyOperate = 0;
	int				isRemoteApplyKey = 0;
	int				isRemoteDistributeKey = 0;
	int				isDistributeFlag = 0;
	int				isApplyFlag = 0;
	char				remoteKeyValue[64];
	char				remoteCheckValue[32];
	char				sql[128];
	char				exportLmkKey[32];
	char				checkValue[32];
	char				keyName[160];
	int				flag = 0;
	char				algorithmID[8];
	
	char	        	        sysID[32];
        char    	                appID[32];
        char                    	creator[64];

	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;
	TUnionSymmetricKeyDB		zmkKeyDB;
	PUnionSymmetricKeyValue		pzmkKeyValue = NULL;
	TUnionRemoteKeyPlatform		tkeyApplyPlatform;
	TUnionRemoteKeyPlatform		tkeyDistributePlatform;

	// 模式
        // 1：本地生成密钥，默认值
        // 2：本地生成密钥，并打印
        if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) <= 0)
                mode = 1;
        else
        {
		tmpBuf[ret] = 0;
                mode = atoi(tmpBuf);
        }

	// 检测是否远程操作
	isRemoteKeyOperate = UnionIsRemoteKeyOperate();
	if (isRemoteKeyOperate)
		flag = 1;

	
	// 密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);
	
	// 读取对称密钥
	UnionInitSymmetricKeyDB(&symmetricKeyDB);
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
	{
		if (ret == errCodeKeyCacheMDL_WrongKeyName)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: keyName[%s] not find!\n",keyName);
			UnionSetResponseRemark("对称密钥[%s]不存在",keyName);
			return(errCodeKeyCacheMDL_KeyNonExists);
		}
		UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
		return(ret);
	}

	// 检查密钥是否可以使用
	if (0 == flag)
	{
		// 为兼容旧系统设定的检查标识
		if ((ret = UnionReadRequestXMLPackageValue("body/sysFlag",tmpBuf,sizeof(tmpBuf))) > 0)
			flag = 1;
	}
		
	if ((ret = UnionIsUseCheckSymmetricKeyDB(symmetricKeyDB.status,flag)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE111:: UnionIsUseCheckSymmetricKeyDB symmetricKeyDB.status[%d] flag[%d]!\n",symmetricKeyDB.status,flag);
		return(ret);
	}
	
	// 输出标识
	// 0：不输出，默认值
	// 1：指定密钥名称保护输出
	// 2：外带ZMK密钥保护输出
	if ((ret = UnionReadRequestXMLPackageValue("body/exportFlag",tmpBuf,sizeof(tmpBuf))) <= 0)
		exportFlag = 0;
	else
	{
		tmpBuf[ret] = 0;
		exportFlag = atoi(tmpBuf);
		if ((exportFlag != 0) && (exportFlag != 1) && (exportFlag != 2) && (exportFlag != 3) && (exportFlag != 9))
			exportFlag = 0;
	}

	// 不允许输出
	if ((symmetricKeyDB.outputFlag == 0) && (exportFlag != 0))
	{
		UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.outputFlag[%d]!\n",symmetricKeyDB.outputFlag);
		return(errCodeEsscMDL_KeyOutputNotPermitted);		
	}
	
	// 保护密钥
	// 当exportFlag为0时，不存在
	// 当exportFlag为1时：为密钥名称
	// 当exportFlag为2时，为密钥值
	if (exportFlag != 0 && exportFlag != 9)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/protectKey",protectKey,sizeof(protectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRequestXMLPackageValue[%s]!\n","body/protectKey");
			return(ret);	
		}
		protectKey[ret] = 0;
	}

	exportLmkKey[0] = 0;
	UnionReadRequestXMLPackageValue("body/exportLmkKey",exportLmkKey,sizeof(exportLmkKey));
	
	// 读取保护密钥
	if (exportFlag == 0)
		outputByZMK = 0;
	else if (exportFlag == 1)
	{
		outputByZMK = 1;
		UnionInitSymmetricKeyDB(&zmkKeyDB);
		if ((ret =  UnionReadSymmetricKeyDBRec(protectKey,1,&zmkKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadSymmetricKeyDBRec protectKey[%s]!\n",protectKey);
			return(ret);
		}
		if ((zmkKeyDB.keyType != conZMK) && (zmkKeyDB.keyType != conTMK))
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: is not zmk or tmk [%s]!\n",protectKey);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}

		if ((pzmkKeyValue = UnionGetSymmetricKeyValue(&zmkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
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
				UnionUserErrLog("in UnionDealServiceCodeE111:: keyName[%s] keyType is not tmk,tpk,pvk or tak!\n",symmetricKeyDB.keyName);
				UnionSetResponseRemark("密钥[%s]不支持[TMK]保护方式",symmetricKeyDB.keyName);
				return(errCodeParameter);
			}
			strcpy(tmk,pzmkKeyValue->keyValue);
		}
		else
		strcpy(zmk,pzmkKeyValue->keyValue);
	}
	else if(exportFlag == 9)
	{
		outputByZMK = 1;
		// 使用旧密钥保护新密钥，保护密钥在下面读密钥值时获取
	}
	else
	{
		outputByZMK = 1;
		len = strlen(protectKey);
		if ((len != 16) && (len != 32) && (len != 48))
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: protectKey[%s] len != [16,32,48]\n",protectKey);
			return(errCodeEssc_KeyLength);
		}
		if (exportFlag == 3)
		{
			if (symmetricKeyDB.keyType != conTMK && 
				symmetricKeyDB.keyType != conTPK && 
				symmetricKeyDB.keyType != conPVK && 
			 	symmetricKeyDB.keyType != conTAK)
			{
				UnionUserErrLog("in UnionDealServiceCodeE111:: keyName[%s] keyType is not tmk,tpk,pvk or tak!\n",symmetricKeyDB.keyName);
				UnionSetResponseRemark("密钥[%s]不支持[TMK]保护方式",symmetricKeyDB.keyName);
				return(errCodeParameter);
			}

			strcpy(tmk,protectKey);
		}
		else
			strcpy(zmk,protectKey);
	}
	
	// 生效时间
	if ((ret = UnionReadRequestXMLPackageValue("body/activeDate",symmetricKeyDB.activeDate,sizeof(symmetricKeyDB.activeDate))) <= 0)
	{
		UnionGetFullSystemDate(symmetricKeyDB.activeDate);
	}
	UnionFilterHeadAndTailBlank(symmetricKeyDB.activeDate);
	if (!UnionIsValidFullDateStr(symmetricKeyDB.activeDate))
        {
                UnionUserErrLog("in UnionDealServiceCodeE111:: activeDate[%s] error!\n",symmetricKeyDB.activeDate);
                UnionSetResponseRemark("生效日期格式[%s]非法,必须为YYYYMMDD格式",symmetricKeyDB.activeDate);
                return(errCodeParameter);
        }

	// 读取密钥值
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) != NULL)
	{
		// 当前密钥置为旧密钥
		strcpy(psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue);
		strcpy(symmetricKeyDB.oldCheckValue,symmetricKeyDB.checkValue);

		psymmetricKeyValue->keyValue[0] = 0;
		symmetricKeyDB.checkValue[0] = 0;;
	}
	else
		psymmetricKeyValue = &symmetricKeyDB.keyValue[0];

	strcpy(psymmetricKeyValue->lmkProtectMode,phsmGroupRec->lmkProtectMode);

	// add by leipp 20151022
	// 算法标识,如果算法标识与当前密钥算法标识不同，则清空密钥值,修改密钥属性
	memset(algorithmID,0,sizeof(algorithmID));
	ret = UnionReadRequestXMLPackageValue("body/algorithmID",algorithmID,sizeof(algorithmID));
	if (ret > 0)
	{
		UnionFilterHeadAndTailBlank(algorithmID);
		if ((strcasecmp(algorithmID,"DES") == 0) && (symmetricKeyDB.algorithmID != conSymmetricAlgorithmIDOfDES))
		{
			symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfDES;
			psymmetricKeyValue->oldKeyValue[0] = 0;
			symmetricKeyDB.oldCheckValue[0] = 0;
			strcpy(symmetricKeyDB.keyValue[1].lmkProtectMode,phsmGroupRec->lmkProtectMode);
		}	
		else if ((strcasecmp(algorithmID,"SM4") == 0) && (symmetricKeyDB.algorithmID != conSymmetricAlgorithmIDOfSM4))
		{
			symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfSM4;
			psymmetricKeyValue->oldKeyValue[0] = 0;
			symmetricKeyDB.oldCheckValue[0] = 0;
			strcpy(symmetricKeyDB.keyValue[1].lmkProtectMode,phsmGroupRec->lmkProtectMode);
		}
	}
	// add end

	// 如果是使用旧密钥导出新密钥，设置导出密钥的保护密钥值
	if(exportFlag == 9)
	{
		// 非主密钥不允许使用旧密钥输出
		if (symmetricKeyDB.outputFlag > 0 && symmetricKeyDB.keyType != conTMK && symmetricKeyDB.keyType != conZMK)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: work key not alow to exp by its old value!\n");
			return(errCodeEsscMDL_KeyOutputNotPermitted);
		}

		if(symmetricKeyDB.keyType == conTMK)
		{
			strcpy(tmk, psymmetricKeyValue->oldKeyValue);
		}
		else
		{
			strcpy(zmk, psymmetricKeyValue->oldKeyValue);
		}
	}
	
	// 本地操作
	if (!isRemoteKeyOperate)
	{
		// modify by leipp 20151209
		// 读取并检查申请和分发平台
		if ((ret = UnionCheckRemoteKeyPlatform(symmetricKeyDB.keyName, symmetricKeyDB.keyApplyPlatform, symmetricKeyDB.keyDistributePlatform, &tkeyApplyPlatform, &tkeyDistributePlatform, &isRemoteApplyKey, &isRemoteDistributeKey)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionCheckRemoteKeyPlatform!\n");
			return(ret);
		}
		// modify end
	}
	else	// 远程操作
	{
		isRemoteApplyKey = 0;
		isRemoteDistributeKey = 0;
		// 获取远程操作标识
		if ((ret = UnionReadRequestXMLPackageValue("body/remoteOperateFlag",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRequestXMLPackageValue[%s]!\n","body/remoteOperateFlag");
			return(ret);
		}
		tmpBuf[ret] = 0;

		if (tmpBuf[0] == '1')
			isApplyFlag = 1;
		else 
			isDistributeFlag = 1;
			
		// 系统ID
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);
		}
		tmpBuf[ret] = 0;

		//获取保护密钥
		snprintf(sql,sizeof(sql),"select protectKey from localKeyPlatform where sysID = '%s' and algorithmID = %d",tmpBuf,symmetricKeyDB.algorithmID);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSelectRealDBRecord[%s]\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: remoteProtectKey not found!\n");
			UnionSetResponseRemark("远程保护密钥不存在");
			return(errCodeParameter);
		}

		if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionLocateXMLPackage[%s]!\n","detail");	
			return(ret);
		}

		// 读取平台保护密钥
		if ((ret = UnionReadXMLPackageValue("protectKey",platformProtectKey,sizeof(platformProtectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadXMLPackageValue[%s]!\n","protectKey");
			return(ret);
		}
		platformProtectKey[ret] = 0;
	}

	// 远程申请密钥
	if (isRemoteApplyKey)
	{
		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// 设置申请标志 1:申请
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/remoteOperateFlag","1")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/remoteOperateFlag","1");
			return(ret);
		}

		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	switch(mode)
	{
		case 1:
			if (isRemoteApplyKey)	//远程生成，本地存储
			{
				// 读取远程密钥值
				if((ret = UnionReadResponseRemoteXMLPackageValue("body/keyValue",remoteKeyValue,sizeof(remoteKeyValue))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/keyValue");
					return(ret);
				}
				remoteKeyValue[ret] = 0;

				// 读取远程密钥校验值
				if((ret = UnionReadResponseRemoteXMLPackageValue("body/checkValue",remoteCheckValue,sizeof(remoteCheckValue))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/checkValue");
					return(ret);
				}
				remoteCheckValue[ret] = 0;

				switch(symmetricKeyDB.algorithmID)
				{
					case	conSymmetricAlgorithmIDOfDES:	// DES算法
						// 密钥转成本地LMK对加密
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case conHsmCmdVerRacalStandardHsmCmd:
							case conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,tkeyApplyPlatform.protectKey,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdA6!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("非法的加密机指令类型");
								return(errCodeParameter);
						}

						// 检查校验值
						if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
							return(errCodeEssc_CheckValue);
						}

						if (outputByZMK == 1)
						{
							// 把本地LMK对加密转成ZMK加密
							switch(phsmGroupRec->hsmCmdVersionID)
							{
								case	conHsmCmdVerRacalStandardHsmCmd:
								case	conHsmCmdVerSJL06StandardHsmCmd:

									if (((exportFlag == 1) && (zmkKeyDB.keyType == conTMK)) || (exportFlag == 3) || (exportFlag == 9 && symmetricKeyDB.keyType == conTMK))
									{
										// TMK保护输出
										if (symmetricKeyDB.keyType == conTMK || symmetricKeyDB.keyType == conTPK || symmetricKeyDB.keyType == conPVK)
										{
											if ((ret = UnionHsmCmdAE(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
											{
												UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdAE!\n");
												return(ret);
											}
											strcpy(keyByZMK,keyByTMK);
										}
										else if (symmetricKeyDB.keyType == conTAK)
										{
											if ((ret = UnionHsmCmdAG(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
											{
												UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdAG!\n");
												return(ret);
											}
											strcpy(keyByZMK,keyByTMK);
										}
									}
									else
									{
										if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,zmk,keyByZMK,symmetricKeyDB.checkValue)) < 0)
										{
											UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdA8!\n");
											return(ret);
										}
									}

									break;
								default:
									UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
									UnionSetResponseRemark("非法的加密机指令类型");
									return(errCodeParameter);
							}
							// 检查校验值
							if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
								return(errCodeEssc_CheckValue);
							}
						}
						break;
					case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
						// 密钥转成本地LMK对加密
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case conHsmCmdVerRacalStandardHsmCmd:
							case conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,tkeyApplyPlatform.protectKey,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdSV!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("非法的加密机指令类型");
								return(errCodeParameter);
						}
						// 检查校验值
						if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
							return(errCodeEssc_CheckValue);
						}

						if (outputByZMK == 1)
						{
							// 把本地LMK对加密转成ZMK加密
							switch(phsmGroupRec->hsmCmdVersionID)
							{
								case conHsmCmdVerRacalStandardHsmCmd:
								case conHsmCmdVerSJL06StandardHsmCmd:
									if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,zmk,keyByZMK,symmetricKeyDB.checkValue)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdST!\n");
										return(ret);
									}
									break;
								default:
									UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
									UnionSetResponseRemark("非法的加密机指令类型");
									return(errCodeParameter);
							}
							// 检查校验值
							if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
								return(errCodeEssc_CheckValue);
							}
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
						return(errCodeEsscMDL_InvalidAlgorithmID);
				}			
			}
			else if (isDistributeFlag) //　远程存储
			{
				// 读取远程密钥值
				if((ret = UnionReadRequestXMLPackageValue("body/keyValue",remoteKeyValue,sizeof(remoteKeyValue))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyValue");
					return(ret);
				}
				remoteKeyValue[ret] = 0;

				// 读取远程密钥校验值
				if((ret = UnionReadRequestXMLPackageValue("body/checkValue",remoteCheckValue,sizeof(remoteCheckValue))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRequestXMLPackageValue[%s]!\n","body/checkValue");
					return(ret);
				}
				remoteCheckValue[ret] = 0;

				switch(symmetricKeyDB.algorithmID)
				{
					case	conSymmetricAlgorithmIDOfDES:	// DES算法
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case	conHsmCmdVerRacalStandardHsmCmd:
							case	conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,platformProtectKey,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdA6!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("非法的加密机指令类型");
								return(errCodeParameter);
						}

						// 检查校验值
						if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
							return(errCodeEssc_CheckValue);
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
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdSV!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("非法的加密机指令类型");
								return(errCodeParameter);
						}
						// 检查校验值
						if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
							return(errCodeEssc_CheckValue);
						}

						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
						return(errCodeEsscMDL_InvalidAlgorithmID);
				}			
			}
			else if(isRemoteApplyKey == 0)		// 本地生成
			{
				switch(symmetricKeyDB.algorithmID)
				{
					case	conSymmetricAlgorithmIDOfDES:	// DES算法
						if (((exportFlag == 1) && (zmkKeyDB.keyType == conTMK)) || (exportFlag == 3) || (exportFlag == 9 && symmetricKeyDB.keyType == conTMK))
							strcpy(tmpBuf,"0");
						else
							sprintf(tmpBuf,"%d",outputByZMK);

						if ((ret =  UnionHsmCmdA0(atoi(tmpBuf),symmetricKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen),zmk,psymmetricKeyValue->keyValue,keyByZMK,symmetricKeyDB.checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdA0!\n");
							return(ret);
						}

						if (((exportFlag == 1) && (zmkKeyDB.keyType == conTMK)) || (exportFlag == 3) || (exportFlag == 9))
						{
							// TMK保护输出
							if (symmetricKeyDB.keyType == conTMK || symmetricKeyDB.keyType == conTPK || symmetricKeyDB.keyType == conPVK)
							{
								if ((ret = UnionHsmCmdAE(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdAE!\n");
									return(ret);
								}
								strcpy(keyByZMK,keyByTMK);
							}
							else if (symmetricKeyDB.keyType == conTAK)
							{
								if ((ret = UnionHsmCmdAG(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdAG!\n");
									return(ret);
								}
								strcpy(keyByZMK,keyByTMK);
							}
						}
		
						if (isApplyFlag) // 远程分发
						{
							// 把本地LMK对加密转成远程ZMK加密
							switch(phsmGroupRec->hsmCmdVersionID)
							{
								case conHsmCmdVerRacalStandardHsmCmd:
								case conHsmCmdVerSJL06StandardHsmCmd:
									if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,platformProtectKey,keyByZMK,remoteCheckValue)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key UnionHsmCmdA8!\n");
										return(ret);
									}
									break;
								default:
									UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
									UnionSetResponseRemark("非法的加密机指令类型");
									return(errCodeParameter);
							}
							// 检查校验值
							if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
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
								if ((ret = UnionHsmCmdWI(tmpBuf,symmetricKeyDB.keyType,zmk,psymmetricKeyValue->keyValue,keyByZMK,symmetricKeyDB.checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdWI!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("非法的加密机指令类型");
								return(errCodeParameter);
						}
						if (isApplyFlag) // 远程分发
						{
							
							// 把本地LMK对加密转成远程ZMK加密
							switch(phsmGroupRec->hsmCmdVersionID)
							{
								case	conHsmCmdVerRacalStandardHsmCmd:
								case	conHsmCmdVerSJL06StandardHsmCmd:
									if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,platformProtectKey,keyByZMK,remoteCheckValue)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key UnionHsmCmdST!\n");
										return(ret);
									}
									break;
								default:
									UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
									UnionSetResponseRemark("非法的加密机指令类型");
									return(errCodeParameter);
							}
							// 检查校验值
							if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
								return(errCodeEssc_CheckValue);
							}					
							break;
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
						return(errCodeEsscMDL_InvalidAlgorithmID);
				}
			}
			break;
		case 2:
		default:
			UnionUserErrLog("in UnionDealServiceCodeE111:: mode = [%d]!\n",mode);
			return(errCodeEsscMDLKeyOperationNotPermitted);
	}

	if (isRemoteDistributeKey)	// 远程分发密钥
	{
		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// 设置申请标志 2:分发
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/remoteOperateFlag","2")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/remoteOperateFlag","2");
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
							if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key UnionHsmCmdA8!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					// 检查校验值
					if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
						return(errCodeEssc_CheckValue);
					}					
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
					// 把本地LMK对加密转成远程ZMK加密
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key UnionHsmCmdST!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					// 检查校验值
					if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
						return(errCodeEssc_CheckValue);
					}					
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}			
			if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyValue",keyByRKM)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyValue",keyByRKM);
				return(ret);
			}
			if ((ret = UnionSetRequestRemoteXMLPackageValue("body/checkValue",remoteCheckValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/checkValue",remoteCheckValue);
				return(ret);
			}
		}
		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}
	//add by zhouxw 	20150520
	if (isRemoteKeyOperate)         // 远程密钥操作
        {
                if ((ret = UnionReadRemoteSysIDAndAppID(sysID,sizeof(sysID),appID,sizeof(appID))) < 0)
                {       
                        UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRemoteSysIDAndAppID!\n");
                        return(ret);
                }
        }
        else                            // 本地密钥操作
        {       
                if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
                {       
                        UnionUserErrLog("in UnionDealServiceCodeE119:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");            
                        return(ret);
                }
                if ((ret = UnionReadRequestXMLPackageValue("head/appID",appID,sizeof(appID))) < 0)
                {       
                        UnionUserErrLog("in UnionDealServiceCodeE119:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");            
                        return(ret);
                }
        }
	
	
	// 设置创建者
        UnionPieceSymmetricKeyCreator(sysID,appID,creator);
	
	//if(((symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfApp) && ((strcmp(symmetricKeyDB.creator,creator) == 0) || (strcmp(symmetricKeyDB.creator,"APP") == 0))) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfUser) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfKMS) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfMove))

	if(((symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfApp) && (strcmp(symmetricKeyDB.creator,creator) == 0)) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfUser) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfKMS) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfMove) || (strcmp(symmetricKeyDB.creator,"APP") == 0))
	{
	//add end 20150520
		if ((ret =  UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionUpdateSymmetricKeyDBKeyValue,keyName[%s]!\n",symmetricKeyDB.keyName);
			return(ret);
		}
	//add by zhouxw 20150520
	}
	else
	{
		UnionSetResponseRemark("密钥[%s]非此创建者[%s]创建,不允许更新密钥!",keyName,creator);
                UnionUserErrLog("in UnionDealServiceCodeE111:: 密钥[%s]非此创建者[%s]创建,不允许更新密钥�!",keyName,creator);
		return(errCodeEsscMDLKeyOperationNotPermitted);
	}
	//add end 20150520

	// modify by leipp 20160125	修复向远程分发密钥不需要返回密钥与校验值,增加[isRemoteKeyOperate == 0]
	if ((exportFlag != 0 && isRemoteKeyOperate == 0) || (isApplyFlag == 1))
	{
		if ((ret = UnionSetResponseXMLPackageValue("body/keyValue",keyByZMK)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyValue");
			return(ret);
		}
		if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",symmetricKeyDB.checkValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetResponseXMLPackageValue[%s]!\n","body/checkValue");
			return(ret);
		}
	}
	// modify end
	if (exportLmkKey[0] == '1' && !isRemoteKeyOperate)
	{
		if ((ret = UnionSetResponseXMLPackageValue("body/keyValue2",psymmetricKeyValue->keyValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyValue2");
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",symmetricKeyDB.checkValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetResponseXMLPackageValue[%s]!\n","body/checkValue");
			return(ret);
		}
	}
	
	return 0;
}

