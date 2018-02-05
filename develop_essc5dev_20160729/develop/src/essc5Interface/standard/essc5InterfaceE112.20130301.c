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
�������:	E112
������:		����Գ���Կ
��������:	����Գ���Կ
***************************************/
int UnionDealServiceCodeE112(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				len = 0;
	char				protectFlag[32];
	char				protectKey[128];
	char				zmk[64];
	char				keyByLMK[64];
	char				keyByRKM[64];
	char				keyValue[64];
	char				checkValue[32];
	char				localCheckValue[32];
	char				remoteCheckValue[32];
	int				isRemoteKeyOperate = 0;
	int				isRemoteDistributeKey = 0;
	char				platformProtectKey[64];
	char				sql[128];
	char				tmpBuf[128];
	int				lenOfKey = 0;
	char				keyName[160];
	int				flag = 0;
	
	char				appID[32];
	char				keyNode[80];
	char				keyType[16];
	char				keyVer[16];
	int 				version = 0;
	char				activeDate[16];

	int				checkValueFlag = 0;//add by chenwd 20150521 ����Ƿ���Ҫ����У��ֵ

	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;
	TUnionSymmetricKeyDB		zmkKeyDB;
	PUnionSymmetricKeyValue		pzmkKeyValue = NULL;
	TUnionRemoteKeyPlatform		tkeyDistributePlatform;

	// ����Ƿ�Զ�̲���
	isRemoteKeyOperate = UnionIsRemoteKeyOperate();
	if (isRemoteKeyOperate)
		flag = 1;
	
	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(keyName);
	
	//��Կ�汾�� Ŀǰ��kms2���ľɱ���ת��XML������Ҫ����version
        if((ret = UnionReadRequestXMLPackageValue("body/version",tmpBuf, sizeof(tmpBuf))) > 0)
        {
                tmpBuf[ret] = 0;
		if(3 < strlen(tmpBuf))
                {
                        UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadRecFldFromRecStr currentKeyVer[%s] len > 3\n", tmpBuf);
                        return(errCodeObjectMDL_FieldValueTooLong);
                }
                else if(3 >= strlen(tmpBuf))
                {
                        memcpy(keyVer, "-000", 4-strlen(tmpBuf));
                        keyVer[4-strlen(tmpBuf)] = 0;
                        strcat(keyVer, tmpBuf);
                }
                version = atoi(tmpBuf);
		if(version >= 1)
		{
			if((ret = UnionReadRequestXMLPackageValue("body/sysID", tmpBuf, sizeof(tmpBuf))) <= 0)
        	        {
        	                UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]\n","body/sysID");
        	                return(ret);
        	        }
                	tmpBuf[ret] = 0;
                	if((ret = UnionSetRequestXMLPackageValue("head/sysID", tmpBuf)) < 0)
                	{
                	       UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s][%s]\n","head/sysID",tmpBuf);
                	       return(ret);
               	 	}
		
        	        if((ret = UnionReadRequestXMLPackageValue("body/appID", tmpBuf, sizeof(tmpBuf))) <= 0)
        	        {
        	                UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]\n","body/appID");
        	                return(ret);
        	        }
                	tmpBuf[ret] = 0;
               	 	if((ret = UnionSetRequestXMLPackageValue("head/appID", tmpBuf)) < 0)
                	{	
                	        UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s][%s]\n","head/appID",tmpBuf);
                	        return(ret);
                	}
			
			if((ret = UnionAnalysisSymmetricKeyName(keyName, appID, keyNode, keyType)) < 0)
                	{
                	        UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionAnalysisSymmetricKeyName keyName[%s]\n", keyName);
                	        return(ret);
                	}
                	strcat(keyNode, keyVer);
                	memset(keyName, 0, sizeof(keyName));
                	memcpy(keyName, appID, strlen(appID));
                	strcat(keyName, ".");
                	strcat(keyName, keyNode);
                	strcat(keyName, ".");
                	strcat(keyName, keyType);
			if((ret = UnionSetRequestXMLPackageValue("body/keyName", keyName)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s]\n", "body/keyName");
				return(ret);
			}
                	      
                	UnionGetFullSystemDate(activeDate);
                	if((ret = UnionSetRequestXMLPackageValue("body/activeDate", activeDate)) < 0)
                	{
                	        UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s]\n", "body/activeDate");
                	        return(ret);
                	}
	
	                if((ret = UnionSetRequestXMLPackageValue("body/effectiveDays" , "36500")) < 0)
	                {
	                        UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s]\n","body/effectiveDays");
	                        return(ret);
	                }
	
	                if((ret = UnionSetRequestXMLPackageValue("body/keyGroup", "default")) < 0)
	                {
	                        UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s]\n","body/keyGroup");
	                        return(ret);
	                }
	
	                if((ret = UnionSetRequestXMLPackageValue("body/inputFlag", "1")) < 0)
	                {
	                        UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s]\n","body/inputFlag");
	                        return(ret);
	                }
	                      
	                if((ret = UnionSetRequestXMLPackageValue("body/outputFlag", "1")) < 0)
	                {
	                        UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s]\n","body/outputFlag");
	                        return(ret);
	                }
	
	                if((ret = UnionSetRequestXMLPackageValue("body/oldVersionKeyIsUsed", "1")) < 0)
	                {
	                        UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s]\n","body/oldVersionKeyIsUsed");
	                        return(ret);
	                }
	
	                if((ret = UnionSetRequestXMLPackageValue("body/mode", "0")) < 0)
	                {
	                        UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestXMLPackageValue[%s]\n","body/mode");
	                        return(ret);
	                }
		}
		else
                {
                        UnionUserErrLog("in UnionDealServiceCodeE110:: wrong version[%d]\n", version);
                        UnionSetResponseRemark("��Կ�汾�Ŵ���");
                        return(errCodeParameter);
                }
	}
		

	// ��ȡ�Գ���Կ
	UnionInitSymmetricKeyDB(&symmetricKeyDB);
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
	{
		if(version >= 1)
		{
			//E110Ҳ��version��ƴװ�汾�ŵ���Կ���ƣ������ظ�ƴװ��version����Ϊ��
			if((ret = UnionSetRequestXMLPackageValue("body/version", "")) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE110:: UnionSetRequestXMLPackageValue[%s]\n","body/version");
				return(ret);
			}			

			if ((ret = UnionDealServiceCodeE110(phsmGroupRec)) < 0)
                        {
                                UnionUserErrLog("in UnionDealServiceCodeE112:: UnionDealServiceCodeE110!\n");
                                return(ret);
                        }
			
			if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
			{
				if(errCodeKeyCacheMDL_WrongKeyName == ret)
				{
					UnionUserErrLog("in UnionDealServiceCodeE112:: �Գ���Կ[%s]������!\n",keyName);
                        		UnionSetResponseRemark("�Գ���Կ[%s]������",keyName);
                        		return(errCodeKeyCacheMDL_KeyNonExists);
				}
				else
				{
					UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadSymmetricKeyDBRec[%s] after insert\n", keyName);
					return(ret);
				}
			}
			
		}
		else if (ret == errCodeKeyCacheMDL_WrongKeyName)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: �Գ���Կ[%s]������!\n",keyName);
			UnionSetResponseRemark("�Գ���Կ[%s]������",keyName);
			return(errCodeKeyCacheMDL_KeyNonExists);
		}
		else
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
			return(ret);
		}
	}
	
	// ��ȡ��Կֵ
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) != NULL)
	{
		// ��ǰ��Կ��Ϊ����Կ
		strcpy(psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue);
		strcpy(symmetricKeyDB.oldCheckValue,symmetricKeyDB.checkValue);

		psymmetricKeyValue->keyValue[0] = 0;
		symmetricKeyDB.checkValue[0] = 0;
	}
	else
		psymmetricKeyValue = &symmetricKeyDB.keyValue[0];
		

	strcpy(psymmetricKeyValue->lmkProtectMode,phsmGroupRec->lmkProtectMode);

	if (0 == flag)
	{
		// Ϊ���ݾ�ϵͳ�趨�ļ���ʶ
		if ((ret = UnionReadRequestXMLPackageValue("body/sysFlag",tmpBuf,sizeof(tmpBuf))) > 0)
			flag = 1;
	}
	// �����Կ�Ƿ����ʹ��
	if ((ret = UnionIsUseCheckSymmetricKeyDB(symmetricKeyDB.status,flag)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE112:: UnionIsUseCheckSymmetricKeyDB symmetricKeyDB.status[%d] flag[%d]!\n",symmetricKeyDB.status,flag);
		return(ret);
	}

	if (!symmetricKeyDB.inputFlag)		// ��������
	{
		UnionUserErrLog("in UnionDealServiceCodeE112:: symmetricKeyDB.inputFlag[%d]!\n",symmetricKeyDB.inputFlag);
		UnionSetResponseRemark("��Կ��������");
		return(errCodeEsscMDL_KeyInputNotPermitted);
	}
	
	// ��ȡ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyValue",keyValue,sizeof(keyValue))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyValue");
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(ret);
	}
	else
        {
		UnionFilterHeadAndTailBlank(keyValue);
		lenOfKey = strlen(keyValue);
		if (lenOfKey  == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: keyValue can not be null!\n");
			UnionSetResponseRemark("��Կֵ����Ϊ��");
			return(errCodeParameter);
		}
		if ((lenOfKey * 4) != symmetricKeyDB.keyLen)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: new keyLen[%d] != current keyLen[%d]!\n",lenOfKey,symmetricKeyDB.keyLen/4);
			UnionSetResponseRemark("��Կ���ȷǷ�����Կ���ȱ�����[%d]",symmetricKeyDB.keyLen/4);
			return(errCodeParameter);
		}
                if (!UnionIsBCDStr(keyValue))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE112:: keyValue[%s] is error!\n",keyValue);
                        UnionSetResponseRemark("��ȡ��Կ���ķǷ�,����Ϊʮ��������");
                        return(errCodeParameter);
                }
        }
		
	// ��ȡ����ֵ
	if ((ret = UnionReadRequestXMLPackageValue("body/checkValue",checkValue,sizeof(checkValue))) <= 0)
	{
		checkValue[0] = 0;
		checkValueFlag = 1;//add by chenwd 20150521 ���÷���У��ֵ
	}
 	else
        {
		UnionFilterHeadAndTailBlank(checkValue);
                if ((!UnionIsBCDStr(checkValue)) || (strlen(checkValue) < 4))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE112:: checkValue[%s] is error!\n",checkValue);
                        UnionSetResponseRemark("��ȡ����ֵ�Ƿ�,����Ϊʮ��������,�ҳ��ȱ�����ڵ���4");
                        return(errCodeParameter);
                }
        }

	if ((ret = UnionReadRequestXMLPackageValue("body/protectFlag",protectFlag,sizeof(protectFlag))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]!\n","body/protectFlag");
		return(ret);
	}
	
	if ((protectFlag[0] == '1') || (protectFlag[0] == '2'))
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/protectKey",protectKey,sizeof(protectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]!\n","body/protectKey");
			return(ret);	
		}
	}
	
	// ��ȡ������Կ
	if (protectFlag[0] == '1')	// ָ����Կ����
	{
		if ((ret =  UnionReadSymmetricKeyDBRec(protectKey,1,&zmkKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadSymmetricKeyDBRec protectKey[%s]!\n",protectKey);
			return(ret);
		}
		if (zmkKeyDB.keyType != conZMK)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: is not zmk [%s]!\n",protectKey);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}
		if ((pzmkKeyValue = UnionGetSymmetricKeyValue(&zmkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("��ȡ��Կֵʧ��");
			return(errCodeParameter);
		}
		strcpy(zmk,pzmkKeyValue->keyValue);
	}
	else if (protectFlag[0] == '2')	// ���ZMK��Կ����
	{
		len = strlen(protectKey);
		if ((len != 16) && (len != 32) && (len != 48))
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: protectKey[%s] len != [16,32,48]\n",protectKey);
			return(errCodeEssc_KeyLength);
		}
		strcpy(zmk,protectKey);
	}
	else if (protectFlag[0] == '3')	// ���LMK����Կ����
	{
		//
	}
	else if (protectFlag[0] == '4')	// ����sysID��ȡ������Կ
	{
		// ϵͳID
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);
		}

		//��ȡ������Կ
		snprintf(sql,sizeof(sql),"select protectKey from localKeyPlatform where sysID = '%s' and algorithmID = %d",tmpBuf,symmetricKeyDB.algorithmID);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSelectRealDBRecord[%s]\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: platformProtectKey not found!\n");
			UnionSetResponseRemark("Զ��ƽ̨������Կ������");
			return(errCodeParameter);
		}

		if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionLocateXMLPackage[%s]!\n","detail");	
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("protectKey",platformProtectKey,sizeof(platformProtectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadXMLPackageValue[%s]!\n","protectKey");
			return(ret);
		}

		snprintf(zmk,sizeof(zmk),"%s",platformProtectKey);
	}
	else
	{
		UnionUserErrLog("in UnionDealServiceCodeE112:: protectFlag[%s] error!\n",protectFlag);
		return(errCodeParameter);
	}

	if (!isRemoteKeyOperate)  // ���ز���
	{
		// modify by leipp 20151209
		// ��ȡ���������ͷַ�ƽ̨
		if ((ret = UnionCheckRemoteKeyPlatform(symmetricKeyDB.keyName, NULL, symmetricKeyDB.keyDistributePlatform, NULL, &tkeyDistributePlatform, NULL, &isRemoteDistributeKey)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionCheckRemoteKeyPlatform!\n");
			return(ret);
		}
		// modify end

		memset(keyByLMK,0,sizeof(keyByLMK));
		memset(localCheckValue,0,sizeof(localCheckValue));
		if ((protectFlag[0] == '1') ||		// ָ����Կ����
			(protectFlag[0] == '2') ||	// ���ZMK��Կ����
			(protectFlag[0] == '4'))	// ����sysID����ȡ���ر�����Կ
		{
		
			memset(keyByRKM,0,sizeof(keyByRKM));
			memset(remoteCheckValue,0,sizeof(remoteCheckValue));
			switch(symmetricKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,zmk,keyValue,keyByLMK,localCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE112:: UnionHsmCmdA6!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE112:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,zmk,keyValue,keyByLMK,localCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE112:: UnionHsmCmdA6!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE112:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE112:: Distribute Key algorithmID[%d] is error!\n",symmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
		}
		else
		{
			// ������ԿУ��ֵ
			switch(symmetricKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:
					UnionSetIsUseNormalZmkType();
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdBU(0,symmetricKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen),keyValue,localCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE112:: UnionHsmCmdBU!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE112:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
					UnionSetIsUseNormalZmkType();
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdBU(1,symmetricKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen),keyValue,localCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE112:: UnionHsmCmdBU!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE112:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE112:: Distribute Key algorithmID[%d] is error!\n",symmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
			strcpy(keyByLMK,keyValue);
		}

		// ���У��ֵ
		if (memcmp(localCheckValue,checkValue,strlen(checkValue)) != 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: localCheckValue[%s] != checkValue[%s]\n",localCheckValue,checkValue);
			return(errCodeEssc_CheckValue);
		}
	}
	else	// Զ�̲���
	{
		// ϵͳID
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);
		}

		//��ȡ������Կ
		snprintf(sql,sizeof(sql),"select protectKey from localKeyPlatform where sysID = '%s' and algorithmID = %d",tmpBuf,symmetricKeyDB.algorithmID);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSelectRealDBRecord[%s]\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: platformProtectKey not found!\n");
			UnionSetResponseRemark("Զ��ƽ̨������Կ������");
			return(errCodeParameter);
		}

		if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionLocateXMLPackage[%s]!\n","detail");	
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("protectKey",platformProtectKey,sizeof(platformProtectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadXMLPackageValue[%s]!\n","protectKey");
			return(ret);
		}

		// ��ȡԶ����Կ
		if ((ret = UnionReadRequestXMLPackageValue("body/remoteKeyValue",keyValue,sizeof(keyValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]!\n","body/remoteKeyValue");
			return(ret);
		}
	
		// ��ȡԶ�̼���ֵ
		if ((ret = UnionReadRequestXMLPackageValue("body/remoteCheckValue",checkValue,sizeof(checkValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionReadRequestXMLPackageValue[%s]!\n","body/remoteCheckValue");
			return(ret);
		}

		memset(keyByLMK,0,sizeof(keyByLMK));
		memset(localCheckValue,0,sizeof(localCheckValue));
		switch(symmetricKeyDB.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,platformProtectKey,keyValue,keyByLMK,localCheckValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE112:: UnionHsmCmdA6!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE112:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}

				break;
			case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,platformProtectKey,keyValue,keyByLMK,localCheckValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE112:: UnionHsmCmdA6!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE112:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}

				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE112:: Distribute Key algorithmID[%d] is error!\n",symmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}
		// ���У��ֵ
		if (memcmp(localCheckValue,checkValue,strlen(checkValue)) != 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: localCheckValue[%s] != checkValue[%s]\n",localCheckValue,checkValue);
			return(errCodeEssc_CheckValue);
		}
	}

	if (isRemoteDistributeKey)	// ���ز���
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		memset(keyByRKM,0,sizeof(keyByRKM));
		memset(remoteCheckValue,0,sizeof(remoteCheckValue));
		switch(symmetricKeyDB.algorithmID)
		{
			case	conSymmetricAlgorithmIDOfDES:
				// �ѱ���LMK�Լ���ת��Զ��ZMK����
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case conHsmCmdVerRacalStandardHsmCmd:
					case conHsmCmdVerSJL06StandardHsmCmd:
						if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,keyByLMK,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE112:: Distribute Key UnionHsmCmdA8!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE112:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
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
						if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,keyByLMK,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE112:: Distribute Key UnionHsmCmdST!\n");
							return(ret);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE112:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
						return(errCodeParameter);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE112:: Distribute Key algorithmID[%d] is error!\n",symmetricKeyDB.algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
		}

		// ���У��ֵ
		if (memcmp(localCheckValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: Distribute Key localCheckValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
			return(errCodeEssc_CheckValue);
		}

		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/remoteKeyValue",keyByRKM)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyValue",keyByRKM);
			return(ret);
		}

		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/remoteCheckValue",remoteCheckValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/checkValue",remoteCheckValue);
			return(ret);
		}
		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	// ��ǰ��Կ��Ϊ����Կ

	strcpy(psymmetricKeyValue->keyValue,keyByLMK);
	if (strlen(checkValue) != 16)
		strcpy(symmetricKeyDB.checkValue,localCheckValue);
	else
		strcpy(symmetricKeyDB.checkValue,checkValue);

	// ��Чʱ��
	if ((ret = UnionReadRequestXMLPackageValue("body/activeDate",symmetricKeyDB.activeDate,sizeof(symmetricKeyDB.activeDate))) <= 0)
		UnionGetFullSystemDate(symmetricKeyDB.activeDate);

	UnionFilterHeadAndTailBlank(symmetricKeyDB.activeDate);
	if (!UnionIsValidFullDateStr(symmetricKeyDB.activeDate))
        {
                UnionUserErrLog("in UnionDealServiceCodeE112:: activeDate[%s] error!\n",symmetricKeyDB.activeDate);
                UnionSetResponseRemark("��Ч���ڸ�ʽ[%s]�Ƿ�,����ΪYYYYMMDD��ʽ",symmetricKeyDB.activeDate);
                return(errCodeParameter);
        }
	if ((ret =  UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE112:: UnionUpdateSymmetricKeyDBKeyValue,keyName[%s]!\n",symmetricKeyDB.keyName);
		return(ret);	
	}
	
	// add by chenwd 20150521 ���û��У��ֵ�򷵻�У��ֵ
	if (checkValueFlag == 1)
	{
		if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",symmetricKeyDB.checkValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE112:: UnionSetResponseXMLPackageValue[%s]!\n","body/checkValue");
			return(ret);
		}
	}
	// add by chenwd end
	
	return(0);
}


