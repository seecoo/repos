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
#include "asymmetricKeyDB.h"
#include "UnionMD5.h"
#include "UnionSHA1.h"
#include "base64.h"
#include "unionHsmCmdVersion.h"
#include "unionCertFunSM2.h"
#include "unionCertFun.h"
#include "unionREC.h"

/***************************************
服务代码:	E404
服务名:		数字签名核签
功能描述:	数字签名核签
***************************************/
int UnionDealServiceCodeE404(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	char				dataBcd[8192];
	char				data[8192*2];
	int				lenOfData = 0;
	char				sign[1024];
	char				tmpSign[1024];
	char				tmpData[64];
	int				lenOfSign = 0;
	char				pix[16];
	char				bankID[48];
	char				certVersion[8];
	char				shapadStr[64];
	int				len = 0;
	//char				tmpStr[20+1];
	char				signOfBCD[1024];
	int				lenOfBCD = 0;
	char				keyName[136];
		
	char				rsSign[128];
	int				lenOfRSSign = 0;
	char				ascSign[256];
	
	//int				algFlagOfMyBankCert = 0;
	//char				algFlag[16];

	TUnionAsymmetricKeyDB		asymmetricKeyDB;

	// 应用ID
	if ((ret = UnionReadRequestXMLPackageValue("body/pix",pix,sizeof(pix))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE404:: UnionReadRequestXMLPackageValue[%s]!\n","body/pix");
		return(ret);
	}
	pix[ret] = 0;
	UnionFilterHeadAndTailBlank(pix);
        if (strlen(pix) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE404:: pix can not be null!\n");
                UnionSetResponseRemark("应用ID不能为空!");
                return(errCodeParameter);
        }
	if (!UnionIsBCDStr(pix))
        {
                UnionUserErrLog("in UnionDealServiceCodeE404:: pix[%s] is error!\n",pix);
                UnionSetResponseRemark("应用ID[%s]非法,必须为十六进制数",pix);
                return(errCodeParameter);
        }

	// 参与行机构号
	if ((ret = UnionReadRequestXMLPackageValue("body/bankID",bankID,sizeof(bankID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE404:: UnionReadRequestXMLPackageValue[%s]!\n","body/bankID");
		return(ret);
	}
	bankID[ret] = 0;
	UnionFilterHeadAndTailBlank(bankID);

	// 证书版本号
	if ((ret = UnionReadRequestXMLPackageValue("body/certVersion",certVersion,sizeof(certVersion))) <= 0)
		strcpy(certVersion,"01");
	else
		certVersion[ret] = 0;
	
	UnionFilterHeadAndTailBlank(certVersion);

	// 签名数据
	if ((lenOfData = UnionReadRequestXMLPackageValue("body/data",data,sizeof(data))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE404:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
		return(lenOfData);
	}
	else if(lenOfData == 0)
        {
		UnionUserErrLog("in UnionDealServiceCodeE404:: data can not be null!\n");
		UnionSetResponseRemark("签名数据不能为空!");
		return(errCodeParameter);
        }
	data[lenOfData] = 0;


	// 签名
	if ((lenOfSign = UnionReadRequestXMLPackageValue("body/sign",tmpSign,sizeof(tmpSign))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE404:: UnionReadRequestXMLPackageValue[%s]!\n","body/sign");
		return(lenOfSign);
	}
	tmpSign[lenOfSign] = 0;
	memset(sign,0,sizeof(sign));
	UnionTrimCtrlMN(tmpSign,sign);
	
	// 签名转换
	if ((lenOfBCD = from64tobits(signOfBCD,sign)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE404:: from64tobits failed!\n");
		UnionSetResponseRemark("签名转换错误");
		return(errCodeParameter);
	}
	/*
	// 本行使用的证书的算法标识
	if((algFlagOfMyBankCert = UnionReadIntTypeRECVar("algFlagOfMyBankCert")) < 0)
	{
		algFlagOfMyBankCert = 0;
	}
	
	switch(algFlagOfMyBankCert)
	{
		case	0:	
			//RSA AND SM2
			// 读取算法标识
			if((ret = UnionReadRequestXMLPackageValue("body/algFlag", algFlag, sizeof(algFlag))) <= 0)
			{
				// 拼接密钥名称， RSA
				sprintf(algFlag, "0");
				sprintf(keyName,"CNAPS2.%s-%s-%s.cer",pix,bankID,certVersion);
			}
			else
			{
				algFlag[ret] = 0;
				if(algFlag[0] != '0' && algFlag[0] != '1')
				{
					UnionUserErrLog("in UnionDealServiceCodeE401:: algFlag[%s] error!\n", algFlag);
					UnionSetResponseRemark("algFlag[%s]应为0或1", algFlag);
					return(errCodeParameter);
				}
				if(algFlag[0] == '0')
				{
					// 拼接密钥名称， RSA
					sprintf(keyName,"CNAPS2.%s-%s-%s.cer",pix,bankID,certVersion);
				}
				else
				{
					// 拼接密钥名称， SM2
					sprintf(keyName,"CNAPS2.%s-%s-%s.cer_sm2",pix,bankID,certVersion);
				}
			}
			break;
		case	1:	
			// RSA
			sprintf(algFlag, "0");
			sprintf(keyName,"CNAPS2.%s-%s-%s.cer",pix,bankID,certVersion);
			break;
		case	2:
			// SM2
			sprintf(algFlag, "1");
			sprintf(keyName,"CNAPS2.%s-%s-%s.cer_sm2",pix,bankID,certVersion);
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE401:: algFlagOfMyBankCert[%d] error!\n", algFlagOfMyBankCert);
			UnionSetResponseRemark("客户端设置的\"本行使用的证书的算法标识\"参数有错，应为0,1或2");
			return(errCodeParameter);
	}
	`*/
	// 读取密钥结构体
	//memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));
	snprintf(keyName,sizeof(keyName),"CNAPS2.%s-%s-%s.cer",pix,bankID,certVersion);
	if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,1,&asymmetricKeyDB)) < 0)
	{
		// 国际的不存在读国密
		UnionLog("in UnionDealServiceCodeE404:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n", keyName);
		snprintf(keyName,sizeof(keyName),"CNAPS2.%s-%s-%s.cer_sm2",pix,bankID,certVersion);
		if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,1,&asymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE404:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",keyName);
			return(ret);
		}
	}

	switch(asymmetricKeyDB.algorithmID)
	{
		case	conAsymmetricAlgorithmIDOfRSA:
			// HASH算法标识
			// modify 20150331 leipp	因为银联发过来的数据有2M左右，所以计算hash放到了javaAPI
			//UnionSHA1((unsigned char *)data,lenOfData,(unsigned char *)tmpStr);
			//tmpStr[20] = 0;

			memset(tmpData,0,sizeof(tmpData));
			len = sprintf(shapadStr,"%s","3021300906052B0E03021A05000414");
			len = aschex_to_bcdhex(shapadStr,len,tmpData);

			lenOfData = aschex_to_bcdhex(data,lenOfData,dataBcd);
			memcpy(tmpData + len,dataBcd,lenOfData);
			len += lenOfData;
			tmpData[len] = 0;

			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmd38('1',NULL,lenOfBCD,signOfBCD,len,tmpData,asymmetricKeyDB.pkValue)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE404:: UnionHsmCmd38!\n");
						return(ret);
					}
					break;
				default:	
					UnionUserErrLog("in UnionDealServiceCodeE404:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}
			break;
		case	conAsymmetricAlgorithmIDOfSM2:
			if((lenOfRSSign = UnionGetRSFromSM2SigDer((unsigned char*)signOfBCD, lenOfBCD, (unsigned char*)rsSign)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE403:: UnionGetRSFromSM2SigDer failed ret = [%d]!\n", lenOfRSSign);
				return(lenOfRSSign);
			}
			rsSign[lenOfRSSign] = 0;
			bcdhex_to_aschex(rsSign, lenOfRSSign, ascSign);
			lenOfRSSign = lenOfRSSign * 2;
			ascSign[lenOfRSSign] = 0;
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmdK4(-1, "01", strlen(SM2DefaultUserID), SM2DefaultUserID, strlen(asymmetricKeyDB.pkValue), asymmetricKeyDB.pkValue, lenOfRSSign, ascSign, lenOfData, data)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE403:: UnionSignWithHsmK4!\n");	
						return(ret);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE403:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("非法的加密机指令类型");
					return(errCodeParameter);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE403:: algorithmID[%d] error!\n", asymmetricKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	return(0);
}
