//      Author:         张永定
//      Copyright:      Union Tech. Guangzhou
//      Date:           2013-01-10

/*
	修改人:		leipp
	修改内容:	国密改造
	修改时间:	2016-03-05
*/

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
#include "asymmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "unionHsmCmdVersion.h"

/***************************************

服务代码:	E204
服务名:  	字符PIN转换(PK->ZPK)
功能描述:	字符PIN转换(PK->ZPK)

***************************************/

int UnionDealServiceCodeE204(PUnionHsmGroupRec phsmGroupRec)
{

	int		ret;
	char		accNo[48];			//账号
	char		pinByPK[520];			//公钥加密的PIN			
	char 		fillMode[8];			//填充方式
	char		pinByZPK[80];			//ZPK加密的PIN
	char		strLenOfPin[8];			
	char		pinByZPK2[80];		//PIN密文2
	char		lenOfUniqueID[8];		//ID码长度
	char		uniqueID[48];			//ID码明文
	char		bcdPinByPK[520];
	int		lenOfPK = 0;
	char		zpkName[136];
	char		pkName[136];
	char		appName[136];
	int		vkIndex = 0;		// add by leipp 20160304


	TUnionAsymmetricKeyDB 	tPk;
	TUnionSymmetricKeyDB    tZpk;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;

	memset(&tZpk,		0, 	sizeof(tZpk));
	memset(&tPk,		0,	sizeof(tPk));
	memset(pkName, 0, sizeof(pkName));

// modify by lusj begin 20150907, 修改公钥名称，改为读应用编号
#if	0  //changed by lusj 20150907 兼容东莞农商以应用编号读取索引
	if ((ret = UnionReadRequestXMLPackageValue("body/pkKeyName", pkName, sizeof(pkName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkKeyName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: pkKeyName is null!\n");
		return(errCodeParameter);
	}
	pkName[ret] = 0;
#endif
	
	//读取应用编号
	if ((ret = UnionReadRequestXMLPackageValue("body/pkKeyName", appName, sizeof(appName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEUD:: UnionReadRequestXMLPackageValue [%s]!\n", "body/pkKeyName");
		return(ret);	
	}
	appName[ret]=0;
	
	if (strstr(appName,".") == NULL)
	{
		//从应用编号获取公钥名称
		sprintf(pkName,"%s.pksc10.pk",appName);
	}
	else
	{
		//直接送入密钥名称
		sprintf(pkName,"%s",appName);
	}
	// modify by lusj end 20150907


	if ((ret = UnionReadRequestXMLPackageValue("body/zpkKeyName", zpkName, sizeof(zpkName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: UnionReadRequestXMLPackageValue[%s]!\n","body/zpkKeyName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: zpkKeyName is null!\n");
		return(errCodeParameter);
	}
	zpkName[ret] = 0 ;

	// 检测账号
	if ((ret = UnionReadRequestXMLPackageValue("body/accNo", accNo, sizeof(accNo)))< 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
		return(ret);
	}
	accNo[ret] = 0;
	UnionFilterHeadAndTailBlank(accNo);
	if (strlen(accNo) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: accNo can not be null!\n");
		UnionSetResponseRemark("账号不能为空!");
		return(errCodeParameter);
	}
	if (!UnionIsDigitString(accNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: accNo[%s] is error!\n",accNo);
		UnionSetResponseRemark("账号非法[%s],必须为数字",accNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}
	
	if ((ret = UnionReadRequestXMLPackageValue("body/pinByPK", pinByPK, sizeof(pinByPK))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinByPK");
		return(ret);
	}
	pinByPK[ret] = 0;
       	//检测公钥加密的PIN
	UnionFilterHeadAndTailBlank(pinByPK);
	lenOfPK = strlen(pinByPK);
       	if (lenOfPK == 0)
       	{
	       	UnionUserErrLog("in UnionDealServiceCodeE204:: pinByPK can not be null!\n");
	       	UnionSetResponseRemark("公钥加密的PIN不能为空!");
	       	return(errCodeParameter);
       	}
       	if (!UnionIsBCDStr(pinByPK))
       	{
       		UnionUserErrLog("in UnionDealServiceCodeE204:: pinByPK[%s] is error!\n",pinByPK);
       		UnionSetResponseRemark("公钥加密的PIN非法,必须为十六进制数");
      	 	return(errCodeParameter);
       	}

	// 获取填充方式
	if ((ret = UnionReadRequestXMLPackageValue("body/fillMode", fillMode, sizeof(fillMode))) <= 0)
		fillMode[0] = '1';
	else
	{
		fillMode[ret] = 0;
		if (fillMode[0] != '0' && fillMode[0] != '1')
		{
			UnionUserErrLog("in UnionDealServiceCodeE204:: fillMode[%s] error!\n",fillMode);
			return(errCodeParameter);
		}
	}

	 // 读取对称密钥
	if ((ret =  UnionReadSymmetricKeyDBRec(zpkName, 1, &tZpk)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", zpkName);
		return(ret);
	}
	// 读取对称密钥值
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&tZpk,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("读取密钥值失败");
		return(errCodeParameter);
	}

	//读取非对称密钥
	if ((ret = UnionReadAsymmetricKeyDBRec(pkName, 1, &tPk)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", pkName);
		return(ret);
	}

	// modify by leipp 20160305
	// 检测是否存在索引
	if (tPk.vkStoreLocation != 0)
	{
		vkIndex = atoi(tPk.vkIndex);
	}
	else
	{
		vkIndex = 99;
	}

	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case	conHsmCmdVerRacalStandardHsmCmd:
		case	conHsmCmdVerSJL06StandardHsmCmd:
			aschex_to_bcdhex(pinByPK, lenOfPK, bcdPinByPK);
			bcdPinByPK[lenOfPK/2] = 0;
			if ((ret = UnionHsmCmdN6(tPk.algorithmID,vkIndex,strlen(tPk.vkValue),tPk.vkValue, fillMode[0], tZpk.algorithmID,psymmetricKeyValue->keyValue,accNo,lenOfPK/2,bcdPinByPK,strLenOfPin,pinByZPK,pinByZPK2, lenOfUniqueID, uniqueID))<0 )
			{
				UnionUserErrLog("in UnionDealServiceCodeE204: UnionHsmCmdN6 ret[%d]!\n", ret);
				return(ret);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServicePINBYSM2PKTOZPK:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("非法的加密机指令类型");
			return(errCodeParameter);
	}	
	// modify end by leipp 20160305

	// pin长度
	if ((ret = UnionSetResponseXMLPackageValue("body/pinLen", strLenOfPin)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204: UnionSetResponseXMLPackageValue [body/pinLen]!\n");
		return(ret);
	}

	//设置 pinByZPK	// 可用ZPK解密还原真实PIN
	if ((ret = UnionSetResponseXMLPackageValue("body/pinByZPK", pinByZPK)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204: UnionSetResponseXMLPackageValue [body/pinByZPK]!\n");
		return(ret);
	}

	// 与H3指令相同
	if ((ret = UnionSetResponseXMLPackageValue("body/pinByZPK2", pinByZPK2)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204: UnionSetResponseXMLPackageValue [body/pinByZPK2]!\n");
		return(ret);
	}

	// modify by leipp begin 20141218, 增加unionIDLen长度,同时为了兼容3.x去除对unionqueID的转换
	/*
	aschex_to_bcdhex(uniqueID,strlen(uniqueID),bcdPinByPK);
	bcdPinByPK[strlen(uniqueID)/2] = 0;
	*/

	// uninqueID长度
	if ((ret = UnionSetResponseXMLPackageValue("body/idLen", lenOfUniqueID)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204: UnionSetResponseXMLPackageValue [body/uniqueIDLen]!\n");
		return(ret);
	}
	
	// 设置uniqueID
	if ((ret = UnionSetResponseXMLPackageValue("body/uniqueID", uniqueID)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE204: UnionSetResponseXMLPackageValue [body/uniqueID]!\n");
		return(ret);
	}
	// modify by leipp end 20141218

	return 0;
}



