//      Author:         张永定
//      Copyright:      Union Tech. Guangzhou
//      Date:           2013-01-10

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
服务代码:       E304
服务名:         E304脚本数据计算mac(528)
功能描述:       E304脚本数据计算mac(528)
***************************************/

int UnionDealServiceCodeE304(PUnionHsmGroupRec phsmGroupRec)
{
	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;

	int			len;
        char                    pan[32];
        char                    atc[32];	// modify by leipp 3.x中是送16位长度,现从4改为32
        char                    plainData[4096];
        char                    versionTmp[16];
        char                    appID[48];
        char                    ownerName[48];
        char                    keyType[48];
        char                    mac[48];
	char 			keyTypeStr[8];
	char			ivMac[48];
	char			keyName[136];

        int                     ret;
        int                     version;

        memset(appID,           0,      sizeof(appID));
        memset(ownerName,       0,      sizeof(ownerName));
        memset(keyType,         0,      sizeof(keyType));
        memset(mac,             0,      sizeof(mac));
        memset(keyTypeStr,      0,      sizeof(keyTypeStr));
	
        // mk-ac密钥名称
        if ((ret = UnionReadRequestXMLPackageValue("body/keyName", keyName, sizeof(keyName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE304:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
                return(ret);
        }

        // .....读其他请求域 .......
        if ((ret = UnionReadRequestXMLPackageValue("body/version", versionTmp, sizeof(versionTmp))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE304:: UnionReadRequestXMLPackageValue[%s]!\n","body/version");
                return(ret);
        }
        else
        {
		UnionFilterHeadAndTailBlank(versionTmp);
                if (strlen(versionTmp) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE304:: versionTmp can not be null!\n");
                        UnionSetResponseRemark("密钥的版本号不能为空!");
                        return(errCodeParameter);
                }
                if (!UnionIsDigitString(versionTmp))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE304:: version[%s] is error!\n",versionTmp);
                        UnionSetResponseRemark("密钥的版本号[%s]非法,必须为数字",versionTmp);
                        return(errCodeParameter);
                }
                version = atoi(versionTmp);
        }


        UnionAnalysisPBOCSymmetricKeyName(keyName, version, appID, ownerName, keyType);

        snprintf(keyName,sizeof(keyName), "%s.%s.%s", appID, ownerName, keyType);

        if ((ret = UnionReadRequestXMLPackageValue("body/pan",  pan, sizeof(pan)))< 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE304:: UnionReadRequestXMLPackageValue[%s]!\n","body/pan");
                return(ret);
        }
        else
        {
		UnionFilterHeadAndTailBlank(pan);
                if ((len = strlen(pan)) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE304:: pan can not be null!\n");
                        UnionSetResponseRemark("卡号或（卡号+卡序列号）不能为空!");
                        return(errCodeParameter);
                }
		else if (len < 16)
		{
                        UnionUserErrLog("in UnionDealServiceCodeE304:: pan len[%d] < 16!\n",len);
                        UnionSetResponseRemark("卡号或（卡号+卡序列号）长度小于16，不正确!");
                        return(errCodeParameter);
		}
		else if (len > 16)
		{
                        UnionLog("in UnionDealServiceCodeE304:: pan len[%d] > 16,取最后16位,new pan[%s]!\n",len,pan + len - 16);
			memmove(pan,pan + len - 16,16);
			pan[16] = 0;
		}

                if (!UnionIsBCDStr(pan))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE304:: pan[%s] is error!\n",pan);
                        UnionSetResponseRemark("  卡号或（卡号+卡序列号）[%s]非法,必须为十六进制数",pan);
                        return(errCodeParameter);
                }
        }

        if ((ret = UnionReadRequestXMLPackageValue("body/atc", atc, sizeof(atc)))< 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE304:: UnionReadRequestXMLPackageValue[%s]!\n","body/atc");
                return(ret);
        }
        else
        {
		UnionFilterHeadAndTailBlank(atc);
                if (!UnionIsBCDStr(atc))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE304:: atc[%s] is error!\n",atc);
                        UnionSetResponseRemark("离散过程因子[%s]非法,必须为十六进制数",atc);
                        return(errCodeParameter);
                }
        }

        if ((ret = UnionReadRequestXMLPackageValue("body/plainData", plainData, sizeof(plainData)))< 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE304:: UnionReadRequestXMLPackageValue[%s]!\n","body/plainData");
                return(ret);
        }
        else
        {
		plainData[ret] = 0;
		UnionFilterHeadAndTailBlank(plainData);
                if (strlen(plainData) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE304:: plainData can not be null!\n");
                        UnionSetResponseRemark("明文数据不能为空!");
                        return(errCodeParameter);
                }		
                if (!UnionIsBCDStr(plainData))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE304:: plainData[%s] is error!\n",plainData);
                        UnionSetResponseRemark("明文数据非法,必须为十六进制数");
                        return(errCodeParameter);
                }
        }

        //判断参数是否合法
	if ((strlen(atc) != 4) && (strlen(atc) != 16))
	{
		UnionUserErrLog("in UnionDealServiceCodeE304:: strlen(atc)=[%zu] err!", strlen(atc));
		return (errCodeParameter);
	}

        // 读取对称密钥
        if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1, &symmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE304:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", keyName);
                return(ret);
        }

	// 读取对称密钥值
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE304:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("读取密钥值失败");
		return(errCodeParameter);
	}

	UnionTranslateDesKeyTypeTo3CharFormat(symmetricKeyDB.keyType, keyTypeStr);

        switch(symmetricKeyDB.algorithmID)
        {
                case    conSymmetricAlgorithmIDOfDES:   // DES算法
			switch(phsmGroupRec->hsmCmdVersionID)
        		{
                		case conHsmCmdVerRacalStandardHsmCmd:
                		case conHsmCmdVerSJL06StandardHsmCmd:
                        		if ((ret = UnionHsmCmdUB("1", "3", keyTypeStr, psymmetricKeyValue->keyValue, 0, 1, pan, atc, "1", NULL, strlen(plainData), plainData, "1", NULL, mac)) < 0)
                        		{
                                		UnionUserErrLog("in UnionDealServiceCodeE304:: UnionHsmCmdUB return:[%d]!\n", ret);
                                		return ret;
                        		}
					break;
                		default:
                        		UnionUserErrLog("in UnionDealServiceCodeE304:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        		UnionSetResponseRemark("非法的加密机指令类型");
                        		return(errCodeParameter);
        		}
                        break;
                case    conSymmetricAlgorithmIDOfSM4:   // SM4算法
			memset(ivMac,'0',sizeof(ivMac));
			ivMac[32] = 0;
			switch(phsmGroupRec->hsmCmdVersionID)
        		{
                		case conHsmCmdVerRacalStandardHsmCmd:
                		case conHsmCmdVerSJL06StandardHsmCmd:
                        		if ((ret = UnionHsmCmdWD("1", "4", keyTypeStr, psymmetricKeyValue->keyValue, symmetricKeyDB.checkValue, 0,1, pan, atc, "1", ivMac, strlen(plainData), plainData, "1", NULL, mac)) < 0)
                        		{
                                		UnionUserErrLog("in UnionDealServiceCodeE304:: UnionHsmCmdWD return:[%d]!\n", ret);
                                		return ret;
                        		}
					break;
                		default:
                        		UnionUserErrLog("in UnionDealServiceCodeE304:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        		UnionSetResponseRemark("非法的加密机指令类型");
                        		return(errCodeParameter);
        		}
			break;
                default:
                        UnionUserErrLog("in UnionDealServiceCodeE304:: key [%s] algorithmID = [%d] illegal!\n", symmetricKeyDB.keyName, symmetricKeyDB.algorithmID);
                        return(errCodeEsscMDL_InvalidAlgorithmID);
        }

        //设置mac
        if ((ret = UnionSetResponseXMLPackageValue("body/mac", mac)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE302:: UnionSetResponseXMLPackageValue mac[%s]!\n", mac);
                return(ret);
        }

        return(0);
}
