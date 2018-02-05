#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"essc5Interface.h"
#include"unionErrCode.h"
#include"unionXMLPackage.h"
#include"unionRealBaseDB.h"
#include"UnionLog.h"
#include"UnionStr.h"
#include"symmetricKeyDB.h"
#include"unionHsmCmdVersion.h"

/************************************************************
服务代码：E11A
服务名称: 交换密钥
功能描述：交换密钥
************************************************************/

int UnionDealServiceCodeE11A(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret = 0;
	char				keyName[136];
	char				oldKeyValue[64];
	char				oldCheckValue[32];

        TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;

	// 读取密钥名称
	if((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE11A:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;

	//读取密钥容器
	UnionInitSymmetricKeyDB(&symmetricKeyDB);
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
        {
               UnionUserErrLog("in UnionDealServiceCodeE11A:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
               return(ret);
        }

	// 读取密钥值
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE11A:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
		UnionSetResponseRemark("读取密钥值失败");
		return(ret);
	}
	if (strlen(psymmetricKeyValue->oldKeyValue) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE11A:: keyName[%s] oldKeyValue is null!\n",keyName);
		UnionSetResponseRemark("旧密钥不存在");
		return(errCodeParameter);
	}

	// 交换密钥值
	snprintf(oldKeyValue,sizeof(oldKeyValue),"%s",psymmetricKeyValue->oldKeyValue);
	snprintf(psymmetricKeyValue->oldKeyValue,sizeof(psymmetricKeyValue->oldKeyValue),"%s",psymmetricKeyValue->keyValue);
	snprintf(psymmetricKeyValue->keyValue,sizeof(psymmetricKeyValue->keyValue),"%s",oldKeyValue);

	// 交换校验值
	snprintf(oldCheckValue,sizeof(oldCheckValue),"%s",symmetricKeyDB.oldCheckValue);
	snprintf(symmetricKeyDB.oldCheckValue,sizeof(symmetricKeyDB.oldCheckValue),"%s",symmetricKeyDB.checkValue);
	snprintf(symmetricKeyDB.checkValue,sizeof(symmetricKeyDB.checkValue),"%s",oldCheckValue);

	//更新密钥对象
	if((ret =  UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE11A:: UnionUpdateSymmetricsymmetricKeyDBKeyValue,keyName[%s]!\n",keyName);
                return(ret);
	}
		
	return(0);
}
