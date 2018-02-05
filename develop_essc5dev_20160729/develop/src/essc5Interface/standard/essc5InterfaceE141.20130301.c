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

/***************************************
�������:	E141
������:		����PIN
��������:	����PIN
***************************************/
int UnionDealServiceCodeE141(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				pinLen = 0;
	int				mode = 1;
	char				plainPin[32];
	char				tmpBuf[32];
	char				accNo[64];
	char				pinBlock[64];
	char				keyName[160];
	char				format[32];
	char				referenceNumber[32];

	TUnionSymmetricKeyDB		zpkKeyDB;
	PUnionSymmetricKeyValue		pzpkKeyValue = NULL;
	
	// PIN����
	if ((pinLen = UnionReadRequestXMLPackageValue("body/pinBlock",pinBlock,sizeof(pinBlock))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE141:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinBlock");
		return(pinLen);
	}
	else
        {
		UnionFilterHeadAndTailBlank(pinBlock);
        	if (strlen(pinBlock) == 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeE141:: pinBlock can not be null!\n");
                	UnionSetResponseRemark("PIN���Ĳ���Ϊ��!");
                	return(errCodeParameter);
        	}
                if (!UnionIsBCDStr(pinBlock))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE141:: pinBlock[%s] is error!\n",pinBlock);
                        UnionSetResponseRemark("PIN����[%s]�Ƿ�,����Ϊʮ��������",pinBlock);
                        return(errCodeParameter);
                }
        }
	
	// �˺�
	if ((ret = UnionReadRequestXMLPackageValue("body/accNo",accNo,sizeof(accNo))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE141:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
		return(ret);
	}

	// ����˺�
	UnionFilterHeadAndTailBlank(accNo);
        if (strlen(accNo) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE141:: accNo can not be null!\n");
                UnionSetResponseRemark("�˺Ų���Ϊ��!");
                return(errCodeParameter);
        }
	if (!UnionIsDigitString(accNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE141:: accNo[%s] is error!\n",accNo);
		UnionSetResponseRemark("�˺ŷǷ�[%s],����Ϊ����",accNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}

	// ģʽ
	// 1��LMK02-03��������PIN
	// 2��ָ��zpk����pin
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) < 0)
		mode = 1;
	else
		mode = atoi(tmpBuf);

	if (mode == 2)
	{
		// ZPK��Կ����
		if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE141:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
			return(ret);
		}
		keyName[ret] = 0;

		UnionFilterHeadAndTailBlank(keyName);
		if (strlen(keyName) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE141:: keyName can not be null!\n");
			UnionSetResponseRemark("��Կ���Ʋ���Ϊ��");
			return(errCodeParameter);
		}
		
		// ��ǰ����format�������ݻᱨ�� , �޸����=0������� bychengj 20160721 
		if ((ret = UnionReadRequestXMLPackageValue("body/format",format,sizeof(format))) <= 0)
			strcpy(format,"01");
		//end

		if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1,&zpkKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE141:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
			return(ret);
		}
		if (zpkKeyDB.keyType != conZPK)
		{
			UnionUserErrLog("in UnionDealServiceCodeE141:: is not zpk [%s]!\n",keyName);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}
		// ��ȡ��Կֵ
		if ((pzpkKeyValue = UnionGetSymmetricKeyValue(&zpkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE141:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("��ȡ��Կֵʧ��");
			return(errCodeParameter);
		}

		switch(zpkKeyDB.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
				switch(phsmGroupRec->hsmCmdVersionID)
        			{
                			case conHsmCmdVerRacalStandardHsmCmd:
                			case conHsmCmdVerSJL06StandardHsmCmd:
						if ((pinLen = UnionHsmCmdJE(zpkKeyDB.keyLen/4,pzpkKeyValue->keyValue,format,pinBlock,accNo,strlen(accNo),pinBlock)) < 0)
						{
							// ʹ�þɰ汾��Կ
							if ((pinLen <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&zpkKeyDB))
							{
								if ((pinLen = UnionHsmCmdJE(zpkKeyDB.keyLen/4,pzpkKeyValue->oldKeyValue,format,pinBlock,accNo,strlen(accNo),pinBlock)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE141:: UnionHsmCmdJG old key ret = [%d]!\n",ret);
									return(pinLen);
								}
							}
							else
							{
								UnionUserErrLog("in UnionDealServiceCodeE141:: UnionHsmCmdJG current key ret = [%d]!\n",ret);
								return(pinLen);
							}
						}
						break;
                			default:
                        			UnionUserErrLog("in UnionDealServiceCodeE141:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        			UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                        			return(errCodeParameter);
        			}
				break;
			case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
				switch(phsmGroupRec->hsmCmdVersionID)
        			{
                			case conHsmCmdVerRacalStandardHsmCmd:
                			case conHsmCmdVerSJL06StandardHsmCmd:
						if ((pinLen = UnionHsmCmdWX(1,pzpkKeyValue->keyValue,pinBlock,format,accNo,strlen(accNo),pinBlock,sizeof(pinBlock))) < 0)
						{
							// ʹ�þɰ汾��Կ
							if ((pinLen <= errCodeOffsetOfHsmReturnCodeMDL) && 
								UnionOldVersionSymmetricKeyIsUsed(&zpkKeyDB))
							{
								if ((pinLen = UnionHsmCmdWX(1,pzpkKeyValue->oldKeyValue,pinBlock,format,accNo,strlen(accNo),pinBlock,sizeof(pinBlock))) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE141:: UnionHsmCmdWX old key ret = [%d]!\n",ret);
									return(pinLen);
								}
							}
							else
							{
								UnionUserErrLog("in UnionDealServiceCodeE141:: UnionHsmCmdJG current key ret = [%d]!\n",ret);
								return(pinLen);
							}
						}
						break;
                			default:
                        			UnionUserErrLog("in UnionDealServiceCodeE141:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        			UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                        			return(errCodeParameter);
        			}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE141:: zpkKeyDB.algorithmID[%d] error!\n",zpkKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
	}
	
	if (mode == 1 || (zpkKeyDB.algorithmID == conSymmetricAlgorithmIDOfDES && mode == 2))
	{
		switch(phsmGroupRec->hsmCmdVersionID)
		{
			case conHsmCmdVerRacalStandardHsmCmd:
			case conHsmCmdVerSJL06StandardHsmCmd:
				if ((ret = UnionHsmCmdNG(pinLen,pinBlock,accNo,strlen(accNo),referenceNumber,plainPin)) < 0)
				{  
					UnionUserErrLog("in UnionDealServiceCodeE141:: UnionHsmCmdNG!\n");
					return(ret);
				}
				 break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE141:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
				UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
				return(errCodeParameter);
		}
	}
	else if (mode == 0 || (zpkKeyDB.algorithmID == conSymmetricAlgorithmIDOfSM4 && mode == 2))
	{
		switch(phsmGroupRec->hsmCmdVersionID)
		{
			case	conHsmCmdVerRacalStandardHsmCmd:
			case	conHsmCmdVerSJL06StandardHsmCmd:
				if ((ret = UnionHsmCmdWW(1,pinBlock,accNo,strlen(accNo),plainPin,sizeof(plainPin),referenceNumber,sizeof(referenceNumber))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE141:: UnionHsmCmdWW!\n");
					return(ret);
				}
				break;
			default:	
				UnionUserErrLog("in UnionDealServiceCodeE141:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
				UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
				return(errCodeParameter);
		}
	
	}
	
	// ����PIN����
	if ((ret = UnionSetResponseXMLPackageValue("body/plainPin",plainPin)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE141:: UnionSetResponseXMLPackageValue plainPin[%s]!\n",plainPin);
		return(ret);
	}

	// ���òο���
	if ((ret = UnionSetResponseXMLPackageValue("body/referenceNumber",referenceNumber)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE141:: UnionSetResponseXMLPackageValue referenceNumber[%s]!\n",referenceNumber);
		return(ret);
	}
	
	return(0);
}
