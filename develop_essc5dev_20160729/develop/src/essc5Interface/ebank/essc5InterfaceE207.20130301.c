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
#include "unionHsmCmd.h"
#include "defaultIBMPinOffsetDef.h"
#include "asymmetricKeyDB.h"
#include "symmetricKeyDB.h"
#include "UnionMD5.h"
#include "UnionSHA1.h"
#include "unionHsmCmdVersion.h"

/***************************************
  �������:	E207
  ������:	SM2��Կ���ܵ�PIN��SM2˽Կ������SM4�㷨ZPK����
  ��������:	SM2��Կ���ܵ�PIN��SM2˽Կ������SM4�㷨ZPK����

 ***************************************/
int UnionDealServiceCodeE207(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	char				vkIndex[8];
	char				keyName[132];
	char				zpkName[132];
	char				pinByPK[528];
	char				ipAddr[32];
	char				sql[128];
	int				lenOfPinByPK = 0;
	int				lenOfVK=0;
	char				vkValue[272];
	char				accNo[48];
	char				pinByZpk[128];
	int				mode = 0;
	char				modeBuf[8];

	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	TUnionSymmetricKeyDB		zpkKeyDB;
	PUnionSymmetricKeyValue		pzpkKeyValue = NULL;

	// ģʽ
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",modeBuf,sizeof(modeBuf))) <= 0)
		mode = 0;
	else
	{
		modeBuf[ret] = 0;
		mode = atoi(modeBuf);
	}

	// ˽Կ��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		// ˽Կ����
		if ((ret = UnionReadRequestXMLPackageValue("body/vkIndex",vkIndex,sizeof(vkIndex))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE207:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkIndex");
			return(ret);
		}
		UnionFilterHeadAndTailBlank(vkIndex);
		if (strlen(vkIndex) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE207:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkIndex");
			UnionSetResponseRemark("��������Ϊ��");
			return(errCodeParameter);
		}

		// vkValue
		if (atoi(vkIndex) == 99)
		{
			if ((lenOfVK = UnionReadRequestXMLPackageValue("body/vkValue",vkValue,sizeof(vkValue))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE207:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkValue");
				return(lenOfVK);
			}
			if (lenOfVK == 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE207:: vkValue is null!\n");
				UnionSetResponseRemark("˽Կֵ����Ϊ��");
				return(errCodeParameter);
			}
			vkValue[lenOfVK] = 0;
		}
	}
	else
	{
		UnionFilterHeadAndTailBlank(keyName);

		// ��ȡ��Կ��Կ��Ϣ     
		memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));
		if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,1,&asymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE207:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",keyName);
			return(ret);
		}

		// ���˽Կ����
		if (asymmetricKeyDB.keyType != 1 && asymmetricKeyDB.keyType != 2)
		{
			UnionUserErrLog("in UnionDealServiceCodeE207:: keyType[%d] is not 1 or 2!\n",asymmetricKeyDB.keyType);
			UnionSetResponseRemark("˽Կ����[%d]��֧�ּ���,����Ϊ[1��2]");
			return(errCodeHsmCmdMDL_InvalidKeyType);
		}

		// ����Ƿ��������
		if (asymmetricKeyDB.vkStoreLocation != 0)
			snprintf(vkIndex,sizeof(vkIndex),"%s",asymmetricKeyDB.vkIndex);
		else
		{
			snprintf(vkIndex,sizeof(vkIndex),"99");
			lenOfVK = strlen(asymmetricKeyDB.vkValue);
			memcpy(vkValue,asymmetricKeyDB.vkValue,lenOfVK);
			vkValue[lenOfVK] = 0;	
		}

		//��������    
		if (asymmetricKeyDB.vkStoreLocation != 0)
		{
			snprintf(sql,sizeof(sql),"select * from hsm where hsmGroupID = '%s' and enabled = 1",asymmetricKeyDB.hsmGroupID);
			if ( (ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE207:: UnionExecRealDBSql[%s]!\n",sql);
				return(ret);
			}
			else if (ret == 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE207:: hsmGroupID[%s]��û�п��������!\n",asymmetricKeyDB.hsmGroupID);
				return(errCodeParameter);
			}

			UnionLocateXMLPackage("detail", 1);

			if ((ret = UnionReadXMLPackageValue("ipAddr", ipAddr, sizeof(ipAddr))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE207:: UnionReadXMLPackageValue[%s]!\n","ipAddr");
				return(ret);
			}
			ipAddr[ret] = 0;
			UnionSetUseSpecHsmIPAddrForOneCmd(ipAddr);
		}
	}

	// ��������
	if ((lenOfPinByPK = UnionReadRequestXMLPackageValue("body/pinByPK",pinByPK,sizeof(pinByPK))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE207:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinByPK");
		return(lenOfPinByPK);
	}
	if (lenOfPinByPK == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE207:: pinByPK is null!\n");
		UnionSetResponseRemark("��Կ���ܵ����ݲ���Ϊ��");
		return(errCodeParameter);
	}

	pinByPK[lenOfPinByPK] = 0;

	if (mode == 0 || mode == 2)
	{
		// ZPK��Կ����
		if ((ret = UnionReadRequestXMLPackageValue("body/zpkName",zpkName,sizeof(zpkName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE207:: UnionReadRequestXMLPackageValue[%s]!\n","body/zpkName");
			return(ret);
		}
		zpkName[ret] = 0;

		memset(&zpkKeyDB,0,sizeof(zpkKeyDB));
		if ((ret =  UnionReadSymmetricKeyDBRec(zpkName,1,&zpkKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE207:: UnionReadSymmetricKeyDBRec zpkName[%s]!\n",zpkName);
			return(ret);
		}

		if (zpkKeyDB.keyType != conZPK)
		{
			UnionUserErrLog("in UnionDealServiceCodeE207:: is not zpk [%s]!\n",zpkName);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}

		// ��ȡ��Կֵ
		if ((pzpkKeyValue = UnionGetSymmetricKeyValue(&zpkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE207::UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("��ȡ[%s]��Կֵʧ��",zpkName);
			return(errCodeParameter);
		}

		// �˺�
		if ((ret = UnionReadRequestXMLPackageValue("body/accNo",accNo,sizeof(accNo))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE207:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
			return(ret);
		}
		if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE207:: accNo is null!\n");
			UnionSetResponseRemark("�˺Ų���Ϊ��");
			return(errCodeParameter);
		}
		accNo[ret] = 0;
	}

	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case conHsmCmdVerRacalStandardHsmCmd:
		case conHsmCmdVerSJL06StandardHsmCmd:
			memset(pinByZpk, 0, sizeof(pinByZpk));
			if ((ret = UnionHsmCmdKH(mode,atoi(vkIndex),lenOfVK,vkValue,lenOfPinByPK,pinByPK,pzpkKeyValue->keyValue,accNo,pinByZpk,sizeof(pinByZpk))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE207:: UnionHsmCmdKH!\n");
				return(ret);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE207:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
			return(errCodeParameter);
	}

	// ������Ӧ����
	if ((ret = UnionSetResponseXMLPackageValue("body/pinByZPK",pinByZpk)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE207:: UnionSetResponseXMLPackageValue[%s]!\n","body/pinByZPK");
		return(ret);
	}

	return(0);
}
