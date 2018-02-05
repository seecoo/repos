//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "defaultIBMPinOffsetDef.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmdVersion.h"

/***************************************
�������:	E303
������:		E303���ܽű�����(527)
��������:	E303���ܽű�����(527)
***************************************/

static int UnionCheckIVIsH(const char *IV, int lenOfIV);

int UnionDealServiceCodeE303(PUnionHsmGroupRec phsmGroupRec)
{
	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;

	char 			pan[32];
	char 			atc[4+1];
	char 			plainData[4096+1];
	char			versionTmp[8+1];
	char			mode[1+1];
	char			ciperData[4096+1];
	char			ciperDataAsc[4096+1];
	char			appID[40+1];
	char			ownerName[40+1];
	char			keyType[40+1];
	char			iv[64+1];
	char			keyName[136];

	int			ret;
	int 			version;
	int 			lenOfCiperData;

	memset(&symmetricKeyDB,		0,	sizeof(symmetricKeyDB));
	memset(appID,		0,	sizeof(appID));
	memset(ownerName,	0,	sizeof(ownerName));
	memset(keyType,		0,	sizeof(keyType));
	
	// mk-ac��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName", keyName, sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE303:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;

	// .....������������ .......
	if ((ret = UnionReadRequestXMLPackageValue("body/version", versionTmp, sizeof(versionTmp))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE303:: UnionReadRequestXMLPackageValue[%s]!\n","body/version");
		return(ret);
	}
	else
        {
		versionTmp[ret] = 0;
		UnionFilterHeadAndTailBlank(versionTmp);
                if (strlen(versionTmp) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE303:: versionTmp can not be null!\n");
                        UnionSetResponseRemark("��Կ�İ汾�Ų���Ϊ��!");
                        return(errCodeParameter);
                }
                if (!UnionIsDigitString(versionTmp))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE303:: version[%s] is error!\n",versionTmp);
                        UnionSetResponseRemark("��Կ�İ汾��[%s]�Ƿ�,����Ϊ����",versionTmp);
                        return(errCodeParameter);
                }
                version = atoi(versionTmp);
        }
	
	UnionAnalysisPBOCSymmetricKeyName(keyName, version, appID, ownerName, keyType);

	snprintf(keyName,sizeof(keyName), "%s.%s.%s", appID, ownerName, keyType);

	if ((ret = UnionReadRequestXMLPackageValue("body/pan",  pan, sizeof(pan)))< 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE303:: UnionReadRequestXMLPackageValue[%s]!\n","body/pan");
		return(ret);
	}
	else
        {
		pan[ret] = 0;
		UnionFilterHeadAndTailBlank(pan);
                if (strlen(pan) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE303:: pan can not be null!\n");
                        UnionSetResponseRemark("���Ż򣨿���+�����кţ�����Ϊ��!");
                        return(errCodeParameter);
                }
                if (!UnionIsBCDStr(pan))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE303:: pan[%s] is error!\n",pan);
                        UnionSetResponseRemark("  ���Ż򣨿���+�����кţ�[%s]�Ƿ�,����Ϊʮ��������",pan);
                        return(errCodeParameter);
                }
        }
	
	if ((ret = UnionReadRequestXMLPackageValue("body/atc", atc, sizeof(atc)))< 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE303:: UnionReadRequestXMLPackageValue[%s]!\n","body/atc");
		return(ret);
	}
	else
        {
		atc[ret] = 0;
		UnionFilterHeadAndTailBlank(atc);
                if (strlen(atc) != 4)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE303:: atc can not be null or the len of atc !=4!\n");
                        UnionSetResponseRemark("��ɢ�������Ӳ���Ϊ�ջ򳤶Ȳ�Ϊ4!");
                        return(errCodeParameter);
                }
                if (!UnionIsBCDStr(atc))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE303:: atc[%s] is error!\n",atc);
                        UnionSetResponseRemark("��ɢ��������[%s]�Ƿ�,����Ϊʮ��������",atc);
                        return(errCodeParameter);
                }
        }

	if ((ret = UnionReadRequestXMLPackageValue("body/mode", mode, sizeof(mode)))< 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE303:: UnionReadRequestXMLPackageValue[%s]!\n","body/mode");
		return(ret);
	}
	else
	{
		mode[ret] = 0;
		if (mode[0] != '0' && mode[0] != '1')
		{
			UnionUserErrLog("in UnionDealServiceCodeE303:: mode[%s] is error!\n",mode);
                        UnionSetResponseRemark("����ģʽ[%s]�Ƿ�,����Ϊ0����Ϊ1",mode);
                        return(errCodeParameter);
		}
	}

	if ((ret = UnionReadRequestXMLPackageValue("body/plainData", plainData, sizeof(plainData)))< 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE303:: UnionReadRequestXMLPackageValue[%s]!\n","body/plainData");
		return(ret);
	}
	else
        {
		plainData[ret] = 0;
 		UnionFilterHeadAndTailBlank(plainData);
                if (strlen(plainData) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE303:: plainData can not be null!\n");
                        UnionSetResponseRemark("�������ݲ���Ϊ��!");
                        return(errCodeParameter);
                }
                if (!UnionIsBCDStr(plainData))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE303:: plainData[%s] is error!\n",plainData);
                        UnionSetResponseRemark("�������ݷǷ�,����Ϊʮ��������");
                        return(errCodeParameter);
                }
        }

	if(mode[0] == '0')
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/iv", iv, sizeof(iv)))<= 0)
			memset(iv, '0', 32);
		else
			iv[ret] = 0;
		
		//static int UnionCheckIVIsH(const char *IV, int lenOfIV)

		if ((strlen(iv) > 32) || (UnionCheckIVIsH(iv, strlen(iv)) < 0) || (!UnionIsBCDStr(pan)))
		{
			UnionUserErrLog("in UnionDealServiceCodeE303:: iv format err\n");
			return errCodeOffsetOfPBOC_ivFormatErr;
		}
	
	}
		
        // ��ȡ�Գ���Կ
        if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1, &symmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE303:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", keyName);
                return(ret);
        }

	// ��ȡ�Գ���Կֵ
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE303:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(errCodeParameter);
	}

	switch(symmetricKeyDB.algorithmID)
	{
		case    conSymmetricAlgorithmIDOfDES:   // DES�㷨
			switch(phsmGroupRec->hsmCmdVersionID)
        		{
                		case conHsmCmdVerRacalStandardHsmCmd:
                		case conHsmCmdVerSJL06StandardHsmCmd:
					if(mode[0] == '1')//ECB
					{
						if ((ret = UnionHsmCmdU0("1", "9", 0, psymmetricKeyValue->keyValue, NULL, pan, NULL, atc, strlen(plainData),  plainData, &lenOfCiperData, ciperData)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE303::  UnionHsmCmdU0 return[%d]!\n", ret);
							return ret;
						}
					}
					else if(mode[0] == '0')//BCB
					{
						if ((ret = UnionHsmCmdU0("0", "9", 0, psymmetricKeyValue->keyValue, iv, pan, NULL, atc, strlen(plainData),  plainData, &lenOfCiperData, ciperData)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE303::  UnionHsmCmdU0 return[%d]!\n", ret);
							return ret;
						}
					}
					else
					{
						UnionUserErrLog("in UnionDealServiceCodeE303::  mode=[%s]!\n", mode);
						return errCodeOffsetOfPBOC_encryptModeErr;	
					}
					break;
                		default:
                        		UnionUserErrLog("in UnionDealServiceCodeE303:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        		UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                        		return(errCodeParameter);
        		}
			break;

		case    conSymmetricAlgorithmIDOfSM4:   // SM4�㷨
			if ((ret = UnionHsmCmdWB(mode, "A", 0, psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue,pan, atc,iv, strlen(plainData),  plainData, &lenOfCiperData, ciperData)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE303::  UnionHsmCmdWB return[%d]!\n", ret);
				return ret;
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE111:: key [%s] algorithmID = [%d] illegal!\n", symmetricKeyDB.keyName, symmetricKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	bcdhex_to_aschex(ciperData, lenOfCiperData, ciperDataAsc);
	ciperDataAsc[lenOfCiperData*2] = 0;

	//����ARPC
	if ((ret = UnionSetResponseXMLPackageValue("body/ciperData", ciperDataAsc)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE302:: UnionSetResponseXMLPackageValue aprc[%s]!\n", ciperDataAsc);
		return(ret);
	}

	return(0);
}

static int UnionCheckIVIsH(const char *IV, int lenOfIV)
{
	int i = 0;

	for(; i!=lenOfIV; i++)
	{
		if (IV[i] <'0' || IV[i] >'F')
			return -1;
	}

	return 0;	
}
