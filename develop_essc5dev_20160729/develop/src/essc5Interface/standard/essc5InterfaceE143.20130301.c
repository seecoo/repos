//	Author:		张永定
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
服务代码:	E143
服务名:		验证PIN
功能描述:	验证PIN
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
	// PIN密文
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
                        UnionSetResponseRemark("PIN密文不能为空!");
                        return(errCodeParameter);
                }
                if (!UnionIsBCDStr(pinBlock))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE143:: pinBlock[%s] is error!\n",pinBlock);
                        UnionSetResponseRemark("PIN密文[%s]非法,必须为十六进制数",pinBlock);
                        return(errCodeParameter);
                }
        }

	// PIN格式
	if ((ret = UnionReadRequestXMLPackageValue("body/format",format,sizeof(format))) <= 0)
		strcpy(format,"01");
	else
		format[ret] = 0;
		
	// 账号
	if ((ret = UnionReadRequestXMLPackageValue("body/accNo",accNo,sizeof(accNo))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE143:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
		return(ret);
	}
	else
		accNo[ret] = 0;

	// 检测账号
	UnionFilterHeadAndTailBlank(accNo);
        if (strlen(accNo) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE143:: accNo can not be null!\n");
                UnionSetResponseRemark("账号不能为空!");
                return(errCodeParameter);
        }
	if (!UnionIsDigitString(accNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE143:: accNo[%s] is error!\n",accNo);
		UnionSetResponseRemark("账号非法[%s],必须为数字",accNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}

	// 密钥名称
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
	// 读取密钥值
	if ((pzpkKeyValue = UnionGetSymmetricKeyValue(&zpkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE143:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("读取密钥值失败");
		return(errCodeParameter);
	}

	// 模式
	// 1:IBM PinOffset验证
	// 2:PVV方式验证
	// 3:LMK密文验证
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
		case	1:	// IBM PinOffset验证
			// 读取PVK
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
			// 读取密钥值
			if ((ppvkKeyValue = UnionGetSymmetricKeyValue(&pvkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
			{
				UnionUserErrLog("in UnionDealServiceCodeE142:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
				UnionSetResponseRemark("读取密钥值失败");
				return(errCodeParameter);
			}
			
			// 读取PIN Offset
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

			// 检验标识
			if ((ret = UnionReadRequestXMLPackageValue("body/checkFlag",tmpBuf,sizeof(tmpBuf))) < 0)
				checkFlag = 0;
			else
				checkFlag = atoi(tmpBuf);
			
			if (checkFlag == 0)	// 系统设定
				snprintf(checkData,sizeof(checkData),"%s",UnionGetIBMUserDefinedData());
			else if (checkFlag == 1)	// 用户输入
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
							UnionSetResponseRemark("pin校验数据[checkData]必须为12位");
							return(errCodeParameter);
						}
					}
				}
				else
				{
					// PIN校验数据
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
						UnionSetResponseRemark("十进制换号表[decimalTable]必须为16位");
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
				case	conSymmetricAlgorithmIDOfDES:	// DES算法
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case	conHsmCmdVerRacalStandardHsmCmd:
						case	conHsmCmdVerSJL06StandardHsmCmd:
							// add by chenqy 20151129
							//DESZPK加密的pinBlock转SM4ZPK加密后再校验(PVK为SM4密钥)
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
						//modify by lusj 20151210 ,原因: 当旧密钥值为空，不进行校验		
							// 使用旧版本密钥
							if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&zpkKeyDB) && (strlen(pzpkKeyValue->oldKeyValue)!= 0) )
							{
								if ((second_result=ret = UnionHsmCmdEA(pinBlock,UnionGetIBMMinPINLength(),checkData,decimalTable,
									pinOffset,UnionConvertSymmetricKeyKeyLen(zpkKeyDB.keyLen),pzpkKeyValue->oldKeyValue,
									UnionConvertSymmetricKeyKeyLen(pvkKeyDB.keyLen),ppvkKeyValue->keyValue,format,accNo,strlen(accNo))) >= 0)
							break;
							}

							// 使用旧版本密钥
							if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&pvkKeyDB)&& (strlen(ppvkKeyValue->oldKeyValue)!= 0) )
							{
							if ((first_result=ret = UnionHsmCmdEA(pinBlock,UnionGetIBMMinPINLength(),checkData,decimalTable,
								pinOffset,UnionConvertSymmetricKeyKeyLen(zpkKeyDB.keyLen),pzpkKeyValue->keyValue,
								UnionConvertSymmetricKeyKeyLen(pvkKeyDB.keyLen),ppvkKeyValue->oldKeyValue,format,accNo,strlen(accNo))) >= 0)
							break;
							}
							
							// 使用旧版本密钥
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

							/*原因:
							1、未做密钥更新前，当送入的PinBlock为其他pin加密的PinBlock时，
							 EA指令能解密过，但是校验不过，报-1001错误，上层解释为输入数据错误。（报错合理）
							2、当做密钥更新后，当送入的PinBlock为其他pin加密的PinBlock时，
							EA指令进行两次校验，第一次报-1001错，第二次为-1020错，并返回-1020，上层解释为密钥未更新，不合理，应该报-1001错。	
							*/
							if((first_result <= errCodeOffsetOfHsmReturnCodeMDL)&&(second_result <= errCodeOffsetOfHsmReturnCodeMDL))
								ret= first_result;	
							//end
							
							UnionUserErrLog("in UnionDealServiceCodeE143:: UnionHsmCmdEA ret = [%d]!\n",ret);
							return(ret);
						default:
							UnionUserErrLog("in UnionDealServiceCodeE143:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
					//add by yuanwb 20150410
					//将ZPK加密的PIN转换为LMK加密
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							//add by zhouxw 20150722
							//SM4ZPK加密的pinBlock转DESZPK加密后再校验(PVK为DES密钥)
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
								//使用旧版本密钥
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
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					break;
					//end
				default:
					UnionUserErrLog("in UnionDealServiceCodeE143:: zpkKeyDB.algorithmID[%d] error!\n",zpkKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
			break;
		case	2:	// ZPK转LMK
			// 读取PVK
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

			// 读取密钥值
			if ((ppvkKeyValue = UnionGetSymmetricKeyValue(&pvkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
			{
				UnionUserErrLog("in UnionDealServiceCodeE142:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
				UnionSetResponseRemark("读取密钥值失败");
				return(errCodeParameter);
			}

			// 读取PVV
			if ((ret = UnionReadRequestXMLPackageValue("body/PVV",PVV,sizeof(PVV))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE143:: UnionReadRequestXMLPackageValue[%s]!\n","body/PVV");
				return(ret);
			}
			
			//检测PVV
			UnionFilterHeadAndTailBlank(PVV);
        		if (strlen(PVV) > 0)
        		{
        			if (!UnionIsDigitString(PVV))
        			{
                			UnionUserErrLog("in UnionDealServiceCodeE143:: PVV[%s] is error!\n",PVV);
                			UnionSetResponseRemark("PVV[%s],必须为数字",PVV);
                			return(errCodeHsmCmdMDL_InvalidAcc);
        			}
			}
	
			switch(zpkKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DES算法
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case	conHsmCmdVerRacalStandardHsmCmd:
						case	conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdEC(pinBlock,PVV,UnionConvertSymmetricKeyKeyLen(zpkKeyDB.keyLen),pzpkKeyValue->keyValue,
								UnionConvertSymmetricKeyKeyLen(pvkKeyDB.keyLen),ppvkKeyValue->keyValue,format,accNo,strlen(accNo))) >= 0)
								break;

							// 使用旧版本密钥
							if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&zpkKeyDB))
							{
								if ((ret = UnionHsmCmdEC(pinBlock,PVV,UnionConvertSymmetricKeyKeyLen(zpkKeyDB.keyLen),pzpkKeyValue->oldKeyValue,
									UnionConvertSymmetricKeyKeyLen(pvkKeyDB.keyLen),ppvkKeyValue->keyValue,format,accNo,strlen(accNo))) >= 0)
								break;
							}
							
							// 使用旧版本密钥
							if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&pvkKeyDB))
							{
								if ((ret = UnionHsmCmdEC(pinBlock,PVV,UnionConvertSymmetricKeyKeyLen(zpkKeyDB.keyLen),pzpkKeyValue->keyValue,
									UnionConvertSymmetricKeyKeyLen(pvkKeyDB.keyLen),ppvkKeyValue->oldKeyValue,format,accNo,strlen(accNo))) >= 0)
								break;
							}
							
							// 使用旧版本密钥
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
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
				default:
					UnionUserErrLog("in UnionDealServiceCodeE143:: zpkKeyDB.algorithmID[%d] error!\n",zpkKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
			break;
		case	3:	// LMK转ZPK
			// PIN密文
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
                        			UnionSetResponseRemark("LMK加密的PIN密文非法,必须为十六进制数");
                        			return(errCodeParameter);
                			}
				}
        		}
			
			switch(zpkKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DES算法
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case	conHsmCmdVerRacalStandardHsmCmd:
						case	conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdBE(pinBlock,UnionConvertSymmetricKeyKeyLen(zpkKeyDB.keyLen),pzpkKeyValue->keyValue,
								strlen(pinBlockByLMK),pinBlockByLMK,format,accNo,strlen(accNo))) < 0)
							{
								// 使用旧版本密钥
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
							UnionSetResponseRemark("非法的加密机指令类型");
							return(errCodeParameter);
					}
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4算法
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


