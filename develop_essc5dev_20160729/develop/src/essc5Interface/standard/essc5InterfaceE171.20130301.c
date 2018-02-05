//	Author:		������	
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
#include "defaultIBMPinOffsetDef.h"
#include "asymmetricKeyDB.h"
#include "UnionMD5.h"
#include "UnionSHA1.h"
#include "unionHsmCmdVersion.h"
#include "symmetricKeyDB.h"

/***************************************
  �������:	E171
  ������:	��Կ��ǩ
  ��������:	��Կ��ǩ
 ***************************************/
int UnionDealServiceCodeE171(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				mode;
	char				keyName[160];
	char				pkValue[1024];
	char				algorithmID[32];
	char				dataFillMode[32];
	char				hashID[32];
	char				data[4096];
	char				tmpBuf[1024];
	char				sign[1024];
	char				userID[1024];
	char				tmpData[5120];
	int				vkIndex = -1;
	int				keyLen = 0;
	int				lenOfSign = 0;
	int				keyType;
	int 				pkExponent = 65537;
	int 				tpkExponent = 0;
	char				shapadStr[]="3021300906052B0E03021A05000414";
	char				md5padStr[]="3020300C06082A864886F70D020505000410";
	int				lenOfData = 0;

	TUnionAsymmetricKeyDB		asymmetricKeyDB;

	// ģʽ
	// 1����Կ����
	// 2�������Կ	
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE171:: UnionReadRequestXMLPackageValue[%s]!\n","body/mode");
		return(ret);
	}
	else
	{
		tmpBuf[ret] = 0;
		mode = atoi(tmpBuf);
	}

	if ((mode != 1) && (mode != 2))
	{
		UnionUserErrLog("in UnionDealServiceCodeE171:: mode[%d] Invalid\n",mode);
		UnionSetResponseRemark("ģʽmode[%d]��Ч",mode);
		return(errCodeParameter);
	}

	// ǩ������
	if ((lenOfData = UnionReadRequestXMLPackageValue("body/data",data,sizeof(data))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE171:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
		return(lenOfData);
	}
	data[lenOfData] = 0;

	// ǩ��
	if ((lenOfSign = UnionReadRequestXMLPackageValue("body/sign",sign,sizeof(sign))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE171:: UnionReadRequestXMLPackageValue[%s]!\n","body/sign");
		return(lenOfSign);
	}
	UnionFilterHeadAndTailBlank(sign);
	if ((lenOfSign = strlen(sign)) > 0)
	{
		if (!UnionIsBCDStr(sign))
		{
			UnionUserErrLog("in UnionDealServiceCodeE171:: sign[%s] is error!\n",sign);
			UnionSetResponseRemark("ǩ���Ƿ�,����Ϊʮ��������");
			return(errCodeParameter);
		}
	}
	sign[lenOfSign] = 0;

	tpkExponent = UnionGetCurrentRsaExponent();	
	// ģʽ
	if (mode == 1)	// ��Կ����
	{
		// ��ȡ��Կ����
		if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE171:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
			return(ret);
		}
		UnionFilterHeadAndTailBlank(keyName);

		if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,1,&asymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE171:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",keyName);
			return(ret);
		}

		// ���˽Կ����
		if (asymmetricKeyDB.keyType != 0 && asymmetricKeyDB.keyType != 2)
		{
			UnionUserErrLog("in UnionDealServiceCodeE171:: keyType[%d] is not 0 or 2!\n",asymmetricKeyDB.keyType);
			UnionSetResponseRemark("˽Կ����[%d]��֧��ǩ��,����Ϊ[0��2]");
			return(errCodeHsmCmdMDL_InvalidKeyType);
		}

		snprintf(pkValue,sizeof(pkValue),"%s",asymmetricKeyDB.pkValue);

		if (asymmetricKeyDB.algorithmID == conAsymmetricAlgorithmIDOfRSA)
		{
			keyType = conAsymmetricAlgorithmIDOfRSA;
			keyLen = asymmetricKeyDB.keyLen / 8;
			if(asymmetricKeyDB.pkExponent >0)
			{
				UnionSetCurrentRsaExponent(asymmetricKeyDB.pkExponent);
			}
		}
		else
			keyType = conAsymmetricAlgorithmIDOfSM2;

		// add by leipp 20151225 ����Կ���ڼ��ܻ���ʱ,�Ӽ��ܻ��л�ȡ
		if (asymmetricKeyDB.vkStoreLocation != 0)
			vkIndex = atoi(asymmetricKeyDB.vkIndex);
	}
	else	// �����Կ
	{
		//��Կ	modeΪ2ʱ����
		if ((ret = UnionReadRequestXMLPackageValue("body/pkValue",pkValue,sizeof(pkValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE171:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkValue");
			return(ret);
		}
		pkValue[ret] = 0;
		UnionFilterHeadAndTailBlank(pkValue);

		//�㷨��ʶ	modeΪ2ʱ���ڣ�RSA��SM2
		if ((ret = UnionReadRequestXMLPackageValue("body/algorithmID",algorithmID,sizeof(algorithmID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE171:: UnionReadRequestXMLPackageValue[%s]!\n","body/algorithmID");
			return(ret);
		}
		UnionFilterHeadAndTailBlank(algorithmID);

		if (strcasecmp(algorithmID,"RSA") == 0)
		{
			// modify by leipp 20150921
			//keyType = conAsymmetricAlgorithmIDOfSM2;	
			keyType = conAsymmetricAlgorithmIDOfRSA;	
			keyLen = strlen(pkValue) / 2;
			if ((ret = UnionReadRequestXMLPackageValue("body/pkExponent",tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UnionSetCurrentRsaExponent(pkExponent);
			}
			else
			{
				tmpBuf[ret] = 0;
				pkExponent =atoi(tmpBuf);
				if(pkExponent != 3 && pkExponent != 65537)
				{
					UnionUserErrLog("in UnionDealServiceCodeE171:: the pkExponent is not 3 or 65537!\n");
					UnionSetResponseRemark("ָ������ȷ[%d]",pkExponent);
					return(errCodeParameter);
				}
				UnionSetCurrentRsaExponent(pkExponent);
			}
		}
		else if (strcasecmp(algorithmID,"SM2") == 0)
			keyType = conAsymmetricAlgorithmIDOfSM2;
		else
		{
			UnionUserErrLog("in UnionDealServiceCodeE171:: algorithmID[%s]!\n",algorithmID);
			UnionSetResponseRemark("�㷨��ʶ[%s]����ȷ",algorithmID);
			return(errCodeParameter);
		}
	}

	// ���ݲ�λ��ʽ	
	if ((ret = UnionReadRequestXMLPackageValue("body/dataFillMode",dataFillMode,sizeof(dataFillMode))) <= 0)
		snprintf(dataFillMode,sizeof(dataFillMode),"1");
	else
	{
		if ((dataFillMode[0] != '0') && (dataFillMode[0] != '1'))
		{
			UnionUserErrLog("in UnionDealServiceCodeE171:: dataFillMode[%s] error!\n",dataFillMode);
			UnionSetResponseRemark("�����������dataFillMode[%s]",dataFillMode);
			return(errCodeParameter);
		}
	}

	// HASH�㷨��ʶ
	if ((ret = UnionReadRequestXMLPackageValue("body/hashID",hashID,sizeof(hashID))) > 0)
	{
		// RSA�㷨
		if (keyType == conAsymmetricAlgorithmIDOfRSA)
		{	
			if (strcmp(hashID,"01") == 0)
			{
				UnionSHA1((unsigned char *)data,lenOfData,(unsigned char *)tmpData);	
				lenOfData = aschex_to_bcdhex(shapadStr,strlen(shapadStr),data);
				memcpy(data+lenOfData,tmpData,20);
				lenOfData += 20;
				data[lenOfData] = 0;
			}
			else if (strcmp(hashID,"02") == 0)
			{
				ret = sprintf(tmpData,"%s",md5padStr);
				UnionMD5((unsigned char *)data,lenOfData,(unsigned char *)tmpData+ret);
				lenOfData = aschex_to_bcdhex(tmpData,ret+32,data);
				data[lenOfData] = 0;
			} 

			if (strcmp(hashID,"03") == 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE171:: hashID[%s] Invalid!\n",hashID);
				UnionSetResponseRemark("RSA�㷨��֧�ִ�hashID[%s]�㷨��ʶ",hashID);
				return(errCodeParameter);
			}
		}
		else
		{
			if (strcmp(hashID,"03") == 0)
			{
				// ��ȡ�û���ʶ
				if ((ret = UnionReadRequestXMLPackageValue("body/userID",userID,sizeof(userID))) <= 0)
				{
					snprintf(userID,sizeof(userID),"%s","1234567812345678");
					UnionLog("in UnionDealServiceCodeE171:: set default userID[%s]\n",userID);
				}
				else
					userID[ret] = 0;
			}
		}
	}

	switch(keyType)
	{
		case	conAsymmetricAlgorithmIDOfRSA:	// RSA�㷨

			lenOfSign = aschex_to_bcdhex(sign,lenOfSign,tmpData);
			memcpy(sign,tmpData,lenOfSign);
			sign[lenOfSign] = 0;

			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmd38(dataFillMode[0],NULL,lenOfSign,sign,lenOfData,data,pkValue)) < 0)
					{
						UnionSetCurrentRsaExponent(tpkExponent);
						//modify by lisq 20150313 ��ǰ�汾��Կʧ�ܺ��Ծɰ汾��Կ
						if ((mode == 1) && UnionOldVersionAsymmetricKeyIsUsed(&asymmetricKeyDB))
						{
							snprintf(pkValue,sizeof(pkValue),"%s",asymmetricKeyDB.oldPKValue);
							if ((ret = UnionHsmCmd38(dataFillMode[0],NULL,lenOfSign,sign,lenOfData,data,pkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE171:: UnionHsmCmd38 old version pk!\n");
								UnionSetCurrentRsaExponent(tpkExponent);
								return(ret);
							}
						}
						else
						{	UnionUserErrLog("in UnionDealServiceCodeE171:: [%s] old version key not used!\n", asymmetricKeyDB.keyName);
							UnionSetCurrentRsaExponent(tpkExponent);
							return(ret);
						}
						//modify by lisq 20150313 end
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE171:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
					UnionSetCurrentRsaExponent(tpkExponent);
					return(errCodeParameter);
			}
			UnionSetCurrentRsaExponent(tpkExponent);
			break;
		case	conAsymmetricAlgorithmIDOfSM2:	// SM2�㷨

			// ������ݳ���
			// modify by leipp 20151224
			if (strcmp(hashID,"03") != 0)
			{
				if (lenOfData != 64)
				{
					UnionUserErrLog("in UnionDealServiceCodeE171:: data[%s][%d] != 64!\n",data,lenOfData);
					UnionSetResponseRemark("���ݳ���[%d] != 64",lenOfData);
					return(errCodeParameter);
				}
			}
			else if ((lenOfData % 2 != 0) || (!UnionIsBCDStr(data)))
			{
				UnionUserErrLog("in UnionDealServiceCodeE171:: data[%s] is invalid!\n",data);
				UnionSetResponseRemark("���ݲ�����ʮ�����Ƹ�ʽ");
				return(errCodeParameter);
			}
			// modify end 20151224

			if (strcmp(hashID,"03") != 0)
				strcpy(hashID,"01");
			else
				strcpy(hashID,"02");

			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmdK4(vkIndex,hashID,strlen(userID),userID,strlen(pkValue),pkValue,lenOfSign,sign,lenOfData,data)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE171:: UnionHsmCmdK4!\n");
						//modify by lisq 20150313 ��ǰ�汾��Կʧ�ܺ��Ծɰ汾��Կ
						if ((mode == 1) && UnionOldVersionAsymmetricKeyIsUsed(&asymmetricKeyDB))
						{
							snprintf(pkValue,sizeof(pkValue),"%s",asymmetricKeyDB.oldPKValue);
							if ((ret = UnionHsmCmdK4(vkIndex,hashID,strlen(userID),userID,strlen(pkValue),pkValue,lenOfSign,sign,strlen(data),data)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE171:: UnionHsmCmdK4 old version pk!\n");
								return(ret);
							}
						}
						else
						{
							UnionUserErrLog("in UnionDealServiceCodeE171:: [%s] old version key not used!\n", asymmetricKeyDB.keyName);
							return(ret);
						}
						//modify by lisq 20150313 end
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE171:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
					return(errCodeParameter);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE171: algorithmID[%d] error!\n",keyType);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	return(0);
}
