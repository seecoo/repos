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
#include "commWithHsmSvr.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "symmetricKeyDB.h"
#include "3DesRacalSyntaxRules.h"
#include "unionHsmCmdVersion.h"

/***************************************
�������:	E601
������:		���㰲ȫ����
��������:	���㰲ȫ����
***************************************/
int UnionDealServiceCodeE601(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret = 0;
	char			mode[1+1];
	char			scheme[1+1];
	char			rootDiscreteData[48+1];
	char			pkDiscreteData[48+1];
	char			sessionFlag[1+1];
	char			sessionFactor[32+1];
	char			iv_cbc[32+1];
	char			encFillData[1024+1];
	int			encFillOffset = 0;
	char			iv_mac[32+1];
	char			macFillData[1024+1];
	char			macFillData_asc[2048+1];
	int			macFillOffset = 0;
	char			mac[32+1];
	char			data[2048+1];
	char			tmpBuf[4096+1];
	char			encFillData_asc[4096+1];
	int			lenOfMacFillData = 0;
	char			rootKeyType[3+1];
	char			pkType[3+1];
	int			rootDiscreteNum = 0;
	int			pkDiscreteNum = 0;
	int			lenOfData = 0;
	char			checkValue[16+1];
	char			keyName[136];

	TUnionSymmetricKeyDB	rootKeyDB;
	PUnionSymmetricKeyValue	prootKeyValue = NULL;

	TUnionSymmetricKeyDB	protectKeyDB;
	PUnionSymmetricKeyValue	pprotectKeyValue = NULL;
	
	// ����ģʽ
	memset(mode,0,sizeof(mode));
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",mode,sizeof(mode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadRequestXMLPackageValue[%s]!\n","body/mode");
		return(ret);
	}
	
	if (mode[0] != '0' && mode[0] != '1')
	{
		UnionUserErrLog("in UnionDealServiceCodeE601:: mode[%s] not 0 or 1!\n",mode);
		UnionSetResponseRemark("����ģʽ[%s]������[0,1]֮��",mode);
		return(errCodeParameter);
	}

	// ������־
	memset(scheme,0,sizeof(scheme));
	if ((ret = UnionReadRequestXMLPackageValue("body/scheme",scheme,sizeof(scheme))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadRequestXMLPackageValue[%s]!\n","body/scheme");
		return(ret);
	}

	if (scheme[0] != '0' && scheme[0] != '1' && scheme[0] != '2')
	{
		UnionUserErrLog("in UnionDealServiceCodeE601:: scheme[%s] not 0 or 1 or 2!\n",scheme);
		UnionSetResponseRemark("������־[%s]������[0,1,2]֮��",scheme);
		return(errCodeParameter);
	}

	// ����Կ����
	memset(&rootKeyDB,0,sizeof(rootKeyDB));
	if ((ret = UnionReadRequestXMLPackageValue("body/rootKeyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadRequestXMLPackageValue[%s]!\n","body/rootKeyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);

	// ��ȡ����Կ
	if ((ret = UnionReadSymmetricKeyDBRec(keyName,1,&rootKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}

	// ��ȡ��Կֵ
	if ((prootKeyValue = UnionGetSymmetricKeyValue(&rootKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(errCodeParameter);
	}

	// ����Կ��ɢ����
	if ((ret = UnionReadRequestXMLPackageValue("body/rootDiscreteData",rootDiscreteData,sizeof(rootDiscreteData))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadRequestXMLPackageValue[%s]!\n","body/rootDiscreteData");
		return(ret);
	}
	else
        {
		rootDiscreteData[ret] = 0;
		UnionFilterHeadAndTailBlank(rootDiscreteData);
		if (strlen(rootDiscreteData) > 0 )
		{
                	if (!UnionIsBCDStr(rootDiscreteData))
                	{
                	        UnionUserErrLog("in UnionDealServiceCodeE601:: rootDiscreteData[%s] is error!\n",rootDiscreteData);
                	        UnionSetResponseRemark("����Կ��ɢ���ݷǷ�,����Ϊʮ��������");
                	        return(errCodeParameter);
                	}
		}
        }

	// ����Կ��ɢ����
	if ((ret = UnionReadRequestXMLPackageValue("body/rootDiscreteNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadRequestXMLPackageValue[%s]!\n","body/rootDiscreteNum");
		return(ret);
	}
	else
	{
		tmpBuf[ret] = 0;
		rootDiscreteNum = atoi(tmpBuf);
	}

	if (rootDiscreteNum < 0 || rootDiscreteNum > 3)
	{
		UnionUserErrLog("in UnionDealServiceCodeE601:: rootDiscreteNum [%d] not between 0 and 3!\n",rootDiscreteNum);
		UnionSetResponseRemark("����Կ��ɢ����[%d]����0��3֮��",rootDiscreteNum);
		return(errCodeParameter);
	}

	// ������Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/pkType",pkType,sizeof(pkType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkType");
		return(ret);
	}
	pkType[ret] = 0;

	if (pkType[0] != '0' && pkType[0] != '1')
	{
		UnionUserErrLog("in UnionDealServiceCodeE601:: pkType[%s] not 0 or 1!\n",pkType);
		UnionSetResponseRemark("������Կ����[%s]������[0,1]֮��",pkType);
		return(errCodeParameter);
	}

	// ������Կ����
	memset(&protectKeyDB,0,sizeof(protectKeyDB));
	memset(keyName,0,sizeof(keyName));
	if ((ret = UnionReadRequestXMLPackageValue("body/protectKeyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadRequestXMLPackageValue[%s]!\n","body/protectKeyName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(keyName);

	// ��ȡ������Կ
	if ((ret = UnionReadSymmetricKeyDBRec(keyName,1,&protectKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}

	// ��ȡ��Կֵ
	if ((pprotectKeyValue = UnionGetSymmetricKeyValue(&protectKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(errCodeParameter);
	}

	if (pkType[0] == '1')
	{
		// ������Կ��ɢ����
		//memset(pkDiscreteData,0,sizeof(pkDiscreteData));
		if ((ret = UnionReadRequestXMLPackageValue("body/pkDiscreteData",pkDiscreteData,sizeof(pkDiscreteData))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkDiscreteData");
			return(ret);
		}
		else
        	{
			pkDiscreteData[ret] = 0;
			UnionFilterHeadAndTailBlank(pkDiscreteData);
			if (strlen(pkDiscreteData) > 0)
			{
                		if (!UnionIsBCDStr(pkDiscreteData))
               	 		{
                        		UnionUserErrLog("in UnionDealServiceCodeE601:: pkDiscreteData[%s] is error!\n",pkDiscreteData);
                        		UnionSetResponseRemark("������Կ��ɢ���ݷǷ�,����Ϊʮ��������");
                        		return(errCodeParameter);
                		}
			}
        	}
			
		// ����Կ��ɢ����
		//memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("body/pkDiscreteNum",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkDiscreteNum");
			return(ret);
		}
		else
		{
			tmpBuf[ret] = 0;
			pkDiscreteNum = atoi(tmpBuf);
		}

		if (pkDiscreteNum < 0 || pkDiscreteNum > 3)
		{
			UnionUserErrLog("in UnionDealServiceCodeE601:: rootDiscreteNum [%d] not between 0 and 3!\n",pkDiscreteNum);
			UnionSetResponseRemark("������Կ��ɢ����[%d]����0��3֮��",pkDiscreteNum);
			return(errCodeParameter);
		}
	}

	// ������Կ��־
	//memset(sessionFlag,0,sizeof(sessionFlag));
	if ((ret = UnionReadRequestXMLPackageValue("body/sessionFlag",sessionFlag,sizeof(sessionFlag))) <= 0)
	{
		//UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadRequestXMLPackageValue[%s]!\n","body/sessionFlag");
		//return(ret);
		strcpy(sessionFlag,"N");
	}
	else
	{
		sessionFlag[ret] = 0;
	}
	if (sessionFlag[0] == 'Y')
	{
		// ��������
		//memset(sessionFactor,0,sizeof(sessionFactor));
		if ((ret = UnionReadRequestXMLPackageValue("body/sessionFactor",sessionFactor,sizeof(sessionFactor))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadRequestXMLPackageValue[%s]!\n","body/sessionFactor");
			return(ret);
		}
		else
                {
			sessionFactor[ret] = 0;
			UnionFilterHeadAndTailBlank(sessionFactor);
                        if (strlen(sessionFactor) > 0)
                        {
				if (!UnionIsBCDStr(sessionFactor))
                        	{
                        	        UnionUserErrLog("in UnionDealServiceCodeE601:: sessionFactor[%s] is error!\n",sessionFactor);
                        	        UnionSetResponseRemark("�������ӷǷ�,����Ϊʮ��������");
                        	        return(errCodeParameter);
                        	}
			}
                }
	}


	if (scheme[0] == '0')
	{
		// IV-CBC
		//memset(iv_cbc,0,sizeof(iv_cbc));
		if ((ret = UnionReadRequestXMLPackageValue("body/iv_cbc",iv_cbc,sizeof(iv_cbc))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadRequestXMLPackageValue[%s]!\n","body/iv_cbc");
			return(ret);
		}
		else
                {
			iv_cbc[ret] = 0;
			UnionFilterHeadAndTailBlank(iv_cbc);
                	if(strlen(iv_cbc) == 0)
                	{
                	        UnionUserErrLog("in UnionDealServiceCodeE601:: iv_cbc can not be null!\n");
                	        UnionSetResponseRemark("iv_cbc����Ϊ��");
                	        return(errCodeParameter);
                	}
                        if (!UnionIsBCDStr(iv_cbc))
                        {
                                UnionUserErrLog("in UnionDealServiceCodeE601:: iv_cbc[%s] is error!\n",iv_cbc);
                                UnionSetResponseRemark("IV-CBC�Ƿ�,����Ϊʮ��������");
                                return(errCodeParameter);
                        }       
                }
	}
	else if (scheme[0] == '2')
	{
		// �����������
		//memset(encFillData,0,sizeof(encFillData));
		if ((ret = UnionReadRequestXMLPackageValue("body/encFillData",encFillData,sizeof(encFillData))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadRequestXMLPackageValue[%s]!\n","body/encFillData");
			return(ret);
		}
		else
                {
			encFillData[ret] = 0;
			UnionFilterHeadAndTailBlank(encFillData);
                        if(strlen(encFillData) == 0)
                        {
                                UnionUserErrLog("in UnionDealServiceCodeE601:: encFillData can not be null!\n");
                                UnionSetResponseRemark("����������ݲ���Ϊ��");
                                return(errCodeParameter);
                        }
                        if (!UnionIsBCDStr(encFillData))
                        {
                                UnionUserErrLog("in UnionDealServiceCodeE601:: encFillData[%s] is error!\n",encFillData);
                                UnionSetResponseRemark("����������ݷǷ�,����Ϊʮ��������");
                                return(errCodeParameter);
                        }
                }

		
		//changed  begin by lusj  20150914
		memset(encFillData_asc, 0, sizeof(encFillData_asc));
		bcdhex_to_aschex(encFillData, strlen(encFillData), encFillData_asc);
		//changed  end by lusj 20150914
		
	
		// �������ƫ��
		//memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("body/encFillOffset",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadRequestXMLPackageValue[%s]!\n","body/encFillOffset");
			return(ret);
		}
		else
		{
			tmpBuf[ret] = 0;
			UnionFilterHeadAndTailBlank(tmpBuf);
                        if(strlen(tmpBuf) == 0)
                        {
                                UnionUserErrLog("in UnionDealServiceCodeE601:: encFillOffset can not be null!\n");
                                UnionSetResponseRemark("�������ƫ�Ʋ���Ϊ��");
                                return(errCodeParameter);
                        }
			if (!UnionIsDigitString(tmpBuf))
                	{
                        	UnionUserErrLog("in UnionDealServiceCodeE601:: encFillOffset[%s] is error!\n",tmpBuf);
                        	UnionSetResponseRemark("�������ƫ��[%s]�Ƿ�,����Ϊ����",tmpBuf);
                        	return(errCodeParameter);
                	}
			encFillOffset=((atoi(tmpBuf)/10)*6+atoi(tmpBuf))*2;//�ײ�U2ָ���ǰ����ݳ���2��תΪ16���ƣ�����Ҫ����
		}
	}

	if (mode[0] == '1')
	{
		// IV-MAC
		memset(iv_mac,0,sizeof(iv_mac));
		if ((ret = UnionReadRequestXMLPackageValue("body/iv_mac",iv_mac,sizeof(iv_mac))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadRequestXMLPackageValue[%s]!\n","body/iv_mac");
			return(ret);
		}
		else
                {
			UnionFilterHeadAndTailBlank(iv_mac);
                        if(strlen(iv_mac) == 0)
                        {
                                UnionUserErrLog("in UnionDealServiceCodeE601:: iv_mac can not be null!\n");
                                UnionSetResponseRemark("iv_mac����Ϊ��");
                                return(errCodeParameter);
                        }
                        if (!UnionIsBCDStr(iv_mac))
                        {
                                UnionUserErrLog("in UnionDealServiceCodeE601:: iv_mac[%s] is error!\n",iv_mac);
                                UnionSetResponseRemark("IV-MAC�Ƿ�,����Ϊʮ��������");
                                return(errCodeParameter);
                        }
                }

		// MAC�������
		memset(macFillData,0,sizeof(macFillData));
		if ((lenOfMacFillData = UnionReadRequestXMLPackageValue("body/macFillData",macFillData,sizeof(macFillData))) < 0)
		{
			//UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadRequestXMLPackageValue[%s]!\n","body/macFillData");
			//return(lenOfMacFillData);
			lenOfMacFillData = 0;
		}
		else
		{
			UnionFilterHeadAndTailBlank(macFillData);
			if (strlen(macFillData) > 0)
			{
				if (!UnionIsBCDStr(macFillData))
				{
					UnionUserErrLog("in UnionDealServiceCodeE601:: macFillData[%s] is error!\n",macFillData);
					UnionSetResponseRemark("MAC������ݷǷ�,����Ϊʮ��������");
					return(errCodeParameter);
				}
			}

			//changed  begin by lusj  20150914
			memset(macFillData_asc, 0, sizeof(macFillData_asc));
			bcdhex_to_aschex(macFillData, strlen(macFillData), macFillData_asc);
		    //changed  end by lusj 20150914
		}

		if (lenOfMacFillData != 0) 
		{
			// MAC���ƫ��
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((ret = UnionReadRequestXMLPackageValue("body/macFillOffset",tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE601:: UnionReadRequestXMLPackageValue[%s]!\n","body/macFillOffset");
				return(ret);
			}
			else
                	{
				UnionFilterHeadAndTailBlank(tmpBuf);
                        	if(strlen(tmpBuf) == 0)
                        	{
                        	        UnionUserErrLog("in UnionDealServiceCodeE601:: macFillOffset can not be null!\n");
                        	        UnionSetResponseRemark("MAC���ƫ�Ʋ���Ϊ��");
                        	        return(errCodeParameter);
                        	}
                        	if (!UnionIsDigitString(tmpBuf))
                        	{
                                	UnionUserErrLog("in UnionDealServiceCodeE601:: macFillOffset[%s] is error!\n",tmpBuf);
                                	UnionSetResponseRemark("MAC���ƫ��[%s]�Ƿ�,����Ϊ����",tmpBuf);
                                	return(errCodeParameter);
                        	}
                        //	macFillOffset = atoi(tmpBuf);
				//changed  begin by lusj  20150914
				macFillOffset=((atoi(tmpBuf)/10)*6+atoi(tmpBuf))*2;//�ײ�U2ָ���ǰ����ݳ���2��תΪ16���ƣ�����Ҫ����
				//changed  end by lusj 20150914
                	}
		}
	}

	// ��ȡ����Կ����
	memset(rootKeyType,0,sizeof(rootKeyType));
	if ((ret = UnionTranslateHsmKeyTypeString(rootKeyDB.keyType,rootKeyType)) < 0) 
	{
		UnionUserErrLog("in UnionDealServiceCodeE601:: UnionTranslateHsmKeyTypeString[%d]!\n",rootKeyDB.keyType);
		return(ret);
	}

	memset(mac,0,sizeof(mac));
	memset(data,0,sizeof(data));
	memset(checkValue,0,sizeof(checkValue));
	switch(rootKeyDB.algorithmID)
	{
		case	conSymmetricAlgorithmIDOfDES:
			switch(phsmGroupRec->hsmCmdVersionID)
        		{
                		case conHsmCmdVerRacalStandardHsmCmd:
                		case conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmdU2("T",mode,scheme,rootKeyType,rootKeyDB.keyValue[0].keyValue,0,rootDiscreteNum,rootDiscreteData,pkType,
							protectKeyDB.keyValue[0].keyValue,0,pkDiscreteNum,pkDiscreteData,sessionFlag,sessionFactor,iv_cbc,
							strlen(encFillData_asc),encFillData_asc,encFillOffset,iv_mac,
							strlen(macFillData_asc),macFillData_asc,macFillOffset,
							mac,&lenOfData,data,checkValue)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE601:: UnionHsmCmdU2!\n");
						return(ret);
					}
					break;
                		default:
                        		UnionUserErrLog("in UnionDealServiceCodeE601:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        		UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                        		return(errCodeParameter);
        		}
			break;
		case	conSymmetricAlgorithmIDOfSM4:
			switch(phsmGroupRec->hsmCmdVersionID)
        		{
                		case conHsmCmdVerRacalStandardHsmCmd:
                		case conHsmCmdVerSJL06StandardHsmCmd:
					if ((ret = UnionHsmCmdWH("R",mode,scheme,rootKeyType,prootKeyValue->keyValue,0,rootKeyDB.checkValue,rootDiscreteNum, \
								rootDiscreteData,pkType,pprotectKeyValue->keyValue,0,pkDiscreteNum,pkDiscreteData, \
								protectKeyDB.checkValue,sessionFlag,sessionFactor,iv_cbc, \
								strlen(encFillData_asc),encFillData_asc,encFillOffset,iv_mac,	\
								strlen(macFillData_asc),macFillData_asc,macFillOffset,	\
								mac,&lenOfData,data,checkValue)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE601:: UnionHsmCmdWH!\n");
						return(ret);
					}
					break;
                		default:
                        		UnionUserErrLog("in UnionDealServiceCodeE601:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        		UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
                        		return(errCodeParameter);
        		}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE601:: rootKeyDB.algorithmID[%d] error!\n",rootKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}

	// MAC
	if (mode[0] == '1')
	{
		if ((ret = UnionSetResponseXMLPackageValue("body/mac",mac)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE601:: UnionSetResponseXMLPackageValue[%s]","body/mac");
			return(ret);
		}
	}
	
	// ��������
	if (rootKeyDB.algorithmID == conSymmetricAlgorithmIDOfDES)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		bcdhex_to_aschex(data,lenOfData,tmpBuf);
	
		if ((ret = UnionSetResponseXMLPackageValue("body/data",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE601:: UnionSetResponseXMLPackageValue[%s]!\n","body/data");	
			return(ret);
		}
	}
	else
	{
		if ((ret = UnionSetResponseXMLPackageValue("body/data",data)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE601:: UnionSetResponseXMLPackageValue[%s]!\n","body/data");	
			return(ret);
		}
	}

        return(0);
}
