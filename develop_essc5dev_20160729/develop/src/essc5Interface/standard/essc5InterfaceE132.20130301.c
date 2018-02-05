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
#include "symmetricKeyDB.h"
#include "unionHsmCmdVersion.h"
#include "3DesRacalSyntaxRules.h"
#include "unionREC.h"

/***************************************
�������:	E132
������:		����PIN Offset
��������:	����PIN Offset,��IBM��ʽ����һ��PIN��PIN Offset��
		��Ӧ�ļ��ܻ�ָ��Ϊ��JE��DG
***************************************/
int UnionDealServiceCodeE132(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				accNoLen = 0;
	int				pinLen = 0;
	int				minPinLen = 0;
	int				checkFlag = 0;
	int				result=-1;
	char				protectFlag[8];
	char				pinOffset[32];
	char				accNo[48];
	char				pinBlock[32+1];
	char				pinByLmk[32+1];
	char				keyName[128+1];
	char				keyNameOfZPK[128+1];
	char				tmpBuf[128+1];
	char				checkData[12+1];
	char				checkPINByZpk[1+1];
	char				pinFormat[2+1];
	char				ruleOfPIN[1+1];
	char 				resultOfCheck[1+1];
	char				*ptr = NULL;
	char				decimalTable[128];

	TUnionSymmetricKeyDB		pvkKeyDB;
	PUnionSymmetricKeyValue		ppvkKeyValue = NULL;
	TUnionSymmetricKeyDB		zpkKeyDB;
	PUnionSymmetricKeyValue		pzpkKeyValue = NULL;
	
	// PVK��Կ����
	//memset(keyName,0,sizeof(keyName));
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE132:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);
	if (strlen(keyName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE132:: keyName can not be null!\n");
		UnionSetResponseRemark("��Կ���Ʋ���Ϊ��");
		return(errCodeParameter);
	}

	// ��ȡPVK��Կ
	memset(&pvkKeyDB,0,sizeof(pvkKeyDB));
	if ((ret = UnionReadSymmetricKeyDBRec(keyName,1,&pvkKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE132:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}
	if (pvkKeyDB.keyType != conPVK)
	{
		UnionUserErrLog("in UnionDealServiceCodeE132:: is not pvk [%s]!\n",keyName);
		return(errCodeEsscMDL_WrongUsageOfKey);
	}
	// ��ȡ��Կֵ
	if ((ppvkKeyValue = UnionGetSymmetricKeyValue(&pvkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE132:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(errCodeParameter);
	}

	// PIN����
	memset(pinBlock,0,sizeof(pinBlock));
	if ((pinLen = UnionReadRequestXMLPackageValue("body/pinBlock",pinBlock,sizeof(pinBlock))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE132:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinBlock");
		return(pinLen);
	}
	else
        {
		UnionFilterHeadAndTailBlank(pinBlock);
		if(strlen(pinBlock) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE132:: pinBlock can not be null!\n");
                	UnionSetResponseRemark("PIN���Ĳ���Ϊ��!");
                	return(errCodeParameter);
		}
                if (!UnionIsBCDStr(pinBlock))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE132:: pinBlock[%s] is error!\n",pinBlock);
                        UnionSetResponseRemark("PIN���ķǷ�,����Ϊʮ��������");
                        return(errCodeHsmCmdMDL_PinByLmk0203);
                }
        }

	// ��С��PIN����
	//memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/minPinLen",tmpBuf,sizeof(tmpBuf))) <= 0)
		minPinLen = UnionGetIBMMinPINLength();
	else
	{
		tmpBuf[ret] = 0;
		UnionFilterHeadAndTailBlank(tmpBuf);
		if (!UnionIsDigitString(tmpBuf))
        	{
                	UnionUserErrLog("in UnionDealServiceCodeE132:: minPinLen[%s] is error!\n",tmpBuf);
                        UnionSetResponseRemark("��С��PIN����[%s]�Ƿ�,����Ϊ����",tmpBuf);
                	return(errCodeParameter);
        	}
		minPinLen = atoi(tmpBuf);
	}

	// PIN������ʽ
	// 1:LMK�Ա���	2:ZPK����
	memset(protectFlag,0,sizeof(protectFlag));
	if ((ret = UnionReadRequestXMLPackageValue("body/protectFlag",protectFlag,sizeof(protectFlag))) <= 0)
		protectFlag[0] = '1';
	if (atoi(protectFlag) != 1 && atoi(protectFlag) != 2)
	{
	 	UnionUserErrLog("in UnionDealServiceCodeE132:: protectFlag[%s] is error!\n",protectFlag);
                UnionSetResponseRemark(" PIN������ʽ[%s],����Ϊ1����2",protectFlag);
                return(errCodeParameter);     
	}
		
	// �˺�
	if ((accNoLen = UnionReadRequestXMLPackageValue("body/accNo",accNo,sizeof(accNo))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE132:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
		return(accNoLen);
	}
	accNo[accNoLen] = 0;	
	// ����˺�
	accNoLen = UnionFilterHeadAndTailBlank(accNo);
	if(accNoLen == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE132:: accNo can not be null!\n");
                UnionSetResponseRemark("�˺Ų���Ϊ��!");
                return(errCodeParameter);
	}
	if (!UnionIsDigitString(accNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE132:: accNo[%s] is error!\n",accNo);
		UnionSetResponseRemark("�˺ŷǷ�[%s],����Ϊ����",accNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}
	
	memset(pinByLmk,0,sizeof(pinByLmk));
	if (protectFlag[0] == '2')
	{
		//memset(keyNameOfZPK,0,sizeof(keyNameOfZPK));
		if ((ret = UnionReadRequestXMLPackageValue("body/keyNameOfZPK",keyNameOfZPK,sizeof(keyNameOfZPK))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE132:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyNameOfZPK");
			return(ret);
		}
		keyNameOfZPK[ret] = 0;
		
		// ��ȡZPK��Կ
		memset(&zpkKeyDB,0,sizeof(zpkKeyDB));
		if ((ret = UnionReadSymmetricKeyDBRec(keyNameOfZPK,1,&zpkKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE132:: UnionReadSymmetricKeyDBRec[%s]!\n",keyNameOfZPK);
			return(ret);
		}

		if (zpkKeyDB.keyType != conZPK)
		{
			UnionUserErrLog("in UnionDealServiceCodeE132:: is not zpk [%s]!\n",keyNameOfZPK);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}
		// ��ȡ��Կֵ
		if ((pzpkKeyValue = UnionGetSymmetricKeyValue(&zpkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE132:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("��ȡ��Կֵʧ��");
			return(errCodeParameter);
		}
		
		memset(pinByLmk,0,sizeof(pinByLmk));
		switch(zpkKeyDB.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
				// ��ZPK���ܵ�PINת��ΪLMK����
				switch(phsmGroupRec->hsmCmdVersionID)
        			{
                			case conHsmCmdVerRacalStandardHsmCmd:
                			case conHsmCmdVerSJL06StandardHsmCmd:
						if ((pinLen = UnionHsmCmdJE(UnionConvertSymmetricKeyKeyLen(zpkKeyDB.keyLen),
							pzpkKeyValue->keyValue,"01",pinBlock,accNo,accNoLen,pinByLmk)) < 0)
						{
							// ʹ�þɰ汾��Կ
							if ((pinLen <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&zpkKeyDB))
							{
								if ((pinLen = UnionHsmCmdJE(UnionConvertSymmetricKeyKeyLen(zpkKeyDB.keyLen),
									pzpkKeyValue->oldKeyValue,"01",pinBlock,accNo,accNoLen,pinByLmk)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE132:: old UnionHsmCmdJE!\n");
									return(pinLen);
								}
							}
							else
							{
								UnionUserErrLog("in UnionDealServiceCodeE132:: current UnionHsmCmdJE!\n");
								return(pinLen);
							}
						}
						break;
                			default:
                        			UnionUserErrLog("in UnionDealServiceCodeE132:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        			UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                        			return(errCodeParameter);
        			}
				break;
			case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
				//��ZPK���ܵ�PINת��ΪLMK����
                                switch(phsmGroupRec->hsmCmdVersionID)
                                {
                                        case conHsmCmdVerRacalStandardHsmCmd:
                                        case conHsmCmdVerSJL06StandardHsmCmd:
                                                if((pinLen = UnionHsmCmdWX(1,pzpkKeyValue->keyValue,pinBlock,"01",accNo,strlen(accNo),pinByLmk,sizeof(pinByLmk))) < 0)
                                                {
                                                        //ʹ�þɰ汾��Կ
                                                        if((pinLen <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&zpkKeyDB))
                                                        {
                                                                if((pinLen = UnionHsmCmdWX(1,pzpkKeyValue->oldKeyValue,pinBlock,"01",accNo,strlen(accNo),pinByLmk,sizeof(pinByLmk))) <0)
                                                                {
                                                                        UnionUserErrLog("in UnionDealServiceE132:: UnionHsmCmdWX old key ret = [%d]!\n",pinLen);
                                                                        return(pinLen);
                                                                }
                                                        }
                                                        else
                                                        {
                                                                UnionUserErrLog("in UnionDealServiceCodeE132:: UnionHsmCmdWX current key ret = [%d]!\n",pinLen);
                                                                return(pinLen);
                                                        }
                                                }
                                                break;
                                        default:
                                                UnionUserErrLog("in UnionDealServiceCodeE132:: phsmGroupRec->hsmCmdVersionID[%d]!\n",phsmGroupRec->hsmCmdVersionID);
                                                UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                                                return(errCodeParameter);
                                }
                                break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE132:: zpkKeyDB.algorithmID[%d] error!\n",zpkKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
	}
	else
	{
		strcpy(pinByLmk,pinBlock);
	}
	
	// �����ʶ
	//memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/checkFlag",tmpBuf,sizeof(tmpBuf))) <= 0)
		checkFlag = 0;
	else
	{
		tmpBuf[ret] = 0;
		if (atoi(tmpBuf) != 0 && atoi(tmpBuf) != 1 && atoi(tmpBuf) != 2 && atoi(tmpBuf) != 3)
		{
			UnionUserErrLog("in UnionDealServiceCodeE132:: checkFlag[%s] error!\n",tmpBuf);
                        UnionSetResponseRemark("�����ʶ[%s]��Ч",tmpBuf);
			return(errCodeParameter);
		}
		checkFlag = atoi(tmpBuf);
	}

	memset(checkData,0,sizeof(checkData));
	/*
	// add by zhouxw 20160225 begin
	if(strcasecmp(UnionGetIDOfCustomization(), "CBHB") == 0)
	{
		if((ptr = UnionReadStringTypeRECVar("checkData")) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE132:: UnionReadStringTypeRECVar[%s]\n", "checkData");
			return(errCodeRECMDL_VarNotExists);
		}
		else
			strcpy(checkData, ptr);
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
	// add by zhouxw 20160225 end
	*/
	if (checkFlag == 0)	// ϵͳ�趨
		strcpy(checkData,UnionGetIBMUserDefinedData());
	else if (checkFlag == 1)	// �û�����
	{
		// modify begin by zhouxw 20160506 
		// PINУ������
		//if ((ret = UnionReadRequestXMLPackageValue("body/checkData",checkData,sizeof(checkData))) < 0)
		//{
		//	UnionUserErrLog("in UnionDealServiceCodeE132:: UnionReadRequestXMLPackageValue[%s]!\n","body/checkData");
		//	return(ret);
		//}
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
			{
				checkData[ret] = 0;
				UnionFilterHeadAndTailBlank(checkData);
				if(strlen(checkData) != 12)
				{
					UnionUserErrLog("in UnionDealServiceCodeE132:: checkData[%s]!\n", checkData);
					UnionSetResponseRemark("pinУ������[checkData]����Ϊ12λ");
					return(errCodeParameter);
				}
			}
		}
		else
		{
			// PINУ������
			if ((ret = UnionReadRequestXMLPackageValue("body/checkData",checkData,sizeof(checkData))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE132:: UnionReadRequestXMLPackageValue[%s]!\n","body/checkData");
				return(ret);
			}
			else if(ret == 0)
			{	
				UnionUserErrLog("in UnionDealServiceCodeE132:: UnionReadRequestXMLPackageValue[%s]!\n","body/checkData");
				UnionSetResponseRemark("У������Ϊ��");
				return(errCodeParameter);
			}
			else
				checkData[ret] = 0;
		}
		// modify end 
	}
	else if (checkFlag == 2)	// ϵͳ�������
		UnionGetIBMPinCheckData(checkData);
	else if (checkFlag == 3)	// �û����ƻ�
	{
		if ((ret = UnionForm12LenAccountNumber(accNo,accNoLen,tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE132:: UnionForm12LenAccountNumber[%s]!\n",accNo);
			return(ret);
		}
		tmpBuf[ret] = 0;
		snprintf(checkData,sizeof(checkData),"%s",UnionGetIBMUserDefinedDataByAccNo(tmpBuf));
	}
	else
		strcpy(checkData,UnionGetIBMUserDefinedData());

	// add by zhouxw 20160506 begin
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
		// add by zhouxw 20160225 begin
		strcpy(decimalTable, UnionGetIBMDecimalizationTable());
		// add by zhouxw 20160225 end
	}
	// add by zhouxw 20160506 end

	memset(pinOffset,0,sizeof(pinOffset));
	switch(pvkKeyDB.algorithmID)
	{
		case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					// modify leipp 2015317 begin	
					if ((ret = UnionHsmCmdDE(minPinLen,checkData,
						decimalTable,pinLen,pinByLmk,UnionConvertSymmetricKeyKeyLen(pvkKeyDB.keyLen),
						ppvkKeyValue->keyValue,accNo,accNoLen,pinOffset)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE132:: UnionHsmCmdDE!\n");
						return(ret);
					}
					// modify leipp end
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE132:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
					return(errCodeParameter);
			}
			break;
		case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
                        switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmd8E(minPinLen,checkData,
						decimalTable,pinLen,pinByLmk,
						ppvkKeyValue->keyValue,accNo,accNoLen,pinOffset)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE132:: UnionHsmCmd8E!\n");
						return(ret);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE132:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
					return(errCodeParameter);
			}
                        break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE132:: pvkKeyDB.algorithmID[%d] error!\n",pvkKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	
	if (strcasecmp(UnionGetIDOfCustomization(),"UnionPay") == 0)
		strcat(pinOffset,"FFFF");	

	//add begin by lusj  20151124 ��֦����������

	//	checkPINByZpk  PIN�������жϱ�ʶ	0�����ж� 1���ж�
	memset(checkPINByZpk,0,sizeof(checkPINByZpk));
	if ((ret = UnionReadRequestXMLPackageValue("body/checkPINByZpk",checkPINByZpk,sizeof(checkPINByZpk))) < 0)
		checkPINByZpk[0] = '0';
	if (atoi(checkPINByZpk) != 1 && atoi(checkPINByZpk) != 0)
	{
	 	UnionUserErrLog("in UnionDealServiceCodeE132:: checkPINByZpk[%s] is error!\n",checkPINByZpk);
                UnionSetResponseRemark(" PIN����Կ�жϱ�ʶ[%s],����Ϊ0����1",checkPINByZpk);
                return(errCodeParameter);     
	}

	//��������Կ���ж�
	if(atoi(checkPINByZpk) == 1)
	{
		//pin��ʽ
		memset(pinFormat,0,sizeof(pinFormat));
		if ((ret = UnionReadRequestXMLPackageValue("body/pinFormat",pinFormat,sizeof(pinFormat))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE132:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinFormat");
			return(ret);
		}
	
		//���������
		memset(ruleOfPIN,0,sizeof(ruleOfPIN));
		if ((ret = UnionReadRequestXMLPackageValue("body/ruleOfPIN",ruleOfPIN,sizeof(ruleOfPIN))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE132:: UnionReadRequestXMLPackageValue[%s]!\n","body/ruleOfPIN");
			return(ret);
		}

		if (atoi(ruleOfPIN) != 1 && atoi(ruleOfPIN) != 0 && atoi(ruleOfPIN) != 2)
		{
	 			UnionUserErrLog("in UnionDealServiceCodeE132:: ruleOfPIN[%s] is error!\n",ruleOfPIN);
                UnionSetResponseRemark(" PIN����Կ����[%s],����Ϊ0��1����2",ruleOfPIN);
                return(errCodeParameter);     
		}


		memset(resultOfCheck,0,sizeof(resultOfCheck));
		//�жϲ���
		switch(zpkKeyDB.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
				switch(phsmGroupRec->hsmCmdVersionID)
        			{
               			case conHsmCmdVerRacalStandardHsmCmd:
                		case conHsmCmdVerSJL06StandardHsmCmd:
							if ((result = UnionHsmCmdZY(1,"",pzpkKeyValue->keyValue,zpkKeyDB.checkValue,pinFormat,pinBlock,accNo,atoi(ruleOfPIN))) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE132:: UnionHsmCmdZY!\n");
								return(result);
							}
							break;
                		default:
                        		UnionUserErrLog("in UnionDealServiceCodeE132:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        		UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                        		return(errCodeParameter);
        			}
				break;
			  case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
                    switch(phsmGroupRec->hsmCmdVersionID)
                   {
                    	case conHsmCmdVerRacalStandardHsmCmd:
                     	case conHsmCmdVerSJL06StandardHsmCmd:
							if ((result = UnionHsmCmdZY(2,"",pzpkKeyValue->keyValue,zpkKeyDB.checkValue,pinFormat,pinBlock,accNo,atoi(ruleOfPIN))) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE132:: UnionHsmCmdZY!\n");
								return(result);
							}                
                              break;
                        default:
                       			UnionUserErrLog("in UnionDealServiceCodeE132:: phsmGroupRec->hsmCmdVersionID[%d]!\n",phsmGroupRec->hsmCmdVersionID);
                        		UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                        		return(errCodeParameter);
                   }
             break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE132:: zpkKeyDB.algorithmID[%d] error!\n",zpkKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
		
		sprintf(resultOfCheck,"%d",result);
		// ����resultOfCheck
		if ((ret = UnionSetResponseXMLPackageValue("body/resultOfCheck",resultOfCheck)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE132:: UnionSetResponseXMLPackageValue resultOfCheck[%s]!\n",resultOfCheck);
			return(ret);
		}
	}
	//add end by lusj 


	// ����PIN Offset
	if ((ret = UnionSetResponseXMLPackageValue("body/pinOffset",pinOffset)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE132:: UnionSetResponseXMLPackageValue pinOffset[%s]!\n",pinOffset);
		return(ret);
	}
	
	// ����checkData
	if ((ret = UnionSetResponseXMLPackageValue("body/checkData",checkData)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE132:: UnionSetResponseXMLPackageValue checkData[%s]!\n",checkData);
		return(ret);
	}
	
	if(strcasecmp(UnionGetIDOfCustomization(), "CBHB") == 0)
	{
		if((ret = UnionSetResponseXMLPackageValue("body/decimalTable", decimalTable)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE132:: UnionSetResponseXMLPackageValue decimalTable[%s]!\n", decimalTable);
			return(ret);
		}
	}
	
	return(0);
}

