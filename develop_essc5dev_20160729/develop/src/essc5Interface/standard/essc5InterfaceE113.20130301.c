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
#include "unionHsmCmdVersion.h"

/***************************************
�������:	E113
������:		�����Գ���Կ
��������:	�����Գ���Կ
***************************************/
int UnionDealServiceCodeE113(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				len = 0;
	char				sql[256];
	char				protectFlag[32];
	char				protectKey[128];
	char				zmk[64];
	char				tmk[64];
	char				keyByZMK[64];
	char				keyByTMK[64];
	char				checkValue[32];
	char				exportLmkKey[32];
	char				keyName[160];

	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;
	TUnionSymmetricKeyDB		zmkKeyDB;
	PUnionSymmetricKeyValue		pzmkKeyValue = NULL;
	
	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);

	// ��ȡ�Գ���Կ
	UnionInitSymmetricKeyDB(&symmetricKeyDB);
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1,&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
		return(ret);
	}

	// ��ȡ��Կֵ
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(errCodeParameter);
	}

	if (symmetricKeyDB.status != conSymmetricKeyStatusOfEnabled)	// ������״̬
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: symmetricKeyDB.status[%d]!\n",symmetricKeyDB.status);
		return(errCodeEsscMDL_KeyStatusDisabled);
	}
	
	if (!symmetricKeyDB.outputFlag)		// ��������
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: symmetricKeyDB.outputFlag[%d]!\n",symmetricKeyDB.outputFlag);
		return(errCodeEsscMDL_KeyOutputNotPermitted);
	}

	exportLmkKey[0] = 0;
	ret = UnionReadRequestXMLPackageValue("body/exportLmkKey",exportLmkKey,sizeof(exportLmkKey));
	exportLmkKey[ret] = 0;
	
	if ((ret = UnionReadRequestXMLPackageValue("body/protectFlag",protectFlag,sizeof(protectFlag))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: UnionReadRequestXMLPackageValue[%s]!\n","body/protectFlag");
		return(ret);
	}
	protectFlag[ret] = 0;
	
	if (protectFlag[0] != '4' && protectFlag[0] != '9')
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/protectKey",protectKey,sizeof(protectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: UnionReadRequestXMLPackageValue[%s]!\n","body/protectKey");
			return(ret);	
		}
		protectKey[ret] = 0;
	}

	// ��ȡ������Կ
	// 1��ָ����Կ����
	// 2�����ZMK��Կ����
	// 3: ���TMK��Կ����
	// 4�����ݲ�����Կ�ĵ�λ�����ұ�����Կ
	// 9������Կֵ�����µ���Կֵ
	if (protectFlag[0] == '1')	// ָ����Կ����
	{
		if ((ret =  UnionReadSymmetricKeyDBRec(protectKey,1,&zmkKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: UnionReadSymmetricKeyDBRec protectKey[%s]!\n",protectKey);
			return(ret);
		}

		if ((zmkKeyDB.keyType != conZMK) && (zmkKeyDB.keyType != conTMK))
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: is not zmk or tmk [%s]!\n",protectKey);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}

		if ((pzmkKeyValue = UnionGetSymmetricKeyValue(&zmkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
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

			snprintf(tmk,sizeof(tmk),"%s",pzmkKeyValue->keyValue);
		}
		else
		{
			if (strcmp(protectKey,keyName) == 0)
			{
				if ((pzmkKeyValue->oldKeyValue == NULL) || (strlen(pzmkKeyValue->oldKeyValue) == 0))
				{
					UnionUserErrLog("in UnionDealServiceCodeE111:: keyName[%s] old zmk not found!\n",symmetricKeyDB.keyName);
					UnionSetResponseRemark("��Կ����[%s]������Կ������",symmetricKeyDB.keyName);
					return(errCodeParameter);
				}
				else
					snprintf(zmk,sizeof(zmk),"%s",pzmkKeyValue->oldKeyValue);
			}
			else
				snprintf(zmk,sizeof(zmk),"%s",pzmkKeyValue->keyValue);
		}
	}
	else if (protectFlag[0] == '2' || protectFlag[0] == '3')	// �����Կ����
	{
		len = strlen(protectKey);
		if ((len != 16) && (len != 32) && (len != 48))
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: protectKey[%s] len != [16,32,48]\n",protectKey);
			return(errCodeEssc_KeyLength);
		}
		if (protectFlag[0] == '3')
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
			snprintf(tmk,sizeof(tmk),"%s",protectKey);
		}
		else
			snprintf(zmk,sizeof(zmk),"%s",protectKey);
	}
	else if (protectFlag[0] == '4')		// Լ���ı�����Կ
	{
		//��ȡ������Կ
		snprintf(sql,sizeof(sql),"select protectKey from localKeyPlatform where sysID = '%s' and algorithmID = %d",symmetricKeyDB.usingUnit,symmetricKeyDB.algorithmID);

		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: UnionSelectRealDBRecord[%s]\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: remoteProtectKey not found!\n");
			UnionSetResponseRemark("������Կ������");
			return(errCodeParameter);
		}

		if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: UnionLocateXMLPackage[%s]!\n","detail");	
			return(ret);
		}

		// ��ȡƽ̨������Կ
		if ((ret = UnionReadXMLPackageValue("protectKey",protectKey,sizeof(protectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: UnionReadXMLPackageValue[%s]!\n","protectKey");
			return(ret);
		}
		protectKey[ret] = 0;

		snprintf(zmk,sizeof(zmk),"%s",protectKey);
	}
	else if (protectFlag[0] == '9')	// 9������Կֵ�����µ���Կֵ
	{
		if (psymmetricKeyValue->oldKeyValue == NULL || strlen(psymmetricKeyValue->oldKeyValue) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: keyName[%s] oldKeyValue not found!\n",symmetricKeyDB.keyName);
			UnionSetResponseRemark("����Կֵ������");
			return(errCodeParameter);
		}
		snprintf(zmk,sizeof(zmk),"%s",psymmetricKeyValue->oldKeyValue);
	}
	else
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: protectFlag[%s] error!\n",protectFlag);
		return(errCodeParameter);
	}
	
	switch(symmetricKeyDB.algorithmID)
	{
		case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					if (((protectFlag[0] == '1') && (zmkKeyDB.keyType == conTMK)) || (protectFlag[0] == '3') || (protectFlag[0] == '9' && symmetricKeyDB.keyType == conTMK))
					{
						// TMK�������
						if (symmetricKeyDB.keyType == conTMK || symmetricKeyDB.keyType == conTPK || symmetricKeyDB.keyType == conPVK)
						{
							if ((ret = UnionHsmCmdAE(tmk,psymmetricKeyValue->keyValue,keyByTMK,checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE113:: UnionHsmCmdAE!\n");
								return(ret);
							}
						}
						else if (symmetricKeyDB.keyType == conTAK)
						{
							if ((ret = UnionHsmCmdAG(tmk,psymmetricKeyValue->keyValue,keyByTMK,checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE113:: UnionHsmCmdAG!\n");
								return(ret);
							}
						}
						snprintf(checkValue,sizeof(checkValue),"%s",symmetricKeyDB.checkValue);
						snprintf(keyByZMK,sizeof(keyByZMK),"%s",keyByTMK);
					}
					else
					{
						if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,zmk,keyByZMK,checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE113:: UnionHsmCmdA8!\n");
							return(ret);
						}
					}

					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE113: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
					return(errCodeParameter);
			}
			break;
		case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case conHsmCmdVerRacalStandardHsmCmd:
				case conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,zmk,keyByZMK,checkValue)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE113:: UnionHsmCmdST!\n");
						return(ret);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE113: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
					return(errCodeParameter);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE113: symmetricKeyDB.algorithmID[%d] is invalid\n",symmetricKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	// ���У��ֵ
	if (memcmp(checkValue,symmetricKeyDB.checkValue,strlen(symmetricKeyDB.checkValue)) != 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: checkValue[%s] != symmetricKeyDB.checkValue[%s]\n",checkValue,symmetricKeyDB.checkValue);
		return(errCodeEssc_CheckValue);
	}
	
	if ((ret = UnionSetResponseXMLPackageValue("body/keyValue",keyByZMK)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyValue");
		return(ret);
	}

	if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",checkValue)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE113:: UnionSetResponseXMLPackageValue[%s]!\n","body/checkValue");
		return(ret);
	}

	if (exportLmkKey[0] == '1')
	{
		if ((ret = UnionSetResponseXMLPackageValue("body/keyValue2",psymmetricKeyValue->keyValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyValue2");
			return(ret);
		}
	}

	if (protectFlag[0] == '4')
	{
		if ((ret = UnionSetResponseXMLPackageValue("body/usingUnit",symmetricKeyDB.usingUnit)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE113:: UnionSetResponseXMLPackageValue[%s]!\n","body/usingUnit");
			return(ret);
		}
	}

	return(0);
}
