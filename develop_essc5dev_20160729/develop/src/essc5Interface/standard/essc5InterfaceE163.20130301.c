//	Author:		zhouxw
//	Copyright:	Union Tech. Guangzhou
//	Date:		2015-06-01

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

int UnionIsUppercaseHEX(char *str);
int UnionTranslateHsmKeyTypeString(TUnionDesKeyType type,char *keyTypeString);

/***************************************
  �������:	E163
  ������:		��ZEK����ת����һ��ZEK����
  ��������:	��ZEK����ת����һ��ZEK����
 ***************************************/
int UnionDealServiceCodeE163(PUnionHsmGroupRec phsmGroupRec)
{
	int		ret;
	int		algorithmID = 0;
	char		zekKeyName1[128];
	char		zekKeyName2[128];
	char		cipherData[5112];
	char		IV[64];
	int		encryptMode = 1;
	char		tmpBuf[16];
	int		lenOfCipherData = 0;
	int		lenOfPlainData = 0;
	char		plainData[5112];
	char		mode[4];
	char		bcdhexData[5112];
	int		lenOfBcdhexData;
	char		keyType1[4];
	char		keyType2[4];
	char		dst_IV[128+1];
	char		algorithmID_buf[2+1];

	TUnionSymmetricKeyDB		zekKeyDB1;
	PUnionSymmetricKeyValue         zekKeyValue1 = NULL;
	TUnionSymmetricKeyDB            zekKeyDB2;
	PUnionSymmetricKeyValue         zekKeyValue2 = NULL;


	// ԴZEK��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/zekKeyName1",zekKeyName1,sizeof(zekKeyName1))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE163:: UnionReadRequestXMLPackageValue[%s]!\n","body/zekKeyName1");
		return(ret);
	}
	else if(0 == ret)
	{
		UnionUserErrLog("in UnionDealServiceCodeE163:: UnionReadRequestXMLPackageValue[%s]!\n","body/zekKeyName1");
		UnionSetResponseRemark("zekԴ��Կ������");
		return(errCodeParameter);
	}
	zekKeyName1[ret] = 0;

	// ��ȡ��Կ��Ϣ
	if ((ret =  UnionReadSymmetricKeyDBRec(zekKeyName1,1,&zekKeyDB1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE163:: UnionReadSymmetricKeyDBRec zekKeyName1[%s]!\n",zekKeyName1);
		return(ret);
	}
	// ��ȡ��Կֵ
	if ((zekKeyValue1 = UnionGetSymmetricKeyValue(&zekKeyDB1,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE163:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(errCodeParameter);
	}

	if (zekKeyDB1.keyType != conZEK)
	{
		UnionUserErrLog("in UnionDealServiceCodeE163:: is not zek [%s]!\n",zekKeyName1);
		return(errCodeEsscMDL_WrongUsageOfKey);
	}

	// Ŀ��ZEK��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/zekKeyName2",zekKeyName2,sizeof(zekKeyName2))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE163:: UnionReadRequestXMLPackageValue[%s]!\n","body/zekKeyName2");
		return(ret);
	}
	else if(0 == ret)
	{
		UnionUserErrLog("in UnionDealServiceCodeE163:: UnionReadRequestXMLPackageValue[%s]!\n","body/zekKeyName2");
		UnionSetResponseRemark("zekԴ��Կ������");
		return(errCodeParameter);
	}
	zekKeyName2[ret] = 0;

	// ��ȡ��Կ��Ϣ
	if ((ret =  UnionReadSymmetricKeyDBRec(zekKeyName2,1,&zekKeyDB2)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE163:: UnionReadSymmetricKeyDBRec zekKeyName2[%s]!\n",zekKeyName2);
		return(ret);
	}
	// ��ȡ��Կֵ
	if ((zekKeyValue2 = UnionGetSymmetricKeyValue(&zekKeyDB2,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE163:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(errCodeParameter);
	}

	if (zekKeyDB2.keyType != conZEK)
	{
		UnionUserErrLog("in UnionDealServiceCodeE163:: is not zek [%s]!\n",zekKeyName2);
		return(errCodeEsscMDL_WrongUsageOfKey);
	}

	//��ȡ��������
	if((lenOfCipherData = UnionReadRequestXMLPackageValue("body/cipherData", cipherData, sizeof(cipherData))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE163:: UnionReadRequestXMLPackageValue[%s]!\n","body/cipherData");
		return(lenOfCipherData);
	}
	else if(0 == lenOfCipherData)
	{
		UnionUserErrLog("in UnionDealServiceCodeE163:: UnionReadRequestXMLPackageValue[%s]!\n","body/cipherData");
		UnionSetResponseRemark("�������ݲ�����");
		return(errCodeParameter);
	}
	cipherData[lenOfCipherData] = 0;
	if((ret = UnionIsUppercaseHEX(cipherData)) == -1)
	{
		UnionUserErrLog("in UnionDealServiceCodeE163:: UnionIsUppercaseHEX\n");
		UnionSetResponseRemark("�������ݲ��Ǵ�дʮ������");
		return(errCodeParameter);
	}
	if (lenOfCipherData % 16)
	{
		UnionUserErrLog("in UnionDealServiceCodeE163:: lenOfCipherData[%d]\n", lenOfCipherData);
		UnionSetResponseRemark("��������Ϊʮ�����ƣ����ȱ���Ϊ16/32��������");
		return(errCodeParameter);
	}

	//��ȡ�㷨��ʶ
	if((ret = UnionReadRequestXMLPackageValue("body/algorithmID", tmpBuf, sizeof(tmpBuf))) <= 0)
	{
		algorithmID = 0;
		encryptMode = 1;
	}
	else
	{
		tmpBuf[ret] = 0;
		algorithmID = atoi(tmpBuf);
	}
	if((0 != algorithmID) && (1 != algorithmID) && (2 != algorithmID) && (3 != algorithmID))
	{
		UnionUserErrLog("in UnionDealServiceCodeE163:: wrong algorithmID[%d]\n", algorithmID);
		UnionSetResponseRemark("�㷨��ʶ��");
		return(errCodeParameter);
	}

	if(0 != algorithmID)
	{
		encryptMode = algorithmID+1;
		if((ret = UnionReadRequestXMLPackageValue("body/IV", IV, sizeof(IV))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE163:: UnionSetResponseXMLPackageValue[%s]\n","body/IV");
			return(ret);
		}
		else if(0 == ret)
		{
			UnionUserErrLog("in UnionDealServiceCodeE163:: UnionSetResponseXMLPackageValue[%s]\n","body/IV");
			UnionSetResponseRemark("��ʼ����IV������");
			return(errCodeParameter);
		}
		IV[ret] = 0;
	}

	// add by liwj 20150723
	if ((zekKeyDB1.algorithmID == conSymmetricAlgorithmIDOfSM4) && zekKeyDB2.algorithmID == conSymmetricAlgorithmIDOfSM4)
	{
		if (algorithmID == 0)
		{
			snprintf(mode, sizeof(mode), "%s", "01");
		}
		else
		{
			snprintf(mode, sizeof(mode), "%s", "02");
		}

		aschex_to_bcdhex(cipherData, lenOfCipherData, bcdhexData);
		lenOfBcdhexData = lenOfCipherData / 2;

		if ((ret = UnionTranslateHsmKeyTypeString(zekKeyDB1.keyType, keyType1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE163:: UnionTranslateHsmKeyTypeString keyType1!\n");
			return(ret);
		}
		keyType1[ret] = 0;
		if ((ret = UnionTranslateHsmKeyTypeString(zekKeyDB2.keyType, keyType2)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE163:: UnionTranslateHsmKeyTypeString keyType2!\n");
			return(ret);
		}
		keyType2[ret] = 0;

		// zmkKey1 ����
		if ((lenOfPlainData = UnionHsmCmdWA(1, mode, keyType1, 1, zekKeyValue1->keyValue, zekKeyDB1.checkValue, IV, lenOfBcdhexData, 
						(unsigned char *)bcdhexData, plainData)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE163:: UnionHsmCmdWA encrypt!\n");
			return(lenOfPlainData);
		}
		// zmkKey2 ����
		if ((lenOfBcdhexData = UnionHsmCmdWA(2, mode, keyType2, 1, zekKeyValue2->keyValue, zekKeyDB2.checkValue, IV, lenOfPlainData, 
						(unsigned char *)plainData, bcdhexData)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE163:: UnionHsmCmdWA decrypt!\n");
			return(lenOfBcdhexData);
		}

		bcdhex_to_aschex(bcdhexData, lenOfBcdhexData, cipherData);
		cipherData[lenOfBcdhexData * 2] = 0;

		//�����ؼ��ܺ����������cipherData
		if ((ret = UnionSetResponseXMLPackageValue("body/data", cipherData)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE163:: UnionSetResponseXMLPackageValue!\n");
			return(ret);
		}
		return(0);
	}
	// end

	// add by lusj 20151030  �ൺ����֧�ֹ��ʹ����㷨��ת
	if (((zekKeyDB1.algorithmID ==conSymmetricAlgorithmIDOfDES ) && (zekKeyDB2.algorithmID == conSymmetricAlgorithmIDOfSM4))|| ((zekKeyDB1.algorithmID ==conSymmetricAlgorithmIDOfSM4 ) && (zekKeyDB2.algorithmID == conSymmetricAlgorithmIDOfDES)))
	{
	
		if(0 != algorithmID)
		{
			if((ret = UnionReadRequestXMLPackageValue("body/dst_IV", dst_IV, sizeof(dst_IV))) < 0)
			{
					UnionUserErrLog("in UnionDealServiceCodeE163:: UnionSetResponseXMLPackageValue[%s]\n","body/dst_IV");
					return(ret);
			}
			else if(0 == ret)
			{
				UnionUserErrLog("in UnionDealServiceCodeE163:: UnionSetResponseXMLPackageValue[%s]\n","body/dst_IV");
				UnionSetResponseRemark("��ʼ����dst_IV������");
				return(errCodeParameter);
			}
				dst_IV[ret]=0;
		}	

		//�㷨ģʽ����
		switch(algorithmID)
		{
			case 0: 		//	"00"= ECB
				memcpy(algorithmID_buf, "01", 2);
				algorithmID_buf[2]= 0;
				break;
			case 1: 	//	"10"= CBC
				memcpy(algorithmID_buf, "02", 2);
				algorithmID_buf[2]= 0;
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE163:: algorithmID	err! algorithmID=[%d]!\n", algorithmID);
				break;
		}

		//DES->SM4
		if((zekKeyDB1.algorithmID ==conSymmetricAlgorithmIDOfDES ) && (zekKeyDB2.algorithmID == conSymmetricAlgorithmIDOfSM4))
		{
	
				if ((ret = UnionTranslateHsmKeyTypeString(zekKeyDB2.keyType, keyType2)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE163:: UnionTranslateHsmKeyTypeString keyType2!\n");
					return(ret);
				}
				keyType2[ret] = 0;
	
				//����E0ָ���������
				aschex_to_bcdhex(cipherData, lenOfCipherData, bcdhexData);
				lenOfBcdhexData = lenOfCipherData / 2;
				bcdhexData[lenOfBcdhexData]=0;
	
				if((lenOfPlainData = UnionHsmCmdE0(1, 0, encryptMode, zekKeyDB1.keyLen, zekKeyValue1->keyValue,0, IV, lenOfBcdhexData,(unsigned char*)bcdhexData, (unsigned char*)plainData, &lenOfPlainData, sizeof(plainData))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE163:: UnionHsmCmdE0 err\n");
					return(ret);
				}
	
	
				// Key2  WA����
				if ((lenOfBcdhexData = UnionHsmCmdWA(2, algorithmID_buf, keyType2, 1, zekKeyValue2->keyValue, zekKeyDB2.checkValue, dst_IV, lenOfPlainData, 
								(unsigned char *)plainData, bcdhexData)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE163:: UnionHsmCmdWA decrypt!\n");
					return(lenOfBcdhexData);
				}
	
		}
		else if((zekKeyDB2.algorithmID ==conSymmetricAlgorithmIDOfDES ) && (zekKeyDB1.algorithmID == conSymmetricAlgorithmIDOfSM4))
		{	//SM4->DES
			if ((ret = UnionTranslateHsmKeyTypeString(zekKeyDB1.keyType, keyType1)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE163:: UnionTranslateHsmKeyTypeString keyType1!\n");
				return(ret);
			}
			keyType1[ret] = 0;
		
			aschex_to_bcdhex(cipherData, lenOfCipherData, bcdhexData);
			lenOfBcdhexData = lenOfCipherData / 2;
			bcdhexData[lenOfBcdhexData]=0;
		
			// Key1 ����
			if ((lenOfPlainData = UnionHsmCmdWA(1, algorithmID_buf, keyType1, 1, zekKeyValue1->keyValue, zekKeyDB1.checkValue, IV, lenOfBcdhexData, 
										(unsigned char *)bcdhexData, plainData)) < 0)
			{
					UnionUserErrLog("in UnionDealServiceCodeE163:: UnionHsmCmdWA encrypt!\n");
					return(lenOfPlainData);
			}
		
			//����E0���¼���
			if((lenOfBcdhexData = UnionHsmCmdE0(0, 0, encryptMode, zekKeyDB2.keyLen, zekKeyValue2->keyValue, 0, dst_IV, lenOfPlainData, (unsigned char*)plainData, (unsigned char*)bcdhexData, &lenOfBcdhexData, sizeof(bcdhexData))) < 0)
			{
					UnionUserErrLog("in UnionDealServiceCodeE163:: UnionHsmCmdE0 err\n");
					return(ret);
			}
		}

		bcdhex_to_aschex(bcdhexData, lenOfBcdhexData, cipherData);
		cipherData[lenOfBcdhexData * 2] = 0;
		
		//�����ؼ��ܺ����������data
		if ((ret = UnionSetResponseXMLPackageValue("body/data", cipherData)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE163:: UnionSetResponseXMLPackageValue!\n");
			return(ret);
		}
		return(0);

	}
	// end

	//����E0ָ���������
	if((ret = UnionHsmCmdE0(1, 0, encryptMode, zekKeyDB1.keyLen, zekKeyValue1->keyValue, 1, IV, lenOfCipherData,(unsigned char*)cipherData, (unsigned char*)plainData, &lenOfPlainData, sizeof(plainData))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE163:: UnionHsmCmdE0 err\n");
		return(ret);
	}

	//����E0���¼���
	if((ret = UnionHsmCmdE0(0, 0, encryptMode, zekKeyDB2.keyLen, zekKeyValue2->keyValue, 1, IV, lenOfPlainData, (unsigned char*)plainData, (unsigned char*)cipherData, &lenOfCipherData, sizeof(cipherData))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE163:: UnionHsmCmdE0 err\n");
		return(ret);
	}

	//�����ؼ��ܺ����������cipherData
	if ((ret = UnionSetResponseXMLPackageValue("body/data", cipherData)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE163:: UnionSetResponseXMLPackageValue!\n");
		return(ret);
	}

	return(0);
}
