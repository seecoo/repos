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
#include "asymmetricKeyDB.h"
#include "UnionMD5.h"
#include "UnionSHA1.h"
#include "unionHsmCmdVersion.h"

/***************************************
�������:	E170
������:		˽Կǩ��
��������:	˽Կǩ��
***************************************/

int UnionDealServiceCodeE170(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				len = 0;
	int				lenOfVkValue = 0;
	char				keyName[160];
	char				dataFillMode[32];
	char				hashID[32];
	char				data[4096];
	char				tmpData[5120];
	char				sign[1024];
	char				vkIndex[32];
	char				userID[1024];
	char                    	ipAddr[64];
	char                    	sql[1024];
	int				lenOfData = 0;
	char				shapadStr[]="3021300906052B0E03021A05000414";
	char				md5padStr[]="3020300C06082A864886F70D020505000410";

	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	
	// ��Կ����	
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE170:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);
	
	// ��ȡ��Կ��Կ��Ϣ
	if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,1,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE170:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",keyName);
		return(ret);
	}

	// ���˽Կ����
	if (asymmetricKeyDB.keyType != 0 && asymmetricKeyDB.keyType != 2)
	{
		UnionUserErrLog("in UnionDealServiceCodeE170:: keyType[%d] is not 0 or 2!\n",asymmetricKeyDB.keyType);
		UnionSetResponseRemark("˽Կ����[%d]��֧��ǩ��,����Ϊ[0��2]");
		return(errCodeHsmCmdMDL_InvalidKeyType);
	}

	// ���ݲ�λ��ʽ	
	if ((ret = UnionReadRequestXMLPackageValue("body/dataFillMode",dataFillMode,sizeof(dataFillMode))) <= 0)
		snprintf(dataFillMode,sizeof(dataFillMode),"1");
	else
	{
		if ((dataFillMode[0] != '0') && (dataFillMode[0] != '1'))
		{
			UnionUserErrLog("in UnionDealServiceCodeE170:: dataFillMode[%s] error!\n",dataFillMode);
			UnionSetResponseRemark("�����������dataFillMode[%s]",dataFillMode);
			return(errCodeParameter);
		}
	}
	
	// ǩ������
	if ((lenOfData = UnionReadRequestXMLPackageValue("body/data",data,sizeof(data))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE170:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
		return(lenOfData);
	}
	data[lenOfData] = 0;
	
	// HASH�㷨��ʶ
	if ((ret = UnionReadRequestXMLPackageValue("body/hashID",hashID,sizeof(hashID))) > 0)
	{
		// RSA�㷨
		if (asymmetricKeyDB.algorithmID == conAsymmetricAlgorithmIDOfRSA)
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
				UnionUserErrLog("in UnionDealServiceCodeE170:: hashID[%s] Invalid!\n",hashID);
				UnionSetResponseRemark("RSA�㷨��֧�ִ�hashID[%s]�㷨��ʶ",hashID);
				return(errCodeParameter);
			}
		}
		else
		{
			// SM2
			if (strcmp(hashID,"03") == 0)
			{
				// ��ȡ�û���ʶ
				if ((ret = UnionReadRequestXMLPackageValue("body/userID",userID,sizeof(userID))) <= 0)
				{
					snprintf(userID,sizeof(userID),"%s","1234567812345678");
					UnionLog("in UnionDealServiceCodeE170:: set default userID[%s]\n",userID);
				}
				else
					userID[ret] = 0;
			}
		}
	}
	
	lenOfVkValue = aschex_to_bcdhex(asymmetricKeyDB.vkValue,strlen(asymmetricKeyDB.vkValue),tmpData);
	memcpy(asymmetricKeyDB.vkValue,tmpData,lenOfVkValue);
	asymmetricKeyDB.vkValue[lenOfVkValue] = 0;

	len = 0;
	
	//��������	
	if (asymmetricKeyDB.vkStoreLocation == 1)
	{
        	snprintf(sql,sizeof(sql),"select * from hsm where hsmGroupID = '%s' and enabled = 1",asymmetricKeyDB.hsmGroupID);

        	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeE170:: UnionExecRealDBSql[%s]!\n",sql);
                	return(ret);
        	}
        	else if (ret == 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeE170:: hsmGroupID[%s]��û�п��������!\n",asymmetricKeyDB.hsmGroupID);
                	return(errCodeParameter);
        	}

        	UnionLocateXMLPackage("detail", 1);

        	if ((ret = UnionReadXMLPackageValue("ipAddr", ipAddr, sizeof(ipAddr))) < 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeE170:: UnionReadXMLPackageValue[%s]!\n","ipAddr");
                	return(ret);
        	}
		ipAddr[ret] = 0;
        	UnionSetUseSpecHsmIPAddrForOneCmd(ipAddr);
	}
	
	switch(asymmetricKeyDB.algorithmID)
	{
		case	conAsymmetricAlgorithmIDOfRSA:	// RSA�㷨

			// ����Ƿ��������
			if (asymmetricKeyDB.vkStoreLocation == 1)
				snprintf(vkIndex,sizeof(vkIndex),"%s",asymmetricKeyDB.vkIndex);	
			else
				snprintf(vkIndex,sizeof(vkIndex),"%s","99");

			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					if ((lenOfData = UnionHsmCmd37(dataFillMode[0],vkIndex,lenOfVkValue,asymmetricKeyDB.vkValue,lenOfData,data,sign,sizeof(sign))) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE170:: UnionHsmCmd37!\n");
						return(lenOfData);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE110:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
					return(errCodeParameter);
			}
			bcdhex_to_aschex(sign,lenOfData,tmpData);
			tmpData[lenOfData*2] = 0;
			break;
		case	conAsymmetricAlgorithmIDOfSM2:	// SM2�㷨

			// �������
			// modify by leipp 20151224
			if (strcmp(hashID,"03") != 0)
			{
				if (lenOfData != 64)
				{
					UnionUserErrLog("in UnionDealServiceCodeE170:: data[%s][%d] != 64!\n",data,lenOfData);
					UnionSetResponseRemark("���ݳ���[%d] != 64",lenOfData);
					return(errCodeParameter);
				}
			}
			else if ((lenOfData % 2 != 0) || (!UnionIsBCDStr(data)))
			{
				UnionUserErrLog("in UnionDealServiceCodeE170:: data[%s] is invalid!\n",data);
				UnionSetResponseRemark("���ݲ�����ʮ�����Ƹ�ʽ");
				return(errCodeParameter);
			}
			// modify end 20151224

			// ����Ƿ��������
			// modify by lisq 20141210 
			//if (asymmetricKeyDB.vkStoreLocation == 1)
			if (asymmetricKeyDB.vkStoreLocation != 0)
				snprintf(vkIndex,sizeof(vkIndex),"%s",asymmetricKeyDB.vkIndex);	
			else
				snprintf(vkIndex,sizeof(vkIndex),"%s","-1");	

			if (strcmp(hashID,"03") != 0)
				snprintf(hashID,sizeof(hashID),"%s","01");
			else
				snprintf(hashID,sizeof(hashID),"%s","02");

			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					if ((lenOfData = UnionHsmCmdK3(hashID,strlen(userID),userID,lenOfData,data,atoi(vkIndex),lenOfVkValue,asymmetricKeyDB.vkValue,tmpData,sizeof(tmpData))) < 0)	
					{
						UnionUserErrLog("in UnionDealServiceCodeE170:: UnionHsmCmdK3!\n");
						return(lenOfData);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE110:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
					return(errCodeParameter);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE170:: asymmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	
	// ������Ӧǩ��
	if ((ret = UnionSetResponseXMLPackageValue("body/sign",tmpData)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE170:: UnionSetResponseXMLPackageValue[%s]!\n","body/sign");
		return(ret);
	}
	
	return(0);
}
