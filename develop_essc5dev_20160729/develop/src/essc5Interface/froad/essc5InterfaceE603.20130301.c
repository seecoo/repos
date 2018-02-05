//      Author:         ������
//      Copyright:      Union Tech. Guangzhou
//      Date:           2013-01-10

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
#include "unionHsmCmd.h"
#include "symmetricKeyDB.h"
#include "3DesRacalSyntaxRules.h"
#include "unionHsmCmdVersion.h"

/***************************************
�������:       E603
������:         ʹ����ɢ��Կת����PIN
��������:       ʹ����ɢ��Կת����PIN
***************************************/
int UnionDealServiceCodeE603(PUnionHsmGroupRec phsmGroupRec)
{
        int                     ret = 0;
        char                    inScheme[1+1];
        char                    inKeyFlag[1+1];
        char                    inKeyIndex[4+1];
        char                    inDiscreteNum[1+1];
        char                    inDiscreteData[48+1];
        char                    inGC_Data[48+1];
        int                     inKeyLenFlag = 0;
        char                    outScheme[1+1];
        char                    outKeyFlag[1+1];
        char                    outKeyIndex[4+1];
        char                    outDiscreteNum[1+1];
        char                    outDiscreteData[48+1];
        char                    outGC_Data[48+1];
        int                     outKeyLenFlag = 0;
        char                    oriAccNo[32+1];
        char                    desAccNo[32+1];
        char                    accNo[32+1];
        char                    oriPIN[2+1];
        char                    desPIN[2+1];
        char                    oriPINBlock[48];
        char                    desPINBlock[48];
     // char                    pinBlock[48];
	char			keyName[136];
	char			keyValue[48];
	char			checkValue[32];

        TUnionSymmetricKeyDB    inKeyDB;
	PUnionSymmetricKeyValue	pinKeyValue = NULL;
        TUnionSymmetricKeyDB    outKeyDB;
	PUnionSymmetricKeyValue	poutKeyValue = NULL;

        // ���뷽��
        memset(inScheme,0,sizeof(inScheme));
        if ((ret = UnionReadRequestXMLPackageValue("body/inScheme",inScheme,sizeof(inScheme))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/inScheme");
                return(ret);
        }

        if (inScheme[0] == '1')
        {
                // ��������(����)
                memset(inGC_Data,0,sizeof(inGC_Data));
                if ((ret = UnionReadRequestXMLPackageValue("body/inGC_Data",inGC_Data,sizeof(inGC_Data))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/inGC_Data");
                        return(ret);
                }
                else
                {
                        UnionFilterHeadAndTailBlank(inGC_Data);
                        if (strlen(inGC_Data) > 0)
                        {
                                if (!UnionIsBCDStr(inGC_Data))
                                {
                                        UnionUserErrLog("in UnionDealServiceCodeE603:: inGC_Data[%s] is error!\n",inGC_Data);
                                        UnionSetResponseRemark("�������ݣ����룩�Ƿ�,����Ϊʮ��������");
                                        return(errCodeParameter);
                                }
                        }
                }
        }

        // ������Կ��־
        memset(inKeyFlag,0,sizeof(inKeyFlag));
        if ((ret = UnionReadRequestXMLPackageValue("body/inKeyFlag",inKeyFlag,sizeof(inKeyFlag))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/inKeyFlag");
                return(ret);
        }

        if (inKeyFlag[0] == '0')
        {
                // ���ܻ��ڲ�������Կ
                memset(inKeyIndex,0,sizeof(inKeyIndex));
                if ((ret = UnionReadRequestXMLPackageValue("body/inKeyIndex",inKeyIndex,sizeof(inKeyIndex))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/inKeyIndex");
                        return(ret);
                }
        }
		
	if (inKeyFlag[0] == '1')
	{
		// ������Կ����
		memset(&inKeyDB,0,sizeof(inKeyDB));
		if ((ret = UnionReadRequestXMLPackageValue("body/inKeyName",keyName,sizeof(keyName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/inKeyName");
			return(ret);
		}
		keyName[ret] = 0;
		UnionFilterHeadAndTailBlank(keyName); 

		// ��ȡ��Կ
		if ((ret = UnionReadSymmetricKeyDBRec(keyName,1,&inKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
			return(ret);
		}

		// ��ȡ��Կֵ
		if ((pinKeyValue = UnionGetSymmetricKeyValue(&inKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE603:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("��ȡ������Կֵʧ��");
			return(errCodeParameter);
		}

		// ��ȡ��Կ���ȱ�ʶ
		switch(inKeyDB.keyLen)
		{
			case    con64BitsSymmetricKey:
					inKeyLenFlag = 0;
					break;
			case    con128BitsSymmetricKey:
					inKeyLenFlag = 1;
					break;
			case    con192BitsSymmetricKey:
					inKeyLenFlag = 2;
					break;
			default :
					inKeyLenFlag = 1;
		}
	}
		
        // ��Կ��ɢ����(����)
        memset(inDiscreteNum,0,sizeof(inDiscreteNum));
        if ((ret = UnionReadRequestXMLPackageValue("body/inDiscreteNum",inDiscreteNum,sizeof(inDiscreteNum))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/inDiscreteNum");
                return(ret);
        }

	if (inDiscreteNum[0] == '1' || inDiscreteNum[0] == '2' || inDiscreteNum[0] == '3')
	{	
		// ��Կ��ɢ����(����)
		memset(inDiscreteData,0,sizeof(inDiscreteData));
		if ((ret = UnionReadRequestXMLPackageValue("body/inDiscreteData",inDiscreteData,sizeof(inDiscreteData))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/inDiscreteData");
			return(ret);
		}
		else
		{
			UnionFilterHeadAndTailBlank(inDiscreteData);
			if (strlen(inDiscreteData) > 0)
			{
				if (!UnionIsBCDStr(inDiscreteData))
				{
					UnionUserErrLog("in UnionDealServiceCodeE603:: inDiscreteData[%s] is error!\n",inDiscreteData);
					UnionSetResponseRemark("��Կ��ɢ����(����)�Ƿ�,����Ϊʮ��������");
					return(errCodeParameter);
				}
			}
		}
	}


        // ��������
        memset(outScheme,0,sizeof(outScheme));
        if ((ret = UnionReadRequestXMLPackageValue("body/outScheme",outScheme,sizeof(outScheme))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/outScheme");
                return(ret);
        }

        if (outScheme[0] == '1')
        {
                // ��������(����)
                memset(outGC_Data,0,sizeof(outGC_Data));
                if ((ret = UnionReadRequestXMLPackageValue("body/outGC_Data",outGC_Data,sizeof(outGC_Data))) < 0)
                {
                        UnionUserErrLog("out UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/outGC_Data");
                        return(ret);
                }
                else
                {
                        UnionFilterHeadAndTailBlank(outGC_Data);
                        if (strlen(outGC_Data) > 0)
                        {
                                if (!UnionIsBCDStr(outGC_Data))
                                {
                                        UnionUserErrLog("in UnionDealServiceCodeE603:: outGC_Data[%s] is error!\n",outGC_Data);
                                        UnionSetResponseRemark("�������ݣ��������Ƿ�,����Ϊʮ��������");
                                        return(errCodeParameter);
                                }
                        }
                }
        }

        // ������Կ��־
        memset(outKeyFlag,0,sizeof(outKeyFlag));
        if ((ret = UnionReadRequestXMLPackageValue("body/outKeyFlag",outKeyFlag,sizeof(outKeyFlag))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/outKeyFlag");
                return(ret);
        }

        if (outKeyFlag[0] == '0')
        {
                // ���ܻ��ڲ�������Կ
                memset(outKeyIndex,0,sizeof(outKeyIndex));
                if ((ret = UnionReadRequestXMLPackageValue("body/outKeyIndex",outKeyIndex,sizeof(outKeyIndex))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/outKeyIndex");
                        return(ret);
                }
        }

	if (outKeyFlag[0] == '1')
	{
		// ��Կ����
		memset(&outKeyDB,0,sizeof(outKeyDB));
		if ((ret = UnionReadRequestXMLPackageValue("body/outKeyName",keyName,sizeof(keyName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/outKeyName");
			return(ret);
		}
		keyName[ret] = 0;
		UnionFilterHeadAndTailBlank(keyName); 

		// ��ȡ��Կ
		if ((ret = UnionReadSymmetricKeyDBRec(keyName,1,&outKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE603:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
			return(ret);
		}

		// ��ȡ��Կֵ
		if ((poutKeyValue = UnionGetSymmetricKeyValue(&outKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE603:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("��ȡ������Կֵʧ��");
			return(errCodeParameter);
		}

		// ��ȡ��Կ���ȱ�ʶ
		switch(outKeyDB.keyLen)
		{
			case    con64BitsSymmetricKey:
					outKeyLenFlag = 0;
					break;
			case    con128BitsSymmetricKey:
					outKeyLenFlag = 1;
					break;
			case    con192BitsSymmetricKey:
					outKeyLenFlag = 2;
					break;
			default :
					outKeyLenFlag = 1;
		}
	}

        // ��Կ��ɢ����(����)
        memset(outDiscreteNum,0,sizeof(outDiscreteNum));
        if ((ret = UnionReadRequestXMLPackageValue("body/outDiscreteNum",outDiscreteNum,sizeof(outDiscreteNum))) < 0)
        {
                UnionUserErrLog("out UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/outDiscreteNum");
                return(ret);
        }

        if (outDiscreteNum[0] == '1' || outDiscreteNum[0] == '2' || outDiscreteNum[0] == '3')
        {
                // ��Կ��ɢ����(����)
                memset(outDiscreteData,0,sizeof(outDiscreteData));
                if ((ret = UnionReadRequestXMLPackageValue("body/outDiscreteData",outDiscreteData,sizeof(outDiscreteData))) < 0)
                {
                        UnionUserErrLog("out UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/outDiscreteData");
                        return(ret);
                }
                else
                {
                        UnionFilterHeadAndTailBlank(outDiscreteData);
                        if (strlen(outDiscreteData) > 0)
                        {
                                if (!UnionIsBCDStr(outDiscreteData))
                                {
                                        UnionUserErrLog("in UnionDealServiceCodeE603:: outDiscreteData[%s] is error!\n",outDiscreteData);
                                        UnionSetResponseRemark("��Կ��ɢ����(����)[%s]�Ƿ�,����Ϊʮ��������",outDiscreteData);
                                        return(errCodeParameter);
                                }
                        }
                }
        }
	

        // Դ�ʺ�
        memset(oriAccNo,0,sizeof(oriAccNo));
        if ((ret = UnionReadRequestXMLPackageValue("body/oriAccNo",oriAccNo,sizeof(oriAccNo))) < 0)
        {
                UnionUserErrLog("out UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/oriAccNo");
                return(ret);
        }

        // Ŀ���ʺ�
        memset(desAccNo,0,sizeof(desAccNo));
        if ((ret = UnionReadRequestXMLPackageValue("body/desAccNo",desAccNo,sizeof(desAccNo))) < 0)
        {
                UnionUserErrLog("out UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/desAccNo");
                return(ret);
        }

        // Դpin���ʽ
        memset(oriPIN,0,sizeof(oriPIN));
        if ((ret = UnionReadRequestXMLPackageValue("body/oriPIN",oriPIN,sizeof(oriPIN))) < 0)
        {
                UnionUserErrLog("out UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/oriPIN");
                return(ret);
        }
        else
        {
                UnionFilterHeadAndTailBlank(oriPIN);
                if (strlen(oriPIN) > 0)
                {
                        if (!UnionIsDigitString(oriPIN))
                        {
                                UnionUserErrLog("in UnionDealServiceCodeE603:: oriPIN[%s] is error!\n",oriPIN);
                                UnionSetResponseRemark("Դpin���ʽ[%s]�Ƿ�,����Ϊ����",oriPIN);
                                return(errCodeParameter);
                        }
                }
                else
                {
                        UnionUserErrLog("in UnionDealServiceCodeE603:: oriPIN can not be null!\n");
                        UnionSetResponseRemark("Դpin���ʽ����Ϊ��");
                        return(errCodeParameter);
                }
		
		if (strcmp(oriPIN,"01") != 0 && strcmp(oriPIN,"03") != 0 && strcmp(oriPIN,"13") != 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE603:: oriPIN[%s] is error!\n",oriPIN);
                        UnionSetResponseRemark("Դpin���ʽ[%s]�Ƿ�,���ݲ�����Ҫ��",oriPIN);
                        return(errCodeParameter);
		}
        }

        // Ŀ��pin���ʽ
        memset(desPIN,0,sizeof(desPIN));
        if ((ret = UnionReadRequestXMLPackageValue("body/desPIN",desPIN,sizeof(desPIN))) < 0)
        {
                UnionUserErrLog("out UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/desPIN");
                return(ret);
        }
        else
        {
                UnionFilterHeadAndTailBlank(desPIN);
                if(strlen(desPIN) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE603:: desPIN can not be null!\n");
                        UnionSetResponseRemark("Ŀ��pin���ʽ����Ϊ��");
                        return(errCodeParameter);
                }
                if (!UnionIsDigitString(desPIN))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE603:: desPIN[%s] is error!\n",desPIN);
                        UnionSetResponseRemark("Ŀ��pin���ʽ[%s]�Ƿ�,����Ϊ����",desPIN);
                        return(errCodeParameter);
                }
		if (strcmp(desPIN,"01") != 0 && strcmp(desPIN,"03") != 0 && strcmp(desPIN,"13") != 0 && strcmp(desPIN,"15") != 0 && strcmp(desPIN,"16") != 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE603:: desPIN[%s] is error!\n",desPIN);
                        UnionSetResponseRemark("Ŀ��pin���ʽ[%s]�Ƿ�,���ݲ�����Ҫ��",desPIN);
                        return(errCodeParameter);
		}
        }

        // Ŀ��ȫ�˺�
        if (memcmp(desPIN,"16",2) == 0)
        {
                memset(accNo,0,sizeof(accNo));
                if ((ret = UnionReadRequestXMLPackageValue("body/accNo",accNo,sizeof(accNo))) < 0)
                {
                                UnionUserErrLog("out UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
                                return(ret);
                }
        }

        // Դpin����
        memset(oriPINBlock,0,sizeof(oriPINBlock));
        if ((ret = UnionReadRequestXMLPackageValue("body/oriPINBlock",oriPINBlock,sizeof(oriPINBlock))) < 0)
        {
                UnionUserErrLog("out UnionDealServiceCodeE603:: UnionReadRequestXMLPackageValue[%s]!\n","body/oriPINBlock");
                return(ret);
        }
        else
        {
                UnionFilterHeadAndTailBlank(oriPINBlock);
                if(strlen(oriPINBlock) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE603:: oriPINBlock can not be null!\n");
                        UnionSetResponseRemark("Դpin���Ĳ���Ϊ��");
                        return(errCodeParameter);
                }
                if (!UnionIsBCDStr(oriPINBlock))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE603:: oriPINBlock[%s] is error!\n",oriPINBlock);
                        UnionSetResponseRemark("Դpin����[%s]�Ƿ�,����Ϊʮ��������",oriPINBlock);
                        return(errCodeParameter);
                }
        }

        memset(desPINBlock,0,sizeof(desPINBlock));
        switch(inKeyDB.algorithmID)
        {
                case    conSymmetricAlgorithmIDOfDES:
                        switch(phsmGroupRec->hsmCmdVersionID)
                        {
                                case conHsmCmdVerRacalStandardHsmCmd:
                                case conHsmCmdVerSJL06StandardHsmCmd:
                                        if ((ret = UnionHsmCmdN4(atoi(inScheme),atoi(inKeyFlag),inKeyIndex,atoi(inDiscreteNum),inDiscreteData,inGC_Data,
                                                                inKeyLenFlag,inKeyDB.keyValue[0].keyValue,atoi(outScheme),atoi(outKeyFlag),outKeyIndex,atoi(outDiscreteNum),outDiscreteData,
                                                                outGC_Data,outKeyLenFlag,outKeyDB.keyValue[0].keyValue,12,oriAccNo,desAccNo,atoi(oriPIN),atoi(desPIN),accNo,oriPINBlock,desPINBlock)) < 0)
                                        {
                                                UnionUserErrLog("in UnionDealServiceCodeE603:: UnionHsmCmdN4!\n");
                                                return(ret);
                                        }
                                        break;
                                default:
                                        UnionUserErrLog("in UnionDealServiceCodeE603:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                                        UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                                        return(errCodeParameter);
                        }
                        break;
                case    conSymmetricAlgorithmIDOfSM4:
			if ((ret = UnionHsmCmdWE("02",inKeyDB.keyType,pinKeyValue->keyValue,inKeyDB.checkValue,0,inKeyDB.keyType,"1",inDiscreteData,"",keyValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE603:: UnionHsmCmdWE !\n");
				return(ret);
			}
			keyValue[ret] = 0;
			if ((ret = UnionHsmCmdBU(1,inKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(inKeyDB.keyLen),keyValue,checkValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE603:: UnionHsmCmdBU !\n");
				return(ret);
			}
			if ((ret = UnionHsmCmdWE("02",inKeyDB.keyType,keyValue,checkValue,0,conZPK,"1",inGC_Data,"",keyValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE603:: UnionHsmCmdWE !\n");
				return(ret);
			}
			keyValue[ret] = 0;
			
			//modify by lusj 20160111�������ε�WXʵ�ֵĹ��������һ�ε�W8ָ��һ�£������ڴ�ũ�ſͻ�ʹ�÷��ֵ�������WXָ���õ���PIN����ܵ������Ѿ��ı� 
			/*
			if ((ret = UnionHsmCmdWX(1,keyValue,oriPINBlock,oriPIN,oriAccNo,strlen(oriAccNo),pinBlock,sizeof(pinBlock))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE603:: UnionHsmCmdWX !\n");
				return(ret);
			}
			if ((ret = UnionHsmCmdWX(0,poutKeyValue->keyValue,pinBlock,desPIN,desAccNo,strlen(desAccNo),desPINBlock,sizeof(desPINBlock))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE603:: UnionHsmCmdWX !\n");
				return(ret);
			}
			*/
			if ((ret = UnionHsmCmdW8(2,2,2,NULL,keyValue,2,NULL,poutKeyValue->keyValue,oriPIN,desPIN,oriPINBlock,oriAccNo,desAccNo,desPINBlock,sizeof(desPINBlock))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE603:: UnionHsmCmdW8 !\n");
				return(ret);
			}
			//end by lusj 	

                        break;
                default:
                        UnionUserErrLog("in UnionDealServiceCodeE603:: inKeyDB.algorithmID[%d] error!\n",inKeyDB.algorithmID);
                        return(errCodeEsscMDL_InvalidAlgorithmID);
        }

        // Ŀ��pin����
        if ((ret = UnionSetResponseXMLPackageValue("body/desPINBlock",desPINBlock)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE603:: UnionSetResponseXMLPackageValue[%s]","body/desPINBlock");
                return(ret);
        }

	return(0);
}
