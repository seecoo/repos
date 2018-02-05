/*
 * Copyright (c) 2011 Union Tech.
 * All rights reserved.
 *
 * Created on: 2015-06-16
 *   @Author: chenwd
 *   @Version 1.0
 *   @changed:lusj  20150910
 */

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

/*
E206��Կ���ܵ�PINתΪDES���ܣ������ӣ�pk->zpk��(N8)
���������
	pkKeyName	128A	��Կ����	
	vkIndex		2N	˽Կ����	����Կ���Ʋ�����ʱ������Ŵ���
	zpkKeyName	128A	ZPK��Կ����	
	accNo		32A	�˺�	
	pinByPK		512H	��Կ���ܵ�PIN	
���������
	pinLen		2N	PIN����	04~12
	pinByZPK	16H	ZPK���ܵ�PIN	����ZPK���ܵ�PIN���ģ�ANSI9.8��ʽ��
	idLen		2H	ID�볤��	
	id		nH	ID������	
*/


int UnionDealServiceCodeE206(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret = 0,pinLen = 0,idLen = 0,lenOfPinByPK = 0;
	int	vkIndex = 0;
	char	zpkKeyName[512];
	char	accNo[64];
	char	pinByPK[1024];
	char	pinByZpk[256];
	char	id[2048];
	char	tmpBuf[1024];
	char	appName[128];
	char	fillMode[8];
	int	algorithmID = 0;
	
	TUnionSymmetricKeyDB            oriKey;
	PUnionSymmetricKeyValue         oriKeyValue = NULL;
	TUnionAsymmetricKeyDB           asymmetricKeyDB;

	//changed  begin by lusj 20150910
	
	//��ȡӦ�ñ��
	if ((ret = UnionReadRequestXMLPackageValue("body/pkKeyName", appName, sizeof(appName))) < 0)
	{
		//����������
		if ((ret = UnionReadRequestXMLPackageValue("body/vkIndex", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE206:: UnionReadRequestXMLPackageValue [%s]!\n", "body/vkIndex");
			return(ret);
		}
		tmpBuf[ret] = 0;
		vkIndex = atoi(tmpBuf);;

		// add by leipp 20160305
		// ��ȡ�㷨��ʶ
		if ((ret = UnionReadRequestXMLPackageValue("body/algorithmID", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE206:: UnionReadRequestXMLPackageValue [%s]!\n", "body/algorithmID");
			return(ret);
		}
		tmpBuf[ret] = 0;
		if (strcasecmp(tmpBuf,"RSA") == 0)
			algorithmID = conAsymmetricAlgorithmIDOfRSA;
		else if (strcasecmp(tmpBuf,"SM2") == 0)
			algorithmID = conAsymmetricAlgorithmIDOfSM2;
		else
		{
			UnionUserErrLog("in UnionDealServiceCodeE206:: algorithmID [%s] invalid!\n", tmpBuf);
			return(errCodeParameter);
		}
		// add end by leipp 20160305
	}
	else if (ret ==0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE206:: pkKeyName is null!\n");
		return(errCodeParameter);
	}
	else
	{
		appName[ret]=0;
		memset(&asymmetricKeyDB, 0, sizeof(asymmetricKeyDB));
		if (strstr(appName,".") == NULL)
		{
			//��Ӧ�ñ�Ż�ȡ��Կ����
			sprintf(asymmetricKeyDB.keyName,"%s.pksc10.pk",appName);
		}
		else
		{
			//ֱ��������Կ����
			sprintf(asymmetricKeyDB.keyName,"%s",appName);
		}
		
		UnionFilterHeadAndTailBlank(asymmetricKeyDB.keyName);
		
		// ��ȡ��Կ��Կ��Կ��Ϣ 	
		if ((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName, 1, &asymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE206:: UnionReadAsymmetricKeyDBRec keyName [%s]!\n", asymmetricKeyDB.keyName);
			return(ret);
		}
		
		// modify by leipp 20160305
		// ����Ƿ��������
		if (asymmetricKeyDB.vkStoreLocation != 0)
		{
			vkIndex = atoi(asymmetricKeyDB.vkIndex);
		}
		else
		{
			vkIndex = 99;
		}
		algorithmID = asymmetricKeyDB.algorithmID;
		// modify end by leipp 20160305
	}

	// ��ȡ���ģʽ
	if ((ret = UnionReadRequestXMLPackageValue("body/fillMode", fillMode, sizeof(fillMode))) <= 0)
	{
		strcpy(fillMode,"1");
	}
	else
		fillMode[ret] = 0;

	if (fillMode[0] != '0' && fillMode[0] != '1')
	{
		UnionUserErrLog("in UnionDealServiceCodeE206:: fillMode[%s] not in[0,1]!\n",fillMode);
		return(errCodeParameter);
	}
	//changed end by lusj 20150910 

	//��ȡzpk��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/zpkKeyName", zpkKeyName, sizeof(zpkKeyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE206:: UnionReadRequestXMLPackageValue [%s]!\n", "body/zpkKeyName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE206:: zpkKeyName is null!\n");
		return(errCodeParameter);
	}
	zpkKeyName[ret] = 0;
	
	memset(&oriKey,0,sizeof(oriKey));
	if ((ret = UnionReadSymmetricKeyDBRec(zpkKeyName, 1, &oriKey)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE206:: UnionReadSymmetricKeyDBRec [%s]!\n", zpkKeyName);
		return(ret);
	}
	// ��ȡ�Գ���Կֵ
	if ((oriKeyValue = UnionGetSymmetricKeyValue(&oriKey, phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE206:: UnionGetSymmetricKeyValue lmkProtectMode [%s]!\n", phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(errCodeParameter);
	}

	//��ȡ�˺�����
	if ((ret = UnionReadRequestXMLPackageValue("body/accNo", accNo, sizeof(accNo))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE206:: UnionReadRequestXMLPackageValue [%s]!\n", "body/accNo");
		return(ret);
	}
	accNo[ret] = 0;
	
	// ��ȡpk���ܵ�pin
	if ((lenOfPinByPK = UnionReadRequestXMLPackageValue("body/pinByPK", pinByPK, sizeof(pinByPK))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE206:: UnionReadRequestXMLPackageValue [%s]!\n", "body/pinByPK");
		return(ret);
	}
	pinByPK[lenOfPinByPK] = 0;

	aschex_to_bcdhex(pinByPK, lenOfPinByPK, tmpBuf);
	tmpBuf[lenOfPinByPK/2] = 0;
	
	// modify by leipp 20160305
	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case	conHsmCmdVerRacalStandardHsmCmd:
		case	conHsmCmdVerSJL06StandardHsmCmd:
			if ((ret = RacalCmdN8(algorithmID,vkIndex,strlen(asymmetricKeyDB.vkValue),asymmetricKeyDB.vkValue, fillMode[0], oriKey.algorithmID,oriKeyValue->keyValue, accNo, strlen(pinByPK)/2, tmpBuf, &pinLen, pinByZpk, &idLen, id)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE206: RacalCmdN8 ret [%d]!\n", ret);
				return(ret);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
			return(errCodeParameter);
	}	
	// modify end by leipp 20160305
	
	// ���÷�����
	
	// ����pinLen
	sprintf(tmpBuf, "%02d", pinLen);
	tmpBuf[2] = 0;
	if ((ret = UnionSetResponseXMLPackageValue("body/pinLen",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE206: UnionSetResponseXMLPackageValue [body/pinLen]!\n");
		return(ret);
	}
	
	// ����pinByZpk
	if ((ret = UnionSetResponseXMLPackageValue("body/pinByZPK", pinByZpk)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE206: UnionSetResponseXMLPackageValue [body/pinByZpk]!\n");
		return(ret);
	}
	
	// ����idLen
	sprintf(tmpBuf, "%02d", idLen);
	tmpBuf[2] = 0;
	if ((ret = UnionSetResponseXMLPackageValue("body/idLen",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE206: UnionSetResponseXMLPackageValue [body/idLen!\n");
		return(ret);
	}
	
	// ����ID
	aschex_to_bcdhex(id, idLen*2, tmpBuf);
	tmpBuf[idLen] = 0;
	if ((ret = UnionSetResponseXMLPackageValue("body/id",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE206: UnionSetResponseXMLPackageValue [body/id]!\n");
		return(ret);
	}	

	return(0);
}


