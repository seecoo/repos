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
#include "commWithHsmSvr.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "symmetricKeyDB.h"
#include "asymmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "unionHsmCmdVersion.h"

/*************************************** 
�������:	E205
������:  	E205 ��PIN��X9.8תΪ�����㷨����(ZPK->ZPK)
��������:       E205 ��PIN��X9.8תΪ�����㷨����(ZPK->ZPK)
***************************************/
int UnionDealServiceCodeE205(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	keyName1[136];
	char	keyName2[136];

	char 	pinBlock[48+1]; 	//PIN����
	char	srcNo[32+1];		//ԭ�˺�
	char	dstNo[32+1];		//Ŀ���˺�
	char	newPinBlock[48+1];
	int	lenOfPin = 0;	
	char	zpk[48];
	char	checkValue[32];
	char	tmpPin[64];

	TUnionSymmetricKeyDB		srcZpk;
	TUnionSymmetricKeyDB		dstZpk;
	PUnionSymmetricKeyValue		psrcKeyValue = NULL;
	PUnionSymmetricKeyValue		pdstKeyValue = NULL;


	memset(&srcZpk,		0,	sizeof(srcZpk));
	memset(&dstZpk,		0,	sizeof(dstZpk));

	if ((ret = UnionReadRequestXMLPackageValue("body/pinBlock", pinBlock, sizeof(pinBlock)))< 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE205:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinBlock");
                return(ret);
        }
	pinBlock[ret] = 0;

	if ((ret = UnionReadRequestXMLPackageValue("body/accNo1", srcNo, sizeof(srcNo)))< 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE205:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo1");
                return(ret);
        }
	srcNo[ret] = 0;
	
	if ((ret = UnionReadRequestXMLPackageValue("body/accNo2", dstNo, sizeof(dstNo)))< 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE205:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo2");
                return(ret);
        }
	dstNo[ret] = 0;

	// ����˺�
	UnionFilterHeadAndTailBlank(srcNo);
        if (strlen(srcNo) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE205:: srcNo can not be null!\n");
                UnionSetResponseRemark("Դ�˺Ų���Ϊ��!");
                return(errCodeParameter);
        }
	if (!UnionIsDigitString(srcNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE205:: accNo1[%s] is error!\n",srcNo);
		UnionSetResponseRemark("Դ�˺ŷǷ�[%s],����Ϊ����",srcNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}

	// ����˺�
	UnionFilterHeadAndTailBlank(dstNo);
        if (strlen(dstNo) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE205:: dstNo can not be null!\n");
                UnionSetResponseRemark("Ŀ���˺Ų���Ϊ��!");
                return(errCodeParameter);
        }
	if (!UnionIsDigitString(dstNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE205:: accNo2[%s] is error!\n",dstNo);
		UnionSetResponseRemark("Ŀ���˺ŷǷ�[%s],����Ϊ����",dstNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}
	
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName1", keyName1, sizeof(keyName1))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE205:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName1");
                return(ret);
	}
	keyName1[ret] = 0;

	if ((ret = UnionReadRequestXMLPackageValue("body/keyName2", keyName2, sizeof(keyName2))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE205:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName2");
                return(ret);
	}
	keyName2[ret] = 0;

	// ��ȡ�Գ���Կ
        if ((ret =  UnionReadSymmetricKeyDBRec(keyName1, 1, &srcZpk)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE205:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", keyName1);
                return(ret);
        }

	// ��ȡ�Գ���Կֵ
	if ((psrcKeyValue = UnionGetSymmetricKeyValue(&srcZpk,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE205:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("��ȡԴ��Կֵʧ��");
		return(errCodeParameter);
	}
	
	 // ��ȡ�Գ���Կ
	if ((ret = UnionReadSymmetricKeyDBRec(keyName2, 1, &dstZpk)) < 0)
	{
                UnionUserErrLog("in UnionDealServiceCodeE205:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", keyName2);
                return(ret);
	}
	// ��ȡ�Գ���Կֵ
	if ((pdstKeyValue = UnionGetSymmetricKeyValue(&dstZpk,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE205:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("��ȡĿ����Կֵʧ��");
		return(errCodeParameter);
	}

	switch(srcZpk.algorithmID)
	{
		case	conSymmetricAlgorithmIDOfDES:
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmdN7(psrcKeyValue->keyValue, pdstKeyValue->keyValue, pinBlock, srcNo, dstNo, &lenOfPin, newPinBlock)) < 0)
					{
						if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) &&		\
							(UnionOldVersionSymmetricKeyIsUsed(&srcZpk)))
						{
							if ((ret = UnionHsmCmdN7(psrcKeyValue->oldKeyValue, pdstKeyValue->keyValue, pinBlock, srcNo, dstNo, &lenOfPin, newPinBlock)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE205: UnionHsmCmdN7 old version return[%d]!\n", ret);
								return(ret);
							}
						}
						else
						{
							UnionUserErrLog("in UnionDealServiceCodeE205: UnionHsmCmdN7 return[%d]!\n", ret);
							return(ret);
						}
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE205:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
					return(errCodeParameter);
			}
			break;
		case	conSymmetricAlgorithmIDOfSM4:
			switch(phsmGroupRec->hsmCmdVersionID)
                        {
                                case conHsmCmdVerRacalStandardHsmCmd:
                                case conHsmCmdVerSJL06StandardHsmCmd:
					//����ZPK��Կ
					if((ret = UnionHsmCmdA0(0, conZPK, con128BitsDesKey, NULL, zpk, NULL, checkValue)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE205:: UnionHsmCmdA0 return[%d]\n", ret);
						return(ret);
					}
					//SM4���ܵ�PinתΪDES����
					if((ret = UnionHsmCmdW8(2, 1, 2, NULL, psrcKeyValue->keyValue, 2, NULL, zpk, "01", "01", pinBlock, srcNo, srcNo, tmpPin, sizeof(tmpPin))) < 0)
					{
						if((ret <= errCodeOffsetOfHsmReturnCodeMDL) && UnionOldVersionSymmetricKeyIsUsed(&srcZpk))
						{
							if((ret = UnionHsmCmdW8(2, 1, 2, NULL, psrcKeyValue->oldKeyValue, 2, NULL, zpk, "01", "01", pinBlock, srcNo, srcNo, tmpPin, sizeof(tmpPin))) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE205:: UnionHsmCmdW8 old version return[%d]\n", ret);
								return(ret);
							}
						}
						else
						{
							UnionUserErrLog("in UnionDealServiceCodeE205:: UnionHsmCmdW8 return[%d]\n", ret);
							return(ret);
						}
					}
					//DES����PINת������ʽPIN
					if((ret = UnionHsmCmdN7(zpk, pdstKeyValue->keyValue, tmpPin, srcNo, dstNo, &lenOfPin, newPinBlock)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE205:: UnionHsmCmdN7 return[%d]\n", ret);
						return(ret);
					}
					break;
				default:
                                        UnionUserErrLog("in UnionDealServiceCodeE205:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                                        UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                                        return(errCodeParameter);
                        }
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE205:: key [%s] algorithmID = [%d] key[%s] algorithmID=[%d] illegal!\n",srcZpk.keyName, srcZpk.algorithmID, dstZpk.keyName, dstZpk.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	
	//���� pinBlock
        if ((ret = UnionSetResponseXMLPackageValue("body/pinBlock", newPinBlock)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE205: UnionSetResponseXMLPackageValue[body/pinBlock]!\n");
                return(ret);
        }

	return 0;
}
