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
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "symmetricKeyDB.h"
#include "UnionXOR.h"
#include "unionHsmCmdVersion.h"

int UnionFillStr(char *bcdStr, int algGrpLen, int fillMode);

/***************************************
服务代码:	E150
服务名:		生成MAC
功能描述:	生成MAC
***************************************/
int UnionDealServiceCodeE150(PUnionHsmGroupRec phsmGroupRec)
{
	int				i;
	int				ret;
	int				len;
	int				offset;
	int				lenOfPerData = 1024;
	int				lenOfData = 0;
	int				mode = 1;
	int				algorithmID = 1;
	char				algorithmIDBuf[8];
	int				msgNum = 0;
	TUnionSymmetricKeyType		keyType = conZAK;
	char				tmpBuf[32];
	char				data[8192];
	char				ascData[8192];
	char				keyByLMK[64];
	char				keyName[160];
	char				keyValue[64];
	char				checkValue[32];
	char				mac[64];
	char				msgNo;
	int				algGrpLen = 8;
	int				dataType = 2;

	TUnionDesKeyLength		keyLen = con128BitsDesKey;
	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		symmetricKeyValue = NULL;
	
	// 模式
	// 1：指定密钥名称
	// 2：指定密钥密文(ZMK加密)
	// 3：指定密钥密文(LMK加密,国际)
	// 4：指定密钥密文(LMK加密,国密)
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) <= 0)
		mode = 1;
	else
	{
		tmpBuf[ret] = 0;
		mode = atoi(tmpBuf);
	}
	
	memset(keyByLMK, 0, sizeof(keyByLMK));
	if (mode == 1 || mode == 5)//modify by huangh 20160523,增加了mode==5模式，该模式为湖南农信专用
	{
		// 密钥名称
		if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE150:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
			return(ret);
		}
		keyName[ret] = 0;

		if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1,&symmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE150:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
			return(ret);
		}

		if (symmetricKeyDB.keyType != conZAK && symmetricKeyDB.keyType != conTAK)
		{
			UnionUserErrLog("in UnionDealServiceCodeE150:: keyType [%d] is not zak or tak [%s]!\n",symmetricKeyDB.keyType, keyName);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}
		// 读取密钥值
		if ((symmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE150:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("读取密钥值失败");
			return(errCodeParameter);
		}

		snprintf(keyByLMK,sizeof(keyByLMK),"%s",symmetricKeyValue->keyValue);
		keyLen = UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen);
		keyType = symmetricKeyDB.keyType;
	}
	else if (mode == 2)
	{
		// 密钥名称
		if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE150:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
			return(ret);
		}
		keyName[ret] = 0;

		if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1,&symmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE150:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
			return(ret);
		}

		if (symmetricKeyDB.keyType != conZMK)
		{
			UnionUserErrLog("in UnionDealServiceCodeE150:: is not zmk [%s]!\n",keyName);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}
		// 读取密钥值
		if ((symmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE150:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("读取密钥值失败");
			return(errCodeParameter);
		}

		// 密钥密文
		if ((ret = UnionReadRequestXMLPackageValue("body/keyValue",keyValue,sizeof(keyValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE150:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyValue");
			return(ret);
		}
		keyValue[ret] = 0;

		// 后续需要判断算法标识，国密算法的功能需要补充
		switch(symmetricKeyDB.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:	
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdA6(NULL,conZAK,symmetricKeyValue->keyValue,keyValue,keyByLMK,checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE150:: UnionHsmCmdA6!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE150:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			case	conSymmetricAlgorithmIDOfSM4:
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdSV(conZAK,symmetricKeyValue->keyValue,keyValue,keyByLMK,checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE150:: UnionHsmCmdSV!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE150:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE150:: symmetricKeyDB.algorithmID[%d] error!\n",symmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
		if (strlen(keyByLMK) == 16)
			keyLen = con64BitsDesKey;
		else if (strlen(keyByLMK) == 32)
			keyLen = con128BitsDesKey;
		else if (strlen(keyByLMK) == 48)
			keyLen = con192BitsDesKey;
		else
			keyLen = con128BitsDesKey;

		keyType = conZAK;
	}
	if (mode == 3 || mode == 4)	// LMK对保护(国际算法)
	{
		// 密钥密文
		if ((ret = UnionReadRequestXMLPackageValue("body/keyValue",keyByLMK,sizeof(keyByLMK))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE150:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyValue");
			return(ret);
		}
		keyByLMK[ret] = 0;
		
		if (strlen(keyByLMK) == 16)
			keyLen = con64BitsDesKey;
		else if (strlen(keyByLMK) == 32)
			keyLen = con128BitsDesKey;
		else if (strlen(keyByLMK) == 48)
			keyLen = con192BitsDesKey;
		else
			keyLen = con128BitsDesKey;

		keyType = conZAK;
		if (mode == 3)
			symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfDES;
		else
			symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfSM4;
	}
	
	// 算法标识
	if ((ret = UnionReadRequestXMLPackageValue("body/algorithmID",tmpBuf,sizeof(tmpBuf))) <= 0)
		algorithmID = 1;
	else
	{
		tmpBuf[ret] = 0;
		algorithmID = atoi(tmpBuf);
	}

	// 数据类型
	if ((ret = UnionReadRequestXMLPackageValue("body/dataType",tmpBuf,sizeof(tmpBuf))) <= 0)
		dataType = 2;
	else
	{
		tmpBuf[ret] = 0;
		dataType = atoi(tmpBuf);
	}

	if (dataType != 1 && dataType != 2)	
	{
		UnionUserErrLog("in UnionDealServiceCodeE150:: dataType[%d] not in [1,2]\n",dataType);
		return(errCodeParameter);
	}

	// 数据
	if ((lenOfData = UnionReadRequestXMLPackageValue("body/data",data,sizeof(data))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE150:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
		return(lenOfData);
	}
	else
        {
		data[lenOfData] = 0;
		if (strlen(data) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE150:: data can not be null!\n");
                	UnionSetResponseRemark("数据不能为空!");
                	return(errCodeParameter);
		}

		if (dataType == 1)
		{
			if (lenOfData > 4096)
			{
				UnionUserErrLog("in UnionDealServiceCodeE150:: data[%d] > 4096!\n",lenOfData);
				UnionSetResponseRemark("当不为16进制,数据[%d]不能大于4096!",lenOfData);
				return(errCodeParameter);
			}
			lenOfData = bcdhex_to_aschex(data,lenOfData,ascData);	
			memcpy(data,ascData,lenOfData);
			data[lenOfData] = 0;
			
		}
		else
		{
			if (!UnionIsBCDStr(data))
			{
				UnionUserErrLog("in UnionDealServiceCodeE150:: data is not hex!\n");
				return(errCodeParameter);
			}
		}
        }
	lenOfData = strlen(data);

	// 数据填充
	if ((ret = UnionReadRequestXMLPackageValue("body/fillMode",tmpBuf,sizeof(tmpBuf))) <= 0)
		snprintf(tmpBuf,sizeof(tmpBuf),"%s","1");
	else
		tmpBuf[ret] = 0;

	if(symmetricKeyDB.algorithmID == conSymmetricAlgorithmIDOfSM4) // sm4 16B分组
	{
		algGrpLen = 16;
	}
	else // des 8B分组
	{
		algGrpLen = 8;
	}

	lenOfData = UnionFillStr(data, algGrpLen, atoi(tmpBuf));
	data[lenOfData] = 0;
	if (lenOfData / 2 % algGrpLen != 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE150:: lenOfData[%d] %% 16 != 0 ,data[%s]!\n",lenOfData,data);
		UnionSetResponseRemark("数据长度[%d]必须为16的整数倍",strlen(data));
		return(errCodeParameter);
	}
	
	if (algorithmID == 3)	// 银联POS标准
	{
		// modify by leipp 20160425
		len = algGrpLen * 2;
		memset(tmpBuf,'0',sizeof(tmpBuf));
		for (i = 0; i < lenOfData / len; i++)
		{
			if ((ret = UnionXOR(tmpBuf,data + i * len,len,tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE150:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
				return(ret);
			}
		}
		memcpy(data,tmpBuf,len);
		data[len] = 0;
		lenOfData = len;
		// modify be leipp 20160425
	}
	
	// 计算块的数量
	msgNum = lenOfData / lenOfPerData;
	if (lenOfData % lenOfPerData != 0)
		msgNum += 1;

	for (i = 0; i < msgNum; i++)
	{
		if (msgNum == 1)
		{
			msgNo = '0';
			len = lenOfData;
		}
		else
		{
			if (i == 0)
			{
				msgNo = '1';
				len = lenOfPerData;
			}
			else if(i == msgNum - 1)
			{
				msgNo = '3';
				//len = lenOfData - (i - 1) * lenOfPerData;
				len = lenOfData - i * lenOfPerData;
			}
			else
			{
				msgNo = '2';
				len = lenOfPerData;
			}
		}
		offset = i * lenOfPerData;
		
		switch(symmetricKeyDB.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:	// DES算法		
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if (algorithmID == 1)	// ANSIX9.19
						{
							if(keyType == conZAK)
							{	
								if(mode == 5)//add by huangh 20160523
								{
									if ((ret = UnionHsmCmdMS(msgNo,2,keyLen,keyByLMK,'1',mac,len,data + offset,mac)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE150:: UnionHsmCmdMS zak [%s]!\n",data + offset);
										return(ret);
									}
								}
								else
								{
									if ((ret = UnionHsmCmdMS(msgNo,1,keyLen,keyByLMK,'1',mac,len,data + offset,mac)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE150:: UnionHsmCmdMS zak [%s]!\n",data + offset);
										return(ret);
									}
								}
							}
							else // conTak
							{
								if ((ret = UnionHsmCmdMS(msgNo,0,keyLen,keyByLMK,'1',mac,len,data + offset,mac)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE150:: UnionHsmCmdMS tak[%s]!\n",data + offset);
									return(ret);
								}
							}
						}
						else if ((algorithmID == 2) || (algorithmID == 3))	// 中国银联标准、银联POS标准
						{
							// modify by leipp 20150806
							if ((algorithmID == 2) || (strcasecmp(UnionGetIDOfCustomization(), "GXNX") == 0))
								snprintf(algorithmIDBuf,sizeof(algorithmIDBuf),"1");
							else
								snprintf(algorithmIDBuf,sizeof(algorithmIDBuf),"0");
							if(keyType == conZAK)
							{
								if ((ret = UnionHsmCmdMU(msgNo,1,keyLen,keyByLMK,algorithmIDBuf[0],mac,len,data + offset,mac)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE150:: UnionHsmCmdMU zak [%s]!\n",data + offset);
									return(ret);
								}
							}
							else	// conTak
							{
								if ((ret = UnionHsmCmdMU(msgNo,0,keyLen,keyByLMK,algorithmIDBuf[0],mac,len,data + offset,mac)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE150:: UnionHsmCmdMU tak[%s]!\n",data + offset);
									return(ret);
								}
							}
							// modify end
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE150:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			case	conSymmetricAlgorithmIDOfSM4:	// SM4算法

				if (i > 0)
					UnionXOR(mac,data + offset,32,data+offset);

				if ((ret = UnionHsmCmdW9(1,NULL,keyByLMK,1,1,len,data+offset,NULL,mac,sizeof(mac))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE150:: UnionHsmCmdW9!\n");
					return(ret);
				}

				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE150:: symmetricKeyDB.algorithmID[%d] error!\n",symmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
	}

	// 设置mac
	// add by leipp 20151127,国密国际统一返回16
	mac[16] = 0;
	// add end
	if ((ret = UnionSetResponseXMLPackageValue("body/mac",mac)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE150:: UnionSetResponseXMLPackageValue mac[%s]!\n",mac);
		return(ret);
	}
	
	return(0);
}
