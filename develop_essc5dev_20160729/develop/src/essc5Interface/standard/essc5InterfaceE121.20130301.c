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
#include "asymmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "commWithHsmSvr.h"
#include "unionHsmCmdVersion.h"

/***************************************
�������:	E121
������:		���·ǶԳ���Կ
��������:	���·ǶԳ���Կ
***************************************/
int UnionDealServiceCodeE121(PUnionHsmGroupRec phsmGroupRec)
{
	int				i;
	int				ret;
	int				len;
	int				hsmNum = 0;
	int				mode = 1;
	int				exportFlag = 0;
	int				lenOfPKExp = 0;
	int				lenOfVK;
	int				lenOfPK;
	int				isDistributeFlag = 0;
	int				isApplyFlag = 0;
	int				isRemoteApplyKey = 0;
	int				isRemoteDistributeKey = 0;
	int				isRemoteKeyOperate = 0;
	int				addLen = 0;
	unsigned char			pkexp[32];
	char				pkValue[1024];
	char				vkValue[2560];
	char				tmpBuf[2560];
	char				keyType[32];
	char				ipAddr[64];
	char				sql[1024];
	char				platformProtectKey[64];
	char				platformCheckValue[32];
	char				remoteVK[2560];
	char				vkByKek[2560];
	char				keyName[160];

	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	TUnionRemoteKeyPlatform		tkeyApplyPlatform;
	TUnionRemoteKeyPlatform		tkeyDistributePlatform;
	
	// ���Զ�̱�ʶ
	isRemoteKeyOperate = UnionIsRemoteKeyOperate();
	
	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE121:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);

	// ��ȡ�ǶԳ���Կ
	if ((ret = UnionReadAsymmetricKeyDBRec(keyName,0,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE121:: UnionReadAsymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}

	// ��Կָ��
	if (asymmetricKeyDB.algorithmID == conAsymmetricAlgorithmIDOfRSA)
	{
		if (asymmetricKeyDB.pkExponent < 256)
			snprintf(tmpBuf,sizeof(tmpBuf),"%02X",asymmetricKeyDB.pkExponent);
		else if (asymmetricKeyDB.pkExponent < 256 * 256)
			snprintf(tmpBuf,sizeof(tmpBuf),"%04X",asymmetricKeyDB.pkExponent);
		else
			snprintf(tmpBuf,sizeof(tmpBuf),"%06X",asymmetricKeyDB.pkExponent);
		
		lenOfPKExp = aschex_to_bcdhex(tmpBuf,strlen(tmpBuf),(char *)pkexp);
	}
	
	// ģʽ
	// 1��������Կ��������㹫Կ
	// 2��������Կ�������DER���빫Կ
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) <= 0)
		mode = 1;
	else
	{
		tmpBuf[ret] = 0;
		mode = atoi(tmpBuf);
		if ((mode != 1) && (mode != 2))
		{
			UnionUserErrLog("in UnionDealServiceCodeE121:: mode[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}
	
	// �����ʶ
	// 0���������Ĭ��ֵ
	// 1��LMK�Ա������
	if ((ret = UnionReadRequestXMLPackageValue("body/exportFlag",tmpBuf,sizeof(tmpBuf))) <= 0)
		exportFlag = 0;
	else
	{
		tmpBuf[ret] = 0;
		exportFlag = atoi(tmpBuf);
	}
	
	UnionLog("in UnionDealServiceCodeE121:: mode[%d] exportFlag[%d]\n",mode,exportFlag);
	
	
	// �����Կ����
	if (!UnionIsValidAsymmetricKeyDB(&asymmetricKeyDB))
	{
		UnionUserErrLog("in UnionDealServiceCodeE121:: UnionIsValidAsymmetricKeyDB!\n");
		return(errCodeKeyCacheMDL_WrongKeyName);	
	}
	
	// ����ǰ˽Կ����Կ������Ϊ�ɵ�
	strcpy(asymmetricKeyDB.oldPKValue,asymmetricKeyDB.pkValue);
	strcpy(asymmetricKeyDB.oldVKValue,asymmetricKeyDB.vkValue);

	asymmetricKeyDB.pkValue[0] = 0;
	asymmetricKeyDB.vkValue[0] = 0;
	
	snprintf(keyType,sizeof(keyType),"%d",asymmetricKeyDB.keyType);

	if (!isRemoteKeyOperate)		// ������Կ����	
	{
		// modify by leipp 20151209
		// ��ȡ���������ͷַ�ƽ̨
		if ((ret = UnionCheckRemoteKeyPlatform(asymmetricKeyDB.keyName, asymmetricKeyDB.keyApplyPlatform, asymmetricKeyDB.keyDistributePlatform, &tkeyApplyPlatform, &tkeyDistributePlatform, &isRemoteApplyKey, &isRemoteDistributeKey)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE121:: UnionCheckRemoteKeyPlatform!\n");
			return(ret);
		}
		// modify by leipp end
	}
	else			// Զ����Կ����
	{
		isRemoteApplyKey = 0;
		isRemoteDistributeKey = 0;

		// ��ȡԶ�̲�����ʶ
		if ((ret = UnionReadRequestXMLPackageValue("body/remoteOperateFlag",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE121:: UnionReadRequestXMLPackageValue[%s]!\n","body/remoteOperateFlag");
			return(ret);
		}
		if (tmpBuf[0] == '1')
			isApplyFlag = 1;
		else
			isDistributeFlag = 1;
		
		// ϵͳID
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE121:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);
		}
		tmpBuf[ret] = 0;

		//��ȡ������Կ
		snprintf(sql,sizeof(sql),"select * from localKeyPlatform where sysID = '%s' and algorithmID = %d",tmpBuf,asymmetricKeyDB.algorithmID);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCodeE121:: UnionSelectRealDBRecord[%s]\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE121:: protectKey not found!\n");
			UnionSetResponseRemark("Զ�̱�����Կ������");
			return(errCodeParameter);
		}

		if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE121:: UnionLocateXMLPackage[%s]!\n","detail");	
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("protectKey",platformProtectKey,sizeof(platformProtectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE121:: UnionReadXMLPackageValue[%s]!\n","protectKey");
			return(ret);
		}
		
		if ((ret = UnionReadXMLPackageValue("checkValue",platformCheckValue,sizeof(platformCheckValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE121:: UnionReadXMLPackageValue[%s]!\n","checkValue");
			return(ret);
		}
	}

	// Զ��������Կ
	if (isRemoteApplyKey)
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE121:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// ���������־ 1:����
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/remoteOperateFlag","1")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE121:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/remoteOperateFlag","1");
			return(ret);
		}

		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE121:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	switch(mode)
	{
		case 1:
		case 2:
			if (isRemoteApplyKey) // Զ������
			{
				if((ret = UnionReadResponseRemoteXMLPackageValue("body/vkValue",remoteVK,sizeof(remoteVK))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE121:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/vkValue");
					return(ret);
				}

				if((ret = UnionReadResponseRemoteXMLPackageValue("body/pkValue",asymmetricKeyDB.pkValue,sizeof(asymmetricKeyDB.pkValue))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE121:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/pkValue");
					return(ret);
				}
				strcpy(platformProtectKey,tkeyApplyPlatform.protectKey);
				// add by leipp 20160126,����ʱ���ع�Կ
				snprintf(pkValue,sizeof(pkValue),"%s",asymmetricKeyDB.pkValue);
				// add end
			}
			if (isDistributeFlag)	// Զ�̴洢
			{
				// ��ȡԶ����Կֵ
				if((ret = UnionReadRequestXMLPackageValue("body/vkValue",remoteVK,sizeof(remoteVK))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE121:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkValue");
					return(ret);
				}

				// ��ȡԶ����ԿУ��ֵ
				if((ret = UnionReadRequestXMLPackageValue("body/pkValue",asymmetricKeyDB.pkValue,sizeof(asymmetricKeyDB.pkValue))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE121:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkValue");
					return(ret);
				}
				
				// add by leipp 20160126,�ַ�ʱ,�᷵������
				snprintf(pkValue,sizeof(pkValue),"%s",asymmetricKeyDB.pkValue);
				// add end
			}

			if (isRemoteApplyKey || isDistributeFlag)
			{
				switch(asymmetricKeyDB.algorithmID)
				{
					case	conAsymmetricAlgorithmIDOfRSA:	// RSA�㷨
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case	conHsmCmdVerRacalStandardHsmCmd:
							case	conHsmCmdVerSJL06StandardHsmCmd:
								lenOfVK = aschex_to_bcdhex(remoteVK,strlen(remoteVK),vkValue);
								if ((lenOfVK = UnionHsmCmdUI2("00",NULL,strlen(platformProtectKey),platformProtectKey,NULL,vkValue,lenOfVK,(unsigned char*)asymmetricKeyDB.vkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE121:: UnionHsmCmdUI2!\n");
									return(lenOfVK);
								}
								
								switch (asymmetricKeyDB.keyType)
								{
									case 	conAsymmetricKeyTypeOfSignature:
										asymmetricKeyDB.vkValue[3] = '0';
										break;
									case	conAsymmetricKeyTypeOfEncryption:
										asymmetricKeyDB.vkValue[3] = '1';
										break;
									case	conAsymmetricKeyTypeOfSignatureAndEncryption:
                                                                                asymmetricKeyDB.vkValue[3] = '2';
										break;
									default:
										UnionUserErrLog("in UnionDealServiceCodeE120:: keyType[%d]\n",asymmetricKeyDB.keyType);
										UnionSetResponseRemark("�Ƿ�����Կ���ͱ�ʶ");
										return(errCodeParameter);
								}
								
								if (mode == 2)
								{
									lenOfPK = aschex_to_bcdhex(asymmetricKeyDB.pkValue,strlen(asymmetricKeyDB.pkValue),pkValue);
									// ��ȡ�㹫Կ
									if ((ret = UnionGetPKOutOfRacalHsmCmdReturnStr((unsigned char *)pkValue,lenOfPK,asymmetricKeyDB.pkValue,&len,sizeof(asymmetricKeyDB.pkValue))) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE121:: UnionGetPKOutOfRacalHsmCmdReturnStr!\n");
										return(ret);
									}
									asymmetricKeyDB.pkValue[len] = 0;
									bcdhex_to_aschex(pkValue,lenOfPK,tmpBuf);
									memcpy(pkValue,tmpBuf,lenOfPK*2);
									pkValue[lenOfPK*2] = 0;
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE121:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
								return(errCodeParameter);
						}
						break;
					case	conAsymmetricAlgorithmIDOfSM2:
						switch(phsmGroupRec->hsmCmdVersionID)
                                                {
                                                        case    conHsmCmdVerRacalStandardHsmCmd:
                                                        case    conHsmCmdVerSJL06StandardHsmCmd:
                                                                lenOfVK = aschex_to_bcdhex(remoteVK,strlen(remoteVK),vkValue);
                                                                // �������
                                                                if (lenOfVK % 16 != 0)
                                                                {
                                                                        addLen = 16 - lenOfVK % 16;
                                                                	sprintf(vkValue + lenOfVK,"%0*d",addLen,0);
									lenOfVK += addLen;
								}
										
                                                                if ((lenOfVK = UnionHsmCmdWA(1,"01","000",1,platformProtectKey,platformCheckValue,NULL,lenOfVK,(unsigned char*)vkValue,tmpBuf)) < 0)
                                                                {
                                                                        UnionUserErrLog("in UnionDealServiceCodeE121:: UnionHsmCmdWA!\n");
                                                                        return(lenOfVK);
                                                                }
								bcdhex_to_aschex(tmpBuf,lenOfVK,asymmetricKeyDB.vkValue);
								asymmetricKeyDB.vkValue[lenOfVK*2] = 0;
                                                                break;
                                                        default:
                                                                UnionUserErrLog("in UnionDealServiceCodeE121:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                                                                UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                                                                return(errCodeParameter);
                                                }
                                                break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE121:: asymmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
						return(errCodeEsscMDL_InvalidAlgorithmID);
				}
			}
			if (isRemoteApplyKey == 0 && isDistributeFlag == 0)
			{
				switch(asymmetricKeyDB.algorithmID)
				{
					case	conAsymmetricAlgorithmIDOfRSA:	// RSA�㷨
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case	conHsmCmdVerRacalStandardHsmCmd:
							case	conHsmCmdVerSJL06StandardHsmCmd:
								if ((lenOfPK = UnionHsmCmdEI(keyType[0],asymmetricKeyDB.keyLen,"01",
									lenOfPKExp,pkexp,0,pkValue,sizeof(pkValue),&lenOfVK,(unsigned char *)vkValue,sizeof(vkValue))) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE121:: UnionHsmCmdEI!\n");
									return(lenOfPK);
								}
								// ��ȡ˽Կ
								bcdhex_to_aschex(vkValue,lenOfVK,asymmetricKeyDB.vkValue);
								asymmetricKeyDB.vkValue[lenOfVK*2] = 0;
								if (isApplyFlag)	// Զ��	
								{
									if ((lenOfVK = UnionHsmCmdUG("00",conZMK,platformProtectKey,NULL,lenOfVK,vkValue,vkByKek)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE121:: UnionHsmCmdUG!\n");
										return(lenOfVK);
									}
									bcdhex_to_aschex(vkByKek,lenOfVK,remoteVK);
									memcpy(vkByKek,remoteVK,lenOfVK*2);
									vkByKek[lenOfVK*2] = 0;
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE121:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
								return(errCodeParameter);
						}
						
						// ��ȡ�㹫Կ
						if ((ret = UnionGetPKOutOfRacalHsmCmdReturnStr((unsigned char *)pkValue,lenOfPK,asymmetricKeyDB.pkValue,&len,sizeof(asymmetricKeyDB.pkValue))) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE121:: UnionGetPKOutOfRacalHsmCmdReturnStr!\n");
							return(ret);
						}
						asymmetricKeyDB.pkValue[len] = 0;
						if (mode == 1)	// �㹫Կ
							strcpy(pkValue,asymmetricKeyDB.pkValue);
						else
						{
							bcdhex_to_aschex(pkValue,lenOfPK,tmpBuf);
							memcpy(pkValue,tmpBuf,lenOfPK*2);
							pkValue[lenOfPK*2] = 0;
						}

						break;
					case	conAsymmetricAlgorithmIDOfSM2:	// SM2�㷨
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case conHsmCmdVerRacalStandardHsmCmd:
							case conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret = UnionHsmCmdK1(keyType[0],asymmetricKeyDB.keyLen,asymmetricKeyDB.pkValue,sizeof(asymmetricKeyDB.pkValue),&lenOfVK,(unsigned char *)vkValue,sizeof(vkValue))) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE121:: UnionHsmCmdK1!\n");
									return(ret);
								}
							
								strcpy(pkValue,asymmetricKeyDB.pkValue);
								// ��ȡ˽Կ
								if (isApplyFlag)	// Զ��	
								{
									// �������
					                                if (lenOfVK % 16 != 0)
                                					{
                                					       	addLen = 16 - lenOfVK % 16;
										sprintf(vkValue + lenOfVK,"%0*d",addLen,0);
										lenOfVK += addLen;
									}
	
									if ((lenOfVK = UnionHsmCmdWA(2,"01","000",1,platformProtectKey,platformCheckValue,NULL,lenOfVK,(unsigned char*)vkValue,remoteVK)) < 0)
                                    					{
                                    					        UnionUserErrLog("in UnionDealServiceCodeE121:: UnionHsmCmdWA!\n");
										return(lenOfVK);
									}
									bcdhex_to_aschex(remoteVK,lenOfVK,vkByKek);
									vkByKek[lenOfVK*2] = 0;
								}      
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE121:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
								return(errCodeParameter);
						}
						bcdhex_to_aschex(vkValue,lenOfVK,asymmetricKeyDB.vkValue);
						asymmetricKeyDB.vkValue[lenOfVK*2] = 0;
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE121:: asymmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
						return(errCodeEsscMDL_InvalidAlgorithmID);
				}
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE121:: mode = [%d] error!\n",mode);
			return(errCodeEsscMDLKeyOperationNotPermitted);
	}

	if (isRemoteDistributeKey)	// Զ�̷ַ���Կ
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE121:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// ���������ʶ 2:�ַ�
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/remoteOperateFlag","2")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE121:: UnionSetRequestRemoteXMLPackageValue[%s]!\n","body/remoteOperateFlag");
			return(ret);
		}

		if (mode == 1)	// ������Կ
		{
			// ������Կ��У��ֵ
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
								UnionUserErrLog("in UnionDealServiceCodeE121:: UnionHsmCmdUG!\n");
								return(lenOfVK);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE121:: phsmGroupRec->hsmCmdVersionID[%d] error\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					break;
				case	conAsymmetricAlgorithmIDOfSM2:	// SM2�㷨
					switch(phsmGroupRec->hsmCmdVersionID)
                                        {
                                                case    conHsmCmdVerRacalStandardHsmCmd:
                                                case    conHsmCmdVerSJL06StandardHsmCmd:
                                                        // �������
                                                        if (lenOfVK % 16 != 0)
                                                        {
                                                                addLen = 16 - lenOfVK % 16;
        							sprintf(vkValue + lenOfVK,"%0*d",addLen,0);
								lenOfVK += addLen;	                                                
							}
	
                                                        if ((lenOfVK = UnionHsmCmdWA(2,"01","000",1,tkeyDistributePlatform.protectKey,tkeyDistributePlatform.checkValue,NULL,lenOfVK,(unsigned char*)vkValue,remoteVK)) < 0)
                                                        {
                                                                UnionUserErrLog("in UnionDealServiceCodeE121:: UnionHsmCmdWA!\n");
                                                                return(lenOfVK);
                                                        }
                                                        break;
                                                default:
                                                        UnionUserErrLog("in UnionDealServiceCodeE121:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                                                        UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                                                        return(errCodeParameter);
                                        }
                                        break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE121:: Distribute Key symmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
			bcdhex_to_aschex(remoteVK,lenOfVK,vkByKek);
			vkByKek[lenOfVK*2] = 0;
			if ((ret = UnionSetRequestRemoteXMLPackageValue("body/vkValue",vkByKek)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE121:: UnionSetRequestRemoteXMLPackageValue[%s]!\n","body/vkValue");
				return(ret);
			}
			if ((ret = UnionSetRequestRemoteXMLPackageValue("body/pkValue",asymmetricKeyDB.pkValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE121:: UnionSetRequestRemoteXMLPackageValue[%s]!\n","body/pkValue");
				return(ret);
			}
		}
		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE121:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	if (asymmetricKeyDB.vkStoreLocation != 0)	// ˽Կ�洢���������
	{
				
		snprintf(sql,sizeof(sql),"select * from hsm where hsmGroupID = '%s' and enabled = 1",UnionGetHsmGroupIDForHsmSvr());
		if ((hsmNum = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE121:: UnionExecRealDBSql[%s]!\n",sql);
			return(hsmNum);
		}
		else if (hsmNum == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE121:: hsmGroupID[%s]��û�п��������!\n",UnionGetHsmGroupIDForHsmSvr());
			return(errCodeParameter);
		}

		// add by leipp 20160126
		lenOfVK = aschex_to_bcdhex(asymmetricKeyDB.vkValue,strlen(asymmetricKeyDB.vkValue),vkValue);
		vkValue[lenOfVK] = 0;
		// add end

		for (i = 0; i < hsmNum; i++)
		{
			UnionLocateXMLPackage("detail", i+1);
			
			if ((ret = UnionReadXMLPackageValue("ipAddr", ipAddr, sizeof(ipAddr))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE121:: UnionReadXMLPackageValue[%s]!\n","ipAddr");
				return(ret);
			}
			UnionSetUseSpecHsmIPAddrForOneCmd(ipAddr);
			switch(asymmetricKeyDB.algorithmID)
			{
				case	conAsymmetricAlgorithmIDOfRSA:	// RSA�㷨
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case	conHsmCmdVerRacalStandardHsmCmd:
						case	conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdEK(atoi(asymmetricKeyDB.vkIndex),lenOfVK,(unsigned char *)vkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE121:: UnionHsmCmdEK hsm[%s] vkIndex[%s]!\n",ipAddr,asymmetricKeyDB.vkIndex);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE121:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					break;
				case	conAsymmetricAlgorithmIDOfSM2:	// SM2�㷨
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case	conHsmCmdVerRacalStandardHsmCmd:
						case	conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdK2(atoi(asymmetricKeyDB.vkIndex),lenOfVK,(unsigned char *)vkValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE121:: UnionHsmCmdK2 hsm[%s] vkIndex[%s]!\n",ipAddr,asymmetricKeyDB.vkIndex);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE121:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE121:: asymmetricKeyDB.algorithmID[%d] error!\n",asymmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
		}

		// modify by leipp 20160516���ȵ�����Կ�����ܻ����ٽ����ж��Ƿ����˽Կֵ
		if (asymmetricKeyDB.vkStoreLocation == 1)
			asymmetricKeyDB.vkValue[0] = 0;
		// modify by leipp end 20160516
	}

	// ��Ч����
	if ((ret = UnionReadRequestXMLPackageValue("body/activeDate",asymmetricKeyDB.activeDate,sizeof(asymmetricKeyDB.activeDate))) <= 0)
	{
		UnionGetFullSystemDate(asymmetricKeyDB.activeDate);
	}
	else
	{
		UnionFilterHeadAndTailBlank(asymmetricKeyDB.activeDate);
		if (!UnionIsValidFullDateStr(asymmetricKeyDB.activeDate))
        	{
                	UnionUserErrLog("in UnionDealServiceCodeE121:: activeDate[%s] error!\n",asymmetricKeyDB.activeDate);
                	UnionSetResponseRemark("��Ч���ڸ�ʽ[%s]�Ƿ�,����ΪYYYYMMDD��ʽ",asymmetricKeyDB.activeDate);
                	return(errCodeParameter);
        	}
	}
		
	// ���·ǶԳ���Կ
	if ((ret =  UnionUpdateAsymmetricKeyDBKeyValue(&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE121:: UnionUpdateAsymmetricKeyDBKeyValue,keyName[%s]!\n",asymmetricKeyDB.keyName);
		return(ret);	
	}

	if ((ret = UnionSetResponseXMLPackageValue("body/pkValue",pkValue)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE121:: UnionSetResponseXMLPackageValue pkValue[%s]!\n",pkValue);
		return(ret);
	}
	
	if (exportFlag || (isRemoteKeyOperate && isApplyFlag))
	{
		if (isApplyFlag)	//��Զ��
		{
			if ((ret = UnionSetResponseXMLPackageValue("body/vkValue",vkByKek)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE121:: UnionSetResponseXMLPackageValue[%s]!\n","body/vkValue");
				return(ret);
			}
		}

		if (isRemoteKeyOperate == 0)	// ����
		{
			if ((ret = UnionSetResponseXMLPackageValue("body/vkValue",asymmetricKeyDB.vkValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE121:: UnionSetResponseXMLPackageValue[%s]!\n","body/vkValue");
				return(ret);
			}
		}
	}
	return(0);
}



