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
#include "essc5Interface.h"
#include "UnionStr.h"
#include "asymmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "commWithHsmSvr.h"
#include "unionHsmCmdVersion.h"

/***************************************
�������:       8E16
������:         �ַ��ǶԳ���Կ
��������:       �ַ��ǶԳ���Կ
***************************************/
int UnionDealServiceCode8E16(PUnionHsmGroupRec phsmGroupRec)
{
        int                             ret;
        char                            keyName[128];
        char                            pkValue[512];
        int                             len = 0;
        char                            tmpBuf[32];
        int                             lenOfVK = 0;
        int                             lenOfPK = 0;
        char                            vkValue[2560];
        char                            remoteVK[2560];
        char                            vkByKek[2560];
        char                            keyType[32];
        unsigned char                   pkexp[32];
        int                             lenOfPKExp = 0;
	char				flag[32];
	int				addLen = 0;

        TUnionAsymmetricKeyDB           asymmetricKeyDB;
        TUnionRemoteKeyPlatform         tkeyDistributePlatform;

        memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));

        // ��ȡ��Կ����
        memset(keyName,0,sizeof(keyName));
        if ((ret = UnionReadRequestXMLPackageValue("body/keyName",asymmetricKeyDB.keyName,sizeof(asymmetricKeyDB.keyName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E16:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
                return(ret);
        }
        UnionFilterHeadAndTailBlank(asymmetricKeyDB.keyName);
        if (strlen(asymmetricKeyDB.keyName) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E16:: keyName can not be null!\n");
                UnionSetResponseRemark("��Կ���Ʋ���Ϊ��!");
                return(errCodeParameter);
        }

        // ��ȡ�ǶԳ���Կ
        if ((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName,0,&asymmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E16:: UnionReadAsymmetricKeyDBRec[%s]!\n",asymmetricKeyDB.keyName);
                return(ret);
        }
	
	//add by zhouxw 20150908
	UnionSetHsmGroupIDForHsmSvr(asymmetricKeyDB.keyGroup);
	//add end
	//add by zhouxw 20151013
        if ((ret = UnionGetHsmGroupRecByHsmGroupID(asymmetricKeyDB.keyGroup,phsmGroupRec)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E16:: UnionGetHsmGroupRecByHsmGroupID �������[%s]������!\n",asymmetricKeyDB.keyGroup);
                return(ret);
        }
        //add end

        // ��Կָ��
        if (asymmetricKeyDB.algorithmID == conAsymmetricAlgorithmIDOfRSA)
        {
                memset(tmpBuf,0,sizeof(tmpBuf));
                if (asymmetricKeyDB.pkExponent < 256)
                        sprintf(tmpBuf,"%02X",asymmetricKeyDB.pkExponent);
                else if (asymmetricKeyDB.pkExponent < 256 * 256)
                        sprintf(tmpBuf,"%04X",asymmetricKeyDB.pkExponent);
                else
                        sprintf(tmpBuf,"%06X",asymmetricKeyDB.pkExponent);

                memset(pkexp,0,sizeof(pkexp));
                lenOfPKExp = aschex_to_bcdhex(tmpBuf,strlen(tmpBuf),(char *)pkexp);
        }

		
        // ģʽ
        // 1���ַ���ǰ��Կ��Ĭ��ֵ
        // 2���ַ�����Կ
        memset(flag,0,sizeof(flag));
        if ((ret = UnionReadRequestXMLPackageValue("body/flag",flag,sizeof(flag))) <= 0)
		strcpy(flag,"1");
        else
        {
                if ((flag[0] != '1') && (flag[0] != '2'))
                {
                        UnionUserErrLog("in UnionDealServiceCode8E16:: flag[%s] error!\n",flag);
                        return(errCodeParameter);
                }
        }

        // ��Կ�ַ�ƽ̨
        if (strlen(asymmetricKeyDB.keyDistributePlatform) > 0)
        {
                memset(&tkeyDistributePlatform,0,sizeof(tkeyDistributePlatform));
                if ((ret =  UnionReadRemoteKeyPlatformRec(asymmetricKeyDB.keyDistributePlatform,conRemoteKeyPlatformKeyDirectionOfDistribute,&tkeyDistributePlatform)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E16:: UnionReadRemoteKeyPlatformRec keyDistributePlatform[%s]!\n",asymmetricKeyDB.keyDistributePlatform);
                        return(ret);
                }
        }
        else
        {
                UnionUserErrLog("in UnionDealServiceCode8E16:: keyName[%s] keyDistributePlatform can not be null!\n",asymmetricKeyDB.keyName);
                UnionSetResponseRemark("�ַ�ƽ̨����Ϊ��");
		return(errCodeParameter);
        }

        // ��ʼ��Զ����Կ����������
        if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E16:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
                return(ret);
        }

	// �޸Ķ�Ӧ������ 
        if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","E125")) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E16:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","E125");
                return(ret);
        }

        if ((ret = UnionSetRequestRemoteXMLPackageValue("body/mode",flag)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E16:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/mode",flag);
                return(ret);
        }

        if (flag[0] == '2') // �ַ�����Կ
        {
                memset(asymmetricKeyDB.oldPKValue,0,sizeof(asymmetricKeyDB.oldPKValue));
                memset(asymmetricKeyDB.oldVKValue,0,sizeof(asymmetricKeyDB.oldVKValue));

                strcpy(asymmetricKeyDB.oldPKValue,asymmetricKeyDB.pkValue);
                strcpy(asymmetricKeyDB.oldVKValue,asymmetricKeyDB.vkValue);

                memset(asymmetricKeyDB.pkValue,0,sizeof(asymmetricKeyDB.pkValue));
                memset(asymmetricKeyDB.vkValue,0,sizeof(asymmetricKeyDB.vkValue));

                memset(vkValue,0,sizeof(vkValue));
                memset(pkValue,0,sizeof(pkValue));

                memset(keyType,0,sizeof(keyType));
                sprintf(keyType,"%d",asymmetricKeyDB.keyType);

                switch(asymmetricKeyDB.algorithmID)
                {
                        case    conAsymmetricAlgorithmIDOfRSA:  // RSA�㷨
				switch(phsmGroupRec->hsmCmdVersionID)
                                {
                                        case    conHsmCmdVerRacalStandardHsmCmd:
                                        case    conHsmCmdVerSJL06StandardHsmCmd:
                                                if ((lenOfPK = UnionHsmCmdEI(keyType[0],asymmetricKeyDB.keyLen,"01",
                                                        lenOfPKExp,pkexp,0,pkValue,sizeof(pkValue),&lenOfVK,(unsigned char *)vkValue,sizeof(vkValue))) < 0)
                                                {
                                                        UnionUserErrLog("in UnionDealServiceCode8E16:: UnionHsmCmdEI!\n");
                                                        return(lenOfPK);
                                                }
                                                // ��ȡ˽Կ
                                                bcdhex_to_aschex(vkValue,lenOfVK,asymmetricKeyDB.vkValue);

                                                break;
                                        default:
                                                UnionUserErrLog("in UnionDealServiceCode8E16:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
                                                UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                                                return(errCodeParameter);
                                }

                                // ��ȡ�㹫Կ
                                if ((ret = UnionGetPKOutOfRacalHsmCmdReturnStr((unsigned char *)pkValue,lenOfPK,asymmetricKeyDB.pkValue,&len,sizeof(asymmetricKeyDB.pkValue))) < 0)
                                {
                                        UnionUserErrLog("in UnionDealServiceCode8E16:: UnionGetPKOutOfRacalHsmCmdReturnStr!\n");
                                        return(ret);
                                }

                                break;
                        case    conAsymmetricAlgorithmIDOfSM2:  // SM2�㷨
				switch(phsmGroupRec->hsmCmdVersionID)
                                {
					case conHsmCmdVerRacalStandardHsmCmd:
                                        case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdK1(keyType[0],asymmetricKeyDB.keyLen,asymmetricKeyDB.pkValue,sizeof(asymmetricKeyDB.pkValue),&lenOfVK,(unsigned char *)vkValue,sizeof(vkValue))) < 0)
                                                {
							UnionUserErrLog("in UnionDealServiceCode8E16:: UnionHsmCmdK1!\n");
                                                        return(ret);
                                                }
                                                break;
					default:
                                                UnionUserErrLog("in UnionDealServiceCode8E16:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
                                                UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                                                return(errCodeParameter);
                                }
				bcdhex_to_aschex(vkValue,lenOfVK,asymmetricKeyDB.vkValue);
                                break;
                        default:
				UnionUserErrLog("in UnionDealServiceCode8E16:: asymmetricKeyDB.algorithmID[%d] is invalid!\n",asymmetricKeyDB.algorithmID);
                                return(errCodeEsscMDL_InvalidAlgorithmID);
                }
        }

        if ((ret = UnionSetRequestRemoteXMLPackageValue("body/pkValue",asymmetricKeyDB.pkValue)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E16:: UnionSetRequestRemoteXMLPackageValue[%s]!\n","body/pkValue");
                return(ret);
        }

        if (flag[0] == '1')  // �ַ�����Կ
        {
                memset(vkValue,0,sizeof(vkValue));
                lenOfVK = aschex_to_bcdhex(asymmetricKeyDB.vkValue,strlen(asymmetricKeyDB.vkValue),vkValue);
        }

        memset(remoteVK,0,sizeof(remoteVK));
	switch(asymmetricKeyDB.algorithmID)
        {	
		case    conAsymmetricAlgorithmIDOfRSA:  // RSA�㷨
			switch(phsmGroupRec->hsmCmdVersionID)
        		{
        	        	case    conHsmCmdVerRacalStandardHsmCmd:
        	        	case    conHsmCmdVerSJL06StandardHsmCmd:
        	                	if ((lenOfVK = UnionHsmCmdUG("00",conZMK,tkeyDistributePlatform.protectKey,NULL,lenOfVK,vkValue,remoteVK)) < 0)
        	                	{
        	        	                UnionUserErrLog("in UnionDealServiceCode8E16:: UnionHsmCmdUG!\n");
                		                return(lenOfVK);
                        		}
                        		break;
                		default:
                		        UnionUserErrLog("in UnionDealServiceCode8E16:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
                		        UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
					return(errCodeParameter);
        		}
			break;
		case	conAsymmetricAlgorithmIDOfSM2:	// SM2�㷨
			switch(phsmGroupRec->hsmCmdVersionID)
			{
				case	conHsmCmdVerRacalStandardHsmCmd:
				case	conHsmCmdVerSJL06StandardHsmCmd:
					// �������
					if (lenOfVK % 16 != 0)
					{
						memset(tmpBuf,0,sizeof(tmpBuf));
						addLen = 16 - lenOfVK % 16;
						lenOfVK = sprintf(tmpBuf,"%s%0*d",vkValue,addLen,0);
						memset(vkValue,0,sizeof(vkValue));
						memcpy(vkValue,tmpBuf,lenOfVK);
					}
					if ((lenOfVK = UnionHsmCmdWA(2,"01","000",1,tkeyDistributePlatform.protectKey,tkeyDistributePlatform.checkValue,NULL,lenOfVK,(unsigned char*)vkValue,remoteVK)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode8E16:: UnionHsmCmdWA!\n");
						return(lenOfVK);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCode8E16:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
					return(errCodeParameter);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCode8E16:: Distribute Key symmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}
		

        memset(vkByKek,0,sizeof(vkByKek));
        bcdhex_to_aschex(remoteVK,lenOfVK,vkByKek);
        if ((ret = UnionSetRequestRemoteXMLPackageValue("body/vkValue",vkByKek)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E16:: UnionSetRequestRemoteXMLPackageValue[%s]!\n","body/vkValue");
		return(ret);
        }

	// ת����Կ������Զ��ƽ̨
        if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E16:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
                return(ret);
	}

        // ���·ǶԳ���Կ
        if ((ret =  UnionUpdateAsymmetricKeyDBKeyValue(&asymmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E16:: UnionUpdateAsymmetricKeyDBKeyValue,keyName[%s]!\n",asymmetricKeyDB.keyName);
                return(ret);
        }

        return 0;
}


