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

/***************************************
�������:	E117
������:		��ȡ�Գ���Կ��Ϣ
��������:	��ȡ�Գ���Կ��Ϣ
***************************************/
int UnionDealServiceCodeE117(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	char				tmpBuf[160];
	char				sql[160];
	char				keyName[160];

	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		symmetricKeyValue;
	
	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);

	// ��ȡ�Գ���Կ
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
	{
		if (ret == errCodeKeyCacheMDL_WrongKeyName)
		{
			UnionUserErrLog("in UnionDealServiceCodeE117:: �Գ���Կ[%s]������!\n",keyName);
			UnionSetResponseRemark("�Գ���Կ[%s]������",keyName);
			return(errCodeKeyCacheMDL_KeyNonExists);
		}
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionReadSymmetricKeyDBRec symmetricKeyDB.keyName[%s] ret[%d]!\n",keyName,ret);
		return(ret);
	}

	// ��ȡ��Կֵ
	if ((symmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(errCodeParameter);
	}

	// ��Կ��
	if ((ret = UnionSetResponseXMLPackageValue("body/keyGroup",symmetricKeyDB.keyGroup)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyGroup");
		return(ret);
	}

	// �㷨��ʶ 
	if (symmetricKeyDB.algorithmID == conSymmetricAlgorithmIDOfDES)
		snprintf(tmpBuf,sizeof(tmpBuf),"%s","DES");
	else
		snprintf(tmpBuf,sizeof(tmpBuf),"%s","SM4");

	if ((ret = UnionSetResponseXMLPackageValue("body/algorithmID",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","body/algorithmID");
		return(ret);	
	}

	// ��Կ����
	snprintf(tmpBuf,sizeof(tmpBuf),"%s",UnionConvertSymmetricKeyKeyTypeToExternal(symmetricKeyDB.keyType));
	if (strlen(tmpBuf) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: keyType[%d] error!\n",symmetricKeyDB.keyType);
		return(errCodeParameter);	
	}

	if ((ret = UnionSetResponseXMLPackageValue("body/keyType",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyType");
		return(ret);
	}

	// ��Կ����
	snprintf(tmpBuf,sizeof(tmpBuf),"%d",symmetricKeyDB.keyLen);
	if ((ret = UnionSetResponseXMLPackageValue("body/keyLen",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyLen");
		return(ret);	
	}

	// ����ʹ�þ���Կ
	snprintf(tmpBuf,sizeof(tmpBuf),"%s",symmetricKeyDB.oldVersionKeyIsUsed == 1 ? "��":"��");
	if ((ret = UnionSetResponseXMLPackageValue("body/oldVersionKeyIsUsed",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","body/oldVersionKeyIsUsed");
		//return(ret);	
	}

	// �������ʶ
	// modify by leipp 20151112
	//snprintf(tmpBuf,sizeof(tmpBuf),"%s",symmetricKeyDB.inputFlag == 1 ? "��":"��");
	if (symmetricKeyDB.inputFlag == 1)
		sprintf(tmpBuf,"��");
	else if (symmetricKeyDB.inputFlag == 1)
		sprintf(tmpBuf,"��");
	else
		sprintf(tmpBuf,"��ֹ����");
	// modify by leipp
		
	if ((ret = UnionSetResponseXMLPackageValue("body/inputFlag",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","body/inputFlag");
		//return(ret);
	}

	// ��������ʶ
	snprintf(tmpBuf,sizeof(tmpBuf),"%s",symmetricKeyDB.outputFlag == 1 ? "��":"��");
	if ((ret = UnionSetResponseXMLPackageValue("body/outputFlag",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","body/outputFlag");
		//return(ret);
	}

	// ��Ч����
	if (symmetricKeyDB.effectiveDays == 0)
		strcpy(tmpBuf,"������");
	else
		snprintf(tmpBuf,sizeof(tmpBuf),"%d��",symmetricKeyDB.effectiveDays);

	if ((ret = UnionSetResponseXMLPackageValue("body/effectiveDays",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","body/effectiveDays");
		return(ret);
	}

	// ��Ч����	
	if ((ret = UnionSetResponseXMLPackageValue("body/activeDate",symmetricKeyDB.activeDate)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","body/activeDate");
		return(ret);
	}
		
	// ״̬
	if (symmetricKeyDB.status == 0)
		strcpy(tmpBuf,"��ʼ��");
	else if (symmetricKeyDB.status == 1)
		strcpy(tmpBuf,"����");
	else if (symmetricKeyDB.status == 2)
		strcpy(tmpBuf,"����");
	else if (symmetricKeyDB.status == 3)
		strcpy(tmpBuf,"����");
	else if (symmetricKeyDB.status == 99)
		strcpy(tmpBuf,"δ֪");
	else
		tmpBuf[0] = 0;

	if ((ret = UnionSetResponseXMLPackageValue("body/status",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue keyValue[%s]!\n","body/status");
		return(ret);
	}

	//����������
	if (symmetricKeyDB.creatorType == 0)
		strcpy(tmpBuf,"APP");
	else if (symmetricKeyDB.creatorType == 1)
		strcpy(tmpBuf,"USER");
	else	
		tmpBuf[0] = 0;

	if ((ret = UnionSetResponseXMLPackageValue("body/creatorType",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue checkValue[%s]!\n","body/creatorType");
		//return(ret);
	}

	//������
	if ((ret = UnionSetResponseXMLPackageValue("body/creator",symmetricKeyDB.creator)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue checkValue[%s]!\n","body/creator");
		//return(ret);
	}

	// ����ʱ��
	if ((ret = UnionSetResponseXMLPackageValue("body/createTime",symmetricKeyDB.createTime)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue checkValue[%s]!\n","body/createTime");
		//return(ret);
	}

	// ��Կ����ʱ��
	if ((ret = UnionSetResponseXMLPackageValue("body/keyUpdateTime",symmetricKeyDB.keyUpdateTime)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue checkValue[%s]!\n","body/keyUpdateTime");
		return(ret);
	}

	// ��Կ����ƽ̨
	if (strlen(symmetricKeyDB.keyApplyPlatform) > 0)
	{
		snprintf(sql,sizeof(sql),"select keyPlatformName from remoteKeyPlatform where keyPlatformID = '%s'",symmetricKeyDB.keyApplyPlatform);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
			UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSelectRealDBRecord[%s]!\n",sql);
		else if (ret == 0)
			UnionUserErrLog("in UnionDealServiceCodeE117:: keyApplyPlatform[%s] not find!\n",symmetricKeyDB.keyApplyPlatform);
		UnionLocateXMLPackage("detail",1);
		if ((ret = UnionReadXMLPackageValue("keyPlatformName",tmpBuf,sizeof(tmpBuf))) < 0)
			UnionUserErrLog("in UnionDealServiceCodeE117:: UnionReadXMLPackageValue[%s]!\n","keyPlatformName");
		tmpBuf[ret] = 0;

	}
	if ((ret = UnionSetResponseXMLPackageValue("body/keyApplyPlatform",tmpBuf)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyApplyPlatform");

	// ��Կ�ַ�ƽ̨
	if (strlen(symmetricKeyDB.keyDistributePlatform) > 0)
	{
		// ���ҷַ�ƽ̨����
		snprintf(sql,sizeof(sql),"select keyPlatformName from remoteKeyPlatform where keyPlatformID = '%s'",symmetricKeyDB.keyDistributePlatform);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
			UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSelectRealDBRecord[%s]!\n",sql);
		else if (ret == 0)
			UnionUserErrLog("in UnionDealServiceCodeE117:: keyDistributePlatform[%s] not find!\n",symmetricKeyDB.keyDistributePlatform);
		UnionLocateXMLPackage("detail",1);
		if ((ret = UnionReadXMLPackageValue("keyPlatformName",tmpBuf,sizeof(tmpBuf))) < 0)
			UnionUserErrLog("in UnionDealServiceCodeE117:: UnionReadXMLPackageValue[%s]!\n","keyPlatformName");
		tmpBuf[ret] = 0;

	}
	if ((ret = UnionSetResponseXMLPackageValue("body/keyDistributePlatform",tmpBuf)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyDistributePlatform");


	// ʹ�õ�λ
	if ((ret = UnionSetResponseXMLPackageValue("body/usingUnit",symmetricKeyDB.usingUnit)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue keyValue[%s]!\n","usingUnit");
		//return(ret);
	}
	
	// ��ע
	if ((ret = UnionSetResponseXMLPackageValue("body/remark",symmetricKeyDB.remark)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","remark");
		//return(ret);
	}

	return(0);
}
