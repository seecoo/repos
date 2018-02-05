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
#include "commWithHsmSvr.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "symmetricKeyDB.h"
#include "3DesRacalSyntaxRules.h"
#include "unionHsmCmdVersion.h"

/***************************************
服务代码:       E603
服务名:         使用离散密钥转加密PIN
功能描述:       使用离散密钥转加密PIN
***************************************/
int UnionDealServiceCodeE603(PUnionHsmGroupRec phsmGroupRec)
{
        int                     ret = 0;
        char                    inScheme[1+1];
        char                    inKeyFlag[1+1];
        char                    inKeyIndex[4+1];
        char                    inDiscreteNum[1+1];
        char                    inDiscreteData[48+1];
        char                    inGC_Data[48+1];
        int                     inKeyLenFlag = 0;
        char                    outScheme[1+1];
        char                    outKeyFlag[1+1];
        char                    outKeyIndex[4+1];
        char                    outDiscreteNum[1+1];
        char                    outDiscreteData[48+1];
        char                    outGC_Data[48+1];
        int                     outKeyLenFlag = 0;
        char                    oriAccNo[32+1];
        char                    desAccNo[32+1];
        char                    accNo[32+1];
        char                    oriPIN[2+1];
        char                    desPIN[2+1];
        char                    oriPINBlock[48];
        char                    desPINBlock[48];
     // char                    pinBlock[48];
	char			keyName[136];
	char			keyValue[48];
	char			checkValue[32];

        TUnionSymmetricKeyDB    inKeyDB;
	PUnionSymmetricKeyValue	pinKeyValue = NULL;
        TUnionSymmetricKeyDB    outKeyDB;
	PUnionSymmetricKeyValue	poutKeyValue = NULL;

        // 导入方案
        memset(inScheme,0,sizeof(inScheme));
        if ((ret = UnionReadRequestXMLPackageValue("body/inScheme",inScheme,sizeof(inScheme))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/inScheme");
                return(ret);
        }

        if (inScheme[0] == '1')
        {
                // 过程数据(导入)
                memset(inGC_Data,0,sizeof(inGC_Data));
                if ((ret = UnionReadRequestXMLPackageValue("body/inGC_Data",inGC_Data,sizeof(inGC_Data))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/inGC_Data");
                        return(ret);
                }
                else
                {
                        UnionFilterHeadAndTailBlank(inGC_Data);
                        if (strlen(inGC_Data) > 0)
                        {
                                if (!UnionIsBCDStr(inGC_Data))
                                {
                                        UnionUserErrLog("in UnionDealServiceCodeE603:: inGC_Data[%s] is error!\n",inGC_Data);
                                        UnionSetResponseRemark("过程数据（导入）非法,必须为十六进制数");
                                        return(errCodeParameter);
                                }
                        }
                }
        }

        // 导入密钥标志
        memset(inKeyFlag,0,sizeof(inKeyFlag));
        if ((ret = UnionReadRequestXMLPackageValue("body/inKeyFlag",inKeyFlag,sizeof(inKeyFlag))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/inKeyFlag");
                return(ret);
        }

        if (inKeyFlag[0] == '0')
        {
                // 加密机内部导入密钥
                memset(inKeyIndex,0,sizeof(inKeyIndex));
                if ((ret = UnionReadRequestXMLPackageValue("body/inKeyIndex",inKeyIndex,sizeof(inKeyIndex))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/inKeyIndex");
                        return(ret);
                }
        }
		
	if (inKeyFlag[0] == '1')
	{
		// 导入密钥名称
		memset(&inKeyDB,0,sizeof(inKeyDB));
		if ((ret = UnionReadRequestXMLPackageValue("body/inKeyName",keyName,sizeof(keyName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/inKeyName");
			return(ret);
		}
		keyName[ret] = 0;
		UnionFilterHeadAndTailBlank(keyName); 

		// 读取密钥
		if ((ret = UnionReadSymmetricKeyDBRec(keyName,1,&inKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
			return(ret);
		}

		// 读取密钥值
		if ((pinKeyValue = UnionGetSymmetricKeyValue(&inKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE603:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("读取导入密钥值失败");
			return(errCodeParameter);
		}

		// 获取密钥长度标识
		switch(inKeyDB.keyLen)
		{
			case    con64BitsSymmetricKey:
					inKeyLenFlag = 0;
					break;
			case    con128BitsSymmetricKey:
					inKeyLenFlag = 1;
					break;
			case    con192BitsSymmetricKey:
					inKeyLenFlag = 2;
					break;
			default :
					inKeyLenFlag = 1;
		}
	}
		
        // 密钥离散次数(导入)
        memset(inDiscreteNum,0,sizeof(inDiscreteNum));
        if ((ret = UnionReadRequestXMLPackageValue("body/inDiscreteNum",inDiscreteNum,sizeof(inDiscreteNum))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/inDiscreteNum");
                return(ret);
        }

	if (inDiscreteNum[0] == '1' || inDiscreteNum[0] == '2' || inDiscreteNum[0] == '3')
	{	
		// 密钥离散数据(导入)
		memset(inDiscreteData,0,sizeof(inDiscreteData));
		if ((ret = UnionReadRequestXMLPackageValue("body/inDiscreteData",inDiscreteData,sizeof(inDiscreteData))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/inDiscreteData");
			return(ret);
		}
		else
		{
			UnionFilterHeadAndTailBlank(inDiscreteData);
			if (strlen(inDiscreteData) > 0)
			{
				if (!UnionIsBCDStr(inDiscreteData))
				{
					UnionUserErrLog("in UnionDealServiceCodeE603:: inDiscreteData[%s] is error!\n",inDiscreteData);
					UnionSetResponseRemark("密钥离散数据(导入)非法,必须为十六进制数");
					return(errCodeParameter);
				}
			}
		}
	}


        // 导出方案
        memset(outScheme,0,sizeof(outScheme));
        if ((ret = UnionReadRequestXMLPackageValue("body/outScheme",outScheme,sizeof(outScheme))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/outScheme");
                return(ret);
        }

        if (outScheme[0] == '1')
        {
                // 过程数据(导出)
                memset(outGC_Data,0,sizeof(outGC_Data));
                if ((ret = UnionReadRequestXMLPackageValue("body/outGC_Data",outGC_Data,sizeof(outGC_Data))) < 0)
                {
                        UnionUserErrLog("out UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/outGC_Data");
                        return(ret);
                }
                else
                {
                        UnionFilterHeadAndTailBlank(outGC_Data);
                        if (strlen(outGC_Data) > 0)
                        {
                                if (!UnionIsBCDStr(outGC_Data))
                                {
                                        UnionUserErrLog("in UnionDealServiceCodeE603:: outGC_Data[%s] is error!\n",outGC_Data);
                                        UnionSetResponseRemark("过程数据（导出）非法,必须为十六进制数");
                                        return(errCodeParameter);
                                }
                        }
                }
        }

        // 导出密钥标志
        memset(outKeyFlag,0,sizeof(outKeyFlag));
        if ((ret = UnionReadRequestXMLPackageValue("body/outKeyFlag",outKeyFlag,sizeof(outKeyFlag))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/outKeyFlag");
                return(ret);
        }

        if (outKeyFlag[0] == '0')
        {
                // 加密机内部导出密钥
                memset(outKeyIndex,0,sizeof(outKeyIndex));
                if ((ret = UnionReadRequestXMLPackageValue("body/outKeyIndex",outKeyIndex,sizeof(outKeyIndex))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/outKeyIndex");
                        return(ret);
                }
        }

	if (outKeyFlag[0] == '1')
	{
		// 密钥名称
		memset(&outKeyDB,0,sizeof(outKeyDB));
		if ((ret = UnionReadRequestXMLPackageValue("body/outKeyName",keyName,sizeof(keyName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/outKeyName");
			return(ret);
		}
		keyName[ret] = 0;
		UnionFilterHeadAndTailBlank(keyName); 

		// 读取密钥
		if ((ret = UnionReadSymmetricKeyDBRec(keyName,1,&outKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
			return(ret);
		}

		// 读取密钥值
		if ((poutKeyValue = UnionGetSymmetricKeyValue(&outKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE603:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("读取导出密钥值失败");
			return(errCodeParameter);
		}

		// 获取密钥长度标识
		switch(outKeyDB.keyLen)
		{
			case    con64BitsSymmetricKey:
					outKeyLenFlag = 0;
					break;
			case    con128BitsSymmetricKey:
					outKeyLenFlag = 1;
					break;
			case    con192BitsSymmetricKey:
					outKeyLenFlag = 2;
					break;
			default :
					outKeyLenFlag = 1;
		}
	}

        // 密钥离散次数(导出)
        memset(outDiscreteNum,0,sizeof(outDiscreteNum));
        if ((ret = UnionReadRequestXMLPackageValue("body/outDiscreteNum",outDiscreteNum,sizeof(outDiscreteNum))) < 0)
        {
                UnionUserErrLog("out UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/outDiscreteNum");
                return(ret);
        }

        if (outDiscreteNum[0] == '1' || outDiscreteNum[0] == '2' || outDiscreteNum[0] == '3')
        {
                // 密钥离散数据(导出)
                memset(outDiscreteData,0,sizeof(outDiscreteData));
                if ((ret = UnionReadRequestXMLPackageValue("body/outDiscreteData",outDiscreteData,sizeof(outDiscreteData))) < 0)
                {
                        UnionUserErrLog("out UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/outDiscreteData");
                        return(ret);
                }
                else
                {
                        UnionFilterHeadAndTailBlank(outDiscreteData);
                        if (strlen(outDiscreteData) > 0)
                        {
                                if (!UnionIsBCDStr(outDiscreteData))
                                {
                                        UnionUserErrLog("in UnionDealServiceCodeE603:: outDiscreteData[%s] is error!\n",outDiscreteData);
                                        UnionSetResponseRemark("密钥离散数据(导出)[%s]非法,必须为十六进制数",outDiscreteData);
                                        return(errCodeParameter);
                                }
                        }
                }
        }
	

        // 源帐号
        memset(oriAccNo,0,sizeof(oriAccNo));
        if ((ret = UnionReadRequestXMLPackageValue("body/oriAccNo",oriAccNo,sizeof(oriAccNo))) < 0)
        {
                UnionUserErrLog("out UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/oriAccNo");
                return(ret);
        }

        // 目的帐号
        memset(desAccNo,0,sizeof(desAccNo));
        if ((ret = UnionReadRequestXMLPackageValue("body/desAccNo",desAccNo,sizeof(desAccNo))) < 0)
        {
                UnionUserErrLog("out UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/desAccNo");
                return(ret);
        }

        // 源pin块格式
        memset(oriPIN,0,sizeof(oriPIN));
        if ((ret = UnionReadRequestXMLPackageValue("body/oriPIN",oriPIN,sizeof(oriPIN))) < 0)
        {
                UnionUserErrLog("out UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/oriPIN");
                return(ret);
        }
        else
        {
                UnionFilterHeadAndTailBlank(oriPIN);
                if (strlen(oriPIN) > 0)
                {
                        if (!UnionIsDigitString(oriPIN))
                        {
                                UnionUserErrLog("in UnionDealServiceCodeE603:: oriPIN[%s] is error!\n",oriPIN);
                                UnionSetResponseRemark("源pin块格式[%s]非法,必须为数字",oriPIN);
                                return(errCodeParameter);
                        }
                }
                else
                {
                        UnionUserErrLog("in UnionDealServiceCodeE603:: oriPIN can not be null!\n");
                        UnionSetResponseRemark("源pin块格式不能为空");
                        return(errCodeParameter);
                }
		
		if (strcmp(oriPIN,"01") != 0 && strcmp(oriPIN,"03") != 0 && strcmp(oriPIN,"13") != 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE603:: oriPIN[%s] is error!\n",oriPIN);
                        UnionSetResponseRemark("源pin块格式[%s]非法,数据不符合要求",oriPIN);
                        return(errCodeParameter);
		}
        }

        // 目的pin块格式
        memset(desPIN,0,sizeof(desPIN));
        if ((ret = UnionReadRequestXMLPackageValue("body/desPIN",desPIN,sizeof(desPIN))) < 0)
        {
                UnionUserErrLog("out UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/desPIN");
                return(ret);
        }
        else
        {
                UnionFilterHeadAndTailBlank(desPIN);
                if(strlen(desPIN) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE603:: desPIN can not be null!\n");
                        UnionSetResponseRemark("目的pin块格式不能为空");
                        return(errCodeParameter);
                }
                if (!UnionIsDigitString(desPIN))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE603:: desPIN[%s] is error!\n",desPIN);
                        UnionSetResponseRemark("目的pin块格式[%s]非法,必须为数字",desPIN);
                        return(errCodeParameter);
                }
		if (strcmp(desPIN,"01") != 0 && strcmp(desPIN,"03") != 0 && strcmp(desPIN,"13") != 0 && strcmp(desPIN,"15") != 0 && strcmp(desPIN,"16") != 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE603:: desPIN[%s] is error!\n",desPIN);
                        UnionSetResponseRemark("目的pin块格式[%s]非法,数据不符合要求",desPIN);
                        return(errCodeParameter);
		}
        }

        // 目的全账号
        if (memcmp(desPIN,"16",2) == 0)
        {
                memset(accNo,0,sizeof(accNo));
                if ((ret = UnionReadRequestXMLPackageValue("body/accNo",accNo,sizeof(accNo))) < 0)
                {
                                UnionUserErrLog("out UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
                                return(ret);
                }
        }

        // 源pin密文
        memset(oriPINBlock,0,sizeof(oriPINBlock));
        if ((ret = UnionReadRequestXMLPackageValue("body/oriPINBlock",oriPINBlock,sizeof(oriPINBlock))) < 0)
        {
                UnionUserErrLog("out UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/oriPINBlock");
                return(ret);
        }
        else
        {
                UnionFilterHeadAndTailBlank(oriPINBlock);
                if(strlen(oriPINBlock) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE603:: oriPINBlock can not be null!\n");
                        UnionSetResponseRemark("源pin密文不能为空");
                        return(errCodeParameter);
                }
                if (!UnionIsBCDStr(oriPINBlock))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE603:: oriPINBlock[%s] is error!\n",oriPINBlock);
                        UnionSetResponseRemark("源pin密文[%s]非法,必须为十六进制数",oriPINBlock);
                        return(errCodeParameter);
                }
        }

        memset(desPINBlock,0,sizeof(desPINBlock));
        switch(inKeyDB.algorithmID)
        {
                case    conSymmetricAlgorithmIDOfDES:
                        switch(phsmGroupRec->hsmCmdVersionID)
                        {
                                case conHsmCmdVerRacalStandardHsmCmd:
                                case conHsmCmdVerSJL06StandardHsmCmd:
                                        if ((ret = UnionHsmCmdN4(atoi(inScheme),atoi(inKeyFlag),inKeyIndex,atoi(inDiscreteNum),inDiscreteData,inGC_Data,
                                                                inKeyLenFlag,inKeyDB.keyValue[0].keyValue,atoi(outScheme),atoi(outKeyFlag),outKeyIndex,atoi(outDiscreteNum),outDiscreteData,
                                                                outGC_Data,outKeyLenFlag,outKeyDB.keyValue[0].keyValue,12,oriAccNo,desAccNo,atoi(oriPIN),atoi(desPIN),accNo,oriPINBlock,desPINBlock)) < 0)
                                        {
                                                UnionUserErrLog("in UnionDealServiceCodeE603:: UnionHsmCmdN4!\n");
                                                return(ret);
                                        }
                                        break;
                                default:
                                        UnionUserErrLog("in UnionDealServiceCodeE603:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                                        UnionSetResponseRemark("非法的加密机指令类型");
                                        return(errCodeParameter);
                        }
                        break;
                case    conSymmetricAlgorithmIDOfSM4:
			if ((ret = UnionHsmCmdWE("02",inKeyDB.keyType,pinKeyValue->keyValue,inKeyDB.checkValue,0,inKeyDB.keyType,"1",inDiscreteData,"",keyValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE603:: UnionHsmCmdWE !\n");
				return(ret);
			}
			keyValue[ret] = 0;
			if ((ret = UnionHsmCmdBU(1,inKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(inKeyDB.keyLen),keyValue,checkValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE603:: UnionHsmCmdBU !\n");
				return(ret);
			}
			if ((ret = UnionHsmCmdWE("02",inKeyDB.keyType,keyValue,checkValue,0,conZPK,"1",inGC_Data,"",keyValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE603:: UnionHsmCmdWE !\n");
				return(ret);
			}
			keyValue[ret] = 0;
			
			//modify by lusj 20160111调用两次的WX实现的功能与调用一次的W8指令一致，并且在川农信客户使用发现调用两次WX指令后得到的PIN块解密的明文已经改变� 
			/*
			if ((ret = UnionHsmCmdWX(1,keyValue,oriPINBlock,oriPIN,oriAccNo,strlen(oriAccNo),pinBlock,sizeof(pinBlock))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE603:: UnionHsmCmdWX !\n");
				return(ret);
			}
			if ((ret = UnionHsmCmdWX(0,poutKeyValue->keyValue,pinBlock,desPIN,desAccNo,strlen(desAccNo),desPINBlock,sizeof(desPINBlock))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE603:: UnionHsmCmdWX !\n");
				return(ret);
			}
			*/
			if ((ret = UnionHsmCmdW8(2,2,2,NULL,keyValue,2,NULL,poutKeyValue->keyValue,oriPIN,desPIN,oriPINBlock,oriAccNo,desAccNo,desPINBlock,sizeof(desPINBlock))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE603:: UnionHsmCmdW8 !\n");
				return(ret);
			}
			//end by lusj 	

                        break;
                default:
                        UnionUserErrLog("in UnionDealServiceCodeE603:: inKeyDB.algorithmID[%d] error!\n",inKeyDB.algorithmID);
                        return(errCodeEsscMDL_InvalidAlgorithmID);
        }

        // 目标pin密文
        if ((ret = UnionSetResponseXMLPackageValue("body/desPINBlock",desPINBlock)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE603:: UnionSetResponseXMLPackageValue[%s]","body/desPINBlock");
                return(ret);
        }

	return(0);
}
