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

/*
	�޸�ʱ��:	2016-07-05
	�޸���:		leipp
	�޸�ԭ��:	��̨������Ǩ�Ƶ���׼����,����������Ҳ�õ��˴����ƹ��ܷ���
*/

/***************************************
�������:       E801
������:         ������ɢ��Կ��������Կ����MAC��TAC
��������:       ������ɢ��Կ��������Կ����MAC��TAC
***************************************/
int UnionDealServiceCodeE801(PUnionHsmGroupRec phsmGroupRec)
{
        int     ret;
        int     discreteNum = 0;
        char    tmpBuf[32];
        char    mode[16];
	char	keyIndex[32];
        char    keyName[160];
        char    discreteData[128];
        char    sessionData[64];
        char    data[1024];
        char    iv[64];
        char    mac[64];
        char    tmpMac[64];
        char    tmpMac1[32];
        char    tmpMac2[32];
        char    tmpMac3[32];
        char    tmpMac4[32];
        char    macPrefix[64];
        char    macSuffix[64];
	char	keyValue[64];
	char	checkValue[32];
	char    patternID[32];
        char    macLenFlag[32];
	char	appID[32];
	char	keyType[32];
	char 	ICV[32];
	char	subKeyValue[64];

	char	schemeID[8];
	char	fillMode[8];
	
	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue         psymmetricKeyValue = NULL;

	// ģʽ��ʶ
        if ((ret = UnionReadRequestXMLPackageValue("body/mode",mode,sizeof(mode))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]!\n","body/mode");
                return(ret);
        }
	mode[ret] = 0;
        if (strncmp(mode,"1",1) != 0 && strncmp(mode,"2",1) != 0 && strncmp(mode,"3",1) != 0 && strncmp(mode,"4",1) != 0 && strncmp(mode,"5",1) != 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE801::Service mode [%s] is not right!\n",mode);
                UnionSetResponseRemark("mode ���󣬱���Ϊ1��2,3,4,5");
                return(errCodeParameter);
        }
	
	// ��Կ����
        if((ret = UnionReadRequestXMLPackageValue("body/keyIndex", keyIndex, sizeof(keyIndex))) <= 0)
        {
      		// ��Կ����
        	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) <= 0)
        	{
			UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]\n","body/keyName");
			UnionSetResponseRemark("��Կ���ƻ������������һ��");
			return(errCodeParameter);
		}
		else
		{
			keyName[ret] = 0;
			if (UnionIsValidSymmetricKeyName(keyName))
			{
				if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1,&symmetricKeyDB)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
					return(ret);
				}
				if(strncmp(mode,"3",1) == 0)
				{
					if(conZAK != symmetricKeyDB.keyType)
					{
						UnionUserErrLog("in UnionDealServiceCodeE801:: Err keyType\n");
						UnionSetResponseRemark("��C-MACʱ��Կ����ΪZAK��Կ");
						return(errCodeParameter);
					}
				}	

				// ��ȡ��Կֵ
				if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
				{
					UnionUserErrLog("in UnionDealServiceCodeE801:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
					UnionSetResponseRemark("��ȡ��Կֵʧ��");
					return(errCodeParameter);
				}
				snprintf(keyValue, sizeof(keyValue),"%s",psymmetricKeyValue->keyValue);
				snprintf(checkValue, sizeof(checkValue),"%s",symmetricKeyDB.checkValue);
				keyValue[strlen(psymmetricKeyValue->keyValue)] = 0;
				checkValue[strlen(symmetricKeyDB.checkValue)] = 0;
			}
			else if ((mode[0] == '3') && (ret == 32))
			{
				snprintf(keyValue, sizeof(keyValue),"%s",keyName);
				symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfDES;
				symmetricKeyDB.keyType = conZAK;
			}
			else
			{
				UnionUserErrLog("in UnionDealServiceCodeE801:: keyName[%s] invalid!\n",keyName);
				UnionSetResponseRemark("��Կ���Ƹ�ʽ����ȷ����Կֵ���ȴ���");
				return(errCodeParameter);
			}
		}
        }
	else		//ƴ��Կ����appID.keyIndex.mk-ac��appID.keyIndex.zak(modeΪ3ʱ)
	{
		keyIndex[ret] = 0;
		if(strncmp(mode,"3",1) != 0)
		{
			strcpy(keyType, "mk-ac");
			keyType[5] = 0;
		}
		else
		{ 
			strcpy(keyType, "zak");
			keyType[3] = 0;
		}
		if((ret = UnionReadRequestXMLPackageValue("head/appID",appID,sizeof(appID))) <= 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]\n","head/appID");
			return(errCodeParameter);
		}
		appID[ret] = 0;
		snprintf(keyName,sizeof(keyName),"%s.%s.%s",appID,keyIndex,keyType);
                if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1,&symmetricKeyDB)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
                        return(ret);
                }
	
		if(strncmp(mode,"3",1) == 0)
                {
                        if(conZAK != symmetricKeyDB.keyType)
                        {
                                UnionUserErrLog("in UnionDealServiceCodeE801:: Err keyType\n");
                                UnionSetResponseRemark("��C-MACʱ��Կ����ΪZAK��Կ");
                                return(errCodeParameter);
                        }
                }

                // ��ȡ��Կֵ
                if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE801:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
                        UnionSetResponseRemark("��ȡ��Կֵʧ��");
                        return(errCodeParameter);
                }
                snprintf(keyValue,sizeof(keyValue),"%s", psymmetricKeyValue->keyValue);
                snprintf(checkValue,sizeof(checkValue),"%s", symmetricKeyDB.checkValue);
		keyValue[strlen(psymmetricKeyValue->keyValue)] = 0;
                checkValue[strlen(symmetricKeyDB.checkValue)] = 0;
        }

	switch(symmetricKeyDB.keyType)
	{
		case conMKAC:
			memcpy(keyType, "109", 3);
			break;
		/*case conMKSMI:
			memcpy(keyType, "209", 3);
			break;*/
		case conMKSMC:
			memcpy(keyType, "309", 3);
			break;
		case conTAK:
			memcpy(keyType, "003", 3);
			break;
		case conZAK:
			memcpy(keyType, "008", 3);
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE801::symmetricKeyDB.keyType[%d] error!\n",symmetricKeyDB.keyType);
			return(errCodeHsmCmdMDL_InvalidKeyType);
	}
	keyType[3] = 0;

	
        // ��ɢ����
	if ((ret = UnionReadRequestXMLPackageValue("body/discreteNum",tmpBuf,sizeof(tmpBuf))) <= 0)
		discreteNum = 0;
	else
        	discreteNum = atoi(tmpBuf);

        // ��ɢ����
        if (discreteNum > 0)
        {
                if ((ret = UnionReadRequestXMLPackageValue("body/discreteData",discreteData,sizeof(discreteData))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]!\n","body/discreteData");
                        return(ret);
                }
		else if(ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE801:: discreteData is null!\n");
			UnionSetResponseRemark("��ɢ���ݳ��ȴ�����Ϊ��");
			return(errCodeParameter);
		}
		discreteData[ret] = 0;
        }
	
	// �������ݣ��ڷ���IDΪ1��4ʱ�������,��ȷ����Կʹ�������㷨���ٶ�ȡ

        // ����
        if ((ret = UnionReadRequestXMLPackageValue("body/data",data,sizeof(data))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
                return(ret);
        }
	else if(ret == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE801:: data is null!\n");
                UnionSetResponseRemark("���ݲ���Ϊ��");
                return(errCodeParameter);
        }
	data[ret] = 0;

        // ��ʼ����
        if ((ret = UnionReadRequestXMLPackageValue("body/iv",iv,sizeof(iv))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]!\n","body/iv");
                return(ret);
        }
        else if(ret == 0)
        {
		UnionUserErrLog("in UnionDealServiceCodeE801:: iv is null!\n");
                UnionSetResponseRemark("��ʼ��������Ϊ��");
                return(errCodeParameter);
        }
	iv[ret] = 0;
        switch(phsmGroupRec->hsmCmdVersionID)
        {
                case conHsmCmdVerRacalStandardHsmCmd:
                case conHsmCmdVerSJL06StandardHsmCmd:
                        switch(symmetricKeyDB.algorithmID)
                        {
                                case    conSymmetricAlgorithmIDOfDES:   // DES�㷨
					if(strncmp(mode,"3",1) == 0)
					{
						//��C-MAC
						if(0 != discreteNum)
						{
							if(32 != strlen(discreteData))
							{
								UnionUserErrLog("in UnionDealServiceCodeE801:: wrong discreteData len\n");
								UnionSetResponseRemark("��ɢ���ݳ��ȴ�");
								return(errCodeParameter);
							}
							if((ret = UnionHsmCmdX1("02", symmetricKeyDB.keyType, keyValue, symmetricKeyDB.keyType, "1", discreteData, NULL, subKeyValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE801:: UnionHsmCmdX1\n");
								return(ret);
							}
						}
						else
							memcpy(subKeyValue, keyValue, strlen(keyValue)+1);
						
						if(16 != strlen(iv))
						{
							UnionUserErrLog("in UnionDealServiceCodeE801:: wrong iv len\n");
							UnionSetResponseRemark("��ʼ�������ȴ�ӦΪ16λ");
							return(errCodeParameter);
						}
						
						if(0 != strlen(data) % 8)
						{
							UnionUserErrLog("in UnionDealServiceCodeE801:: wrong data len\n");
							UnionSetResponseRemark("Mac���ݳ���ӦΪ8�ı���");
							return(errCodeParameter);
						}
						
						if((ret = UnionHsmCmdCF(subKeyValue, iv, data,tmpMac, ICV)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE801:: UnionHsmCmdCF[%s]!\n",subKeyValue);
							return(ret);
						}
						
						// modify by leipp 20150506	begin
						snprintf(mac,sizeof(mac),"%s", tmpMac);
						//ret = snprintf(mac,sizeof(mac),"%s", tmpMac);
                                                //snprintf(mac+ret,sizeof(mac)-ret,"%s", ICV);
						// modify end
					}
					else
					{
						if(strncmp(mode,"2",1) == 0 || strncmp(mode,"1",1) == 0)
						{
							snprintf(patternID, sizeof(patternID),"0");
							snprintf(macLenFlag,sizeof(macLenFlag), "2");
						}
						if(strncmp(mode,"4",1) == 0)
						{
							snprintf(patternID, sizeof(patternID),"2");
							snprintf(macLenFlag,sizeof(macLenFlag), "1");
						}
						if(strncmp(mode, "5", 1) == 0)
                                        	{
							snprintf(patternID, sizeof(patternID),"1");
							snprintf(macLenFlag,sizeof(macLenFlag), "1");
                                        	        if ((ret = UnionReadRequestXMLPackageValue("body/sessionData",sessionData,sizeof(sessionData))) < 0)
                                        	        {
                                        	                UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]!\n","body/sessionData");
                                        	                return(ret);
                                        	        }
                                                	if(ret != 16)
                                                	{
                                                	        UnionUserErrLog("in UnionDealServiceCodeE801:: sessionData[%s]!\n",sessionData);
                                                	        UnionSetResponseRemark("�������ݳ��ȴ�");
                                                	        return(errCodeParameter);
                                                	}
							sessionData[ret] = 0;
                                        	}
						if ((ret = UnionHsmCmdUB("1", patternID, "109", keyValue, -1, discreteNum, discreteData, sessionData,"1", iv, strlen(data), data, macLenFlag, NULL, tmpMac)) < 0)
                                        	{
                                                	UnionUserErrLog("in UnionDealServiceCodeE801:: UnionHsmCmdUB[%s]!\n",data);
                                                	return(ret);
                                        	}
						tmpMac[ret] = 0;
                                        	if(strncmp(mode,"2",1) == 0)
                                        	{
                                                	strncpy(macPrefix,tmpMac,8);
                                               		strncpy(macSuffix,tmpMac+8,8);
                                                	UnionXOR(macPrefix,macSuffix,8,mac);
							mac[8] = 0;
                                        	}
						else
							snprintf(mac,sizeof(mac),"%s",tmpMac);
					}
                                        break;
                                case    conSymmetricAlgorithmIDOfSM4:
					if(strncmp(mode, "4", 1) == 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE801:: mode[%s] err\n",mode);
						UnionSetResponseRemark("��Կ��־ΪSM4ʱ��mode����Ϊ4");
						return(errCodeParameter);
					}
					if ((ret = UnionReadRequestXMLPackageValue("body/sessionData",sessionData,sizeof(sessionData))) > 0)
					{
						if ((strlen(sessionData) != 4) && (strlen(sessionData) != 16))
						{
							UnionUserErrLog("in UnionDealServiceCodeE801:: UnionReadRequestXMLPackageValue[%s]!\n",sessionData);
							UnionSetResponseRemark("�������ݳ��ȴ�����Ϊ4��16λ");
							return(errCodeParameter);
						}
						snprintf(schemeID,sizeof(schemeID),"4");
						snprintf(fillMode,sizeof(fillMode),"1");
					}
					else
					{
						snprintf(schemeID,sizeof(schemeID),"1");
						snprintf(fillMode,sizeof(fillMode),"0");
					}

                                        //����WDָ��
                                        if ((ret = UnionHsmCmdWD("1", schemeID, keyType, keyValue, checkValue, -1, discreteNum, discreteData, sessionData, fillMode, iv, strlen(data), data, "2", NULL, tmpMac)) < 0)
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
						mac[8] = 0;
                                        }
					else
					{
                                                strncpy(mac,tmpMac,32);
						mac[32] = 0;
					}
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

	// add by leipp 20150506	begin
	if (mode[0] == '3')
	{
		// ����ICV
		if ((ret = UnionSetResponseXMLPackageValue("body/icv",ICV)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE801:: UnionSetResponseXMLPackageValue[%s][%s]!\n","body/icv",ICV);
			return(ret);
		}
	}
	// add end

        return(0);
}
