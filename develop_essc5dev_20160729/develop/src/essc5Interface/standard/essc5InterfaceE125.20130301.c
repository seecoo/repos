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
#include "commWithHsmSvr.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "remoteKeyPlatform.h"
#include "unionHsmCmd.h"
#include "asymmetricKeyDB.h"
#include "unionHsmCmdVersion.h"

/***************************************
服务代码:	E125
服务名:		分发非对称密钥
功能描述:	分发非对称密钥
***************************************/
int UnionDealServiceCodeE125(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	char				keyName[160];
	char				pkValue[512];
	int				mode = 1;
	int				len = 0;
	char				tmpBuf[32];
	int				lenOfVK = 0;
	int				lenOfPK = 0;
	int				addLen = 0;
	int				isRemoteKeyOperate = 0;
	char				vkValue[2560];
	char				sql[1024];
	char				platformProtectKey[64];
	char				platformCheckValue[32];
	char				remoteVK[2560];
	char				vkByKek[2560];
	char				keyType[32];
	unsigned char			pkexp[32];
	int				lenOfPKExp = 0;
	
	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	TUnionRemoteKeyPlatform		tkeyDistributePlatform;

	// 检测远程标识
	isRemoteKeyOperate = UnionIsRemoteKeyOperate();

	// 读取密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE125:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);

	// 读取非对称密钥
	if ((ret = UnionReadAsymmetricKeyDBRec(keyName,0,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE125:: UnionReadAsymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}

	// 公钥指数
	if (asymmetricKeyDB.algorithmID == conAsymmetricAlgorithmIDOfRSA)
	{
		if (asymmetricKeyDB.pkExponent < 256)
			sprintf(tmpBuf,"%02X",asymmetricKeyDB.pkExponent);
		else if (asymmetricKeyDB.pkExponent < 256 * 256)
			sprintf(tmpBuf,"%04X",asymmetricKeyDB.pkExponent);
		else
			sprintf(tmpBuf,"%06X",asymmetricKeyDB.pkExponent);
		
		lenOfPKExp = aschex_to_bcdhex(tmpBuf,strlen(tmpBuf),(char *)pkexp);
	}

	// 模式
	// 1：分发当前密钥，默认值
	// 2：分发新密钥
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) <= 0)
		mode = 1;
	else
	{
		mode = atoi(tmpBuf);	

		if ((mode != 1) && (mode != 2))
		{
			UnionUserErrLog("in UnionDealServiceCodeE125:: mode[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}
	
	if (!isRemoteKeyOperate)		// 本地密钥操作	
	{
		// 密钥分发平台
		if (strlen(asymmetricKeyDB.keyDistributePlatform) > 0)
		{
			// modify by leipp 20151209
			// 读取并检查申请和分发平台
			if ((ret = UnionCheckRemoteKeyPlatform(asymmetricKeyDB.keyName, NULL, asymmetricKeyDB.keyDistributePlatform, NULL, &tkeyDistributePlatform, NULL, &ret)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE125:: UnionCheckRemoteKeyPlatform!\n");
				return(ret);
			}
			// modify by leipp end
		}
		else
		{
			UnionUserErrLog("in UnionDealServiceCodeE125:: keyName[%s] keyDistributePlatform can not be null!\n",asymmetricKeyDB.keyName);
			UnionSetResponseRemark("分发平台不能为空");
			return(errCodeParameter);
		}

		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE125:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		if (mode == 2) // 分发新密钥
		{
			strcpy(asymmetricKeyDB.oldPKValue,asymmetricKeyDB.pkValue);
			strcpy(asymmetricKeyDB.oldVKValue,asymmetricKeyDB.vkValue);

			sprintf(keyType,"%d",asymmetricKeyDB.keyType);

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
								UnionUserErrLog("in UnionDealServiceCodeE125:: UnionHsmCmdEI!\n");
								return(lenOfPK);
							}
							// 获取私钥
							bcdhex_to_aschex(vkValue,lenOfVK,asymmetricKeyDB.vkValue);
							asymmetricKeyDB.vkValue[lenOfVK*2] = 0;
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE125:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					
					// 获取裸公钥
					if ((ret = UnionGetPKOutOfRacalHsmCmdReturnStr((unsigned char *)pkValue,lenOfPK,asymmetricKeyDB.pkValue,&len,sizeof(asymmetricKeyDB.pkValue))) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE125:: UnionGetPKOutOfRacalHsmCmdReturnStr!\n");
						return(ret);
					}

					break;
				case	conAsymmetricAlgorithmIDOfSM2:	// SM2算法
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdK1(keyType[0],asymmetricKeyDB.keyLen,asymmetricKeyDB.pkValue,sizeof(asymmetricKeyDB.pkValue),&lenOfVK,(unsigned char *)vkValue,sizeof(vkValue))) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE125:: UnionHsmCmdK1!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE125:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					bcdhex_to_aschex(vkValue,lenOfVK,asymmetricKeyDB.vkValue);
					asymmetricKeyDB.vkValue[lenOfVK*2] = 0;
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE125:: asymmetricKeyDB.algorithmID[%d] is invalid!\n",asymmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
		}

		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/pkValue",asymmetricKeyDB.pkValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE125:: UnionSetRequestRemoteXMLPackageValue[%s]!\n","body/pkValue");
			return(ret);
		}

		if (mode == 1)	// 分发旧密钥
		{
			lenOfVK = aschex_to_bcdhex(asymmetricKeyDB.vkValue,strlen(asymmetricKeyDB.vkValue),vkValue);
		}
		
		switch(asymmetricKeyDB.algorithmID)
                {
                        case    conAsymmetricAlgorithmIDOfRSA:  // RSA算法
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((lenOfVK = UnionHsmCmdUG("00",conZMK,tkeyDistributePlatform.protectKey,NULL,lenOfVK,vkValue,remoteVK)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE125:: UnionHsmCmdUG!\n");
							return(lenOfVK);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE125:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			case    conAsymmetricAlgorithmIDOfSM2:
                                switch(phsmGroupRec->hsmCmdVersionID)
                                {
                                        case    conHsmCmdVerRacalStandardHsmCmd:
                                        case    conHsmCmdVerSJL06StandardHsmCmd:
                                                // 填充数据
                                                if (lenOfVK % 16 != 0)
                                                {
                                                        addLen = 16 - lenOfVK % 16;
                                                        lenOfVK = sprintf(tmpBuf,"%s%0*d",vkValue,addLen,0);
                                                        memcpy(vkValue,tmpBuf,lenOfVK);
							vkValue[lenOfVK] = 0;
                                                }
                                                if ((lenOfVK = UnionHsmCmdWA(2,"01","000",1,tkeyDistributePlatform.protectKey,tkeyDistributePlatform.checkValue,NULL,lenOfVK,(unsigned char*)vkValue,remoteVK)) < 0)
                                                {
                                                        UnionUserErrLog("in UnionDealServiceCodeE125:: UnionHsmCmdWA!\n");
                                                        return(lenOfVK);
                                                }
                                                break;
                                        default:
                                                UnionUserErrLog("in UnionDealServiceCodeE125:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                                                UnionSetResponseRemark("非法的加密机指令类型");
                                                return(errCodeParameter);
                                }
                                break;
                        default:
                                UnionUserErrLog("in UnionDealServiceCodeE125:: Distribute Key symmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
                                return(errCodeEsscMDL_InvalidAlgorithmID);
                }

		bcdhex_to_aschex(remoteVK,lenOfVK,vkByKek);
		vkByKek[lenOfVK*2] = 0;
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/vkValue",vkByKek)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE125:: UnionSetRequestRemoteXMLPackageValue[%s]!\n","body/vkValue");
			return(ret);
		}

		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE125:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}

		// 更新非对称密钥
		if ((ret =  UnionUpdateAsymmetricKeyDBKeyValue(&asymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE125:: UnionUpdateAsymmetricKeyDBKeyValue,keyName[%s]!\n",asymmetricKeyDB.keyName);
			return(ret);
		}

		return 0;
	}
	else			// 远程密钥操作
	{

		strcpy(asymmetricKeyDB.oldPKValue,asymmetricKeyDB.pkValue);
		strcpy(asymmetricKeyDB.oldVKValue,asymmetricKeyDB.vkValue);

		// 系统ID
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE125:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);
		}

		//获取保护密钥
		snprintf(sql,sizeof(sql),"select * from localKeyPlatform where sysID = '%s' and algorithmID = %d",tmpBuf,asymmetricKeyDB.algorithmID);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCodeE125:: UnionSelectRealDBRecord[%s]\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE125:: protectKey not found!\n");
			UnionSetResponseRemark("远程保护密钥不存在");
			return(errCodeParameter);
		}

		if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE125:: UnionLocateXMLPackage[%s]!\n","detail");	
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("protectKey",platformProtectKey,sizeof(platformProtectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE125:: UnionReadXMLPackageValue[%s]!\n","protectKey");
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("checkValue",platformCheckValue,sizeof(platformCheckValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE125:: UnionReadXMLPackageValue[%s]!\n","checkValue");
			return(ret);
		}

		if((ret = UnionReadRequestXMLPackageValue("body/vkValue",remoteVK,sizeof(remoteVK))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE125:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkValue");
			return(ret);
		}

		if((ret = UnionReadRequestXMLPackageValue("body/pkValue",asymmetricKeyDB.pkValue,sizeof(asymmetricKeyDB.pkValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE125:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkValue");
			return(ret);
		}

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
							UnionUserErrLog("in UnionDealServiceCodeE125:: UnionHsmCmdUI2!\n");
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
								UnionUserErrLog("in UnionDealServiceCodeE125:: keyType[%d]\n",asymmetricKeyDB.keyType);
								UnionSetResponseRemark("非法的密钥类型标识");
								return(errCodeParameter);
						}

						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE125:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
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
                                                        UnionUserErrLog("in UnionDealServiceCodeE125:: UnionHsmCmdWA!\n");
                                                        return(lenOfVK);
                                                }
						bcdhex_to_aschex(tmpBuf,lenOfVK,asymmetricKeyDB.vkValue);
						asymmetricKeyDB.vkValue[lenOfVK*2] = 0;
                                                break;
                                        default:
                                                UnionUserErrLog("in UnionDealServiceCodeE125:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                                                UnionSetResponseRemark("非法的加密机指令类型");
                                                return(errCodeParameter);
                                }
                                break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE125:: asymmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}

		// 更新非对称密钥
		if ((ret =  UnionUpdateAsymmetricKeyDBKeyValue(&asymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE125:: UnionUpdateAsymmetricKeyDBKeyValue,keyName[%s]!\n",asymmetricKeyDB.keyName);
			return(ret);	
		}

		return(0);	
	}
}


