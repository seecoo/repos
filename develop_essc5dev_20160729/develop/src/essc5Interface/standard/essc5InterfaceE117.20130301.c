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
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"

/***************************************
服务代码:	E117
服务名:		获取对称密钥信息
功能描述:	获取对称密钥信息
***************************************/
int UnionDealServiceCodeE117(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	char				tmpBuf[160];
	char				sql[160];
	char				keyName[160];

	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		symmetricKeyValue;
	
	// 密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);

	// 读取对称密钥
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
	{
		if (ret == errCodeKeyCacheMDL_WrongKeyName)
		{
			UnionUserErrLog("in UnionDealServiceCodeE117:: 对称密钥[%s]不存在!\n",keyName);
			UnionSetResponseRemark("对称密钥[%s]不存在",keyName);
			return(errCodeKeyCacheMDL_KeyNonExists);
		}
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionReadSymmetricKeyDBRec symmetricKeyDB.keyName[%s] ret[%d]!\n",keyName,ret);
		return(ret);
	}

	// 读取密钥值
	if ((symmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("读取密钥值失败");
		return(errCodeParameter);
	}

	// 密钥组
	if ((ret = UnionSetResponseXMLPackageValue("body/keyGroup",symmetricKeyDB.keyGroup)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyGroup");
		return(ret);
	}

	// 算法标识 
	if (symmetricKeyDB.algorithmID == conSymmetricAlgorithmIDOfDES)
		snprintf(tmpBuf,sizeof(tmpBuf),"%s","DES");
	else
		snprintf(tmpBuf,sizeof(tmpBuf),"%s","SM4");

	if ((ret = UnionSetResponseXMLPackageValue("body/algorithmID",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","body/algorithmID");
		return(ret);	
	}

	// 密钥类型
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

	// 密钥长度
	snprintf(tmpBuf,sizeof(tmpBuf),"%d",symmetricKeyDB.keyLen);
	if ((ret = UnionSetResponseXMLPackageValue("body/keyLen",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyLen");
		return(ret);	
	}

	// 允许使用旧密钥
	snprintf(tmpBuf,sizeof(tmpBuf),"%s",symmetricKeyDB.oldVersionKeyIsUsed == 1 ? "是":"否");
	if ((ret = UnionSetResponseXMLPackageValue("body/oldVersionKeyIsUsed",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","body/oldVersionKeyIsUsed");
		//return(ret);	
	}

	// 允许导入标识
	// modify by leipp 20151112
	//snprintf(tmpBuf,sizeof(tmpBuf),"%s",symmetricKeyDB.inputFlag == 1 ? "是":"否");
	if (symmetricKeyDB.inputFlag == 1)
		sprintf(tmpBuf,"是");
	else if (symmetricKeyDB.inputFlag == 1)
		sprintf(tmpBuf,"否");
	else
		sprintf(tmpBuf,"禁止更新");
	// modify by leipp
		
	if ((ret = UnionSetResponseXMLPackageValue("body/inputFlag",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","body/inputFlag");
		//return(ret);
	}

	// 允许导出标识
	snprintf(tmpBuf,sizeof(tmpBuf),"%s",symmetricKeyDB.outputFlag == 1 ? "是":"否");
	if ((ret = UnionSetResponseXMLPackageValue("body/outputFlag",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","body/outputFlag");
		//return(ret);
	}

	// 有效天数
	if (symmetricKeyDB.effectiveDays == 0)
		strcpy(tmpBuf,"无限制");
	else
		snprintf(tmpBuf,sizeof(tmpBuf),"%d天",symmetricKeyDB.effectiveDays);

	if ((ret = UnionSetResponseXMLPackageValue("body/effectiveDays",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","body/effectiveDays");
		return(ret);
	}

	// 生效日期	
	if ((ret = UnionSetResponseXMLPackageValue("body/activeDate",symmetricKeyDB.activeDate)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","body/activeDate");
		return(ret);
	}
		
	// 状态
	if (symmetricKeyDB.status == 0)
		strcpy(tmpBuf,"初始化");
	else if (symmetricKeyDB.status == 1)
		strcpy(tmpBuf,"启用");
	else if (symmetricKeyDB.status == 2)
		strcpy(tmpBuf,"挂起");
	else if (symmetricKeyDB.status == 3)
		strcpy(tmpBuf,"撤销");
	else if (symmetricKeyDB.status == 99)
		strcpy(tmpBuf,"未知");
	else
		tmpBuf[0] = 0;

	if ((ret = UnionSetResponseXMLPackageValue("body/status",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue keyValue[%s]!\n","body/status");
		return(ret);
	}

	//创建者类型
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

	//创建者
	if ((ret = UnionSetResponseXMLPackageValue("body/creator",symmetricKeyDB.creator)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue checkValue[%s]!\n","body/creator");
		//return(ret);
	}

	// 创建时间
	if ((ret = UnionSetResponseXMLPackageValue("body/createTime",symmetricKeyDB.createTime)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue checkValue[%s]!\n","body/createTime");
		//return(ret);
	}

	// 密钥更新时间
	if ((ret = UnionSetResponseXMLPackageValue("body/keyUpdateTime",symmetricKeyDB.keyUpdateTime)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue checkValue[%s]!\n","body/keyUpdateTime");
		return(ret);
	}

	// 密钥申请平台
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

	// 密钥分发平台
	if (strlen(symmetricKeyDB.keyDistributePlatform) > 0)
	{
		// 查找分发平台名称
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


	// 使用单位
	if ((ret = UnionSetResponseXMLPackageValue("body/usingUnit",symmetricKeyDB.usingUnit)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue keyValue[%s]!\n","usingUnit");
		//return(ret);
	}
	
	// 备注
	if ((ret = UnionSetResponseXMLPackageValue("body/remark",symmetricKeyDB.remark)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE117:: UnionSetResponseXMLPackageValue[%s]!\n","remark");
		//return(ret);
	}

	return(0);
}
