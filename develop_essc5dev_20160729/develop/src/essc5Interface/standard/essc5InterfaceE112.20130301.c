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
服务代码:	E112
服务名:		导入对称密钥
功能描述:	导入对称密钥
***************************************/
int UnionDealServiceCodeE112(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				len = 0;
	char				protectFlag[32];
	char				protectKey[128];
	char				zmk[64];
	char				keyByLMK[64];
	char				keyByRKM[64];
	char				keyValue[64];
	char				checkValue[32];
	char				localCheckValue[32];
	char				remoteCheckValue[32];
	int				isRemoteKeyOperate = 0;
	int				isRemoteDistributeKey = 0;
	char				platformProtectKey[64];
	char				sql[128];
	char				tmpBuf[128];
	int				lenOfKey = 0;
	char				keyName[160];
	int				flag = 0;
	
	char				appID[32];
	char				keyNode[80];
	char				keyType[16];
	char				keyVer[16];
	int 				version = 0;
	char				activeDate[16];

	int				checkValueFlag = 0;//add by chenwd 20150521 标记是否需要返回校验值

	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;
	TUnionSymmetricKeyDB		zmkKeyDB;
	PUnionSymmetricKeyValue		pzmkKeyValue = NULL;
	TUnionRemoteKeyPlatform		tkeyDistributePlatform;

	// 检测是否远程操作
	isRemoteKeyOperate = UnionIsRemoteKeyOperate();
	if (isRemoteKeyOperate)
		flag = 1;
	
	// 密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(keyName);
	
	//密钥版本号 目前仅kms2发的旧报文转成XML报文需要带有version
        if((ret = UnionReadRequestXMLPackageValue("body/version",tmpBuf, sizeof(tmpBuf))) > 0)
        {
                tmpBuf[ret] = 0;
		if(3 < strlen(tmpBuf))
                {
                        UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadRecFldFromRecStr currentKeyVer[%s] len > 3\n", tmpBuf);
                        return(errCodeObjectMDL_FieldValueTooLong);
                }
                else if(3 >= strlen(tmpBuf))
                {
                        memcpy(keyVer, "-000", 4-strlen(tmpBuf));
                        keyVer[4-strlen(tmpBuf)] = 0;
                        strcat(keyVer, tmpBuf);
                }
                version = atoi(tmpBuf);
		if(version >= 1)
		{
			if((ret = UnionReadRequestXMLPackageValue("body/sysID", tmpBuf, sizeof(tmpBuf))) <= 0)
        	        {
        	                UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]\n","body/sysID");
        	                return(ret);
        	        }
                	tmpBuf[ret] = 0;
                	if((ret = UnionSetRequestXMLPackageValue("head/sysID", tmpBuf)) < 0)
                	{
                	       UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s][%s]\n","head/sysID",tmpBuf);
                	       return(ret);
               	 	}
		
        	        if((ret = UnionReadRequestXMLPackageValue("body/appID", tmpBuf, sizeof(tmpBuf))) <= 0)
        	        {
        	                UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]\n","body/appID");
        	                return(ret);
        	        }
                	tmpBuf[ret] = 0;
               	 	if((ret = UnionSetRequestXMLPackageValue("head/appID", tmpBuf)) < 0)
                	{	
                	        UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s][%s]\n","head/appID",tmpBuf);
                	        return(ret);
                	}
			
			if((ret = UnionAnalysisSymmetricKeyName(keyName, appID, keyNode, keyType)) < 0)
                	{
                	        UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionAnalysisSymmetricKeyName keyName[%s]\n", keyName);
                	        return(ret);
                	}
                	strcat(keyNode, keyVer);
                	memset(keyName, 0, sizeof(keyName));
                	memcpy(keyName, appID, strlen(appID));
                	strcat(keyName, ".");
                	strcat(keyName, keyNode);
                	strcat(keyName, ".");
                	strcat(keyName, keyType);
			if((ret = UnionSetRequestXMLPackageValue("body/keyName", keyName)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s]\n", "body/keyName");
				return(ret);
			}
                	      
                	UnionGetFullSystemDate(activeDate);
                	if((ret = UnionSetRequestXMLPackageValue("body/activeDate", activeDate)) < 0)
                	{
                	        UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s]\n", "body/activeDate");
                	        return(ret);
                	}
	
	                if((ret = UnionSetRequestXMLPackageValue("body/effectiveDays" , "36500")) < 0)
	                {
	                        UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s]\n","body/effectiveDays");
	                        return(ret);
	                }
	
	                if((ret = UnionSetRequestXMLPackageValue("body/keyGroup", "default")) < 0)
	                {
	                        UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s]\n","body/keyGroup");
	                        return(ret);
	                }
	
	                if((ret = UnionSetRequestXMLPackageValue("body/inputFlag", "1")) < 0)
	                {
	                        UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s]\n","body/inputFlag");
	                        return(ret);
	                }
	                      
	                if((ret = UnionSetRequestXMLPackageValue("body/outputFlag", "1")) < 0)
	                {
	                        UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s]\n","body/outputFlag");
	                        return(ret);
	                }
	
	                if((ret = UnionSetRequestXMLPackageValue("body/oldVersionKeyIsUsed", "1")) < 0)
	                {
	                        UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s]\n","body/oldVersionKeyIsUsed");
	                        return(ret);
	                }
	
	                if((ret = UnionSetRequestXMLPackageValue("body/mode", "0")) < 0)
	                {
	                        UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s]\n","body/mode");
	                        return(ret);
	                }
		}
		else
                {
                        UnionUserErrLog("in UnionDealServiceCodeE110:: wrong version[%d]\n", version);
                        UnionSetResponseRemark("密钥版本号错误");
                        return(errCodeParameter);
                }
	}
		

	// 读取对称密钥
	UnionInitSymmetricKeyDB(&symmetricKeyDB);
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
	{
		if(version >= 1)
		{
			//E110也读version并拼装版本号到密钥名称，避免重复拼装将version设置为空
			if((ret = UnionSetRequestXMLPackageValue("body/version", "")) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE110:: UnionSetRequestXMLPackageValue[%s]\n","body/version");
				return(ret);
			}			

			if ((ret = UnionDealServiceCodeE110(phsmGroupRec)) < 0)
                        {
                                UnionUserErrLog("in UnionDealServiceCodeE112:: UnionDealServiceCodeE110!\n");
                                return(ret);
                        }
			
			if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
			{
				if(errCodeKeyCacheMDL_WrongKeyName == ret)
				{
					UnionUserErrLog("in UnionDealServiceCodeE112:: 对称密钥[%s]不存在!\n",keyName);
                        		UnionSetResponseRemark("对称密钥[%s]不存在",keyName);
                        		return(errCodeKeyCacheMDL_KeyNonExists);
				}
				else
				{
					UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadSymmetricKeyDBRec[%s] after insert\n", keyName);
					return(ret);
				}
			}
			
		}
		else if (ret == errCodeKeyCacheMDL_WrongKeyName)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: 对称密钥[%s]不存在!\n",keyName);
			UnionSetResponseRemark("对称密钥[%s]不存在",keyName);
			return(errCodeKeyCacheMDL_KeyNonExists);
		}
		else
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
			return(ret);
		}
	}
	
	// 读取密钥值
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) != NULL)
	{
		// 当前密钥置为旧密钥
		strcpy(psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue);
		strcpy(symmetricKeyDB.oldCheckValue,symmetricKeyDB.checkValue);

		psymmetricKeyValue->keyValue[0] = 0;
		symmetricKeyDB.checkValue[0] = 0;
	}
	else
		psymmetricKeyValue = &symmetricKeyDB.keyValue[0];
		

	strcpy(psymmetricKeyValue->lmkProtectMode,phsmGroupRec->lmkProtectMode);

	if (0 == flag)
	{
		// 为兼容旧系统设定的检查标识
		if ((ret = UnionReadRequestXMLPackageValue("body/sysFlag",tmpBuf,sizeof(tmpBuf))) > 0)
			flag = 1;
	}
	// 检查密钥是否可以使用
	if ((ret = UnionIsUseCheckSymmetricKeyDB(symmetricKeyDB.status,flag)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE112:: UnionIsUseCheckSymmetricKeyDB symmetricKeyDB.status[%d] flag[%d]!\n",symmetricKeyDB.status,flag);
		return(ret);
	}

	if (!symmetricKeyDB.inputFlag)		// 不允许导入
	{
		UnionUserErrLog("in UnionDealServiceCodeE112:: symmetricKeyDB.inputFlag[%d]!\n",symmetricKeyDB.inputFlag);
		UnionSetResponseRemark("密钥不允许导入");
		return(errCodeEsscMDL_KeyInputNotPermitted);
	}
	
	// 读取密钥密文
	if ((ret = UnionReadRequestXMLPackageValue("body/keyValue",keyValue,sizeof(keyValue))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyValue");
		UnionSetResponseRemark("读取密钥值失败");
		return(ret);
	}
	else
        {
		UnionFilterHeadAndTailBlank(keyValue);
		lenOfKey = strlen(keyValue);
		if (lenOfKey  == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: keyValue can not be null!\n");
			UnionSetResponseRemark("密钥值不能为空");
			return(errCodeParameter);
		}
		if ((lenOfKey * 4) != symmetricKeyDB.keyLen)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: new keyLen[%d] != current keyLen[%d]!\n",lenOfKey,symmetricKeyDB.keyLen/4);
			UnionSetResponseRemark("密钥长度非法，密钥长度必须是[%d]",symmetricKeyDB.keyLen/4);
			return(errCodeParameter);
		}
                if (!UnionIsBCDStr(keyValue))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE112:: keyValue[%s] is error!\n",keyValue);
                        UnionSetResponseRemark("读取密钥密文非法,必须为十六进制数");
                        return(errCodeParameter);
                }
        }
		
	// 读取检验值
	if ((ret = UnionReadRequestXMLPackageValue("body/checkValue",checkValue,sizeof(checkValue))) <= 0)
	{
		checkValue[0] = 0;
		checkValueFlag = 1;//add by chenwd 20150521 设置返回校验值
	}
 	else
        {
		UnionFilterHeadAndTailBlank(checkValue);
                if ((!UnionIsBCDStr(checkValue)) || (strlen(checkValue) < 4))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE112:: checkValue[%s] is error!\n",checkValue);
                        UnionSetResponseRemark("读取检验值非法,必须为十六进制数,且长度必须大于等于4");
                        return(errCodeParameter);
                }
        }

	if ((ret = UnionReadRequestXMLPackageValue("body/protectFlag",protectFlag,sizeof(protectFlag))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]!\n","body/protectFlag");
		return(ret);
	}
	
	if ((protectFlag[0] == '1') || (protectFlag[0] == '2'))
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/protectKey",protectKey,sizeof(protectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]!\n","body/protectKey");
			return(ret);	
		}
	}
	
	// 读取保护密钥
	if (protectFlag[0] == '1')	// 指定密钥名称
	{
		if ((ret =  UnionReadSymmetricKeyDBRec(protectKey,1,&zmkKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadSymmetricKeyDBRec protectKey[%s]!\n",protectKey);
			return(ret);
		}
		if (zmkKeyDB.keyType != conZMK)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: is not zmk [%s]!\n",protectKey);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}
		if ((pzmkKeyValue = UnionGetSymmetricKeyValue(&zmkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("读取密钥值失败");
			return(errCodeParameter);
		}
		strcpy(zmk,pzmkKeyValue->keyValue);
	}
	else if (protectFlag[0] == '2')	// 外带ZMK密钥保护
	{
		len = strlen(protectKey);
		if ((len != 16) && (len != 32) && (len != 48))
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: protectKey[%s] len != [16,32,48]\n",protectKey);
			return(errCodeEssc_KeyLength);
		}
		strcpy(zmk,protectKey);
	}
	else if (protectFlag[0] == '3')	// 外带LMK对密钥保护
	{
		//
	}
	else if (protectFlag[0] == '4')	// 根据sysID获取保护密钥
	{
		// 系统ID
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);
		}

		//获取保护密钥
		snprintf(sql,sizeof(sql),"select protectKey from localKeyPlatform where sysID = '%s' and algorithmID = %d",tmpBuf,symmetricKeyDB.algorithmID);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSelectRealDBRecord[%s]\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: platformProtectKey not found!\n");
			UnionSetResponseRemark("远程平台保护密钥不存在");
			return(errCodeParameter);
		}

		if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionLocateXMLPackage[%s]!\n","detail");	
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("protectKey",platformProtectKey,sizeof(platformProtectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadXMLPackageValue[%s]!\n","protectKey");
			return(ret);
		}

		snprintf(zmk,sizeof(zmk),"%s",platformProtectKey);
	}
	else
	{
		UnionUserErrLog("in UnionDealServiceCodeE112:: protectFlag[%s] error!\n",protectFlag);
		return(errCodeParameter);
	}

	if (!isRemoteKeyOperate)  // 本地操作
	{
		// modify by leipp 20151209
		// 读取并检查申请和分发平台
		if ((ret = UnionCheckRemoteKeyPlatform(symmetricKeyDB.keyName, NULL, symmetricKeyDB.keyDistributePlatform, NULL, &tkeyDistributePlatform, NULL, &isRemoteDistributeKey)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionCheckRemoteKeyPlatform!\n");
			return(ret);
		}
		// modify end

		memset(keyByLMK,0,sizeof(keyByLMK));
		memset(localCheckValue,0,sizeof(localCheckValue));
		if ((protectFlag[0] == '1') ||		// 指定密钥名称
			(protectFlag[0] == '2') ||	// 外带ZMK密钥保护
			(protectFlag[0] == '4'))	// 根据sysID，获取本地保护密钥
		{
		
			memset(keyByRKM,0,sizeof(keyByRKM));
			memset(remoteCheckValue,0,sizeof(remoteCheckValue));
			switch(symmetricKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,zmk,keyValue,keyByLMK,localCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE112:: UnionHsmCmdA6!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE112:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,zmk,keyValue,keyByLMK,localCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE112:: UnionHsmCmdA6!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE112:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE112:: Distribute Key algorithmID[%d] is error!\n",symmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
		}
		else
		{
			// 计算密钥校验值
			switch(symmetricKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:
					UnionSetIsUseNormalZmkType();
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdBU(0,symmetricKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen),keyValue,localCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE112:: UnionHsmCmdBU!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE112:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
					UnionSetIsUseNormalZmkType();
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdBU(1,symmetricKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen),keyValue,localCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE112:: UnionHsmCmdBU!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE112:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE112:: Distribute Key algorithmID[%d] is error!\n",symmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
			strcpy(keyByLMK,keyValue);
		}

		// 检查校验值
		if (memcmp(localCheckValue,checkValue,strlen(checkValue)) != 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: localCheckValue[%s] != checkValue[%s]\n",localCheckValue,checkValue);
			return(errCodeEssc_CheckValue);
		}
	}
	else	// 远程操作
	{
		// 系统ID
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);
		}

		//获取保护密钥
		snprintf(sql,sizeof(sql),"select protectKey from localKeyPlatform where sysID = '%s' and algorithmID = %d",tmpBuf,symmetricKeyDB.algorithmID);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSelectRealDBRecord[%s]\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: platformProtectKey not found!\n");
			UnionSetResponseRemark("远程平台保护密钥不存在");
			return(errCodeParameter);
		}

		if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionLocateXMLPackage[%s]!\n","detail");	
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("protectKey",platformProtectKey,sizeof(platformProtectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadXMLPackageValue[%s]!\n","protectKey");
			return(ret);
		}

		// 读取远程密钥
		if ((ret = UnionReadRequestXMLPackageValue("body/remoteKeyValue",keyValue,sizeof(keyValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]!\n","body/remoteKeyValue");
			return(ret);
		}
	
		// 读取远程检验值
		if ((ret = UnionReadRequestXMLPackageValue("body/remoteCheckValue",checkValue,sizeof(checkValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]!\n","body/remoteCheckValue");
			return(ret);
		}

		memset(keyByLMK,0,sizeof(keyByLMK));
		memset(localCheckValue,0,sizeof(localCheckValue));
		switch(symmetricKeyDB.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,platformProtectKey,keyValue,keyByLMK,localCheckValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE112:: UnionHsmCmdA6!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE112:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}

				break;
			case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,platformProtectKey,keyValue,keyByLMK,localCheckValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE112:: UnionHsmCmdA6!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE112:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}

				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE112:: Distribute Key algorithmID[%d] is error!\n",symmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
		// 检查校验值
		if (memcmp(localCheckValue,checkValue,strlen(checkValue)) != 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: localCheckValue[%s] != checkValue[%s]\n",localCheckValue,checkValue);
			return(errCodeEssc_CheckValue);
		}
	}

	if (isRemoteDistributeKey)	// 本地操作
	{
		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		memset(keyByRKM,0,sizeof(keyByRKM));
		memset(remoteCheckValue,0,sizeof(remoteCheckValue));
		switch(symmetricKeyDB.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:
				// 把本地LMK对加密转成远程ZMK加密
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,keyByLMK,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE112:: Distribute Key UnionHsmCmdA8!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE112:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
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
						if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,keyByLMK,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE112:: Distribute Key UnionHsmCmdST!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE112:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE112:: Distribute Key algorithmID[%d] is error!\n",symmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}

		// 检查校验值
		if (memcmp(localCheckValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: Distribute Key localCheckValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
			return(errCodeEssc_CheckValue);
		}

		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/remoteKeyValue",keyByRKM)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyValue",keyByRKM);
			return(ret);
		}

		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/remoteCheckValue",remoteCheckValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/checkValue",remoteCheckValue);
			return(ret);
		}
		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	// 当前密钥置为旧密钥

	strcpy(psymmetricKeyValue->keyValue,keyByLMK);
	if (strlen(checkValue) != 16)
		strcpy(symmetricKeyDB.checkValue,localCheckValue);
	else
		strcpy(symmetricKeyDB.checkValue,checkValue);

	// 生效时间
	if ((ret = UnionReadRequestXMLPackageValue("body/activeDate",symmetricKeyDB.activeDate,sizeof(symmetricKeyDB.activeDate))) <= 0)
		UnionGetFullSystemDate(symmetricKeyDB.activeDate);

	UnionFilterHeadAndTailBlank(symmetricKeyDB.activeDate);
	if (!UnionIsValidFullDateStr(symmetricKeyDB.activeDate))
        {
                UnionUserErrLog("in UnionDealServiceCodeE112:: activeDate[%s] error!\n",symmetricKeyDB.activeDate);
                UnionSetResponseRemark("生效日期格式[%s]非法,必须为YYYYMMDD格式",symmetricKeyDB.activeDate);
                return(errCodeParameter);
        }
	if ((ret =  UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE112:: UnionUpdateSymmetricKeyDBKeyValue,keyName[%s]!\n",symmetricKeyDB.keyName);
		return(ret);	
	}
	
	// add by chenwd 20150521 如果没送校验值则返回校验值
	if (checkValueFlag == 1)
	{
		if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",symmetricKeyDB.checkValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetResponseXMLPackageValue[%s]!\n","body/checkValue");
			return(ret);
		}
	}
	// add by chenwd end
	
	return(0);
}


