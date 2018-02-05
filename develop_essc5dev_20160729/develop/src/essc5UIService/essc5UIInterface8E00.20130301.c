//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

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

#define maxGrpNum 100

/***************************************
  �������:	8E00
  ������:	���ӶԳ���Կ
  ��������:	���ӶԳ���Կ
 ***************************************/
int UnionDealServiceCode8E00(PUnionHsmGroupRec phsmGroupRec)
{
	int				i = 0;
	int				j = 0;
	int				ret;
	char				keyNodeList[4096];
	char				keyTypeList[128];
	char				keyNodeGrp[maxGrpNum][128];
	char				keyTypeGrp[maxGrpNum][128];
	int				keyNodeNum = 0;
	int				keyTypeNum = 0;
	int				keyTotalNum = 0;
	int				mode = 1;
	int				inputFlag = 0;
	int				outputFlag = 0;
	char				tmpBuf[128];
	char				keyType[maxGrpNum][32];
	int				isRemoteApplyKey = 0;
	int				isRemoteDistributeKey = 0;
	char				keyByZMK[64];
	char				keyByRKM[64];
	//char				remoteKeyValue[64];
	char				remoteCheckValue[32];
//	int				totalNum = 0;
	char				*ptrKeyType = NULL;	
	int				isFailed = 0;
	char				appID[32];
	int				failNum = 0;
	int				successNum = 0;
	int				keyNameFlag = 0;

	TUnionSymmetricKeyDB		symmetricKeyDB;
	TUnionRemoteKeyPlatform		tkeyApplyPlatform;
	TUnionRemoteKeyPlatform		tkeyDistributePlatform;

	memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));

	// ��Կ��
	if ((ret = UnionReadRequestXMLPackageValue("body/keyGroup",symmetricKeyDB.keyGroup,sizeof(symmetricKeyDB.keyGroup))) < 0)
	{
		//modify by zhouxw 20150907
		//strcpy(symmetricKeyDB.keyGroup,"default");
		UnionUserErrLog("in UnionDealServiceCode8E00:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyGroup");
		return(ret);
		//modify end
	}
	//add by zhouxw 20150907
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E00:: keyGroup should not be null!\n");
		UnionSetResponseRemark("��Կ�鲻��Ϊ��");
		return(errCodeParameter);
	}
	UnionSetHsmGroupIDForHsmSvr(symmetricKeyDB.keyGroup);
	//add end
	//add by zhouxw 20151013
	if ((ret = UnionGetHsmGroupRecByHsmGroupID(symmetricKeyDB.keyGroup,phsmGroupRec)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E00:: UnionGetHsmGroupRecByHsmGroupID �������[%s]������!\n",symmetricKeyDB.keyGroup);
		return(ret);	
        }
	//add end
	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",symmetricKeyDB.keyName,sizeof(symmetricKeyDB.keyName))) > 0)
	{
		// ��Կ����
		if ((ret = UnionReadRequestXMLPackageValue("body/keyType",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyType");
			return(ret);
		}
		tmpBuf[ret] = 0;
		symmetricKeyDB.keyType = atoi(tmpBuf);
		keyNameFlag = 1;
	}
	else
	{
		// ��ԿӦ�ñ��
		if ((ret = UnionReadRequestXMLPackageValue("body/appID",appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: UnionReadRequestXMLPackageValue[%s]!\n","body/appID");
			return(ret);
		}
		UnionFilterHeadAndTailBlank(appID);
		if (strlen(appID) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: appID can not be null!\n");
			UnionSetResponseRemark("appID����Ϊ��");
			return(errCodeParameter);
		}
		
		// ��Կ�ڵ�
		if ((ret = UnionReadRequestXMLPackageValue("body/keyNodeList",keyNodeList,sizeof(keyNodeList))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyNodeList");
			return(ret);
		}
		UnionFilterHeadAndTailBlank(keyNodeList);
		if (strlen(keyNodeList) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: keyNodeList can not be null!\n");
			UnionSetResponseRemark("nodeID����Ϊ��");
			return(errCodeParameter);
		}

		// ƴ�����崮
		if ((keyNodeNum = UnionSeprateVarStrIntoVarGrp(keyNodeList,strlen(keyNodeList),',',keyNodeGrp,maxGrpNum)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSeprateVarStrIntoVarGrp[%s]!\n",keyNodeList);
			return(keyNodeNum);
		}

		if (keyNodeNum > 100)
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: keyNodeNum[%d] > 100!\n",keyNodeNum);
			UnionSetResponseRemark("��Կ�ڵ������ܳ���100��");
			return(errCodeTooManyNetNodes);
		}

		// ��Կ����
		if ((ret = UnionReadRequestXMLPackageValue("body/keyTypeList",keyTypeList,sizeof(keyTypeList))) < 0)
		{
			if ((ret = UnionReadRequestXMLPackageValue("body/keyTypeListForRemote",keyTypeList,sizeof(keyTypeList))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E00:: UnionReadRequestXMLPackageValue[%s] or [%s]!\n","body/keyTypeList", "body/keyNodeListForRemote");
				return(ret);
			}
		}

		UnionFilterHeadAndTailBlank(keyTypeList);
		if (strlen(keyTypeList) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: keyType can not be null!\n");
			UnionSetResponseRemark("keyType����Ϊ��");
			return(errCodeParameter);
		}

		// added 2016-06-28 ֧��^�ָ���Կ�����嵥
		UnionConvertOneFldSeperatorInRecStrIntoAnother(keyTypeList, strlen(keyTypeList), '^', ',', keyTypeList, sizeof(keyTypeList));
		// added end 2016-06-28

		// ƴ�����崮
		if ((keyTypeNum = UnionSeprateVarStrIntoVarGrp(keyTypeList,strlen(keyTypeList),',',keyTypeGrp,maxGrpNum)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSeprateVarStrIntoVarGrp[%s]!\n",keyTypeList);
			return(keyTypeNum);
		}

		for (i = 0; i < keyTypeNum; i++)
		{
			UnionFilterHeadAndTailBlank(keyTypeGrp[i]);
			symmetricKeyDB.keyType = atoi(keyTypeGrp[i]);
			memset(keyType[i],0,sizeof(keyType[i]));
			if ((ptrKeyType = UnionConvertSymmetricKeyKeyTypeToExternal(symmetricKeyDB.keyType)) == NULL)
			{
				UnionUserErrLog("in UnionDealServiceCode8E00:: %d error!\n",symmetricKeyDB.keyType);
				UnionSetResponseRemark("��Կ���Ͳ���ȷ");
				return(errCodeHsmCmdMDL_InvalidKeyType);
			}
			strcpy(keyType[i],ptrKeyType);
			// 2014/12/26 zhangyd modify
			// �㷢��������TMK/TPK/TAK��ZMK/ZPK/ZAK, PVK, CVK ��׺���ô�д
			if (strcasecmp(UnionGetIDOfCustomization(),"cgb") == 0)
			{
				switch(symmetricKeyDB.keyType)
				{
					case	conZPK:
					case	conZAK:
					case	conZMK:
					case	conTMK:
					case	conTPK:
					case	conTAK:
					case	conPVK:
					case	conCVK:
						UnionToUpperCase(keyType[i]);
						break;
					default:
						UnionToLowerCase(keyType[i]);
						break;
				}
			}
			else
				UnionToLowerCase(keyType[i]);
		}
	}

	// �㷨��ʶ 
	if ((ret = UnionReadRequestXMLPackageValue("body/algorithmID",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E00:: UnionReadRequestXMLPackageValue[%s]!\n","body/algorithmID");
		return(ret);	
	}
	symmetricKeyDB.algorithmID = atoi(tmpBuf);

	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyLen",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E00:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyLen");
		return(ret);	
	}
	symmetricKeyDB.keyLen = atoi(tmpBuf);

	// ����ʹ�þ���Կ
	if ((ret = UnionReadRequestXMLPackageValue("body/oldVersionKeyIsUsed",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E00:: UnionReadRequestXMLPackageValue[%s]!\n","body/oldVersionKeyIsUsed");
		return(ret);	
	}
	symmetricKeyDB.oldVersionKeyIsUsed = atoi(tmpBuf);

	if ((symmetricKeyDB.oldVersionKeyIsUsed != 0) && (symmetricKeyDB.oldVersionKeyIsUsed != 1))
	{
		UnionUserErrLog("in UnionDealServiceCode8E00:: oldVersionKeyIsUsed[%s] error!\n",tmpBuf);
		return(errCodeParameter);
	}

	// ��ȡ��Ч����
	if ((ret = UnionReadRequestXMLPackageValue("body/effectiveDays",tmpBuf,sizeof(tmpBuf))) <= 0)
		symmetricKeyDB.effectiveDays = 0;
	else
		symmetricKeyDB.effectiveDays = atoi(tmpBuf);


	// ģʽ
	// 0����������Կ
	// 1������������Կ��Ĭ��ֵ
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) < 0)
		mode = 1;
	else
	{
		mode = atoi(tmpBuf);
		if ((mode != 0) && (mode != 1))
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: mode[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}

	// �������ʶ
	// ��ѡ��Ĭ��ֵΪ0
	// 0��������
	// 1������
	if ((ret = UnionReadRequestXMLPackageValue("body/inputFlag",tmpBuf,sizeof(tmpBuf))) < 0)
		symmetricKeyDB.inputFlag = 0;
	else
	{
		symmetricKeyDB.inputFlag = atoi(tmpBuf);
		if ((symmetricKeyDB.inputFlag != 0) && (symmetricKeyDB.inputFlag != 1) && (symmetricKeyDB.inputFlag != 2))
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: inputFlag[%s] error!\n",tmpBuf);
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
		symmetricKeyDB.outputFlag = atoi(tmpBuf);
		if ((symmetricKeyDB.outputFlag != 0) && (symmetricKeyDB.outputFlag != 1))
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: outputFlag[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}

	// ���ñ�ʶ
	// 0��������
	// 1���������ã�����Ч��Ĭ��ֵ
	// 2���������ã�ָ��ʱ����Ч
	if ((ret = UnionReadRequestXMLPackageValue("body/status",tmpBuf,sizeof(tmpBuf))) < 0)
		symmetricKeyDB.status = 1;
	else
		symmetricKeyDB.status = atoi(tmpBuf);

	if (mode == 0)
		symmetricKeyDB.status = 0;

	UnionLog("in UnionDealServiceCode8E00:: mode[%d] inputFlag[%d] outputFlag[%d]\n",mode,inputFlag,outputFlag);

	// ��Կ����ƽ̨
	if ((ret = UnionReadRequestXMLPackageValue("body/keyApplyPlatform",symmetricKeyDB.keyApplyPlatform,sizeof(symmetricKeyDB.keyApplyPlatform))) > 0)
	{
		// ����remoteKeyPlatform
		memset(&tkeyApplyPlatform,0,sizeof(tkeyApplyPlatform));
		if ((ret = UnionReadRemoteKeyPlatformRec(symmetricKeyDB.keyApplyPlatform,conRemoteKeyPlatformKeyDirectionOfApply,&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: UnionReadRemoteKeyPlatformRec keyApplyPlatform[%s]!\n",symmetricKeyDB.keyApplyPlatform);
			return(ret);
		}	
		isRemoteApplyKey = 1;
	}

	// ��Կ�ַ�ƽ̨
	if ((ret = UnionReadRequestXMLPackageValue("body/keyDistributePlatform",symmetricKeyDB.keyDistributePlatform,sizeof(symmetricKeyDB.keyDistributePlatform))) > 0)
	{
		// ����remoteKeyPlatform
		memset(&tkeyDistributePlatform,0,sizeof(tkeyDistributePlatform));
		if ((ret = UnionReadRemoteKeyPlatformRec(symmetricKeyDB.keyDistributePlatform,conRemoteKeyPlatformKeyDirectionOfDistribute,&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: UnionReadRemoteKeyPlatformRec keyDistributePlatform[%s]!\n",symmetricKeyDB.keyDistributePlatform);
			return(ret);
		}
		isRemoteDistributeKey = 1;
	}

	// ���ַ��������Ƿ�ͬһƽ̨
	if (isRemoteApplyKey && isRemoteDistributeKey)
	{
		if (strcmp(tkeyApplyPlatform.ipAddr,tkeyDistributePlatform.ipAddr) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: tkeyApplyPlatform.ipAddr[%s] == tkeyDistributePlatform.ipAddr[%s] is error!\n",tkeyApplyPlatform.ipAddr,tkeyDistributePlatform.ipAddr);	
			UnionSetResponseRemark("����ͷַ�ƽ̨������ͬ");
			return(errCodeParameter);
		}
	}

	// ��Ч����
	if ((ret = UnionReadRequestXMLPackageValue("body/activeDate",symmetricKeyDB.activeDate,sizeof(symmetricKeyDB.activeDate))) <= 0)
		UnionGetFullSystemDate(symmetricKeyDB.activeDate);

	// ��ע
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",symmetricKeyDB.remark,sizeof(symmetricKeyDB.remark))) <= 0)
		strcpy(symmetricKeyDB.remark,"");

	// ����������
	symmetricKeyDB.creatorType = conSymmetricCreatorTypeOfUser;

	// ������
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",symmetricKeyDB.creator,sizeof(symmetricKeyDB.creator))) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E00:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);	
	}

	if (strcasecmp(UnionGetIDOfCustomization(),"aliyun") == 0)      // ������
	{
		// ��������֯
		if ((ret = UnionReadRequestXMLPackageValue("body/usingUnit",symmetricKeyDB.usingUnit,sizeof(symmetricKeyDB.usingUnit))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: UnionReadRequestXMLPackageValue[%s]!\n","body/usingUnit");
			return(ret);	
		}
	}
	else
	{
		// modified 2016-07-04
		//symmetricKeyDB.usingUnit[0] = 0;
		if ((ret = UnionReadRequestXMLPackageValue("body/usingUnit",symmetricKeyDB.usingUnit,sizeof(symmetricKeyDB.usingUnit))) < 0)
		{
			symmetricKeyDB.usingUnit[0] = 0;
		}
		// end 
	}

	if (!keyNameFlag)
	{
		// ��ʾ������
		if ((ret = UnionSetResponseXMLPackageValue("head/displayBody","1")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSetResponseXMLPackageValue[%s]!\n","head/displayBody");
			return(ret);
		}

		// �����ֶ��嵥
		strcpy(tmpBuf,"keyName,status");
		if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSetResponseXMLPackageValue[%s]!\n","body/fieldList");
			return(ret);
		}

		//�������ֶ��嵥������
		strcpy(tmpBuf,"��Կ����,״̬");
		if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSetResponseXMLPackageValue[%s]!\n","body/fieldListChnName");
			return(ret);
		}

		// ��������
		keyTotalNum = keyNodeNum * keyTypeNum;
		snprintf(tmpBuf,sizeof(tmpBuf),"%d",keyTotalNum);
		if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
			return(ret);
		}

		//totalNum = 1;
		successNum = keyTotalNum + 1;
		//successNum = 0;
		failNum = 0;
	}
	else
	{
		keyTypeNum = 1;	
		keyNodeNum = 1;
	}
	for (i = 0; i < keyTypeNum; i++)
	{
		for (j = 0; j < keyNodeNum; j++)
		{
			// ƴװ��Կ����
			if (!keyNameFlag)
			{
				UnionFilterHeadAndTailBlank(keyNodeGrp[j]);
				snprintf(symmetricKeyDB.keyName,sizeof(symmetricKeyDB.keyName),"%s.%s.%s",appID,keyNodeGrp[j],keyType[i]);
				// ��Կ����
				symmetricKeyDB.keyType = atoi(keyTypeGrp[i]);
			}
			// ����Զ����Կ
			if (isRemoteApplyKey)
			{
				// remark by liwj 20150617
				/*
				// ��ʼ��Զ����Կ����������
				if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E00:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
					return(ret);
				}

				// �޸Ķ�Ӧ������ 
				// modify by lisq 20150104
				//if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","E110")) < 0)
				if (tkeyApplyPlatform.packageType == 3)
				{
					if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","292")) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","292");
						return(ret);
					}
				}
				else if (tkeyApplyPlatform.packageType == 5)
				{
					if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","E110")) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","E110");
						return(ret);
					}
				}
				else
				{
					UnionUserErrLog("in UnionDealServiceCode8E00::packageType[%d] is invalid!\n",tkeyApplyPlatform.packageType);
					return(errCodeParameter);
				}
				// modify by lisq 20150104 end

				memset(tmpBuf,0,sizeof(tmpBuf));
				UnionConvertSymmetricKeyStringAlgorithmID(symmetricKeyDB.algorithmID,tmpBuf);

				// �޸��㷨��ʶ
				if ((ret = UnionSetRequestRemoteXMLPackageValue("body/algorithmID",tmpBuf)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/algorithmID",tmpBuf);
					return(ret);
				}

				// ��Կ����
				if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyName",symmetricKeyDB.keyName)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyName",symmetricKeyDB.keyName);
					return(ret);
				}

				// ��Կ����
				if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyType",keyType[i])) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyType",keyTypeGrp[i]);
					return(ret);
				}

				// add by liwj 20150616
				if (keyNameFlag) // ����Կ����ģʽ����Ҫ������Կ����
				{
					// ��Կ����
					snprintf(tmpBuf, sizeof(tmpBuf), "%d", symmetricKeyDB.keyType);
					if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyType",tmpBuf)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyType", tmpBuf);
						return(ret);
					}
				}
				// end

				// ת����Կ������Զ��ƽ̨
				if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E00:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
					return(ret);
				}
				*/
				// remark end by liwj
			}
			switch(mode)
			{
				case 0:
					break;
				case 1:
					strcpy(symmetricKeyDB.keyValue[0].lmkProtectMode,phsmGroupRec->lmkProtectMode);
					// remark by liwj 20150617
					/*
					if (isRemoteApplyKey)	// Զ������,���ش洢
					{
						// ��ȡԶ����Կֵ
						if((ret = UnionReadResponseRemoteXMLPackageValue("body/keyValue",remoteKeyValue,sizeof(remoteKeyValue))) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode8E00:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/keyValue");
							return(ret);
						}

						// ��ȡԶ����ԿУ��ֵ
						if((ret = UnionReadResponseRemoteXMLPackageValue("body/checkValue",remoteCheckValue,sizeof(remoteCheckValue))) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode8E00:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/checkValue");
							return(ret);
						}

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
											UnionUserErrLog("in UnionDealServiceCode8E00:: UnionHsmCmdA6!\n");
											return(ret);
										}
										break;
									default:
										UnionUserErrLog("in UnionDealServiceCode8E00:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
										UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ��汾");
										return(errCodeParameter);
								}

								// ���У��ֵ
								if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,16) != 0)
								{
									UnionUserErrLog("in UnionDealServiceCode8E00:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
									return(errCodeEssc_CheckValue);
								}
								break;
							case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
								// ��Կת�ɱ���LMK�Լ���
								switch(phsmGroupRec->hsmCmdVersionID)
								{
									case	conHsmCmdVerRacalStandardHsmCmd:
									case	conHsmCmdVerSJL06StandardHsmCmd:
										if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,tkeyApplyPlatform.protectKey,remoteKeyValue,symmetricKeyDB.keyValue[0].keyValue,symmetricKeyDB.checkValue)) < 0)
										{
											UnionUserErrLog("in UnionDealServiceCode8E00:: UnionHsmCmdSV!\n");
											return(ret);
										}
										break;
									default:
										UnionUserErrLog("in UnionDealServiceCode8E00:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
										UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ��汾");
										return(errCodeParameter);
								}
								// ���У��ֵ
								if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,16) != 0)
								{
									UnionUserErrLog("in UnionDealServiceCode8E00:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
									return(errCodeEssc_CheckValue);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCode8E00:: symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
								return(errCodeEsscMDL_InvalidAlgorithmID);
						}			
					}
					else
					{
					*/
					// remakr end by  liwj
						memset(remoteCheckValue,0,sizeof(remoteCheckValue));
						switch(symmetricKeyDB.algorithmID)
						{
							case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
								switch(phsmGroupRec->hsmCmdVersionID)
								{
									case	conHsmCmdVerRacalStandardHsmCmd:
									case	conHsmCmdVerSJL06StandardHsmCmd:
										if ((ret = UnionHsmCmdA0(0,symmetricKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen),NULL,symmetricKeyDB.keyValue[0].keyValue,keyByZMK,symmetricKeyDB.checkValue)) < 0)
										{
											UnionUserErrLog("in UnionDealServiceCode8E00:: UnionHsmCmdA0[%d]!\n",i+1);
											return(ret);
										}
										break;
									default:
										UnionUserErrLog("in UnionDealServiceCode8E00:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
										UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ��汾");
										return(errCodeParameter);
								}
								break;
							case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
								switch(phsmGroupRec->hsmCmdVersionID)
								{
									case	conHsmCmdVerRacalStandardHsmCmd:
									case	conHsmCmdVerSJL06StandardHsmCmd:
										if ((ret = UnionHsmCmdWI("0",symmetricKeyDB.keyType,NULL,symmetricKeyDB.keyValue[0].keyValue,keyByZMK,symmetricKeyDB.checkValue)) < 0)
										{
											UnionUserErrLog("in UnionDealServiceCode8E00:: UnionHsmCmdWI[%d]!\n",i+1);
											return(ret);
										}
										break;
									default:
										UnionUserErrLog("in UnionDealServiceCode8E00:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
										UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ��汾");
										return(errCodeParameter);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCode8E00:: symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
								return(errCodeEsscMDL_InvalidAlgorithmID);
						}
					//} //remark by liwj 20150617
					break;
				case 2:
				default:
					UnionUserErrLog("in UnionDealServiceCode8E00:: mode = [%d] [%d]!\n",mode,i+1);
					return(errCodeEsscMDLKeyOperationNotPermitted);
			}

			// �����Կ����
			if (!UnionIsValidSymmetricKeyDB(&symmetricKeyDB))
			{
				UnionUserErrLog("in UnionDealServiceCode8E00:: UnionIsValidSymmetricKeyDB!\n");
				return(errCodeKeyCacheMDL_WrongKeyName);	
			}

			// Զ�̷ַ���Կ
			if (isRemoteDistributeKey)
			{
				// ��ʼ��Զ����Կ����������
				if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E00:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
					return(ret);
				}

				// �޸Ķ�Ӧ������ 
				// modify by lisq 20150104
				//if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","E110")) < 0)
				if (tkeyDistributePlatform.packageType == 3)
				{
					if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","294")) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","294");
						return(ret);
					}
				}
				else if (tkeyDistributePlatform.packageType == 5)
				{
					if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","E110")) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","E110");
						return(ret);
					}
				}
				else
				{
					UnionUserErrLog("in UnionDealServiceCode8E00::packageType[%d] is invalid!\n",tkeyDistributePlatform.packageType);
					return(errCodeParameter);
				}
				// modify by lisq 20150104 end

				memset(tmpBuf,0,sizeof(tmpBuf));
				UnionConvertSymmetricKeyStringAlgorithmID(symmetricKeyDB.algorithmID,tmpBuf);

				// �޸��㷨��ʶ
				if ((ret = UnionSetRequestRemoteXMLPackageValue("body/algorithmID",tmpBuf)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/algorithmID",tmpBuf);
					return(ret);
				}

				// ��Կ����
				if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyName",symmetricKeyDB.keyName)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyName",symmetricKeyDB.keyName);
					return(ret);
				}

				// ��Կ����
				if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyType",keyType[i])) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyType",keyType[i]);
					return(ret);
				}
				// add by liwj 20150617
				if (keyNameFlag)
				{
					// ��Կ����
					snprintf(tmpBuf, sizeof(tmpBuf), "%d", symmetricKeyDB.keyType);
					if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyType",tmpBuf)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyType", tmpBuf);
						return(ret);
					}
				}
				// end

				if (mode == 1)	// ������Կ
				{
					// ������Կ��У��ֵ
					memset(keyByRKM,0,sizeof(keyByRKM));
					memset(remoteCheckValue,0,sizeof(remoteCheckValue));
					switch(symmetricKeyDB.algorithmID)
					{
						case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
							// �ѱ���LMK�Լ���ת��Զ��ZMK����
							switch(phsmGroupRec->hsmCmdVersionID)
							{
								case	conHsmCmdVerRacalStandardHsmCmd:
								case	conHsmCmdVerSJL06StandardHsmCmd:
									if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,symmetricKeyDB.keyValue[0].keyValue,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCode8E00:: Distribute Key UnionHsmCmdA8!\n");
										return(ret);
									}
									break;
								default:
									UnionUserErrLog("in UnionDealServiceCode8E00:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
									UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ��汾");
									return(errCodeParameter);
							}
							// ���У��ֵ
							if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,16) != 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E00:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
								return(errCodeEssc_CheckValue);
							}					
							break;
						case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
							// �ѱ���LMK�Լ���ת��Զ��ZMK����
							switch(phsmGroupRec->hsmCmdVersionID)
							{
								case	conHsmCmdVerRacalStandardHsmCmd:
								case	conHsmCmdVerSJL06StandardHsmCmd:
									if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,symmetricKeyDB.keyValue[0].keyValue,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCode8E00:: Distribute Key UnionHsmCmdST!\n");
										return(ret);
									}
									break;
								default:
									UnionUserErrLog("in UnionDealServiceCode8E00:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
									UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ��汾");
									return(errCodeParameter);
							}
							// ���У��ֵ
							if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,16) != 0)
							{
								UnionUserErrLog("in UnionDealServiceCode8E00:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
								return(errCodeEssc_CheckValue);
							}					
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCode8E00:: Distribute Key symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
							return(errCodeEsscMDL_InvalidAlgorithmID);
					}

					if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyValue",keyByRKM)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyValue",keyByRKM);
						return(ret);
					}

					if ((ret = UnionSetRequestRemoteXMLPackageValue("body/checkValue",remoteCheckValue)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode8E00:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/checkValue",remoteCheckValue);
						return(ret);
					}
				}

				// ת����Կ������Զ��ƽ̨
				if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E00:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
					return(ret);
				}
			}

			if (mode == 0)	// ��������Կ
			{
				if ((ret =  UnionCreateSymmetricKeyDB(&symmetricKeyDB)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E00:: UnionCreateSymmetricKeyDB,keyName[%s]!\n",symmetricKeyDB.keyName);
					isFailed = 1;
					//return(ret);	
				}
				else
					isFailed = 0;
			}
			else
			{
				if ((ret =  UnionGenerateSymmetricKeyDBRec(&symmetricKeyDB)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E00:: UnionGenerateSymmetricKeyDBRec,keyName[%s]!\n",symmetricKeyDB.keyName);
					isFailed = 1;
					//return(ret);	
				}
				else
				{
					isFailed = 0;
					//successNum++;
				}
			}
			if (!keyNameFlag)
			{
				if (isFailed)
				{
					if ((ret = UnionLocateResponseNewXMLPackage("body/detail",++failNum)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode8E00:: UnionLocateResponseNewXMLPackage[%s][%d]!\n","body/detail",failNum);
						return(ret);
					}

					if ((ret = UnionSetResponseXMLPackageValue("keyName",symmetricKeyDB.keyName)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode8E00:: UnionLocateResponseNewXMLPackage[%s][%d]!\n","keyName",failNum);
						return(ret);
					}

					if ((ret = UnionSetResponseXMLPackageValue("status","ʧ��")) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode8E00:: UnionLocateResponseNewXMLPackage[%s][%d]!\n","status",failNum);
						return(ret);
					}
				}
				else
				{
					successNum --;

					if ((ret = UnionLocateResponseNewXMLPackage("body/detail",successNum)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode8E00:: UnionLocateResponseNewXMLPackage[%s][%d]!\n","body/detail",successNum);
						return(ret);
					}

					if ((ret = UnionSetResponseXMLPackageValue("keyName",symmetricKeyDB.keyName)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode8E00:: UnionLocateResponseNewXMLPackage[%s][%d]!\n","keyName",successNum);
						return(ret);
					}

					if ((ret = UnionSetResponseXMLPackageValue("status","�ɹ�")) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode8E00:: UnionLocateResponseNewXMLPackage[%s][%d]!\n","status",successNum);
						return(ret);
					}
				}

				if (keyTotalNum < (keyTotalNum - successNum + 1) + failNum)
				//if (keyTotalNum < (successNum + failNum))
				{
					UnionUserErrLog("in UnionDealServiceCode8E00:: failNum[%d] + successNum[%d] > totalNum[%d]!\n",failNum,keyTotalNum + 1 - successNum,keyTotalNum);
					UnionSetResponseRemark("ʧ����[%d]�ͳɹ���[%d]��������Կ��[%d]",failNum,keyTotalNum + 1 - successNum,keyTotalNum);
					return(errCodeParameter);
				}
				if (failNum == keyTotalNum)
				{
					UnionSetResponseRemark("������Կʧ��");
					return(errCodeParameter);
				}
			}
			else
				return(ret);
		}
	}

	return(0);
}

