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
#include "unionHsmCmdVersion.h"

/***************************************
�������:       8E08
������:         �ָ��Գ���Կ
��������:       �ָ��Գ���Կ
***************************************/
int UnionDealServiceCode8E08(PUnionHsmGroupRec phsmGroupRec)
{
        int                             ret;
        char                            keyValue[64];
        char                            checkValue[32];
        char                            tmpCheckValue[32];
        int                             len = 0;

        TUnionSymmetricKeyDB            symmetricKeyDB;
        PUnionSymmetricKeyValue         psymmetricKeyValue = NULL;

        memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));

        // ��ȡ��Կ����
        if ((ret = UnionReadRequestXMLPackageValue("body/keyName",symmetricKeyDB.keyName,sizeof(symmetricKeyDB.keyName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E08:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
                return(ret);
        }
        UnionFilterHeadAndTailBlank(symmetricKeyDB.keyName);

        // ��ȡ�Գ���Կ
        if ((ret = UnionReadSymmetricKeyDBRec(symmetricKeyDB.keyName,0,&symmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E08:: UnionReadSymmetricKeyDBRec[%s]!\n",symmetricKeyDB.keyName);
                return(ret);
        }
	//add by zhouxw 20150907
	UnionSetHsmGroupIDForHsmSvr(symmetricKeyDB.keyGroup);
	//add end
	//add by zhouxw 20151013
        if ((ret = UnionGetHsmGroupRecByHsmGroupID(symmetricKeyDB.keyGroup,phsmGroupRec)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E08:: UnionGetHsmGroupRecByHsmGroupID �������[%s]������!\n",symmetricKeyDB.keyGroup);
                return(ret);
        }
        //add end
        // ��ȡ��Կֵ
        if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
        {
                UnionUserErrLog("in UnionDealServiceCode8E08:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
                UnionSetResponseRemark("��ȡ��Կֵʧ��");
                return(errCodeParameter);
        }

        // ��ȡ��Կֵ
        memset(keyValue,0,sizeof(keyValue));
        if ((ret = UnionReadRequestXMLPackageValue("body/oldKeyValue",keyValue,sizeof(keyValue))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E08:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyValue");
                return(ret);
        }
        UnionFilterHeadAndTailBlank(keyValue);
        if (strlen(keyValue) == 0 || strlen(psymmetricKeyValue->keyValue) == 0)
        {
                UnionSetResponseRemark("��ǰ��Կֵ�����Կֵ����Ϊ��");
                return(errCodeParameter);
        }

        // ��ȡУ��ֵ
        memset(checkValue,0,sizeof(checkValue));
        if (strlen(keyValue) != 0)
        {
                if ((ret = UnionReadRequestXMLPackageValue("body/oldCheckValue",checkValue,sizeof(checkValue))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E08:: UnionReadRequestXMLPackageValue[%s]!\n","body/checkValue");
                        return(ret);
                }

                UnionSetIsUseNormalZmkType();
                memset(tmpCheckValue,0,sizeof(tmpCheckValue));
		switch(symmetricKeyDB.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES: // DES�㷨
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdBU(0,symmetricKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen),keyValue,tmpCheckValue)) < 0)
						{
								UnionUserErrLog("in UnionDealServiceCode8E08:: UnionHsmCmdBU!\n");
								return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E08:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
				break;
			case    conSymmetricAlgorithmIDOfSM4: // SM4�㷨
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdBU(1,symmetricKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen),keyValue,tmpCheckValue)) < 0)
						{
								UnionUserErrLog("in UnionDealServiceCode8E08:: UnionHsmCmdBU!\n");
								return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E08:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCode8E08:: key algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
                                return(errCodeEsscMDL_InvalidAlgorithmID);
		}

                len = strlen(checkValue) > strlen(tmpCheckValue) ? strlen(tmpCheckValue) : strlen(checkValue);

                if (memcmp(tmpCheckValue,checkValue,len) != 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E08:: tmpCheckValue[%s] != checkValue[%s]!\n",tmpCheckValue,checkValue);
                        return(errCodeEssc_CheckValue);
                }
        }

        // �ָ�����Կ
        memset(psymmetricKeyValue->oldKeyValue,0,sizeof(psymmetricKeyValue->oldKeyValue));
        strcpy(psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue);

        memset(symmetricKeyDB.oldCheckValue,0,sizeof(symmetricKeyDB.oldCheckValue));
        strcpy(symmetricKeyDB.oldCheckValue,symmetricKeyDB.checkValue);

        memset(psymmetricKeyValue->keyValue,0,sizeof(psymmetricKeyValue->keyValue));
        strcpy(psymmetricKeyValue->keyValue,keyValue);

        memset(symmetricKeyDB.checkValue,0,sizeof(symmetricKeyDB.checkValue));
        strcpy(symmetricKeyDB.checkValue,checkValue);

        // ������Կ
        if ((ret = UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E08:: UnionUpdateSymmetricKeyDBKeyValue!\n");
                return(ret);
        }
        return(0);
}


