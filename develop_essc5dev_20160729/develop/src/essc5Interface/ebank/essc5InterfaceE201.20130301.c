#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif  


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionHsmCmd.h"
#include "unionXMLPackage.h"
#include "symmetricKeyDB.h"
#include "UnionLog.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "essc5Interface.h"
#include "UnionDes.h"
#include "unionRealBaseDB.h"
#include "unionHsmCmdVersion.h"
#include "asymmetricKeyDB.h"
#include "commWithHsmSvr.h"
#include "asymmetricKeyDB.h"



/***************************************
  �������:	E201
  ������:  	E201 ����PINת��(PK->ZPK)	
  ��������:	 E201 ����PINת��(PK->ZPK)
  �жϹ�Կ�㷨��ʶ�������Ӧ���㷨ת��  
 ***************************************/
int UnionDealServicePINBYSM2PKTOZPK(PUnionHsmGroupRec phsmGroupRec);

int UnionDealServicePINBYRSAPKTOZPK(PUnionHsmGroupRec phsmGroupRec);


int UnionDealServiceCodeE201(PUnionHsmGroupRec phsmGroupRec)
{										
	int 	ret;
	char	keyNameofpk[136];


	TUnionAsymmetricKeyDB		asymmetricKeyDB;


	//��ȡ��Կ����
	if((ret = UnionReadRequestXMLPackageValue("body/pkKeyName", keyNameofpk, sizeof(keyNameofpk))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE201:: UnionReadRequestXMLPackageValue[%s]\n", "body/keyName");
		return(ret);
	}
	keyNameofpk[ret] = 0;
	UnionFilterHeadAndTailBlank(keyNameofpk);

	//��ȡ�ǶԳ���Կ��Ϣ
	memset(&asymmetricKeyDB, 0, sizeof(asymmetricKeyDB));
	if ((ret =  UnionReadAsymmetricKeyDBRec(keyNameofpk,1,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE201:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",keyNameofpk);
		return(ret);
	}

	//���й�Կ�㷨��ǩ���͵��жϣ������в�ͬ��ת���ܲ���
	switch (asymmetricKeyDB.algorithmID)
	{
		case conAsymmetricAlgorithmIDOfSM2:	

			if((ret = UnionDealServicePINBYSM2PKTOZPK(phsmGroupRec))<0)
			{
				UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: error!\n");	
				return(ret);
			}
			break;

		case conAsymmetricAlgorithmIDOfRSA:

			if((ret = UnionDealServicePINBYRSAPKTOZPK(phsmGroupRec))<0)
			{
				UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: error!\n");
				return(ret);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE201:: keyName [%s] algorithmID = [%d] illegal!\n", asymmetricKeyDB.keyName, asymmetricKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	return 0;
}


int UnionDealServicePINBYRSAPKTOZPK(PUnionHsmGroupRec phsmGroupRec)
{

	int		ret;
	int		lenOfPinByPK;
	char	zpkName[136];
	char	pkName[136];

	TUnionSymmetricKeyDB	tZpk;
	TUnionAsymmetricKeyDB 	tPk;

	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;

	char	accNo[32+1];			//	�˺�
	char	pinByPK[512+1];			//	��Կ���ܵ�PIN			
	char 	pinByPKBcd[512+1];
	char 	formatTmp[2+1];			
	char	pinByZPK[32+1];			//	ZPK���ܵ�PIN

	char			vkIndex[32];
	char			vkValue[4096+1];	
	int 			lenOfVK=0;

	memset(&tZpk,0,sizeof(tZpk));
	memset(&tPk,0,sizeof(tPk));

	if ((ret = UnionReadRequestXMLPackageValue("body/pkKeyName", pkName, sizeof(pkName))) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkKeyName");
		return(ret);
	}
	pkName[ret] = 0;

	if ((ret = UnionReadRequestXMLPackageValue("body/zpkKeyName", zpkName, sizeof(zpkName))) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: UnionReadRequestXMLPackageValue[%s]!\n","body/zpkKeyName");
		return(ret);
	}
	zpkName[ret] = 0;

	if ((ret = UnionReadRequestXMLPackageValue("body/accNo", accNo, sizeof(accNo)))< 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
		return(ret);
	}

	// ����˺�
	accNo[ret] = 0;
	UnionFilterHeadAndTailBlank(accNo);
	if (strlen(accNo) == 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: accNo can not be null!\n");
		UnionSetResponseRemark("�˺Ų���Ϊ��!");
		return(errCodeParameter);
	}		
	if (!UnionIsDigitString(accNo))
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: accNo[%s] is error!\n",accNo);
		UnionSetResponseRemark("�˺ŷǷ�[%s],����Ϊ����",accNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}

	if ((ret = UnionReadRequestXMLPackageValue("body/format", formatTmp, sizeof(formatTmp)))< 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: UnionReadRequestXMLPackageValue[%s]!\n","body/format");
		return(ret);
	}
	else
	{
		formatTmp[ret] = 0;
		if (formatTmp[0] != '0' && formatTmp[0] != '1')
		{
			UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: format[%s] error!\n",formatTmp);
			return(errCodeParameter);
		}
	}

	if ((lenOfPinByPK = UnionReadRequestXMLPackageValue("body/pinByPK", pinByPK, sizeof(pinByPK)))< 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinByPK");
		return(lenOfPinByPK);
	}
	else
	{
		//��⹫Կ���ܵ�PIN
		pinByPK[lenOfPinByPK] = 0;
		if (lenOfPinByPK == 0)
		{
			UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: pinByPK can not be null!\n");
			UnionSetResponseRemark("��Կ���ܵ�PIN����Ϊ��!");
			return(errCodeParameter);
		}
		if (!UnionIsBCDStr(pinByPK))
		{
			UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: pinByPK[%s] is error!\n",pinByPK);
			UnionSetResponseRemark("��Կ���ܵ�PIN�Ƿ�,����Ϊʮ��������");
			return(errCodeParameter);
		}
	}

	aschex_to_bcdhex(pinByPK, lenOfPinByPK, pinByPKBcd);
	pinByPKBcd[lenOfPinByPK/2] = 0;

	// ��ȡ�Գ���Կ
	if ((ret = UnionReadSymmetricKeyDBRec(zpkName, 1, &tZpk)) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", zpkName);
		return(ret);
	}

	// ��ȡ�Գ���Կֵ
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&tZpk,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(errCodeParameter);
	}

	//��ȡ�ǶԳ���Կ
	if ((ret = UnionReadAsymmetricKeyDBRec(pkName, 1, &tPk)) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", pkName);
		return(ret);
	}

	//add  begin by lusj 20160125 ������˽Կ����
	// ����Ƿ��������
	if (tPk.vkStoreLocation != 0)
	{
		sprintf(vkIndex,"%02d",atoi(tPk.vkIndex));
		vkIndex[2] = 0;
	}
	else
	{
		ret=sprintf(vkIndex,"%s","99");
		vkIndex[ret]=0;

		lenOfVK = strlen(tPk.vkValue);
		memcpy(vkValue,tPk.vkValue,lenOfVK);
		vkValue[lenOfVK] = 0;
	}
	//add end by lusj 20160125

	switch(tZpk.algorithmID)
	{
		case	conSymmetricAlgorithmIDOfDES:   // DES�㷨��
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					//modify by lusj 20160125 �޸����ݣ�������˽Կ���ܣ�������ǰ����
					if ((ret= UnionHsmCmd43(vkIndex,lenOfVK,vkValue, '2', psymmetricKeyValue->keyValue, '1' , accNo, (unsigned char*)pinByPKBcd, lenOfPinByPK/2, pinByZPK)) < 0)
					{
						UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK: UnionHsmCmd43 return[%d]!\n", ret);
						return(ret);
					}
					//modify end by lusj 20160125
					if (ret == 0)
					{
						UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK: UnionHsmCmd43 return[%d]!\n", ret);
						return(errCodeHsmCmdMDL_ReturnLen);
					}
					pinByZPK[ret] = 0;
					break;
				default:
					UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
					return(errCodeParameter);
			}	
			break;
		case	conSymmetricAlgorithmIDOfSM4:   // SM4�㷨
		default:
			UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK:: key [%s] algorithmID = [%d] illegal!\n", tZpk.keyName, tZpk.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	//���� pinByZPK
	if ((ret = UnionSetResponseXMLPackageValue("body/pinByZPK", pinByZPK)) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYRSAPKTOZPK: UnionSetResponseXMLPackageValue [%s]!\n", pinByZPK);
		return(ret);
	}
	return 0;
}

int UnionDealServicePINBYSM2PKTOZPK(PUnionHsmGroupRec phsmGroupRec)
{

	int 	ret;
	int 	keyType=0;
	char	keyName[136];
	char	pinByPK[512];
	char	zpkName[136];
	char	format[8];
	char	accNo[40];	
	char	pinByZPK[136];
	int	lenOfPinByPK = 0;
	int 	algorithmID = 0;
	int	lenOfAccNo = 0;
	char	vkIndex[16];
	char				specialAlg[8];

	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue 	psymmetricKeyValue;
	TUnionAsymmetricKeyDB		asymmetricKeyDB;

	//��ȡSM2��Կ����
	if((ret = UnionReadRequestXMLPackageValue("body/pkKeyName", keyName, sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionReadRequestXMLPackageValue[%s]\n", "body/pkKeyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);

	//��ȡ�ǶԳ���Կ��Ϣ
	memset(&asymmetricKeyDB, 0, sizeof(asymmetricKeyDB));
	if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,1,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",keyName);
		return(ret);
	}

	// ���˽Կ����
	if (asymmetricKeyDB.keyType != conAsymmetricKeyTypeOfEncryption && asymmetricKeyDB.keyType != conAsymmetricKeyTypeOfSignatureAndEncryption)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: keyType[%d] is not 1 or 2!\n",asymmetricKeyDB.keyType);
		UnionSetResponseRemark("˽Կ����[%d]��֧�ּ���,����Ϊ[1��2]");
		return(errCodeHsmCmdMDL_InvalidKeyType);
	}

	if (asymmetricKeyDB.vkStoreLocation != 1 && asymmetricKeyDB.vkStoreLocation != 2)
		snprintf(vkIndex,sizeof(vkIndex),"99");
	else
		snprintf(vkIndex,sizeof(vkIndex),"%s",asymmetricKeyDB.vkIndex);

	//��ȡ���ܵ�����
	if((lenOfPinByPK = UnionReadRequestXMLPackageValue("body/pinByPK", pinByPK, sizeof(pinByPK))) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionReadRequestXMLPackageValue[%s]\n","body/pinByPK");
		return(lenOfPinByPK);
	}
	pinByPK[lenOfPinByPK] = 0;

	//��ȡzpk��Կ����
	if((ret = UnionReadRequestXMLPackageValue("body/zpkKeyName", zpkName, sizeof(zpkName))) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionReadRequestXMLPackageValue[%s]\n", "body/zpkKeyName");
		return(ret);
	}
	zpkName[ret] = 0;
	UnionFilterHeadAndTailBlank(zpkName);

	//��ȡ�Գ���Կ��Ϣ
	memset(&symmetricKeyDB, 0, sizeof(symmetricKeyDB));
	if((ret = UnionReadSymmetricKeyDBRec(zpkName, 1, &symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionReadAsymmetricKeyDBRec zpkName[%s]\n",zpkName);
		return(ret);
	}

	//modify by lusj 2016118 ԭ�汾ֻ֧��ZPK���ͣ������TPK����
	if(conZPK == symmetricKeyDB.keyType)
	{	
		keyType=1;
	}
	else if (conTPK == symmetricKeyDB.keyType)
	{
		keyType=2;
	}
	else
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: keyType err\n");
		UnionSetResponseRemark("��Կ���ʹ�,��Ϊzpk/tpk��Կ");
		return(errCodeHsmCmdMDL_InvalidKeyType);
	}
	//modify end by lusj 

	if(conSymmetricAlgorithmIDOfSM4 == symmetricKeyDB.algorithmID) // ����
		algorithmID = 1;
	else if(conSymmetricAlgorithmIDOfDES == symmetricKeyDB.algorithmID) // ����
		algorithmID = 2;
	else
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: algorithmID[%d]\n",symmetricKeyDB.algorithmID);
		UnionSetResponseRemark("zpk��Կ�㷨��ʶ��[������DES��SM4��Կ]");
		return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	//��ȡ�Գ���Կֵ
	if((psymmetricKeyValue= UnionGetSymmetricKeyValue(&symmetricKeyDB, phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionGetSymmetricKeyValue err\n");
		UnionSetResponseRemark("��ȡ�Գ���Կֵʧ��");
		return(errCodeParameter);
	}

	//��ȡPIN��ʽ
	if((ret = UnionReadRequestXMLPackageValue("body/format",format,sizeof(format))) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionReadRequestXMLPackageValue[%s]\n", "body/format");
		return(ret);
	}
	format[ret] = 0;

	//��ȡ�˺�
	if((lenOfAccNo = UnionReadRequestXMLPackageValue("body/accNo", accNo, sizeof(accNo))) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionReadRequestXMLPackageValue[%s]\n", "body/accNo");	
		return(lenOfAccNo);
	}
	accNo[lenOfAccNo] = 0;

	//add by lusj 20160118   ��������ר���㷨������ͨ�ð汾
	//ר���㷨��ʾspecialAlg
	memset(specialAlg,0,sizeof(specialAlg));
	if ((ret = UnionReadRequestXMLPackageValue("body/specialAlg",specialAlg,sizeof(specialAlg))) >0)
	{
		specialAlg[ret]=0;
	}
	//add end lusj

	switch(asymmetricKeyDB.algorithmID)
	{
		case	conAsymmetricAlgorithmIDOfSM2:
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					if((ret = unionHsmCmdKK(vkIndex, strlen(asymmetricKeyDB.vkValue), asymmetricKeyDB.vkValue, lenOfPinByPK, pinByPK, algorithmID,keyType, format,lenOfAccNo, accNo, psymmetricKeyValue->keyValue,specialAlg, pinByZPK,sizeof(pinByZPK))) < 0)
					{
						UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionHsmCmdKK \n");
						return(ret);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
					return(errCodeParameter);
			}	
			break;
		case	conAsymmetricAlgorithmIDOfRSA:
		default:
			UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: keyName [%s] algorithmID = [%d] illegal!\n", asymmetricKeyDB.keyName, asymmetricKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	if((ret = UnionSetResponseXMLPackageValue("body/pinByZPK", pinByZPK)) < 0)
	{
		UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: UnionSetResponseXMLPackageValue[%s]\n", "body/pinByZPK");
		return(ret);
	}	
	return 0;
}
