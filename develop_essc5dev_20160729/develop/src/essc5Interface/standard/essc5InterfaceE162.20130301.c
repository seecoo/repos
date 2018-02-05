//	Author:		zhangsb	
//	Copyright:	Union Tech. Guangzhou
//	Date:		2014-06-27

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
#include "symmetricKeyDB.h"
#include "UnionXOR.h"
#include "unionHsmCmdVersion.h"

/***************************************
服务代码:	E162
服务名:		RC4 加密数据
功能描述:	RC4 加密数据
***************************************/
int UnionDealServiceCodeE162(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				lenOfData = 0;
	char				tmpBuf[32];
	char				hexData[5120];
	char				data[10240];
	char				keyName[160];


	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue         psymmetricKeyValue = NULL;
	

	// 密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE162:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;

	if ((ret = UnionReadRequestXMLPackageValue("body/dataLen", tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE162:: UnionReadRequestXMLPackageValue[%s]!\n","body/dataLen");
		return(ret);
	}
	tmpBuf[ret] = 0;
	lenOfData = atoi(tmpBuf);
	
	if ((ret = UnionReadRequestXMLPackageValue("body/data", data,sizeof(data))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE162:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
		return(ret);
	}
	if (ret != lenOfData)
	{
		UnionUserErrLog("in UnionDealServiceCodeE162::len of data [%d] != dataLen [%d]!\n", ret, lenOfData);
		UnionSetResponseRemark("数据长度错误");
		return(errCodeParameter);
	}
	lenOfData = aschex_to_bcdhex(data, ret, hexData);
	// 读取密钥信息
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1,&symmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE162:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
		return(ret);
	}
	// 读取密钥值
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE162:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("读取密钥值失败");
		return(errCodeParameter);
	}
	
	if ((symmetricKeyDB.keyType != conEDK) && (symmetricKeyDB.keyType != conZEK))
	{
		UnionUserErrLog("in UnionDealServiceCodeE162:: is not edk or zek [%s]!\n",keyName);
		return(errCodeEsscMDL_WrongUsageOfKey);
	}

	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case conHsmCmdVerRacalStandardHsmCmd:
		case conHsmCmdVerSJL06StandardHsmCmd:
			if ((ret = UnionHsmCmdRD(symmetricKeyDB.keyType, psymmetricKeyValue->keyValue, hexData, lenOfData, hexData, sizeof hexData)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE162:: UnionHsmCmdRD!\n");
				return(ret);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE162:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("非法的加密机指令类型");
			return(errCodeParameter);
	}
	bcdhex_to_aschex(hexData, ret, data);
	data[ret*2] = 0;	
	// 设置data

	if ((ret = UnionSetResponseXMLPackageValue("body/data", data)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE162:: UnionSetResponseXMLPackageValue!\n");
		return(ret);
	}
	
	return(0);
}
