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

int UnionDealServiceCodeE210(PUnionHsmGroupRec phsmGroupRec)
{
	int 	ret;
	char	keyName[136];
	char	pinByPK[512];
	char	zpkName[136];
	char	mode[16];
	char	format[8];
	char	accNo[40];	
	char	pinByZPK[136];
	int	lenOfPinByPK = 0;
	int 	algorithmID = 0;
	int	lenOfAccNo = 0;
	char	vkIndex[16];
	
	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue 	psymmetricKeyValue;
	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	
	//��ȡSM2��Կ����
	if((ret = UnionReadRequestXMLPackageValue("body/keyName", keyName, sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE210:: UnionReadRequestXMLPackageValue[%s]\n", "body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);
	
	//��ȡ�ǶԳ���Կ��Ϣ
	memset(&asymmetricKeyDB, 0, sizeof(asymmetricKeyDB));
	if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,1,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE210:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",keyName);
		return(ret);
	}
	
	// ���˽Կ����
        if (asymmetricKeyDB.keyType != conAsymmetricKeyTypeOfEncryption && asymmetricKeyDB.keyType != conAsymmetricKeyTypeOfSignatureAndEncryption)
        {
                UnionUserErrLog("in UnionDealServiceCodeE210:: keyType[%d] is not 1 or 2!\n",asymmetricKeyDB.keyType);
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
		UnionUserErrLog("in UnionDealServiceCodeE210:: UnionReadRequestXMLPackageValue[%s]\n","body/pinByPK");
		return(lenOfPinByPK);
	}
	pinByPK[lenOfPinByPK] = 0;
	
	//��ȡzpk��Կ����
	if((ret = UnionReadRequestXMLPackageValue("body/zpkName", zpkName, sizeof(zpkName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE210:: UnionReadRequestXMLPackageValue[%s]\n", "body/zpkName");
		return(ret);
	}
	zpkName[ret] = 0;
	UnionFilterHeadAndTailBlank(zpkName);

	//��ȡ�Գ���Կ��Ϣ
	memset(&symmetricKeyDB, 0, sizeof(symmetricKeyDB));
	if((ret = UnionReadSymmetricKeyDBRec(zpkName, 1, &symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE210:: UnionReadAsymmetricKeyDBRec zpkName[%s]\n",zpkName);
		return(ret);
	}
	
	if(conZPK != symmetricKeyDB.keyType)
	{	
		UnionUserErrLog("in UnionDealServiceCodeE210:: keyType err\n");
		UnionSetResponseRemark("��Կ���ʹ�,��Ϊzpk��Կ");
		return(errCodeHsmCmdMDL_InvalidKeyType);
	}
	
	if(conSymmetricAlgorithmIDOfSM4 == symmetricKeyDB.algorithmID) // ����
		algorithmID = 1;
	else if(conSymmetricAlgorithmIDOfDES == symmetricKeyDB.algorithmID) // ����
		algorithmID = 2;
	else
	{
		UnionUserErrLog("in UnionDealServiceCodeE210L:: algorithmID[%d]\n",symmetricKeyDB.algorithmID);
		UnionSetResponseRemark("zpk��Կ�㷨��ʶ��[������DES��SM4��Կ]");
		return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	
	//��ȡ�Գ���Կֵ
	if((psymmetricKeyValue= UnionGetSymmetricKeyValue(&symmetricKeyDB, phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE210:: UnionGetSymmetricKeyValue err\n");
		UnionSetResponseRemark("��ȡ�Գ���Կֵʧ��");
		return(errCodeParameter);
	}
	
	//��ȡģʽ
	if((ret = UnionReadRequestXMLPackageValue("body/mode", mode, sizeof(mode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE210:: UnionReadRequestXMLPackageValue[%s]\n", "body/mode");
		return(ret);
	}
	mode[ret] = 0;
	if (mode[0] != '1' && mode[0] != '2')
	{
		UnionUserErrLog("in UnionDealServiceCodeE210:: mode[%s] not in [1,2]\n",mode);
		return(errCodeParameter);
	}
	
	if('1' == mode[0])
	{
		//��ȡPIN��ʽ
		if((ret = UnionReadRequestXMLPackageValue("body/format",format,sizeof(format))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE210:: UnionReadRequestXMLPackageValue[%s]\n", "body/format");
			return(ret);
		}
		format[ret] = 0;
	
		//��ȡ�˺�
		if((lenOfAccNo = UnionReadRequestXMLPackageValue("body/accNo", accNo, sizeof(accNo))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE210:: UnionReadRequestXMLPackageValue[%s]\n", "body/accNo");		
			return(lenOfAccNo);
		}
		accNo[lenOfAccNo] = 0;
	}
	
	switch(asymmetricKeyDB.algorithmID)
	{
		case	conAsymmetricAlgorithmIDOfSM2:
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					if('1' == mode[0]) // pinת����
					{
						if((ret = unionHsmCmdKK(vkIndex, strlen(asymmetricKeyDB.vkValue), asymmetricKeyDB.vkValue, lenOfPinByPK, pinByPK, algorithmID, 1, format,lenOfAccNo, accNo, psymmetricKeyValue->keyValue, NULL, pinByZPK,sizeof(pinByZPK))) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE210:: UnionHsmCmdKK \n");
							return(ret);
						}
						
					}
					else if('2' == mode[0])	// ��������ת����
					{
						if((ret = UnionHsmCmdKL(vkIndex, strlen(asymmetricKeyDB.vkValue), asymmetricKeyDB.vkValue, lenOfPinByPK, pinByPK, algorithmID, 1, psymmetricKeyValue->keyValue, 0, 0, NULL, pinByZPK,sizeof(pinByZPK), NULL, 0)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE210:: UnionHsmCmdKL \n");
							return(ret);
						}
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE210:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
					return(errCodeParameter);
			}	
			break;
		case	conAsymmetricAlgorithmIDOfRSA:
		default:
			UnionUserErrLog("in UnionDealServiceCodeE210:: keyName [%s] algorithmID = [%d] illegal!\n", asymmetricKeyDB.keyName, asymmetricKeyDB.algorithmID);
                        return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	
	if((ret = UnionSetResponseXMLPackageValue("body/pinByZPK", pinByZPK)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE210:: UnionSetResponseXMLPackageValue[%s]\n", "body/pinByZPK");
		return(ret);
	}
	
	return(0);
}
