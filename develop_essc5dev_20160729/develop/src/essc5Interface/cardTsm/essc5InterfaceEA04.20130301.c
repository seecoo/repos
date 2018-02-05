#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionHsmCmd.h"
#include "unionHsmCmdJK.h"
#include "unionXMLPackage.h"
#include "symmetricKeyDB.h"
#include "UnionLog.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "essc5Interface.h"
#include "base64.h"
#include "UnionDes.h"
#include "unionRealBaseDB.h"
#include "unionHsmCmd.h"
#include "UnionXOR.h"
#include "unionHsmCmdVersion.h"
#include "defaultIBMPinOffsetDef.h"

/**************************************************************
������룺EA04 (ԭ904����)
��������: KEK����ת����ΪB��Կ����
��������:��������KEK����ת����ΪB��Կ����
**************************************************************/

int UnionDealServiceCodeEA04(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret = 0;
	
	char				kekName[128] = {0};	
	char				keyType[16] = {0};
	char				srcEncMode[16] = {0};
	char				srcIV[64] = {0};
	char				keyBCipherTextByLmk[256] = {0};
	char				keyBType[16] = {0};
	char				dstEncMode[16] = {0};
	char				dstIV[64] = {0};
	char				dataCipherTextBykek[512] = {0};		
	char				dataCipherTextBykeyB[512] = {0};					

        TUnionSymmetricKeyDB            kekKeyDB;
        PUnionSymmetricKeyValue         kekKeyValue = NULL;

        //��ȡkek��Կ����
	memset(&kekKeyDB, 0, sizeof(kekKeyDB));
        if((ret = UnionReadRequestXMLPackageValue("body/kekName",kekName,sizeof(kekName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEM4:: UnionReadRequestXMLPackageValue[%s]!\n","body/kekName");
                return(ret);
        }
        kekName[ret] = 0;
        UnionFilterHeadAndTailBlank(kekName);
        if(strlen(kekName) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCeodeEEM4:: kekName can not be null!\n");
                UnionSetResponseRemark("KEK��Կ���Ʋ���Ϊ��");
                return(errCodeParameter);
        }
	
	//��ȡkek��Կ
        if ((ret = UnionReadSymmetricKeyDBRec(kekName,1,&kekKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEM4:: UnionReadSymmetricKeyDBRec[%s]!\n",kekName);
                return(ret);
        }
	//��ȡkek��Կֵ
        if ((kekKeyValue = UnionGetSymmetricKeyValue(&kekKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEM4:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
                UnionSetResponseRemark("��ȡkek��Կֵʧ��");
                return(errCodeParameter);
        }
	
	//��ȡԴ�����㷨srcEncMode
	if((ret = UnionReadRequestXMLPackageValue("body/srcEncMode",srcEncMode,sizeof(srcEncMode))) <0)
	{
                UnionUserErrLog("in UnionDealServiceCodeEEM4:: UnionReadRequestXMLPackageValue[%s]!\n","body/srcEncMode");
                return(ret);
	}
	srcEncMode[ret] = 0;

	if(strcmp(srcEncMode,"00") != 0 && strcmp(srcEncMode,"01") != 0 && strcmp(srcEncMode,"02") != 0 && strcmp(srcEncMode,"10") != 0 && strcmp(srcEncMode,"11") != 0 && strcmp(srcEncMode,"12") != 0 && strcmp(srcEncMode,"20") != 0 && strcmp(srcEncMode,"21") != 0)	
	{
                        UnionUserErrLog("in UnionDealServiceCodeEEM4:: srcEncMode[%s] error!\n",srcEncMode);
                        UnionSetResponseRemark("Դ�����㷨ģʽ[%s]��Ч",srcEncMode);
                        return(errCodeParameter);
	}
	
	//��ȡԴ�����㷨ʹ�õĳ�ʼ����srcIV
	if(strcmp(srcEncMode,"10") == 0 || strcmp(srcEncMode,"11") == 0 || strcmp(srcEncMode,"12") == 0)
	{
        	if((ret = UnionReadRequestXMLPackageValue("body/srcIV",srcIV,sizeof(srcIV))) < 0)
        	{	
                	UnionUserErrLog("in UnionDealServiceCodeEEM4:: UnionReadRequestXMLPackageValue[%s]!\n","body/srcIV");
                	return(ret);
        	}
		srcIV[ret] = 0;
	}	
	
	//��ȡkeyB����Կ����keyBCipherTextByLmk
        if((ret = UnionReadRequestXMLPackageValue("body/keyBCipherTextByLmk",keyBCipherTextByLmk,sizeof(keyBCipherTextByLmk))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEM4:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyBCipherTextByLmk");
                return(ret);
        }
	keyBCipherTextByLmk[ret] = 0;

	//��ȡkeyB����Կ����
        if((ret = UnionReadRequestXMLPackageValue("body/keyBType",keyBType,sizeof(keyBType))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEM4:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyBType");
                return(ret);
        }
	keyBType[ret] = 0;

	//��ȡĿ�ļ����㷨ģʽdstEncMode
        if((ret = UnionReadRequestXMLPackageValue("body/dstEncMode",dstEncMode,sizeof(dstEncMode))) <0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEM4:: UnionReadRequestXMLPackageValue[%s]!\n","body/dstEncMode");
                return(ret);
        }
	dstEncMode[ret] = 0;

        if(strcmp(dstEncMode,"00") != 0 && strcmp(dstEncMode,"01") != 0 && strcmp(dstEncMode,"02") != 0 && strcmp(dstEncMode,"10") != 0 && strcmp(dstEncMode,"11") != 0 && strcmp(dstEncMode,"12") != 0 && strcmp(dstEncMode,"20") != 0 && strcmp(dstEncMode,"21") != 0)
        {
                        UnionUserErrLog("in UnionDealServiceCodeEEM4:: dstEncMode[%s] error!\n",dstEncMode);
                        UnionSetResponseRemark("Ŀ�ļ����㷨ģʽ[%s]��Ч",dstEncMode);
                        return(errCodeParameter);
        }


	//��ȡĿ�ļ����㷨ʹ�õĳ�ʼ����dstIV
        if(strcmp(dstEncMode,"10") == 0 || strcmp(dstEncMode,"11") == 0 || strcmp(dstEncMode,"12") == 0)
	{
        	if((ret = UnionReadRequestXMLPackageValue("body/dstIV",dstIV,sizeof(dstIV))) < 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeEEM4:: UnionReadRequestXMLPackageValue[%s]!\n","body/dstIV");
                	return(ret);
        	}
		dstIV[ret] = 0;
	}
	
	//��ȡkek���ܵ���������dataCipherTextBykek
        if((ret = UnionReadRequestXMLPackageValue("body/dataCipherTextBykek",dataCipherTextBykek,sizeof(dataCipherTextBykek))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEM4:: UnionReadRequestXMLPackageValue[%s]!\n","body/dataCipherTextBykek");
                return(ret);
        }
	dataCipherTextBykek[ret] = 0;

	UnionTranslateDesKeyTypeTo3CharFormat(kekKeyDB.keyType, keyType);
	
	//��������kek����ת����ΪB��Կ����
	memset(dataCipherTextBykeyB, 0, sizeof(dataCipherTextBykeyB));
	if((ret = UnionHsmCmdUE(keyType,kekKeyValue->keyValue,0,srcEncMode,srcIV,keyBType,keyBCipherTextByLmk,0,dstEncMode,dstIV,strlen(dataCipherTextBykek),dataCipherTextBykek,dataCipherTextBykeyB)) < 0)
	{	
		UnionUserErrLog("in UnionDealServiceCodeEEM7:: UnionHsmCmdUE!\n");
		return(ret);
	}
	

	//����dataCipherTextBykeyB
        if((ret = UnionSetResponseXMLPackageValue("body/dataCipherTextBykeyB",dataCipherTextBykeyB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEM4:: UnionSetResponseXMLPackageValue dataCipherTextBykeyB[%s]!\n",dataCipherTextBykeyB);
                return(ret);
        }
	
	return(0);
}
