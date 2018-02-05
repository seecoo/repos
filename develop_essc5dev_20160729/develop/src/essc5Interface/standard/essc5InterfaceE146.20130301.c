//	Author:		chenqy
//	Copyright:	Union Tech. Guangzhou
//	Date:		2015-08-03

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symmetricKeyDB.h"
#include "asymmetricKeyDB.h"
#include "essc5Interface.h"
#include "unionHsmCmd.h"
#include "unionXMLPackage.h"
#include "unionErrCode.h"
#include "unionRealBaseDB.h"
#include "unionHsmCmdVersion.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "UnionDes.h"

/***************************************
�������:	E146
������:		��ZPK��DES��SM4�����ܵ�pinת��SM2��Կ����
��������:	��ZPK��DES��SM4�����ܵ�pinת��SM2��Կ����
***************************************/

int UnionDealServiceCodeE146(PUnionHsmGroupRec phsmGroupRec)
{
	int 	ret;
	char 	keyName[136];
	int 	algorithmID = 0;
	int 	keyType = 0;
	char 	format[8];
	char 	accNo[40];
	int		lenOfAccNo = 0;
	char 	pinBlock[136];
	char 	SM2KeyName[136];
	char 	SM2Index[16];
	char 	pinByPK[512];
	
	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue;
	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	
	// �����ж�ȡZPK��TPK��Կ����
	if (0 > (ret = UnionReadRequestXMLPackageValue("body/keyName", keyName, sizeof(keyName)))) 
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: UnionReadRequestXMLPackageValue[%s]\n", "body/keyName");
		return ret;
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);
	
	// �Գ���Կ��Ϣ
	memset(&symmetricKeyDB, 0, sizeof(symmetricKeyDB));
	if (0 > (ret = UnionReadSymmetricKeyDBRec(keyName, 1, &symmetricKeyDB)))
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: UnionReadSymmetricKeyDBRec keyName[%s]\n", keyName);
		return ret;
	}
	
	// �Գ���Կ�㷨��ʶ
	if(conSymmetricAlgorithmIDOfSM4 == symmetricKeyDB.algorithmID) // ����
		algorithmID = 1;
	else if(conSymmetricAlgorithmIDOfDES == symmetricKeyDB.algorithmID) // ����
		algorithmID = 2;
	else
	{
		UnionUserErrLog("in UnionDealServiceCode146:: algorithmID[%d]\n",symmetricKeyDB.algorithmID);
		UnionSetResponseRemark("zpk��Կ�㷨��ʶ��[������DES��SM4��Կ]");
		return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	
	// �Գ���Կ����
	if (conZPK == symmetricKeyDB.keyType)
		keyType = 1;
	else if (conTPK == symmetricKeyDB.keyType)
		keyType = 2;
	else
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: keyType err\n");
		UnionSetResponseRemark("��Կ���ʹ�,��ΪZPK��TPK��Կ");
		return(errCodeHsmCmdMDL_InvalidKeyType);
	}
	
	// �Գ���Կֵ
	if (NULL == (psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB, phsmGroupRec->lmkProtectMode)))
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: UnionGetSymmetricKeyValue err\n");
		UnionSetResponseRemark("��ȡ�Գ���Կֵʧ��");
		return(errCodeParameter);
	}
	
	// �����ж�ȡPIN��ʽ
	if (0 > (ret = UnionReadRequestXMLPackageValue("body/format", format, sizeof(format))))
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: UnionReadRequestXMLPackageValue[%s]\n", "body/format");
		return ret;
	}
	format[ret] = 0;
	
	// �����ж�ȡ�˺�
	if (0 > (lenOfAccNo = UnionReadRequestXMLPackageValue("body/accNo", accNo, sizeof(accNo))))
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: UnionReadRequestXMLPackageValue[%s]\n", "body/accNo");		
		return lenOfAccNo;
	}
	accNo[lenOfAccNo] = 0;
	
	// �����ж�ȡ�Գ���Կ���ܵ�pin��
	if (0 > (ret = UnionReadRequestXMLPackageValue("body/pinBlock", pinBlock, sizeof(pinBlock))))
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: UnionReadRequestXMLPackageValue[%s]\n","body/pinBlock");
		return ret;
	}
	pinBlock[ret] = 0;
			
	// �����ж�ȡSM2��Կ����
	if(0 > (ret = UnionReadRequestXMLPackageValue("body/SM2KeyName", SM2KeyName, sizeof(SM2KeyName))))
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: UnionReadRequestXMLPackageValue[%s]\n", "body/SM2KeyName");
		return(ret);
	}
	SM2KeyName[ret] = 0;
	UnionFilterHeadAndTailBlank(SM2KeyName);
	
	// SM2��Կ��Կ��Ϣ
	memset(&asymmetricKeyDB, 0, sizeof(asymmetricKeyDB));
	if ((ret =  UnionReadAsymmetricKeyDBRec(SM2KeyName,1,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: UnionReadAsymmetricKeyDBRec SM2KeyName[%s]!\n",SM2KeyName);
		return(ret);
	}
	
	// SM2��Կ����
	if (asymmetricKeyDB.vkStoreLocation != 1)
		snprintf(SM2Index, sizeof(SM2Index), "99");
	else
	{
		//snprintf(SM2Index, sizeof(SM2Index), "%s", asymmetricKeyDB.vkIndex);
		UnionUserErrLog("in UnionDealServiceCodeE146:: asymmetricKeyDB.vkIndex[%d]\n", atoi(asymmetricKeyDB.vkIndex));
		UnionSetResponseRemark("ֻ��ʹ�ô������Կ�����SM2��Կ");
		return(errCodeParameter);
	}
	
	// �����������ָ��
	memset(pinByPK, 0, sizeof(pinByPK));
	switch (phsmGroupRec->hsmCmdVersionID)
	{
		  case	conHsmCmdVerRacalStandardHsmCmd:
		  case	conHsmCmdVerSJL06StandardHsmCmd:
		  		if (0 > (ret = UnionHsmCmdKN(algorithmID, atoi(SM2Index), strlen(asymmetricKeyDB.vkValue), asymmetricKeyDB.vkValue, 
					keyType, NULL, psymmetricKeyValue->keyValue, format, lenOfAccNo, accNo, pinBlock, pinByPK, sizeof(pinByPK))))
				{
					UnionUserErrLog("in UnionDealServiceCodeE146:: UnionHsmCmdKN, ret = [%d]\n", ret);
					return ret;
				}
				break;
		  default:
				UnionUserErrLog("in UnionDealServiceCodeE146:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
				UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
				return errCodeParameter;
	}
	
	// ���ñ��ķ���
	if (0 > (ret = UnionSetResponseXMLPackageValue("body/pinByPK", pinByPK)))
	{
		UnionUserErrLog("in UnionDealServiceCodeE146:: UnionSetResponseXMLPackageValue[%s]\n", "body/pinByPK");
		return ret;
	}
	
	return 0;
}

