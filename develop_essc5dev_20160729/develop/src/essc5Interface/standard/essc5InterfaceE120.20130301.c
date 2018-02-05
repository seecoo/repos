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
#include "asymmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "commWithHsmSvr.h"
#include "unionHsmCmdVersion.h"
#include "remoteKeyPlatform.h"

/***************************************
服务代码:	E120
服务名:		生成非对称密钥
功能描述:	生成非对称密钥
***************************************/
int UnionDealServiceCodeE120(PUnionHsmGroupRec phsmGroupRec)
{
	int				i;
	int				ret;
	int				len = 0;
	int				hsmNum = 0;
	int				mode = 1;
	int				exportFlag = 0;
	int				enabled = 0;
	int				lenOfPKExp = 0;
	int				lenOfVK = 0;
	int				lenOfPK = 0;
	int				isDistributeFlag = 0;
	int				isApplyFlag = 0;
	int				isRemoteApplyKey = 0;
	int				isRemoteDistributeKey = 0;
	int				isRemoteKeyOperate = 0;
	int                         	addLen = 0;
	unsigned char			pkexp[32];
	char				pkValue[512];
	char				vkValue[2560];
	char				tmpBuf[2560];
	char				keyType[32];
	char				ipAddr[64];
	char				sql[1024];
	char				sysID[32];
	char				appID[32];
	char				platformProtectKey[64];
	char				platformCheckValue[32];
	char				iv[32];
	char				remoteVK[2560];
	char				vkByKek[2560];
	char				updateKeyFlag[32];
	char				keyName[160];
	char				*ptr = NULL;
	char				vkIndex[8];

	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	TUnionRemoteKeyPlatform		tkeyApplyPlatform;
	TUnionRemoteKeyPlatform		tkeyDistributePlatform;

	memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));
	// 检测远程标识
	isRemoteKeyOperate = UnionIsRemoteKeyOperate();

	// 防止乱码
	asymmetricKeyDB.hsmGroupID[0] = 0;
	asymmetricKeyDB.vkIndex[0] = 0;
	asymmetricKeyDB.pkCheckValue[0] = 0;
	asymmetricKeyDB.vkCheckValue[0] = 0;
	asymmetricKeyDB.oldPKValue[0] = 0;
	asymmetricKeyDB.oldPKCheckValue[0] = 0;
	asymmetricKeyDB.oldVKValue[0] = 0;
	asymmetricKeyDB.oldVKCheckValue[0] = 0;
	asymmetricKeyDB.remark[0] = 0;

	// 密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);
	if (strlen(keyName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE120:: keyName can not be null!\n");
		UnionSetResponseRemark("密钥名称不能为空");	
		return(ret);
	}

	// 读取更新密钥标识
	if ((ret = UnionReadRequestXMLPackageValue("body/updateKeyFlag",updateKeyFlag,sizeof(updateKeyFlag))) < 0)
		strcpy(updateKeyFlag,"0");

	// 检查密钥是否存在
	if ((ret = UnionReadAsymmetricKeyDBRec(keyName,0,&asymmetricKeyDB)) != errCodeKeyCacheMDL_WrongKeyName)
	{
		if (updateKeyFlag[0] == '1')	// 密钥存在,且更新标识为1,则调用E121服务更新密钥
		{
			if ((ret = UnionDealServiceCodeE121(phsmGroupRec)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE120:: UnionDealServiceCodeE121!\n");
				return(ret);
			}
			return 0;
		}
		else
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: keyName[%s] is already exists!\n",keyName);
			UnionSetResponseRemark("非对称密钥[%s]已存在",keyName);
			return(errCodeKeyCacheMDL_KeyAlreadyExists);
		}
	}	

	// 密钥组
	if ((ret = UnionReadRequestXMLPackageValue("body/keyGroup",asymmetricKeyDB.keyGroup,sizeof(asymmetricKeyDB.keyGroup))) <= 0)
	{
		strcpy(asymmetricKeyDB.keyGroup,"default");
	}

	// 算法标识 
	if ((ret = UnionReadRequestXMLPackageValue("body/algorithmID",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadRequestXMLPackageValue[%s]!\n","body/algorithmID");
		return(ret);	
	}
	if ((asymmetricKeyDB.algorithmID = UnionConvertAsymmetricKeyAlgorithmID(tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE120:: algorithmID[%s] error!\n",tmpBuf);
		return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	// 密钥类型
	if ((ret = UnionReadRequestXMLPackageValue("body/keyType",keyType,sizeof(keyType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyType");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(keyType);
	if (atoi(keyType) != 0 && atoi(keyType) != 1 && atoi(keyType) != 2)
	{
		UnionUserErrLog("in UnionDealServiceCodeE120:: keyType[%s] error!\n",keyType);
		UnionSetResponseRemark("密钥类型[%s]无效",keyType);
		return(errCodeParameter);
	}
	asymmetricKeyDB.keyType = atoi(keyType);
	
	// 公钥指数
	if (asymmetricKeyDB.algorithmID == conAsymmetricAlgorithmIDOfRSA)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/pkExponent",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkExponent");
			return(ret);	
		}
		asymmetricKeyDB.pkExponent = atoi(tmpBuf);
		if (asymmetricKeyDB.pkExponent < 256)
			sprintf(tmpBuf,"%02X",asymmetricKeyDB.pkExponent);
		else if (asymmetricKeyDB.pkExponent < 256 * 256)
			sprintf(tmpBuf,"%04X",asymmetricKeyDB.pkExponent);
		else
			sprintf(tmpBuf,"%06X",asymmetricKeyDB.pkExponent);
		
		lenOfPKExp = aschex_to_bcdhex(tmpBuf,strlen(tmpBuf),(char *)pkexp);

		// 密钥长度
		if ((ret = UnionReadRequestXMLPackageValue("body/keyLen",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyLen");
			return(ret);
		}
		asymmetricKeyDB.keyLen = atoi(tmpBuf);
	}
	else
	{
		asymmetricKeyDB.pkExponent = 0;		    
		asymmetricKeyDB.keyLen = 256;
	}
	
	// 私钥存储位置
	if ((ret = UnionReadRequestXMLPackageValue("body/vkStoreLocation",tmpBuf,sizeof(tmpBuf))) < 0)
		asymmetricKeyDB.vkStoreLocation = 0;
	else
		asymmetricKeyDB.vkStoreLocation = atoi(tmpBuf);
	
	if (asymmetricKeyDB.vkStoreLocation == 1 || asymmetricKeyDB.vkStoreLocation == 2)
	{
		// 密码机组
		if ((ret = UnionReadRequestXMLPackageValue("body/hsmGroupID",asymmetricKeyDB.hsmGroupID,sizeof(asymmetricKeyDB.hsmGroupID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGroupID");
			return(ret);
		}
		UnionFilterHeadAndTailBlank(asymmetricKeyDB.hsmGroupID);
		if (strlen(asymmetricKeyDB.hsmGroupID) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: hsmGroupID can not be null!\n");
			UnionSetResponseRemark("密码机组不能为空");
			return(errCodeParameter);
		}
		
		// 私钥存储位置
		// modify by leipp 20151215 如果送入的索引号小于10，前面补0
		if ((ret = UnionReadRequestXMLPackageValue("body/vkIndex",vkIndex,sizeof(vkIndex))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkIndex");
			return(ret);
		}
		UnionFilterHeadAndTailBlank(vkIndex);
		snprintf(asymmetricKeyDB.vkIndex,sizeof(asymmetricKeyDB.vkIndex),"%02d",atoi(vkIndex));
		// modify end
		if(strlen(asymmetricKeyDB.vkIndex) >0)
		{
			if (!UnionIsDigitString(asymmetricKeyDB.vkIndex) || (atoi(asymmetricKeyDB.vkIndex) < 0 || atoi(asymmetricKeyDB.vkIndex) >= 21))
			{
				UnionUserErrLog("in UnionDealServiceCodeE120:: vkIndex[%s] error!\n",asymmetricKeyDB.vkIndex);
				UnionSetResponseRemark("非法的私钥索引[%s]",asymmetricKeyDB.vkIndex);
				return(errCodeHsmCmdMDL_InvalidIndex);	
			}
		}
	}

	// 允许使用旧密钥
	if ((ret = UnionReadRequestXMLPackageValue("body/oldVersionKeyIsUsed",tmpBuf,sizeof(tmpBuf))) < 0)
		asymmetricKeyDB.oldVersionKeyIsUsed = 1;
	else
		asymmetricKeyDB.oldVersionKeyIsUsed = atoi(tmpBuf);
	if ((asymmetricKeyDB.oldVersionKeyIsUsed != 0) && (asymmetricKeyDB.oldVersionKeyIsUsed != 1))
	{
		UnionUserErrLog("in UnionDealServiceCodeE120:: oldVersionKeyIsUsed[%s] error!\n",tmpBuf);
		return(errCodeParameter);
	}

	// 模式
	// 0：不生成密钥,默认值
	// 1：生成密钥，并输出裸公钥
	// 2：生成密钥，并输出DER编码公钥
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) < 0)
		mode = 1;
	else
	{
		mode = atoi(tmpBuf);
		if ((mode != 0) && (mode != 1) && (mode != 2))
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: mode[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}
	
	// 允许导入标识
	// 可选，默认值为0
	// 0：不允许
	// 1：允许
	if ((ret = UnionReadRequestXMLPackageValue("body/inputFlag",tmpBuf,sizeof(tmpBuf))) < 0)
		asymmetricKeyDB.inputFlag = 0;
	else
	{
		asymmetricKeyDB.inputFlag = atoi(tmpBuf);
		if ((asymmetricKeyDB.inputFlag != 0) && (asymmetricKeyDB.inputFlag != 1))
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: inputFlag[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}

	// 允许导出标识
	// 可选，默认值为0
	// 0：不允许
	// 1：允许
	if ((ret = UnionReadRequestXMLPackageValue("body/outputFlag",tmpBuf,sizeof(tmpBuf))) < 0)
		asymmetricKeyDB.outputFlag = 0;
	else
	{
		asymmetricKeyDB.outputFlag = atoi(tmpBuf);
		if ((asymmetricKeyDB.outputFlag != 0) && (asymmetricKeyDB.outputFlag != 1))
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: outputFlag[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}

	// 输出标识
	// 0：不输出，默认值
	// 1：LMK对保护输出
	if (asymmetricKeyDB.outputFlag == 1)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/exportFlag",tmpBuf,sizeof(tmpBuf))) < 0)
			exportFlag = 0;
		else
			exportFlag = atoi(tmpBuf);
	}
	else
		exportFlag = 0;

	//　有效天数
	if ((ret = UnionReadRequestXMLPackageValue("body/effectiveDays",tmpBuf,sizeof(tmpBuf))) <= 0)
		asymmetricKeyDB.effectiveDays = 0;
	else
		asymmetricKeyDB.effectiveDays = atoi(tmpBuf);

	// 启用标识
	// 0：不启用
	// 1：即刻启用，并生效，默认值
	// 2：即刻启用：指定时间生效
	if ((ret = UnionReadRequestXMLPackageValue("body/enabled",tmpBuf,sizeof(tmpBuf))) < 0)
		enabled = 1;
	else
		enabled = atoi(tmpBuf);
		
	if (mode == 0)
	{
		exportFlag = 0;
		enabled = 0;
	}
	
	
	if (mode == 0)
		asymmetricKeyDB.status = conAsymmetricKeyStatusOfInitial;	// 状态设置为初始化状态
	else
	{
		if (enabled == 0)
			asymmetricKeyDB.status = conAsymmetricKeyStatusOfInitial;	// 状态设置为初始化状态
		else
			asymmetricKeyDB.status = conAsymmetricKeyStatusOfEnabled;	// 状态设置为启用状态
	}

	if (!isRemoteKeyOperate)		// 本地密钥操作	
	{
		// 密钥申请平台
		UnionReadRequestXMLPackageValue("body/keyApplyPlatform",asymmetricKeyDB.keyApplyPlatform,sizeof(asymmetricKeyDB.keyApplyPlatform));

		// 密钥分发平台
		UnionReadRequestXMLPackageValue("body/keyDistributePlatform",asymmetricKeyDB.keyDistributePlatform,sizeof(asymmetricKeyDB.keyDistributePlatform));

		// modify by leipp 20151209
		// 读取并检查申请和分发平台
		if ((ret = UnionCheckRemoteKeyPlatform(asymmetricKeyDB.keyName, asymmetricKeyDB.keyApplyPlatform, asymmetricKeyDB.keyDistributePlatform, &tkeyApplyPlatform, &tkeyDistributePlatform, &isRemoteApplyKey, &isRemoteDistributeKey)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionCheckRemoteKeyPlatform!\n");
			return(ret);
		}
		// modify by leipp end

		// 检查本地密钥是否存在
		/*if (isRemoteApplyKey || isRemoteDistributeKey)
		{
			// 读取非对称密钥
			if ((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName,0,&asymmetricKeyDB)) != errCodeKeyCacheMDL_WrongKeyName)
			{
				UnionUserErrLog("in UnionDealServiceCodeE120:: keyName[%s] is already exists!\n",asymmetricKeyDB.keyName);
				UnionSetResponseRemark("非对称密钥[%s]已存在",asymmetricKeyDB.keyName);
				return(errCodeKeyCacheMDL_KeyAlreadyExists);
			}	
		}*/
	}
	else			// 远程密钥操作
	{
		isRemoteApplyKey = 0;
		isRemoteDistributeKey = 0;
		
		// 系统ID
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);
		}

		//获取保护密钥
		snprintf(sql,sizeof(sql),"select * from localKeyPlatform where sysID = '%s' and algorithmID = %d",tmpBuf,asymmetricKeyDB.algorithmID);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionSelectRealDBRecord[%s]\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: protectKey not found!\n");
			UnionSetResponseRemark("远程保护密钥不存在");
			return(errCodeParameter);
		}

		if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionLocateXMLPackage[%s]!\n","detail");	
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("protectKey",platformProtectKey,sizeof(platformProtectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadXMLPackageValue[%s]!\n","protectKey");
			return(ret);
		}
		
		if ((ret = UnionReadXMLPackageValue("checkValue",platformCheckValue,sizeof(platformCheckValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadXMLPackageValue[%s]!\n","checkValue");
			return(ret);
		}

		// 密钥申请
		if ((ret = UnionReadRequestXMLPackageValue("body/keyApplyPlatform",asymmetricKeyDB.keyApplyPlatform,sizeof(asymmetricKeyDB.keyApplyPlatform))) > 0)
		{
			strcpy(asymmetricKeyDB.keyApplyPlatform,"");
			isApplyFlag = 1;
		}

		// 密钥分发
		if ((ret = UnionReadRequestXMLPackageValue("body/keyDistributePlatform",asymmetricKeyDB.keyDistributePlatform,sizeof(asymmetricKeyDB.keyDistributePlatform))) > 0)
		{
			strcpy(asymmetricKeyDB.keyDistributePlatform,"");
			isDistributeFlag = 1;
		}

		UnionLog("in UnionDealServiceCodeE120:: isApplyFlag = %d   isDistributeFlag = %d  isRemoteApplyKey = %d\n",isApplyFlag,isDistributeFlag,isRemoteApplyKey);
	}
	
	if (isRemoteApplyKey)		// 远程申请密钥
	{
		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}
		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	// 检查密钥属性
	if (!UnionIsValidAsymmetricKeyDB(&asymmetricKeyDB))
	{
		UnionUserErrLog("in UnionDealServiceCodeE120:: UnionIsValidAsymmetricKeyDB!\n");
		return(errCodeKeyCacheMDL_WrongKeyName);	
	}
	
	switch(mode)
	{
		case 0:
			break;
		case 1:
		case 2:
			if (isRemoteApplyKey) // 远程生成
			{
				if((ret = UnionReadResponseRemoteXMLPackageValue("body/vkValue",remoteVK,sizeof(remoteVK))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/vkValue");
					return(ret);
				}

				if((ret = UnionReadResponseRemoteXMLPackageValue("body/pkValue",asymmetricKeyDB.pkValue,sizeof(asymmetricKeyDB.pkValue))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/pkValue");
					return(ret);
				}
				snprintf(platformProtectKey,sizeof(platformProtectKey),"%s",tkeyApplyPlatform.protectKey);
				// add by leipp 20160126,申请时返回公钥
				snprintf(pkValue,sizeof(pkValue),"%s",asymmetricKeyDB.pkValue);
				// add end
			}
			if (isDistributeFlag)	// 远程存储
			{
				// 读取远程密钥值
				if((ret = UnionReadRequestXMLPackageValue("body/vkValue",remoteVK,sizeof(remoteVK))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkValue");
					return(ret);
				}

				// 读取远程密钥校验值
				if((ret = UnionReadRequestXMLPackageValue("body/pkValue",asymmetricKeyDB.pkValue,sizeof(asymmetricKeyDB.pkValue))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkValue");
					return(ret);
				}
				// add by leipp 20160126,分发时,会返回乱码
				snprintf(pkValue,sizeof(pkValue),"%s",asymmetricKeyDB.pkValue);
				// add end
			}

			if (isRemoteApplyKey || isDistributeFlag)
			{
				switch(asymmetricKeyDB.algorithmID)
				{
					case	conAsymmetricAlgorithmIDOfRSA:	// RSA算法
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case	conHsmCmdVerRacalStandardHsmCmd:
							case	conHsmCmdVerSJL06StandardHsmCmd:
								lenOfVK = aschex_to_bcdhex(remoteVK,strlen(remoteVK),vkValue);
								if ((lenOfVK = UnionHsmCmdUI2("00",NULL,strlen(platformProtectKey),platformProtectKey,NULL,vkValue,lenOfVK,(unsigned char*)asymmetricKeyDB.vkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE120:: UnionHsmCmdUI2!\n");
									return(lenOfVK);
								}

								switch (asymmetricKeyDB.keyType)
								{
									case 	conAsymmetricKeyTypeOfSignature:
										asymmetricKeyDB.vkValue[3] = '0';
										break;
									case	conAsymmetricKeyTypeOfEncryption:
										asymmetricKeyDB.vkValue[3] = '1';
										break;
									case	conAsymmetricKeyTypeOfSignatureAndEncryption:
                                                                                asymmetricKeyDB.vkValue[3] = '2';
										break;
									default:
										UnionUserErrLog("in UnionDealServiceCodeE120:: keyType[%d]\n",asymmetricKeyDB.keyType);
										UnionSetResponseRemark("非法的密钥类型标识");
										return(errCodeParameter);
								}
								/*
								if (mode == 2)
								{
									lenOfPK = aschex_to_bcdhex(asymmetricKeyDB.pkValue,strlen(asymmetricKeyDB.pkValue),pkValue);
									// 获取裸公钥
									if ((ret = UnionGetPKOutOfRacalHsmCmdReturnStr((unsigned char *)pkValue,lenOfPK,asymmetricKeyDB.pkValue,&len,sizeof(asymmetricKeyDB.pkValue))) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE120:: UnionGetPKOutOfRacalHsmCmdReturnStr!\n");
										return(ret);
									}
									asymmetricKeyDB.pkValue[len] = 0;
									bcdhex_to_aschex(pkValue,lenOfPK,tmpBuf);
									memcpy(pkValue,tmpBuf,lenOfPK*2);
									pkValue[lenOfPK*2] = 0;
								}*/
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE120:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("非法的加密机指令类型");
								return(errCodeParameter);
						}
						break;
					case	conAsymmetricAlgorithmIDOfSM2:
						switch(phsmGroupRec->hsmCmdVersionID)
                                                {
                                                        case    conHsmCmdVerRacalStandardHsmCmd:
                                                        case    conHsmCmdVerSJL06StandardHsmCmd:
                                                                lenOfVK = aschex_to_bcdhex(remoteVK,strlen(remoteVK),vkValue);
                                                                // 填充数据
                                                                if (lenOfVK % 16 != 0)
                                                                {
                                                                        addLen = 16 - lenOfVK % 16;
									sprintf(vkValue + lenOfVK,"%0*d",addLen,0);
									lenOfVK += addLen;
                                                                }
                                                               
                                                                if ((lenOfVK = UnionHsmCmdWA(1,"01","000",1,platformProtectKey,platformCheckValue,NULL,lenOfVK,(unsigned char*)vkValue,tmpBuf)) < 0)
                                                                {
                                                                        UnionUserErrLog("in UnionDealServiceCodeE120:: UnionHsmCmdWA!\n");
                                                                        return(lenOfVK);
                                                                }
								bcdhex_to_aschex(tmpBuf,lenOfVK,asymmetricKeyDB.vkValue);
								asymmetricKeyDB.vkValue[lenOfVK*2] = 0;
                                                                break;
                                                        default:
                                                                UnionUserErrLog("in UnionDealServiceCodeE120:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                                                                UnionSetResponseRemark("非法的加密机指令类型");
                                                                return(errCodeParameter);
                                                }
                                                break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE120:: asymmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
						return(errCodeEsscMDL_InvalidAlgorithmID);
				}
			}
			
			if (isRemoteApplyKey == 0 && isDistributeFlag == 0)
			{
				switch(asymmetricKeyDB.algorithmID)
				{
					case	conAsymmetricAlgorithmIDOfRSA:	// RSA算法
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case	conHsmCmdVerRacalStandardHsmCmd:
							case	conHsmCmdVerSJL06StandardHsmCmd:
								if ((lenOfPK = UnionHsmCmdEI(keyType[0],asymmetricKeyDB.keyLen,"01",
									lenOfPKExp,pkexp,0,pkValue,sizeof(pkValue),&lenOfVK,(unsigned char *)vkValue,sizeof(vkValue))) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE120:: UnionHsmCmdEI!\n");
									return(lenOfPK);
								}

								// 获取私钥
								bcdhex_to_aschex(vkValue,lenOfVK,asymmetricKeyDB.vkValue);
								asymmetricKeyDB.vkValue[lenOfVK*2] = 0;
								if (isApplyFlag)	// 远程	
								{
									if ((ret = UnionHsmCmdUG("00",conZMK,platformProtectKey,iv,lenOfVK,vkValue,remoteVK)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE120:: UnionHsmCmdUG!\n");
										return(ret);
									}
									bcdhex_to_aschex(remoteVK,ret,vkByKek);
									vkByKek[ret*2] = 0;
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE120:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("非法的加密机指令类型");
								return(errCodeParameter);
						}
						
						// 获取裸公钥
						if ((ret = UnionGetPKOutOfRacalHsmCmdReturnStr((unsigned char *)pkValue,lenOfPK,asymmetricKeyDB.pkValue,&len,sizeof(asymmetricKeyDB.pkValue))) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE120:: UnionGetPKOutOfRacalHsmCmdReturnStr!\n");
							return(ret);
						}
						asymmetricKeyDB.pkValue[len] = 0;
						if (mode == 1)	// 裸公钥
							strcpy(pkValue,asymmetricKeyDB.pkValue);
						else
						{
							bcdhex_to_aschex(pkValue,lenOfPK,tmpBuf);
							memcpy(pkValue,tmpBuf,lenOfPK*2);
							pkValue[lenOfPK*2] = 0;
						}

						if (isApplyFlag && mode == 2) //远程
						{
							snprintf(pkValue,sizeof(pkValue),"%s",asymmetricKeyDB.pkValue);
						}

						break;
					case	conAsymmetricAlgorithmIDOfSM2:	// SM2算法
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case	conHsmCmdVerRacalStandardHsmCmd:
							case	conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret = UnionHsmCmdK1(keyType[0],asymmetricKeyDB.keyLen,asymmetricKeyDB.pkValue,sizeof(asymmetricKeyDB.pkValue),&lenOfVK,(unsigned char *)vkValue,sizeof(vkValue))) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE120:: UnionHsmCmdK1!\n");
									return(ret);
								}

								// 获取私钥
								bcdhex_to_aschex(vkValue,lenOfVK,asymmetricKeyDB.vkValue);
								asymmetricKeyDB.vkValue[lenOfVK*2] = 0;
								if (isApplyFlag)	// 远程	
								{
									// 填充数据
                                    					if (lenOfVK % 16 != 0)
                                    					{
                                					        addLen = 16 - lenOfVK % 16;
										sprintf(vkValue + lenOfVK,"%0*d",addLen,0);
										lenOfVK += addLen;	
									}
		
									if ((lenOfVK = UnionHsmCmdWA(2,"01","000",1,platformProtectKey,platformCheckValue,NULL,lenOfVK,(unsigned char*)vkValue,remoteVK)) < 0)
                                    					{
                                        					UnionUserErrLog("in UnionDealServiceCodeE120:: UnionHsmCmdWA!\n");
										return(lenOfVK);
									}
									bcdhex_to_aschex(remoteVK,lenOfVK,vkByKek);
									vkByKek[lenOfVK*2] = 0;
								}                                                               
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE120:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("非法的加密机指令类型");
								return(errCodeParameter);
						}
						strcpy(pkValue,asymmetricKeyDB.pkValue);
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE120:: asymmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
						return(errCodeEsscMDL_InvalidAlgorithmID);
				}
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE120:: mode = [%d] error!\n",mode);
			return(errCodeEsscMDLKeyOperationNotPermitted);
	}

	// 

	// 生效日期
	// 当enabled为2时，存在
	if (enabled == 1)
	{
		UnionGetFullSystemDate(asymmetricKeyDB.activeDate);
	}
	else if (enabled == 2)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/activeDate",asymmetricKeyDB.activeDate,sizeof(asymmetricKeyDB.activeDate))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadRequestXMLPackageValue[%s]!\n","body/activeDate");
			return(ret);	
		}
	
		UnionFilterHeadAndTailBlank(asymmetricKeyDB.activeDate);
		if(strlen(asymmetricKeyDB.activeDate) ==0)
		{	
			UnionUserErrLog("in UnionDealServiceCodeE120:: activeDate can not be null!\n");
			UnionSetResponseRemark("生效日期不能为空");
			return(errCodeParameter);
		}

		if (!UnionIsValidFullDateStr(asymmetricKeyDB.activeDate))
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: activeDate[%s] error!\n",asymmetricKeyDB.activeDate);
			UnionSetResponseRemark("生效日期格式[%s]非法,必须为YYYYMMDD格式",asymmetricKeyDB.activeDate);
			return(errCodeParameter);
		}
	}
	
	asymmetricKeyDB.creatorType = conAsymmetricCreatorTypeOfApp;
	if (isRemoteKeyOperate)		// 远程密钥操作
	{
		if ((ret = UnionReadRemoteSysIDAndAppID(sysID,sizeof(sysID),appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadRemoteSysIDAndAppID!\n");
			return(ret);	
		}
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",asymmetricKeyDB.usingUnit,sizeof(asymmetricKeyDB.usingUnit))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);	
		}
	}
	else				// 本地密钥操作
	{
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);	
		}
		if ((ret = UnionReadRequestXMLPackageValue("head/appID",appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");
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
			snprintf(asymmetricKeyDB.usingUnit,sizeof(asymmetricKeyDB.usingUnit),":%s",ptr);
		}
	}
	UnionPieceAsymmetricKeyCreator(sysID,appID,asymmetricKeyDB.creator);
	
	/*if ((ret = UnionReadRequestXMLPackageValue("head/appID",asymmetricKeyDB.creator,sizeof(asymmetricKeyDB.creator))) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");
		return(ret);	
	}*/

	if (isRemoteDistributeKey)	// 远程分发密钥
	{
		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}
		if (mode == 1)	// 生成密钥
		{
			// 增加密钥和校验值
			switch(asymmetricKeyDB.algorithmID)
			{
				case	conAsymmetricAlgorithmIDOfRSA:	// RSA算法
					// 把本地LMK对加密转成远程Kek加密
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case	conHsmCmdVerRacalStandardHsmCmd:
						case	conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdUG("00",conZMK,tkeyDistributePlatform.protectKey,iv,lenOfVK,vkValue,remoteVK)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE120:: UnionHsmCmdUG!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE120:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					break;
				case	conAsymmetricAlgorithmIDOfSM2:	// SM2算法
					switch(phsmGroupRec->hsmCmdVersionID)
                                        {
                                                case    conHsmCmdVerRacalStandardHsmCmd:
                                                case    conHsmCmdVerSJL06StandardHsmCmd:
                                                        // 填充数据
                                                        if (lenOfVK % 16 != 0)
                                                        {
                                                                addLen = 16 - lenOfVK % 16;
								sprintf(vkValue + lenOfVK,"%0*d",addLen,0);
								lenOfVK += addLen;                                                        
							}

                                                        if ((ret = UnionHsmCmdWA(2,"01","000",1,tkeyDistributePlatform.protectKey,tkeyDistributePlatform.checkValue,NULL,lenOfVK,(unsigned char*)vkValue,remoteVK)) < 0)
                                                        {
                                                                UnionUserErrLog("in UnionDealServiceCodeE120:: UnionHsmCmdWA!\n");
                                                                return(ret);
                                                        }
                                                        break;
                                                default:
                                                        UnionUserErrLog("in UnionDealServiceCodeE120:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                                                        UnionSetResponseRemark("非法的加密机指令类型");
                                                        return(errCodeParameter);
                                        }
                                        break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE120:: Distribute Key symmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
			bcdhex_to_aschex(remoteVK,ret,vkByKek);
			vkByKek[ret*2] = 0;
			if ((ret = UnionSetRequestRemoteXMLPackageValue("body/vkValue",vkByKek)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE120:: UnionSetRequestRemoteXMLPackageValue[%s]!\n","body/vkValue");
				return(ret);
			}
			if ((ret = UnionSetRequestRemoteXMLPackageValue("body/pkValue",asymmetricKeyDB.pkValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE120:: UnionSetRequestRemoteXMLPackageValue[%s]!\n","body/pkValue");
				return(ret);
			}
		}
		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	if (asymmetricKeyDB.vkStoreLocation != 0 && mode != 0)	// 私钥存储在密码机中
	{

		// 检测私钥索引是否被使用
		//sprintf(sql,"select * from vkKeyIndex where hsmGroupID = '%s' and vkIndex = '%s' and status = 0",asymmetricKeyDB.hsmGroupID,asymmetricKeyDB.vkIndex);
		snprintf(sql,sizeof(sql),"select * from vkKeyIndex where hsmGroupID = '%s' and vkIndex = '%s' and status = 0 and algorithmID = %d",asymmetricKeyDB.hsmGroupID,asymmetricKeyDB.vkIndex,asymmetricKeyDB.algorithmID);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionExecRealDBSql[%s]!\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: vkIndex[%s] is already use!\n",asymmetricKeyDB.vkIndex);
			UnionSetResponseRemark("vkIndex[%s]已经使用",asymmetricKeyDB.vkIndex);
			return(errCodeParameter);
		}
				
		snprintf(sql,sizeof(sql),"select * from hsm where hsmGroupID = '%s' and enabled = 1",asymmetricKeyDB.hsmGroupID);
		if ((hsmNum = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionExecRealDBSql[%s]!\n",sql);
			return(hsmNum);
		}
		else if (hsmNum == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: hsmGroupID[%s]中没有可用密码机!\n",asymmetricKeyDB.hsmGroupID);
			return(errCodeParameter);
		}

		// add by leipp 20160126,确保vkValue为存储私钥
		lenOfVK = aschex_to_bcdhex(asymmetricKeyDB.vkValue,strlen(asymmetricKeyDB.vkValue),vkValue);
		vkValue[lenOfVK] = 0;
		// add end

		for (i = 0; i < hsmNum; i++)
		{
			UnionLocateXMLPackage("detail", i+1);
			
			if ((ret = UnionReadXMLPackageValue("ipAddr", ipAddr, sizeof(ipAddr))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE120:: UnionReadXMLPackageValue[%s]!\n","ipAddr");
				return(ret);
			}
			UnionSetUseSpecHsmIPAddrForOneCmd(ipAddr);
			switch(asymmetricKeyDB.algorithmID)
			{
				case	conAsymmetricAlgorithmIDOfRSA:	// RSA算法
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdEK(atoi(asymmetricKeyDB.vkIndex),lenOfVK,(unsigned char *)vkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE120:: UnionHsmCmdEK hsm[%s] vkIndex[%s]!\n",ipAddr,asymmetricKeyDB.vkIndex);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE120:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					break;
				case	conAsymmetricAlgorithmIDOfSM2:	// SM2算法
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdK2(atoi(asymmetricKeyDB.vkIndex),lenOfVK,(unsigned char *)vkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE120:: UnionHsmCmdK2 hsm[%s] vkIndex[%s]!\n",ipAddr,asymmetricKeyDB.vkIndex);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE120:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE120:: asymmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
		}
		
		// 修改私钥索引
		//sprintf(sql,"update vkKeyIndex set status = 1 where hsmGroupID = '%s' and vkIndex = '%s'",asymmetricKeyDB.hsmGroupID,asymmetricKeyDB.vkIndex);
		snprintf(sql,sizeof(sql),"update vkKeyIndex set status = 1 where hsmGroupID = '%s' and vkIndex = '%s' and algorithmID = %d",asymmetricKeyDB.hsmGroupID,asymmetricKeyDB.vkIndex,asymmetricKeyDB.algorithmID);
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionExecRealDBSql[%s]!\n",sql);
			UnionSetResponseRemark("更新私钥[%s]状态失败",asymmetricKeyDB.vkIndex);
			return(ret);
		}

		// modify by leipp 20160516 , 之前是先设置私钥值为空,后才导入加密机
		if (asymmetricKeyDB.vkStoreLocation == 1)
			asymmetricKeyDB.vkValue[0] = 0;;
		// modify by leipp end 20160516
	}

	if (mode == 0)	// 不生成密钥
	{
		memset(pkValue,0,sizeof(pkValue));
		if ((ret =  UnionCreateAsymmetricKeyDB(&asymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionCreateAsymmetricKeyDB,keyName[%s]!\n",asymmetricKeyDB.keyName);
			return(ret);	
		}
	}
	else
	{
		if ((ret =  UnionGenerateAsymmetricKeyDBRec(&asymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE120:: UnionGenerateAsymmetricKeyDBRec,keyName[%s]!\n",asymmetricKeyDB.keyName);
			return(ret);	
		}
	}

	// 设置响应
	if ((ret = UnionSetResponseXMLPackageValue("body/pkValue",pkValue)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE120:: UnionSetResponseXMLPackageValue pkValue[%s]!\n",pkValue);
		return(ret);
	}

	if ((exportFlag) || (isRemoteKeyOperate == 1 && isApplyFlag))
	{

		if (isApplyFlag)
		{
			if ((ret = UnionSetResponseXMLPackageValue("body/vkValue",vkByKek)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE120:: UnionSetResponseXMLPackageValue vkValue[%s]!\n","body/vkValue");
				return(ret);
			}
		}

		if (isRemoteKeyOperate == 0)
		{
			if ((ret = UnionSetResponseXMLPackageValue("body/vkValue",asymmetricKeyDB.vkValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE120:: UnionSetResponseXMLPackageValue vkValue[%s]!\n","body/vkValue");
				return(ret);
			}
		}
	}
	return(0);
}
