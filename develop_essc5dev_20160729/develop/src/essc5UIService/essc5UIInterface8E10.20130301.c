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
#include "remoteKeyPlatform.h"

/***************************************
  �������:       8E10
  ������:         ���ӷǶԳ���Կ
  ��������:       ���ӷǶԳ���Կ
 ***************************************/
int UnionDealServiceCode8E10(PUnionHsmGroupRec phsmGroupRec)
{
	int				i;
	int				ret;
	int				len;
	int				hsmNum = 0;
	int				mode = 1;
	int				inputFlag = 0;
	int            	      		outputFlag = 0;
	int            	         	exportFlag = 0;
	int            	         	enabled;
	int            	         	lenOfPKExp = 0;
	int            	         	lenOfVK;
	int            	         	lenOfPK;
	int            	         	isRemoteApplyKey = 0;
	int            	         	isRemoteDistributeKey = 0;
	int                         	addLen = 0;	
	unsigned char  	         	pkexp[32];
	char                    	pkValue[512];
	char                   		vkValue[2560];
	char                    	tmpBuf[128];
	char                    	keyType[32];
	char                   		ipAddr[64];
	char                    	sql[1024];
	char                    	vkIndex[32];
	char				remoteVK[2560];
	char				vkByKek[2560];
	char				algorithmID[32];
	//int				vkIdx = 0;

	TUnionAsymmetricKeyDB   	asymmetricKeyDB;
	TUnionRemoteKeyPlatform 	tkeyApplyPlatform;
	TUnionRemoteKeyPlatform 	tkeyDistributePlatform;

	memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));

	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",asymmetricKeyDB.keyName,sizeof(asymmetricKeyDB.keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E10:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(asymmetricKeyDB.keyName);
	if (strlen(asymmetricKeyDB.keyName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E10:: keyName can not be null!\n");
		UnionSetResponseRemark("��Կ���Ʋ���Ϊ��!");
		return(errCodeParameter);
	}
	
	// ��鱾����Կ�Ƿ����
	if ((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName,0,&asymmetricKeyDB)) != errCodeKeyCacheMDL_WrongKeyName)
	{
		UnionUserErrLog("in UnionDealServiceCode8E10:: keyName[%s] is already exists!\n",asymmetricKeyDB.keyName);
		UnionSetResponseRemark("�Գ���Կ[%s]�Ѵ���",asymmetricKeyDB.keyName);
		return(errCodeKeyCacheMDL_KeyAlreadyExists);
	}	

	// ��Կ��
	if ((ret = UnionReadRequestXMLPackageValue("body/keyGroup",asymmetricKeyDB.keyGroup,sizeof(asymmetricKeyDB.keyGroup))) <= 0)
	{
		strcpy(asymmetricKeyDB.keyGroup,"default");
	}
	//add by zhouxw 20150908
	UnionSetHsmGroupIDForHsmSvr(asymmetricKeyDB.keyGroup);
	//end
	//add by zhouxw 20151013
        if ((ret = UnionGetHsmGroupRecByHsmGroupID(asymmetricKeyDB.keyGroup,phsmGroupRec)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E10:: UnionGetHsmGroupRecByHsmGroupID �������[%s]������!\n",asymmetricKeyDB.keyGroup);
                return(ret);
        }
        //add end
	// �㷨��ʶ 
	memset(algorithmID,0,sizeof(algorithmID));
	if ((ret = UnionReadRequestXMLPackageValue("body/algorithmID",algorithmID,sizeof(algorithmID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E10:: UnionReadRequestXMLPackageValue[%s]!\n","body/algorithmID");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(algorithmID);
	if (strlen(algorithmID) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E10:: algorithmID can not be null!\n");
		UnionSetResponseRemark("�㷨��ʶ����Ϊ��!");
		return(errCodeParameter);
	}
	asymmetricKeyDB.algorithmID = atoi(algorithmID);

	switch (asymmetricKeyDB.algorithmID)
	{
		case	conAsymmetricAlgorithmIDOfRSA:
			strcpy(algorithmID,"RSA");
			break;
		case	conAsymmetricAlgorithmIDOfSM2:
			strcpy(algorithmID,"SM2");
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCode8E10:: asymmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	// ��Կ����
	memset(keyType,0,sizeof(keyType));
	if ((ret = UnionReadRequestXMLPackageValue("body/keyType",keyType,sizeof(keyType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E10:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyType");
		return(ret);
	}
	if (atoi(keyType) != 0 && atoi(keyType) != 1 && atoi(keyType) != 2)
	{
		UnionUserErrLog("in UnionDealServiceCode8E10:: keyType[%s] error!\n",keyType);
		UnionSetResponseRemark("��Կ����[%s]��Ч",keyType);
		return(errCodeParameter);
	}
	asymmetricKeyDB.keyType = atoi(keyType);

	// ��Կָ��
	if (asymmetricKeyDB.algorithmID == conAsymmetricAlgorithmIDOfRSA)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("body/pkExponent",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkExponent");
			return(ret);
		}
		asymmetricKeyDB.pkExponent = atoi(tmpBuf);
		memset(tmpBuf,0,sizeof(tmpBuf));
		if (asymmetricKeyDB.pkExponent < 256)
			sprintf(tmpBuf,"%02X",asymmetricKeyDB.pkExponent);
		else if (asymmetricKeyDB.pkExponent < 256 * 256)
			sprintf(tmpBuf,"%04X",asymmetricKeyDB.pkExponent);
		else
			sprintf(tmpBuf,"%06X",asymmetricKeyDB.pkExponent);

		memset(pkexp,0,sizeof(pkexp));
		lenOfPKExp = aschex_to_bcdhex(tmpBuf,strlen(tmpBuf),(char *)pkexp);

		// ��Կ����
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("body/keyLen",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyLen");
			return(ret);
		}
		asymmetricKeyDB.keyLen = atoi(tmpBuf);
	}
	else
	{
		asymmetricKeyDB.pkExponent = 0;             
		asymmetricKeyDB.keyLen = 256;
	}

	// ˽Կ�洢λ��
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/vkStoreLocation",tmpBuf,sizeof(tmpBuf))) < 0)
		asymmetricKeyDB.vkStoreLocation = 0;
	else
		asymmetricKeyDB.vkStoreLocation = atoi(tmpBuf);

	if (asymmetricKeyDB.vkStoreLocation != 0)
	{
		// �������
		if ((ret = UnionReadRequestXMLPackageValue("body/hsmGroupID",asymmetricKeyDB.hsmGroupID,sizeof(asymmetricKeyDB.hsmGroupID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGroupID");
			return(ret);
		}
		UnionFilterHeadAndTailBlank(asymmetricKeyDB.hsmGroupID);
		if (strlen(asymmetricKeyDB.hsmGroupID) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: hsmGroupID can not be null!\n");
			UnionSetResponseRemark("������鲻��Ϊ��");
			return(errCodeParameter);
		}

		// ˽Կ�洢λ��
		memset(vkIndex,0,sizeof(vkIndex));
		if ((ret = UnionReadRequestXMLPackageValue("body/vkIndex",vkIndex,sizeof(vkIndex))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkIndex");
			return(ret);
		}
		sprintf(asymmetricKeyDB.vkIndex,"%02d",atoi(vkIndex));
		if (!UnionIsDigitString(asymmetricKeyDB.vkIndex) || (atoi(asymmetricKeyDB.vkIndex) < 0 || atoi(asymmetricKeyDB.vkIndex) > 20))
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: vkIndex[%s] error!\n",asymmetricKeyDB.vkIndex);
			UnionSetResponseRemark("�Ƿ���˽Կ����[%s],������0��20֮��",asymmetricKeyDB.vkIndex);
			return(errCodeHsmCmdMDL_InvalidIndex);
		}
	}

	// ����ʹ�þ���Կ
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/oldVersionKeyIsUsed",tmpBuf,sizeof(tmpBuf))) < 0)
		asymmetricKeyDB.oldVersionKeyIsUsed = 1;
	else
		asymmetricKeyDB.oldVersionKeyIsUsed = atoi(tmpBuf);
	if ((asymmetricKeyDB.oldVersionKeyIsUsed != 0) && (asymmetricKeyDB.oldVersionKeyIsUsed != 1))
	{
		UnionUserErrLog("in UnionDealServiceCode8E10:: oldVersionKeyIsUsed[%s] error!\n",tmpBuf);
		return(errCodeParameter);
	}

	// ģʽ
	// 0����������Կ,Ĭ��ֵ
	// 1��������Կ
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) < 0)
		mode = 1;
	else
	{
		mode = atoi(tmpBuf);
		if ((mode != 0) && (mode != 1))
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: mode[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}

	// �������ʶ
	// ��ѡ��Ĭ��ֵΪ0
	// 0��������
	// 1������
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/inputFlag",tmpBuf,sizeof(tmpBuf))) < 0)
		asymmetricKeyDB.inputFlag = 0;
	else
	{
		asymmetricKeyDB.inputFlag = atoi(tmpBuf);
		if ((asymmetricKeyDB.inputFlag != 0) && (asymmetricKeyDB.inputFlag != 1))
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: inputFlag[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}

	// ��������ʶ
	// ��ѡ��Ĭ��ֵΪ0
	// 0��������
	// 1������
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/outputFlag",tmpBuf,sizeof(tmpBuf))) <= 0)
		asymmetricKeyDB.outputFlag = 0;
	else
	{
		asymmetricKeyDB.outputFlag = atoi(tmpBuf);
		if ((asymmetricKeyDB.outputFlag != 0) && (asymmetricKeyDB.outputFlag != 1))
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: outputFlag[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}

	// ��ȡ��Ч����
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/effectiveDays",tmpBuf,sizeof(tmpBuf))) <= 0)
		asymmetricKeyDB.effectiveDays = 0;
	else
		asymmetricKeyDB.effectiveDays = atoi(tmpBuf);

	// ���ñ�ʶ
	// 0��������
	// 1���������ã�����Ч��Ĭ��ֵ
	// 2���������ã�ָ��ʱ����Ч
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/enabled",tmpBuf,sizeof(tmpBuf))) <= 0)
		enabled = 1;
	else
		enabled = atoi(tmpBuf);

	if (mode == 0)
	{
		enabled = 0;
	}

	UnionLog("in UnionDealServiceCode8E10:: mode[%d] inputFlag[%d] outputFlag[%d] enabled[%d] exportFlag[%d]\n",mode,inputFlag,outputFlag,enabled,exportFlag);

	if (mode == 0)
		asymmetricKeyDB.status = conAsymmetricKeyStatusOfInitial;       // ״̬����Ϊ��ʼ��״̬
	else
	{
		if (enabled == 0)
			asymmetricKeyDB.status = conAsymmetricKeyStatusOfInitial;       // ״̬����Ϊ��ʼ��״̬
		else
			asymmetricKeyDB.status = conAsymmetricKeyStatusOfEnabled;       // ״̬����Ϊ����״̬
	}

	// ��Կ����ƽ̨
	if ((ret = UnionReadRequestXMLPackageValue("body/keyApplyPlatform",asymmetricKeyDB.keyApplyPlatform,sizeof(asymmetricKeyDB.keyApplyPlatform))) > 0)
	{
		memset(&tkeyApplyPlatform,0,sizeof(tkeyApplyPlatform));
		if ((ret =  UnionReadRemoteKeyPlatformRec(asymmetricKeyDB.keyApplyPlatform,conRemoteKeyPlatformKeyDirectionOfApply,&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionReadRemoteKeyPlatformRec keyApplyPlatform[%s]!\n",asymmetricKeyDB.keyApplyPlatform);
			return(ret);
		}
		isRemoteApplyKey = 1;
	}

	// ��Կ�ַ�ƽ̨
	if ((ret = UnionReadRequestXMLPackageValue("body/keyDistributePlatform",asymmetricKeyDB.keyDistributePlatform,sizeof(asymmetricKeyDB.keyDistributePlatform))) > 0)
	{
		memset(&tkeyDistributePlatform,0,sizeof(tkeyDistributePlatform));
		if ((ret =  UnionReadRemoteKeyPlatformRec(asymmetricKeyDB.keyDistributePlatform,conRemoteKeyPlatformKeyDirectionOfDistribute,&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionReadRemoteKeyPlatformRec keyDistributePlatform[%s]!\n",asymmetricKeyDB.keyDistributePlatform);
			return(ret);	
		}
		isRemoteDistributeKey = 1;
	}

	// ���ַ��������Ƿ�ͬһƽ̨
	if (isRemoteApplyKey && isRemoteDistributeKey)
	{
		if (strcmp(tkeyApplyPlatform.ipAddr,tkeyDistributePlatform.ipAddr) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: tkeyApplyPlatform.ipAddr[%s] == tkeyDistributePlatform.ipAddr[%s] is error!\n",tkeyApplyPlatform.ipAddr,tkeyDistributePlatform.ipAddr);	
			UnionSetResponseRemark("����ͷַ�ƽ̨������ͬ");
			return(errCodeParameter);
		}
	}

	if (isRemoteApplyKey)		// Զ��������Կ
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// �޸Ķ�Ӧ������ 
		if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","E120")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","E120");
			return(ret);
		}


		// �޸���Կ�㷨��ʶ
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/algorithmID",algorithmID)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/algorithmID",algorithmID);
			return(ret);
		}

		// ����Կ�ַ�ƽ̨�ֶ��ÿ�
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyDistributePlatform","")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyDistributePlatform","");
			return(ret);
		}

		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}

		// ����Կ�ַ�ƽ̨�ֶλ�ԭ
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyDistributePlatform",asymmetricKeyDB.keyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyDistributePlatform",asymmetricKeyDB.keyDistributePlatform);
			return(ret);
		}
	}

	// ��ȡ��ע
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",asymmetricKeyDB.remark,sizeof(asymmetricKeyDB.remark))) <= 0)
		strcpy(asymmetricKeyDB.remark,"");

	// �����Կ����
	if (!UnionIsValidAsymmetricKeyDB(&asymmetricKeyDB))
	{
		UnionUserErrLog("in UnionDealServiceCode8E10:: UnionIsValidAsymmetricKeyDB!\n");
		return(errCodeKeyCacheMDL_WrongKeyName);
	}

	memset(pkValue,0,sizeof(pkValue));
	memset(vkValue,0,sizeof(vkValue));
	switch(mode)
	{
		case 0:
			break;
		case 1:
			if (isRemoteApplyKey) // Զ������
			{
				memset(remoteVK,0,sizeof(remoteVK));
				if((ret = UnionReadResponseRemoteXMLPackageValue("body/vkValue",remoteVK,sizeof(remoteVK))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E10:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/vkValue");
					return(ret);
				}

				if((ret = UnionReadResponseRemoteXMLPackageValue("body/pkValue",asymmetricKeyDB.pkValue,sizeof(asymmetricKeyDB.pkValue))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E10:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/pkValue");
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
									UnionUserErrLog("in UnionDealServiceCode8E10:: UnionHsmCmdUI2!\n");
									return(lenOfVK);
								}
								memset(vkValue,0,sizeof(vkValue));
								lenOfVK = aschex_to_bcdhex(asymmetricKeyDB.vkValue,strlen(asymmetricKeyDB.vkValue),vkValue);
								if (asymmetricKeyDB.keyType == 0)
									asymmetricKeyDB.vkValue[3] = '0';
								else if (asymmetricKeyDB.keyType == 1)
									asymmetricKeyDB.vkValue[3] = '1';
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCode8E10:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
								return(errCodeParameter);
						}
						break;
					case	conAsymmetricAlgorithmIDOfSM2:
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case	conHsmCmdVerRacalStandardHsmCmd:
							case	conHsmCmdVerSJL06StandardHsmCmd:
								memset(vkValue,0,sizeof(vkValue));
								lenOfVK = aschex_to_bcdhex(remoteVK,strlen(remoteVK),vkValue);
								memset(tmpBuf,0,sizeof(tmpBuf));
								// �������
								if (lenOfVK % 16 != 0)
								{
									addLen = 16 - lenOfVK % 16;
									sprintf(vkValue + lenOfVK,"%0*d",addLen,0);
									lenOfVK += addLen;
								}
								if ((lenOfVK = UnionHsmCmdWA(1,"01","000",1,tkeyApplyPlatform.protectKey,tkeyApplyPlatform.checkValue,NULL,lenOfVK,(unsigned char*)vkValue,tmpBuf)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCode8E10:: UnionHsmCmdWA!\n");
									return(lenOfVK);
								}
								memset(asymmetricKeyDB.vkValue,0,sizeof(asymmetricKeyDB.vkValue));
								memset(vkValue,0,sizeof(vkValue));
								bcdhex_to_aschex(tmpBuf,lenOfVK,asymmetricKeyDB.vkValue);
								sprintf(vkValue,tmpBuf);
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCode8E10:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
								return(errCodeParameter);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E10:: asymmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
						return(errCodeEsscMDL_InvalidAlgorithmID);
				}
			}
			else
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
									UnionUserErrLog("in UnionDealServiceCode8E10:: UnionHsmCmdEI!\n");
									return(lenOfPK);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCode8E10:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
								return(errCodeParameter);
						}

						// ��ȡ�㹫Կ
						if ((ret = UnionGetPKOutOfRacalHsmCmdReturnStr((unsigned char *)pkValue,lenOfPK,asymmetricKeyDB.pkValue,&len,sizeof(asymmetricKeyDB.pkValue))) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode8E10:: UnionGetPKOutOfRacalHsmCmdReturnStr!\n");
							return(ret);
						}

						strcpy(pkValue,asymmetricKeyDB.pkValue);

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
									UnionUserErrLog("in UnionDealServiceCode8E10:: UnionHsmCmdK1!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCode8E10:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
								return(errCodeParameter);
						}
						bcdhex_to_aschex(vkValue,lenOfVK,asymmetricKeyDB.vkValue);
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E10:: asymmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
						return(errCodeEsscMDL_InvalidAlgorithmID);
				}
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCode8E10:: mode = [%d] error!\n",mode);
			return(errCodeEsscMDLKeyOperationNotPermitted);
	}

	if (isRemoteDistributeKey)	// Զ�̷ַ���Կ
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// �޸Ķ�Ӧ������ 
		if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","E120")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","E120");
			return(ret);
		}

		// �޸��㷨��ʶ
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/algorithmID",algorithmID)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/algorithmID",algorithmID);
			return(ret);
		}

		// ����Կ����ƽ̨�ֶ��ÿ�
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyApplyPlatform","")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyApplyPlatform","");
			return(ret);
		}

		if (mode == 1)	// ������Կ
		{
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
								UnionUserErrLog("in UnionDealServiceCode8E10:: UnionHsmCmdUG!\n");
								return(lenOfVK);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCode8E10:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
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
								addLen = 16 - lenOfVK % 16;
								sprintf(vkValue + lenOfVK,"%0*d",addLen,0);
								lenOfVK += addLen;
							}
							if ((lenOfVK = UnionHsmCmdWA(2,"01","000",1,tkeyDistributePlatform.protectKey,tkeyDistributePlatform.checkValue,NULL,lenOfVK,(unsigned char*)vkValue,remoteVK)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E10:: UnionHsmCmdWA!\n");
								return(lenOfVK);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCode8E10:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCode8E10:: Distribute Key symmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}

			memset(vkByKek,0,sizeof(vkByKek));
			bcdhex_to_aschex(remoteVK,lenOfVK,vkByKek);
			if ((ret = UnionSetRequestRemoteXMLPackageValue("body/vkValue",vkByKek)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E10:: UnionSetRequestRemoteXMLPackageValue[%s]!\n","body/vkValue");
				return(ret);
			}
			if ((ret = UnionSetRequestRemoteXMLPackageValue("body/pkValue",asymmetricKeyDB.pkValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E10:: UnionSetRequestRemoteXMLPackageValue[%s]!\n","body/pkValue");
				return(ret);
			}
		}
		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}

		// ����Կ����ƽ̨�ֶλ�ԭ
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyApplyPlatform",asymmetricKeyDB.keyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyApplyPlatform",asymmetricKeyDB.keyApplyPlatform);
			return(ret);
		}
	}

	if (asymmetricKeyDB.vkStoreLocation != 0)	// ˽Կ�洢���������
	{
		if (asymmetricKeyDB.vkStoreLocation == 1)
			memset(asymmetricKeyDB.vkValue,0,sizeof(asymmetricKeyDB.vkValue));

		// ���˽Կ�����Ƿ�ʹ��
		snprintf(sql,sizeof(sql),"select status from vkKeyIndex where hsmGroupID = '%s' and vkIndex = '%s' and status = 0 and algorithmID= %d",asymmetricKeyDB.hsmGroupID,asymmetricKeyDB.vkIndex,asymmetricKeyDB.algorithmID);
		
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionExecRealDBSql[%s]!\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: vkIndex[%s] is already use!\n",asymmetricKeyDB.vkIndex);
			UnionSetResponseRemark("������[%s]�Ѿ���ʹ��,��ѡ������������",asymmetricKeyDB.vkIndex);
			return(errCodeParameter);
		}

		// add by lisq 20141210 �����ж����� generate key value 
		if (mode == 1)
		{
			memset(sql,0,sizeof(sql));
			sprintf(sql,"select * from hsm where hsmGroupID = '%s' and enabled = 1",asymmetricKeyDB.hsmGroupID);
			if ((hsmNum = UnionSelectRealDBRecord(sql,0,0)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E10:: UnionExecRealDBSql[%s]!\n",sql);
				return(hsmNum);
			}
			else if (hsmNum == 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E10:: hsmGroupID[%s]��û�п��������!\n",asymmetricKeyDB.hsmGroupID);
				return(errCodeParameter);
			}

			for (i = 0; i < hsmNum; i++)
			{
				UnionLocateXMLPackage("detail", i+1);

				memset(ipAddr,0,sizeof(ipAddr));
				if ((ret = UnionReadXMLPackageValue("ipAddr", ipAddr, sizeof(ipAddr))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E10:: UnionReadXMLPackageValue[%s]!\n","ipAddr");
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
								UnionLog("in UnionDealServiceCode8E10:: [%d]!\n",lenOfVK);
								if ((ret = UnionHsmCmdEK(atoi(asymmetricKeyDB.vkIndex),lenOfVK,(unsigned char *)vkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCode8E10:: UnionHsmCmdEK hsm[%s] vkIndex[%s]!\n",ipAddr,asymmetricKeyDB.vkIndex);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCode8E10:: phsmGroupRec->hsmCmdVersionID[%d] invalid\n",phsmGroupRec->hsmCmdVersionID);
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
									UnionUserErrLog("in UnionDealServiceCode8E10:: UnionHsmCmdK2 hsm[%s] vkIndex[%s]!\n",ipAddr,asymmetricKeyDB.vkIndex);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCode8E10:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
								return(errCodeParameter);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode8E10:: asymmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
						return(errCodeEsscMDL_InvalidAlgorithmID);
				}
			}
		}

		snprintf(sql,sizeof(sql),"update vkKeyIndex set status = 1 where hsmGroupID = '%s' and vkIndex = '%s' and algorithmID= %d",asymmetricKeyDB.hsmGroupID,asymmetricKeyDB.vkIndex,asymmetricKeyDB.algorithmID);
		if ((ret = UnionExecRealDBSql(sql)) <= 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionExecRealDBSql[%s]!\n",sql);
			UnionSetResponseRemark("����˽Կ״̬ʧ��");
			return(ret);
		}
	}

	// ��Ч����
	// ��enabledΪ2ʱ������
	if (enabled == 1)
	{
		UnionGetFullSystemDate(asymmetricKeyDB.activeDate);
	}
	else if (enabled == 2)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/activeDate",asymmetricKeyDB.activeDate,sizeof(asymmetricKeyDB.activeDate))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E10:: UnionReadRequestXMLPackageValue[%s]!\n","body/activeDate");
			return(ret);
		}
	}

	asymmetricKeyDB.creatorType = conAsymmetricCreatorTypeOfUser;

	if ((ret = UnionReadRequestXMLPackageValue("head/userID",asymmetricKeyDB.creator,sizeof(asymmetricKeyDB.creator))) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E10:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);
	}

	if ((ret = UnionReadRequestXMLPackageValue("body/usingUnit",asymmetricKeyDB.usingUnit,sizeof(asymmetricKeyDB.usingUnit))) <= 0)
	{
		strcpy(asymmetricKeyDB.usingUnit,"");
	}

	if (strlen(asymmetricKeyDB.keyName) > 0)
	{
		if (mode == 0)  // ��������Կ
		{
			if ((ret =  UnionCreateAsymmetricKeyDB(&asymmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E10:: UnionCreateAsymmetricKeyDB,keyName[%s]!\n",asymmetricKeyDB.keyName);
				return(ret);
			}
		}
		else
		{
			if ((ret =  UnionGenerateAsymmetricKeyDBRec(&asymmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E10:: UnionGenerateAsymmetricKeyDBRec,keyName[%s]!\n",asymmetricKeyDB.keyName);
				return(ret);
			}
		}
	}

	return(0);
}


