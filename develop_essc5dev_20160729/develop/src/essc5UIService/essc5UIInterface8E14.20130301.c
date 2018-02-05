//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5UIService.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "asymmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "commWithHsmSvr.h"

/***************************************
服务代码:	8E14
服务名:		导入公钥
功能描述:	导入公钥
***************************************/
int UnionDealServiceCode8E14(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	char			pkValue[4096];
	char			tmpBuf[512];
	char			format[32];
	int			lenPK = 0;
	int			len = 0;

	TUnionAsymmetricKeyDB	asymmetricKeyDB;
	
	memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));
	
	// 密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",asymmetricKeyDB.keyName,sizeof(asymmetricKeyDB.keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E14:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(asymmetricKeyDB.keyName);
	if (strlen(asymmetricKeyDB.keyName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E14:: keyName can not be null!\n");
		UnionSetResponseRemark("密钥名称不能为空!");
		return(errCodeParameter);
	}

	// 读取非对称密钥
	if ((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName,0,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E14:: UnionReadAsymmetricKeyDBRec[%s]!\n",asymmetricKeyDB.keyName);
		return(ret);
	}	

	// 读取公钥编码格式
	memset(format,0,sizeof(format));
	if ((ret = UnionReadRequestXMLPackageValue("body/format",format,sizeof(format))) <= 0)
		strcpy(format,"1");

	// 读取公钥
	if ((ret = UnionReadRequestXMLPackageValue("body/pkValue",pkValue,sizeof(pkValue))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E14:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkValue");
		return(ret);
	}
	ret = UnionFilterHeadAndTailBlank(pkValue);
	if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E14:: pkValue can not be null!\n");
		UnionSetResponseRemark("公钥不能为空!");
		return(errCodeParameter);
	}
	else
	{
		// modify by leipp 20150731 判断RSA与SM2,再进行长度控制
		// add by lisq 20141212 对公钥值长度作控制
		if (asymmetricKeyDB.algorithmID == conAsymmetricAlgorithmIDOfRSA)
		{
			if ((asymmetricKeyDB.keyLen/4) != strlen(pkValue))
			{
				UnionUserErrLog("in UnionDealServiceCode8E14:: pkValue algorithmID[%d] length [%zu] invalid!\n",asymmetricKeyDB.algorithmID, strlen(pkValue));
				UnionSetResponseRemark("公钥长度与密钥库中长度不一致！");
				return(errCodeParameter);
			}
		}
		else
		{
			if ((asymmetricKeyDB.keyLen/2) != strlen(pkValue))
			{
				UnionUserErrLog("in UnionDealServiceCode8E14:: pkValue algorithmID[%d] length [%zu] invalid!\n",asymmetricKeyDB.algorithmID, strlen(pkValue));
				UnionSetResponseRemark("公钥长度与密钥库中长度不一致！");
				return(errCodeParameter);
			}
		}
		// add by lisq 20141212 end
		// modify end 
		if (format[0] == '2')
		{
			memset(tmpBuf,0,sizeof(tmpBuf));
			memset(pkValue,0,sizeof(pkValue));
			lenPK = aschex_to_bcdhex(pkValue,strlen(pkValue),tmpBuf);
			if ((ret = UnionGetPKOutOfRacalHsmCmdReturnStr((unsigned char *)tmpBuf,lenPK,pkValue,&len,sizeof(pkValue))) < 0)	
			{
				UnionUserErrLog("in UnionDealServiceCode8E14:: UnionGetPKOutOfRacalHsmCmdReturnStr");
				return(ret);
			}
		}

		if (asymmetricKeyDB.inputFlag)	
		{
			memset(asymmetricKeyDB.oldPKValue,0,sizeof(asymmetricKeyDB.oldPKValue));
			strcpy(asymmetricKeyDB.oldPKValue,asymmetricKeyDB.pkValue);

			memset(asymmetricKeyDB.pkValue,0,sizeof(asymmetricKeyDB.pkValue));
			strcpy(asymmetricKeyDB.pkValue,pkValue);

			memset(asymmetricKeyDB.oldVKValue,0,sizeof(asymmetricKeyDB.oldVKValue));
			strcpy(asymmetricKeyDB.oldVKValue,asymmetricKeyDB.vkValue);
			memset(asymmetricKeyDB.vkValue,0,sizeof(asymmetricKeyDB.vkValue));
		}
		else
		{
			UnionUserErrLog("in UnionDealServiceCode8E14:: inputFlag[%d]!\n",asymmetricKeyDB.inputFlag);
			UnionSetResponseRemark("密钥[%s]不允许导入!",asymmetricKeyDB.keyName);
			return(errCodeEsscMDLKeyOperationNotPermitted);	
		}
	}

	// 更新非对称密钥
	if ((ret =  UnionUpdateAsymmetricKeyDBKeyValue(&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E14:: UnionUpdateAsymmetricKeyDBKeyValue,keyName[%s]!\n",asymmetricKeyDB.keyName);
		return(ret);	
	}
	return(0);
}


