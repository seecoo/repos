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
#include "unionHsmCmd.h"
#include "symmetricKeyDB.h"
#include "UnionXOR.h"
#include "unionHsmCmdVersion.h"

/***************************************
服务代码:	E161
服务名:		解密数据
功能描述:	解密数据
***************************************/
int UnionDealServiceCodeE161(PUnionHsmGroupRec phsmGroupRec)
{
	int				i;
	int				ret;
	int				msgNo = 0;
	int				len;
	int				offset = 0;
	int				lenOfPerData = 1024;
	int				lenOfData = 0;
	int				lenOfOut = 0;
	int				mode = 1;
	int				algorithmID = 0;
	char				tmpAlgorithmID[32];
	int				msgNum = 0;
	char				tmpBuf[20480];
	char				data[20480];
	char				keyByLMK[64];
	char				keyName[160];
	char				keyValue[64];
	char				checkValue[32];
	char				iv[64];
	char				format[32];
	char				lenBuf[32];
	char				tmpData[10240];
	int				exportFlag = 0;

	TUnionSymmetricKeyType		keyType;
	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;
	
	// 模式
	// 1：指定密钥名称
	// 2：指定密钥密文
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) < 0)
		mode = 1;
	else
	{
		tmpBuf[ret] = 0;
		mode = atoi(tmpBuf);
	}

	// 密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE161:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1,&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE161:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
		return(ret);
	}
	// 读取密钥值
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE161:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("读取密钥值失败");
		return(errCodeParameter);
	}
	
	if (mode == 1)
	{
		if ((symmetricKeyDB.keyType != conEDK) && (symmetricKeyDB.keyType != conZEK) && (symmetricKeyDB.keyType != conWWK))
		{
			UnionUserErrLog("in UnionDealServiceCodeE161:: is not edk or zek or wwk[%s]!\n",keyName);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}

		// SM4 算法
		if (symmetricKeyDB.algorithmID  == conSymmetricAlgorithmIDOfSM4)
		{
			if (symmetricKeyDB.keyType != conZEK)
			{
				UnionUserErrLog("in UnionDealServiceCodeE161:: is not zek [%s]!\n",keyName);
				return(errCodeEsscMDL_WrongUsageOfKey);
			}
		}
		strcpy(keyByLMK,psymmetricKeyValue->keyValue);
		keyType = symmetricKeyDB.keyType;
		strcpy(checkValue,symmetricKeyDB.checkValue);
	}
	else
	{
		if (symmetricKeyDB.keyType != conZMK)
		{
			UnionUserErrLog("in UnionDealServiceCodeE161:: is not zmk [%s]!\n",keyName);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}
		// 密钥密文
		if ((ret = UnionReadRequestXMLPackageValue("body/keyValue",keyValue,sizeof(keyValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE161:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyValue");
			return(ret);
		}
		
		// 密钥类型
		keyType = conZEK;		

		switch(symmetricKeyDB.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES: // DES算法
				// 密钥转成本地LMK对加密
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdA6(NULL,keyType,psymmetricKeyValue->keyValue,keyValue,keyByLMK,checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE161:: UnionHsmCmdA6!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE161:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
				// 将密钥转成本地LMK对加密
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdSV(keyType,psymmetricKeyValue->keyValue,keyValue,keyByLMK,checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE161:: UnionHsmCmdSV!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE161:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE161:: symmetricKeyDB.algorithmID[%d] error!\n",symmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}			
	}
	
	if (keyType == conZEK || keyType == conWWK)
	{
		// 算法标识 0:ECB 1:CBC
		if ((ret = UnionReadRequestXMLPackageValue("body/algorithmID",tmpBuf,sizeof(tmpBuf))) < 0)
			algorithmID = 0;
		else
		{
			tmpBuf[ret] = 0;
			algorithmID = atoi(tmpBuf);
		}
		
		if (algorithmID != 0)
		{
			// IV
			if ((ret = UnionReadRequestXMLPackageValue("body/iv",iv,sizeof(iv))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE161:: UnionReadRequestXMLPackageValue[%s]!\n","body/iv");
				return(ret);
			}
			else
                        {
				UnionFilterHeadAndTailBlank(iv);
				if (symmetricKeyDB.algorithmID  == conSymmetricAlgorithmIDOfSM4)
				{
					if (!UnionIsBCDStr(iv) || strlen(iv) != 32)
					{
						UnionUserErrLog("in UnionDealServiceCodeE161:: iv[%s] is error!\n",iv);
						UnionSetResponseRemark("初始向量[%s]非法,必须为十六进制数,且长度必须为32",iv);
						return(errCodeParameter);
					}
				}
				else
				{
					if (!UnionIsBCDStr(iv) || strlen(iv) != 16)
					{
						UnionUserErrLog("in UnionDealServiceCodeE161:: iv[%s] is error!\n",iv);
						UnionSetResponseRemark("初始向量[%s]非法,必须为十六进制数,且长度必须为16",iv);
						return(errCodeParameter);
					}
				}
                        }
		}
	}

	// 数据输出标识
	if ((ret = UnionReadRequestXMLPackageValue("body/exportFlag",tmpBuf,sizeof(tmpBuf))) <= 0)
		exportFlag = 0;
	else
	{
		tmpBuf[ret] = 0;
		exportFlag = atoi(tmpBuf);
	}
		
	// 数据
	if ((lenOfData = UnionReadRequestXMLPackageValue("body/data",data,sizeof(data))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE161:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
		return(lenOfData);
	}
	else
        {
		data[lenOfData] = 0;
		if (lenOfData == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE161:: data can not be null!\n");
                	UnionSetResponseRemark("数据不能为空!");
                	return(errCodeParameter);
		}
                if (!UnionIsBCDStr(data))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE161:: data[%s] is error!\n",data);
                        UnionSetResponseRemark("密文数据非法,必须为十六进制数");
                        return(errCodeParameter);
                }
        }

	if (symmetricKeyDB.keyType != conEDK)
	{
		lenOfData = aschex_to_bcdhex(data,lenOfData,tmpData);
		memcpy(data,tmpData,lenOfData);
		data[lenOfData] = 0;
	}
	
	// 格式
	if ((ret = UnionReadRequestXMLPackageValue("body/format",format,sizeof(format))) < 0)
		format[0] = '0';
		
	// 计算块的数量
	msgNum = lenOfData / lenOfPerData;
	if (lenOfData % lenOfPerData != 0)
		msgNum += 1;

	for (i = 0; i < msgNum; i++)
	{
		if (msgNum == 1)
		{
			msgNo = 0;
			len = lenOfData;
		}
		else
		{
			if (i == 0)
			{
				msgNo = 1;
				len = lenOfPerData;
			}
			else if(i == msgNum - 1)
			{
				msgNo = 3;
				//len = lenOfData - (i - 1) * lenOfPerData;
				len = lenOfData - i * lenOfPerData;
			}
			else
			{
				msgNo = 2;
				len = lenOfPerData;
			}
		}
		offset = i * lenOfPerData;
		switch(symmetricKeyDB.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:	// DES算法		
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if (keyType == conEDK)
						{
							if ((len = UnionHsmCmd50('1',keyByLMK,len,data + offset,tmpBuf + lenOfOut,sizeof(tmpBuf) - lenOfOut)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE161:: UnionHsmCmd50[%d][%s]!\n",len,data + offset);
								return(len);
							}
						}
						else if (keyType == conZEK)
						{
							if ((len = UnionHsmCmdE0(1,msgNo,algorithmID + 1,strlen(keyByLMK),keyByLMK,0,iv,len,(unsigned char *)data + offset,(unsigned char *)tmpBuf + offset,&len,sizeof(data) - offset)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE161:: UnionHsmCmdE0[%d][%s]!\n",len,data + offset);
								return(len);
							}
						}
						else if (keyType == conWWK)
						{
							if ((len = UnionHsmCmdB8(keyByLMK,0,algorithmID,iv,len,data + offset,tmpBuf + offset,sizeof(tmpBuf) - offset)) < 0)	
							{
								UnionUserErrLog("in UnionDealServiceCodeE161:: UnionHsmCmdB8[%d][%s]!\n",len,data + offset);
								return(len);
							}
						}
						lenOfOut += len;
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE161:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
				sprintf(tmpAlgorithmID,"%02d",algorithmID + 1);
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((len = UnionHsmCmdWA(1,tmpAlgorithmID,"00A",1,keyByLMK,checkValue,iv,len,(unsigned char *)data + offset,tmpBuf + offset)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE161:: UnionHsmCmdWA[%d][%s]!\n",len,data + offset);
							return(len);
						}
						lenOfOut += len;
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE161:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE161:: symmetricKeyDB.algorithmID[%d] error!\n",symmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
	}
	tmpBuf[lenOfOut] = 0;

	// 0：不带填充
	// 1：4字节明文长度+明文+补位‘0’
	// 2：填充0x00
	if (format[0] == '1')
	{
		memcpy(lenBuf,tmpBuf,4);
		lenBuf[4] = 0;
		lenOfOut = atoi(lenBuf);		
		memmove(tmpBuf,tmpBuf+4,lenOfOut);
		tmpBuf[lenOfOut] = 0;
	}
	else if (format[0] == '3')
	{
		for(i = 1; i <= 8; i++)
		{
			if ((unsigned char)tmpBuf[lenOfOut-i] == 0x80)
			{
				tmpBuf[lenOfOut-i] = 0;
				break;
			}
		}
		if (i == 9)
		{
			UnionUserErrLog("in UnionDealServiceCodeE161:: filldata not found 0x80!\n");
			return(errCodeParameter);
		}
	}

	if (exportFlag)
	{
		if (lenOfOut > 8192)
		{
			UnionUserErrLog("in UnionDealServiceCodeE161:: lenOfOut[%d] > 8192! error\n",lenOfOut);
			return(errCodeSmallBuffer);
		}
		lenOfOut = bcdhex_to_aschex(tmpBuf,lenOfOut,data);	
		data[lenOfOut] = 0;
		strcpy(tmpBuf,data);
	}

	// 设置data
	if ((ret = UnionSetResponseXMLPackageValue("body/data",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE161:: UnionSetResponseXMLPackageValue [%s]!\n","body/data");
		return(ret);
	}
	
	return(0);
}
