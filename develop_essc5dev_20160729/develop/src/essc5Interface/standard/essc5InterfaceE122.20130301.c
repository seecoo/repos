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
#include "commWithHsmSvr.h"

/***************************************
�������:	E122
������:		���빫Կ
��������:	���빫Կ
***************************************/
int UnionDealServiceCodeE122(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	char				pkValue[1024];
	char				format[32];
	char				tmpBuf[512];
	int				lenPK = 0;
	int				len = 0;
	int				isRemoteDistributeKey = 0;
	int				isRemoteKeyOperate = 0;
	char				keyName[160];

	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	TUnionRemoteKeyPlatform		tkeyDistributePlatform;
	
	memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));

	// ���Զ�̱�ʶ
	isRemoteKeyOperate = UnionIsRemoteKeyOperate();
	
	// ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE122:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);

	// ��ȡ�ǶԳ���Կ
	if ((ret = UnionReadAsymmetricKeyDBRec(keyName,0,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE122:: UnionReadAsymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}	

	if (!isRemoteKeyOperate)		// ������Կ����	
	{
		// modify by leipp 20151209
		// ��ȡ���������ͷַ�ƽ̨
		if ((ret = UnionCheckRemoteKeyPlatform(asymmetricKeyDB.keyName, NULL, asymmetricKeyDB.keyDistributePlatform, NULL, &tkeyDistributePlatform, NULL, &isRemoteDistributeKey)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE122:: UnionCheckRemoteKeyPlatform!\n");
			return(ret);
		}
		// modify by leipp end
	}
	else
		isRemoteDistributeKey = 0;

	// ��ȡ�����ʽ
	if ((ret = UnionReadRequestXMLPackageValue("body/format",format,sizeof(format))) <= 0)
		strcpy(format,"1");

	// ��ȡ��Կ
	if ((ret = UnionReadRequestXMLPackageValue("body/pkValue",pkValue,sizeof(pkValue))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE122:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkValue");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(pkValue);
	if (strlen(pkValue) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE122:: pkValue can not be null!\n");
		UnionSetResponseRemark("��Կ����Ϊ��!");
		return(errCodeParameter);
	}
	if (!UnionIsBCDStr(pkValue))
	{
		UnionUserErrLog("in UnionDealServiceCodeE122:: pkValue[%s] error!\n",pkValue);
                return(ret);
	}

	if (format[0] == '2')
	{
		lenPK = aschex_to_bcdhex(pkValue,strlen(pkValue),tmpBuf);
		if ((ret = UnionGetPKOutOfRacalHsmCmdReturnStr((unsigned char *)tmpBuf,lenPK,pkValue,&len,sizeof(pkValue))) < 0)	
		{
			UnionUserErrLog("in UnionDealServiceCodeE122:: UnionGetPKOutOfRacalHsmCmdReturnStr");
			return(ret);
		}
		pkValue[len] = 0;
	}

	if (!asymmetricKeyDB.inputFlag)	
	{
		UnionUserErrLog("in UnionDealServiceCodeE122:: inputFlag[%d]!\n",asymmetricKeyDB.inputFlag);
		UnionSetResponseRemark("��Կ[%s]��������!",asymmetricKeyDB.keyName);
		return(errCodeEsscMDLKeyOperationNotPermitted);	
	}

	strcpy(asymmetricKeyDB.oldPKValue,asymmetricKeyDB.pkValue);
	strcpy(asymmetricKeyDB.pkValue,pkValue);

	strcpy(asymmetricKeyDB.oldVKValue,asymmetricKeyDB.vkValue);
	asymmetricKeyDB.vkValue[0] = 0;

	if (isRemoteDistributeKey)	// Զ�̷ַ���Կ
	{
		// ��ʼ��Զ����Կ����������
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE122:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// ת����Կ������Զ��ƽ̨
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE122:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	// ���·ǶԳ���Կ
	if ((ret =  UnionUpdateAsymmetricKeyDBKeyValue(&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE122:: UnionUpdateAsymmetricKeyDBKeyValue,keyName[%s]!\n",asymmetricKeyDB.keyName);
		return(ret);
	}
	
	return(0);
}
