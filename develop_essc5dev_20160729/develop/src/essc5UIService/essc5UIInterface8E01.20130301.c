//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5UIService.h"
#include "UnionStr.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "unionHighCachedAPI.h"
#include "remoteKeyPlatform.h"

/***************************************
服务代码:	8E01
服务名:		修改对称密钥
功能描述:	修改对称密钥
***************************************/
int UnionDealServiceCode8E01(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	char				tmpBuf[128];
	char				sql[512];
	char				highCachedKey[512];

	TUnionSymmetricKeyDB		symmetricKeyDB;
	TUnionRemoteKeyPlatform		tkeyApplyPlatform;
	TUnionRemoteKeyPlatform		tkeyDistributePlatform;

	memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));
	
	// 读取密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",symmetricKeyDB.keyName,sizeof(symmetricKeyDB.keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E01:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(symmetricKeyDB.keyName);
	if (strlen(symmetricKeyDB.keyName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E01:: keyName can not be null!\n");
		return(errCodeParameter);
	}

	// 读取密钥组
	if ((ret = UnionReadRequestXMLPackageValue("body/keyGroup",symmetricKeyDB.keyGroup,sizeof(symmetricKeyDB.keyGroup))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E01:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyGroup");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(symmetricKeyDB.keyGroup);
	if (strlen(symmetricKeyDB.keyGroup) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E01:: keyGroup can not be null!\n");
		UnionSetResponseRemark("密钥组不能为空");
		return(errCodeParameter);
	}

	// 读取对称密钥
	/*
	if ((ret = UnionReadSymmetricKeyDBRec(symmetricKeyDB.keyName,&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E03:: UnionReadSymmetricKeyDBRec[%s]!\n",symmetricKeyDB.keyName);
		return(ret);
	}
	*/
	
	// 密钥长度
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/keyLen",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E01:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyLen");
		return(ret);	
	}
	symmetricKeyDB.keyLen = atoi(tmpBuf);
	if ((symmetricKeyDB.keyLen != 64) && (symmetricKeyDB.keyLen != 128) && (symmetricKeyDB.keyLen != 192))
	{
		UnionUserErrLog("in UnionDealServiceCode8E01:: keyLen[%s] error!\n",tmpBuf);
		UnionSetResponseRemark("密钥长度[%s]错误",tmpBuf);
		return(errCodeParameter);
	}

	// 有效天数 
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/effectiveDays",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E01:: UnionReadRequestXMLPackageValue[%s]!\n","body/effectiveDays");
		return(ret);	
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E01:: effectiveDays can not be null!\n");
		return(errCodeParameter);
	}
	else
	{
		symmetricKeyDB.effectiveDays =  atoi(tmpBuf);
	}

	// 允许导入标识
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/inputFlag",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E01:: UnionReadRequestXMLPackageValue[%s]!\n","body/inputFlag");
		return(ret);
	}
	else
	{
		symmetricKeyDB.inputFlag = atoi(tmpBuf);
		if ((symmetricKeyDB.inputFlag != 0) && (symmetricKeyDB.inputFlag != 1) && (symmetricKeyDB.inputFlag != 2))
		{
			UnionUserErrLog("in UnionDealServiceCode8E01:: inputFlag[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}

	// 允许导出标识
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/outputFlag",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E01:: UnionReadRequestXMLPackageValue[%s]!\n","body/outputFlag");
		return(ret);
	}
	else
	{
		symmetricKeyDB.outputFlag = atoi(tmpBuf);
		if ((symmetricKeyDB.outputFlag != 0) && (symmetricKeyDB.outputFlag != 1))
		{
			UnionUserErrLog("in UnionDealServiceCode8E01:: outputFlag[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}

	// 允许使用旧密钥
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/oldVersionKeyIsUsed",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E01:: UnionReadRequestXMLPackageValue[%s]!\n","body/oldVersionKeyIsUsed");
		return(ret);	
	}
	else
	{
		symmetricKeyDB.oldVersionKeyIsUsed = atoi(tmpBuf);
		if ((symmetricKeyDB.oldVersionKeyIsUsed != 0) && (symmetricKeyDB.oldVersionKeyIsUsed != 1))
		{
			UnionUserErrLog("in UnionDealServiceCode8E01:: oldVersionKeyIsUsed[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}

	// 生效日期
	memset(symmetricKeyDB.activeDate,0,sizeof(symmetricKeyDB.activeDate));
	if ((ret = UnionReadRequestXMLPackageValue("body/activeDate",symmetricKeyDB.activeDate,sizeof(symmetricKeyDB.activeDate))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E01:: UnionReadRequestXMLPackageValue[%s]!\n","body/activeDate");
		return(ret);	
	}
	
	// 密钥申请平台
	if ((ret = UnionReadRequestXMLPackageValue("body/keyApplyPlatform",symmetricKeyDB.keyApplyPlatform,sizeof(symmetricKeyDB.keyApplyPlatform))) > 0)
	{
		// 查找remoteKeyPlatform
		memset(&tkeyApplyPlatform,0,sizeof(tkeyApplyPlatform));
		if ((ret = UnionReadRemoteKeyPlatformRec(symmetricKeyDB.keyApplyPlatform,conRemoteKeyPlatformKeyDirectionOfApply,&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E01:: UnionReadRemoteKeyPlatformRec keyApplyPlatform[%s]!\n",symmetricKeyDB.keyApplyPlatform);
			return(ret);
		}	
	}

	// 密钥分发平台
	if ((ret = UnionReadRequestXMLPackageValue("body/keyDistributePlatform",symmetricKeyDB.keyDistributePlatform,sizeof(symmetricKeyDB.keyDistributePlatform))) > 0)
	{
		// 查找remoteKeyPlatform
		memset(&tkeyDistributePlatform,0,sizeof(tkeyDistributePlatform));
		if ((ret = UnionReadRemoteKeyPlatformRec(symmetricKeyDB.keyDistributePlatform,conRemoteKeyPlatformKeyDirectionOfDistribute,&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E01:: UnionReadRemoteKeyPlatformRec keyDistributePlatform[%s]!\n",symmetricKeyDB.keyDistributePlatform);
			return(ret);
		}
	}

	// 备注 
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",symmetricKeyDB.remark,sizeof(symmetricKeyDB.remark))) <= 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode8E01:: UnionReadRequestXMLPackageValue[%s]!\n","body/remark");
		//return(ret);	
		strcpy(symmetricKeyDB.remark,"");
	}

	// 检查密钥属性
	if (!UnionIsValidSymmetricKeyDB(&symmetricKeyDB))
	{
		UnionUserErrLog("in UnionDealServiceCode8E01:: UnionIsValidSymmetricKeyDB!\n");
		return(errCodeParameter);	
	}

	memset(sql,0,sizeof(sql));
	sprintf(sql,"update symmetricKeyDB set keyGroup = '%s', keyLen = %d,inputFlag = %d,outputFlag = %d,oldVersionKeyIsUsed = %d,effectiveDays = %d,activeDate = '%s',keyApplyPlatform = '%s',keyDistributePlatform = '%s',remark = '%s' where keyName = '%s'",
			symmetricKeyDB.keyGroup,
			symmetricKeyDB.keyLen,
			symmetricKeyDB.inputFlag,
			symmetricKeyDB.outputFlag,
			symmetricKeyDB.oldVersionKeyIsUsed,
			symmetricKeyDB.effectiveDays,
			symmetricKeyDB.activeDate,
			symmetricKeyDB.keyApplyPlatform,
			symmetricKeyDB.keyDistributePlatform,
			symmetricKeyDB.remark,
			symmetricKeyDB.keyName);

	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E01:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
		return(ret);
	}
	
	memset(highCachedKey,0,sizeof(highCachedKey));
	UnionGetKeyOfHighCachedForSymmetricKeyDB(symmetricKeyDB.keyName,highCachedKey);	
	
	// 删除高速缓存
	if ((ret = UnionDeleteHighCachedValue(highCachedKey)) < 0)
	{
		UnionProgramerLog("in UnionDealServiceCode8E01:: UnionDeleteHighCachedValue[%s] error!\n",highCachedKey);
	}
	
	return(0);
}

