//	Author:		张永定	
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
  服务代码:	E207
  服务名:	SM2公钥加密的PIN用SM2私钥解密由SM4算法ZPK加密
  功能描述:	SM2公钥加密的PIN用SM2私钥解密由SM4算法ZPK加密

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

	// 模式
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",modeBuf,sizeof(modeBuf))) <= 0)
		mode = 0;
	else
	{
		modeBuf[ret] = 0;
		mode = atoi(modeBuf);
	}

	// 私钥密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		// 私钥索引
		if ((ret = UnionReadRequestXMLPackageValue("body/vkIndex",vkIndex,sizeof(vkIndex))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE207:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkIndex");
			return(ret);
		}
		UnionFilterHeadAndTailBlank(vkIndex);
		if (strlen(vkIndex) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE207:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkIndex");
			UnionSetResponseRemark("索引不能为空");
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
				UnionSetResponseRemark("私钥值不能为空");
				return(errCodeParameter);
			}
			vkValue[lenOfVK] = 0;
		}
	}
	else
	{
		UnionFilterHeadAndTailBlank(keyName);

		// 读取密钥密钥信息     
		memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));
		if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,1,&asymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE207:: UnionReadAsymmetricKeyDBRec keyName[%s]!\n",keyName);
			return(ret);
		}

		// 检测私钥类型
		if (asymmetricKeyDB.keyType != 1 && asymmetricKeyDB.keyType != 2)
		{
			UnionUserErrLog("in UnionDealServiceCodeE207:: keyType[%d] is not 1 or 2!\n",asymmetricKeyDB.keyType);
			UnionSetResponseRemark("私钥类型[%d]不支持加密,必须为[1或2]");
			return(errCodeHsmCmdMDL_InvalidKeyType);
		}

		// 检测是否存在索引
		if (asymmetricKeyDB.vkStoreLocation != 0)
			snprintf(vkIndex,sizeof(vkIndex),"%s",asymmetricKeyDB.vkIndex);
		else
		{
			snprintf(vkIndex,sizeof(vkIndex),"99");
			lenOfVK = strlen(asymmetricKeyDB.vkValue);
			memcpy(vkValue,asymmetricKeyDB.vkValue,lenOfVK);
			vkValue[lenOfVK] = 0;	
		}

		//检查密码机    
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
				UnionUserErrLog("in UnionDealServiceCodeE207:: hsmGroupID[%s]中没有可用密码机!\n",asymmetricKeyDB.hsmGroupID);
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

	// 密文数据
	if ((lenOfPinByPK = UnionReadRequestXMLPackageValue("body/pinByPK",pinByPK,sizeof(pinByPK))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE207:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinByPK");
		return(lenOfPinByPK);
	}
	if (lenOfPinByPK == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE207:: pinByPK is null!\n");
		UnionSetResponseRemark("公钥加密的数据不能为空");
		return(errCodeParameter);
	}

	pinByPK[lenOfPinByPK] = 0;

	if (mode == 0 || mode == 2)
	{
		// ZPK密钥名称
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

		// 读取密钥值
		if ((pzpkKeyValue = UnionGetSymmetricKeyValue(&zpkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE207::UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("读取[%s]密钥值失败",zpkName);
			return(errCodeParameter);
		}

		// 账号
		if ((ret = UnionReadRequestXMLPackageValue("body/accNo",accNo,sizeof(accNo))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE207:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
			return(ret);
		}
		if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE207:: accNo is null!\n");
			UnionSetResponseRemark("账号不能为空");
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
			UnionSetResponseRemark("非法的加密机指令类型");
			return(errCodeParameter);
	}

	// 设置响应数据
	if ((ret = UnionSetResponseXMLPackageValue("body/pinByZPK",pinByZpk)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE207:: UnionSetResponseXMLPackageValue[%s]!\n","body/pinByZPK");
		return(ret);
	}

	return(0);
}
