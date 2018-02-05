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
#include "unionREC.h"

/***************************************
�������:	E143
������:		��֤PIN
��������:	��֤PIN
***************************************/
int UnionDealServiceCodeE143(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				first_result=-1;		//add by lusj  20151207
	int				second_result=-1;		//add by lusj  20151207
	int				mode = 1;
	int				checkFlag = 0;
	int				pinLen = 0;
	char				tmpBuf[32];
	char				accNo[64];
	char				pinBlock[64];
	char				pinBlockByLMK[64];
	char				keyName[160];
	char				keyNameOfPVK[160];
	char				format[32];
	char				pinOffset[32];
	char				PVV[32];
	char				checkData[32];
	char				pinByLmk[32+1];
	char				tPinOffset[32+1];
	TUnionSymmetricKeyDB		zpkKeyDB;
	PUnionSymmetricKeyValue		pzpkKeyValue = NULL;
	TUnionSymmetricKeyDB		pvkKeyDB;
	PUnionSymmetricKeyValue		ppvkKeyValue = NULL;
	char				tmpZpk[64];
	char				tmpCheckValue[32];
	char				tmpPinBlock[64];
	
	char				decimalTable[32];
	char				*ptr = NULL;
	// PIN����
	if ((ret = UnionReadRequestXMLPackageValue("body/pinBlock",pinBlock,sizeof(pinBlock))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE143:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinBlock");
		return(ret);
	}
	else
        {
		pinBlock[ret] = 0;
		UnionFilterHeadAndTailBlank(pinBlock);
                if (strlen(pinBlock) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE143:: pinBlock can not be null!\n");
                        UnionSetResponseRemark("PIN���Ĳ���Ϊ��!");
                        return(errCodeParameter);
                }
                if (!UnionIsBCDStr(pinBlock))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE143:: pinBlock[%s] is error!\n",pinBlock);
                        UnionSetResponseRemark("PIN����[%s]�Ƿ�,����Ϊʮ��������",pinBlock);
                        return(errCodeParameter);
                }
        }

	// PIN��ʽ
	if ((ret = UnionReadRequestXMLPackageValue("body/format",format,sizeof(format))) <= 0)
		strcpy(format,"01");
	else
		format[ret] = 0;
		
	// �˺�
	if ((ret = UnionReadRequestXMLPackageValue("body/accNo",accNo,sizeof(accNo))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE143:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
		return(ret);
	}
	else
		accNo[ret] = 0;

	// ����˺�
	UnionFilterHeadAndTailBlank(accNo);
        if (strlen(accNo) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE143:: accNo can not be null!\n");
                UnionSetResponseRemark("�˺Ų���Ϊ��!");
                return(errCodeParameter);
        }
	if (!UnionIsDigitString(accNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE143:: accNo[%s] is error!\n",accNo);
		UnionSetResponseRemark("�˺ŷǷ�[%s],����Ϊ����",accNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}

	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE143:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;

	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1,&zpkKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE143:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
		return(ret);
	}
	if (zpkKeyDB.keyType != conZPK)
	{
		UnionUserErrLog("in UnionDealServiceCodeE143:: is not zpk [%s]!\n",keyName);
		return(errCodeEsscMDL_WrongUsageOfKey);
	}
	// ��ȡ��Կֵ
	if ((pzpkKeyValue = UnionGetSymmetricKeyValue(&zpkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE143:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(errCodeParameter);
	}

	// ģʽ
	// 1:IBM PinOffset��֤
	// 2:PVV��ʽ��֤
	// 3:LMK������֤
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE143:: UnionReadRequestXMLPackageValue[%s]!\n","body/mode");
		return(ret);
	}
	else
		tmpBuf[ret] = 0;
	mode = atoi(tmpBuf);

	switch(mode)
	{
		case	1:	// IBM PinOffset��֤
			// ��ȡPVK
			if ((ret = UnionReadRequestXMLPackageValue("body/keyNameOfPVK",keyNameOfPVK,sizeof(keyNameOfPVK))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE143:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyNameOfPVK");
				return(ret);
			}
			keyNameOfPVK[ret] = 0;

			if ((ret =  UnionReadSymmetricKeyDBRec(keyNameOfPVK,1,&pvkKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE143:: UnionReadSymmetricKeyDBRec keyNameOfPVK[%s]!\n",keyNameOfPVK);
				return(ret);
			}
			if (pvkKeyDB.keyType != conPVK)
			{
				UnionUserErrLog("in UnionDealServiceCodeE143:: is not pvk [%s]!\n",keyNameOfPVK);
				return(errCodeEsscMDL_WrongUsageOfKey);
			}
			// ��ȡ��Կֵ
			if ((ppvkKeyValue = UnionGetSymmetricKeyValue(&pvkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
			{
				UnionUserErrLog("in UnionDealServiceCodeE142:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
				UnionSetResponseRemark("��ȡ��Կֵʧ��");
				return(errCodeParameter);
			}
			
			// ��ȡPIN Offset
			if ((ret = UnionReadRequestXMLPackageValue("body/pinOffset",pinOffset,sizeof(pinOffset))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE143:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinOffset");
				return(ret);
			}
			else if (ret == 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE143:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinOffset");
				return(errCodeParameter);
			}

			// �����ʶ
			if ((ret = UnionReadRequestXMLPackageValue("body/checkFlag",tmpBuf,sizeof(tmpBuf))) < 0)
				checkFlag = 0;
			else
				checkFlag = atoi(tmpBuf);
			
			if (checkFlag == 0)	// ϵͳ�趨
				snprintf(checkData,sizeof(checkData),"%s",UnionGetIBMUserDefinedData());
			else if (checkFlag == 1)	// �û�����
			{
				// modify by zhouxw 20150506 begin
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
							UnionUserErrLog("in UnionDealServiceCodeE143:: checkData[%s]!\n", checkData);
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
						UnionUserErrLog("in UnionDealServiceCodeE142:: UnionReadRequestXMLPackageValue[%s]!\n","body/checkData");
						return(ret);
					}
				}
				// modify by zhouxw 20160506 end
			}
			else
				snprintf(checkData,sizeof(checkData),"%s",UnionGetIBMUserDefinedData());
			
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
				strcpy(decimalTable, UnionGetIBMDecimalizationTable());
			}
			// add by zhouxw 20160506 end
			
			switch(zpkKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case	conHsmCmdVerRacalStandardHsmCmd:
						case	conHsmCmdVerSJL06StandardHsmCmd:
							// add by chenqy 20151129
							//DESZPK���ܵ�pinBlockתSM4ZPK���ܺ���У��(PVKΪSM4��Կ)
							if(pvkKeyDB.algorithmID == conSymmetricAlgorithmIDOfSM4)
							{
								if ((ret = UnionHsmCmdWI("0",conZPK,NULL,tmpZpk,NULL,tmpCheckValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE143:: UnionHsmCmdWI!\n");
									return(ret);
								}
	
								if((ret = UnionHsmCmdW8(1, 2, 2, NULL, pzpkKeyValue->keyValue, 2, NULL, tmpZpk, format, format, pinBlock, accNo, accNo, tmpPinBlock, sizeof(tmpPinBlock))) < 0)
								{
									if((ret <= errCodeOffsetOfHsmReturnCodeMDL) &&
                                                                		UnionOldVersionSymmetricKeyIsUsed(&zpkKeyDB))
									{
										if((ret = UnionHsmCmdW8(1, 2, 2, NULL, pzpkKeyValue->oldKeyValue, 2, NULL, tmpZpk, format, format, pinBlock, accNo, accNo, tmpPinBlock, sizeof(tmpPinBlock))) < 0)
										{
											UnionUserErrLog("in UnionDealServiceE143:: UnionHsmCmdW8 old version return[%d]\n", ret);
											return(ret);
										}
									}
									else
									{
										UnionUserErrLog("in UnionDealServiceE143:: UnionHsmCmdW8 return[%d]\n", ret);
										return(ret);
									}
									
								}
								strcpy(pzpkKeyValue->keyValue, tmpZpk);
								strcpy(pinBlock, tmpPinBlock);
								goto ChkPinOffsetSM4;
							}	
							// add end
ChkPinOffsetDES:
							if ((first_result=ret = UnionHsmCmdEA(pinBlock,UnionGetIBMMinPINLength(),checkData,decimalTable,
								pinOffset,UnionConvertSymmetricKeyKeyLen(zpkKeyDB.keyLen),pzpkKeyValue->keyValue,
								UnionConvertSymmetricKeyKeyLen(pvkKeyDB.keyLen),ppvkKeyValue->keyValue,format,accNo,strlen(accNo))) >= 0)
								break;
						//modify by lusj 20151210 ,ԭ��: ������ԿֵΪ�գ�������У��		
							// ʹ�þɰ汾��Կ
							if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&zpkKeyDB) && (strlen(pzpkKeyValue->oldKeyValue)!= 0) )
							{
								if ((second_result=ret = UnionHsmCmdEA(pinBlock,UnionGetIBMMinPINLength(),checkData,decimalTable,
									pinOffset,UnionConvertSymmetricKeyKeyLen(zpkKeyDB.keyLen),pzpkKeyValue->oldKeyValue,
									UnionConvertSymmetricKeyKeyLen(pvkKeyDB.keyLen),ppvkKeyValue->keyValue,format,accNo,strlen(accNo))) >= 0)
							break;
							}

							// ʹ�þɰ汾��Կ
							if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&pvkKeyDB)&& (strlen(ppvkKeyValue->oldKeyValue)!= 0) )
							{
							if ((first_result=ret = UnionHsmCmdEA(pinBlock,UnionGetIBMMinPINLength(),checkData,decimalTable,
								pinOffset,UnionConvertSymmetricKeyKeyLen(zpkKeyDB.keyLen),pzpkKeyValue->keyValue,
								UnionConvertSymmetricKeyKeyLen(pvkKeyDB.keyLen),ppvkKeyValue->oldKeyValue,format,accNo,strlen(accNo))) >= 0)
							break;
							}
							
							// ʹ�þɰ汾��Կ
							if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&zpkKeyDB) &&
								UnionOldVersionSymmetricKeyIsUsed(&pvkKeyDB) && (strlen(pzpkKeyValue->oldKeyValue)!= 0) && (strlen(ppvkKeyValue->oldKeyValue)!= 0))
							{
								if ((second_result=ret = UnionHsmCmdEA(pinBlock,UnionGetIBMMinPINLength(),checkData,decimalTable,
									pinOffset,UnionConvertSymmetricKeyKeyLen(zpkKeyDB.keyLen),pzpkKeyValue->oldKeyValue,
									UnionConvertSymmetricKeyKeyLen(pvkKeyDB.keyLen),ppvkKeyValue->oldKeyValue,format,accNo,strlen(accNo))) >= 0)
								break;
							}

							//add begin  by lusj 20151207 

							/*ԭ��:
							1��δ����Կ����ǰ���������PinBlockΪ����pin���ܵ�PinBlockʱ��
							 EAָ���ܽ��ܹ�������У�鲻������-1001�����ϲ����Ϊ�������ݴ��󡣣��������
							2��������Կ���º󣬵������PinBlockΪ����pin���ܵ�PinBlockʱ��
							EAָ���������У�飬��һ�α�-1001���ڶ���Ϊ-1020��������-1020���ϲ����Ϊ��Կδ���£�������Ӧ�ñ�-1001��	
							*/
							if((first_result <= errCodeOffsetOfHsmReturnCodeMDL)&&(second_result <= errCodeOffsetOfHsmReturnCodeMDL))
								ret= first_result;	
							//end
							
							UnionUserErrLog("in UnionDealServiceCodeE143:: UnionHsmCmdEA ret = [%d]!\n",ret);
							return(ret);
						default:
							UnionUserErrLog("in UnionDealServiceCodeE143:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
					//add by yuanwb 20150410
					//��ZPK���ܵ�PINת��ΪLMK����
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							//add by zhouxw 20150722
							//SM4ZPK���ܵ�pinBlockתDESZPK���ܺ���У��(PVKΪDES��Կ)
							if(pvkKeyDB.algorithmID == conSymmetricAlgorithmIDOfDES)
							{
								if((ret = UnionHsmCmdA0(0, conZPK, con128BitsDesKey, NULL, tmpZpk, NULL, tmpCheckValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceE143:: UnionHsmCmdA0 return[%d]\n", ret);
									return(ret);
								}
								if((ret = UnionHsmCmdW8(2, 1, 2, NULL, pzpkKeyValue->keyValue, 2, NULL, tmpZpk, format, format, pinBlock, accNo, accNo, tmpPinBlock, sizeof(tmpPinBlock))) < 0)
								{
									if((ret <= errCodeOffsetOfHsmReturnCodeMDL) &&
                                                                		UnionOldVersionSymmetricKeyIsUsed(&zpkKeyDB))
									{
										if((ret = UnionHsmCmdW8(2, 1, 2, NULL, pzpkKeyValue->oldKeyValue, 2, NULL, tmpZpk, format, format, pinBlock, accNo, accNo, tmpPinBlock, sizeof(tmpPinBlock))) < 0)
										{
											UnionUserErrLog("in UnionDealServiceE143:: UnionHsmCmdW8 old version return[%d]\n", ret);
											return(ret);
										}
									}
									else
									{
										UnionUserErrLog("in UnionDealServiceE143:: UnionHsmCmdW8 return[%d]\n", ret);
										return(ret);
									}
									
								}
								strcpy(pzpkKeyValue->keyValue, tmpZpk);
								strcpy(pinBlock, tmpPinBlock);
								goto ChkPinOffsetDES;
							}
							//add by zhouxw end 
ChkPinOffsetSM4:
							memset(pinByLmk,0,sizeof(pinByLmk));
							if((pinLen = UnionHsmCmdWX(1,pzpkKeyValue->keyValue,pinBlock,"01",accNo,strlen(accNo),pinByLmk,sizeof(pinByLmk))) < 0)
							{
								//ʹ�þɰ汾��Կ
								if((pinLen <= errCodeOffsetOfHsmReturnCodeMDL) &&UnionOldVersionSymmetricKeyIsUsed(&zpkKeyDB))
								{
									if((pinLen = UnionHsmCmdWX(1,pzpkKeyValue->oldKeyValue,pinBlock,"01",accNo,strlen(accNo),pinByLmk,sizeof(pinByLmk))) <0)
									{
										UnionUserErrLog("in UnionDealServiceE143:: UnionHsmCmdWX old key ret = [%d]!\n",pinLen);
										return(pinLen);
									}
								}
								else
								{
									UnionUserErrLog("in UnionDealServiceCodeE143:: UnionHsmCmdWX current key ret = [%d]!\n",pinLen);
									return(pinLen);
								}
							}
							memset(tPinOffset,0,sizeof(tPinOffset));
							if ((ret = UnionHsmCmd8E(UnionGetIBMMinPINLength(),checkData,decimalTable,pinLen,pinByLmk,ppvkKeyValue->keyValue,accNo,strlen(accNo),tPinOffset)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE143:: UnionHsmCmd8E!\n");
								return(ret);
							}
							if(strncmp(pinOffset,tPinOffset,strlen(pinOffset)) != 0)
							{
								UnionDebugLog("in UnionDealServiceCodeE143:: the pinoffset is not match");
								// modify by chenqy 
								//return(errCodeHsmCmdMDL_VerifyFailure);
								return(-1001);
								// modify end
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE143:: phsmGroupRec->hsmCmdVersionID[%d]!\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					break;
					//end
				default:
					UnionUserErrLog("in UnionDealServiceCodeE143:: zpkKeyDB.algorithmID[%d] error!\n",zpkKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
			break;
		case	2:	// ZPKתLMK
			// ��ȡPVK
			if ((ret = UnionReadRequestXMLPackageValue("body/keyNameOfPVK",keyNameOfPVK,sizeof(keyNameOfPVK))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE143:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyNameOfPVK");
				return(ret);
			}
			keyNameOfPVK[ret] = 0;

			if ((ret =  UnionReadSymmetricKeyDBRec(keyNameOfPVK,1,&pvkKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE143:: UnionReadSymmetricKeyDBRec keyNameOfPVK[%s]!\n",keyNameOfPVK);
				return(ret);
			}
			if (pvkKeyDB.keyType != conPVK)
			{
				UnionUserErrLog("in UnionDealServiceCodeE143:: is not pvk [%s]!\n",keyNameOfPVK);
				return(errCodeEsscMDL_WrongUsageOfKey);
			}

			// ��ȡ��Կֵ
			if ((ppvkKeyValue = UnionGetSymmetricKeyValue(&pvkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
			{
				UnionUserErrLog("in UnionDealServiceCodeE142:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
				UnionSetResponseRemark("��ȡ��Կֵʧ��");
				return(errCodeParameter);
			}

			// ��ȡPVV
			if ((ret = UnionReadRequestXMLPackageValue("body/PVV",PVV,sizeof(PVV))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE143:: UnionReadRequestXMLPackageValue[%s]!\n","body/PVV");
				return(ret);
			}
			
			//���PVV
			UnionFilterHeadAndTailBlank(PVV);
        		if (strlen(PVV) > 0)
        		{
        			if (!UnionIsDigitString(PVV))
        			{
                			UnionUserErrLog("in UnionDealServiceCodeE143:: PVV[%s] is error!\n",PVV);
                			UnionSetResponseRemark("PVV[%s],����Ϊ����",PVV);
                			return(errCodeHsmCmdMDL_InvalidAcc);
        			}
			}
	
			switch(zpkKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case	conHsmCmdVerRacalStandardHsmCmd:
						case	conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdEC(pinBlock,PVV,UnionConvertSymmetricKeyKeyLen(zpkKeyDB.keyLen),pzpkKeyValue->keyValue,
								UnionConvertSymmetricKeyKeyLen(pvkKeyDB.keyLen),ppvkKeyValue->keyValue,format,accNo,strlen(accNo))) >= 0)
								break;

							// ʹ�þɰ汾��Կ
							if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&zpkKeyDB))
							{
								if ((ret = UnionHsmCmdEC(pinBlock,PVV,UnionConvertSymmetricKeyKeyLen(zpkKeyDB.keyLen),pzpkKeyValue->oldKeyValue,
									UnionConvertSymmetricKeyKeyLen(pvkKeyDB.keyLen),ppvkKeyValue->keyValue,format,accNo,strlen(accNo))) >= 0)
								break;
							}
							
							// ʹ�þɰ汾��Կ
							if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&pvkKeyDB))
							{
								if ((ret = UnionHsmCmdEC(pinBlock,PVV,UnionConvertSymmetricKeyKeyLen(zpkKeyDB.keyLen),pzpkKeyValue->keyValue,
									UnionConvertSymmetricKeyKeyLen(pvkKeyDB.keyLen),ppvkKeyValue->oldKeyValue,format,accNo,strlen(accNo))) >= 0)
								break;
							}
							
							// ʹ�þɰ汾��Կ
							if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&zpkKeyDB) &&
								UnionOldVersionSymmetricKeyIsUsed(&pvkKeyDB))
							{
								if ((ret = UnionHsmCmdEC(pinBlock,PVV,UnionConvertSymmetricKeyKeyLen(zpkKeyDB.keyLen),pzpkKeyValue->oldKeyValue,
									UnionConvertSymmetricKeyKeyLen(pvkKeyDB.keyLen),ppvkKeyValue->oldKeyValue,format,accNo,strlen(accNo))) >= 0)
								break;
							}
							
							UnionUserErrLog("in UnionDealServiceCodeE143:: UnionHsmCmdEC ret = [%d]!\n",ret);
							return(ret);
						default:
							UnionUserErrLog("in UnionDealServiceCodeE143:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
				default:
					UnionUserErrLog("in UnionDealServiceCodeE143:: zpkKeyDB.algorithmID[%d] error!\n",zpkKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
			break;
		case	3:	// LMKתZPK
			// PIN����
			if ((ret = UnionReadRequestXMLPackageValue("body/pinBlockByLMK",pinBlockByLMK,sizeof(pinBlockByLMK))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE143:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinBlockByLMK");
				return(ret);
			}
			else
        		{
				UnionFilterHeadAndTailBlank(pinBlockByLMK);
                        	if (strlen(pinBlockByLMK) > 0)
                        	{
                			if (!UnionIsBCDStr(pinBlockByLMK))
                			{
                        			UnionUserErrLog("in UnionDealServiceCodeE143:: pinBlockByLMK[%s] is error!\n",pinBlockByLMK);
                        			UnionSetResponseRemark("LMK���ܵ�PIN���ķǷ�,����Ϊʮ��������");
                        			return(errCodeParameter);
                			}
				}
        		}
			
			switch(zpkKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case	conHsmCmdVerRacalStandardHsmCmd:
						case	conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdBE(pinBlock,UnionConvertSymmetricKeyKeyLen(zpkKeyDB.keyLen),pzpkKeyValue->keyValue,
								strlen(pinBlockByLMK),pinBlockByLMK,format,accNo,strlen(accNo))) < 0)
							{
								// ʹ�þɰ汾��Կ
								if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
									UnionOldVersionSymmetricKeyIsUsed(&zpkKeyDB))
								{
									if ((ret = UnionHsmCmdBE(pinBlock,UnionConvertSymmetricKeyKeyLen(zpkKeyDB.keyLen),pzpkKeyValue->oldKeyValue,
										strlen(pinBlockByLMK),pinBlockByLMK,format,accNo,strlen(accNo))) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE143:: UnionHsmCmdBE old key ret = [%d]!\n",ret);
										return(ret);
									}
								}
								else
								{
									UnionUserErrLog("in UnionDealServiceCodeE143:: UnionHsmCmdBE current key ret = [%d]!\n",ret);
									return(ret);
								}
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE143:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
				default:
					UnionUserErrLog("in UnionDealServiceCodeE143:: zpkKeyDB.algorithmID[%d] error!\n",zpkKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE143:: mode[%d] error!\n",mode);
			return(errCodeEsscMDLKeyOperationNotPermitted);
	}
	
	return(0);
}


