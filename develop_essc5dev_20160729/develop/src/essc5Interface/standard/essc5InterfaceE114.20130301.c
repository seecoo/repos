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
�������:	E114
������:		����Գ���Կ
��������:	����Գ���Կ
***************************************/
int UnionDealServiceCodeE114(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				mode = 1;
	char				tmpBuf[512];
	int				isRemoteKeyOperate = 0;
	char				platformProtectKey[64];
	char				keyByRKM[64];
	char				remoteCheckValue[32];
	char				remoteKeyValue[64];
	char				sql[128];
	char				keyName[160];
	int				isRemainOriOldKey = 0;
	int				isUseOldKey = 1;

	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;
	TUnionRemoteKeyPlatform		tkeyApplyPlatform;

	// add by liwj 20150616
        TUnionSymmetricKeyDB            symmetricKeyDBZmk;
        PUnionSymmetricKeyValue         psymmetricKeyValueZmk = NULL;
	memset(&symmetricKeyDBZmk, 0, sizeof(symmetricKeyDBZmk));
	// end 

	// ����Ƿ�Զ�̲���
	isRemoteKeyOperate = UnionIsRemoteKeyOperate();

	// ��ȡ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE114:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(keyName);

	// ��ȡ�Գ���Կ
	UnionInitSymmetricKeyDB(&symmetricKeyDB);
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
	{
		if (ret == errCodeKeyCacheMDL_WrongKeyName)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: �Գ���Կ[%s]������!\n",keyName);
			UnionSetResponseRemark("�Գ���Կ[%s]������",keyName);
			return(errCodeKeyCacheMDL_KeyNonExists);
		}
		UnionUserErrLog("in UnionDealServiceCodeE114:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
		return(ret);
	}

	// ģʽ
	// 1�����뵱ǰ��Կ��Ĭ��ֵ
	// 2����������Կ
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) <= 0)
		mode = 1;
	else
	{
		tmpBuf[ret] = 0;
		mode = atoi(tmpBuf);	

		if ((mode != 1) && (mode != 2))
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: mode[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}

	// ��ȡ��Կֵ
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		if (mode == 1 && isRemoteKeyOperate == 1)	//modify 20140709
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("��ȡ��Կֵʧ��");
			return(errCodeParameter);
		}
		else
			psymmetricKeyValue = &symmetricKeyDB.keyValue[0];

		strcpy(psymmetricKeyValue->lmkProtectMode,phsmGroupRec->lmkProtectMode);
	}
	
	if (!isRemoteKeyOperate) // ���ز���
	{
		// ��ǰ��Կ��Ϊ����Կ
		strcpy(psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue);
		strcpy(symmetricKeyDB.oldCheckValue,symmetricKeyDB.checkValue);

		psymmetricKeyValue->keyValue[0] = 0;
		symmetricKeyDB.checkValue[0] = 0;;

		if (strlen(symmetricKeyDB.keyApplyPlatform) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: keyName[%s] keyApplyPlatform can not be null!\n",symmetricKeyDB.keyName);
			UnionSetResponseRemark("��Կ����ƽ̨����Ϊ��");
			return(errCodeParameter);
		}
		else
		{
			// modify by leipp 20151209
			// ��ȡ���������ͷַ�ƽ̨
			if ((ret = UnionCheckRemoteKeyPlatform(symmetricKeyDB.keyName, symmetricKeyDB.keyApplyPlatform, NULL, &tkeyApplyPlatform, NULL, &ret, NULL)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE114:: UnionCheckRemoteKeyPlatform!\n");
				return(ret);
			}
			// modify end
		}

		// add by liwj 20150616
		if (tkeyApplyPlatform.packageType != 5)
		{
			// ��ȡ zmk ��Կ����
			snprintf(symmetricKeyDBZmk.keyName, sizeof(symmetricKeyDBZmk.keyName), "%s", symmetricKeyDB.keyName);
			memcpy(symmetricKeyDBZmk.keyName + strlen(symmetricKeyDBZmk.keyName) - 3, "zmk", 3);

			// ��ȡ�Գ���Կ
			if ((ret =  UnionReadSymmetricKeyDBRec(symmetricKeyDBZmk.keyName,0,&symmetricKeyDBZmk)) < 0)
			{
				if (ret == errCodeKeyCacheMDL_WrongKeyName)
				{
					UnionUserErrLog("in UnionDealServiceCode8E03:: �Գ���Կ[%s]������!\n",symmetricKeyDBZmk.keyName);
					UnionSetResponseRemark("�Գ���Կ[%s]������",symmetricKeyDBZmk.keyName);
					return(errCodeKeyCacheMDL_KeyNonExists);
				}
				UnionUserErrLog("in UnionDealServiceCode8E03:: UnionReadSymmetricKeyDBRec[%s]!\n",symmetricKeyDBZmk.keyName);
				return(ret);
			}

			// ��ȡ��Կֵ
			if ((psymmetricKeyValueZmk = UnionGetSymmetricKeyValue(&symmetricKeyDBZmk,phsmGroupRec->lmkProtectMode)) == NULL)
			{
				UnionUserErrLog("in UnionDealServiceCode8E03:: UnionGetSymmetricKeyValue [%s] key value is null!\n",symmetricKeyDBZmk.keyName);
				return(errCodeDesKeyDBMDL_KeyNotEffective);
			}
		}
		// end

		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		if (tkeyApplyPlatform.packageType != 5)
		{
			if (mode == 1)
			{
				if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","291")) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE114:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
					return(ret);
				}
			}
			else
			{
				if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","292")) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE114:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
					return(ret);
				}
			}
		}

		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	if (isRemoteKeyOperate) // Զ�̲���
	{
		// ϵͳID
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);
		}
		tmpBuf[ret] = 0;

		//��ȡ������Կ
		snprintf(sql,sizeof(sql),"select protectKey from localKeyPlatform where sysID = '%s' and algorithmID = %d",tmpBuf,symmetricKeyDB.algorithmID);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionSelectRealDBRecord[%s]\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: protectKey not found!\n");
			UnionSetResponseRemark("Զ��ƽ̨������Կ������");
			return(errCodeParameter);
		}

		if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionLocateXMLPackage[%s]!\n","detail");	
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("protectKey",platformProtectKey,sizeof(platformProtectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionReadXMLPackageValue[%s]!\n","protectKey");
			return(ret);
		}

		if (mode == 1) //���뵱ǰ��Կ
		{
			switch(symmetricKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
					// �ѱ���LMK�Լ���ת��Զ��ZMK����
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,platformProtectKey,keyByRKM,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE114:: Distribute Key UnionHsmCmdA8!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE114: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
					// �ѱ���LMK�Լ���ת��Զ��ZMK����
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,platformProtectKey,keyByRKM,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE114:: Distribute Key UnionHsmCmdST!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE114: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE114:: Distribute Key symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}			
		}
		else // ��������Կ
		{
			// ����ǰ��Կ��Ϊ����Կ
			strcpy(psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue);
			strcpy(symmetricKeyDB.oldCheckValue,symmetricKeyDB.checkValue);

			psymmetricKeyValue->keyValue[0] = 0;
			symmetricKeyDB.checkValue[0] = 0;

			switch(symmetricKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret =  UnionHsmCmdA0(0,symmetricKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen),NULL,psymmetricKeyValue->keyValue,NULL,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE114:: UnionHsmCmdA0!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE114: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}

					// �ѱ���LMK�Լ���ת��Զ��ZMK����
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,platformProtectKey,keyByRKM,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE114:: Distribute Key UnionHsmCmdA8!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE114: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					// ���У��ֵ
					if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE114:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
						return(errCodeEssc_CheckValue);
					}					
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdWI("0",symmetricKeyDB.keyType,NULL,psymmetricKeyValue->keyValue,NULL,symmetricKeyDB.checkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE114:: UnionHsmCmdWI!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE114: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					// �ѱ���LMK�Լ���ת��Զ��ZMK����
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,platformProtectKey,keyByRKM,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE114:: Distribute Key UnionHsmCmdST!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE114: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					// ���У��ֵ
					if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE114:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
						return(errCodeEssc_CheckValue);
					}					
					break;
				default:
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
		}

		if ((ret = UnionSetResponseXMLPackageValue("body/keyValue",keyByRKM)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionSetResponseXMLPackageValue keyValue[%s]!\n",keyByRKM);
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",symmetricKeyDB.checkValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionSetResponseXMLPackageValue checkValue[%s]!\n",symmetricKeyDB.checkValue);
			return(ret);
		}

		if (mode == 2)
		{
			if ((ret =  UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE114:: UnionUpdateSymmetricKeyDBKeyValue,keyName[%s]!\n",symmetricKeyDB.keyName);
				return(ret);
			}
		}
	}
	else	// ���ز���
	{
		// ��ȡԶ����Կֵ
		if((ret = UnionReadResponseRemoteXMLPackageValue("body/keyValue",remoteKeyValue,sizeof(remoteKeyValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/keyValue");
			return(ret);
		}

		// ��ȡԶ����ԿУ��ֵ
		if((ret = UnionReadResponseRemoteXMLPackageValue("body/checkValue",remoteCheckValue,sizeof(remoteCheckValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/checkValue");
			return(ret);
		}

		// add by leipp 20150708
		if (tkeyApplyPlatform.packageType != 5)
			snprintf(platformProtectKey,sizeof(platformProtectKey),"%s",psymmetricKeyValueZmk->keyValue);
		else
			snprintf(platformProtectKey,sizeof(platformProtectKey),"%s",tkeyApplyPlatform.protectKey);
		// modify end
		
useOldKey:
		switch(symmetricKeyDB.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
				// ��Կת�ɱ���LMK�Լ���
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,platformProtectKey,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE114:: UnionHsmCmdA6 ret[%d]!\n",ret);
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE114: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
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
						if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,platformProtectKey,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE114:: UnionHsmCmdSV ret[%d]!\n",ret);
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE114: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE114:: symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}			

		// modify by leipp 20150708
		// ���У��ֵ
		if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
		{
			if ((tkeyApplyPlatform.packageType != 5) && (isUseOldKey))
			{
				snprintf(platformProtectKey,sizeof(platformProtectKey),"%s",psymmetricKeyValueZmk->oldKeyValue);
				isUseOldKey = 0;
				isRemainOriOldKey = 1;
				UnionLog("in UnionDealServiceCodeE114:: use old protectKey!\n");
				goto useOldKey;
			}
			else if(!isUseOldKey)
			{
				UnionUserErrLog("in UnionDealServiceCodeE114:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s] old key\n",symmetricKeyDB.checkValue,remoteCheckValue);
				return(errCodeEssc_CheckValue);
			}
			else
			{
				UnionUserErrLog("in UnionDealServiceCodeE114:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s] current key\n",symmetricKeyDB.checkValue,remoteCheckValue);
				return(errCodeEssc_CheckValue);

			}
		}
		else
		{
			// Զ����ԿУ��ֵ������6λ
			strcpy(symmetricKeyDB.checkValue, remoteCheckValue);
		}
		
		if (strcmp(psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue) == 0)
		{
			UnionLog("in UnionDealServiceCodeE114:: remoteKeyValue[%s] == currentkeyValue[%s]\n",psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue);
			return(0);
		}

		if (isRemainOriOldKey)
		{
			if (symmetricKeyDB.keyType == conZMK)
			{
				snprintf(psymmetricKeyValue->oldKeyValue,sizeof(psymmetricKeyValue->oldKeyValue),"%s",psymmetricKeyValueZmk->oldKeyValue);
				snprintf(symmetricKeyDB.oldCheckValue,sizeof(symmetricKeyDB.oldCheckValue),"%s",symmetricKeyDBZmk.oldCheckValue);
			}
		}
		// modify end	
		
		if ((ret =  UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE114:: UnionUpdateSymmetricKeyDBKeyValue,keyName[%s]!\n",symmetricKeyDB.keyName);
			return(ret);
		}
	}

	return(0);
}
