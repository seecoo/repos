//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

/*
	修改人:		leipp
	修改内容:	支持国密改造
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
#include "unionHsmCmd.h"
#include "unionHsmCmdVersion.h"

/*************************************** 
服务代码:	E203
服务名:  	E203 字符PIN转换(ZPK->ZPK)
功能描述:       E203 字符PIN转换(ZPK->ZPK)
***************************************/
int UnionDealServiceCodeE203(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;

	char 	pinBlock[80]; 	//PIN密文
	char	srcNo[48];		//原账号
	char	dstNo[48];		//目的账号
	char	zpkName1[136];
	char	zpkName2[136];
	char	format1[8];
	char	format2[8];
	char	flag[8] = "N";

	char	pin[80];
	char	tmpBuf[32];

	int	lenOfPin = 0;	

	TUnionSymmetricKeyDB    srcZpk;
	TUnionSymmetricKeyDB    dstZpk;
	PUnionSymmetricKeyValue		psrcKeyValue = NULL;
	PUnionSymmetricKeyValue		pdstKeyValue = NULL;

	memset(pinBlock,	0,	sizeof(pinBlock));
	memset(srcNo,		0,	sizeof(srcNo));

	memset(dstNo, 		0,	sizeof(dstNo));
	memset(pin,		0,	sizeof(pin));

	memset(&srcZpk,		0,	sizeof(srcZpk));
	memset(&dstZpk,		0,	sizeof(dstZpk));

	if ((ret = UnionReadRequestXMLPackageValue("body/pinBlock", pinBlock, sizeof(pinBlock)))< 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE203:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinBlock");
                return(ret);
        }

	// add by leipp 20160304
	if ((ret = UnionReadRequestXMLPackageValue("body/format1", format1, sizeof(format1))) > 0)
        {
		format1[ret] = 0;
		if ((ret = UnionReadRequestXMLPackageValue("body/format2", format2, sizeof(format2))) > 0)
		{
			snprintf(flag,sizeof(flag),"Y");
			format2[ret] = 0;
		}
        }
	// add by leipp 20160304 end

	if ((ret = UnionReadRequestXMLPackageValue("body/accNo1", srcNo, sizeof(srcNo)))< 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE203:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo1");
                return(ret);
        }
	
	if ((ret = UnionReadRequestXMLPackageValue("body/accNo2", dstNo, sizeof(dstNo)))< 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE203:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo1");
                return(ret);
        }

	// 检测账号
	UnionFilterHeadAndTailBlank(srcNo);
        if (strlen(srcNo) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE203:: srcNo can not be null!\n");
                UnionSetResponseRemark("源账号不能为空!");
                return(errCodeParameter);
        }
	if (!UnionIsDigitString(srcNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE203:: accNo1[%s] is error!\n",srcNo);
		UnionSetResponseRemark("源账号非法[%s],必须为数字",srcNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}

	// 检测账号
	UnionFilterHeadAndTailBlank(dstNo);
        if (strlen(dstNo) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE203:: dstNo can not be null!\n");
                UnionSetResponseRemark("目的账号不能为空!");
                return(errCodeParameter);
        }
	if (!UnionIsDigitString(dstNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE203:: accNo2[%s] is error!\n",dstNo);
		UnionSetResponseRemark("目的账号非法[%s],必须为数字",dstNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}
	
	// modify by lisq 20141216 账号没有特殊要求
	/*
	if((strlen(srcNo) == 16) && (strlen(dstNo)) == 16)	
	{
		strncpy(srcNo, &srcNo[3], 12);	
		srcNo[12] = '\0';
		strncpy(dstNo, &dstNo[3], 12);	
		dstNo[12] = '\0';
	}
	else
	{
                UnionUserErrLog("in UnionDealServiceCodeE203:: srcNo[%s], strlen(srcNo)=[%d], dstNo[%s],strlen(dstNo)=[%d]!\n",
			srcNo, (int)strlen(srcNo), dstNo, (int)strlen(dstNo));
		return -1;
	}
	*/
	
	if ((strlen(srcNo) < 12) || (strlen(dstNo) < 12))
	{
		UnionUserErrLog("in UnionDealServiceCodeE203:: srcNo[%s], strlen(srcNo)=[%d], dstNo[%s],strlen(dstNo)=[%d]!\n", srcNo, (int)strlen(srcNo), dstNo, (int)strlen(dstNo));
		return(errCodeHsmCmdMDL_InvalidAcc);
	}
	// modify by lisq 20141216 end 账号没有特殊要求

	if ((ret = UnionReadRequestXMLPackageValue("body/keyName1", zpkName1, sizeof(zpkName1))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE203:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName1");
                return(ret);
	}
	zpkName1[ret] = 0;

	if ((ret = UnionReadRequestXMLPackageValue("body/keyName2", zpkName2, sizeof(zpkName2))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE203:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName2");
                return(ret);
	}
	zpkName2[ret] = 0;

	 // 读取对称密钥
        if ((ret =  UnionReadSymmetricKeyDBRec(zpkName1, 1, &srcZpk)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE203:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", zpkName1);
                return(ret);
        }

	// 读取对称密钥值
	if ((psrcKeyValue = UnionGetSymmetricKeyValue(&srcZpk,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE203:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("读取密钥值失败");
		return(errCodeParameter);
	}
	
	 // 读取对称密钥
	if ((ret = UnionReadSymmetricKeyDBRec(zpkName2, 1, &dstZpk)) < 0)
	{
                UnionUserErrLog("in UnionDealServiceCodeE203:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", zpkName2);
                return(ret);
	}
	// 读取对称密钥值
	if ((pdstKeyValue = UnionGetSymmetricKeyValue(&dstZpk,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE203:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("读取密钥值失败");
		return(errCodeParameter);
	}

	// modify by leipp 20160305
	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case conHsmCmdVerRacalStandardHsmCmd:
		case conHsmCmdVerSJL06StandardHsmCmd:
			if ((ret = UnionHsmCmdH8(srcZpk.algorithmID,psrcKeyValue->keyValue, dstZpk.algorithmID,pdstKeyValue->keyValue, pinBlock,format1,format2, srcNo, dstNo, &lenOfPin, pin)) < 0)
			{
				if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && (UnionOldVersionSymmetricKeyIsUsed(&srcZpk)))
				{
					if ((ret = UnionHsmCmdH8(srcZpk.algorithmID,psrcKeyValue->oldKeyValue, dstZpk.algorithmID,pdstKeyValue->keyValue, pinBlock,format1,format2, srcNo, dstNo, &lenOfPin, pin)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE203: UnionHsmCmdH8 old version return[%d]!\n", ret);
						return(ret);
					}
				}
				else
				{
					UnionUserErrLog("in UnionDealServiceCodeE203: UnionHsmCmdH8 return[%d]!\n", ret);
					return(ret);
				}
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE203:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("非法的加密机指令类型");
			return(errCodeParameter);
	}
	// modify end by leipp 20160305
	
	//设置 pinLen
	sprintf(tmpBuf, "%02d", lenOfPin);
	tmpBuf[2] = 0;
        if ((ret = UnionSetResponseXMLPackageValue("body/pinLen", tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE203: UnionSetResponseXMLPackageValue[%s]!\n", "body/pinLen");
                return(ret);
        }
	
	//设置 pinBlock
        if ((ret = UnionSetResponseXMLPackageValue("body/pinBlock", pin)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE203: UnionSetResponseXMLPackageValue[%s]!\n", "body/pinBlock");
                return(ret);
        }

	return 0;
}
