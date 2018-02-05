/*
 * Copyright (c) 2011 Union Tech.
 * All rights reserved.
 *
 * Created on: 2016-01-27
 *   @Author: chenwd
 *   @Version 1.0
 */

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
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "symmetricKeyDB.h"
#include "asymmetricKeyDB.h"
#include "UnionXOR.h"
#include "unionHsmCmdVersion.h"
#include "hsmDefaultFunction.h"
/*
功能：指令A2生成DES密钥并打印
输入参数：
	keyType		3A	密钥类型
	keyLength	2A	密钥长度
	fldNum		2A	描述字段个数
	fld1		128A	描述字段1
	fld2		128A	描述字段2
	fld3		128A	描述字段n

输出参数：
	component	32H	分量

*/
int UnionDealServiceCodeEB03(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret = 0,keyType,keyLength,fldNum,len = 0,j = 0;
	char	tmpBuf[64];
	char	fld[20][80];
	char	component[64];
	char	sql[128];
	char	ipAddr[32];

	//密钥类型
	if ((ret = UnionReadRequestXMLPackageValue("body/keyType",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB03:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyType");
		return(ret);
	}
	tmpBuf[ret] = 0;
	keyType = UnionConvertSymmetricKeyKeyType(tmpBuf);	

	//密钥长度
	if ((ret = UnionReadRequestXMLPackageValue("body/keyLength",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB03:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyLength");
		return(ret);
	}
	tmpBuf[ret] = 0;
	
	if(strcmp(tmpBuf,"16") == 0)
		keyLength = con64BitsDesKey;
	else if(strcmp(tmpBuf,"32") == 0)
		keyLength = con128BitsDesKey;
	else if(strcmp(tmpBuf,"48") == 0)
		keyLength = con192BitsDesKey;
	else
	{
		UnionUserErrLog("in UnionDealServiceCodeEB03:: UnionReadRequestXMLPackageValue keyLength = [%s], Not 16 or 32 or 48!\n",tmpBuf);
		return(ret);
	}

        // 获取密码机组内的打印密码机    
        ret = sprintf(sql,"select ipAddr from hsm where status=1 and enabled=1 and uses=1 and hsmGroupID='%s' order by ipAddr",phsmGroupRec->hsmGroupID);
	sql[ret] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB02:: UnionExecRealDBSql[%s]!\n",sql);
		UnionSetResponseRemark("读取密码机表失败");
		return(ret);
        }
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB02:: UnionExecRealDBSql[%s]!\n",sql);
		UnionSetResponseRemark("密码机组[%s]没有可用的打印用途的密码机",phsmGroupRec->hsmGroupID);
		return(errCodeDatabaseMDL_RecordNotFound);
	}

	UnionLocateXMLPackage("detail", 1);
	memset(ipAddr,0,sizeof(ipAddr));
	if ((ret = UnionReadXMLPackageValue("ipAddr", ipAddr, sizeof(ipAddr))) < 0)
	{
			UnionUserErrLog("in UnionDealServiceCodeEB02:: UnionReadXMLPackageValue[%s]!\n","ipAddr");
			return(ret);
	}
        
        // 使用指定的加密机
        UnionSetUseSpecHsmIPAddrForOneCmd(ipAddr);
        
	//描述字段个数
	if ((ret = UnionReadRequestXMLPackageValue("body/fldNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB03:: UnionReadRequestXMLPackageValue[%s]!\n","body/partKeyNum");
		return(ret);
	}
	tmpBuf[ret] = 0;
	fldNum = atoi(tmpBuf);
	
	for(j=0; j<fldNum; j++)
	{
		memset(fld[j], 0, sizeof(fld[j]));
		
		len = sprintf(tmpBuf,"body/fld%d",j+1);
		tmpBuf[len] = 0;
		if ((ret = UnionReadRequestXMLPackageValue(tmpBuf,fld[j],sizeof(fld[j]))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB03:: UnionReadRequestXMLPackageValue[%s]!\n",tmpBuf);
			return(ret);
		}
	}

	UnionSetIsUseNormalZmkType();
	memset(component,0,sizeof(component));
	if ((ret = UnionHsmCmdA2(keyType, keyLength, fldNum, fld, component)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB03:: UnionHsmCmdA2 err!\n");
		return(ret);
	}

	// 返回分量
	if ((ret = UnionSetResponseXMLPackageValue("body/component",component)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB03:: UnionSetResponseXMLPackageValue component[%s]!\n",component);
		return(ret);
	}

	return(0);
}
