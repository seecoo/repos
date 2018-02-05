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
#include "asymmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "commWithHsmSvr.h"
#include "unionHsmCmdVersion.h"

/***************************************
�������:       8E13
������:         ���·ǶԳ���Կ
��������:       ���·ǶԳ���Կ
***************************************/
int UnionDealServiceCode8E13(PUnionHsmGroupRec phsmGroupRec)
{
        int                     	i;
        int                     	ret;
        int                     	len;
        int                     	hsmNum = 0;
        int                     	lenOfPKExp = 0;
        int                     	lenOfVK;
        int                     	lenOfPK;
        int				addLen = 0;
	unsigned char           	pkexp[32];
        char                    	pkValue[512];
        char                   		vkValue[2560];
        char                    	tmpBuf[1024];
        char                    	keyType[32];
        char                   		ipAddr[64];
        char           		        sql[1024];
	int				isRemoteApplyKey = 0;
	int				isRemoteDistributeKey = 0;
	char				vkByKek[2560];
	char				remoteVK[2560];

        TUnionAsymmetricKeyDB   	asymmetricKeyDB;
	TUnionRemoteKeyPlatform		tkeyApplyPlatform;
	TUnionRemoteKeyPlatform		tkeyDistributePlatform;

        memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));

        // ��Կ����
        if ((ret = UnionReadRequestXMLPackageValue("body/keyName",asymmetricKeyDB.keyName,sizeof(asymmetricKeyDB.keyName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E13:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
                return(ret);
        }
        UnionFilterHeadAndTailBlank(asymmetricKeyDB.keyName);
        if (strlen(asymmetricKeyDB.keyName) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E13:: keyName can not be null!\n");
                UnionSetResponseRemark("��Կ���Ʋ���Ϊ��!");
                return(errCodeParameter);
        }

        // ��ȡ�ǶԳ���Կ
        if ((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName,0,&asymmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E13:: UnionReadAsymmetricKeyDBRec[%s]!\n",asymmetricKeyDB.keyName);
                return(ret);
        }
        UnionLog("in UnionDealServiceCode8E13::1  pkValue[%s] vkValue[%s] keyType[%d]  keyLen[%d],algorithmID[%d], pkExponent[%d]\n",asymmetricKeyDB.pkValue,asymmetricKeyDB.vkValue,asymmetricKeyDB.keyType,asymmetricKeyDB.keyLen,asymmetricKeyDB.algorithmID,asymmetricKeyDB.pkExponent);
	
	//add by zhouxw
	UnionSetHsmGroupIDForHsmSvr(asymmetricKeyDB.keyGroup);
	//end
	//add by zhouxw 20151013
        if ((ret = UnionGetHsmGroupRecByHsmGroupID(asymmetricKeyDB.keyGroup,phsmGroupRec)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E13:: UnionGetHsmGroupRecByHsmGroupID �������[%s]������!\n",asymmetricKeyDB.keyGroup);
                return(ret);
        }
        //add end
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

        memset(pkValue,0,sizeof(pkValue));
        memset(vkValue,0,sizeof(vkValue));

        // ����ǰ˽Կ����Կ�Լ�У��ֵ����Ϊ�ɵ�
        memset(asymmetricKeyDB.oldPKValue,0,sizeof(asymmetricKeyDB.oldPKValue));
        memset(asymmetricKeyDB.oldVKValue,0,sizeof(asymmetricKeyDB.oldVKValue));

        strcpy(asymmetricKeyDB.oldPKValue,asymmetricKeyDB.pkValue);
        strcpy(asymmetricKeyDB.oldVKValue,asymmetricKeyDB.vkValue);

        memset(asymmetricKeyDB.pkValue,0,sizeof(asymmetricKeyDB.pkValue));
        memset(asymmetricKeyDB.vkValue,0,sizeof(asymmetricKeyDB.vkValue));

        memset(keyType,0,sizeof(keyType));
        sprintf(keyType,"%d",asymmetricKeyDB.keyType);

	// ��Կ����ƽ̨
	if (strlen(asymmetricKeyDB.keyApplyPlatform) > 0)
	{
		memset(&tkeyApplyPlatform,0,sizeof(tkeyApplyPlatform));
		if ((ret =  UnionReadRemoteKeyPlatformRec(asymmetricKeyDB.keyApplyPlatform,conRemoteKeyPlatformKeyDirectionOfApply,&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E13:: UnionReadRemoteKeyPlatformRec keyApplyPlatform[%s]!\n",asymmetricKeyDB.keyApplyPlatform);
			return(ret);	
		}
		isRemoteApplyKey = 1;
	}

	// ��Կ�ַ�ƽ̨
	if (strlen(asymmetricKeyDB.keyDistributePlatform) > 0)
	{
		memset(&tkeyDistributePlatform,0,sizeof(tkeyDistributePlatform));
		if ((ret =  UnionReadRemoteKeyPlatformRec(asymmetricKeyDB.keyDistributePlatform,conRemoteKeyPlatformKeyDirectionOfDistribute,&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E13:: UnionReadRemoteKeyPlatformRec keyDistributePlatform[%s]!\n",asymmetricKeyDB.keyDistributePlatform);
			return(ret);	
		}
		isRemoteDistributeKey = 1;
	}

	// ���ַ��������Ƿ�ͬһƽ̨
	if (isRemoteApplyKey && isRemoteDistributeKey)
	{
		if (strcmp(tkeyApplyPlatform.ipAddr,tkeyDistributePlatform.ipAddr) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E13:: tkeyApplyPlatform.ipAddr[%s] == tkeyDistributePlatform.ipAddr[%s] is error!\n",tkeyApplyPlatform.ipAddr,tkeyDistributePlatform.ipAddr);	
			UnionSetResponseRemark("����ͷַ�ƽ̨������ͬ");
			return(errCodeParameter);
		}
	}

	// Զ��������Կ
	if (isRemoteApplyKey)
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E13:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

                // �޸Ķ�Ӧ������ 
                if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","E121")) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E03:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","E121");
                        return(ret);
                }

		// ���������־ 1:����
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/remoteOperateFlag","1")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E13:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/remoteOperateFlag","1");
			return(ret);
		}

		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E13:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}

		memset(remoteVK,0,sizeof(remoteVK));
		if((ret = UnionReadResponseRemoteXMLPackageValue("body/vkValue",remoteVK,sizeof(remoteVK))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E13:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/vkValue");
			return(ret);
		}

		if((ret = UnionReadResponseRemoteXMLPackageValue("body/pkValue",asymmetricKeyDB.pkValue,sizeof(asymmetricKeyDB.pkValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E13:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/pkValue");
			return(ret);
		}
		switch(asymmetricKeyDB.algorithmID)
		{
			case	conAsymmetricAlgorithmIDOfRSA:	// RSA�㷨
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						memset(vkValue,0,sizeof(vkValue));
						lenOfVK = aschex_to_bcdhex(remoteVK,strlen(remoteVK),vkValue);
						if ((lenOfVK = UnionHsmCmdUI2("00",NULL,strlen(tkeyApplyPlatform.protectKey),tkeyApplyPlatform.protectKey,NULL,vkValue,lenOfVK,(unsigned char*)asymmetricKeyDB.vkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode8E13:: UnionHsmCmdUI2!\n");
							return(lenOfVK);
						}
						memset(vkValue,0,sizeof(vkValue));
						lenOfVK = aschex_to_bcdhex(asymmetricKeyDB.vkValue,strlen(asymmetricKeyDB.vkValue),vkValue);				
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E13:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
				break;
			case	conAsymmetricAlgorithmIDOfSM2:
				switch(phsmGroupRec->hsmCmdVersionID)
				{
                                        case    conHsmCmdVerRacalStandardHsmCmd:
                                        case    conHsmCmdVerSJL06StandardHsmCmd:
                                        	memset(vkValue,0,sizeof(vkValue));
                                        	lenOfVK = aschex_to_bcdhex(remoteVK,strlen(remoteVK),vkValue);
                                        	// �������
                                        	if (lenOfVK % 16 != 0)
                                        	{
                                                        addLen = 16 - lenOfVK % 16;
                                        		sprintf(vkValue + lenOfVK,"%0*d",addLen,0);
							lenOfVK += addLen;
						}
                                                
						memset(tmpBuf,0,sizeof(tmpBuf));
                                        	if ((lenOfVK = UnionHsmCmdWA(1,"01","000",1,tkeyApplyPlatform.protectKey,tkeyApplyPlatform.checkValue,NULL,lenOfVK,(unsigned char*)vkValue,tmpBuf)) < 0)
                                                {
                                                        UnionUserErrLog("in UnionDealServiceCode8E13:: UnionHsmCmdWA!\n");
                                                        return(lenOfVK);
                                                }
						memset(asymmetricKeyDB.vkValue,0,sizeof(asymmetricKeyDB.vkValue));
						bcdhex_to_aschex(tmpBuf,lenOfVK,asymmetricKeyDB.vkValue);
                                                break;
                                        default:
                                                UnionUserErrLog("in UnionDealServiceCode8E13:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                                                UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                                                return(errCodeParameter);
                                }
                                                break;
			default:
				UnionUserErrLog("in UnionDealServiceCode8E13:: asymmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
	}
	else	// ��������
	{
		switch(asymmetricKeyDB.algorithmID)
		{
			case    conAsymmetricAlgorithmIDOfRSA:  // RSA�㷨
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((lenOfPK = UnionHsmCmdEI(keyType[0],asymmetricKeyDB.keyLen,"01",
								lenOfPKExp,pkexp,0,pkValue,sizeof(pkValue),&lenOfVK,(unsigned char *)vkValue,sizeof(vkValue))) < 0)
						{
								UnionUserErrLog("in UnionDealServiceCode8E13:: UnionHsmCmdEI!\n");
								return(lenOfPK);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E13:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}

				// ��ȡ�㹫Կ
				if ((ret = UnionGetPKOutOfRacalHsmCmdReturnStr((unsigned char *)pkValue,lenOfPK,asymmetricKeyDB.pkValue,&len,sizeof(asymmetricKeyDB.pkValue))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E13:: UnionGetPKOutOfRacalHsmCmdReturnStr!\n");
					return(ret);
				}

				// ��ȡ˽Կ
				bcdhex_to_aschex(vkValue,lenOfVK,asymmetricKeyDB.vkValue);
				break;
			case    conAsymmetricAlgorithmIDOfSM2:  // SM2�㷨
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdK1(keyType[0],asymmetricKeyDB.keyLen,asymmetricKeyDB.pkValue,sizeof(asymmetricKeyDB.pkValue),&lenOfVK,(unsigned char *)vkValue,sizeof(vkValue))) < 0)
						{
								UnionUserErrLog("in UnionDealServiceCode8E13:: UnionHsmCmdK1!\n");
								return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E13:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
				bcdhex_to_aschex(vkValue,lenOfVK,asymmetricKeyDB.vkValue);
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCode8E13:: asymmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
	}

	if (isRemoteDistributeKey)	// Զ�̷ַ���Կ
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E13:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// �޸Ķ�Ӧ������ 
		if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","E121")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E03:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","E121");
			return(ret);
		}

		// ���������ʶ 2:�ַ�
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/remoteOperateFlag","2")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E13:: UnionSetRequestRemoteXMLPackageValue[%s]!\n","body/remoteOperateFlag");
			return(ret);
		}

		// ������Կ��У��ֵ
		memset(remoteVK,0,sizeof(remoteVK));
		switch(asymmetricKeyDB.algorithmID)
		{
			case	conAsymmetricAlgorithmIDOfRSA:	// RSA�㷨
				// �ѱ���LMK�Լ���ת��Զ��Kek����
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:
						if ((lenOfVK = UnionHsmCmdUG("00",conZMK,tkeyDistributePlatform.protectKey,NULL,lenOfVK,vkValue,remoteVK)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode8E13:: UnionHsmCmdUG!\n");
							return(lenOfVK);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E13:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
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
							UnionUserErrLog("in UnionDealServiceCode8E13:: UnionHsmCmdWA!\n");
							return(lenOfVK);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E13:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
				break;	
			default:
				UnionUserErrLog("in UnionDealServiceCode8E13:: Distribute Key symmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}

		memset(vkByKek,0,sizeof(vkByKek));
		bcdhex_to_aschex(remoteVK,lenOfVK,vkByKek);
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/vkValue",vkByKek)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E13:: UnionSetRequestRemoteXMLPackageValue[%s]!\n","body/vkValue");
			return(ret);
		}
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/pkValue",asymmetricKeyDB.pkValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E13:: UnionSetRequestRemoteXMLPackageValue[%s]!\n","body/pkValue");
			return(ret);
		}

		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E13:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

        if (asymmetricKeyDB.vkStoreLocation != 0)       // ˽Կ�洢���������
        {
		if (asymmetricKeyDB.vkStoreLocation == 1)
			memset(asymmetricKeyDB.vkValue,0,sizeof(asymmetricKeyDB.vkValue));

                memset(sql,0,sizeof(sql));
                sprintf(sql,"select * from hsm where hsmGroupID = '%s' and enabled = 1",UnionGetHsmGroupIDForHsmSvr());
                if ((hsmNum = UnionSelectRealDBRecord(sql,0,0)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E13:: UnionExecRealDBSql[%s]!\n",sql);
                        return(hsmNum);
                }
                else if (hsmNum == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E13:: hsmGroupID[%s]��û�п��������!\n",UnionGetHsmGroupIDForHsmSvr());
                        return(errCodeParameter);
                }

                for (i = 0; i < hsmNum; i++)
                {
                        UnionLocateXMLPackage("detail", i+1);

                        memset(ipAddr,0,sizeof(ipAddr));
                        if ((ret = UnionReadXMLPackageValue("ipAddr", ipAddr, sizeof(ipAddr))) < 0)
                        {
                                UnionUserErrLog("in UnionDealServiceCode8E13:: UnionReadXMLPackageValue[%s]!\n","ipAddr");
                                return(ret);
                        }
                        UnionSetUseSpecHsmIPAddrForOneCmd(ipAddr);
                        switch(asymmetricKeyDB.algorithmID)
                        {
                                case    conAsymmetricAlgorithmIDOfRSA:  // RSA�㷨
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case	conHsmCmdVerRacalStandardHsmCmd:
						case	conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdEK(atoi(asymmetricKeyDB.vkIndex),lenOfVK,(unsigned char *)vkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E13:: UnionHsmCmdEK hsm[%s] vkIndex[%s]!\n",ipAddr,asymmetricKeyDB.vkIndex);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCode8E13:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
                                        break;
                                case    conAsymmetricAlgorithmIDOfSM2:  // SM2�㷨
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case	conHsmCmdVerRacalStandardHsmCmd:
						case	conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdK2(atoi(asymmetricKeyDB.vkIndex),lenOfVK,(unsigned char *)vkValue)) < 0)
							{
									UnionUserErrLog("in UnionDealServiceCode8E13:: UnionHsmCmdK2 hsm[%s] vkIndex[%s]!\n",ipAddr,asymmetricKeyDB.vkIndex);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCode8E13:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
                                        break;
                                default:
                                        UnionUserErrLog("in UnionDealServiceCode8E13:: asymmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
                                        return(errCodeEsscMDL_InvalidAlgorithmID);
                        }
                }
        }

        // ���·ǶԳ���Կ
        if ((ret =  UnionUpdateAsymmetricKeyDBKeyValue(&asymmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E13:: UnionUpdateAsymmetricKeyDBKeyValue,keyName[%s]!\n",asymmetricKeyDB.keyName);
                return(ret);
        }

        return(0);
}


