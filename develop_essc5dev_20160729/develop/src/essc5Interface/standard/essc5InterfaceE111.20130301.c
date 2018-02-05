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
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "unionHsmCmdVersion.h"

/***************************************
�������:	E111
������:		���¶Գ���Կ
��������:	���¶Գ���Կ
***************************************/
int UnionDealServiceCodeE111(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				len;
	int				outputByZMK = 0;
	int				mode = 1;
	int				exportFlag = 0;
	char				protectKey[128];
	char				tmpBuf[128];
	char				zmk[64];
	char				tmk[64];
	char				keyByZMK[64];
	char				keyByRKM[64];
	char				keyByTMK[64];
	char				platformProtectKey[64];
	int				isRemoteKeyOperate = 0;
	int				isRemoteApplyKey = 0;
	int				isRemoteDistributeKey = 0;
	int				isDistributeFlag = 0;
	int				isApplyFlag = 0;
	char				remoteKeyValue[64];
	char				remoteCheckValue[32];
	char				sql[128];
	char				exportLmkKey[32];
	char				checkValue[32];
	char				keyName[160];
	int				flag = 0;
	char				algorithmID[8];
	
	char	        	        sysID[32];
        char    	                appID[32];
        char                    	creator[64];

	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;
	TUnionSymmetricKeyDB		zmkKeyDB;
	PUnionSymmetricKeyValue		pzmkKeyValue = NULL;
	TUnionRemoteKeyPlatform		tkeyApplyPlatform;
	TUnionRemoteKeyPlatform		tkeyDistributePlatform;

	// ģʽ
        // 1������������Կ��Ĭ��ֵ
        // 2������������Կ������ӡ
        if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) <= 0)
                mode = 1;
        else
        {
		tmpBuf[ret] = 0;
                mode = atoi(tmpBuf);
        }

	// ����Ƿ�Զ�̲���
	isRemoteKeyOperate = UnionIsRemoteKeyOperate();
	if (isRemoteKeyOperate)
		flag = 1;

	
	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);
	
	// ��ȡ�Գ���Կ
	UnionInitSymmetricKeyDB(&symmetricKeyDB);
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
	{
		if (ret == errCodeKeyCacheMDL_WrongKeyName)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: keyName[%s] not find!\n",keyName);
			UnionSetResponseRemark("�Գ���Կ[%s]������",keyName);
			return(errCodeKeyCacheMDL_KeyNonExists);
		}
		UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
		return(ret);
	}

	// �����Կ�Ƿ����ʹ��
	if (0 == flag)
	{
		// Ϊ���ݾ�ϵͳ�趨�ļ���ʶ
		if ((ret = UnionReadRequestXMLPackageValue("body/sysFlag",tmpBuf,sizeof(tmpBuf))) > 0)
			flag = 1;
	}
		
	if ((ret = UnionIsUseCheckSymmetricKeyDB(symmetricKeyDB.status,flag)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE111:: UnionIsUseCheckSymmetricKeyDB symmetricKeyDB.status[%d] flag[%d]!\n",symmetricKeyDB.status,flag);
		return(ret);
	}
	
	// �����ʶ
	// 0���������Ĭ��ֵ
	// 1��ָ����Կ���Ʊ������
	// 2�����ZMK��Կ�������
	if ((ret = UnionReadRequestXMLPackageValue("body/exportFlag",tmpBuf,sizeof(tmpBuf))) <= 0)
		exportFlag = 0;
	else
	{
		tmpBuf[ret] = 0;
		exportFlag = atoi(tmpBuf);
		if ((exportFlag != 0) && (exportFlag != 1) && (exportFlag != 2) && (exportFlag != 3) && (exportFlag != 9))
			exportFlag = 0;
	}

	// ���������
	if ((symmetricKeyDB.outputFlag == 0) && (exportFlag != 0))
	{
		UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.outputFlag[%d]!\n",symmetricKeyDB.outputFlag);
		return(errCodeEsscMDL_KeyOutputNotPermitted);		
	}
	
	// ������Կ
	// ��exportFlagΪ0ʱ��������
	// ��exportFlagΪ1ʱ��Ϊ��Կ����
	// ��exportFlagΪ2ʱ��Ϊ��Կֵ
	if (exportFlag != 0 && exportFlag != 9)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/protectKey",protectKey,sizeof(protectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRequestXMLPackageValue[%s]!\n","body/protectKey");
			return(ret);	
		}
		protectKey[ret] = 0;
	}

	exportLmkKey[0] = 0;
	UnionReadRequestXMLPackageValue("body/exportLmkKey",exportLmkKey,sizeof(exportLmkKey));
	
	// ��ȡ������Կ
	if (exportFlag == 0)
		outputByZMK = 0;
	else if (exportFlag == 1)
	{
		outputByZMK = 1;
		UnionInitSymmetricKeyDB(&zmkKeyDB);
		if ((ret =  UnionReadSymmetricKeyDBRec(protectKey,1,&zmkKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadSymmetricKeyDBRec protectKey[%s]!\n",protectKey);
			return(ret);
		}
		if ((zmkKeyDB.keyType != conZMK) && (zmkKeyDB.keyType != conTMK))
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: is not zmk or tmk [%s]!\n",protectKey);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}

		if ((pzmkKeyValue = UnionGetSymmetricKeyValue(&zmkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("��ȡ��Կֵʧ��");
			return(errCodeParameter);
		}

		if (zmkKeyDB.keyType == conTMK)
		{
			if (symmetricKeyDB.keyType != conTMK && 
				symmetricKeyDB.keyType != conTPK && 
				symmetricKeyDB.keyType != conPVK && 
			 	symmetricKeyDB.keyType != conTAK)
			{
				UnionUserErrLog("in UnionDealServiceCodeE111:: keyName[%s] keyType is not tmk,tpk,pvk or tak!\n",symmetricKeyDB.keyName);
				UnionSetResponseRemark("��Կ[%s]��֧��[TMK]������ʽ",symmetricKeyDB.keyName);
				return(errCodeParameter);
			}
			strcpy(tmk,pzmkKeyValue->keyValue);
		}
		else
		strcpy(zmk,pzmkKeyValue->keyValue);
	}
	else if(exportFlag == 9)
	{
		outputByZMK = 1;
		// ʹ�þ���Կ��������Կ��������Կ���������Կֵʱ��ȡ
	}
	else
	{
		outputByZMK = 1;
		len = strlen(protectKey);
		if ((len != 16) && (len != 32) && (len != 48))
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: protectKey[%s] len != [16,32,48]\n",protectKey);
			return(errCodeEssc_KeyLength);
		}
		if (exportFlag == 3)
		{
			if (symmetricKeyDB.keyType != conTMK && 
				symmetricKeyDB.keyType != conTPK && 
				symmetricKeyDB.keyType != conPVK && 
			 	symmetricKeyDB.keyType != conTAK)
			{
				UnionUserErrLog("in UnionDealServiceCodeE111:: keyName[%s] keyType is not tmk,tpk,pvk or tak!\n",symmetricKeyDB.keyName);
				UnionSetResponseRemark("��Կ[%s]��֧��[TMK]������ʽ",symmetricKeyDB.keyName);
				return(errCodeParameter);
			}

			strcpy(tmk,protectKey);
		}
		else
			strcpy(zmk,protectKey);
	}
	
	// ��Чʱ��
	if ((ret = UnionReadRequestXMLPackageValue("body/activeDate",symmetricKeyDB.activeDate,sizeof(symmetricKeyDB.activeDate))) <= 0)
	{
		UnionGetFullSystemDate(symmetricKeyDB.activeDate);
	}
	UnionFilterHeadAndTailBlank(symmetricKeyDB.activeDate);
	if (!UnionIsValidFullDateStr(symmetricKeyDB.activeDate))
        {
                UnionUserErrLog("in UnionDealServiceCodeE111:: activeDate[%s] error!\n",symmetricKeyDB.activeDate);
                UnionSetResponseRemark("��Ч���ڸ�ʽ[%s]�Ƿ�,����ΪYYYYMMDD��ʽ",symmetricKeyDB.activeDate);
                return(errCodeParameter);
        }

	// ��ȡ��Կֵ
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) != NULL)
	{
		// ��ǰ��Կ��Ϊ����Կ
		strcpy(psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue);
		strcpy(symmetricKeyDB.oldCheckValue,symmetricKeyDB.checkValue);

		psymmetricKeyValue->keyValue[0] = 0;
		symmetricKeyDB.checkValue[0] = 0;;
	}
	else
		psymmetricKeyValue = &symmetricKeyDB.keyValue[0];

	strcpy(psymmetricKeyValue->lmkProtectMode,phsmGroupRec->lmkProtectMode);

	// add by leipp 20151022
	// �㷨��ʶ,����㷨��ʶ�뵱ǰ��Կ�㷨��ʶ��ͬ���������Կֵ,�޸���Կ����
	memset(algorithmID,0,sizeof(algorithmID));
	ret = UnionReadRequestXMLPackageValue("body/algorithmID",algorithmID,sizeof(algorithmID));
	if (ret > 0)
	{
		UnionFilterHeadAndTailBlank(algorithmID);
		if ((strcasecmp(algorithmID,"DES") == 0) && (symmetricKeyDB.algorithmID != conSymmetricAlgorithmIDOfDES))
		{
			symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfDES;
			psymmetricKeyValue->oldKeyValue[0] = 0;
			symmetricKeyDB.oldCheckValue[0] = 0;
			strcpy(symmetricKeyDB.keyValue[1].lmkProtectMode,phsmGroupRec->lmkProtectMode);
		}	
		else if ((strcasecmp(algorithmID,"SM4") == 0) && (symmetricKeyDB.algorithmID != conSymmetricAlgorithmIDOfSM4))
		{
			symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfSM4;
			psymmetricKeyValue->oldKeyValue[0] = 0;
			symmetricKeyDB.oldCheckValue[0] = 0;
			strcpy(symmetricKeyDB.keyValue[1].lmkProtectMode,phsmGroupRec->lmkProtectMode);
		}
	}
	// add end

	// �����ʹ�þ���Կ��������Կ�����õ�����Կ�ı�����Կֵ
	if(exportFlag == 9)
	{
		// ������Կ������ʹ�þ���Կ���
		if (symmetricKeyDB.outputFlag > 0 && symmetricKeyDB.keyType != conTMK && symmetricKeyDB.keyType != conZMK)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: work key not alow to exp by its old value!\n");
			return(errCodeEsscMDL_KeyOutputNotPermitted);
		}

		if(symmetricKeyDB.keyType == conTMK)
		{
			strcpy(tmk, psymmetricKeyValue->oldKeyValue);
		}
		else
		{
			strcpy(zmk, psymmetricKeyValue->oldKeyValue);
		}
	}
	
	// ���ز���
	if (!isRemoteKeyOperate)
	{
		// modify by leipp 20151209
		// ��ȡ���������ͷַ�ƽ̨
		if ((ret = UnionCheckRemoteKeyPlatform(symmetricKeyDB.keyName, symmetricKeyDB.keyApplyPlatform, symmetricKeyDB.keyDistributePlatform, &tkeyApplyPlatform, &tkeyDistributePlatform, &isRemoteApplyKey, &isRemoteDistributeKey)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionCheckRemoteKeyPlatform!\n");
			return(ret);
		}
		// modify end
	}
	else	// Զ�̲���
	{
		isRemoteApplyKey = 0;
		isRemoteDistributeKey = 0;
		// ��ȡԶ�̲�����ʶ
		if ((ret = UnionReadRequestXMLPackageValue("body/remoteOperateFlag",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRequestXMLPackageValue[%s]!\n","body/remoteOperateFlag");
			return(ret);
		}
		tmpBuf[ret] = 0;

		if (tmpBuf[0] == '1')
			isApplyFlag = 1;
		else 
			isDistributeFlag = 1;
			
		// ϵͳID
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);
		}
		tmpBuf[ret] = 0;

		//��ȡ������Կ
		snprintf(sql,sizeof(sql),"select protectKey from localKeyPlatform where sysID = '%s' and algorithmID = %d",tmpBuf,symmetricKeyDB.algorithmID);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSelectRealDBRecord[%s]\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: remoteProtectKey not found!\n");
			UnionSetResponseRemark("Զ�̱�����Կ������");
			return(errCodeParameter);
		}

		if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionLocateXMLPackage[%s]!\n","detail");	
			return(ret);
		}

		// ��ȡƽ̨������Կ
		if ((ret = UnionReadXMLPackageValue("protectKey",platformProtectKey,sizeof(platformProtectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadXMLPackageValue[%s]!\n","protectKey");
			return(ret);
		}
		platformProtectKey[ret] = 0;
	}

	// Զ��������Կ
	if (isRemoteApplyKey)
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// ���������־ 1:����
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/remoteOperateFlag","1")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/remoteOperateFlag","1");
			return(ret);
		}

		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	switch(mode)
	{
		case 1:
			if (isRemoteApplyKey)	//Զ�����ɣ����ش洢
			{
				// ��ȡԶ����Կֵ
				if((ret = UnionReadResponseRemoteXMLPackageValue("body/keyValue",remoteKeyValue,sizeof(remoteKeyValue))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/keyValue");
					return(ret);
				}
				remoteKeyValue[ret] = 0;

				// ��ȡԶ����ԿУ��ֵ
				if((ret = UnionReadResponseRemoteXMLPackageValue("body/checkValue",remoteCheckValue,sizeof(remoteCheckValue))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/checkValue");
					return(ret);
				}
				remoteCheckValue[ret] = 0;

				switch(symmetricKeyDB.algorithmID)
				{
					case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
						// ��Կת�ɱ���LMK�Լ���
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case conHsmCmdVerRacalStandardHsmCmd:
							case conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,tkeyApplyPlatform.protectKey,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdA6!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
								return(errCodeParameter);
						}

						// ���У��ֵ
						if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
							return(errCodeEssc_CheckValue);
						}

						if (outputByZMK == 1)
						{
							// �ѱ���LMK�Լ���ת��ZMK����
							switch(phsmGroupRec->hsmCmdVersionID)
							{
								case	conHsmCmdVerRacalStandardHsmCmd:
								case	conHsmCmdVerSJL06StandardHsmCmd:

									if (((exportFlag == 1) && (zmkKeyDB.keyType == conTMK)) || (exportFlag == 3) || (exportFlag == 9 && symmetricKeyDB.keyType == conTMK))
									{
										// TMK�������
										if (symmetricKeyDB.keyType == conTMK || symmetricKeyDB.keyType == conTPK || symmetricKeyDB.keyType == conPVK)
										{
											if ((ret = UnionHsmCmdAE(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
											{
												UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdAE!\n");
												return(ret);
											}
											strcpy(keyByZMK,keyByTMK);
										}
										else if (symmetricKeyDB.keyType == conTAK)
										{
											if ((ret = UnionHsmCmdAG(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
											{
												UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdAG!\n");
												return(ret);
											}
											strcpy(keyByZMK,keyByTMK);
										}
									}
									else
									{
										if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,zmk,keyByZMK,symmetricKeyDB.checkValue)) < 0)
										{
											UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdA8!\n");
											return(ret);
										}
									}

									break;
								default:
									UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
									UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
									return(errCodeParameter);
							}
							// ���У��ֵ
							if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
								return(errCodeEssc_CheckValue);
							}
						}
						break;
					case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
						// ��Կת�ɱ���LMK�Լ���
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case conHsmCmdVerRacalStandardHsmCmd:
							case conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,tkeyApplyPlatform.protectKey,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdSV!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
								return(errCodeParameter);
						}
						// ���У��ֵ
						if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
							return(errCodeEssc_CheckValue);
						}

						if (outputByZMK == 1)
						{
							// �ѱ���LMK�Լ���ת��ZMK����
							switch(phsmGroupRec->hsmCmdVersionID)
							{
								case conHsmCmdVerRacalStandardHsmCmd:
								case conHsmCmdVerSJL06StandardHsmCmd:
									if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,zmk,keyByZMK,symmetricKeyDB.checkValue)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdST!\n");
										return(ret);
									}
									break;
								default:
									UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
									UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
									return(errCodeParameter);
							}
							// ���У��ֵ
							if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
								return(errCodeEssc_CheckValue);
							}
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
						return(errCodeEsscMDL_InvalidAlgorithmID);
				}			
			}
			else if (isDistributeFlag) //��Զ�̴洢
			{
				// ��ȡԶ����Կֵ
				if((ret = UnionReadRequestXMLPackageValue("body/keyValue",remoteKeyValue,sizeof(remoteKeyValue))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyValue");
					return(ret);
				}
				remoteKeyValue[ret] = 0;

				// ��ȡԶ����ԿУ��ֵ
				if((ret = UnionReadRequestXMLPackageValue("body/checkValue",remoteCheckValue,sizeof(remoteCheckValue))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRequestXMLPackageValue[%s]!\n","body/checkValue");
					return(ret);
				}
				remoteCheckValue[ret] = 0;

				switch(symmetricKeyDB.algorithmID)
				{
					case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case	conHsmCmdVerRacalStandardHsmCmd:
							case	conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,platformProtectKey,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdA6!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
								return(errCodeParameter);
						}

						// ���У��ֵ
						if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
							return(errCodeEssc_CheckValue);
						}

						break;
					case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
						// ��Կת�ɱ���LMK�Լ���
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case conHsmCmdVerRacalStandardHsmCmd:
							case conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,platformProtectKey,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdSV!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
								return(errCodeParameter);
						}
						// ���У��ֵ
						if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
							return(errCodeEssc_CheckValue);
						}

						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
						return(errCodeEsscMDL_InvalidAlgorithmID);
				}			
			}
			else if(isRemoteApplyKey == 0)		// ��������
			{
				switch(symmetricKeyDB.algorithmID)
				{
					case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
						if (((exportFlag == 1) && (zmkKeyDB.keyType == conTMK)) || (exportFlag == 3) || (exportFlag == 9 && symmetricKeyDB.keyType == conTMK))
							strcpy(tmpBuf,"0");
						else
							sprintf(tmpBuf,"%d",outputByZMK);

						if ((ret =  UnionHsmCmdA0(atoi(tmpBuf),symmetricKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen),zmk,psymmetricKeyValue->keyValue,keyByZMK,symmetricKeyDB.checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdA0!\n");
							return(ret);
						}

						if (((exportFlag == 1) && (zmkKeyDB.keyType == conTMK)) || (exportFlag == 3) || (exportFlag == 9))
						{
							// TMK�������
							if (symmetricKeyDB.keyType == conTMK || symmetricKeyDB.keyType == conTPK || symmetricKeyDB.keyType == conPVK)
							{
								if ((ret = UnionHsmCmdAE(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdAE!\n");
									return(ret);
								}
								strcpy(keyByZMK,keyByTMK);
							}
							else if (symmetricKeyDB.keyType == conTAK)
							{
								if ((ret = UnionHsmCmdAG(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdAG!\n");
									return(ret);
								}
								strcpy(keyByZMK,keyByTMK);
							}
						}
		
						if (isApplyFlag) // Զ�̷ַ�
						{
							// �ѱ���LMK�Լ���ת��Զ��ZMK����
							switch(phsmGroupRec->hsmCmdVersionID)
							{
								case conHsmCmdVerRacalStandardHsmCmd:
								case conHsmCmdVerSJL06StandardHsmCmd:
									if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,platformProtectKey,keyByZMK,remoteCheckValue)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key UnionHsmCmdA8!\n");
										return(ret);
									}
									break;
								default:
									UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
									UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
									return(errCodeParameter);
							}
							// ���У��ֵ
							if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
								return(errCodeEssc_CheckValue);
							}					
						}
						break;
					case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
						snprintf(tmpBuf,sizeof(tmpBuf),"%d",outputByZMK);
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case conHsmCmdVerRacalStandardHsmCmd:
							case conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret = UnionHsmCmdWI(tmpBuf,symmetricKeyDB.keyType,zmk,psymmetricKeyValue->keyValue,keyByZMK,symmetricKeyDB.checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdWI!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
								return(errCodeParameter);
						}
						if (isApplyFlag) // Զ�̷ַ�
						{
							
							// �ѱ���LMK�Լ���ת��Զ��ZMK����
							switch(phsmGroupRec->hsmCmdVersionID)
							{
								case	conHsmCmdVerRacalStandardHsmCmd:
								case	conHsmCmdVerSJL06StandardHsmCmd:
									if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,platformProtectKey,keyByZMK,remoteCheckValue)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key UnionHsmCmdST!\n");
										return(ret);
									}
									break;
								default:
									UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
									UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
									return(errCodeParameter);
							}
							// ���У��ֵ
							if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
								return(errCodeEssc_CheckValue);
							}					
							break;
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
						return(errCodeEsscMDL_InvalidAlgorithmID);
				}
			}
			break;
		case 2:
		default:
			UnionUserErrLog("in UnionDealServiceCodeE111:: mode = [%d]!\n",mode);
			return(errCodeEsscMDLKeyOperationNotPermitted);
	}

	if (isRemoteDistributeKey)	// Զ�̷ַ���Կ
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// ���������־ 2:�ַ�
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/remoteOperateFlag","2")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/remoteOperateFlag","2");
			return(ret);
		}
		if (mode == 1)	// ������Կ
		{
			// ������Կ��У��ֵ
			switch(symmetricKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
					// �ѱ���LMK�Լ���ת��Զ��ZMK����
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key UnionHsmCmdA8!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					// ���У��ֵ
					if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
						return(errCodeEssc_CheckValue);
					}					
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
					// �ѱ���LMK�Լ���ת��Զ��ZMK����
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key UnionHsmCmdST!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					// ���У��ֵ
					if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
						return(errCodeEssc_CheckValue);
					}					
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}			
			if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyValue",keyByRKM)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyValue",keyByRKM);
				return(ret);
			}
			if ((ret = UnionSetRequestRemoteXMLPackageValue("body/checkValue",remoteCheckValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/checkValue",remoteCheckValue);
				return(ret);
			}
		}
		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}
	//add by zhouxw 	20150520
	if (isRemoteKeyOperate)         // Զ����Կ����
        {
                if ((ret = UnionReadRemoteSysIDAndAppID(sysID,sizeof(sysID),appID,sizeof(appID))) < 0)
                {       
                        UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRemoteSysIDAndAppID!\n");
                        return(ret);
                }
        }
        else                            // ������Կ����
        {       
                if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
                {       
                        UnionUserErrLog("in UnionDealServiceCodeE119:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");            
                        return(ret);
                }
                if ((ret = UnionReadRequestXMLPackageValue("head/appID",appID,sizeof(appID))) < 0)
                {       
                        UnionUserErrLog("in UnionDealServiceCodeE119:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");            
                        return(ret);
                }
        }
	
	
	// ���ô�����
        UnionPieceSymmetricKeyCreator(sysID,appID,creator);
	
	//if(((symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfApp) && ((strcmp(symmetricKeyDB.creator,creator) == 0) || (strcmp(symmetricKeyDB.creator,"APP") == 0))) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfUser) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfKMS) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfMove))

	if(((symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfApp) && (strcmp(symmetricKeyDB.creator,creator) == 0)) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfUser) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfKMS) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfMove) || (strcmp(symmetricKeyDB.creator,"APP") == 0))
	{
	//add end 20150520
		if ((ret =  UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionUpdateSymmetricKeyDBKeyValue,keyName[%s]!\n",symmetricKeyDB.keyName);
			return(ret);
		}
	//add by zhouxw 20150520
	}
	else
	{
		UnionSetResponseRemark("��Կ[%s]�Ǵ˴�����[%s]����,�����������Կ!",keyName,creator);
                UnionUserErrLog("in UnionDealServiceCodeE111:: ��Կ[%s]�Ǵ˴�����[%s]����,�����������Կ�!",keyName,creator);
		return(errCodeEsscMDLKeyOperationNotPermitted);
	}
	//add end 20150520

	// modify by leipp 20160125	�޸���Զ�̷ַ���Կ����Ҫ������Կ��У��ֵ,����[isRemoteKeyOperate == 0]
	if ((exportFlag != 0 && isRemoteKeyOperate == 0) || (isApplyFlag == 1))
	{
		if ((ret = UnionSetResponseXMLPackageValue("body/keyValue",keyByZMK)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyValue");
			return(ret);
		}
		if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",symmetricKeyDB.checkValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetResponseXMLPackageValue[%s]!\n","body/checkValue");
			return(ret);
		}
	}
	// modify end
	if (exportLmkKey[0] == '1' && !isRemoteKeyOperate)
	{
		if ((ret = UnionSetResponseXMLPackageValue("body/keyValue2",psymmetricKeyValue->keyValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyValue2");
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",symmetricKeyDB.checkValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetResponseXMLPackageValue[%s]!\n","body/checkValue");
			return(ret);
		}
	}
	
	return 0;
}

