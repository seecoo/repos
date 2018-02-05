
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
#include "symmetricKeyDB.h"
#include "asymmetricKeyDB.h"
#include "unionHsmCmd.h"

/***************************************

�������:	EEH1
������:  	EEH1 ��˽Կ���ܺ�ĵ�¼����ת��Ϊ������EDK����
��������:	EEH1 ��˽Կ���ܺ�ĵ�¼����ת��Ϊ������EDK����

***************************************/
int UnionDealServiceCodeEEH1(PUnionHsmGroupRec phsmGroupRec)
{

	int             ret = 0;
	int		lenOfVK = 0;
	int		lenOfPinByPK = 0;
	char		mode[2];
        char            pinByPK[1032];
	char		vkValue[2568];
        char  		pinOffset[64];
	char		edkName[136];	     //edk����
	char		keyName[136];	     //edk����
        int 		vkIndex = 0;        //˽Կ����
	char		fillMode[8];

	TUnionSymmetricKeyDB		edk;
	PUnionSymmetricKeyValue         pedkValue = NULL;
	TUnionAsymmetricKeyDB           asymmetricKeyDB;

	// ��ȡ���ģʽ
	if ((ret = UnionReadRequestXMLPackageValue("body/fillMode", fillMode, sizeof(fillMode))) <= 0)
		strcpy(fillMode,"1");
	else
		fillMode[ret] = 0;

	if (fillMode[0] != '0' && fillMode[0] != '1')
	{
                UnionUserErrLog("in UnionDealServiceCodeEEH1:: fillMode[%s] not in [0,1]!\n",fillMode);
                return(errCodeParameter);
	}

	//��ȡmode
	if ((ret = UnionReadRequestXMLPackageValue("body/mode", mode, sizeof(mode))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH1:: UnionReadRequestXMLPackageValue[%s]!\n","body/mode");
                return(ret);
        }
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH1:: mode is null!\n");
		return(errCodeParameter);
	}
	mode[ret] = 0;

	if(mode[0] == '0')
	{
		//��ȡ˽Կ����
  		memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));
		if ((ret = UnionReadRequestXMLPackageValue("body/rsaName", keyName, sizeof(keyName))) < 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeEEH1:: UnionReadRequestXMLPackageValue[%s]!\n","body/rsaName");
                	return(ret);
        	}
		keyName[ret] = 0;

        	// ��ȡ��Կ��Կ��Ϣ     
        	if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,1,&asymmetricKeyDB)) < 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeEEH1:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",keyName);
                	return(ret);
        	}

		// ����Ƿ��������
        	if (asymmetricKeyDB.vkStoreLocation != 0)
			vkIndex = atoi(asymmetricKeyDB.vkIndex);
		else
		{
			vkIndex = 99;	
			lenOfVK = snprintf(vkValue,sizeof(vkValue),"%s",asymmetricKeyDB.vkValue);
		}
	}
	else if (mode[0] == '1')
	{
                //vkValue
                if ((lenOfVK = UnionReadRequestXMLPackageValue("body/vkValue",vkValue, sizeof(vkValue))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeEEH1:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkValue");
                        return(lenOfVK);
                }
		vkValue[lenOfVK] = 0;
		vkIndex = 99;
	}
	else	
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH1:: mode[%s] not in [0,1]!\n",mode);
		return(errCodeParameter);
	}
	
        // ��Կ���ܵ�����
	if ((lenOfPinByPK = UnionReadRequestXMLPackageValue("body/pinByPK",pinByPK, sizeof(pinByPK))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH1:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinByPK");
                return(lenOfPinByPK);
        }
        else if (lenOfPinByPK == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH1:: pinByPK is null!\n");
                return(errCodeParameter);
        }
	pinByPK[lenOfPinByPK] = 0;

	if ((ret = UnionReadRequestXMLPackageValue("body/edkName",edkName, sizeof(edkName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH1:: UnionReadRequestXMLPackageValue[%s]!\n","body/edkName");
                return(ret);
        }
	else if (ret == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH1:: edkName is null!\n");
                return(errCodeParameter);
        }
	edkName[ret] = 0;

	// ��ȡ��Կ
        memset(&edk,0,sizeof(edk));
        if ((ret = UnionReadSymmetricKeyDBRec(edkName,1,&edk)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH1:: UnionReadSymmetricKeyDBRec[%s]!\n",edkName);
                return(ret);
        }

	// ��ȡ�Գ���Կֵ
        if ((pedkValue = UnionGetSymmetricKeyValue(&edk,phsmGroupRec->lmkProtectMode)) == NULL)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH1:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
                UnionSetResponseRemark("��ȡ��Կֵʧ��");
                return(errCodeParameter);
        }

	// �������ָ�����PIN
	switch(edk.algorithmID)
        {
                case    conSymmetricAlgorithmIDOfDES:   // DES�㷨
                        if ((ret = UnionHsmCmd4E(atoi(fillMode), vkIndex,lenOfPinByPK, pinByPK, lenOfVK,vkValue,pedkValue->keyValue, pinOffset)) < 0)
                        {
                                UnionUserErrLog("in UnionDealServiceCodeEEH1: UnionHsmCmd4E ret[%d]!\n", ret);
                                return(ret);
                        }
			pinOffset[ret] = 0;
                        break;
                case    conSymmetricAlgorithmIDOfSM4:   // SM4�㷨
                default:
                        UnionUserErrLog("in UnionDealServiceCodeEEH1:: key [%s] algorithmID = [%d] illegal!\n", edk.keyName, edk.algorithmID);
                        return(errCodeEsscMDL_InvalidAlgorithmID);
        }
	
	//��ȡpinOffset
	if ((ret = UnionSetResponseXMLPackageValue("body/pinOffset", pinOffset)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH1: UnionSetResponseXMLPackageValue [body/pinOffset]!\n");
		return(ret);
	}

	return 0;
}
