//	Author:		������
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
#include "asymmetricKeyDB.h"
#include "UnionMD5.h"
#include "UnionSHA1.h"
#include "base64.h"
#include "unionHsmCmdVersion.h"
#include "unionREC.h"
#include "unionCertFun.h"
#include "unionCertFunSM2.h"

/***************************************
�������:	E403
������:		����ǩ����ǩ
��������:	����ǩ����ǩ
***************************************/

int UnionDealServiceCodeE403(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				lenOfVkValue = 0;
	char				dataBcd[8192];
	char				data[8192*2];
	char				tmpData[1024];
	char				sign[1024];
	char				bankID[48];
	char				certVersion[8];
	int				lenOfData = 0;
	char				pix[16];
	char				shapadStr[48];
	int				len = 0;
	//char				tmpStr[30+1];
	char				keyName[136];

	char				tmpData1[4096];
	
	int				algFlagOfMyBankCert = 0;
	char				algFlag[16];
	
	int				vkIndex = 0;
	char				signDer[512];
	char				bcdSign[512];

	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	
	// Ӧ��ID
	if ((ret = UnionReadRequestXMLPackageValue("body/pix",pix,sizeof(pix))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE403:: UnionReadRequestXMLPackageValue[%s]!\n","body/pix");
		return(ret);
	}
	pix[ret] = 0;
	UnionFilterHeadAndTailBlank(pix);
	if (strlen(pix) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE403:: pix can not be null!\n");
                UnionSetResponseRemark("Ӧ��ID����Ϊ��!");
                return(errCodeParameter);
        }
	if (!UnionIsBCDStr(pix))
        {
                UnionUserErrLog("in UnionDealServiceCodeE403:: pix[%s] is error!\n",pix);
                UnionSetResponseRemark("Ӧ��ID[%s]�Ƿ�,����Ϊʮ��������",pix);
                return(errCodeParameter);
        }

	// �����л�����
	if ((ret = UnionReadRequestXMLPackageValue("body/bankID",bankID,sizeof(bankID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE403:: UnionReadRequestXMLPackageValue[%s]!\n","body/bankID");
		return(ret);
	}
	bankID[ret] = 0;
	UnionFilterHeadAndTailBlank(bankID);

	// ֤��汾��
	if ((ret = UnionReadRequestXMLPackageValue("body/certVersion",certVersion,sizeof(certVersion))) <= 0)
		strcpy(certVersion,"01");
	else
		certVersion[ret] = 0;
	UnionFilterHeadAndTailBlank(certVersion);

	// ǩ������
	if ((lenOfData = UnionReadRequestXMLPackageValue("body/data",data,sizeof(data))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE403:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
		return(lenOfData);
	}
	else if (lenOfData == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE403:: data can not be null!\n");
		UnionSetResponseRemark("ǩ�����ݲ���Ϊ��!");
		return(errCodeParameter);
	}
	data[lenOfData] = 0;
	
	// ����ʹ�õ�֤����㷨��ʶ
	if((algFlagOfMyBankCert = UnionReadIntTypeRECVar("algFlagOfMyBankCert")) < 0)
	{
		algFlagOfMyBankCert = 2;
	}
	switch(algFlagOfMyBankCert)
	{
		case	2:	
			//RSA AND SM2
			// ��ȡ�㷨��ʶ
			if((ret = UnionReadRequestXMLPackageValue("body/algFlag", algFlag, sizeof(algFlag))) <= 0)
			{
				// ƴ����Կ���ƣ� RSA
				sprintf(algFlag, "0");
				sprintf(keyName,"CNAPS2.%s-%s-%s.cer",pix,bankID,certVersion);
			}
			else
			{
				algFlag[ret] = 0;
				if(algFlag[0] != '0' && algFlag[0] != '1')
				{
					UnionUserErrLog("in UnionDealServiceCodeE401:: algFlag[%s] error!\n", algFlag);
					UnionSetResponseRemark("algFlag[%s]ӦΪ0��1", algFlag);
					return(errCodeParameter);
				}
				if(algFlag[0] == '0')
				{
					// ƴ����Կ���ƣ� RSA
					sprintf(keyName,"CNAPS2.%s-%s-%s.cer",pix,bankID,certVersion);
				}
				else
				{
					// ƴ����Կ���ƣ� SM2
					sprintf(keyName,"CNAPS2.%s-%s-%s.cer_sm2",pix,bankID,certVersion);
				}
			}
			break;
		case	0:	
			// RSA
			sprintf(algFlag, "0");
			sprintf(keyName,"CNAPS2.%s-%s-%s.cer",pix,bankID,certVersion);
			break;
		case	1:
			// SM2
			sprintf(algFlag, "1");
			sprintf(keyName,"CNAPS2.%s-%s-%s.cer_sm2",pix,bankID,certVersion);
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE401:: algFlagOfMyBankCert[%d] error!\n", algFlagOfMyBankCert);
			UnionSetResponseRemark("�ͻ������õ�\"����ʹ�õ�֤����㷨��ʶ\"�����д�ӦΪ0,1��2");
			return(errCodeParameter);
	}
	
	// ��ȡ��Կ��Կ��Ϣ
	//memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));
	//snprintf(keyName,sizeof(keyName),"CNAPS2.%s-%s-%s.cer",pix,bankID,certVersion);
	if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,1,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE403:: UnionReadAsymmetricKeyDBRec asymmetricKeyDB.keyName[%s]!\n",keyName);
		return(ret);
	}
	if(asymmetricKeyDB.vkStoreLocation == 0)
	{
		lenOfVkValue = aschex_to_bcdhex(asymmetricKeyDB.vkValue,strlen(asymmetricKeyDB.vkValue),tmpData1);
		tmpData1[lenOfVkValue] = 0;
		memcpy(asymmetricKeyDB.vkValue,tmpData1,lenOfVkValue);
		asymmetricKeyDB.vkValue[lenOfVkValue] = 0;
	}
	
	switch(asymmetricKeyDB.algorithmID)
	{
		case	conAsymmetricAlgorithmIDOfRSA:
			// HASH�㷨��ʶ 
			// modify 20150331 leipp	��Ϊ������������������2M���ң����Լ���hash�ŵ���javaAPI
			//UnionSHA1((unsigned char *)data,lenOfData,(unsigned char *)tmpStr);
			memset(tmpData,0,sizeof(tmpData));
			len = sprintf(shapadStr,"%s","3021300906052B0E03021A05000414");
			len = aschex_to_bcdhex(shapadStr,len,tmpData);
	
			lenOfData = aschex_to_bcdhex(data,lenOfData,dataBcd);
			memcpy(tmpData + len,dataBcd,lenOfData);
			len += lenOfData;
			tmpData[len] = 0;
			
			if(!asymmetricKeyDB.vkStoreLocation)
				strcpy(asymmetricKeyDB.vkIndex, "99");
			
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					if ((lenOfData = UnionHsmCmd37('1',asymmetricKeyDB.vkIndex,lenOfVkValue,asymmetricKeyDB.vkValue,len,tmpData,sign,sizeof(sign))) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE403:: UnionSignWithHsm37!\n");	
						return(lenOfData);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE403:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
					return(errCodeParameter);
			}
			break;
		case	conAsymmetricAlgorithmIDOfSM2:
			if(asymmetricKeyDB.vkStoreLocation == 0)
				vkIndex = -1;
			else
				vkIndex = atoi(asymmetricKeyDB.vkIndex);
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					//if ((lenOfData = UnionHsmCmdK3("02", strlen(SM2DefaultUserID), SM2DefaultUserID, lenOfData, data, vkIndex, lenOfVkValue, asymmetricKeyDB.vkValue, sign, sizeof(sign))) < 0)
					if ((lenOfData = UnionHsmCmdK3("01", strlen(SM2DefaultUserID), SM2DefaultUserID, lenOfData, data, vkIndex, lenOfVkValue, asymmetricKeyDB.vkValue, sign, sizeof(sign))) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE403:: UnionSignWithHsmK3!\n");	
						return(lenOfData);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE403:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
					return(errCodeParameter);
			}
			aschex_to_bcdhex(sign, 128, bcdSign);
			bcdSign[64] = 0;
			if((ret = UnionRSToSM2SigDer((unsigned char*)bcdSign, (unsigned char*)signDer, &lenOfData)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE403:: UnionRSToSM2SigDer failed ret = [%d]!\n", ret);
				return(ret);
			}
			memcpy(sign, signDer, lenOfData);
			sign[lenOfData] = 0;
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE403:: algorithmID[%d] error!\n", asymmetricKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	
	// ǩ��ת��
	to64frombits((unsigned char *)tmpData,(unsigned char *)sign,lenOfData);
	
	// ������Ӧǩ��
	if ((ret = UnionSetResponseXMLPackageValue("body/sign",tmpData)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE403:: UnionSetResponseXMLPackageValue[%s]!\n","body/sign");
		return(ret);
	}
	
	return(0);
}
