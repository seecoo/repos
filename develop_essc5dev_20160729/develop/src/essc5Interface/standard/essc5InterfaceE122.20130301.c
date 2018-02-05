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
#include "asymmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "commWithHsmSvr.h"

/***************************************
服务代码:	E122
服务名:		导入公钥
功能描述:	导入公钥
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

	// 检测远程标识
	isRemoteKeyOperate = UnionIsRemoteKeyOperate();
	
	// 密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE122:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);

	// 读取非对称密钥
	if ((ret = UnionReadAsymmetricKeyDBRec(keyName,0,&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE122:: UnionReadAsymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}	

	if (!isRemoteKeyOperate)		// 本地密钥操作	
	{
		// modify by leipp 20151209
		// 读取并检查申请和分发平台
		if ((ret = UnionCheckRemoteKeyPlatform(asymmetricKeyDB.keyName, NULL, asymmetricKeyDB.keyDistributePlatform, NULL, &tkeyDistributePlatform, NULL, &isRemoteDistributeKey)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE122:: UnionCheckRemoteKeyPlatform!\n");
			return(ret);
		}
		// modify by leipp end
	}
	else
		isRemoteDistributeKey = 0;

	// 读取编码格式
	if ((ret = UnionReadRequestXMLPackageValue("body/format",format,sizeof(format))) <= 0)
		strcpy(format,"1");

	// 读取公钥
	if ((ret = UnionReadRequestXMLPackageValue("body/pkValue",pkValue,sizeof(pkValue))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE122:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkValue");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(pkValue);
	if (strlen(pkValue) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE122:: pkValue can not be null!\n");
		UnionSetResponseRemark("公钥不能为空!");
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
		UnionSetResponseRemark("密钥[%s]不允许导入!",asymmetricKeyDB.keyName);
		return(errCodeEsscMDLKeyOperationNotPermitted);	
	}

	strcpy(asymmetricKeyDB.oldPKValue,asymmetricKeyDB.pkValue);
	strcpy(asymmetricKeyDB.pkValue,pkValue);

	strcpy(asymmetricKeyDB.oldVKValue,asymmetricKeyDB.vkValue);
	asymmetricKeyDB.vkValue[0] = 0;

	if (isRemoteDistributeKey)	// 远程分发公钥
	{
		// 初始化远程密钥操作请求报文
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE122:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// 转发密钥操作到远程平台
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE122:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	// 更新非对称密钥
	if ((ret =  UnionUpdateAsymmetricKeyDBKeyValue(&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE122:: UnionUpdateAsymmetricKeyDBKeyValue,keyName[%s]!\n",asymmetricKeyDB.keyName);
		return(ret);
	}
	
	return(0);
}
