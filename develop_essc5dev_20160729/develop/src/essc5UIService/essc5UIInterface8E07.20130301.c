//      Author:         ������
//      Copyright:      Union Tech. Guangzhou
//      Date:           2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5UIService.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "unionHsmCmdVersion.h"

/***************************************
�������:       8E07
������:         �ַ��Գ���Կ
��������:       �ַ��Գ���Կ
***************************************/
int UnionDealServiceCode8E07(PUnionHsmGroupRec phsmGroupRec)
{
        int                             ret;
        int                             mode = 1;
        char                            tmpBuf[512];
        char                            remoteCheckValue[32];
        char                            remoteKeyValue[64];
	char				keyByZMK[64];

        TUnionSymmetricKeyDB            symmetricKeyDB;
        PUnionSymmetricKeyValue         psymmetricKeyValue = NULL;
        TUnionRemoteKeyPlatform         tkeyDistributePlatform;

	// add by liwj 20150616
        TUnionSymmetricKeyDB            symmetricKeyDBZmk;
        PUnionSymmetricKeyValue         psymmetricKeyValueZmk = NULL;
	memset(&symmetricKeyDBZmk, 0, sizeof(symmetricKeyDBZmk));
	// end 

        memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));

        // ��ȡ��Կ����
        if ((ret = UnionReadRequestXMLPackageValue("body/keyName",symmetricKeyDB.keyName,sizeof(symmetricKeyDB.keyName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E07:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
                return(ret);
        }

        UnionFilterHeadAndTailBlank(symmetricKeyDB.keyName);

        // ��ȡ�Գ���Կ
        if ((ret =  UnionReadSymmetricKeyDBRec(symmetricKeyDB.keyName,0,&symmetricKeyDB)) < 0)
        {
                if (ret == errCodeKeyCacheMDL_WrongKeyName)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E07:: �Գ���Կ[%s]������!\n",symmetricKeyDB.keyName);
                        UnionSetResponseRemark("�Գ���Կ[%s]������",symmetricKeyDB.keyName);
                        return(errCodeKeyCacheMDL_KeyNonExists);
                }
                UnionUserErrLog("in UnionDealServiceCode8E07:: UnionReadSymmetricKeyDBRec[%s]!\n",symmetricKeyDB.keyName);
                return(ret);
        }
	//add by zhouxw
	UnionSetHsmGroupIDForHsmSvr(symmetricKeyDB.keyGroup);
	//add end
	//add by zhouxw 20151013
        if ((ret = UnionGetHsmGroupRecByHsmGroupID(symmetricKeyDB.keyGroup,phsmGroupRec)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E07:: UnionGetHsmGroupRecByHsmGroupID �������[%s]������!\n",symmetricKeyDB.keyGroup);
                return(ret);
        }
        //add end
        // ģʽ
        // 1���ַ���ǰ��Կ��Ĭ��ֵ
        // 2���ַ�����Կ 
        memset(tmpBuf,0,sizeof(tmpBuf));
        if ((ret = UnionReadRequestXMLPackageValue("body/modeFlag",tmpBuf,sizeof(tmpBuf))) <= 0)
                mode = 1;
        else
        {
                mode = atoi(tmpBuf);
                if ((mode != 1) && (mode != 2))
                {
                        UnionUserErrLog("in UnionDealServiceCode8E07:: mode[%s] error!\n",tmpBuf);
                        UnionSetResponseRemark("�Ƿ���ģʽ[%s]",tmpBuf);
                        return(errCodeParameter);
                }
        }

        // ��ȡ��Կֵ
        if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
        {
		if (mode == 1)
		{
			UnionUserErrLog("in UnionDealServiceCode8E07:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("��ȡ��Կֵʧ��");
			return(errCodeParameter);
		}
		else
			psymmetricKeyValue = &symmetricKeyDB.keyValue[0];
        }

        strcpy(psymmetricKeyValue->lmkProtectMode,phsmGroupRec->lmkProtectMode);

        // ���ַ�ƽ̨�Ƿ����
        if (strlen(symmetricKeyDB.keyDistributePlatform) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E07:: keyDistributePlatform is null!\n");
                UnionSetResponseRemark("��Կ�ַ�ƽ̨����Ϊ��");
                return(errCodeParameter);
        }

        // ��ȡ�ַ�ƽ̨��Ϣ
        memset(&tkeyDistributePlatform,0,sizeof(tkeyDistributePlatform));
        if ((ret =  UnionReadRemoteKeyPlatformRec(symmetricKeyDB.keyDistributePlatform,conRemoteKeyPlatformKeyDirectionOfDistribute,&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E07:: UnionReadRemoteKeyPlatformRec keyDistributePlatform[%s]!\n",symmetricKeyDB.keyDistributePlatform);
                return(ret);
        }

        // ��ʼ��Զ����Կ����������
        if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E07:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
                return(ret);
        }

	if (tkeyDistributePlatform.packageType != 5)
	{
		if (mode == 1)
		{
			if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","293")) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E07:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
				return(ret);
			}
		}
		else
		{
			if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","294")) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E07:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
				return(ret);
			}
		}
	}
	else
	{
		// �޸Ķ�Ӧ������ 
		if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","E115")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E07:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","E115");
			return(ret);
		}

		// �ַ�ģʽ
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",mode);
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/mode",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E07:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/mode",tmpBuf);
			return(ret);
		}
	}

	// add by liwj 20150616
	if (tkeyDistributePlatform.packageType != 5)
	{
		// ��ȡ zmk ��Կ����
		snprintf(symmetricKeyDBZmk.keyName, sizeof(symmetricKeyDBZmk.keyName), "%s", symmetricKeyDB.keyName);
		memcpy(symmetricKeyDBZmk.keyName + strlen(symmetricKeyDBZmk.keyName) - 3, "zmk", 3);

		// ��ȡ�Գ���Կ
		if ((ret =  UnionReadSymmetricKeyDBRec(symmetricKeyDBZmk.keyName,0,&symmetricKeyDBZmk)) < 0)
		{
			if (ret == errCodeKeyCacheMDL_WrongKeyName)
			{
				UnionUserErrLog("in UnionDealServiceCode8E07:: �Գ���Կ[%s]������!\n",symmetricKeyDBZmk.keyName);
				UnionSetResponseRemark("�Գ���Կ[%s]������",symmetricKeyDBZmk.keyName);
				return(errCodeKeyCacheMDL_KeyNonExists);
			}
			UnionUserErrLog("in UnionDealServiceCode8E07:: UnionReadSymmetricKeyDBRec[%s]!\n",symmetricKeyDBZmk.keyName);
			return(ret);
		}

		// ��ȡ��Կֵ
		if ((psymmetricKeyValueZmk = UnionGetSymmetricKeyValue(&symmetricKeyDBZmk,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCode8E07:: UnionGetSymmetricKeyValue [%s] key value is null!\n",symmetricKeyDBZmk.keyName);
			return(errCodeDesKeyDBMDL_KeyNotEffective);
		}
	}
	// end

        memset(remoteKeyValue,0,sizeof(remoteKeyValue));
        memset(remoteCheckValue,0,sizeof(remoteCheckValue));
        if (mode == 1)
        {
                switch(symmetricKeyDB.algorithmID)
                {
                        case    conSymmetricAlgorithmIDOfDES:   // DES�㷨
                                // �ѱ���LMK�Լ���ת��Զ��ZMK����
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						//modify by liwj 20150616
						if (tkeyDistributePlatform.packageType != 5)
						{
							if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,psymmetricKeyValueZmk->keyValue,remoteKeyValue,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key UnionHsmCmdA8!\n");
								return(ret);
							}
						}
						else
						{
							if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,tkeyDistributePlatform.protectKey,remoteKeyValue,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key UnionHsmCmdA8!\n");
								return(ret);
							}
						}
						// end
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E07:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
				strcpy(remoteCheckValue, symmetricKeyDB.checkValue);
                                break;
                        case    conSymmetricAlgorithmIDOfSM4:   // SM4�㷨
                                // �ѱ���LMK�Լ���ת��Զ��ZMK����
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						//modify by liwj 20150616
						if (tkeyDistributePlatform.packageType != 5)
						{
							if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,psymmetricKeyValueZmk->keyValue,remoteKeyValue,remoteCheckValue)) < 0)
							{
									UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key UnionHsmCmdST!\n");
									return(ret);
							}
						}
						else
						{
							if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,tkeyDistributePlatform.protectKey,remoteKeyValue,remoteCheckValue)) < 0)
							{
									UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key UnionHsmCmdST!\n");
									return(ret);
							}
						}
						// end
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E07:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
				strcpy(remoteCheckValue, symmetricKeyDB.checkValue);
                                break;
                        default:
                                UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key symmetricKeyDB.algorithmID[%d] error!\n",symmetricKeyDB.algorithmID);
                                return(errCodeEsscMDL_InvalidAlgorithmID);
                }
        }
        else
        {
                // ����ǰ��Կ��Ϊ����Կ
                memset(psymmetricKeyValue->oldKeyValue,0,sizeof(psymmetricKeyValue->oldKeyValue));
                memset(symmetricKeyDB.oldCheckValue,0,sizeof(symmetricKeyDB.oldCheckValue));
                strcpy(psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue);
                strcpy(symmetricKeyDB.oldCheckValue,symmetricKeyDB.checkValue);

                memset(psymmetricKeyValue->keyValue,0,sizeof(psymmetricKeyValue->keyValue));
                memset(symmetricKeyDB.checkValue,0,sizeof(symmetricKeyDB.checkValue));

                switch(symmetricKeyDB.algorithmID)
                {
                        case    conSymmetricAlgorithmIDOfDES:   // DES�㷨
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret =  UnionHsmCmdA0(0,symmetricKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen),NULL,psymmetricKeyValue->keyValue,keyByZMK,symmetricKeyDB.checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode8E07:: UnionHsmCmdA0!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E07:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
								
                                // �ѱ���LMK�Լ���ת��Զ��ZMK����
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						//modify by liwj 20150616
						if (tkeyDistributePlatform.packageType != 5)
						{
							if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,psymmetricKeyValueZmk->keyValue,remoteKeyValue,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key UnionHsmCmdA8!\n");
								return(ret);
							}
						}
						else
						{
							if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,tkeyDistributePlatform.protectKey,remoteKeyValue,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key UnionHsmCmdA8!\n");
								return(ret);
							}
						}
						// end
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E07:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
                                // ���У��ֵ
                                if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,16) != 0)
                                {
                                        UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
                                        return(errCodeEssc_CheckValue);
                                }
                                break;
                        case    conSymmetricAlgorithmIDOfSM4:   // SM4�㷨
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdWI("0",symmetricKeyDB.keyType,NULL,psymmetricKeyValue->keyValue,keyByZMK,symmetricKeyDB.checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode8E07:: UnionHsmCmdWI!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E07:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
                                // �ѱ���LMK�Լ���ת��Զ��ZMK����
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						//modify by liwj 20150616
						if (tkeyDistributePlatform.packageType != 5)
						{
							if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,psymmetricKeyValueZmk->keyValue,remoteKeyValue,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key UnionHsmCmdST!\n");
								return(ret);
							}
						}
						else
						{
							if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,tkeyDistributePlatform.protectKey,remoteKeyValue,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key UnionHsmCmdST!\n");
								return(ret);
							}
						}
						// end
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E07:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
                                // ���У��ֵ
                                if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,16) != 0)
                                {
                                        UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
                                        return(errCodeEssc_CheckValue);
                                }
                                break;
                        default:
                                UnionUserErrLog("in UnionDealServiceCode8E07:: Distribute Key symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
                                return(errCodeEsscMDL_InvalidAlgorithmID);
                }
        }
        if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyValue",remoteKeyValue)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E07:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyValue",remoteKeyValue);
                return(ret);
        }
        if ((ret = UnionSetRequestRemoteXMLPackageValue("body/checkValue",remoteCheckValue)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E07:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/checkValue",remoteCheckValue);
                return(ret);
        }

        // ת����Կ������Զ��ƽ̨
        if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E07:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
                return(ret);
        }

        if (mode == 2)
        {
                if ((ret =  UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E07:: UnionUpdateSymmetricKeyDBKeyValue,keyName[%s]!\n",symmetricKeyDB.keyName);
                        return(ret);
                }
        }

        return(0);
}


