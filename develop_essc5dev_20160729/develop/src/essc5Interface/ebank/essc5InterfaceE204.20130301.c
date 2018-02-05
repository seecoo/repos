//      Author:         ������
//      Copyright:      Union Tech. Guangzhou
//      Date:           2013-01-10

/*
	�޸���:		leipp
	�޸�����:	���ܸ���
	�޸�ʱ��:	2016-03-05
*/

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
#include "unionHsmCmdVersion.h"

/***************************************

�������:	E204
������:  	�ַ�PINת��(PK->ZPK)
��������:	�ַ�PINת��(PK->ZPK)

***************************************/

int UnionDealServiceCodeE204(PUnionHsmGroupRec phsmGroupRec)
{

	int		ret;
	char		accNo[48];			//�˺�
	char		pinByPK[520];			//��Կ���ܵ�PIN			
	char 		fillMode[8];			//��䷽ʽ
	char		pinByZPK[80];			//ZPK���ܵ�PIN
	char		strLenOfPin[8];			
	char		pinByZPK2[80];		//PIN����2
	char		lenOfUniqueID[8];		//ID�볤��
	char		uniqueID[48];			//ID������
	char		bcdPinByPK[520];
	int		lenOfPK = 0;
	char		zpkName[136];
	char		pkName[136];
	char		appName[136];
	int		vkIndex = 0;		// add by leipp 20160304


	TUnionAsymmetricKeyDB 	tPk;
	TUnionSymmetricKeyDB    tZpk;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;

	memset(&tZpk,		0, 	sizeof(tZpk));
	memset(&tPk,		0,	sizeof(tPk));
	memset(pkName, 0, sizeof(pkName));

// modify by lusj begin 20150907, �޸Ĺ�Կ���ƣ���Ϊ��Ӧ�ñ��
#if	0  //changed by lusj 20150907 ���ݶ�ݸũ����Ӧ�ñ�Ŷ�ȡ����
	if ((ret = UnionReadRequestXMLPackageValue("body/pkKeyName", pkName, sizeof(pkName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkKeyName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: pkKeyName is null!\n");
		return(errCodeParameter);
	}
	pkName[ret] = 0;
#endif
	
	//��ȡӦ�ñ��
	if ((ret = UnionReadRequestXMLPackageValue("body/pkKeyName", appName, sizeof(appName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEUD:: UnionReadRequestXMLPackageValue [%s]!\n", "body/pkKeyName");
		return(ret);	
	}
	appName[ret]=0;
	
	if (strstr(appName,".") == NULL)
	{
		//��Ӧ�ñ�Ż�ȡ��Կ����
		sprintf(pkName,"%s.pksc10.pk",appName);
	}
	else
	{
		//ֱ��������Կ����
		sprintf(pkName,"%s",appName);
	}
	// modify by lusj end 20150907


	if ((ret = UnionReadRequestXMLPackageValue("body/zpkKeyName", zpkName, sizeof(zpkName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: UnionReadRequestXMLPackageValue[%s]!\n","body/zpkKeyName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: zpkKeyName is null!\n");
		return(errCodeParameter);
	}
	zpkName[ret] = 0 ;

	// ����˺�
	if ((ret = UnionReadRequestXMLPackageValue("body/accNo", accNo, sizeof(accNo)))< 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
		return(ret);
	}
	accNo[ret] = 0;
	UnionFilterHeadAndTailBlank(accNo);
	if (strlen(accNo) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: accNo can not be null!\n");
		UnionSetResponseRemark("�˺Ų���Ϊ��!");
		return(errCodeParameter);
	}
	if (!UnionIsDigitString(accNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: accNo[%s] is error!\n",accNo);
		UnionSetResponseRemark("�˺ŷǷ�[%s],����Ϊ����",accNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}
	
	if ((ret = UnionReadRequestXMLPackageValue("body/pinByPK", pinByPK, sizeof(pinByPK))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinByPK");
		return(ret);
	}
	pinByPK[ret] = 0;
       	//��⹫Կ���ܵ�PIN
	UnionFilterHeadAndTailBlank(pinByPK);
	lenOfPK = strlen(pinByPK);
       	if (lenOfPK == 0)
       	{
	       	UnionUserErrLog("in UnionDealServiceCodeE204:: pinByPK can not be null!\n");
	       	UnionSetResponseRemark("��Կ���ܵ�PIN����Ϊ��!");
	       	return(errCodeParameter);
       	}
       	if (!UnionIsBCDStr(pinByPK))
       	{
       		UnionUserErrLog("in UnionDealServiceCodeE204:: pinByPK[%s] is error!\n",pinByPK);
       		UnionSetResponseRemark("��Կ���ܵ�PIN�Ƿ�,����Ϊʮ��������");
      	 	return(errCodeParameter);
       	}

	// ��ȡ��䷽ʽ
	if ((ret = UnionReadRequestXMLPackageValue("body/fillMode", fillMode, sizeof(fillMode))) <= 0)
		fillMode[0] = '1';
	else
	{
		fillMode[ret] = 0;
		if (fillMode[0] != '0' && fillMode[0] != '1')
		{
			UnionUserErrLog("in UnionDealServiceCodeE204:: fillMode[%s] error!\n",fillMode);
			return(errCodeParameter);
		}
	}

	 // ��ȡ�Գ���Կ
	if ((ret =  UnionReadSymmetricKeyDBRec(zpkName, 1, &tZpk)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", zpkName);
		return(ret);
	}
	// ��ȡ�Գ���Կֵ
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&tZpk,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(errCodeParameter);
	}

	//��ȡ�ǶԳ���Կ
	if ((ret = UnionReadAsymmetricKeyDBRec(pkName, 1, &tPk)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", pkName);
		return(ret);
	}

	// modify by leipp 20160305
	// ����Ƿ��������
	if (tPk.vkStoreLocation != 0)
	{
		vkIndex = atoi(tPk.vkIndex);
	}
	else
	{
		vkIndex = 99;
	}

	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case	conHsmCmdVerRacalStandardHsmCmd:
		case	conHsmCmdVerSJL06StandardHsmCmd:
			aschex_to_bcdhex(pinByPK, lenOfPK, bcdPinByPK);
			bcdPinByPK[lenOfPK/2] = 0;
			if ((ret = UnionHsmCmdN6(tPk.algorithmID,vkIndex,strlen(tPk.vkValue),tPk.vkValue, fillMode[0], tZpk.algorithmID,psymmetricKeyValue->keyValue,accNo,lenOfPK/2,bcdPinByPK,strLenOfPin,pinByZPK,pinByZPK2, lenOfUniqueID, uniqueID))<0 )
			{
				UnionUserErrLog("in UnionDealServiceCodeE204: UnionHsmCmdN6 ret[%d]!\n", ret);
				return(ret);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
			return(errCodeParameter);
	}	
	// modify end by leipp 20160305

	// pin����
	if ((ret = UnionSetResponseXMLPackageValue("body/pinLen", strLenOfPin)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204: UnionSetResponseXMLPackageValue [body/pinLen]!\n");
		return(ret);
	}

	//���� pinByZPK	// ����ZPK���ܻ�ԭ��ʵPIN
	if ((ret = UnionSetResponseXMLPackageValue("body/pinByZPK", pinByZPK)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204: UnionSetResponseXMLPackageValue [body/pinByZPK]!\n");
		return(ret);
	}

	// ��H3ָ����ͬ
	if ((ret = UnionSetResponseXMLPackageValue("body/pinByZPK2", pinByZPK2)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204: UnionSetResponseXMLPackageValue [body/pinByZPK2]!\n");
		return(ret);
	}

	// modify by leipp begin 20141218, ����unionIDLen����,ͬʱΪ�˼���3.xȥ����unionqueID��ת��
	/*
	aschex_to_bcdhex(uniqueID,strlen(uniqueID),bcdPinByPK);
	bcdPinByPK[strlen(uniqueID)/2] = 0;
	*/

	// uninqueID����
	if ((ret = UnionSetResponseXMLPackageValue("body/idLen", lenOfUniqueID)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204: UnionSetResponseXMLPackageValue [body/uniqueIDLen]!\n");
		return(ret);
	}
	
	// ����uniqueID
	if ((ret = UnionSetResponseXMLPackageValue("body/uniqueID", uniqueID)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204: UnionSetResponseXMLPackageValue [body/uniqueID]!\n");
		return(ret);
	}
	// modify by leipp end 20141218

	return 0;
}



