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
服务代码:       E602
服务名:         使用离散密钥加解密数据或计算MAC
功能描述:       使用离散密钥加解密数据或计算MAC
***************************************/

static int froadAllPackageEncryData(int encMode,PUnionSymmetricKeyDB psymmetricKeyDB,char *keyValue,char *discreteData,char *processFlag,char *processData,int lenOfData,char *data,char *encData);
static int froadOneWayEncryData(int encMode,PUnionSymmetricKeyDB psymmetricKeyDB,char *keyValue,char *discreteData,char *processData,int lenOfData,char *data,char *encData);
static int froadGenerateMac(PUnionSymmetricKeyDB psymmetricKeyDB,char *keyValue,char *keyDiscreteData,int lenOfData,char *data,char *mac);
int UnionDealServiceCodeE602(PUnionHsmGroupRec phsmGroupRec)
{
        int                     ret = 0;
        char                    keyDiscreteData[128];
        char                    mac[48];
        char                    data[2048];
        char                    data1[2048];
        int                     keyDiscreteNum = 0;
        int                     lenOfData = 0;
        char                    keyIndex[8];
        char                    GC_Data[128];
        char                    algorithmID[8];
        char                    encryptFlag[8];
        char                    encryptAlgorithmID[8];
        char                    macFlag[8];
        char                    macAlgorithmID[8];
        char                    keyType[8];
        char                    tmpBuf[128];
	char			keyName[132];
	char			processFlag[8];
 	int			encryptAlgorithmID_flag=-1; // modify by lusj begin 20150909 ，18指令接口函数要判断 加密算法选择 标识为0或1 时是否对报文数据进行16个0填充

        TUnionSymmetricKeyDB    symmetricKeyDB;
	PUnionSymmetricKeyValue	psymmetricKeyValue = NULL;

        memset(keyIndex,0,sizeof(keyIndex));
        // 密钥索引
        if ((ret = UnionReadRequestXMLPackageValue("body/keyIndex",keyIndex,sizeof(keyIndex))) < 0)
        {
                // 密钥名称
                memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));
                if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE602:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
                        return(ret);
                }
		keyName[ret] = 0;
                UnionFilterHeadAndTailBlank(keyName); 
				
		// 读取密钥
                if ((ret = UnionReadSymmetricKeyDBRec(keyName,1,&symmetricKeyDB)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE602:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
                        return(ret);
                }

		// 读取密钥值
		if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE602:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
			UnionSetResponseRemark("读取密钥值失败");
			return(errCodeParameter);
		}

                // 获取根密钥类型
                memset(keyType,0,sizeof(keyType));
                if ((ret = UnionTranslateHsmKeyTypeString(symmetricKeyDB.keyType,keyType)) < 0) 
                {
                        UnionUserErrLog("in UnionDealServiceCodeE602:: UnionTranslateHsmKeyTypeString[%d]!\n",symmetricKeyDB.keyType);
                        return(ret);
                }
		sprintf(keyIndex,"T%s",keyType);
        }
	else
	{
		if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE602:: keyIndex[%s] is error!\n",keyIndex);
			UnionSetResponseRemark("密钥索引不能为空");
			return(errCodeParameter);
		}
		
		UnionFilterHeadAndTailBlank(keyIndex);
		if ((keyIndex[0] == 'T' && strlen(keyIndex) == 4))
		{
			// 密钥名称
			memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));
			if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE602:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
				return(ret);
			}
			keyName[ret] = 0;
			UnionFilterHeadAndTailBlank(keyName); 

			// 读取密钥
			if ((ret = UnionReadSymmetricKeyDBRec(keyName,1,&symmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE602:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
				return(ret);
			}	
		}
	}

        // 密钥离散数据
        memset(keyDiscreteData,0,sizeof(keyDiscreteData));
        if ((ret = UnionReadRequestXMLPackageValue("body/keyDiscreteData",keyDiscreteData,sizeof(keyDiscreteData))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE602:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyDiscreteData");
                return(ret);
        }
        else
        {
                UnionFilterHeadAndTailBlank(keyDiscreteData);
                if (strlen(keyDiscreteData) > 0)
                {
                        if (!UnionIsBCDStr(keyDiscreteData))
                        {
                                UnionUserErrLog("in UnionDealServiceCodeE602:: keyDiscreteData[%s] is error!\n",keyDiscreteData);
                                UnionSetResponseRemark("密钥离散数据非法,必须为十六进制数");
                                return(errCodeParameter);
                        }
                }
		keyDiscreteNum = strlen(keyDiscreteData) / 16;
        }

	// MAC标志
        memset(macFlag,0,sizeof(macFlag));
        if ((ret = UnionReadRequestXMLPackageValue("body/macFlag",macFlag,sizeof(macFlag))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE602:: UnionReadRequestXMLPackageValue[%s]!\n","body/macFlag");
                return(ret);
        }

	if (symmetricKeyDB.algorithmID == conSymmetricAlgorithmIDOfDES)	// 只有DES存在此域
	{
		// 当MAC标志为1或2时存在
		if (macFlag[0] == '1' || macFlag[0] == '2')
		{
			// MAC算法选择
			memset(macAlgorithmID,0,sizeof(macAlgorithmID));
			if ((ret = UnionReadRequestXMLPackageValue("body/macAlgorithmID",macAlgorithmID,sizeof(macAlgorithmID))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE602:: UnionReadRequestXMLPackageValue[%s]!\n","body/macAlgorithmID");
				return(ret);
			}
		}
	}

	if (symmetricKeyDB.algorithmID == conSymmetricAlgorithmIDOfDES)
	{
		// 密钥离散次数
		if ((ret = UnionReadRequestXMLPackageValue("body/keyDiscreteNum",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE602:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyDiscreteNum");
			return(ret);
		}
		else
		{
			tmpBuf[ret] = 0;
			keyDiscreteNum = atoi(tmpBuf);
		}

		if (keyDiscreteNum < 0 || keyDiscreteNum > 2)
		{
			UnionUserErrLog("in UnionDealServiceCodeE602:: keyDiscreteNum [%d] not between 0 and 2!\n",keyDiscreteNum);
			UnionSetResponseRemark("密钥离散次数[%d]不在0到2之间",keyDiscreteNum);
			return(errCodeParameter);
		}

		// 过程密钥算法标志
		memset(algorithmID,0,sizeof(algorithmID));
		if ((ret = UnionReadRequestXMLPackageValue("body/algorithmID",algorithmID,sizeof(algorithmID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE602:: UnionReadRequestXMLPackageValue[%s]!\n","body/algorithmID");
			return(ret);
		}
	}
	else
	{
		if (macFlag[0] != '0')
			snprintf(algorithmID,sizeof(algorithmID),"4");
		else
			snprintf(algorithmID,sizeof(algorithmID),"1");
	}

	if (algorithmID[0] != '4')
	{
		// GC_Data
		memset(GC_Data,0,sizeof(GC_Data));
		ret = UnionReadRequestXMLPackageValue("body/GC_Data",GC_Data,sizeof(GC_Data));
		if ((ret < 0) && (encryptAlgorithmID_flag != 6))
		{
			UnionUserErrLog("in UnionDealServiceCodeE602:: UnionReadRequestXMLPackageValue[%s]!\n","body/GC_Data");
			return(ret);
		}
		else if ((ret <= 0) && (encryptAlgorithmID_flag == 6))
		{
			snprintf(processFlag,sizeof(processFlag),"N");
		}
		else
		{
			snprintf(processFlag,sizeof(processFlag),"Y");
			UnionFilterHeadAndTailBlank(GC_Data);
			if(strlen(GC_Data) == 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE602:: GC_Data can not be null!\n");
				UnionSetResponseRemark("GC_Data不能为空");
				return(errCodeParameter);
			}
			if (!UnionIsBCDStr(GC_Data))
			{
				UnionUserErrLog("in UnionDealServiceCodeE602:: GC_Data[%s] is error!\n",GC_Data);
				UnionSetResponseRemark("过程密钥离散数据[%s]非法,必须为十六进制数",GC_Data);
				return(errCodeParameter);
			}
		}
	}

	// 加密标志
	memset(encryptFlag,0,sizeof(encryptFlag));
	if ((ret = UnionReadRequestXMLPackageValue("body/encryptFlag",encryptFlag,sizeof(encryptFlag))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE602:: UnionReadRequestXMLPackageValue[%s]!\n","body/encryptFlag");
		return(ret);
	}

	if (encryptFlag[0] != '0' && encryptFlag[0] != '1' && encryptFlag[0] != '2')
	{
		UnionUserErrLog("in UnionDealServiceCodeE602:: encryptFlag [%s] not between 0 and 2!\n",encryptFlag);
		UnionSetResponseRemark("保护密钥离散次数[%s]不在0到2之间",encryptFlag);
		return(errCodeParameter);
	}

        if (encryptFlag[0] == '1' || encryptFlag[0] == '2')
        {
                // 加密算法选择
                memset(encryptAlgorithmID,0,sizeof(encryptAlgorithmID));
                if ((ret = UnionReadRequestXMLPackageValue("body/encryptAlgorithmID",encryptAlgorithmID,sizeof(encryptAlgorithmID))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE602:: UnionReadRequestXMLPackageValue[%s]!\n","body/encryptAlgorithmID");
                        return(ret);
                }
		// modify by lusj begin 20150909 ，18指令接口函数要判断 加密算法选择 标识为0或1 时是否对报文数据进行16个0填充
                encryptAlgorithmID_flag=atoi(encryptAlgorithmID);
		// modify by lusj end 20150909
        }

        // 数据
        if ((lenOfData = UnionReadRequestXMLPackageValue("body/data",data,sizeof(data))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE602:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
                return(lenOfData);
        }
	data[lenOfData] = 0;

        memset(mac,0,sizeof(mac));
	memset(data1,0,sizeof(data1));

        switch(symmetricKeyDB.algorithmID)
        {
                case    conSymmetricAlgorithmIDOfDES:
                        switch(phsmGroupRec->hsmCmdVersionID)
                        {
                                case conHsmCmdVerRacalStandardHsmCmd:
                                case conHsmCmdVerSJL06StandardHsmCmd:
        
					// modify by lusj begin 20150909 ，18指令接口函数要判断 加密算法选择 标识为0或1 时是否对报文数据进行16个0填充
/*
                                        if ((ret = UnionHsmCmd18(keyIndex,symmetricKeyDB.keyValue[0].keyValue,keyDiscreteNum,keyDiscreteData,atoi(algorithmID),GC_Data,
					atoi(encryptFlag),atoi(encryptAlgorithmID),atoi(macFlag),atoi(macAlgorithmID),lenOfData,data,mac,data1)) < 0)
*/
					if ((ret = UnionHsmCmd18(keyIndex,symmetricKeyDB.keyValue[0].keyValue,keyDiscreteNum,keyDiscreteData,atoi(algorithmID),GC_Data,
					  atoi(encryptFlag),encryptAlgorithmID_flag,atoi(macFlag),atoi(macAlgorithmID),lenOfData,data,mac,data1)) < 0)	// modify by lusj end 20150909
                                        {
                                                UnionUserErrLog("in UnionDealServiceCodeE602:: UnionHsmCmdU2!\n");
                                                return(ret);
                                        }
                                        break;
                                default:
                                        UnionUserErrLog("in UnionDealServiceCodeE602:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                                        UnionSetResponseRemark("非法的加密机指令类型");
                                        return(errCodeParameter);
                        }
                        break;
                case    conSymmetricAlgorithmIDOfSM4:

			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					if (macFlag[0] != '0')
					{
						lenOfData = UnionFillStr(data,16,2);
						lenOfData = aschex_to_bcdhex(data,lenOfData,data1);
						if ((ret = froadGenerateMac(&symmetricKeyDB,psymmetricKeyValue->keyValue,keyDiscreteData,lenOfData,data1,mac)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE602:: froadGenerateMac[%d]\n",ret);
							return(ret);
						}
					}
					if (encryptAlgorithmID_flag == 5)	// 单向加解密
					{
						if (atoi(encryptFlag) == 1)
						{
							snprintf(data1,sizeof(data1),"%02d%s",lenOfData/2,data);
						}
						else
						{
							memcpy(data1,data,lenOfData);
						}
						lenOfData = UnionFillStr(data1,16,2);
						data1[lenOfData] = 0;
						lenOfData = aschex_to_bcdhex(data1,lenOfData,data);
						if ((ret = froadOneWayEncryData(atoi(encryptFlag),&symmetricKeyDB,psymmetricKeyValue->keyValue,keyDiscreteData,GC_Data,lenOfData,data,data1)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE602:: froadOneWayEncryData[%d]\n",ret);
							return(ret);
						}
					}
					else if (encryptAlgorithmID_flag == 6)	//全包加解密
					{
						if ((ret = froadAllPackageEncryData(atoi(encryptFlag),&symmetricKeyDB,psymmetricKeyValue->keyValue,keyDiscreteData,processFlag,GC_Data,lenOfData,data,data1)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE602:: froadAllPackageEncryData[%d]\n",ret);
							return(ret);
						}
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE602:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}
                        break;
                default:
                        UnionUserErrLog("in UnionDealServiceCodeE602:: symmetricKeyDB.algorithmID[%d] error!\n",symmetricKeyDB.algorithmID);
                        return(errCodeEsscMDL_InvalidAlgorithmID);
        }

        // MAC
        if (macFlag[0] == '1' || macFlag[0] == '2')
        {
                if ((ret = UnionSetResponseXMLPackageValue("body/mac",mac)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE602:: UnionSetResponseXMLPackageValue[%s]","body/mac");
                        return(ret);
                }
        }

        // 密文/明文数据
        if (encryptFlag[0] == '1' || encryptFlag[0] == '2')
        {
		if (symmetricKeyDB.algorithmID == conSymmetricAlgorithmIDOfDES)
		{
			if (encryptFlag[0] == '2')
				data1[(strlen(data1)-16)] = 0;
		}

		if ((ret = UnionSetResponseXMLPackageValue("body/data",data1)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE602:: UnionSetResponseXMLPackageValue[%s]!\n","body/data");
                        return(ret);
                }
        }

        return(0);
}

static int froadGenerateMac(PUnionSymmetricKeyDB psymmetricKeyDB,char *keyValue,char *keyDiscreteData,int lenOfData,char *data,char *mac)
{
	int	ret = 0;
	char	zekValue[48];
	char	checkValue[32];
	char	iv[48]="00000000000000000000000000000000";
	char	encData[2048];
	char	tmpBuf[16];
	int	keyDiscreteNum = 0;
	int	i = 0;
	int	keyType = 0;
	char	discreteData[64];
	
	keyDiscreteNum = strlen(keyDiscreteData)/32;
	keyType = psymmetricKeyDB->keyType;	
	snprintf(checkValue,sizeof(checkValue),"%s",psymmetricKeyDB->checkValue);

	for (i = 0; i < keyDiscreteNum; i++)
	{
		memcpy(discreteData,keyDiscreteData+i*32,32);	
		discreteData[32] = 0;

		if ((ret = UnionHsmCmdWE("02",keyType,keyValue,checkValue,0,conZEK,"1",discreteData,"",zekValue)) < 0)
		{
			UnionUserErrLog("in froadGenerateMac:: UnionHsmCmdWE !\n");
			return(ret);
		}
		zekValue[ret] = 0;

		memcpy(keyValue,zekValue,ret);
		keyValue[ret] = 0;

		if ((ret = UnionHsmCmdBU(1,conZEK,UnionConvertSymmetricKeyKeyLen(psymmetricKeyDB->keyLen),zekValue,checkValue)) < 0)
		{
			UnionUserErrLog("in froadGenerateMac:: UnionHsmCmdBU !\n");
			return(ret);
		}
		keyType = conZEK;
	}

	if ((ret = UnionHsmCmdWA(2,"02","00A",1,zekValue,checkValue,iv,lenOfData,(unsigned char *)data,encData)) < 0)
	{
		UnionUserErrLog("in froadGenerateMac:: UnionHsmCmdWA !\n");
		return(ret);
	}

	// 取后16字节中的前四个字节
	memcpy(tmpBuf, encData+ret-16,4);
	bcdhex_to_aschex(tmpBuf,4,mac);
	mac[8] = 0;

	return 8;
}

static int froadOneWayEncryData(int encMode,PUnionSymmetricKeyDB psymmetricKeyDB,char *keyValue,char *discreteData,char *processData,int lenOfData,char *data,char *encData)
{
	int		ret = 0;
	char		zmkValue[48];
	char		checkValue[32];
	char		tmpData[2048];

	if ((ret = UnionHsmCmdWE("02",psymmetricKeyDB->keyType,keyValue,psymmetricKeyDB->checkValue,0,conZMK,"1",discreteData,"",zmkValue)) < 0)
	{
		UnionUserErrLog("in froadOneWayEncryData:: UnionHsmCmdWE !\n");
		return(ret);
	}
	zmkValue[ret] = 0;
	UnionSetIsUseNormalZmkType();
	if ((ret = UnionHsmCmdBU(1,conZMK,UnionConvertSymmetricKeyKeyLen(psymmetricKeyDB->keyLen),zmkValue,checkValue)) < 0)
	{
		UnionUserErrLog("in froadOneWayEncryData:: UnionHsmCmdBU !\n");
		return(ret);
	}
	if ((ret = UnionHsmCmdWE("02",psymmetricKeyDB->keyType,zmkValue,checkValue,0,conZMK,"1",processData,"",zmkValue)) < 0)
	{
		UnionUserErrLog("in froadOneWayEncryData:: UnionHsmCmdWE !\n");
		return(ret);
	}
	zmkValue[ret] = 0;
	UnionSetIsUseNormalZmkType();
	if ((ret = UnionHsmCmdBU(1,conZMK,UnionConvertSymmetricKeyKeyLen(psymmetricKeyDB->keyLen),zmkValue,checkValue)) < 0)
	{
		UnionUserErrLog("in froadOneWayEncryData:: UnionHsmCmdBU !\n");
		return(ret);
	}
	if (encMode == 2)
		encMode = 1;
	else
		encMode = 2;
	if ((ret = UnionHsmCmdWA(encMode,"01","000",1,zmkValue,checkValue,"",lenOfData,(unsigned char *)data,tmpData)) < 0)
	{
		UnionUserErrLog("in froadOneWayEncryData:: UnionHsmCmdWA !\n");
		return(ret);
	}
	bcdhex_to_aschex(tmpData,ret,encData);
	encData[ret*2] = 0;
	
	return(ret*2);
}

static int froadAllPackageEncryData(int encMode,PUnionSymmetricKeyDB psymmetricKeyDB,char *keyValue,char *discreteData,char *processFlag,char *processData,int lenOfData,char *data,char *encData)
{
	int		ret = 0;
	char		zmkValue[48];
	char		checkValue[32];
	char		tmpData[2048];
	char		tmpValue[48];
	int		len = 0;
	int		lenOfEncData = 0;
	char		remainData[32];

	if ((ret = UnionHsmCmdWE("02",psymmetricKeyDB->keyType,keyValue,psymmetricKeyDB->checkValue,0,conZMK,"1",discreteData,"",zmkValue)) < 0)
	{
		UnionUserErrLog("in froadAllPackageEncryData:: UnionHsmCmdWE !\n");
		return(ret);
	}
	zmkValue[ret] = 0;
	UnionSetIsUseNormalZmkType();
	if ((ret = UnionHsmCmdBU(1,conZMK,UnionConvertSymmetricKeyKeyLen(psymmetricKeyDB->keyLen),zmkValue,checkValue)) < 0)
	{
		UnionUserErrLog("in froadAllPackageEncryData:: UnionHsmCmdBU !\n");
		return(ret);
	}
	if ((ret = UnionHsmCmdWE("02",psymmetricKeyDB->keyType,zmkValue,checkValue,0,conZMK,"1",processData,"",zmkValue)) < 0)
	{
		UnionUserErrLog("in froadAllPackageEncryData:: UnionHsmCmdWE !\n");
		return(ret);
	}
	zmkValue[ret] = 0;
	UnionSetIsUseNormalZmkType();
	if ((ret = UnionHsmCmdBU(1,conZMK,UnionConvertSymmetricKeyKeyLen(psymmetricKeyDB->keyLen),zmkValue,checkValue)) < 0)
	{
		UnionUserErrLog("in froadAllPackageEncryData:: UnionHsmCmdBU !\n");
		return(ret);
	}
	if (encMode == 2)
		encMode = 1;
	else
		encMode = 2;
	if ((len = lenOfData % 32) != 0)
	{
		memcpy(remainData,data+lenOfData-len,len);
		remainData[len] = 0;
	}
	lenOfData = aschex_to_bcdhex(data,lenOfData-len,tmpData);
	if ((ret = UnionHsmCmdWA(encMode,"01","000",1,zmkValue,checkValue,"",lenOfData,(unsigned char *)tmpData,tmpData)) < 0)
	{
		UnionUserErrLog("in froadAllPackageEncryData:: UnionHsmCmdWA !\n");
		return(ret);
	}
	lenOfEncData = bcdhex_to_aschex(tmpData,ret,encData);
	encData[lenOfEncData] = 0;

	if (len > 0)
	{
		if ((ret = UnionHsmCmdUS(2,'S',psymmetricKeyDB->keyType,keyValue,1,discreteData,processFlag,processData,len,remainData,psymmetricKeyDB->keyType,tmpValue,checkValue,&len,encData+lenOfEncData)) < 0)
		{
			UnionUserErrLog("in froadAllPackageEncryData:: UnionHsmCmdUS[%d]\n",ret);
			return(ret);
		}
	}

	return(len);
}
