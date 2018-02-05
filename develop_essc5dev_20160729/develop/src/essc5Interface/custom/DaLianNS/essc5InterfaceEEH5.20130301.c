
#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "commWithHsmSvr.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"

/***************************************

服务代码:	EEH5
服务名:  	EEH5 ZPK加密的PIN转化MD5加密
功能描述:	EEH5 ZPK加密的PIN转化MD5加密

***************************************/
int UnionDealServiceCodeEEH5(PUnionHsmGroupRec phsmGroupRec)
{

	int		ret;
	char		pinByZpk[40+1];
	char		pinByMD5[32+1];
	char		accNo[32+1];
	char		flag[1+1];

	TUnionSymmetricKeyDB		tZpk;
	PUnionSymmetricKeyValue		pzpkValue = NULL;

	memset(&tZpk,		0, 	sizeof(tZpk));

	// 源密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName", tZpk.keyName, sizeof(tZpk.keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH5:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH5:: keyName is null!\n");
		return(errCodeParameter);
	}

	// 读取对称密钥
	if ((ret =  UnionReadSymmetricKeyDBRec(tZpk.keyName, 1, &tZpk)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH5:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", tZpk.keyName);
		return(ret);
	}

	// 读取对称密钥值
	if ((pzpkValue = UnionGetSymmetricKeyValue(&tZpk,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH5:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("读取密钥值失败");
		return(errCodeParameter);
	}

	// 账号
	if ((ret = UnionReadRequestXMLPackageValue("body/accNo", accNo, sizeof(accNo))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH5:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH5:: accNo is null!\n");
		return(errCodeParameter);
	}

	// PIN密文
	if ((ret = UnionReadRequestXMLPackageValue("body/pinByZpk", pinByZpk, sizeof(pinByZpk))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH5:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinByZpk");
		return(ret);
	}
	pinByZpk[ret] = 0;
	if (strlen(pinByZpk) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH5:: pinByZpk can not be null!\n");
		return(errCodeParameter);
	}

	// 标识
	if ((ret = UnionReadRequestXMLPackageValue("body/flag",flag,sizeof(flag))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH5:: UnionReadRequestXMLPackageValue[body/flag]!\n");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH5:: flag cat not be null!\n");
		return(errCodeParameter);
	}
	flag[ret] = 0;
	if (flag[0] != '0' && flag[0] != '1')
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH5:: flag[%s] not in [0,1]!\n",flag);
		return(errCodeParameter);
	}
	
	switch(tZpk.algorithmID)
	{
		case    conSymmetricAlgorithmIDOfDES:   // DES算法
			if ((ret = UnionHsmCmdS4(pzpkValue->keyValue,pinByZpk,accNo,strlen(accNo),atoi(flag),strlen(accNo),accNo,0,"",pinByMD5)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeEEH5: UnionHsmCmdS4 ret[%d]!\n", ret);
				return(ret);
			}
			break;
		case    conSymmetricAlgorithmIDOfSM4:   // SM4算法
		default:
			UnionUserErrLog("in UnionDealServiceCodeEEH5:: key [%s] algorithmID = [%d] illegal!\n", tZpk.keyName, tZpk.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	pinByMD5[ret] = 0;

	if ((ret = UnionSetResponseXMLPackageValue("body/pinByMD5", pinByMD5)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH5: UnionSetResponseXMLPackageValue [bod/pinByMD5]!\n");
		return(ret);
	}

	return 0;
}
