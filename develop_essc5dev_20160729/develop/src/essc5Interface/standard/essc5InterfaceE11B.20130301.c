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
�������:	E11B
������:		������Կ������Ϣ	
��������:	������Կ������Ϣ 	
***************************************/
int UnionDealServiceCodeE11B(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				len = 0;
	int				outputByZMK = 0;
	int				mode = 1;
	int				inputFlag = 0;
	int				outputFlag = 0;
	int				exportFlag = 0;
	int				enabled;
	int				isDistributeFlag = 0;
	int				isApplyFlag = 0;
	int				isRemoteApplyKey = 0;
	int				isRemoteDistributeKey = 0;
	int				isRemoteKeyOperate = 0;
	char				protectKey[128];
	char				tmpBuf[256];
	char				zmk[64];
	char				tmk[64];
	char				keyByZMK[64];
	char				keyByTMK[64];
	char				keyByRKM[64];
	char				platformProtectKey[64];	
	char				remoteKeyValue[64];
	char				remoteCheckValue[32];
	char				sysID[32];
	char				appID[32];
	char				sql[256];
	char				checkValue[32];
	char				keyName[128];
	char				*ptr = NULL;

        char                            keyNode[80];
        char                            keyType[16];

	TUnionSymmetricKeyDB		symmetricKeyDB;
	TUnionSymmetricKeyDB		zmkKeyDB;
	PUnionSymmetricKeyValue		pzmkKeyValue = NULL;
	TUnionRemoteKeyPlatform		tkeyApplyPlatform;
	TUnionRemoteKeyPlatform		tkeyDistributePlatform;
	
	// ���Զ�̱�ʶ
	isRemoteKeyOperate = UnionIsRemoteKeyOperate();
	
	memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));

	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
		keyName[0] = 0;
	else
		keyName[ret] = 0;

	UnionFilterHeadAndTailBlank(keyName);
	
	//��Կ�汾�� Ŀǰ��kms2���ľɱ���ת��XML������Ҫ����version
        if((ret = UnionReadRequestXMLPackageValue("body/version",tmpBuf, sizeof(tmpBuf))) > 0)
        {
		if((ret = UnionAnalysisSymmetricKeyName(keyName, appID, keyNode, keyType)) < 0)
               	{
                       	UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionAnalysisSymmetricKeyName keyName[%s]\n", keyName);
                       	return(ret);
               	}
               	strcat(keyNode, "-001");
               	memset(keyName, 0, sizeof(keyName));
               	memcpy(keyName, appID, strlen(appID));
               	strcat(keyName, ".");
               	strcat(keyName, keyNode);
               	strcat(keyName, ".");
               	strcat(keyName, keyType);
	}
		
	snprintf(symmetricKeyDB.keyName,sizeof(symmetricKeyDB.keyName),"%s",keyName);

	// ģʽ
	// 0����������Կ
	// 1��������Կ��Ĭ��ֵ
	/*if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) <= 0)
		mode = 1;
	else
	{
		tmpBuf[ret] = 0;
		mode = atoi(tmpBuf);
		if ((mode != 0) && (mode != 1) && (mode != 2))
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: mode[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}*


	// ��ȡ������Կ��ʶ
	if ((ret = UnionReadRequestXMLPackageValue("body/updateKeyFlag",updateKeyFlag,sizeof(updateKeyFlag))) <= 0)
		strcpy(updateKeyFlag,"0");*/

	// �����Կ�����Ƿ����
	if (strlen(symmetricKeyDB.keyName) > 0)
	{
		if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) != errCodeKeyCacheMDL_WrongKeyName)
		{
		}
	}
	
	// ��Կ������ʽ
	strcpy(symmetricKeyDB.keyValue[0].lmkProtectMode,phsmGroupRec->lmkProtectMode);

	// ��Կ��
	if ((ret = UnionReadRequestXMLPackageValue("body/keyGroup",symmetricKeyDB.keyGroup,sizeof(symmetricKeyDB.keyGroup))) <= 0)
		strcpy(symmetricKeyDB.keyGroup,"default");

	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyType",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyType");
		return(ret);
	}
	tmpBuf[ret] = 0;
	if (UnionIsDigitString(tmpBuf))
	{
		symmetricKeyDB.keyType = atoi(tmpBuf);
	}
	else	
	{
		if ((symmetricKeyDB.keyType = UnionConvertSymmetricKeyKeyType(tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: keyType[%s] error!\n",tmpBuf);
			return(symmetricKeyDB.keyType);	
		}
	}

	// �㷨��ʶ 
	if ((ret = UnionReadRequestXMLPackageValue("body/algorithmID",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","body/algorithmID");
		return(ret);	
	}
	tmpBuf[ret] = 0;
	if ((symmetricKeyDB.algorithmID = UnionConvertSymmetricKeyAlgorithmID(tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE11B:: algorithmID[%s] error!\n",tmpBuf);
		return(errCodeEsscMDL_InvalidAlgorithmID);	
	}

	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyLen",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyLen");
		return(ret);
	}
	tmpBuf[ret] = 0;
	if (!UnionIsDigitString(tmpBuf))
	{
		UnionUserErrLog("in UnionDealServiceCodeE11B:: keyLen[%s] is invalid!\n",tmpBuf);
		return(errCodeEssc_KeyLength);
	}

	switch (atoi(tmpBuf))
	{
		case	0:
		case	16:
		case	64:
			symmetricKeyDB.keyLen = con64BitsSymmetricKey;
			break;
		case	1:
		case	32:
		case	128:
			symmetricKeyDB.keyLen = con128BitsSymmetricKey;
			break;
		case	2:
		case	48:
		case	192:
			symmetricKeyDB.keyLen = con192BitsSymmetricKey;
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE11B:: keyLen[%s] is error!\n",tmpBuf);
			return(errCodeEssc_KeyLength);
	}

	if (symmetricKeyDB.algorithmID == conSymmetricAlgorithmIDOfSM4 && (symmetricKeyDB.keyLen != con128BitsSymmetricKey))
	{
		UnionUserErrLog("in UnionDealServiceCodeE11B:: SM4 keyLen[%d]!\n",symmetricKeyDB.keyLen);
		UnionSetResponseRemark("SM4��Կ���ȷǷ�");
		return(errCodeParameter);
	}

	// ����ʹ�þ���Կ
	if ((ret = UnionReadRequestXMLPackageValue("body/oldVersionKeyIsUsed",tmpBuf,sizeof(tmpBuf))) <= 0)
		symmetricKeyDB.oldVersionKeyIsUsed = 1;
	else
	{
		tmpBuf[ret] = 0;
		symmetricKeyDB.oldVersionKeyIsUsed = atoi(tmpBuf);
	}
	if ((symmetricKeyDB.oldVersionKeyIsUsed != 0) && (symmetricKeyDB.oldVersionKeyIsUsed != 1))
	{
		UnionUserErrLog("in UnionDealServiceCodeE11B:: oldVersionKeyIsUsed[%s] error!\n",tmpBuf);
		return(errCodeParameter);
	}

	// �������ʶ
	// ��ѡ��Ĭ��ֵΪ0
	// 0��������
	// 1������
	if ((ret = UnionReadRequestXMLPackageValue("body/inputFlag",tmpBuf,sizeof(tmpBuf))) <= 0)
		symmetricKeyDB.inputFlag = 0;
	else
	{
		tmpBuf[ret] = 0;
		symmetricKeyDB.inputFlag = atoi(tmpBuf);
		if ((symmetricKeyDB.inputFlag != 0) && (symmetricKeyDB.inputFlag != 1) && (symmetricKeyDB.inputFlag != 2))
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: inputFlag[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}

	// ��������ʶ
	// ��ѡ��Ĭ��ֵΪ0
	// 0��������
	// 1������
	if ((ret = UnionReadRequestXMLPackageValue("body/outputFlag",tmpBuf,sizeof(tmpBuf))) <= 0)
		symmetricKeyDB.outputFlag = 0;
	else
	{
		tmpBuf[ret] = 0;
		symmetricKeyDB.outputFlag = atoi(tmpBuf);
		if ((symmetricKeyDB.outputFlag != 0) && (symmetricKeyDB.outputFlag != 1))
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: outputFlag[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}

	// ��Ч����
	if ((ret = UnionReadRequestXMLPackageValue("body/effectiveDays",tmpBuf,sizeof(tmpBuf))) <= 0)
		symmetricKeyDB.effectiveDays = 0;
	else
	{
		UnionFilterHeadAndTailBlank(tmpBuf);
		if (!UnionIsDigitString(tmpBuf))
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: effectiveDays[%s] error!\n",tmpBuf);
			UnionSetResponseRemark("��Ч����[%s]�Ƿ�,����Ϊ����",tmpBuf);
                        return(errCodeParameter);
		}
		
		symmetricKeyDB.effectiveDays = atoi(tmpBuf);
	}

	// ���ñ�ʶ
	// 0��������
	// 1���������ã�����Ч��Ĭ��ֵ
	// 2���������ã�ָ��ʱ����Ч
	if ((ret = UnionReadRequestXMLPackageValue("body/enabled",tmpBuf,sizeof(tmpBuf))) <= 0)
		enabled = 1;
	else
	{
		tmpBuf[ret] = 0;
		enabled = atoi(tmpBuf);
	}
		
	if (mode == 0 || mode == 2)
	{
		enabled = 0;
	}
	
	UnionLog("in UnionDealServiceCodeE11B:: mode[%d] inputFlag[%d] outputFlag[%d] enabled[%d] exportFlag[%d],isRemoteKeyOperate[%d]\n",mode,inputFlag,outputFlag,enabled,exportFlag,isRemoteKeyOperate);
	
	if (enabled == 0)
		symmetricKeyDB.status = conSymmetricKeyStatusOfInitial;	// ״̬����Ϊ��ʼ��״̬
	else
		symmetricKeyDB.status = conSymmetricKeyStatusOfEnabled;	// ״̬����Ϊ����״̬

	// ������Կ
	// ��exportFlagΪ0ʱ��������
	// ��exportFlagΪ1ʱ��Ϊ��Կ����
	// ��exportFlagΪ2,3ʱ��Ϊ��Կֵ
	if (exportFlag != 0)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/protectKey",protectKey,sizeof(protectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","body/protectKey");
			return(ret);
		}
		protectKey[ret] = 0;
	}
	
	// ��ȡ������Կ
	if (exportFlag == 0)
		outputByZMK = 0;
	else if (exportFlag == 1)
	{
		outputByZMK = 1;
		if ((ret =  UnionReadSymmetricKeyDBRec(protectKey,0,&zmkKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadSymmetricKeyDBRec protectKey[%s]!\n",protectKey);
			return(ret);
		}
		if ((zmkKeyDB.keyType != conZMK) && (zmkKeyDB.keyType != conTMK))
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: is not zmk or tmk [%s]!\n",protectKey);
			UnionSetResponseRemark("�Ƿ�������Կ����");	
			return(errCodeEsscMDL_WrongUsageOfKey);
		}
		if ((pzmkKeyValue = UnionGetSymmetricKeyValue(&zmkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
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
				UnionUserErrLog("in UnionDealServiceCodeE11B:: keyName[%s] keyType is not tmk,tpk,pvk or tak!\n",symmetricKeyDB.keyName);
				UnionSetResponseRemark("��Կ[%s]��֧��[TMK]������ʽ",symmetricKeyDB.keyName);
				return(errCodeParameter);
			}
			
			strcpy(tmk,pzmkKeyValue->keyValue);
		}
		else
			strcpy(zmk,pzmkKeyValue->keyValue);
	}
	else
	{
		outputByZMK = 1;
		len = strlen(protectKey);
		if ((len != 16) && (len != 32) && (len != 48))
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: protectKey[%s] len != [16,32,48]\n",protectKey);
			return(errCodeEssc_KeyLength);
		}
		if (exportFlag == 3)
		{
			if (symmetricKeyDB.keyType != conTMK && 
				symmetricKeyDB.keyType != conTPK && 
				symmetricKeyDB.keyType != conPVK && 
			 	symmetricKeyDB.keyType != conTAK)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: keyName[%s] keyType is not tmk,tpk,pvk or tak!\n",symmetricKeyDB.keyName);
				UnionSetResponseRemark("��Կ[%s]��֧��[TMK]������ʽ",symmetricKeyDB.keyName);
				return(errCodeParameter);
			}

			strcpy(tmk,protectKey);
		}
		else
			strcpy(zmk,protectKey);
	}
	
	if (!isRemoteKeyOperate)		// ������Կ����	
	{
		// modify by leipp 20151209 ����UnionCheckRemoteKeyPlatform����
		// ��Կ����ƽ̨
		UnionReadRequestXMLPackageValue("body/keyApplyPlatform",symmetricKeyDB.keyApplyPlatform,sizeof(symmetricKeyDB.keyApplyPlatform));
	
		// ��Կ�ַ�ƽ̨
		UnionReadRequestXMLPackageValue("body/keyDistributePlatform",symmetricKeyDB.keyDistributePlatform,sizeof(symmetricKeyDB.keyDistributePlatform));

		// ��ȡ���������ͷַ�ƽ̨
		if ((ret = UnionCheckRemoteKeyPlatform(symmetricKeyDB.keyName, symmetricKeyDB.keyApplyPlatform, symmetricKeyDB.keyDistributePlatform, &tkeyApplyPlatform, &tkeyDistributePlatform, &isRemoteApplyKey, &isRemoteDistributeKey)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionCheckRemoteKeyPlatform!\n");
			return(ret);
		}
		// modify by leipp end
	}
	else			// Զ����Կ����
	{
		isRemoteApplyKey = 0;
		isRemoteDistributeKey = 0;
		
		// ϵͳID
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);
		}
		sysID[ret] = 0;

		//��ȡ������Կ
		snprintf(sql,sizeof(sql),"select protectKey from localKeyPlatform where sysID = '%s' and algorithmID = %d",sysID,symmetricKeyDB.algorithmID);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionSelectRealDBRecord[%s]\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: protectKey not found!\n");
			UnionSetResponseRemark("Զ�̱�����Կ������");
			return(errCodeParameter);
		}

		if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionLocateXMLPackage[%s]!\n","detail");	
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("protectKey",platformProtectKey,sizeof(platformProtectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadXMLPackageValue[%s]!\n","protectKey");
			return(ret);
		}
		platformProtectKey[ret] = 0;

		// ��Կ����
		if ((ret = UnionReadRequestXMLPackageValue("body/keyApplyPlatform",symmetricKeyDB.keyApplyPlatform,sizeof(symmetricKeyDB.keyApplyPlatform))) > 0)
		{
			strcpy(symmetricKeyDB.keyApplyPlatform,"");
			isApplyFlag = 1;
		}

		// ��Կ�ַ�
		if ((ret = UnionReadRequestXMLPackageValue("body/keyDistributePlatform",symmetricKeyDB.keyDistributePlatform,sizeof(symmetricKeyDB.keyDistributePlatform))) > 0)
		{
			strcpy(symmetricKeyDB.keyDistributePlatform,"");
			isDistributeFlag = 1;
		}
	}
	
	if (isRemoteApplyKey)		// Զ��������Կ
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}
		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	if (mode == 1)	// ������Կ
	{
		if (isRemoteApplyKey)	// Զ������
		{
			// ��ȡԶ����Կֵ
			if((ret = UnionReadResponseRemoteXMLPackageValue("body/keyValue",remoteKeyValue,sizeof(remoteKeyValue))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/keyValue");
				return(ret);
			}
			remoteKeyValue[ret] = 0;

			// ��ȡԶ����ԿУ��ֵ
			if((ret = UnionReadResponseRemoteXMLPackageValue("body/checkValue",remoteCheckValue,sizeof(remoteCheckValue))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/checkValue");
				return(ret);
			}
			remoteCheckValue[ret] = 0;

			switch(symmetricKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
					// ��Կת�ɱ���LMK�Լ���
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case	conHsmCmdVerRacalStandardHsmCmd:
						case	conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,tkeyApplyPlatform.protectKey,remoteKeyValue,symmetricKeyDB.keyValue[0].keyValue,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdA6!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}

					// ���У��ֵ
					if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE11B:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
						return(errCodeEssc_CheckValue);
					}

					if (outputByZMK == 1)
					{
						// �ѱ���LMK�Լ���ת��ZMK/TMK����
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case	conHsmCmdVerRacalStandardHsmCmd:
							case	conHsmCmdVerSJL06StandardHsmCmd:
								if (((exportFlag == 1) && (zmkKeyDB.keyType == conTMK)) || (exportFlag == 3))
								{
									// TMK�������
									if (symmetricKeyDB.keyType == conTMK || symmetricKeyDB.keyType == conTPK || symmetricKeyDB.keyType == conPVK)
									{
										if ((ret = UnionHsmCmdAE(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
										{
											UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdAE!\n");
											return(ret);
										}
										strcpy(keyByZMK,keyByTMK);
									}
									else if (symmetricKeyDB.keyType == conTAK)
									{
										if ((ret = UnionHsmCmdAG(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
										{
											UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdAG!\n");
											return(ret);
										}
										strcpy(keyByZMK,keyByTMK);
									}
								}
								else	// zmk�������
								{
									if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,symmetricKeyDB.keyValue[0].keyValue,zmk,keyByZMK,symmetricKeyDB.checkValue)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdA8!\n");
										return(ret);
									}

									// ���У��ֵ
									if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE11B:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
										return(errCodeEssc_CheckValue);
									}
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
								return(errCodeParameter);
						}
					}
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
					// ��Կת�ɱ���LMK�Լ���
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,tkeyApplyPlatform.protectKey,remoteKeyValue,symmetricKeyDB.keyValue[0].keyValue,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdSV!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					// ���У��ֵ
					if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE11B:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
						return(errCodeEssc_CheckValue);
					}

					if (outputByZMK == 1)
					{
						// �ѱ���LMK�Լ���ת��ZMK����
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case conHsmCmdVerRacalStandardHsmCmd:
							case conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,symmetricKeyDB.keyValue[0].keyValue,zmk,keyByZMK,symmetricKeyDB.checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdST!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
								return(errCodeParameter);
						}
						// ���У��ֵ
						if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE11B:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
							return(errCodeEssc_CheckValue);
						}
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE11B:: symmetricKeyDB.algorithmID[%d] error!\n",symmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}			
		}
		else if (isDistributeFlag)	// Զ�̷ַ�
		{
			// ��ȡԶ����Կֵ
			if((ret = UnionReadRequestXMLPackageValue("body/keyValue",remoteKeyValue,sizeof(remoteKeyValue))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyValue");
				return(ret);
			}
			remoteKeyValue[ret] = 0;

			// ��ȡԶ����ԿУ��ֵ
			if((ret = UnionReadRequestXMLPackageValue("body/checkValue",remoteCheckValue,sizeof(remoteCheckValue))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","body/checkValue");
				return(ret);
			}
			remoteCheckValue[ret] = 0;


			switch(symmetricKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,platformProtectKey,remoteKeyValue,symmetricKeyDB.keyValue[0].keyValue,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdA6!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}


					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
					// ��Կת�ɱ���LMK�Լ���
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,platformProtectKey,remoteKeyValue,symmetricKeyDB.keyValue[0].keyValue,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdSV!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key symmetricKeyDB.algorithmID[%d] error!\n",symmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}			

			// ���У��ֵ
			if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
				return(errCodeEssc_CheckValue);
			}
		}
		else if(isRemoteApplyKey == 0)		// ��������
		{
			switch(symmetricKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if (((exportFlag == 1) && (zmkKeyDB.keyType == conTMK)) || (exportFlag == 3))
								strcpy(tmpBuf,"0");
							else
								sprintf(tmpBuf,"%d",outputByZMK);
								
							if ((ret = UnionHsmCmdA0(atoi(tmpBuf),symmetricKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen),zmk,symmetricKeyDB.keyValue[0].keyValue,keyByZMK,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdA0!\n");
								return(ret);
							}

							if (((exportFlag == 1) && (zmkKeyDB.keyType == conTMK)) || (exportFlag == 3))
							{
								// TMK�������
								if (symmetricKeyDB.keyType == conTMK || symmetricKeyDB.keyType == conTPK || symmetricKeyDB.keyType == conPVK)
								{
									if ((ret = UnionHsmCmdAE(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdAE!\n");
										return(ret);
									}
									strcpy(keyByZMK,keyByTMK);
								}
								else if (symmetricKeyDB.keyType == conTAK)
								{
									if ((ret = UnionHsmCmdAG(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdAG!\n");
										return(ret);
									}
									strcpy(keyByZMK,keyByTMK);
								}
							}
							
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					if (isApplyFlag) // Զ��
					{
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case conHsmCmdVerRacalStandardHsmCmd:
							case conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,symmetricKeyDB.keyValue[0].keyValue,platformProtectKey,keyByZMK,remoteCheckValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key UnionHsmCmdA8!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
								return(errCodeParameter);
						}
						// ���У��ֵ
						if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
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
							if ((ret = UnionHsmCmdWI(tmpBuf,symmetricKeyDB.keyType,zmk,symmetricKeyDB.keyValue[0].keyValue,keyByZMK,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionHsmCmdWI!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					if (isApplyFlag) // Զ��
					{
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case conHsmCmdVerRacalStandardHsmCmd:
							case conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,symmetricKeyDB.keyValue[0].keyValue,platformProtectKey,keyByZMK,remoteCheckValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key UnionHsmCmdST!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
								return(errCodeParameter);
						}
						// ���У��ֵ
						if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
							return(errCodeEssc_CheckValue);
						}
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE11B:: symmetricKeyDB.algorithmID[%d] error!\n",symmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
		}
	}

	// ��Ч����
	// ��enabledΪ2ʱ������
	if (enabled == 1)
	{
		UnionGetFullSystemDate(symmetricKeyDB.activeDate);
	}
	else if (enabled == 2)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/activeDate",symmetricKeyDB.activeDate,sizeof(symmetricKeyDB.activeDate))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","body/activeDate");
			return(ret);	
		}
		
		UnionFilterHeadAndTailBlank(symmetricKeyDB.activeDate);
		if(strlen(symmetricKeyDB.activeDate) >0)
		{
			if (!UnionIsValidFullDateStr(symmetricKeyDB.activeDate))
			{
                	        UnionUserErrLog("in UnionDealServiceCodeE11B:: activeDate[%s] error!\n",symmetricKeyDB.activeDate);
                	        UnionSetResponseRemark("��Ч���ڸ�ʽ[%s]�Ƿ�,����ΪYYYYMMDD��ʽ",symmetricKeyDB.activeDate);
				return(errCodeParameter);
                	}
		}
		else
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: activeDate can not be null!\n");
			UnionSetResponseRemark("��Ч���ڲ���Ϊ��");
			return(errCodeParameter);
		}
	}
	
	if ((ret = UnionReadRequestXMLPackageValue("body/creatorType",tmpBuf,sizeof(tmpBuf))) <= 0)
		symmetricKeyDB.creatorType = conSymmetricCreatorTypeOfApp;
	else
	{
		tmpBuf[ret] = 0;
		if (atoi(tmpBuf) == conSymmetricCreatorTypeOfKMS)
			symmetricKeyDB.creatorType = conSymmetricCreatorTypeOfKMS;
		else
			symmetricKeyDB.creatorType = conSymmetricCreatorTypeOfApp;
	}
	
	if (isRemoteKeyOperate)		// Զ����Կ����
	{
		if ((ret = UnionReadRemoteSysIDAndAppID(sysID,sizeof(sysID),appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRemoteSysIDAndAppID!\n");
			return(ret);	
		}
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",symmetricKeyDB.usingUnit,sizeof(symmetricKeyDB.usingUnit))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);
		}
	}
	else				// ������Կ����
	{
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);	
		}
		if ((ret = UnionReadRequestXMLPackageValue("head/appID",appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");
			return(ret);	
		}
		if (strcasecmp(UnionGetIDOfCustomization(),"aliyun") == 0)	// ������
		{
			if ((ptr = strchr(sysID,':')) == NULL)
			{
				UnionUserErrLog("in UnionCheckUnitID:: sysID[%s] error\n",sysID);
				return(errCodeParameter);
			}               
			ptr += 1;
			sprintf(symmetricKeyDB.usingUnit,"%s",ptr);
		}
	}
	// ���ô�����
	UnionPieceSymmetricKeyCreator(sysID,appID,symmetricKeyDB.creator);

	if (isRemoteDistributeKey)	// Զ�̷ַ���Կ
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
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
							if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,symmetricKeyDB.keyValue[0].keyValue,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key UnionHsmCmdA8!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					// ���У��ֵ
					if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
						return(errCodeEssc_CheckValue);
					}					
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
					// �ѱ���LMK�Լ���ת��Զ��ZMK����
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,symmetricKeyDB.keyValue[0].keyValue,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key UnionHsmCmdST!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE11B:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					// ���У��ֵ
					if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
						return(errCodeEssc_CheckValue);
					}					
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE11B:: Distribute Key symmetricKeyDB.algorithmID[%d] error!\n",symmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}			
			if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyValue",keyByRKM)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyValue",keyByRKM);
				return(ret);
			}
			if ((ret = UnionSetRequestRemoteXMLPackageValue("body/checkValue",remoteCheckValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/checkValue",remoteCheckValue);
				return(ret);
			}
		}
		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	if (strlen(symmetricKeyDB.keyName) > 0)
	{
		if (mode == 0 || mode == 2)	// ��������Կ
		{
			if ((ret =  UnionCreateSymmetricKeyDB(&symmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionCreateSymmetricKeyDB,keyName[%s]!\n",symmetricKeyDB.keyName);
				return(ret);	
			}
		}
		else
		{
			if ((ret =  UnionGenerateSymmetricKeyDBRec(&symmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionGenerateSymmetricKeyDBRec,keyName[%s]!\n",symmetricKeyDB.keyName);
				return(ret);	
			}
		}
	}
	
	// modify by leipp 20160125	�޸���Զ�̷ַ���Կ����Ҫ������Կ��У��ֵ,����[isRemoteKeyOperate == 0]
	if ((exportFlag != 0 && isRemoteKeyOperate == 0)|| (isRemoteKeyOperate && isApplyFlag == 1))
	{
		if (!isRemoteKeyOperate)
		{
			if ((ret = UnionSetResponseXMLPackageValue("body/keyValue2",symmetricKeyDB.keyValue[0].keyValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyValue2");
				return(ret);
			}
		}

		if ((ret = UnionSetResponseXMLPackageValue("body/keyValue",keyByZMK)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyValue");
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",symmetricKeyDB.checkValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE11B:: UnionSetResponseXMLPackageValue[%s]!\n","body/checkValue");
			return(ret);
		}
	}
	// modify end
	return(0);
}
