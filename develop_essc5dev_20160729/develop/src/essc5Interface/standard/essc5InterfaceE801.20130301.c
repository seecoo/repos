//      Author:         ������
//      Copyright:      Union Tech. Guangzhou
//      Date:           2013-01-10

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
#include "unionHsmCmdVersion.h"
#include "unionHsmCmd.h"
#include "UnionXOR.h"
#include "symmetricKeyDB.h"

/***************************************
�������:       E801
������:         ����MAC��TAC
��������:       ����MAC��TAC
***************************************/
int UnionDealServiceCodeE801(PUnionHsmGroupRec phsmGroupRec)
{
        int     ret;
        int     discreteNum = 0;
        char    tmpBuf[16+1];
        char    mode[1+1];
	char	keyIndex[4+1];
        char    keyName[128+1];
        char    discreteData[32*3+1];
        char    sessionData[32+1];
        char    data[1024+1];
        char    iv[32+1];
        char    mac[32+1];
        char    tmpMac[32+1];
        char    tmpMac1[8+1];
        char    tmpMac2[8+1];
        char    tmpMac3[8+1];
        char    tmpMac4[8+1];
        char    macPrefix[32+1];
        char    macSuffix[32+1];
	char	keyValue[32+1];
	char	checkValue[16+1];
	char    patternID[1+1];
        char    macLenFlag[1+1];
	char	appID[16+1];
	char	keyType[10+1];
	
	TUnionSymmetricKeyDB	symmetricKeyDB;
	PUnionSymmetricKeyValue         psymmetricKeyValue = NULL;

	memset(keyValue, 0, sizeof(keyValue));
	memset(checkValue, 0, sizeof(checkValue));	

	// ��Կ����
	memset(keyIndex, 0, sizeof(keyIndex));
        if((ret = UnionReadRequestXMLPackageValue("body/keyIndex", keyIndex, sizeof(keyIndex))) <= 0)
        {
      		// ��Կ����
        	memset(keyName,0,sizeof(keyName));
        	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) <= 0)
        	{
			UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]\n","body/keyName");
			UnionSetResponseRemark("��Կ���ƻ������������һ��");
			return(errCodeParameter);
		}
		else
		{
			memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));
                	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1,&symmetricKeyDB)) < 0)
                	{
                        	UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
                        	return(ret);
                	}
	
        	        // ��ȡ��Կֵ
        	        if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
        	        {
        	                UnionUserErrLog("in UnionDealServiceCodeE801:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
        	                UnionSetResponseRemark("��ȡ��Կֵʧ��");
        	                return(errCodeParameter);
        	        }
        	        memcpy(keyValue, psymmetricKeyValue->keyValue, 32);
        	        memcpy(checkValue, symmetricKeyDB.checkValue, 16);
		}
        }
	else		//ƴ��Կ����appID.keyIndex.mk-ac
	{
		memset(keyType, 0, sizeof(keyType));
		strcpy(keyType, "mk-ac");
		memset(appID, 0, sizeof(appID));
		if((ret = UnionReadRequestXMLPackageValue("head/appID",appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]\n","head/appID");
			return(ret);
		}
		memset(keyName, 0, sizeof(keyName));
		strncpy(keyName, appID, strlen(appID));
		strncat(keyName, ".", 1);
		strncat(keyName, keyIndex, strlen(keyIndex));
		strncat(keyName, ".", 1);
		strncat(keyName, keyType, strlen(keyType));
                memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));
                if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1,&symmetricKeyDB)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
                        return(ret);
                }

                // ��ȡ��Կֵ
                if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE801:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
                        UnionSetResponseRemark("��ȡ��Կֵʧ��");
                        return(errCodeParameter);
                }
                memcpy(keyValue, psymmetricKeyValue->keyValue, 32);
                memcpy(checkValue, symmetricKeyDB.checkValue, 16);
        }
	
        // ģʽ��ʶ
        memset(mode, 0, sizeof(mode));
        if ((ret = UnionReadRequestXMLPackageValue("body/mode",mode,sizeof(mode))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]!\n","body/mode");
                return(ret);
        }
        if (strncmp(mode,"1",1) != 0 && strncmp(mode,"2",1) != 0 && strncmp(mode,"4",1) != 0 && strncmp(mode,"5",1) != 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE801::Service mode [%s] is not right!\n",mode);
                UnionSetResponseRemark("mode ���󣬱���Ϊ1��2,4,5");
                return(errCodeParameter);
        }

        // ��ɢ����
        memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/discreteNum",tmpBuf,sizeof(tmpBuf))) <= 0)
        {
		discreteNum = 0;
        }
        discreteNum = atoi(tmpBuf);

        // ��ɢ����
        memset(discreteData,0,sizeof(discreteData));
        if (discreteNum > 0)
        {
                if ((ret = UnionReadRequestXMLPackageValue("body/discreteData",discreteData,sizeof(discreteData))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]!\n","body/discreteData");
                        return(ret);
                }

                if(strlen(discreteData) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]!\n","body/discreteData");
                        UnionSetResponseRemark("��ɢ���ݳ��ȴ�����Ϊ��");
                        return(errCodeParameter);
                }
        }
	
	// �������ݣ��ڷ���IDΪ1��4ʱ�������,��ȷ����Կʹ�������㷨���ٶ�ȡ
        memset(sessionData,0,sizeof(sessionData));

        // ����
        if ((ret = UnionReadRequestXMLPackageValue("body/data",data,sizeof(data))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
                return(ret);
        }
        if(strlen(data) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
                UnionSetResponseRemark("���ݲ���Ϊ��");
                return(errCodeParameter);
        }
        data[ret] = 0;

        // ��ʼ����
        memset(iv,0,sizeof(iv));
        if ((ret = UnionReadRequestXMLPackageValue("body/iv",iv,sizeof(iv))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]!\n","body/iv");
                return(ret);
        }
        if(strlen(iv) == 0)
        {
		UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]!\n","body/iv");
                UnionSetResponseRemark("��ʼ��������Ϊ��");
                return(errCodeParameter);
        }
        memset(mac,0,sizeof(mac));
        memset(tmpMac,0,sizeof(tmpMac));
        memset(tmpMac1,0,sizeof(tmpMac1));
        memset(tmpMac2,0,sizeof(tmpMac2));
        memset(tmpMac3,0,sizeof(tmpMac3));
        memset(tmpMac4,0,sizeof(tmpMac4));
        memset(macPrefix,0,sizeof(macPrefix));
        memset(macSuffix,0,sizeof(macSuffix));
        switch(phsmGroupRec->hsmCmdVersionID)
        {
                case conHsmCmdVerRacalStandardHsmCmd:
                case conHsmCmdVerSJL06StandardHsmCmd:
                        switch(symmetricKeyDB.algorithmID)
                        {
                                case    conSymmetricAlgorithmIDOfDES:   // DES�㷨
					memset(patternID, 0, sizeof(patternID));
					memset(macLenFlag, 0, sizeof(macLenFlag));
					if(strncmp(mode,"2",1) == 0 || strncmp(mode,"1",1) == 0)
					{
						strncpy(patternID, "0", 1);
						strncpy(macLenFlag, "2", 1);
					}
					if(strncmp(mode,"4",1) == 0)
					{
						strncpy(patternID, "2", 1);
						strncpy(macLenFlag, "1", 1);
					}
					if(strncmp(mode, "5", 1) == 0)
                                        {
                                                strncpy(patternID,"1",1);
                                                strncpy(macLenFlag,"1",1);
                                                if ((ret = UnionReadRequestXMLPackageValue("body/sessionData",sessionData,sizeof(sessionData))) < 0)
                                                {
                                                        UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]!\n","body/sessionData");
                                                        return(ret);
                                                }
                                                if(strlen(sessionData) != 16)
                                                {
                                                        UnionUserErrLog("in UnionDealServiceCodeE801:: sessionData[%s]!\n",sessionData);
                                                        UnionSetResponseRemark("�������ݳ��ȴ�");
                                                        return(errCodeParameter);
                                                }
                                        }
					if ((ret = UnionHsmCmdUB("1", patternID, "109", keyValue, -1, discreteNum, discreteData, sessionData,"1", iv, strlen(data), data, macLenFlag, NULL, tmpMac)) < 0)
                                        {
                                                UnionUserErrLog("in UnionDealServiceCodeE801:: UnionHsmCmdUB[%s]!\n",data);
                                                return(ret);
                                        }
                                        if(strncmp(mode,"2",1) == 0)
                                        {
                                                strncpy(macPrefix,tmpMac,8);
                                                strncpy(macSuffix,tmpMac+8,8);
                                                UnionXOR(macPrefix,macSuffix,8,mac);
                                        }
					else if(strncmp(mode,"1",1) == 0)
                                                strncpy(mac,tmpMac,16);
                                        else
                                                strncpy(mac,tmpMac,8);
                                        break;
                                case    conSymmetricAlgorithmIDOfSM4:
					if(strncmp(mode, "4", 1) == 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE801:: mode[%s] err\n",mode);
						UnionSetResponseRemark("��Կ��־ΪSM4ʱ��mode����Ϊ4");
						return(errCodeParameter);
					}
                                        if ((ret = UnionReadRequestXMLPackageValue("body/sessionData",sessionData,sizeof(sessionData))) < 0)
                                        {
                                                UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]!\n","body/sessionData");
						return(ret);
                                        }
                                        if(strlen(sessionData) != 4)
                                        {
                                                UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]!\n",sessionData);
                                                UnionSetResponseRemark("�������ݳ��ȴ�����Ϊ4λ");
                                                return(errCodeParameter);
                                        }
                                        //����WDָ��
                                        if ((ret = UnionHsmCmdWD("1", "4", "109", keyValue, checkValue, -1, discreteNum, discreteData, sessionData, "1", iv, strlen(data), data, "2", NULL, tmpMac)) < 0)
                                        {
                                                UnionUserErrLog("in UnionDealServiceCodeE801:: UnionHsmCmdWD[%s]!\n",data);
                                                return(ret);
                                        }
                                        if(strncmp(mode,"2",1) == 0)
                                        {
                                                strncpy(tmpMac1,tmpMac,8);
                                                strncpy(tmpMac2,tmpMac+8,8);
                                                strncpy(tmpMac3,tmpMac+16,8);
                                                strncpy(tmpMac4,tmpMac+24,8);
                                                UnionXOR(tmpMac1,tmpMac2,8,macPrefix);
                                                UnionXOR(tmpMac3,tmpMac4,8,macSuffix);
                                                UnionXOR(macPrefix,macSuffix,8,mac);
                                        }
					else
                                                strncpy(mac,tmpMac,32);
                                        break;
                                default:
                                        UnionUserErrLog("in UnionDealServiceCodeE801:: algorithmID[%d] error!\n",symmetricKeyDB.algorithmID);
                                        UnionSetResponseRemark("�㷨��ʶ����");
                                        return(errCodeParameter);
                        }
                        break;
                default:
                        UnionUserErrLog("in UnionDealServiceCodeE801:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                        return(errCodeParameter);
        }

        // ������Ӧ
        if ((ret = UnionSetResponseXMLPackageValue("body/mac",mac)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE801:: UnionSetResponseXMLPackageValue[%s][%s]!\n","body/mac",mac);
                return(ret);
        }

        return(0);
}
