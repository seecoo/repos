//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-08-27

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
#include "symmetricKeyDB.h"
#include "unionHsmCmdVersion.h"

/***************************************
�������:	E144
������:		ת����PIN
��������:	ת����PIN
***************************************/
int UnionDealServiceCodeE144(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				isSameOfAccNo = 0;
	int				pinLen = 0;
	int				checkFlag1 = 0;
	int				checkFlag2 = 0;
	char				tmpBuf[32];
	char				accNo1[64];
	char				accNo2[64];
	char				pinByLmk[64];
	char				keyName1[160];
	char				keyName2[160];
	char				pinOffset[32];
	char				tmpZpkValue[64];
	char				tmpZpkCheckValue[32];
	char				tmpSm4ZpkValue[64];
	char				tmpSm4ZpkCheckValue[32];
	char				pinBlockByZpk[64];
	char				pinBlockBySm4Zpk[64];
	char				checkData1[32];
	char				checkData2[32];
	char				*ptr = NULL;
	int				len  = 0;

	TUnionSymmetricKeyDB		pvkKeyDB1;
	PUnionSymmetricKeyValue		ppvkKeyValue1 = NULL;
	TUnionSymmetricKeyDB		pvkKeyDB2;
	PUnionSymmetricKeyValue		ppvkKeyValue2 = NULL;
	
	// PIN����
	if ((ret = UnionReadRequestXMLPackageValue("body/pinLen",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE144:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinLen");
		return(ret);
	}
	tmpBuf[ret] = 0; // add by chenqy 20151202
	pinLen = atoi(tmpBuf);

	// PIN����
	if ((ret = UnionReadRequestXMLPackageValue("body/pinOffset",pinOffset,sizeof(pinOffset))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE144:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinOffset");
		return(ret);
	}
	else
	{
		pinOffset[ret] = 0;
		UnionFilterHeadAndTailBlank(pinOffset);
		if (strlen(pinOffset) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE144:: pinOffset can not be null!\n");
			UnionSetResponseRemark("PIN���Ĳ���Ϊ��!");
			return(errCodeParameter);
		}
		
		if ((ptr = strchr(pinOffset,'F')) != NULL)
		{
			len = ptr - pinOffset;
			memset(tmpBuf,'F',strlen(pinOffset) - len);
			tmpBuf[strlen(pinOffset) - len] = 0; // add by chenqy 20151202
			if (strcmp(tmpBuf,pinOffset+len) != 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE144:: tmpBuf[%s] pinOffset[%s] is invalid!\n",tmpBuf, pinOffset);
				UnionSetResponseRemark("PIN����[%s]�Ƿ����,������ΪF",pinOffset);
				return(errCodeParameter);
			}
			memcpy(tmpBuf,pinOffset,len);
			tmpBuf[len] = 0;
		}
		else
			strcpy(tmpBuf,pinOffset);

		if (!UnionIsDigitString(tmpBuf))
		{
			UnionUserErrLog("in UnionDealServiceCodeE144:: pinOffset[%s] is invalid!\n",pinOffset);
			UnionSetResponseRemark("PIN����[%s]�Ƿ�,����Ϊ����",tmpBuf);
			return(errCodeParameter);
		}
	}
	// ԴPVK��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName1",keyName1,sizeof(keyName1))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE144:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName1");
		return(ret);
	}
		
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName1,1,&pvkKeyDB1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE144:: UnionReadSymmetricKeyDBRec keyName1[%s]!\n",keyName1);
		return(ret);
	}
	if (pvkKeyDB1.keyType != conPVK)
	{
		UnionUserErrLog("in UnionDealServiceCodeE144:: is not pvk [%s]!\n",keyName1);
		return(errCodeEsscMDL_WrongUsageOfKey);
	}
	// ��ȡ��Կֵ
	if ((ppvkKeyValue1 = UnionGetSymmetricKeyValue(&pvkKeyDB1,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE142:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(errCodeParameter);
	}
		
	// Ŀ��PVK��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName2",keyName2,sizeof(keyName2))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE144:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName2");
		return(ret);
	}
		
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName2,1,&pvkKeyDB2)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE144:: UnionReadSymmetricKeyDBRec keyName2[%s]!\n",keyName2);
		return(ret);
	}
	if (pvkKeyDB2.keyType != conPVK)
	{
		UnionUserErrLog("in UnionDealServiceCodeE144:: is not pvk [%s]!\n",keyName2);
		return(errCodeEsscMDL_WrongUsageOfKey);
	}
	// ��ȡ��Կֵ
	if ((ppvkKeyValue2 = UnionGetSymmetricKeyValue(&pvkKeyDB2,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE142:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(errCodeParameter);
	}

	// �˺�1
	if ((ret = UnionReadRequestXMLPackageValue("body/accNo1",accNo1,sizeof(accNo1))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE144:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo1");
		return(ret);
	}
	
	// ���Դ�ʺ�
        UnionFilterHeadAndTailBlank(accNo1);
        if (strlen(accNo1) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE144:: accNo1 can not be null!\n");
                UnionSetResponseRemark("Դ�˺Ų���Ϊ��!");
                return(errCodeParameter);
        }
        if (!UnionIsDigitString(accNo1))
        {
                UnionUserErrLog("in UnionDealServiceCodeE144:: accNo1[%s] is error!\n",accNo1);
                UnionSetResponseRemark("Դ�˺ŷǷ�[%s],����Ϊ����",accNo1);
                return(errCodeHsmCmdMDL_InvalidAcc);
        }

	// �˺�2
	if ((ret = UnionReadRequestXMLPackageValue("body/accNo2",accNo2,sizeof(accNo2))) < 0)
		strcpy(accNo2,accNo1);
	
	if (strcmp(accNo1,accNo2) == 0)
		isSameOfAccNo = 1;
	else
		isSameOfAccNo = 0;

	// ���Ŀ���ʺ�
	UnionFilterHeadAndTailBlank(accNo2);
        if (strlen(accNo2) > 0)
        {
		if (!UnionIsDigitString(accNo2))
		{
			UnionUserErrLog("in UnionDealServiceCodeE144:: accNo2[%s] is error!\n",accNo2);
			UnionSetResponseRemark("Ŀ���˺ŷǷ�[%s],����Ϊ����",accNo2);
			return(errCodeHsmCmdMDL_InvalidAcc);
		}
	}

	// �����ʶ1
	if ((ret = UnionReadRequestXMLPackageValue("body/checkFlag1",tmpBuf,sizeof(tmpBuf))) < 0)
		checkFlag1 = 0;
	else
		checkFlag1 = atoi(tmpBuf);
		
	if (checkFlag1 == 0)	// ϵͳ�趨
		snprintf(checkData1,sizeof(checkData1),"%s",UnionGetIBMUserDefinedData());
	else if (checkFlag1 == 1)	// �û�����
	{
		// PINУ������1
		if ((ret = UnionReadRequestXMLPackageValue("body/checkData1",checkData1,sizeof(checkData1))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE144:: UnionReadRequestXMLPackageValue[%s]!\n","body/checkData1");
			return(ret);
		}	
	}
	else
		snprintf(checkData1,sizeof(checkData1),"%s",UnionGetIBMUserDefinedData());

	// �����ʶ2
	if ((ret = UnionReadRequestXMLPackageValue("body/checkFlag2",tmpBuf,sizeof(tmpBuf))) < 0)
		checkFlag2 = 0;
	else
		checkFlag2 = atoi(tmpBuf);
		
	if (checkFlag2 == 0)	// ϵͳ�趨
		snprintf(checkData2,sizeof(checkData2),"%s",UnionGetIBMUserDefinedData());
	else if (checkFlag2 == 1)	// �û�����
	{
		// PINУ������2
		if ((ret = UnionReadRequestXMLPackageValue("body/checkData2",checkData2,sizeof(checkData2))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE144:: UnionReadRequestXMLPackageValue[%s]!\n","body/checkData2");
			return(ret);
		}	
	}
	else if (checkFlag2 == 2)	// ϵͳ�������
		UnionGetIBMPinCheckData(checkData2);
	else
		snprintf(checkData2,sizeof(checkData2),"%s",UnionGetIBMUserDefinedData());

	switch(pvkKeyDB1.algorithmID)
	{
		case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					// ���������ָ��EE����PVK1���ܵ�PINOffsetת��ΪLMK���ܵ�PIN
					if ((ret = UnionHsmCmdEE(pinLen,checkData1,UnionGetIBMDecimalizationTable(),
						pinOffset,UnionConvertSymmetricKeyKeyLen(pvkKeyDB1.keyLen),ppvkKeyValue1->keyValue,accNo1,strlen(accNo1),pinByLmk)) < 0)
					{
						// ʹ�þɰ汾��Կ
						if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
							UnionOldVersionSymmetricKeyIsUsed(&pvkKeyDB1))
						{
							if ((ret = UnionHsmCmdEE(pinLen,checkData1,UnionGetIBMDecimalizationTable(),
								pinOffset,UnionConvertSymmetricKeyKeyLen(pvkKeyDB1.keyLen),ppvkKeyValue1->oldKeyValue,accNo1,strlen(accNo1),pinByLmk)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE144:: UnionHsmCmdEE current key ret = [%d]!\n",ret);
								return(ret);
							}
						}
						else
						{
							UnionUserErrLog("in UnionDealServiceCodeE144:: UnionHsmCmdEE current key ret = [%d]!\n",ret);
							return(ret);
						}
					}
					// add by chenqy 20151130
					// ����pinOffsetת����pinOffset
					if (pvkKeyDB2.algorithmID == conSymmetricAlgorithmIDOfSM4)
					{
						// ���������ָ��A0������һ����ʱ��DES�㷨ZPK
						if ((ret = UnionHsmCmdA0(0,conZPK,UnionConvertSymmetricKeyKeyLen(pvkKeyDB1.keyLen),NULL,tmpZpkValue,NULL,tmpZpkCheckValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE144:: UnionHsmCmdA0 ret = [%d]!\n",ret);
							return(ret);
						}
						// ���������ָ��JG����LMK���ܵ�PIN����ת��ΪZPK���ܵ�PIN����
						if ((ret = UnionHsmCmdJG(strlen(tmpZpkValue),tmpZpkValue,
							"01",strlen(pinByLmk),pinByLmk,accNo1,strlen(accNo1),pinBlockByZpk)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE144:: UnionHsmCmdJG ret = [%d]!\n",ret);
							return(ret);
						}
						// ���������ָ��WI������һ����ʱ��SM4�㷨ZPK
						if ((ret = UnionHsmCmdWI("0",conZPK,NULL,tmpSm4ZpkValue,NULL,tmpSm4ZpkCheckValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE144:: UnionHsmCmdWI ret = [%d]!\n", ret);
							return(ret);
						}
						// ���������ָ��W8����Deszpk���ܵ�pinBLock תΪ Sm4Zpk���ܵ�pinBlock
						if ((ret = UnionHsmCmdW8(1, 2, 2, NULL, tmpZpkValue, 2, NULL, tmpSm4ZpkValue, "01", "01", pinBlockByZpk, accNo1, accNo2, pinBlockBySm4Zpk, sizeof(pinBlockBySm4Zpk))) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE144:: UnionHsmCmdW8 ret = [%d]!\n", ret);
							return(ret);
						}
						// ���������ָ��WX����Sm4Zpk���ܵ�pinBlock תΪ Lmk���ܵ�pin����
						if ((ret = UnionHsmCmdWX(1,tmpSm4ZpkValue,pinBlockBySm4Zpk,"01",accNo2,strlen(accNo2),pinByLmk,sizeof(pinByLmk))) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE144:: UnionHsmCmdWX ret = [%d]!\n", ret);
							return(ret);	
						}
						// ����8Eָ���IBM��ʽ����һ��PIN��Offset
						if ((ret = UnionHsmCmd8E(pinLen,checkData2,UnionGetIBMDecimalizationTable(),strlen(pinByLmk),pinByLmk,ppvkKeyValue2->keyValue,accNo2,strlen(accNo2),pinOffset)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE144:: UnionHsmCmd8E ret = [%d]!\n", ret);
							return(ret);
						}
						break;	
					}
					// add end
					if (strcmp(accNo1,accNo2) != 0) // �˺Ų�ͬ
					{
						// ���������ָ��A0������һ����ʱ��ZPK
						if ((ret = UnionHsmCmdA0(0,conZPK,UnionConvertSymmetricKeyKeyLen(pvkKeyDB1.keyLen),NULL,tmpZpkValue,NULL,tmpZpkCheckValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE144:: UnionHsmCmdA0 ret = [%d]!\n",ret);
							return(ret);
						}
						// ���������ָ��JG����LMK���ܵ�PIN���ģ��ʺ�1��ת��ΪZPK���ܵ�PIN���ģ����ʺţ�
						if ((ret = UnionHsmCmdJG(strlen(tmpZpkValue),tmpZpkValue,
							"03",strlen(pinByLmk),pinByLmk,accNo1,strlen(accNo1),pinBlockByZpk)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE144:: UnionHsmCmdJG ret = [%d]!\n",ret);
							return(ret);
						}
						// ���������ָ��JE����ZPK���ܵ�PIN���ģ����ʺţ�ת��ΪLMK���ܵ�PIN���ģ��ʺ�2��
						if ((ret = UnionHsmCmdJE(strlen(tmpZpkValue),tmpZpkValue,
							"03",pinBlockByZpk,accNo2,strlen(accNo2),pinByLmk)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE144:: UnionHsmCmdJE ret = [%d]!\n",ret);
							return(ret);
						}
					}
			
					// ���������ָ��DE����LMK���ܵ�PIN����ΪPVK2���ܵ�PINOffset
					if ((ret = UnionHsmCmdDE(pinLen,checkData2,UnionGetIBMDecimalizationTable(),
						strlen(pinByLmk),pinByLmk,UnionConvertSymmetricKeyKeyLen(pvkKeyDB2.keyLen),ppvkKeyValue2->keyValue,
						accNo2,strlen(accNo2),pinOffset)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE144:: UnionHsmCmdDE ret = [%d]!\n",ret);
						return(ret);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE144:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
					return(errCodeParameter);
			}
			break;
		case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
		default:
			UnionUserErrLog("in UnionDealServiceCodeE144:: pvkKeyDB1.algorithmID[%d] error!\n",pvkKeyDB1.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	// ����PIN����
	if ((ret = UnionSetResponseXMLPackageValue("body/pinOffset",pinOffset)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE144:: UnionSetResponseXMLPackageValue pinOffset[%s]!\n",pinOffset);
		return(ret);
	}

	// ����PINУ������
	if ((ret = UnionSetResponseXMLPackageValue("body/checkData",checkData2)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE144:: UnionSetResponseXMLPackageValue checkData[%s]!\n",checkData2);
		return(ret);
	}

	return(0);
}
