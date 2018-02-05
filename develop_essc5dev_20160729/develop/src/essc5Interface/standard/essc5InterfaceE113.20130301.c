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
#include "unionHsmCmdVersion.h"

/***************************************
服务代码:	E113
服务名:		导出对称密钥
功能描述:	导出对称密钥
***************************************/
int UnionDealServiceCodeE113(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				len = 0;
	char				sql[256];
	char				protectFlag[32];
	char				protectKey[128];
	char				zmk[64];
	char				tmk[64];
	char				keyByZMK[64];
	char				keyByTMK[64];
	char				checkValue[32];
	char				exportLmkKey[32];
	char				keyName[160];

	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;
	TUnionSymmetricKeyDB		zmkKeyDB;
	PUnionSymmetricKeyValue		pzmkKeyValue = NULL;
	
	// 密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);

	// 读取对称密钥
	UnionInitSymmetricKeyDB(&symmetricKeyDB);
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1,&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
		return(ret);
	}

	// 读取密钥值
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
		UnionSetResponseRemark("读取密钥值失败");
		return(errCodeParameter);
	}

	if (symmetricKeyDB.status != conSymmetricKeyStatusOfEnabled)	// 非启用状态
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: symmetricKeyDB.status[%d]!\n",symmetricKeyDB.status);
		return(errCodeEsscMDL_KeyStatusDisabled);
	}
	
	if (!symmetricKeyDB.outputFlag)		// 不允许导出
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: symmetricKeyDB.outputFlag[%d]!\n",symmetricKeyDB.outputFlag);
		return(errCodeEsscMDL_KeyOutputNotPermitted);
	}

	exportLmkKey[0] = 0;
	ret = UnionReadRequestXMLPackageValue("body/exportLmkKey",exportLmkKey,sizeof(exportLmkKey));
	exportLmkKey[ret] = 0;
	
	if ((ret = UnionReadRequestXMLPackageValue("body/protectFlag",protectFlag,sizeof(protectFlag))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: UnionReadRequestXMLPackageValue[%s]!\n","body/protectFlag");
		return(ret);
	}
	protectFlag[ret] = 0;
	
	if (protectFlag[0] != '4' && protectFlag[0] != '9')
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/protectKey",protectKey,sizeof(protectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: UnionReadRequestXMLPackageValue[%s]!\n","body/protectKey");
			return(ret);	
		}
		protectKey[ret] = 0;
	}

	// 读取保护密钥
	// 1：指定密钥名称
	// 2：外带ZMK密钥保护
	// 3: 外带TMK密钥保护
	// 4：根据产生密钥的单位，查找保护密钥
	// 9：旧密钥值保护新的密钥值
	if (protectFlag[0] == '1')	// 指定密钥名称
	{
		if ((ret =  UnionReadSymmetricKeyDBRec(protectKey,1,&zmkKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: UnionReadSymmetricKeyDBRec protectKey[%s]!\n",protectKey);
			return(ret);
		}

		if ((zmkKeyDB.keyType != conZMK) && (zmkKeyDB.keyType != conTMK))
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: is not zmk or tmk [%s]!\n",protectKey);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}

		if ((pzmkKeyValue = UnionGetSymmetricKeyValue(&zmkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
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

			snprintf(tmk,sizeof(tmk),"%s",pzmkKeyValue->keyValue);
		}
		else
		{
			if (strcmp(protectKey,keyName) == 0)
			{
				if ((pzmkKeyValue->oldKeyValue == NULL) || (strlen(pzmkKeyValue->oldKeyValue) == 0))
				{
					UnionUserErrLog("in UnionDealServiceCodeE111:: keyName[%s] old zmk not found!\n",symmetricKeyDB.keyName);
					UnionSetResponseRemark("密钥名称[%s]，旧密钥不存在",symmetricKeyDB.keyName);
					return(errCodeParameter);
				}
				else
					snprintf(zmk,sizeof(zmk),"%s",pzmkKeyValue->oldKeyValue);
			}
			else
				snprintf(zmk,sizeof(zmk),"%s",pzmkKeyValue->keyValue);
		}
	}
	else if (protectFlag[0] == '2' || protectFlag[0] == '3')	// 外带密钥保护
	{
		len = strlen(protectKey);
		if ((len != 16) && (len != 32) && (len != 48))
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: protectKey[%s] len != [16,32,48]\n",protectKey);
			return(errCodeEssc_KeyLength);
		}
		if (protectFlag[0] == '3')
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
			snprintf(tmk,sizeof(tmk),"%s",protectKey);
		}
		else
			snprintf(zmk,sizeof(zmk),"%s",protectKey);
	}
	else if (protectFlag[0] == '4')		// 约定的保护密钥
	{
		//获取保护密钥
		snprintf(sql,sizeof(sql),"select protectKey from localKeyPlatform where sysID = '%s' and algorithmID = %d",symmetricKeyDB.usingUnit,symmetricKeyDB.algorithmID);

		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: UnionSelectRealDBRecord[%s]\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: remoteProtectKey not found!\n");
			UnionSetResponseRemark("保护密钥不存在");
			return(errCodeParameter);
		}

		if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: UnionLocateXMLPackage[%s]!\n","detail");	
			return(ret);
		}

		// 读取平台保护密钥
		if ((ret = UnionReadXMLPackageValue("protectKey",protectKey,sizeof(protectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: UnionReadXMLPackageValue[%s]!\n","protectKey");
			return(ret);
		}
		protectKey[ret] = 0;

		snprintf(zmk,sizeof(zmk),"%s",protectKey);
	}
	else if (protectFlag[0] == '9')	// 9：旧密钥值保护新的密钥值
	{
		if (psymmetricKeyValue->oldKeyValue == NULL || strlen(psymmetricKeyValue->oldKeyValue) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: keyName[%s] oldKeyValue not found!\n",symmetricKeyDB.keyName);
			UnionSetResponseRemark("旧密钥值不存在");
			return(errCodeParameter);
		}
		snprintf(zmk,sizeof(zmk),"%s",psymmetricKeyValue->oldKeyValue);
	}
	else
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: protectFlag[%s] error!\n",protectFlag);
		return(errCodeParameter);
	}
	
	switch(symmetricKeyDB.algorithmID)
	{
		case	conSymmetricAlgorithmIDOfDES:	// DES算法
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					if (((protectFlag[0] == '1') && (zmkKeyDB.keyType == conTMK)) || (protectFlag[0] == '3') || (protectFlag[0] == '9' && symmetricKeyDB.keyType == conTMK))
					{
						// TMK保护输出
						if (symmetricKeyDB.keyType == conTMK || symmetricKeyDB.keyType == conTPK || symmetricKeyDB.keyType == conPVK)
						{
							if ((ret = UnionHsmCmdAE(tmk,psymmetricKeyValue->keyValue,keyByTMK,checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE113:: UnionHsmCmdAE!\n");
								return(ret);
							}
						}
						else if (symmetricKeyDB.keyType == conTAK)
						{
							if ((ret = UnionHsmCmdAG(tmk,psymmetricKeyValue->keyValue,keyByTMK,checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE113:: UnionHsmCmdAG!\n");
								return(ret);
							}
						}
						snprintf(checkValue,sizeof(checkValue),"%s",symmetricKeyDB.checkValue);
						snprintf(keyByZMK,sizeof(keyByZMK),"%s",keyByTMK);
					}
					else
					{
						if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,zmk,keyByZMK,checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE113:: UnionHsmCmdA8!\n");
							return(ret);
						}
					}

					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE113: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}
			break;
		case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,zmk,keyByZMK,checkValue)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE113:: UnionHsmCmdST!\n");
						return(ret);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE113: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE113: symmetricKeyDB.algorithmID[%d] is invalid\n",symmetricKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	// 检查校验值
	if (memcmp(checkValue,symmetricKeyDB.checkValue,strlen(symmetricKeyDB.checkValue)) != 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: checkValue[%s] != symmetricKeyDB.checkValue[%s]\n",checkValue,symmetricKeyDB.checkValue);
		return(errCodeEssc_CheckValue);
	}
	
	if ((ret = UnionSetResponseXMLPackageValue("body/keyValue",keyByZMK)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyValue");
		return(ret);
	}

	if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",checkValue)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: UnionSetResponseXMLPackageValue[%s]!\n","body/checkValue");
		return(ret);
	}

	if (exportLmkKey[0] == '1')
	{
		if ((ret = UnionSetResponseXMLPackageValue("body/keyValue2",psymmetricKeyValue->keyValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyValue2");
			return(ret);
		}
	}

	if (protectFlag[0] == '4')
	{
		if ((ret = UnionSetResponseXMLPackageValue("body/usingUnit",symmetricKeyDB.usingUnit)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: UnionSetResponseXMLPackageValue[%s]!\n","body/usingUnit");
			return(ret);
		}
	}

	return(0);
}
