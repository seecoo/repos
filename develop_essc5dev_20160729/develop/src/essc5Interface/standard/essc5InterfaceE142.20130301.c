//	Author:		������
//	Copyright:	Union Tech. Guangzhou

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
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "defaultIBMPinOffsetDef.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmdVersion.h"
//add by zhouxw 20151110
#include "3DesRacalSyntaxRules.h"
#include "unionREC.h"
//add end

/***************************************
�������:	E142
������:		ת����PIN
��������:	ת����PIN
***************************************/
int UnionDealServiceCodeE142(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				isSameOfAccNo = 0;
	int				isNeedTwoChange = 0;
	int				isLmk1 = 0;
	int				isLmk2 = 0;
	int				checkFlag = 0;
	int				decimalFlag = 0;
	char				tmpBuf[32];
	char				accNo1[64];
	char				accNo2[64];
	char				pinBlock[64];
	char				newPinBlock[64];
	char				keyName1[128];
	char				keyName2[128];
	char				format1[32] = "01";
	char				format2[32] = "01";
	char				tmpFormat[32];
	char				pinOffset[32];
	char				checkData[32];
	char				decimalTable[32];
	char				accNo2PinBlock[64];
	char				tmpKeyByLMK[64];
	char				tmpCheckValue[64];
	char				keyType[8];
	//char				tmpNewPinBlock[64];
	//add by zhouxw 20150923
	int				lenOfAccNo = 0;
	char				tmpAccNo[16];
	char				*ptr = NULL;
	// add by chenqy
	int				pinLen;
	// add end

	TUnionSymmetricKeyDB		symmetricKeyDB1;
	PUnionSymmetricKeyValue		psymmetricKeyValue1 = NULL;
	TUnionSymmetricKeyDB		symmetricKeyDB2;
	PUnionSymmetricKeyValue		psymmetricKeyValue2 = NULL;
	
	// PIN����
	if ((ret = UnionReadRequestXMLPackageValue("body/pinBlock",pinBlock,sizeof(pinBlock))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE142:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinBlock");
		return(ret);
	}
	else
        {
		UnionFilterHeadAndTailBlank(pinBlock);
		if (strlen(pinBlock) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE142:: pinBlock can not be null!\n");
                	UnionSetResponseRemark("PIN���Ĳ���Ϊ��!");
                	return(errCodeParameter);
		}
                if (!UnionIsBCDStr(pinBlock))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE142:: pinBlock[%s] is error!\n",pinBlock);
                        UnionSetResponseRemark("PIN����[%s]�Ƿ�,����Ϊʮ��������",pinBlock);
                        return(errCodeParameter);
                }
        }

	// ��Կ����1
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName1",keyName1,sizeof(keyName1))) > 0)
	{
		keyName1[ret] = 0;
		UnionFilterHeadAndTailBlank(keyName1);

		// ��ȡ��Կ��Ϣ
		if ((ret =  UnionReadSymmetricKeyDBRec(keyName1,1,&symmetricKeyDB1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE142:: UnionReadSymmetricKeyDBRec keyName1[%s]!\n",keyName1);
			return(ret);
		}
		// ��ȡ��Կֵ
		if ((psymmetricKeyValue1 = UnionGetSymmetricKeyValue(&symmetricKeyDB1,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE142:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("��ȡ��Կֵʧ��");
			return(errCodeParameter);
		}

		// �˺�1
		if ((ret = UnionReadRequestXMLPackageValue("body/accNo1",accNo1,sizeof(accNo1))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE142:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo1");
			return(ret);
		}
		accNo1[ret] = 0;

		UnionFilterHeadAndTailBlank(accNo1);
		// modify by zhouxw 20151110
		/*
		if ((lenOfAccNo = strlen(accNo1)) > 0)
		{
			// add by zhouxw 20150923
			// �������֤��Ϊ�˺�ʱ�����һλΪ'X'�����񱨴�
			// �˺Ŵ���12λʱ�����һλ����������޸Ĳ���Ӱ��ԭ�й���
			if(lenOfAccNo > 12)
				accNo1[lenOfAccNo - 1] = '0';
			//add end
			if (!UnionIsDigitString(accNo1))
			{
				UnionUserErrLog("in UnionDealServiceCodeE142:: accNo1[%s] is error!\n",accNo1);
				UnionSetResponseRemark("Դ�˺ŷǷ�[%s],����Ϊ����",accNo1);
				return(errCodeHsmCmdMDL_InvalidAcc);
			}
		}
		*/
		if ((lenOfAccNo = strlen(accNo1)) == 0)
        	{
        	        UnionUserErrLog("in UnionDealServiceCodeE142:: accNo can not be null!\n");
        	        UnionSetResponseRemark("�˺Ų���Ϊ��!");
        	        return(errCodeParameter);
        	}
		if(lenOfAccNo > 12)
		{
			if((ret = UnionForm12LenAccountNumber(accNo1, lenOfAccNo, tmpAccNo)) != 12)
			{
				UnionUserErrLog("in UnionDealServiceCodeE142:: UnionForm12LenAccountNumber err\n");
				UnionSetResponseRemark("ȡ12λ�˺�ʱ����!");
				return(errCodeParameter);
			}
			memcpy(accNo1, tmpAccNo, 12);
			accNo1[12] = 0;
		}
		if (!UnionIsDigitString(accNo1))
		{
			UnionUserErrLog("in UnionDealServiceCodeE142:: accNo1[%s] is error!\n",accNo1);
			UnionSetResponseRemark("Դ�˺ŷǷ�[%s],����Ϊ����",accNo1);
			return(errCodeHsmCmdMDL_InvalidAcc);
		}
		// modify end 20151110

		// ��ʽ
		if (symmetricKeyDB1.keyType != conPVK)
		{
			if ((ret = UnionReadRequestXMLPackageValue("body/format1",format1,sizeof(format1))) <= 0)
				strcpy(format1,"01");
		}
	}
	else
		isLmk1 = 1;

	// ��Կ����2
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName2",keyName2,sizeof(keyName2))) > 0)
	{
		keyName2[ret] = 0;
		UnionFilterHeadAndTailBlank(keyName2);
		
		if ((ret =  UnionReadSymmetricKeyDBRec(keyName2,1,&symmetricKeyDB2)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE142:: UnionReadSymmetricKeyDBRec keyName2[%s]!\n",keyName2);
			return(ret);
		}

		// ��ȡ��Կֵ
		if ((psymmetricKeyValue2 = UnionGetSymmetricKeyValue(&symmetricKeyDB2,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE142:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("��ȡ��Կֵʧ��");
			return(errCodeParameter);
		}
		// ��ʽ
		if (symmetricKeyDB2.keyType != conPVK)
		{
			if ((ret = UnionReadRequestXMLPackageValue("body/format2",format2,sizeof(format2))) <= 0)
				strcpy(format2,"01");
		}

		// �˺�2
		if ((ret = UnionReadRequestXMLPackageValue("body/accNo2",accNo2,sizeof(accNo2))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE142:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo2");
			return(ret);
		}
		accNo2[ret] = 0;
		UnionFilterHeadAndTailBlank(accNo2);
		/*
		if ((lenOfAccNo = strlen(accNo2)) > 0)
		{
			// add by zhouxw 20150923
			// �������֤��Ϊ�˺�ʱ�����һλΪ'X'�����񱨴�
			// �˺Ŵ���12λʱ�����һλ����������޸Ĳ���Ӱ��ԭ�й���
			if(lenOfAccNo > 12)
				accNo2[lenOfAccNo - 1] = '0';
			//add end
			if (!UnionIsDigitString(accNo2))
			{
				UnionUserErrLog("in UnionDealServiceCodeE142:: accNo2[%s] is error!\n",accNo2);
				UnionSetResponseRemark("Ŀ���˺ŷǷ�[%s],����Ϊ����",accNo2);
				return(errCodeHsmCmdMDL_InvalidAcc);
			}
		}
		*/
		if ((lenOfAccNo = strlen(accNo2)) == 0)
        	{
        	        UnionUserErrLog("in UnionDealServiceCodeE142:: accNo can not be null!\n");
        	        UnionSetResponseRemark("�˺Ų���Ϊ��!");
        	        return(errCodeParameter);
        	}
		if((strcmp(format2,"17")==0) && (lenOfAccNo == 16))
		{
			UnionLog("format2 = 17,AccNo len = 16 \n");
		}
		else if(lenOfAccNo > 12)
		{
			if((ret = UnionForm12LenAccountNumber(accNo2, lenOfAccNo, tmpAccNo)) != 12)
			{
				UnionUserErrLog("in UnionDealServiceCodeE142:: UnionForm12LenAccountNumber err\n");
				UnionSetResponseRemark("ȡ12λ�˺�ʱ����!");
				return(errCodeParameter);
			}
			memcpy(accNo2, tmpAccNo, 12);
			accNo2[12] = 0;
		}
		if (!UnionIsDigitString(accNo2))
		{
			UnionUserErrLog("in UnionDealServiceCodeE142:: accNo2[%s] is error!\n",accNo2);
			UnionSetResponseRemark("Ŀ���˺ŷǷ�[%s],����Ϊ����",accNo2);
			return(errCodeHsmCmdMDL_InvalidAcc);
		}
		// modify end 20151110

/*		// ��ʽ
		if (symmetricKeyDB2.keyType != conPVK)
		{
			if ((ret = UnionReadRequestXMLPackageValue("body/format2",format2,sizeof(format2))) <= 0)
				strcpy(format2,"01");
		}*/
	}
	else
		isLmk2 = 1;

	// ZPKתZPK
	if ((isLmk2 == 0) && (isLmk1 == 0) && (symmetricKeyDB1.keyType == conZPK) && (symmetricKeyDB2.keyType == conZPK))
	{
		if (strcmp(accNo1,accNo2) == 0)
			isSameOfAccNo = 1;
		else
			isSameOfAccNo = 0;
		
		// �˺Ų�ͬ����PIN��ʽ��Ҫ�˺Ų������㣬��Ҫ������ת��
		if ((isSameOfAccNo == 0) && ((strcmp(format2,"01") == 0) || (strcmp(format2,"04") == 0)))
		{
			strcpy(tmpFormat,"03");
			isNeedTwoChange = 1;
		}
		else
		{
			strcpy(tmpFormat,format2);
			isNeedTwoChange = 0;
		}
		switch(symmetricKeyDB1.algorithmID || symmetricKeyDB2.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdCC(UnionConvertSymmetricKeyKeyLen(symmetricKeyDB1.keyLen),psymmetricKeyValue1->keyValue,
							UnionConvertSymmetricKeyKeyLen(symmetricKeyDB2.keyLen),psymmetricKeyValue2->keyValue,
							12,format1,pinBlock,accNo1,strlen(accNo1),tmpFormat,newPinBlock,&pinLen)) < 0)
						{
							// ʹ�þɰ汾��Կ
							if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&symmetricKeyDB1))
							{
								if ((ret = UnionHsmCmdCC(UnionConvertSymmetricKeyKeyLen(symmetricKeyDB1.keyLen),psymmetricKeyValue1->oldKeyValue,
									UnionConvertSymmetricKeyKeyLen(symmetricKeyDB2.keyLen),psymmetricKeyValue2->keyValue,
									12,format1,pinBlock,accNo1,strlen(accNo1),tmpFormat,newPinBlock,&pinLen)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdCC current key ret = [%d]!\n",ret);
									return(ret);
								}
							}
							else
							{
								UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdCC current key ret = [%d]!\n",ret);
								return(ret);
							}
						}
						
						if (isNeedTwoChange)
						{
							if ((ret = UnionHsmCmdCC(UnionConvertSymmetricKeyKeyLen(symmetricKeyDB2.keyLen),psymmetricKeyValue2->keyValue,
								UnionConvertSymmetricKeyKeyLen(symmetricKeyDB2.keyLen),psymmetricKeyValue2->keyValue,
								12,tmpFormat,newPinBlock,accNo2,strlen(accNo2),format2,newPinBlock,&pinLen)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdCC two key ret = [%d]!\n",ret);
								return(ret);
							}
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE142:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
				break;
				
			case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
				if ((ret = UnionHsmCmdW8(symmetricKeyDB1.algorithmID+1,symmetricKeyDB2.algorithmID+1,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB1.keyLen)+1,NULL,psymmetricKeyValue1->keyValue,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB2.keyLen)+1,NULL,psymmetricKeyValue2->keyValue,format1,format2,pinBlock,accNo1,accNo2,newPinBlock,sizeof(newPinBlock))) < 0)
				{
					// ʹ�þɰ汾��Կ
					if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
						UnionOldVersionSymmetricKeyIsUsed(&symmetricKeyDB1))
					{
						if ((ret = UnionHsmCmdW8(symmetricKeyDB1.algorithmID+1,symmetricKeyDB2.algorithmID+1,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB1.keyLen)+1,NULL,psymmetricKeyValue1->oldKeyValue,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB2.keyLen)+1,NULL,psymmetricKeyValue2->keyValue,format1,format2,pinBlock,accNo1,accNo2,newPinBlock,sizeof(newPinBlock))) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdW8 old key ret = [%d]!\n",ret);
							return(ret);
						}
					}
					else
					{
						UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdW8 current key ret = [%d]!\n",ret);
						return(ret);
					}
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE142:: symmetricKeyDB1.algorithmID[%d] error!\n",symmetricKeyDB1.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
	}	
	// ZPKתLMK
	else if (isLmk1 == 0 && symmetricKeyDB1.keyType == conZPK && isLmk2)
	{
		switch(symmetricKeyDB1.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdJE(UnionConvertSymmetricKeyKeyLen(symmetricKeyDB1.keyLen),psymmetricKeyValue1->keyValue,format1,
							pinBlock,accNo1,strlen(accNo1),newPinBlock)) < 0)
						{
							// ʹ�þɰ汾��Կ
							if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&symmetricKeyDB1))
							{
								if ((ret = UnionHsmCmdJE(UnionConvertSymmetricKeyKeyLen(symmetricKeyDB1.keyLen),psymmetricKeyValue1->oldKeyValue,format1,
									pinBlock,accNo1,strlen(accNo1),newPinBlock)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdJE old key ret = [%d]!\n",ret);
									return(ret);
								}
							}
							else
							{
								UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdJE current key ret = [%d]!\n",ret);
								return(ret);
							}
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE142:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
				break;
			case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdWX(1,psymmetricKeyValue1->keyValue,pinBlock,format1,accNo1,strlen(accNo1),newPinBlock,sizeof(newPinBlock))) < 0)
						{
							// ʹ�þɰ汾��Կ
							if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&symmetricKeyDB1))
							{
								if ((ret = UnionHsmCmdWX(1,psymmetricKeyValue1->oldKeyValue,pinBlock,format1,accNo1,strlen(accNo1),newPinBlock,sizeof(newPinBlock))) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdWX  old key ret = [%d]!\n",ret);
									return(ret);

								}
							}
							else
							{
								UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdWX cur key ret = [%d]!\n",ret);
								return(ret);
							}
						}
						break;
					default:	
						UnionUserErrLog("in UnionDealServiceCodeE142:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE142:: symmetricKeyDB1.algorithmID[%d] error!\n",symmetricKeyDB1.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
	}	
	// LMKתZPK
	else if (isLmk1 && symmetricKeyDB2.keyType == conZPK && isLmk2 == 0)	
	{
		switch(symmetricKeyDB2.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:	
						if ((ret = UnionHsmCmdJG(UnionConvertSymmetricKeyKeyLen(symmetricKeyDB2.keyLen),psymmetricKeyValue2->keyValue,format2,
							strlen(pinBlock),pinBlock,accNo2,strlen(accNo2),newPinBlock)) < 0)
						{
							// ʹ�þɰ汾��Կ
							if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&symmetricKeyDB2))
							{
								if ((ret = UnionHsmCmdJG(UnionConvertSymmetricKeyKeyLen(symmetricKeyDB2.keyLen),psymmetricKeyValue2->oldKeyValue,format2,
									strlen(pinBlock),pinBlock,accNo2,strlen(accNo2),newPinBlock)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdJE old key ret = [%d]!\n",ret);
									return(ret);
								}
							}
							else
							{
								UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdJE current key ret = [%d]!\n",ret);
								return(ret);
							}
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE142:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
				break;
			case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdWX(0,psymmetricKeyValue2->keyValue,pinBlock,format2,accNo2,strlen(accNo2),newPinBlock,sizeof(newPinBlock))) < 0)
						{
							// ʹ�þɰ汾��Կ
							if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&symmetricKeyDB2))
							{
								if ((ret = UnionHsmCmdWX(0,psymmetricKeyValue2->oldKeyValue,pinBlock,format2,accNo2,strlen(accNo2),newPinBlock,sizeof(newPinBlock))) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdWX  old key ret = [%d]!\n",ret);
									return(ret);

								}
							}
							else
							{
								UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdWX cur key ret = [%d]!\n",ret);
								return(ret);
							}
						}
						break;
					default:	
						UnionUserErrLog("in UnionDealServiceCodeE142:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
				break;

			default:
				UnionUserErrLog("in UnionDealServiceCodeE142:: symmetricKeyDB2.algorithmID[%d] error!\n",symmetricKeyDB2.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
	}	
	// ZPKתPVK
	else if (isLmk1 == 0 && isLmk2 == 0 && symmetricKeyDB1.keyType == conZPK && symmetricKeyDB2.keyType == conPVK)
	{
		if (strcmp(accNo1,accNo2) == 0)
		{
			snprintf(newPinBlock,sizeof(newPinBlock),"%s",pinBlock);
			isSameOfAccNo = 1;
		}
		else
			isSameOfAccNo = 0;
		
		// �����ʶ
		if ((ret = UnionReadRequestXMLPackageValue("body/checkFlag",tmpBuf,sizeof(tmpBuf))) < 0)
			checkFlag = 0;
		else
			checkFlag = atoi(tmpBuf);
		
		if (checkFlag == 0)	// ϵͳ�趨
			snprintf(checkData,sizeof(checkData),"%s",UnionGetIBMUserDefinedData());
		else if (checkFlag == 1)	// �û�����
		{
			// modify by zhouxw 20160506 begin
			if(strcasecmp(UnionGetIDOfCustomization(), "CBHB") == 0)
			{
				if((ret = UnionReadRequestXMLPackageValue("body/checkData", checkData, sizeof(checkData))) <= 0)
				{
					if((ptr = UnionReadStringTypeRECVar("checkData")) == NULL)
					{
						UnionUserErrLog("in UnionDealServiceCodeE132:: UnionReadStringTypeRECVar[%s]\n", "checkData");
						return(errCodeRECMDL_VarNotExists);
					}
					else
						strcpy(checkData, ptr);
				}
				else
					checkData[ret] = 0;
			}
			else
			{
				// PINУ������
				if ((ret = UnionReadRequestXMLPackageValue("body/checkData",checkData,sizeof(checkData))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE142:: UnionReadRequestXMLPackageValue[%s]!\n","body/checkData");
					return(ret);
				}	
				UnionFilterHeadAndTailBlank(checkData);
				if (strlen(checkData) != 12)
				{
					UnionUserErrLog("in UnionDealServiceCodeE142:: checkData[%s]!\n",checkData);
					UnionSetResponseRemark("pinУ������[checkData]����Ϊ12λ");
					return(errCodeParameter);
				}
			}
			// modify by zhouxw 20160506 end
		}	
		else if (checkFlag == 2)	// ϵͳ�������
			UnionGetIBMPinCheckData(checkData);
		else
			snprintf(checkData,sizeof(checkData),"%s",UnionGetIBMUserDefinedData());

		// ʮ����ת�����ʶ
		if ((ret = UnionReadRequestXMLPackageValue("body/decimalFlag",tmpBuf,sizeof(tmpBuf))) < 0)
			decimalFlag = 0;
		else
			decimalFlag = atoi(tmpBuf);
		
		if (decimalFlag == 0)	// ϵͳ�趨
			snprintf(decimalTable,sizeof(decimalTable),"%s",UnionGetIBMDecimalizationTable());
		else if (decimalFlag == 1)	// �û�����
		{
			// modify by zhouxw 20160506 begin
			if(strcasecmp(UnionGetIDOfCustomization(), "CBHB") == 0)
			{
				if((ret = UnionReadRequestXMLPackageValue("body/decimalTable", decimalTable, sizeof(decimalTable))) <= 0)
				{
					if((ptr = UnionReadStringTypeRECVar("decimalizationTable")) == NULL)
					{
						UnionUserErrLog("in UnionDealServiceCodeE132:: UnionReadStringTypeRECVar[%s]\n", "decimalizationTable");
						return(errCodeRECMDL_VarNotExists);
					}
					else
						strcpy(decimalTable, ptr);
				}
				else
				{
					decimalTable[ret] = 0;
					UnionFilterHeadAndTailBlank(decimalTable);
					if (strlen(decimalTable) != 16)
					{
						UnionUserErrLog("in UnionDealServiceCodeE132:: decimalTable[%s]!\n", decimalTable);
						UnionSetResponseRemark("ʮ���ƻ��ű�[decimalTable]����Ϊ16λ");
						return(errCodeParameter);
					}
				}
			}
			else
			{
				// ʮ����ת����
				if ((ret = UnionReadRequestXMLPackageValue("body/decimalTable",decimalTable,sizeof(decimalTable))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE142:: UnionReadRequestXMLPackageValue[%s]!\n","body/decimalTable");
					return(ret);
				}	
				UnionFilterHeadAndTailBlank(decimalTable);
				if (strlen(decimalTable) != 16)
				{
					UnionUserErrLog("in UnionDealServiceCodeE142:: decimalTable[%s]!\n",decimalTable);
					UnionSetResponseRemark("ʮ���ƻ��ű�[decimalTable]����Ϊ16λ");
					return(errCodeParameter);
				}
			}
			//modify by zhouxw 20160506 end 
		}	
		else if (decimalFlag == 2)	// ϵͳ�������
			UnionGenerateIBMDecimalizationTable(decimalTable);
		else
			snprintf(decimalTable,sizeof(decimalTable),"%s",UnionGetIBMDecimalizationTable());

		switch(symmetricKeyDB1.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
				if (symmetricKeyDB2.algorithmID == conSymmetricAlgorithmIDOfSM4) // �ڶ�����Կ��SM4�㷨
				{
					// ������ʱ��ZPK(SM4)
					if ((ret = UnionHsmCmdWI("0",conZPK,NULL,tmpKeyByLMK,NULL,tmpCheckValue)) < 0) 
					{
						UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdWI!\n");
						return(ret);
					}
					// ��DES�㷨��PINBLOCKת��SM4�㷨
					if ((ret = UnionHsmCmdW8(1,2,2,NULL,psymmetricKeyValue1->keyValue,0,NULL,tmpKeyByLMK,format1,format1,pinBlock,accNo1,accNo1,pinBlock,sizeof(pinBlock))) < 0)
					{
						// ʹ�þɰ汾��Կ
						if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
							UnionOldVersionSymmetricKeyIsUsed(&symmetricKeyDB1))
						{
							if ((ret = UnionHsmCmdW8(1,2,2,NULL,psymmetricKeyValue1->oldKeyValue,0,NULL,tmpKeyByLMK,format1,format1,pinBlock,accNo1,accNo1,pinBlock,sizeof(pinBlock))) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdW8 old key ret = [%d]!\n",ret);
								return(ret);
							}
						}
						else
						{
							UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdW8 current key ret = [%d]!\n",ret);
							return(ret);
						}
					}
					// ����ʱ��Կ���Ƹ���һ����Կ
					strcpy(psymmetricKeyValue1->keyValue,tmpKeyByLMK);
					goto ZPKToPVKSM4;
				}
				else
				{
ZPKToPVKDES:
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case	conHsmCmdVerRacalStandardHsmCmd:
						case	conHsmCmdVerSJL06StandardHsmCmd:
							// zpk,pin��ʽ��01ת03,�˺�ΪaccNo1
							//strcpy(format1,"01");		
							if (!isSameOfAccNo)
							{
								if ((ret = UnionHsmCmdCC(UnionConvertSymmetricKeyKeyLen(symmetricKeyDB1.keyLen),psymmetricKeyValue1->keyValue,
									UnionConvertSymmetricKeyKeyLen(symmetricKeyDB1.keyLen),psymmetricKeyValue1->keyValue,
									12,"01",pinBlock,accNo1,strlen(accNo1),"03",newPinBlock,&pinLen)) < 0)
								{
									// ʹ�þɰ汾��Կ
									if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
										UnionOldVersionSymmetricKeyIsUsed(&symmetricKeyDB1))
									{
										if ((ret = UnionHsmCmdCC(UnionConvertSymmetricKeyKeyLen(symmetricKeyDB1.keyLen),psymmetricKeyValue1->oldKeyValue,
											UnionConvertSymmetricKeyKeyLen(symmetricKeyDB1.keyLen),psymmetricKeyValue1->oldKeyValue,
											12,"01",pinBlock,accNo1,strlen(accNo1),"03",newPinBlock,&pinLen)) < 0)
										{
											UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdCC current key ret = [%d]!\n",ret);
											return(ret);
										}
									}
									else
									{
										UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdCC current key ret = [%d]!\n",ret);
										return(ret);
									}
								}
		
								// zpk,pin��ʽ����03תΪ01,�˺Ÿ�ΪaccNo2
								if ((ret = UnionHsmCmdCC(UnionConvertSymmetricKeyKeyLen(symmetricKeyDB1.keyLen),psymmetricKeyValue1->keyValue,
									UnionConvertSymmetricKeyKeyLen(symmetricKeyDB1.keyLen),psymmetricKeyValue1->keyValue,
									12,"03",newPinBlock,accNo2,strlen(accNo2),"01",newPinBlock,&pinLen)) < 0)
								{
									// ʹ�þɰ汾��Կ
									if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
										UnionOldVersionSymmetricKeyIsUsed(&symmetricKeyDB1))
									{
										if ((ret = UnionHsmCmdCC(UnionConvertSymmetricKeyKeyLen(symmetricKeyDB1.keyLen),psymmetricKeyValue1->oldKeyValue,
											UnionConvertSymmetricKeyKeyLen(symmetricKeyDB1.keyLen),psymmetricKeyValue1->oldKeyValue,
											12,"03",newPinBlock,accNo2,strlen(accNo2),"01",newPinBlock,&pinLen)) < 0)
										{
											UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdCC current key ret = [%d]!\n",ret);
											return(ret);
										}
									}
									else
									{
										UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdCC current key ret = [%d]!\n",ret);
										return(ret);
									}
								}
								snprintf(pinBlock,sizeof(pinBlock),"%s",newPinBlock);
							}
								
							snprintf(accNo2PinBlock,sizeof(accNo2PinBlock),"%s",pinBlock);
		
							// ��ZPK���ܵ�PINת��ΪLMK����
							if ((ret = UnionHsmCmdJE(UnionConvertSymmetricKeyKeyLen(symmetricKeyDB1.keyLen),
								psymmetricKeyValue1->keyValue,"01",pinBlock,accNo2,strlen(accNo2),newPinBlock)) < 0)
							{
								// ʹ�þɰ汾��Կ
								if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
									UnionOldVersionSymmetricKeyIsUsed(&symmetricKeyDB1))
								{
									if ((ret = UnionHsmCmdJE(UnionConvertSymmetricKeyKeyLen(symmetricKeyDB1.keyLen),
										psymmetricKeyValue1->oldKeyValue,"01",pinBlock,accNo2,strlen(accNo2),
										newPinBlock)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdJE old key ret = [%d]!\n",ret);
										return(ret);
									}
								}
								else
								{
									UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdJE current key ret = [%d]!\n",ret);
									return(ret);
								}
							}
	
							// ��LMK���ܵ�PIN����ΪPVK���ܵ�PINOffset
							if ((ret = UnionHsmCmdDE(UnionGetIBMMinPINLength(),checkData,
								decimalTable,strlen(newPinBlock),newPinBlock,
								UnionConvertSymmetricKeyKeyLen(symmetricKeyDB2.keyLen),
								psymmetricKeyValue2->keyValue,accNo2,strlen(accNo2),pinOffset)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdDE key ret = [%d]!\n",ret);
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE142:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					break;
				}
			case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
				//add by zhouxw 20150722
				if(symmetricKeyDB2.algorithmID == conSymmetricAlgorithmIDOfDES)
				{
					//������ʱDESZPK
					if((ret = UnionHsmCmdA0(0, conZPK, con128BitsDesKey, NULL, tmpKeyByLMK, NULL, tmpCheckValue)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE142:: return[%d]\n", ret);
						return(ret);
					}
					if((ret = UnionHsmCmdW8(2, 1, 2, NULL, psymmetricKeyValue1->keyValue, 2, NULL, tmpKeyByLMK, format1, format1, pinBlock, accNo1, accNo1, pinBlock, sizeof(pinBlock))) < 0)
					{
						if((ret <= errCodeOffsetOfHsmReturnCodeMDL) &&
                                                        UnionOldVersionSymmetricKeyIsUsed(&symmetricKeyDB1))
						{
							if((ret = UnionHsmCmdW8(2, 1, 2, NULL, psymmetricKeyValue1->keyValue, 2 ,NULL, tmpKeyByLMK, format1, format1, pinBlock, accNo1, accNo1, pinBlock, sizeof(pinBlock))) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdW8 old version return[%d]\n", ret);
								return(ret);
							}
						}
						else
						{
							UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdW8 return[%d]\n", ret);
							return(ret);
						}
					}
					strcpy(psymmetricKeyValue1->keyValue,tmpKeyByLMK);
					goto ZPKToPVKDES;
				}
				//addd by zhouxw end 
ZPKToPVKSM4:
				if (!isSameOfAccNo)
				{
					// ZPKתZPK,��ͬ�˺�֮���໥ת��
					if ((ret = UnionHsmCmdW8(2,2,2,NULL,psymmetricKeyValue1->keyValue,2,NULL,psymmetricKeyValue1->keyValue,format1,format1,pinBlock,accNo1,accNo2,newPinBlock,sizeof(newPinBlock))) < 0)
					{
						// ʹ�þɰ汾��Կ
						if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
							UnionOldVersionSymmetricKeyIsUsed(&symmetricKeyDB1))
						{
							if ((ret = UnionHsmCmdW8(2,2,2,NULL,psymmetricKeyValue1->oldKeyValue,2,NULL,psymmetricKeyValue1->oldKeyValue,format1,format1,pinBlock,accNo1,accNo2,newPinBlock,sizeof(newPinBlock))) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdW8 old key ret = [%d]!\n",ret);
								return(ret);
							}
						}
						else
						{
							UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdW8 current key ret = [%d]!\n",ret);
							return(ret);
						}
					}
					snprintf(pinBlock,sizeof(pinBlock),"%s",newPinBlock);
				}
	
				snprintf(accNo2PinBlock,sizeof(accNo2PinBlock),"%s",pinBlock);

				//ZPKתLMK
				if ((ret = UnionHsmCmdWX(1,psymmetricKeyValue1->keyValue,pinBlock,format1,accNo2,strlen(accNo2),newPinBlock,sizeof(newPinBlock))) < 0)
				{
					// ʹ�þɰ汾��Կ
					if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
						UnionOldVersionSymmetricKeyIsUsed(&symmetricKeyDB1))
					{
						if ((ret = UnionHsmCmdWX(1,psymmetricKeyValue1->oldKeyValue,pinBlock,format1,accNo2,strlen(accNo2),newPinBlock,sizeof(newPinBlock))) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdWX  old key ret = [%d]!\n",ret);
							return(ret);
						}
					}
					else
					{
						UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdWX cur key ret = [%d]!\n",ret);
						return(ret);
					}
				}
				
				// LMK ת PVK
				if ((ret = UnionHsmCmd8E(UnionGetIBMMinPINLength(),checkData,
					decimalTable,strlen(newPinBlock),newPinBlock,
					psymmetricKeyValue2->keyValue,accNo2,strlen(accNo2),pinOffset)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmd8E!\n");
					return(ret);
				}
				pinOffset[ret] = 0;
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE142:: symmetricKeyDB1.algorithmID[%d] error!\n",symmetricKeyDB1.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}	
	}
	// TPKתZPK
	else if (isLmk1 == 0 && isLmk2 == 0 && symmetricKeyDB1.keyType == conTPK && symmetricKeyDB2.keyType == conZPK)
	{
		switch(symmetricKeyDB1.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
			{
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdCA(strlen(psymmetricKeyValue1->keyValue),psymmetricKeyValue1->keyValue,
							strlen(psymmetricKeyValue2->keyValue),psymmetricKeyValue2->keyValue,
							format1,pinBlock,accNo1,strlen(accNo1),format2,newPinBlock)) < 0)
						{
							// ʹ�þɰ汾��Կ
							if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&symmetricKeyDB1))
							{
								if ((ret = UnionHsmCmdCA(strlen(psymmetricKeyValue1->oldKeyValue),psymmetricKeyValue1->oldKeyValue,
									strlen(psymmetricKeyValue2->keyValue),psymmetricKeyValue2->keyValue,
									format1,pinBlock,accNo1,strlen(accNo1),format2,newPinBlock)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdCA old key ret = [%d]!\n",ret);
									return(ret);
								}
							}
							else
							{
								UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdCA current key ret = [%d]!\n",ret);
								return(ret);
							}
						}
						 break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE142:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
				break;
			}
			case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
			default:
				UnionUserErrLog("in UnionDealServiceCodeE142:: symmetricKeyDB1.algorithmID[%d] error!\n",symmetricKeyDB1.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
	}//TPKתLMK 
	else if (isLmk1 == 0 && isLmk2 == 1 && symmetricKeyDB1.keyType == conTPK)
        {
                switch(symmetricKeyDB1.algorithmID)
                {
                        case    conSymmetricAlgorithmIDOfDES:   // DES�㷨
                                switch(phsmGroupRec->hsmCmdVersionID)
                                {
                                        case conHsmCmdVerRacalStandardHsmCmd:
                                        case conHsmCmdVerSJL06StandardHsmCmd:
                                                if ((ret = UnionHsmCmdJC(strlen(psymmetricKeyValue1->keyValue),psymmetricKeyValue1->keyValue,
                                                       format1, pinBlock, accNo1,strlen(accNo1),newPinBlock)) < 0)
                                                {
                                                        // ʹ�þɰ汾��Կ
                                                        if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) &&
                                                                UnionOldVersionSymmetricKeyIsUsed(&symmetricKeyDB1))
                                                        {
                                                                if ((ret = UnionHsmCmdJC(strlen(psymmetricKeyValue1->oldKeyValue),psymmetricKeyValue1->oldKeyValue,
                                                       format1, pinBlock, accNo1,strlen(accNo1),newPinBlock)) < 0)
                                                                {
                                                                        UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdJC old key ret = [%d]!\n",ret);
                                                                        return(ret);
                                                                }
                                                        }
                                                        else
                                                        {
                                                                UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdJC current key ret = [%d]!\n",ret);
                                                                return(ret);
                                                        }
                                                }
                                                 break;
                                        default:
                                                UnionUserErrLog("in UnionDealServiceCodeE142:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
                                                UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                                                return(errCodeParameter);
                                }
                                break;
                        case    conSymmetricAlgorithmIDOfSM4:   // SM4�㷨
                        default:
                                UnionUserErrLog("in UnionDealServiceCodeE142:: symmetricKeyDB1.algorithmID[%d] error!\n",symmetricKeyDB1.algorithmID);
                                return(errCodeEsscMDL_InvalidAlgorithmID);
                }
        }
	//add by huangh 2016.04.14
	//ZPKתZEK
	else if ((isLmk2 == 0) && (isLmk1 == 0) && (symmetricKeyDB1.keyType == conZPK) && (symmetricKeyDB2.keyType == conZEK))
	{
		UnionTranslateHsmKeyTypeString(symmetricKeyDB2.keyType,keyType);
		if ((ret = UnionHsmCmdW8_Base(symmetricKeyDB1.algorithmID+1,symmetricKeyDB2.algorithmID+1,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB1.keyLen)+1,NULL,psymmetricKeyValue1->keyValue,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB2.keyLen)+1,keyType,NULL,psymmetricKeyValue2->keyValue,format1,format2,pinBlock,accNo1,accNo2,newPinBlock,sizeof(newPinBlock))) < 0)
		{
			// ʹ�þɰ汾��Կ
			if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
				UnionOldVersionSymmetricKeyIsUsed(&symmetricKeyDB1))
			{	
				if ((ret = UnionHsmCmdW8_Base(symmetricKeyDB1.algorithmID+1,symmetricKeyDB2.algorithmID+1,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB1.keyLen)+1,NULL,psymmetricKeyValue1->oldKeyValue,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB2.keyLen)+1,keyType,NULL,psymmetricKeyValue2->keyValue,format1,format2,pinBlock,accNo1,accNo2,newPinBlock,sizeof(newPinBlock))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdW8 old key ret = [%d]!\n",ret);
					return(ret);
				}
			}
			else
			{
				UnionUserErrLog("in UnionDealServiceCodeE142:: UnionHsmCmdW8 current key ret = [%d]!\n",ret);
				return(ret);
			}
		}
	}
	//add by huangh 2016.04.14 end
	else
	{
		UnionUserErrLog("in UnionDealServiceCodeE142:: not support this fundtion!\n");
		return(errCodeEsscMDLKeyOperationNotPermitted);
	}
	
	if (isLmk1 == 0 && isLmk2 == 0 && symmetricKeyDB1.keyType == conZPK && symmetricKeyDB2.keyType == conPVK)
	{
		// ����PIN����
    		if ((ret = UnionSetResponseXMLPackageValue("body/pinBlock",accNo2PinBlock)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE142:: UnionSetResponseXMLPackageValue pinBlock[%s]!\n",accNo2PinBlock);
                        return(ret);
                }
		// ����PIN Offset
		if ((ret = UnionSetResponseXMLPackageValue("body/pinOffset",pinOffset)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE142:: UnionSetResponseXMLPackageValue pinOffset[%s]!\n",pinOffset);
			return(ret);
		}

		if (checkFlag != 1)
		{
			if ((ret = UnionSetResponseXMLPackageValue("body/checkData",checkData)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE142:: UnionSetResponseXMLPackageValue checkData[%s]!\n",checkData);
				return(ret);
			}
		}
		if (decimalFlag != 1)
		{
			if ((ret = UnionSetResponseXMLPackageValue("body/decimalTable",decimalTable)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE142:: UnionSetResponseXMLPackageValue decimalTable[%s]!\n",decimalTable);
				return(ret);
			}
		}
	}
	else
	{
		// ����PIN����
		if ((ret = UnionSetResponseXMLPackageValue("body/pinBlock",newPinBlock)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE142:: UnionSetResponseXMLPackageValue pinBlock[%s]!\n",newPinBlock);
			return(ret);
		}
	}

	return(0);
}
