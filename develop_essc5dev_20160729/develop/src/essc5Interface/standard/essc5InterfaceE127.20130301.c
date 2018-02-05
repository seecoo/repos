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
#include "asymmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"

/***************************************
�������:	E127
������:		��ȡ�ǶԳ���Կ��Ϣ
��������:	��ȡ�ǶԳ���Կ��Ϣ
***************************************/
int UnionDealServiceCodeE127(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	char				tmpBuf[128+1];
	char				keyName[136];
	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	
	memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));

	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);

	// ��ȡ�ǶԳ���Կ
	if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,0,&asymmetricKeyDB)) < 0)
	{
		if (ret == errCodeKeyCacheMDL_WrongKeyName)
		{
			UnionUserErrLog("in UnionDealServiceCodeE127:: keyName[%s] not found!\n",keyName);
			UnionSetResponseRemark("�ǶԳ���Կ[%s]������",keyName);
			return(errCodeKeyCacheMDL_KeyNonExists);
		}
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionReadAsymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}

	// ��Կ��
	if ((ret = UnionSetResponseXMLPackageValue("body/keyGroup",asymmetricKeyDB.keyGroup)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyGroup");
		return(ret);
	}

	// �㷨��ʶ 
	memset(tmpBuf,0,sizeof(tmpBuf));
	if (asymmetricKeyDB.algorithmID == conAsymmetricAlgorithmIDOfRSA)
		strcpy(tmpBuf,"RSA");
	else
		strcpy(tmpBuf,"SM2");	

	if ((ret = UnionSetResponseXMLPackageValue("body/algorithmID",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/algorithmID");
		return(ret);
	}

	// ��Կ����
	memset(tmpBuf,0,sizeof(tmpBuf));
	if (asymmetricKeyDB.keyType == conAsymmetricKeyTypeOfSignature)	
		strcpy(tmpBuf,"ǩ��");
	else if (asymmetricKeyDB.keyType == conAsymmetricKeyTypeOfEncryption)
		strcpy(tmpBuf,"����");
	else if (asymmetricKeyDB.keyType == conAsymmetricKeyTypeOfSignatureAndEncryption)
		strcpy(tmpBuf,"ǩ���ͼ���");
		
	if ((ret = UnionSetResponseXMLPackageValue("body/keyType",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyType");
		return(ret);
	}

	// ��Կָ��
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",asymmetricKeyDB.pkExponent);
	if ((ret = UnionSetResponseXMLPackageValue("body/pkExponent",tmpBuf)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/pkExponent");

	// ��Կ����
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",asymmetricKeyDB.keyLen);
	if ((ret = UnionSetResponseXMLPackageValue("body/keyLen",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyLen");
		return(ret);
	}

	// ˽Կ�洢λ��
	memset(tmpBuf,0,sizeof(tmpBuf));
	if (asymmetricKeyDB.vkStoreLocation == 0)
		strcpy(tmpBuf,"��Կ��");
	else
		strcpy(tmpBuf,"�����");

	if ((ret = UnionSetResponseXMLPackageValue("body/vkStoreLocation",tmpBuf)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/vkStoreLocation");

	// ˽Կ����
	if ((ret = UnionSetResponseXMLPackageValue("body/vkIndex",asymmetricKeyDB.vkIndex)) < 0)
		UnionLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/vkIndex");


	// ����ʹ�þ���Կ
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%s",asymmetricKeyDB.oldVersionKeyIsUsed == 1 ? "��":"��");
	if ((ret = UnionSetResponseXMLPackageValue("body/oldVersionKeyIsUsed",tmpBuf)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/oldVersionKeyIsUsed");

	// �������ʶ
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%s",asymmetricKeyDB.inputFlag == 1 ? "��":"��");
	if ((ret = UnionSetResponseXMLPackageValue("body/inputFlag",tmpBuf)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/inputFlag");

	// ��������ʶ
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%s",asymmetricKeyDB.outputFlag == 1 ? "��":"��");
	if ((ret = UnionSetResponseXMLPackageValue("body/outputFlag",tmpBuf)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/outputFlag");

	// ��Կ����ʱ��
	if ((ret = UnionSetResponseXMLPackageValue("body/keyUpdateTime",asymmetricKeyDB.keyUpdateTime)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyUpdateTime");
		return(ret);
	}

	// ��Ч����
	memset(tmpBuf,0,sizeof(tmpBuf));
	if (asymmetricKeyDB.effectiveDays == 0)
		strcpy(tmpBuf,"������");
	else
		sprintf(tmpBuf,"%d��",asymmetricKeyDB.effectiveDays);

	if ((ret = UnionSetResponseXMLPackageValue("body/effectiveDays",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/effectiveDays");
		return(ret);
	}

	// ״̬
	memset(tmpBuf,0,sizeof(tmpBuf));
	if (asymmetricKeyDB.status == conAsymmetricKeyStatusOfInitial)
		strcpy(tmpBuf,"��ʼ��");
	else if (asymmetricKeyDB.status == conAsymmetricKeyStatusOfEnabled)
		strcpy(tmpBuf,"����");
	else if (asymmetricKeyDB.status == conAsymmetricKeyStatusOfSuspend)
		strcpy(tmpBuf,"����");
	else if (asymmetricKeyDB.status == conAsymmetricKeyStatusOfCancel)
		strcpy(tmpBuf,"����");
	else if (asymmetricKeyDB.status == conAsymmetricKeyStatusOfUnknown)
		strcpy(tmpBuf,"δ֪");

	if ((ret = UnionSetResponseXMLPackageValue("body/status",tmpBuf)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/status");

	// ��Ч����	
	if ((ret = UnionSetResponseXMLPackageValue("body/activeDate",asymmetricKeyDB.activeDate)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/activeDate");
		return(ret);
	}
		
	// ��Կ����ƽ̨
	/*memset(tmpBuf,0,sizeof(tmpBuf));
	if (strlen(asymmetricKeyDB.keyApplyPlatform) > 0)
	{
		memset(sql,0,sizeof(sql));	
		sprintf(sql,"select keyPlatformName from remoteKeyPlatform where keyPlatformID = '%s'",asymmetricKeyDB.keyApplyPlatform);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
			UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSelectRealDBRecord[%s]!\n",sql);
		else if (ret == 0)
			UnionUserErrLog("in UnionDealServiceCodeE127:: keyApplyPlatform[%s] not find!\n",asymmetricKeyDB.keyApplyPlatform);
		UnionLocateXMLPackage("detail",1);
		if ((ret = UnionReadXMLPackageValue("keyPlatformName",tmpBuf,sizeof(tmpBuf))) < 0)
			UnionUserErrLog("in UnionDealServiceCodeE127:: UnionReadXMLPackageValue[%s]!\n","keyPlatformName");

	}
	if ((ret = UnionSetResponseXMLPackageValue("body/keyApplyPlatform",tmpBuf)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyApplyPlatform");


	// ��Կ�ַ�ƽ̨
	memset(tmpBuf,0,sizeof(tmpBuf));
	if (strlen(asymmetricKeyDB.keyDistributePlatform) > 0)
	{
		// ���ҷַ�ƽ̨����
		memset(sql,0,sizeof(sql));	
		sprintf(sql,"select keyPlatformName from remoteKeyPlatform where keyPlatformID = '%s'",asymmetricKeyDB.keyDistributePlatform);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
			UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSelectRealDBRecord[%s]!\n",sql);
		else if (ret == 0)
			UnionUserErrLog("in UnionDealServiceCodeE127:: keyDistributePlatform[%s] not find!\n",asymmetricKeyDB.keyDistributePlatform);
		UnionLocateXMLPackage("detail",1);
		if ((ret = UnionReadXMLPackageValue("keyPlatformName",tmpBuf,sizeof(tmpBuf))) < 0)
			UnionUserErrLog("in UnionDealServiceCodeE127:: UnionReadXMLPackageValue[%s]!\n","keyPlatformName");

	}
	if ((ret = UnionSetResponseXMLPackageValue("body/keyDistributePlatform",tmpBuf)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyDistributePlatform");
	*/
	//����������
	memset(tmpBuf,0,sizeof(tmpBuf));
	if (asymmetricKeyDB.creatorType == conAsymmetricCreatorTypeOfApp)
		strcpy(tmpBuf,"Ӧ��");
	else if (asymmetricKeyDB.creatorType == conAsymmetricCreatorTypeOfUser)
		strcpy(tmpBuf,"�û�");

	if ((ret = UnionSetResponseXMLPackageValue("body/creatorType",tmpBuf)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/creatorType");

	//������
	if ((ret = UnionSetResponseXMLPackageValue("body/creator",asymmetricKeyDB.creator)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/creator");

	// ����ʱ��
	if ((ret = UnionSetResponseXMLPackageValue("body/createTime",asymmetricKeyDB.createTime)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/createTime");

	// ʹ�õ�λ
	if ((ret = UnionSetResponseXMLPackageValue("body/usingUnit",asymmetricKeyDB.usingUnit)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/usingUnit");
	
	// ��ע
	if ((ret = UnionSetResponseXMLPackageValue("body/remark",asymmetricKeyDB.remark)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/remark");

	return(0);
}


