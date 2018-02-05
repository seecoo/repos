
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

/***************************************

服务代码:	EEH2
服务名:  	EEH2 服务将解密后的登录口令(AN9.8)转换为网银的pinOffset
功能描述:	EEH2 服务将解密后的登录口令(AN9.8)转换为网银的pinOffset

***************************************/
int UnionDealServiceCodeEEH2(PUnionHsmGroupRec phsmGroupRec)
{

	int             ret=0;
        char           	zpkName[128+1];
        char        	accNo[48+1];
        char        	pinBlock[16+1];
        char        	pinOffset[16+1];
	char		edkName[128+1];

	TUnionSymmetricKeyDB		edkKeyDB;
	TUnionSymmetricKeyDB		zpkKeyDB;
	PUnionSymmetricKeyValue         pzpkValue = NULL;
	PUnionSymmetricKeyValue         pedkValue = NULL;

	memset(zpkName,0,sizeof(zpkName));
	//读取密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/zpkName", zpkName, sizeof(zpkName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH2:: UnionReadRequestXMLPackageValue[%s]!\n","body/zpkName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH2:: zpkName is null!\n");
		return(errCodeParameter);
	}
	UnionFilterHeadAndTailBlank(zpkName);
        // 读取密钥密钥信息     
        memset(&zpkKeyDB,0,sizeof(zpkKeyDB));
	if ((ret = UnionReadSymmetricKeyDBRec(zpkName,1,&zpkKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH2:: UnionReadSymmetricKeyDBRec[%s]!\n",zpkName);
                return(ret);
        }
	// 读取对称密钥值
        if ((pzpkValue = UnionGetSymmetricKeyValue(&zpkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH2:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
                UnionSetResponseRemark("读取密钥值失败");
                return(errCodeParameter);
        }

	// accNo
        memset(accNo,0,sizeof(accNo));
	if ((ret = UnionReadRequestXMLPackageValue("body/accNo", accNo, sizeof(accNo))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH2:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
                return(ret);
        }
	else if (ret == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH2:: accNo is null!\n");
                return(errCodeParameter);
        }

        // 读pinBlock
        memset(pinBlock,0,sizeof(pinBlock));
	if ((ret = UnionReadRequestXMLPackageValue("body/pinBlock", pinBlock, sizeof(pinBlock))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH2:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinBlock");
                return(ret);
        }
	else if (ret == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH2:: pinBlock is null!\n");
                return(errCodeParameter);
        }
	//读取edk密钥名称
	memset(edkName,0,sizeof(edkName));
	if ((ret = UnionReadRequestXMLPackageValue("body/edkName", edkName, sizeof(edkName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH2:: UnionReadRequestXMLPackageValue[%s]!\n","body/edkName");
                return(ret);
        }
	else if (ret == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH2:: edkName is null!\n");
                return(errCodeParameter);
        }
        // 读edkvalue
	memset(&edkKeyDB,0,sizeof(edkKeyDB));
        if ((ret = UnionReadSymmetricKeyDBRec(edkName,1,&edkKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH2:: UnionReadSymmetricKeyDBRec[%s]!\n",edkName);
                return(ret);
        }

	// 读取对称密钥值
        if ((pedkValue = UnionGetSymmetricKeyValue(&edkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
        {
                UnionUserErrLog("in UnionDealServiceCodeEEH2:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
                UnionSetResponseRemark("读取密钥值失败");
                return(errCodeParameter);
        }
	memset(pinOffset,0,sizeof(pinOffset));

	// 调密码机指令解密PIN
	switch(zpkKeyDB.algorithmID)
	{
                case    conSymmetricAlgorithmIDOfDES:   // DES算法
			if ((ret = UnionHsmCmd4B(pzpkValue->keyValue, accNo, pinBlock, pedkValue->keyValue, pinOffset)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeEEH2: UnionHsmCmdAB ret[%d]!\n", ret);
				return(ret);
			}	
			break;
		case    conSymmetricAlgorithmIDOfSM4:   // SM4算法
                default:
                        UnionUserErrLog("in UnionDealServiceCodeEEH2:: key [%s] algorithmID = [%d] illegal!\n", zpkKeyDB.keyName, zpkKeyDB.algorithmID);
                        return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	
	//读取pinOffset
	if ((ret = UnionSetResponseXMLPackageValue("body/pinOffset", pinOffset)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEEH2: UnionSetResponseXMLPackageValue pinOffset[%s]!\n", pinOffset);
		return(ret);
	}

	return 0;
}
