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
#include "symmetricKeyDB.h"
#include "UnionXOR.h"
#include "unionHsmCmdVersion.h"

/***************************************
�������:	E160
������:		��������
��������:	��������
***************************************/
int UnionDealServiceCodeE160(PUnionHsmGroupRec phsmGroupRec)
{
	int				i;
	int				ret;
	int				msgNo;
	int				len;
	int				lenFlag = 0;
	int				addLen = 0;
	int				offset = 0;
	int				lenOfPerData = 1024;
	int				lenOfData = 0;
	int				lenOfOut = 0;
	int				mode = 1;
	int				algorithmID = 0;
	int				msgNum = 0;
	char				tmpBuf[32];
	char				tmpData[20480];
	char				data[10240];
	char				keyByLMK[64];
	char				keyName[160];
	char				keyValue[64];
	char				checkValue[32];
	int				dataType = 0;
	char				iv[64];
	char				format[32];
	char				tmpAlgorithmID[32];


	TUnionSymmetricKeyType		keyType;
	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;
	
	// ģʽ
	// 1��ָ����Կ����,Ĭ��Ϊ1
	// 2��ָ����Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) <= 0)
		mode = 1;
	else
		mode = atoi(tmpBuf);

	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE160:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}

	// ��ȡ��Կ��Ϣ
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1,&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE160:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
		return(ret);
	}
	// ��ȡ��Կֵ
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE160:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(errCodeParameter);
	}
	
	if (mode == 1)
	{
		if ((symmetricKeyDB.keyType != conEDK) && (symmetricKeyDB.keyType != conZEK) && (symmetricKeyDB.keyType != conWWK))
		{
			UnionUserErrLog("in UnionDealServiceCodeE160:: is not edk or zek or wwk [%s]!\n",keyName);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}
		
		// SM4 �㷨
		if (symmetricKeyDB.algorithmID  == conSymmetricAlgorithmIDOfSM4)
		{
			if (symmetricKeyDB.keyType != conZEK)
			{
				UnionUserErrLog("in UnionDealServiceCodeE160:: is not zmk or zek [%s]!\n",keyName);
				UnionSetResponseRemark("��Կ����[%s],SM4�㷨ֻ֧��zek��Կ����",keyName);
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
			UnionUserErrLog("in UnionDealServiceCodeE160:: is not zmk [%s]!\n",keyName);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}

		// ��Կ����
		if ((ret = UnionReadRequestXMLPackageValue("body/keyValue",keyValue,sizeof(keyValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE160:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyValue");
			return(ret);
		}
				
		// ��Կ����
		keyType = conZEK;

		switch(symmetricKeyDB.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:	// DES�㷨		
				// ��Կת�ɱ���LMK�Լ���
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdA6(NULL,keyType,psymmetricKeyValue->keyValue,keyValue,keyByLMK,checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE160:: UnionHsmCmdA6!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE160:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
				break;
			case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
				// ����Կת�ɱ���LMK�Լ���
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdSV(keyType,psymmetricKeyValue->keyValue,keyValue,keyByLMK,checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE160:: UnionHsmCmdSV!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE160:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE160:: symmetricKeyDB.algorithmID[%d] error!\n",symmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}			
	}
	
	if (keyType == conZEK || keyType == conWWK)
	{
		// �㷨��ʶ 0:ECB 1:CBC
		if ((ret = UnionReadRequestXMLPackageValue("body/algorithmID",tmpBuf,sizeof(tmpBuf))) <= 0)
			algorithmID = 0;
		else
			algorithmID = atoi(tmpBuf);

		if (algorithmID != 0)
		{
			// IV
			if ((ret = UnionReadRequestXMLPackageValue("body/iv",iv,sizeof(iv))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE160:: UnionReadRequestXMLPackageValue[%s]!\n","body/iv");
				return(ret);
			}
			else
        		{
				UnionFilterHeadAndTailBlank(iv);
				if (symmetricKeyDB.algorithmID  == conSymmetricAlgorithmIDOfSM4)
				{
					if (!UnionIsBCDStr(iv) || strlen(iv) != 32)
					{
						UnionUserErrLog("in UnionDealServiceCodeE160:: iv[%s] is error!\n",iv);
						UnionSetResponseRemark("��ʼ����[%s]�Ƿ�,����Ϊʮ��������,�ҳ��ȱ���Ϊ32",iv);
						return(errCodeParameter);
					}
				}
				else
				{
					if (!UnionIsBCDStr(iv) || strlen(iv) != 16)
					{
						UnionUserErrLog("in UnionDealServiceCodeE160:: iv[%s] is error!\n",iv);
						UnionSetResponseRemark("��ʼ����[%s]�Ƿ�,����Ϊʮ��������,�ҳ��ȱ���Ϊ16",iv);
						return(errCodeParameter);
					}
				}
        		}
		}
	}

	// ����
	if ((lenOfData = UnionReadRequestXMLPackageValue("body/data",data,sizeof(data) - 16)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE160:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
		return(lenOfData);
	}
	data[lenOfData] = 0;
	if (lenOfData == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE160:: data is null!\n");
		return(errCodeParameter);
	}

	// ��������
	if ((ret = UnionReadRequestXMLPackageValue("body/dataType",tmpBuf,sizeof(tmpBuf))) < 0)
		dataType = 0;
	else
	{
		tmpBuf[ret] = 0;
		dataType = atoi(tmpBuf);
	}
	// modify  by leipp 20150327
	//if (dataType == 1 && symmetricKeyDB.keyType == conZEK && symmetricKeyDB.algorithmID == conSymmetricAlgorithmIDOfDES)
	
	//if (dataType == 1 && symmetricKeyDB.keyType == conZEK)// modify  by chenwd  20150618
	if (dataType == 1)
	{
		if ((lenOfData % 2 != 0) || !UnionIsBCDStr(data))       
		{                       
			UnionUserErrLog("in UnionDealServiceCodeE160:: data[%d][%s] error!\n",lenOfData,data);
			UnionSetResponseRemark("���ݴ���, ����Ϊʮ����������");
			return(errCodeParameter);
		}
		aschex_to_bcdhex(data,lenOfData,tmpData);
		lenOfData = lenOfData / 2;
		memcpy(data,tmpData,lenOfData);
	}
	// modify end

	// ��ʽ
	if ((ret = UnionReadRequestXMLPackageValue("body/format",format,sizeof(format))) < 0)
		format[0] = '0';
		
	// 0���������
	// 1��4�ֽ����ĳ���+����+��λ��0��
	// 2�����0x00
	if (symmetricKeyDB.algorithmID == conSymmetricAlgorithmIDOfDES)
		lenFlag = 8;
	else
		lenFlag = 16;

	if (format[0] == '0')
	{
		if (symmetricKeyDB.keyType != conZEK)
		{
			if ((lenOfData % lenFlag) != 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE160:: lenOfData[%d]��%d��������!\n",lenOfData,lenFlag);
				UnionSetResponseRemark("���ݳ��ȷ�%d��������",lenFlag);
				return(errCodeParameter);
			}
		}
	}
	else if (format[0] == '1')
	{
		memmove(data + 4,data,lenOfData);
		sprintf(data,"%04d%s",lenOfData,data + 4);
		if ((lenOfData += 4) % lenFlag != 0)
		{
			addLen = lenFlag - lenOfData % lenFlag;
			memset(data + lenOfData,'0',addLen);
			lenOfData += addLen;
		}
	}
	else if (format[0] == '2')
	{
		if (lenOfData % lenFlag != 0)
		{
			addLen = lenFlag - lenOfData % lenFlag;
			memset(data + lenOfData,0,addLen);
			lenOfData += addLen;
		}
	}
	else if (format[0] == '3')
	{
		data[lenOfData] = 0x80;
		if (lenOfData % lenFlag != 0)
			addLen = lenFlag - lenOfData % lenFlag;
		else
			addLen = 8;

		memset(data + lenOfData + 1,0x00,addLen);
		lenOfData += addLen;
	}
	else
	{
		UnionUserErrLog("in UnionDealServiceCodeE160:: lenOfData[%d]��%d��������!\n",lenOfData,lenFlag);
		UnionSetResponseRemark("format�Ƿ�");
		return(errCodeParameter);
	}

	// ����������
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
			case	conSymmetricAlgorithmIDOfDES:	// DES�㷨		
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if (keyType == conEDK)
						{
							if ((len = UnionHsmCmd50('0',keyByLMK,len,data + offset,tmpData + lenOfOut,sizeof(tmpData) - lenOfOut)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE160:: UnionHsmCmd50[%d][%s]!\n",len,data + offset);
								return(len);
							}
						}
						else if (keyType == conZEK)
						{
							if ((len = UnionHsmCmdE0(0,msgNo,algorithmID + 1,strlen(keyByLMK),keyByLMK,0,iv,len,(unsigned char *)data + offset,(unsigned char *)tmpData + lenOfOut,&len,sizeof(tmpData) - lenOfOut)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE160:: UnionHsmCmdE0[%d][%s]!\n",len,data + offset);
								return(len);
							}
						}
						else if (keyType == conWWK)
						{
							if ((len = UnionHsmCmdB8(keyByLMK,1,algorithmID,iv,len,data + offset,tmpData + lenOfOut,sizeof(tmpData) - lenOfOut)) < 0)	
							{
								UnionUserErrLog("in UnionDealServiceCodeE160:: UnionHsmCmdB8[%d][%s]!\n",len,data + offset);
								return(len);
							}
						}
						lenOfOut += len;
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE160:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
				break;
			case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
				sprintf(tmpAlgorithmID,"%02d",algorithmID + 1);
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((len = UnionHsmCmdWA(2,tmpAlgorithmID,"00A",1,keyByLMK,checkValue,iv,len,(unsigned char *)data + offset,tmpData + lenOfOut)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE160:: UnionHsmCmdWA[%d][%s]!\n",len,data + offset);
							return(len);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE160:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
				lenOfOut += len;
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE160:: symmetricKeyDB.algorithmID[%d] error!\n",symmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
	}
	
	// ����data
	if (symmetricKeyDB.keyType != conEDK)
	{
		len = bcdhex_to_aschex(tmpData,lenOfOut,data);
		memcpy(tmpData,data,len);
		tmpData[len] = 0;
	}

	if ((ret = UnionSetResponseXMLPackageValue("body/data",tmpData)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE160:: UnionSetResponseXMLPackageValue!\n");
		return(ret);
	}
	
	return(0);
}
