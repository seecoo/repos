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

/***************************************
服务代码:	E127
服务名:		获取非对称密钥信息
功能描述:	获取非对称密钥信息
***************************************/
int UnionDealServiceCodeE127(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	char				tmpBuf[128+1];
	char				keyName[136];
	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	
	memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));

	// 密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);

	// 读取非对称密钥
	if ((ret =  UnionReadAsymmetricKeyDBRec(keyName,0,&asymmetricKeyDB)) < 0)
	{
		if (ret == errCodeKeyCacheMDL_WrongKeyName)
		{
			UnionUserErrLog("in UnionDealServiceCodeE127:: keyName[%s] not found!\n",keyName);
			UnionSetResponseRemark("非对称密钥[%s]不存在",keyName);
			return(errCodeKeyCacheMDL_KeyNonExists);
		}
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionReadAsymmetricKeyDBRec[%s]!\n",keyName);
		return(ret);
	}

	// 密钥组
	if ((ret = UnionSetResponseXMLPackageValue("body/keyGroup",asymmetricKeyDB.keyGroup)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyGroup");
		return(ret);
	}

	// 算法标识 
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

	// 密钥类型
	memset(tmpBuf,0,sizeof(tmpBuf));
	if (asymmetricKeyDB.keyType == conAsymmetricKeyTypeOfSignature)	
		strcpy(tmpBuf,"签名");
	else if (asymmetricKeyDB.keyType == conAsymmetricKeyTypeOfEncryption)
		strcpy(tmpBuf,"加密");
	else if (asymmetricKeyDB.keyType == conAsymmetricKeyTypeOfSignatureAndEncryption)
		strcpy(tmpBuf,"签名和加密");
		
	if ((ret = UnionSetResponseXMLPackageValue("body/keyType",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyType");
		return(ret);
	}

	// 公钥指数
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",asymmetricKeyDB.pkExponent);
	if ((ret = UnionSetResponseXMLPackageValue("body/pkExponent",tmpBuf)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/pkExponent");

	// 密钥长度
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",asymmetricKeyDB.keyLen);
	if ((ret = UnionSetResponseXMLPackageValue("body/keyLen",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyLen");
		return(ret);
	}

	// 私钥存储位置
	memset(tmpBuf,0,sizeof(tmpBuf));
	if (asymmetricKeyDB.vkStoreLocation == 0)
		strcpy(tmpBuf,"密钥库");
	else
		strcpy(tmpBuf,"密码机");

	if ((ret = UnionSetResponseXMLPackageValue("body/vkStoreLocation",tmpBuf)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/vkStoreLocation");

	// 私钥索引
	if ((ret = UnionSetResponseXMLPackageValue("body/vkIndex",asymmetricKeyDB.vkIndex)) < 0)
		UnionLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/vkIndex");


	// 允许使用旧密钥
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%s",asymmetricKeyDB.oldVersionKeyIsUsed == 1 ? "是":"否");
	if ((ret = UnionSetResponseXMLPackageValue("body/oldVersionKeyIsUsed",tmpBuf)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/oldVersionKeyIsUsed");

	// 允许导入标识
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%s",asymmetricKeyDB.inputFlag == 1 ? "是":"否");
	if ((ret = UnionSetResponseXMLPackageValue("body/inputFlag",tmpBuf)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/inputFlag");

	// 允许导出标识
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%s",asymmetricKeyDB.outputFlag == 1 ? "是":"否");
	if ((ret = UnionSetResponseXMLPackageValue("body/outputFlag",tmpBuf)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/outputFlag");

	// 密钥更新时间
	if ((ret = UnionSetResponseXMLPackageValue("body/keyUpdateTime",asymmetricKeyDB.keyUpdateTime)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyUpdateTime");
		return(ret);
	}

	// 有效天数
	memset(tmpBuf,0,sizeof(tmpBuf));
	if (asymmetricKeyDB.effectiveDays == 0)
		strcpy(tmpBuf,"无限制");
	else
		sprintf(tmpBuf,"%d天",asymmetricKeyDB.effectiveDays);

	if ((ret = UnionSetResponseXMLPackageValue("body/effectiveDays",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/effectiveDays");
		return(ret);
	}

	// 状态
	memset(tmpBuf,0,sizeof(tmpBuf));
	if (asymmetricKeyDB.status == conAsymmetricKeyStatusOfInitial)
		strcpy(tmpBuf,"初始化");
	else if (asymmetricKeyDB.status == conAsymmetricKeyStatusOfEnabled)
		strcpy(tmpBuf,"启用");
	else if (asymmetricKeyDB.status == conAsymmetricKeyStatusOfSuspend)
		strcpy(tmpBuf,"挂起");
	else if (asymmetricKeyDB.status == conAsymmetricKeyStatusOfCancel)
		strcpy(tmpBuf,"撤销");
	else if (asymmetricKeyDB.status == conAsymmetricKeyStatusOfUnknown)
		strcpy(tmpBuf,"未知");

	if ((ret = UnionSetResponseXMLPackageValue("body/status",tmpBuf)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/status");

	// 生效日期	
	if ((ret = UnionSetResponseXMLPackageValue("body/activeDate",asymmetricKeyDB.activeDate)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/activeDate");
		return(ret);
	}
		
	// 密钥申请平台
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


	// 密钥分发平台
	memset(tmpBuf,0,sizeof(tmpBuf));
	if (strlen(asymmetricKeyDB.keyDistributePlatform) > 0)
	{
		// 查找分发平台名称
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
	//创建者类型
	memset(tmpBuf,0,sizeof(tmpBuf));
	if (asymmetricKeyDB.creatorType == conAsymmetricCreatorTypeOfApp)
		strcpy(tmpBuf,"应用");
	else if (asymmetricKeyDB.creatorType == conAsymmetricCreatorTypeOfUser)
		strcpy(tmpBuf,"用户");

	if ((ret = UnionSetResponseXMLPackageValue("body/creatorType",tmpBuf)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/creatorType");

	//创建者
	if ((ret = UnionSetResponseXMLPackageValue("body/creator",asymmetricKeyDB.creator)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/creator");

	// 创建时间
	if ((ret = UnionSetResponseXMLPackageValue("body/createTime",asymmetricKeyDB.createTime)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/createTime");

	// 使用单位
	if ((ret = UnionSetResponseXMLPackageValue("body/usingUnit",asymmetricKeyDB.usingUnit)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/usingUnit");
	
	// 备注
	if ((ret = UnionSetResponseXMLPackageValue("body/remark",asymmetricKeyDB.remark)) < 0)
		UnionUserErrLog("in UnionDealServiceCodeE127:: UnionSetResponseXMLPackageValue[%s]!\n","body/remark");

	return(0);
}


