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
�������:	E302
������:		E302 ����ARPC(526/529)
��������:	E302 ����ARPC(526/529)
***************************************/
void UnionGetResponseRemarkByResponseCodeE302(int retCode);
int UnionDealServiceCodeE302(PUnionHsmGroupRec phsmGroupRec) 
{
	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;

	char 			pan[32]; 
	char                    atc[10+1];
	char                  	arqcData[2048+1]; 
	char                    arqc[20+1];
	char			versionTmp[8+1]; 
	char                    iccTypeTmp[10+1]; 
	char			verifyArqcTmp[10+1];
	char			arcTmp[4+1];
	char			arc[4+1];
        char    		arpcData[32+1];
	char			appID[40+1];
	char			ownerName[40+1];
	char			keyType[40+1];	

	int                     iccType;
	int			verifyArqc;
	
	int			ret;
	int			version;	
	char			keyName[136];

	memset(&symmetricKeyDB,		0, 	sizeof(symmetricKeyDB));
	memset(appID,		0,	sizeof(appID));
	memset(ownerName,	0,	sizeof(ownerName));
	memset(keyType,		0,	sizeof(keyType));

	// mk-ac��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName", keyName, sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE302:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;

	// .....������������ .......
	if ((ret = UnionReadRequestXMLPackageValue("body/version", versionTmp, sizeof(versionTmp))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE302:: UnionReadRequestXMLPackageValue[%s]!\n","body/version");
		return(ret);
	}
	else
	{
		versionTmp[ret] = 0;
		UnionFilterHeadAndTailBlank(versionTmp);
                if (strlen(versionTmp) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE302:: versionTmp can not be null!\n");
                        UnionSetResponseRemark("��Կ�İ汾�Ų���Ϊ��!");
                        return(errCodeParameter);
                }
		if (!UnionIsDigitString(versionTmp))
    		{
        		UnionUserErrLog("in UnionDealServiceCodeE302:: version[%s] is error!\n",versionTmp);
        		UnionSetResponseRemark("��Կ�İ汾��[%s]�Ƿ�,����Ϊ����",versionTmp);
        		return(errCodeParameter);
    		}
		version = atoi(versionTmp);
	}

	UnionAnalysisPBOCSymmetricKeyName(keyName, version, appID, ownerName, keyType);
	
	snprintf(keyName,sizeof(keyName),"%s.%s.%s", appID, ownerName, keyType);

	UnionDebugLog("in UnionDealServiceCodeE302:: keyName:[%s]\n", keyName);	

	if ((ret = UnionReadRequestXMLPackageValue("body/pan",  pan, sizeof(pan)))< 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE302:: UnionReadRequestXMLPackageValue[%s]!\n","body/pan");
		return(ret);
	}
	else
        {
		pan[ret] = 0;
		UnionFilterHeadAndTailBlank(pan);
                if (strlen(pan) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE302:: pan can not be null!\n");
                        UnionSetResponseRemark("���Ż򣨿���+�����кţ�����Ϊ��!");
                        return(errCodeParameter);
                }
                if (!UnionIsBCDStr(pan))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE302:: pan[%s] is error!\n",pan);
                        UnionSetResponseRemark("���Ż򣨿���+�����кţ�[%s]�Ƿ�,����Ϊʮ��������",pan);
                        return(errCodeParameter);
                }
        }

	if ((ret = UnionReadRequestXMLPackageValue("body/atc", atc, sizeof(atc)))< 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE302:: UnionReadRequestXMLPackageValue[%s]!\n","body/atc");
		return(ret);
	}
	else
        {
		atc[ret] = 0;
		UnionFilterHeadAndTailBlank(atc);
                if (strlen(atc) != 4)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE302:: atc can not be null or the len of atc != 4!\n");
                        UnionSetResponseRemark("��ɢ�������Ӳ���Ϊ�ջ򳤶Ȳ�Ϊ4!");
                        return(errCodeParameter);
                }
                if (!UnionIsBCDStr(atc))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE302:: atc[%s] is error!\n",atc);
                        UnionSetResponseRemark("��ɢ��������[%s]�Ƿ�,����Ϊʮ��������",atc);
                        return(errCodeParameter);
                }
        }

	if ((ret = UnionReadRequestXMLPackageValue("body/arqcData", arqcData, sizeof(arqcData)))< 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE302:: UnionReadRequestXMLPackageValue[%s]!\n", "body/arqcData");
		return(ret);
	}
	else
        {
		arqcData[ret] = 0;
		UnionFilterHeadAndTailBlank(arqcData);
                if (strlen(arqcData) == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE302:: arqcData can not be null!\n");
                        UnionSetResponseRemark("����ARQCʹ�õ����ݲ���Ϊ��!");
                        return(errCodeParameter);
                }
                if (!UnionIsBCDStr(arqcData))
                {
                        UnionUserErrLog("in UnionDealServiceCodeE302:: arqcData[%s] is error!\n",arqcData);
                        UnionSetResponseRemark("����ARQCʹ�õ����ݷǷ�,����Ϊʮ��������");
                        return(errCodeParameter);
                }
        }

	if ((ret = UnionReadRequestXMLPackageValue("body/verifyArqc", verifyArqcTmp, sizeof(verifyArqcTmp)))< 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE302:: UnionReadRequestXMLPackageValue[%s]!\n","body/verifyArqc");
		return(ret);
	}
	verifyArqcTmp[ret] = 0;
	verifyArqc = atoi(verifyArqcTmp);

//	if (verifyArqc)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/arqc", arqc, sizeof(arqc)))< 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE302:: UnionReadRequestXMLPackageValue[%s]!\n","body/arqc");
			return(ret);
		}
		else
        	{
			arqc[ret] = 0;
			UnionFilterHeadAndTailBlank(arqc);
                	if (strlen(arqc) == 0)
                	{
                        	UnionUserErrLog("in UnionDealServiceCodeE302:: arqc can not be null!\n");
                        	UnionSetResponseRemark("����֤��TC/ARQC/ACC����Ϊ��!");
                        	return(errCodeParameter);
                	}
                	if (!UnionIsBCDStr(arqc))
                	{
                        	UnionUserErrLog("in UnionDealServiceCodeE302:: arqc[%s] is error!\n",arqc);
                        	UnionSetResponseRemark("����֤��TC/ARQC/ACC[%s]�Ƿ�,����Ϊʮ��������",arqc);
                        	return(errCodeParameter);
                	}
		}
        }
/*
	if ((ret = UnionReadRequestXMLPackageValue("body/iccType", iccTypeTmp, sizeof(iccTypeTmp)))< 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE302:: UnionReadRequestXMLPackageValue[%s]!\n","body/iccType");
		return(ret);
	}
	iccTypeTmp[ret] = 0;
	UnionFilterHeadAndTailBlank(iccTypeTmp);
        if (strlen(iccTypeTmp) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE302:: iccTypeTmp can not be null!\n");
                UnionSetResponseRemark("Ic�����Ͳ���Ϊ��!");
                return(errCodeParameter);
        }
	if (!UnionIsDigitString(iccTypeTmp))	
	{	
		UnionUserErrLog("in UnionDealServiceCodeE302:: iccType formate error iccType[%s]\n", iccTypeTmp);
		return errCodeOffsetOfPBOC_iccTypeERR;
	}
	iccType = atoi(iccTypeTmp);
*/
	if ((ret = UnionReadRequestXMLPackageValue("body/arc", arcTmp, sizeof(arcTmp)))< 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE302:: UnionReadRequestXMLPackageValue[%s]!\n","body/arc");
		return(ret);
	}
	arcTmp[ret] = 0;	
	UnionFilterHeadAndTailBlank(arcTmp);
        if (strlen(arcTmp) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE302:: arcTmp can not be null!\n");
                UnionSetResponseRemark("��Ȩ��Ӧ�벻��Ϊ��!");
                return(errCodeParameter);
        }
	if (!UnionIsBCDStr(arcTmp))
	{
		UnionUserErrLog("in UnionDealServiceCodeE302:: arc Type err!\n");
		return(errCodeOffsetOfPBOC_arcFormatErr);
	}

	if (strlen(arcTmp) == 4)
		aschex_to_bcdhex(arcTmp, 4, arc);
	else
		strcpy(arc,arcTmp);
	arc[2] = 0;
		
        // ��ȡ�Գ���Կ
        if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1, &symmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE302:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", keyName);
                return(ret);
        }

	// ��ȡ�Գ���Կֵ
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE301:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(errCodeParameter);
	}

	//modify begin  by lusj 20151111  ���ڹ����㷨û���õ������͡����Ե��������ԿΪSM4ʱ���ɲ����뿨����
	if(symmetricKeyDB.algorithmID == conSymmetricAlgorithmIDOfDES)
	{
		
		
		if ((ret = UnionReadRequestXMLPackageValue("body/iccType", iccTypeTmp, sizeof(iccTypeTmp)))< 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE302:: UnionReadRequestXMLPackageValue[%s]!\n","body/iccType");
			return(ret);
		}
		iccTypeTmp[ret] = 0;
		UnionFilterHeadAndTailBlank(iccTypeTmp);
			if (strlen(iccTypeTmp) == 0)
			{
					UnionUserErrLog("in UnionDealServiceCodeE302:: iccTypeTmp can not be null!\n");
					UnionSetResponseRemark("Ic�����Ͳ���Ϊ��!");
					return(errCodeParameter);
			}
		if (!UnionIsDigitString(iccTypeTmp))	
		{	
			UnionUserErrLog("in UnionDealServiceCodeE302:: iccType formate error iccType[%s]\n", iccTypeTmp);
			return (errCodeOffsetOfPBOC_iccTypeERR);
		}
		iccType = atoi(iccTypeTmp);
	}
	//modify end  by lusj 20151111

	memset(arpcData, 0, sizeof(arpcData));
	switch(symmetricKeyDB.algorithmID)
	{
		case    conSymmetricAlgorithmIDOfDES:   // DES�㷨
			switch(phsmGroupRec->hsmCmdVersionID)
        		{
                		case conHsmCmdVerRacalStandardHsmCmd:
                		case conHsmCmdVerSJL06StandardHsmCmd:
					if(verifyArqc == 1)		
					{
						if(iccType == 0)
						{
							if ( (ret =  UnionHsmCmdKX("1", "9", 0, psymmetricKeyValue->keyValue, NULL, 0, pan, NULL, atc, strlen(arqcData), arqcData, arqc, arc, NULL, 0,  NULL, arpcData)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE302:: UnionHsmCmdKX return=[%d]!\n", ret);
								UnionGetResponseRemarkByResponseCodeE302(ret);
								return(ret);
							}
						}
						else if(iccType == 1)
						{
							if ( (ret =  UnionHsmCmdKX("6", "4", 0, psymmetricKeyValue->keyValue, NULL, 0, pan, NULL, atc, strlen(arqcData), arqcData, arqc, arc, NULL, 0, NULL, arpcData)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE302:: UnionHsmCmdKX return=[%d]!\n", ret);
								UnionGetResponseRemarkByResponseCodeE302(ret);
								return(ret);
							}
						}
						else
						{
							UnionUserErrLog("in UnionDealServiceCodeE302:: iccType =[%d] error!\n", iccType);
							return (errCodeOffsetOfPBOC_iccTypeERR);
						}
					}
					else if(verifyArqc == 0)	
					{
						if ( (ret =  UnionHsmCmdKX("2", "9", 0, psymmetricKeyValue->keyValue, NULL, 0, pan, NULL, atc, strlen(arqcData), arqcData, arqc, arc, NULL, 0,  NULL, arpcData)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE302:: UnionHsmCmdKX return=[%d]!\n", ret);
							UnionGetResponseRemarkByResponseCodeE302(ret);
							return(ret);
						}
					}
					else
					{
						// modify by leipp 20151215 ,�����룬��-1��ΪerrCodeParameter
						UnionUserErrLog("in UnionDealServiceCodeE302:: verifyArqc=[%d] error!\n", verifyArqc);
						return (errCodeParameter);
						// modify end 20151215
					}
					break;
                		default:
                        		UnionUserErrLog("in UnionDealServiceCodeE302:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        		UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                        		return(errCodeParameter);
        		}
			break;

		case    conSymmetricAlgorithmIDOfSM4:   // SM4�㷨
			// add by liwj 20141230  WG �е� arc����Ҫ�� bcd ���
			if (strlen(arcTmp) == 4)
				strcpy(arc,arcTmp);
			else
				bcdhex_to_aschex(arcTmp, 2, arc);
			arc[4] = 0;
			// add end
			if (verifyArqc == 0)
			{
				if ((ret = UnionHsmCmdWG("2","0",0,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue,strlen(pan),pan,atc,strlen(arqcData),arqcData,arqc,arc,arpcData)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE302:: UnionHsmCmdWG ret = [%d]\n",ret);
					UnionGetResponseRemarkByResponseCodeE302(ret);
					return(ret);
				}
			}
			else if (verifyArqc == 1)
			{
				if ((ret = UnionHsmCmdWG("1","0",0,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue,strlen(pan),pan,atc,strlen(arqcData),arqcData,arqc,arc,arpcData)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE302:: UnionHsmCmdWG ret = [%d]\n",ret);
					UnionGetResponseRemarkByResponseCodeE302(ret);
					return(ret);
				}
			} // add by leipp 20151215
			else
			{
				UnionUserErrLog("in UnionDealServiceCodeE302:: verifyArqc=[%d] error!\n", verifyArqc);
				return (errCodeParameter);
			} // add end 
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE302:: key [%s] algorithmID = [%d] illegal!\n", symmetricKeyDB.keyName, symmetricKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	//bcdhex_to_aschex(arpcData, 8, arpcDataAsc);


	//����ARPC
	if ((ret = UnionSetResponseXMLPackageValue("body/arpc", arpcData)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE302:: UnionSetResponseXMLPackageValue aprc[%s]!\n", arpcData);
		return(ret);
	}

	return(0);
}

void UnionGetResponseRemarkByResponseCodeE302(int retCode)
{
	switch(retCode)
	{
		case	errCodeOffsetOfHsmReturnCodeMDL-1:
			UnionSetResponseRemark("ARQC/TC/AACУ��ʧ��/��ԿУ��ֵ��");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-4:
			UnionSetResponseRemark("ģʽ��־��");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-5:
			UnionSetResponseRemark("δ����ķ���ID");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-10:
			UnionSetResponseRemark("MK-AC��żУ���");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-12:
			UnionSetResponseRemark("�û��洢��û��װ����Կ");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-13:
			UnionSetResponseRemark("LMK����");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-15:
			UnionSetResponseRemark("�������ݴ�");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-33:
			UnionSetResponseRemark("��Կ������");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-36:
			UnionSetResponseRemark("�㷨��־��");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-52:
			UnionSetResponseRemark("�Ƿ���B/Hѡ��");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-80:
			UnionSetResponseRemark("���ݳ��ȴ�");
			break;
		case	errCodeOffsetOfHsmReturnCodeMDL-81:
			UnionSetResponseRemark("PAN���ȴ�");
			break;
	}
}
