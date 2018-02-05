//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "defaultIBMPinOffsetDef.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmdVersion.h"

/***************************************
服务代码:	E301
服务名:		E301校验ARQC(525)
功能描述:	E301校验ARQC(525)
***************************************/
void UnionGetResponseRemarkByResponseCodeE301(int retCode);
int UnionDealServiceCodeE301(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	char				keyName[136];
	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;

	char 			pan[32];
	char                    atc[10+1];
	char                  	arqcData[2048+1];
	char                    arqc[20+1];
	char			versionTmp[8+1];
	char                    iccTypeTmp[10+1];
	char 			appID[40+1];
	char 			ownerName[40+1];
	char			keyType[40+1];	

	int                     iccType;
	int 			version;

	memset(&symmetricKeyDB,	 	0, 	sizeof(symmetricKeyDB));
	memset(appID,		0,	sizeof(appID));
	memset(ownerName,	0,	sizeof(ownerName));
	memset(keyType,		0,	sizeof(keyType));

	// mk-ac密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName", keyName, sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE301:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;

	// .....读其他请求域 .......
	if ((ret = UnionReadRequestXMLPackageValue("body/version", versionTmp, sizeof(versionTmp))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE301:: UnionReadRequestXMLPackageValue[%s]!\n","body/version");
		return(ret);
	}
	versionTmp[ret] = 0;

	version = atoi(versionTmp);

	UnionAnalysisPBOCSymmetricKeyName(keyName, version, appID, ownerName, keyType);
	
	snprintf(keyName,sizeof(keyName), "%s.%s.%s", appID, ownerName, keyType);

	if ((ret = UnionReadRequestXMLPackageValue("body/pan",  pan, sizeof(pan)))< 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE301:: UnionReadRequestXMLPackageValue[%s]!\n","body/pan");
		return(ret);
	}
	else
	{
		pan[ret] = 0;
		UnionFilterHeadAndTailBlank(pan);
                if (strlen(pan) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE301:: pan can not be null!\n");
                        UnionSetResponseRemark("卡号或（卡号+卡序列号）不能为空!");
                        return(errCodeParameter);
                }
		if (!UnionIsDigitString(pan))
    		{
        		UnionUserErrLog("in UnionDealServiceCodeE301:: pan[%s] is error!\n",pan);
        		UnionSetResponseRemark("卡号或（卡号+卡序列号）[%s]非法,必须为十进制数",pan);
        		return(errCodeParameter);
    		}
	}

	if ((ret = UnionReadRequestXMLPackageValue("body/atc", atc, sizeof(atc)))< 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE301:: UnionReadRequestXMLPackageValue[%s]!\n","body/atc");
		return(ret);
	}
 	else
        {
		atc[ret] = 0;
		UnionFilterHeadAndTailBlank(atc);
                if (strlen(atc) != 4)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE301:: atc can not be null or the len of atc != 4!\n");
                        UnionSetResponseRemark("离散过程因子不能为空或长度不为4!");
                        return(errCodeParameter);
                }
                if (!UnionIsBCDStr(atc))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE301:: atc[%s] is error!\n",atc);
                        UnionSetResponseRemark("离散过程因子[%s]非法,必须为十六进制数",atc);
                        return(errCodeParameter);
                }
        }
	
	if ((ret = UnionReadRequestXMLPackageValue("body/arqcData", arqcData, sizeof(arqcData)))< 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE301:: UnionReadRequestXMLPackageValue[%s]!\n", "body/arqcData");
		return(ret);
	}
	else
        {
		arqcData[ret] = 0;
		UnionFilterHeadAndTailBlank(arqcData);
                if (strlen(arqcData) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE301:: arqcData can not be null!\n");
                        UnionSetResponseRemark("计算ARQC使用的数据不能为空!");
                        return(errCodeParameter);
                }
                if (!UnionIsBCDStr(arqcData))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE301:: arqcData[%s] is error!\n",arqcData);
                        UnionSetResponseRemark("计算ARQC使用的数据非法,必须为十六进制数");
                        return(errCodeParameter);
                }
        }

	if ((ret = UnionReadRequestXMLPackageValue("body/arqc", arqc, sizeof(arqc)))< 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE301:: UnionReadRequestXMLPackageValue[%s]!\n","body/arqc");
		return(ret);
	}
	else
        {
		arqc[ret] = 0;
		UnionFilterHeadAndTailBlank(arqc);
                if (strlen(arqc) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE301:: arqc can not be null!\n");
                        UnionSetResponseRemark("待验证的TC/ARQC/ACC不能为空!");
                        return(errCodeParameter);
                }
                if (!UnionIsBCDStr(arqc))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE301:: arqc[%s] is error!\n",arqc);
                        UnionSetResponseRemark("待验证的TC/ARQC/ACC[%s]非法,必须为十六进制数",arqc);
                        return(errCodeParameter);
                }
        }
/*
	if ((ret = UnionReadRequestXMLPackageValue("body/iccType", iccTypeTmp, sizeof(iccTypeTmp)))< 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE301:: UnionReadRequestXMLPackageValue[%s]!\n","body/iccType");
		return(ret);
	}
	iccTypeTmp[ret] = 0;
	iccType = atoi(iccTypeTmp);
*/
        // 读取对称密钥
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName, 1, &symmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE301:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", keyName);
                return(ret);
        }

	// 读取对称密钥值
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE301:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("读取密钥值失败");
		return(errCodeParameter);
	}


	//modify begin  by lusj 20151111  由于国密算法没有用到卡类型。所以当输入的密钥为SM4时，可不送入卡类型
	if(symmetricKeyDB.algorithmID == conSymmetricAlgorithmIDOfDES)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/iccType", iccTypeTmp, sizeof(iccTypeTmp)))< 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE301:: UnionReadRequestXMLPackageValue[%s]!\n","body/iccType");
			return(ret);
		}
		iccTypeTmp[ret] = 0;
		iccType = atoi(iccTypeTmp);
	}
	//modify end  by lusj 20151111


	switch(symmetricKeyDB.algorithmID)
	{
		case    conSymmetricAlgorithmIDOfDES:   // DES算法
			switch(phsmGroupRec->hsmCmdVersionID)
        		{
                		case conHsmCmdVerRacalStandardHsmCmd:
                		case conHsmCmdVerSJL06StandardHsmCmd:
					if(iccType == 0)
					{
						if ( (ret =  UnionHsmCmdKX("0", "9", 0, psymmetricKeyValue->keyValue, NULL, 0, pan, NULL, atc, strlen(arqcData), arqcData, arqc, NULL, NULL, 0, NULL, NULL)) < 0)
						{
							if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) &&		\
								(UnionOldVersionSymmetricKeyIsUsed(&symmetricKeyDB)))
							{
								if ( (ret =  UnionHsmCmdKX("0", "9", 0, psymmetricKeyValue->oldKeyValue, NULL, 0, pan, NULL, atc, strlen(arqcData), arqcData, arqc, NULL, NULL, 0, NULL, NULL)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE301:: UnionHsmCmdKX old return=[%d]!\n", ret);
									UnionGetResponseRemarkByResponseCodeE301(ret);
									return(ret);
								}
							}
							else
							{
								UnionUserErrLog("in UnionDealServiceCodeE301:: UnionHsmCmdKX return=[%d]!\n", ret);
								UnionGetResponseRemarkByResponseCodeE301(ret);
								return(ret);
							}
						}
					}
					else if(iccType == 1)
					{
						if ( (ret =  UnionHsmCmdKX("5", "4", 0, psymmetricKeyValue->keyValue, NULL, 0, pan, NULL, atc, strlen(arqcData), arqcData, arqc, NULL, NULL, 0, NULL, NULL)) < 0)
						{
							if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) &&		\
								(UnionOldVersionSymmetricKeyIsUsed(&symmetricKeyDB)))
							{
								if ( (ret =  UnionHsmCmdKX("5", "4", 0, psymmetricKeyValue->oldKeyValue, NULL, 0, pan, NULL, atc, strlen(arqcData), arqcData, arqc, NULL, NULL, 0, NULL, NULL)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE301:: UnionHsmCmdKX old return=[%d]!\n", ret);
									UnionGetResponseRemarkByResponseCodeE301(ret);
									return(ret);
								}
							}
							else
							{
								UnionUserErrLog("in UnionDealServiceCodeE301:: UnionHsmCmdKX return=[%d]!\n", ret);
								UnionGetResponseRemarkByResponseCodeE301(ret);
								return(ret);
							}
						}
					}
					else
					{
						UnionUserErrLog("in UnionDealServiceCodeE301:: iccType =[%d] error!\n", iccType);
						return errCodeOffsetOfPBOC_iccTypeERR;
					}
					break;
                		default:
                        		UnionUserErrLog("in UnionDealServiceCodeE301:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        		UnionSetResponseRemark("非法的加密机指令类型");
                        		return(errCodeParameter);
        		}
			break;

		case    conSymmetricAlgorithmIDOfSM4:   // SM4算法
			if ((ret = UnionHsmCmdWG("0","0",0,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue,strlen(pan),pan,atc,strlen(arqcData),arqcData,arqc,NULL,NULL)) < 0)
			{

				if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) &&		\
					(UnionOldVersionSymmetricKeyIsUsed(&symmetricKeyDB)))
				{
					if ((ret = UnionHsmCmdWG("0","0",0,psymmetricKeyValue->oldKeyValue,symmetricKeyDB.oldCheckValue,strlen(pan),pan,atc,strlen(arqcData),arqcData,arqc,NULL,NULL)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE301:: UnionHsmCmdWG old version ret = [%d]\n",ret);
						UnionGetResponseRemarkByResponseCodeE301(ret);
						return(ret);
					}
				}
				else
				{
					UnionUserErrLog("in UnionDealServiceCodeE301:: UnionHsmCmdWG ret = [%d]\n",ret);
					UnionGetResponseRemarkByResponseCodeE301(ret);
					return(ret);
				}
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE301:: key [%s] algorithmID = [%d] illegal!\n", symmetricKeyDB.keyName, symmetricKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	return(0);
}

void UnionGetResponseRemarkByResponseCodeE301(int retCode)
{
	switch(retCode)
	{
		case	errCodeOffsetOfHsmReturnCodeMDL-1:
			UnionSetResponseRemark("ARQC/TC/AAC校验失败/密钥校验值错");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-4:
			UnionSetResponseRemark("模式标志错");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-5:
			UnionSetResponseRemark("未定义的方案ID");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-10:
			UnionSetResponseRemark("MK-AC奇偶校验错");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-12:
			UnionSetResponseRemark("用户存储区没有装载密钥");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-13:
			UnionSetResponseRemark("LMK错误");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-15:
			UnionSetResponseRemark("输入数据错");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-33:
			UnionSetResponseRemark("密钥索引错");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-36:
			UnionSetResponseRemark("算法标志错");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-52:
			UnionSetResponseRemark("非法的B/H选择");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-80:
			UnionSetResponseRemark("数据长度错");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-81:
			UnionSetResponseRemark("PAN长度错");
			break;
	}
}
